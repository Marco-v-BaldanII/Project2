#ifndef __INTRO_H__
#define __INTRO_H__

#include "Module.h"
//#include "p2List.h"
#include "Defs.h"
#include "Physics.h"
#include "Animation.h"

struct SDL_Texture;
class EnemyDummy;

class Intro : public Module
{
public:

	Intro(bool isActive);

	// Destructor
	virtual ~Intro();

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

	float EaseInElastic(float t, float b, float c, float d);
	float EaseInQuad(float t, float b, float c, float d);

	uint intro;
private:
	SDL_Texture* part1;
	SDL_Texture* part2;
	SDL_Rect rect;
	SDL_Rect rect2;

	float initLogo1PosX;
	float initLogo2PosX;
	float Logo1PosX;
	float Logo2PosX;

	pugi::xml_node myNode;
	
	bool playintro = true;

	int waitTime;
	int EasingwaitTime;

	int logoXpos;
	uint music;
	float easingTime = 500; //millisec
	float counter = 0;
};

#endif // __INTRO_H__
