#ifndef __ENTITY_H__
#define __ENTITY_H__


#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Pathfinding.h"
#include "Render.h"
#include "Animation.h"
#include "Map.h"

class Frame;

enum class EntityType
{
	PLAYER,
	ITEM,
	ENEMY,
	NPC,
	BACKSTAGEPLAYER,
	UNKNOWN
};

enum States {
	MOVE,
	IDLE,
	DEAD,
	BATTLE,
	DISABLED
};

class PhysBody;

enum UnitType {

	PALADIN,
	ARCHER,
	KNIGHT,
	ARMOURED_KNIGHT,
	MAGE,
	DARK_MAGE


};

class Entity
{
public:

	Entity(EntityType type) : type(type), active(true) {}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}
	virtual bool PreUpdate() 
	{
		return true;
	}

	virtual bool Update(float dt)
	{
		return true;
	}
	virtual bool PostUpdate() 
	{
		return true;
	}


	virtual bool CleanUp()
	{
		return true;
	}

	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	void Entity::Enable()
	{
		if (!active)
		{
			active = true;
			Start();
		}
	}

	void Entity::Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	virtual void OnCollision(PhysBody* physA, PhysBody* physB) {

	};

	bool MovePath();

	bool InitPath(iPoint destiantion);

	void AssocateEntity_Tile(iPoint position);


	void InitializeStats(pugi::xml_node config);


public:

	UnitType unitType;

	iPoint* currentP;
	iPoint* nextP;
	iPoint* direction;
	//counts the tiles advanced in combat mode
	int stepCounter = 0;
	int moveRange;
	int attackRange;
	bool Move;
	bool nextStep = true;
	float moveTime = 32;
	float counter = 0;
	States state;

	Frame* myFrame = nullptr;

	bool HasAttackAction = false;
	bool HasMoveAction = false;

	iPoint tilePos;
	bool ExpandedBFS;
	bool ConfirmMovement;

	SString name;
	PathFinding* pathfinding;
	EntityType type;
	bool active = true;
	pugi::xml_node parameters;
	Entity* entity;
	Entity* target;
	bool entityTurn = false;
	SDL_Texture* battleBg = NULL;
	SDL_Texture* myBattleTexture = NULL;
	// Possible properties, it depends on how generic we
	// want our Entity class, maybe it's not renderable...
	iPoint position;

	Tile* currentTile = nullptr;

	float attack;
	float hp;
	float precision;
	float luck;
	int speed;
	float movement;

	Animation* currentAnim;
	Animation rightAnim;
	Animation upAnim;
	Animation downAnim;


	float battleTimer;

	int currentHP;
	int maxHp;

	float lerpingHp;

	Item* myItem = nullptr;

	bool renderable = true;
};

#endif // __ENTITY_H__