#include "pch.hpp"
static AEGfxTexture* pTeamLogo = nullptr;      // DigiPen logo texture
static AEGfxVertexList* pTeamMesh = nullptr;     // Mesh for logo
static float totalElapsedTime = 0.0f;            // Timer for screen duration
static float fade = 1.0f;                   // Start fully visible
static s8 fontId1 = -1;                           // Font ID 1
static s8 fontId2 = -2;                           // Font ID 2


void Team_Credits_Load() {
	printf("Team_Credits Load\n");

	totalElapsedTime = 0.0f;

    // -------------------------------------------------------------------
    // LOAD FONT
    // -------------------------------------------------------------------
    fontId1 = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId1 < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId1);

    fontId2 = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 40);
    if (fontId2 < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId2);

    // Load Digipen logo texture
    //pSchoolLogo = AEGfxTextureLoad("Assets/Graphics/DigiPen_Singapore_WEB_RED.png");
    //if (!pSchoolLogo)
    //    printf("ERROR: Failed to load DigiPen logo\n");
    //else
    //    printf("SUCCESS: DigiPen logo loaded\n");

    //pLogoMesh = CreateSquareMesh(0.5, 0.5);
    //printf(pLogoMesh ? "SUCCESS: Logo mesh created\n" : "ERROR: Failed to create logo mesh\n");
}

void Team_Credits_Init() {
    printf("Team Credits Init called\n");
    fade = 1.0f;  // Reset fade when entering screen
}
void Team_Credits_Update() {
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
        next = GS_MAINMENU;
}
void Team_Credits_Draw() {
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    s32 windowWidth = AEGfxGetWindowWidth();
    s32 windowHeight = AEGfxGetWindowHeight();

    // -------------------------------------------------------------------
    // DRAW COPYRIGHT TEXT (FIXED)
    // -------------------------------------------------------------------
    if (fontId1 >= 0)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        AEGfxPrint(
            fontId1,
            "All content (c) 2025 DigiPen Institute of Technology Singapore. All Rights Reserved.",
            -0.50f,   // X (normalized)
            -0.9f,    // Y (normalized)
            0.6f,     // Scale
            1.0f, 1.0f, 1.0f, fade);

        AEGfxSetBlendMode(AE_GFX_BM_NONE);  // Reset blend mode
    }

    if (fontId2 >= 0)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        AEGfxPrint(
            fontId2,
            "FATTY PATTIES",
            -0.30f,   // X (normalized)
            0.0f,    // Y (normalized)
            3.0f,     // Scale
            1.0f, 1.0f, 1.0f, fade);

        AEGfxSetBlendMode(AE_GFX_BM_NONE);  // Reset blend mode
    }
}
void Team_Credits_Free() {
    /*printf("Team Credits Free called\n");*/
}
void Team_Credits_Unload() {
    if (fontId1 >= 0)
    {
        AEGfxDestroyFont(fontId1);
        fontId1 = -1;
    }

    if (fontId2 >= 0)
    {
        AEGfxDestroyFont(fontId2);
        fontId2 = -1;
    }

    if (fade == 0)
    {
        fade = 1.0f;
    }
}