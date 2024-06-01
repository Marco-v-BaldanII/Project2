#ifndef __TURNMANAGER_H__
#define __TURNMANAGER_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../Player.h"
#include "../Enemy.h"
#include "../GuiManager.h"

using namespace std;

enum Turn {
	PLAYER,
	ENEMY,
	DIALOGUE
};

class TurnManager : public Module
{
public:

	TurnManager(bool isActive);

	// Destructor
	virtual ~TurnManager();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();
	bool PreUpdate() override;
	// Called every frame
	bool Update(float dt);
	bool PostUpdate() override;
	// Called before quitting
	bool CleanUp();

	void SelectPlayer(Player* player);

	void DeSelectPlayer();

	void InitializeChessPieces(List<Player*>* players, List<Enemy*>* enemies);

	bool EnemyTurn();

	void PlayerTurn();

	void CheckBattleEnding();


	Turn currentTurn = PLAYER;

	Player* currentPlayer = nullptr;
	int availablePlayers = 3;
	bool noEnemyMoving = true;
	bool isPlayerMoving = false;
public:

	List<Player*> players;


	List<Enemy*> enemies;
	Entity* currentenemy = nullptr;

	EasingText* playerTurnPopUp = nullptr;
	EasingText* enemyTurnPopUp = nullptr;

	bool enemySign = false;

	bool enemyTurnFinished;

	uint enemyIndex = 0;
	uint maxEnemies;

	bool playerLoss = true;
	bool enemyLoss = true;

	bool MainQuest = false;

};

#endif // __TURNMANAGER_H__
