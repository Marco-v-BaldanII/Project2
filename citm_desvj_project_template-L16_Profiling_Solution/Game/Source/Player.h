#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;



class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();
	bool PreUpdate() override;
	bool PostUpdate() override;
	bool Update(float dt);

	bool CleanUp();

	void ClickOnMe();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

public:

	//L02: DONE 2: Declare player parameters
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	pugi::xml_node config;
	uint texW, texH;
	int stepCounter = 0;
	//Audio fx
	int pickCoinFxId;

	SDL_Rect clickBox = { 0,0,32,32 };

	bool movedThisTurn = false;

};

#endif // __PLAYER_H__