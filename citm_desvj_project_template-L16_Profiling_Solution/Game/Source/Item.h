#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "../ItemManager.h"
#include <string>

struct SDL_Texture;
using namespace std;

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

	bool PostUpdate();

	bool CleanUp();

	void InitModifiers(int x, int y, float hpMod,float atkMod,float LckMod,float PrecMod,float EvsMod, float spdMod, int movMod, const char* name, const char* textPath);

	const float GetHp() { return hpMod; }
	const float GetAtk() { return atkMod; }
	const float GetLck() { return LckMod; }
	const float GetPrec() { return PrecMod; }
	const float GetEvs() { return EvsMod; }
	const float GetSpd() { return spdMod; }
	const float GetMov() { return movMod; }

public:

	bool isPicked = false;

	ItemState myState;

	iPoint mapPos;

	const char* name;

	Player* myUnit = nullptr;

	SDL_Texture* texture = nullptr;

private:

	
	const char* texturePath;
	uint texW, texH;

	float hpMod =  1;
	float atkMod = 1;
	float LckMod = 1;
	float PrecMod = 1;
	float EvsMod = 1;
	float spdMod = 1;
	float movMod = 0;

};

#endif // __ITEM_H__