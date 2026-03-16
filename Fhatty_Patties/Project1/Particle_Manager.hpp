#pragma once

void Particle_SetSource(float truckX, float truckY, bool truckLanded, float spawnInterval, float sz);
void Particle_Load();
void Particle_Init();
void Particle_Update(float dt);
void Particle_Draw();
void Particle_Free();
void Particle_Unload();