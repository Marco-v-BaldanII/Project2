#include "App.h"
#include "Render.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"

#include "SDL_image/include/SDL_image.h"
#include <map>
using namespace std;
//#pragma comment(lib, "../Game/Source/External/SDL_image/libx86/SDL2_image.lib")

Textures::Textures(bool isActive) : Module(isActive)
{
	name.Create("textures");
}

// Destructor
Textures::~Textures()
{}

// Called before render is available
bool Textures::Awake(pugi::xml_node config)
{
	LOG("Init Image library");
	bool ret = true;

	// Load support for the PNG image format
	int flags = IMG_INIT_PNG;
	int init = IMG_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
		ret = false;
	}

	return ret;
}

// Called before the first frame
bool Textures::Start()
{
	LOG("start textures");
	bool ret = true;
	return ret;
}

// Called before quitting
bool Textures::CleanUp()
{
	LOG("Freeing textures and Image library");

	// Has been modified to work with a map dictionary
	for (auto& pair : textureMap) {

		SDL_DestroyTexture(pair.second);

	}

	textureMap.clear();

	//for(item = textures.start; item != NULL; item = item->next)
	//{
	//	SDL_DestroyTexture(item->data);
	//}

	//textures.Clear();
	IMG_Quit();
	return true;
}

// Load new texture from file path
SDL_Texture* const Textures::Load(const char* path)
{
	// Check if the texture exists
	if (textureMap.find(path) != textureMap.end()) {
		return textureMap[path];
	}
	else {

		SDL_Texture* texture = NULL;
		SDL_Surface* surface = IMG_Load(path);

		if (surface == NULL)
		{
			LOG("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
		}
		else
		{
			texture = LoadSurface(surface, path);
			SDL_FreeSurface(surface);

			textureMap[path] = texture; /*Add texture to texture map to be available later*/

		}

		return texture;
	}
}

// Unload texture
bool Textures::UnLoad(SDL_Texture* texture)
{
	/*ListItem<SDL_Texture*>* item;

	for(item = textures.start; item != NULL; item = item->next)
	{
		if(texture == item->data)
		{
			SDL_DestroyTexture(item->data);
			textures.Del(item);
			return true;
		}
	}*/
	for (auto& pair : textureMap) {

		if (pair.second == texture) {
			
			textureMap.erase(pair.first);
			SDL_DestroyTexture(texture);
		}

	}

	return false;
}

// Translate a surface into a texture
SDL_Texture* const Textures::LoadSurface(SDL_Surface* surface, const char* path)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);

	if(texture == NULL)
	{
		LOG("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		//textures.Add(texture);
		textureMap[path] = texture;
		textureIndex++;
	}

	return texture;
}

// Returns the texture if it exists, if not nullptr
SDL_Texture* Textures::GetTexture(const char* path) {

	auto it = textureMap.find(path);

	if (it != textureMap.end())/* the texture exists */ {
		return textureMap[path];
	}

	else {
		return nullptr;
	}

}

// Retrieve size of a texture
void Textures::GetSize(const SDL_Texture* texture, uint& width, uint& height) const
{
	SDL_QueryTexture((SDL_Texture*)texture, NULL, NULL, (int*) &width, (int*) &height);
}
