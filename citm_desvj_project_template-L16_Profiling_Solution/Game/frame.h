#include "../GuiControl.h"
#include "../GuiControlButton.h"
#include "../Timer.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "../GuiManager.h"


enum Appearance {
	LEFTWARDS,
	RIGHTWARDS,
	DOWN,
	UP,
	FADE
};

class Frame {
public:
	// position where frame lands, time for interpolation, direction of interpolation, size of frame, time the frame is rendered, when does the frame start to get rendered
	Frame(iPoint pos, float t, Appearance appr, SDL_Rect size, float StayTime, float whenToStart) {
		this->t = t;
		desiredPos = pos;
		myAppearance = appr;
		startingPos = desiredPos;
		this->size = size;
		stayTime = StayTime + whenToStart;
		startValue = whenToStart;

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


	float t;
	float stayTime;

	Appearance myAppearance;

	void lerp() {

		startingPos.x += (t) * (desiredPos.x - startingPos.x);
		startingPos.y += (t) * (desiredPos.x - startingPos.x);
	}


	void Update(float dt) {
		// when the frames designates time starts we use linear interpolation from its given starting position to its target position
		if (!finished) {
			if (!started)
			{
				startTimer.Start();
				nextTimer.Start();
				started = true;
			}
			if (startTimer.ReadMSec() > startValue) {
				// if the frame has appeareance type FADE we interpolate its alpha
				if (myAppearance != Appearance::FADE) {
					startingPos.x += (t * dt) * (desiredPos.x - startingPos.x);
					startingPos.y += (t * dt) * (desiredPos.y - startingPos.y);
					app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size);

				}
				else {
					alpha += (t * dt) * (255 - alpha);
					app->render->DrawTexture(texture, startingPos.x, startingPos.y, false, &size, alpha);
				}
				if (nextTimer.ReadMSec() > stayTime) {
					finished = true;
				}
			}
		}
	}

};