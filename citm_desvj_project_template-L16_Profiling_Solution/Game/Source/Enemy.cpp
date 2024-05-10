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
#include "../Dialogue.h"
#include "../random.h"

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
	InitializeStats(config, true);


	counter = moveTime;
	pathfinding = new PathFinding();
	HasMoveAction = true;
	HasAttackAction = true;

	uchar* navigationMap = NULL;
	app->entityManager->enemies.Add(this);
	entity = this;

	app->map->CreateNavigationMap(app->map->mapData.width, app->map->mapData.height, &navigationMap);
	pathfinding->SetNavigationMap((uint)app->map->mapData.width, (uint)app->map->mapData.height, navigationMap);
	RELEASE_ARRAY(navigationMap);

	return true;
}

bool Enemy::Start() {

	//initilize textures
	

	std::string s = config.attribute("name").as_string();

	battleBg = app->tex->Load("Assets/Textures/BattleStageOG.png");

	/*Frame(iPoint pos, float t, Appearance appr, SDL_Rect size, SDL_Texture* texture, int attack, int& hp, int precision, int luck, int speed, int movement , string name) {*/
	
	myFrame = new Frame(iPoint(480 + (-94 * 2), 20), 4.0f, FADE, SDL_Rect{ 0,0,94,99 }, Uitex, attack, hp, precision, luck, speed, movement, s);
	currentAnim = &downAnim;
	maxHp = hp;
	lerpingHp = hp;

	myItem = new Item(); 
	deathQuote = new Dialogue(name.GetString(), config.child("dialogue").attribute("text").as_string());
	collider = app->physics->AddCollider(SDL_Rect{ 0,0,80 *3,80 *3 }, ColliderType::ENEMY_C, this, ColliderShape::QUAD);

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

			int cnt = 0;
			while (app->turnManager->players.At(cnt) != NULL) {
				if (app->map->pathfinding->DistanceBetweenTiles(app->turnManager->players.At(cnt)->data->tilePos, tilePos) <= 20)
				{
					playerNear = true;
				}

				cnt++;
			}

			if (target == nullptr && playerNear == true)
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
			else if (playerNear == true) {

				if (HasMoveAction && app->map->pathfinding->DistanceBetweenTiles(target->tilePos, tilePos) <= 1 && HasAttackAction)
				{
					state = BATTLE;
					
					
				}
				else if (HasMoveAction)
				{
					state = MOVE;
					
				}
			
			}
			if (playerNear == false)
			{
				newTarget = false;
				target = nullptr;
				app->turnManager->noEnemyMoving = true;
				Move = false;
				ExpandedBFS = false;
				nextStep = true;
				app->map->pathfinding->ResetBFSPath();
				stepCounter = 0;
				state = IDLE;
				HasMoveAction = false;
				app->turnManager->isPlayerMoving = false;
				entityTurn = false;
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

	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 255,1.0f,255,255,255);
	app->render->DrawTexture(texture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 60, 1.0f, 50, 50, 255, 0,0,0,SDL_BLENDMODE_ADD);
	switch (state)
	{
	case IDLE:
		
		 if(!defending) Bposition = iPoint(300*3, 80*3);
	
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

		
			if (HasAttackAction && app->map->pathfinding->DistanceBetweenTiles(target->tilePos, tilePos) <= 1 ) {

				state = BATTLE;
			}
			else if (MovePath())
			{
				newTarget = false;
				target = nullptr;
				app->turnManager->noEnemyMoving = true;

			}
		
		

		break;
	case BATTLE:

		if (curtains == false) {
			app->guiManager->OpenCloseCurtains();
			curtains = true;
			curtainTimer.Start();
		}

		if (curtainTimer.ReadSec() > 1) {


			/* stick figure movement */
			if (!reachedTarget && !opponentAttacking && !opponentReachTarget) {
				battleTimer++;

				FigureStickMovement(dt);
			}

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
		
	}
	finishedLerp = false;

	//app->render->DrawRectangle(r, 255, 100, 255, 150, true);
	switch (state)
	{
	case IDLE:
		//Nothing to do
		
		break;
	case MOVE:
	{
		if (!defending) lerpingHp = hp;
		/*iPoint spotPos = iPoint(position.x + 16, position.y + 16);
		app->render->DrawCircle(position.x + 16, position.y + 16, 2, 0, 0, 0, 255, true);
		app->guiManager->spotLight->Target = spotPos;*/

		//draw movement area
		//app->map->pathfinding->DrawBFSPath();
		//Search for new pos//Draw path
		const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
		if (path != nullptr && path->Count() > 0)
		{
			SDL_Rect rect;

			for (uint i = 0; i < movement + 1 && i < path->Count() && path->Count() < 6; ++i)
			{
				iPoint pos = app->map->MapToWorld(path->At(i)->x, path->At(i)->y);
				rect.x = (pos.x);
				rect.y = (pos.y);
				rect.w = (app->map->GetTileWidth());
				rect.h = (app->map->GetTileHeight());
				if(!defending) app->render->DrawRectangle(rect, 255, 125, 125, 150);
			}
		}
	}
	break;
	case BATTLE:


		if (curtainTimer.ReadSec() > 1) {

			if (battleTimer == 1)/*Determine amount of attacks*/ {

				if (speed > target->speed) {

					int doubleChance = getRandomNumber(0, 100);
					if (doubleChance <= speed) /*Double attack*/ {

						numberofAttacks = 2;
						maxNumofATTACKS = numberofAttacks;
					}

				}


			}




				if (app->battleScene->godMode) app->render->DrawRectangle(target->collider->data, b2Color(1, 1, 1, 1), false, false);


				if (opponentAttacking) {
					target->defending = true;
					target->FigureStickMovement(16.0f);
				}

				if (opponentReachTarget) {

					bool opLerp = false;

					opLerp = app->battleScene->DrawHPBars(lerpingHp, hp - target->attack, target->lerpingHp, target->lerpingHp, maxHp, target->maxHp, true);

					if (opLerp) /*hp bar lerping has finished*/ {

						hp -= target->attack;

						target->defending = false;
						opponentAttacking = false;
						opponentReachTarget = false;
						target->Bposition = iPoint(300 * 3, 80 * 3);
					}


				}


				


			

		}

		if (curtainTimer.ReadMSec() > 800) {

			SDL_Rect bg = { 0,0,256 * 2,192 * 2 };
			app->render->DrawTexture(battleBg, app->render->camera.x / -3, app->render->camera.y / -3, &bg, false, 255);
			//app->render->DrawTexture(myBattleTexture,  Bposition.x,  Bposition.y, false, true, 255);

			//int x = curvedTrajectory()
			app->render->DrawTexture(target->myBattleTexture, -app->render->camera.x / 3 + target->Bposition.x / 3, -app->render->camera.y / 3 + target->Bposition.y / 3, false, false, 255);
			app->render->DrawTexture(myBattleTexture, -app->render->camera.x / 3 + Bposition.x / 3, -app->render->camera.y / 3 + Bposition.y / 3, false, true, 255);


			if (reachedTarget) {
				if (DealDMG()) {
					// ready for another attack ? , lerping hp must be = hp
					numberofAttacks--;
					lerpingHp = hp;
					reachedTarget = false;
					Bposition = iPoint(300 * 3, 80 * 3);

				}
			}
			else {
				app->battleScene->DrawHPBars(lerpingHp, hp, target->lerpingHp, target->lerpingHp, maxHp, target->maxHp, false);
			}

		}
		
		break;
	}


	ClickOnMe();
	if (drawPath) {

		
		pathfinding->GenerateWalkeableArea(tilePos, movement + 1);
		pathfinding->DrawBFSPath();
		myFrame->Render(1.0 / 60.0, hp, attack, speed, precision, luck, movement);

	}
	else 	
	{
		pathfinding->ClearPath();
		pathfinding->ClearLastPath();
		pathfinding->visited.clear();
	}

	myFrame->Update();
	
	if (hp <= 0) {

		if (!giveExp) {
			target->lerpedExp = false;
			target->oponent = this;
			target->opponentAttacking = false;
			giveExp = true;
		}

		if (!lastWords && deathQuote->text != "") app->dialogueManager->SpontaneousDialogue(deathQuote); /* Last word quote */
		lastWords = true;

		for (int i = 0; i < app->entityManager->enemies.Count(); ++i) {
			if (app->entityManager->enemies.At(i)->data->entity == this){
				app->entityManager->enemies.Del(app->entityManager->enemies.At(i));
				app->turnManager->noEnemyMoving = true;
				app->turnManager->noEnemyMoving = true;
			}


		}

	

		app->questManager->CheckQuestCompletion(name.GetString());
		app->entityManager->DestroyEntity(this);
		//app->battleScene->KillUnit(false, this);
	}

	if (target != nullptr) { target->collider->data.x = (-40) + target->Bposition.x; target->collider->data.y = (0) + target->Bposition.y; }
	collider->data.x = (-40) + Bposition.x; collider->data.y = (0) + Bposition.y;
	//app->render->DrawTexture(myBattleTexture,-app->render->camera.x/2 +  Bposition.x /3,-app->render->camera.y/3 +   Bposition.y/3, false, true, 255);
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

bool Enemy::DealDMG() {

	if (numberofAttacks <= 0) {
		state = MOVE;
		HasAttackAction = false;
		HasMoveAction = false;
		battleTimer = 0;
		
	}
	else {
		finishedLerp = app->battleScene->DrawHPBars(lerpingHp, lerpingHp, target->lerpingHp, target->hp - attack, maxHp, target->maxHp, true);

		if (finishedLerp || numberofAttacks < 0) {

			target->hp -= attack;
			target->lerpingHp = target->hp;
			if (numberofAttacks == maxNumofATTACKS) opponentAttacking = true;

			return true;
		}
	}
	return false;
}


void Enemy::OnCollision(Collider* physA, Collider* physB) {

	if (physA->type != physB->type && state == BATTLE) {
		LOG("something");


		if (opponentAttacking) {

			opponentReachTarget = true;
			opponentAttacking = false;

		}
		else if (!opponentReachTarget) {

			reachedTarget = true;
		}
	}
	                                      

}

void Enemy::FigureStickMovement(float dt) {

	Bposition.x += velocity.x * (dt / 100);
	Bposition.y += velocity.y * (dt / 100);

	velocity.y += 10 * (dt / 100);
	if (Bposition.y >= GROUND) {
		Bposition.y = GROUND;
		velocity.y *= -BOUNCE;
	}

}