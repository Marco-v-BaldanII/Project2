#include "App.h"
#include "Map.h"
#include "Entity.h"
#include "Audio.h"
#include "Physics.h"
#include "GuiManager.h"
#include "BackStage.h"
#include "../NPC.h"
#include "Log.h"
#include "StartMenu.h"
#include "EntityManager.h"
#include "Window.h"
#include "../Dialogue.h"
#include "../TurnManager.h"
#include "BattleScene.h"

class Dialogue;

BackStage::BackStage(bool isActive) : Module(isActive)
{
	name.Create("backStage");
	//toSave = false;
	//saveConfigs = false;
}

BackStage::~BackStage()
{
}

bool BackStage::Awake(pugi::xml_node config)
{
	mynode = config;

	return true;
}

bool BackStage::Start()
{
	if (active)
	{
		randomText = app->tex->Load("Assets/Textures/UI/Sprite-0001.png");
		music = app->audio->PlayMusic("assets/audio/music/Musica-overworld-_Big-Map_.wav", 0);
		app->backstageplayer->Enable();

		if (backStageID == 1)
		{
			background = app->tex->Load("Assets/Textures/Backstageprops2-export.png");

			app->backstageplayer->position.x = 100;
			app->backstageplayer->position.y = 100;
			
			for (pugi::xml_node npcNode = mynode.child("npc"); npcNode != NULL; npcNode = npcNode.next_sibling("npc")) {

				Npc* dude = (Npc*)app->entityManager->PlaceNPC(npcNode.attribute("name").as_string(), npcNode.attribute("x").as_int(), npcNode.attribute("y").as_int(), npcNode.attribute("wait").as_int());

				dude->texture = app->tex->Load(npcNode.child("texture").attribute("path").as_string());

				Tree* t = new Tree();
				Node* root = nullptr;

				for (pugi::xml_node dialogueNode = npcNode.child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {
					// Create all the dialogues for the npc
					string text = dialogueNode.attribute("text").as_string();
					Dialogue* di = new Dialogue(npcNode.attribute("name").as_string(), text, dialogueNode.attribute("choiceA").as_string(), dialogueNode.attribute("choiceB").as_string());


					root = t->Insert(root, di, dialogueNode.attribute("ID").as_int());


					//dude->myDialogues.PushBack(di);
				}

				dude->dialogues = root;
				dude->currentDialogue = root;

				npcsList.Add(dude);
			}
		}
		else if (backStageID == 2)
		{
			background = app->tex->Load("Assets/Textures/test.png");
		}

		talkPrompt = app->tex->Load(mynode.child("talkBtn").attribute("path").as_string());
	}

	testPanel = new GuiPanel(true);
	testPanel->position = iPoint(0, 0);
	testPanel->bounds = SDL_Rect{ 82,120,200,200 };
	testPanel->texture = randomText;

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

	if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y -= 10;

	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y += 10;

	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x -= 10;

	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x += 10;

	near = false;
	for (item = npcsList.start; item != NULL; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		iPoint centerPos = app->backstageplayer->position;
		centerPos.x += 64; centerPos.y += 64;
		// Threshold distance from where to talk to the npcs
		if (centerPos.DistanceTo(pEntity->centerPos) < 90) {
			near = true;
			LOG("NEAR");
			if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN && app->dialogueManager->myState == NPCS) {
				//block movimiento de player
				app->dialogueManager->npcTalk(pEntity->currentDialogue);
				app->dialogueManager->currentNPC = pEntity;
			}
		}
		
	}

	return true;
}

bool BackStage::PostUpdate()
{
	SDL_Rect r = { 0,0,720,384 };
	app->render->DrawTexture(background, 0, 0, &r);
	// if the player is near an npc draw the talking prompt
	if(near){ app->render->DrawTexture(talkPrompt, app->backstageplayer->position.x - 76, app->backstageplayer->position.y ); }

	//testPanel->Draw();
	//app->render->DrawRectangle(testPanel->bounds, b2Color(1, 0, 0, 1), true, true);

	return true;
}

bool BackStage::CleanUp()
{
	app->map->CleanUp();
	//app->entityManager->CleanUp();
	app->backstageplayer->Disable();
	app->render->camera.x = 0;
	app->render->camera.y = 0;
	//app->physics->CleanUp();
	//app->audio->StopMusic();
	return true;
}

bool BackStage::OnGuiMouseClickEvent(GuiControl* control)
{
	

	return true;
}

void BackStage::FinishBackStage() {
	backStageID++;
	
}

bool BackStage::LoadState(pugi::xml_node node)
{
	for (ListItem<Npc*>* it = npcsList.start; it != nullptr; it = it->next) {

		for (pugi::xml_node n = node.child("Npc"); n != nullptr; n = n.next_sibling("Npc")) {

			if (n.attribute("name").as_string() == it->data->name) {

				it->data->position.x = n.attribute("x").as_int();
				it->data->position.y = n.attribute("y").as_int();
			}

		}
	}
	return true;
}

bool BackStage::SaveState(pugi::xml_node node)
{
	for (ListItem<Npc*>* it = npcsList.start; it != nullptr; it = it->next) {

		pugi::xml_node n = node.append_child("Npc");
		n.append_attribute("x").set_value(it->data->position.x);
		n.append_attribute("y").set_value(it->data->position.y);

		n.append_attribute("name").set_value(it->data->name.c_str());

	}

	return true;
}