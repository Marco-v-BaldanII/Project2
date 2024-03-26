#pragma once
#pragma once
#ifndef __DIALOGUEMANAGER_H__
#define __DIALOGUEMANAGER_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/DynArray.h"
#include <string>
#include "../Timer.h"
using namespace std;

#define DIALOGUE_SIZE 8


struct Dialogue {

public:
	string owner;

	string text;

	Dialogue(string owner, string text) {
		this->owner = owner;
		this->text = text;
	}
	

};


class DialogueManager : public Module
{
public:

	DialogueManager(bool isActive);

	// Destructor
	virtual ~DialogueManager();

	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	void ManageScrolling();

	void ResetScroll();

	bool HasScrollFinished();

	void FinishScrolling();

	// Called before quitting
	bool CleanUp();

	bool PostUpdate();

public:
	int numLines = 0;

private:

	DynArray<Dialogue*> dialogues;

	uint dialogueSize = 0;
	uint dialogueIndex = 0;

	SDL_Rect dialogueBox = SDL_Rect{ 0,0,160,100 };

	bool scrolling = true;

	// values used for interpolation of scrolling effect
	int w1_1 = -dialogueBox.w;
	int w2_1 = -dialogueBox.w;
	int w3_1 = -dialogueBox.w;
	int w4_1 = -dialogueBox.w;

};

#endif // __DIALOGUEMANAGER_H__
