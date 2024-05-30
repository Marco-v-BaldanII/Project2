#include "PauseMenu.h"
#include "Source/Log.h"
#include <stdio.h>

PauseMenu::PauseMenu(bool isActive) : Module(isActive)
{
	name.Create("pauseMenu");

	pauseMenu = false;
}

PauseMenu::~PauseMenu()
{

}

bool PauseMenu::Start()
{	
	menuY = 300;
	pos1 = { (int)windowW / 2 - 140, (int)windowH / 2 - 140 - (int)menuY, 200,60 };
	pos2 = { (int)windowW / 2 - 140, (int)windowH / 2 - 70 - (int)menuY, 200,60 };
	pos3 = { (int)windowW / 2 - 140, (int)windowH / 2 - (int)menuY, 200,60 };
	pos4 = { (int)windowW / 2 - 140, (int)windowH / 2 + 70 - (int)menuY, 200,60 };
	pos5 = { (int)windowW / 2 - 140, (int)windowH / 2 + 140 - (int)menuY, 200,60 };
	pos6 = { (int)windowW / 2 - 140, (int)windowH / 2 + 210 - (int)menuY, 200,60 };
	pos7 = { (int)windowW / 2 - 140, (int)windowH / 2 + 280 - (int)menuY, 200,60 };
	pos8 = { (int)windowW / 2 - 140, (int)windowH / 2 + 350 - (int)menuY, 200,60 };
	pos9 = { (int)windowW / 2 - 140, (int)windowH / 2 + 420 - (int)menuY, 200,60 };

	P_resume = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 13, " Resume ", pos1, this);
	P_FullScreen = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 14, " FullScreen ", pos2, this);
	P_VSync = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 15, " VSync ", pos3, this);
	P_Music = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 16, " Music ", pos4, this, { 0,0 - (int)menuY,40,60 });
	P_FX = (GuiSlider*)app->guiManager->CreateGuiControl(GuiControlType::SLIDER, 17, " FX ", pos5, this, { 0,0 - (int)menuY,40,60 });

	P_Music->state = GuiControlState::NORMAL;
	P_FX->state = GuiControlState::NORMAL;

	P_Music->minValue = 0;
	P_Music->maxValue = 128;
	P_Music->value = 128;

	P_FX->minValue = 0;
	P_FX->maxValue = 128;
	P_FX->value = 128;

	if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		laguageText = " Language : Shakesperean";
	}
	else if (app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		laguageText = " Language : English";
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
		if (menuY > 0) menuY -= animationSpeed;
		if (menuY <= 0) menuY = 0;

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
		if (menuY < 1000) menuY += animationSpeed;
		if (menuY >= 1000) menuY = 1000;
		
		if (menuY >= 1000)
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
	}

	// Cosas de los botones

	if (app->dialogueManager->myLanguage == Language::SHAKESPEREAN)
	{
		P_laguage->text = " Language : Shakesperean ";
	}
	else if (app->dialogueManager->myLanguage == Language::ENGLISH)
	{
		P_laguage->text = " Language : English ";
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

	if (app->levelManager->gameScene != GameScene::START) app->audio->SetFxVolume(P_FX->value);
	if (app->levelManager->gameScene != GameScene::START) app->audio->SetMusicVolume(P_Music->value);

	//update position of the buttons
	P_resume->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 - 140 - (int)menuY, 200,60 };
	P_FullScreen->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 - 70 - (int)menuY, 200,60 };
	P_VSync->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 - (int)menuY, 200,60 };
	P_Music->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 70 - (int)menuY, 200,60 };
	P_FX->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 140 - (int)menuY, 200,60 };
	P_laguage->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 210 - (int)menuY, 200,60 };
	P_textSpeed->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 280 - (int)menuY, 200,60 };
	P_save->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 350 - (int)menuY, 200,60 };
	P_backtomenu->bounds = { (int)windowW / 2 - 140, (int)windowH / 2 + 420 - (int)menuY, 200,60 };

	//update position of the sliders
	P_Music->thumbBounds = { 0,0 - (int)menuY,40,60 };
	P_FX->thumbBounds = { 0,0 - (int)menuY,40,60 };

	return true;
}

bool PauseMenu::PostUpdate()
{
	if (P_Music->state == GuiControlState::NORMAL) app->render->DrawText("FX", pos5.x, pos5.y + 10, pos5.w, pos5.h);
	if (P_Music->state == GuiControlState::NORMAL) app->render->DrawText("Music", pos4.x, pos4.y + 10, pos4.w, pos4.h);

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
		//Before going to the main menu, we need to clean up all the modules
		app->backStage->CleanUp();
		app->turnManager->CleanUp();
		app->battleScene->CleanUp();
		app->castingScene->CleanUp();
		app->backstageplayer->CleanUp();
		app->mainMenu->CleanUp();
		app->audio->CleanUp();

		
		//BACK TO MENU
		app->levelManager->LoadScene(GameScene::START);
		pauseMenu = false;
	}

	return true;
}
