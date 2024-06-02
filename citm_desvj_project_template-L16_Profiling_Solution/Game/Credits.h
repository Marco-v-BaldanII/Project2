

#include "../Module.h"
#include "ParticleSystem.h"
#include "../Defs.h"
#include "../Physics.h"
#include "../Animation.h"
#include "../Victory.h"

struct SDL_Texture;
class EnemyDummy;
class ParticleEffect;

#define SCROLLING_SPEED 0.1f

class Credits : public Module
{
public:

	Credits(bool isActive) : Module(isActive)
	{
		name.Create("credits");
		//toSave = false;
		//saveConfigs = false;
	}

	// Destructor
	virtual ~Credits() {};

	// Called before render is available
	bool Awake(pugi::xml_node config) 
	{
		LOG("Loading Intro Scene");
		bool ret = true;
		config = app->win->myNode.next_sibling("lose");
		position = iPoint(0, 0);
		myNode = config;

		return ret;
	}

	// Called before the first frame
	bool Start() {
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

			rosesR = new ParticleEffect(roseParticle, 70, 0.025f, true, false, SDL_Rect{ 0,0,10,0 }, true);


			Particle* roseParticle2 = new Particle(tex1, fPoint(492, 400), fPoint(-0.5f, -5), fPoint(0, 0.03f), 4.0f, SDL_Rect{ 0,0,32,32 }, 1, 1, 255, 255, fPoint(-8, 2), animations, tex2);

			rosesL = new ParticleEffect(roseParticle2, 70, 0.025f, true, true, SDL_Rect{ 0,0,10,0 }, true);


			app->particleSystem->AddParticleEffect(rosesR);
			app->particleSystem->AddParticleEffect(rosesL);

			app->audio->PlayMusic("Assets/Audio/Music/WaltzOfFlowers.wav");
			thankYouFx = app->audio->LoadFx("Assets/Audio/lines/ThankyouVoice.wav");
			/*app->guiManager->credits = true;
			app->guiManager->shouldCurtainsReOpen = false;*/
		}

		return true;
	}

	// Called before all Updates
	bool PreUpdate() { return true; }

	// Called each loop iteration
	bool Update(float dt) {
		position.y -= SCROLLING_SPEED * dt;

		if (position.y < -2560 && !thanked) {
			app->audio->PlayFx(thankYouFx);
			thanked = true;
		}

		if (position.y < -2780 && !victory) {

			victory = true;

			app->victory->Enable();
			app->victory->Start();
		}


		return true;
	}

	// Called before all Updates
	bool PostUpdate() {

		app->render->DrawTexture(img, position.x + (app->render->camera.x / -3), position.y + (app->render->camera.y / -3));

		return true;
	}

	// Called before quitting
	bool CleanUp() {
		if (rosesL != nullptr) rosesL->CleanUp();
		if (rosesR != nullptr) rosesR->CleanUp();
		victory = false;
		thanked = false;

		LOG("Freeing scene lose");
		//app->guiManager->credits = false;
		active = false;
		img = nullptr;
		return true;
	}

	void ReStart() {};

	uint lose;
private:
	iPoint position = iPoint(0, 0);

	SDL_Texture* img;
	SDL_Rect rect;

	ParticleEffect* rosesL;
	ParticleEffect* rosesR;
	ParticleEffect* snow;

	pugi::xml_node myNode;

	bool playintro = true;

	int waitTime;

	int logoXpos;
	uint thankYouFx;
	bool thanked = false;

	bool victory = false;

	float easingTime = 500; //millisec
	float counter = 0;
};

