/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Definition file for particle system
*/

#include "particle_manager.h"
#include "asset_manager.h"
#include "utils.h"

typedef struct ParticleDataStruct {
	CP_Image root_image; // the root image of the particle
	// impt const values
	unsigned int img_h, img_w, sub_h, sub_w, rows, columns;
} ParticleData;

typedef struct ParticleStruct {
	ParticleType type; // the type of this particle
	CP_Vector pos, vel; // the pos and velocity of this particle
	float elapsed_dur, duration, scale, og_scale; // the lifespan & scale of this particle
	unsigned int current_row, current_column; // the current frame row & column
} Particle;

static ObjectArray particles;
static ParticleData particle_types[PARTICLE_LAST];

// set the particle values at this particle pointer
static void set_particle(Particle* p, ParticleType type, CP_Vector pos, float radius, float speed, float duration, float minSize, float maxSize) {
	p->type = type;
	p->duration = duration;
	p->current_column = 0;
	p->current_row = 0;
	p->elapsed_dur = 0.f;
	p->vel = CP_Vector_Set(CP_Random_RangeFloat(-0.5f, 0.5f) * radius, CP_Random_RangeFloat(-0.5f, 0.5f) * radius);
	p->pos = CP_Vector_Add(p->vel, pos);
	p->vel = speed == 0.f ? CP_Vector_Zero() : CP_Vector_Scale(CP_Vector_Normalize(p->vel), speed);
	p->scale = CP_Random_RangeFloat(minSize, maxSize);
	p->og_scale = p->scale;
}

// create the always const particle data, this is only used internally and not exposed to the outside (!!!)
static ParticleData create_particle_data(CP_Image root, int rows, int columns) {
	return (ParticleData){root,CP_Image_GetHeight(root),CP_Image_GetWidth(root),CP_Image_GetHeight(root)/rows,CP_Image_GetWidth(root)/columns,rows,columns};
}

void Particles_Preload(void) {
	//particle_types[PARTICLE_FLAME] = create_particle_data(Asset_GetImage(FLAME_PARTICLE_SPRITE_ASSET), 2, 2);
	particle_types[PARTICLE_SPARKLE] = create_particle_data(Asset_GetImage(SPARKLE_SPRITE_ASSET), 3, 3);
	//particle_types[PARTICLE_RAINBOW_SPARKLE] = create_particle_data(Asset_GetImage(RAINBOW_SPARKLE_SPRITE_ASSET), 2, 2);
	//particle_types[PARTICLE_HALO] = create_particle_data(Asset_GetImage(HALO_SPRITE_ASSET), 8, 6);
	//particle_types[PARTICLE_STAR] = create_particle_data(Asset_GetImage(YELLOW_STAR), 1, 1);
}

void Particles_Spawn(ParticleType type, CP_Vector pos, int amount, float radius, float speed, float duration, float minSize, float maxSize) {
	// if empty array, create new one and load in
	if (!particles.arr) {
		particles.length += amount;
		particles.arr = gc_malloc(sizeof(Particle) * particles.length);
		for (int i = 0; i < amount; ++i) {
			set_particle(&((Particle*)particles.arr)[i], type, pos, radius, speed, duration, minSize, maxSize);
		}
		return;
	}
	
	// particle pooling, reuse those that have finished updating
	int index;
	for (index = 0; index < particles.length && amount > 0; ++index) {
		Particle* p = &((Particle*)particles.arr)[index];
		if (p->elapsed_dur < 0.f) {
			set_particle(p, type, pos, radius, speed, duration, minSize, maxSize);
			--amount;
		}
	}
	if (amount <= 0)
		return;

	// resize and add to the back of the array if too much
	particles.length += amount;
	particles.arr = gc_realloc(particles.arr, sizeof(Particle) * particles.length);
	for (int i = 0; i < amount; ++i) {
		Particle* p = &((Particle*)particles.arr)[i + index];
		set_particle(p, type, pos, radius, speed, duration, minSize, maxSize);
	}
}

void Particles_Update(float dt) {
	for (int i = 0; i < particles.length; ++i) {
		Particle* p = &((Particle*)particles.arr)[i];
		if (p->elapsed_dur < 0.f)
			continue;

		// get the particle data information of this type
		ParticleData* data = &particle_types[p->type];
		if (!data || !data->root_image) {
			Particles_Preload();
			data = &particle_types[p->type];
		}
		int frameCount = data->rows * data->columns;
		float edt = p->duration / (float)frameCount;

		// move the particles and fade them away after time
		p->pos = CP_Vector_Add(p->pos, CP_Vector_Scale(p->vel, dt));
		p->scale -= (p->og_scale / (float)frameCount) * dt;

		// update the current frame
		if ((p->elapsed_dur += dt) > edt) {
			p->elapsed_dur = 0.f;
			if (++p->current_column >= data->columns) {
				p->current_column = 0;

				if (++p->current_row >= data->rows) {
					p->elapsed_dur = -1.f;
				}
			}
		}
	}
}

void Particles_Render(CP_Vector world_limits, CP_Vector world_ratios) {
	for (int i = 0; i < particles.length; ++i) {
		Particle* p = &((Particle*)particles.arr)[i];
		if (p->elapsed_dur < 0.f)
			continue;

		// get the particle data information of this type
		ParticleData* data = &particle_types[p->type];
		CP_Vector coords = World_To_Screen(p->pos.x, p->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Settings_Stroke(Get_Color(COLOR_WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		
		// render similar to sprite sheet but per particle time frame
		int u0 = p->current_column * data->sub_w;
		int v0 = p->current_row * data->sub_h;
		CP_Image_DrawSubImageAdvanced(data->root_image, 
			coords.x, coords.y, 
			p->scale * world_ratios.x,
			p->scale * world_ratios.y, 
			(float) u0, (float) v0, 
			(float) (u0 + data->sub_w), (float) (v0 + data->sub_h), 
			255, 0.f);
	}
}

void Particles_Free(void) {
	FREE_MEMORY(particles.arr);
	particles.length = 0;
	for (int i = 0; i < PARTICLE_LAST; ++i) {
		particle_types[i] = (ParticleData) { 0 };
	}
}