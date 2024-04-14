#pragma once
#pragma once
#ifndef __NPC_H__
#define __NPC_H__

#include "../Entity.h"
#include "../Point.h"
#include "SDL/include/SDL.h"
#include "../DynArray.h"
#include "../Timer.h"
#include "Dialogue.h"

struct SDL_Texture;

enum NPC_BEHAVIOUR {
	
	STATIONARY,
	WALK
};



class Npc : public Entity
{
public:

	enum MOVEMENT {

		LEFT,
		RIGHT,
		WAIT

	};

	Npc(string name, int x, int y, int wait);

	virtual ~Npc();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void Walk();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	string name;

	Node* dialogues;
	Node* currentDialogue;

	iPoint position;
	SDL_Rect body = SDL_Rect{ 0,0,71,150 };

	SDL_Texture* texture = nullptr;

	
	int waitTime = 0;

	float speed = 0.2f;

	pugi::xml_node config;
	uint texW, texH;

	
	MOVEMENT myMovement =  WAIT;

	Timer dirTimer;

};

#endif // __PLAYER_H__