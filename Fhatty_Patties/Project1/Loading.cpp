// ---------------------------------------------------------------------------
// Loading.cpp
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Loading State Variables
// ---------------------------------------------------------------------------
static s8 fontId = -1;
static AEGfxVertexList* bgMesh = nullptr;
static AEGfxTexture* background = nullptr; // game BG
static AEMtx33 bgScale, bgTranslate, bgTransform;

static float loadingTimer = 0.0f;
static const float MIN_LOADING_TIME = 3.0f;

static int   loadStep = 0;
static int   totalSteps = 0;
static float displayProgress = 0.0f;
static bool  loadingDone = false;

// ---------------------------------------------------------------------------
// Truck Variables
// ---------------------------------------------------------------------------
static AEGfxTexture* LoadTruck = nullptr;
static AEGfxVertexList* LoadTruckMesh = nullptr;

static float truckX = 1000.0f;  // starts off right side
static float truckY = -125.0f;  // vertical position

static int   truckFrame = 0;
static float truckFrameTimer = 0.0f;

static const float TRUCK_FRAME_DURATION = 1.0f / 8.0f;
static const float TRUCK_FRAME_UV[2] = { 0.0f, 0.5f };
static const float TRUCK_START_X = 1000.0f;
static const float TRUCK_END_X = -1000.0f;

// ---------------------------------------------------------------------------
// Load Steps
// ---------------------------------------------------------------------------
struct LoadStep {
    const char* label;
    void (*loadFunc)();
};

static void Step_Ingredients() { Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png"); }
static void Step_GameBG_Back() { GameplayBG_back = AEGfxTextureLoad("Assets/Assets/Gameplay_back.png"); }
static void Step_GameBG_Front() { GameplayBG_front = AEGfxTextureLoad("Assets/Assets/Gameplay_front.png"); }
static void Step_Customer0() { CustomerTexture[0] = AEGfxTextureLoad("Assets/Assets/syakilcust.png"); }
static void Step_Customer1() { CustomerTexture[1] = AEGfxTextureLoad("Assets/Assets/elicust.png"); }
static void Step_Customer2() { CustomerTexture[2] = AEGfxTextureLoad("Assets/Assets/tebcust.png"); }
static void Step_Customer3() { CustomerTexture[3] = AEGfxTextureLoad("Assets/Assets/diyacust.png"); }
static void Step_Lock() { LockTexture = AEGfxTextureLoad("Assets/Assets/lock.png"); }
static void Step_Plate() { PlateTexture = AEGfxTextureLoad("Assets/Assets/plate.png"); }

static LoadStep steps[] = {
    { "Loading ingredients...",  Step_Ingredients  },
    { "Loading background...",   Step_GameBG_Back  },
    { "Loading foreground...",   Step_GameBG_Front },
    { "Loading characters...",   Step_Customer0    },
    { "Loading characters...",   Step_Customer1    },
    { "Loading characters...",   Step_Customer2    },
    { "Loading characters...",   Step_Customer3    },
    { "Loading UI...",           Step_Lock         },
    { "Loading UI...",           Step_Plate        },
};

// ---------------------------------------------------------------------------
// Game State Functions
// ---------------------------------------------------------------------------
void Loading_Load()
{
    fontId = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 32);
    bgMesh = CreateSquareMesh(0.5f, 0.5f);
    background = AEGfxTextureLoad("Assets/Assets/Loading_BG.png");
    LoadTruckMesh = CreateTruckMesh(0.5f, 0.5f);
    LoadTruck = AEGfxTextureLoad("Assets/Assets/Food_Truck.png");

    Sound_Load();
    Particle_Load();
}

void Loading_Init()
{
    loadStep = 0;
    displayProgress = 0.0f;
    loadingDone = false;
    loadingTimer = 0.0f;
    totalSteps = sizeof(steps) / sizeof(steps[0]);

    // Reset truck
    truckX = TRUCK_START_X;
    truckFrame = 0;
    truckFrameTimer = 0.0f;

    Particle_Init();
    Sound_PlaySFX_TruckMove();
}

void Loading_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    loadingTimer += dt;

    // Load one asset per frame
    if (loadStep < totalSteps)
    {
        steps[loadStep].loadFunc();
        loadStep++;
    }
    else if (!loadingDone)
    {
        loadingDone = true;
    }

    // Smooth progress 0 → 1
    float targetProgress = (float)loadStep / (float)totalSteps;
    displayProgress += (targetProgress - displayProgress) * dt * 8.0f;
    if (displayProgress > 1.0f) displayProgress = 1.0f;

    // -----------------------------------------------------------------------
    // Move truck left based on progress
    // -----------------------------------------------------------------------
    float targetX = TRUCK_START_X + (TRUCK_END_X - TRUCK_START_X) * displayProgress;
    truckX += (targetX - truckX) * dt * 3.0f; // smooth follow

    // Wheel animation
    truckFrameTimer += dt;
    if (truckFrameTimer >= TRUCK_FRAME_DURATION)
    {
        truckFrameTimer -= TRUCK_FRAME_DURATION;
        truckFrame = (truckFrame + 1) % 2;
    }


    // Smoke
    Particle_SetSmoke(truckX - 70.0f, truckY - 100.0f, true, 0.005f, 50.0f);
    Particle_Update(dt);

    // Transition once done and truck has moved far enough
    if (loadingDone && displayProgress >= 0.99f && loadingTimer >= MIN_LOADING_TIME)
    {
        Sound_StopSFX_TruckMove();
        next = GS_GAME;
    }
}

void Loading_Draw()
{
    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.05f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // -------------------------------------------------------------------
    // DRAW BACK BG
    // -------------------------------------------------------------------
    AEMtx33Scale(&bgScale, 1600.0f, 900.0f);
    AEMtx33Trans(&bgTranslate, 0.0f, 0.0f);
    AEMtx33Concat(&bgTransform, &bgTranslate, &bgScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(bgTransform.m);
    AEGfxTextureSet(background, 0.f, 0.f);
    AEGfxMeshDraw(bgMesh, AE_GFX_MDM_TRIANGLES);


    // -----------------------------------------------------------------------
    // DRAW SMOKE (behind truck)
    // -----------------------------------------------------------------------
    Particle_Draw();

    // -----------------------------------------------------------------------
    // DRAW TRUCK
    // -----------------------------------------------------------------------
    if (LoadTruck && LoadTruckMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetTransparency(1.0f);

        AEMtx33 s, t, m;
        AEMtx33Scale(&s, 350.0f, 350.0f);
        AEMtx33Trans(&t, truckX, truckY);
        AEMtx33Concat(&m, &t, &s);

        AEGfxSetTransform(m.m);
        AEGfxTextureSet(LoadTruck, TRUCK_FRAME_UV[truckFrame], 0.f);
        AEGfxMeshDraw(LoadTruckMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -----------------------------------------------------------------------
    // TEXT
    // -----------------------------------------------------------------------
    if (fontId >= 0)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        AEGfxPrint(fontId, "LOADING", -0.12f, 0.8f, 2.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        //const char* label = (loadStep < totalSteps) ? steps[loadStep].label : "Ready!";
        //AEGfxPrint(fontId, label, -0.18f, -0.55f, 0.6f, 0.8f, 0.8f, 0.8f, 1.0f);

        char pctStr[16];
        int pct = (int)(displayProgress * 100.0f);
        if (pct > 100) pct = 100;
        sprintf_s(pctStr, "%d%%", pct);
        AEGfxPrint(fontId, pctStr, -0.025f, 0.65f, 1.5f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    AEGfxSetBlendMode(AE_GFX_BM_NONE);
}

void Loading_Free()
{
    Particle_Free();
}

void Loading_Unload()
{
    Sound_Unload();
    Particle_Unload();
    if (fontId >= 0) { AEGfxDestroyFont(fontId);          fontId = -1; }
    if (bgMesh) { AEGfxMeshFree(bgMesh);             bgMesh = nullptr; }
    if (LoadTruckMesh) { AEGfxMeshFree(LoadTruckMesh);      LoadTruckMesh = nullptr; }
    if (LoadTruck) { AEGfxTextureUnload(LoadTruck);     LoadTruck = nullptr; }
    if (background) { AEGfxTextureUnload(background);     background = nullptr; }
}