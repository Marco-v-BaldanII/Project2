#include "App.h"
#include "Map.h"
#include "Entity.h"
#include "Audio.h"
#include "Physics.h"
#include "GuiManager.h"
#include "BackStage.h"
#include "../NPC.h"
#include "Log.h"

BackStage::BackStage(bool isActive) : Module(isActive)
{
	name.Create("BackStage");
	//toSave = false;
	//saveConfigs = false;
}

BackStage::~BackStage()
{
}

bool BackStage::Awake(pugi::xml_node&)
{
	return true;
}

bool BackStage::Start()
{
	if (active)
	{
		music = app->audio->PlayMusic("assets/audio/music/Musica-overworld-_Big-Map_.wav", 0);
		background = app->tex->Load("Assets/Textures/Backstageprops1.png");
		createplayer = true;
		app->backstageplayer->Enable();
		app->backstageplayer->position.x = 100;
		app->backstageplayer->position.y = 100;

		//noc creation
		for (pugi::xml_node npcNode = mynode.child("npc"); npcNode != NULL; npcNode = npcNode.next_sibling("npc")) {

			Npc* dude = (Npc*)app->entityManager->PlaceNPC(npcNode.attribute("name").as_string(), npcNode.attribute("x").as_int(), npcNode.attribute("y").as_int(), npcNode.attribute("wait").as_int());

			for (pugi::xml_node dialogueNode = npcNode.child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {
				// Create all the dialogues for the npc
				string text = dialogueNode.attribute("text").as_string();
				Dialogue* di = new Dialogue(npcNode.attribute("name").as_string(), text);

				dude->myDialogues.PushBack(di);
			}

			npcsList.Add(dude);
		}
	}

	return true;
}

bool BackStage::PreUpdate()
{	
	return true;
}

bool BackStage::Update(float dt)
{
	//recorrer la lista de npc y ver si el player esta cerca de alguno mostar un cubo para, y si le da a la E mostrar el dialogo
	ListItem<Npc*>* item;
	Npc* pEntity = NULL;

	for (item = npcsList.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		if (app->backstageplayer->position.DistanceTo(pEntity->position) < 50) {
			LOG("NEAR");
			if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
				//block movimiento de player
				app->dialogueManager->npcTalk(pEntity->myDialogues);
			}
		}
	}

	// Activate Pause Menu
	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
	{
		
	}

	return true;
}

bool BackStage::PostUpdate()
{
	SDL_Rect r = { 0,0,720,384 };
	app->render->DrawTexture(background, 0, 0, &r);

	return true;
}

bool BackStage::CleanUp()
{
	app->map->CleanUp();
	app->entityManager->CleanUp();
	//app->physics->CleanUp();
	//app->audio->StopMusic();
	return true;
}

