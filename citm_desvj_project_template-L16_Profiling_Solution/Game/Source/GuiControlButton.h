#ifndef __GUICONTROLBUTTON_H__
#define __GUICONTROLBUTTON_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

enum VIEW;

class GuiControlButton : public GuiControl
{

public:

	GuiControlButton(uint32 id, SDL_Rect bounds, const char* text, VIEW viewMode);
	
	virtual ~GuiControlButton();

	// Called each loop iteration
	bool Update(float dt);

private:

	int mouseX, mouseY;
	unsigned int click;

	VIEW viewMode;

	bool canClick = true;
	bool drawBasic = false;
};

#endif // __GUICONTROLBUTTON_H__