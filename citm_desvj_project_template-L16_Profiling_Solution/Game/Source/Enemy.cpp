#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Player.h"
#include "Point.h"
#include "EntityManager.h"
#include "Pathfinding.h"
#include "Map.h"
#include "Physics.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	name = config.attribute("name").as_string();
	stepCounter = 0;
	moveRange = 5;
	state = IDLE;
	moveTime = 32;
	movement = 3;
	attack = 50;
	hp = 100;

	counter = moveTime;
	pathfinding = new PathFinding();
	uchar* navigationMap = NULL;
	app->entityManager->enemies.Add(this);
	entity = this;

	app->map->CreateNavigationMap(app->map->mapData.width, app->map->mapData.height, &navigationMap);
	pathfinding->SetNavigationMap((uint)app->map->GetTileWidth(), (uint)app->map->GetTileHeight(), navigationMap);
	RELEASE_ARRAY(navigationMap);

	return true;
}

bool Enemy::Start() {

	//initilize textures


	return true;
}

bool Enemy::PreUpdate() {
	switch (state)
	{
	case IDLE:
	{
		//store the entity position in tiles
		iPoint pos;
		pos.x = position.x;
		pos.y = position.y;
		pos = app->map->WorldToMap(pos.x, pos.y);

		tilePos = pos;

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
			entityTurn = true;
			HasMoveAction = true;
		}
			

		if (entityTurn)
		{

			/*if (battleState == DEAD)
				break;*/

			if (target == nullptr)
			{
				
				p2ListItem<Entity*>* p = app->entityManager->GetNearestPlayer(this);
				while (p != nullptr)
				{
					if (p->data->state == DEAD)
					{
						target = nullptr;
					}
					else {
						target = p->data;
						break;
					}

					p = p->next;
				}
			}
			else {

				if (HasAttackAction && app->map->pathfinding->CreatePath(tilePos, target->tilePos) <= 6)
				{
					state = BATTLE;
					
					HasMoveAction = false;
				}
				else if (HasMoveAction)
				{
					state = MOVE;
					
				}
				else
				{
					// if moved and no range no attack action
					HasAttackAction = false;
				}
			}
		}
		//if no attack and no move action end turn
		if (!HasAttackAction && !HasMoveAction) {
			entityTurn = false;
		}

	}
	break;
	}
	return true;
}

bool Enemy::Update(float dt)
{

	switch (state)
	{
	case IDLE:
	
		break;
	case MOVE:
		
		if (!Move) {

			//Set available movement tiles
			if (!newTarget)
			{
				app->map->pathfinding->InitBFS(tilePos);
				for (int i = 0; i < 20; i++)
				{
					app->map->pathfinding->PropagateBFS();
				}

				


				if (InitPath(target->tilePos)) {
					newTarget = true;
				}
				else {
					entityTurn = false;
				}

			}
		}

		if (MovePath()) {
			newTarget = false;
			target = nullptr;
		
		}
		break;
	case BATTLE:
	
		break;
	default:
		break;
	}


	return true;
}


bool Enemy::CleanUp()
{
	return true;
}

bool Enemy::PostUpdate() {

	//render current tile pos
	

	//app->render->DrawRectangle(r, 255, 100, 255, 150, true);
	switch (state)
	{
	case IDLE:
		//Nothing to do
		break;
	case MOVE:
	{
		//draw movement area
		app->map->pathfinding->DrawBFSPath();
		//Search for new pos//Draw path
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
				//app->render->DrawRectangle(rect, 255, 125, 125, 150);
			}
		}
	}
	break;
	default:
		break;
	}

	app->render->DrawTexture(texture, position.x, position.y, &section);

	return true;
}