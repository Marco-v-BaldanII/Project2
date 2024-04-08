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

class BackStagePlayer : public Entity
{
public:
	BackStagePlayer(iPoint pos);
	~BackStagePlayer();

	bool Start() override;
	bool PreUpdate() override;
	bool Update(float dt) override;
	bool PostUpdate() override;
	bool CleanUp() override;

	void OnCollision(PhysBody* bodyA, PhysBody* bodyB) override;

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

private:
	SDL_Texture* playerTexture = nullptr;
	
	Animation walkUp;
	Animation walkDown;
	Animation walkSide;
	Animation idleUp;
	Animation idleDown;
	Animation idleSide;
	Animation* currentAnimation;

	float walkSpeed;
	bool goingLeft;
	bool godMode;
	bool canMove;
};