#ifndef __LOSE_H__
#define __LOSE_H__

#include "Module.h"
//#include "p2List.h"
#include "Defs.h"
#include "Physics.h"
#include "Animation.h"

struct SDL_Texture;
class EnemyDummy;

class Lose : public Module
{
public:

	Lose(bool isActive);

	// Destructor
	virtual ~Lose();

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
	SDL_Texture* img;
	SDL_Rect rect;

	pugi::xml_node myNode;
	
	bool playintro = true;

	int waitTime;

	int logoXpos;
	uint music;
	float easingTime = 500; //millisec
	float counter = 0;
};

#endif // __LOSE_H__
