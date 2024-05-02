#include "TurnManager.h"
#include "../Player.h"
#include "../Item.h"
#include "../App.h"
#include "../Textures.h"
#include "../Scene.h"
#include <string>
#include "../Defs.h"
#include "../Log.h"
#include "../EntityManager.h"
#include "../Entity.h"
#include"../BattleScene.h"
#include "DialogueManager.h"
#include "../GuiManager.h"

using namespace std;

TurnManager::TurnManager(bool isActive) : Module(isActive)
{
	name.Create("turnmanager");
}

// Destructor
TurnManager::~TurnManager()
{}

// Called before render is available
bool TurnManager::Awake(pugi::xml_node config)
{
	LOG("Loading Turn Manager");
	bool ret = true;




	return ret;

}

bool TurnManager::Start() {

	bool ret = true;
	availablePlayers = 3;
	enemyTurnFinished = false;
	noEnemyMoving = true;


	return ret;
}

// Called before quitting
bool TurnManager::CleanUp()
{
	bool ret = true;
	// clear everything

	return ret;
}


bool TurnManager::PreUpdate()
{
	bool ret = true;

	return ret;
}

bool TurnManager::Update(float dt)
{
	bool ret = true;

	// my selected player has moved
	if (currentPlayer != nullptr && currentPlayer->state == IDLE) {

		
		currentPlayer->movedThisTurn = true;
		availablePlayers--;
		currentPlayer = nullptr; // CHANGE THIS TO A FIGHTING STATE
		/*Turn off spotLight*/
		app->guiManager->TurnOffSpotLight();
	}
	
	if (availablePlayers <= 0) {
		// enemy turn
		LOG("All players have moved, initiating enemy turn");
		currentTurn = ENEMY;
		// if enemy turn has finished and there is no enemy moving
		if (noEnemyMoving)
			for (int i = 0; i < app->entityManager->enemies.Count(); ++i)
			{
				enemyTurnFinished = true;
				if (app->entityManager->enemies.At(i)->data->HasMoveAction == true)
				{

					app->entityManager->enemies.At(i)->data->entityTurn = true;
					app->entityManager->enemies.At(i)->data->target = nullptr;
					app->entityManager->enemies.At(i)->data->battleTimer = 0;
					enemyTurnFinished = false;
					noEnemyMoving = false;
					break;
				}
			}

		CheckBattleEnding();
		if (enemyTurnFinished) {

			for (int i = 0; i < app->entityManager->enemies.Count(); ++i)
			{
				app->entityManager->enemies.At(i)->data->HasMoveAction = true;
				app->entityManager->enemies.At(i)->data->HasAttackAction = true;
				app->entityManager->enemies.At(i)->data->playerNear = false;
			}

			PlayerTurn();
			
		}
	}




	return ret;
}
bool TurnManager::PostUpdate()
{
	bool ret = true;
	

	return ret;
}

void TurnManager::SelectPlayer(Player* player) {

	// should also check if the player has already moved this turn
	if (player != nullptr) {

		currentPlayer = player;
	}
}

void TurnManager::DeSelectPlayer() {

	if (currentPlayer != nullptr) {
		currentPlayer = nullptr;
	}

}

void TurnManager::InitializeChessPieces(List<Player*>* players, List<Enemy*>* enemies) {
	
	this->players = *players;
	this->enemies = *enemies;
	maxEnemies = enemies->Count();
}

bool TurnManager::EnemyTurn() {

	LOG("insert enemy turn");
	currentTurn = ENEMY;
	bool finished = true;







	// Update enemies - comented for now

	//if (enemies[enemyIndex]->state != MOVE) enemies[enemyIndex]->state = MOVE;





	//ListItem<Enemy*>* it = enemies.start;
	//while (it != NULL) {
	//	if (it->data->movedThisTurn == false) {
	//		// don't end the turn unless all enemies have acted
	//		finished = false;
	//	}
	//	it = it->next;
	//}




	return finished;
}
void TurnManager::PlayerTurn() {
	LOG("Starting player turn");
	currentTurn = PLAYER;


	availablePlayers = 0;

	for (ListItem<Player*>* pItem = players.start; pItem != nullptr; pItem = pItem->next) {

		if (pItem->data->hp > 0) {
			availablePlayers++;
		}

	}

	/*ListItem<Player*>* p = players.start;

	Player* pEntity = p->data;
	while (pEntity != nullptr) {
		pEntity->movedThisTurn = false;
		pEntity->battleTimer = 0;
		pEntity->state = IDLE;

		p = p->next;
		pEntity = p->data;
	}*/

	for (ListItem<Player*>* pEntity = players.start; pEntity != nullptr; pEntity = pEntity->next) {

		if (pEntity->data == nullptr || pEntity->data == NULL) {
			LOG("uuuuy");
		}

		LOG("Reactivating %s", pEntity->data->name);
	}
	for (ListItem<Player*>* pEntity = players.start; pEntity != nullptr; pEntity = pEntity->next) {

		pEntity->data->movedThisTurn = false;
		pEntity->data->battleTimer = 0;
		pEntity->data->state = IDLE;
		pEntity->data->HasMoveAction = true;

		LOG("Reactivating %s", pEntity->data->name);
	}


}

void TurnManager::CheckBattleEnding() {

	bool playerLoss = true;
	bool enemyLoss = true;

	ListItem<Player*>* p = players.start;
	while (p != NULL) {
		
		if (p->data->hp > 0) {
			playerLoss = false;
		}

		p = p->next;
	}
	ListItem<Enemy*>* e = enemies.start;
	while (e != NULL) {

		if (e->data->hp > 0) {
			enemyLoss = false;
		}

		e = e->next;
	}

	/*if (enemies.Count() == 0) { 
		enemyLoss = true; 
	}*/

	if (playerLoss || enemyLoss || MainQuest)/*Split into restart & next battle outcomes*/ {
		// Restart the fight
		app->battleScene->CleanUp();

		app->dialogueManager->NextAct();
		app->dialogueManager->Disable();
		app->dialogueManager->CleanUp();

		
	
		app->dialogueManager->Enable();
		app->dialogueManager->myState = CUTSCENE;

		app->battleScene->Start();
		Start();
		// Instanciate retry buttons
	}

}