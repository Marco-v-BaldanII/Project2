#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Lose.h"
#include "LevelManagement.h"

#include "Defs.h"
#include "Log.h"

Lose::Lose(bool isActive) : Module(isActive)
{
	name.Create("lose");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
Lose::~Lose()
{}

// Called before render is available
bool Lose::Awake(pugi::xml_node config)
{
	LOG("Loading Intro Scene");
	bool ret = true;
	config = app->win->myNode.next_sibling("lose");

	myNode = config;

	return ret;
}

// Called before the first frame
bool Lose::Start()
{
	if (active)
	{
		lose = app->audio->LoadFx("Assets/audio/fx/AbucheosFX.ogg");
		app->audio->PlayFx(lose);
		waitTime = 200;
		//music = app->audio->PlayMusic("assets/audio/music/Logo Screen.wav", 0.5f);
		img = app->tex->Load(myNode.child("titleScreen").attribute("path").as_string());
		rect = { 0, 0, 512, 384 };
	}
 	
	return true;
}

// Called each loop iteration
bool Lose::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Lose::Update(float dt)
{
	
	/*if (counter < easingTime)
	{

		auto easingFunction = getEasingFunction(EaseOutBounce);
		double progress = easingFunction(UpdateProgress(counter, 0, 100, 0, 1));
		logoXpos =(int) UpdateProgress(progress, 0, 1, -100, 0);
		counter++;
	}*/

	if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		waitTime = 0;
	}
	else
	{
		waitTime--;
	}

	if (waitTime < 0)
	{
		app->levelManager->LoadScene(GameScene::START);
	}
	
	return true;
}

// Called each loop iteration
bool Lose::PostUpdate()
{

	app->render->DrawTexture(img, 0 + (app->render->camera.x / -3), 0 + (app->render->camera.y / -3), &rect);

	return true;
}

// Called before quitting
bool Lose::CleanUp()
{
	LOG("Freeing scene lose");
	active = false;
	img = nullptr;
	return true;
}
