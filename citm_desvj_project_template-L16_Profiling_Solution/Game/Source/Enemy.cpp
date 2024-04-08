#include "Enemy.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Enemy::Enemy() : Entity(EntityType::ENEMY)
{
	name.Create("enemy");
}

Enemy::~Enemy() {}

bool Enemy::Awake() {

	position = iPoint(config.attribute("x").as_int(), config.attribute("y").as_int());
	name = config.attribute("name").as_string();
	state = IDLE;

	return true;
}

bool Enemy::Start() {

	//initilize textures


	return true;
}

bool Enemy::Update(float dt)
{

	switch (state)
	{
	case IDLE:
		//update idle
		break;
	case MOVE:

		//Expand tiles to available
		if (!ExpandedBFS) {



			app->map->pathfinding->GenerateWalkeableArea(tilePos, 3);

			ExpandedBFS = true;
		}

		//move
		MovePath();


		break;
	}

	return true;
}


bool Enemy::CleanUp()
{
	return true;
}

bool Enemy::PostUpdate() {

	app->render->DrawTexture(texture, position.x, position.y, &section);

	return true;
}