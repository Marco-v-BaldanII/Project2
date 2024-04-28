#include "PauseMenu.h"
#include "Source/Log.h"

PauseMenu::PauseMenu(bool isActive) : Module(isActive)
{
}

PauseMenu::~PauseMenu()
{

}

bool PauseMenu::Start()
{
	uint windowW = 512 * 3;
	uint windowH = 384 * 3;
	
	SDL_Rect pos1 = { windowW / 2 - 140, windowH / 2 - 140, 200,60 };
	SDL_Rect pos2 = { windowW / 2 - 140, windowH / 2 - 70, 200,60 };
	SDL_Rect pos3 = { windowW / 2 - 140, windowH / 2, 200,60 };
	SDL_Rect pos4 = { windowW / 2 - 140, windowH / 2 + 70, 200,60 };
	SDL_Rect pos5 = { windowW / 2 - 140, windowH / 2 + 140, 200,60 };
	SDL_Rect pos6 = { windowW / 2 - 140, windowH / 2 + 210, 200,60 };
	SDL_Rect pos7 = { windowW / 2 - 140, windowH / 2 + 280, 200,60 };
	SDL_Rect pos8 = { windowW / 2 - 140, windowH / 2 + 350, 200,60 };
	SDL_Rect pos9 = { windowW / 2 - 140, windowH / 2 + 420, 200,60 };

	P_resume = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, " Resume ", pos1, this);
	P_FullScreen = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 14, " FullScreen ", pos2, this);
	P_VSync = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 15, " VSync ", pos3, this);
	P_Music = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 16, " Music ", pos4, this);
	P_FX = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 17, " FX ", pos5, this);
	if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		laguageText = " Laguage : Shakesperean";
	}
	else if (app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		laguageText = " Laguage : English";
	}
	P_laguage = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 18, laguageText, pos6, this);
	P_textSpeed = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 19, " Text Speed : Normal ", pos7, this);
	P_save = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 20, " Save ", pos8, this);
	P_backtomenu = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 21, " Back to Menu ", pos9, this);

	P_resume->state = GuiControlState::DISABLED;
	P_FullScreen->state = GuiControlState::DISABLED;
	P_VSync->state = GuiControlState::DISABLED;
	P_Music->state = GuiControlState::DISABLED;
	P_FX->state = GuiControlState::DISABLED;
	P_laguage->state = GuiControlState::DISABLED;
	P_textSpeed->state = GuiControlState::DISABLED;
	P_save->state = GuiControlState::DISABLED;
	P_backtomenu->state = GuiControlState::DISABLED;

	return true;
}

bool PauseMenu::PreUpdate()
{
	
	return true;
}

bool PauseMenu::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN && (app->levelManager->gameScene == GameScene::BACKSTAGE || app->levelManager->gameScene == GameScene::BACKSTAGE2 || app->levelManager->gameScene == GameScene::COMBAT))
	{
		pauseMenu = !pauseMenu;
		app->backstageplayer->SetCanMove(!pauseMenu);
	}

	if (pauseMenu)
	{
		if (app->levelManager->gameScene == GameScene::BACKSTAGE) app->backstageplayer->SetCanMove(false);

		P_resume->state = GuiControlState::NORMAL;
		P_FullScreen->state = GuiControlState::NORMAL;
		P_VSync->state = GuiControlState::NORMAL;
		P_Music->state = GuiControlState::NORMAL;
		P_FX->state = GuiControlState::NORMAL;
		P_laguage->state = GuiControlState::NORMAL;
		P_textSpeed->state = GuiControlState::NORMAL;
		P_save->state = GuiControlState::NORMAL;
		P_backtomenu->state = GuiControlState::NORMAL;
	}
	else if (!pauseMenu)
	{
		if (app->levelManager->gameScene == GameScene::BACKSTAGE) app->backstageplayer->SetCanMove(true);

		P_resume->state = GuiControlState::DISABLED;
		P_FullScreen->state = GuiControlState::DISABLED;
		P_VSync->state = GuiControlState::DISABLED;
		P_Music->state = GuiControlState::DISABLED;
		P_FX->state = GuiControlState::DISABLED;
		P_laguage->state = GuiControlState::DISABLED;
		P_textSpeed->state = GuiControlState::DISABLED;
		P_save->state = GuiControlState::DISABLED;
		P_backtomenu->state = GuiControlState::DISABLED;
	}

	// Cosas de los botones

	if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		P_laguage->text = " Laguage : Shakesperean ";
	}
	else if (app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		P_laguage->text = " Laguage : English ";
	}

	if (app->dialogueManager->textVelocity == app->dialogueManager->slowVelocity)
	{
		P_textSpeed->text = " Text Speed : Slow ";
	}
	else if (app->dialogueManager->textVelocity == app->dialogueManager->normalVelocity)
	{
		P_textSpeed->text = " Text Speed : Normal ";
	}
	else if (app->dialogueManager->textVelocity == app->dialogueManager->fastVelocity)
	{
		P_textSpeed->text = " Text Speed : Fast ";
	}

	if (app->mainMenu->full)
	{
		P_FullScreen->text = " FullScreen : On ";
	}
	else if (!app->mainMenu->full)
	{
		P_FullScreen->text = " FullScreen : Off ";
	}

	if (app->mainMenu->vsyncOn)
	{
		P_VSync->text = " VSync : On ";
	}
	else if (!app->mainMenu->vsyncOn)
	{
		P_VSync->text = " VSync : Off ";
	}

	if (app->mainMenu->musicOn)
	{
		P_Music->text = " Music : On ";
	}
	else if (!app->mainMenu->musicOn)
	{
		P_Music->text = " Music : Off ";
	}

	if (app->mainMenu->fxOn)
	{
		P_FX->text = " FX : On ";
	}
	else if (!app->mainMenu->fxOn)
	{
		P_FX->text = " FX : Off ";
	}

	return true;
}

bool PauseMenu::PostUpdate()
{
	
	return true;
}

bool PauseMenu::CleanUp()
{
	
	return true;
}

bool PauseMenu::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 13)
	{
		pauseMenu = false;
	}
	else if (control->id == 14 && !app->mainMenu->full)
	{
		app->mainMenu->full = true;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_FULLSCREEN);
	}
	else if (control->id == 14 && app->mainMenu->full)
	{
		app->mainMenu->full = false;
		SDL_SetWindowFullscreen(app->win->window, SDL_WINDOW_SHOWN);
	}
	else if (control->id == 15 && !app->mainMenu->vsyncOn)
	{
		//funcion para activar vsync
		app->mainMenu->vsyncOn = true;
		SDL_GL_SetSwapInterval(1);
	}
	else if (control->id == 15 && app->mainMenu->vsyncOn)
	{
		//funcion para desactivar vsync
		app->mainMenu->vsyncOn = false;
		SDL_GL_SetSwapInterval(0);
	}
	else if (control->id == 16 && !app->mainMenu->musicOn)
	{
		//activar musica
		app->mainMenu->musicOn = true;
		app->audio->SetMusicVolume(app->mainMenu->lastMusicVolume);
	}
	else if (control->id == 16 && app->mainMenu->musicOn)
	{
		//desactivar musica
		app->mainMenu->musicOn = false;
		app->mainMenu->lastMusicVolume = app->audio->GetMusicVolume();
		app->audio->SetMusicVolume(0.0f);
	}
	else if (control->id == 17 && !app->mainMenu->fxOn)
	{
		//activar fx
		app->mainMenu->fxOn = true;
		app->audio->SetFxVolume(app->mainMenu->lastFxVolume);
	}
	else if (control->id == 17 && app->mainMenu->fxOn)
	{
		//desactivar fx
		app->mainMenu->fxOn = false;
		app->mainMenu->lastFxVolume = app->audio->GetFxVolume();
		app->audio->SetFxVolume(0.0f);
	}
	else if (control->id == 18 && app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		//lagunage
		app->dialogueManager->SetLanguage(Language::ENGLISH);
	}
	else if (control->id == 18 && app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		//lagunage
		app->dialogueManager->SetLanguage(Language::SHAKESPEREAN);
	}
	else if (control->id == 19 && app->dialogueManager->textVelocity == app->dialogueManager->slowVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::NORMAL);
	}
	else if (control->id == 19 && app->dialogueManager->textVelocity == app->dialogueManager->normalVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::FAST);
	}
	else if (control->id == 19 && app->dialogueManager->textVelocity == app->dialogueManager->fastVelocity)
	{
		app->dialogueManager->SetTextVelocity(TextVelocity::SLOW);
	}
	else if (control->id == 20)
	{
		//SAVE STATE
		//anadir todos los savestate aqui
	}
	else if (control->id == 21)
	{
		//BACK TO MENU
		app->levelManager->LoadScene(GameScene::START);
		pauseMenu = false;
	}

	return true;
}