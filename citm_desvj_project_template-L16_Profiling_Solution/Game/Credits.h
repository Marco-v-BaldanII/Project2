#pragma once
#ifndef __CREDITS_H__
#define __CREDITS_H__

#include "../Module.h"
#include "ParticleSystem.h"
#include "../Defs.h"
#include "../Physics.h"
#include "../Animation.h"

struct SDL_Texture;
class EnemyDummy;
class ParticleEffect;

class Credits : public Module
{
public:

	Credits(bool isActive);

	// Destructor
	virtual ~Credits();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void ReStart();

	uint lose;
private:
	iPoint position = iPoint(0, 0);

	SDL_Texture* img;
	SDL_Rect rect;

	ParticleEffect* rosesL;
	ParticleEffect* rosesR;
	ParticleEffect* snow;

	pugi::xml_node myNode;

	bool playintro = true;

	int waitTime;

	int logoXpos;
	uint thankYouFx;
	bool thanked = false;

	bool victory = false;

	float easingTime = 500; //millisec
	float counter = 0;
};

#endif // __LOSE_H__
