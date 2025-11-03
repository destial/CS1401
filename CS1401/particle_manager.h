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
#include "sprite_manager.h"

typedef enum ParticleTypeEnum {
	PARTICLE_FLAME = 0,
	PARTICLE_SPARKLE,
	PARTICLE_RAINBOW_SPARKLE,
	PARTICLE_HALO,
	PARTICLE_LAST
} ParticleType;

typedef struct ParticleDataStruct {
	CP_Image root_image; // the root image of the particle
	// impt const values
	int img_h, img_w, sub_h, sub_w, rows, columns;
} ParticleData;

typedef struct ParticleStruct {
	ParticleType type; // the type of this particle
	CP_Vector pos, vel; // the pos and velocity of this particle
	int current_row, current_column; // the current frame row & column
	float elapsed_dur, duration, scale; // the lifespan & scale of this particle
} Particle;

/*!
@brief Preload particles into the memory
*/
void Particles_Preload(void);

/*!
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
@brief Update the spawned particles per frame
@param dt - The frame delta time
*/
void Particles_Update(float dt);

/*!
@brief Render the spawned particles to the world
@param world_limits - The world size limits
@param world_ratios - The world size to screen ratios
*/
void Particles_Render(CP_Vector world_limits, CP_Vector world_ratios);

/*!
@brief Free the particles after using. !!! CALL IT PLEASEEE !!!
*/
void Particles_Free(void);

#endif