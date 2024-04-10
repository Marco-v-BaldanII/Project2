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

enum TextVelocity {
	SLOW,
	FAST,
	NORMAL
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

	DynArray<Dialogue*> dialogues;
	DynArray<Dialogue*> shakespeareDialogues;

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


	void SetLanguage(Language lang);

	void SetTextVelocity(TextVelocity velocity);

	void Next_Dialogue();

public:
	int numLines = 0;

	DialogueStates myState = CUTSCENE;

	SDL_Texture* background = nullptr;

	Language myLanguage;

	//Text speed
	float slowVelocity = 0.002;
	float normalVelocity = 0.003;
	float fastVelocity = 0.004;

	float textVelocity = normalVelocity;

private:

	pugi::xml_node myConfig;

	DynArray<Dialogue*>* currentDialogues = nullptr;

	DynArray<Dialogue*> dialogues;
	DynArray<Dialogue*> shakespeareDialogues;
	std::map <std::string, SDL_Texture*> portraitTextures; // diccionary used to identify textures by a name

	uint dialogueSize = 0;
	uint dialogueIndex = 0;

	SDL_Rect dialogueBox = SDL_Rect{ 0,00,0,0};
	SDL_Rect narratorBox = SDL_Rect{ 45*2,100*2,160*2,60*2};
	SDL_Rect speechBox = SDL_Rect{ narratorBox.x - 3 - BOX_OFFSET, narratorBox.y - 3 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h -25 };
	SDL_Rect speechBoxRight = SDL_Rect{ narratorBox.x - 3 + BOX_OFFSET, narratorBox.y - 3 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h - 25 };

	SDL_Rect nameBoxL = SDL_Rect{ speechBox.x, speechBox.y - 22,48*2,12*2 };
	SDL_Rect nameBoxR = SDL_Rect{ speechBoxRight.x, speechBoxRight.y - 22,48*2,12*2};

	bool scrolling = true;

	// values used for interpolation of scrolling effect
	int w1_1 = -dialogueBox.w;
	int w2_1 = -dialogueBox.w;
	int w3_1 = -dialogueBox.w;
	int w4_1 = -dialogueBox.w;


	Position currentPos;
	b2Color TextColor;

	// CHANGE with correct portrait size
	SDL_Rect portraitBoxL = SDL_Rect{ 0,0,150,179 };
	SDL_Rect portraitBoxR = SDL_Rect{ 0,0,150,179 };

	

	DynArray<Dialogue*> currentNPC_Dialogues;
	int npcDialogueIndex = -1;

	DynArray<Scene*> Scenes;

	uint sceneIndex = 0;
	bool skipScene = false;

	uint skipFrames = 0;

	SDL_Rect skipRect = SDL_Rect{ 90,300,0,48 };
	SDL_Texture* skipBttnTex = nullptr;

};

#endif // __DIALOGUEMANAGER_H__
