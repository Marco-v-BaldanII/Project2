#ifndef __ITEMMANAGER_H__
#define __ITEMMANAGER_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../p2List.h"
#include "../Player.h"
#include "../Item.h"
#include "Door.h"
#include "Lever.h"

using namespace std;

enum ItemTypes {

	EQUIPABLE,
	CONSUMABLE

};

class Item;

class ItemManager : public Module
{
public:

	ItemManager(bool isActive);

	// Destructor
	

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

	void DestroyItem(Item* item);

	// Additional methods

	Item* AddItem();

	Door* AddDoor();

	Lever* AddLever();

	pugi::xml_node myNode;
	
	pugi::xml_node config;
	List<Item*> items;
	List<Door*> doors;
	List<Lever*> levers;

private:
	bool Active = false;
	

};

#endif // __ENTITYMANAGER_H__
