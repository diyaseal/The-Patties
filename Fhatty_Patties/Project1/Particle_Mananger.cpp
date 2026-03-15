// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for Particles Credits

// ---------------------------------------------------------------------------
// Smoke Particle System
// ---------------------------------------------------------------------------
struct SmokeParticle {
    float x, y;          // position
    float velX, velY;    // velocity
    float size;          // current size
    float alpha;         // transparency
    float lifetime;      // how long it has lived
    float maxLifetime;   // when it dies
    bool  active;        // is it alive
};

static const int MAX_SMOKE = 30;
static SmokeParticle smokeParticles[MAX_SMOKE];
static AEGfxVertexList* smokeMesh = nullptr;

static float spawnTimer = 0.0f;
static const float SPAWN_INTERVAL = 0.15f; // seconds between spawns

// ---------------------------------------------------------------------------
// Spawn offset — where smoke comes out relative to truck
// Tune these to match your truck's exhaust pipe position
// ---------------------------------------------------------------------------
static const float EXHAUST_OFFSET_X = 30.0f; // left side of truck
static const float EXHAUST_OFFSET_Y = -5.0f; // near top of truck

// ---------------------------------------------------------------------------
// Source data — set externally via Particle_SetSource
// ---------------------------------------------------------------------------
static float sourceX = 0.0f;
static float sourceY = 0.0f;
static bool  emitting = false;

// ---------------------------------------------------------------------------
// Helper: spawn one smoke particle at truck position
// ---------------------------------------------------------------------------
static void SpawnSmoke(float truckX, float truckY)
{
    for (int i = 0; i < MAX_SMOKE; i++)
    {
        if (!smokeParticles[i].active)
        {
            SmokeParticle& p = smokeParticles[i];
            p.active = true;
            p.x = truckX + EXHAUST_OFFSET_X;
            p.y = truckY + EXHAUST_OFFSET_Y;
            p.velX = 30.0f + (rand() % 40);  // drifts left (truck faces left)
            p.velY = 10.0f + (rand() % 30);  // rises upward
            p.size = 8.0f + (rand() % 8);   // starts small
            p.alpha = 0.6f + (rand() % 30) / 100.0f; // 0.6 to 0.9
            p.lifetime = 0.0f;
            p.maxLifetime = 1.5f + (rand() % 100) / 100.0f; // 1.5 to 2.5s
            return;
        }
    }
}

void Particle_SetSource(float truckX, float truckY, bool truckLanded)
{
    sourceX = truckX + EXHAUST_OFFSET_X;
    sourceY = truckY + EXHAUST_OFFSET_Y;
    emitting = truckLanded;
}

// ---------------------------------------------------------------------------
// Game State Functions

void Particle_Load()
{
    smokeMesh = CreateCircleMesh(0.5f, 16); // 16 segments is enough for smoke
}

void Particle_Init()
{
    spawnTimer = 0.0f;

    // Deactivate all particles
    for (int i = 0; i < MAX_SMOKE; i++)
        smokeParticles[i].active = false;
}

void Particle_Update(float dt)
{
    // Only spawn if source has been set and truck landed
    if (emitting)
    {
        spawnTimer += dt;
        if (spawnTimer >= SPAWN_INTERVAL)
        {
            spawnTimer = 0.0f;
            SpawnSmoke(sourceX, sourceY); // uses stored source
        }
    }

    for (int i = 0; i < MAX_SMOKE; i++)
    {
        SmokeParticle& p = smokeParticles[i];
        if (!p.active) continue;

        p.lifetime += dt;
        if (p.lifetime >= p.maxLifetime) { p.active = false; continue; }

        float lifeRatio = p.lifetime / p.maxLifetime;

        p.x += p.velX * dt;
        p.y += p.velY * dt;
        p.velX *= (1.0f - dt * 0.5f);
        p.velY *= (1.0f - dt * 0.3f);
        p.size = 8.0f + (60.0f * lifeRatio);
        p.alpha = 0.7f * (1.0f - lifeRatio);
    }
}

void Particle_Draw()
{
    if (!smokeMesh) return;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    for (int i = 0; i < MAX_SMOKE; i++)
    {
        SmokeParticle& p = smokeParticles[i];
        if (!p.active) continue;

        float lifeRatio = p.lifetime / p.maxLifetime;

        // Color shifts white → grey as it ages
        float brightness = 0.9f - (lifeRatio * 0.5f); // 0.9 → 0.4

        AEGfxSetColorToMultiply(brightness, brightness, brightness, p.alpha);

        AEMtx33 scale, translate, transform;
        AEMtx33Scale(&scale, p.size, p.size);
        AEMtx33Trans(&translate, p.x, p.y);
        AEMtx33Concat(&transform, &translate, &scale);

        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(smokeMesh, AE_GFX_MDM_TRIANGLES);
    }

    AEGfxSetBlendMode(AE_GFX_BM_NONE);
}

void Particle_Free()
{
    for (int i = 0; i < MAX_SMOKE; i++)
        smokeParticles[i].active = false;
}

void Particle_Unload()
{
    if (smokeMesh) { AEGfxMeshFree(smokeMesh); smokeMesh = nullptr; }
}
