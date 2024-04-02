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
#include "someColors.h"
#include <string>
#include <map>

using namespace std;

DialogueManager::DialogueManager(bool isActive) : Module(isActive)
{
	name.Create("dialoguemanager");
}

// Destructor
DialogueManager::~DialogueManager()
{}

// NOTA recuerda hacer esta movida modular para que el textbox pueda estar MODULAR CON LOS BOCADILLOS QUE APAREZCAN EN DISTINTOS LADOS, estoy cansado buenas noches

// Called before render is availableposition
bool DialogueManager::Awake(pugi::xml_node config)
{
	LOG("Loading Dialogue Manager");
	bool ret = true;

	// Load all the portrait textures
	
	myLanguage = ENGLISH;
	
	myConfig = config;

	return ret;

}

bool DialogueManager::Start() {

	bool ret = true;


	for (pugi::xml_node dialogueNode = myConfig.child("dialogues").child("portrait"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("portrait")) {

		TextureDef* texD = new TextureDef;
		const char* path = dialogueNode.attribute("texturePath").as_string();
		texD->texture = app->tex->Load(path);
		texD->name = dialogueNode.attribute("texturePath").as_string();

		texD->name.erase(0, 26);
		texD->name.erase(texD->name.length() - 4, 4);


		// insert TextureDefinition to the map diccionary
		portraitTextures.insert(std::make_pair(texD->name, texD->texture));

	}

	for (pugi::xml_node dialogueNode = myConfig.child("dialogues").child("english").child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {

		Dialogue* D = new Dialogue(dialogueNode.attribute("owner").as_string(), dialogueNode.attribute("text").as_string());
		int p = dialogueNode.attribute("position").as_int();
		if (p == 1) {
			D->myPos = RIGHT;
		}
		else if (p == 2 || p == 4) {
			D->myPos = LEFT;
		}

		// Assign a texture portrait
		if (D->owner != "Narrator") {
			// use the keycode (owner name) to find the correct portrait
			D->texture = portraitTextures[D->owner];
		
		}





		dialogues.PushBack(D);
		dialogueSize++;
	}
	// Shakespearean dialogues
	for (pugi::xml_node dialogueNode = myConfig.child("dialogues").child("shakesperean").child("dialogue"); dialogueNode != NULL; dialogueNode = dialogueNode.next_sibling("dialogue")) {

		Dialogue* D = new Dialogue(dialogueNode.attribute("owner").as_string(), dialogueNode.attribute("text").as_string());
		int p = dialogueNode.attribute("position").as_int();
		if (p == 1) {
			D->myPos = RIGHT;
		}
		else if (p == 2 || p == 4) {
			D->myPos = LEFT;
		}

		// Assign a texture portrait
		if (D->owner != "Narrator") {
			// use the keycode (owner name) to find the correct portrait
			D->texture = portraitTextures[D->owner];

		}
		shakespeareDialogues.PushBack(D);
		//dialogueSize++;
	}

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
	
	
	currentPos = dialogues[dialogueIndex]->myPos;

	// Change font color to black for speech dialogues
	if (currentPos != MIDDLE) { TextColor = black; }

	// This method checks for the input to advance to the next dialogue
	AdvanceText();
	
	// Adjust the position of the text box

	DrawTextBox(dialogues[dialogueIndex]->myPos);

	DrawPortrait();

	
	if (scrolling) {
		ManageScrolling();
	}
	
	// Check to change dialogue language
	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		ChangeLanguage();
	}

	return ret;
}

void DialogueManager::ManageScrolling() {


	SDL_Rect r1 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10, -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r2 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + DIALOGUE_SIZE, -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r3 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE * 2), -dialogueBox.w , DIALOGUE_SIZE };
	SDL_Rect r4 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE * 3), -dialogueBox.w , DIALOGUE_SIZE };

	if (dialogues[dialogueIndex]->myPos != MIDDLE) {
		 r1 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 , -dialogueBox.w , DIALOGUE_SIZE };
		 r2 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE), -dialogueBox.w , DIALOGUE_SIZE };
		 r3 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE *2), -dialogueBox.w , DIALOGUE_SIZE };
		 r4 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE * 3), -dialogueBox.w , DIALOGUE_SIZE };
	}
	
		
	


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

	if (myState == NPCS ||  currentPos != MIDDLE) {
		app->render->DrawRectangle(r1, whitey, true, true);
		app->render->DrawRectangle(r2, whitey, true, true);
		app->render->DrawRectangle(r3, whitey, true, true);
		app->render->DrawRectangle(r4, whitey, true, true);
	}
	else {

		app->render->DrawRectangle(r1, royalBlue, true, true);
		app->render->DrawRectangle(r2, royalBlue, true, true);
		app->render->DrawRectangle(r3, royalBlue, true, true);
		app->render->DrawRectangle(r4, royalBlue, true, true);
	}
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

void DialogueManager::DrawTextBox(Position pos) {


	if (myState == CUTSCENE) {
		string txt = "fhfjf";
		if (myLanguage == ENGLISH) {
			txt = dialogues[dialogueIndex]->text.c_str();
		}
		else if (myLanguage == SHAKESPEREAN) {
			txt = shakespeareDialogues[dialogueIndex]->text.c_str();
		}
		const char* text = txt.c_str();
		const char* owner = dialogues[dialogueIndex]->owner.c_str();
		numLines = 0;

		if (dialogues[dialogueIndex]->myPos == MIDDLE) {
			dialogueBox = narratorBox;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3, dialogueBox.y - 3, dialogueBox.w + 6, dialogueBox.h + 6 }, b2Color(0, 0, 10, 1), true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2, dialogueBox.y - 2, dialogueBox.w + 4, dialogueBox.h + 4 }, b2Color(0, 0, 0.5f, 1), true, true);
			app->render->DrawRectangle(dialogueBox, royalBlue, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 10) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * app->win->GetScale(), true);

		}
 		else if (dialogues[dialogueIndex]->myPos == LEFT) {
			dialogueBox = speechBox;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3 , dialogueBox.y - 3 , dialogueBox.w + 6, dialogueBox.h + 6 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2 , dialogueBox.y - 2 , dialogueBox.w + 4, dialogueBox.h + 4 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x , dialogueBox.y , dialogueBox.w , dialogueBox.h }, whitey, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 3) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * app->win->GetScale(), true, SDL_Color{ 0,0,0,255 });

			app->render->DrawRectangle(nameBoxL, black, true, true);
			app->render->DrawText(owner, (nameBoxL.x + 3) * app->win->GetScale(), (nameBoxL.y + 3) * app->win->GetScale(), nameBoxL.w * 6 * app->win->GetScale(), (DIALOGUE_SIZE)*app->win->GetScale(), false, SDL_Color{ 255,255,255,255 });
		}
		else {
			dialogueBox = speechBoxRight;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3 , dialogueBox.y - 3 , dialogueBox.w + 6, dialogueBox.h + 6 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2 , dialogueBox.y - 2 , dialogueBox.w + 4, dialogueBox.h + 4 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x , dialogueBox.y , dialogueBox.w , dialogueBox.h }, whitey, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 3) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * app->win->GetScale(), true, SDL_Color{ 0,0,0,255 });

			app->render->DrawText(owner, (nameBoxR.x + 3) * app->win->GetScale(), (nameBoxR.y + 3) * app->win->GetScale(), nameBoxL.w * 6 * app->win->GetScale(), (DIALOGUE_SIZE)*app->win->GetScale(), false, SDL_Color{ 255,255,255,255 });
		}
	}
	else {
		string txt = "fhfjf";
		
		txt = currentNPC_Dialogues[npcDialogueIndex]->text;

		const char* text = txt.c_str();
		const char* owner = dialogues[dialogueIndex]->owner.c_str();
		numLines = 0;

		// For now i am drawing npc dialogue in the narrator box
			dialogueBox = narratorBox;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3, dialogueBox.y - 3, dialogueBox.w + 6, dialogueBox.h + 6 }, b2Color(0, 0, 10, 1), true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2, dialogueBox.y - 2, dialogueBox.w + 4, dialogueBox.h + 4 }, b2Color(0, 0, 0.5f, 1), true, true);
			app->render->DrawRectangle(dialogueBox, whitey, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 10) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * app->win->GetScale(), true, SDL_Color{0,0,0,1});

		
		
	}


}

void DialogueManager::FinishScrolling() {
	w1_1 = 0;
	w2_1 = 0;
	w3_1 = 0;
	w4_1 = 0;
}

void DialogueManager::DrawPortrait(){

	// for the moment portrait size is assumed as 56x52 (whatever it ends up being it will have to be the same texture size for all)
	int i = 0;
	if (dialogueIndex > 0) {
		i = dialogueIndex - 1;
	}

	// Prints on screen the current and previous dialogue's portrait
	for (i; i <= dialogueIndex; ++i) {
		if (dialogues[i]->myPos == LEFT) {
			app->render->DrawTexture(dialogues[i]->texture, 0, 136, &portraitBoxL, true);
		}
		else if (dialogues[i]->myPos == RIGHT) {
			app->render->DrawTexture(dialogues[i]->texture, 204, 136, &portraitBoxR, false);
		}
	}
}

void DialogueManager::AdvanceText() {

	int mouseX, mouseY;

	app->input->GetMousePosition(mouseX, mouseY);

	//If the position of the mouse if inside the bounds of the box 
	if (mouseX > dialogueBox.x && mouseX < dialogueBox.x + dialogueBox.w && mouseY > dialogueBox.y && mouseY < dialogueBox.y + dialogueBox.h) {

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			// If all the text has finished skip to next dialogue, else skip scrolling
			if (HasScrollFinished()) {
				if (myState == CUTSCENE) dialogueIndex++;
				else if (myState == NPCS) npcTalk(currentNPC_Dialogues);
				scrolling = true;
				numLines = 0;
				ResetScroll();
			}
			else {
				FinishScrolling();

			}

		}
	}
	if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN || app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		if (HasScrollFinished()) {
			if (myState == CUTSCENE) dialogueIndex++;
			else if (myState == NPCS) npcTalk(currentNPC_Dialogues);
			scrolling = true;
			numLines = 0;
			ResetScroll();
		}
		else {
			FinishScrolling();

		}
	}


}

void DialogueManager::ChangeLanguage() {

	if (myLanguage == ENGLISH) {
		myLanguage = SHAKESPEREAN;
	}
	else {
		myLanguage = ENGLISH;
	}

}

void DialogueManager::npcTalk(DynArray<Dialogue*>& npcDialogues) {

	int size = npcDialogues.Count();

	if (npcDialogueIndex < size -1) {
		myState = NPCS;
		this->currentNPC_Dialogues = npcDialogues;
		npcDialogueIndex++;
	}
	else {
		myState = CUTSCENE;
		npcDialogueIndex = -1;
		currentNPC_Dialogues.Clear();
	}
}