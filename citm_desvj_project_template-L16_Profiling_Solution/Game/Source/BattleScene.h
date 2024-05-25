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
#include "Enemy.h"
#include "EntityManager.h"
#include "GuiSlider.h"
#include "GuiPanel.h"
#include <map>
#include "GuiToggle.h"
#include "../random.h"
#include "../Particle.h"
#include "../ParticleSystem.h"

struct SDL_Texture;
class Quest;

class BattleScene : public Module
{
public:

	BattleScene(bool isActive);

	// Destructor
	virtual ~BattleScene();

	// Called before render is available
	bool Awake(pugi::xml_node config);

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

	bool SaveState(pugi::xml_node node);

	bool LoadState(pugi::xml_node node);

	void Disable()
	{
		
	}

	void LoadAnimations();

	void PassAnimations(Entity* entity);

	bool DrawHPBars(float& eHp_B, float eHp_A, float& aHpB, float aHpA, float maxE, float maxA, bool attack = false);

	bool DrawExpBar( float& xpB, float xpA);

	void KillUnit(bool isPlayer, Entity* entity);

	void StartSnowStorm();

	void StartRossing();


private:
	SDL_Texture* spriteSheet;
	SDL_Texture* yorkUI;
	SDL_Texture* lancasterUI;

	SDL_Texture* yorkHp;
	SDL_Texture* lancasterHp;

	SDL_Texture* battleBackground;

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


	bool started = false;
	std::map <std::string, SDL_Texture*> portraitTextures; // diccionary used to identify textures by a name


	UniqueRandomNumberGenerator uniqueNumber;

	uint archerFx;
	uint knightFx;
	uint mageFx;
	uint LancerFx;

	Particle* snowParticle;
	

public:
	ParticleEffect* snowSystem = nullptr;
	ParticleEffect* roseSystemRight = nullptr;

	List<Player*> party;
	List<Enemy*> goons;

	List<Quest*> quests;

	pugi::xml_node mynode;

	List<pugi::xml_node*> nodes;

	// All unit animations
	Animation mageRight;
	Animation mageDown;
	Animation mageUp;

	Animation archerRight;
	Animation archerDown;
	Animation archerUp;

	Animation knightRight;
	Animation knightDown;
	Animation knightUp;

	Animation dukeYorkRight;
	Animation dukeYorkDown;
	Animation dukeYorkUp;

	Animation princeEdRight;
	Animation princeEdDown;
	Animation princeEdUp;

	Animation henryTudorRight;
	Animation henryTudorDown;
	Animation henryTudorUp;

	Animation warwickRight;
	Animation warwickDown;
	Animation warwickUp;

	Animation edwardRight;
	Animation edwardDown;
	Animation edwardUp;

	Animation margaretRight;
	Animation margaretDown;
	Animation margaretUp;

	Animation Richard3Right;
	Animation Richard3Down;
	Animation Richard3Up;

	Animation lancasterRight;
	Animation lancasterDown;
	Animation lancasterUp;


	SDL_Rect yorkHPBar = SDL_Rect{ 255 * 2,167 * 2,75 * 2,2 * 2 };
	SDL_Rect LancasterHPBar = SDL_Rect{ 2 * 2,167 * 2,75 * 2,2* 2};

	SDL_Rect expBar = SDL_Rect{ 206,250,100 * 2, 25 * 2 };

	bool godMode;
	bool firstMap = false;

	bool MainQuest = false;

	SDL_Texture* expTexture;
	SDL_Texture* lvlUpTexture = nullptr;

	bool talking = false;
	Player* talker1;
	Player* talker2;

	bool inBattle = false;

};
#endif // __BATTLESCENE_H__

