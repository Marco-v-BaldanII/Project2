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
#include "../random.h"
#include "Timer.h"
#include "../Inventory.h"
#include "Timer.h"

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
	realname = config.attribute("name").as_string();
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
	InitializeStats(config, false);

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

	switch (unitType) {
	case 0:
		//unitType = PALADIN;
		attackRange = 1;
		break;
	case 1:
		//	unitType = ARCHER;
		attackRange = 2;
		break;
	case 2:
		//	unitType = KNIGHT;
		attackRange = 1;
		break;
	case 3:
		//unitType = ARMOURED_KNIGHT;
		attackRange = 1;
		break;
	case 4:
		//unitType = MAGE;
		attackRange = 2;
		break;
	case 5:
		//unitType = DARK_MAGE;
		attackRange = 2;
		break;
	}


	battleBg = app->tex->Load("Assets/Textures/BattleStageOG.png");

	std::string s = config.attribute("name").as_string();

	myFrame = new Frame(iPoint(512 + (-94 * 2), 20), 4.0f, FADE, SDL_Rect{ 0,0,94,99 }, UiTex, attack, hp, precision, luck, speed, movement, s, this);
	currentAnim = &downAnim;
	maxHp = hp;
	lerpingHp = hp;

	myItem = new Item();

	HasMoveAction = true;
	deathQuote = new Dialogue( name.GetString(), config.child("dialogue").attribute("text").as_string());
	lvlUpQuote = new Dialogue(name.GetString(), config.child("lvlUp").attribute("text").as_string());


	personalPathfinding = new PathFinding();

	uchar* navigationMap = NULL;
	app->map->CreateNavigationMap(app->map->mapData.width, app->map->mapData.height, &navigationMap);
	personalPathfinding->SetNavigationMap((uint)app->map->mapData.width, (uint)app->map->mapData.height, navigationMap);
	RELEASE_ARRAY(navigationMap);
	collider = app->physics->AddCollider(SDL_Rect{ 0,0,80 * 3,80 * 3 }, ColliderType::PLAYER_C, this, ColliderShape::QUAD);


	if (atkButton == nullptr) atkButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, atkBtnId, " Attack ", SDL_Rect{ 0,0,28  ,14 }, this, SDL_Rect{ 0,0,0,0 }, WORLD, SDL_Color{ 158,112,63,255 }, SDL_Color{70,51,29,255});
	if (waitButton == nullptr) waitButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, waitBtnId, " Wait ", SDL_Rect{ 0,0,28,14 }, this, SDL_Rect{ 0,0,0,0 }, WORLD, SDL_Color{ 158,112,63,255 }, SDL_Color{ 70,51,29,255 });
	if (talkButton == nullptr) talkButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, talkBtnId, " Talk ", SDL_Rect{ 0,0,28,14 }, this, SDL_Rect{ 0,0,0,0 }, WORLD, SDL_Color{ 158,112,63,255 }, SDL_Color{ 70,51,29,255 });

	atkButton->state = GuiControlState::DISABLED; waitButton->state = GuiControlState::DISABLED; talkButton->state = GuiControlState::DISABLED;

	for (pugi::xml_node no = config.child("conversation"); no != NULL; no = no.next_sibling("conversation")) {

		Conversation* conv = new Conversation();
		conv->name1 = this->name.GetString();
		conv->name2 = no.attribute("owner").as_string();



		for (pugi::xml_node dN = no.child("dialogue"); dN != NULL; dN = dN.next_sibling("dialogue")) {

			Dialogue* di = new Dialogue(dN.attribute("owner").as_string(), dN.attribute("text").as_string());
			conv->dialogues.Add(di);
		}
		conversations.Add(conv);
	}

	return true;
}

bool Player::PreUpdate() 
{

	atkButton->bounds.x = position.x - 30; atkButton->bounds.y = position.y - 40;
	waitButton->bounds.x = position.x + 30; waitButton->bounds.y = position.y - 40;
	talkButton->bounds.x = position.x ; talkButton->bounds.y = position.y - 70;

	// if the player hasn't moved this turn it can be clicked on
	

	switch (state)
	{
	case IDLE:
	{
		iPoint pos;
		pos.x = position.x;
		pos.y = position.y;
		pos = app->map->WorldToMap(pos.x, pos.y);
		tilePos = pos;

		atkButton->state = GuiControlState::DISABLED;
		waitButton->state = GuiControlState::DISABLED;
		talkButton->state = GuiControlState::DISABLED;
		ExpandedBFS = false;
	}
	break;
	case MOVE:
		currentAnim->Update();

		if(atkButton->state == GuiControlState::DISABLED)atkButton->state = GuiControlState::NORMAL;
		if(waitButton->state == GuiControlState::DISABLED) waitButton->state = GuiControlState::NORMAL;
		if (talkButton->state == GuiControlState::DISABLED) talkButton->state = GuiControlState::NORMAL;

		//Select tile to move to

		if (waitButton->state != GuiControlState::FOCUSED && waitButton->state != GuiControlState::PRESSED && waitButton->state != GuiControlState::SELECTED
			&& atkButton->state != GuiControlState::FOCUSED && atkButton->state != GuiControlState::PRESSED && atkButton->state != GuiControlState::SELECTED
			&& talkButton->state != GuiControlState::FOCUSED && talkButton->state != GuiControlState::PRESSED && talkButton->state != GuiControlState::SELECTED
			
			) {

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && !Move && ExpandedBFS && HasMoveAction) {
				app->map->drawGrid = false;
				int x, y;
				app->input->GetMouseWorldPosition(x, y);
				iPoint p;
				p.x = x;
				p.y = y;
				p = app->map->WorldToMap(x, y);

				if (app->map->pathfinding->IsTileEmpty(p) && app->map->pathfinding->DistanceBetweenTiles(p, tilePos) < movement) {
					if (!InitPath(p)) {
						ExpandedBFS = false;
						app->map->pathfinding->ResetBFSPath();
						state = IDLE;
					}
				}
			}
			if (!Move && app->turnManager->currentPlayer == this ) {

				
				if (atkButton->state == GuiControlState::DISABLED) atkButton->state = GuiControlState::NORMAL;
				if (waitButton->state == GuiControlState::DISABLED) waitButton->state = GuiControlState::NORMAL;
				if (talkButton->state == GuiControlState::DISABLED) talkButton->state = GuiControlState::NORMAL;

				int x, y;
				app->input->GetMouseWorldPosition(x, y);

				// Attack code
				if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && atckedClicked) {
					app->map->drawGrid = false;
					iPoint p;
					p.x = x;
					p.y = y;
					p = app->map->WorldToMap(x, y);

					if (app->entityManager->IsEnemyThere(p) != nullptr && app->map->pathfinding->DistanceBetweenTiles(app->entityManager->IsEnemyThere(p)->data->tilePos, tilePos) <= attackRange) {
						oponent = app->entityManager->IsEnemyThere(p)->data->entity;
						oponent->target = this;
						state = BATTLE;
						atckedClicked = false;
					}
				}
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && !Move && app->turnManager->currentPlayer == this) {
				app->map->drawGrid = false;
				int x, y;
				app->input->GetMouseWorldPosition(x, y);
				iPoint p;
				p.x = x;
				p.y = y;
				p = app->map->WorldToMap(x, y);

				if (app->map->pathfinding->IsLeverThere(p) && app->map->pathfinding->DistanceBetweenTiles(p, tilePos) == 1)
				{

					app->map->pathfinding->ActivateLever(p);
				}
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
		if(!defending) Bposition = iPoint(50 * 3, 80 * 3);

		//if click enemy and enemay is on attack range engage combat
	
		if (app->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && !Move && app->turnManager->currentPlayer == this) {
			// lo de el ataque
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

		if (app->map->myTiles[mapPos.x][mapPos.y]->myItem != nullptr)/* If the tile i'm on has an item */ {

			if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {

				myItem = app->map->myTiles[mapPos.x][mapPos.y]->myItem;

				app->inventory->InventoryItems.Add(myItem);
				app->map->myTiles[mapPos.x][mapPos.y]->myItem = nullptr;
			}

		}

		break;
	case MOVE:
		//app->guiManager->spotLight->visible = true;

		

		//Expand tiles to available
		if (!ExpandedBFS) {



			app->map->pathfinding->GenerateWalkeableArea(tilePos, movement + myItem->GetMov());

			ExpandedBFS = true;
		}

		//move
		MovePath();
		
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

			if (oponent != nullptr && oponent->hp <= 0 && lerpedExp) {
				LOG("earn experience");
				lerpedExp = false;
			}
		}
		break;
	}

	if (hp > 0) {
		if (movedThisTurn) {
			
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 120);
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 60, 1, 255, 0, 0, 0, 0, SDL_BLENDMODE_ADD);
		}
		else {
			
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 255);
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), false, 60, 1, 255, 0, 0, 0, 0, SDL_BLENDMODE_ADD);
		}
	}

	SDL_Rect randRect = SDL_Rect{ 8,0,32,32 };

	

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

		if (!movedThisTurn) {
			ClickOnMe();
		}
	

		//draw movement area
		if (app->turnManager->currentTurn == ENEMY) {
			

		}
		finishedLerp = false;

		switch (state)
		{
		case IDLE:
			//render idle 
			
			break;
		case MOVE:
		{
			if (!defending) lerpingHp = hp; lerpingEXP = experiencePoints;
			//Draw spotLight
			iPoint spotPos = iPoint(position.x + 16, position.y + 16);
			app->render->DrawCircle(position.x +16, position.y +16, 2, 0, 0, 0, 255, true);
			

			//app->render->CameraCenterOn(position.x -200, position.y -200);
			app->guiManager->spotLight->Target = spotPos;
			app->guiManager->TurnOnSpotLight();
			
			//Draw path
			if (app->turnManager->currentPlayer == this) 
			{
				
					personalPathfinding->GenerateWalkeableArea(tilePos, movement + myItem->GetMov());
				
				
				personalPathfinding->DrawBFSPath();
				
			}
			
			// ! important, movement is the only item modifier that is added rather than multiplied
			myFrame->Render(1.0 / 60.0, hp, attack * myItem->GetAtk(), speed * myItem->GetSpd(), precision * myItem->GetPrec(), luck * myItem->GetLck(), movement + myItem->GetMov());


			const DynArray<iPoint>* path = app->map->pathfinding->GetLastPath();
			if (path != nullptr && app->turnManager->currentPlayer == this)
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
			personalPathfinding->ResetBFSPath();
			break;

		}
		case BATTLE:

			atkButton->state = GuiControlState::DISABLED;
			waitButton->state = GuiControlState::DISABLED;
			talkButton->state = GuiControlState::DISABLED;


			if (curtainTimer.ReadSec() > 1) {


				app->guiManager->TurnOffSpotLight();

				if (battleTimer == 1 || battleTimer == 0)/*Determine amount of attacks*/ {

					if (speed > oponent->speed - 1) {

						int doubleChance = getRandomNumber(0, 100);
						if (doubleChance <= speed) /*Double attack*/ {

							numberofAttacks = 2;
							maxNumofATTACKS = numberofAttacks;
						}

					}

				}

				




			}
			if (curtainTimer.ReadMSec() > 800) {
				DrawCombatScene();
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

	if (lerpedExp == false && oponent != nullptr) {

		bool finishedLERP = app->battleScene->DrawExpBar(lerpingEXP, experiencePoints + oponent->rewardEXP);
		if (finishedLERP) {
			if (!receivedEXP) {
				receivedEXP = true;
				experiencePoints += oponent->rewardEXP;
			}

			if (experiencePoints >= 100)/*Level up*/ {

				level++;
				experiencePoints -= 100;
				
				lvlUp = true;

			}
			if (!lvlUp) {
				lerpingEXP = experiencePoints;
				lerpedExp = true;
				oponent = nullptr;
			}
			
		}

	}
	/* end the lvl up pannel */
	

	if (lvlUp) {
		SDL_Rect rect = SDL_Rect{ 0,0,189 * 2, 39 * 2 };
		app->render->DrawTexture(app->battleScene->lvlUpTexture, (app->render->camera.x / -3) + 33 * 3, (app->render->camera.y / -3) + 23 * 3, &rect);
		if (app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			lvlUp = false;
			lerpedExp = true;
			oponent = nullptr;
			lvlMods = LevelUp();
			showLvlUp.Start();
			app->dialogueManager->SpontaneousDialogue(lvlUpQuote);
		}
	}

	if(lvlMods != iPoint(-1,-1) && showLvlUp.ReadSec() < 4){
		// ! important, movement is the only item modifier that is added rather than multiplied
		int spdMod = 0; int luckMod = 0; int atkMod = 0; int hpMod= 0;
		switch (lvlMods.x) {
		case 0:
			spdMod = 5;
			break;
		case 1:
			luckMod = 5;
			break;
		}
		switch (lvlMods.y) {
		case 0:
			hpMod = 4;
			break;
		case 1:
			atkMod = 2;
			break;
		}
		myFrame->Render(1.0 / 60.0, hp - hpMod, attack* myItem->GetAtk() -atkMod, speed* myItem->GetSpd() - spdMod, precision* myItem->GetPrec(), luck* myItem->GetLck() - luckMod, movement + myItem->GetMov(), hpMod, atkMod, spdMod, 0 , luckMod);
		if ((app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN )&& showLvlUp.ReadSec() > 1) {
			lvlMods = iPoint(-1, -1);
		}
	
	}

	if (atckedClicked ) {
		app->map->DrawAdjacents(tilePos);
	}
	
	collider->data.x = (-40) + Bposition.x; collider->data.y = (0) + Bposition.y;
	if(oponent != nullptr){ oponent->collider->data.x = (-40) + oponent->Bposition.x; oponent->collider->data.y = (0) + oponent->Bposition.y; }
	if (app->battleScene->godMode) app->render->DrawRectangle(oponent->collider->data, b2Color(1, 1, 1, 1), false, false);


	

	return true;
}

bool Player::SuperPostUpdate() {

	// I have put this here specificaly so that it reads the input from the buttons before this

	
		myFrame->Render(1.0 / 60.0, hp, attack, speed, precision, luck, movement);

	

	return true;
}

void Player::ClickOnMe() {

	

	if (!movedThisTurn) {

		

		clickBox.x = position.x; clickBox.y = position.y;

		int mouseX, mouseY;

		app->input->GetMouseWorldPosition(mouseX, mouseY);


		//If the position of the mouse if inside the bounds of the box 
		if (mouseX > clickBox.x && mouseX < clickBox.x + clickBox.w && mouseY > clickBox.y && mouseY < clickBox.y + clickBox.h && app->turnManager->isPlayerMoving == false) {

			

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {

				app->turnManager->DeSelectPlayer();

				if (app->battleScene->talking == true) {
					if (app->battleScene->talker1 != nullptr && app->battleScene->talker1 != this) {
						app->battleScene->talker2 = this;
					}
				}

				if (state == IDLE) {
					
					app->turnManager->SelectPlayer(this);
					state = MOVE;
				}
				else if (app->entityManager->IsEnemyThere(app->map->WorldToMap(mouseX - app->render->camera.x, mouseY - app->render->camera.y)) == nullptr) {
					
					state = IDLE;
					app->turnManager->DeSelectPlayer();
				}

			}
		}
	}
	

}

bool Player::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 


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

void Player::OnCollision(Collider* physA, Collider* physB)  {

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

bool Player::DealDMG() {

	if (numberofAttacks <= 0) {
		state = MOVE;
		HasAttackAction = false;
		HasMoveAction = false;
		reachedTarget = false;
		battleTimer = 1;
	}
	else {
		finishedLerp = app->battleScene->DrawHPBars( oponent->lerpingHp, oponent->hp - attack, lerpingHp, lerpingHp, oponent->maxHp, maxHp, true);

		if (finishedLerp || numberofAttacks < 0) {

			oponent->hp -= attack;
			oponent->lerpingHp = oponent->hp;
			reachedTarget = false;
			if (numberofAttacks == maxNumofATTACKS) opponentAttacking = true;

			return true;
		}
	}

	return false;

}

iPoint Player::LevelUp() {

	int coin = getRandomNumber(0, 1);
	int coin2 = getRandomNumber(0, 1);

	switch (coin) {
	case 0:
		speed += 5;
		break;
	case 1:
		luck += 5;
		break;
	}

	switch (coin2) {
	case 0:
		hp += 4;
		break;
	case 1:
		attack += 2;
		break;
	}

	return iPoint(coin, coin2);

}

void Player::FigureStickMovement(float dt) {

	Bposition.x -= velocity.x * (dt / 100);
	Bposition.y += velocity.y * (dt / 100);

	velocity.y += 10 * (dt / 100);
	if (Bposition.y >= GROUND) {
		Bposition.y = GROUND;
		velocity.y *= -BOUNCE;
	}

	if (numberofAttacks <= 0 && lerpedExp == true) {
		state = MOVE;
		HasAttackAction = false;
		HasMoveAction = false;
		reachedTarget = false;
		battleTimer = 1;
	}

}

void Player::DrawCombatScene() {

	if (oponent != nullptr) {
		SDL_Rect bg = { 0,0,256 * 2,192 * 2 };
		app->render->DrawTexture(battleBg, app->render->camera.x / -3, app->render->camera.y / -3, &bg, false, 255);

		//app->render->DrawTexture(myBattleTexture,  Bposition.x,  Bposition.y, false, true, 255);

		//int x = curvedTrajectory()


		app->render->DrawTexture(oponent->myBattleTexture, -app->render->camera.x / 3 + oponent->Bposition.x / 3, -app->render->camera.y / 3 + oponent->Bposition.y / 3, false, true, 255);
		app->render->DrawTexture(myBattleTexture, -app->render->camera.x / 3 + Bposition.x / 3, -app->render->camera.y / 3 + Bposition.y / 3, false, false, 255);


		/* Enemy amnimation logic */

		if (opponentAttacking) {
			oponent->defending = true;
			oponent->FigureStickMovement(16.0f);
		}

		if (opponentReachTarget) {

			bool opLerp = false;

			opLerp = app->battleScene->DrawHPBars(oponent->lerpingHp, oponent->lerpingHp, lerpingHp, hp - oponent->attack, oponent->maxHp, maxHp, true);

			if (opLerp) /*hp bar lerping has finished*/ {

				hp -= oponent->attack;

				oponent->defending = false;
				opponentAttacking = false;
				opponentReachTarget = false;
				oponent->Bposition = iPoint(300 * 3, 80 * 3);
			}


		}

		if (reachedTarget) {
			if (DealDMG()) {
				// ready for another attack ? , lerping hp must be = hp
				numberofAttacks--;


				lerpingHp = hp;
				reachedTarget = false;

				Bposition = iPoint(50 * 3, 80 * 3);/* change to interpolate back to start */

			}
		}
		else if (!opponentReachTarget) {
			app->battleScene->DrawHPBars(oponent->hp, oponent->hp, hp, hp, oponent->maxHp, maxHp, false);
		}

	}

}

bool Player::OnGuiMouseClickEvent(GuiControl* control)  {

	LOG("my button has been pressed");
	if (control->id == atkBtnId) {
		// state attacking
		// find adjacent tiles and draw them in red
		// if you click on a tile with an enemy perform the attacking code
		 iPoint y = tilePos;
		 atckedClicked = true;
		 numberofAttacks = 1;

	}


	if (control->id == waitBtnId && app->turnManager->currentPlayer == this && app->turnManager->isPlayerMoving == false) {
		state = IDLE;
		HasMoveAction = false;
		atckedClicked = false;
	}

	if (control->id == talkBtnId) {
		// state attacking
		// find adjacent tiles and draw them in red
		// if you click on a tile with an enemy perform the attacking code
		LOG("talk");
		app->battleScene->talking = true;
		app->battleScene->talker1 = this;
		

	}
	return true;
}