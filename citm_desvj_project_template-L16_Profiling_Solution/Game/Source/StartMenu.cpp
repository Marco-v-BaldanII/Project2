#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "StartMenu.h"
#include "App.h"
#include "GuiManager.h"
#include "LevelManagement.h"
#include "../DialogueManager.h"
//#include "Fonts.h"
#include "Defs.h"
#include "Log.h"

StartMenu::StartMenu(bool isActive) : Module(isActive)
{
	name.Create("start_menu");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
StartMenu::~StartMenu()
{}

// Called before render is available
bool StartMenu::Awake()
{
	LOG("Loading Start Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool StartMenu::Start()
{
	if (active)
	{
		img = app->tex->Load("Assets/Textures/portrait1.png");
		music = app->audio->PlayMusic("assets/audio/music/title-screen-music.wav", 0.5f);
		//app->dialogueManager->Enable();
		rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
		//app->guiManager->OpenPanel(PanelID::P_START_MENU);  //IMPORTANT

		//Menu Buttons
		app->win->GetWindowSize(windowW, windowH);

		SDL_Rect playPos = { windowW / 2 - 60, windowH / 2 + 10, 100,50 };
		SDL_Rect loadPos = { windowW / 2 - 60, windowH / 2 + 90, 100,50 };
		SDL_Rect quitPos = { windowW / 2 - 60, windowH / 2 + 180, 100,50 };
		SDL_Rect optionsPos = { windowW / 2 - 60, windowH / 2 + 270, 100,50 };

		start = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "   Play   ", playPos, this);
		load = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "   Load   ", loadPos, this);
		options = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, "   Options   ", optionsPos, this);
		quit = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, "   Quit   ", quitPos, this);

		start->state = GuiControlState::NORMAL;
		load->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

		//Config Buttons
		SDL_Rect backPos = { windowW / 2 + 60, windowH / 2 + 10, 100,50 };
		SDL_Rect FullScreenPos = { windowW / 2 + 60, windowH / 2 + 90, 100,50 };
		SDL_Rect VSyncPos = { windowW / 2 + 60, windowH / 2 + 180, 100,50 };
		SDL_Rect MusicPos = { windowW / 2 + 60, windowH / 2 + 270, 100,50 };
		SDL_Rect FXPos = { windowW / 2 + 60, windowH / 2 + 360, 100,50 };

		back = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, "   Back   ", backPos, this);
		FullScreen = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, "   FullScreen   ", FullScreenPos, this);
		VSync = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, "   VSync   ", VSyncPos, this);
		Music = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, "   Music   ", MusicPos, this);
		FX = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, "   FX   ", FXPos, this);

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
	}

	return true;
}

// Called each loop iteration
bool StartMenu::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool StartMenu::Update(float dt)
{
	if (goConfig)
	{
		start->state = GuiControlState::DISABLED;
		load->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		back->state = GuiControlState::NORMAL;
		FullScreen->state = GuiControlState::NORMAL;
		VSync->state = GuiControlState::NORMAL;
		Music->state = GuiControlState::NORMAL;
		FX->state = GuiControlState::NORMAL;
	}
	else if (!goConfig)
	{
		start->state = GuiControlState::NORMAL;
		load->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
	}


	return true;
}

// Called each loop iteration
bool StartMenu::PostUpdate()
{
	bool ret = true;
	//app->render->DrawTexture(img, 0, 0, &rect);
	return ret;
}

// Called before quitting
bool StartMenu::CleanUp()
{
	LOG("Freeing scene main menu ");
	img = nullptr;
	app->guiManager->Main_Menu_Panel->Disable();
	//app->audio->StopMusic();
	return true;
	
}

bool StartMenu::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 1)
	{ 
		app->levelManager->LoadScene(GameScene::BACKSTAGE);
	}
	else if (control->id == 2)
	{
		app->levelManager->LoadScene(GameScene::BACKSTAGE);
	}
	else if (control->id == 3)
	{
		goConfig = true;
	}
	else if (control->id == 4)
	{
		return false;
	}
	else if (control->id == 5)
	{
		goConfig = false;
	}
	else if (control->id == 6)
	{

	}
	else if (control->id == 7)
	{

	}
	else if (control->id == 8)
	{

	}
	else if (control->id == 9)
	{

	}

	return true;
}
