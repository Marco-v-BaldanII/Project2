#ifndef __STARTMENU_H__
#define __STARTMENU_H__

#include "Module.h"
//#include "p2List.h"
#include "Defs.h"
#include "Physics.h"
#include "Animation.h"
#include "Audio.h"


#include "GuiSlider.h"
#include "GuiPanel.h"
#include "GuiToggle.h"
#include "GuiControl.h"
#include "GuiControlButton.h"

struct SDL_Texture;

class StartMenu : public Module
{
public:

	StartMenu(bool isActive);

	// Destructor
	virtual ~StartMenu();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	SDL_Texture* img;
	SDL_Rect rect;
	uint music;
	bool playMusic = true;
	bool goConfig = false;

	//Menu Buttons
	GuiControlButton* start;
	GuiControlButton* load;
	GuiControlButton* quit;
	GuiControlButton* options;

	//Config Buttons
	GuiControlButton* back;
	GuiControlButton* FullScreen;
	GuiControlButton* VSync;
	GuiControlButton* Music;
	GuiControlButton* FX;

	uint windowW, windowH;

};
#endif // __STARTMENU_H__

