#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Intro.h"
#include "LevelManagement.h"

#include "Defs.h"
#include "Log.h"

Intro::Intro(bool isActive) : Module(isActive)
{
	name.Create("intro");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
Intro::~Intro()
{}

// Called before render is available
bool Intro::Awake(pugi::xml_node config)
{
	LOG("Loading Intro Scene");
	bool ret = true;
	config = app->win->myNode.next_sibling("intro");

	myNode = config;

	return ret;
}

// Called before the first frame
bool Intro::Start()
{

 	intro = app->audio->LoadFx("Assets/audio/fx/intro.wav");
	app->audio->PlayFx(intro);
	waitTime = 300;
	EasingwaitTime = 40;
	music = app->audio->PlayMusic("assets/audio/music/Logo Screen.wav", 0.5f);
	part1 = app->tex->Load("Assets/Textures/loog/part1.png");
	part2 = app->tex->Load("Assets/Textures/loog/part2.png");
	rect = { 0, 0, 66 * 2, 120 * 2 };
	//rect2 = { 0, 0, 512 / 2 - 50, 384 - 50 };
	initLogo1PosX = -400;
	initLogo2PosX = 512 + 400;
	Logo1PosX = 0;
	Logo2PosX = 512 / 2;
	return true;
}

// Called each loop iteration
bool Intro::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Intro::Update(float dt)
{

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		waitTime = 0;
	}
	else
	{
		waitTime -= 1;
		EasingwaitTime -= 1;
	}

	float progress = 1.0f - (float)EasingwaitTime / 100;

	if (initLogo1PosX < 126)
	{
		// Animate initLogo1PosX from -400 to 126 using EaseInElastic
		initLogo1PosX = EaseInElastic(progress, -400, 126 + 400, 1);
	}
	else
	{
		initLogo1PosX = 126;
	}

	if (initLogo2PosX > 512 / 2)
	{
		// Animate initLogo2PosX from 512 + 400 to 512 / 2 using EaseInElastic
		initLogo2PosX = EaseInElastic(progress, 512 + 400, 512 / 2 - (512 + 400), 1);
	}
	else
	{
		initLogo2PosX = 512 / 2;
	}

	if (waitTime < 0)
		app->levelManager->LoadScene(GameScene::START);
	
	return true;
}

// Called each loop iteration
bool Intro::PostUpdate()
{
	app->render->DrawRectangle(SDL_Rect{0,0,500,500}, {0,0,0,255}, true, false);
	app->render->DrawTexture(part2, (int)initLogo1PosX, 70, &rect); 
	app->render->DrawTexture(part1, (int)initLogo2PosX, 70, &rect); 

	return true;
}

// Called before quitting
bool Intro::CleanUp()
{
	LOG("Freeing scene intro");
	active = false;
	return true;
}

float Intro::EaseInElastic(float t, float b, float c, float d)
{
	float s = 1.70158f;
	float p = 0;
	float a = c;

	if (t == 0)
		return b;

	t /= d;

	if (t == 1)
		return b + c;

	if (!p)
		p = d * 0.3f;

	if (a < abs(c))
	{
		a = c;
		s = p / 4;
	}
	else
	{
		s = p / (2 * M_PI) * asin(c / a);
	}

	t -= 1;
	return -(a * pow(2, 10 * t) * sin((t * d - s) * (2 * M_PI) / p)) + b;
}

float Intro::EaseInQuad(float t, float b, float c, float d)
{
	t /= d;
	return c * t * t + b;
}