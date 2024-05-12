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

	GuiControlButton* P_resume;
	GuiControlButton* P_FullScreen;
	GuiControlButton* P_VSync;
	GuiSlider* P_Music;
	GuiSlider* P_FX;
	GuiControlButton* P_laguage;
	GuiControlButton* P_textSpeed;
	GuiControlButton* P_save;
	GuiControlButton* P_backtomenu;

	SDL_Rect pos1;
	SDL_Rect pos2;
	SDL_Rect pos3;
	SDL_Rect pos4;
	SDL_Rect pos5;
	SDL_Rect pos6;
	SDL_Rect pos7;
	SDL_Rect pos8;
	SDL_Rect pos9;

	const char* laguageText;
};