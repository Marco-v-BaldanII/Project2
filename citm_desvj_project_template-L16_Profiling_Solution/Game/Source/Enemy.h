#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"

#define GROUND 200
#define BOUNCE 1

struct SDL_Texture;
class Dialogue;

class Enemy : public Entity
{
public:

	Enemy();
	virtual ~Enemy();

	bool Awake();

	bool Start();

	bool PreUpdate();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void ClickOnMe();

	bool DealDMG();

	void OnCollision(Collider* physA, Collider* physB) override;

	void FigureStickMovement(float dt);

public:

	bool isPicked = false;

	bool drawPath = false;

	pugi::xml_node config;

	SDL_Texture* texture; 
	SDL_Texture* Uitex = nullptr;
	const char* texturePath;
	uint texW, texH;

	bool movedThisTurn = false;

	bool newTarget = false;

	bool finishedLerp = false;

	SDL_Rect section = SDL_Rect{ 75, 6, 26, 27 };

	SDL_Rect clickBox = { 0,0,32,32 };

	Animation* currentAnim = nullptr;
	Animation idle;

	PathFinding* personalPathfinding;

	bool centerCamera = false;

	bool opponentAttacking = false;
	bool opponentReachTarget = false;

	bool giveExp = false;

};

#endif // __ENEMY_H__