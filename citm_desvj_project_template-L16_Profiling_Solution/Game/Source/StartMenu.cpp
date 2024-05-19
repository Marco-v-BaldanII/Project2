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
#include "BattleScene.h"
#include "BackStage.h"
#include "../TurnManager.h"
#include "Victory.h"
#include "Lose.h"
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
		img = app->tex->Load("Assets/Textures/WOTR-MainScreen.png");
		title = app->tex->Load("Assets/Textures/Title.png");
		titleRect = { 0,0,256,192 };
		music = app->audio->PlayMusic("assets/audio/music/title-screen-music.wav", 0.5f);
		//app->dialogueManager->Enable();
		rect = { 0,0,512,384 };
		lastMusicVolume = app->audio->GetMusicVolume();
		lastFxVolume = app->audio->GetFxVolume();
		windowW = 512 * 3;
		windowH = 384 * 3;

		pos1 = { (int)windowW / 2 - 140, (int)windowH / 2 - 140, 200,60 };
		pos2 = { (int)windowW / 2 - 140, (int)windowH / 2 - 70, 200,60 };
		pos3 = { (int)windowW / 2 - 140, (int)windowH / 2, 200,60 };
		pos4 = { (int)windowW / 2 - 140, (int)windowH / 2 + 70, 200,60 };
		pos5 = { (int)windowW / 2 - 140, (int)windowH / 2 + 140, 200,60 };
		pos6 = { (int)windowW / 2 - 140, (int)windowH / 2 + 210, 200,60 };
		pos7 = { (int)windowW / 2 - 140, (int)windowH / 2 + 280, 200,60 };
		pos8 = { (int)windowW / 2 - 140, (int)windowH / 2 + 350, 200,60 };
		pos9 = { (int)windowW / 2 - 140, (int)windowH / 2 + 280, 200,60 };

		start = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, " Start ", pos1, this);
		load = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, " Load ", pos2, this);
		options = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 3, " Options ", pos3, this);
		quit = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 4, " Quit ", pos4, this);

		start->state = GuiControlState::NORMAL;
		load->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

		back = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 5, " Back ", pos1, this);
		FullScreen = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 6, " FullScreen ", pos2, this);
		VSync = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 7, " VSync ", pos3, this);
		Music = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 8, " Music ", pos4, this, { 0,0,40,60 });
		FX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 9, " FX ", pos5, this, { 0,0,40,60 });
		
		Music->state = GuiControlState::NORMAL;
		FX->state = GuiControlState::NORMAL;

		Music->minValue = 0;
		Music->maxValue = 128;
		Music->value = 128;

		FX->minValue = 0;
		FX->maxValue = 128;
		FX->value = 128;

		if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
		{
			laguageText = " Laguage : Shakesperean";
		}
		else if (app->dialogueManager->myLanguage == Language::ENGLISH)
		{
			laguageText = " Laguage : English";
		}
		laguage = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, laguageText, pos6, this);
		textSpeed = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 14, " Text Speed : Normal ", pos7, this);

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
		laguage->state = GuiControlState::DISABLED;
		textSpeed->state = GuiControlState::DISABLED;

		backstage = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 10, " Backstage ", pos2, this);
		combatstage = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 11, " Combatstage ", pos3, this);
		backstage2 = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 69, " Backstage 2", pos4, this);
		win = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 40, " Win ", pos5, this);
		lose = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 41, " Lose ", pos6, this);
		combatstage2 = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 420, " combatstage2 ", pos9, this);

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;
		backstage2->state = GuiControlState::DISABLED;
		win->state = GuiControlState::DISABLED;
		lose->state = GuiControlState::DISABLED;
		combatstage2->state = GuiControlState::DISABLED;

		resume = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, " Resume ", pos1, this);

		resume->state = GuiControlState::DISABLED;

		state = MenuState::START;
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
	switch (state) {
	case MenuState::OFF:
		start->state = GuiControlState::DISABLED;
		load->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
		laguage->state = GuiControlState::DISABLED;
		textSpeed->state = GuiControlState::DISABLED;

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;
		backstage2->state = GuiControlState::DISABLED;
		win->state = GuiControlState::DISABLED;
		lose->state = GuiControlState::DISABLED;
		combatstage2->state = GuiControlState::DISABLED;

		resume->state = GuiControlState::DISABLED;
		break;
	case MenuState::OPTIONS:
		start->state = GuiControlState::DISABLED;
		load->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		back->state = GuiControlState::NORMAL;
		FullScreen->state = GuiControlState::NORMAL;
		VSync->state = GuiControlState::NORMAL;
		Music->state = GuiControlState::NORMAL;
		FX->state = GuiControlState::NORMAL;
		laguage->state = GuiControlState::NORMAL;
		textSpeed->state = GuiControlState::NORMAL;

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;
		backstage2->state = GuiControlState::DISABLED;
		win->state = GuiControlState::DISABLED;
		lose->state = GuiControlState::DISABLED;
		combatstage2->state = GuiControlState::DISABLED;

		resume->state = GuiControlState::DISABLED;
		break;
	case MenuState::START:
		start->state = GuiControlState::NORMAL;
		load->state = GuiControlState::NORMAL;
		quit->state = GuiControlState::NORMAL;
		options->state = GuiControlState::NORMAL;

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
		laguage->state = GuiControlState::DISABLED;
		textSpeed->state = GuiControlState::DISABLED;

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;
		backstage2->state = GuiControlState::DISABLED;
		win->state = GuiControlState::DISABLED;
		lose->state = GuiControlState::DISABLED;
		combatstage2->state = GuiControlState::DISABLED;

		resume->state = GuiControlState::DISABLED;
		break;
	case MenuState::SCENES:
		start->state = GuiControlState::DISABLED;
		load->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		back->state = GuiControlState::NORMAL;
		FullScreen->state = GuiControlState::DISABLED;
		VSync->state = GuiControlState::DISABLED;
		Music->state = GuiControlState::DISABLED;
		FX->state = GuiControlState::DISABLED;
		laguage->state = GuiControlState::DISABLED;
		textSpeed->state = GuiControlState::DISABLED;

		backstage->state = GuiControlState::NORMAL;
		combatstage->state = GuiControlState::NORMAL;
		backstage2->state = GuiControlState::NORMAL;
		win->state = GuiControlState::NORMAL;
		lose->state = GuiControlState::NORMAL;
		combatstage2->state = GuiControlState::NORMAL;

		resume->state = GuiControlState::DISABLED;
		break;
	case MenuState::PAUSE:
		start->state = GuiControlState::DISABLED;
		load->state = GuiControlState::DISABLED;
		quit->state = GuiControlState::DISABLED;
		options->state = GuiControlState::DISABLED;

		back->state = GuiControlState::DISABLED;
		FullScreen->state = GuiControlState::NORMAL;
		VSync->state = GuiControlState::NORMAL;
		Music->state = GuiControlState::NORMAL;
		FX->state = GuiControlState::NORMAL;
		laguage->state = GuiControlState::NORMAL;
		textSpeed->state = GuiControlState::NORMAL;

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;
		backstage2->state = GuiControlState::DISABLED;
		win->state = GuiControlState::DISABLED;
		lose->state = GuiControlState::DISABLED;
		combatstage2->state = GuiControlState::DISABLED;

		resume->state = GuiControlState::NORMAL;
		break;
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		//state = MenuState::PAUSE;
	}

	if (cerrar)
	{
		return false;
	}

	if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		laguage->text = " Laguage : Shakesperean ";
	}
	else if (app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		laguage->text = " Laguage : English ";
	}

	if (app->dialogueManager->textVelocity == app->dialogueManager->slowVelocity)
	{
		textSpeed->text = " Text Speed : Slow ";
	}
	else if (app->dialogueManager->textVelocity == app->dialogueManager->normalVelocity)
	{
		textSpeed->text = " Text Speed : Normal ";
	}
	else if (app->dialogueManager->textVelocity == app->dialogueManager->fastVelocity)
	{
		textSpeed->text = " Text Speed : Fast ";
	}

	if(full)
	{
		FullScreen->text = " FullScreen : On ";
	}
	else if (!full)
	{
		FullScreen->text = " FullScreen : Off ";
	}

	if (vsyncOn)
	{
		VSync->text = " VSync : On ";
	}
	else if (!vsyncOn)
	{
		VSync->text = " VSync : Off ";
	}

	if (active) app->audio->SetFxVolume(FX->value);
	if (active) app->audio->SetMusicVolume(Music->value);

	return true;
}

// Called each loop iteration
bool StartMenu::PostUpdate()
{
	bool ret = true;
	app->render->DrawTexture(img, 0 + (app->render->camera.x /-3), 0 + (app->render->camera.y / -3), &rect);
	app->render->DrawTexture(title, 120 + (app->render->camera.x / -3), 20 + (app->render->camera.y / -3), &titleRect);

	if (Music->state == GuiControlState::NORMAL) app->render->DrawText("FX", pos5.x, pos5.y + 10, pos5.w, pos5.h);
	if (Music->state == GuiControlState::NORMAL) app->render->DrawText("Music", pos4.x, pos4.y + 10, pos4.w, pos4.h);
	return ret;
}

// Called before quitting
bool StartMenu::CleanUp()
{
	LOG("Freeing scene main menu ");
	img = nullptr;
	//app->guiManager->Main_Menu_Panel->Disable();
	//app->audio->StopMusic();
	return true;
	
}

bool StartMenu::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 1)
	{ 
		state = MenuState::SCENES;
	}
	else if (control->id == 2)
	{
		//app->levelManager->LoadScene(GameScene::BACKSTAGE);
	}
	else if (control->id == 3)
	{
		state = MenuState::OPTIONS;
	}
	else if (control->id == 4)
	{
		cerrar = true;
	}
	else if (control->id == 5)
	{
		state = MenuState::START;
	}
	else if (control->id == 6 && !full)
	{
		full = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
	}
	else if (control->id == 6 && full)
	{
		full = false;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_SHOWN);
	}
	else if (control->id == 7 && !vsyncOn)
	{
		//funcion para activar vsync
		vsyncOn = true;
		SDL_GL_SetSwapInterval(1);
	}
	else if (control->id == 7 && vsyncOn)
	{
		//funcion para desactivar vsync
		vsyncOn = false;
		SDL_GL_SetSwapInterval(0);
	}
	else if (control->id == 10)
	{
		app->entityManager->Enable();
		app->dialogueManager->Enable();
	 	app->levelManager->LoadScene(GameScene::CASTING);
		state = MenuState::OFF;

	}
	else if (control->id == 11)
	{
		app->entityManager->Enable();
		app->turnManager->Enable();
		app->battleScene->Enable();
		app->levelManager->LoadScene(GameScene::COMBAT);
		state = MenuState::OFF;
	}
	else if (control->id == 12)
	{
		state = MenuState::OFF;
	}
	else if (control->id == 13 && app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		//lagunage
		app->dialogueManager->SetLanguage(Language::ENGLISH);
	}
	else if (control->id == 13 && app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		//lagunage
		app->dialogueManager->SetLanguage(Language::SHAKESPEREAN);
	}
	else if (control->id == 14 && app->dialogueManager->textVelocity == app->dialogueManager->slowVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::NORMAL);
	}
	else if (control->id == 14 && app->dialogueManager->textVelocity == app->dialogueManager->normalVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::FAST);
	}
	else if (control->id == 14 && app->dialogueManager->textVelocity == app->dialogueManager->fastVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::SLOW);
	}
	else if (control->id == 40)
	{
		app->levelManager->LoadScene(GameScene::WIN);
		state = MenuState::OFF;
	}
	else if (control->id == 41)
	{
		app->levelManager->LoadScene(GameScene::LOSE);
		state = MenuState::OFF;
	}
	else if (control->id == 69)
	{
		app->entityManager->Enable();
		app->dialogueManager->Enable();
		app->levelManager->LoadScene(GameScene::BACKSTAGE2);
		state = MenuState::OFF;
	}
	else if (control->id == 420)
	{
		//app->levelManager->LoadScene(GameScene::BACKSTAGE2);
		app->map->blockedGid = 187;
		app->map->Disable();
		app->map->CleanUp();
		app->map->mapData.layers.Clear();
		app->map->level = 1;
		app->map->Enable();
		
		app->levelManager->LoadScene(GameScene::COMBAT);
		state = MenuState::OFF;
	}

	return true;
}
