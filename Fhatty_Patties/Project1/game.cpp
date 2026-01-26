// ---------------------------------------------------------------------------
// includes
#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "GameStateManager.h"
#include "game.h"

// ---------------------------------------------------------------------------
// Global variables for Game State
static AEGfxVertexList* pMesh = nullptr;  // Square mesh
static AEMtx33 transform, scale, translate;  // Transformation matrices

// ---------------------------------------------------------------------------
// Game State Functions

void Game_Init() {
    printf("Game_Init called\n");
}

void Game_Load() {
    printf("Game_Load called\n");

    // -------------------------------------------------------------------
    // CREATE THE SQUARE MESH
    // -------------------------------------------------------------------
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,  // Bottom-left
        0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,   // Bottom-right
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );
    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,   // Bottom-right
        0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,    // Top-right
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    pMesh = AEGfxMeshEnd();

    printf("Square mesh created\n");
}

void Game_Update() {
    // Your update logic goes here
    // Example: Move the square, check collisions, etc.
}

void Game_Draw() {
    // -------------------------------------------------------------------
    // RENDERING SETUP
    // -------------------------------------------------------------------
    AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f); // Gray background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // -------------------------------------------------------------------
    // DRAW SQUARE
    // -------------------------------------------------------------------
    // Set color (maroon)
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f); // Maroon color

    // Create transformation: scale then translate
    AEMtx33Scale(&scale, 100.0f, 100.0f);           // 100x100 pixel square
    AEMtx33Trans(&translate, 0.0f, 0.0f);          // Center of screen
    AEMtx33Concat(&transform, &translate, &scale);

    // Apply transformation and draw
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
}

void Game_Free() {
    printf("Game_Free called\n");
    // Free any dynamic memory here
}

void Game_Unload() {
    printf("Game_Unload called\n");

    // Clean up mesh
    if (pMesh) {
        AEGfxMeshFree(pMesh);
        pMesh = nullptr;
        printf("Square mesh freed\n");
    }
}