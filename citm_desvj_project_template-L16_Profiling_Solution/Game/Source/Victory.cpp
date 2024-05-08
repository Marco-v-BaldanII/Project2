#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Victory.h"
#include "LevelManagement.h"

#include "Defs.h"
#include "Log.h"

Victory::Victory(bool isActive) : Module(isActive)
{
	name.Create("Victory");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
Victory::~Victory()
{}

// Called before render is available
bool Victory::Awake(pugi::xml_node config)
{
	LOG("Loading Victory Scene");
	bool ret = true;
	config = app->win->myNode.next_sibling("win");

	myNode = config;

	return ret;
}

// Called before the first frame
bool Victory::Start()
{
	if (active)
	{
		victory = app->audio->LoadFx("Assets/audio/fx/ApplausosFX.ogg");
		app->audio->PlayFx(victory);
		waitTime = 200;
		//music = app->audio->PlayMusic("assets/audio/music/Logo Screen.wav", 0.5f);
		img = app->tex->Load(myNode.child("titleScreen").attribute("path").as_string());
		rect = { 0, 0, 512, 384 };
	}

	return true;
}

// Called each loop iteration
bool Victory::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Victory::Update(float dt)
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
bool Victory::PostUpdate()
{

	app->render->DrawTexture(img, 0, 0, &rect);

	return true;
}

// Called before quitting
bool Victory::CleanUp()
{
	LOG("Freeing scene Victory");
	active = false;
	img = nullptr;
	return true;
}