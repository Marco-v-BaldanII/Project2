#include "Lever.h"
#include "../App.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Input.h"
#include "../Render.h"
#include "../Scene.h"
#include "../Log.h"
#include "../Point.h"
#include "../Physics.h"

Lever::Lever()
{
	//name.Create("item");
}

Lever::~Lever() {}

bool Lever::Awake() {

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();*/

	return true;
}

bool Lever::Start() {

	//initilize textures
	//texture = app->tex->Load(texturePath);

	return true;
}

bool Lever::Update(float dt)
{
	//app->render->DrawTexture(texture, position.x, position.y);


	return true;
}

bool Lever::PostUpdate() {


	if (isPressed == false)
	app->render->DrawTexture(texture, mapPos.x, mapPos.y);
	


	return true;
}

void Lever::InitModifiers(int x, int y, int num, const char* textPath) {


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

bool Lever::CleanUp()
{
	return true;
}