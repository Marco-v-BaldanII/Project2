#pragma once
#include "../Module.h"
#include "../p2List.h"
#include "../Defs.h"
#include "../Animation.h"
#include "../Point.h"
#include "../Entity.h"

struct SDL_Texture;
struct ActorSprite;
struct TextureDef;
using namespace std;

struct Actor{
public:

	Actor(string name, string dialogue) {
		this->name = name;
		this->dialogue = dialogue;
	}

	string name ="unNamed";
	string dialogue = "I am error";

	UnitType myClass;
	string role = "tree";

	SDL_Texture* texture = nullptr;

	bool selected = false;

};

class CastingScene : public Module
{
public:

	CastingScene(bool isActive);

	// Destructor
	virtual ~CastingScene();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	bool PreUpdate();
	
	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void AssignRole();

	void RemoveRoles();

private:
	//buttons and variables
	//GuiControlButton* P_resume;
	//GuiControlButton* P_FullScreen;
	//GuiControlButton* P_VSync;
	//GuiControlButton* P_Music;
	//GuiControlButton* P_FX;
	//GuiControlButton* P_laguage;
	//GuiControlButton* P_textSpeed;
	//GuiControlButton* P_save;
	//GuiControlButton* P_backtomenu;

	//GuiPanel* testPanel;
	SDL_Rect portraitBox = SDL_Rect{ 0,0,96,118 };

	pugi::xml_node config;

	SDL_Texture* background = nullptr;
	 List< ActorSprite*> actorSprites;
	
	 DynArray<Actor*> actors;

	 DynArray<TextureDef*> roles;
	 DynArray<bool> assignedRoles;

	 int roleIndex = 0;
	 int actorIndex = 0;

	 bool finishedCasting = false;

	 ActorSprite* actorPortrait;
	 bool loaded = false;

};

