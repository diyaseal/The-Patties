#include "pch.hpp"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static float totalElapsedTime = 0.0f;
static float fade = 1.0f;

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------
enum CreditScreen { SCREEN_SCHOOL, SCREEN_TEAM };
static CreditScreen currentScreen = SCREEN_SCHOOL;

// ---------------------------------------------------------------------------
// School Credits Assets
// ---------------------------------------------------------------------------
static AEGfxTexture* pSchoolLogo = nullptr;
static AEGfxVertexList* pLogoMesh = nullptr;
static s8               fontId0 = -1;

// ---------------------------------------------------------------------------
// Team Credits Assets
// ---------------------------------------------------------------------------
static AEGfxTexture* pTeamLogo = nullptr;
static AEGfxVertexList* pTeamMesh = nullptr;
static AEMtx33          TeamTransform, TeamTranslate, TeamScale;
static s8               fontId1 = -1;
static s8               fontId2 = -1;

// ---------------------------------------------------------------------------
// Start Credits State Functions
// ---------------------------------------------------------------------------
void Start_Credits_Load()
{
    printf("Credits Load\n");

    // School assets
    fontId0 = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    pSchoolLogo = AEGfxTextureLoad("Assets/Assets/DigiPen_Singapore_WEB_RED.png");
    pLogoMesh = CreateSquareMesh(0.5f, 0.5f);

    // Team assets
    fontId1 = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    fontId2 = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 40);
    pTeamLogo = AEGfxTextureLoad("Assets/Assets/Food_truck.png");
    pTeamMesh = CreateSquareMesh(0.5f, 0.5f);
}

void Start_Credits_Init()
{
    printf("Credits Init called\n");
    totalElapsedTime = 0.0f;
    fade = 1.0f;
    currentScreen = SCREEN_SCHOOL; // starts with school
}

void Start_Credits_Update()
{
    float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
    totalElapsedTime += dt;

    // Fade out after 2 seconds over 1 second
    if (totalElapsedTime > 2.0f)
    {
        fade = 1.0f - ((totalElapsedTime - 2.0f) / 1.0f);
        if (fade < 0.0f) fade = 0.0f;
    }

    if (totalElapsedTime > 3.0f)
    {
        if (currentScreen == SCREEN_SCHOOL)
        {
            // Switch to team screen — reset timer and fade
            currentScreen = SCREEN_TEAM;
            totalElapsedTime = 0.0f;
            fade = 1.0f;
        }
        else
        {
            // Both screens done — go to main menu
            next = GS_MAINMENU;
        }
    }
}

void Start_Credits_Draw()
{
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    s32 windowWidth = AEGfxGetWindowWidth();
    s32 windowHeight = AEGfxGetWindowHeight();

    if (currentScreen == SCREEN_SCHOOL)
    {
        // -------------------------------------------------------------------
        // DRAW SCHOOL LOGO
        // -------------------------------------------------------------------
        if (pSchoolLogo && pLogoMesh)
        {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxSetTransparency(fade);
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
            AEGfxSetBlendMode(AE_GFX_BM_NONE);
        }

        // -------------------------------------------------------------------
        // DRAW SCHOOL COPYRIGHT
        // -------------------------------------------------------------------
        if (fontId0 >= 0)
        {
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxPrint(fontId0,
                "All content (c) 2026 DigiPen Institute of Technology Singapore. All Rights Reserved.",
                -0.50f, -0.9f, 0.6f, 1.0f, 1.0f, 1.0f, fade);
            AEGfxSetBlendMode(AE_GFX_BM_NONE);
        }
    }
    else // SCREEN_TEAM
    {
        // -------------------------------------------------------------------
        // DRAW TEAM LOGO
        // -------------------------------------------------------------------
        if (pTeamLogo && pTeamMesh)
        {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxSetTransparency(fade);

            AEMtx33Scale(&TeamScale, 600.0f, 300.0f);
            AEMtx33Trans(&TeamTranslate, 10.0f, 100.0f);
            AEMtx33Concat(&TeamTransform, &TeamTranslate, &TeamScale);
            AEGfxSetTransform(TeamTransform.m);
            AEGfxTextureSet(pTeamLogo, 0, 0.f);
            AEGfxMeshDraw(pTeamMesh, AE_GFX_MDM_TRIANGLES);
        }

        // -------------------------------------------------------------------
        // DRAW TEAM NAME
        // -------------------------------------------------------------------
        if (fontId2 >= 0)
        {
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxPrint(fontId2, "THE PATTIES",
                -0.38f, -0.3f, 2.6f, 1.0f, 1.0f, 1.0f, fade);
            AEGfxSetBlendMode(AE_GFX_BM_NONE);
        }

        // -------------------------------------------------------------------
        // DRAW TEAM COPYRIGHT
        // -------------------------------------------------------------------
        if (fontId1 >= 0)
        {
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxPrint(fontId1,
                "All content (c) 2026 DigiPen Institute of Technology Singapore. All Rights Reserved.",
                -0.50f, -0.9f, 0.6f, 1.0f, 1.0f, 1.0f, fade);
            AEGfxSetBlendMode(AE_GFX_BM_NONE);
        }
    }
}

void Start_Credits_Free() {}

void Start_Credits_Unload()
{
    printf("Credits Unload called\n");

    if (pLogoMesh) { AEGfxMeshFree(pLogoMesh);        pLogoMesh = nullptr; }
    if (pSchoolLogo) { AEGfxTextureUnload(pSchoolLogo);  pSchoolLogo = nullptr; }
    if (fontId0 >= 0) { AEGfxDestroyFont(fontId0);         fontId0 = -1; }

    if (pTeamMesh) { AEGfxMeshFree(pTeamMesh);         pTeamMesh = nullptr; }
    if (pTeamLogo) { AEGfxTextureUnload(pTeamLogo);    pTeamLogo = nullptr; }
    if (fontId1 >= 0) { AEGfxDestroyFont(fontId1);       fontId1 = -1; }
    if (fontId2 >= 0) { AEGfxDestroyFont(fontId2);       fontId2 = -1; }
}