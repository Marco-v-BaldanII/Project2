#include "../Item.h"
#include "Door.h"
#include "Lever.h"
#include "Particle.h"
#include <list>
#include "ParticleSystem.h"

using namespace std;

ParticleSystem::ParticleSystem(bool isActive) : Module(isActive)
{
}


bool ParticleSystem::Awake(pugi::xml_node config) {


	return true;
};


void ParticleSystem::AddParticleEffect(ParticleEffect* effect) {

	pEffects.Add(effect);

}

void ParticleSystem::RemoveParticleEffect(ParticleEffect* effect) {

	for (ListItem<ParticleEffect*>* pE = pEffects.start; pE != NULL; pE = pE->next) {

		if (pE->data == effect) {

			pEffects.Del(pE);
			break;
		}

	}


}

// CaParticleSystem::lled after Awake
bool ParticleSystem::Start() {



	return true;
};
bool ParticleSystem::PreUpdate() {


	return true;
};
// CaParticleSystem::lled every frame
bool ParticleSystem::Update(float dt) {

	for (ListItem<ParticleEffect*>* pIt = pEffects.start; pIt != NULL; pIt = pIt->next) {

		
		ParticleEffect* Effect = pIt->data;

		if (Effect->active) {
			bool alive = Effect->Update(dt);
		}
		
	

	}

	return true;
};
bool ParticleSystem::PostUpdate() {

	for (ListItem<ParticleEffect*>* pIt = pEffects.start; pIt != NULL; pIt = pIt->next) {

		ParticleEffect* Effect = pIt->data;

		Effect->Render();



	}
	return true;
};
// CaParticleSystem::lled before quitting
bool ParticleSystem::CleanUp() {


	return true;
};
