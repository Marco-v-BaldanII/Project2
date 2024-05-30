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
#include "../frame.h"
#include "Defs.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../random.h"
#include "../Victory.h"
#include <algorithm>
#include <iostream>

#include "../Inventory.h"


BattleScene::BattleScene(bool isActive) : Module(isActive)
{
	name.Create("battle_scene");
	//toSave = false;
	//saveConfigs = false;
}

// Destructor
BattleScene::~BattleScene()
{
	LOG("destructor");

}

// Called before render is available
bool BattleScene::Awake(pugi::xml_node config)
{

	LOG("Loading Start Scene");
	mynode = config;

	bool ret = true;

	Start();
	
	return ret;
}

// Called before the first frame
bool BattleScene::Start()
{
	if (active) {
		if (!started) {
			applauseFx = app->audio->LoadFx("Assets/audio/fx/ApplausosFX.ogg");

			MargaretVoice1 = app->audio->LoadFx("Assets/Audio/lines/MargaretBattle01.wav");
			MargaretVoice2 = app->audio->LoadFx("Assets/Audio/lines/MargaretBattle02.wav");
			HenryTudorVoice1 = app->audio->LoadFx("Assets/Audio/lines/HenryTudorBattle01.wav");
			HenryTudorVoice2 = app->audio->LoadFx("Assets/Audio/lines/HenryTudorBattle02.wav");
			PrinceEdwardVoice1 = app->audio->LoadFx("Assets/Audio/lines/PrinceEdwardBattle01.wav");
			PrinceEdwardVoice2 = app->audio->LoadFx("Assets/Audio/lines/PrinceEdwardBattle02.wav");
			HenryVIVoice1 = app->audio->LoadFx("Assets/Audio/lines/HenryVIBattle01.wav");
			HenryVIVoice2 = app->audio->LoadFx("Assets/Audio/lines/HenryVIBattle02.wav");
			JasperTudorVoice1 = app->audio->LoadFx("Assets/Audio/lines/JasperTudorBattle01.wav");
			JasperTudorVoice2 = app->audio->LoadFx("Assets/Audio/lines/JasperTudorBattle02.wav");

			app->guiManager->spotLight->mode = BAT;
			lancasterUI = app->tex->Load(mynode.child("lancasterUI").attribute("path").as_string());
			yorkUI = app->tex->Load(mynode.child("yorkUI").attribute("path").as_string());
			battleBackground = app->tex->Load(mynode.child("background").attribute("path").as_string());

			yorkHp = app->tex->Load(mynode.child("yorhHpBar").attribute("path").as_string());
			lancasterHp = app->tex->Load(mynode.child("lancasterHpBar").attribute("path").as_string());

			LoadAnimations();

			spriteSheet = app->tex->Load(mynode.child("texture").attribute("path").as_string());

			time_t t;
			srand((unsigned)time(&t));
			SDL_Rect btPos = { 100, 500, 120,20 };
			SDL_Rect btPos2 = { 100, 700, 120,20 };

			app->render->camera.y = -3200;

			// like in the dialogue manager implement reading the texture paths  into a TextureDef
			for (pugi::xml_node node = mynode.child("battleSprites").child("Sprite"); node != NULL; node = node.next_sibling("Sprite")) {

				TextureDef* texD = new TextureDef;
				const char* path = node.attribute("path").as_string();
				texD->texture = app->tex->Load(path);
				texD->name = node.attribute("path").as_string();

				texD->name.erase(0, 36);
				texD->name.erase(texD->name.length() - 4, 4);


				// insert TextureDefinition to the map diccionary
				portraitTextures.insert(std::make_pair(texD->name, texD->texture));

			}

			for (pugi::xml_node FxNode = mynode.child("soundeffect"); FxNode != NULL; FxNode = FxNode.next_sibling("soundeffect")) {

				string t = FxNode.attribute("name").as_string();
				if (t == "archerSound") { archerFx = app->audio->LoadFx(FxNode.attribute("path").as_string()); }
				if (t == "knigthsound") {
					knightFx = app->audio->LoadFx(FxNode.attribute("path").as_string());
				}
				if (t == "magesound") { mageFx = app->audio->LoadFx(FxNode.attribute("path").as_string()); }
				if (t == "lancersound") {
					LancerFx = app->audio->LoadFx(FxNode.attribute("path").as_string()); 
				}

			}

		}

		

		app->itemManager->Enable();
		List<Player*> l;
		party = l;
		List<Enemy*> le; goons = le;

		// The following entities are loaded depending on the current map

		pugi::xml_node mapNode;
		// Find the current map node
		if (app->map->level == 0) {
			for (mapNode = mynode.child("battleMaps").child("map"); mapNode != NULL; mapNode = mapNode.next_sibling("map")) {
				string s = mapNode.attribute("name").as_string();
				if (s == "Northampton") {
					break;
				}
			}
		}
		else if (app->map->level == 1) {
			for (mapNode = mynode.child("battleMaps").child("map"); mapNode != NULL; mapNode = mapNode.next_sibling("map")) {
				string s = mapNode.attribute("name").as_string();
				if (s == "Castle") {
					break;
				}
			}
		}

		for (pugi::xml_node questNode = mapNode.child("mainQuest"); questNode != NULL; questNode = questNode.next_sibling("mainQuest")) {

			Quest* quest = nullptr;
			int type = questNode.attribute("type").as_int();

			switch (type) {

			case 0:
				quest = new DefeatQuest(questNode.attribute("objective").as_string(), questNode.attribute("target").as_string());
				break;
			case 1:
				quest = new FetchQuest(questNode.attribute("objective").as_string(), questNode.attribute("target").as_string());
				break;
			}
			quest->isMain = true;

			app->questManager->quests.Add(quest);
			StartRossing();

		}


		// Read party members form config and instanciate them
		int nPlayers = 0;

		pugi::xml_document saveFile;
		pugi::xml_parse_result result = saveFile.load_file("save_game.xml");

		saveNode = saveFile.child("game_state").child("battle_scene");

		
		for (pugi::xml_node Pnode = mapNode.child("player"); Pnode != NULL; Pnode = Pnode.next_sibling("player")) {

			if (app->map->level == 0) {

				Player* p = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
				p->config = Pnode;

				string name = p->config.attribute("name").as_string();
				// Here assign the correct texture

				p->Awake();
				party.Add(p);
				p->myTexture = spriteSheet;
				p->myBattleTexture = portraitTextures[name];
				p->UiTex = lancasterUI;

				// make each unit have their own unique control ID
				p->atkBtnId = uniqueNumber.generateUniqueNumber(110, 300);
				p->waitBtnId = uniqueNumber.generateUniqueNumber(110, 300);
				p->itemBtnId = uniqueNumber.generateUniqueNumber(400, 800);

				p->Start();
				PassAnimations(p);
			}
			else {
				
 				if (saveNode) {
					bool spawned = false;
					for (pugi::xml_node Pnode2 = saveNode.child("player"); Pnode2 != NULL; Pnode2 = Pnode2.next_sibling("player")) {

						string n1 = Pnode.attribute("name").as_string(); string n2 = Pnode2.attribute("name").as_string();
						if (n1 == n2)/*Load that player's save data*/ {
							spawned = true;
							Player* p = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
							p->config = Pnode2;

							string name = p->config.attribute("name").as_string();
							// Here assign the correct texture

							p->Awake();
							party.Add(p);
							p->myTexture = spriteSheet;
							p->myBattleTexture = portraitTextures[name];
							p->UiTex = lancasterUI;

							// make each unit have their own unique control ID
							p->atkBtnId = uniqueNumber.generateUniqueNumber(110, 300);
							p->waitBtnId = uniqueNumber.generateUniqueNumber(110, 300);

							p->Start();
							p->LoadConversation(Pnode);

							p->hp = p->maxHp; /* Reset its health */
							PassAnimations(p);
							p->position =  app->map->MapToWorld(Pnode.attribute("x").as_int(), Pnode.attribute("y").as_int()); /*The position in the new map*/

						}
						
					}
					if (!spawned) {
					 /*Brand new unit which means they don't have save data*/ 
						Player* p = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
						p->config = Pnode;

						string name = p->config.attribute("name").as_string();
						// Here assign the correct texture

						p->Awake();
						party.Add(p);
						p->myTexture = spriteSheet;
						p->myBattleTexture = portraitTextures[name];
						p->UiTex = lancasterUI;

						// make each unit have their own unique control ID
						p->atkBtnId = uniqueNumber.generateUniqueNumber(110, 300);
						p->waitBtnId = uniqueNumber.generateUniqueNumber(110, 300);

						p->Start();
						PassAnimations(p);

					}
					
				}


			}
		}

		// Read enemies from config and instantiate them
		for (pugi::xml_node Enode = mapNode.child("enemy"); Enode != NULL; Enode = Enode.next_sibling("enemy")) {

			Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
			e->config = Enode;
			string name = e->config.attribute("name").as_string();

			e->Awake();
			goons.Add(e);
			e->texture = spriteSheet;
			e->myBattleTexture = portraitTextures[name];
			e->Uitex = yorkUI;
			e->Start();
			e->battleBg = battleBackground;
			PassAnimations(e);
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

		expTexture = app->tex->Load("Assets/Textures/UI/expBar-export.png");
		lvlUpTexture = app->tex->Load("Assets/Textures/UI/LevelUpText.png");

	}

	//6
	
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
	if (!firstMap) app->render->camera.y = -2000; firstMap = true;
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

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT && !app->inventory->isVisible)
		app->render->camera.y -= 10;

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT && !app->inventory->isVisible)
		app->render->camera.y += 10;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT && !app->inventory->isVisible)
		app->render->camera.x -= 10;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT && !app->inventory->isVisible)
		app->render->camera.x += 10;
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		godMode = !godMode;

	if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		for (ListItem<Enemy*>* p = app->turnManager->enemies.start; p != NULL; p = p->next) {

			if (p->data->name == "Duke of York") {
				p->data->hp = 0;
			}
			else if (p->data->name == "Richard III") {
				p->data->hp = 0;
			}
			
		}


	}

	return true;
}

// Called each loop iteration
bool BattleScene::PostUpdate()
{
	bool ret = true;

	if (talking && talker1 != nullptr && talker2 != nullptr) {

		ListItem<Conversation*>* cIt = talker1->conversations.start;
		while (cIt != NULL) {
			string s1 = cIt->data->name2; string s2 = talker2->name.GetString();

			if (s1 == s2) {
				break;
				// found the correct conversation
			}

			cIt = cIt->next;
		}

		if (cIt != NULL) app->dialogueManager->SpontaneousDialogue(cIt->data);

		talker1 = nullptr;
		talker2 = nullptr;
	}

	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		for (ListItem<Player*>* pIt = party.start; pIt != NULL; pIt = pIt->next) {
			pIt->data->hp = 0;
		}
	}
	
	if (app->input->GetKey(SDL_SCANCODE_N) == KEY_DOWN) {
		StartRossing();
	}

	return ret;
}

// Called before quitting00
bool BattleScene::CleanUp()
{
	LOG("Freeing Battlescene main menu ");
	//app->tex->UnLoad(spriteSheet);
	
	//app->guiManager->Main_Menu_Panel->Disable();

	Entity* pEntity = nullptr;
	//Destroy Players and enemies
	/*for (ListItem<Player*>* pIt = party.start; pIt != nullptr; pIt = pIt->next) {

		pEntity = pIt->data;
		app->entityManager->DestroyEntity(pEntity);
		LOG("Destroying players");

	}
	for (ListItem<Enemy*>* pIt = goons.start; pIt != nullptr; pIt = pIt->next) {

		pEntity = pIt->data;
		app->entityManager->DestroyEntity(pEntity);

	}*/
	if (snowSystem != nullptr) {
		app->particleSystem->RemoveParticleEffect(snowSystem);
		snowSystem = nullptr;
	}

	app->entityManager->enemies.Clear();
	party.Clear(); goons.Clear();

	app->turnManager->Disable();

	//app->audio->StopMusic();
	return true;
	
}

void BattleScene::StartSnowStorm() {

	// Initialize particle system
	if (snowSystem == nullptr && app->dialogueManager->myState != CUTSCENE) {
		const char* too = "Assets/snowP.png";
		
		snowParticle = new Particle(too, fPoint(0, 0), fPoint(0, 2), fPoint(0,0), 3.0f, SDL_Rect{ 0,0,8,8 }, 1.0f, 2.5f, 100, 255);

		snowSystem = new ParticleEffect(snowParticle, 40, 0.1f,true,false, SDL_Rect{ 0,0, 512 , 0 });

		app->particleSystem->AddParticleEffect(snowSystem);
	}
}

void BattleScene::StartRossing() {

	app->audio->PlayFx(applauseFx);

	if (roseSystemLeft == nullptr) {
		Animation anim;	Animation anim2; 	Animation anim3;
		anim.PushBack(SDL_Rect{ 0,0,32,32 }); anim.PushBack(SDL_Rect{ 32,0,32,32 }); anim.PushBack(SDL_Rect{ 64,0,32,32 });
		anim.PushBack(SDL_Rect{ 96,0,32,32 }); anim.PushBack(SDL_Rect{ 128,0,32,32 }); anim.speed = 0.4f; anim.loop = true;

		anim2.PushBack(SDL_Rect{ 0,32,32,32 }); anim2.PushBack(SDL_Rect{ 32,32,32,32 }); anim2.PushBack(SDL_Rect{ 64,32,32,32 });
		anim2.PushBack(SDL_Rect{ 96,32,32,32 }); anim2.PushBack(SDL_Rect{ 128,32,32,32 }); anim2.speed = 0.4f; anim2.loop = true;

		anim3.PushBack(SDL_Rect{ 0,64,32,32 }); anim3.PushBack(SDL_Rect{ 32,64,32,32 }); anim3.PushBack(SDL_Rect{ 64,64,32,32 });
		anim3.PushBack(SDL_Rect{ 96,64,32,32 }); anim3.PushBack(SDL_Rect{ 128,64,32,32 }); anim3.speed = 0.4f; anim3.loop = true;

		Animation animations[3] = { anim, anim2, anim3 };

		SDL_Texture* tex1 = app->tex->Load("Assets/roseP.png");

		Particle* roseParticle = new Particle(tex1, fPoint(20, 400), fPoint(1, -4), fPoint(0, 0.045f), 4.0f, SDL_Rect{ 0,0,32,32 }, 1, 1, 255, 255, fPoint(-2, 8), animations);

		roseSystemRight = new ParticleEffect(roseParticle, 30, 0.015f, false, false, SDL_Rect{ 0,0,10,0 }, true);


		Particle* roseParticle2 = new Particle(tex1, fPoint(492, 400), fPoint(-1, -4), fPoint(0, 0.045f), 4.0f, SDL_Rect{ 0,0,32,32 }, 1, 1, 255, 255, fPoint(-8, 2), animations);

		roseSystemLeft = new ParticleEffect(roseParticle2, 30, 0.015f, false, true, SDL_Rect{ 0,0,10,0 }, true);


		app->particleSystem->AddParticleEffect(roseSystemRight);
		app->particleSystem->AddParticleEffect(roseSystemLeft);
	}
	else {
		roseSystemRight->Restart();
		roseSystemLeft->Restart();
	}
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

bool BattleScene::SaveState(pugi::xml_node node) {

	
		node.remove_children();
	

	// Save data of current players and enemies

	for (ListItem<Player*>* pIt = party.start; pIt != nullptr; pIt = pIt->next) {
		//<player  name="PrinceEdward"  x="0" y="0" unit_type="1"  attack="8" hp="12" precision="75" luck="25" speed="20" movement="8"           />
		pugi::xml_node playerNode = node.append_child("player");
		playerNode.append_attribute("name").set_value(pIt->data->name.GetString());
		playerNode.append_attribute("x").set_value(pIt->data->position.x/32);
		playerNode.append_attribute("y").set_value(pIt->data->position.y/32);

		switch (pIt->data->unitType) {
		case PALADIN:
			playerNode.append_attribute("unit_type").set_value(0);
			break;
		case ARCHER:
			playerNode.append_attribute("unit_type").set_value(1);
			break;
		case KNIGHT:
			playerNode.append_attribute("unit_type").set_value(2);
			break;
		case ARMOURED_KNIGHT:
			playerNode.append_attribute("unit_type").set_value(3);
			break;
		case MAGE:
			playerNode.append_attribute("unit_type").set_value(4);
			break;
		case DARK_MAGE:
			playerNode.append_attribute("unit_type").set_value(5);
			break;
		}

		playerNode.append_attribute("attack").set_value(pIt->data->attack);
		playerNode.append_attribute("hp").set_value(pIt->data->hp);
		playerNode.append_attribute("precision").set_value(pIt->data->precision);
		playerNode.append_attribute("luck").set_value(pIt->data->luck);
		playerNode.append_attribute("speed").set_value(pIt->data->speed);
		playerNode.append_attribute("movement").set_value(pIt->data->movement);
		playerNode.append_attribute("level").set_value((int) pIt->data->level);
		playerNode.append_attribute("exp").set_value((int)pIt->data->experiencePoints);

		nodes.Add(&playerNode);
	}
	for (ListItem<Enemy*>* pIt = goons.start; pIt != nullptr; pIt = pIt->next) {
		//<player  name="PrinceEdward"  x="0" y="0" unit_type="1"  attack="8" hp="12" precision="75" luck="25" speed="20" movement="8"           />
		pugi::xml_node playerNode = node.append_child("enemy");
		playerNode.append_attribute("name").set_value(pIt->data->name.GetString());
		playerNode.append_attribute("x").set_value(pIt->data->position.x/32);
		playerNode.append_attribute("y").set_value(pIt->data->position.y/32);

		switch (pIt->data->unitType) {
		case PALADIN:
			playerNode.append_attribute("unit_type").set_value(0);
			break;
		case ARCHER:
			playerNode.append_attribute("unit_type").set_value(1);
			break;
		case KNIGHT:
			playerNode.append_attribute("unit_type").set_value(2);
			break;
		case ARMOURED_KNIGHT:
			playerNode.append_attribute("unit_type").set_value(3);
			break;
		case MAGE:
			playerNode.append_attribute("unit_type").set_value(4);
			break;
		case DARK_MAGE:
			playerNode.append_attribute("unit_type").set_value(5);
			break;
		}

		playerNode.append_attribute("attack").set_value(pIt->data->attack);
		playerNode.append_attribute("hp").set_value(pIt->data->hp);
		playerNode.append_attribute("precision").set_value(pIt->data->precision);
		playerNode.append_attribute("luck").set_value(pIt->data->luck);
		playerNode.append_attribute("speed").set_value(pIt->data->speed);
		playerNode.append_attribute("movement").set_value(pIt->data->movement);

		nodes.Add(&playerNode);
	}



	return true;
}

bool BattleScene::LoadState(pugi::xml_node node) {

	
	Entity* pEntity = nullptr;
	 //Destroy Players and enemies
	for (ListItem<Player*>* pIt = party.start; pIt != nullptr; pIt = pIt->next) {

		pEntity = pIt->data;
		app->entityManager->DestroyEntity(pEntity);
		LOG("Destroying players");

	}
	for (ListItem<Enemy*>* pIt = goons.start; pIt != nullptr; pIt = pIt->next) {

		pEntity = pIt->data;
		app->entityManager->DestroyEntity(pEntity);

	}
	app->entityManager->enemies.Clear();
	party.Clear(); goons.Clear();
	app->turnManager->availablePlayers = 3;
	/*for (ListItem<pugi::xml_node*>* nod = nodes.start; nod != nullptr; nod = nod->next) {

		nod->data->parent().remove_child(nod->data->name());
	}*/


	int created = 0;

	// Spawn players and enemies
	for (pugi::xml_node pNode = node.child("player"); pNode != NULL; pNode = pNode.next_sibling("player")) {

		Player* p = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		p->config = pNode;

		string name = p->config.attribute("name").as_string();
		// Here assign the correct texture

		p->Awake();
		p->UiTex = lancasterUI;
		p->Start();
		party.Add(p);
		p->myTexture = spriteSheet;
		p->myBattleTexture = portraitTextures[name];
		
		PassAnimations(p);

		created++;

	}
	for (pugi::xml_node pNode = node.child("enemy"); pNode != NULL; pNode = pNode.next_sibling("enemy")) {

		Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY);
		e->config = pNode;

		string name = e->config.attribute("name").as_string();
		// Here assign the correct texture
		
		e->config = pNode;
		e->Awake();
		e->Uitex = yorkUI;
		e->Start();
		goons.Add(e);
		e->texture = spriteSheet;

		e->battleBg = battleBackground;
		

		app->turnManager->noEnemyMoving = true;
		app->entityManager->enemies.Add(e);

		PassAnimations(e);

		created++;
	}
	app->turnManager->InitializeChessPieces(&party, &goons);
	int b = created;

	return true;
}

void BattleScene::KillUnit(bool isPlayer, Entity* entity) {

	if (isPlayer) {

		app->entityManager->players.remove(entity);

				List<Player*> party2;
				for (ListItem<Player*>* pItem = party.start; pItem != NULL; pItem = pItem->next) {

					if (pItem->data->hp > 0) {
						party2.Add(pItem->data);
					}

				}
				party.Clear();
				party = party2;
				app->turnManager->players.Clear();
				app->turnManager->players = party;

				/*party.Del(pEntity);
				app->turnManager->players.Del(pEntity);*/
				//app->entityManager->DestroyEntity(entity);
			
			
		

		//for (int i = 0; i < party.Count(); ++i) {
		//	if (party.At(i)->data == entity) {

		//		//app->turnManager->players.Del(party.At(i));
		//		party.Del(party.At(i));
		//		
		//		app->entityManager->DestroyEntity(entity);

		//		/*app->turnManager->noEnemyMoving = true;
		//		app->turnManager->noEnemyMoving = true;*/
		//	}


		//}
	}
	else {

		for (int i = 0; i < goons.Count(); ++i) {
			if ((Entity*)goons.At(i) == entity) {

				goons.Del(goons.At(i));
				app->turnManager->enemies.Del(goons.At(i));
				app->entityManager->DestroyEntity(entity);

				/*app->turnManager->noEnemyMoving = true;
				app->turnManager->noEnemyMoving = true;*/
			}


		}
	}




}

void BattleScene::LoadAnimations() {


	for (pugi::xml_node node = mynode.child("Mage").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				mageRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				mageRight.speed = 0.1f;
			}
			else if (s == "up") {
				mageUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				mageUp.speed = 0.1f;
			}
			else if (s == "down") {
				mageDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				mageDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Archer").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				archerRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerRight.speed = 0.1f;
			}
			else if (s == "up") {
				archerUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerUp.speed = 0.1f;
			}
			else if (s == "down") {
				archerDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Knight").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				knightRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightRight.speed = 0.1f;
			}
			else if (s == "up") {
				knightUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightUp.speed = 0.1f;
			}
			else if (s == "down") {
				knightDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("HenryTudor").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				henryTudorRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorRight.speed = 0.1f;
			}
			else if (s == "up") {
				henryTudorUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorUp.speed = 0.1f;
			}
			else if (s == "down") {
				henryTudorDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("DukeofYork").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				dukeYorkRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkRight.speed = 0.1f;
			}
			else if (s == "up") {
				dukeYorkUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkUp.speed = 0.1f;
			}
			else if (s == "down") {
				dukeYorkDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("PrinceEdward").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				princeEdRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdRight.speed = 0.1f;
			}
			else if (s == "up") {
				princeEdUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdUp.speed = 0.1f;
			}
			else if (s == "down") {
				princeEdDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Warwick").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				warwickRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				warwickRight.speed = 0.1f;
			}
			else if (s == "up") {
				warwickUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				warwickUp.speed = 0.1f;
			}
			else if (s == "down") {
				warwickDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				warwickDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("EdwardVI").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				edwardRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				edwardRight.speed = 0.1f;
			}
			else if (s == "up") {
				edwardUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				edwardUp.speed = 0.1f;
			}
			else if (s == "down") {
				edwardDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				edwardDown.speed = 0.1f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Margaret").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				margaretRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				margaretRight.speed = 0.1f;
			}
			else if (s == "up") {
				margaretUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				margaretUp.speed = 0.1f;
			}
			else if (s == "down") {
				margaretDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				margaretDown.speed = 0.1f;
			}
		}
	}
	for (pugi::xml_node node = mynode.child("Lancastrian").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				lancasterRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				lancasterRight.speed = 0.1f;
			}
			else if (s == "up") {
				lancasterUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				lancasterUp.speed = 0.1f;
			}
			else if (s == "down") {
				lancasterDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				lancasterDown.speed = 0.1f;
			}
		}
	}
	for (pugi::xml_node node = mynode.child("Richard3").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				Richard3Right.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				Richard3Right.speed = 0.1f;
			}
			else if (s == "up") {
				Richard3Up.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				Richard3Up.speed = 0.1f;
			}
			else if (s == "down") {
				Richard3Down.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				Richard3Down.speed = 0.1f;
			}
		}
	}
	for (pugi::xml_node node = mynode.child("Henry").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				henryVIRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIRight.speed = 0.1f;
			}
			else if (s == "up") {
				henryVIUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIUp.speed = 0.1f;
			}
			else if (s == "down") {
				henryVIDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIDown.speed = 0.1f;
			}
		}
	}
	for (pugi::xml_node node = mynode.child("Henry").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				henryVIRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIRight.speed = 0.1f;
			}
			else if (s == "up") {
				henryVIUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIUp.speed = 0.1f;
			}
			else if (s == "down") {
				henryVIDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryVIDown.speed = 0.1f;
			}
		}
	}
	for (pugi::xml_node node = mynode.child("Jasper").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				jasperRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				jasperRight.speed = 0.1f;
			}
			else if (s == "up") {
				jasperUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				jasperUp.speed = 0.1f;
			}
			else if (s == "down") {
				jasperDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				jasperDown.speed = 0.1f;
			}
		}
	}
	
}

void BattleScene::PassAnimations(Entity* entity) {

	if (entity->name == "Prince Edward") {
		entity->upAnim = princeEdUp;
		entity->downAnim = princeEdDown;
		entity->rightAnim = princeEdRight;
		entity->hitFx = archerFx; entity->battleVoiceLines.PushBack(PrinceEdwardVoice2);
		entity->battleVoiceLines.PushBack(PrinceEdwardVoice1);
	}
	else if (entity->name == "Duke of York") {
		entity->upAnim = dukeYorkUp;
		entity->downAnim = dukeYorkDown;
		entity->rightAnim = dukeYorkRight;
		entity->hitFx = LancerFx;
	}
	else if (entity->name == "Henry Tudor") {
		entity->upAnim = henryTudorUp;
		entity->downAnim = henryTudorDown;
		entity->rightAnim = henryTudorRight;
		entity->hitFx = LancerFx; 
		entity->battleVoiceLines.PushBack(HenryTudorVoice2);
	}
	else if (entity->name == "Earl of Warwick") {

		entity->upAnim = warwickUp;
		entity->rightAnim = warwickRight;
		entity->downAnim = warwickDown;
		entity->hitFx = knightFx;
	}
	else if (entity->name == "Edward of York") {

		entity->upAnim = edwardUp;
		entity->rightAnim = edwardRight;
		entity->downAnim = edwardDown;
		entity->hitFx = LancerFx;

	}
	else if (entity->name == "Margaret") {
		entity->upAnim = margaretUp;
		entity->rightAnim = margaretRight;
		entity->downAnim = margaretDown;
		entity->hitFx = knightFx; entity->battleVoiceLines.PushBack(MargaretVoice2);
		entity->battleVoiceLines.PushBack(MargaretVoice1);
	}
	else if (entity->name == "Henry VI" ) {

		entity->upAnim = henryVIUp;
		entity->rightAnim = henryVIRight;
		entity->downAnim = henryVIDown;
		entity->hitFx = mageFx;
		
			entity->battleVoiceLines.PushBack(HenryVIVoice1); entity->battleVoiceLines.PushBack(HenryVIVoice2);
		
	}
	else if (entity->name == "Jasper Tudor") {
		entity->upAnim = jasperUp;
		entity->rightAnim = jasperRight;
		entity->downAnim = jasperDown;
		entity->hitFx = knightFx;
	
			entity->battleVoiceLines.PushBack(JasperTudorVoice2);

	}
	else if (entity->name == "Richard III") {
		 
		entity->upAnim = Richard3Up;
		entity->rightAnim = Richard3Right;
		entity->downAnim = Richard3Down;
		entity->hitFx = mageFx;


	}
	else {


		switch (entity->unitType) {
		case(KNIGHT):
			entity->upAnim = knightUp;
			entity->downAnim = knightDown;
			entity->rightAnim = knightRight;
			entity->hitFx = knightFx;
			break;
		case(ARCHER):
			entity->upAnim = archerUp;
			entity->downAnim = archerDown;
			entity->rightAnim = archerRight;
			entity->hitFx = archerFx;
			break;
		case(MAGE):
			entity->upAnim = mageUp;
			entity->downAnim = mageDown;
			entity->rightAnim = archerRight;
			entity->hitFx = mageFx;
			break;
		}
	}
}
void Lerp2(float& value, float t, int destination) {
	value += (t) * (destination - value);
}

bool BattleScene::DrawHPBars(float& eHp_B, float eHp_A, float& aHpB, float aHpA, float maxE, float maxA, bool attack) {

	

	if (eHp_A < 0) { eHp_A = 0; }
	if (aHpA < 0) { aHpA = 0; }

	
	

		yorkHPBar.w = eHp_B; LancasterHPBar.w = aHpB;

		float yorkWidth = (eHp_B / maxE) * 150;
		float lancasterWidth = (aHpB / maxA) * 150;

		// Update the width of the health bars
		yorkHPBar.w = yorkWidth;
		LancasterHPBar.w = lancasterWidth;


		app->render->DrawRectangle(SDL_Rect{ yorkHPBar.x - (app->render->camera.x / 3), yorkHPBar.y - (app->render->camera.y / 3), -yorkHPBar.w , yorkHPBar.h * 3 }, b2Color(1, 1, 1, 1), true, true);
		app->render->DrawRectangle(SDL_Rect{ LancasterHPBar.x - (app->render->camera.x / 3), LancasterHPBar.y - (app->render->camera.y / 3), LancasterHPBar.w , LancasterHPBar.h * 3 }, b2Color(1, 0, 0, 1), true, true);

		SDL_Rect doubleRect = SDL_Rect{ 0,0, 172, 90 };
		app->render->DrawTexture(yorkHp, 165 * 2 + (app->render->camera.x / -3), 134 * 2 + (app->render->camera.y / -3), &doubleRect, true);
		app->render->DrawTexture(lancasterHp, 2 * 2 + (app->render->camera.x / -3), 134 * 2 + (app->render->camera.y / -3), &doubleRect, true);

		
		string YorkcurHP = to_string((int(clamp((int)eHp_B, 0, 100))));
		string LanccurHP = to_string((int(clamp((int)aHpB, 0, 100))));

		app->render->DrawText(YorkcurHP,( 183 * 2 * 3 ), (147 * 2 * 3) , 20 * YorkcurHP.length(), 38,false, SDL_Color{49,90,53,255}, CENTER_ALIGN);
		app->render->DrawText(LanccurHP, (69 * 2 * 3), (148 * 2 * 3), 20 * LanccurHP.length(), 38, false, SDL_Color{ 49,90,53,255 }, CENTER_ALIGN);

	if (attack) {

		if ((int)eHp_B <= (int)eHp_A && (int)aHpB <= (int)aHpA) {
			return true;
		}
		else if (!((int)eHp_B <= (int)eHp_A)) {
			Lerp2(eHp_B, 0.09f, eHp_A);
			return false;
		}
		else if (!((int)aHpB <= (int)aHpA)) {
			Lerp2(aHpB, 0.09f, aHpA);
			return false;
		}
	}
	else { return false; }
}

bool BattleScene::DrawExpBar(float& xpB, float xpA) {


	if ((int)xpB+1 >= (int)xpA) {
		return true;
	}

	Lerp2(xpB, 0.03f, xpA);

	expBar.w = 2 * xpB;
	if (expBar.w > 200) expBar.w = 200;

	app->render->DrawRectangle(SDL_Rect{ expBar.x - (app->render->camera.x / 3), expBar.y - (app->render->camera.y / 3) +2, expBar.w/2  , expBar.h   }, b2Color(1, 1, 1, 1), true, true);
	app->render->DrawTexture(expTexture, expBar.x - (app->render->camera.x / 3), expBar.y - (app->render->camera.y / 3));

	return false;


}

void BattleScene::SaveDeadPlayers() {

	for (ListItem<Player*>* pIt = deadPlayers.start; pIt != NULL; pIt = pIt->next) {

		pugi::xml_node playerNode = saveNode.append_child("player");
		playerNode.append_attribute("name").set_value(pIt->data->name.GetString());
		playerNode.append_attribute("x").set_value(pIt->data->position.x / 32);
		playerNode.append_attribute("y").set_value(pIt->data->position.y / 32);

		switch (pIt->data->unitType) {
		case PALADIN:
			playerNode.append_attribute("unit_type").set_value(0);
			break;
		case ARCHER:
			playerNode.append_attribute("unit_type").set_value(1);
			break;
		case KNIGHT:
			playerNode.append_attribute("unit_type").set_value(2);
			break;
		case ARMOURED_KNIGHT:
			playerNode.append_attribute("unit_type").set_value(3);
			break;
		case MAGE:
			playerNode.append_attribute("unit_type").set_value(4);
			break;
		case DARK_MAGE:
			playerNode.append_attribute("unit_type").set_value(5);
			break;
		}

		playerNode.append_attribute("attack").set_value(pIt->data->attack);
		playerNode.append_attribute("hp").set_value(pIt->data->hp);
		playerNode.append_attribute("precision").set_value(pIt->data->precision);
		playerNode.append_attribute("luck").set_value(pIt->data->luck);
		playerNode.append_attribute("speed").set_value(pIt->data->speed);
		playerNode.append_attribute("movement").set_value(pIt->data->movement);
		playerNode.append_attribute("level").set_value((int)pIt->data->level);
		playerNode.append_attribute("exp").set_value((int)pIt->data->experiencePoints);


	}
	deadPlayers.Clear();

}