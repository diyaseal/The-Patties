// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"
extern std::vector<Customer> customers;
extern const float EXIT_X = 800.0f;

// ---------------------------------------------------------------------------
// Global variables for Game State:Diya
static AEGfxVertexList* TableMesh = nullptr;  // Square mesh
static AEGfxVertexList* PlateMesh = nullptr;  // circle mesh
static AEGfxVertexList* BGMesh = nullptr;  // BG mesh
AEGfxTexture* Ingredients = nullptr; // Ingredients Mesh (not static so other pages can use it)
static AEGfxTexture* GameplayBG = nullptr; // game BG
AEGfxTexture* CustomerTexture[4] = { nullptr }; // Customer sprite sheet texture (T)

static AEMtx33 TableTransform, TableScale, TableTranslate, PlateTransform, PlateTranslate, PlateScale,
               PattieTransform, PattieScale, PattieTranslate,
               CheeseTransform, CheeseScale, CheeseTranslate,
               TomatoTransform, TomatoScale, TomatoTranslate,
               LettuceTransform, LettuceScale, LettuceTranslate,
               TBurgerTransform, TBurgerScale, TBurgerTranslate,
               BBurgerTransform, BBurgerScale, BBurgerTranslate,  // Transformation matrices
               BGTransform, BGScale, BGTranslate;

// Ingredient positions on the plate
static AEMtx33 PlateLettuceTransform, PlateLettuceScale, PlateLettuceTranslate;
static std::vector<int> currentBurger;  // What player has built
static bool burgerComplete = false;     // Is burger ready to serve?

// Add near other static variables in game.cpp:
// Drag-and-drop system : Syakil 
static bool isDragging = false;
static float dragOffsetX = 0.0f;
static float dragOffsetY = 0.0f;
static float burgerDragX = 0.0f;
static float burgerDragY = 0.0f;
static bool burgerIsBeingDragged = false;

// This is the hitbox coordinates: using world map i found the 
// clickable area and averaged it to find the centre coordinates X,Y 
// then i put the hitbox area set to 100 : Diya 
static float BunTopX = -523.0f;
static float BunTopY = -81.0f;
static float BunTopWidth = 100.0f;
static float BunTopHeight = 100.0f;

static float BunBottomX = -560.0f;  
static float BunBottomY = -180.0f;  
static float BunBottomWidth = 90.0f;
static float BunBottomHeight = 90.0f; 

static float PattieX = -275.0f; //updated hitbox
static float PattieY = -110.0f;
static float PattieWidth = 90.0f;
static float PattieHeight = 90.0f;

static float CheeseX = -150.0f; //updated hitbox
static float CheeseY = -110.0f;
static float CheeseWidth = 90.0f;
static float CheeseHeight = 90.0f;

static float LettuceX = -150.0f; //updated hitbox
static float LettuceY = -170.0f;
static float LettuceWidth = 110.0f;
static float LettuceHeight = 110.0f;

static float TomatoX = -300.0f;   //updated hitbox  
static float TomatoY = -175.0f;     
static float TomatoWidth = 90.0f;   
static float TomatoHeight = 90.0f;  

// Add these with your other ingredient position variables
static float OnionX = 3.0f;          // From click (3.00, -175.00)
static float OnionY = -175.0f;
static float OnionWidth = 90.0f;
static float OnionHeight = 90.0f;

static float CucumberX = -18.0f;      // From click (-18.00, -108.00)
static float CucumberY = -108.0f;
static float CucumberWidth = 90.0f;
static float CucumberHeight = 90.0f;

// Track what's on the plate: Set 
// everything to false first :Diya
static bool lettuceOnPlate = false;
static bool cheeseOnPlate = false;
static bool pattieOnPlate = false;
static bool tomatoOnPlate = false;
static bool onionOnPlate = false;      
static bool cucumberOnPlate = false;   
static bool bunTopOnPlate = false;
static bool bunBottomOnPlate = false; 

// ---------------------------------------------------------------------------
// Helper Functions Syakil 

// Helper function to check if burger is complete (has both buns)
static bool IsBurgerComplete() {
    return bunBottomOnPlate && bunTopOnPlate;
}

// Helper to reset drag state5 Syakil
static void ResetDragState() {
    isDragging = false;
    burgerIsBeingDragged = false;
    burgerDragX = 400.0f;  // Plate position
    burgerDragY = -225.0f; // Plate position
}

// ---------------------------------------------------------------------------
// Game State Functions

void Game_Init() {
    printf("Game_Init called\n");
    // Elizabeth : Set Level Number
    Level_Manager_LoadLevel(1);
	Customer_Init();
    Points_Init();
}

void Game_Load() {
    printf("Game_Load called\n");

    // Load meshes ONCE
    TableMesh = SquareMesh(8.0f, 2.5f);
    PlateMesh = CircleMesh(2.0f, 32);

    // Load BG 
    GameplayBG = AEGfxTextureLoad("Assets/Assets/Gameplay.png");

    if (GameplayBG) {
        printf("GameplayBG texture loaded successfully\n");
    }
    else {
        printf("ERROR: Failed to load GameplayBG.png\n");
    }

    // Load ingredient sprite sheet
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");

    if (Ingredients) {
        printf("Ingredients texture loaded successfully\n");
    }
    else {
        printf("ERROR: Failed to load Ingredients.png\n");
    }

    // Load customer sprite sheet (T)
    CustomerTexture[0] = AEGfxTextureLoad("Assets/Assets/syakilcust.png");
    CustomerTexture[1] = AEGfxTextureLoad("Assets/Assets/elicust.png");
    CustomerTexture[2] = AEGfxTextureLoad("Assets/Assets/tebcust.png");
    CustomerTexture[3] = AEGfxTextureLoad("Assets/Assets/diyacust.png");

    if (CustomerTexture) {
    }
    else {
        printf("ERROR: Failed to load testCharacter.png\n");
    }

    printf("Rectangle mesh created\n");
}

bool Burger_MatchesOrder(const std::vector<int>& playerBurger,
    const std::vector<int>& customerOrder) {

    // Quick check: sizes must match
    if (playerBurger.size() != customerOrder.size()) {
        printf("FAIL: Different number of ingredients (%zu vs %zu)\n",
            playerBurger.size(), customerOrder.size());
        return false;
    }

    // Check each ingredient in order
    for (size_t i = 0; i < customerOrder.size(); i++) {
        if (playerBurger[i] != customerOrder[i]) {
            printf("FAIL: Wrong ingredient at position %zu\n", i);
            printf("  Player: %d, Customer wanted: %d\n", playerBurger[i], customerOrder[i]);
            return false;
        }
    }

    // All ingredients match!
    printf("SUCCESS: Burger matches order perfectly!\n");
    return true;
}

void Burger_ServeToCustomer() {
    // Check if burger is complete (has top bun)
    if (!burgerComplete) {
        printf("Burger is not complete! Need top bun.\n");
        return;
    }

    // Check if there's a customer
    if (customers.empty()) {
        printf("No customers to serve!\n");
        return;
    }

    // Get the first customer
    Customer& customer = customers[0];

    // Check if customer is waiting for food
    if (!customer.isWaitingForFood) {
        printf("Customer is not at counter yet!\n");
        return;
    }

    // Compare burger with customer's order
    bool isCorrect = Burger_MatchesOrder(currentBurger, customer.order);

    if (isCorrect) {
        // SUCCESS!
        printf("Customer is happy! Order correct!\n");

        // Make customer leave (walk off-screen)
        customer.isWalking = true;
        customer.isWaitingForFood = false;
        customer.targetX = EXIT_X;
        customer.targetY = customer.y;
        customer.flipHorizontal = true;

        // Clear burger for next order
        Burger_Clear();

    }
    else {
        // FAILURE
        printf("Customer is unhappy! Wrong order.\n");

        // Decrease patience
        customer.patience -= 30.0f;
        if (customer.patience < 0) customer.patience = 0;

        printf("Customer patience: %.0f/%.0f\n", customer.patience, customer.maxPatience);

        // Let player remake the burger
        Burger_Clear();
    }
}

void Game_Update() {
    // Your update logic goes here
    // Example: Move the square, check collisions, etc. :Diya 
    f64 dt = AEFrameRateControllerGetFrameTime();
    Customer_Update(static_cast<float>(dt));
    Points_Update();

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to world coordinates
    float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

    float worldMouseX = (float)mouseX - windowWidth / 2.0f;
    float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

    // Check for mouse click on the ingredients to see what player is adding to the burger
    if (AEInputCheckTriggered(AEVK_LBUTTON)) {
        printf("Mouse clicked at world: (%.2f, %.2f)\n", worldMouseX, worldMouseY);

        // Check if lettuce was clicked
        if (worldMouseX >= LettuceX - LettuceWidth / 2.0f &&
            worldMouseX <= LettuceX + LettuceWidth / 2.0f &&
            worldMouseY >= LettuceY - LettuceHeight / 2.0f &&
            worldMouseY <= LettuceY + LettuceHeight / 2.0f) {

            lettuceOnPlate = true;
            currentBurger.push_back(LETTUCE); // Track ingredient
            printf("Lettuce added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if cheese was clicked
        if (worldMouseX >= CheeseX - CheeseWidth / 2.0f &&
            worldMouseX <= CheeseX + CheeseWidth / 2.0f &&
            worldMouseY >= CheeseY - CheeseHeight / 2.0f &&
            worldMouseY <= CheeseY + CheeseHeight / 2.0f) {

            cheeseOnPlate = true;
            currentBurger.push_back(CHEESE);  // Track ingredient
            printf("Cheese added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if pattie was clicked
        if (worldMouseX >= PattieX - PattieWidth / 2.0f &&
            worldMouseX <= PattieX + PattieWidth / 2.0f &&
            worldMouseY >= PattieY - PattieHeight / 2.0f &&
            worldMouseY <= PattieY + PattieHeight / 2.0f) {

            pattieOnPlate = true;
            currentBurger.push_back(PATTY);  // Track ingredient
            printf("Patty added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if tomato was clicked
        if (worldMouseX >= TomatoX - TomatoWidth / 2.0f &&
            worldMouseX <= TomatoX + TomatoWidth / 2.0f &&
            worldMouseY >= TomatoY - TomatoHeight / 2.0f &&
            worldMouseY <= TomatoY + TomatoHeight / 2.0f) {

            tomatoOnPlate = true;
            currentBurger.push_back(TOMATO);  // NEW: Track ingredient
            printf("Tomato added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if bun top was clicked
        if (worldMouseX >= BunTopX - BunTopWidth / 2.0f &&
            worldMouseX <= BunTopX + BunTopWidth / 2.0f &&
            worldMouseY >= BunTopY - BunTopHeight / 2.0f &&
            worldMouseY <= BunTopY + BunTopHeight / 2.0f) {

            bunTopOnPlate = true;
            currentBurger.push_back(TOP_BUN);  // Track ingredient
            burgerComplete = true;  // Burger is complete!
            printf("Top bun added. Burger is complete and ready to serve!\n");
        }

        // Check if bun bottom was clicked
        if (worldMouseX >= BunBottomX - BunBottomWidth / 2.0f &&
            worldMouseX <= BunBottomX + BunBottomWidth / 2.0f &&
            worldMouseY >= BunBottomY - BunBottomHeight / 2.0f &&
            worldMouseY <= BunBottomY + BunBottomHeight / 2.0f) {

            bunBottomOnPlate = true;
            currentBurger.push_back(BOTTOM_BUN);  // Track ingredient
            printf("Bottom bun added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if onion was clicked   UNLOCK INGREDIENTS 
        if (worldMouseX >= OnionX - OnionWidth / 2.0f &&
            worldMouseX <= OnionX + OnionWidth / 2.0f &&
            worldMouseY >= OnionY - OnionHeight / 2.0f &&
            worldMouseY <= OnionY + OnionHeight / 2.0f) {

            onionOnPlate = true;
            currentBurger.push_back(ONION);  // Make sure ONION is defined in your enum
            printf("Onion added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if cucumber was clicked UUNLOCK INGREDIENTS 
        if (worldMouseX >= CucumberX - CucumberWidth / 2.0f &&
            worldMouseX <= CucumberX + CucumberWidth / 2.0f &&
            worldMouseY >= CucumberY - CucumberHeight / 2.0f &&
            worldMouseY <= CucumberY + CucumberHeight / 2.0f) {

            cucumberOnPlate = true;
            currentBurger.push_back(CUCUMBER);  // Make sure CUCUMBER is defined in your enum
            printf("Cucumber added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if player clicks on the plate (burger) to start dragging
        float plateCenterX = 400.0f;
        float plateCenterY = -225.0f;
        float plateRadius = 100.0f;  // Approximate plate radius

        // Calculate distance from mouse to plate center
        float dx = worldMouseX - plateCenterX;
        float dy = worldMouseY - plateCenterY;
        float distanceSquared = dx * dx + dy * dy;

        // Check if burger exists and mouse is over plate
        if (!currentBurger.empty() && distanceSquared <= (plateRadius * plateRadius)) {
            isDragging = true;
            burgerIsBeingDragged = true;

            // Calculate offset from burger center to mouse position
            dragOffsetX = worldMouseX - plateCenterX;
            dragOffsetY = worldMouseY - plateCenterY;

            // Start position for dragging
            burgerDragX = worldMouseX;
            burgerDragY = worldMouseY;

            printf("Started dragging burger!\n");
        }
    }

    // Continuous drag update (if burger is being dragged)
    if (isDragging && burgerIsBeingDragged) {
        // Update burger position to follow mouse
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        // Convert to world coordinates
        float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
        float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

        float worldMouseX = (float)mouseX - windowWidth / 2.0f;
        float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

        burgerDragX = worldMouseX;
        burgerDragY = worldMouseY;
    }

    // Check for mouse release
    if (AEInputCheckReleased(AEVK_LBUTTON)) {
        if (isDragging && burgerIsBeingDragged) {
            // Mouse released while dragging burger
            isDragging = false;

            // Check if burger was dropped on a customer
            bool servedToCustomer = false;

            for (int ci = 0; ci < (int)customers.size(); ci++) {
                Customer& customer = customers[ci];
                if (customer.isWaitingForFood) {
                    // Calculate distance from burger to customer
                    float dx = burgerDragX - customer.x;
                    float dy = burgerDragY - customer.y;
                    float distanceSquared = dx * dx + dy * dy;

                    // If burger is close enough to customer (within 150 pixels)
                    if (distanceSquared <= 150.0f * 150.0f) {
                        printf("Burger dropped on customer!\n");
                        servedToCustomer = true;

                        // Check if burger matches order
                        bool isCorrect = Burger_MatchesOrder(currentBurger, customer.order);

                        if (isCorrect) {
                            // SUCCESS!
                            printf("Customer is happy! Order correct!\n");

                            // Make customer leave with burger
                            Customer_Satisfy(ci);

                            // Clear burger for next order
                            Burger_Clear();

                            // Add points (you can integrate with Points system later)
                            printf("+35 points!\n");
                            Points_AddPoints(35.0f);

                        }
                        else {
                            // FAILURE - Wrong order
                            printf("Customer is unhappy! Wrong order.\n");

                            // Decrease patience
                            customer.patience -= 30.0f;
                            if (customer.patience < 0) customer.patience = 0;

                            printf("Customer patience: %.0f/%.0f\n",
                                customer.patience, customer.maxPatience);

                            // Burger returns to plate (we'll just clear and let player remake)
                            Burger_Clear();
                        }
                        break; // Exit loop since we found a customer
                    }
                }
            }

            // If burger wasn't served to any customer, it returns to plate
            if (!servedToCustomer) {
                printf("Burger returned to plate.\n");
                burgerIsBeingDragged = false;
                // Burger stays on plate (no need to clear)
            }
            else {
                burgerIsBeingDragged = false;
            }
        }
    }

    if (AEInputCheckTriggered(AEVK_R)) {
        Burger_Clear();
    }

	//REMEMBER TO REMOVE THIS IS JUST CHECKING PURPOSESSSSS
    if (AEInputCheckTriggered(AEVK_E)) {
        next = GS_LVLCMP;
    }

    if (Customer_IsLevelComplete()) {
        // switch to results screen / next state
    }
}

void Game_Draw() {
    // -------------------------------------------------------------------
    // RENDERING SETUP
    // -------------------------------------------------------------------
    AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f); // Gray background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // -------------------------------------------------------------------
    // DRAW BG
    // -------------------------------------------------------------------
    BGMesh = SquareMesh(0.5, 0.5);
    AEMtx33Scale(&BGScale, 1600.0f, 900.0f);
    AEMtx33Trans(&BGTranslate, 0.0f, 0.0f);
    AEMtx33Concat(&BGTransform, &BGTranslate, &BGScale);
    // Apply transformation and draw
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(GameplayBG, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // DRAW CUSTOMERS (draw first so they appear behind counter)
    // -------------------------------------------------------------------
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    Customer_Draw(CustomerTexture);

	AEGfxSetRenderMode(AE_GFX_RM_COLOR); //Reset to color mode for table and plate



    Points_Draw();

    // -------------------------------------------------------------------
   // DRAW INGREDIENTS ON PLATE (STACKED)
   // -------------------------------------------------------------------

    float plateX = 400.0f;
    float plateY = -225.0f;
    float ingredientSize = 150.0f;

    // Determine where to draw burger (plate or dragged position)
    float drawX, drawY;
    if (burgerIsBeingDragged) {
        drawX = burgerDragX;
        drawY = burgerDragY;
    }
    else {
        drawX = 400.0f;    // Plate X
        drawY = -225.0f;   // Plate Y
    }

    /*
    What it does: Creates a variable that keeps track of the burger's current height
    Starts at 0: Bottom bun is at the baseGrows: 
    Each ingredient adds to this height*/

    float currentHeight = 0.0f;  // Track how tall our stack is

    // Bottom Bun (always at the bottom)
    if (bunBottomOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3.f / 4.f, 1.f / 2.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY - 10.0f + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 15.0f;  // Bottom bun adds 15px height
    }

    // Lettuce (thin, compresses easily)
    if (lettuceOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 2.f / 4.f, 0.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 8.0f;   // Lettuce adds only 8px (thin and compresses)
    }

    // Cheese (melts, compresses)
    if (cheeseOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0.f, 1.f / 2.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 10.0f;  // Cheese adds 10px (melts a bit)
    }

    // Patty (thick, doesn't compress much)
    if (pattieOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0.f, 0.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 25.0f;  // Patty adds 25px (thick meat)
    }

    // Tomato (slices, medium thickness)
    if (tomatoOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 1.f / 4.f, 0.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 18.0f;  // Tomato adds 18px
    }

    // Cucumber (thin slices)
    if (cucumberOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 2.f / 4.f, 1.f / 2.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 8.0f;   // Cucumber adds 8px (thin)
    }

    // Onion (thin rings)
    if (onionOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 1.f / 4.f, 1.f / 2.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
        currentHeight += 8.0f;   // Onion adds 8px (thin)
    }

    // Top Bun (sits on top, compresses everything slightly)
    if (bunTopOnPlate) {
        // Top bun compresses the burger slightly
        currentHeight -= 5.0f;   // Compression!

        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3.f / 4.f, 0.f);
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + currentHeight + 15.0f);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }


}

void Burger_Clear() {
    currentBurger.clear();
    burgerComplete = false;

    // Clear all plate flags
    lettuceOnPlate = false;
    cheeseOnPlate = false;
    pattieOnPlate = false;
    tomatoOnPlate = false;
    onionOnPlate = false;     
    cucumberOnPlate = false;  
    bunTopOnPlate = false;
    bunBottomOnPlate = false;

    // Reset drag state
    ResetDragState();

    printf("Burger cleared. Ready to build new one.\n");
}

void Game_Free() {
    printf("Game_Free called\n");
	Customer_Free();
    // Free any dynamic memory here
    Points_Free();

    // Free texture
    if (Ingredients) {
        AEGfxTextureUnload(Ingredients);
        Ingredients = nullptr;
        printf("Ingredients texture freed\n");
    }

    // Free customer texture
    for (int i = 0; i < 4; i++) {
        if (CustomerTexture[i]) {
            AEGfxTextureUnload(CustomerTexture[i]);
            CustomerTexture[i] = nullptr;
        }
    }
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
    for (int i = 0; i < 4; i++) {
        if (CustomerTexture[i]) {
            AEGfxTextureUnload(CustomerTexture[i]);
            CustomerTexture[i] = nullptr;
        }
    }

    Points_Unload();
}

