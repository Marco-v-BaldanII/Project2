#ifndef __DOOR_H__
#define __DOOR_H__

#include "../Entity.h"
#include "../Point.h"
#include "SDL/include/SDL.h"
#include <string>

struct SDL_Texture;
using namespace std;




class Door
{
public:

	Door();
	virtual ~Door();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool PostUpdate();

	bool CleanUp();

	void InitModifiers(int x, int y, int num, const char* textPath);
	

public:

	bool isOpen = false;

	iPoint mapPos;

	int num;

	iPoint tilePos;

private:

	SDL_Texture* texture = nullptr;
	SDL_Texture* texture2 = nullptr;
	const char* texturePath;
	uint texW, texH;


};

#endif // __DOOR_H__