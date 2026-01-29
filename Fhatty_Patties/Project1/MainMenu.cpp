// ---------------------------------------------------------------------------
// includes
#include "pch.h"

// ---------------------------------------------------------------------------
// Global variables for School Credits

static float totalElapsedTime = 0.0f;            // Timer for screen duration

// ---------------------------------------------------------------------------
// Button Struct
struct Button
{
    float x, y;         // Position
    float width, height; // Size
    bool isHovered;    // Hover state
	bool isClicked;    // Click state
};

static Button playButton;
static AEGfxVertexList* pButtonMesh = nullptr;

// ---------------------------------------------------------------------------
// Game State Functions

void MainMenu_Load()
{
 //Create a button mesh
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
	pButtonMesh = AEGfxMeshEnd();
}

void MainMenu_Init()
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

void MainMenu_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert mouse position to world coordinates
    float worldX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
    float worldY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

    // Check if mouse is hovering over button
    playButton.isHovered = (worldX >= playButton.x - playButton.width / 2.0f &&
        worldX <= playButton.x + playButton.width / 2.0f &&
        worldY >= playButton.y - playButton.height / 2.0f &&
        worldY <= playButton.y + playButton.height / 2.0f);

    // Check if button is clicked
    if (playButton.isHovered && AEInputCheckTriggered(AEVK_LBUTTON))
    {
        playButton.isClicked = true;
        next = GS_LEVEL1;  // Go to next level
    }
}

void MainMenu_Draw()
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
}

void MainMenu_Free()
{
	
}

void MainMenu_Unload()
{
    // Free the button mesh
    if (pButtonMesh)
    {
        AEGfxMeshFree(pButtonMesh);
        pButtonMesh = nullptr;
    }
}
