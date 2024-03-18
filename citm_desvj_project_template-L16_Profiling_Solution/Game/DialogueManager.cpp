#include "DialogueManager.h"
#include "Source/Player.h"
#include "Source/Item.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Scene.h"

#include "Source/Defs.h"
#include "Source/Log.h"

DialogueManager::DialogueManager(bool isActive) : Module(isActive)
{
	name.Create("dialoguemanager");
}

// Destructor
DialogueManager::~DialogueManager()
{}

// Called before render is available
bool DialogueManager::Awake(pugi::xml_node config)
{
	LOG("Loading Dialogue Manager");
	bool ret = true;

	for (pugi::xml_node dialogueNode = config.child("dialogues").child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {

		Dialogue* D = new Dialogue(dialogueNode.attribute("owner").as_string(), dialogueNode.attribute("text").as_string());
		dialogues.PushBack(D);
		dialogueSize++;
	}



	return ret;

}

bool DialogueManager::Start() {

	bool ret = true;

	//Iterates over the entities and calls Start

	return ret;
}

// Called before quitting
bool DialogueManager::CleanUp()
{
	bool ret = true;
	/*ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}

	entities.Clear();*/

	return ret;
}


bool DialogueManager::Update(float dt)
{
	bool ret = true;
	
	int mouseX, mouseY;

	app->input->GetMousePosition(mouseX, mouseY);

	//If the position of the mouse if inside the bounds of the box 
	if (mouseX > dialogueBox.x && mouseX < dialogueBox.x + dialogueBox.w && mouseY > dialogueBox.y && mouseY < dialogueBox.y + dialogueBox.h) {

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			dialogueIndex++;
		}
	}


	app->render->DrawRectangle(dialogueBox, b2Color(10, 10, 10, 1), true, true);
	const char* text = dialogues[dialogueIndex]->text.c_str();
	app->render->DrawText(text, dialogueBox.x + 10, dialogueBox.y + 10, dialogueBox.w, 30);

	return ret;
}