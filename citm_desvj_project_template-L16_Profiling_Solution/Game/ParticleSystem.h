#ifndef __PARTICLESYSTEM_H__
#define __PARTICLESYSTEM_H__

#include "../Module.h"
#include "../Entity.h"
#include "../List.h"
#include <string>
#include "../p2List.h"
#include "../Player.h"
#include "../Item.h"
#include "Door.h"
#include "Lever.h"
#include "Particle.h"
#include <list>
#include "../GuiManager.h"

using namespace std;

enum RenderLayer {

	FRONT,
	BACK

};



class ParticleEffect {

public:

	ParticleEffect(Particle* baseParticle, uint poolSize, float spawnRate,bool loop, bool flipped = false, SDL_Rect spawnBox = SDL_Rect{0,0,0,0}, bool animated = false, RenderLayer renderLayer = FRONT, VIEW v = SCREEN) {

		this->baseParticle = baseParticle;
		this->poolSize = poolSize;
		this->spawnRate = spawnRate;
		this->spawnBox = spawnBox;
		this->loop = loop;
		this->animated = animated;
		this->flipped = flipped;
		this->drawLayer = renderLayer;
		overlayPos = v;

		counter = poolSize;

		for (int i = poolSize; i > 0; --i) {
			Particle* part = new Particle(baseParticle);

			myParticles.Add(part);

		}
		myTimer.Start();
	}

	bool Update(float dt) {

		// check ttiem to activate more particles
		if (myTimer.ReadMSec() >= spawnRate*1000) {
			for (ListItem<Particle*>* pIt = myParticles.start; pIt != NULL; pIt = pIt->next) {

				if (pIt->data->active == false) {
					pIt->data->ReActivate(spawnBox);
					counter--;
					break;
				}

			}
			myTimer.Start();
		}


		for (ListItem<Particle*>* pIt = myParticles.start; pIt != NULL; pIt = pIt->next) {
			if (pIt->data != NULL && pIt->data->active && pIt->data->CheckToDie() == false) {
				Particle* p = pIt->data;

				p->position.x += p->velocity.x +((float) p->currentSpeedXVariation/5.0f )* dt/10;
				p->position.y += p->velocity.y * dt/10;

				p->velocity.x += p->acceleration.x * dt/10;
				p->velocity.y += p->acceleration.y * dt/10;


			}
		}


		// If the effect is not loopeable when the pool has finished deactivate this effect

		if (!loop && counter < 0) {
			active = false;
			return false;
		}


		return true;
	}

	bool Render() {
		if (active) {
			for (ListItem<Particle*>* pIt = myParticles.start; pIt != NULL; pIt = pIt->next) {
				if (pIt->data->active) {
					Particle* particle = pIt->data;

					SDL_Rect r = particle->rect;
					r.w *= (particle->currentVariation / 10); r.h *= particle->currentVariation / 10;
					if (!animated) {
						if (flipped) {
							if (overlayPos == SCREEN) {
								app->render->DrawTexture(particle->texture, particle->position.x + app->render->camera.x / -3, particle->position.y + app->render->camera.y / -3, &r, true, particle->currentAlpha);
							}
							else { app->render->DrawTexture(particle->texture, particle->position.x , particle->position.y , &r, true, particle->currentAlpha); }	
						}
						else {
							if (overlayPos == SCREEN) {
								app->render->DrawTexture(particle->texture, particle->position.x + app->render->camera.x / -3, particle->position.y + app->render->camera.y / -3, &r, false, particle->currentAlpha);
							}
							else { app->render->DrawTexture(particle->texture, particle->position.x , particle->position.y, &r, false, particle->currentAlpha); }
						}
					}
					else {
						particle->currentAnim->Update();
						if (flipped) {
							app->render->DrawTexture(particle->texture, particle->position.x + app->render->camera.x / -3, particle->position.y + app->render->camera.y / -3, &particle->currentAnim->GetCurrentFrame(), true, particle->currentAlpha);
						}
						else {
							app->render->DrawTexture(particle->texture, particle->position.x + app->render->camera.x / -3, particle->position.y + app->render->camera.y / -3, &particle->currentAnim->GetCurrentFrame(), false, particle->currentAlpha);
						}
					}
				}
			}
		}
		return true;

	}

	void Restart() {
		counter = poolSize;
		active = true;
	}

	~ParticleEffect() {
		CleanUp();
	}

	void CleanUp() {

		myParticles.Clear();
		delete baseParticle;
	}

	RenderLayer drawLayer;
	VIEW overlayPos;

	List<Particle*> myParticles;

	Particle* baseParticle;

	uint poolSize;
	float spawnRate;

	Timer myTimer;

	SDL_Rect spawnBox;

	bool loop;
	int counter;
	bool  active = true;
	bool animated;
	bool flipped;

};

class ParticleSystem : public Module
{
public:
	ParticleSystem(bool isActive);


	// Destructor


	// Called before render is available
	bool Awake(pugi::xml_node config);

	// Called after Awake
	bool Start();
	bool PreUpdate() override;
	// Called every frame
	bool Update(float dt);
	bool PostUpdate() override;
	// Called before quitting
	bool CleanUp();


	// Additional methods

	void AddParticleEffect(ParticleEffect* effect);
	void RemoveParticleEffect(ParticleEffect* effect);

	pugi::xml_node myNode;
	
	List<ParticleEffect*> pEffects;


private:
	bool Active = false;


};

#endif // __PARTICLESYSTEM_H__
