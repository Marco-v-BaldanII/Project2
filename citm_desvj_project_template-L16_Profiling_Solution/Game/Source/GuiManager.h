#ifndef __GUIMANAGER_H__
#define __GUIMANAGER_H__

#include "Module.h"
#include "GuiControl.h"
#include "p2List.h"
#include "GuiPanel.h"
#include "List.h"

class GuiManager : public Module
{
public:

	// Constructor
	GuiManager(bool isActive);

	// Destructor
	virtual ~GuiManager();

	// Called before the first frame
	bool Start();

	 // Called each loop iteration
	bool Update(float dt);
	bool UpdateAll(float dt, bool logic); 
	bool PostUpdate();
	// Called before quitting
	bool CleanUp();

	// Additional methods
	GuiControl* CreateGuiControl(GuiControlType type, int id, const char* text, SDL_Rect bounds, Module* observer, SDL_Rect sliderBounds = { 0,0,0,0 });

	void OnPause(bool paused);

	void OpenPanel(PanelID panel_id);

	bool OnGuiMouseClickEvent(GuiControl* control);

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

	int mainFont;
	int numberFont;
	bool Debug = false;

	List<GuiControl*> guiControlsList;
	SDL_Texture* texture;
	p2List<GuiPanel*> panels;

	float accumulatedTime = 0.0f;
	float updateMsCycle = 0.0f;
	bool doLogic = false;

	GuiPanel *Main_Menu_Panel = nullptr;
	GuiPanel *Settings_Panel = nullptr;
	GuiPanel *Pause_Panel = nullptr;

	p2List<PanelID> panelsID;
	PanelID lastPanel;
	int currentPanel;
};

#endif // __GUIMANAGER_H__
