#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"
#include <string>
#include "p2List.h"
#include "Player.h"

using namespace std;

class EntityManager : public Module
{
public:

	EntityManager(bool isActive);

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();
	bool PreUpdate() override;
	// Called every frame
	bool Update(float dt);
	bool PostUpdate() override;

	bool SuperPostUpdate() override;
	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

	Entity* PlaceNPC(string name, int x, int y, int wait);

	ListItem<Entity*>* IsEnemyThere(const iPoint& pos);

	p2ListItem<Entity*>* GetNearestPlayer(Entity* player);

public:

	List<Entity*> entities;
	List<Entity*> enemies;
	p2List<Entity*> players;

};

#endif // __ENTITYMANAGER_H__
