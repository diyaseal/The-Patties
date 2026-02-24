// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"
extern std::vector<Customer> customers;
extern const float EXIT_X = 800.0f;

// ---------------------------------------------------------------------------
// Global variables for Game State
static AEGfxVertexList* TableMesh = nullptr;  // Square mesh
static AEGfxVertexList* PlateMesh = nullptr;  // circle mesh
static AEGfxVertexList* BGMesh = nullptr;  // BG mesh
static AEGfxTexture* Ingredients = nullptr; // Ingredients
static AEGfxTexture* GameplayBG = nullptr; // game BG
static AEGfxTexture* CustomerTexture = nullptr; // Customer sprite sheet texture

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
// Drag-and-drop system
static bool isDragging = false;
static float dragOffsetX = 0.0f;
static float dragOffsetY = 0.0f;
static float burgerDragX = 0.0f;
static float burgerDragY = 0.0f;
static bool burgerIsBeingDragged = false;

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
// Helper Functions

// Helper function to check if burger is complete (has both buns)
static bool IsBurgerComplete() {
    return bunBottomOnPlate && bunTopOnPlate;
}

// Helper to reset drag state
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

    // Load customer sprite sheet
    CustomerTexture = AEGfxTextureLoad("Assets/Assets/testChar.png");

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
    // Example: Move the square, check collisions, etc.
    float dt = AEFrameRateControllerGetFrameTime();
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

            for (Customer& customer : customers) {
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
                            customer.isWalking = true;
                            customer.isWaitingForFood = false;
                            customer.targetX = EXIT_X;
                            customer.targetY = customer.y;
                            customer.flipHorizontal = true;

                            // Clear burger for next order
                            Burger_Clear();

                            // Add points (you can integrate with Points system later)
                            printf("+200 points!\n");
                            Points_AddPoints(200.0f);

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

    // -------------------------------------------------------------------
    // DRAW SQUARE
    // -------------------------------------------------------------------
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(8.0f, 2.5f);
    //// Set color (maroon)
    //AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f); // Maroon color
    //// Create transformation: scale then translate
    //AEMtx33Scale(&TableScale, 100.0f, 100.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, 0.0f, -225.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh Burger Bun Top
    //TableMesh = SquareMesh(3.0f, 3.15f);
    //AEGfxSetColorToMultiply(0.96f, 0.96f, 0.86f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -600.0f, -129.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&TBurgerScale, 200, 200);
    //AEMtx33Trans(&TBurgerTranslate, -600.0f, -129.0f);          // Center of screen
    //AEMtx33Concat(&TBurgerTransform, &TBurgerTranslate, &TBurgerScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 3.f/4.f, 0.f );
    //AEGfxSetTransform(TBurgerTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh Burger Bun Bottom
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(3.0f, 3.1f);
    //AEGfxSetColorToMultiply(0.96f, 0.96f, 0.86f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -600.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&BBurgerScale, 200, 200);           // 100x100 pixel square
    //AEMtx33Trans(&BBurgerTranslate, -600.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&BBurgerTransform, &BBurgerTranslate, &BBurgerScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 3.f/4.f, 1.f/2.f);
    //AEGfxSetTransform(BBurgerTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh square 1 Pattie
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(3.0f, 3.0f);
    //AEGfxSetColorToMultiply(0.6f, 0.3f, 0.1f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -350.0f, -129.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&PattieScale, 200.f, 200.f); 
    //AEMtx33Trans(&PattieTranslate, -350.0f, -129.0f);
    //AEMtx33Concat(&PattieTransform, &PattieTranslate, &PattieScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 0.f, 0.f);
    //AEGfxSetTransform(PattieTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5 ,0.5), AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh square 2 Cheese
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(3.0f, 3.0f);
    //AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -100.0f, -129.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&CheeseScale, 200, 200);           // 100x100 pixel square
    //AEMtx33Trans(&CheeseTranslate, -100.0f, -129.0f);          // Center of screen
    //AEMtx33Concat(&CheeseTransform, &CheeseTranslate, &CheeseScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 0.f, 1.f/2.f);
    //AEGfxSetTransform(CheeseTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh square 3 Lettuce 
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(3.0f, 3.0f);
    //AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -100.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&LettuceScale, 200, 200);           // 100x100 pixel square
    //AEMtx33Trans(&LettuceTranslate, -100.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&LettuceTransform, &LettuceTranslate, &LettuceScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 2.f/4.f, 0.f);
    //AEGfxSetTransform(LettuceTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    ////Food tray mesh square 4 tomato
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //TableMesh = SquareMesh(3.0f, 3.0f);
    //AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);
    //AEMtx33Scale(&TableScale, 30.0f, 30.0f);           // 100x100 pixel square
    //AEMtx33Trans(&TableTranslate, -350.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&TableTransform, &TableTranslate, &TableScale);
    //AEMtx33Scale(&TomatoScale, 200, 200);           // 100x100 pixel square
    //AEMtx33Trans(&TomatoTranslate, -350.0f, -325.0f);          // Center of screen
    //AEMtx33Concat(&TomatoTransform, &TomatoTranslate, &TomatoScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(TableTransform.m);
    //AEGfxMeshDraw(TableMesh, AE_GFX_MDM_TRIANGLES);
    //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    //AEGfxTextureSet(Ingredients, 1.f/4.f, 0.f);
    //AEGfxSetTransform(TomatoTransform.m);
    //AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);

    ////Plate mesh
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //PlateMesh = CircleMesh(2.0f, 32);
    //AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    //// Create transformation: scale then translate
    //AEMtx33Scale(&PlateScale, 100.0f, 50.0f);           // 100x100 pixel square
    //AEMtx33Trans(&PlateTranslate, 400.0f, -225.0f);          // Center of screen
    //AEMtx33Concat(&PlateTransform, &PlateTranslate, &PlateScale);
    //// Apply transformation and draw
    //AEGfxSetTransform(PlateTransform.m);
    //AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

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

    // Draw ingredients on plate if clicked
    if (bunBottomOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3.f/4.f, 1.f/ 2.f);  // Bun bottom sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY - 20.0f);  // Below lettuce
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }
    if (lettuceOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 2.f/ 4.f, 0.f);  // Lettuce sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }
    if (cheeseOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0.f, 1.f / 2.f);  // Cheese sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + 20.0f);  // Slightly above lettuce
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }

    if (pattieOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 0.f, 0.f);  // Pattie sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + 40.0f);  // Above cheese
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }

    if (tomatoOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 1.f / 4.f, 0.f);  // Tomato sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + 60.0f);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(IngredientMesh(0.5, 0.5), AE_GFX_MDM_TRIANGLES);
    }


    if (bunTopOnPlate) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(Ingredients, 3.f / 4.f, 0.f);  // Bun top sprite
        AEMtx33Scale(&PlateLettuceScale, ingredientSize, ingredientSize);
        AEMtx33Trans(&PlateLettuceTranslate, drawX, drawY + 80.0f);  // On top
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
    if (CustomerTexture) {
        AEGfxTextureUnload(CustomerTexture);
        CustomerTexture = nullptr;
        printf("Customer texture freed\n");
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
    if (CustomerTexture) {
        AEGfxTextureUnload(CustomerTexture);
        CustomerTexture = nullptr;
        printf("Customer texture freed\n");
    }

    Points_Unload();
}

