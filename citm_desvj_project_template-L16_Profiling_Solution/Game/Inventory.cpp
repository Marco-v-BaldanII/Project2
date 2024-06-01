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

	inventoryUI = app->tex->Load("Assets/Textures/inventoryUI.png");
	descriptionTex = app->tex->Load("Assets/Textures/UI/inventoryDescription.png");
	inventoryRect = { 0,0,410,250 };
	itemRect = { 0,0,100,100 };
	playerRect = { 0,0,120,100 };
	
	I_Drop = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 71, "DROP", {490,930 - (int)menuY,150,100}, this);
	I_Equip = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 72, "EQUIP", {690,930 - (int)menuY,150,100}, this);
	I_Close = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 73, "CLOSE", { 890,930 - (int)menuY,150,100 }, this);

	I_Drop->state = GuiControlState::DISABLED;
	I_Equip->state = GuiControlState::DISABLED;
	I_Close->state = GuiControlState::DISABLED;

	changeitemfx = app->audio->LoadFx("Assets/Audio/Fx/EquipItemFX.ogg");

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
		selectedItem = 0;
	}

	if (app->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
	{
		Item* item = new Item();
		item->name = "Test Item";
		item->texture = app->tex->Load("Assets/Textures/Items/veil.png");
		InventoryItems.Add(item);
	}

	if (app->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN)
	{
		Item* item = new Item();
		item->name = "Test Item 2";
		item->texture = app->tex->Load("Assets/Textures/Items/veil.png");
		InventoryItems.Add(item);
	}

	if (isVisible)
	{
		if (menuY > 0) menuY -= animationSpeed;
		if (menuY <= 0) menuY = 0;

		I_Drop->state = GuiControlState::NORMAL;
		I_Equip->state = GuiControlState::NORMAL;
		I_Close->state = GuiControlState::NORMAL;
		
		if (InventoryItems.Count() > 0)
		{
			if (app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
			{
				selectedItem--;
				app->audio->PlayFx(changeitemfx);
			}
			else if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
			{
				selectedItem++;
				app->audio->PlayFx(changeitemfx);
			}
		}
		if (players.Count() > 0)
		{
			if (app->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN)
			{
				selectedPlayer++;
				app->audio->PlayFx(changeitemfx);
			}
			else if (app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN)
			{
				selectedPlayer--;
				app->audio->PlayFx(changeitemfx);
			}
		}

		if (selectedItem < 0) selectedItem = InventoryItems.Count() - 1;
		if (selectedItem >= InventoryItems.Count()) selectedItem = 0;

		if (selectedPlayer < 0) selectedPlayer = players.Count() - 1;
		if (selectedPlayer >= players.Count()) selectedPlayer = 0;
	}
	else
	{
		if (menuY < 1200) menuY += animationSpeed;
		if (menuY >= 1200) menuY = 1200;
		
		I_Drop->state = GuiControlState::DISABLED;
		I_Equip->state = GuiControlState::DISABLED;
		I_Close->state = GuiControlState::DISABLED;
	}

	//update the position of the buttons
	I_Drop->bounds = { 490,930 - (int)menuY,150,100 };
	I_Equip->bounds = { 690,930 - (int)menuY,150,100 };
	I_Close->bounds = { 890,930 - (int)menuY,150,100 };

	return true;
}

bool Inventory::PostUpdate()
{
	if (isVisible)
	{
		app->render->DrawTexture(inventoryUI, app->render->camera.x / -3 + 50 , app->render->camera.y / -3 + 50 - (int)menuY, &inventoryRect);
		if (InventoryItems.At(selectedItem) != nullptr && InventoryItems.At(selectedItem)->data != nullptr)
		{
			app->render->DrawText(InventoryItems.At(selectedItem)->data->name, 300, 660 - (int)menuY, 150 * 2, 53 * 2);
			app->render->DrawTexture(InventoryItems.At(selectedItem)->data->texture, app->render->camera.x / -3 + 100, app->render->camera.y / -3 + 100 - (int)menuY, &itemRect);

			app->render->DrawTexture(descriptionTex, 213 + (app->render->camera.x /-3), 70 + (app->render->camera.y / -3) - (int)menuY);
			app->render->DrawText(InventoryItems.At(selectedItem)->data->description, 663, 250 - (int)menuY, 1.5f * InventoryItems.At(selectedItem)->data->description.length(), 20, false, SDL_Color{79,62,43,255}, LEFT_ALIGN, 15);
		}

		if (players.At(selectedPlayer) != nullptr && players.At(selectedPlayer)->data != nullptr)
		{
			app->render->DrawText(players.At(selectedPlayer)->data->realname, 940, 660 - (int)menuY, 150 * 2, 53 * 2);
			app->render->DrawTexture(players.At(selectedPlayer)->data->myBattleTexture, app->render->camera.x / -3 + 300, app->render->camera.y / -3 + 70 - (int)menuY, &playerRect);
		}
	}

	return true;
}

bool Inventory::CleanUp()
{
	app->tex->UnLoad(inventoryUI);
	app->tex->UnLoad(descriptionTex);
	for (int i = 0; i < InventoryItems.Count(); i++)
	{
		app->tex->UnLoad(InventoryItems.At(i)->data->texture);
		DeleteItemAndReorderInventory(InventoryItems.At(i)->data);
	}
	InventoryItems.Clear();

	return true;
}

void Inventory::EquipItem(Player* player, Item* item) {
	if (!item || !player) {
		return;
	}

	if (player->myItem != nullptr)
	{
		Item* oldItem = player->myItem;

		player->myItem = nullptr;

		InventoryItems.Add(oldItem);
	}

	player->myItem = item;
	DeleteItemAndReorderInventory(item);
}


void Inventory::DeleteItemAndReorderInventory(Item* item)
{
	for (int i = 0; i < InventoryItems.Count(); i++)
	{
		if (InventoryItems.At(i)->data == item)
		{
			InventoryItems.Del(InventoryItems.At(i));
			break;
		}
	}

	InventoryItems.BubbleSort();
}


bool Inventory::OnGuiMouseClickEvent(GuiControl* control)
{
	
	if (InventoryItems.Count() > 0 && players.Count() > 0)
	{
		if (control->id == 71)
		{
			DeleteItemAndReorderInventory(InventoryItems.At(selectedItem)->data);
		}
		else if (control->id == 72)
		{
			EquipItem(players.At(selectedPlayer)->data, InventoryItems.At(selectedItem)->data);
			/*if (selectedItem < InventoryItems.Count())
			{
				DeleteItemAndReorderInventory(InventoryItems.At(selectedItem)->data);
			}*/
		}
	}
	
	if (control->id == 73)
	{
		isVisible = false;
	}

	return true;
}