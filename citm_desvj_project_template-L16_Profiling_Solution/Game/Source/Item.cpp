#include "Item.h"
#include "App.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Point.h"
#include "Physics.h"

Item::Item() 
{
	//name.Create("item");
}

Item::Item(Item* it) {

	isPicked = true;

	this->myState = it->myState;
	this->mapPos =  it->mapPos;
	this->name =    it->name;
	this->myUnit =  it->myUnit;
	this->texture = it->texture;

	this->texturePath = it->texturePath;

	hpMod =   it->hpMod;
	atkMod =  it->atkMod;
	LckMod =  it->LckMod;
	PrecMod = it->PrecMod;
	EvsMod =  it->EvsMod;
	spdMod =  it->spdMod;
	movMod =  it->movMod;

	texW = it->texW;
	texH = it->texH;

}

Item::~Item() {

	delete name;
	app->tex->UnLoad(texture);

}

bool Item::Awake() {

	/*position.x = parameters.attribute("x").as_int();
	position.y = parameters.attribute("y").as_int();
	texturePath = parameters.attribute("texturepath").as_string();*/

	return true;
}

bool Item::Start() {

	//initilize textures
	//texture = app->tex->Load(texturePath);

	return true;
}

bool Item::Update(float dt)
{
	//app->render->DrawTexture(texture, position.x, position.y);

	return true;
}

bool Item::PostUpdate() {


	if (overworld)/*If i am in the overworld*/ {
		app->render->DrawTexture(texture, mapPos.x, mapPos.y);

	}
	
	return true;
}

void Item::InitModifiers(int x, int y, float hpMod, float atkMod, float LckMod, float PrecMod, float EvsMod, float SpdMod, int movMod, const char* name, const char* textPath) {

	if(hpMod != 0) this->hpMod = hpMod;
	if(atkMod != 0) this->atkMod = atkMod;
	if(LckMod != 0) this->LckMod = LckMod;
	if(PrecMod != 0) this->PrecMod = PrecMod;
	if(EvsMod != 0) this->EvsMod = EvsMod;
	if(SpdMod != 0) this->spdMod = SpdMod;
	if(movMod != 0) this->movMod = movMod;
	this->name = name;
	mapPos.x = x; mapPos.y = y;
	mapPos =  app->map->MapToWorld(mapPos.x, mapPos.y);

	texture = app->tex->Load(textPath);

}

bool Item::CleanUp()
{
	return true;
}