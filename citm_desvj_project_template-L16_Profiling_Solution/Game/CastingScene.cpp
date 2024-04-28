#ifndef __CASTINGSCENE_H__
#define __CASTINGSCENE_H__

#include "CastingScene.h"
#include "../Textures.h"
#include "../App.h"
#include "../Render.h"
#include "../Textures.h"
#include "../Defs.h"
#include "../Log.h"



CastingScene::CastingScene(bool isActive) : Module(isActive)
{
	name.Create("castingScene");
}

CastingScene::~CastingScene()
{

}


bool CastingScene::Awake(pugi::xml_node config) {

	this->config = config;


	return true;
}

bool CastingScene::Start() {


	// Load all the actors from the config
	for (pugi::xml_node ActorNode = config.child("actor"); ActorNode != NULL; ActorNode = ActorNode.next_sibling("actor")) {

		Actor* actor = new Actor(ActorNode.attribute("name").as_string(), ActorNode.attribute("dialogue").as_string());

		actor->texture = app->tex->Load(ActorNode.attribute("path").as_string());

		actors.PushBack(actor);
	}

	for (pugi::xml_node ActorNode = config.child("role"); ActorNode != NULL; ActorNode = ActorNode.next_sibling("role")) {

		string role = ActorNode.attribute("text").as_string();

		roles.PushBack(role);
	}

	background = app->tex->Load("Assets/Textures/BattleStageOG.png");

	return true;
}

bool CastingScene::PreUpdate() {

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN) {

		roleIndex++;

	}

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN) {

		roleIndex--;
		if (roleIndex < 0) {
			roleIndex = roles.Count() - 1;
		}
	}

	int result = roleIndex % roles.Count();
	roleIndex = abs(result);
	return true;
}

bool CastingScene::Update(float dt) {

	



	return true;
}

bool CastingScene::PostUpdate() {


	SDL_Rect screen = SDL_Rect{ 0,0,256*2,198*2 };
	app->render->DrawTexture(background, 0,0, &screen);

	// role
	app->render->DrawText(roles[roleIndex], 128 * 3, 48 * 3, 370 * 3, 20 * 3, true);

	// actor



	app->render->DrawText(actors[actorIndex]->name, 16 * 3, 192 * 3, 370 * 3, 20 * 3, true);
	app->render->DrawTexture(actors[actorIndex]->texture, 32 , 224 , &portraitBox);


	return true;
}

bool CastingScene::CleanUp() {


	return true;
}


#endif // __CASTINGSCENE_H__