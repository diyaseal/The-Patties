// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for Game State
static AEGfxVertexList* TableMesh = nullptr;  // Square mesh
static AEGfxVertexList* PlateMesh = nullptr;  //
static AEMtx33 TableTransform, TableScale, TableTranslate, PlateTransform, PlateTranslate, PlateScale;  // Transformation matrices

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
    printf("Rectangle mesh created\n");
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
    
    TableMesh = SquareMesh(8.0f, 2.5f);
    // Set color (maroon)
    AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f); // Maroon color
    // Create transformation: scale then translate
    AEMtx33Scale(&TableScale, 100.0f, 100.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, 0.0f, -225.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Food tray mesh Burger Bun
    TableMesh = SquareMesh(3.0f, 6.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -600.0f, -225.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 1 Pattie
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -350.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 2 Cheese
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -100.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 3 Lettuce 
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -100.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 4 tomato
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -350.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    //Plate mesh
    PlateMesh = CircleMesh(2.0f, 32);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    // Create transformation: scale then translate
    AEMtx33Scale(&PlateScale, 100.0f, 50.0f);           // 100x100 pixel square
    AEMtx33Trans(&PlateTranslate, 400.0f, -225.0f);          // Center of screen
    AEMtx33Concat(&PlateTransform, &PlateTranslate, &PlateScale);
    // Apply transformation and draw
    AEGfxSetTransform(PlateTransform.m);
    AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

}

void Game_Free() {
    printf("Game_Free called\n");
    // Free any dynamic memory here
}

void Game_Unload() {
    printf("Game_Unload called\n");

    // Clean up mesh
    if (TableMesh) {
        AEGfxMeshFree(TableMesh);
        TableMesh = nullptr;
        printf("Square mesh freed\n");
    }
}