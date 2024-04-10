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
		lastMusicVolume = app->audio->GetMusicVolume();
		lastFxVolume = app->audio->GetFxVolume();

		windowW = 512 * 3;
		windowH = 384 * 3;

		SDL_Rect pos1 = { windowW / 2 - 80, windowH / 2 - 90, 200,60 };
		SDL_Rect pos2 = { windowW / 2 - 80, windowH / 2 - 10, 200,60 };
		SDL_Rect pos3 = { windowW / 2 - 80, windowH / 2 + 60, 200,60 };
		SDL_Rect pos4 = { windowW / 2 - 80, windowH / 2 + 130, 200,60 };
		SDL_Rect pos5 = { windowW / 2 - 80, windowH / 2 + 200, 200,60 };
		SDL_Rect pos6 = { windowW / 2 - 80, windowH / 2 + 270, 200,60 };
		SDL_Rect pos7 = { windowW / 2 - 80, windowH / 2 + 340, 200,60 };
		SDL_Rect pos8 = { windowW / 2 - 80, windowH / 2 + 410, 200,60 };

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
		Music = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 8, " Music ", pos4, this);
		FX = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 9, " FX ", pos5, this);
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

		backstage->state = GuiControlState::DISABLED;
		combatstage->state = GuiControlState::DISABLED;

		resume = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 12, " Resume ", pos1, this);

		resume->state = GuiControlState::DISABLED;
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

		resume->state = GuiControlState::NORMAL;
		break;
	}

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		state = MenuState::PAUSE;
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

	if (musicOn)
	{
		Music->text = " Music : On ";
	}
	else if (!musicOn)
	{
		Music->text = " Music : Off ";
	}

	if (fxOn)
	{
		FX->text = " FX : On ";
	}
	else if (!fxOn)
	{
		FX->text = " FX : Off ";
	}

	LOG("Music Volume %d", app->audio->GetMusicVolume());
	LOG("Fx Volume %d", app->audio->GetFxVolume());

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
	else if (control->id == 8 && !musicOn)
	{
		//activar musica
		musicOn = true;
		app->audio->SetMusicVolume(lastMusicVolume);
	}
	else if (control->id == 8 && musicOn)
	{
		//desactivar musica
		musicOn = false;
		lastMusicVolume = app->audio->GetMusicVolume();
		app->audio->SetMusicVolume(0.0f);
	}
	else if (control->id == 9 && !fxOn)
	{
		//activar fx
		fxOn = true;
		app->audio->SetFxVolume(lastFxVolume);
	}
	else if (control->id == 9 && fxOn)
	{
		//desactivar fx
		fxOn = false;
		lastFxVolume = app->audio->GetFxVolume();
		app->audio->SetFxVolume(0.0f);
	}
	else if (control->id == 10)
	{
		app->levelManager->LoadScene(GameScene::BACKSTAGE);
		state = MenuState::OFF;

	}
	else if (control->id == 11)
	{
		app->entityManager->Enable();
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

	return true;
}
