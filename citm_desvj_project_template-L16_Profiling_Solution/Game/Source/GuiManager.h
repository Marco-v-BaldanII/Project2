#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"
#include "p2List.h"
#include "GuiPanel.h"
#include "List.h"
#include "Textures.h"
#include "Render.h"

struct SDL_Texture;

enum LightMode {
	DIALOG,
	BAT
};

class SpotLight {

public:

	SpotLight(int radius, SDL_Color color, float speed) {

		this->radius = radius;
		this->color = color;
		this->speed = speed;
		texture = app->tex->Load("Assets/Textures/spotLight.png");
		vigneteText = app->tex->Load("Assets/Textures/DarkVignete.png");
	}

	int radius;
	SDL_Color color;

	float speed;

	LightMode mode = DIALOG;

	iPoint Target;
	iPoint position;
	SDL_Texture* texture;
	SDL_Texture* vigneteText;
	SDL_Rect vigneteRect = SDL_Rect{ 0,0,1000,1000 };

	bool visible = false;

	void Lerp3(int& value, float t, int destination) {
		value += (t) * (destination - value);
	}

	void MoveToTarget() {

		if (position.x == Target.x) {
			int u = 9;
		}
		

		Lerp3(position.x, 0.1f, Target.x);
		Lerp3(position.y, 0.1f, Target.y);
	}
	void Render() {
		if (visible == true) {

			/*
				app->render->FillCircle(position.x, position.y, radius, color.r, color.g, color.b, color.a / 2.6f, true, SDL_BLENDMODE_ADD);
				app->render->FillCircle(position.x, position.y, radius * 1.2f, color.r, color.g, color.b, color.a / 3.0f, true, SDL_BLENDMODE_ADD);
				app->render->FillCircle(position.x, position.y, radius * 1.35f, color.r, color.g, color.b, color.a / 3.2f, true, SDL_BLENDMODE_ADD);
				app->render->FillCircle(position.x, position.y, radius * 1.5f, color.r, color.g, color.b, color.a / 3.4f, true, SDL_BLENDMODE_ADD);*/
			if (mode == BAT) {
				app->render->DrawTexture(texture, position.x - 32, position.y - 32, (const SDL_Rect*)0, true, 210, 1, 255, 255, 255, 0, 0, 0, SDL_BLENDMODE_ADD);
				SDL_Rect rect = SDL_Rect{ 0,0,1000,1000 };

				app->render->DrawTexture(vigneteText, position.x - 500, position.y - 500, &rect, false, 190);
			}
			else {

				SDL_Rect BigRect = SDL_Rect{ 0,0,196,196 };

				app->render->DrawTexture(texture, position.x - 32, position.y - 32, &BigRect, true, 210, 1, 255, 255, 255, 0, 0, 0, SDL_BLENDMODE_ADD);
				/*SDL_Rect rect = SDL_Rect{ 0,0,1000,1000 };

				app->render->DrawTexture(vigneteText, position.x - 500, position.y - 500, &rect, false, 190);*/
			}
				/*Circle c; c = Circle { position, (uint)radius };
				SDL_Rect rect = SDL_Rect{ 0,0,800,800 };
				app->render->DrawRectMask(rect,c, 0,0,0,200, true, true);*/
			

			//app->render->FillCircle(position.x, position.y, radius*2, color.r, color.g, color.b, color.a/1.5f, true);
			//app->render->DrawCircle(Target.x, Target.y, 2, 255, 0, 0, 255, true);
		}
	}

};

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager(bool isActive);

	// Destructor
	virtual ~GuiManager();

	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	 // Called each loop iteration
	bool Update(float dt);
	bool UpdateAll(float dt, bool logic); 
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	void OnPause(bool paused);

	void OpenPanel(PanelID panel_id);

	bool OnGuiMouseClickEvent(GuiControl* control);

	void TurnOffSpotLight();
	void TurnOnSpotLight();

public:
	//Gui textures Used
	SDL_Texture* UItexture = nullptr;
	SDL_Texture* UItexture2 = nullptr;
	SDL_Texture* tex_smallPanel = nullptr;
	SDL_Texture* tex_mediumPanel = nullptr;
	SDL_Texture* tex_bigPanel = nullptr;
	SDL_Texture* tex_inventory = nullptr;
	SDL_Texture* tex_inventory_tabs = nullptr;
	SDL_Texture* tex_inventory_Items = nullptr;

	pugi::xml_node myNode;

	uint buttonClickFx = 0;
	uint buttonHoverFx = 0;

	int mainFont;
	int numberFont;
	bool Debug = false;

	List<GuiControl*> guiControlsList;
	SDL_Texture* texture;
	p2List<GuiPanel*> panels;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	GuiPanel *Main_Menu_Panel = nullptr;
	GuiPanel *Settings_Panel = nullptr;
	GuiPanel *Pause_Panel = nullptr;

	p2List<PanelID> panelsID;
	PanelID lastPanel;
	int currentPanel;

	SpotLight* spotLight = nullptr;
};

#endif // __GUIMANAGER_H__
