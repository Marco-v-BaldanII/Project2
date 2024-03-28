#include "GuiManager.h"
#include "App.h"
#include "Textures.h"
#include "MainMenuPanel.h"

#include "GuiControlButton.h"
#include "Audio.h"

GuiManager::GuiManager(bool isActive) :Module(isActive)
{
	name.Create("guiManager");
}

GuiManager::~GuiManager() {}

bool GuiManager::Start()
{
	Main_Menu_Panel = new MainMenuPanel(false);
	//Options_Panel = new OptionsPanel(false);
	//Pause_Panel = new PausePanel(false);
	//MORE PANELS TOO ADD

	panels.add(Main_Menu_Panel);
	//panels.add(Options_Panel);
	//panels.add(Pause_Panel);

	//LOAD TEXTURES
	UItexture = app->tex->Load("Assets/Textures/goldCoin.png");
	UItexture2 = app->tex->Load("Assets/Textures/UI2.png");
	tex_smallPanel = app->tex->Load("Assets/Textures/smallPanel.png");
	tex_mediumPanel = app->tex->Load("Assets/Textures/mediumPanel.png");
	tex_bigPanel = app->tex->Load("Assets/Textures/bigPanel.png");
	tex_inventory = app->tex->Load("Assets/Textures/inventory.png");
	tex_inventory_tabs = app->tex->Load("Assets/Textures/inventory_tabs.png");
	tex_inventory_Items = app->tex->Load("Assets/Textures/inventory_Items.png");

	p2ListItem<GuiPanel*>* panel = panels.start;

	while (panel != nullptr)
	{
		panel->data->Start();
		panel = panel->next;
	}

	return true;
}

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

	ListItem<GuiControl*>* control = guiControlsList.start;

	while (control != nullptr)
	{
		control->data->Update(dt);
		control = control->next;
	}

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

	return true;
}

// L15: DONE1: Implement CreateGuiControl function that instantiates a new GUI control and add it to the list of controls
GuiControl* GuiManager::CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds)
{
	GuiControl* guiControl = nullptr;

	//Call the constructor according to the GuiControlType
	switch (type)
	{
	case GuiControlType::BUTTON:
		guiControl = new GuiControlButton(id, bounds, text);
		break;
	}

	//Set the observer
	guiControl->observer = observer;

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