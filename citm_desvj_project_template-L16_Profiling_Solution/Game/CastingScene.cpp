#ifndef __CASTINGSCENE_H__
#define __CASTINGSCENE_H__

#include "CastingScene.h"
#include "../Textures.h"
#include "../App.h"
#include "../Render.h"
#include "../Textures.h"
#include "../Defs.h"
#include "../Log.h"
#include "DialogueManager.h"
#include "../LevelManagement.h"


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

	// Load the portrait box
	if (active)
	{
		// Load all the actors from the config
		for (pugi::xml_node ActorNode = config.child("actor"); ActorNode != NULL; ActorNode = ActorNode.next_sibling("actor")) {

			Actor* actor = new Actor(ActorNode.attribute("name").as_string(), ActorNode.attribute("dialogue").as_string());
			string t = ActorNode.attribute("path").as_string();
			actor->texture = app->tex->Load(t.c_str());

			actors.PushBack(actor);
		}

		for (pugi::xml_node ActorNode = config.child("role"); ActorNode != NULL; ActorNode = ActorNode.next_sibling("role")) {

			string role = ActorNode.attribute("text").as_string();
			TextureDef* def = new TextureDef();
			def->name = role;
			def->texture = app->tex->Load(ActorNode.attribute("path").as_string());

			roles.PushBack(def);
			assignedRoles.PushBack(false); /* shares the same index as roles (tells you if a role is assigned ior not) */
		}

		//Load FX
		//selectButton = app->audio->LoadFx("Assets/Audio/Fx/selectButton.wav");
		//enterButton = app->audio->LoadFx("Assets/Audio/Fx/enterButton.wav");

		background = app->tex->Load("Assets/Textures/Battle scene editted.png");
	}

	return true;
}

bool CastingScene::PreUpdate() {

	actorIndex = actorIndex % actors.Count();

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


	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {

		AssignRole();

	}

	actorIndex = actorIndex % actors.Count();
	return true;
}

bool CastingScene::Update(float dt) {

	if (!finishedCasting) {
		bool allAssigned = true;
		for (int i = 0; i < assignedRoles.Count(); ++i) {
			if (assignedRoles[i] == false) {
				allAssigned = false;
			}
		}
		if (allAssigned) {

			app->dialogueManager->WriteTheScript();
			
			finishedCasting = true;
			app->levelManager->LoadScene(GameScene::BACKSTAGE);
			//Disable();
		}
	}

	return true;
}

bool CastingScene::PostUpdate() {

	SDL_Rect screen = SDL_Rect{ 0,0,256*2,198*2 };
	app->render->DrawTexture(background, 0,0, &screen);
	app->render->DrawRectangle(SDL_Rect{ 270,140,950,300 }, 0, 0, 0, 255, true, false);
	app->render->DrawText("With <- & -> keys select a role", 100 * 3, 50 * 3, 470 * 3, 20 * 3, true);
	app->render->DrawText("Press Enter to assign a role", 100 * 3, 70 * 3, 470 * 3, 20 * 3, true);
	// role
	//app->render->DrawText(roles[roleIndex]->name, 200 * 3, 100 * 3, 370 * 3, 20 * 3, true);

	// actor
	app->render->DrawText(actors[actorIndex % actors.Count()]->name, 100 * 3, 100 * 3, 470 * 3, 20 * 3, true);

	if (assignedRoles[roleIndex % roles.Count()] == false) {
		app->render->DrawText(roles[roleIndex]->name, 270 * 3, 100 * 3, 470 * 3, 20 * 3, true);
	}
	else {
		app->render->DrawText(roles[roleIndex]->name, 270 * 3, 100 * 3, 470 * 3, 20 * 3, true, SDL_Color {255,0,0,255});
	}
	app->render->DrawTexture(actors[actorIndex]->texture, 32 , 224 , &portraitBox);


	if (actorPortrait != nullptr) {

		app->render->DrawActor(actorPortrait,368, 128);
	}

	return true;
}

bool CastingScene::CleanUp() {


	return true;
}

void CastingScene::AssignRole() {

	if (assignedRoles[roleIndex] == false) {
		TextureDef* textA = new TextureDef(actors[actorIndex]->texture, actors[actorIndex]->name);

		TextureDef* textB = new TextureDef(roles[roleIndex]->texture, roles[roleIndex]->name);
		assignedRoles[roleIndex] = true;

		ActorSprite* sprite = new ActorSprite(textA, textB);

		actorPortrait = sprite;
		actorSprites.Add(sprite);
		app->dialogueManager->actorPortraits.insert(std::make_pair(textB->name, sprite));/*Save compound portrait in dialogue manager*/

		actorIndex++;
	}
}

#endif // __CASTINGSCENE_H__