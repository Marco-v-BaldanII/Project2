#include "GuiSlider.h"
#include "Log.h"
#include "App.h"
#include "GuiManager.h"
#include "Window.h"

GuiSlider::GuiSlider(uint32 id, SDL_Rect bounds, SDL_Rect Thumb) : GuiControl(GuiControlType::SLIDER, id)
{
	this->bounds = bounds;
	this->text = text;
	this->thumbBounds = Thumb;
	this->thumbBounds.x = bounds.x + bounds.w - 40;
	this->thumbBounds.y = bounds.y - Thumb.h / 5 + 11;
	texture = app->guiManager->UItexture;
	canClick = true;
	drawBasic = false;
	maxValue = 128;
	minValue = 0;
	minValueFrom = bounds.x;
	maxValueFrom = bounds.x + bounds.w;

	backgroundRect = {89,240,82,8};
	thumbRect = {172,240,6,10};

	name.Create("Slider");
	
}

GuiSlider::~GuiSlider()
{
}

bool GuiSlider::Update(float dt)
{

	if (state != GuiControlState::DISABLED)
	{
		// Update the state of the GUiButton according to the mouse position
		int mouseX, mouseY;
		float screenScale = 1 / (float)app->win->GetScale();

		SDL_GetMouseState(&mouseX, &mouseY);
		

		if ((mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h)|| 
			(mouseX > bounds.x && mouseX < bounds.x + bounds.w && mouseY > bounds.y && mouseY < bounds.y + bounds.h)) {
			state = GuiControlState::FOCUSED;

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT)
			{

				//10* is an offset to get the thumb in the right position
				thumbBounds.x = mouseX - (10);

				if (mouseX < bounds.x || thumbBounds.x < bounds.x)
					thumbBounds.x = bounds.x;

				if (mouseX > (bounds.x + bounds.w) || thumbBounds.x + 40 > (bounds.x + bounds.w))
					thumbBounds.x = (bounds.x + bounds.w) - 40;

				
				value = UpdateValue(mouseX);
				//LOG("slider value:%f", GetValue(mouseX));
				state = GuiControlState::PRESSED;
			}
			// If mouse button pressed -> Generate event!
			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP)
			{
				NotifyObserver();
			}

		}else state = GuiControlState::NORMAL;
	}

	Draw(app->render);

	return false;
}

bool GuiSlider::Draw(Render* render)
{
	//this text render could go to the state machine if necessary
	float screenScale = 1 / (float)app->win->GetScale();
	render->DrawTexture(textTex, textPosition.x * screenScale, textPosition.y * screenScale, &textRect, 0, 0, 0, 0, screenScale);

	SDL_Rect Normalbluergb = { 0, 0, 200, 155 };
	SDL_Rect Focusedbluergb = { 0, 0, 255, 155 };
	SDL_Rect Pressedbluergb = { 255, 0, 0, 155 };
	SDL_Rect Disabledbluergb = { 0, 0, 200, 0 };

	SDL_Rect NormalLightbluergb = { 0, 0, 100, 255 };
	SDL_Rect FocusedLightbluergb = { 0, 0, 155, 255 };
	SDL_Rect PressedLightbluergb = { 255, 0, 0, 255 };
	SDL_Rect DisabledLightbluergb = { 0, 0, 100, 0 };

 	switch (state)
	{

	case GuiControlState::DISABLED:
	{
		render->DrawRectangle(bounds, Disabledbluergb.x, Disabledbluergb.y, Disabledbluergb.w, Disabledbluergb.h);
		render->DrawRectangle(thumbBounds, DisabledLightbluergb.x, DisabledLightbluergb.y, DisabledLightbluergb.w, DisabledLightbluergb.h);
	} break;

	case GuiControlState::NORMAL:
	{
		render->DrawRectangle(bounds, Normalbluergb.x, Normalbluergb.y, Normalbluergb.w, Normalbluergb.h, true, false);
		render->DrawRectangle(thumbBounds, NormalLightbluergb.x, NormalLightbluergb.y, NormalLightbluergb.w, NormalLightbluergb.h, true, false);
	} break;
	case GuiControlState::FOCUSED:
	{
		render->DrawRectangle(bounds, Focusedbluergb.x, Focusedbluergb.y, Focusedbluergb.w, Focusedbluergb.h, true, false);
		render->DrawRectangle(thumbBounds, FocusedLightbluergb.x, FocusedLightbluergb.y, FocusedLightbluergb.w, FocusedLightbluergb.h, true, false);
	} break;
	case GuiControlState::PRESSED:
	{
		render->DrawRectangle(bounds, Pressedbluergb.x, Pressedbluergb.y, Pressedbluergb.w, Pressedbluergb.h, true, false);
		render->DrawRectangle(thumbBounds, PressedLightbluergb.x, PressedLightbluergb.y, PressedLightbluergb.w, PressedLightbluergb.h, true, false);
	} break;

	case GuiControlState::SELECTED:
	{	
		render->DrawRectangle(bounds, Focusedbluergb.x, Focusedbluergb.y, Focusedbluergb.w, Focusedbluergb.h, true, false);
		render->DrawRectangle(thumbBounds, FocusedLightbluergb.x, FocusedLightbluergb.y, FocusedLightbluergb.w, FocusedLightbluergb.h, true, false);
	}break;

	default:
		break;
	}
	return true;
}

bool GuiSlider::CleanUp()
{
	return true;
}

int GuiSlider::UpdateValue(float pos)
{

	if (pos <= bounds.x+ (thumbBounds.w * 0.5f))
		return value = minValue;

	if (pos >= (bounds.x + bounds.w) + (thumbBounds.w * 0.5f))
		return	value = maxValue;

	value = minValue + (maxValue - minValue) * ((pos - minValueFrom) / (maxValueFrom - minValueFrom));

	return value;
}

void GuiSlider::UpdateThumbPos(int _value)
{
	thumbBounds.x = bounds.x + ((bounds.x +bounds.w) - bounds.x) * ((_value - 0) / (128 - 0));
}