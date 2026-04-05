// ---------------------------------------------------------------------------
// Tutorial.cpp
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

// ---------------------------------------------------------------------------
// Typewriter effect state
// ---------------------------------------------------------------------------
static char   typewriterBuf[512] = {};   // currently displayed (partial) text
static char   typewriterFull[512] = {};   // target full text
static float  typewriterTimer = 0.0f;
static int    typewriterIndex = 0;
static float  typewriterSpeed = 0.032f; // seconds per character
static bool   typewriterDone = false;

// ---------------------------------------------------------------------------
// Wrong-order flash state
// ---------------------------------------------------------------------------
static bool  showWrongOrderMsg = false;

// Helper: start a new typewriter sequence
static void TypewriterSet(const char* text) {
    strncpy_s(typewriterFull, sizeof(typewriterFull), text, _TRUNCATE);
    memset(typewriterBuf, 0, sizeof(typewriterBuf));
    typewriterIndex = 0;
    typewriterTimer = 0.0f;
    typewriterDone = false;
}

// Update typewriter each frame; call with dt
static void TypewriterUpdate(float dt) {
    if (typewriterDone) return;
    typewriterTimer += dt;
    while (!typewriterDone && typewriterTimer >= typewriterSpeed) {
        typewriterTimer -= typewriterSpeed;
        int len = (int)strlen(typewriterFull);
        if (typewriterIndex < len) {
            typewriterBuf[typewriterIndex] = typewriterFull[typewriterIndex];
            typewriterIndex++;
            typewriterBuf[typewriterIndex] = '\0';
        }
        if (typewriterIndex >= len) typewriterDone = true;
    }
}

// Hardcoded tutorial order: bottom bun, patty, top bun + soda as side item
static const std::vector<int> TUTORIAL_ORDER = { BOTTOM_BUN, PATTY, TOP_BUN, SODA };

// ===========================================================================
// CENTRAL TEXT TABLE
// All tutorial strings, positions, colours, and display conditions live here.
// To change any wording, position, or colour - edit this table only.
// ===========================================================================
enum TutTextCond {
    TTC_ALWAYS,         // show whenever phase/step match
    TTC_TYPEWRITER,     // this is the typewriter line (print typewriterBuf, not text[])
    TTC_AFTER_TW,       // only show once typewriter has finished
    TTC_WRONG_ORDER,    // only show while showWrongOrderMsg is true (BUILD_BURGER only)
    TTC_SODA_NOT_PLACED,// only show while soda is not yet on the plate
    TTC_NO_WRONG_ORDER  // like TTC_TYPEWRITER but hidden when showWrongOrderMsg is true
};

struct TutorialText {
    int            phase;       // TutorialPhase cast to int; -1 = all phases
    int            orderStep;   // showOrderStep required; -1 = any
    TutTextCond    cond;
    float          x, y, scale;
    float          r, g, b;
    const char* text;
};

// ---- EDIT TEXT, POSITIONS AND COLOURS HERE --------------------------------
static const TutorialText TUT_TEXTS[] =
{
    // ---- CUSTOMER WALKING ------------------------------------------------
    { TUTE_CUSTOMER_WALKING, -1, TTC_TYPEWRITER,
        0.06f, 0.45f, 1.0f,   1.0f, 1.0f, 1.0f,
        "A customer is on their way..." },

        // ---- SHOW ORDER  (step 0) --------------------------------------------
        { TUTE_SHOW_ORDER, 0, TTC_TYPEWRITER,
            -0.12f, 0.5f, 1.0f,   1.0f, 1.0f, 1.0f,
            "A customer has arrived! Check their order above." },

        { TUTE_SHOW_ORDER, 0, TTC_AFTER_TW,
            -0.17f, 0.4f, 0.85f,  1.0f, 0.9f, 0.2f,
            "The burger AND soda must be made together - in EXACT order!" },

            // ---- SHOW ORDER  (step 1) --------------------------------------------
            { TUTE_SHOW_ORDER, 1, TTC_TYPEWRITER,
                -0.1f, 0.5f, 1.0f,   1.0f, 1.0f, 1.0f,
                "The GREEN bar shows the customer's patience." },

            { TUTE_SHOW_ORDER, 1, TTC_AFTER_TW,
                -0.1f, 0.4f, 0.85f,  1.0f, 0.9f, 0.2f,
                "Don't take too long or the customer will leave in anger!" },

                // ---- BUILD BURGER ----------------------------------------------------
                // Normal instruction — hidden when a wrong ingredient has been placed
                { TUTE_BUILD_BURGER, -1, TTC_NO_WRONG_ORDER,
                    -0.1f, 0.45f, 1.0f,   1.0f, 1.0f, 1.0f,
                    "Build the order! Click the ingredients below." },

                    // Wrong-ingredient message — replaces the instruction at the same position
                    { TUTE_BUILD_BURGER, -1, TTC_WRONG_ORDER,
                        -0.17f, 0.45f, 1.0f,   1.0f, 0.3f, 0.3f,
                        "WRONG INGREDIENT/ORDER! Drag to the TRASH to redo." },

                        // ---- FILL SODA -------------------------------------------------------
                        { TUTE_FILL_SODA, -1, TTC_TYPEWRITER,
                            -0.12f, 0.5f, 1.0f,   1.0f, 1.0f, 1.0f,
                            "Burger done! Now click the soda machine button." },

                        { TUTE_FILL_SODA, -1, TTC_AFTER_TW,
                            -0.14f,  0.4f, 0.85f,  0.4f, 1.0f, 0.55f,
                            "Wait for the cup to fill, then CLICK it to snap onto the plate." },

                            // ---- SERVE -----------------------------------------------------------
                            { TUTE_SERVE, -1, TTC_TYPEWRITER,
                                -0.18f, 0.45f, 1.0f,  1.0f, 1.0f, 1.0f,
                                "Order complete! DRAG the plate to the customer to serve!" },

                                // ---- COMPLETE --------------------------------------------------------
                                { TUTE_COMPLETE, -1, TTC_ALWAYS,
                                    -0.23f, 0.25f, 1.5f,  1.0f, 0.85f, 0.1f,
                                    "Tutorial Complete!" },

                                { TUTE_COMPLETE, -1, TTC_ALWAYS,
                                    -0.37f, 0.08f, 1.0f,  1.0f, 1.0f,  1.0f,
                                    "You served your first customer. You're ready!" },

                                { TUTE_COMPLETE, -1, TTC_ALWAYS,
                                    -0.28f, -0.68f, 1.0f, 1.0f, 1.0f,  1.0f,
                                    "Main Menu" },

                                { TUTE_COMPLETE, -1, TTC_ALWAYS,
                                    0.09f, -0.68f, 1.0f,  1.0f, 1.0f,  1.0f,
                                    "Play Levels" },

                                    // ---- CLICK TO CONTINUE hint ------------------------------------------
                                    { TUTE_CUSTOMER_WALKING, -1, TTC_AFTER_TW,
                                        0.15f, 0.3f, 0.75f,   0.7f, 0.7f, 0.7f,
                                        "[ Click to continue ]" },

                                    { TUTE_SHOW_ORDER, -1, TTC_AFTER_TW,
                                        0.15f, 0.3f, 0.75f,   0.7f, 0.7f, 0.7f,
                                        "[ Click to continue ]" },
};
static const int TUT_TEXTS_COUNT = (int)(sizeof(TUT_TEXTS) / sizeof(TUT_TEXTS[0]));
// --------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Mesh / texture pointers
// ---------------------------------------------------------------------------
static AEGfxVertexList* SquareMesh = nullptr;
static AEGfxVertexList* BGMesh = nullptr;
static AEGfxVertexList* IngredientsMesh = nullptr;
static AEGfxVertexList* SodaMesh = nullptr;

// ---------------------------------------------------------------------------
// Pause Menu
// ---------------------------------------------------------------------------
static bool tutIsPaused = false;
static AEGfxVertexList* PlateMesh = nullptr;

static float TutPauseMenuBtnX = 0.0f, TutPauseMenuBtnY = -30.0f;
static float TutPauseMenuBtnW = 300.0f, TutPauseMenuBtnH = 55.0f;
static float TutPauseSettBtnX = 0.0f, TutPauseSettBtnY = -100.0f;
static float TutPauseSettBtnW = 300.0f, TutPauseSettBtnH = 55.0f;
static float TutPauseResumeBtnX = 0.0f, TutPauseResumeBtnY = 55.0f;
static float TutPauseResumeBtnW = 300.0f, TutPauseResumeBtnH = 55.0f;
static AEGfxVertexList* CircleMesh = nullptr;

// ---------------------------------------------------------------------------
// Shared transform matrices
// ---------------------------------------------------------------------------
static AEMtx33 BGTransform, BGScale, BGTranslate;
static AEMtx33 PlateTransform, PlateScale, PlateTranslate;
static AEMtx33 IngTransform, IngScale, IngTranslate;

// ---------------------------------------------------------------------------
// Ingredient hitbox positions (identical to game.cpp)
// ---------------------------------------------------------------------------
static const float BunTopX = -523.0f, BunTopY = -81.0f, BunTopW = 120.0f, BunTopH = 100.0f;
static const float BunBotX = -560.0f, BunBotY = -180.0f, BunBotW = 120.0f, BunBotH = 100.0f;
static const float PattieX = -275.0f, PattieY = -110.0f, PattieW = 83.0f, PattieH = 83.0f;
static const float CheeseX = -150.0f, CheeseY = -110.0f, CheeseW = 83.0f, CheeseH = 83.0f;
static const float LettuceX = -150.0f, LettuceY = -170.0f, LettuceW = 90.0f, LettuceH = 83.0f;
static const float TomatoX = -300.0f, TomatoY = -175.0f, TomatoW = 83.0f, TomatoH = 83.0f;
static const float OnionX = 3.0f, OnionY = -175.0f, OnionW = 83.0f, OnionH = 83.0f;
static const float CucumbX = -18.0f, CucumbY = -108.0f, CucumbW = 83.0f, CucumbH = 83.0f;
static const float SodaX = 574.0f, SodaY = -88.0f, SodaW = 70.0f, SodaH = 70.0f;
static const float TrashX = 630.0f, TrashY = -350.0f, TrashW = 200.0f, TrashH = 200.0f;

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
// Soda now snaps directly to the plate on click (no dragging)

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
    burgerDragX = 300.0f;
    burgerDragY = -175.0f;
    printf("[Tutorial] Burger cleared.\n");
}

// ---------------------------------------------------------------------------
// Helper: Check whether the ingredient just added matches the expected slot.
// ---------------------------------------------------------------------------
static void CheckLastIngredient()
{
    if (currentBurger.empty()) return;

    std::vector<int> placed;
    for (int id : currentBurger)
        if (id != SODA) placed.push_back(id);

    std::vector<int> expected;
    for (int id : TUTORIAL_ORDER)
        if (id != SODA) expected.push_back(id);

    size_t slot = placed.size() - 1;
    if (slot < expected.size() && placed[slot] != expected[slot])
    {
        printf("[Tutorial] Wrong ingredient at slot %zu (got %d, want %d)\n",
            slot, placed[slot], expected[slot]);
        showWrongOrderMsg = true;
    }
}

// ---------------------------------------------------------------------------
// Helper: Phase transition with logging.
// Always restarts the typewriter for the new phase.
// ---------------------------------------------------------------------------
static void SetPhase(TutorialPhase p) {
    tutorialPhase = p;
    const char* names[] = {
        "CUSTOMER_WALKING", "SHOW_ORDER", "BUILD_BURGER",
        "FILL_SODA", "SERVE", "COMPLETE"
    };
    printf("[Tutorial] Phase -> %s\n", names[(int)p]);
    showOrderStep = 0;
    showWrongOrderMsg = false;

    // Look up the opening typewriter line from TUT_TEXTS[]
    const char* twText = nullptr;
    for (int i = 0; i < TUT_TEXTS_COUNT; ++i) {
        const TutorialText& e = TUT_TEXTS[i];
        if ((e.cond == TTC_TYPEWRITER || e.cond == TTC_NO_WRONG_ORDER) &&
            e.phase == (int)p &&
            (e.orderStep == -1 || e.orderStep == 0))
        {
            twText = e.text;
            break;
        }
    }
    if (twText)
        TypewriterSet(twText);
    else {
        memset(typewriterBuf, 0, sizeof(typewriterBuf));
        typewriterDone = true;
    }
}

// ---------------------------------------------------------------------------
// Helper: Reset burger to BUILD_BURGER WITHOUT restarting the typewriter.
// Used by trash-drop and R-key reset.
// ---------------------------------------------------------------------------
static void ResetBurger()
{
    ClearBurger();
    tutorialPhase = TUTE_BUILD_BURGER;
    showOrderStep = 0;
    showWrongOrderMsg = false;
    // Snap typewriter to done so the static text shows immediately
    if (!typewriterDone) {
        strcpy_s(typewriterBuf, sizeof(typewriterBuf), typewriterFull);
        typewriterIndex = (int)strlen(typewriterFull);
        typewriterDone = true;
    }
    printf("[Tutorial] Burger reset - instruction stays on screen.\n");
}

// ===========================================================================
// Tutorial_Load
// ===========================================================================
void Tutorial_Load() {
    printf("[Tutorial] Tutorial_Load\n");

    FontId = AEGfxCreateFont("Assets/Font/Fredoka-Bold.ttf", 28);

    SquareMesh = CreateSquareMesh(0.5f, 0.5f);
    BGMesh = CreateSquareMesh(0.5f, 0.5f);
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);
    SodaMesh = CreateSodaMesh(0.5f, 0.5f);
    PlateMesh = CreateCircleMesh(2.0f, 32);

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
    Settings_Load();
}

// ===========================================================================
// Tutorial_Init
// ===========================================================================
void Tutorial_Init() {
    printf("[Tutorial] Tutorial_Init\n");

    tutEND = false;

    ClearBurger();
    showWrongOrderMsg = false;
    SetPhase(TUTE_CUSTOMER_WALKING);

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

    Customer_Init();
    Points_Init();
    GarageDoor_Init();

    if (!customers.empty()) {
        customers[0].order = TUTORIAL_ORDER;
        customers[0].hasOrder = true;
        customers[0].maxPatience = 99999.0f;
        customers[0].patience = 99999.0f;
    }
    CircleMesh = CreateCircleMesh(2.0f, 32);
}

// ===========================================================================
// Tutorial_Update
// ===========================================================================
void Tutorial_Update() {
    f64 dt = AEFrameRateControllerGetFrameTime();

    Settings_Update((float)dt, FontId);
    if (Settings_IsOpen()) return;

    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        tutIsPaused = !tutIsPaused;
    }

    if (tutIsPaused) {
        s32 mx, my;
        AEInputGetCursorPosition(&mx, &my);
        float ww = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
        float wh = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
        float wmx = (float)mx - ww / 2.0f;
        float wmy = -(float)my + wh / 2.0f;

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (wmx >= TutPauseResumeBtnX - TutPauseResumeBtnW / 2.0f &&
                wmx <= TutPauseResumeBtnX + TutPauseResumeBtnW / 2.0f &&
                wmy >= TutPauseResumeBtnY - TutPauseResumeBtnH / 2.0f &&
                wmy <= TutPauseResumeBtnY + TutPauseResumeBtnH / 2.0f) {
                tutIsPaused = false;
            }
            if (wmx >= TutPauseMenuBtnX - TutPauseMenuBtnW / 2.0f &&
                wmx <= TutPauseMenuBtnX + TutPauseMenuBtnW / 2.0f &&
                wmy >= TutPauseMenuBtnY - TutPauseMenuBtnH / 2.0f &&
                wmy <= TutPauseMenuBtnY + TutPauseMenuBtnH / 2.0f) {
                tutIsPaused = false;
                next = GS_MAINMENU;
            }
            if (wmx >= TutPauseSettBtnX - TutPauseSettBtnW / 2.0f &&
                wmx <= TutPauseSettBtnX + TutPauseSettBtnW / 2.0f &&
                wmy >= TutPauseSettBtnY - TutPauseSettBtnH / 2.0f &&
                wmy <= TutPauseSettBtnY + TutPauseSettBtnH / 2.0f) {
                Settings_Open();
            }
        }
        return;
    }

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float ww = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    float wh = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    float wmx = (float)mx - ww / 2.0f;
    float wmy = -(float)my + wh / 2.0f;

    // -----------------------------------------------------------------------
    // PHASE: CUSTOMER_WALKING
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_CUSTOMER_WALKING) {
        Customer_Update(static_cast<float>(dt));
        Points_Update();
        TypewriterUpdate(static_cast<float>(dt));

        if (!customers.empty() && customers[0].isWaitingForFood) {
            customers[0].patience = customers[0].maxPatience;
            SetPhase(TUTE_SHOW_ORDER);
        }
        return;
    }

    Points_Update();
    if (!customers.empty())
        customers[0].patience = customers[0].maxPatience;

    TypewriterUpdate(static_cast<float>(dt));

    // -----------------------------------------------------------------------
    // PHASE: SHOW_ORDER
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_SHOW_ORDER) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (!typewriterDone) {
                strcpy_s(typewriterBuf, sizeof(typewriterBuf), typewriterFull);
                typewriterIndex = (int)strlen(typewriterFull);
                typewriterDone = true;
            }
            else if (showOrderStep < 1) {
                showOrderStep++;
                for (int i = 0; i < TUT_TEXTS_COUNT; ++i) {
                    const TutorialText& e = TUT_TEXTS[i];
                    if ((e.cond == TTC_TYPEWRITER || e.cond == TTC_NO_WRONG_ORDER) &&
                        e.phase == (int)TUTE_SHOW_ORDER &&
                        e.orderStep == showOrderStep)
                    {
                        TypewriterSet(e.text);
                        break;
                    }
                }
            }
            else {
                SetPhase(TUTE_BUILD_BURGER);
            }
        }
        return;
    }

    // -----------------------------------------------------------------------
    // PHASES: BUILD_BURGER / FILL_SODA / SERVE
    // -----------------------------------------------------------------------
    if (tutorialPhase == TUTE_BUILD_BURGER ||
        tutorialPhase == TUTE_FILL_SODA ||
        tutorialPhase == TUTE_SERVE)
    {
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

            // ---- Ingredient clicks: only allowed during BUILD_BURGER ----
            if (tutorialPhase == TUTE_BUILD_BURGER) {
                if (!bunBottomOnPlate &&
                    InBounds(wmx, wmy, BunBotX, BunBotY, BunBotW / 2, BunBotH / 2)) {
                    bunBottomOnPlate = true;
                    currentBurger.push_back(BOTTOM_BUN);
                    CheckLastIngredient();
                    printf("[Tutorial] Bottom bun added.\n");
                }
                if (!pattieOnPlate &&
                    InBounds(wmx, wmy, PattieX, PattieY, PattieW / 2, PattieH / 2)) {
                    pattieOnPlate = true;
                    currentBurger.push_back(PATTY);
                    CheckLastIngredient();
                    printf("[Tutorial] Patty added.\n");
                }
                if (!cheeseOnPlate &&
                    InBounds(wmx, wmy, CheeseX, CheeseY, CheeseW / 2, CheeseH / 2)) {
                    cheeseOnPlate = true;
                    currentBurger.push_back(CHEESE);
                    CheckLastIngredient();
                    printf("[Tutorial] Cheese added.\n");
                }
                if (!lettuceOnPlate &&
                    InBounds(wmx, wmy, LettuceX, LettuceY, LettuceW / 2, LettuceH / 2)) {
                    lettuceOnPlate = true;
                    currentBurger.push_back(LETTUCE);
                    CheckLastIngredient();
                    printf("[Tutorial] Lettuce added.\n");
                }
                if (!tomatoOnPlate &&
                    InBounds(wmx, wmy, TomatoX, TomatoY, TomatoW / 2, TomatoH / 2)) {
                    tomatoOnPlate = true;
                    currentBurger.push_back(TOMATO);
                    CheckLastIngredient();
                    printf("[Tutorial] Tomato added.\n");
                }
                if (!onionOnPlate &&
                    InBounds(wmx, wmy, OnionX, OnionY, OnionW / 2, OnionH / 2)) {
                    onionOnPlate = true;
                    currentBurger.push_back(ONION);
                    CheckLastIngredient();
                    printf("[Tutorial] Onion added.\n");
                }
                if (!cucumberOnPlate &&
                    InBounds(wmx, wmy, CucumbX, CucumbY, CucumbW / 2, CucumbH / 2)) {
                    cucumberOnPlate = true;
                    currentBurger.push_back(CUCUMBER);
                    CheckLastIngredient();
                    printf("[Tutorial] Cucumber added.\n");
                }
                if (!bunTopOnPlate &&
                    InBounds(wmx, wmy, BunTopX, BunTopY, BunTopW / 2, BunTopH / 2)) {
                    bunTopOnPlate = true;
                    burgerComplete = true;
                    currentBurger.push_back(TOP_BUN);
                    CheckLastIngredient();
                    printf("[Tutorial] Top bun added - burger body complete!\n");
                    if (!showWrongOrderMsg)
                        SetPhase(TUTE_FILL_SODA);
                }
            } // end BUILD_BURGER-only ingredient block

            // ---- Soda machine: click to start filling ----
            if (tutorialPhase == TUTE_FILL_SODA &&
                !sodaMachineHasCup && !sodaOnPlate &&
                InBounds(wmx, wmy, SodaX, SodaY, SodaW / 2, SodaH / 2)) {
                sodaMachineHasCup = true;
                sodaIsFilled = false;
                sodaFillTimer = 0.0f;
                Sound_PlaySFX_Pour();
                printf("[Tutorial] Started filling soda...\n");
            }

            // ---- Click filled soda cup to snap it straight onto the plate ----
            if (sodaMachineHasCup && sodaIsFilled &&
                InBounds(wmx, wmy, SodaX, SodaY - 55.0f, 35.0f, 45.0f)) {
                sodaOnPlate = true;
                sodaMachineHasCup = false;
                sodaIsFilled = false;
                sodaFillTimer = 0.0f;
                currentBurger.push_back(SODA);
                printf("[Tutorial] Soda snapped to plate!\n");
                if (burgerComplete)
                    SetPhase(TUTE_SERVE);
            }

            // ---- Begin dragging burger from plate ----
            float plateCX = 290.0f, plateCY = -210.0f;
            float platePW = 300.0f, platePH = 300.0f;
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
        }

        // ==============================================================
        // CONTINUOUS DRAG UPDATE
        // ==============================================================
        if (isDragging && burgerIsBeingDragged) {
            burgerDragX = wmx;
            burgerDragY = wmy;
        }

        // ==============================================================
        // MOUSE RELEASED
        // ==============================================================
        if (AEInputCheckReleased(AEVK_LBUTTON)) {

            // ---- Drop burger ----
            if (isDragging && burgerIsBeingDragged) {
                isDragging = false;

                if (tutorialPhase == TUTE_BUILD_BURGER &&
                    InBounds(burgerDragX, burgerDragY,
                        TrashX, TrashY, TrashW / 2, TrashH / 2)) {
                    printf("[Tutorial] Burger trashed - rebuild!\n");
                    ResetBurger();
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
                            printf("[Tutorial] Wrong order served - drag to TRASH to reset!\n");
                            burgerIsBeingDragged = false;
                        }
                    }
                    else {
                        printf("[Tutorial] Missed customer - burger returned to plate.\n");
                        burgerIsBeingDragged = false;
                    }
                }
                else {
                    burgerIsBeingDragged = false;
                }
            }
        }

        if (tutorialPhase == TUTE_BUILD_BURGER && AEInputCheckTriggered(AEVK_R)) {
            ResetBurger();
        }

        return;
    }

    // -----------------------------------------------------------------------
    // PHASE: COMPLETE
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
// Tutorial_DrawText  - single function that renders ALL tutorial text.
// Every string, position, scale and colour comes from TUT_TEXTS above.
// ===========================================================================
static void Tutorial_DrawText()
{
    // tutEND suppresses the in-game prompt bar, but COMPLETE still needs text.
    if (tutEND && tutorialPhase != TUTE_COMPLETE) return;

    // -----------------------------------------------------------------------
    // All text printed here.
    // -----------------------------------------------------------------------
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

    for (int i = 0; i < TUT_TEXTS_COUNT; ++i)
    {
        const TutorialText& e = TUT_TEXTS[i];

        // Phase filter
        if (e.phase != -1 && e.phase != (int)tutorialPhase) continue;

        // Order-step filter
        if (e.orderStep != -1 && e.orderStep != showOrderStep) continue;

        // Condition filter
        switch (e.cond)
        {
        case TTC_TYPEWRITER:
            AEGfxPrint(FontId, typewriterBuf,
                e.x, e.y, e.scale, e.r, e.g, e.b, 1.0f);
            continue;

        case TTC_NO_WRONG_ORDER:
            // Like TTC_TYPEWRITER but suppressed when a wrong ingredient is active
            if (showWrongOrderMsg) continue;
            AEGfxPrint(FontId, typewriterBuf,
                e.x, e.y, e.scale, e.r, e.g, e.b, 1.0f);
            continue;

        case TTC_AFTER_TW:
            if (!typewriterDone) continue;
            break;

        case TTC_WRONG_ORDER:
            if (!showWrongOrderMsg) continue;
            break;

        case TTC_SODA_NOT_PLACED:
            if (!typewriterDone) continue;
            if (sodaOnPlate)     continue;
            break;

        case TTC_ALWAYS:
        default:
            break;
        }

        AEGfxPrint(FontId, e.text,
            e.x, e.y, e.scale, e.r, e.g, e.b, 1.0f);
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
        AEMtx33Trans(&tr, SodaX, SodaY - 55.0f);
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
    float h = 0.0f;
    const float S = 150.0f;

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
        h -= 5.0f;
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
    // TUTORIAL OVERLAY
    // All text is rendered by Tutorial_DrawText() which reads TUT_TEXTS[].
    // ================================================================

    if (!tutEND)
    {
        // Semi-transparent prompt bar background
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.65f);
        {
            AEMtx33 s, t, m;
            AEMtx33Scale(&s, 800.0f, 150.0f);
            AEMtx33Trans(&t, 225.0f, 200.0f);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(SquareMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    }

    // COMPLETE screen - button geometry only (text comes from Tutorial_DrawText)
    if (tutorialPhase == TUTE_COMPLETE)
    {
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
    }

    // ---- Single call that prints ALL tutorial text ----
    Tutorial_DrawText();

    if (tutIsPaused) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEMtx33 s, t, m;

        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.6f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, 1600.0f, 900.0f);
        AEMtx33Trans(&t, 0.0f, 0.0f);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        float panelCX = 0.0f, panelCY = 0.0f;
        float panelW = 400.0f, panelH = 420.0f, panelR = 30.0f;
        float titleH = 70.0f;
        float titleBarTop = panelCY + panelH * 0.5f;
        float titleBarBottom = titleBarTop - titleH;
        float titleBarCY = (titleBarTop + titleBarBottom) * 0.5f;
        float btnW = 300.0f, btnH = 55.0f, btnR = 28.0f;

        TutPauseResumeBtnX = panelCX; TutPauseResumeBtnY = 55.0f;
        TutPauseMenuBtnX = panelCX; TutPauseMenuBtnY = -30.0f;
        TutPauseSettBtnX = panelCX; TutPauseSettBtnY = -100.0f;

        DrawRoundedRect(BGMesh, PlateMesh,
            panelCX, panelCY, panelW, panelH, panelR,
            0.98f, 0.96f, 0.90f);

        AEGfxSetColorToMultiply(0.85f, 0.31f, 0.055f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, panelW - panelR * 2.0f, titleH);
        AEMtx33Trans(&t, panelCX, titleBarCY);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Scale(&s, panelR * 2.0f, titleH - panelR);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Scale(&s, panelR / 2.0f, panelR / 2.0f);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR, panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR, panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);

        DrawRoundedRect(BGMesh, PlateMesh, panelCX, TutPauseResumeBtnY, btnW, btnH, btnR,
            0.85f, 0.31f, 0.055f);
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, TutPauseMenuBtnY, btnW, btnH, btnR,
            0.17f, 0.17f, 0.17f);
        DrawRoundedRect(BGMesh, PlateMesh, panelCX, TutPauseSettBtnY, btnW, btnH, btnR,
            0.17f, 0.17f, 0.17f);

        if (FontId >= 0) {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            AEGfxPrint(FontId, "PAUSED",
                -0.10f, (titleBarCY - 14.0f) / 450.0f,
                1.8f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(FontId, "Resume",
                -0.06f, (TutPauseResumeBtnY - 10.0f) / 450.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(FontId, "Back to Main Menu",
                -0.13f, (TutPauseMenuBtnY - 10.0f) / 450.0f,
                0.85f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(FontId, "Settings",
                -0.07f, (TutPauseSettBtnY - 10.0f) / 450.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    Settings_Draw(FontId, BGMesh, CircleMesh);
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
    tutIsPaused = false;
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
    if (PlateMesh) { AEGfxMeshFree(PlateMesh); PlateMesh = nullptr; }
    if (CircleMesh) { AEGfxMeshFree(CircleMesh); CircleMesh = nullptr; }

    GarageDoor_Unload();
    Points_Unload();
    Sound_Unload();
    Settings_Unload();
}