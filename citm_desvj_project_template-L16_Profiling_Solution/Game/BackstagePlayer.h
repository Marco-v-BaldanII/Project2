#pragma once
#include "Source/App.h"
#include "Source/Render.h"
#include "Source/Audio.h"
#include "Source/Defs.h"
#include "Source/Input.h"
#include "Source/EntityManager.h"
#include "Source/Entity.h"
#include "Source/Physics.h"
#include "Source/Map.h"
#include "Source/LevelManagement.h"
#include "Source/Animation.h"
#include "Source/Point.h"
#include "Source/Textures.h"
#include "Source/Module.h"

class BackStagePlayer : public Module
{
public:
	BackStagePlayer(bool isActive);
	~BackStagePlayer();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void SetPlayerPosition(iPoint pos);
	void SetPlayerPosition(int x, int y);

	void SetCanMove(bool active)
	{
		canMove = active;
	}

	bool GetCanMove() const
	{
		return canMove;
	}

	float32 velocity = 0.0f;

	uint fxSteps;

	bool isMoving = false;

	iPoint position;

private:
	SDL_Texture* playerTexture = nullptr;
	
	Animation walkUp;
	Animation walkDown;
	Animation walkLeft;
	Animation walkRight;
	Animation idleUp;
	Animation idleDown;
	Animation idleLeft;
	Animation idleRight;
	Animation* currentAnimation;

	SDL_RendererFlip lastDirection = SDL_FLIP_NONE;
	SDL_RendererFlip flipHorizontal = lastDirection;

	float walkSpeed;
	bool goingLeft;
	bool goingRight;
	bool goingUp;
	bool goingDown;
	bool godMode;
	bool canMove;
};