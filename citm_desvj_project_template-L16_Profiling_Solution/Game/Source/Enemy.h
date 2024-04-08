#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

public:

	bool isPicked = false;

	pugi::xml_node config;

	SDL_Texture* texture;
	const char* texturePath;
	uint texW, texH;

	bool movedThisTurn = false;

	SDL_Rect section = SDL_Rect{ 75, 6, 26, 27 };

};

#endif // __ENEMY_H__