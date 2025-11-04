/*!
@author Rance Andres (andresrancerowell.g@digipen.edu)
@date 3 November 2025
@course CSD1401
@brief Definition file for particle system
*/

#include "particle_manager.h"
#include "asset_manager.h"
#include "utils.h"

ObjectArray particles;
ParticleData particle_types[PARTICLE_LAST];

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
}

static ParticleData create_particle_data(CP_Image root, int rows, int columns) {
	return (ParticleData){root,CP_Image_GetHeight(root),CP_Image_GetWidth(root),CP_Image_GetHeight(root)/rows,CP_Image_GetWidth(root)/columns,rows,columns};
}

void Particles_Preload(void) {
	particle_types[PARTICLE_FLAME] = create_particle_data(Asset_GetImage(FLAME_PARTICLE_SPRITE_ASSET), 2, 2);
	particle_types[PARTICLE_SPARKLE] = create_particle_data(Asset_GetImage(SPARKLE_SPRITE_ASSET), 5, 5);
	particle_types[PARTICLE_RAINBOW_SPARKLE] = create_particle_data(Asset_GetImage(RAINBOW_SPARKLE_SPRITE_ASSET), 2, 2);
	particle_types[PARTICLE_HALO] = create_particle_data(Asset_GetImage(HALO_SPRITE_ASSET), 8, 6);
	particle_types[PARTICLE_STAR] = create_particle_data(Asset_GetImage(YELLOW_STAR), 1, 1);
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

		// get the particle data of this type
		ParticleData* data = &particle_types[p->type];
		if (data == NULL) {
			Particles_Preload();
			data = &particle_types[p->type];
		}
		int frameCount = data->rows * data->columns;
		p->pos = CP_Vector_Add(p->pos, CP_Vector_Scale(p->vel, dt));

		// update frame
		if ((p->elapsed_dur += dt) > p->duration / frameCount) {
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

		ParticleData* data = &particle_types[p->type];
		CP_Vector coords = World_To_Screen(p->pos.x, p->pos.y, world_limits.x, world_limits.y);
		CP_Settings_Fill(Get_Color(COLOR_WHITE));
		CP_Settings_Stroke(Get_Color(COLOR_WHITE));
		CP_Settings_ImageMode(CP_POSITION_CENTER);
		CP_Settings_ImageFilterMode(CP_IMAGE_FILTER_LINEAR);
		
		// render similar to sprite sheet but individual time frame
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
}