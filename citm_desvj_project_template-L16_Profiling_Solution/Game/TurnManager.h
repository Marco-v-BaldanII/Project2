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

	void InitializeChessPieces(List<Player*>* players, List<Enemy*>* enemies);

	bool EnemyTurn();

	void PlayerTurn();

private:

	List<Player*> players;
	Player* currentPlayer = nullptr;

	List<Enemy*> enemies;
	Enemy* currentenemy = nullptr;

	uint availablePlayers = 3;

	Turn currentTurn = PLAYER;

	uint enemyIndex = 0;
	uint maxEnemies;

};

#endif // __ENTITYMANAGER_H__
