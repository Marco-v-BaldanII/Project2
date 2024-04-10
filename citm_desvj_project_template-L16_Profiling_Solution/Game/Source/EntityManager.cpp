#include "EntityManager.h"
#include "Player.h"
#include "Item.h"
#include "../NPC.h"
#include "../Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include <string>
#include "Defs.h"
#include "Map.h"
#include "Log.h"
#include "../BackstagePlayer.h"

using namespace std;

EntityManager::EntityManager(bool isActive) : Module(isActive)
{
	name.Create("entitymanager");
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	// NPCs
	for (pugi::xml_node npcNode = config.child("npc"); npcNode != NULL; npcNode = npcNode.next_sibling("npc")) {

		Npc* dude = (Npc*) PlaceNPC(npcNode.attribute("name").as_string(), npcNode.attribute("x").as_int(), npcNode.attribute("wait").as_int());
		

		for (pugi::xml_node dialogueNode = npcNode.child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {
			// Create all the dialogues for the npc
			string text = dialogueNode.attribute("text").as_string();
			Dialogue* di = new Dialogue(npcNode.attribute("name").as_string(), text);

			dude->myDialogues.PushBack(di);
		}
	}



	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	//L03: DONE 3a: Instantiate entity according to the type and add the new entity to the list of Entities
	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::ENEMY:
		entity = new Enemy();
		break;
	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) entities.Del(item);
	}
}

Entity* EntityManager::PlaceNPC(string name, int x, int wait) {

	Entity* entity = new Npc(name, x , wait);
	entity->position.x = x;
	entities.Add(entity);

	return entity;
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

bool EntityManager::PreUpdate()
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->PreUpdate();
	}


	return ret;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}


	return ret;
}
bool EntityManager::PostUpdate()
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->PostUpdate();
	}


	return ret;
}

p2ListItem<Entity*>* EntityManager::GetNearestPlayer(Entity* player)
{
	p2ListItem<Entity*>* ret = nullptr;
	p2ListItem<Entity*>* uPlayer = players.getFirst();

	if (uPlayer != NULL)
	{
		int closest = player->pathfinding->CreatePath(player->tilePos, uPlayer->data->tilePos);
		ret = uPlayer;
		while (uPlayer != nullptr)
		{
			int temp = player->pathfinding->CreatePath(player->tilePos, uPlayer->data->tilePos);

			if (temp < closest)
			{
				closest = temp;
				ret = uPlayer;
			}

			uPlayer = uPlayer->next;
		}
		player->pathfinding->ClearPath();
		return ret;

	}
	else return ret;

}