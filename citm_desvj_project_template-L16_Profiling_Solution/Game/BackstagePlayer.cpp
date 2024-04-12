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
		playerTexture = app->tex->Load("Assets/Textures/BackstageNPCs/PersonajeWOTR resized.png");

		// Load animationss
		walkSpeed = 0.1f;

		//walk
		walkUp.PushBack({ 8, 295, 82, 120 });

		// walk up loop
		walkUp.PushBack({ 112, 296, 76, 116 });
		walkUp.PushBack({ 216, 294, 72, 118 });
		walkUp.PushBack({ 314, 292, 70, 120 });
		walkUp.PushBack({ 412, 294, 70, 118 });
		walkUp.PushBack({ 512, 292, 72, 120 });
		

		//mas
		walkUp.loop = true;
		walkUp.speed = walkSpeed;

		walkDown.PushBack({ 8, 434, 84, 122 });
		// walk down loop
		walkDown.PushBack({ 112, 432, 74, 124 });
		walkDown.PushBack({ 216, 430, 66, 126 });
		walkDown.PushBack({ 316, 432, 72, 122 });
		walkDown.PushBack({ 416, 434, 72, 122 });
		walkDown.PushBack({ 518, 432, 66, 124 });
		walkDown.PushBack({ 616, 434, 66, 122 });
		

		//mas
		walkDown.loop = true;
		walkDown.speed = walkSpeed;

		walkLeft.PushBack({ 22, 154, 54, 122 });
		 //walk left loop
		walkLeft.PushBack({ 120, 150, 58, 126 });
		walkLeft.PushBack({ 220, 148, 60, 128 });
		walkLeft.PushBack({ 322, 156, 56, 120 });
		walkLeft.PushBack({ 422, 154, 56, 122 });
		walkLeft.PushBack({ 520, 152, 60, 124 });
		walkLeft.PushBack({ 618, 158, 60, 120 });
		walkLeft.PushBack({ 724, 156, 56, 120 });
		
		//mas
		walkLeft.loop = true;
		walkLeft.speed = walkSpeed;

		walkRight.PushBack({ 22, 14, 54, 122 });
		// walk rigth loop
		walkRight.PushBack({ 116, 10, 58, 126 });
		walkRight.PushBack({ 214, 8, 60, 128 });
		walkRight.PushBack({ 316, 16, 56, 120 });
		walkRight.PushBack({ 418, 14, 54, 122 });
		walkRight.PushBack({ 514, 12, 60, 124 });
		walkRight.PushBack({ 616, 18, 60, 120 });
		walkRight.PushBack({ 714, 16, 56, 120 });
		

		//mas
		walkRight.loop = true;
		walkRight.speed = walkSpeed;


		//idle
		idleUp.PushBack({ 8, 295, 82, 120 });
		//mas
		idleUp.loop = true;
		idleUp.speed = walkSpeed;

		idleDown.PushBack({ 8, 434, 84, 122 });
		//mas
		idleDown.loop = true;
		idleDown.speed = walkSpeed;

		idleLeft.PushBack({ 22, 154, 54, 122 });
		//mas
		idleLeft.loop = true;
		idleLeft.speed = walkSpeed;

		idleRight.PushBack({ 22, 14, 54, 122 });
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
	currentAnimation->Update();
	// Set player position
	if (canMove && !talking)
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
	//app->render->camera.y = (-position.y + SCREEN_HEIGHT / 2 * SCREEN_SIZE);
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
	if (position.x < 30)
	{
		position.x = 30;
	}
	if (position.y < 120)
	{
		position.y = 120;
	}
	if (position.x > 580)
	{
		position.x = 580;
	}
	if (position.y > 264)
	{
		position.y = 264;
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

