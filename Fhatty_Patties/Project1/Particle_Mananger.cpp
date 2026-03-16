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
    float baseSize;          // original size
    float size;           // current size  
    float alpha;         // transparency
    float lifetime;      // how long it has lived
    float maxLifetime;   // when it dies
    bool  active;        // is it alive
};

static const int MAX_SMOKE = 30;
static SmokeParticle smokeParticles[MAX_SMOKE];
static AEGfxVertexList* smokeMesh = nullptr;

static float spawnSmokeTimer = 0.0f;
static float spawnSmokeRate = 0.15f; // seconds between spawns

static const float SMOKE_OFFSET_X = 30.0f; // left side of truck
static const float SMOKE_OFFSET_Y = -5.0f; // near top of truck

static float sourceSmokeX = 0.0f;
static float sourceSmokeY = 0.0f;
static float sizeSmoke = 0.0f;
static bool  emittingSmoke = false;

// ---------------------------------------------------------------------------
// Helper Functions

// ---------------------------------------------------------------------------
// Spawn Smoke
// ---------------------------------------------------------------------------
static void SpawnSmoke(float truckX, float truckY)
{
    for (int i = 0; i < MAX_SMOKE; i++)
    {
        if (!smokeParticles[i].active)
        {
            SmokeParticle& p = smokeParticles[i];
            p.active = true;
            p.x = truckX + SMOKE_OFFSET_X;
            p.y = truckY + SMOKE_OFFSET_Y;
            p.velX = 30.0f + (rand() % 40);  // drifts left (truck faces left)
            p.velY = 10.0f + (rand() % 30);  // rises upward
            p.baseSize = sizeSmoke + (rand() % 8);   // starts small
            p.size = p.baseSize;
            p.alpha = 0.6f + (rand() % 30) / 100.0f; // 0.6 to 0.9
            p.lifetime = 0.0f;
            p.maxLifetime = 1.5f + (rand() % 100) / 100.0f; // 1.5 to 2.5s
            return;
        }
    }
}

void Particle_SetSmoke(float truckX, float truckY, bool truckLanded, float spawnInterval, float sz)
{
    sourceSmokeX = truckX + SMOKE_OFFSET_X;
    sourceSmokeY = truckY + SMOKE_OFFSET_Y;
    emittingSmoke = truckLanded;
    spawnSmokeRate = spawnInterval; 
    sizeSmoke = sz;
}

// ---------------------------------------------------------------------------
// Game State Functions

void Particle_Load()
{
    smokeMesh = CreateCircleMesh(0.5f, 16); 
}

void Particle_Init()
{
    spawnSmokeTimer = 0.0f;

    // Deactivate all particles
    for (int i = 0; i < MAX_SMOKE; i++)
        smokeParticles[i].active = false;
}

void Particle_Update(float dt)
{
    // ---------------------------------------------------------------------------
    // Smoke Update
    // ---------------------------------------------------------------------------
    if (emittingSmoke)
    {
        spawnSmokeTimer += dt;
        if (spawnSmokeTimer >= spawnSmokeRate)
        {
            spawnSmokeTimer = 0.0f;
            SpawnSmoke(sourceSmokeX, sourceSmokeY); // uses stored source
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
        p.size = p.baseSize + (60.0f * lifeRatio * (p.baseSize / 8.0f));
        p.alpha = 0.7f * (1.0f - lifeRatio);
    }
}

void Particle_Draw()
{
    // ---------------------------------------------------------------------------
    // Draw Smoke
    // ---------------------------------------------------------------------------
    if (!smokeMesh) return;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    for (int i = 0; i < MAX_SMOKE; i++)
    {
        SmokeParticle& p = smokeParticles[i];
        if (!p.active) continue;

        float lifeRatio = p.lifetime / p.maxLifetime;

        // Color shifts white → grey as it ages
        float brightness = 0.7f - (lifeRatio * 0.5f); // 0.9 → 0.4

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
