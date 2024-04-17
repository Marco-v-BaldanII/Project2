#ifndef __ITEMMANAGER_H__
#define __ITEMMANAGER_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../p2List.h"
#include "../Player.h"

using namespace std;

enum ItemTypes {

	EQUIPABLE,
	CONSUMABLE

};


class ItemManager : public Module
{
public:

	ItemManager(bool isActive);

	// Destructor
	virtual ~ItemManager();

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

	// Additional methods

	void AddItem();
	

public:



};

#endif // __ENTITYMANAGER_H__
