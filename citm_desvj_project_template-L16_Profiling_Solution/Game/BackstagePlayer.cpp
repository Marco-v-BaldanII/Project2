#include "BackstagePlayer.h"
#include "Source/Log.h"

BackStagePlayer::BackStagePlayer(iPoint pos) : Entity(EntityType::BACKSTAGEPLAYER)
{
	name.Create("BackStagePlayer");
	position = pos;
}

BackStagePlayer::~BackStagePlayer()
{
}

bool BackStagePlayer::Start()
{
	// Set player texture
	playerTexture = app->tex->Load("Assets/Textures/player1.png");
	
	// Load animations
	walkSpeed = 0.1f;
	//walk
	walkUp.PushBack({ 0, 0, 32, 32 });
	//mas
	walkUp.loop = true;
	walkUp.speed = walkSpeed;

	walkDown.PushBack({ 0, 0, 32, 32 });
	//mas
	walkDown.loop = true;
	walkDown.speed = walkSpeed;

	walkLeft.PushBack({ 0, 0, 32, 32 });
	//mas
	walkLeft.loop = true;
	walkLeft.speed = walkSpeed;

	walkRight.PushBack({ 0, 0, 32, 32 });
	//mas
	walkRight.loop = true;
	walkRight.speed = walkSpeed;


	//idle
	idleUp.PushBack({ 0, 0, 32, 32 });
	//mas
	idleUp.loop = true;
	idleUp.speed = walkSpeed;

	idleDown.PushBack({ 0, 0, 32, 32 });
	//mas
	idleDown.loop = true;
	idleDown.speed = walkSpeed;

	idleLeft.PushBack({ 0, 0, 32, 32 });
	//mas
	idleLeft.loop = true;
	idleLeft.speed = walkSpeed;

	idleRight.PushBack({ 0, 0, 32, 32 });
	//mas
	idleRight.loop = true;
	idleRight.speed = walkSpeed;

	currentAnimation = &idleDown;

	// Set player direction
	goingLeft = false;
	goingRight = false;
	goingUp = false;
	goingDown = false;

	// Set god mode
	godMode = false;

	// Set player movement
	canMove = true;

	// Set player velocity
	velocity = 0.0f;

	// Set player steps
	fxSteps = app->audio->LoadFx("Assets/Audio/Fx/Steps.wav");

	return true;
}

bool BackStagePlayer::PreUpdate()
{
	// Set player position
	this->SetPlayerPosition(position.x, position.y);

	// Set player velocity
	velocity = 0.15f;

	// Set player movement
	

	return true;
}

bool BackStagePlayer::Update(float dt)
{
	// Set player position
	if (canMove)
	{
		// Set player direction
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			position.y -= velocity * dt;
			goingLeft = false;
			goingUp = true;
			goingDown = false;
			goingRight = false;
			isMoving = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			position.y += velocity * dt;
			goingLeft = false;
			goingUp = false;
			goingDown = true;
			goingRight = false;
			isMoving = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			position.x -= velocity * dt;
			goingLeft = true;
			goingUp = false;
			goingDown = false;
			goingRight = false;
			isMoving = true;
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			position.x += velocity * dt;
			goingLeft = false;
			goingUp = false;
			goingDown = false;
			goingRight = true;
			isMoving = true;
		}
		else
		{
			isMoving = false;
		}
	}

	// Set player animation
	if (isMoving)
	{
		if (goingUp)
		{
			currentAnimation = &walkUp;
		}
		else if (goingDown)
		{
			currentAnimation = &walkDown;
		}
		else if (goingLeft)
		{
			currentAnimation = &walkLeft;
		}
		else if (goingRight)
		{
			currentAnimation = &walkRight;
		}
	}
	else
	{
		if (goingUp)
		{
			currentAnimation = &idleUp;
		}
		else if (goingDown)
		{
			currentAnimation = &idleDown;
		}
		else if (goingLeft)
		{
			currentAnimation = &idleLeft;
		}
		else if (goingRight)
		{
			currentAnimation = &idleRight;
		}
	}

	return true;
}

bool BackStagePlayer::PostUpdate()
{
	// Set player position
	//SetPosition(position.x, position.y);

	// Set all player animations
	//IMPORTANTE

	//Render player
	app->render->DrawTexture(playerTexture, position.x, position.y);

	//camera follow player
	//app->render->camera.x = (-position.x + SCREEN_WIDTH / 2);
	//app->render->camera.y = (-position.y + SCREEN_HEIGHT / 2);
	//

	////camera boundaries in backStage
	//if (app->render->camera.x > 0)
	//{
	//	app->render->camera.x = 0;
	//}
	//if (app->render->camera.y > 0)
	//{
	//	app->render->camera.y = 0;
	//}

	//player boundaries in backStage
	if (position.x < 0)
	{
		position.x = 0;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}
	if (position.x > 3200)
	{
		position.x = 3200;
	}
	if (position.y > 3200)
	{
		position.y = 3200;
	}


	return true;
}

bool BackStagePlayer::CleanUp()
{
	return true;
}

void BackStagePlayer::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	switch (bodyB->ctype)
	{
	case ColliderType::PLATFORM:
		LOG("Collision PLATFORM");
		break;
	case ColliderType::ITEM:
		LOG("Collision ITEM");
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}

void BackStagePlayer::SetPlayerPosition(iPoint pos)
{
	position = pos;
}

void BackStagePlayer::SetPlayerPosition(int x, int y)
{
	position.x = x;
	position.y = y;
}