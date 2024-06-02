#include "../App.h"
#include "../Input.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Render.h"
#include "../Window.h"
#include "../LevelManagement.h"
#include "../BattleScene.h"
#include "Credits.h"
#include "../Defs.h"
#include "../Log.h"
#include "ParticleSystem.h"
#include "../Victory.h"

#define SCROLLING_SPEED 0.1f

Credits::Credits(bool isActive) : Module(isActive)
{
	name.Create("credits");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
Credits::~Credits()
{}

// Called before render is available
bool Credits::Awake(pugi::xml_node config)
{
	LOG("Loading Intro Scene");
	bool ret = true;
	config = app->win->myNode.next_sibling("lose");
	position = iPoint(0, 0);
	myNode = config;

	return ret;
}

// Called before the first frame
bool Credits::Start()
{
	if (active)
	{
		app->itemManager->CleanUp();

		//app->dialogueManager->PrevAct();
		app->dialogueManager->myState = NPCS;
		position = iPoint(0, 0);
		thanked = false;
		img = app->tex->Load("Assets/Textures/WarOfRosesCreditsDark.png");
		Animation anim;	Animation anim2; 	Animation anim3;
		anim.PushBack(SDL_Rect{ 0,0,32,32 }); anim.PushBack(SDL_Rect{ 32,0,32,32 }); anim.PushBack(SDL_Rect{ 64,0,32,32 });
		anim.PushBack(SDL_Rect{ 96,0,32,32 }); anim.PushBack(SDL_Rect{ 128,0,32,32 }); anim.speed = 0.4f; anim.loop = true;

		anim2.PushBack(SDL_Rect{ 0,32,32,32 }); anim2.PushBack(SDL_Rect{ 32,32,32,32 }); anim2.PushBack(SDL_Rect{ 64,32,32,32 });
		anim2.PushBack(SDL_Rect{ 96,32,32,32 }); anim2.PushBack(SDL_Rect{ 128,32,32,32 }); anim2.speed = 0.4f; anim2.loop = true;

		anim3.PushBack(SDL_Rect{ 0,64,32,32 }); anim3.PushBack(SDL_Rect{ 32,64,32,32 }); anim3.PushBack(SDL_Rect{ 64,64,32,32 });
		anim3.PushBack(SDL_Rect{ 96,64,32,32 }); anim3.PushBack(SDL_Rect{ 128,64,32,32 }); anim3.speed = 0.4f; anim3.loop = true;

		Animation animations[3] = { anim, anim2, anim3 };

		SDL_Texture* tex1 = app->tex->Load("Assets/roseP.png");
		SDL_Texture* tex2 = app->tex->Load("Assets/rosePw.png");

		Particle* roseParticle = new Particle(tex1, fPoint(20, 400), fPoint(0.5f, -5), fPoint(0, 0.03f), 4.0f, SDL_Rect{ 0,0,32,32 }, 1, 1, 255, 255, fPoint(-2, 8), animations, tex2);

		rosesR = new ParticleEffect(roseParticle, 100, 0.02f, true, false, SDL_Rect{ 0,0,10,0 }, true);


		Particle* roseParticle2 = new Particle(tex1, fPoint(492, 400), fPoint(-0.5f, -5), fPoint(0, 0.03f), 4.0f, SDL_Rect{ 0,0,32,32 }, 1, 1, 255, 255, fPoint(-8, 2), animations, tex2);

		rosesL = new ParticleEffect(roseParticle2, 100, 0.02f, true, true, SDL_Rect{ 0,0,10,0 }, true);


		app->particleSystem->AddParticleEffect(rosesR);
		app->particleSystem->AddParticleEffect(rosesL);

		app->audio->PlayMusic("Assets/Audio/Music/WaltzOfFlowers.wav");
		thankYouFx = app->audio->LoadFx("Assets/Audio/lines/ThankyouVoice.wav");
		app->guiManager->credits = true;
		app->guiManager->shouldCurtainsReOpen = false;
	}

	return true;
}

// Called each loop iteration
bool Credits::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Credits::Update(float dt)
{
	position.y -= SCROLLING_SPEED * dt;

	if (position.y < -2560 && !thanked) {
		app->audio->PlayFx(thankYouFx);
		thanked = true;
	}

	if (position.y < -2780 && ! victory ) {
		
		victory = true;

		app->victory->Enable();
		app->victory->Start();
	}

	
	return true;
}

void Credits::ReStart() {
	

}

// Called each loop iteration
bool Credits::PostUpdate()
{

	app->render->DrawTexture(img, position.x + (app->render->camera.x / -3), position.y + (app->render->camera.y / -3));

	return true;
}

// Called before quitting
bool Credits::CleanUp()
{
	if (rosesL != nullptr) rosesL->CleanUp();
	if (rosesR != nullptr) rosesR->CleanUp();
	victory = false;
	thanked = false;

	LOG("Freeing scene lose");
	app->guiManager->credits = false;
	active = false;
	img = nullptr;
	return true;
}