#include "App.h"
#include "Window.h"
#include "Render.h"
#include "Box2D/Box2D/Box2D.h"
#include <string>
#include "Defs.h"
#include "Log.h"
#include "Timer.h"
#include "../DialogueManager.h"

#define VSYNC true

#define LINE_LENGTH 60

Render::Render(bool isActive) : Module(isActive)
{
	name.Create("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}



int Clamp(int value, int min, int max) {
	return std::max(min, std::min(value, max));
}

// Destructor
Render::~Render()
{}

// Called before render is available
bool Render::Awake(pugi::xml_node config)
{
	LOG("Create SDL rendering context");
	bool ret = true;

	Uint32 flags = SDL_RENDERER_ACCELERATED;

	// L04: DONE 6: Load the VSYNC status from config.xml and adapt the code to set it on / off
	if (config.child("vsync").attribute("value").as_bool()) {
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
	}
	else {
		LOG("vsync OFF");
	}
	 
	renderer = SDL_CreateRenderer(app->win->window, -1, flags);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = app->win->screenSurface->w;
		camera.h = app->win->screenSurface->h;
		camera.x = 0;
		camera.y = 0;
	}

	//initialise the SDL_ttf library
	TTF_Init();

	//load a font into memory
	font = TTF_OpenFont("Assets/Fonts/PixelFJVerdana12pt.ttf", 25);                       

	return ret;
}

// Called before the first frame
bool Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool Render::Update(float dt)
{
	return true;
}

bool Render::PostUpdate()
{
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

// Blit to screen
bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, bool flipped, int opacity, float speed, int R, int G, int B, double angle, int pivotX, int pivotY, SDL_BlendMode blendMode) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	SDL_SetTextureColorMod(texture, R, G, B);
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_SetTextureBlendMode(texture, blendMode); 


	if (section != NULL)
	{
		rect.w = section->w;
		rect.h = section->h;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivotX != INT_MAX && pivotY != INT_MAX)
	{
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}
	if (flipped == false) {
   		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}
	}
	else {
		if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_HORIZONTAL) != 0)
		{
			LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
			ret = false;
		}

	}

	return ret;
}

bool Render::DrawActor(ActorSprite* sprite, int x, int y, const SDL_Rect* section, bool flipped, int opacity, float speed, int R, int G, int B, double angle, int pivotX, int pivotY) const
{
	if (sprite != nullptr) {
		app->render->DrawTexture(sprite->actor->texture, x, y, section, flipped, opacity, speed, R, G, B, angle, pivotX, pivotY);

		app->render->DrawTexture(sprite->role->texture, x, y, section, flipped, opacity, speed, R, G, B, angle, pivotX, pivotY);
	}
	return true;
}

bool Render::DrawRectangle(const SDL_Rect& rect, int r, int g, int b, Uint8 a, bool filled, bool use_camera) const
{


	if (r < 0) { r = 0; }
	if (b < 0) { b = 0; }
	if (g < 0) { g = 0; }

	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section, int actualWidth, int opacity, float speed, int R, int G, int B, double angle, int pivotX, int pivotY, SDL_BlendMode blendMode) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_Rect rect;
	rect.x = (int)(camera.x * speed) + x * scale;
	rect.y = (int)(camera.y * speed) + y * scale;

	SDL_SetTextureColorMod(texture, R, G, B);
	SDL_SetTextureAlphaMod(texture, opacity);
	SDL_SetTextureBlendMode(texture, blendMode);

	int textureWidth, textureHeight;
	if (section != nullptr) {
		textureWidth = section->w;
		textureHeight = section->h;
	}
	else {
		SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, &textureHeight);
	}

	

	// Set the rectangle's dimensions
	rect.w = (int)(textureWidth * 1 * scale);
	rect.h = textureHeight * scale;

	// Set the pivot point
	SDL_Point* p = nullptr;
	SDL_Point pivot;
	if (pivotX != INT_MAX && pivotY != INT_MAX) {
		pivot.x = pivotX;
		pivot.y = pivotY;
		p = &pivot;
	}

	// Render the texture
	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, SDL_FLIP_NONE) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawRectMask(const SDL_Rect& rect, Circle mask, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool useCamera) const {

	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if (useCamera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	// Apply color modulation to mask out the specified area

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); // Set to transparent black (0 alpha) to mask out the area
		//SDL_Rect maskedArea = *maskRect;
		//maskedArea.x += rec.x; // Adjust masked area to match screen coordinates
		//maskedArea.y += rec.y;
		//SDL_RenderFillRect(renderer, &maskedArea);
		FillCircle(mask.position.x, mask.position.y, mask.radius, 0, 0, 0, 0, true);
		SDL_SetRenderDrawColor(renderer, r, g, b, a); // Reset color modulation
	

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;

}

bool Render::DrawRectangle(const SDL_Rect& rect, b2Color col , bool filled, bool use_camera, SDL_BlendMode blendMode) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, blendMode);
	SDL_SetRenderDrawColor(renderer, col.r*255, col.g*255, col.b*255, col.a*255);

	SDL_Rect rec(rect);
	if (use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if (result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		points[i].x = (int)(x * scale + camera.x) + (radius * cos(i * factor));
		points[i].y = (int)(y * scale + camera.y) + (radius * sin(i * factor));
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool Render::FillCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera, SDL_BlendMode blendMode) const
{
	bool ret = true;
	uint scale = app->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, blendMode);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	// Calculate circle position
	int posX = x * scale;
	int posY = y * scale;
	if (use_camera)
	{
		posX += camera.x;
		posY += camera.y;
	}

	// Draw filled circle
	for (int dy = -radius; dy <= radius; dy++)
	{
		for (int dx = -radius; dx <= radius; dx++)
		{
			if (dx * dx + dy * dy <= radius * radius)
			{
				result = SDL_RenderDrawPoint(renderer, posX + dx, posY + dy);
				if (result != 0)
				{
					LOG("Cannot draw point to screen. SDL_RenderDrawPoint error: %s", SDL_GetError());
					ret = false;
				}
			}
		}
	}

	return ret;
}


bool Render::DrawText(const std::string& text, int posx, int posy, int w, int h, bool isDialogue, SDL_Color color, TextAlingment aligment, int maxLineLength ) {
	
	SDL_Surface* surface;
	SDL_Texture* texture;

	if(isDialogue) app->dialogueManager->numLines++;

	int numletters = text.length();

	float numLines = static_cast<float>(numletters) / maxLineLength; // 70 characters per line
	if (numLines > 1.0f) {
		// Copy rest to a new string
		std::string newLine = text.substr(maxLineLength);
		DrawText(newLine, posx, posy + h*1.6f, w, h,isDialogue, color, aligment, maxLineLength);

		// Create the actual string for this line with a max of LINE_LENGTH chars
		std::string thisLine = text.substr(0, maxLineLength);

		
		surface = TTF_RenderText_Solid(font, thisLine.c_str(), color);
		texture = SDL_CreateTextureFromSurface(renderer, surface);
	}
	else {
		
		surface = TTF_RenderText_Solid(font, text.c_str(), color);
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		
	}


	if(isDialogue) w = w / maxLineLength * Clamp(numletters, 0, maxLineLength);


	int texW = 0;
	int texH = 0;
	int check = SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect;
	switch (aligment) {

	case ( LEFT_ALIGN):

        dstrect = { posx, posy, w, h };
		break;
	case (CENTER_ALIGN):
		int x_left_corner = posx - texW / 2;
        dstrect = { x_left_corner, posy, w, h };
		break;

	}
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}

bool Render::DrawTextButton(const char* text, int posx, int posy, int w, int h, SDL_Color color) 
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { posx, posy, w, h };

	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);

	return true;
}


void Render::DrawVignette(int centerX, int centerY, int radius, Uint8 maxAlpha)
{
	// Calculate the alpha value based on distance from the center
	//for (int y = centerY - radius; y <= centerY + radius; ++y)
	//{
	//	for (int x = centerX - radius; x <= centerX + radius; ++x)
	//	{
	//		// Calculate distance from center (pythagoras)
	//		int distanceSq = (x - centerX) * (x - centerX) + (y - centerY) * (y - centerY);

	//		// Calculate alpha based on distance (the bigger the distance the more alpha)
	//		Uint8 alpha = (Uint8)(maxAlpha * (0 + ((double)(distanceSq) / (radius * radius))));

	//		// Clamp alpha to ensure it's within valid range
	//		alpha = Clamp(alpha, 0, maxAlpha);

	//		// Set render color with the calculated alpha
	//		SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);

	//		// Render a point at (x, y)
	//		SDL_RenderDrawPoint(renderer, x, y);
	//	}
	//}
}

void Render::CameraCenterOn(int targetX, int targetY) {

	//printf("%d X %d Y \n", &targetX, &targetY);
	//// center the  pos on the target
	int xOffset = 0;
	int yOffset = 0;
	int max = 5;


	camera.x = (-targetX * 3) + (96*0);
	camera.y = (-targetY * 3) + (96*0);

	//if (app->render->camera.x > 0) {
	//	app->render->camera.x = 0;

	//}
	//if (app->render->camera.y > 0) { app->render->camera.y = 0; }
	//if (app->render->camera.x < -43 * TILE_RESOLUTION * 3) { app->render->camera.x = -43 * TILE_RESOLUTION * 3; }
	//if (app->render->camera.y < -36 * TILE_RESOLUTION * 3) {
	//	app->render->camera.y = -36 * TILE_RESOLUTION * 3;
	//}

	/*while (camera.x + 100 > 0 && max > 0) {
		camera.x + 100;
		max--;
	}
	max = 5;*/
	/*while (camera.y + 100 < 0 && max > 0) {
		camera.y += 100;
		max--;
	}*/
}

// L14: TODO 6: Implement a method to load the state
// for now load camera's x and y
bool Render::LoadState(pugi::xml_node node) {

	camera.x = node.child("camera").attribute("x").as_int();
	camera.y = node.child("camera").attribute("y").as_int();

	return true;
}

// L14: TODO 8: Create a method to save the state of the renderer
// using append_child and append_attribute
bool Render::SaveState(pugi::xml_node node) {
	
	pugi::xml_node camNode = node.append_child("camera");
	camNode.append_attribute("x").set_value(camera.x);
	camNode.append_attribute("y").set_value(camera.y);

	return true;
}