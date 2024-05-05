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

ItemManager::ItemManager(bool isActive) : Module(isActive)
{
	name.Create("itemmanager");

}

bool ItemManager::Start() {

	// Call start for all items

	for (pugi::xml_node n = config.child("item"); n != NULL; n = n.next_sibling("item")) {

		Item* item = AddItem();
		item->InitModifiers(n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("hp").as_float(), n.attribute("attack").as_float(), n.attribute("luck").as_float(),
			n.attribute("precision").as_float(), n.attribute("evasion").as_float(), n.attribute("speed").as_float(), n.attribute("movement").as_int(), (const char*)n.attribute("name").as_string(),
			n.attribute("textPath").as_string());
		// assign item to the tile
		app->map->myTiles[app->map->WorldToMap( item->mapPos.x, item->mapPos.y).x][app->map->WorldToMap(item->mapPos.x, item->mapPos.y).y]->myItem = item;
	}
	

	return true;

}

bool ItemManager::Awake(pugi::xml_node config) {

	this->config = config;

	return true;

}

Item* ItemManager::AddItem() {

	Item* item = new Item();

	items.Add(item);
	return item;

}

bool ItemManager::PreUpdate() {


	// Update all items


	return true;
}




bool ItemManager::Update(float dt) {


	// Update all items
	for (ListItem<Item*>* it = items.start; it != nullptr; it = it->next) {

		Item* _item = it->data;
		_item->Update(dt);

	}

	return true;
}

bool ItemManager::PostUpdate() {


	// Update all items
	for (ListItem<Item*>* it = items.start; it != nullptr; it = it->next) {

		Item* _item = it->data;
		_item->PostUpdate();

	}


	return true;
}


bool ItemManager::CleanUp() {

	// Delet all items

	return true;

}

