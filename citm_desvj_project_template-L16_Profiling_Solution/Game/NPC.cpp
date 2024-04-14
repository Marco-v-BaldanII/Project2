#include "NPC.h"
#include "../App.h"
#include "../Textures.h"
#include "../Audio.h"
#include "../Input.h"
#include "../Render.h"
#include "../Scene.h"
#include "../Log.h"
#include "../Point.h"
#include "../Physics.h"
#include "../Map.h"
#include "random.h"
#include "BackstagePlayer.h"
#include "../Timer.h"

Npc::Npc(string name, int x, int y, int wait) : Entity(EntityType::NPC)
{
	this->name = name;
	position.x = x;
	position.y = y;
	waitTime = wait;
	//name.Create("Npc");
}

Npc::~Npc() {

}

bool Npc::Awake() {

	return true;
}

bool Npc::Start() {

	dirTimer.Start();

	return true;
}

bool Npc::Update(float dt)
{
	//LOG(name.c_str());

	centerPos = position;
	centerPos.x += 54; centerPos.y += 95;


	int mouseX, mouseY;

	app->input->GetMousePosition(mouseX, mouseY);

	//If the position of the mouse if inside the bounds of the box 
	//if (mouseX > body.x && mouseX < body.x + body.w && mouseY > body.y && mouseY < body.y + body.h) {

	//	if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {
	//		
	//		// Code for talking
	//		app->dialogueManager->npcTalk(myDialogues);
	//	}
	//}

	if (dirTimer.ReadMSec() > waitTime*1000) {

		dirTimer.Start();

		int rand = getRandomNumber(0, 2);
		switch (rand) {
		case(0):
			myMovement = MOVEMENT::LEFT;
			break;
		case(1):
			myMovement = MOVEMENT::RIGHT;
			break;
		case(2):
			myMovement = MOVEMENT::WAIT;
			break;
		}

	}
	
	if (app->backstageplayer->talking == false) {
		Walk();
	}

	return true;
}

bool Npc::PostUpdate()
{
	if(myMovement == MOVEMENT::RIGHT) app->render->DrawTexture(texture, position.x, position.y, &body, false );
	else {
		app->render->DrawTexture(texture, position.x, position.y, &body, true);
	}
	
	return true;
}

bool Npc::CleanUp()
{
	return true;
}

// L07 DONE 6: Define OnCollision function for the player. 
void Npc::OnCollision(PhysBody* physA, PhysBody* physB) {
	/*switch (physB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		app->audio->PlayFx(pickCoinFxId);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}*/
}

void Npc::Walk() {
	// Incredibly basic movement pattern

	int rand = getRandomNumber(0, 1);

	if (rand == 0) {
		if (myMovement == LEFT) {
			if (position.x > 0)		position.x--;
		}
		else if (myMovement == RIGHT) {
			if (position.x < 250)  position.x++;
		}
	}

}