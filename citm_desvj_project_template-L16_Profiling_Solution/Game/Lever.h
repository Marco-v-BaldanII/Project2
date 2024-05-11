#ifndef __LEVER_H__
#define __LEVER_H__

#include "../Entity.h"
#include "../Point.h"
#include "SDL/include/SDL.h"
#include <string>

struct SDL_Texture;
using namespace std;




class Lever
{
public:

	Lever();
	virtual ~Lever();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void InitModifiers(int x, int y, int num, const char* textPath);


public:

	bool isPressed = false;

	iPoint mapPos;
	const char* ownedDoor;

	int num;

	iPoint tilePos;

private:

	SDL_Texture* texture = nullptr;
	SDL_Texture* texture2 = nullptr;
	const char* texturePath;
	uint texW, texH;


};

#endif // __DOOR_H__