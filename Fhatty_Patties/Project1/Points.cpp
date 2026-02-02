// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for Point State
static AEGfxVertexList* BarMesh = nullptr;  // Square mesh
static AEGfxVertexList* s1Mesh = nullptr;  // Star mesh
static AEGfxVertexList* s2Mesh = nullptr;  // Star mesh
static AEGfxVertexList* s3Mesh = nullptr;  // Star mesh

// FIXED: Removed duplicate declarations
static AEMtx33 BarTransform, BarScale, BarTranslate;  // Bar transformation matrices
static AEMtx33 TempTransform, StarTransform, StarScale, StarRotate, StarTranslate;  // Star transformation matrices

// ---------------------------------------------------------------------------
// Points State Functions

void Points_Init() {
    printf("Points_Init called\n");
}

void Points_Load() {
    printf("Points_Load called\n");
}

void Points_Update() {
    // Your update logic goes here
    // Example: Move the square, check collisions, etc.
}

void Points_Draw() {
    // -------------------------------------------------------------------
    // DRAW POINT BAR
    // -------------------------------------------------------------------
    BarMesh = SquareMesh(0.5f, 0.5f);
    AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f); // Gray color

    // Create transformation: scale then translate
    AEMtx33Scale(&BarScale, 500.0f, 40.0f);
    AEMtx33Trans(&BarTranslate, 0.0f, 400.0f);  // Top of screen
    AEMtx33Concat(&BarTransform, &BarTranslate, &BarScale);

    // Apply transformation and draw
    AEGfxSetTransform(BarTransform.m);
    AEGfxMeshDraw(BarMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // DRAW STAR 1
    // -------------------------------------------------------------------
    s1Mesh = StarMesh(0.5f, 0.2f);  // Adjusted inner radius for better star shape
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color

    AEMtx33Scale(&StarScale, 50.0f, 50.0f);
    AEMtx33RotDeg(&StarRotate, 180.0f);
    AEMtx33Trans(&StarTranslate, 250.0f, 400.0f);  // Left side of points bar
    AEMtx33Concat(&TempTransform, &StarRotate, &StarScale);
    AEMtx33Concat(&StarTransform, &StarTranslate, &TempTransform);

    // Apply transformation and draw
    AEGfxSetTransform(StarTransform.m);
    AEGfxMeshDraw(s1Mesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // DRAW STAR 2
    // -------------------------------------------------------------------
    s2Mesh = StarMesh(0.5f, 0.2f);  // Adjusted inner radius for better star shape
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color

    AEMtx33Scale(&StarScale, 50.0f, 50.0f);
    AEMtx33RotDeg(&StarRotate, 180.0f);
    AEMtx33Trans(&StarTranslate, 170.0f, 400.0f);  // Left side of points bar
    AEMtx33Concat(&TempTransform, &StarRotate, &StarScale);
    AEMtx33Concat(&StarTransform, &StarTranslate, &TempTransform);

    // Apply transformation and draw
    AEGfxSetTransform(StarTransform.m);
    AEGfxMeshDraw(s2Mesh, AE_GFX_MDM_TRIANGLES);
    // -------------------------------------------------------------------
    // DRAW STAR 3
    // -------------------------------------------------------------------
    s3Mesh = StarMesh(0.5f, 0.2f);  // Adjusted inner radius for better star shape
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Yellow color

    AEMtx33Scale(&StarScale, 50.0f, 50.0f);
    AEMtx33RotDeg(&StarRotate, 180.0f);
    AEMtx33Trans(&StarTranslate, 50.0f, 400.0f);  // Left side of points bar
    AEMtx33Concat(&TempTransform, &StarRotate, &StarScale);
    AEMtx33Concat(&StarTransform, &StarTranslate, &TempTransform);

    // Apply transformation and draw
    AEGfxSetTransform(StarTransform.m);
    AEGfxMeshDraw(s3Mesh, AE_GFX_MDM_TRIANGLES);
}

void Points_Free() {
    printf("Points_Free called\n");
    // Free any dynamic memory here
}

void Points_Unload() {
    printf("Points_Unload called\n");

    // Clean up bar mesh
    if (BarMesh) {
        AEGfxMeshFree(BarMesh);
        BarMesh = nullptr;
        printf("Bar mesh freed\n");
    }

    // Clean up star mesh
    if (s1Mesh) {
        AEGfxMeshFree(s1Mesh);
        s1Mesh = nullptr;
        printf("Star 1 mesh freed\n");
    }

    // Clean up star mesh
    if (s2Mesh) {
        AEGfxMeshFree(s2Mesh);
        s2Mesh = nullptr;
        printf("Star 2 mesh freed\n");
    }

    // Clean up star mesh
    if (s3Mesh) {
        AEGfxMeshFree(s3Mesh);
        s3Mesh = nullptr;
        printf("Star 3 mesh freed\n");
    }
}