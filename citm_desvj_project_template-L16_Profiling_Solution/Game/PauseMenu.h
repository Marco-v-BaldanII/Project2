#pragma once
#include "Source/App.h"
#include "Source/Render.h"
#include "Source/Audio.h"
#include "Source/Defs.h"
#include "Source/Input.h"
#include "Source/EntityManager.h"
#include "Source/Entity.h"
#include "Source/Physics.h"
#include "Source/Map.h"
#include "Source/LevelManagement.h"
#include "Source/Animation.h"
#include "Source/Point.h"
#include "Source/Textures.h"
#include "Source/Module.h"
#include "Source/GuiControl.h"
#include "Source/GuiControlButton.h"
#include "Source/GuiSlider.h"
#include "Source/GuiPanel.h"
#include "Source/GuiToggle.h"
#include "Source/GuiManager.h"
#include "Source/Window.h"
#include "DialogueManager.h"
#include "BackstagePlayer.h"
#include "Source/BackStage.h"
#include "Source/BattleScene.h"
#include "TurnManager.h"
#include "CastingScene.h"

class PauseMenu : public Module
{
public:
	PauseMenu(bool isActive);
	~PauseMenu();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	
	bool pauseMenu = false;

	GuiControlButton* P_resume = nullptr;
	GuiControlButton* P_FullScreen = nullptr;
	GuiControlButton* P_VSync = nullptr;
	GuiSlider* P_Music = nullptr;
	GuiSlider* P_FX = nullptr;
	GuiControlButton* P_laguage = nullptr;
	GuiControlButton* P_textSpeed = nullptr;
	GuiControlButton* P_save = nullptr;
	GuiControlButton* P_backtomenu = nullptr;

	SDL_Rect pos1;
	SDL_Rect pos2;
	SDL_Rect pos3;
	SDL_Rect pos4;
	SDL_Rect pos5;
	SDL_Rect pos6;
	SDL_Rect pos7;
	SDL_Rect pos8;
	SDL_Rect pos9;

	uint windowW = 512 * 3;
	uint windowH = 384 * 3;

	float menuY = 1000;
	const float animationSpeed = 30.0f;

	const char* laguageText;
};