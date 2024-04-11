#ifndef __FRAME_H__
#define __FRAME_H__


#include "../GuiControl.h"
#include "../GuiControlButton.h"
#include "../Timer.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "../GuiManager.h"
#include "../App.h"
#include "../Render.h"
#include <string>
#include "../Window.h"

using namespace std;

class App;
class Render;


enum Appearance {
	LEFTWARDS,
	RIGHTWARDS,
	DOWN,
	UP,
	FADE
};

/*float attack;
	float hp;
	float precision;
	float luck;
	float speed;
	float movement;                                                 
*/

class Frame {
public:
	// position where frame lands, time for interpolation, direction of interpolation, size of frame, time the frame is rendered, when does the frame start to get rendered
	Frame(iPoint pos, float t, Appearance appr, SDL_Rect size, SDL_Texture* texture, int attack, int hp, int precision, int luck, int speed, int movement , string name) {
		this->t = t;
		desiredPos = pos;
		myAppearance = appr;
		startingPos = desiredPos;
		

		size.x *= 2; size.y *= 2; size.w *= 2; size.h *= 2;
		this->size = size;

		this->texture = texture;

		// pass stats to strings
		this->attack = to_string(attack);
		this->hp = to_string(hp);
		this->precision = to_string(precision);
		this->luck = to_string(luck);
		this->movement = to_string(movement);
		this->speed = to_string(speed);
		this->name = name;


		// Frames are initialized to have 5 appearance methods, unless they are type FADE, they need to be assignes a starting position outside the camera area
		switch (myAppearance) {
		case LEFTWARDS:
			startingPos.x = -300;
			break;
		case RIGHTWARDS:
			startingPos.x = 700;
			break;
		case UP:
			startingPos.y = -300;
			break;
		case DOWN:
			startingPos.y = 600;
			break;

		}

	}


	SDL_Texture* texture;
	iPoint desiredPos;
	iPoint startingPos;
	Timer nextTimer;

	Timer startTimer;
	float startValue;

	bool started = false;
	bool finished = false;

	int alpha = 0;

	SDL_Rect size;

	App* myApp = nullptr;

	float t;
	float stayTime;

	Appearance myAppearance;

	string attack;
	string hp;
	string speed;
	string precision;
	string luck;
	string movement;
	string name;

	void lerp() {

		startingPos.x += (t) * (desiredPos.x - startingPos.x);
		startingPos.y += (t) * (desiredPos.x - startingPos.x);
	}


	void Render(float dt) {
		//  we use linear interpolation from its given starting position to its target position


				// if the frame has appeareance type FADE we interpolate its alpha
		if (myAppearance != Appearance::FADE) {
			startingPos.x += (t * dt) * (desiredPos.x - startingPos.x);
			startingPos.y += (t * dt) * (desiredPos.y - startingPos.y);
			app->render->DrawTexture(texture, startingPos.x, startingPos.y,  &size);

		}
		else {
			alpha += (t * dt) * (255 - alpha);
			app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size, alpha);
		}
		if (nextTimer.ReadMSec() > stayTime) {
			finished = true;
		}

		// Render all the stats:
		app->render->DrawText(attack, (startingPos.x  +95) *3, (startingPos.y +73) * 3, 30, 50, false, SDL_Color{254, 254, 0, 255});
		app->render->DrawText(hp, (startingPos.x + 150) * 3, (startingPos.y + 73) * 3, 30, 50, false, SDL_Color{ 254, 254, 0, 255 });

		app->render->DrawText(speed, (startingPos.x + 95) * 3, (startingPos.y + 100) * 3, 30, 50, false, SDL_Color{ 254, 254, 0, 255 });
		app->render->DrawText(precision, (startingPos.x + 150) * 3, (startingPos.y + 100) * 3, 30, 50, false, SDL_Color{ 254, 254, 0, 255 });

		app->render->DrawText(luck, (startingPos.x + 95) * 3, (startingPos.y + 127) * 3, 30, 50, false, SDL_Color{ 254, 254, 0, 255 });
		app->render->DrawText(movement, (startingPos.x + 150) * 3, (startingPos.y + 127) * 3, 30, 50, false, SDL_Color{ 254, 254, 0, 255 });

		// render name
		app->render->DrawText(name, (startingPos.x + 76) * 3, (startingPos.y + 35) * 3, 42*5, 12*5, false, SDL_Color{ 81, 51, 19, 255 });
	}

};

#endif // __FRAME_H__