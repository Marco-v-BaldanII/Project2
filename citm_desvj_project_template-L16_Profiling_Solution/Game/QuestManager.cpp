#include "ItemManager.h"
#include "../Player.h"
#include "../Item.h"
#include "NPC.h"
#include "../Enemy.h"
#include "../App.h"
#include "../Textures.h"
#include "../Scene.h"
#include <string>
#include "../Defs.h"
#include "../Map.h"
#include "../Log.h"
#include "QuestManager.h"
#include "TurnManager.h"
#include "../GuiManager.h"


using namespace std;

QuestManager::QuestManager(bool isActive) : Module(isActive)
{
	name.Create("questmanager");

}

bool QuestManager::Start() {

	// Call start for all items
	completedQuest = "";
	menuTexture = app->tex->Load("Assets/Textures/UI/QuestMenu.png");

	return true;

}

bool QuestManager::Awake(pugi::xml_node config) {

	this->config = config;
	

	return true;

}



bool QuestManager::PreUpdate() {


	if (app->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
		showQuests = !showQuests;
	}


	return true;
}




bool QuestManager::Update(float dt) {


	// Update all items
	
	return true;
}

bool QuestManager::PostUpdate() {


	// Update all items

	if (showQuests) {

		app->render->DrawTexture(menuTexture, 20 + (app->render->camera.x / -3), 0 + (app->render->camera.y / -3));
		int i = 0;
		for (ListItem<Quest*>* it = quests.start; it != NULL; it = it->next) {

			
			app->render->DrawText(it->data->GetObjective(), 140, 60 + (i * 100), 12 * it->data->GetObjective().length(), 40, false, SDL_Color{0,0,0,255});

			i++;
		}

	}

	// Show the quest completed
	if (completedQuest != "" && showTimer.ReadSec() < 4) {
		string s = "Quest completed: "; s += completedQuest;
		app->render->DrawText(s, 200, 200, 30 * s.length(), 75, true, SDL_Color{ 255, 54, 0 ,255 });
	}
	else if ((completedQuest != "" && showTimer.ReadSec() > 4)){
		completedQuest = "";
	}

	return true;
}

void QuestManager::CheckQuestCompletion(string target) {

	for (ListItem<Quest*>* it = quests.start; it != NULL; it = it->next) {

		if (it->data->CheckCondition(target) == true) /* Quest completed */ {

			showTimer.Start();
			completedQuest = it->data->GetObjective();
			if (it->data->isMain == true)/* If it is a main quest end the lvl*/ {
				app->turnManager->MainQuest = true;
				app->guiManager->spotLight->mode = DIALOG;
				app->SaveRequest(); /*save characters progression for the next map*/
			}
			delete it->data;
			quests.Del(it);

		};
	}


}

bool QuestManager::CleanUp() {

	// Delet all items

	return true;

}

