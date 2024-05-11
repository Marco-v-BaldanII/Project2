#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Animation.h"
#include "Timer.h"
#include "GuiControlButton.h"
#include "GuiControl.h"


struct SDL_Texture;
class Dialogue;
class Conversation;
class Easing;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();
	bool PreUpdate() override;
	bool PostUpdate() override;
	bool Update(float dt);
	bool SuperPostUpdate() override;

	bool CleanUp();

	void ClickOnMe();

	void CalculateAttack();

	void OnCollision(Collider* physA, Collider* physB) override;

	bool OnGuiMouseClickEvent(GuiControl* control) override;

	bool DealDMG();


	void FigureStickMovement(float dt);

	iPoint LevelUp();

	void HandleMiss();

	void DrawCombatScene();

public:

	Frame* frames;

	PathFinding* personalPathfinding;


	//L02: DONE 2: Declare player parameters
	float speed = 0.2f;
	SDL_Texture* myTexture = nullptr;
	SDL_Texture* UiTex = nullptr;
	const char* realname;
	pugi::xml_node config;
	uint texW, texH;
	int stepCounter = 0;
	//Audio fx
	int pickCoinFxId;

	SDL_Rect clickBox = { 0,0,32,32 };

	bool movedThisTurn = false;
	bool endTurn;
	
	bool finishedLerp = false;


	bool opponentReachTarget = false;

	bool receivedEXP = false;

	bool atckedClicked = false;

	float lerpingEXP;
	

	Timer showLvlUp;
	iPoint lvlMods = iPoint(-1, -1);

	GuiControlButton* atkButton = nullptr;
	GuiControlButton* waitButton = nullptr;
	GuiControlButton* talkButton = nullptr;

	int atkBtnId = -1;
	int waitBtnId = -1;
	int talkBtnId = -1;

	List<Conversation*> conversations;

	SDL_Texture* missText;

};

#endif // __PLAYER_H__