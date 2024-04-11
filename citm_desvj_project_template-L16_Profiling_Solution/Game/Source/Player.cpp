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
#include "../TurnManager.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

bool Player::Awake() {

	//L03: DONE 2: Initialize Player parameters
	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
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
	hp = 100;
	attack = 20;

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
		//Select tile to move to
		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && !Move && ExpandedBFS) {
			int x, y;
			app->input->GetMouseWorldPosition(x, y);
			iPoint p;
			p.x = x;
			p.y = y;
			p = app->map->WorldToMap(p.x - app->render->camera.x, p.y - app->render->camera.y);
			
	
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
			p = app->map->WorldToMap(p.x - app->render->camera.x, p.y - app->render->camera.y);

			if (app->entityManager->IsEnemyThere(p) != nullptr) {
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

	switch (state)
	{
	case IDLE:
		//if click enemy and enemay is on attack range engage combat
		
		break;
	case MOVE:

		//Expand tiles to available
		if (!ExpandedBFS) {


		    
			app->map->pathfinding->GenerateWalkeableArea(tilePos, movement);

			ExpandedBFS = true;
		}

		//move
		MovePath();

		break;
	case BATTLE:
		if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
		{
			hp -= oponent->attack;
			oponent->hp -= attack;
			state = IDLE;
		}
		break;
	}
		
	SDL_Rect randRect = SDL_Rect{ 8,0,32,32 };
	app->render->DrawTexture(myTexture,position.x,position.y, &randRect,false, 100);

	return true;
}

bool Player::PostUpdate() 
{
	//draw movement area
	
	switch (state)
	{
	case IDLE:
		//render idle 
		break;
	case MOVE:
	{
		//Draw path
		app->map->pathfinding->DrawBFSPath();

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
	}
	return true;
}

void Player::ClickOnMe() {

		clickBox.x = position.x; clickBox.y = position.y;

		int mouseX, mouseY;

		app->input->GetMousePosition(mouseX, mouseY);

		//If the position of the mouse if inside the bounds of the box 
		if (mouseX > clickBox.x && mouseX < clickBox.x + clickBox.w && mouseY > clickBox.y && mouseY < clickBox.y + clickBox.h) {

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
				if (state == IDLE) {
					app->turnManager->SelectPlayer(this);
					state = MOVE;
				}
				else if (app->entityManager->IsEnemyThere(app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y)) == nullptr)  {
					//state = IDLE;
					//app->turnManager->DeSelectPlayer();
				}

			}
		}

	
	
	//app->render->DrawRectangle(clickBox, b2Color(1,0,1,1),false, true);

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