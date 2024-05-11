#include "Door.h"
#include "../App.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Input.h"
#include "../Render.h"
#include "../Scene.h"
#include "../Log.h"
#include "../Point.h"
#include "../Physics.h"

Door::Door()
{
	//name.Create("item");
}

Door::~Door() {}

bool Door::Awake() {

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();*/

	return true;
}

bool Door::Start() {

	//initilize textures
	//texture = app->tex->Load(texturePath);

	return true;
}

bool Door::Update(float dt)
{
	//app->render->DrawTexture(texture, position.x, position.y);


	return true;
}

bool Door::PostUpdate() {


	if (isOpen == false)
	app->render->DrawTexture(texture, mapPos.x, mapPos.y);


	return true;
}

void Door::InitModifiers(int x, int y, int num, const char* textPath) {


	this->num = num;
	mapPos.x = x; mapPos.y = y;
	mapPos = app->map->MapToWorld(mapPos.x, mapPos.y);

	texture = app->tex->Load(textPath);
	

	iPoint pos;
	pos.x = mapPos.x;
	pos.y = mapPos.y;
	pos = app->map->WorldToMap(pos.x, pos.y);

	tilePos = pos;

}

bool Door::CleanUp()
{
	return true;
}