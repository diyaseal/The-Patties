// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for School Credits
static AEGfxTexture* pSchoolLogo = nullptr;      // DigiPen logo texture
static AEGfxVertexList* pLogoMesh = nullptr;     // Mesh for logo
static float totalElapsedTime = 0.0f;            // Timer for screen duration
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
    pSchoolLogo = AEGfxTextureLoad("Assets/Graphics/DigiPen_Singapore_WEB_RED.png");
    if (!pSchoolLogo)
        printf("ERROR: Failed to load DigiPen logo\n");
    else
        printf("SUCCESS: DigiPen logo loaded\n");

    // Create mesh for logo
    AEGfxMeshStart();

    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
    );

    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
    );

    pLogoMesh = AEGfxMeshEnd();
    printf(pLogoMesh ? "SUCCESS: Logo mesh created\n" : "ERROR: Failed to create logo mesh\n");
}

void School_Credits_Init()
{
    printf("School Credits Init called\n");
}

void School_Credits_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();
    totalElapsedTime += dt;

    if (totalElapsedTime > 3.0f)
        next = GS_MAINMENU;
}

void School_Credits_Draw()
{
    AEGfxSetBackgroundColor(1.0f, 1.0f, 1.0f);

    s32 windowWidth = AEGfxGetWindowWidth();
    s32 windowHeight = AEGfxGetWindowHeight();

    // -------------------------------------------------------------------
    // DRAW LOGO
    // -------------------------------------------------------------------
    if (pSchoolLogo && pLogoMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetTransparency(1.0f);

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
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
}

void School_Credits_Free()
{
    printf("School Credits Free called\n");
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
}
