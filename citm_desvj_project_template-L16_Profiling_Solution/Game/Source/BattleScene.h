#ifndef __BATTLESCENE_H__
#define __BATTLESCENE_H__

#include "Module.h"
//#include "p2List.h"
#include "Defs.h"
#include "Physics.h"
#include "Animation.h"
#include "Audio.h"
#include "GuiControlButton.h"
#include "Player.h"
#include "EntityManager.h"
#include "GuiSlider.h"
#include "GuiPanel.h"
#include "GuiToggle.h"

struct SDL_Texture;

class BattleScene : public Module
{
public:

	BattleScene(bool isActive);

	// Destructor
	virtual ~BattleScene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool OnGuiMouseClickEvent(GuiControl* control);

private:
	SDL_Texture* img;
	SDL_Rect rect;
	uint music;
	bool playMusic = true;
	bool test;

	int playerHp = 100;
	int enemyHp = 100;
	GuiControlButton* AttackButton;
	GuiControlButton* HealButton;
	Player* player;
	int waitTimer;
	int attackPattern;
	bool playerTurn = true;
};
#endif // __BATTLESCENE_H__

