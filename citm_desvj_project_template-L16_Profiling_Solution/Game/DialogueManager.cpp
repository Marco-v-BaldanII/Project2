#include "DialogueManager.h"
#include "Source/Player.h"
#include "Source/Item.h"
#include "Source/App.h"
#include "Source/Textures.h"
#include "Source/Scene.h"
#include "Source/Window.h"
#include "Source/Defs.h"
#include "Source/Log.h"
#include "TurnManager.h"
#include "../BattleScene.h"
#include "Interpolation.h"
#include "someColors.h"
#include <string>
#include <map>
#include "../BackStage.h"
#include "BackstagePlayer.h"
#include "../GuiManager.h"
#include "Dialogue.h"
#include "NPC.h"

using namespace std;

DialogueManager::DialogueManager(bool isActive) : Module(isActive)
{
	name.Create("dialoguemanager");
}

// Destructor
DialogueManager::~DialogueManager()
{}


// Called before render is availableposition
bool DialogueManager::Awake(pugi::xml_node config)
{
	LOG("Loading Dialogue Manager");
	bool ret = true;

	// Load all the portrait textures

	myLanguage = ENGLISH;
	myState = NPCS;

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

		string path = dialogueNode.attribute("background").as_string();
		D->background = app->tex->Load(dialogueNode.attribute("background").as_string());
		// add to backgrounds array
		if (D->background != nullptr) backgrounds.PushBack(D->background);

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




	// Load the scens info
	for (pugi::xml_node sNode = myConfig.child("dialogues").child("english").child("Scene"); sNode != NULL; sNode = sNode.next_sibling("Scene")) {

		Scene* aScene = new Scene(sNode.attribute("num").as_int(), sNode.attribute("dialogues").as_int());
		Scenes.PushBack(aScene);

	}

	// put dialogues in scenes
	int prevAmount = 0;

	for (int i = 0; i < Scenes.Count(); ++i) {

		if (i != 0) { prevAmount += Scenes[i - 1]->numDialogues; } /*Acumulative offset*/

		int o = Scenes[i]->numDialogues;
		for (int j = 0; j < Scenes[i]->numDialogues; ++j) {

			Scenes[i]->dialogues.PushBack(dialogues[j + prevAmount]);
			Scenes[i]->shakespeareDialogues.PushBack(shakespeareDialogues[j + prevAmount]);
		}

	}

	//CHANGE
	skipBttnTex = app->tex->Load("Assets/Textures/Skip_button.png");


	choiceA_button = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 100, " choiceA ", choiceABox , this);
	choiceB_button = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 101, " choiceB ", choiceBBox, this);

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


	//currentPos = dialogues[dialogueIndex]->myPos;
	currentPos = Scenes[sceneIndex]->dialogues[dialogueIndex]->myPos;


	if (currentPos != MIDDLE) { TextColor = black; }

	// This method checks for the input to advance to the next dialogue
	AdvanceText();

	




	if (myState == CUTSCENE) {



		// Check to change dialogue language
		if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
			ChangeLanguage();
		}


	}
	else {

		if ( (currentNPC_Dialogues == NULL) || (currentNPC_Dialogues->leftChild == nullptr || currentNPC_Dialogues->rightChild == nullptr)) {

			choiceA_button->state = GuiControlState::DISABLED;
			choiceB_button->state = GuiControlState::DISABLED;
		}
		else {
			choiceA_button->state = GuiControlState::NORMAL;
			choiceB_button->state = GuiControlState::NORMAL;
		}

	}
	string t = "Let the show begin";
	int i = 0;

	if (currentNPC_Dialogues != nullptr) {
		for (int j = 0; j < 3; ++j) {
			LOG("dslfmlsfm");
			if (app->backStage->backStageID == 0 && currentNPC_Dialogues->ID == 469 ) {
				LOG("dojorjg");
				myState = CUTSCENE;
				app->backStage->FinishBackStage();
				currentNPC_Dialogues = nullptr;
				
				
				
				for (ListItem<Npc*>* it = app->backStage->npcsList.start; it != nullptr; it=it->next) {

					Entity* e = it->data;
					app->entityManager->DestroyEntity(e);
				}

				app->backStage->npcsList.Clear();
			}

		}
	}

	return ret;
}

bool DialogueManager::PostUpdate() {

	// Draw stuff
	DrawBackground();

	DrawTextBox(dialogues[dialogueIndex]->myPos);
	if (scrolling) {
		ManageScrolling();
	}

	DrawPortrait();

	return true;

}

void DialogueManager::DrawBackground() {

	if (myState == CUTSCENE) {

		//if (Scenes[sceneIndex]->dialogues[dialogueIndex]->background != nullptr) { background = Scenes[sceneIndex]->dialogues[dialogueIndex]->background; }
		app->render->camera.x = 0;
		app->render->camera.y = 0;

		SDL_Rect dsScreen = SDL_Rect{ 0,0,256 * 2,198 * 2 };
		app->render->DrawTexture(backgrounds[backgroundIndex], 0, 0, &dsScreen);

		SDL_Texture* b1 = Scenes[sceneIndex]->dialogues[dialogueIndex]->background;
		SDL_Texture* b2 = backgrounds[backgroundIndex];

		int h = backgrounds.Count();

		// if the displayed background is not the current one
		if (Scenes[sceneIndex]->dialogues[dialogueIndex]->background != nullptr && b1 != b2) {

			backgroundIndex++;
		}



	}

}

void DialogueManager::ManageScrolling() {


	SDL_Rect r1 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10, -dialogueBox.w , DIALOGUE_SIZE * 2 };
	SDL_Rect r2 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + DIALOGUE_SIZE * 2, -dialogueBox.w , DIALOGUE_SIZE * 2 };
	SDL_Rect r3 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE * 2 * 2), -dialogueBox.w , DIALOGUE_SIZE * 2 };
	SDL_Rect r4 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 10 + (DIALOGUE_SIZE * 3 * 2), -dialogueBox.w , DIALOGUE_SIZE * 2 };

	if (Scenes[sceneIndex]->dialogues[dialogueIndex]->myPos != MIDDLE) {
		r1 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 , -dialogueBox.w , DIALOGUE_SIZE * 2 };
		r2 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE * 2), -dialogueBox.w , DIALOGUE_SIZE * 2 };
		r3 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE * 2 * 2), -dialogueBox.w , DIALOGUE_SIZE * 2 };
		r4 = { dialogueBox.x + dialogueBox.w , dialogueBox.y + 3 + (DIALOGUE_SIZE * 3 * 2), -dialogueBox.w , DIALOGUE_SIZE * 2 };
	}





	//Must investigate more robotic type of interpolation such as STEP
	Lerp(w1_1, textVelocity, 0);
	r1.w = w1_1;

	if (w1_1 >= 0) /*first line interpolation finished*/ {
		Lerp(w2_1, textVelocity, 0);
		r2.w = w2_1;
	}

	if (w2_1 >= 0) /*second line interpolation finished*/ {
		Lerp(w3_1, textVelocity, 0);
		r3.w = w3_1;
	}
	if (w3_1 >= 0) /*third line interpolation finished*/ {
		Lerp(w4_1, textVelocity, 0);
		r4.w = w4_1;
	}

	if (myState == NPCS || currentPos != MIDDLE) {
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
			txt = Scenes[sceneIndex]->dialogues[dialogueIndex]->text.c_str();
		}
		else if (myLanguage == SHAKESPEREAN) {
			txt = Scenes[sceneIndex]->shakespeareDialogues[dialogueIndex]->text.c_str();
		}
		const char* text = txt.c_str();
		const char* owner = Scenes[sceneIndex]->dialogues[dialogueIndex]->owner.c_str();
		numLines = 0;

		if (Scenes[sceneIndex]->dialogues[dialogueIndex]->myPos == MIDDLE) {
			dialogueBox = narratorBox;


			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3 , dialogueBox.y - 3 , (dialogueBox.w + 6) , (dialogueBox.h + 6) }, b2Color(0, 0, 10, 1), true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2 , dialogueBox.y - 2 , (dialogueBox.w + 4) , (dialogueBox.h + 4) }, b2Color(0, 0, 0.5f, 1), true, true);
			app->render->DrawRectangle(dialogueBox, royalBlue, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 10) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * 2 * app->win->GetScale(), true);

		}
		else if (Scenes[sceneIndex]->dialogues[dialogueIndex]->myPos == LEFT) {
			dialogueBox = speechBox;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3 , dialogueBox.y - 3 , dialogueBox.w + 6, dialogueBox.h + 6 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2 , dialogueBox.y - 2 , dialogueBox.w + 4, dialogueBox.h + 4 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x , dialogueBox.y , dialogueBox.w , dialogueBox.h }, whitey, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 3) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * 2 * app->win->GetScale(), true, SDL_Color{ 0,0,0,255 });

			app->render->DrawRectangle(nameBoxL, black, true, true);
			app->render->DrawText(owner, (nameBoxL.x + 3) * app->win->GetScale(), (nameBoxL.y + 3) * app->win->GetScale(), nameBoxL.w * app->win->GetScale() - 5, (DIALOGUE_SIZE) * 2 * app->win->GetScale(), false, SDL_Color{ 255,255,255,255 });
		}
		else {
			dialogueBox = speechBoxRight;

			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3 , dialogueBox.y - 3 , dialogueBox.w + 6, dialogueBox.h + 6 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2 , dialogueBox.y - 2 , dialogueBox.w + 4, dialogueBox.h + 4 }, whitey, true, true);
			app->render->DrawRectangle(SDL_Rect{ dialogueBox.x , dialogueBox.y , dialogueBox.w , dialogueBox.h }, whitey, true, true);

			app->render->DrawText(text, (dialogueBox.x + 8) * app->win->GetScale(), (dialogueBox.y + 3) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * 2 * app->win->GetScale(), true, SDL_Color{ 0,0,0,255 });

			app->render->DrawRectangle(nameBoxR, black, true, true);
			app->render->DrawText(owner, (nameBoxR.x + 3) * app->win->GetScale(), (nameBoxR.y + 3) * app->win->GetScale(), nameBoxL.w * app->win->GetScale() - 5, (DIALOGUE_SIZE) * 2 * app->win->GetScale(), false, SDL_Color{ 255,255,255,255 });
		}

		if (app->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) {

			skipFrames += 2;

			if (skipFrames > 100) {
				skipFrames = 0;
				skipScene = true;
				Next_Dialogue();
			}

		}
		else {
			skipFrames = 0;
		}

		skipRect.w = skipFrames;

		app->render->DrawTexture(skipBttnTex, skipRect.x, skipRect.y);

		app->render->DrawRectangle(skipRect, b2Color(1, 1, 0, 0.5f), true, true);
	}
	else if (currentNPC_Dialogues != nullptr) {
		

		numLines = 0;
		dialogueBox = narratorBox;
		dialogueBox.x -= (app->render->camera.x/3);
		dialogueBox.y -= (app->render->camera.y/3);
		string txt = currentNPC_Dialogues->dialogue->text;

		app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 3, dialogueBox.y - 3, dialogueBox.w + 6, dialogueBox.h + 6 }, b2Color(0, 0, 10, 1), true, true);
		app->render->DrawRectangle(SDL_Rect{ dialogueBox.x - 2, dialogueBox.y - 2, dialogueBox.w + 4, dialogueBox.h + 4 }, b2Color(0, 0, 0.5f, 1), true, true);
		app->render->DrawRectangle(dialogueBox, whitey, true, true);

		app->render->DrawText(txt, (dialogueBox.x + (app->render->camera.x / 3) +200), (dialogueBox.y + 10) * app->win->GetScale(), (dialogueBox.w - 3) * app->win->GetScale(), DIALOGUE_SIZE * 2 * app->win->GetScale(), true, SDL_Color{ 0,0,0,1 });

	}


}

void DialogueManager::FinishScrolling() {
	w1_1 = 0;
	w2_1 = 0;
	w3_1 = 0;
	w4_1 = 0;
}

void DialogueManager::DrawPortrait() {

	// for the moment portrait size is assumed as 56x52 (whatever it ends up being it will have to be the same texture size for all)
	int i = 0;
	if (dialogueIndex > 0) {
		i = dialogueIndex - 1;
	}

	// Prints on screen the current and previous dialogue's portrait
	for (i; i <= dialogueIndex; ++i) {
		if (Scenes[sceneIndex]->dialogues[i]->myPos == LEFT) {
			app->render->DrawTexture(Scenes[sceneIndex]->dialogues[i]->texture, 0, 200, &portraitBoxL, true);
		}
		else if (Scenes[sceneIndex]->dialogues[i]->myPos == RIGHT) {
			app->render->DrawTexture(Scenes[sceneIndex]->dialogues[i]->texture, 204 * 2, 200, &portraitBoxR, false);
		}
	}
}

void DialogueManager::AdvanceText() {

	int mouseX, mouseY;

	app->input->GetMouseWorldPosition(mouseX, mouseY);

	//If the position of the mouse if inside the bounds of the box 
	if (mouseX > dialogueBox.x && mouseX < dialogueBox.x + dialogueBox.w && mouseY > dialogueBox.y && mouseY < dialogueBox.y + dialogueBox.h) {

		if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
			// If all the text has finished skip to next dialogue, else skip scrolling
			if (HasScrollFinished()) {
				if (myState == CUTSCENE) {


					Next_Dialogue();
					scrolling = true;
					numLines = 0;
					ResetScroll();

				}
				//else if (myState == NPCS)/* npcTalk(currentNPC_Dialogues);*/

			}
			else {
				FinishScrolling();

			}

		}
	}
	if (app->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN) {
		if (HasScrollFinished()) {
			if (myState == CUTSCENE) {
				Next_Dialogue();
				//else if (myState == NPCS) /*npcTalk(currentNPC_Dialogues);*/
				scrolling = true;
				numLines = 0;
				ResetScroll();
			}
			else if (myState == NPCS && currentNPC_Dialogues != nullptr) {
				
				
			}
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

void DialogueManager::SetLanguage(Language lang) {
	myLanguage = lang;
}

void DialogueManager::SetTextVelocity(TextVelocity velocity) {
	if (velocity == SLOW) {
		textVelocity = slowVelocity;
	}
	else if (velocity == FAST) {
		textVelocity = fastVelocity;
	}
	else if (velocity == NORMAL) {
		textVelocity = normalVelocity;
	}
}

void DialogueManager::npcTalk(Node* npcDialogues) {

	if (HasScrollFinished() == true || currentNPC_Dialogues == NULL) {

		scrolling = true;
		numLines = 0;
		ResetScroll();



		// reset npc dialogues
		if (npcDialogues == NULL) {
			currentNPC->currentDialogue = currentNPC->dialogues;
			currentNPC_Dialogues = currentNPC->currentDialogue;
		}

		app->backstageplayer->talking = true;
		// first dialogue
		if (currentNPC_Dialogues == nullptr) {
			this->currentNPC_Dialogues = npcDialogues;

			int choices = 0;
			if (npcDialogues->dialogue->choiceA != "") { choices++; }
			if (npcDialogues->dialogue->choiceB != "") { choices++; }

			if (choices == 2) {
				// display choices
				choiceA_button->text = npcDialogues->dialogue->choiceA.c_str();
				choiceB_button->text = npcDialogues->dialogue->choiceB.c_str();

			}
		}
		else if (this->currentNPC_Dialogues == npcDialogues)/*Advance to new dialogue , update the pointers*/ {
			
			if ((currentNPC->currentDialogue->leftChild == nullptr && currentNPC->currentDialogue->rightChild == nullptr)) {
				// finish dialoues
				app->backstageplayer->talking = false;
				currentNPC_Dialogues = nullptr;
				currentNPC->currentDialogue = currentNPC->dialogues;

				scrolling = false;
				numLines = 0;
				FinishScrolling();


			}
			else {
				if (currentNPC_Dialogues->leftChild != nullptr) {
					currentNPC_Dialogues = currentNPC_Dialogues->leftChild;
					currentNPC->currentDialogue = currentNPC_Dialogues;

					int choices = 0;
					if (currentNPC_Dialogues->dialogue->choiceA != "") { choices++; }
					if (currentNPC_Dialogues->dialogue->choiceB != "") { choices++; }
					if (choices == 2) {
						choiceA_button->text = currentNPC_Dialogues->dialogue->choiceA.c_str();
						choiceB_button->text = currentNPC_Dialogues->dialogue->choiceB.c_str();
					}
				}
			}



		}
	}
	else {
		FinishScrolling();
	}
	// draw choices 

	

	/*int size = npcDialogues.Count();

	if (npcDialogueIndex < size - 1) {
		myState = NPCS;
		this->currentNPC_Dialogues = npcDialogues;
		npcDialogueIndex++;
		app->backstageplayer->talking = true;
	}
	else {
		app->backstageplayer->talking = false;
		npcDialogueIndex = -1;
		currentNPC_Dialogues.Clear();
	}*/


}

void DialogueManager::Next_Dialogue() {

	dialogueIndex++;

	// checks if the scene has finished
	if (dialogueIndex + 1 == Scenes[sceneIndex]->numDialogues || skipScene) {

		skipScene = false;
		dialogueIndex = 0;
		if (sceneIndex + 1 != 6) {
			sceneIndex++;
		}
		else  {
			// the cutscene for act 1 has finished
			myState = NPCS;
			app->turnManager->Enable();
			app->battleScene->Enable();
			app->backStage->Disable();
			
		}

	}
}

bool DialogueManager::SaveState(pugi::xml_node node) {

	pugi::xml_node Indexes = node.append_child("index");
	Indexes.append_attribute("scene").set_value(sceneIndex);
	Indexes.append_attribute("dialogue").set_value(dialogueIndex);
	Indexes.append_attribute("background").set_value(backgroundIndex);

	pugi::xml_node Mode = node.append_child("mode");
	int s = -1;
	if (myState == CUTSCENE) { s = 0; }
	else { s = 1; }
	Mode.append_attribute("value").set_value(s);

	pugi::xml_node Language = node.append_child("language");
	if (myLanguage == ENGLISH) {
		Language.append_attribute("value").set_value("english");
	}
	else {
		Language.append_attribute("value").set_value("shakesperean");
	}
	return true;
}

bool DialogueManager::LoadState(pugi::xml_node node) {

	sceneIndex = node.child("index").attribute("scene").as_int();
	dialogueIndex = node.child("index").attribute("dialogue").as_int();
	backgroundIndex = node.child("index").attribute("background").as_int();

	string language = node.child("language").attribute("value").as_string();
	if (language == "english") { myLanguage = ENGLISH; }
	else { myLanguage = SHAKESPEREAN; }

	int mode = node.child("mode").attribute("value").as_int();
	if (mode == 0) { myState = CUTSCENE; }
	else { myState = NPCS; }


	return true;
}

bool DialogueManager::OnGuiMouseClickEvent(GuiControl* control) {
	if (currentNPC_Dialogues != NULL) {

		if (control->id == 100)
		{
			LOG("Choice A");
			if (currentNPC_Dialogues->leftChild != nullptr) {
				//currentNPC_Dialogues = currentNPC_Dialogues->leftChild;
				currentNPC->currentDialogue = currentNPC->currentDialogue->leftChild;
				currentNPC_Dialogues = currentNPC->currentDialogue;
			}
		}
		else if (control->id == 101)
		{
			LOG("Choice B");
			if (currentNPC_Dialogues->rightChild != nullptr) {
				//currentNPC_Dialogues = currentNPC_Dialogues->rightChild;
				currentNPC->currentDialogue = currentNPC->currentDialogue->rightChild;
				currentNPC_Dialogues = currentNPC->currentDialogue;
			}
		}
		int choices = 0;
		if (currentNPC_Dialogues->dialogue->choiceA != "") { choices++; }
		if (currentNPC_Dialogues->dialogue->choiceB != "") { choices++; }

		if (choices == 2) {
			// display choices
			choiceA_button->text = currentNPC_Dialogues->dialogue->choiceA.c_str();
			choiceB_button->text = currentNPC_Dialogues->dialogue->choiceB.c_str();

		}
		

		
		
	}

	return true;
}