#include "BackstagePlayer.h"
#include "Source/Log.h"

BackStagePlayer::BackStagePlayer(bool isActive) : Module(isActive)
{
}

BackStagePlayer::~BackStagePlayer()
{

}

bool BackStagePlayer::Start()
{
	// Set player texture
	if (active)
	{
		playerTexture = app->tex->Load("Assets/Textures/player2.png");

		// Load animationss
		walkSpeed = 0.1f;
		//walk
		walkUp.PushBack({ 3, 67, 21, 23 });
		//mas
		walkUp.loop = true;
		walkUp.speed = walkSpeed;

		walkDown.PushBack({ 3, 15, 21, 23 });
		//mas
		walkDown.loop = true;
		walkDown.speed = walkSpeed;

		walkLeft.PushBack({ 3, 42, 21, 23 });
		//mas
		walkLeft.loop = true;
		walkLeft.speed = walkSpeed;

		walkRight.PushBack({ 3, 0, 21, 23 });
		//mas
		walkRight.loop = true;
		walkRight.speed = walkSpeed;


		//idle
		idleUp.PushBack({ 3, 0, 21, 23 });
		//mas
		idleUp.loop = true;
		idleUp.speed = walkSpeed;

		idleDown.PushBack({ 3, 0, 21, 23 });
		//mas
		idleDown.loop = true;
		idleDown.speed = walkSpeed;

		idleLeft.PushBack({ 3, 0, 21, 23 });
		//mas
		idleLeft.loop = true;
		idleLeft.speed = walkSpeed;

		idleRight.PushBack({ 3, 0, 21, 23 });
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
	}

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
		SDL_RendererFlip flip = lastDirection;
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
			LOG("MOVING DOWN");
		}
		else if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			position.x -= velocity * dt;
			flipHorizontal = SDL_FLIP_HORIZONTAL;
			goingLeft = true;
			goingUp = false;
			goingDown = false;
			goingRight = false;
			isMoving = true;
			LOG("MOVING LEFT");
		}
		else if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			position.x += velocity * dt;
			flipHorizontal = SDL_FLIP_NONE;
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

		lastDirection = flipHorizontal;
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
		else
		{
			currentAnimation = &idleDown;
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
	SDL_RendererFlip flips = (SDL_RendererFlip)(flipHorizontal);
	SDL_Rect currentFrame = currentAnimation->GetCurrentFrame();
	SDL_Rect destRect = { position.x - 5, position.y - 8, currentFrame.w, currentFrame.h };
	app->render->DrawTexture(playerTexture, destRect.x, destRect.y, &currentFrame, flips);

	//camera follow player
	app->render->camera.x = (-position.x + SCREEN_WIDTH / 2);
	app->render->camera.y = (-position.y + SCREEN_HEIGHT / 2);
	//

	//camera boundaries in backStage
	if (app->render->camera.x > 0)
	{
		app->render->camera.x = 0;
	}
	if (app->render->camera.y > 0)
	{
		app->render->camera.y = 0;
	}
	/*if (app->render->camera.x < 30000)
	{
		app->render->camera.x = 30000;
	}
	if (app->render->camera.y < 30000)
	{
		app->render->camera.y = 30000;
	}*/

	//player boundaries in backStage
	if (position.x < 0)
	{
		position.x = 0;
	}
	if (position.y < 0)
	{
		position.y = 0;
	}
	if (position.x > 680)
	{
		position.x = 680;
	}
	if (position.y > 384)
	{
		position.y = 384;
	}


	return true;
}

bool BackStagePlayer::CleanUp()
{
	return true;
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