#pragma once
#pragma once
#ifndef __DIALOGUEMANAGER_H__
#define __DIALOGUEMANAGER_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/DynArray.h"
#include <string>
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

	// Called before quitting
	bool CleanUp();

	bool PostUpdate();

private:

	DynArray<Dialogue*> dialogues;

	uint dialogueSize = 0;
	uint dialogueIndex = 0;

	SDL_Rect dialogueBox = SDL_Rect{ 0,0,160,100 };

};

#endif // __DIALOGUEMANAGER_H__
