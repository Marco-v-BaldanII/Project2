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
	players = app->battleScene->party;
	InventoryItems = app->itemManager->items;

	inventoryUI = app->tex->Load("Assets/Textures/inventoryUI.png");
	inventoryRect = { 0,0,410,250 };
	itemRect = { 0,0,100,100 };
	playerRect = { 0,0,100,100 };
	
	I_Drop = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 71, "DROP", {650,1000,150,100}, this);
	I_Equip = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 72, "EQUIP", {850,1000,150,100}, this);
	I_Close = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 73, "CLOSE", { 1050,1000,150,100 }, this);

	I_Drop->state = GuiControlState::DISABLED;
	I_Equip->state = GuiControlState::DISABLED;
	I_Close->state = GuiControlState::DISABLED;

	isVisible = false;

	return true;
}

bool Inventory::PreUpdate()
{
	
	return true;
}

bool Inventory::Update(float dt)
{
	players = app->battleScene->party;
	
	if (app->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN && (app->levelManager->gameScene == GameScene::BACKSTAGE || app->levelManager->gameScene == GameScene::BACKSTAGE2 || app->levelManager->gameScene == GameScene::COMBAT))
	{
		isVisible = !isVisible;
	}

	if (isVisible)
	{
		I_Drop->state = GuiControlState::NORMAL;
		I_Equip->state = GuiControlState::NORMAL;
		I_Close->state = GuiControlState::NORMAL;
		
		if (InventoryItems.Count() > 0 && players.Count() > 0)
		{
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
			{
				selectedItem--;

				if (selectedItem < 0)
				{
					selectedItem = InventoryItems.Count() - 1;
				}
			}
			else if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			{
				selectedItem++;

				if (selectedItem >= InventoryItems.Count())
				{
					selectedItem = 0;
				}
			}

			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			{
				selectedPlayer++;

				if (selectedPlayer >= players.Count())
				{
					selectedPlayer = 0;
				}
			}
			else if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			{
				selectedPlayer--;

				if (selectedPlayer < 0)
				{
					selectedPlayer = players.Count() - 1;
				}
			}
		}
	}
	else
	{
		I_Drop->state = GuiControlState::DISABLED;
		I_Equip->state = GuiControlState::DISABLED;
		I_Close->state = GuiControlState::DISABLED;
	}

	return true;
}

bool Inventory::PostUpdate()
{
	if (isVisible)
	{
		app->render->DrawTexture(inventoryUI, app->render->camera.x / -3 + 50 , app->render->camera.y / -3 + 50, &inventoryRect);
		if (InventoryItems.Count() > 0 && players.Count() > 0)
		{
			app->render->DrawText(InventoryItems.At(selectedItem)->data->name, 300, 650, 150 * 2, 55 * 2);
			app->render->DrawTexture(InventoryItems.At(selectedItem)->data->texture, app->render->camera.x / -3 + 100, app->render->camera.y / -3 + 100, &itemRect);
			app->render->DrawText(players.At(selectedPlayer)->data->realname, 900, 650, 150 * 2, 55 * 2);
			app->render->DrawTexture(players.At(selectedPlayer)->data->myTexture, app->render->camera.x / -3 + 300, app->render->camera.y / -3 + 100, &playerRect);
		}
	}

	return true;
}

bool Inventory::CleanUp()
{
	
	return true;
}

void Inventory::DropItem(Item* item)
{
	InventoryItems.Del(InventoryItems.At(selectedItem));
}

void Inventory::EquipItem(Player* player, Item* item)
{
	if (player->myItem != nullptr)
	{
		InventoryItems.Add(player->myItem);
	}
	player->myItem = item;
	DropItem(item);
}

bool Inventory::OnGuiMouseClickEvent(GuiControl* control)
{
	if (control->id == 71 && InventoryItems.Count() > 0 && players.Count() > 0)
	{
		DropItem(InventoryItems.At(selectedItem)->data);
	}
	else if (control->id == 72 && InventoryItems.Count() > 0 && players.Count() > 0)
	{
		EquipItem(players.At(selectedPlayer)->data, InventoryItems.At(selectedItem)->data);
		DropItem(InventoryItems.At(selectedItem)->data);
	}
	else if (control->id == 73)
	{
		isVisible = false;
	}

	return true;
}