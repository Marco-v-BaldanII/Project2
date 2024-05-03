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

	Start();
	
	return ret;
}

// Called before the first frame
bool BattleScene::Start()
{
	if (active) {
		if (!started) {
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
		}
			
		for (pugi::xml_node questNode = mynode.child("battleMaps").child("map").child("mainQuest"); questNode != NULL; questNode = questNode.next_sibling("mainQuest")) {

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
				p->myBattleTexture = portraitTextures[name];
				p->UiTex = lancasterUI;
				p->Start();
				PassAnimations(p);

			}

			// Read enemies from config and instantiate them
			for (pugi::xml_node Enode = mynode.child("battleMaps").child("map").child("enemy"); Enode != NULL; Enode = Enode.next_sibling("enemy")) {

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
	if (!firstMap) app->render->camera.y = 0; firstMap = true;
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

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 10;

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 10;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 10;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 10;
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
		godMode = !godMode;

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
	LOG("Freeing Battlescene main menu ");
	//app->tex->UnLoad(spriteSheet);
	
	//app->guiManager->Main_Menu_Panel->Disable();

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
				mageRight.speed = 0.2f;
			}
			else if (s == "up") {
				mageUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				mageUp.speed = 0.2f;
			}
			else if (s == "down") {
				mageDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				mageDown.speed = 0.2f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Archer").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				archerRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerRight.speed = 0.2f;
			}
			else if (s == "up") {
				archerUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerUp.speed = 0.2f;
			}
			else if (s == "down") {
				archerDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				archerDown.speed = 0.2f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("Knight").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				knightRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightRight.speed = 0.2f;
			}
			else if (s == "up") {
				knightUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightUp.speed = 0.2f;
			}
			else if (s == "down") {
				knightDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				knightDown.speed = 0.2f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("HenryTudor").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				henryTudorRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorRight.speed = 0.2f;
			}
			else if (s == "up") {
				henryTudorUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorUp.speed = 0.2f;
			}
			else if (s == "down") {
				henryTudorDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				henryTudorDown.speed = 0.2f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("DukeofYork").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				dukeYorkRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkRight.speed = 0.2f;
			}
			else if (s == "up") {
				dukeYorkUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkUp.speed = 0.2f;
			}
			else if (s == "down") {
				dukeYorkDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				dukeYorkDown.speed = 0.2f;
			}
		}
	}

	for (pugi::xml_node node = mynode.child("PrinceEdward").child("animation"); node != NULL; node = node.next_sibling("animation")) {

		string s = node.attribute("name").as_string();
		for (pugi::xml_node fNode = node.child("frame"); fNode != NULL; fNode = fNode.next_sibling("frame")) {

			if (s == "wrigth") {
				princeEdRight.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdRight.speed = 0.2f;
			}
			else if (s == "up") {
				princeEdUp.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdUp.speed = 0.2f;
			}
			else if (s == "down") {
				princeEdDown.PushBack(SDL_Rect{ fNode.attribute("x").as_int(), fNode.attribute("y").as_int(), fNode.attribute("w").as_int(), fNode.attribute("h").as_int() });
				princeEdDown.speed = 0.2f;
			}
		}
	}

	LancasterAnim.PushBack(SDL_Rect{ 0,288,32,32 });
}

void BattleScene::PassAnimations(Entity* entity) {

	if (entity->name == "Prince Edward") {
		entity->upAnim = princeEdUp;
		entity->downAnim = princeEdDown;
		entity->rightAnim = princeEdRight;

	}
	else if (entity->name == "Duke of York") {
		entity->upAnim = dukeYorkUp;
		entity->downAnim = dukeYorkDown;
		entity->rightAnim = dukeYorkRight;
	}
	else if (entity->name == "Henry Tudor") {
		entity->upAnim = henryTudorUp;
		entity->downAnim = henryTudorDown;
		entity->rightAnim = henryTudorRight;
	}
	else {
		entity->downAnim = LancasterAnim;

		switch (entity->unitType) {
		case(KNIGHT):
			entity->upAnim = knightUp;
			entity->downAnim = knightDown;
			entity->rightAnim = knightRight;
			break;
		case(ARCHER):
			entity->upAnim = archerUp;
			entity->downAnim = archerDown;
			entity->rightAnim = archerRight;
			break;
		case(MAGE):
			entity->upAnim = mageUp;
			entity->downAnim = mageDown;
			entity->rightAnim = archerRight;
			break;
		}
	}
}
void Lerp2(float& value, float t, int destination) {
	value += (t) * (destination - value);
}

bool BattleScene::DrawHPBars(float& eHp_B, float eHp_A, float& aHpB, float aHpA, float maxE, float maxA) {

	

	if (eHp_A < 0) { eHp_A = 0; }
	if (aHpA < 0) { aHpA = 0; }

	
	yorkHPBar.w = eHp_B; LancasterHPBar.w = aHpB;

	float yorkWidth = (eHp_B / maxE) * 150;
	float lancasterWidth = (aHpB / maxA) * 150;

	// Update the width of the health bars
	yorkHPBar.w = yorkWidth;
	LancasterHPBar.w = lancasterWidth;


	app->render->DrawRectangle(SDL_Rect{ yorkHPBar.x - (app->render->camera.x / 3), yorkHPBar.y - (app->render->camera.y / 3), -yorkHPBar.w , yorkHPBar.h *3} , b2Color(1, 1, 1, 1), true, true);
	app->render->DrawRectangle(SDL_Rect{ LancasterHPBar.x - (app->render->camera.x / 3), LancasterHPBar.y - (app->render->camera.y / 3), LancasterHPBar.w , LancasterHPBar.h * 3 }, b2Color(1, 0, 0, 1), true, true);

	SDL_Rect doubleRect = SDL_Rect{ 0,0, 172, 90 };
	app->render->DrawTexture(yorkHp, 165 * 2 + (app->render->camera.x / -3), 134 * 2 + (app->render->camera.y / -3), &doubleRect, true);
	app->render->DrawTexture(lancasterHp, 2 * 2 + (app->render->camera.x / -3), 134 * 2 + (app->render->camera.y / -3), &doubleRect, true);

	if ((int)eHp_B <= (int)eHp_A && (int)aHpA <= (int)aHpA) {
		return true;
	}
	else if (!((int) eHp_B <=(int) eHp_A)) {
		Lerp2(eHp_B, 0.009f, eHp_A); 
		return false;
	}
	else if (!((int)aHpB <= (int)aHpA)) {
		Lerp2(aHpB, 0.009f, aHpA);
		return false;
	}
}