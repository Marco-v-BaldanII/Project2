#include "DialogueManager.h"
#include "Source/Player.h"
#include "Source/Item.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Scene.h"
#include "Source/Window.h"
#include "Source/Defs.h"
#include "Source/Log.h"
#include "Interpolation.h"

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
			// If all the text has finished skip to next dialogue, else skip scrolling
			if (HasScrollFinished()) {
				dialogueIndex++;
				scrolling = true;
				numLines = 0;
				ResetScroll();
			}
			else {
				FinishScrolling();

			}
			
		}
	}

	app->render->DrawRectangle(dialogueBox, b2Color(10, 10, 10, 1), true, true);
	const char* text = dialogues[dialogueIndex]->text.c_str();
	numLines = 0;
	app->render->DrawText(text, dialogueBox.x + 3 * app->win->GetScale(), dialogueBox.y + 10 * app->win->GetScale(), dialogueBox.w * app->win->GetScale(), DIALOGUE_SIZE * app->win->GetScale(), true);

	if (scrolling) {
		ManageScrolling();
	}
	

	return ret;
}

void DialogueManager::ManageScrolling() {

	SDL_Rect r1 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10, -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r2 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + DIALOGUE_SIZE, -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r3 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE*2), -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r4 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE * 3), -dialogueBox.w , DIALOGUE_SIZE };

	//Must investigate more robotic type of interpolation such as STEP
	Lerp(w1_1, 0.002, 0);
	r1.w = w1_1;

	if (w1_1 >= 0) /*first line interpolation finished*/ {
		Lerp(w2_1, 0.002, 0);
		r2.w = w2_1;
	}

	if (w2_1 >= 0) /*second line interpolation finished*/ {
		Lerp(w3_1, 0.002, 0);
		r3.w = w3_1;
	}
	if (w3_1 >= 0) /*third line interpolation finished*/ {
		Lerp(w4_1, 0.002, 0);
		r4.w = w4_1;
	}
	app->render->DrawRectangle(r1, b2Color{ 10,10,10,1 }, true, true);
	app->render->DrawRectangle(r2, b2Color{ 10,10,10,1 }, true, true);
	app->render->DrawRectangle(r3, b2Color{ 10,10,10,1 }, true, true);
	app->render->DrawRectangle(r4, b2Color{ 10,10,10,1 }, true, true);

}

bool DialogueManager::PostUpdate() {


	return true;
}

void DialogueManager::ResetScroll() {
	w1_1 = -dialogueBox.w;
	w2_1 = -dialogueBox.w;
	w3_1 = -dialogueBox.w;
	w4_1 = -dialogueBox.w;
}

bool DialogueManager::HasScrollFinished() {

	if (numLines == 1 && w1_1 == 0) {
		return true;
	}
	else if (numLines == 2 && w2_1 == 0) {
		return true;
	}
	else if (numLines == 3 && w3_1 == 0) {
		return true;
	}
	else if (numLines == 4 && w4_1 == 0) {
		return true;
	}

	return false;

}

void DialogueManager::FinishScrolling() {
	w1_1 = 0;
	w2_1 = 0;
	w3_1 = 0;
	w4_1 = 0;
}