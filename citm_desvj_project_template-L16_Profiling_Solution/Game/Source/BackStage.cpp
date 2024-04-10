#include "App.h"
#include "Map.h"
#include "Entity.h"
#include "Audio.h"
#include "Physics.h"
#include "GuiManager.h"
#include "BackStage.h"

BackStage::BackStage(bool isActive) : Module(isActive)
{
	name.Create("BackStage");
	//toSave = false;
	//saveConfigs = false;
}

BackStage::~BackStage()
{
}

bool BackStage::Awake(pugi::xml_node&)
{
	return true;
}

bool BackStage::Start()
{
	if (active)
	{
		music = app->audio->PlayMusic("assets/audio/music/Musica-overworld-_Big-Map_.wav", 0);
		background = app->tex->Load("Assets/Textures/Backstageprops1.png");
		createplayer = true;
		app->backstageplayer->Enable();
		app->backstageplayer->position.x = 100;
		app->backstageplayer->position.y = 100;
	}

	return true;
}

bool BackStage::PreUpdate()
{
	return true;
}

bool BackStage::Update(float dt)
{

	return true;
}

bool BackStage::PostUpdate()
{
	SDL_Rect r = { 0,0,720,384 };
	app->render->DrawTexture(background, 0, 0, &r);

	return true;
}

bool BackStage::CleanUp()
{
	app->map->CleanUp();
	app->entityManager->CleanUp();
	//app->physics->CleanUp();
	//app->audio->StopMusic();
	return true;
}

