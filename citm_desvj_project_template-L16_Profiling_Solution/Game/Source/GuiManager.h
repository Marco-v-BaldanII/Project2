#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"
#include "p2List.h"
#include "GuiPanel.h"
#include "List.h"
#include "Textures.h"
#include "Render.h"
#include "../Easing.h"

struct SDL_Texture;
class Easing;

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
				app->map->drawGrid = true;
				app->render->DrawTexture(vigneteText, position.x - 500, position.y - 500, &rect, false, 80);
			}
			else {

				/*	SDL_Rect BigRect = SDL_Rect{ 0,0,196,196 };

					app->render->DrawTexture(texture, position.x - 32, position.y - 32, &BigRect, true, 210, 1, 255, 255, 255, 0, 0, 0, SDL_BLENDMODE_ADD);
					SDL_Rect rect = SDL_Rect{ 0,0,1000,1000 };

					app->render->DrawTexture(vigneteText, position.x - 500, position.y - 500, &BigRect, false, 190);*/
			}
			/*Circle c; c = Circle { position, (uint)radius };
			SDL_Rect rect = SDL_Rect{ 0,0,800,800 };
			app->render->DrawRectMask(rect,c, 0,0,0,200, true, true);*/


			//app->render->FillCircle(position.x, position.y, radius*2, color.r, color.g, color.b, color.a/1.5f, true);
			//app->render->DrawCircle(Target.x, Target.y, 2, 255, 0, 0, 255, true);
		}
		else {
			app->map->drawGrid = false;
		}
	}

};

enum VIEW {

	WORLD,
	SCREEN

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
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 }, VIEW viewMode = SCREEN, SDL_Color color = SDL_Color{ 0, 0, 200, 255 }, SDL_Color textColor = SDL_Color{ 255,255,255,255 });

	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Entity* observer, SDL_Rect sliderBounds = { 0,0,0,0 }, VIEW viewMode = SCREEN, SDL_Color color = SDL_Color{ 0, 0, 200, 255 }, SDL_Color textColor = SDL_Color{ 255,255,255,255 });

	bool OnGuiMouseClickEvent(GuiControl* control);

	void TurnOffSpotLight();
	void TurnOnSpotLight();

	void OpenCloseCurtains();

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

	SDL_Texture* normalButton = nullptr;
	SDL_Texture* focusedButton = nullptr;
	SDL_Texture* pressedButton = nullptr;

	SDL_Texture* mouseCursor = nullptr;

	SDL_Texture* spotTexture = nullptr;

	pugi::xml_node myNode;

	uint buttonClickFx = 0;
	uint buttonHoverFx = 0;

	int mainFont;
	int numberFont;
	bool Debug = false;

	List<GuiControl*> guiControlsList;
	SDL_Texture* texture;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	SpotLight* spotLight = nullptr;

	SDL_Texture* Curtain;
	SDL_Rect leftCurtain = SDL_Rect{ 0,0,281,384 };
	SDL_Rect rightCurtain = SDL_Rect{ 284,0,296,384 };

	Easing* easing = nullptr;
	Easing* easingR = nullptr;

	bool curtains = false;
	bool bPause = false;
	bool bPauseR = false;

	Timer curtainTimer;
	bool curtainclosed = false;

	bool credits = false;
	bool shouldCurtainsReOpen = true;

};

class EasingText {
public:

	EasingText(SDL_Texture* texture, iPoint start, iPoint end, float time, float waitTimer = 1.5f) {

		startPos = start;
		endingPos = end;
		this->texture = texture;
		easing = new Easing(time);
		waitTime = waitTimer;
	}

	Easing* easing = nullptr;
	bool bPause = false;
	Timer timer;
	iPoint startPos;
	iPoint endingPos;
	SDL_Texture* texture;
	float waitTime;

	bool firstStep = false;

	void Update() {

		if (!easing->GetFinished())
		{
			int a, b;
			int c, d;
			EasingType easingT;

			if (bPause) {
				a = startPos.x; b = endingPos.x;
				c = startPos.y; d = endingPos.y;
				easingT = EasingType::EASE_INOUT_ELASTIC;
			}
			else {
				a = endingPos.x;
				b = startPos.x;
				c = endingPos.y; d = startPos.y;
				easingT = EasingType::EASE_INOUT_ELASTIC;
			}

			// Calculate interpolated position (tracktime, easinganimaion),
			// and print to screen (DrawRectangle)
			double t = easing->TrackTime(18);
			double easedX = easing->EasingAnimation(a, b, t, easingT);
			double easedY = easing->EasingAnimation(c, d, t, easingT);

			SDL_Rect pauseBox = { easedX, 0, 300, 400 };
			app->render->DrawTexture(texture, easedX + (app->render->camera.x / -3), easedY + (app->render->camera.y / -3));

		}
		else if (bPause)
		{
			app->render->DrawTexture(texture, endingPos.x + (app->render->camera.x / -3), endingPos.y + (app->render->camera.y / -3));

			if (!firstStep) {
				firstStep = true;
				timer.Start();
			}

			if (timer.ReadMSec() > waitTime && firstStep) {
				firstStep = false;

				bPause = !bPause;
				easing->SetFinished(false);

			}

		}
		else {

			app->render->DrawTexture(texture, startPos.x + (app->render->camera.x / -3), startPos.y + (app->render->camera.y / -3));

		}
	}

	void Activate() {


		bPause = !bPause;
		easing->SetFinished(false);
	}

};

#endif // __GUIMANAGER_H__
