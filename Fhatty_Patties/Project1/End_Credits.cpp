// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for End Credits
static float totalElapsedTime = 0.0f;            // Timer for screen duration
static float fade = 1.0f;                   // Start fully visible

// Assets
// ---------------------------------------------------------------------------
// FONT
// ---------------------------------------------------------------------------
static s8 fontId = -1;                           // Font ID

// ---------------------------------------------------------------------------
// LAND
// ---------------------------------------------------------------------------
static AEGfxVertexList* landMesh = nullptr;
static AEGfxTexture* Grass = nullptr;
static float landRotation = 0.0f;
static const float LAND_ROTATE_SPEED = 5.0f; // degrees per second

// ---------------------------------------------------------------------------
// STARS
// ---------------------------------------------------------------------------
static AEGfxVertexList* starMesh = nullptr;

struct BackgroundStar {
    float x, y;           // position
    float baseSize;       // original size
    float currentSize;    // animated size
    float pulseSpeed;     // how fast it pulses
    float pulseTimer;     // offset so they dont all pulse together
};

static const int NUM_STARS = 15;
static BackgroundStar bgStars[NUM_STARS];

// ---------------------------------------------------------------------------
// TRUCK
// ---------------------------------------------------------------------------
static AEGfxTexture* Truck = nullptr;
static AEMtx33 TruckTransform, TruckTranslate, TruckScale;
static AEGfxVertexList* TruckMesh = nullptr;
// Wheel animation 2 frames side by side in spritesheet, each frame = 0.5 UV width
static int   truckFrame = 0;        // current frame (0 or 1)
static float truckFrameTimer = 0.0f;     // counts up to TRUCK_FRAME_DURATION
static const float TRUCK_FRAME_DURATION = 1.0f / 8.0f;
static const float TRUCK_FRAME_UV[2] = { 0.0f, 1.f / 2.f };

static float truckY = 600.0f;          // starts above screen
static float truckVelY = 0.0f;
static bool  truckLanded = false;
static float truckBounceTimer = 0.0f;
static const float TRUCK_TARGET_Y = -80.0f;  // resting Y at top middle
static const float TRUCK_GRAVITY = 1200.0f;
static const float TRUCK_BOUNCE_DAMPEN = 0.4f;
static const float TRUCK_LAND_THRESHOLD = 30.0f;

static float truckBobOffset = 0.0f;   // current bob offset
static float truckBobTimer = 0.0f;    // drives the sine wave
static const float TRUCK_BOB_SPEED = 8.0f;   // how fast it bobs
static const float TRUCK_BOB_AMOUNT = 5.0f;   // how many pixels up/down

// ---------------------------------------------------------------------------
// Helper Functions

void End_Credits_Update()
{
    // Quit
    if (AEInputCheckTriggered(AEVK_Q)) {
        next = GS_MAINMENU;
    }

    float dt = (float)AEFrameRateControllerGetFrameTime();
    totalElapsedTime += dt;

    // Set source first, then update
    Particle_SetSource(0.0f, truckY + truckBobOffset, truckLanded);
    Particle_Update(dt);

    // Pulse each star
    for (int i = 0; i < NUM_STARS; i++)
    {
        bgStars[i].pulseTimer += dt * bgStars[i].pulseSpeed;
        // sinf oscillates -1 to 1, scale to 0.5 to 1.5x base size
        float pulse = 1.0f + 0.5f * sinf(bgStars[i].pulseTimer);
        bgStars[i].currentSize = bgStars[i].baseSize * pulse;
    }

    landRotation += LAND_ROTATE_SPEED * dt;
    if (landRotation > 360.0f) landRotation -= 360.0f; // keep it clean

    // Truck drop + bounce
    if (!truckLanded)
    {
        truckVelY -= TRUCK_GRAVITY * dt;
        truckY += truckVelY * dt;

        if (truckY <= TRUCK_TARGET_Y)
        {
            truckY = TRUCK_TARGET_Y;
            truckVelY = -truckVelY * TRUCK_BOUNCE_DAMPEN;

            if (fabsf(truckVelY) < TRUCK_LAND_THRESHOLD)
            {
                truckVelY = 0.0f;
                truckLanded = true;
            }
        }

        // Animate wheels while falling
        truckFrameTimer += dt;
        if (truckFrameTimer >= TRUCK_FRAME_DURATION)
        {
            truckFrameTimer -= TRUCK_FRAME_DURATION;
            truckFrame = (truckFrame + 1) % 2;
        }
    }

    // Truck wheel frame animation + bob — only after landed
    if (truckLanded)
    {
        // Switch frames
        truckFrameTimer += dt;
        if (truckFrameTimer >= TRUCK_FRAME_DURATION)
        {
            truckFrameTimer -= TRUCK_FRAME_DURATION;
            truckFrame = (truckFrame + 1) % 2; // toggles 0 → 1 → 0
        }

        // Bob up and down with sine wave
        truckBobTimer += dt;
        truckBobOffset = sinf(truckBobTimer * TRUCK_BOB_SPEED) * TRUCK_BOB_AMOUNT;
    }

}

// ---------------------------------------------------------------------------
// Helper: calculates fade alpha for a set based on when it starts
// Each set: 1s fade in, 3s hold, 1s fade out = 5s total per set
// ---------------------------------------------------------------------------
static float GetSetFade(float startTime)
{
    const float FADE_IN_DUR = 1.0f;
    const float HOLD_DUR = 3.0f;
    const float FADE_OUT_DUR = 1.0f;
    const float SET_DUR = FADE_IN_DUR + HOLD_DUR + FADE_OUT_DUR; // 5s

    float t = totalElapsedTime - startTime;

    if (t < 0.0f)                              return 0.0f; // not started
    if (t < FADE_IN_DUR)                       return t / FADE_IN_DUR; // fading in
    if (t < FADE_IN_DUR + HOLD_DUR)            return 1.0f; // fully visible
    if (t < SET_DUR)                           return 1.0f - ((t - FADE_IN_DUR - HOLD_DUR) / FADE_OUT_DUR); // fading out
    return 0.0f; // done
}

// ---------------------------------------------------------------------------
// Game State Functions

void End_Credits_Load()
{
    printf("School_Credits Load\n");

    Particle_Load();

    totalElapsedTime = 0.0f;
    landMesh = CreateCircleMeshUV(0.5f, 64);
    starMesh = CreateStarMesh(0.5f, 0.2f);

    // -------------------------------------------------------------------
    // LOAD FONT
    // -------------------------------------------------------------------
    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId);

    // -------------------------------------------------------------------
    // LOAD GRASS
    // -------------------------------------------------------------------
    Grass = AEGfxTextureLoad("Assets/Assets/grass.png");

    if (Grass)
        printf("Truck texture loaded successfully\n");
    else
        printf("ERROR: Failed to load grass.png\n");


    // -------------------------------------------------------------------
    // LOAD TRUCK
    // -------------------------------------------------------------------
    Truck = AEGfxTextureLoad("Assets/Assets/Food_Truck.png");
    TruckMesh = CreateTruckMesh(0.5, 0.5);

    if (Truck)
        printf("Truck texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Food_truck.png\n");

    Sound_Load();
    Sound_PlayBGMCreds();   
}

void End_Credits_Init()
{
    printf("End Credits Init called\n");

    fade = 1.0f;  // Reset fade when entering screen

    Particle_Init();

    // Scatter stars randomly across screen
    srand(72); // fixed seed so positions are consistent each run
    for (int i = 0; i < NUM_STARS; i++)
    {
        // Random position across screen in normalized-ish pixel space
        bgStars[i].x = -700.0f + (rand() % 1400);  // -700 to 700
        bgStars[i].y = -100.0f + (rand() % 600);   // -100 to 500 (upper half)
        bgStars[i].baseSize = 10.0f + (rand() % 25);    // 10 to 35
        bgStars[i].currentSize = bgStars[i].baseSize;
        bgStars[i].pulseSpeed = 0.5f + (rand() % 100) / 100.0f; // 0.5 to 1.5
        bgStars[i].pulseTimer = (rand() % 100) / 100.0f * 6.28f;   // random start offset
    }

    // ... existing code ...
    truckY = 600.0f;
    truckVelY = 0.0f;
    truckLanded = false;
    truckFrame = 0;
    truckFrameTimer = 0.0f;
    totalElapsedTime = 0.0f; 
    truckBobOffset = 0.0f;
    truckBobTimer = 0.0f;
}

void End_Credits_Draw()
{
    // -----------------------------------------------------------------------
    // DRAW SKY
    // -----------------------------------------------------------------------
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.2f);

    // -----------------------------------------------------------------------
    // DRAW LAND
    // -----------------------------------------------------------------------
    if (landMesh && Grass)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEMtx33 scale, rotate, translate, transform, temp;
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f); // no tint, show texture as-is
         
        AEMtx33Scale(&scale, 1800.0f, 1800.0f);
        AEMtx33RotDeg(&rotate, -landRotation); // rotates clockwise
        AEMtx33Trans(&translate, 0.0f, -980.0f);
        AEMtx33Concat(&temp, &rotate, &scale);
        AEMtx33Concat(&transform, &translate, &temp);
        AEGfxSetTransform(transform.m);
        AEGfxTextureSet(Grass, 0.0f, 0.0f);
        AEGfxMeshDraw(landMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -----------------------------------------------------------------------
    // DRAW BACKGROUND STARS
    // -----------------------------------------------------------------------
    if (starMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);

        for (int i = 0; i < NUM_STARS; i++)
        {
            // Slight yellow-white tint
            AEGfxSetColorToMultiply(1.0f, 1.0f, 0.8f, 1.0f);

            AEMtx33 scale, translate, transform;
            AEMtx33Scale(&scale, bgStars[i].currentSize, bgStars[i].currentSize);
            AEMtx33Trans(&translate, bgStars[i].x, bgStars[i].y);
            AEMtx33Concat(&transform, &translate, &scale);

            AEGfxSetTransform(transform.m);
            AEGfxMeshDraw(starMesh, AE_GFX_MDM_TRIANGLES);
        }
    }

    if (Truck && TruckMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetTransparency(1.0f);

        AEMtx33Scale(&TruckScale, 100.0f, 100.0f);
        AEMtx33Trans(&TruckTranslate, 0.0f, truckY + truckBobOffset); // bob applied here
        AEMtx33Concat(&TruckTransform, &TruckTranslate, &TruckScale);

        AEGfxSetTransform(TruckTransform.m);
        AEGfxTextureSet(Truck, TRUCK_FRAME_UV[truckFrame], 0.f);
        AEGfxMeshDraw(TruckMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -----------------------------------------------------------------------
    // DRAW SMOKE PARTICLE
    // -----------------------------------------------------------------------
    Particle_Draw();

    // -----------------------------------------------------------------------
    // DRAW CREDITS TEXT
    // -----------------------------------------------------------------------

    if (fontId < 0) return;

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // Each set starts 5s after the previous
    float f1 = GetSetFade(0.0f);
    float f2 = GetSetFade(5.0f);
    float f3 = GetSetFade(10.0f);
    float f4 = GetSetFade(15.0f);
    float f5 = GetSetFade(20.0f);
    float f6 = GetSetFade(25.0f);

    // School credits line stays throughout using max fade of all sets
    float globalFade = 1.0f;
    AEGfxPrint(fontId, "WWW.DIGIPEN.EDU",
        -0.10f, 0.9f, 0.4f, 1, 1, 1, globalFade);
    AEGfxPrint(fontId, "All content (c) 2025 DigiPen Institute of Technology Singapore. All Rights Reserved.",
        -0.39f, 0.82f, 0.45f, 1, 1, 1, globalFade);

    // -----------------------------------------------------------------------
    // SET 1 — Team Credits
    // -----------------------------------------------------------------------
    if (f1 > 0.0f)
    {
        AEGfxPrint(fontId, "Fatty Patties", -0.15f, 0.60f, 1.0f, 1, 1, 1, f1);

        AEGfxPrint(fontId, "Syakil", -0.20f, 0.50f, 0.5f, 1, 1, 1, f1);
        AEGfxPrint(fontId, "UI/UX Programmer", 0.02f, 0.50f, 0.5f, 1, 1, 1, f1);

        AEGfxPrint(fontId, "Yue Jun", -0.20f, 0.45f, 0.5f, 1, 1, 1, f1);
        AEGfxPrint(fontId, "Animation Programmer", 0.02f, 0.45f, 0.5f, 1, 1, 1, f1);

        AEGfxPrint(fontId, "Diya", -0.20f, 0.40f, 0.5f, 1, 1, 1, f1);
        AEGfxPrint(fontId, "Gameplay Programmer", 0.02f, 0.40f, 0.5f, 1, 1, 1, f1);

        AEGfxPrint(fontId, "Elizabeth", -0.20f, 0.35f, 0.5f, 1, 1, 1, f1);
        AEGfxPrint(fontId, "Level Programmer", 0.02f, 0.35f, 0.5f, 1, 1, 1, f1);
    }

    // -----------------------------------------------------------------------
    // SET 2 — Faculty and Created At
    // -----------------------------------------------------------------------
    if (f2 > 0.0f)
    {
        AEGfxPrint(fontId, "Game Instructors", -0.20f, 0.60f, 1.0f, 1, 1, 1, f2);
        AEGfxPrint(fontId, "Gerald Wong", -0.08f, 0.50f, 0.5f, 1, 1, 1, f2);
        AEGfxPrint(fontId, "Soroor Malekmohammadi Faradounbeh", -0.22f, 0.45f, 0.5f, 1, 1, 1, f2);
        AEGfxPrint(fontId, "Tommy Tan", -0.075f, 0.40f, 0.5f, 1, 1, 1, f2);

        AEGfxPrint(fontId, "Created At", -0.10f, 0.25f, 0.8f, 1, 1, 1, f2);
        AEGfxPrint(fontId, "DigiPen Institute of Technology", -0.18f, 0.15f, 0.5f, 1, 1, 1, f2);
    }

    // -----------------------------------------------------------------------
    // SET 3 — President and Executives
    // -----------------------------------------------------------------------
    if (f3 > 0.0f)
    {
        AEGfxPrint(fontId, "President", -0.115f, 0.60f, 1.0f, 1, 1, 1, f3);
        AEGfxPrint(fontId, "Claude Comair", -0.085f, 0.50f, 0.5f, 1, 1, 1, f3);

        AEGfxPrint(fontId, "Executives", -0.115f, 0.35f, 0.8f, 1, 1, 1, f3);
        AEGfxPrint(fontId, "Jason Chu    Samir Abou Samra    Michele Comair", -0.25f, 0.25f, 0.5f, 1, 1, 1, f3);
        AEGfxPrint(fontId, "Angela Kugler    Erik Mohrmann          Benjamin Ellinger", -0.294f, 0.20f, 0.5f, 1, 1, 1, f3);
        AEGfxPrint(fontId, "Melvin Gonsalves", -0.115f, 0.15f, 0.5f, 1, 1, 1, f3);
    }

    // -----------------------------------------------------------------------
    // SET 4 — Special Thanks
    // -----------------------------------------------------------------------
    if (f4 > 0.0f)
    {
        AEGfxPrint(fontId, "Special Thanks", -0.17f, 0.60f, 1.0f, 1, 1, 1, f4);
        AEGfxPrint(fontId, "All Our Play Testers", -0.12f, 0.50f, 0.5f, 1, 1, 1, f4);
    }

    // -----------------------------------------------------------------------
    // SET 5 — Copyright
    // -----------------------------------------------------------------------
    if (f5 > 0.0f)
    {
        AEGfxPrint(fontId, "Thank You For Playing", -0.25f, 0.5f, 1.0f, 1, 1, 1, f5);
    
    }

    if (f6 > 0.0f) {
        next = GS_MAINMENU;
    }

    AEGfxSetBlendMode(AE_GFX_BM_NONE);
}

void End_Credits_Free()
{
    printf("End Credits Free called\n");
    Particle_Free();
}

void End_Credits_Unload()
{
    printf("End Credits Unload called\n");
    Particle_Unload();
    if (fontId >= 0) { AEGfxDestroyFont(fontId); fontId = -1; }
    if (landMesh) { AEGfxMeshFree(landMesh);  landMesh = nullptr; }
    if (starMesh) { AEGfxMeshFree(starMesh);  starMesh = nullptr; }

    if (TruckMesh) { AEGfxMeshFree(TruckMesh); TruckMesh = nullptr; }
    if (Truck) { AEGfxTextureUnload(Truck); Truck = nullptr; }

    Sound_Unload();
}
