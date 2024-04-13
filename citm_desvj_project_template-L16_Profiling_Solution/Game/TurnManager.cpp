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
	}
	
	if (availablePlayers <= 0) {
		// enemy turn
		LOG("All players have moved, initiating enemy turn");
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


		if (enemyTurnFinished) {

			for (int i = 0; i < app->entityManager->enemies.Count(); ++i)
			{
				app->entityManager->enemies.At(i)->data->HasMoveAction = true;
				app->entityManager->enemies.At(i)->data->HasAttackAction = true;
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

	availablePlayers = players.Count();

	ListItem<Player*>* p = players.start;
	while (p != NULL) {
		p->data->movedThisTurn = false;
		p->data->state = IDLE;

		p = p->next;
	}



}