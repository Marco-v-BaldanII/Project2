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
#include "../GuiButton.h"
#include "NPC.h"
#include "../GuiSlider.h"
#include "../GuiPanel.h"
#include "../GuiToggle.h"
#include "../GuiControl.h"
#include "../GuiControlButton.h"

using namespace std;

#define DIALOGUE_SIZE 7
#define BOX_OFFSET 20

enum Position;

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
	NPCS,
	SPONTANEOUS

};
class Dialogue;
class Tree;
struct Node;
class Npc;

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

	~Scene() {
		// Clear the dialogues array
		for (unsigned int i = 0; i < dialogues.Count(); ++i) {
			delete dialogues[i];
		}

		for (unsigned int i = 0; i < shakespeareDialogues.Count(); ++i) {
			delete shakespeareDialogues[i];
		}
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


	void DrawTextBox(Position pos);

	void DrawDialogue();

	void DrawPortrait();

	void ChangeLanguage();

	void AdvanceText();

	void npcTalk(Node* npcDialogues);

	void DrawBackground();


	void SetLanguage(Language lang);

	void SetTextVelocity(TextVelocity velocity);

	void Next_Dialogue();

	bool SaveState(pugi::xml_node node);

	bool LoadState(pugi::xml_node node);

	bool OnGuiMouseClickEvent(GuiControl* control);

	void SpontaneousDialogue(Dialogue* _dialogue);

	void SpontaneousDialogue(Conversation* _conversation);

	void NextAct();

	void WriteTheScript();

public:
	int numLines = 0;

	DialogueStates myState = CUTSCENE;

	DynArray<SDL_Texture*> backgrounds;
	uint backgroundIndex = 0;

	Language myLanguage;

	//Text speed
	float slowVelocity = 0.002;
	float normalVelocity = 0.005;
	float fastVelocity = 0.02;

	Npc* currentNPC;

	float textVelocity = fastVelocity;

	std::map <std::string, ActorSprite*> actorPortraits;

private:

	pugi::xml_node myConfig;

	pugi::xml_document dialogueFile2;

	DynArray<Dialogue*>* currentDialogues = nullptr;

	DynArray<Dialogue*> dialogues;
	DynArray<Dialogue*> shakespeareDialogues;
	std::map <std::string, SDL_Texture*> portraitTextures; // diccionary used to identify textures by a name

	SDL_Texture* dialogueBoxTexture = nullptr;

	uint dialogueSize = 0;
	uint dialogueIndex = 0;

	SDL_Rect dialogueBox = SDL_Rect{ 0,00,0,0};
	SDL_Rect narratorBox = SDL_Rect{ 45*2,100*2,160*2,60*2};
	SDL_Rect speechBox = SDL_Rect{ narratorBox.x - 3 - BOX_OFFSET, narratorBox.y - 40 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h -25 };
	SDL_Rect speechBoxRight = SDL_Rect{ narratorBox.x - 3 + BOX_OFFSET, narratorBox.y - 40 - BOX_OFFSET, narratorBox.w + 6, narratorBox.h - 25 };

	SDL_Rect nameBoxL = SDL_Rect{ speechBox.x, speechBox.y - 22,48*2,12*2 };
	SDL_Rect nameBoxR = SDL_Rect{ speechBoxRight.x, speechBoxRight.y - 22,48*2,12*2};

	bool scrolling = true;

	// values used for interpolation of scrolling effect
	int w1_1 = -dialogueBox.w-8 ;
	int w2_1 = -dialogueBox.w-8 ;
	int w3_1 = -dialogueBox.w-8 ;
	int w4_1 = -dialogueBox.w-8 ;


	Position currentPos;
	b2Color TextColor;

	// CHANGE with correct portrait size
	SDL_Rect portraitBoxL = SDL_Rect{ 0,0,150,179 };
	SDL_Rect portraitBoxR = SDL_Rect{ 0,0,150,179 };

	

	Node* currentNPC_Dialogues = NULL;
	int npcDialogueIndex = -1;

	GuiControlButton* choiceA_button = nullptr;
	GuiControlButton* choiceB_button = nullptr;

	SDL_Texture* choiceButtontex = nullptr;

	SDL_Rect choiceABox = SDL_Rect{ 368*3, 224*3, 128*3,48*3 };
	SDL_Rect choiceBBox = SDL_Rect{ 368*3, 294*3, 128*3,48*3 };

	DynArray<Scene*> Scenes;

	uint sceneIndex = 0;
	bool skipScene = false;

	uint skipFrames = 0;

	SDL_Rect skipRect = SDL_Rect{ 90,300,0,48 };
	SDL_Texture* skipBttnTex = nullptr;
	SDL_Texture* overlayTextBox = nullptr;

	Dialogue* spontaneousDialogue;

	Conversation* spontaneousConversation;
	int conversationIndex = 0;
	public:
	bool scriptWritten = false;

	int DialogueChannel;

	uint bitAudio[4];
	bool audiolines[4] = { false };
	int audioIndexes[4] = { 0 };

};

#endif // __DIALOGUEMANAGER_H__
