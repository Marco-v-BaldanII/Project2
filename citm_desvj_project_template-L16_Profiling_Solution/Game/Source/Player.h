#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

struct SDL_Texture;
class Dialogue;


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

	void CalculateAttack();

	void OnCollision(Collider* physA, Collider* physB) override;

public:

	Frame* frames;

	PathFinding* personalPathfinding;


	//L02: DONE 2: Declare player parameters
	float speed = 0.2f;
	SDL_Texture* myTexture = nullptr;
	SDL_Texture* UiTex = nullptr;
	
	pugi::xml_node config;
	uint texW, texH;
	int stepCounter = 0;
	//Audio fx
	int pickCoinFxId;

	SDL_Rect clickBox = { 0,0,32,32 };
	Entity* oponent;
	bool movedThisTurn = false;
	bool endTurn;
	
	
};

#endif // __PLAYER_H__