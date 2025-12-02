/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Declaration file for particle system
*/

#pragma once
#ifndef PARTICLE_MANAGER_H
#define PARTICLE_MANAGER_H
#include "cprocessing.h"

typedef enum ParticleTypeEnum {
	PARTICLE_SPARKLE = 0,
	PARTICLE_FLAME,
	PARTICLE_LIGHTNING,
	PARTICLE_STAR,
	PARTICLE_PORTAL,
	PARTICLE_LAST
} ParticleType;

typedef struct ParticleDataStruct ParticleData;

typedef struct ParticleStruct Particle;

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Preload particles into the memory
*/
void Particles_Preload(void);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Spawn particles of this type into this location.
@param type - Particle type
@param pos - The center position in world space
@param amount - The amount of particles to spawn
@param radius - The radius of the spawned particles
@param speed - The speed of the particles (velocity magnitude)
@param duration - The lifespan of these particles
@param minSize - The minimum size of the particles
@param maxSize - The maximum size of the particles
*/
void Particles_Spawn(ParticleType type, CP_Vector pos, int amount, float radius, float speed, float duration, float minSize, float maxSize);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Update the spawned particles per frame
@param dt - The frame delta time
*/
void Particles_Update(float dt);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Render the spawned particles to the world
@param world_limits - The world size limits
@param world_ratios - The world size to screen ratios
*/
void Particles_Render(CP_Vector world_limits, CP_Vector world_ratios);

/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@brief Free the particles after using. !!! CALL IT PLEASEEE !!!
*/
void Particles_Free(void);

#endif