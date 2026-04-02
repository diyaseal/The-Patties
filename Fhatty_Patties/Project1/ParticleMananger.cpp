// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include "pch.hpp"

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
// Star Burst Particle System
// ---------------------------------------------------------------------------
struct StarParticle {
    float x, y;
    float velX, velY;
    float size;
    float alpha;
    float lifetime;
    float maxLifetime;
    bool  active;
};

static const int MAX_STAR_PARTICLES = 20;
static StarParticle starParticles[MAX_STAR_PARTICLES];
static bool starEarned[3] = { false, false, false };

static float spawnStarTimer = 0.0f;
static float spawnStarRate = 0.05f;  // seconds between each star particle
static float sourceStarX = 0.0f;
static float sourceStarY = 0.0f;
static bool  emittingStar = false;
static float starBurstTimer = 0.0f;   // how long the burst lasts
static const float STAR_BURST_DURATION = 0.8f; // total burst duration

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

// ---------------------------------------------------------------------------
// Set Smoke
// ---------------------------------------------------------------------------
void Particle_SetSmoke(float truckX, float truckY, bool truckLanded, float spawnInterval, float sz)
{
    sourceSmokeX = truckX + SMOKE_OFFSET_X;
    sourceSmokeY = truckY + SMOKE_OFFSET_Y;
    emittingSmoke = truckLanded;
    spawnSmokeRate = spawnInterval; 
    sizeSmoke = sz;
}

// ---------------------------------------------------------------------------
// Spawn Stars
// ---------------------------------------------------------------------------
static void SpawnStars()
{
    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
    {
        if (!starParticles[i].active)
        {
            StarParticle& p = starParticles[i];
            p.active = true;
            p.x = sourceStarX;
            p.y = sourceStarY;

            // Random direction burst
            float angle = (rand() % 360) * 3.14159f / 180.0f;
            float speed = 80.0f + (rand() % 120);
            p.velX = cosf(angle) * speed;
            p.velY = sinf(angle) * speed;
            p.size = 6.0f + (rand() % 10);
            p.alpha = 1.0f;
            p.lifetime = 0.0f;
            p.maxLifetime = 0.4f + (rand() % 40) / 100.0f; // 0.4 to 0.8s
            return;
        }
    }
}

// ---------------------------------------------------------------------------
// Set Star Burst
// ---------------------------------------------------------------------------
void Particle_SetStarBurst(float x, float y, int starIndex)
{
    // Only burst once per star
    if (starEarned[starIndex]) return;
    starEarned[starIndex] = true; // mark so it never fires again

    // Set source and start emitting — same pattern as smoke
    sourceStarX = x;
    sourceStarY = y;
    emittingStar = true;
    starBurstTimer = 0.0f;
    spawnStarTimer = 0.0f;
}

// ---------------------------------------------------------------------------
// Draw Star Burst
// ---------------------------------------------------------------------------
void Particle_DrawStarBurst()
{
    if (!smokeMesh) return;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
    {
        StarParticle& p = starParticles[i];
        if (!p.active) continue;

        // Bright yellow burst
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, p.alpha);

        AEMtx33 scale, translate, transform;
        AEMtx33Scale(&scale, p.size, p.size);
        AEMtx33Trans(&translate, p.x, p.y);
        AEMtx33Concat(&transform, &translate, &scale);

        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(smokeMesh, AE_GFX_MDM_TRIANGLES);
    }
}

// ---------------------------------------------------------------------------
// Game State Functions
// ---------------------------------------------------------------------------
void Particle_Init()
{
    spawnSmokeTimer = 0.0f;

    // Reset Smoke Particles
    for (int i = 0; i < MAX_SMOKE; i++)
        smokeParticles[i].active = false;

    // Reset Star Particles
    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
        starParticles[i].active = false;
    for (int i = 0; i < 3; i++)
        starEarned[i] = false;

    // Reset burst state
    emittingStar = false;
    starBurstTimer = 0.0f;
    spawnStarTimer = 0.0f;
}

void Particle_Load()
{
    smokeMesh = CreateCircleMesh(0.5f, 16);
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

    // ---------------------------------------------------------------------------
    // Star Burst Update
    // ---------------------------------------------------------------------------
    // Spawn one particle at a time on timer — same as smoke
    if (emittingStar)
    {
        starBurstTimer += dt;
        spawnStarTimer += dt;

        if (spawnStarTimer >= spawnStarRate)
        {
            spawnStarTimer = 0.0f;
            SpawnStars(); // spawn one per interval
        }

        // Stop emitting after burst duration
        if (starBurstTimer >= STAR_BURST_DURATION)
            emittingStar = false;
    }

    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
    {
        StarParticle& p = starParticles[i];
        if (!p.active) continue;

        p.lifetime += dt;
        if (p.lifetime >= p.maxLifetime) { p.active = false; continue; }

        float lifeRatio = p.lifetime / p.maxLifetime;

        p.x += p.velX * dt;
        p.y += p.velY * dt;
        p.velX *= (1.0f - dt * 2.0f); // slow down quickly
        p.velY *= (1.0f - dt * 2.0f);
        p.alpha = 1.0f - lifeRatio;   // fades out
        p.size *= (1.0f - dt * 1.5f); // shrinks as it flies out
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

    // ---------------------------------------------------------------------------
    // Draw Star Burst
    // ---------------------------------------------------------------------------
    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
    {
        StarParticle& p = starParticles[i];
        if (!p.active) continue;

        // Bright yellow burst
        AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, p.alpha);

        AEMtx33 scale, translate, transform;
        AEMtx33Scale(&scale, p.size, p.size);
        AEMtx33Trans(&translate, p.x, p.y);
        AEMtx33Concat(&transform, &translate, &scale);

        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(smokeMesh, AE_GFX_MDM_TRIANGLES); // reuse circle mesh
    }

    AEGfxSetBlendMode(AE_GFX_BM_NONE);
}

void Particle_Free()
{
    for (int i = 0; i < MAX_SMOKE; i++)
        smokeParticles[i].active = false;

    // Reset Star Particles 
    for (int i = 0; i < MAX_STAR_PARTICLES; i++)
        starParticles[i].active = false;
    for (int i = 0; i < 3; i++)
        starEarned[i] = false;
}

void Particle_Unload()
{
    if (smokeMesh) { AEGfxMeshFree(smokeMesh); smokeMesh = nullptr; }
}
