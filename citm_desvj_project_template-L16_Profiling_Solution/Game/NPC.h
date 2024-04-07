#ifndef __NPC_H__
#define __NPC_H__

#include "../Entity.h"
#include "../Point.h"
#include "SDL/include/SDL.h"
#include "DialogueManager.h"
#include "../DynArray.h"
#include "../Timer.h"

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

	Npc(string name);

	virtual ~Npc();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void Walk();

	// L07 DONE 6: Define OnCollision function for the player. 
	void OnCollision(PhysBody* physA, PhysBody* physB);

public:
	string name;

	DynArray<Dialogue*> myDialogues;

	iPoint position;
	SDL_Rect body = SDL_Rect{ 0,0,50,70 };

	
	float speed = 0.2f;
	SDL_Texture* texture = NULL;
	pugi::xml_node config;
	uint texW, texH;

	
	MOVEMENT myMovement =  WAIT;

	Timer dirTimer;

};

#endif // __PLAYER_H__