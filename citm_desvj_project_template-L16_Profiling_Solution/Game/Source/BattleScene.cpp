#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "BattleScene.h"
#include "App.h"
#include "GuiManager.h"
#include "GuiControl.h"
#include "LevelManagement.h"
#include "../DialogueManager.h"
#include "../TurnManager.h"
//#include "Fonts.h"
#include "Defs.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

BattleScene::BattleScene(bool isActive) : Module(isActive)
{
	name.Create("battle_scene");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
BattleScene::~BattleScene()
{}

// Called before render is available
bool BattleScene::Awake(pugi::xml_node config)
{
	LOG("Loading Start Scene");
	mynode = config;

	bool ret = true;
	
	return ret;
}

// Called before the first frame
bool BattleScene::Start()
{
	lancasterUI = app->tex->Load(mynode.child("lancasterUI").attribute("path").as_string());
	yorkUI = app->tex->Load(mynode.child("yorkUI").attribute("path").as_string());

	if (!started)
	{
		spriteSheet = app->tex->Load(mynode.child("texture").attribute("path").as_string());

		time_t t;
		srand((unsigned)time(&t));
		SDL_Rect btPos = { 100, 500, 120,20 };
		SDL_Rect btPos2 = { 100, 700, 120,20 };

		// like in the dialogue manager implement reading the texture paths  into a TextureDef
		for (pugi::xml_node node = mynode.child("battleSprites").child("Sprite"); node != NULL; node = node.next_sibling("portrait")) {

			TextureDef* texD = new TextureDef;
			const char* path = node.attribute("path").as_string();
			texD->texture = app->tex->Load(path);
			texD->name = node.attribute("path").as_string();

			texD->name.erase(0, 36);
			texD->name.erase(texD->name.length() - 4, 4);


			// insert TextureDefinition to the map diccionary
			portraitTextures.insert(std::make_pair(texD->name, texD->texture));



		}


		// Read party members form config and instanciate them
		for (pugi::xml_node Pnode = mynode.child("battleMaps").child("map").child("player"); Pnode != NULL; Pnode = Pnode.next_sibling("player")) {

			Player* p = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
			p->config = Pnode;

			string name = p->config.attribute("name").as_string();
			// Here assign the correct texture

			p->Awake();
			party.Add(p);
			p->myTexture = spriteSheet;
			p->UiTex = lancasterUI;
		}

		// Read enemies from config and instantiate them
		for (pugi::xml_node Enode = mynode.child("battleMaps").child("map").child("enemy"); Enode != NULL; Enode = Enode.next_sibling("enemy")) {

			Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
			e->config = Enode;
			e->Awake();
			goons.Add(e);
			e->texture = spriteSheet;
			e->Uitex = yorkUI;
		}



		
		// pass the players to be monitoured by the turnManager
		app->turnManager->InitializeChessPieces(&party, &goons);

		//AttackButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Attack", btPos, this);
		//HealButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 2, "Heal", btPos2, this);
		//img = app->tex->Load("Assets/Textures/portrait1.png");
		music = app->audio->PlayMusic("assets/audio/music/Battle-screen-music.wav", 0.5f);
		//app->dialogueManager->Enable();

		rect = { 0,0,64,90 };
		started = true;
	}
	//app->guiManager->OpenPanel(PanelID::P_START_MENU);  //IMPORTANT
	return true;
}

// Called each loop iteration
bool BattleScene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool BattleScene::Update(float dt)
{
	
	//attackPattern = rand() % 2;
	//
	//app->render->DrawText("Player Hp:" + to_string(playerHp), 100, 100, 1000, 100, false);
	//
	//app->render->DrawText("Enemy Hp:" + to_string(enemyHp), 600, 100, 1000, 100, false);
	//
	//waitTimer++;
	//
	//if (playerTurn == false && attackPattern == 0 && waitTimer > 50) {
	//	playerHp -= 10;
	//	playerTurn = true;
	//} else if (playerTurn == false && attackPattern >= 1 && waitTimer > 50) {
	//	enemyHp += 10;
	//	playerTurn = true;
	//}

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y -= 10;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y += 10;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x -= 10;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x += 10;

	return true;
}

// Called each loop iteration
bool BattleScene::PostUpdate()
{
	bool ret = true;
	
	return ret;
}

// Called before quitting
bool BattleScene::CleanUp()
{
	LOG("Freeing scene main menu ");
	app->tex->UnLoad(spriteSheet);
	
	app->guiManager->Main_Menu_Panel->Disable();
	//app->audio->StopMusic();
	return true;
	
}

bool BattleScene::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 1 && playerTurn == true) {
		enemyHp -= 10;
		playerTurn = false;
		waitTimer = 0;
	}
	if (control->id == 2 && playerTurn == true) {
		if (playerHp <=100)
		playerHp += 10;
		playerTurn = false;
		waitTimer = 0;
	}

	return true;
}
