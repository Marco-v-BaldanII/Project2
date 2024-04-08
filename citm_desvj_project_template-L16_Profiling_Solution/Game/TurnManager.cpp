#include "TurnManager.h"
#include "../Player.h"
#include "../Item.h"
#include "../App.h"
#include "../Textures.h"
#include "../Scene.h"
#include <string>
#include "../Defs.h"
#include "../Log.h"

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