#ifndef __TURNMANAGER_H__
#define __TURNMANAGER_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../Player.h"
#include "../Enemy.h"

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

	Turn currentTurn = PLAYER;

	Player* currentPlayer = nullptr;
	bool noEnemyMoving = true;
private:

	List<Player*> players;


	List<Enemy*> enemies;
	Entity* currentenemy = nullptr;

	uint availablePlayers = 3;

	bool enemyTurnFinished;

	uint enemyIndex = 0;
	uint maxEnemies;

};

#endif // __ENTITYMANAGER_H__
