/* Start Header ************************************************************************/
/*!
\file       GarageDoor.cpp
\brief      Garage door closing animation for level end sequence.
            Slides down over the screen, then transitions to LevelComplete state.
*/
/* End Header **************************************************************************/
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------
static const float DOOR_SPEED = 500.0f;  // pixels per second downward
static const float SCREEN_WIDTH = 1700.0f;
static const float SCREEN_HEIGHT = 1000.0f;
static const float LEVEL_COMPLETE_DELAY = 0.5f;  // seconds to wait after door closes

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
static AEGfxTexture* pDoorTexture = nullptr;
static AEGfxVertexList* doorMesh = nullptr;
static bool             isTriggered = false;
static bool             isClosed = false;
static float            doorY = SCREEN_HEIGHT;  // starts fully above screen
static float            transitionTimer = 0.0f;

// ---------------------------------------------------------------------------
// GarageDoor_Load
// ---------------------------------------------------------------------------
void GarageDoor_Load()
{
    pDoorTexture = AEGfxTextureLoad("Assets/Assets/Transition_wall.png");

    if (pDoorTexture)
        printf("GarageDoor: texture loaded\n");
    else
        printf("ERROR: GarageDoor failed to load texture!\n");
}

// ---------------------------------------------------------------------------
// GarageDoor_Init  — call this when entering the gameplay state
// ---------------------------------------------------------------------------
void GarageDoor_Init()
{
    isTriggered = false;
    isClosed = false;
    doorY = SCREEN_HEIGHT;   // fully off-screen above
    transitionTimer = 0.0f;
    printf("GarageDoor: initialized\n");
    doorMesh = CreateSquareMesh(0.5f, 0.5f);
}

// ---------------------------------------------------------------------------
// GarageDoor_Trigger  — call once when Customer_IsLevelComplete() is true
// ---------------------------------------------------------------------------
void GarageDoor_Trigger()
{
    if (isTriggered) return;  // prevent double-trigger
    isTriggered = true;
    printf("GarageDoor: triggered!\n");
}

// ---------------------------------------------------------------------------
// GarageDoor_Update  — call every frame from Game_Update()
// ---------------------------------------------------------------------------
void GarageDoor_Update(float dt)
{
    if (!isTriggered) return;

    // Step 1: Slide door down until it covers the screen
    if (!isClosed) {
        doorY -= DOOR_SPEED * dt;
        

        if (doorY <= 0.0f) {
            doorY = 0.0f;
            isClosed = true;
            transitionTimer = 0.0f;
            printf("GarageDoor: fully closed!\n");
        }
        return;
    }

    // Step 2: Wait briefly then transition to Level Complete
    transitionTimer += dt;
    if (transitionTimer >= LEVEL_COMPLETE_DELAY) {
        printf("GarageDoor: transitioning to LevelComplete\n");
        next = GS_LVLCMP;
    }
}

// ---------------------------------------------------------------------------
// GarageDoor_Draw  — call at the END of Game_Draw() so it draws on top
// ---------------------------------------------------------------------------
void GarageDoor_Draw()
{
    if (!isTriggered) return;

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxTextureSet(pDoorTexture, 0.0f, 0.0f);

    AEMtx33 s, t, m;
    AEMtx33Scale(&s, SCREEN_WIDTH, SCREEN_HEIGHT);
    AEMtx33Trans(&t, 0.0f, doorY);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(doorMesh, AE_GFX_MDM_TRIANGLES);
}

// ---------------------------------------------------------------------------
// GarageDoor_Free
// ---------------------------------------------------------------------------
void GarageDoor_Free()
{
    if (doorMesh) {          
        AEGfxMeshFree(doorMesh);
        doorMesh = nullptr;
    }
    // Reset state in case level is replayed
    isTriggered = false;
    isClosed = false;
    doorY = SCREEN_HEIGHT;
    transitionTimer = 0.0f;
    printf("GarageDoor: freed\n");
}

// ---------------------------------------------------------------------------
// GarageDoor_Unload
// ---------------------------------------------------------------------------
void GarageDoor_Unload()
{
    if (pDoorTexture) {
        AEGfxTextureUnload(pDoorTexture);
        pDoorTexture = nullptr;
        printf("GarageDoor: texture unloaded\n");
    }
}