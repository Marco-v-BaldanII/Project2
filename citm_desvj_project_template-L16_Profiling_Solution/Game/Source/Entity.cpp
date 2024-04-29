#include "Entity.h"
#include "App.h"
#include "Log.h"
#include "Physics.h"
#include "Audio.h"
#include "Render.h"
#include "Player.h"
#include "../TurnManager.h"
#include "Map.h"

#include "../frame.h"

#define MOVE_SPEED 2


bool Entity::InitPath(iPoint destiantion)
{
	bool ret = true;
	// Todo confirm choice 
	if (app->map->pathfinding->CreateVisitedPath(tilePos, destiantion) > -1)
	{
		Move = true;
	}
	else {
		//LOG("Error creating path...");
		ret = false;
	}
	return ret;
}

//function to update the entity movement
bool Entity::MovePath()
{
	if (Move)
	{
		app->turnManager->isPlayerMoving = true;
		if (app->map->pathfinding->GetLastPath()->Count() <= 0)
		{
			HasMoveAction = false;
			state = IDLE;
			return  false;
		}


		for (stepCounter; stepCounter < app->map->pathfinding->GetLastPath()->Count() && nextStep; )
		{
			nextStep = false;


			//if enemy has moved more than he can 
			//clear the path to stop the movement
			if (movement + myItem->GetMov() <= stepCounter)
				app->map->pathfinding->ClearPath();


			currentP = app->map->pathfinding->GetLastPath()->At(stepCounter);
			nextP = app->map->pathfinding->GetLastPath()->At(stepCounter + 1);



			//end movement
			if (nextP == nullptr)
			{
				Move = false;
				ExpandedBFS = false;
				nextStep = true;
				app->map->pathfinding->ResetBFSPath();
				stepCounter = 0;
				state = IDLE;
				HasMoveAction = false;
				app->turnManager->isPlayerMoving = false;
				return true;
			}

			//calculate movement direction
			direction = new iPoint;
			direction->x = nextP->x - currentP->x;
			direction->y = nextP->y - currentP->y;

			//error control and set anim
			if (direction->x >= 1)
			{
				direction->x = MOVE_SPEED;
				//currentAnim = &walkSide;
			}
			else if (direction->x <= -1)
			{
				direction->x = -MOVE_SPEED;
				//currentAnim = &walkSide;
			}
			else {
				direction->x = 0;
				//walkSide.Reset();
			}

			if (direction->y >= 1)
			{
				direction->y = MOVE_SPEED;
				//currentAnim = &walkDown;
			}
			else if (direction->y <= -1)
			{
				direction->y = -MOVE_SPEED;
				//currentAnim = &walkUp;
			}
			else {
				direction->y = 0;
				//walkUp.Reset();
				//walkDown.Reset();
			}
			// tiles
			stepCounter++;
		}
		// pixels
		counter -= 1;
		if (counter >= 0 && !nextStep)
		{
			//move
			position.x += direction->x;
			position.y += direction->y;

			//get next tile world position
			iPoint p;
			p = app->map->MapToWorld(nextP->x, nextP->y);

			//get the "correct" position
			iPoint pUpleft;

			/*pUpleft.x = position.x - app->map->GetTileWidth() * 0.5f;
			pUpleft.y = position.y - app->map->GetTileHeight() * 0.5f;*/

			// check if is in destination position
			if (position.x == p.x && position.y == p.y)
			{
				nextStep = true;

				//store the entity position in tiles
				iPoint pos;
				pos.x = position.x;
				pos.y = position.y;
				pos = app->map->WorldToMap(pos.x, pos.y);

				tilePos = pos;

				AssocateEntity_Tile(pos);
			
				
			}

			counter = moveTime/MOVE_SPEED;
		}
	}

	return false;
}

// Pass Map coordinates pls
void Entity::AssocateEntity_Tile(iPoint position) {


	if (currentTile != nullptr) {
		currentTile->myEntity = nullptr; // Desacociate the entity from the tile
	}

	//currentTile = app->map->myTiles[position.x][position.y]; // Associate the entity to the tile
	//currentTile->myEntity = this; // Associate the tile to the entity


}

void Entity::InitializeStats(pugi::xml_node config) {

	attack = config.attribute("attack").as_int();
	movement = config.attribute("movement").as_int();
	hp = config.attribute("hp").as_int();
	precision = config.attribute("precision").as_int();
	luck = config.attribute("luck").as_int();
	speed = config.attribute("speed").as_int();

	int u = 3;
}