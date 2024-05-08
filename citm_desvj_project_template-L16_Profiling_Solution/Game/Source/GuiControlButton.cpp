#include "GuiControlButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "GuiManager.h"

GuiControlButton::GuiControlButton(uint32 id, SDL_Rect bounds, const char* text, VIEW viewMode): GuiControl(GuiControlType::BUTTON, id)
{
	this->bounds = bounds;
	this->text = text;
	this->viewMode = viewMode;

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
		if (viewMode == SCREEN) {

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
		}
		else if (viewMode == WORLD) {

			SDL_Rect bounds3 = bounds;
			bounds3.x = (int)(app->render->camera.x + bounds.x * 3);
			bounds3.y = (int)(app->render->camera.y + bounds.y * 3);
			bounds3.w *= 3;
			bounds3.h *= 3;

		/*	SDL_Rect bounds3 = bounds;
			 bounds3.w *= 3;
			bounds3.h *= 3;
			bounds3.x += */
			app->render->DrawRectangle(bounds3, b2Color(1, 0, 0, 1), true, false);

			if (mouseX > bounds3.x && mouseX < bounds3.x + bounds3.w && mouseY > bounds3.y && mouseY < bounds3.y + bounds3.h) {

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

		}

		//L15: DONE 4: Draw the button according the GuiControl State
		switch (state)
		{
		case GuiControlState::DISABLED:
			app->render->DrawRectangle(bounds, 200, 200, 200, 255, true, false);
			break;
		case GuiControlState::NORMAL:
			if(viewMode == VIEW::SCREEN) app->render->DrawRectangle(bounds, 0, 0, 200, 255, true, false);
			else { app->render->DrawRectangle(bounds, 0, 0, 200, 255, true, true); }
			//app->render->DrawTexture(app->guiManager->normalButton, bounds.x, bounds.y);
			break;
		case GuiControlState::FOCUSED:
			if (viewMode == VIEW::SCREEN) {
				app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, false);
				//Draw rectangle at the left  and right side of the button
				app->render->DrawRectangle({ bounds.x - 15, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, false);
				app->render->DrawRectangle({ bounds.x + bounds.w + 10, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, false);
			}
			else if (viewMode == VIEW::WORLD) {
				app->render->DrawRectangle(bounds, 0, 0, 20, 255, true, true);
				//Draw rectangle at the left  and right side of the button
				app->render->DrawRectangle({ bounds.x - 15, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, true);
				app->render->DrawRectangle({ bounds.x + bounds.w + 10, bounds.y, 5, bounds.h }, 0, 0, 0, 255, true, true);

			}
			//app->render->DrawTexture(app->guiManager->focusedButton, bounds.x, bounds.y);
			break;
		case GuiControlState::PRESSED:
			app->render->DrawRectangle(bounds, 255, 0, 0, 255, true, false);
			//app->render->DrawTexture(app->, bounds.x, bounds.y);
			break;
		}

		if (viewMode == VIEW::SCREEN)app->render->DrawTextButton(text, bounds.x, bounds.y, bounds.w, bounds.h, { 255,255,255,255});

		else {
		
			app->render->DrawText(text,  (app->render->camera.x + bounds.x * 3), (app->render->camera.y + bounds.y * 3), bounds.w*3, bounds.h*3, false);
		}

	}

	return false;
}

