// ---------------------------------------------------------------------------
// tutorial.cpp
// Tutorial game state - teaches the player how to build and serve a burger
// Reuses all rendering and customer systems from game.cpp but adds a guided
// state machine on top. No level CSV data is required.
// ---------------------------------------------------------------------------
#include "pch.hpp"

extern std::vector<Customer> customers;
extern const float EXIT_X;
static s8 FontId = -1;
static bool tutEND = false;

static TutorialPhase tutorialPhase = TUTE_CUSTOMER_WALKING;
static int showOrderStep = 0;

// Hardcoded tutorial order: bottom bun, patty, top bun + soda as side item
static const std::vector<int> TUTORIAL_ORDER = { BOTTOM_BUN, PATTY, TOP_BUN, SODA };
//static bool nextStep = false;

// ---------------------------------------------------------------------------
// Mesh / texture pointers
// ---------------------------------------------------------------------------
static AEGfxVertexList* SquareMesh = nullptr;
static AEGfxVertexList* BGMesh = nullptr;
static AEGfxVertexList* IngredientsMesh = nullptr;
static AEGfxVertexList* SodaMesh = nullptr;

// These are defined as globals in game.cpp and referenced here via extern
// so that Customer.cpp's DrawThoughtBubble can find them during the tutorial.
extern AEGfxTexture* Ingredients;
extern AEGfxTexture* SodaTexture;

static AEGfxTexture* GameplayBG_back = nullptr;
static AEGfxTexture* GameplayBG_front = nullptr;
static AEGfxTexture* CustomerTexture[4] = { nullptr };
static AEGfxTexture* PlateTexture = nullptr;

// ---------------------------------------------------------------------------
// Shared transform matrices
// ---------------------------------------------------------------------------
static AEMtx33 BGTransform, BGScale, BGTranslate;
static AEMtx33 PlateTransform, PlateScale, PlateTranslate;
static AEMtx33 IngTransform, IngScale, IngTranslate;

// ---------------------------------------------------------------------------
// Ingredient hitbox positions (identical to game.cpp)
// ---------------------------------------------------------------------------
static const float BunTopX = -523.0f, BunTopY = -81.0f, BunTopW = 100.0f, BunTopH = 100.0f;
static const float BunBotX = -560.0f, BunBotY = -180.0f, BunBotW = 90.0f, BunBotH = 90.0f;
static const float PattieX = -275.0f, PattieY = -110.0f, PattieW = 90.0f, PattieH = 90.0f;
static const float CheeseX = -150.0f, CheeseY = -110.0f, CheeseW = 90.0f, CheeseH = 90.0f;
static const float LettuceX = -150.0f, LettuceY = -170.0f, LettuceW = 110.0f, LettuceH = 110.0f;
static const float TomatoX = -300.0f, TomatoY = -175.0f, TomatoW = 90.0f, TomatoH = 90.0f;
static const float OnionX = 3.0f, OnionY = -175.0f, OnionW = 90.0f, OnionH = 90.0f;
static const float CucumbX = -18.0f, CucumbY = -108.0f, CucumbW = 90.0f, CucumbH = 90.0f;
static const float SodaX = 570.0f, SodaY = -88.0f, SodaW = 80.0f, SodaH = 80.0f;
static const float TrashX = 629.0f, TrashY = -374.0f, TrashW = 100.0f, TrashH = 100.0f;

// ---------------------------------------------------------------------------
// Burger / plate state
// ---------------------------------------------------------------------------
static std::vector<int> currentBurger;
static bool burgerComplete = false;
static bool lettuceOnPlate = false;
static bool cheeseOnPlate = false;
static bool pattieOnPlate = false;
static bool tomatoOnPlate = false;
static bool onionOnPlate = false;
static bool cucumberOnPlate = false;
static bool bunTopOnPlate = false;
static bool bunBottomOnPlate = false;
static bool sodaOnPlate = false;
static bool sodaMachineHasCup = false;
static bool sodaIsFilled = false;
static float sodaFillTimer = 0.0f;
static bool sodaIsBeingDragged = false;
static float sodaDragX = 0.0f;
static float sodaDragY = 0.0f;

// Drag state
static bool  isDragging = false;
static bool  burgerIsBeingDragged = false;
static float burgerDragX = 300.0f;
static float burgerDragY = -175.0f;
static float dragOffsetX = 0.0f;
static float dragOffsetY = 0.0f;

// ---------------------------------------------------------------------------
// End-screen buttons
// ---------------------------------------------------------------------------
static Button btnMainMenu = { -150.0f, -300.0f, 200.0f, 60.0f, false, false };
static Button btnLevels = { 150.0f, -300.0f, 200.0f, 60.0f, false, false };

// ---------------------------------------------------------------------------
// Helper: AABB point test
// ---------------------------------------------------------------------------
static bool InBounds(float px, float py,
    float cx, float cy,
    float hw, float hh) {
    return px >= cx - hw && px <= cx + hw &&
        py >= cy - hh && py <= cy + hh;
}

// ---------------------------------------------------------------------------
// Helper: Does the current burger match TUTORIAL_ORDER?
// Soda is treated as a side item (presence only), burger body checked in order.
// ---------------------------------------------------------------------------
static bool BurgerMatchesOrder() {
    bool playerHasSoda = false;
    bool orderWantsSoda = false;
    for (int i : currentBurger) if (i == SODA) playerHasSoda = true;
    for (int i : TUTORIAL_ORDER)  if (i == SODA) orderWantsSoda = true;
    if (playerHasSoda != orderWantsSoda) return false;

    std::vector<int> fp, fo;
    for (int i : currentBurger) if (i != SODA) fp.push_back(i);
    for (int i : TUTORIAL_ORDER)  if (i != SODA) fo.push_back(i);

    if (fp.size() != fo.size()) return false;
    for (size_t i = 0; i < fo.size(); ++i)
        if (fp[i] != fo[i]) return false;
    return true;
}

// ---------------------------------------------------------------------------
// Helper: Clear the burger (mirrors Burger_Clear in game.cpp)
// ---------------------------------------------------------------------------
static void ClearBurger() {
    currentBurger.clear();
    burgerComplete = false;
    lettuceOnPlate = false;
    cheeseOnPlate = false;
    pattieOnPlate = false;
    tomatoOnPlate = false;
    onionOnPlate = false;
    cucumberOnPlate = false;
    bunTopOnPlate = false;
    bunBottomOnPlate = false;
    sodaOnPlate = false;
    sodaMachineHasCup = false;
    sodaIsFilled = false;
    sodaFillTimer = 0.0f;
    isDragging = false;
    burgerIsBeingDragged = false;
    sodaIsBeingDragged = false;
    burgerDragX = 300.0f;
    burgerDragY = -175.0f;
    sodaDragX = SodaX;
    sodaDragY = SodaY - 55.0f;
    printf("[Tutorial] Burger cleared.\n");
}

// ---------------------------------------------------------------------------
// Helper: Phase transition with logging
// ---------------------------------------------------------------------------
static void SetPhase(TutorialPhase p) {
    tutorialPhase = p;
    const char* names[] = {
        "CUSTOMER_WALKING", "SHOW_ORDER", "BUILD_BURGER",
        "FILL_SODA", "SERVE", "COMPLETE"
    };
    printf("[Tutorial] Phase -> %s\n", names[(int)p]);
    showOrderStep = 0;
}

// ===========================================================================
// Tutorial_Load
// ===========================================================================
void Tutorial_Load() {
    printf("[Tutorial] Tutorial_Load\n");

    FontId = AEGfxCreateFont("Assets/Font/Fredoka-Bold.ttf", 28);

    //SquareMesh = CreateSquareMesh(8.0f, 2.5f);
    SquareMesh = CreateSquareMesh(0.5f, 0.5f);
    BGMesh = CreateSquareMesh(0.5f, 0.5f);
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);
    SodaMesh = CreateSodaMesh(0.5f, 0.5f);

    PlateTexture = AEGfxTextureLoad("Assets/Assets/plate.png");
    GameplayBG_back = AEGfxTextureLoad("Assets/Assets/Gameplay_back.png");
    GameplayBG_front = AEGfxTextureLoad("Assets/Assets/Gameplay_front1.png");
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");
    SodaTexture = AEGfxTextureLoad("Assets/Assets/Soda.png");

    CustomerTexture[0] = AEGfxTextureLoad("Assets/Assets/syakilcust.png");
    CustomerTexture[1] = AEGfxTextureLoad("Assets/Assets/elicust.png");
    CustomerTexture[2] = AEGfxTextureLoad("Assets/Assets/tebcust.png");
    CustomerTexture[3] = AEGfxTextureLoad("Assets/Assets/diyacust.png");

    Points_Load();
    GarageDoor_Load();
    Sound_Load();
    Sound_PlayBGM();
}

// ===========================================================================
// Tutorial_Init
// ===========================================================================
void Tutorial_Init() {
    printf("[Tutorial] Tutorial_Init\n");

    tutEND = false;

    tutorialPhase = TUTE_CUSTOMER_WALKING;
    ClearBurger();

    // -----------------------------------------------------------------------
    // Fake a minimal currentLevelData so Customer_Init() and Points_Init()
    // don't crash or return zero. We never touch the CSV file.
    //
    // Key facts:
    //   - customerCount = 1  -> Customer_Spawn() is only allowed to fire once
    //   - starPositions all = 35 -> one correct serve fills the whole bar
    //   - all ingredient strings present -> RandomOrder() can build an order;
    //     we immediately overwrite it with TUTORIAL_ORDER after Customer_Init()
    // -----------------------------------------------------------------------
    currentLevelData.customerCount = 1;
    currentLevelData.durationMultiplier = 1.0f;
    currentLevelData.starPositions[0] = 35.0f;
    currentLevelData.starPositions[1] = 35.0f;
    currentLevelData.starPositions[2] = 35.0f;
    currentLevelData.starStatus[0] = 0;
    currentLevelData.starStatus[1] = 0;
    currentLevelData.starStatus[2] = 0;
    currentLevelData.highestPts = 0.0f;
    currentLevelData.ingredients = {
        "bottom_bun", "top_bun", "patty", "cheese",
        "lettuce", "tomato", "onion", "cucumber", "soda"
    };

    // Init subsystems that read currentLevelData
    Customer_Init();    // spawns first customer and calls RandomOrder() on it
    Points_Init();      // reads starPositions[2] for MAX_POINTS
    GarageDoor_Init();

    // -----------------------------------------------------------------------
    // Override the customer's order with our fixed tutorial order AFTER
    // Customer_Init() has finished (it calls RandomOrder() internally).
    // Pin patience to a huge value so Customer_Update's drain never reaches 0.
    // -----------------------------------------------------------------------
    if (!customers.empty()) {
        customers[0].order = TUTORIAL_ORDER;
        customers[0].hasOrder = true;
        customers[0].maxPatience = 99999.0f;
        customers[0].patience = 99999.0f;
    }
}

// ===========================================================================
// Tutorial_Update
// ===========================================================================
void Tutorial_Update() {
    f64 dt = AEFrameRateControllerGetFrameTime();

    // -----------------------------------------------------------------------
    // World-space mouse coords (same conversion as game.cpp)
    // -----------------------------------------------------------------------
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float ww = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    float wh = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    float wmx = (float)mx - ww / 2.0f;
    float wmy = -(float)my + wh / 2.0f;

    // -----------------------------------------------------------------------
    // PHASE: CUSTOMER_WALKING
    // Run normal Customer_Update so the walk-in animation plays.
    // Once isWaitingForFood becomes true, freeze patience and advance.
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_CUSTOMER_WALKING) {
        Customer_Update(static_cast<float>(dt));
        Points_Update();

        if (!customers.empty() && customers[0].isWaitingForFood) {
            // Re-pin patience now that the customer has stopped walking
            customers[0].patience = customers[0].maxPatience;
            SetPhase(TUTE_SHOW_ORDER);
        }
        return;
    }

    // -----------------------------------------------------------------------
    // For all remaining phases:
    //   - Do NOT call Customer_Update() - this prevents the spawn timer from
    //     firing a second customer and bypasses the patience drain code entirely.
    //   - Still call Points_Update() for the smooth bar lerp.
    //   - Re-pin patience every frame as a safety net.
    // -----------------------------------------------------------------------
    Points_Update();
    if (!customers.empty())
        customers[0].patience = customers[0].maxPatience;

    /*if (tutorialPhase == TUTE_SHOW_ORDER && AEInputCheckTriggered(AEVK_LBUTTON))
    {
        nextStep = true;
    }*/
    // -----------------------------------------------------------------------
    // PHASE: SHOW_ORDER
    // Display the "check order" prompt. Any click advances to BUILD_BURGER.
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_SHOW_ORDER) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (showOrderStep < 1)
                showOrderStep++;            // advance to next tip
            else
                SetPhase(TUTE_BUILD_BURGER); // third click: move on
        }
        return;
    }

    // -----------------------------------------------------------------------
    // PHASES: BUILD_BURGER  /  FILL_SODA  /  SERVE
    // All interactive building and dragging is handled in this block.
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_BUILD_BURGER ||
        tutorialPhase == TUTE_FILL_SODA ||
        tutorialPhase == TUTE_SERVE)
    {
        // -- Soda fill timer --
        if (sodaMachineHasCup && !sodaIsFilled) {
            sodaFillTimer += static_cast<float>(dt);
            if (sodaFillTimer >= 3.0f) {
                sodaFillTimer = 3.0f;
                sodaIsFilled = true;
                printf("[Tutorial] Soda finished filling!\n");
            }
        }

        // ==============================================================
        // MOUSE PRESSED
        // ==============================================================
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {

            // ---- Ingredient clicks (all unlocked in the tutorial) ----

            if (InBounds(wmx, wmy, BunBotX, BunBotY, BunBotW / 2, BunBotH / 2)) {
                bunBottomOnPlate = true;
                currentBurger.push_back(BOTTOM_BUN);
                printf("[Tutorial] Bottom bun added.\n");
            }
            if (InBounds(wmx, wmy, PattieX, PattieY, PattieW / 2, PattieH / 2)) {
                pattieOnPlate = true;
                currentBurger.push_back(PATTY);
                printf("[Tutorial] Patty added.\n");
            }
            if (InBounds(wmx, wmy, CheeseX, CheeseY, CheeseW / 2, CheeseH / 2)) {
                cheeseOnPlate = true;
                currentBurger.push_back(CHEESE);
                printf("[Tutorial] Cheese added.\n");
            }
            if (InBounds(wmx, wmy, LettuceX, LettuceY, LettuceW / 2, LettuceH / 2)) {
                lettuceOnPlate = true;
                currentBurger.push_back(LETTUCE);
                printf("[Tutorial] Lettuce added.\n");
            }
            if (InBounds(wmx, wmy, TomatoX, TomatoY, TomatoW / 2, TomatoH / 2)) {
                tomatoOnPlate = true;
                currentBurger.push_back(TOMATO);
                printf("[Tutorial] Tomato added.\n");
            }
            if (InBounds(wmx, wmy, OnionX, OnionY, OnionW / 2, OnionH / 2)) {
                onionOnPlate = true;
                currentBurger.push_back(ONION);
                printf("[Tutorial] Onion added.\n");
            }
            if (InBounds(wmx, wmy, CucumbX, CucumbY, CucumbW / 2, CucumbH / 2)) {
                cucumberOnPlate = true;
                currentBurger.push_back(CUCUMBER);
                printf("[Tutorial] Cucumber added.\n");
            }
            if (InBounds(wmx, wmy, BunTopX, BunTopY, BunTopW / 2, BunTopH / 2)) {
                bunTopOnPlate = true;
                burgerComplete = true;
                currentBurger.push_back(TOP_BUN);
                printf("[Tutorial] Top bun added - burger body complete!\n");
                // Advance from BUILD_BURGER -> FILL_SODA once top bun is placed
                if (tutorialPhase == TUTE_BUILD_BURGER)
                    SetPhase(TUTE_FILL_SODA);
            }

            // ---- Soda machine: start filling (only meaningful in FILL_SODA) ----
            if (tutorialPhase == TUTE_FILL_SODA &&
                !sodaMachineHasCup && !sodaOnPlate && !sodaIsBeingDragged &&
                InBounds(wmx, wmy, SodaX, SodaY, SodaW / 2, SodaH / 2)) {

                sodaMachineHasCup = true;
                sodaIsFilled = false;
                sodaFillTimer = 0.0f;
                sodaDragX = SodaX;
                sodaDragY = SodaY - 55.0f;
                printf("[Tutorial] Started filling soda...\n");
            }

            // ---- Begin dragging filled soda cup ----
            if (sodaMachineHasCup && sodaIsFilled &&
                InBounds(wmx, wmy, sodaDragX, sodaDragY, 35.0f, 45.0f)) {

                isDragging = true;
                sodaIsBeingDragged = true;
                burgerIsBeingDragged = false;
                sodaDragX = wmx;
                sodaDragY = wmy;
                printf("[Tutorial] Started dragging soda!\n");
            }

            // ---- Begin dragging burger from plate ----
            float plateCX = 300.0f, plateCY = -175.0f;
            float platePW = 200.0f, platePH = 100.0f;
            if (!currentBurger.empty() &&
                InBounds(wmx, wmy, plateCX, plateCY, platePW / 2, platePH / 2)) {

                isDragging = true;
                burgerIsBeingDragged = true;
                dragOffsetX = wmx - plateCX;
                dragOffsetY = wmy - plateCY;
                burgerDragX = wmx;
                burgerDragY = wmy;
                printf("[Tutorial] Started dragging burger!\n");
            }
        } // end AEVK_LBUTTON triggered

        // ==============================================================
        // CONTINUOUS DRAG UPDATE
        // ==============================================================
        if (isDragging && burgerIsBeingDragged) {
            burgerDragX = wmx;
            burgerDragY = wmy;
        }
        if (isDragging && sodaIsBeingDragged) {
            sodaDragX = wmx;
            sodaDragY = wmy;
        }

        // ==============================================================
        // MOUSE RELEASED
        // ==============================================================
        if (AEInputCheckReleased(AEVK_LBUTTON)) {

            float plateCX = 300.0f, plateCY = -175.0f;
            float platePW = 200.0f, platePH = 100.0f;

            // ---- Drop soda ----
            if (isDragging && sodaIsBeingDragged) {
                isDragging = false;

                bool onPlate = InBounds(sodaDragX, sodaDragY,
                    plateCX, plateCY, platePW / 2, platePH / 2);
                if (onPlate) {
                    sodaOnPlate = true;
                    sodaMachineHasCup = false;
                    sodaIsFilled = false;
                    sodaFillTimer = 0.0f;
                    sodaIsBeingDragged = false;
                    currentBurger.push_back(SODA);
                    printf("[Tutorial] Soda placed on plate!\n");

                    // Burger body + soda both done -> advance to SERVE
                    if (burgerComplete)
                        SetPhase(TUTE_SERVE);
                }
                else {
                    // Cup returns to machine
                    sodaIsBeingDragged = false;
                    sodaDragX = SodaX;
                    sodaDragY = SodaY - 55.0f;
                    printf("[Tutorial] Soda returned to machine.\n");
                }
            }
            // ---- Drop burger ----
            else if (isDragging && burgerIsBeingDragged) {
                isDragging = false;

                // -- Trash drop: clear burger and return to build phase --
                if (InBounds(burgerDragX, burgerDragY,
                    TrashX, TrashY, TrashW / 2, TrashH / 2)) {
                    printf("[Tutorial] Burger trashed - rebuild!\n");
                    ClearBurger();
                    SetPhase(TUTE_BUILD_BURGER);
                }
                // -- Serve to customer (only valid in TUTE_SERVE) --
                else if (tutorialPhase == TUTE_SERVE && !customers.empty()) {
                    Customer& cust = customers[0];
                    float dx = burgerDragX - cust.x;
                    float dy = burgerDragY - cust.y;

                    if (dx * dx + dy * dy <= 150.0f * 150.0f) {
                        bool correct = BurgerMatchesOrder();
                        if (correct) {
                            printf("[Tutorial] Correct order! Customer satisfied.\n");
                            Customer_Satisfy(0);
                            Points_AddPoints(true);
                            ClearBurger();
                            tutEND = true;
                            SetPhase(TUTE_COMPLETE);
                        }
                        else {
                            // Wrong order - player can drag to trash and rebuild
                            printf("[Tutorial] Wrong order - drag to TRASH to reset!\n");
                            burgerIsBeingDragged = false;
                        }
                    }
                    else {
                        // Missed the customer - burger stays on plate
                        printf("[Tutorial] Missed customer - burger returned to plate.\n");
                        burgerIsBeingDragged = false;
                    }
                }
                else {
                    burgerIsBeingDragged = false;
                }
            }
        } // end AEVK_LBUTTON released

        // R key resets the burger at any build phase (same shortcut as game.cpp)
        if (AEInputCheckTriggered(AEVK_R)) {
            ClearBurger();
            SetPhase(TUTE_BUILD_BURGER);
        }

        return;
    } // end build / soda / serve block

    // -----------------------------------------------------------------------
    // PHASE: COMPLETE
    // Show the end screen and listen for button clicks.
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_COMPLETE) {
        if (IsButtonClicked(btnMainMenu)) {
            next = GS_MAINMENU;
            printf("[Tutorial] -> Main Menu\n");
        }
        if (IsButtonClicked(btnLevels)) {
            next = GS_LEVELS;
            printf("[Tutorial] -> Level Select\n");
        }
    }
}

// ===========================================================================
// Tutorial_Draw
// ===========================================================================
void Tutorial_Draw() {

    AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // ---- Background (back layer) ----
    AEMtx33Scale(&BGScale, 1600.0f, 900.0f);
    AEMtx33Trans(&BGTranslate, 0.0f, 0.0f);
    AEMtx33Concat(&BGTransform, &BGTranslate, &BGScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(GameplayBG_back, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    // ---- Customers (behind counter) ----
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    Customer_Draw(CustomerTexture);

    // ---- Background (front layer) ----
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(GameplayBG_front, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    // ---- Points bar ----
    Points_Draw();

    // ---- Soda cup at machine ----
    if (sodaMachineHasCup && SodaTexture) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(SodaTexture, sodaIsFilled ? 0.5f : 0.0f, 0.0f);

        AEMtx33 sc, tr, tf;
        AEMtx33Scale(&sc, 70.0f, 110.0f);
        AEMtx33Trans(&tr,
            sodaIsBeingDragged ? sodaDragX : SodaX,
            sodaIsBeingDragged ? sodaDragY : SodaY - 55.0f);
        AEMtx33Concat(&tf, &tr, &sc);
        AEGfxSetTransform(tf.m);
        AEGfxMeshDraw(SodaMesh, AE_GFX_MDM_TRIANGLES);
    }

    // ---- Decide burger draw position ----
    float drawX = burgerIsBeingDragged ? burgerDragX : 275.0f;
    float drawY = burgerIsBeingDragged ? burgerDragY : -125.0f;

    // ---- Plate image ----
    if (PlateTexture) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(PlateTexture, 0.f, 0.f);
        AEMtx33Scale(&PlateScale, 300.0f, 125.0f);
        AEMtx33Trans(&PlateTranslate, drawX, drawY - 20.0f);
        AEMtx33Concat(&PlateTransform, &PlateTranslate, &PlateScale);
        AEGfxSetTransform(PlateTransform.m);
        AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
    }

    // ---- Stacked burger ingredients ----
    float h = 0.0f;      // running stack height
    const float S = 150.0f;    // ingredient draw size

    auto DrawIng = [&](float u, float v, float yBase, float stackAdd) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(Ingredients, u, v);
        AEMtx33Scale(&IngScale, S, S);
        AEMtx33Trans(&IngTranslate, drawX - 20, drawY + yBase + h);
        AEMtx33Concat(&IngTransform, &IngTranslate, &IngScale);
        AEGfxSetTransform(IngTransform.m);
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
        h += stackAdd;
        };

    if (bunBottomOnPlate) DrawIng(3.f / 4.f, 1.f / 2.f, -10.0f, 15.0f);
    if (lettuceOnPlate)   DrawIng(2.f / 4.f, 0.f, 0.0f, 8.0f);
    if (cheeseOnPlate)    DrawIng(0.f, 1.f / 2.f, 0.0f, 10.0f);
    if (pattieOnPlate)    DrawIng(0.f, 0.f, 0.0f, 25.0f);
    if (tomatoOnPlate)    DrawIng(1.f / 4.f, 0.f, 0.0f, 18.0f);
    if (cucumberOnPlate)  DrawIng(2.f / 4.f, 1.f / 2.f, 0.0f, 8.0f);
    if (onionOnPlate)     DrawIng(1.f / 4.f, 1.f / 2.f, 0.0f, 8.0f);

    if (bunTopOnPlate) {
        h -= 5.0f;  // top bun slightly compresses the stack
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(Ingredients, 3.f / 4.f, 0.f);
        AEMtx33Scale(&IngScale, S, S);
        AEMtx33Trans(&IngTranslate, drawX - 20.0f, drawY + h + 15.0f);
        AEMtx33Concat(&IngTransform, &IngTranslate, &IngScale);
        AEGfxSetTransform(IngTransform.m);
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
    }

    // ---- Soda on plate ----
    if (sodaOnPlate && SodaTexture) {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxTextureSet(SodaTexture, 1.f / 2.f, 0.f);
        AEMtx33Scale(&IngScale, 70.0f, 110.0f);
        AEMtx33Trans(&IngTranslate, drawX + 70.0f, drawY + 15.0f);
        AEMtx33Concat(&IngTransform, &IngTranslate, &IngScale);
        AEGfxSetTransform(IngTransform.m);
        AEGfxMeshDraw(SodaMesh, AE_GFX_MDM_TRIANGLES);
    }

    // ---- Garage door ----
    GarageDoor_Draw();

    // ================================================================
    // TUTORIAL OVERLAY (drawn last so it sits on top of everything)
    // ================================================================

    if (!tutEND)
    {
        // Semi-transparent prompt bar at the bottom of the screen
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.65f);
        {
            AEMtx33 s, t, m;
            AEMtx33Scale(&s, 800.0f, 150.0f);
            //AEMtx33Trans(&t, 200.0f, 125.0f);
            AEMtx33Trans(&t, 225.0f, 200.0f);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        // "click to continue" hint shown on all steps
        AEGfxPrint(FontId, "[ Click to continue ]",
            0.15f, 0.3f, 0.75f, 0.7f, 0.7f, 0.7f, 1.0f);
    }

    // AEGfxPrint(fontId, text, normX, normY, scale, r, g, b)
    // normX / normY are in [-1, 1] normalised screen space.
    switch (tutorialPhase) {

    case TUTE_CUSTOMER_WALKING:
        AEGfxPrint(FontId, "A customer is on their way...",
            0.06f, 0.45f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        break;

    case TUTE_SHOW_ORDER:
        if (showOrderStep == 0)
        {
            AEGfxPrint(FontId, "A customer has arrived! Check their order above.",
                -0.12f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(FontId, "The burger must be made in EXACT order to satisfy the customer!",
                -0.17f, 0.4f, 0.85f, 1.0f, 0.9f, 0.2f, 1.0f);
        }
        else if (showOrderStep == 1)
        {
            AEGfxPrint(FontId, "The Green bar shows the customers patience level",
                -0.12f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(FontId, "Don't take too long or else the customers will leave in anger!",
                -0.13f, 0.4f, 0.85f, 1.0f, 0.9f, 0.2f, 1.0f);
        }
        break;

    case TUTE_BUILD_BURGER:
        AEGfxPrint(FontId, "Build the order! Click the ingredients on the left.",
            -0.12f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(FontId, "Wrong ingredient? Drag the burger to the TRASH BIN to reset.",
            -0.15f, 0.4f, 0.85f, 1.0f, 0.45f, 0.45f, 1.0f);
        break;

    case TUTE_FILL_SODA:
        AEGfxPrint(FontId, "Burger done! Click the button on the SODA MACHINE",
            -0.15f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(FontId, "Wait for the cup to fill, then DRAG it onto the plate.",
            -0.1f, 0.4f, 0.85f, 0.4f, 1.0f, 0.55f, 1.0f);
        break;

    case TUTE_SERVE:
        AEGfxPrint(FontId, "Order complete! DRAG the plate to the customer to serve!",
            -0.18f, 0.45f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        break;

    case TUTE_COMPLETE:
    {
        // Full-screen dark overlay
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.7f);
        {
            AEMtx33 s, t, m;
            AEMtx33Scale(&s, 1600.0f, 900.0f);
            AEMtx33Trans(&t, 0.0f, 0.0f);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxPrint(FontId, "Tutorial Complete!",
            -0.23f, 0.25f, 1.5f, 1.0f, 0.85f, 0.1f, 1.0f);
        AEGfxPrint(FontId, "You served your first customer. You're ready!",
            -0.37f, 0.08f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

        // Main Menu button — brighter when hovered
        btnMainMenu.isHovered = IsButtonHovered(btnMainMenu);
        AEGfxSetColorToMultiply(
            btnMainMenu.isHovered ? 0.3f : 0.15f,
            btnMainMenu.isHovered ? 0.45f : 0.25f,
            btnMainMenu.isHovered ? 0.9f : 0.65f,
            1.0f);
        {
            AEMtx33 s, t, m;
            AEMtx33Scale(&s, btnMainMenu.width, btnMainMenu.height);
            AEMtx33Trans(&t, btnMainMenu.x, btnMainMenu.y);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
        }

        // Levels button — brighter when hovered
        btnLevels.isHovered = IsButtonHovered(btnLevels);
        AEGfxSetColorToMultiply(
            btnLevels.isHovered ? 0.2f : 0.1f,
            btnLevels.isHovered ? 0.75f : 0.5f,
            btnLevels.isHovered ? 0.35f : 0.2f,
            1.0f);
        {
            AEMtx33 s, t, m;
            AEMtx33Scale(&s, btnLevels.width, btnLevels.height);
            AEMtx33Trans(&t, btnLevels.x, btnLevels.y);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        AEGfxPrint(FontId, "Main Menu", -0.28f, -0.68f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(FontId, "Play Levels", 0.09f, -0.68f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        break;
    }

    default: break;
    }
}

// ===========================================================================
// Tutorial_Free
// ===========================================================================
void Tutorial_Free() {
    printf("[Tutorial] Tutorial_Free\n");
    Customer_Free();
    Points_Free();
    GarageDoor_Free();
    Sound_Free();
}

// ===========================================================================
// Tutorial_Unload
// ===========================================================================
void Tutorial_Unload() {
    printf("[Tutorial] Tutorial_Unload\n");

    if (FontId >= 0) {
        AEGfxDestroyFont(FontId);
        FontId = -1;
    }

    if (Ingredients) { AEGfxTextureUnload(Ingredients);      Ingredients = nullptr; }
    if (SodaTexture) { AEGfxTextureUnload(SodaTexture);       SodaTexture = nullptr; }
    if (PlateTexture) { AEGfxTextureUnload(PlateTexture);      PlateTexture = nullptr; }
    if (GameplayBG_back) { AEGfxTextureUnload(GameplayBG_back);   GameplayBG_back = nullptr; }
    if (GameplayBG_front) { AEGfxTextureUnload(GameplayBG_front);  GameplayBG_front = nullptr; }

    for (int i = 0; i < 4; ++i) {
        if (CustomerTexture[i]) {
            AEGfxTextureUnload(CustomerTexture[i]);
            CustomerTexture[i] = nullptr;
        }
    }

    if (SquareMesh) { AEGfxMeshFree(SquareMesh);       SquareMesh = nullptr; }
    if (BGMesh) { AEGfxMeshFree(BGMesh);            BGMesh = nullptr; }
    if (IngredientsMesh) { AEGfxMeshFree(IngredientsMesh);   IngredientsMesh = nullptr; }
    if (SodaMesh) { AEGfxMeshFree(SodaMesh);          SodaMesh = nullptr; }

    GarageDoor_Unload();
    Points_Unload();
    Sound_Unload();
    // Note: Level_Manager_Unload() is NOT called here.
    // We wrote directly to currentLevelData in _Init without touching the CSV,
    // so there is nothing for Level_Manager to clean up.
}