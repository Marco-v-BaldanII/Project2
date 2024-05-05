#include "Inventory.h"
#include "Source/Log.h"

Inventory::Inventory(bool isActive) : Module(isActive)
{
}

Inventory::~Inventory()
{

}

bool Inventory::Start()
{
	InventoryItems = app->itemManager->items;
	// Calculate button dimensions based on window size
	const int buttonWidth = 150; 
	const int buttonHeight = 50;

	int buttonId = 70;
	int row = 0;

	//Create buttons for each item
	for (auto it = InventoryItems.start; it != NULL; it = it->next)
	{
		Item* item = it->data;
		GuiControlButton* button = (GuiControlButton*)app->guiManager->CreateGuiControl(
			GuiControlType::BUTTON,
			buttonId++,
			item->name,
			SDL_Rect{ buttonWidth, row * buttonHeight, buttonWidth, buttonHeight },
			this
		);
		ItemButtons.Add(button);

		row++;
	}

	I_Equip = (GuiControlButton*)app->guiManager->CreateGuiControl(
		GuiControlType::BUTTON,
		200,
		"Equipar",
		SDL_Rect{ 10, windowH - buttonHeight - 10, buttonWidth, buttonHeight },
		this
	);
	I_Drop = (GuiControlButton*)app->guiManager->CreateGuiControl(
		GuiControlType::BUTTON,
		201,
		"Tirar",
		SDL_Rect{ buttonWidth + 10, windowH - buttonHeight - 10, buttonWidth, buttonHeight },
		this
	);
	I_Use = (GuiControlButton*)app->guiManager->CreateGuiControl(
		GuiControlType::BUTTON,
		202,
		"Consumir",
		SDL_Rect{ 2 * buttonWidth + 20, windowH - buttonHeight - 10, buttonWidth, buttonHeight },
		this
	);

	return true;
}

bool Inventory::PreUpdate()
{
	
	return true;
}

bool Inventory::Update(float dt)
{
	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN && (app->levelManager->gameScene == GameScene::BACKSTAGE || app->levelManager->gameScene == GameScene::BACKSTAGE2 || app->levelManager->gameScene == GameScene::COMBAT))
	{
		isVisible = !isVisible;
	}

	if (isVisible)
	{
		if (app->levelManager->gameScene == GameScene::BACKSTAGE) app->backstageplayer->SetCanMove(false);

		//Normal
		for (auto it = ItemButtons.start; it != NULL; it = it->next)
		{
			(*it).data->state = GuiControlState::NORMAL;
		}

		I_Drop->state = GuiControlState::NORMAL;
		I_Equip->state = GuiControlState::NORMAL;
		I_Use->state = GuiControlState::NORMAL;
	}
	else if (!isVisible)
	{
		if (app->levelManager->gameScene == GameScene::BACKSTAGE) app->backstageplayer->SetCanMove(true);

		//Disabled
		for (auto it = ItemButtons.start; it != NULL; it = it->next)
		{
			(*it).data->state = GuiControlState::DISABLED;
		}

		I_Drop->state = GuiControlState::DISABLED;
		I_Equip->state = GuiControlState::DISABLED;
		I_Use->state = GuiControlState::DISABLED;
	}

	// Cosas de los botones

	return true;
}

bool Inventory::PostUpdate()
{
	
	return true;
}

bool Inventory::CleanUp()
{
	
	return true;
}

bool Inventory::OnGuiMouseClickEvent(GuiControl* control)
{
	

	return true;
}
