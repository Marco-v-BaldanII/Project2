#pragma once
#include "Module.h"
#include "p2List.h"
#include "Defs.h"
#include "Animation.h"
#include "Point.h"
#include "Entity.h"
#include "../BackstagePlayer.h"

struct SDL_Texture;

class BackStage : public Module
{
public:

	BackStage(bool isActive);

	// Destructor
	virtual ~BackStage();

	// Called before render is available
	bool Awake(pugi::xml_node&);

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

	iPoint uncheckableTiles[3];
	bool hasBeenCreated;

private:
	SDL_Texture* img;
	SDL_Texture* background;
	SDL_Rect rect;
	bool createplayer = false;
	uint music;

	//create player
	Entity* player;
};

