#pragma once
#pragma once
#ifndef __DIALOGUEMANAGER_H__
#define __DIALOGUEMANAGER_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/DynArray.h"
#include <string>
#include <map>
#include "../Timer.h"
#include "Source/Textures.h"

using namespace std;

#define DIALOGUE_SIZE 7
#define BOX_OFFSET 20

enum Position {
	LEFT,
	RIGHT,
	MIDDLE
};

enum Language {
	SHAKESPEREAN,
	ENGLISH
};


enum DialogueStates {

	CUTSCENE,
	NPCS

};

struct Dialogue {

public:
	string owner;

	string text;

	Position myPos;

	SDL_Texture* texture = nullptr;

	Dialogue(string owner, string text) {
		this->owner = owner;
		this->text = text;
		myPos = MIDDLE;
	}
	
	SDL_Texture* background = nullptr;

};

struct Scene {
public:

	Scene(int num, int numDialogues) {
		this->num = num;
		this->numDialogues = numDialogues;
	}

	int num;
	int numDialogues;

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


	void DrawTextBox(Position pos);

	void DrawPortrait();

	void ChangeLanguage();

	void AdvanceText();

	void npcTalk(DynArray<Dialogue*>& npcDialogues);

	void DrawBackground();

public:
	int numLines = 0;

	DialogueStates myState = CUTSCENE;

	SDL_Texture* background = nullptr;

private:

	pugi::xml_node myConfig;

	DynArray<Dialogue*>* currentDialogues = nullptr;

	DynArray<Dialogue*> dialogues;
	DynArray<Dialogue*> shakespeareDialogues;
	std::map <std::string, SDL_Texture*> portraitTextures; // diccionary used to identify textures by a name

	uint dialogueSize = 0;
	uint dialogueIndex = 0;

	SDL_Rect dialogueBox = SDL_Rect{ 0,00,0,0};
	SDL_Rect narratorBox = SDL_Rect{ 45,100,160,60 };
	SDL_Rect speechBox = SDL_Rect{ narratorBox.x - 3 - BOX_OFFSET, narratorBox.y - 3 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h -25 };
	SDL_Rect speechBoxRight = SDL_Rect{ narratorBox.x - 3 + BOX_OFFSET, narratorBox.y - 3 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h - 25 };

	SDL_Rect nameBoxL = SDL_Rect{ speechBox.x, speechBox.y - 12,32,12 };
	SDL_Rect nameBoxR = SDL_Rect{ speechBoxRight.x, speechBoxRight.y - 12,32,12 };

	bool scrolling = true;

	// values used for interpolation of scrolling effect
	int w1_1 = -dialogueBox.w;
	int w2_1 = -dialogueBox.w;
	int w3_1 = -dialogueBox.w;
	int w4_1 = -dialogueBox.w;


	Position currentPos;
	b2Color TextColor;

	// CHANGE with correct portrait size
	SDL_Rect portraitBoxL = SDL_Rect{ 0,0,52,56 };
	SDL_Rect portraitBoxR = SDL_Rect{ 0,0,52,56 };

	Language myLanguage;



	DynArray<Dialogue*> currentNPC_Dialogues;
	int npcDialogueIndex = -1;

	DynArray<Scene*> Scenes;

};

#endif // __DIALOGUEMANAGER_H__
