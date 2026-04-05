// ---------------------------------------------------------------------------
// Points.cpp
// Description: Points and star rating system for burger serving game.
//              Tracks points earned from satisfied customers, displays
//              a progress bar, and awards up to 3 stars based on thresholds.
//              Includes particle effects when stars are earned.
// Author: ELIZABETH
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
const f32 pointsEarnedH = 35.0f;        // Height of bars
static f32 displayPoints = 0.0f;        // Current points displayed
extern float currentPoints = 0.0f;      // Current points earned
extern int currentStars = 0;            // Current stars

static f32 MAX_POINTS = 0.0f;           // Max points to fill the bar (3 stars * 35)
static f32 BAR_WIDTH = 500.0f;          // Same width as gray bar
static const f32 BAR_HEIGHT = 40.0f;    // Same height as gray bar
static const f32 BAR_POS_X = 0.0f;      // Same X as gray bar
static const f32 BAR_POS_Y = 400.0f;    // Same Y as gray bar

// ---------------------------------------------------------------------------
// Meshes
// ---------------------------------------------------------------------------
static AEGfxVertexList* BarMesh = nullptr; 
static AEGfxVertexList* starMesh = nullptr;

static AEMtx33 BarTransform, BarScale, BarTranslate; 
static AEMtx33 TempTransform, StarTransform, StarScale, StarRotate, StarTranslate; 

// ---------------------------------------------------------------------------
// Points System
// ---------------------------------------------------------------------------
/*
@param - takes in a bool to represent customer state
1. true: customer provides 35 pts 
2. false: customer provides no pts
*/
void Points_AddPoints(bool cust) {
    if (cust) {
        currentPoints += pointsEarnedH;
    }

    if (currentPoints > MAX_POINTS) {
        currentPoints = MAX_POINTS;
    }

    // Update points + save CSV
    Level_Manager_UpdateHighestPoints(currentPoints);

    //printf("\nPoints added: %.0f | Total: %.0f\n", pointsEarnedH, currentPoints);
}


// ---------------------------------------------------------------------------
// Points State Functions
// ---------------------------------------------------------------------------
void Points_Init() {
    //printf("Points_Init called\n");

    MAX_POINTS = 0.0f; // reset first

    MAX_POINTS += Level_Manager_GetMaxPoints();

    currentPoints = 0.0f;
    displayPoints = 0.0f;
    currentStars = 0;

    //printf("MAX_POINTS = %.0f\n", MAX_POINTS);
}

void Points_Load() {
    BarMesh = CreateSquareMesh(0.5f, 0.5f);
    starMesh = CreateStarMesh(0.5f, 0.2f); 

    Particle_Load();
}

void Points_Update() {
    float dt = (float)AEFrameRateControllerGetFrameTime();
    // Smooth bar lerp
    Particle_Update(dt);
    displayPoints += (currentPoints - displayPoints) * dt * 5.0f;
}

void Points_Draw() {

    if (MAX_POINTS <= 0.0f) return; // Guard against division by zero

    // -------------------------------------------------------------------
    // DRAW GRAY BAR (background)
    // -------------------------------------------------------------------
    AEGfxSetColorToMultiply(0.2f, 0.2f, 0.2f, 1.0f); // Gray color

    AEMtx33Scale(&BarScale, BAR_WIDTH, BAR_HEIGHT);
    AEMtx33Trans(&BarTranslate, BAR_POS_X, BAR_POS_Y);
    AEMtx33Concat(&BarTransform, &BarTranslate, &BarScale);

    AEGfxSetTransform(BarTransform.m);
    AEGfxMeshDraw(BarMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // DRAW POINTS GAINED BAR (on top of gray, behind stars)
    // -------------------------------------------------------------------
    f32 pointsPercentage = displayPoints / MAX_POINTS; // use displayPoints
    if (pointsPercentage > 1.0f) pointsPercentage = 1.0f;
    if (pointsPercentage < 0.0f) pointsPercentage = 0.0f;

    if (pointsPercentage > 0.0f) {
        f32 filledWidth = BAR_WIDTH * pointsPercentage;
        // to anchor the bar
        f32 offsetX = -(BAR_WIDTH - filledWidth) / 2.0f;

        AEMtx33 pgScale, pgTranslate, pgTransform;
        AEMtx33Scale(&pgScale, filledWidth, BAR_HEIGHT);
        AEMtx33Trans(&pgTranslate, BAR_POS_X + offsetX, BAR_POS_Y);
        AEMtx33Concat(&pgTransform, &pgTranslate, &pgScale);

        AEGfxSetColorToMultiply(1.0f, 0.75f, 0.0f, 1.0f); // Gold/yellow
        AEGfxSetTransform(pgTransform.m);
        AEGfxMeshDraw(BarMesh, AE_GFX_MDM_TRIANGLES); // reuse BarMesh directly
    }

    // -------------------------------------------------------------------
    // DRAW STARS (on top of points bar)
    // -------------------------------------------------------------------
    const f32 STAR_SIZE = 50.0f;
    const f32 STAR_Y_POS = BAR_POS_Y;
    const f32 BAR_LEFT_EDGE = BAR_POS_X - BAR_WIDTH / 2.0f;

    currentStars = 0;

    for (int i = 0; i < 3; i++)
    {
        f32 threshold = Level_Manager_GetStarPosition(i);
        f32 thresholdPercentage = threshold / MAX_POINTS;
        f32 starX = BAR_LEFT_EDGE + (BAR_WIDTH * thresholdPercentage);

        bool earned = currentPoints >= threshold; // stars use real points not display

        if (earned) {
            currentStars++;
            Particle_SetStarBurst(starX, STAR_Y_POS, i); // trigger burst when earned
            AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);
        }
        else {
            AEGfxSetColorToMultiply(0.4f, 0.4f, 0.4f, 1.0f);
        }

        AEMtx33Scale(&StarScale, STAR_SIZE, STAR_SIZE);
        AEMtx33RotDeg(&StarRotate, 180.0f);
        AEMtx33Trans(&StarTranslate, starX, STAR_Y_POS);

        AEMtx33Concat(&TempTransform, &StarRotate, &StarScale);
        AEMtx33Concat(&StarTransform, &StarTranslate, &TempTransform);

        AEGfxSetTransform(StarTransform.m);

        AEGfxMeshDraw(starMesh, AE_GFX_MDM_TRIANGLES);
    }
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);          // reset mode
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f); // reset color before particles
    Particle_DrawStarBurst();
}

void Points_Free() {
    //printf("Points_Free called\n");
    Particle_Free();
}

void Points_Unload() {
    //printf("Points_Unload called\n");

    if (BarMesh) {
        AEGfxMeshFree(BarMesh);
        BarMesh = nullptr;
    }

    if (starMesh) {
        AEGfxMeshFree(starMesh);
        starMesh = nullptr;
    }

    Particle_Unload();
}