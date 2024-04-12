#ifndef __LEVELMANAGEMENT_H__
#define __LEVELMANAGEMENT_H__
#include "Module.h"

#include "App.h"
#include "Intro.h"
#include "StartMenu.h"
//#include "GameOver.h"
#include "FadeToBlack.h"
#include "Render.h"
#include "Input.h"

#define MAX_SIZE 28

enum GameScene {
	NONE = 0,
	INTRO = 1,
	START = 2,
	GAME_OVER = 3,
	BACKSTAGE = 4,
	COMBAT = 5,
	CREDITS = 20
};

class LevelManagement : public Module
{
public:
	LevelManagement(bool isActive);
	~LevelManagement();
	bool Awake(pugi::xml_node&);
	bool Start();
	bool PreUpdate() ;
	bool Update(float dt) ;
	bool CleanUp();
	void NextLevel();
	void ReturnToMainMenu();
	void RestartLevel();
	void LoadScene(GameScene scene);

	// Called when we want to load data from XML
	//bool LoadState(pugi::xml_node node);

	// Called when we want to save data from XML
	bool SaveState(pugi::xml_node node);

	// Load / Save
	//bool LoadState(pugi::xml_node& data) override;
	//bool SaveState(pugi::xml_node& data) const override;
	
	GameScene gameScene;
	GameScene lastOpenWorldScene;
	//CombatState combatState = NOCOMBAT;
	Module* currentScene = nullptr;
	Module* currentLevel = nullptr;

	int gemCount = 0;
	int frameCounter = 0;
	int delayTime = 100;
	int levelsPassed = 0;
	bool loadLevel = false;

	iPoint playerLastPos_TheFall;
	iPoint playerLastPos_GreenPath;
	iPoint playerLastPos_Village;
	iPoint playerLastPos_GeneralRoom;
	iPoint playerLastPos_MageRoom;
	iPoint playerLastPos_ShopRoom;
	iPoint playerLastPos_TheRuins;
	iPoint playerLastPos_FracturedRoad;
	iPoint playerLastPos_DragonCliff;

	//GameScene combatScene = PLAIN;

	uint music;
	bool playMusic = true;
	bool inCombat = false;
};
#endif // __LEVELMANAGEMENT_H__
