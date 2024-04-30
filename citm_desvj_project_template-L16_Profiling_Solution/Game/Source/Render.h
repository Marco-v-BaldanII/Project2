#ifndef __RENDER_H__
#define __RENDER_H__

#include "Module.h"
#include <string>
#include "Point.h"
#include "Box2D/Box2D/Box2D.h"

#include "SDL/include/SDL.h"
#include "SDL_ttf/include/SDL_ttf.h"

struct ActorSprite;

class Render : public Module
{
public:

	Render(bool isActive);

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();

	// Drawing
	bool DrawTexture(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL , bool flipped = false, int opacity = 255, float speed = 1.0f, int R = 255, int G = 255, int B = 255, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX) const;

	bool DrawActor(ActorSprite* sprite, int x, int y, const SDL_Rect* section = NULL, bool flipped = false, int opacity = 255, float speed = 1.0f, int R = 255, int G = 255, int B = 255, double angle = 0, int pivotX = INT_MAX, int pivotY = INT_MAX) const;

	bool DrawRectangle(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool useCamera = true) const;
	bool DrawRectangle(const SDL_Rect& rect, b2Color col, bool filled, bool use_camera) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool useCamera = true) const;
	bool FillCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const;

	bool DrawText(const std::string& text, int posX, int posY, int w, int h, bool isDialogue = false, SDL_Color col = SDL_Color{ 255,255,255,255 });
	bool DrawTextButton(const char* text, int posx, int posy, int w, int h, SDL_Color color = { 255,255,255,255 });

	void DrawVignette(int centerX, int centerY, int radius, Uint8 maxAlpha);

	// Set background color
	void SetBackgroundColor(SDL_Color color);

	//
	bool LoadState(pugi::xml_node node);

	//
	bool SaveState(pugi::xml_node node);

public:

	SDL_Renderer* renderer;
	SDL_Rect camera;
	SDL_Rect viewport;
	SDL_Color background;
	TTF_Font* font;
};

#endif // __RENDER_H__