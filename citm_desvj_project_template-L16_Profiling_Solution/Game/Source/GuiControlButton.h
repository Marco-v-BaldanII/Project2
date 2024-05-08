#ifndef __GUICONTROLBUTTON_H__
#define __GUICONTROLBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

enum VIEW;

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(uint32 id, SDL_Rect bounds, const char* text, VIEW viewMode, SDL_Color color, SDL_Color textColor);
	
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);

	bool PreUpdate();

	SDL_Color color;
	SDL_Color textColor;

private:

	int mouseX, mouseY;
	unsigned int click;

	VIEW viewMode;

	bool canClick = true;
	bool drawBasic = false;
};

#endif // __GUICONTROLBUTTON_H__