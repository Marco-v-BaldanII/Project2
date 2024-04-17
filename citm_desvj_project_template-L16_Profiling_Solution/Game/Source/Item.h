#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../ItemManager.h"

struct SDL_Texture;

enum ItemState {

	OVERWORLD,
	INVENTORY,
	EQUIPED

};


class Item 
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void InitModifiers();


public:

	bool isPicked = false;

	ItemState myState;

	iPoint mapPos;


private:

	SDL_Texture* texture = app->tex->Load("");
	const char* texturePath;
	uint texW, texH;

	uint hpMod = 0;
	uint atkMod = 0;
	uint LckMod = 0;
	uint PrecMod = 0;
	uint EvsMod = 0;


};

#endif // __ITEM_H__