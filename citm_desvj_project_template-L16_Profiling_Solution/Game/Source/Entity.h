#ifndef __ENTITY_H__
#define __ENTITY_H__


#include "Point.h"
#include "SString.h"
#include "Input.h"
#include "Pathfinding.h"
#include "Render.h"
#include "Animation.h"
#include "Map.h"
#include "Timer.h"

class Frame;
class Dialogue;
class Collider;

enum class EntityType
{
	PLAYER,
	ITEM,
	ENEMY,
	NPC,
	BACKSTAGEPLAYER,
	DOOR,
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
class Timer;
class Easing;
class ParticleEffect;
class Particle;

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

	virtual bool SuperPostUpdate() {

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

	inline virtual bool OnGuiMouseClickEvent(GuiControl* control) { return true; }

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

	virtual void OnCollision(Collider* physA, Collider* physB) {

	};

	bool MovePath();


	bool InitPath(iPoint destiantion);

	void AssocateEntity_Tile(iPoint position);


	void InitializeStats(pugi::xml_node config, bool nerfed);

	double curvedTrajectory(double x, double amplitude, double frequency, double damping, double phase_shift) {
		return amplitude * sin(frequency * x + phase_shift) * exp(-damping * x);
	}


	virtual void FigureStickMovement(float dt) {};

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

	bool playerNear;

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
	SDL_Texture* battleBg = nullptr;
	SDL_Texture* myBattleTexture = nullptr;
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


	float battleTimer = 0;

	int currentHP;
	int maxHp;

	float lerpingHp;

	Item* myItem = nullptr;

	bool renderable = true;

	bool pendingToDelete = false;

	bool lastWords = false;
	Dialogue* deathQuote = nullptr;
	Dialogue* lvlUpQuote = nullptr;

	iPoint Bposition = iPoint(250 * 3, 100 * 3); /* starting pos of attack */

	iPoint velocity = iPoint(-24, 10);

	Collider* collider = nullptr;
	bool BtlMoving = false;
	bool reachedTarget = false;

	int numberofAttacks = 1;
	int maxNumofATTACKS = 1;
	bool misses[2] = { false,false };
	bool missed = false;
	Timer missTimer;
	bool missBoxDown = false;


	Easing* easing = nullptr;
	bool bPause = false;

	iPoint missBox = iPoint(300, -300);

	bool defending = false; /* opponent is touching its attributes for it to defend its attack*/

	uint level = 1;

	int experiencePoints = 0;
	int rewardEXP = 20;

	bool lerpedExp = true;
	bool lvlUp = false;

	Entity* oponent;
	bool opponentAttacking = false;

	Timer curtainTimer;
	bool curtains = false;

	float TypeMultiplier(UnitType rivalType);
	uint hitFx;

	bool onStage = false; /*If the entity is in a battleScene*/

	protected:
		ParticleEffect* walkingEffect;
		Particle* walkingParticle;

};

#endif // __ENTITY_H__