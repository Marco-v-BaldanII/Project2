#include "App.h"
#include "Render.h"
#include "Textures.h"

#include "Defs.h"
#include "Log.h"
#include "src/physfs.h"
#include "SDL_image/include/SDL_image.h"
#include <map>
#include <vector>
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

bool Textures::Awake(pugi::xml_node config)
{
    LOG("Init Image library");
    bool ret = true;

    // Initialize PhysicsFS
    if (!PHYSFS_init(NULL)) {
        LOG("Error initializing PhysicsFS: %s", PHYSFS_getLastError());
        return false;
    }

    // Mount the zip file containing textures
    if (!PHYSFS_mount("Assets.zip", NULL, 1)) {
        LOG("Error mounting zip file: %s", PHYSFS_getLastError());
        return false;
    }

    // Load support for the PNG image format
    int flags = IMG_INIT_PNG;
    int init = IMG_Init(flags);

    if ((init & flags) != flags)
    {
        LOG("Could not initialize Image lib. IMG_Init: %s", IMG_GetError());
        ret = false;
    }

    return ret;
}

SDL_Texture* const Textures::Load(const char* path)
{
    // Check if the texture exists in the map
    auto it = textureMap.find(path);
    if (it != textureMap.end()) {
        return it->second;
    }

    // Open the file from the zip archive
    PHYSFS_File* file = PHYSFS_openRead(path);
    if (!file) {
        LOG("Error opening file %s: %s", path, PHYSFS_getLastError());
        return nullptr;
    }

    // Determine file size
    PHYSFS_sint64 fileSize = PHYSFS_fileLength(file);
    if (fileSize <= 0) {
        LOG("Error getting file size for %s: %s", path, PHYSFS_getLastError());
        PHYSFS_close(file);
        return nullptr;
    }

    // Allocate memory to read file contents
    std::vector<uint8_t> buffer(fileSize);

    // Read file contents into buffer
    PHYSFS_sint64 bytesRead = PHYSFS_read(file, buffer.data(), 1, fileSize);
    if (bytesRead != fileSize) {
        LOG("Error reading file contents for %s: %s", path, PHYSFS_getLastError());
        PHYSFS_close(file);
        return nullptr;
    }

    // Close the file
    PHYSFS_close(file);

    // Create a surface from the memory buffer
    SDL_RWops* rwOps = SDL_RWFromConstMem(buffer.data(), fileSize);
    if (!rwOps) {
        LOG("Error creating SDL_RWops for %s: %s", path, SDL_GetError());
        return nullptr;
    }

    SDL_Surface* surface = IMG_Load_RW(rwOps, 1);
    SDL_FreeRW(rwOps);
    if (!surface) {
        LOG("Error loading surface for %s: %s", path, IMG_GetError());
        return nullptr;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(app->render->renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        LOG("Error creating texture from surface for %s: %s", path, SDL_GetError());
        return nullptr;
    }

    // Store texture in map
    textureMap[path] = texture;
     return texture;
}

bool Textures::UnLoad(SDL_Texture* texture)
{
    // Find and erase the texture from the map
    auto it = std::find_if(textureMap.begin(), textureMap.end(), [&](const auto& pair) {
        return pair.second == texture;
        });

    if (it != textureMap.end()) {
        SDL_DestroyTexture(it->second);
        textureMap.erase(it);
        return true;
    }

    return false;
}