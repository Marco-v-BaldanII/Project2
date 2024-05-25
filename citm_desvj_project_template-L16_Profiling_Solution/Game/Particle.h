#pragma once
#pragma once
#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/DynArray.h"
#include <string>
#include <map>
#include "../Timer.h"
#include "Source/Textures.h"
#include "DialogueManager.h"
#include "random.h"

using namespace std;



class Particle {
public:

	Particle(const char* texturePath, fPoint startingPos, fPoint velocity, float aliveTime, SDL_Rect rect, float minSize = 0.3f, float maxSize = 1.2f, float minAlpha = 255, float maxAlpha = 255) {

		texture = app->tex->Load(texturePath);
		position = startingPos;
		this->velocity = velocity;

		startPos = position;
		startSpd = velocity;
		this->aliveTime = aliveTime;
		this->rect = rect;
		alphaVariation.x = minAlpha; alphaVariation.y = maxAlpha;
		sizeVariation.x = minSize; sizeVariation.y = maxSize;
	}

	Particle(Particle* p) {
		this->texture = p->texture;
		this->anim = p->anim;
		position = p->position;
		velocity = p->velocity;
		centrifugalForce = p->centrifugalForce;
		startingColor = p->startingColor;
		endingColor = p->endingColor;

		startPos = p->startPos;
		startSpd = p->velocity;
		aliveTime = p->aliveTime;
		rect = p->rect;
		sizeVariation = p->sizeVariation;
		alphaVariation = p->alphaVariation;
	}

	void ReActivate(SDL_Rect spawmBox) {
		position = startPos;
		position.x += getRandomNumber(spawmBox.x, spawmBox.x + spawmBox.w);
		position.y += getRandomNumber(spawmBox.y, spawmBox.y + spawmBox.h);
		velocity = startSpd;
		active = true;
		lifeTimer.Start();

		currentVariation = getRandomNumber(sizeVariation.x*10, sizeVariation.y*10);
		currentAlpha = getRandomNumber(alphaVariation.x, alphaVariation.y);
	}

	bool CheckToDie() {
		if (active && lifeTimer.ReadMSec() > aliveTime * 1000)/*If the particle has passed its time of being aline deactivate it to be reUsed*/ {

			active = false;
			return true;
		}
		return false;
	}

	SDL_Texture* texture;
	SDL_Rect rect;

	Animation anim;

	fPoint position;
	fPoint startPos;

	fPoint velocity;
	fPoint startSpd;

	iPoint alphaVariation;
	int currentAlpha;

	iPoint acceleration;

	float centrifugalForce;

	float aliveTime;

	Timer lifeTimer;

	SDL_Color startingColor;

	SDL_Color endingColor;

	bool active = false;

	fPoint sizeVariation;
	float currentVariation = 0;
};


#endif // __PARTICLE_H__