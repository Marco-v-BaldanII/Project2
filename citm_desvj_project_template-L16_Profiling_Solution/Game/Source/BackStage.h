#pragma once
#include "Module.h"
#include "p2List.h"
#include "Defs.h"
#include "Animation.h"
#include "Point.h"
#include "Entity.h"
#include "../BackstagePlayer.h"
#include "../NPC.h"

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

	bool OnGuiMouseClickEvent(GuiControl* control);

	void FinishBackStage();

	//bool Load(pugi::xml_node& data);

	//bool Save(pugi::xml_node& data) const;

	iPoint uncheckableTiles[3];
	bool hasBeenCreated;

	pugi::xml_node mynode;

	List<Npc*> npcsList;

	int backStageID = 0;

private:
	//buttons and variables
	GuiControlButton* P_resume;
	GuiControlButton* P_FullScreen;
	GuiControlButton* P_VSync;
	GuiControlButton* P_Music;
	GuiControlButton* P_FX;
	GuiControlButton* P_laguage;
	GuiControlButton* P_textSpeed;
	GuiControlButton* P_save;
	GuiControlButton* P_backtomenu;

	bool pausemenu;

	const char* laguageText;

	int windowH;
	int windowW;

	SDL_Texture* img;
	SDL_Texture* background;
	SDL_Rect rect;
	bool createplayer = false;
	uint music;

	//create player
	Entity* player;
};

