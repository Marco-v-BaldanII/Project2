#pragma once
#include "Source/App.h"
#include "Source/Render.h"
#include "Source/Audio.h"
#include "Source/Defs.h"
#include "Source/Input.h"
#include "Source/EntityManager.h"
#include "Source/Entity.h"
#include "Source/Physics.h"
#include "Source/Map.h"
#include "Source/LevelManagement.h"
#include "Source/Animation.h"
#include "Source/Point.h"
#include "Source/Textures.h"
#include "Source/Module.h"
#include "Source/GuiControl.h"
#include "Source/GuiControlButton.h"
#include "Source/GuiSlider.h"
#include "Source/GuiPanel.h"
#include "Source/GuiToggle.h"
#include "Source/GuiManager.h"
#include "Source/Window.h"
#include "DialogueManager.h"
#include "BackstagePlayer.h"
#include "ItemManager.h"
#include "../BattleScene.h"
using namespace std;
#include <list>

class Inventory : public Module
{
public:
	Inventory(bool isActive);
	~Inventory();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool OnGuiMouseClickEvent(GuiControl* control);

	bool isVisible = false;
	List<Item*> InventoryItems;
	List<Player*> players;

	int windowW = 1536;
	int windowH = 1152;

	GuiControlButton* I_Use;
	GuiControlButton* I_Drop;
	GuiControlButton* I_Equip;
	GuiControlButton* I_Close;

	void DeleteItemAndReorderInventory(Item* item);
	void EquipItem(Player* player, Item* item);

	int selectedItem = 0;
	int selectedPlayer = 0;

	SDL_Texture* inventoryUI = nullptr;
	SDL_Texture* descriptionTex = nullptr;
	SDL_Rect inventoryRect;

	SDL_Rect itemRect;
	SDL_Rect playerRect;

	uint changeitemfx = 0;

	float menuY = 1200;
	const float animationSpeed = 30.0f;
private:
	
	
};