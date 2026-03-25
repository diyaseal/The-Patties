// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"
extern std::vector<Customer> customers;
extern const float EXIT_X = 800.0f;

// ---------------------------------------------------------------------------
// Global variables for Game State:Diya
static AEGfxVertexList* SquareMesh = nullptr;  // Square mesh
static AEGfxVertexList* PlateMesh = nullptr;  // circle mesh
static AEGfxVertexList* BGMesh = nullptr;  // BG mesh
static AEGfxVertexList* IngredientsMesh = nullptr;  // Ingredient mesh (T)
static AEGfxVertexList* SodaMesh = nullptr; // (D)

AEGfxTexture* Ingredients = nullptr; 
AEGfxTexture* GameplayBG_back = nullptr; // game BG
AEGfxTexture* GameplayBG_front = nullptr; // game BG
AEGfxTexture* CustomerTexture[4] = { nullptr }; // Customer sprite sheet texture (T)
AEGfxTexture* PlateTexture = nullptr; //plate 
AEGfxTexture* SodaTexture = nullptr; // (D) soda sprite sheet so Customer.cpp can show it in bubble
AEGfxTexture* LockTexture = nullptr; // (D) lock icon texture
static bool levelEndTriggered = false; // To ensure we only trigger level end sequence once (T)


static AEMtx33 TableTransform, TableScale, TableTranslate, PlateTransform, PlateTranslate, PlateScale,
PattieTransform, PattieScale, PattieTranslate,
CheeseTransform, CheeseScale, CheeseTranslate,
TomatoTransform, TomatoScale, TomatoTranslate,
LettuceTransform, LettuceScale, LettuceTranslate,
TBurgerTransform, TBurgerScale, TBurgerTranslate,
BBurgerTransform, BBurgerScale, BBurgerTranslate,  // Transformation matrices
BGTransform, BGScale, BGTranslate,
PlateImageTransform, PlateImageScale, PlateImageTranslate;

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
static float BunTopWidth = 120.0f;
static float BunTopHeight = 100.0f;

static float BunBottomX = -560.0f;
static float BunBottomY = -180.0f;
static float BunBottomWidth = 120.0f;
static float BunBottomHeight = 100.0f;

static float PattieX = -275.0f; //updated hitbox
static float PattieY = -110.0f;
static float PattieWidth = 83.0f;
static float PattieHeight = 83.0f;

static float CheeseX = -150.0f; //updated hitbox
static float CheeseY = -110.0f;
static float CheeseWidth = 83.0f;
static float CheeseHeight = 83.0f;

static float LettuceX = -150.0f; //updated hitbox
static float LettuceY = -170.0f;
static float LettuceWidth = 90.0f;
static float LettuceHeight = 83.0f;

static float TomatoX = -300.0f;   //updated hitbox  
static float TomatoY = -175.0f;
static float TomatoWidth = 83.0f;
static float TomatoHeight = 83.0f;

// Add these with your other ingredient position variables
static float OnionX = 3.0f;          // From click (3.00, -175.00)
static float OnionY = -175.0f;
static float OnionWidth = 83.0f;
static float OnionHeight = 83.0f;

static float CucumberX = -18.0f;      // From click (-18.00, -108.00)
static float CucumberY = -108.0f;
static float CucumberWidth = 83.0f;
static float CucumberHeight = 83.0f;

// (D) Soda machine button hitbox
static float SodaX = 574.0f;
static float SodaY = -88.0f;
static float SodaWidth = 70.0f;
static float SodaHeight = 70.0f;

// Trash bin hitbox for deleting burgers
static float TrashX = 629.0f;  // Average of your clicks: (574+683+580+706)/4 ≈ 635.75, adjusted to 629
static float TrashY = -374.0f;  // Average of your clicks: (-307-315-440-435)/4 ≈ -374.25
static float TrashWidth = 100.0f;  // Width of trash hitbox - adjust as needed
static float TrashHeight = 100.0f; // Height of trash hitbox - adjust as needed

// Track what's on the plate: Set 
// everything to false first :Diya


static bool lettuceOnPlate, cheeseOnPlate, pattieOnPlate, tomatoOnPlate, onionOnPlate
, cucumberOnPlate, bunTopOnPlate, bunBottomOnPlate, sodaOnPlate, sodaMachineHasCup, sodaIsFilled
, sodaIsBeingDragged = false;

static float sodaFillTimer = 0.0f; // (D) timer for soda fill
static float sodaDragX = 0.0f; // (D) soda drag position
static float sodaDragY = 0.0f; // (D) soda drag position



// ---------------------------------------------------------------------------
// Helper Functions Syakil 

// Helper function to check if burger is complete (has both buns)
static bool IsBurgerComplete() {
    return bunBottomOnPlate && bunTopOnPlate;
}

// Helper to reset drag state Syakil
static void ResetDragState() {
    isDragging = false;
    burgerIsBeingDragged = false;
    burgerDragX = 400.0f;  // Plate position
    burgerDragY = -225.0f; // Plate position
    sodaIsBeingDragged = false; // (D)
    sodaDragX = SodaX; // (D)
    sodaDragY = SodaY - 55.0f; // (D)
}


// (D) Helper to check if an ingredient is unlocked for the current level
static bool IsIngredientUnlocked(IngredientType ingredient) {
    for (const std::string& ing : currentLevelData.ingredients) {
        switch (ingredient) {
        case BOTTOM_BUN:
            if ( ing == "bottom_bun") return true;
            break;
        case TOP_BUN:
            if ( ing == "top_bun") return true;
            break;
        case PATTY:
            if ( ing == "patty") return true;
            break;
        case CHEESE:
            if ( ing == "cheese") return true;
            break;
        case LETTUCE:
            if (ing == "lettuce") return true;
            break;
        case TOMATO:
            if (ing == "tomato") return true;
            break;
        case ONION:
            if (ing == "onion") return true;
            break;
        case CUCUMBER:
            if ( ing == "cucumber") return true;
            break;
        case SODA:
            if ( ing == "soda") return true;
            break;
        }
    }
    return false;
}

// (D) Helper to draw the lock icon over locked ingredients or soda machine button
static void DrawLockOverlay(float x, float y, float width, float height) {
    if (!LockTexture || !SquareMesh) return;

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxTextureSet(LockTexture, 0.f, 0.f);

    AEMtx33 lockScale, lockTranslate, lockTransform;
    AEMtx33Scale(&lockScale, width, height);
    AEMtx33Trans(&lockTranslate, x, y);
    AEMtx33Concat(&lockTransform, &lockTranslate, &lockScale);

    AEGfxSetTransform(lockTransform.m);
    AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
}

// (D) Treat soda as a side item: burger order must match in sequence, soda only needs matching presence
static bool OrderContainsSoda(const std::vector<int>& order) {
    for (int ingredient : order) {
        if (ingredient == SODA) return true;
    }
    return false;
}

//------------------------- Pause menu (tebs) ------------------------------
static s8   GameFontId = -1;
static s8   GameFontIdSmall = -1;
static bool isPaused = false;

static float PauseMenuBtnX = 0.0f;
static float PauseMenuBtnY = -30.0f;
static float PauseMenuBtnWidth = 260.0f;
static float PauseMenuBtnHeight = 55.0f;

static float PauseSettingsBtnX = 0.0f;
static float PauseSettingsBtnY = -100.0f;
static float PauseSettingsBtnWidth = 260.0f;
static float PauseSettingsBtnHeight = 55.0f;

static float PauseResumeBtnX = 0.0f;
static float PauseResumeBtnY = 60.0f;
static float PauseResumeBtnWidth = 300.0f;
static float PauseResumeBtnHeight = 55.0f;

static float PauseRestartBtnX = 0.0f;
static float PauseRestartBtnY = 20.0f;   
static float PauseRestartBtnWidth = 260.0f;
static float PauseRestartBtnHeight = 55.0f;
//---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Game State Functions

void Game_Init() {
    printf("Game_Init called\n");
    Level_Manager_Init();
    Customer_Init();
    Points_Init();
    GarageDoor_Init();
    levelEndTriggered = false; // Reset level end trigger in case of replay
}

void Game_Load() {
    printf("Game_Load called\n");

    // Load meshes ONCE
    SquareMesh = CreateSquareMesh(8.0f, 2.5f);
    SodaMesh = CreateSodaMesh(0.5f, 0.5f); // (D)
    PlateMesh = CreateCircleMesh(2.0f, 32);
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);
    BGMesh = CreateSquareMesh(0.5, 0.5);
    
    printf("Rectangle mesh created\n");
    // Elizabeth: Check if points are loaded
    Points_Load();
    // Teb : Check if garage door is loaded
    GarageDoor_Load();
    Sound_Load();
    Sound_PlayBGM();
    Settings_Load();

    GameFontId = AEGfxCreateFont("Assets/Font/Fredoka-Bold.ttf", 30);
    GameFontIdSmall = AEGfxCreateFont("Assets/Font/Nunito-Bold.ttf", 24);
}

bool Burger_MatchesOrder(const std::vector<int>& playerBurger,
    const std::vector<int>& customerOrder) {

    // (D) Soda is treated as a side item, so compare soda presence separately
    bool playerHasSoda = OrderContainsSoda(playerBurger);
    bool customerWantsSoda = OrderContainsSoda(customerOrder);

    if (playerHasSoda != customerWantsSoda) {
        printf("FAIL: Soda requirement mismatch");
            return false;
    }

    std::vector<int> filteredPlayer;
    std::vector<int> filteredCustomer;

    for (int ingredient : playerBurger) {
        if (ingredient != SODA) filteredPlayer.push_back(ingredient);
    }
    for (int ingredient : customerOrder) {
        if (ingredient != SODA) filteredCustomer.push_back(ingredient);
    }

    if (filteredPlayer.size() != filteredCustomer.size()) {
        printf("FAIL: Different number of burger ingredients (%zu vs %zu)",
            filteredPlayer.size(), filteredCustomer.size());
        return false;
    }

    for (size_t i = 0; i < filteredCustomer.size(); i++) {
        if (filteredPlayer[i] != filteredCustomer[i]) {
            printf("FAIL: Wrong ingredient at position %zu", i);
                printf("Player: %d, Customer wanted: %d", filteredPlayer[i], filteredCustomer[i]);
                    return false;
        }
    }

    printf("SUCCESS: Order matches perfectly!");
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
    // Settings popup steals all input while open
    Settings_Update((float)dt, GameFontId);
    if (Settings_IsOpen()) return;

    // ESC toggles pause
    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        isPaused = !isPaused;
        printf(isPaused ? "Game PAUSED\n" : "Game RESUMED\n");
    }

    // Pause menu input — block all gameplay while paused
    if (isPaused) {
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);
        float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
        float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
        float worldMouseX = (float)mouseX - windowWidth / 2.0f;
        float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            // Back to Main Menu
            if (worldMouseX >= PauseMenuBtnX - PauseMenuBtnWidth / 2.0f &&
                worldMouseX <= PauseMenuBtnX + PauseMenuBtnWidth / 2.0f &&
                worldMouseY >= PauseMenuBtnY - PauseMenuBtnHeight / 2.0f &&
                worldMouseY <= PauseMenuBtnY + PauseMenuBtnHeight / 2.0f) {
                isPaused = false;
                next = GS_MAINMENU;
            }
            // Settings button
            if (worldMouseX >= PauseSettingsBtnX - PauseSettingsBtnWidth / 2.0f &&
                worldMouseX <= PauseSettingsBtnX + PauseSettingsBtnWidth / 2.0f &&
                worldMouseY >= PauseSettingsBtnY - PauseSettingsBtnHeight / 2.0f &&
                worldMouseY <= PauseSettingsBtnY + PauseSettingsBtnHeight / 2.0f) {
                Settings_Open();
            }
            // Resume button
            if (worldMouseX >= PauseResumeBtnX - PauseResumeBtnWidth / 2.0f &&
                worldMouseX <= PauseResumeBtnX + PauseResumeBtnWidth / 2.0f &&
                worldMouseY >= PauseResumeBtnY - PauseResumeBtnHeight / 2.0f &&
                worldMouseY <= PauseResumeBtnY + PauseResumeBtnHeight / 2.0f) {
                isPaused = false;
            }

            // Restart button
            if (worldMouseX >= PauseRestartBtnX - PauseRestartBtnWidth / 2.0f &&
                worldMouseX <= PauseRestartBtnX + PauseRestartBtnWidth / 2.0f &&
                worldMouseY >= PauseRestartBtnY - PauseRestartBtnHeight / 2.0f &&
                worldMouseY <= PauseRestartBtnY + PauseRestartBtnHeight / 2.0f) {
                isPaused = false;
                Burger_Clear();
                printf("Restarting level %d\n", currentLevel);
                next = GS_LOADING; // or however your engine restarts the current level
            }
        }
        return; // block all gameplay
    }

    Customer_Update(static_cast<float>(dt));
    Points_Update();

    // (D) Fill soda over time once the machine starts pouring
    if (sodaMachineHasCup && !sodaIsFilled) {
        sodaFillTimer += static_cast<float>(dt);
        if (sodaFillTimer >= 3.0f) {
            sodaFillTimer = 3.0f;
            sodaIsFilled = true;
            Sound_StopSFX_Pour();
            printf("Soda finished filling!\n");
        }
    }

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to world coordinates
    float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

    float worldMouseX = (float)mouseX - windowWidth / 2.0f;
    float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

    // ---------------------------------------------------------------------------
    // DIYA was here , and syakil
    // ---------------------------------------------------------------------------

    // Check for mouse click on the ingredients to see what player is adding to the burger
    if (AEInputCheckTriggered(AEVK_LBUTTON)) {
        printf("Mouse clicked at world: (%.2f, %.2f)\n", worldMouseX, worldMouseY);

        // Check if lettuce was clicked
        if (IsIngredientUnlocked(LETTUCE) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= LettuceX - LettuceWidth / 2.0f &&
            worldMouseX <= LettuceX + LettuceWidth / 2.0f &&
            worldMouseY >= LettuceY - LettuceHeight / 2.0f &&
            worldMouseY <= LettuceY + LettuceHeight / 2.0f) {

            lettuceOnPlate = true;
            currentBurger.push_back(LETTUCE); // Track ingredient
            printf("Lettuce added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if cheese was clicked
        if (IsIngredientUnlocked(CHEESE) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= CheeseX - CheeseWidth / 2.0f &&
            worldMouseX <= CheeseX + CheeseWidth / 2.0f &&
            worldMouseY >= CheeseY - CheeseHeight / 2.0f &&
            worldMouseY <= CheeseY + CheeseHeight / 2.0f) {

            cheeseOnPlate = true;
            currentBurger.push_back(CHEESE);  // Track ingredient
            printf("Cheese added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if pattie was clicked
        if (IsIngredientUnlocked(PATTY) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= PattieX - PattieWidth / 2.0f &&
            worldMouseX <= PattieX + PattieWidth / 2.0f &&
            worldMouseY >= PattieY - PattieHeight / 2.0f &&
            worldMouseY <= PattieY + PattieHeight / 2.0f) {

            pattieOnPlate = true;
            currentBurger.push_back(PATTY);  // Track ingredient
            printf("Patty added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if tomato was clicked
        if (IsIngredientUnlocked(TOMATO) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= TomatoX - TomatoWidth / 2.0f &&
            worldMouseX <= TomatoX + TomatoWidth / 2.0f &&
            worldMouseY >= TomatoY - TomatoHeight / 2.0f &&
            worldMouseY <= TomatoY + TomatoHeight / 2.0f) {

            tomatoOnPlate = true;
            currentBurger.push_back(TOMATO);  // NEW: Track ingredient
            printf("Tomato added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if bun top was clicked
        if (IsIngredientUnlocked(TOP_BUN) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= BunTopX - BunTopWidth / 2.0f &&
            worldMouseX <= BunTopX + BunTopWidth / 2.0f &&
            worldMouseY >= BunTopY - BunTopHeight / 2.0f &&
            worldMouseY <= BunTopY + BunTopHeight / 2.0f) {

            bunTopOnPlate = true;
            currentBurger.push_back(TOP_BUN);  // Track ingredient
            burgerComplete = true;  // Burger is complete!
            printf("Top bun added. Burger is complete and ready to serve!\n");
        }

        // Check if bun bottom was clicked
        if (IsIngredientUnlocked(BOTTOM_BUN) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= BunBottomX - BunBottomWidth / 2.0f &&
            worldMouseX <= BunBottomX + BunBottomWidth / 2.0f &&
            worldMouseY >= BunBottomY - BunBottomHeight / 2.0f &&
            worldMouseY <= BunBottomY + BunBottomHeight / 2.0f) {

            bunBottomOnPlate = true;
            currentBurger.push_back(BOTTOM_BUN);  // Track ingredient
            printf("Bottom bun added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if onion was clicked   UNLOCK INGREDIENTS 
        if (IsIngredientUnlocked(ONION) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= OnionX - OnionWidth / 2.0f &&
            worldMouseX <= OnionX + OnionWidth / 2.0f &&
            worldMouseY >= OnionY - OnionHeight / 2.0f &&
            worldMouseY <= OnionY + OnionHeight / 2.0f) {

            onionOnPlate = true;
            currentBurger.push_back(ONION);  // Track ingredient
            printf("Onion added to burger. Total ingredients: %zu\n", currentBurger.size());
        }

        // Check if cucumber was clicked UUNLOCK INGREDIENTS 
        if (IsIngredientUnlocked(CUCUMBER) && // (D) Locked ingredients cannot be clicked
            worldMouseX >= CucumberX - CucumberWidth / 2.0f &&
            worldMouseX <= CucumberX + CucumberWidth / 2.0f &&
            worldMouseY >= CucumberY - CucumberHeight / 2.0f &&
            worldMouseY <= CucumberY + CucumberHeight / 2.0f) {

            cucumberOnPlate = true;
            currentBurger.push_back(CUCUMBER);  // Track ingredient
            printf("Cucumber added to burger. Total ingredients: %zu\n", currentBurger.size());
        }


        // (D) Start filling soda at the machine if soda is unlocked and no soda already exists
        if (IsIngredientUnlocked(SODA) &&
            !sodaMachineHasCup && !sodaOnPlate && !sodaIsBeingDragged &&
            worldMouseX >= SodaX - SodaWidth / 2.0f &&
            worldMouseX <= SodaX + SodaWidth / 2.0f &&
            worldMouseY >= SodaY - SodaHeight / 2.0f &&
            worldMouseY <= SodaY + SodaHeight / 2.0f) {

            sodaMachineHasCup = true;
            sodaIsFilled = false;
            sodaFillTimer = 0.0f;
            sodaDragX = SodaX;
            sodaDragY = SodaY - 55.0f;
            Sound_PlaySFX_Pour();
            printf("Started filling soda...\n");
        }

        // (D) Only a filled soda can be dragged from the machine
        if (sodaMachineHasCup && sodaIsFilled &&
            worldMouseX >= sodaDragX - 35.0f &&
            worldMouseX <= sodaDragX + 35.0f &&
            worldMouseY >= sodaDragY - 45.0f &&
            worldMouseY <= sodaDragY + 45.0f) {

            isDragging = true;
            sodaIsBeingDragged = true;
            burgerIsBeingDragged = false;
            sodaDragX = worldMouseX;
            sodaDragY = worldMouseY;
            printf("Started dragging filled soda!\n");
        }

        // Check if player clicks on the plate (burger) to start dragging
        float plateWidth = 300.0f;    // ADJUST THIS to match your plate.png size
        float plateHeight = 300.0f;   // ADJUST THIS to match your plate.png size
        float plateCenterX = 290.0f;  // Where you draw the plate
        float plateCenterY = -210.0f; // Where you draw the plate

        // Calculate plate boundaries
        float plateLeft = plateCenterX - plateWidth / 2;
        float plateRight = plateCenterX + plateWidth / 2;
        float plateBottom = plateCenterY - plateHeight / 2;
        float plateTop = plateCenterY + plateHeight / 2;

        // Check if burger exists and mouse is over the SQUARE plate
        if (!currentBurger.empty() &&
            worldMouseX >= plateLeft && worldMouseX <= plateRight &&
            worldMouseY >= plateBottom && worldMouseY <= plateTop) {

            isDragging = true;
            burgerIsBeingDragged = true;

            // Calculate offset from burger center to mouse position
            dragOffsetX = worldMouseX - plateCenterX;
            dragOffsetY = worldMouseY - plateCenterY;

            // Start position for dragging
            burgerDragX = worldMouseX;
            burgerDragY = worldMouseY;

            printf("Started dragging burger! (Square collision)\n");
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

    if (isDragging && sodaIsBeingDragged) {
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);

        float windowWidth = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
        float windowHeight = AEGfxGetWinMaxY() - AEGfxGetWinMinY();

        float worldMouseX = (float)mouseX - windowWidth / 2.0f;
        float worldMouseY = -(float)mouseY + windowHeight / 2.0f;

        sodaDragX = worldMouseX;
        sodaDragY = worldMouseY;
    }

    // Check for mouse release
    if (AEInputCheckReleased(AEVK_LBUTTON)) {
        // (D) Release filled soda onto the plate
        if (isDragging && sodaIsBeingDragged) {
            isDragging = false;

            float plateWidth = 300.0f;
            float plateHeight = 150.0f;
            float plateCenterX = 275.0f;
            float plateCenterY = -125.0f;

            bool droppedOnPlate =
                sodaDragX >= plateCenterX - plateWidth / 2.0f &&
                sodaDragX <= plateCenterX + plateWidth / 2.0f &&
                sodaDragY >= plateCenterY - plateHeight / 2.0f &&
                sodaDragY <= plateCenterY + plateHeight / 2.0f;

            if (droppedOnPlate) {
                sodaOnPlate = true;
                sodaMachineHasCup = false;
                sodaIsFilled = false;
                sodaFillTimer = 0.0f;
                sodaIsBeingDragged = false;
                currentBurger.push_back(SODA);
                printf("Soda placed on plate!\n");
            }
            else {
                sodaIsBeingDragged = false;
                sodaDragX = SodaX;
                sodaDragY = SodaY - 55.0f;
                printf("Soda returned to machine.\n");
            }
        }
        else if (isDragging && burgerIsBeingDragged) {
            // Mouse released while dragging burger
            isDragging = false;

            // Check if burger was dropped on the trash bin
            bool droppedInTrash = false;

            // Check if burger position is within trash bin bounds
            if (burgerDragX >= TrashX - TrashWidth / 2.0f &&
                burgerDragX <= TrashX + TrashWidth / 2.0f &&
                burgerDragY >= TrashY - TrashHeight / 2.0f &&
                burgerDragY <= TrashY + TrashHeight / 2.0f) {

                printf("Burger dropped in trash! Deleting burger...\n");
                droppedInTrash = true;
                Burger_Clear(); // This clears the entire burger
            }

            // If not dropped in trash, check if dropped on customer
            if (!droppedInTrash) {
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

                                // Add points
                                Points_AddPoints(isCorrect);
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
            else {
                // Burger was dropped in trash, so it's already cleared
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

    if (!levelEndTriggered && Customer_IsLevelComplete()) {
        levelEndTriggered = true;
        GarageDoor_Trigger();
    }
    GarageDoor_Update(static_cast<float>(dt));

    // TEMP DEBUG - press G to test garage door instantly
    if (AEInputCheckTriggered(AEVK_G)) {
        levelEndTriggered = true;
        GarageDoor_Trigger();
        printf("DEBUG: Garage door manually triggered!\n");
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
    // DRAW BACK BG
    // -------------------------------------------------------------------
    AEMtx33Scale(&BGScale, 1600.0f, 900.0f);
    AEMtx33Trans(&BGTranslate, 0.0f, 0.0f);
    AEMtx33Concat(&BGTransform, &BGTranslate, &BGScale);
    // Apply transformation and draw
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(GameplayBG_back, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // DRAW CUSTOMERS (draw first so they appear behind counter)
    // -------------------------------------------------------------------
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    Customer_Draw(CustomerTexture);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR); //Reset to color mode for table and plate

    // -------------------------------------------------------------------
    // DRAW FRONT BG
    // -------------------------------------------------------------------
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(GameplayBG_front, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR); //Reset to color mode for table and plate
    // -------------------------------------------------------------------
    // DRAW POINTS BAR
    // -------------------------------------------------------------------
    Points_Draw();

    // (D) Draw lock icon on top of any ingredient still locked for this level
    if (!IsIngredientUnlocked(BOTTOM_BUN)) DrawLockOverlay(BunBottomX, BunBottomY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(TOP_BUN)) DrawLockOverlay(BunTopX, BunTopY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(PATTY)) DrawLockOverlay(PattieX, PattieY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(CHEESE)) DrawLockOverlay(CheeseX, CheeseY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(LETTUCE)) DrawLockOverlay(LettuceX, LettuceY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(TOMATO)) DrawLockOverlay(TomatoX, TomatoY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(ONION)) DrawLockOverlay(OnionX, OnionY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(CUCUMBER)) DrawLockOverlay(CucumberX, CucumberY, 8.0f, 8.0f);
    if (!IsIngredientUnlocked(SODA)) DrawLockOverlay(SodaX - 13.0f, SodaY + 28.0f, 8.0f, 8.0f);

    // (D) Draw soda cup at the machine while it is filling or ready
    if (sodaMachineHasCup && SodaTexture) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(SodaTexture, sodaIsFilled ? 0.5f : 0.0f, 0.0f);

        AEMtx33 sodaScale, sodaTrans, sodaTransform;
        AEMtx33Scale(&sodaScale, 70.0f, 110.0f);
        AEMtx33Trans(&sodaTrans,
            sodaIsBeingDragged ? sodaDragX : SodaX,
            sodaIsBeingDragged ? sodaDragY : SodaY - 55.0f);
        AEMtx33Concat(&sodaTransform, &sodaTrans, &sodaScale);

        AEGfxSetTransform(sodaTransform.m);
        AEGfxMeshDraw(SodaMesh, AE_GFX_MDM_TRIANGLES); // (D)
    }


    // -------------------------------------------------------------------
   // DRAW INGREDIENTS ON PLATE (STACKED)
   // -------------------------------------------------------------------

    float plateX = 300.0f;
    float plateY = -175.0f;
    float ingredientSize = 150.0f;

    // Determine where to draw burger (plate or dragged position)
    float drawX, drawY;
    if (burgerIsBeingDragged) {
        drawX = burgerDragX;
        drawY = burgerDragY;
    }
    else {
        drawX = plateX;    // Plate X
        drawY = plateY;    // Plate Y
    }

    // -------------------------------------------------------------------
    // DRAW PLATE IMAGE (underneath the burger)
    // -------------------------------------------------------------------
    if (PlateTexture) {
        // Set texture rendering mode
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        // Set the plate texture
        AEGfxTextureSet(PlateTexture, 0.f, 0.f);

        // Scale the plate - adjust size as needed
        float plateWidth = 20.0f;   // Adjust based on your image
        float plateHeight = 20.0f;  // Adjust based on your image

        // Create scale matrix
        AEMtx33Scale(&PlateImageScale, plateWidth, plateHeight);

        // Create translation matrix - position at same X,Y as burger
        AEMtx33Trans(&PlateImageTranslate, drawX, drawY - 20.0f); // Slightly lower than burger

        // Combine scale and translation
        AEMtx33Concat(&PlateImageTransform, &PlateImageTranslate, &PlateImageScale);

        // Apply transform
        AEGfxSetTransform(PlateImageTransform.m);

        // Draw the plate (using a square mesh)
        AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES); // Reuse SquareMesh
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
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
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
    }

    // (D) Draw soda beside the burger on the same plate once placed
    if (sodaOnPlate && SodaTexture) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxTextureSet(SodaTexture, 1.f / 2.f, 0.f);
        AEMtx33Scale(&PlateLettuceScale, 70.0f, 110.0f);
        AEMtx33Trans(&PlateLettuceTranslate, drawX + 90.0f, drawY + 25.0f);
        AEMtx33Concat(&PlateLettuceTransform, &PlateLettuceTranslate, &PlateLettuceScale);
        AEGfxSetTransform(PlateLettuceTransform.m);
        AEGfxMeshDraw(SodaMesh, AE_GFX_MDM_TRIANGLES); // (D)
    }

    GarageDoor_Draw(); // Teb: Draw garage door on top of everything else

    // -------------------------------------------------------------------
    // PAUSE MENU OVERLAY
    // -------------------------------------------------------------------
    if (isPaused) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEMtx33 s, t, m;

        // Dark backdrop
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.6f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, 1600.0f, 900.0f);
        AEMtx33Trans(&t, 0.0f, 0.0f);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        // Layout
        float panelCX = 0.0f, panelCY = 0.0f;
        float panelW = 400.0f, panelH = 560.0f, panelR = 30.0f;
        float titleH = 70.0f;
        float titleCY = panelCY + panelH * 0.5f - titleH * 0.5f;
        float btnW = 300.0f, btnH = 55.0f, btnR = 28.0f;
        float btn3Y = panelCY + 110.0f;  // Resume       (top)
        float btn4Y = panelCY + 30.0f;   // Restart
        float btn1Y = panelCY - 55.0f;   // Back to Main Menu
        float btn2Y = panelCY - 140.0f;  // Settings     (bottom)

        // Sync hitboxes
        PauseMenuBtnX = panelCX; PauseMenuBtnY = btn1Y;
        PauseMenuBtnWidth = btnW;    PauseMenuBtnHeight = btnH;
        PauseSettingsBtnX = panelCX; PauseSettingsBtnY = btn2Y;
        PauseSettingsBtnWidth = btnW;    PauseSettingsBtnHeight = btnH;
        PauseResumeBtnX = panelCX; PauseResumeBtnY = btn3Y;
        PauseResumeBtnWidth = btnW;    PauseResumeBtnHeight = btnH;
        PauseRestartBtnX = panelCX; PauseRestartBtnY = btn4Y;
        PauseRestartBtnWidth = btnW; PauseRestartBtnHeight = btnH;

        // ---------------------------------------------------------------
        // LAYER 1: Full rounded cream panel (bottom layer)
        // ---------------------------------------------------------------
        DrawRoundedRect(BGMesh, PlateMesh,
            panelCX, panelCY, panelW, panelH, panelR,
            0.98f, 0.96f, 0.90f);

        // ---------------------------------------------------------------
        // LAYER 2: Orange title bar — drawn as a PLAIN rectangle
        // positioned to sit flush with the top of the panel interior
        // We draw it INSIDE the rounded area so no corners poke out
        // ---------------------------------------------------------------
        float titleBarTop = panelCY + panelH * 0.5f;        // top of panel
        float titleBarBottom = titleBarTop - titleH;            // bottom of title bar
        float titleBarCY = (titleBarTop + titleBarBottom) * 0.5f;

        // Horizontal fill (full width, sits inside the rounded panel)
        AEGfxSetColorToMultiply(0.85f, 0.31f, 0.055f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, panelW - panelR * 2.0f, titleH);
        AEMtx33Trans(&t, panelCX, titleBarCY);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        // Side fills (narrow strips on left/right to reach panel edge)
        // but stop short of the very top rounded corners
        AEMtx33Scale(&s, panelR * 2.0f, titleH - panelR);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        // Top-left and top-right orange corner circles
        // (these sit inside the panel's own rounded corners — same radius)
        AEMtx33Scale(&s, panelR / 2.0f, panelR / 2.0f);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR,
            panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR,
            panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

        // ---------------------------------------------------------------
        // LAYER 3: Pill buttons
        // ---------------------------------------------------------------
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, btn3Y, btnW, btnH, btnR,
            0.85f, 0.31f, 0.055f);  // orange Resume
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, btn1Y, btnW, btnH, btnR,
            0.17f, 0.17f, 0.17f);   // dark Back to Main Menu
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, btn2Y, btnW, btnH, btnR,
            0.17f, 0.17f, 0.17f);   // dark Settings
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, btn4Y, btnW, btnH, btnR,
            0.17f, 0.17f, 0.17f);   // dark Restart

        // ---------------------------------------------------------------
        // LAYER 4: Text
        // ---------------------------------------------------------------
        if (GameFontId >= 0) {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);

            char pauseTitle[] = "PAUSED";
            char hintText[] = "Press ESC to Resume";
            char menuLabel[] = "Back to Main Menu";
            char settingsLabel[] = "Settings";
            char resumeLabel[] = "Resume";
            char restartLabel[] = "Restart";

            AEGfxPrint(GameFontId, pauseTitle,
                -0.13f, (titleBarCY - 14.0f) / 450.0f,
                1.8f, 1.0f, 1.0f, 1.0f, 1.0f);

            s8 hintFont = (GameFontIdSmall >= 0) ? GameFontIdSmall : GameFontId;
            AEGfxPrint(hintFont, hintText,
                -0.10f, (titleBarBottom - 30.0f) / 450.0f,
                0.75f, 0.5f, 0.5f, 0.5f, 1.0f);

            AEGfxPrint(GameFontId, resumeLabel,
                -0.07f, (btn3Y - 10.0f) / 450.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            AEGfxPrint(GameFontId, menuLabel,
                -0.15f, (btn1Y - 10.0f) / 450.0f,
                0.85f, 1.0f, 1.0f, 1.0f, 1.0f);

            AEGfxPrint(GameFontId, settingsLabel,
                -0.08f, (btn2Y - 10.0f) / 450.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

            AEGfxPrint(GameFontId, restartLabel,
                -0.07f, (btn4Y - 10.0f) / 450.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    Settings_Draw(GameFontId, BGMesh, PlateMesh);
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

    // Only clear soda if it was part of the served order (was on plate)
    // If it's still filling or ready at machine, leave it alone
    if (sodaOnPlate) {
        sodaOnPlate = false;
        sodaMachineHasCup = false;
        sodaIsFilled = false;
        sodaFillTimer = 0.0f;
        printf("Soda was served, clearing it.\n");
    }

    // Reset drag state
    ResetDragState();

    printf("Burger cleared. Ready to build new one.\n");
}

void Game_Free() {
    printf("Game_Free called\n");
    Burger_Clear();
    Customer_Free();
    // Free any dynamic memory here
    Points_Free();
    Level_Manager_Free();
    GarageDoor_Free();
    Sound_Free();

}

void Game_Unload() {
    printf("Game_Unload called\n");

    // Free texture
    if (Ingredients) {
        AEGfxTextureUnload(Ingredients);
        Ingredients = nullptr;
        printf("Ingredients texture freed\n");
    }

    if (SodaTexture) {
        AEGfxTextureUnload(SodaTexture);
        SodaTexture = nullptr;
        printf("Soda texture freed\n");
    }

    if (LockTexture) {
        AEGfxTextureUnload(LockTexture);
        LockTexture = nullptr;
        printf("Lock texture freed\n");
    }

    // Free customer texture
    for (int i = 0; i < 4; i++) {
        if (CustomerTexture[i]) {
            AEGfxTextureUnload(CustomerTexture[i]);
            CustomerTexture[i] = nullptr;
        }
    }

    if (PlateTexture) {
        AEGfxTextureUnload(PlateTexture);
        PlateTexture = nullptr;
    }
    if (GameplayBG_back) {
        AEGfxTextureUnload(GameplayBG_back);
        GameplayBG_back = nullptr;
    }
    if (GameplayBG_front) {
        AEGfxTextureUnload(GameplayBG_front);
        GameplayBG_front = nullptr;
    }


    // Clean up mesh
    if (SquareMesh) {
        AEGfxMeshFree(SquareMesh);
        SquareMesh = nullptr;
        printf("Table mesh freed\n");
    }
    if (PlateMesh) {
        AEGfxMeshFree(PlateMesh);
        PlateMesh = nullptr;
        printf("Plate mesh freed\n");
    }
    if (IngredientsMesh) {
        AEGfxMeshFree(IngredientsMesh);
        IngredientsMesh = nullptr;
        printf("Ingredient mesh freed\n");
    }
    if (BGMesh) {
        AEGfxMeshFree(BGMesh);
        BGMesh = nullptr;
        printf("Ingredient mesh freed\n");
    }
    if (SodaMesh) {
        AEGfxMeshFree(SodaMesh);
        SodaMesh = nullptr;
    } // (D)

    if (GameFontId >= 0) {
        AEGfxDestroyFont(GameFontId);
        GameFontId = -1;
    }
    if (GameFontIdSmall >= 0) {
        AEGfxDestroyFont(GameFontIdSmall);
        GameFontIdSmall = -1;
    }

    GarageDoor_Unload();
    Points_Unload();
    Level_Manager_Unload();
    Sound_Unload();
    Settings_Unload();
}

