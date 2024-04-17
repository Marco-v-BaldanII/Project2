#include "ItemManager.h"
#include "../Player.h"
#include "../Item.h"
#include "NPC.h"
#include "../Enemy.h"
#include "../App.h"
#include "../Textures.h"
#include "../Scene.h"
#include <string>
#include "../Defs.h"
#include "../Map.h"
#include "../Log.h"


using namespace std;


bool ItemManager::Start() {

	// Call start for all items

	return true;

}

bool ItemManager::Awake(pugi::xml_node config) {

	// Call awake for all items
	return true;

}


bool ItemManager::Update(float dt) {


	// Update all items


	return true;
}



bool ItemManager::CleanUp() {

	// Delet all items

	return true;

}