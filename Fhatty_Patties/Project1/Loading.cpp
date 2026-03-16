// ---------------------------------------------------------------------------
// Loading.cpp
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Loading State Variables
// ---------------------------------------------------------------------------
static s8 fontId = -1;
static AEGfxVertexList* barMesh = nullptr;
static AEGfxVertexList* bgMesh = nullptr;

static int   loadStep = 0;     // which asset we're loading next
static int   totalSteps = 0;     // set on init
static float displayProgress = 0.0f; // smoothed visual progress
static bool  loadingDone = false;

// Each step has a label so the screen shows what's loading
struct LoadStep {
    const char* label;
    void (*loadFunc)();
};

// -----------------------------------------------------------------------
// Define every asset load call as a step
// Add/remove steps to match your game's actual loads
// -----------------------------------------------------------------------
static void Step_Fonts() { /* fonts loaded in each state's own Load */ }
static void Step_Ingredients() { Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png"); }
static void Step_GameBG_Back() { GameplayBG_back = AEGfxTextureLoad("Assets/Assets/Gameplay_back.png"); }
static void Step_GameBG_Front() { GameplayBG_front = AEGfxTextureLoad("Assets/Assets/Gameplay_front.png"); }
static void Step_Customer0() { CustomerTexture[0] = AEGfxTextureLoad("Assets/Assets/syakilcust.png"); }
static void Step_Customer1() { CustomerTexture[1] = AEGfxTextureLoad("Assets/Assets/elicust.png"); }
static void Step_Customer2() { CustomerTexture[2] = AEGfxTextureLoad("Assets/Assets/tebcust.png"); }
static void Step_Customer3() { CustomerTexture[3] = AEGfxTextureLoad("Assets/Assets/diyacust.png"); }
static void Step_Lock() { LockTexture = AEGfxTextureLoad("Assets/Assets/lock.png"); }
static void Step_Plate() { PlateTexture = AEGfxTextureLoad("Assets/Assets/plate.png"); }
static void Step_Truck() { /* loaded in MainMenu_Load */ }
static void Step_Sound() { Sound_Load(); }

static LoadStep steps[] = {
    { "Loading ingredients...",     Step_Ingredients  },
    { "Loading background...",      Step_GameBG_Back  },
    { "Loading foreground...",      Step_GameBG_Front },
    { "Loading characters...",      Step_Customer0    },
    { "Loading characters...",      Step_Customer1    },
    { "Loading characters...",      Step_Customer2    },
    { "Loading characters...",      Step_Customer3    },
    { "Loading UI...",              Step_Lock         },
    { "Loading UI...",              Step_Plate        },
    { "Loading audio...",           Step_Sound        },
};

// ---------------------------------------------------------------------------
// Game State Functions
// ---------------------------------------------------------------------------
void Loading_Load()
{
    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 32);
    barMesh = CreateSquareMesh(0.5f, 0.5f);
    bgMesh = CreateSquareMesh(0.5f, 0.5f);
}

void Loading_Init()
{
    loadStep = 0;
    displayProgress = 0.0f;
    loadingDone = false;
    totalSteps = sizeof(steps) / sizeof(steps[0]);
}

void Loading_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    // Load ONE asset per frame so the screen can still render
    if (loadStep < totalSteps)
    {
        steps[loadStep].loadFunc();
        loadStep++;
    }
    else if (!loadingDone)
    {
        loadingDone = true;
    }

    // Smooth the progress bar toward actual progress
    float targetProgress = (float)loadStep / (float)totalSteps;
    displayProgress += (targetProgress - displayProgress) * dt * 8.0f;
    if (displayProgress > 1.0f) displayProgress = 1.0f;

    // Only move on once bar has visually filled
    if (loadingDone && displayProgress >= 0.99f)
        next = GS_GAME; // go to first real screen
}

void Loading_Draw()
{
    AEGfxSetBackgroundColor(0.05f, 0.05f, 0.05f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // -----------------------------------------------------------------------
    // GREY BACKGROUND BAR
    // -----------------------------------------------------------------------
    const float BAR_W = 600.0f;
    const float BAR_H = 30.0f;
    const float BAR_Y = -300.0f;

    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f);
        AEMtx33 s, t, m;
        AEMtx33Scale(&s, BAR_W, BAR_H);
        AEMtx33Trans(&t, 0.0f, BAR_Y);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(barMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -----------------------------------------------------------------------
    // FILLED PROGRESS BAR
    // -----------------------------------------------------------------------
    if (displayProgress > 0.0f)
    {
        float fillW = BAR_W * displayProgress;
        float offsetX = -(BAR_W - fillW) / 2.0f;

        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(1.0f, 0.75f, 0.0f, 1.0f); // gold
        AEMtx33 s, t, m;
        AEMtx33Scale(&s, fillW, BAR_H);
        AEMtx33Trans(&t, offsetX, BAR_Y);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(barMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -----------------------------------------------------------------------
    // TEXT
    // -----------------------------------------------------------------------
    if (fontId >= 0)
    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);

        // Game title
        AEGfxPrint(fontId, "FATTY PATTIES", -0.22f, 0.1f, 1.5f, 1.0f, 0.85f, 0.0f, 1.0f);

        // Current step label
        const char* label = (loadStep < totalSteps)
            ? steps[loadStep].label
            : "Done!";
        AEGfxPrint(fontId, label, -0.18f, -0.55f, 0.6f, 0.8f, 0.8f, 0.8f, 1.0f);

        // Percentage
        char pctStr[16];
        sprintf_s(pctStr, "%d%%", (int)(displayProgress * 100.0f));
        AEGfxPrint(fontId, pctStr, -0.04f, -0.65f, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    AEGfxSetBlendMode(AE_GFX_BM_NONE);
}

void Loading_Free() {}

void Loading_Unload()
{
    if (fontId >= 0) { AEGfxDestroyFont(fontId); fontId = -1; }
    if (barMesh) { AEGfxMeshFree(barMesh);   barMesh = nullptr; }
    if (bgMesh) { AEGfxMeshFree(bgMesh);    bgMesh = nullptr; }
}