/* Start Header ************************************************************************/
/*!
\file       LevelComplete.cpp
\brief      Level Complete screen - shows "Level Complete!" and a Back to Menu button.
*/
/* End Header **************************************************************************/
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Static variables
// ---------------------------------------------------------------------------
static s8                fontId = -1;
static Button            menuButton;
static AEGfxVertexList* pButtonMesh = nullptr;

// ---------------------------------------------------------------------------
// LevelComplete_Load
// ---------------------------------------------------------------------------
void LevelComplete_Load()
{
    printf("LevelComplete: Load\n");

    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId < 0)
        printf("ERROR: LevelComplete failed to load font!\n");
    else
        printf("SUCCESS: LevelComplete font loaded, ID = %d\n", fontId);

    pButtonMesh = CreateSquareMesh(0.5f, 0.5f);
}

// ---------------------------------------------------------------------------
// LevelComplete_Init
// ---------------------------------------------------------------------------
void LevelComplete_Init()
{
    printf("LevelComplete: Init\n");

    menuButton.x = -10.0f;
    menuButton.y = -30.0f;
    menuButton.width = 280.0f;
    menuButton.height = 80.0f;
    menuButton.isHovered = false;
    menuButton.isClicked = false;
}

// ---------------------------------------------------------------------------
// LevelComplete_Update
// ---------------------------------------------------------------------------
void LevelComplete_Update()
{
    menuButton.isHovered = IsButtonHovered(menuButton);

    if (IsButtonClicked(menuButton))
    {
        next = GS_MAINMENU;
    }
}

// ---------------------------------------------------------------------------
// LevelComplete_Draw
// ---------------------------------------------------------------------------
void LevelComplete_Draw()
{
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // ------------------------------------------------------------------
    // Draw button background
    // ------------------------------------------------------------------
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    if (menuButton.isHovered)
        AEGfxSetColorToMultiply(0.7f, 0.7f, 0.7f, 1.0f);  // lighter on hover
    else
        AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);  // red (matches MainMenu)

    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, menuButton.width, menuButton.height);
    AEMtx33Trans(&translate, menuButton.x, menuButton.y);
    AEMtx33Concat(&transform, &translate, &scale);  // translate * scale (correct order)
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_TRIANGLES);

    // ------------------------------------------------------------------
    // Draw text
    // ------------------------------------------------------------------
    if (fontId >= 0)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        // "Level Complete!" title - gold
        AEGfxPrint(
            fontId,
            "Level Complete!",
            -0.22f,   // X normalized
            0.15f,   // Y normalized
            1.2f,    // scale
            1.0f, 0.85f, 0.0f, 1.0f
        );

        // Button label - black
        AEGfxPrint(
            fontId,
            "Back to Menu",
            -0.15f,   // X normalized
            -0.08f,   // Y normalized (sits inside button)
            0.9f,    // scale
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
}

// ---------------------------------------------------------------------------
// LevelComplete_Free
// ---------------------------------------------------------------------------
void LevelComplete_Free()
{
    printf("LevelComplete: Free\n");
}

// ---------------------------------------------------------------------------
// LevelComplete_Unload
// --------------------------------------- ------------------------------------
void LevelComplete_Unload()
{
    printf("LevelComplete: Unload\n");

    if (pButtonMesh)
    {
        AEGfxMeshFree(pButtonMesh);
        pButtonMesh = nullptr;
    }

    if (fontId >= 0)
    {
        AEGfxDestroyFont(fontId);
        fontId = -1;
    }
}