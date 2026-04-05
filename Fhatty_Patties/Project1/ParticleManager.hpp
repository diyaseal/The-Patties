// ---------------------------------------------------------------------------
// ParticleManager.hpp - Header File for ParticleManager Functionality
// ---------------------------------------------------------------------------
#pragma once

// Particle State Functions
void Particle_Load();
void Particle_Init();
void Particle_Update(float dt);
void Particle_Draw();
void Particle_Free();
void Particle_Unload();

// Set Particle Functions
void Particle_SetSmoke(float truckX, float truckY, bool truckLanded, float spawnInterval, float sz);
void Particle_SetStarBurst(float x, float y, int starIndex);

// Draw Particle Functions
void Particle_DrawStarBurst();