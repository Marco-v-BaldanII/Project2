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
#include "Pathfinding.h"
#include "../Easing.h"

Player::Player() : Entity(EntityType::PLAYER)
{
	name.Create("Player");
}

Player::~Player() {

}

Player::Player(Player* p) : Entity(EntityType::PLAYER) {

	hp = p->hp;
	attack = p->attack;
	maxHp = p->maxHp;
	name = p->name;
	precision = p->precision;
	luck = p->luck;
	movement = p->movement;
	unitType = p->unitType;
	level = p->level;
	experiencePoints = p->experiencePoints;

}

bool Player::Awake() {

	//Initialize Player parameters
	position = app->map->MapToWorld( config.attribute("x").as_int(), config.attribute("y").as_int());
	name = config.attribute("name").as_string();
	realname = config.attribute("name").as_string();
	int type = config.attribute("unit_type").as_int();
	easing = new Easing(1.2f);

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
	
	walkingParticle = new Particle("Assets/snowP.png", fPoint(0, 0), fPoint(0, -0.5f), fPoint(0, 0), 0.5f, SDL_Rect{ 0,0,8,8 },1,1,90,200);

	walkingEffect = new ParticleEffect(walkingParticle, 10, 0.05f, true,false, SDL_Rect{0,0,8,0},false, BACK, WORLD);

	app->particleSystem->AddParticleEffect(walkingEffect);

	walkingEffect->active = false;

	moveTime = 32;
	counter = moveTime;


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

	pickItemFx = app->audio->LoadFx("Assets/Audio/Fx/PickUPItemFX.ogg");

	battleBg = app->tex->Load("Assets/Textures/Battle scene editted.png");
	missText = app->tex->Load("Assets/Textures/UI/MissSign.png");

	std::string s = config.attribute("name").as_string();

	myFrame = new Frame(iPoint(480 + (-94 * 2), 20), 4.0f, FADE, SDL_Rect{ 0,0,94,99 }, UiTex, attack, hp, precision, luck, speed, movement, s, this);
	currentAnim = &downAnim;
	maxHp = hp;
	lerpingHp = hp;

	//myItem = new Item();

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
	if (itemButton == nullptr)itemButton = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, itemBtnId, "PickUp", SDL_Rect{ 0,0,28,14 }, this, SDL_Rect{ 0,0,0,0 }, WORLD, SDL_Color{ 158,112,63,255 }, SDL_Color{ 70,51,29,255 });

	atkButton->state = GuiControlState::DISABLED; waitButton->state = GuiControlState::DISABLED; talkButton->state = GuiControlState::DISABLED; itemButton->state = GuiControlState::DISABLED;

	LoadConversation(config);

	experiencePoints = config.attribute("exp").as_int();
	level = config.attribute("exp").as_int();

	return true;
}

void Player::LoadConversation(pugi::xml_node node) {

	for (pugi::xml_node no = node.child("conversation"); no != NULL; no = no.next_sibling("conversation")) {

		Conversation* conv = new Conversation();
		conv->name1 = this->name.GetString();
		conv->name2 = no.attribute("owner").as_string();



		for (pugi::xml_node dN = no.child("dialogue"); dN != NULL; dN = dN.next_sibling("dialogue")) {

			Dialogue* di = new Dialogue(dN.attribute("owner").as_string(), dN.attribute("text").as_string());
			conv->dialogues.Add(di);
		}
		conversations.Add(conv);
	}


}

bool Player::PreUpdate() 
{
	atkButton->bounds.x = position.x - 30; atkButton->bounds.y = position.y - 40;
	waitButton->bounds.x = position.x + 30; waitButton->bounds.y = position.y - 40;
	talkButton->bounds.x = position.x ; talkButton->bounds.y = position.y - 70;
	itemButton->bounds.x = position.x; itemButton->bounds.y = position.y +30;

	// if the player hasn't moved this turn it can be clicked on

	if (myItem != nullptr && myItem->myType == CONSUMABLE) {
		hp += myItem->GetHp();
		myItem = nullptr;
		if (hp > maxHp) { hp = maxHp; }
		// Insert healing soundFx
	}

	
	walkingEffect->spawnBox.x = position.x; walkingEffect->spawnBox.y = position.y +15;

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
		walkingEffect->active = false;
	}
	break;
	case MOVE:
		currentAnim->Update();
		walkingEffect->active = true;
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

				int extraMove = 0;
				if (myItem != nullptr) { extraMove = myItem->GetMov(); }
					if (app->map->pathfinding->IsTileEmpty(p) && app->map->pathfinding->DistanceBetweenTiles(p, tilePos) < movement + extraMove) {
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



		break;
	case MOVE:
		//app->guiManager->spotLight->visible = true;

		if (app->map->myTiles[mapPos.x][mapPos.y]->myItem != nullptr) {
			if (itemButton->state == GuiControlState::DISABLED)itemButton->state = GuiControlState::NORMAL;

		}

		else {
			itemButton->state = GuiControlState::DISABLED;
		}
		curtains = false;
		//Expand tiles to available
		if (!ExpandedBFS) {


			if (myItem != nullptr) {
				app->map->pathfinding->GenerateWalkeableArea(tilePos, movement + myItem->GetMov());
			}
			else {

				app->map->pathfinding->GenerateWalkeableArea(tilePos, movement );
			}
			

			ExpandedBFS = true;
		}

		//move
		MovePath();
		
		break;
	case BATTLE:
		walkingEffect->active = false;
		app->battleScene->inBattle = true;


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

	if (hp >= 1) {
		if (movedThisTurn) {
			if (!hasWaited) {
				app->turnManager->availablePlayers--;
				hasWaited = true;
			}
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), flippedSprite, 120);
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), flippedSprite, 60, 1, 255, 0, 0, 0, 0, SDL_BLENDMODE_ADD);
		}
		else {
			
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), flippedSprite, 255);
			app->render->DrawTexture(myTexture, position.x, position.y, &currentAnim->GetCurrentFrame(), flippedSprite, 60, 1, 255, 0, 0, 0, 0, SDL_BLENDMODE_ADD);
		}
	}

	SDL_Rect randRect = SDL_Rect{ 8,0,32,32 };

	

	if (hp < 1 ) {
		if (!lastWords) {
			app->dialogueManager->SpontaneousDialogue(deathQuote);
			 /* Last word quote */
			
		}
		lastWords = true;
		if (pendingToDelete == false) {
			if (app->turnManager->currentPlayer != nullptr && app->turnManager->currentPlayer == this) {
				app->turnManager->DeSelectPlayer();
			}


			//save the player cause permadeath is off
			Player* copeDead = new Player(this);
			app->battleScene->deadPlayers.Add(copeDead); /*save a copy of the dead player*/

			app->turnManager->availablePlayers--;
			//app->battleScene->KillUnit(true, this); ESTO crashea y da muchisimos problemas
			pendingToDelete = true;
			app->entityManager->players.remove(this);
			for (ListItem<Player*>* p = app->turnManager->players.start; p != NULL; p = p->next) {
				if (p->data == this) {
					app->turnManager->players.Del(p);
				}
			}
			
		}
	}

	return true;
}

bool Player::PostUpdate() 
{

	if (hp >= 1) {

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
			missed = false;
			misses[0] = false; misses[1] = false;
			if (!defending) Bposition = iPoint(50 * 3, 80 * 3);
			
			break;
		case MOVE:
		{
			
			if (!defending) lerpingHp = hp; lerpingEXP = experiencePoints; Bposition = iPoint(50 * 3, 80 * 3);
			//Draw spotLight
			iPoint spotPos = iPoint(position.x + 16, position.y + 16);
		

			//app->render->CameraCenterOn(position.x -200, position.y -200);
			app->guiManager->spotLight->Target = spotPos;
			app->guiManager->TurnOnSpotLight();
			
			//Draw path
			if (app->turnManager->currentPlayer == this) 
			{
				
				if(myItem != nullptr)	personalPathfinding->GenerateWalkeableArea(tilePos, movement + myItem->GetMov());
				else { personalPathfinding->GenerateWalkeableArea(tilePos, movement); }
				
				
				personalPathfinding->DrawBFSPath();
				
			}
			
			// ! important, movement is the only item modifier that is added rather than multiplied
			if(myItem != nullptr) myFrame->Render(1.0 / 60.0, hp, attack * myItem->GetAtk(), speed * myItem->GetSpd(), precision * myItem->GetPrec(), luck * myItem->GetLck(), movement + myItem->GetMov(),0,0,0,0,0,experiencePoints);
			else { myFrame->Render(1.0 / 60.0, hp, attack, speed , precision , luck, movement , 0, 0, 0, 0, 0, experiencePoints); }


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
			itemButton->state = GuiControlState::DISABLED;


			if (curtainTimer.ReadSec() > 1) {


				app->guiManager->TurnOffSpotLight();

				if (battleTimer == 1 || battleTimer == 0)/*Determine amount of attacks*/ {

					if (speed > oponent->speed - 1) {

						int doubleChance = getRandomNumber(0, speed);
						if (doubleChance <= speed) /*Double attack*/ {

							numberofAttacks = 2;
							maxNumofATTACKS = numberofAttacks;
						}

					}
					for (int i = 0; i < numberofAttacks; ++i) {

						/* Determine evasion */
						int evasion = getRandomNumber(0, 100);
						if (evasion <= precision) {
							// the attach hits
						}
						else {
							misses[i] = true;
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

	}

	if (lerpedExp == false && oponent != nullptr) {

		

		bool finishedLERP = app->battleScene->DrawExpBar( lerpingEXP, experiencePoints + oponent->rewardEXP);
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
				receivedEXP = false;
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
			receivedEXP = false;
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
		if(myItem != nullptr) myFrame->Render(1.0 / 60.0, hp - hpMod, attack* myItem->GetAtk() -atkMod, speed* myItem->GetSpd() - spdMod, precision* myItem->GetPrec(), luck* myItem->GetLck() - luckMod, movement + myItem->GetMov(), hpMod, atkMod, spdMod, 0 , luckMod, experiencePoints);
		else { myFrame->Render(1.0 / 60.0, hp - hpMod, attack  - atkMod, speed  - spdMod, precision , luck  - luckMod, movement , hpMod, atkMod, spdMod, 0, luckMod, experiencePoints); }
		if ((app->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN || app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN )&& showLvlUp.ReadSec() > 1) {
			lvlMods = iPoint(-1, -1);
		}
	
	}

	if (atckedClicked ) {
		app->map->DrawAdjacents(tilePos, attackRange);
	}
	
	collider->data.x = (-40) + Bposition.x; collider->data.y = (0) + Bposition.y;
	if(oponent != nullptr){ oponent->collider->data.x = (-40) + oponent->Bposition.x; oponent->collider->data.y = (0) + oponent->Bposition.y; }
	if (app->battleScene->godMode) app->render->DrawRectangle(oponent->collider->data, b2Color(1, 1, 1, 1), false, false);

	HandleMiss();
	

	return true;
}

void Player::HandleMiss() {




		// Animation not finished? 
		if (!easing->GetFinished())
		{
			int a, b;
			EasingType easingT;

			if (bPause) {
				a = -300; b = -50;
				easingT = EasingType::EASE_IN_BACK;
			}
			else {
				a = -50;
				b = -300;
				easingT = EasingType::EASE_IN_BACK;
			}


			// TODO 1: Implement easings on pause menu
			// Calculate interpolated position (tracktime, easinganimaion),
			// and print to screen (DrawRectangle)
			double t = easing->TrackTime(26);
			double easedX = easing->EasingAnimation(a, b, t, easingT);

			SDL_Rect pauseBox = { easedX, 0, 300, 400 };
			missBox.y = easedX;
			/*app->render->DrawTexture(Curtain, easedX + (app->render->camera.x / -3), 0 + (app->render->camera.y / -3), &rightCurtain);*/

		}
		else if (bPause)
		{
			// static pause menu (animation finished, menu open)
			SDL_Rect pauseBox = { 250 , 0 , 300, 400 };

			missBox.y = -50;

			if (missed && missTimer.ReadMSec() >= 600 && missBoxDown == false) {
				missBoxDown = true;
				bPause = !bPause;
				easing->SetFinished(false);
			}
			/*app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &rightCurtain);*/

			/*app->render->DrawRectangle(pauseBox, 0, 255, 0, 255, false);
			app->render->DrawRectangle(pauseBox, 0, 255, 0, 64, true);*/
		}
		else {
			SDL_Rect pauseBox = { 460 , 0 , 300, 400 };
			missBox.y = -300;
			missBoxDown = false;
			missed = false;
			
			/*app->render->DrawTexture(Curtain, pauseBox.x + (app->render->camera.x / -3), pauseBox.y + (app->render->camera.y / -3), &rightCurtain);*/

		}

	

		app->render->DrawTexture(missText, (-app->render->camera.x / 3) + missBox.x, (-app->render->camera.y / 3) + missBox.y);

	

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

				int i = getRandomNumber(0, battleVoiceLines.Count() - 1);
				app->audio->PlayFx(battleVoiceLines[i]);

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

		if (opponentAttacking) {

			opponentReachTarget = true;
			opponentAttacking = false;
			if (oponent != nullptr && oponent->state == BATTLE )app->audio->PlayFx(oponent->hitFx);

		}
		else if (!opponentReachTarget) {
			if(state == BATTLE ) app->audio->PlayFx(hitFx);
		    reachedTarget = true;
		}
	}

}

/* when lerping hp finishes the number the turn siwtches to the opponent or the attack finishes*/
bool Player::DealDMG() {

	if ((numberofAttacks <= 0 && !missed) || oponent->hp < 1)/*finished attacks*/ {
		state = IDLE;
		movedThisTurn = true;
		app->battleScene->inBattle = false;
		HasAttackAction = false;
		HasMoveAction = false;
		reachedTarget = false;
		battleTimer = 1;
	}
	else if (!missed) {
		if (misses[numberofAttacks - 1] == false) {
			finishedLerp = app->battleScene->DrawHPBars(oponent->lerpingHp, oponent->hp - (int)(attack * TypeMultiplier(oponent->unitType)), lerpingHp, lerpingHp, oponent->maxHp, maxHp, true);
		
			if (finishedLerp || numberofAttacks < 0)/* enters when lerp has finished or no more attacks*/ {



				oponent->hp -= (int) (attack * TypeMultiplier(oponent->unitType) );
				oponent->lerpingHp = oponent->hp;
				reachedTarget = false;

				if ((numberofAttacks <= 0 && !missed) || oponent->hp < 1)/*finished attacks*/ {
					state = IDLE;
					movedThisTurn = true;
					app->battleScene->inBattle = false;
					HasAttackAction = false;
					HasMoveAction = false;
					reachedTarget = false;
					battleTimer = 1;
				}

				// measures if the target is in attacking range, tewk to enable toe double attack of the player
				int dist = app->map->DistanceBetweenTiles(tilePos, oponent->tilePos);

				if (numberofAttacks == maxNumofATTACKS && dist <= oponent->attackRange) opponentAttacking = true;
				else if (numberofAttacks == maxNumofATTACKS && dist > oponent->attackRange) {

					oponent->defending = false;
					opponentAttacking = false;
					opponentReachTarget = false;
					Bposition = iPoint(50 * 3, 80 * 3);
					reachedTarget = false;
					
				}
				return true;
			}
		
		}
			else if (missed== false)/*the attack dosn't hit*/ {

				reachedTarget = false;
				missed = true;
				missTimer.Start();
				bPause = !bPause;
				easing->SetFinished(false);

				// measures if the target is in attacking range, tewk to enable toe double attack of the player
				int dist = app->map->DistanceBetweenTiles(tilePos, oponent->tilePos);

				if (numberofAttacks == maxNumofATTACKS && dist <= oponent->attackRange) opponentAttacking = true;
				else if (numberofAttacks == maxNumofATTACKS && dist > oponent->attackRange) {

					
					oponent->defending = false;
					opponentAttacking = false;
					opponentReachTarget = false;
					Bposition = iPoint(50 * 3, 80 * 3);
					reachedTarget = false;
					
				}
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
		maxHp += 4;
		hp = maxHp;
		
		break;
	case 1:
		attack += 2;
		break;
	}

	app->battleScene->StartRossing();
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

	if (numberofAttacks <= 0 && lerpedExp == true && !missed) {
		state = IDLE;
		movedThisTurn = true;
		app->battleScene->inBattle = false;
		curtains = false;
		numberofAttacks = 1;
		missed = false;
		HasAttackAction = false;
		HasMoveAction = false;
		reachedTarget = false;
		battleTimer = 0;
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

		if (opponentAttacking && !missed) {
			oponent->defending = true;
			oponent->FigureStickMovement(16.0f);
		}

		if (opponentReachTarget) {

			bool opLerp = false;

			opLerp = app->battleScene->DrawHPBars(oponent->lerpingHp, oponent->lerpingHp, lerpingHp, hp - oponent->attack * TypeMultiplier(oponent->unitType), oponent->maxHp, maxHp, true);

			if (opLerp) /*hp bar lerping has finished*/ {

				hp -= (oponent->attack * TypeMultiplier(oponent->unitType));

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
		 HasMoveAction = true;

	}


	if (control->id == waitBtnId && app->turnManager->currentPlayer == this /*&& app->turnManager->isPlayerMoving == false*/) {
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

	if (control->id == itemBtnId && app->turnManager->currentPlayer == this) {

		// Ability to pick up item
		iPoint mapPos = app->map->WorldToMap(position.x, position.y);

		if (app->map->myTiles[mapPos.x][mapPos.y]->myItem != nullptr && state == MOVE)/* If the tile i'm on has an item */ {

				app->audio->PlayFx(pickItemFx);

				app->map->myTiles[mapPos.x][mapPos.y]->myItem->overworld = false;

				app->inventory->InventoryItems.Add(app->map->myTiles[mapPos.x][mapPos.y]->myItem);

				app->questManager->CheckQuestCompletion(app->map->myTiles[mapPos.x][mapPos.y]->myItem->name);
				app->map->myTiles[mapPos.x][mapPos.y]->myItem = nullptr;
			
		}
	}
	return true;
}