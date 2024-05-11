#include "GuiManager.h"
#include "App.h"
#include "Textures.h"
#include "MainMenuPanel.h"
#include "GuiControlButton.h"
#include "GuiSlider.h"
#include "GuiPanel.h"
#include "../Easing.h"
//#include "GuiControlButton.h"
#include "Audio.h"

GuiManager::GuiManager(bool isActive) :Module(isActive)
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Awake(pugi::xml_node config) {

	myNode = config;
	return true;
}

bool GuiManager::Start()
{
	Main_Menu_Panel = new MainMenuPanel(false);
	//Options_Panel = new OptionsPanel(false);
	//Pause_Panel = new PausePanel(false);
	//MORE PANELS TOO ADD

	easing = new Easing(1.2f);
	easingR = new Easing(1.2f);

	panels.add(Main_Menu_Panel);
	//panels.add(Options_Panel);
	//panels.add(Pause_Panel);

	//LOAD TEXTURES
	Curtain = app->tex->Load("Assets/Textures/UI/curtains.png");
	string s = myNode.child("UItexture").attribute("path").as_string();
	UItexture = app->tex->Load(myNode.child("UItexture").attribute("path").as_string());
	UItexture2 = app->tex->Load(myNode.child("UItexture2").attribute("path").as_string());
	tex_smallPanel = app->tex->Load(myNode.child("tex_smallPanel").attribute("path").as_string());
	tex_mediumPanel = app->tex->Load(myNode.child("tex_mediumPanel").attribute("path").as_string());
	tex_bigPanel = app->tex->Load(myNode.child("tex_bigPanel").attribute("path").as_string());
	tex_inventory = app->tex->Load(myNode.child("tex_inventory").attribute("path").as_string());
	tex_inventory_tabs = app->tex->Load(myNode.child("tex_inventory_tabs").attribute("path").as_string());
	tex_inventory_Items = app->tex->Load(myNode.child("tex_inventory_Items").attribute("path").as_string());

	normalButton = app->tex->Load("Assets/Textures/UI/marco1.png");
	focusedButton = app->tex->Load("Assets/Textures/UI/marco2.png");
	pressedButton = app->tex->Load("Assets/Textures/UI/marco3.png");

	//Button effect
	buttonHoverFx = app->audio->LoadFx("Assets/Audio/Fx/coin.ogg");
	buttonClickFx = app->audio->LoadFx("Assets/Audio/Fx/coin.ogg");

	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		panel->data->Start();
		panel = panel->next;
	}

	spotLight = new SpotLight(64, SDL_Color{ 242,149,98,200 }, 0.02f);
	spotLight->visible = false;

	return true;
}

bool GuiManager::PreUpdate() { return true; }

bool GuiManager::Update(float dt)
{

	accumulatedTime += dt;
	if (accumulatedTime >= updateMsCycle) doLogic = true;

	UpdateAll(dt, doLogic);

	if (doLogic == true)
	{
		accumulatedTime = 0.0f;
		doLogic = false;
	}

	iPoint MousePose; app->input->GetMouseWorldPosition(MousePose.x, MousePose.y);

	//spotLight->Target = MousePose;

	spotLight->MoveToTarget();
	

	return true;
}

bool GuiManager::UpdateAll(float dt, bool doLogic) {

	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		if (panel->data->Active)
			panel->data->Update(dt, doLogic);

		panel = panel->next;
	}

	return true;

}

bool GuiManager::PostUpdate() {

	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		if (panel->data->Active)
			panel->data->Draw();

		panel = panel->next;
	}

	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		control->data->Update(0.16f);
		control = control->next;
	}

	
	spotLight->Render();
    

	// Animation not finished? 
	if (!easing->GetFinished())
	{
		int a, b;
		EasingType easingT;

		if (bPause) {
			a = -230; b = -20;
			easingT = EasingType::EASE_IN_BACK;
		}
		else {
			a = -20;
			b = -230;
			easingT = EasingType::EASE_IN_BACK;
		}
		

		// TODO 1: Implement easings on pause menu
		// Calculate interpolated position (tracktime, easinganimaion),
		// and print to screen (DrawRectangle)
		double t = easing->TrackTime(22);
		double easedX = easing->EasingAnimation(a, b, t, easingT);

		SDL_Rect pauseBox = { easedX, 0, 300, 400 };
		app->render->DrawTexture(Curtain, easedX + (app->render->camera.x /-3), 0 + (app->render->camera.y / -3), &leftCurtain);
		
	}
	else if (bPause)
	{
		// Draw black background
		SDL_Rect screenRect = SDL_Rect{ 0 + (app->render->camera.x / -3),0 + (app->render->camera.y / -3),256 * 2, 198 * 2};
		app->render->DrawRectangle(screenRect, b2Color(0, 0, 0, 1), true, true);
		// static pause menu (animation finished, menu open)
		SDL_Rect pauseBox = { -20 , 0 , 300, 400 };

		app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &leftCurtain);

		if (!curtainclosed) {
			curtainclosed = true;
			curtainTimer.Start();
		}
		
		if (curtainTimer.ReadMSec() > 100 && curtainclosed) {
			curtainclosed = false;

			bPause = !bPause;
			easing->SetFinished(false);
			easingR->SetFinished(false);

		}

		/*app->render->DrawRectangle(pauseBox, 0, 255, 0, 255, false);
		app->render->DrawRectangle(pauseBox, 0, 255, 0, 64, true);*/
	}
	else {
		SDL_Rect pauseBox = { -230 , 0 , 300, 400 };

		app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &leftCurtain);


	}

	          






	// Animation not finished? 
	if (!easingR->GetFinished())
	{
		int a, b;
		EasingType easingT;

		if (bPause) {
			a = 460; b = 250;
			easingT = EasingType::EASE_IN_BACK;
		}
		else {
			a = 250;
			b = 460;
			easingT = EasingType::EASE_IN_BACK;
		}


		// TODO 1: Implement easings on pause menu
		// Calculate interpolated position (tracktime, easinganimaion),
		// and print to screen (DrawRectangle)
		double t = easingR->TrackTime(22);
		double easedX = easingR->EasingAnimation(a, b, t, easingT);

		SDL_Rect pauseBox = { easedX, 0, 300, 400 };
		app->render->DrawTexture(Curtain, easedX + (app->render->camera.x / -3), 0 + (app->render->camera.y / -3), &rightCurtain);

	}
	else if (bPause)  
	{
		// static pause menu (animation finished, menu open)
		SDL_Rect pauseBox = { 250 , 0 , 300, 400 };

		app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &rightCurtain);

		/*app->render->DrawRectangle(pauseBox, 0, 255, 0, 255, false);
		app->render->DrawRectangle(pauseBox, 0, 255, 0, 64, true);*/
	}
	else {
		SDL_Rect pauseBox = { 460 , 0 , 300, 400 };

		app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &rightCurtain);

	}
	
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		OpenCloseCurtains();
	}

	return true;
}

void GuiManager::TurnOffSpotLight() {

	if(spotLight->visible != false) spotLight->visible = false;
	// play fx

}
void GuiManager::TurnOnSpotLight() {
	if(spotLight->visible != true)spotLight->visible = true;
	
	// play fx

}

// L15: DONE1: Implement CreateGuiControl function that instantiates a new GUI control and add it to the list of controls
GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds, VIEW viewMode, SDL_Color color, SDL_Color textColor)
{
	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text, viewMode, color, textColor);
		break;
	/*case GuiControlType::SLIDER:
		guiControl = new GuiSlider(id, bounds, text);
		break;*/
	}

	//Set the observer
	guiControl->observer = observer;

	// Created GuiControls are add it to the list of controls
	guiControlsList.Add(guiControl);

	return guiControl;
}

GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Entity* observer, SDL_Rect sliderBounds, VIEW viewMode, SDL_Color color, SDL_Color textColor) {

	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text, viewMode, color, textColor);
		break;
		/*case GuiControlType::SLIDER:
			guiControl = new GuiSlider(id, bounds, text);
			break;*/
	}

	//Set the observer
	guiControl->entityObserver = observer;

	// Created GuiControls are add it to the list of controls
	guiControlsList.Add(guiControl);

	return guiControl;

}

bool GuiManager::CleanUp()
{
	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		RELEASE(control);
	}

	//clen panels
	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		panel->data->CleanUp();
		panel = panel->next;
	}

	panels.clear();

	return true;

	return false;
}


//CUANDO ESTEMOS EN EN OVERWORLD O EN EL BATTLE SCREEN
//void GuiManager::OnPause(bool paused)
//{
//	if (app->gamePaused)
//	{
//		Pause_Panel->Enable();
//		app->entities->Pause = true;
//		app->physics->Pause = true;
//	}
//	else
//	{
//		pn_pause->Disable();
//		app->entities->Pause = false;
//		app->physics->Pause = false;
//	}
//}

void GuiManager::OpenPanel(PanelID panel_id)
{
	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{

		if (panel->data->Active == true)
		{
			panel->data->Disable();
			lastPanel = panel->data->id;
		}

		if (panel->data->id == panel_id)
		{
			panel->data->Enable();
			currentPanel = panel->data->id;
		}
		panel = panel->next;
	}
}

bool GuiManager::OnGuiMouseClickEvent(GuiControl* control)
{

	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		if (control->parent == panel->data && panel->data->Active)
		{
			panel->data->OnGuiMouseClickEvent(control);
		}
		panel = panel->next;
	}

	return true;
}

void GuiManager::OpenCloseCurtains() {
	
		bPause = !bPause;
		easing->SetFinished(false);
		easingR->SetFinished(false);
	
}