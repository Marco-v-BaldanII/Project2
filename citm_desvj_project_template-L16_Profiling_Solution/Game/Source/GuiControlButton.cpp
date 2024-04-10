#include "GuiControlButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "GuiManager.h"

GuiControlButton::GuiControlButton(uint32 id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
}

GuiControlButton::~GuiControlButton()
{

}

bool GuiControlButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		SDL_GetMouseState(&mouseX, &mouseY);

		//If the position of the mouse if inside the bounds of the button 
		if (mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h) {
		
			state = GuiControlState::FOCUSED;
			//app->audio->PlayFx(app->guiManager->buttonHoverFx);

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
				state = GuiControlState::PRESSED;
			}
			
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP) {
				NotifyObserver();
				app->audio->PlayFx(app->guiManager->buttonClickFx);
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}

		//L15: DONE 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			app->render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			app->render->DrawRectangle(bounds, 0, 0, 200, 255, true, false);
			break;
		case GuiControlState::FOCUSED:
			app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
			//Draw rectangle at the left  and right side of the button
			app->render->DrawRectangle({ bounds.x - 15, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, false);
			app->render->DrawRectangle({ bounds.x + bounds.w + 10, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, false);
			break;
		case GuiControlState::PRESSED:
			app->render->DrawRectangle(bounds, 255, 0, 0, 255, true, false);
			break;
		}

		app->render->DrawTextButton(text, bounds.x, bounds.y, bounds.w, bounds.h, { 255,255,255,255});

	}

	return false;
}

