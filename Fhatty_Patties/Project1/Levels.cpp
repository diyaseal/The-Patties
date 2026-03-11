// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for School Credits

static float totalElapsedTime = 0.0f;            // Timer for screen duration
static s8 fontId = -1;                           // Font ID

// ---------------------------------------------------------------------------

static Button playButton;
static AEGfxVertexList* pButtonMesh = nullptr;

// ---------------------------------------------------------------------------
// Game State Functions

void Levels_Load()
{
    //Load font
    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId);

    //Create a button mesh
    pButtonMesh = CreateSquareMesh(0.5f, 0.5f);
}

void Levels_Init()
{
    totalElapsedTime = 0.0f;

    // Initialize button properties
    playButton.x = 0.0f;        // Center of screen
    playButton.y = 0.0f;
    playButton.width = 200.0f;
    playButton.height = 80.0f;
    playButton.isHovered = false;
    playButton.isClicked = false;
}

void Levels_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    // Use utility function to check hover state
    playButton.isHovered = IsButtonHovered(playButton);

    // Use utility function to check if button is clicked
    if (IsButtonClicked(playButton))
    {
        playButton.isClicked = true;
        next = GS_GAME;  // Go to next level
    }
}

void Levels_Draw()
{
    //Render Settings
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    //if mouse is hovering, change color
    if (playButton.isHovered)
        AEGfxSetColorToMultiply(0.7f, 0.7f, 0.7f, 1.0f); // Lighter color
    else
        AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f); // Normal color

    //Set transformation for button
    AEMtx33 scale, translate, transform;
    AEMtx33Scale(&scale, playButton.width, playButton.height);
    AEMtx33Trans(&translate, playButton.x, playButton.y);
    AEMtx33Concat(&transform, &scale, &translate);
    AEGfxSetTransform(transform.m);

    //Draw button mesh
    AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_TRIANGLES);

    //Button Text
    if (fontId >= 0)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        AEGfxPrint(
            fontId,
            "Play",
            -0.05f,   // X (normalized)
            -0.03f,    // Y (normalized)
            0.9f,     // Scale
            0.0f, 0.0f, 0.0f, 1.0f
        );
    }
}

void Levels_Free()
{
    // Free the button mesh
    if (pButtonMesh)
    {
        AEGfxMeshFree(pButtonMesh);
        pButtonMesh = nullptr;
    }
}

void Levels_Unload()
{

}
