 // ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for Point State
static AEGfxVertexList* BarMesh = nullptr;  // Square mesh
static AEGfxVertexList* pointsGainedMesh = nullptr;  // Points gained bar mesh
static AEGfxVertexList* s1Mesh = nullptr;  // Star mesh
static AEGfxVertexList* s2Mesh = nullptr;  // Star mesh
static AEGfxVertexList* s3Mesh = nullptr;  // Star mesh
const f32 pointsEarnedH = 35.0f;

// Removed duplicate declarations
static AEMtx33 BarTransform, BarScale, BarTranslate;  // Bar transformation matrices
static AEMtx33 TempTransform, StarTransform, StarScale, StarRotate, StarTranslate;  // Star transformation matrices

// ---------------------------------------------------------------------------
// Points System System
// ---------------------------------------------------------------------------
static f32 MAX_POINTS = 0.0f;       // Max points to fill the bar (3 stars * 35)
static f32 currentPoints = 0.0f;            // Current points earned
static f32 BAR_WIDTH = 500.0f;        // Same width as gray bar
static const f32 BAR_HEIGHT = 40.0f;        // Same height as gray bar
static const f32 BAR_POS_X = 0.0f;          // Same X as gray bar
static const f32 BAR_POS_Y = 400.0f;        // Same Y as gray bar

// ---------------------------------------------------------------------------
// Points State Functions

void Points_Init() {
    BarMesh = CreateSquareMesh(0.5f, 0.5f);

    printf("Points_Init called\n");

    MAX_POINTS = 0.0f; // reset first

    MAX_POINTS += Level_Manager_GetMaxPoints();

    currentPoints = 0.0f;

    printf("MAX_POINTS = %.0f\n", MAX_POINTS);
}

void Points_Load() {
    printf("Loaded stars: %.0f %.0f %.0f\n",
        currentLevelData.starPositions[0],
        currentLevelData.starPositions[1],
        currentLevelData.starPositions[2]);
}

void Points_Update() {

}

// ---------------------------------------------------------------------------
//@param retrives customer state
// ---------------------------------------------------------------------------
void Points_AddPoints(bool cust) {
    //happy(true) customer provides 35 pts angry(false) customer provides no pts
    if (cust) {
        currentPoints += pointsEarnedH;
    }

    if (currentPoints > MAX_POINTS) {
        currentPoints = MAX_POINTS;
    }

    // Update points + save CSV
    Level_Manager_UpdateHighestPoints(currentPoints);

    // Display Stars Position in Console
    printf("Stars: ");
    for (int i = 0; i < 3; ++i) {
        printf("Stars: %.1f", Level_Manager_GetStarPosition(i));
    }
    printf("\n");
    printf("Points added: %.0f | Total: %.0f\n", pointsEarnedH, currentPoints);
}

void Points_Draw(){

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
    f32 pointsPercentage = currentPoints / MAX_POINTS;
    if (pointsPercentage > 1.0f) pointsPercentage = 1.0f;
    if (pointsPercentage < 0.0f) pointsPercentage = 0.0f;

    if (pointsPercentage > 0.0f) {
        pointsGainedMesh = BarMesh;

        f32 filledWidth = BAR_WIDTH * pointsPercentage;
        // Offset so it grows from the left edge of the gray bar
        f32 offsetX = -(BAR_WIDTH - filledWidth) / 2.0f;

        AEMtx33 pgScale, pgTranslate, pgTransform;
        AEMtx33Scale(&pgScale, filledWidth, BAR_HEIGHT);
        AEMtx33Trans(&pgTranslate, BAR_POS_X + offsetX, BAR_POS_Y);
        AEMtx33Concat(&pgTransform, &pgTranslate, &pgScale);

        // Gold/yellow color for the points fill
        AEGfxSetColorToMultiply(1.0f, 0.75f, 0.0f, 1.0f);
        AEGfxSetTransform(pgTransform.m);
        AEGfxMeshDraw(pointsGainedMesh, AE_GFX_MDM_TRIANGLES);
    }

    // -------------------------------------------------------------------
    // DRAW STARS (on top of points bar)
    // Color: yellow if points reached threshold, gray if not yet
    // -------------------------------------------------------------------
    const f32 STAR_SIZE = 50.0f;
    const f32 STAR_Y_POS = BAR_POS_Y;
    const f32 BAR_LEFT_EDGE = BAR_POS_X - BAR_WIDTH / 2.0f; // Leftmost point of bar

    // Calculate star positions based on bar position and thresholds
    for (int i = 0; i < 3; i++) {
        f32 threshold = Level_Manager_GetStarPosition(i);
        f32 thresholdPercentage = threshold / MAX_POINTS;

        f32 starX = BAR_LEFT_EDGE + (BAR_WIDTH * thresholdPercentage);

        AEGfxVertexList* starMesh = CreateStarMesh(0.5f, 0.2f);

        // Check if current point is greater than threshold
        if (currentPoints >= Level_Manager_GetStarPosition(i))
            AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        else
            AEGfxSetColorToMultiply(0.4f, 0.4f, 0.4f, 1.0f); // Gray

        AEMtx33Scale(&StarScale, STAR_SIZE, STAR_SIZE);
        AEMtx33RotDeg(&StarRotate, 180.0f);
        AEMtx33Trans(&StarTranslate, starX, STAR_Y_POS);
        AEMtx33Concat(&TempTransform, &StarRotate, &StarScale);
        AEMtx33Concat(&StarTransform, &StarTranslate, &TempTransform);

        AEGfxSetTransform(StarTransform.m);
        AEGfxMeshDraw(starMesh, AE_GFX_MDM_TRIANGLES);
    }
}

void Points_Free() {
    printf("Points_Free called\n");
}

void Points_Unload() {
    printf("Points_Unload called\n");

    if (BarMesh) {
        AEGfxMeshFree(BarMesh);
        BarMesh = nullptr;
    }
    if (pointsGainedMesh) {
        AEGfxMeshFree(pointsGainedMesh);
        pointsGainedMesh = nullptr;
    }
    if (s1Mesh) {
        AEGfxMeshFree(s1Mesh);
        s1Mesh = nullptr;
    }
    if (s2Mesh) {
        AEGfxMeshFree(s2Mesh);
        s2Mesh = nullptr;
    }
    if (s3Mesh) {
        AEGfxMeshFree(s3Mesh);
        s3Mesh = nullptr;
    } 
}