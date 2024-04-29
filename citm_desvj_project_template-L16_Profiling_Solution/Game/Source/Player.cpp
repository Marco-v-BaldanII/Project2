#include "Player.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"
#include "Map.h"
#include "BattleScene.h"
#include "../TurnManager.h"
#include <string>
#include "Item.h"
#include "../frame.h" /*dont include this in .h*/
#include "../Dialogue.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//Initialize Player parameters
	position = app->map->MapToWorld( config.attribute("x").as_int(), config.attribute("y").as_int());
	name = config.attribute("name").as_string();

	int type = config.attribute("unit_type").as_int();

	switch (type) {

	case 0:
		unitType = PALADIN;
		break;
	case 1:
		unitType = ARCHER;
		break;
	case 2:
		unitType = KNIGHT;
		break;
	case 3:
		unitType = ARMOURED_KNIGHT;
		break;
	case 4:
		unitType = MAGE;
		break;
	case 5: 
		unitType = DARK_MAGE;
		break;

	}
	InitializeStats(config);

	speed *= 100;


	return true;
}

bool Player::Start() {
	//config.attribute("texturePath").as_string()
	
	moveTime = 32;
	counter = moveTime;
	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
	state = IDLE;
	app->entityManager->players.add(this);


	attackRange = 1;

	battleBg = app->tex->Load("Assets/Textures/BattleStageOG.png");

	std::string s = config.attribute("name").as_string();

	myFrame = new Frame(iPoint(512 + (-94 * 2), 20), 4.0f, FADE, SDL_Rect{ 0,0,94,99 }, UiTex, attack, hp, precision, luck, speed, movement, s, this);
	currentAnim = &downAnim;
	maxHp = hp;
	lerpingHp = hp;

	myItem = new Item();


	deathQuote = new Dialogue( name.GetString(), config.child("dialogue").attribute("text").as_string());

	return true;
}

bool Player::PreUpdate() 
{
	// if the player hasn't moved this turn it can be clicked on
	if (!movedThisTurn) {
		ClickOnMe(); 
	}

	switch (state)
	{
	case IDLE:
	{
		iPoint pos;
		pos.x = position.x;
		pos.y = position.y;
		pos = app->map->WorldToMap(pos.x, pos.y);
		tilePos = pos;


		ExpandedBFS = false;
	}
	break;
	case MOVE:
		currentAnim->Update();
		//Select tile to move to
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && !Move && ExpandedBFS) {
			int x, y;
			app->input->GetMouseWorldPosition(x, y);
			iPoint p;
			p.x = x;
			p.y = y;
			p = app->map->WorldToMap(x, y);
			
	
			if (!InitPath(p)) {
				ExpandedBFS = false;
				app->map->pathfinding->ResetBFSPath();
				state = IDLE;
			}
		}
		else if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && !Move && app->turnManager->currentPlayer == this) {
			int x, y;
			app->input->GetMouseWorldPosition(x, y);
			iPoint p;
			p.x = x;
			p.y = y;
			p = app->map->WorldToMap(x,y);

			if (app->entityManager->IsEnemyThere(p) != nullptr && app->map->pathfinding->DistanceBetweenTiles(app->entityManager->IsEnemyThere(p)->data->tilePos, tilePos) <= attackRange) {
				oponent = app->entityManager->IsEnemyThere(p)->data->entity;
				state = BATTLE;
			}
		}
		break;
	}


	return true;
}

bool Player::Update(float dt)
{
	iPoint mapPos = app->map->WorldToMap(position.x, position.y);

	switch (state)
	{
	case IDLE:
		//if click enemy and enemay is on attack range engage combat
		if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && !Move && app->turnManager->currentPlayer == this) {
			int x, y;
			app->input->GetMouseWorldPosition(x, y);
			iPoint p;
			p.x = x;
			p.y = y;
			p = app->map->WorldToMap(x, y);

			if (app->entityManager->IsEnemyThere(p) != nullptr && app->map->pathfinding->DistanceBetweenTiles(app->entityManager->IsEnemyThere(p)->data->tilePos, tilePos) <= attackRange) {
				oponent = app->entityManager->IsEnemyThere(p)->data->entity;
				state = BATTLE;
			}
		}


		// Ability to pick up item

		//if (app->map->myTiles[mapPos.x][mapPos.y]->myItem != nullptr)/* If the tile i'm on has an item */ {

		//	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) {

		//		if (myItem != nullptr) {
		//			delete myItem;
		//			myItem = nullptr;
		//		}

		//		myItem = app->map->myTiles[mapPos.x][mapPos.y]->myItem;

		//		myItem->myUnit = this;
		//		app->map->myTiles[mapPos.x][mapPos.y]->myItem = nullptr;
		//	}

		//}

		break;
	case MOVE:

		//Expand tiles to available
		if (!ExpandedBFS) {



			app->map->pathfinding->GenerateWalkeableArea(tilePos, movement + myItem->GetMov());

			ExpandedBFS = true;
		}

		//move
		MovePath();

		break;
	case BATTLE:



		break;
	}

	SDL_Rect randRect = SDL_Rect{ 8,0,32,32 };

	if (hp > 0) {
		if (movedThisTurn) {
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 100);
		}
		else {
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 255);
		}
	}

	if (hp <= 0 ) {
		if(!lastWords) app->dialogueManager->SpontaneousDialogue(deathQuote); /* Last word quote */
		lastWords = true;
		if (pendingToDelete == false) {
			//app->battleScene->KillUnit(true, this); ESTO crashea y da muchisimos problemas
			pendingToDelete = true;
			app->entityManager->players.remove(this); 
			
		}
	}

	return true;
}

bool Player::PostUpdate() 
{
	if (hp > 0) {

		//draw movement area
		if (app->turnManager->currentTurn == ENEMY) {
			lerpingHp = hp;

		}
		bool finishedLerp = false;

		switch (state)
		{
		case IDLE:
			//render idle 
			break;
		case MOVE:
		{
			//Draw path
			app->map->pathfinding->DrawBFSPath();
			// ! important, movement is the only item modifier that is added rather than multiplied
			myFrame->Render(1.0 / 60.0, hp, attack * myItem->GetAtk(), speed * myItem->GetSpd(), precision * myItem->GetPrec(), luck * myItem->GetLck(), movement + myItem->GetMov());


			const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
			if (path != nullptr)
			{
				SDL_Rect rect;
				for (uint i = 0; i < path->Count(); ++i)
				{
					iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
					rect.x = (pos.x);
					rect.y = (pos.y);
					rect.w = (app->map->GetTileWidth());
					rect.h = (app->map->GetTileHeight());
					app->render->DrawRectangle(rect, 255, 125, 0, 50);
				}
			}
			break;

		}
		case BATTLE:

			battleTimer++;

			if ((!finishedLerp)) {
				SDL_Rect bg = { 0,0,256 * 2,192 * 2 };
				app->render->DrawTexture(battleBg, app->render->camera.x / -3, app->render->camera.y / -3, &bg, false, 255);
				app->render->DrawTexture(myBattleTexture, app->render->camera.x / -3 + 100, app->render->camera.y / -3 + 100, false, false, 255);
				app->render->DrawTexture(oponent->myBattleTexture, app->render->camera.x / -3 + 250, app->render->camera.y / -3 + 100, false, true, 255);

				finishedLerp = app->battleScene->DrawHPBars(lerpingHp, lerpingHp - oponent->attack, oponent->lerpingHp, oponent->lerpingHp - attack, maxHp, oponent->maxHp);
			}

			if ((finishedLerp) && battleTimer > 298) {
				if (!app->battleScene->godMode)
					CalculateAttack();

				state = IDLE;
				battleTimer = 0;
			}
			break;
		}
		myFrame->Update();

		//-----Gets mouse correctly accoutning the scale and camera-----//
		int mouseX, mouseY;

		app->input->GetMouseWorldPosition(mouseX, mouseY);


		// Draw the cursor at the adjusted position
		app->render->DrawCircle(mouseX, mouseY, 8, 1, 1, 1, 255, true);
	}
	return true;
}

void Player::ClickOnMe() {

		clickBox.x = position.x; clickBox.y = position.y;

		int mouseX, mouseY;

		app->input->GetMouseWorldPosition(mouseX, mouseY);


		//If the position of the mouse if inside the bounds of the box 
		if (mouseX > clickBox.x && mouseX < clickBox.x + clickBox.w && mouseY > clickBox.y && mouseY < clickBox.y + clickBox.h) {

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				if (state == IDLE) {
					app->turnManager->SelectPlayer(this);
					state = MOVE;
				}
				else if (app->entityManager->IsEnemyThere(app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y)) == nullptr)  {
					state = IDLE;
					app->turnManager->DeSelectPlayer();
				}

			}
		}

	
	
	

}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Player::OnCollision(PhysBody* physA, PhysBody* physB) {
	switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void Player::CalculateAttack() {

	if (myItem == nullptr) {
		hp -= oponent->attack;
		oponent->hp -= attack;

	}
	else/*I have an item to apply modifiers*/ {

		 hp = (hp + myItem->GetHp()) - oponent->attack;
		 oponent->hp -= (attack * myItem->GetAtk());
	}
}