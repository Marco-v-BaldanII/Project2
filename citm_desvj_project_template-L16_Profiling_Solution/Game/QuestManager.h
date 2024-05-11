#ifndef __QUESTMANAGER_H__
#define __QUESTMANAGER_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../p2List.h"
#include "../Player.h"
#include "../Item.h"

using namespace std;



class Quest {

public:
	Quest(string objective) {
		this->objective = objective;
	}

	string GetObjective() {
		return objective;
	}

	virtual const bool CheckCondition(string target) {

		if (target == target) {
			return true;
		}
		return false;
	}

	bool isMain = false;

private:
	Player* observer = nullptr;

	bool active = false;

	string objective = "";

	

};


class FetchQuest : public Quest {
public:
	FetchQuest(string objective, string targetName) : Quest(objective) {
		this->ItemName = targetName;
	}

	const bool CheckCondition(string target) override {
		if (ItemName == target) { return true; }
		return false;
	}

private:
	string ItemName = "";




};

class DefeatQuest : public Quest {

public:
	DefeatQuest(string objective, string targetName):Quest(objective) {

		this->TargetName = targetName;
	}

	const bool CheckCondition(string target) override {
		
		if (TargetName == target) { return true; }
		return false;
	}

private:
	string TargetName;

};

class QuestManager : public Module
{
public:

	QuestManager(bool isActive);

	// Destructor


	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();
	bool PreUpdate() override;
	// Called every frame
	bool Update(float dt);
	bool PostUpdate() override;
	// Called before quitting
	bool CleanUp();

	// this is called when enemies are defeated or items are picked up, to check is that enemy or item was a quest
	void CheckQuestCompletion(string target);

	// Additional methods

	Quest* GiveQuest(int QuestID);


	pugi::xml_node config;

	List<Quest*> quests;

	bool showQuests = false;

	Timer showTimer;
	string completedQuest;

	SDL_Texture* menuTexture = nullptr;

};

#endif // __QUESTMANAGER_H__
