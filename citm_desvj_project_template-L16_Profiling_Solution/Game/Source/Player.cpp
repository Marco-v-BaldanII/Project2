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

	return true;
}

bool Player::Start() {
	//config.attribute("texturePath").as_string()
	texture = app->tex->Load("Assets/Textures/player1.png");
	moveTime = 32;
	counter = moveTime;
	//initialize audio effect
	pickCoinFxId = app->audio->LoadFx(config.attribute("coinfxpath").as_string());
	state = IDLE;

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
			p = app->map->WorldToMap(p.x, p.y);

			if (!InitPath(p)) {
				ExpandedBFS = false;
				app->map->pathfinding->ResetBFSPath();
				state = IDLE;
			}
		}
		break;
	}
	return true;
}

bool Player::Update(float dt)
{
	//L03: DONE 4: render the player texture and modify the position of the player using WSAD keys and render the texture

	/*if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		position.x += -0.2*dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		position.x += 0.2*dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) {
		position.y += -0.2 * dt;
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		position.y += 0.2 * dt;
	}*/

	/*if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && !Move) {
		state = MOVE;
	}*/

	/*	int mouseX, mouseY;
		app->input->GetMousePosition(mouseX, mouseY);
		iPoint mouseTile = app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y);

		position.x = mouseTile.x * 32;
		position.y = mouseTile.y * 32;*/
	switch (state)
	{
	case IDLE:
		//update idle
		break;
	case MOVE:

		//Expand tiles to available
		if (!ExpandedBFS) {


		    
			app->map->pathfinding->GenerateWalkeableArea(tilePos, 3);

			ExpandedBFS = true;
		}

		//move
		MovePath();


		break;
	}
		
	app->render->DrawTexture(texture,position.x,position.y);

	return true;
}

bool Player::PostUpdate() 
{
	//draw movement area
	app->map->pathfinding->DrawBFSPath();
	switch (state)
	{
	case IDLE:
		//render idle 
		break;
	case MOVE:
	{
		//Draw path
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
			
			app->turnManager->SelectPlayer(this);
			state = MOVE;

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