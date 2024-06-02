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
#include "Door.h"


using namespace std;

ItemManager::ItemManager(bool isActive) : Module(isActive)
{
	name.Create("itemmanager");
	active = isActive;

}

bool ItemManager::Start() {

	// Call start for all items
	
 	return true;

}

void ItemManager::Init() {

	if (active) {

		for (pugi::xml_node mN = config.child("map"); mN != NULL; mN = mN.next_sibling("map")) {


			int n = mN.attribute("value").as_int();
			if (n == app->map->level) {
				mapNode = mN;
				break;
			}


		}



		for (pugi::xml_node n = mapNode.child("item"); n != NULL; n = n.next_sibling("item")) {

			Item* item = AddItem();
			string s = n.attribute("type").as_string();
			if (s == "Z") {
				item->myType = CONSUMABLE;
			}
			else {
				item->myType = EQUIPABLE;
			}


			item->InitModifiers(n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("hp").as_float(), n.attribute("attack").as_float(), n.attribute("luck").as_float(),
				n.attribute("precision").as_float(), n.attribute("evasion").as_float(), n.attribute("speed").as_float(), n.attribute("movement").as_int(), (const char*)n.attribute("name").as_string(),
				n.attribute("textPath").as_string(), n.attribute("description").as_string());
			// assign item to the tile
			app->map->myTiles[app->map->WorldToMap(item->mapPos.x, item->mapPos.y).x][app->map->WorldToMap(item->mapPos.x, item->mapPos.y).y]->myItem = item;
		}

		for (pugi::xml_node n = mapNode.child("door"); n != NULL; n = n.next_sibling("door")) {

			Door* door = AddDoor();
			door->InitModifiers(n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("num").as_int(),
				n.attribute("textPath").as_string());
			// assign item to the tile
			app->map->myTiles[app->map->WorldToMap(door->mapPos.x, door->mapPos.y).x][app->map->WorldToMap(door->mapPos.x, door->mapPos.y).y]->myDoor = door;
		}
		for (pugi::xml_node n = mapNode.child("lever"); n != NULL; n = n.next_sibling("lever")) {

			Lever* lever = AddLever();
			lever->InitModifiers(n.attribute("x").as_int(), n.attribute("y").as_int(), n.attribute("num").as_int(),
				n.attribute("textPath").as_string());
			// assign item to the tile
			app->map->myTiles[app->map->WorldToMap(lever->mapPos.x, lever->mapPos.y).x][app->map->WorldToMap(lever->mapPos.x, lever->mapPos.y).y]->myLever = lever;
		}
	}


}

bool ItemManager::Awake(pugi::xml_node config) {

	this->config = config;
	myNode = config;

	

	return true;

}

Item* ItemManager::AddItem() {

	Item* item = new Item();

	items.Add(item);
	return item;

}

Door* ItemManager::AddDoor() {

	Door* door = new Door();

	doors.Add(door);
	return door;

}

Lever* ItemManager::AddLever() {

	Lever* lever = new Lever();

	levers.Add(lever);
	return lever;

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

	for (ListItem<Door*>* it = doors.start; it != nullptr; it = it->next) {

		Door* _door = it->data;
		_door->Update(dt);

	}

	for (ListItem<Lever*>* it = levers.start; it != nullptr; it = it->next) {

		Lever* _lever = it->data;
		_lever->Update(dt);

	}

	return true;
}

bool ItemManager::PostUpdate() {


	// Update all items
	for (ListItem<Item*>* it = items.start; it != nullptr; it = it->next) {

		Item* _item = it->data;
		_item->PostUpdate();

	}

	for (ListItem<Door*>* it = doors.start; it != nullptr; it = it->next) {

		Door* _door = it->data;
		_door->PostUpdate();

	}

	for (ListItem<Lever*>* it = levers.start; it != nullptr; it = it->next) {

		Lever* _lever = it->data;
		_lever->PostUpdate();

	}
	return true;
}


bool ItemManager::CleanUp() {

	// Delet all items
  	items.Clear();
	doors.Clear();
	levers.Clear();

	return true;

}

void ItemManager::DestroyItem(Item* item) {

	ListItem<Item*>* it;

	for (it = items.start; it != NULL; it = it->next)
	{
		if (it->data == item) 
		{ 
			
			items.Del(it); 
		}
	}
}
