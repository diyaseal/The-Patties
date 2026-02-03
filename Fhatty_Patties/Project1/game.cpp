// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for Game State
static AEGfxVertexList* TableMesh = nullptr;  // Square mesh
static AEGfxVertexList* PlateMesh = nullptr;  // circle mesh
AEGfxTexture* Ingredients = nullptr;
AEGfxTexture* CustomerTexture = nullptr; // Customer sprite sheet texture

static AEMtx33 TableTransform, TableScale, TableTranslate, PlateTransform, PlateTranslate, PlateScale,
               PattieTransform, PattieScale, PattieTranslate,
               CheeseTransform, CheeseScale, CheeseTranslate,
               TomatoTransform, TomatoScale, TomatoTranslate,
               LettuceTransform, LettuceScale, LettuceTranslate,
               TBurgerTransform, TBurgerScale, TBurgerTranslate,
               BBurgerTransform, BBurgerScale, BBurgerTranslate;  // Transformation matrices

// Ingredient positions on the plate
static AEMtx33 PlateLettuceTransform, PlateLettuceScale, PlateLettuceTranslate;

// Clickable box positions 
static float BunTopX = -600.0f;
static float BunTopY = -129.0f;
static float BunTopWidth = 90.0f;
static float BunTopHeight = 94.5f;  // 30.0f * 3.15f

static float BunBottomX = -600.0f;
static float BunBottomY = -325.0f;
static float BunBottomWidth = 90.0f;
static float BunBottomHeight = 93.0f;  // 30.0f * 3.1f

static float PattieX = -350.0f;
static float PattieY = -129.0f;
static float PattieWidth = 90.0f;
static float PattieHeight = 90.0f;

static float CheeseX = -100.0f;
static float CheeseY = -129.0f;
static float CheeseWidth = 90.0f;
static float CheeseHeight = 90.0f;

static float LettuceX = -100.0f;
static float LettuceY = -325.0f;
static float LettuceWidth = 90.0f;
static float LettuceHeight = 90.0f;

static float TomatoX = -350.0f;
static float TomatoY = -325.0f;
static float TomatoWidth = 90.0f;
static float TomatoHeight = 90.0f;

// Track what's on the plate
static bool lettuceOnPlate = false;
static bool cheeseOnPlate = false;
static bool pattieOnPlate = false;
static bool tomatoOnPlate = false;
static bool bunTopOnPlate = false;
static bool bunBottomOnPlate = false; 


// ---------------------------------------------------------------------------
// Game State Functions

void Game_Init() {
    printf("Game_Init called\n");
	Customer_Init();
    Points_Init();
}

void Game_Load() {
    printf("Game_Load called\n");

    // Load meshes ONCE
    TableMesh = SquareMesh(8.0f, 2.5f);
    PlateMesh = CircleMesh(2.0f, 32);

    // Load ingredient sprite sheet
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");

    if (Ingredients) {
        printf("Ingredients texture loaded successfully\n");
    }
    else {
        printf("ERROR: Failed to load Ingredients.png\n");
    }

    // Load customer sprite sheet
    CustomerTexture = AEGfxTextureLoad("Assets/Assets/testChar.png");

    if (CustomerTexture) {
    }
    else {
        printf("ERROR: Failed to load testCharacter.png\n");
    }

    printf("Rectangle mesh created\n");
}

void Game_Update() {
    // Your update logic goes here
    // Example: Move the square, check collisions, etc.
    double dt = AEFrameRateControllerGetFrameTime();
    Customer_Update(dt);
    Points_Update();

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to world coordinates
    float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

    float worldMouseX = (float)mouseX - windowWidth / 2.0f;
    float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

    // Check for mouse click
    if (AEInputCheckTriggered(AEVK_LBUTTON)) {
        printf("Mouse clicked at world: (%.2f, %.2f)\n", worldMouseX, worldMouseY);

        // Check if lettuce was clicked
        if (worldMouseX >= LettuceX - LettuceWidth / 2.0f &&
            worldMouseX <= LettuceX + LettuceWidth / 2.0f &&
            worldMouseY >= LettuceY - LettuceHeight / 2.0f &&
            worldMouseY <= LettuceY + LettuceHeight / 2.0f) {

            lettuceOnPlate = true;
            printf("Lettuce clicked! Adding to plate\n");
        }

        // Check if cheese was clicked
        if (worldMouseX >= CheeseX - CheeseWidth / 2.0f &&
            worldMouseX <= CheeseX + CheeseWidth / 2.0f &&
            worldMouseY >= CheeseY - CheeseHeight / 2.0f &&
            worldMouseY <= CheeseY + CheeseHeight / 2.0f) {

            cheeseOnPlate = true;
            printf("Cheese clicked! Adding to plate\n");
        }

        // Check if pattie was clicked
        if (worldMouseX >= PattieX - PattieWidth / 2.0f &&
            worldMouseX <= PattieX + PattieWidth / 2.0f &&
            worldMouseY >= PattieY - PattieHeight / 2.0f &&
            worldMouseY <= PattieY + PattieHeight / 2.0f) {

            pattieOnPlate = true;
            printf("Pattie clicked! Adding to plate\n");
        }

        // Check if tomato was clicked
        if (worldMouseX >= TomatoX - TomatoWidth / 2.0f &&
            worldMouseX <= TomatoX + TomatoWidth / 2.0f &&
            worldMouseY >= TomatoY - TomatoHeight / 2.0f &&
            worldMouseY <= TomatoY + TomatoHeight / 2.0f) {

            tomatoOnPlate = true;
            printf("Tomato clicked! Adding to plate\n");
        }

        // Check if bun top was clicked
        if (worldMouseX >= BunTopX - BunTopWidth / 2.0f &&
            worldMouseX <= BunTopX + BunTopWidth / 2.0f &&
            worldMouseY >= BunTopY - BunTopHeight / 2.0f &&
            worldMouseY <= BunTopY + BunTopHeight / 2.0f) {

            bunTopOnPlate = true;
            printf("Bun Top clicked! Adding to plate\n");
        }

        // Check if bun bottom was clicked
        if (worldMouseX >= BunBottomX - BunBottomWidth / 2.0f &&
            worldMouseX <= BunBottomX + BunBottomWidth / 2.0f &&
            worldMouseY >= BunBottomY - BunBottomHeight / 2.0f &&
            worldMouseY <= BunBottomY + BunBottomHeight / 2.0f) {

            bunBottomOnPlate = true;
            printf("Bun Bottom clicked! Adding to plate\n");
        }

    }
}

void Game_Draw() {
    // -------------------------------------------------------------------
    // RENDERING SETUP
    // -------------------------------------------------------------------
    AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f); // Gray background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");
    CustomerTexture = AEGfxTextureLoad("Assets/Assets/testChar.png");

    // -------------------------------------------------------------------
    // DRAW CUSTOMERS (draw first so they appear behind counter)
    // -------------------------------------------------------------------
    Customer_Draw(CustomerTexture);

	AEGfxSetRenderMode(AE_GFX_RM_COLOR); //Reset to color mode for table and plate

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

    //Food tray mesh Burger Bun Top
    TableMesh = SquareMesh(3.0f, 3.15f);
    AEGfxSetColorToMultiply(0.96f, 0.96f, 0.86f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -600.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&TBurgerScale, 200, 200);
    AEMtx33Trans(&TBurgerTranslate, -600.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TBurgerTransform, &TBurgerTranslate, &TBurgerScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 3/4.f, 0);
    AEGfxSetTransform(TBurgerTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    //Food tray mesh Burger Bun Bottom
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    TableMesh = SquareMesh(3.0f, 3.1f);
    AEGfxSetColorToMultiply(0.96f, 0.96f, 0.86f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -600.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&BBurgerScale, 200, 200);           // 100x100 pixel square
    AEMtx33Trans(&BBurgerTranslate, -600.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&BBurgerTransform, &BBurgerTranslate, &BBurgerScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 3/4.f, 1/2.f);
    AEGfxSetTransform(BBurgerTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 1 Pattie
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(0.6f, 0.3f, 0.1f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -350.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&PattieScale, 200.f, 200.f); 
    AEMtx33Trans(&PattieTranslate, -350.0f, -129.0f);
    AEMtx33Concat(&PattieTransform, &PattieTranslate, &PattieScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 0, 0);
    AEGfxSetTransform(PattieTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5 ,0.5), AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 2 Cheese
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -100.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&CheeseScale, 200, 200);           // 100x100 pixel square
    AEMtx33Trans(&CheeseTranslate, -100.0f, -129.0f);          // Center of screen
    AEMtx33Concat(&CheeseTransform, &CheeseTranslate, &CheeseScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 0, 1/2.f);
    AEGfxSetTransform(CheeseTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 3 Lettuce 
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -100.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&LettuceScale, 200, 200);           // 100x100 pixel square
    AEMtx33Trans(&LettuceTranslate, -100.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&LettuceTransform, &LettuceTranslate, &LettuceScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 2/4.f, 0);
    AEGfxSetTransform(LettuceTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    //Food tray mesh square 4 tomato
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    TableMesh = SquareMesh(3.0f, 3.0f);
    AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
    AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    AEMtx33Trans(&TableTranslate, -350.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    AEMtx33Scale(&TomatoScale, 200, 200);           // 100x100 pixel square
    AEMtx33Trans(&TomatoTranslate, -350.0f, -325.0f);          // Center of screen
    AEMtx33Concat(&TomatoTransform, &TomatoTranslate, &TomatoScale);
    // Apply transformation and draw
    AEGfxSetTransform(TableTransform.m);
    AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxTextureSet(Ingredients, 1/4.f, 0);
    AEGfxSetTransform(TomatoTransform.m);
    AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    //Plate mesh
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    PlateMesh = CircleMesh(2.0f, 32);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    // Create transformation: scale then translate
    AEMtx33Scale(&PlateScale, 100.0f, 50.0f);           // 100x100 pixel square
    AEMtx33Trans(&PlateTranslate, 400.0f, -225.0f);          // Center of screen
    AEMtx33Concat(&PlateTransform, &PlateTranslate, &PlateScale);
    // Apply transformation and draw
    AEGfxSetTransform(PlateTransform.m);
    AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

    Points_Draw();

    // -------------------------------------------------------------------
   // DRAW INGREDIENTS ON PLATE (STACKED)
   // -------------------------------------------------------------------

    float plateX = 400.0f;
    float plateY = -225.0f;
    float ingredientSize = 150.0f;

    // Draw ingredients on plate if clicked
    if (bunBottomOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3 / 4.f, 1 / 2.f);  // Bun bottom sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY - 20.0f);  // Below lettuce
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }
    if (lettuceOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 2 / 4.f, 0);  // Lettuce sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }
    if (cheeseOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0, 1 / 2.f);  // Cheese sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY + 20.0f);  // Slightly above lettuce
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }

    if (pattieOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0, 0);  // Pattie sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY + 40.0f);  // Above cheese
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }

    if (tomatoOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 1 / 4.f, 0);  // Tomato sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY + 60.0f);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }


    if (bunTopOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3 / 4.f, 0);  // Bun top sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, plateX, plateY + 80.0f);  // On top
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }

}

void Game_Free() {
    printf("Game_Free called\n");
	Customer_Free();
    // Free any dynamic memory here
    Points_Free();
}

void Game_Unload() {
    printf("Game_Unload called\n");

    // Clean up mesh
    if (TableMesh) {
        AEGfxMeshFree(TableMesh);
        TableMesh = nullptr;
        printf("Table mesh freed\n");
    }
    if (PlateMesh) {
        AEGfxMeshFree(PlateMesh);
        PlateMesh = nullptr;
        printf("Plate mesh freed\n");
    }

    // Free texture
    if (Ingredients) {
        AEGfxTextureUnload(Ingredients);
        Ingredients = nullptr;
        printf("Ingredients texture freed\n");
    }

    // Free customer texture
    if (CustomerTexture) {
        AEGfxTextureUnload(CustomerTexture);
        CustomerTexture = nullptr;
        printf("Customer texture freed\n");
    }

    Points_Unload();
}

