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

enum class MenuState
{
	START,
	OPTIONS,
	PAUSE,
	SCENES,
	OFF
};

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
	bool cerrar = false;
	bool full = false;
	bool scenebuttons = false;
	bool offbuttons = false;
	bool pause = false;
	bool inEnglish = true;
	const char* laguageText;
	bool vsyncOn = false;
	bool musicOn = true;
	bool fxOn = true;
	int lastFxVolume = 0;
	int lastMusicVolume = 0;

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
	GuiControlButton* laguage;
	GuiControlButton* textSpeed;

	//Scenes
	GuiControlButton* backstage;
	GuiControlButton* combatstage;

	//Pause
	GuiControlButton* resume;

	uint windowW, windowH;

	MenuState state = MenuState::START;

};
#endif // __STARTMENU_H__

