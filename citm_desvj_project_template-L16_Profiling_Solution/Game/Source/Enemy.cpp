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
#include "../frame.h"
#include "../TurnManager.h"
#include "BattleScene.h"
#include "Physics.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position = app->map->MapToWorld(config.attribute("x").as_int(), config.attribute("y").as_int());
	name = config.attribute("name").as_string();
	stepCounter = 0;
	moveRange = 5;
	state = IDLE;
	moveTime = 32;
	movement = 3;
	attack = 50;
	hp = 100;
	myBattleTexture = app->tex->Load("Assets/Textures/BattleScreenSprites/KnightBS.png");
	battleBg = app->tex->Load("Assets/Textures/BattleStageOG.png");
	
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


	counter = moveTime;
	pathfinding = new PathFinding();
	HasMoveAction = true;
	HasAttackAction = true;

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
	

	std::string s = config.attribute("name").as_string();

	/*Frame(iPoint pos, float t, Appearance appr, SDL_Rect size, SDL_Texture* texture, int attack, int& hp, int precision, int luck, int speed, int movement , string name) {*/
	myFrame = new Frame(iPoint(512 + (-94 * 2), 20), 4.0f, FADE, SDL_Rect{ 0,0,94,99 }, Uitex, attack, hp, precision, luck, speed, movement, s);

	currentAnim = &downAnim;
	maxHp = hp;
	lerpingHp = hp;

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

		

	

		if (entityTurn)
		{
			drawPath = false;
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

				if (HasMoveAction && app->map->pathfinding->DistanceBetweenTiles(target->tilePos, tilePos) <= 1 && HasAttackAction)
				{
					state = BATTLE;
					
					
				}
				else if (HasMoveAction)
				{
					state = MOVE;
					
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

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame());
	switch (state)
	{
	case IDLE:
		
	
		break;
	case MOVE:
		currentAnim->Update();
		if (!Move) {

			//Set available movement tiles
			if (!newTarget)
			{
				app->map->pathfinding->InitBFS(tilePos);
				for (int i = 0; i < 10; i++)
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

		
		 if (HasAttackAction && app->map->pathfinding->DistanceBetweenTiles(target->tilePos, tilePos) <= 1) {
			state = BATTLE;
		 } else if (MovePath()) 
		 {
			 newTarget = false;
			 target = nullptr;
			 app->turnManager->noEnemyMoving = true;

		 }

		break;
	case BATTLE:
		battleTimer++;

		if (battleTimer >= 1 && battleTimer < 300) {
			//Draw the combate
			
		}

		
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
	if (app->turnManager->currentTurn == PLAYER) {
		lerpingHp = hp;
	}
	bool finishedLerp = false;

	//app->render->DrawRectangle(r, 255, 100, 255, 150, true);
	switch (state)
	{
	case IDLE:
		//Nothing to do
		break;
	case MOVE:
	{
		//draw movement area
		//app->map->pathfinding->DrawBFSPath();
		//Search for new pos//Draw path
		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
		if (path != nullptr && path->Count() > 0)
		{
			SDL_Rect rect;

			for (uint i = 0; i < movement + 1 && i < path->Count(); ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				rect.x = (pos.x);
				rect.y = (pos.y);
				rect.w = (app->map->GetTileWidth());
				rect.h = (app->map->GetTileHeight());
				app->render->DrawRectangle(rect, 255, 125, 125, 150);
			}
		}
	}
	break;
	case BATTLE:
		battleTimer++;

		if (!finishedLerp) {
			SDL_Rect bg = { 0,0,256 * 2,192 * 2 };
			app->render->DrawTexture(battleBg, app->render->camera.x / -3, app->render->camera.y / -3, &bg, false, 255);
			app->render->DrawTexture(myBattleTexture, app->render->camera.x / -3 + 100, app->render->camera.y / -3 + 100, false, false, 255);
			app->render->DrawTexture(target->myBattleTexture, app->render->camera.x / -3 + 250, app->render->camera.y / -3 + 100, false, true, 255);

			finishedLerp = app->battleScene->DrawHPBars(lerpingHp, lerpingHp - target->attack, target->lerpingHp, target->lerpingHp - attack, maxHp, target->maxHp);
		}

		if ( finishedLerp) {
			hp -= target->attack;
			target->hp -= attack;
			state = MOVE;
			HasAttackAction = false;
			HasMoveAction = false;
		}
		break;
	default:
		break;
	}


	ClickOnMe();
	if (drawPath) {

		myFrame->Render(1.0 / 60.0, hp);
		pathfinding->GenerateWalkeableArea(tilePos, movement + 1);
		pathfinding->DrawBFSPath();


	}
	else 	
	{
		pathfinding->ClearPath();
		pathfinding->ClearLastPath();
		pathfinding->visited.clear();
	}

	myFrame->Update();
	
	if (hp <= 0) {

		for (int i = 0; i < app->entityManager->enemies.Count(); ++i) {
			if (app->entityManager->enemies.At(i)->data->entity == this){
				app->entityManager->enemies.Del(app->entityManager->enemies.At(i));
			}


		}

		app->entityManager->DestroyEntity(this);
	}

	return true;
}

void Enemy::ClickOnMe() {

	clickBox.x = position.x; clickBox.y = position.y;

	int mouseX, mouseY;

	app->input->GetMouseWorldPosition(mouseX, mouseY);

	//If the position of the mouse if inside the bounds of the box 
	if (mouseX > clickBox.x && mouseX < clickBox.x + clickBox.w && mouseY > clickBox.y && mouseY < clickBox.y + clickBox.h) {

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {

			drawPath = !drawPath; 
		

		}
	}
}