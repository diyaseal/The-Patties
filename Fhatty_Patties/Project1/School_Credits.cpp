// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for School Credits
static AEGfxTexture* pSchoolLogo = nullptr;      // DigiPen logo texture
static AEGfxVertexList* pLogoMesh = nullptr;     // Mesh for logo
static float totalElapsedTime = 0.0f;            // Timer for screen duration
static float fade = 1.0f;                   // Start fully visible
static s8 fontId = -1;                           // Font ID

// ---------------------------------------------------------------------------
// Game State Functions

void School_Credits_Load()
{
    printf("School_Credits Load\n");

    totalElapsedTime = 0.0f;

    // -------------------------------------------------------------------
    // LOAD FONT
    // -------------------------------------------------------------------
    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId);

    // Load Digipen logo texture
    pSchoolLogo = AEGfxTextureLoad("Assets/Assets/DigiPen_Singapore_WEB_RED.png");
    if (!pSchoolLogo)
        printf("ERROR: Failed to load DigiPen logo\n");
    else
        printf("SUCCESS: DigiPen logo loaded\n");

    pLogoMesh = CreateSquareMesh(0.5, 0.5);
    printf(pLogoMesh ? "SUCCESS: Logo mesh created\n" : "ERROR: Failed to create logo mesh\n");
}

void School_Credits_Init()
{
    printf("School Credits Init called\n");
    fade = 1.0f;  // Reset fade when entering screen
}

void School_Credits_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();
    totalElapsedTime += dt;

    // Calculate fade: starts fading after 2 seconds, completely faded by 3 seconds
    if (totalElapsedTime > 2.0f)
    {
        // Linear interpolation from 1.0 to 0.0 over 1 second
        fade = 1.0f - ((totalElapsedTime - 2.0f) / 1.0f);

        // Clamp to valid range
        if (fade < 0.0f)
            fade = 0.0f;
    }

    if (totalElapsedTime > 3.0f)
        next = GS_TEAM_CRED;
}

void School_Credits_Draw()
{
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    s32 windowWidth = AEGfxGetWindowWidth();
    s32 windowHeight = AEGfxGetWindowHeight();

    // -------------------------------------------------------------------
    // DRAW LOGO WITH FADE
    // -------------------------------------------------------------------
    if (pSchoolLogo && pLogoMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetTransparency(fade); // Apply fade to texture

        AEGfxSetColorToMultiply(1, 1, 1, 1);
        AEGfxSetColorToAdd(0, 0, 0, 0);

        AEGfxTextureSet(pSchoolLogo, 0, 0);

        float logoWidth = windowWidth / 2.0f;
        float logoHeight = windowHeight / 3.0f;

        AEMtx33 scale, translate, transform;
        AEMtx33Scale(&scale, logoWidth, logoHeight);
        AEMtx33Trans(&translate, 0.0f, 0.0f);
        AEMtx33Concat(&transform, &translate, &scale);

        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(pLogoMesh, AE_GFX_MDM_TRIANGLES);

        AEGfxSetTransparency(1.0f);
        AEGfxSetBlendMode(AE_GFX_BM_NONE);  // Reset blend mode
    }
        
    // -------------------------------------------------------------------
    // DRAW COPYRIGHT TEXT (FIXED)
    // -------------------------------------------------------------------
    if (fontId >= 0)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        AEGfxPrint(
            fontId,
            "All content (c) 2025 DigiPen Institute of Technology Singapore. All Rights Reserved.",
            -0.50f,   // X (normalized)
            -0.9f,    // Y (normalized)
            0.6f,     // Scale
            1.0f, 1.0f, 1.0f, fade);

        AEGfxSetBlendMode(AE_GFX_BM_NONE);  // Reset blend mode
    }
}

void School_Credits_Free()
{
    /*printf("School Credits Free called\n");*/
}

void School_Credits_Unload()
{
    printf("School Credits Unload called\n");

    if (pLogoMesh)
    {
        AEGfxMeshFree(pLogoMesh);
        pLogoMesh = nullptr;
    }

    if (pSchoolLogo)
    {
        AEGfxTextureUnload(pSchoolLogo);
        pSchoolLogo = nullptr;
    }

    if (fontId >= 0)
    {
        AEGfxDestroyFont(fontId);
        fontId = -1;
    }

    if (fade == 0)
    {
        fade = 1.0f;
    }
}
