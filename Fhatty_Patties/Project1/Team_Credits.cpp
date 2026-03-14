#include "pch.hpp"
static AEGfxTexture* pTeamLogo = nullptr;      // DigiPen logo texture
static AEGfxVertexList* pTeamMesh = nullptr;     // Mesh for logo
static float totalElapsedTime = 0.0f;            // Timer for screen duration
static float fade = 1.0f;                   // Start fully visible
static s8 fontId = -1;                           // Font ID

void Team_Credits_Load() {
	printf("Team_Credits Load\n");

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
void Team_Credits_Free() {
    /*printf("Team Credits Free called\n");*/
}
void Team_Credits_Unload() {
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