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

	Particle(SDL_Texture* tex, fPoint startingPos, fPoint velocity, fPoint acceleration, float aliveTime, SDL_Rect rect, float minSize = 0.3f, float maxSize = 1.2f,
		float minAlpha = 255, float maxAlpha = 255, fPoint speedXVariation = fPoint(0, 0), Animation animations[] = nullptr, SDL_Texture* tex2 = nullptr) {

		texture = tex;
		position = startingPos;
		this->velocity = velocity;

		startPos = position;
		startSpd = velocity;
		this->aliveTime = aliveTime;
		this->rect = rect;
		this->acceleration = acceleration;
		alphaVariation.x = minAlpha; alphaVariation.y = maxAlpha;
		sizeVariation.x = minSize; sizeVariation.y = maxSize;
		this->speedXVariation = speedXVariation;
		this->texture2 = tex2;
		if (animations != nullptr) {
			for (int i = 0; i < 3; ++i) {
				this->possibleAnimations[i] = animations[i];
			}
		}
	}

	Particle(const char* pathTex, fPoint startingPos, fPoint velocity, fPoint acceleration, float aliveTime, SDL_Rect rect, float minSize = 0.3f, float maxSize = 1.2f,
		float minAlpha = 255, float maxAlpha = 255, fPoint speedXVariation = fPoint(0, 0), Animation animations[] = nullptr) {
		texture = app->tex->Load(pathTex);


		position = startingPos;
		this->velocity = velocity;

		startPos = position;
		startSpd = velocity;
		this->aliveTime = aliveTime;
		this->rect = rect;
		this->acceleration = acceleration;
		alphaVariation.x = minAlpha; alphaVariation.y = maxAlpha;
		sizeVariation.x = minSize; sizeVariation.y = maxSize;
		this->speedXVariation = speedXVariation;
		if (animations != nullptr) {
			for (int i = 0; i < 3; ++i) {
				this->possibleAnimations[i] = animations[i];
			}
		}
	}

	Particle(Particle* p) {
		this->texture = p->texture;
		this->texture2 = p->texture2;

		for (int i = 0; i < 3; ++i) {
			possibleAnimations[i] = p->possibleAnimations[i];
		}

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
		acceleration = p->acceleration;
		speedXVariation = p->speedXVariation;

	}

	void ReActivate(SDL_Rect spawmBox) {
		position = startPos;
		position.x += getRandomNumber(spawmBox.x, spawmBox.x + spawmBox.w);
		position.y += getRandomNumber(spawmBox.y, spawmBox.y + spawmBox.h);
		velocity = startSpd;
		active = true;
		lifeTimer.Start();


		currentVariation = getRandomNumber(sizeVariation.x * 10, sizeVariation.y * 10);
		currentAlpha = getRandomNumber(alphaVariation.x, alphaVariation.y);
		currentSpeedXVariation = getRandomNumber(speedXVariation.x, speedXVariation.y);

		// Assign one of the possible rando textures
		int index = getRandomNumber(0, 3);

		currentAnim = &possibleAnimations[index];
		if (texture2 != nullptr) {
			int i = getRandomNumber(0, 1);
			if (i == 0) {
				maintexture = texture;
			}
			else {
				maintexture = texture2;
			}
		}
		else {
			maintexture = texture;
		}

	}

	bool CheckToDie() {
		if (active && lifeTimer.ReadMSec() > aliveTime * 1000)/*If the particle has passed its time of being aline deactivate it to be reUsed*/ {

			active = false;
			return true;
		}
		return false;
	}
	SDL_Texture* maintexture;
	SDL_Texture* texture;
	SDL_Texture* texture2 = nullptr;
	SDL_Rect rect;

	Animation possibleAnimations[3];
	Animation* currentAnim;

	fPoint position;
	fPoint startPos;

	fPoint velocity;
	fPoint startSpd;

	fPoint speedXVariation;
	int currentSpeedXVariation;

	iPoint alphaVariation;
	int currentAlpha;

	fPoint acceleration;

	float centrifugalForce;

	float aliveTime;

	Timer lifeTimer;

	SDL_Color startingColor;

	SDL_Color endingColor;

	bool active = false;

	fPoint sizeVariation;
	float currentVariation = 0;

	~Particle() {
		delete currentAnim;

	}

};


#endif // __PARTICLE_H__