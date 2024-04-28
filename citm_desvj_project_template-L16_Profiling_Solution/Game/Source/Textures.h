#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include "Module.h"
#include <string>
#include "List.h"
#include <map>

struct SDL_Texture;
struct SDL_Surface;

class Textures : public Module
{
public:

	Textures(bool isActive);

	// Destructor
	virtual ~Textures();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before quitting
	bool CleanUp();

	SDL_Texture* GetTexture(const char* path);

	// Load Texture
	SDL_Texture* const Load(const char* path);
	SDL_Texture* const LoadSurface(SDL_Surface* surface, const char* path);
	bool UnLoad(SDL_Texture* texture);
	void GetSize(const SDL_Texture* texture, uint& width, uint& height) const;

private:

	//List<SDL_Texture*> textures;
	std::map <std::string, SDL_Texture*> textureMap;
	uint textureIndex = 0;
};


struct TextureDef {
public:
	SDL_Texture* texture;
	std::string name;

	TextureDef(SDL_Texture* texture, std::string name) {
		this->texture = texture;
		this->name = name;
	}

	TextureDef() {
		texture = nullptr;
		name = "name";
	}
};

struct ActorSprite {
public:
	TextureDef* actor;

	TextureDef* role;

	ActorSprite(TextureDef* actor, TextureDef* role) {

		this->actor = actor;
		this->role = role;
	}
};

#endif // __TEXTURES_H__