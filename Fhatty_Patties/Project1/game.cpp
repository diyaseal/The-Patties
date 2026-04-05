// ---------------------------------------------------------------------------
// game.cpp
// Description: Core gameplay state — burger assembly, customer serving,
//              drag-and-drop, soda machine, pause menu.
// ---------------------------------------------------------------------------

#include "pch.hpp"

// customers is defined in Customer.cpp
extern std::vector<Customer> customers;

// ---------------------------------------------------------------------------
// Ingredient hitbox — loaded from Assets/Data/Ingredients.csv
// CSV format (header row):  name,x,y,width,height
// Falls back to kDefaultHitboxes if the file cannot be opened.
// ---------------------------------------------------------------------------

struct IngredientHitbox {
    IngredientType type;
    float x, y, width, height;
};

static std::vector<IngredientHitbox> ingredientHitboxes;

static IngredientType IngredientNameToType(const std::string& name) {
    if (name == "top_bun")    return TOP_BUN;
    if (name == "bottom_bun") return BOTTOM_BUN;
    if (name == "patty")      return PATTY;
    if (name == "cheese")     return CHEESE;
    if (name == "lettuce")    return LETTUCE;
    if (name == "tomato")     return TOMATO;
    if (name == "onion")      return ONION;
    if (name == "cucumber")   return CUCUMBER;
    return PATTY;
}

static void LoadIngredientHitboxes() {
    ingredientHitboxes.clear();

    std::ifstream file("Assets/Data/Ingredients.csv");
    if (!file.is_open()) {
        // ingredients.csv is required — place it at Assets/Data/Ingredients.csv
        printf("[game] ERROR: Assets/Data/Ingredients.csv not found. No ingredients will be clickable.\n");
        return;
    }

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string name, sx, sy, sw, sh;
        if (!std::getline(ss, name, ',')) continue;
        if (!std::getline(ss, sx, ',')) continue;
        if (!std::getline(ss, sy, ',')) continue;
        if (!std::getline(ss, sw, ',')) continue;
        if (!std::getline(ss, sh, ',')) continue;

        IngredientHitbox hb;
        hb.type = IngredientNameToType(name);
        hb.x = std::stof(sx);
        hb.y = std::stof(sy);
        hb.width = std::stof(sw);
        hb.height = std::stof(sh);
        ingredientHitboxes.push_back(hb);
    }
#ifdef _DEBUG
    printf("[game] Loaded %zu ingredient hitboxes\n", ingredientHitboxes.size());
#endif
}

// ---------------------------------------------------------------------------
// Game config — loaded from Assets/Data/GameConfig.csv
// CSV format (header row):  key,value
// ---------------------------------------------------------------------------

struct GameConfig {
    float plateCenterX = 290.0f;
    float plateCenterY = -210.0f;
    float plateHitW = 300.0f;
    float plateHitH = 300.0f;
    float sodaX = 574.0f;
    float sodaY = -88.0f;
    float sodaW = 70.0f;
    float sodaH = 70.0f;
    float sodaFillTime = 3.0f;
    float trashX = 629.0f;
    float trashY = -374.0f;
    float trashW = 250.0f;
    float trashH = 250.0f;
    float stackBaseX = 300.0f;
    float stackBaseY = -175.0f;
    float ingredientSize = 150.0f;
};

static GameConfig gCfg;

static void LoadGameConfig() {
    std::ifstream file("Assets/Data/GameConfig.csv");
    if (!file.is_open()) {
        // game_config.csv is required — place it at Assets/Data/GameConfig.csv
        printf("[game] ERROR: Assets/Data/GameConfig.csv not found. Using struct defaults.\n");
        return;
    }

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string key, val;
        if (!std::getline(ss, key, ',')) continue;
        if (!std::getline(ss, val, ',')) continue;
        float v = std::stof(val);

        if (key == "plate_center_x")  gCfg.plateCenterX = v;
        else if (key == "plate_center_y")  gCfg.plateCenterY = v;
        else if (key == "plate_hit_w")     gCfg.plateHitW = v;
        else if (key == "plate_hit_h")     gCfg.plateHitH = v;
        else if (key == "soda_x")          gCfg.sodaX = v;
        else if (key == "soda_y")          gCfg.sodaY = v;
        else if (key == "soda_w")          gCfg.sodaW = v;
        else if (key == "soda_h")          gCfg.sodaH = v;
        else if (key == "soda_fill_time")  gCfg.sodaFillTime = v;
        else if (key == "trash_x")         gCfg.trashX = v;
        else if (key == "trash_y")         gCfg.trashY = v;
        else if (key == "trash_w")         gCfg.trashW = v;
        else if (key == "trash_h")         gCfg.trashH = v;
        else if (key == "stack_base_x")    gCfg.stackBaseX = v;
        else if (key == "stack_base_y")    gCfg.stackBaseY = v;
        else if (key == "ingredient_size") gCfg.ingredientSize = v;
    }
#ifdef _DEBUG
    printf("[game] game_config.csv loaded\n");
#endif
}

// ---------------------------------------------------------------------------
// Ingredient render table — drives draw order and sprite sheet UVs.
// Matches the 4x2 sprite sheet layout used by the Ingredients texture.
// To add a new ingredient: add one row here + one row in ingredients.csv.
// ---------------------------------------------------------------------------

struct IngredientRenderInfo {
    IngredientType type;
    float texU;        // normalised U offset into sprite sheet
    float texV;        // normalised V offset into sprite sheet
    float stackHeight; // pixels this ingredient adds to the stack
};

static const IngredientRenderInfo kRenderOrder[] = {
    { BOTTOM_BUN, 3.f / 4.f, 1.f / 2.f, 15.0f },
    { LETTUCE,    2.f / 4.f, 0.f,         8.0f },
    { CHEESE,     0.f,       1.f / 2.f,  10.0f },
    { PATTY,      0.f,       0.f,        25.0f },
    { TOMATO,     1.f / 4.f, 0.f,        18.0f },
    { CUCUMBER,   2.f / 4.f, 1.f / 2.f,  8.0f },
    { ONION,      1.f / 4.f, 1.f / 2.f,  8.0f },
    { TOP_BUN,    3.f / 4.f, 0.f,         0.0f },
};


// ---------------------------------------------------------------------------
// Meshes & textures
// ---------------------------------------------------------------------------

static AEGfxVertexList* SquareMesh = nullptr;
static AEGfxVertexList* PlateMesh = nullptr;
static AEGfxVertexList* BGMesh = nullptr;
static AEGfxVertexList* IngredientsMesh = nullptr;
static AEGfxVertexList* SodaMesh = nullptr;

AEGfxTexture* Ingredients = nullptr;
AEGfxTexture* GameplayBG_back = nullptr;
AEGfxTexture* GameplayBG_front = nullptr;
AEGfxTexture* CustomerTexture[4] = { nullptr };
AEGfxTexture* PlateTexture = nullptr;
AEGfxTexture* SodaTexture = nullptr;
AEGfxTexture* LockTexture = nullptr;

// Reused transform matrices — declared once to avoid per-frame stack allocation
static AEMtx33 BGTransform, BGScale, BGTranslate;
static AEMtx33 IngTransform, IngScale, IngTranslate;

// ---------------------------------------------------------------------------
// Burger / plate state
// ---------------------------------------------------------------------------

static std::vector<int> currentBurger;
static bool             burgerComplete = false;

// Indexed by IngredientType (0–8) — replaces 8 separate bool variables
static bool ingredientOnPlate[9] = {};

// ---------------------------------------------------------------------------
// Drag-and-drop state  (Syakil)
// ---------------------------------------------------------------------------

static bool  isDragging = false;
static float dragOffsetX = 0.0f;
static float dragOffsetY = 0.0f;
static float burgerDragX = 0.0f;
static float burgerDragY = 0.0f;
static bool  burgerIsBeingDragged = false;

// ---------------------------------------------------------------------------
// Soda machine state  (Diya)
// ---------------------------------------------------------------------------

static bool  sodaMachineHasCup = false;
static bool  sodaIsFilled = false;
static bool  sodaIsBeingDragged = false;
static float sodaFillTimer = 0.0f;
static float sodaDragX = 0.0f;
static float sodaDragY = 0.0f;

// ---------------------------------------------------------------------------
// Level end flag
// ---------------------------------------------------------------------------

static bool levelEndTriggered = false;

// ---------------------------------------------------------------------------
// Pause menu  (Teb)
// ---------------------------------------------------------------------------

static s8   GameFontId = -1;
static s8   GameFontIdSmall = -1;
static bool isPaused = false;

// Button rects — set once in Draw, read in Update (single source of truth)
struct BtnRect { float cx, cy, w, h; };
static BtnRect btnResume, btnRestart, btnMenu, btnSettings;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// AABB point-in-rect
static bool PointInRect(float px, float py,
    float cx, float cy, float w, float h) {
    return px >= cx - w * 0.5f && px <= cx + w * 0.5f
        && py >= cy - h * 0.5f && py <= cy + h * 0.5f;
}

// Draw a single textured quad
static void DrawQuad(AEGfxVertexList* mesh, AEGfxTexture* tex,
    float cx, float cy, float w, float h,
    float u = 0.f, float v = 0.f) {
    AEMtx33 s, t, m;
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
    AEGfxTextureSet(tex, u, v);
    AEMtx33Scale(&s, w, h);
    AEMtx33Trans(&t, cx, cy);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

// Draw lock icon over a locked ingredient slot  (Diya)
static void DrawLockOverlay(float x, float y, float w, float h) {
    if (!LockTexture || !SquareMesh) return;
    DrawQuad(SquareMesh, LockTexture, x, y, w, h);
}

// Is this ingredient unlocked for the current level?
// Reads currentLevelData.ingredients set by LevelManager — no changes needed there.
static bool IsIngredientUnlocked(IngredientType type) {
    // Indexed to match the IngredientType enum order in pch.hpp
    static const char* kNames[] = {
        "bottom_bun", "top_bun", "patty", "cheese",
        "lettuce",    "tomato",  "onion", "cucumber", "soda"
    };
    const char* target = kNames[type];
    for (const auto& ing : currentLevelData.ingredients)
        if (ing == target) return true;
    return false;
}

// Does an order contain soda?  (Diya)
static bool OrderContainsSoda(const std::vector<int>& order) {
    for (int item : order)
        if (item == SODA) return true;
    return false;
}

// Reset drag/drop state back to resting positions  (Syakil)
static void ResetDragState() {
    isDragging = false;
    burgerIsBeingDragged = false;
    burgerDragX = gCfg.plateCenterX;
    burgerDragY = gCfg.plateCenterY;
    sodaIsBeingDragged = false;
    sodaDragX = gCfg.sodaX;
    sodaDragY = gCfg.sodaY - 55.0f;
}

// ---------------------------------------------------------------------------
// Burger logic
// ---------------------------------------------------------------------------

bool Burger_MatchesOrder(const std::vector<int>& playerBurger,
    const std::vector<int>& customerOrder) {
    bool playerHasSoda = OrderContainsSoda(playerBurger);
    bool customerWantsSoda = OrderContainsSoda(customerOrder);
    if (playerHasSoda != customerWantsSoda) return false;

    std::vector<int> filteredPlayer, filteredCustomer;
    for (int i : playerBurger)  if (i != SODA) filteredPlayer.push_back(i);
    for (int i : customerOrder) if (i != SODA) filteredCustomer.push_back(i);

    if (filteredPlayer.size() != filteredCustomer.size()) return false;
    for (size_t i = 0; i < filteredCustomer.size(); ++i)
        if (filteredPlayer[i] != filteredCustomer[i]) return false;

    return true;
}

void Burger_Clear() {
    currentBurger.clear();
    burgerComplete = false;

    // (D) Remember whether soda was actually on the tray before clearing
    bool sodaWasOnTray = ingredientOnPlate[SODA];

    // Clear only tray / plate ingredients
    std::fill(std::begin(ingredientOnPlate), std::end(ingredientOnPlate), false);

    // (D) Only clear machine soda if soda had already been placed on the tray
    if (sodaWasOnTray) {
        sodaMachineHasCup = false;
        sodaIsFilled = false;
        sodaFillTimer = 0.0f;
    }

    ResetDragState();

#ifdef _DEBUG
    printf("[game] Burger cleared\n");
#endif
}

// ---------------------------------------------------------------------------
// Game State Functions
// ---------------------------------------------------------------------------

void Game_Init() {
    Level_Manager_Init();
    Customer_Init();
    Points_Init();
    GarageDoor_Init();
    levelEndTriggered = false;

    // Full soda reset on level start
    sodaMachineHasCup = false;
    sodaIsFilled = false;
    sodaFillTimer = 0.0f;
    sodaIsBeingDragged = false;
    sodaDragX = gCfg.sodaX;
    sodaDragY = gCfg.sodaY - 55.0f;
}

void Game_Load() {
    // Load CSV configs first so gCfg is ready before anything uses it
    LoadGameConfig();
    LoadIngredientHitboxes();

    SquareMesh = CreateSquareMesh(8.0f, 2.5f);
    SodaMesh = CreateSodaMesh(0.5f, 0.5f);
    PlateMesh = CreateCircleMesh(2.0f, 32);
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);
    BGMesh = CreateSquareMesh(0.5f, 0.5f);

    Points_Load();
    GarageDoor_Load();
    Sound_Load();
    Sound_PlayBGM();
    Settings_Load();

    GameFontId = AEGfxCreateFont("Assets/Font/Fredoka-Bold.ttf", 30);
    GameFontIdSmall = AEGfxCreateFont("Assets/Font/Nunito-Bold.ttf", 24);
}

// ---------------------------------------------------------------------------
// Game_Update
// ---------------------------------------------------------------------------

void Game_Update() {
    const float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

    Settings_Update(dt, GameFontId);
    if (Settings_IsOpen()) return;

    if (AEInputCheckTriggered(AEVK_ESCAPE))
        isPaused = !isPaused;

    // -----------------------------------------------------------------------
    // Pause menu input — blocks all gameplay while paused  (Teb)
    // -----------------------------------------------------------------------
    if (isPaused) {
        s32 mx, my;
        AEInputGetCursorPosition(&mx, &my);
        const float hw = (AEGfxGetWinMaxX() - AEGfxGetWinMinX()) * 0.5f;
        const float hh = (AEGfxGetWinMaxY() - AEGfxGetWinMinY()) * 0.5f;
        const float wx = (float)mx - hw;
        const float wy = -(float)my + hh;

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (PointInRect(wx, wy, btnResume.cx, btnResume.cy, btnResume.w, btnResume.h))
                isPaused = false;
            if (PointInRect(wx, wy, btnRestart.cx, btnRestart.cy, btnRestart.w, btnRestart.h))
            {
                isPaused = false; Burger_Clear(); next = GS_LOADING;
            }
            if (PointInRect(wx, wy, btnMenu.cx, btnMenu.cy, btnMenu.w, btnMenu.h))
            {
                isPaused = false; next = GS_MAINMENU;
            }
            if (PointInRect(wx, wy, btnSettings.cx, btnSettings.cy, btnSettings.w, btnSettings.h))
                Settings_Open();
        }
        return;
    }

    // -----------------------------------------------------------------------
    // Normal gameplay
    // -----------------------------------------------------------------------

    Customer_Update(dt);
    Points_Update();

    // Soda fill timer  (Diya)
    if (sodaMachineHasCup && !sodaIsFilled) {
        sodaFillTimer += dt;
        if (sodaFillTimer >= gCfg.sodaFillTime) {
            sodaFillTimer = gCfg.sodaFillTime;
            sodaIsFilled = true;
            Sound_StopSFX_Pour();
        }
    }

    // Mouse -> world space
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    const float winW = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
    const float winH = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
    const float worldX = (float)mx - winW * 0.5f;
    const float worldY = -(float)my + winH * 0.5f;

    // -----------------------------------------------------------------------
    // Mouse click
    // -----------------------------------------------------------------------
    if (AEInputCheckTriggered(AEVK_LBUTTON)) {
#ifdef _DEBUG
        printf("[game] Click at world (%.2f, %.2f)\n", worldX, worldY);
#endif
        // Ingredient hitboxes — one loop replaces 8 separate if-blocks
        for (const auto& hb : ingredientHitboxes) {
            if (hb.type == SODA)                                          continue;
            if (!IsIngredientUnlocked(hb.type))                           continue;
            if (ingredientOnPlate[hb.type])                               continue;
            if (!PointInRect(worldX, worldY, hb.x, hb.y, hb.width, hb.height)) continue;

            ingredientOnPlate[hb.type] = true;
            currentBurger.push_back(hb.type);
            if (hb.type == TOP_BUN) burgerComplete = true;

#ifdef _DEBUG
            printf("[game] Added ingredient %d — stack size %zu\n",
                hb.type, currentBurger.size());
#endif
            break; // only one ingredient per click
        }

        // Soda machine button  (Diya)
        if (IsIngredientUnlocked(SODA)
            && !ingredientOnPlate[SODA]
            && !sodaMachineHasCup
            && !sodaIsBeingDragged
            && PointInRect(worldX, worldY, gCfg.sodaX, gCfg.sodaY, gCfg.sodaW, gCfg.sodaH)) {

            sodaMachineHasCup = true;
            sodaIsFilled = false;
            sodaFillTimer = 0.0f;
            sodaDragX = gCfg.sodaX;
            sodaDragY = gCfg.sodaY - 55.0f;
            Sound_PlaySFX_Pour();
        }

        // Snap filled soda onto the plate  (Diya)
        if (sodaMachineHasCup && sodaIsFilled
            && PointInRect(worldX, worldY, sodaDragX, sodaDragY, 70.0f, 90.0f)) {

            ingredientOnPlate[SODA] = true;
            sodaMachineHasCup = false;
            sodaIsFilled = false;
            sodaFillTimer = 0.0f;
            sodaIsBeingDragged = false;
            currentBurger.push_back(SODA);
        }

        // Start dragging the plate/burger  (Syakil)
        if (!currentBurger.empty()
            && PointInRect(worldX, worldY,
                gCfg.plateCenterX, gCfg.plateCenterY,
                gCfg.plateHitW, gCfg.plateHitH)) {

            isDragging = true;
            burgerIsBeingDragged = true;
            dragOffsetX = worldX - gCfg.plateCenterX;
            dragOffsetY = worldY - gCfg.plateCenterY;
            burgerDragX = worldX;
            burgerDragY = worldY;
        }
    }

    // Continuous drag update
    if (isDragging && burgerIsBeingDragged) {
        AEInputGetCursorPosition(&mx, &my);
        burgerDragX = (float)mx - winW * 0.5f;
        burgerDragY = -(float)my + winH * 0.5f;
    }

    // Mouse release  (Syakil)
    if (AEInputCheckReleased(AEVK_LBUTTON) && isDragging && burgerIsBeingDragged) {
        isDragging = false;

        if (PointInRect(burgerDragX, burgerDragY,
            gCfg.trashX, gCfg.trashY, gCfg.trashW, gCfg.trashH)) {
            // Dropped in trash
            Burger_Clear();
        }
        else {
            // Check if dropped on a customer
            bool served = false;
            for (int ci = 0; ci < (int)customers.size(); ++ci) {
                Customer& c = customers[ci];
                if (!c.isWaitingForFood) continue;

                float dx = burgerDragX - c.x;
                float dy = burgerDragY - c.y;
                if (dx * dx + dy * dy > 150.0f * 150.0f) continue;

                if (Burger_MatchesOrder(currentBurger, c.order)) {
                    Customer_Satisfy(ci);
                    Points_AddPoints(true);
                }
                else {
                    c.patience -= 30.0f;
                    if (c.patience < 0.0f) c.patience = 0.0f;
                }
                Burger_Clear();
                served = true;
                break;
            }

#ifdef _DEBUG
            if (!served) printf("[game] Burger returned to plate\n");
#endif
        }

        burgerIsBeingDragged = false;
    }

    // R key — clear burger
    if (AEInputCheckTriggered(AEVK_R))
        Burger_Clear();

    // Level completion check
    if (!levelEndTriggered && Customer_IsLevelComplete()) {
        levelEndTriggered = true;
        GarageDoor_Trigger();
    }
    GarageDoor_Update(dt);

#ifdef _DEBUG
    if (AEInputCheckTriggered(AEVK_G)) {
        levelEndTriggered = true;
        GarageDoor_Trigger();
        printf("[game] DEBUG: garage door triggered\n");
    }
#endif
}

// ---------------------------------------------------------------------------
// Game_Draw
// ---------------------------------------------------------------------------

void Game_Draw() {
    AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    // Back BG layer
    DrawQuad(BGMesh, GameplayBG_back, 0.f, 0.f, 1600.f, 900.f);

    // Customers drawn before front BG so they appear behind the counter
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    Customer_Draw(CustomerTexture);

    // Front BG layer
    DrawQuad(BGMesh, GameplayBG_front, 0.f, 0.f, 1600.f, 900.f);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    Points_Draw();

    // Lock overlays  (Diya)
    for (const auto& hb : ingredientHitboxes)
        if (!IsIngredientUnlocked(hb.type))
            DrawLockOverlay(hb.x, hb.y, 8.f, 8.f);
    if (!IsIngredientUnlocked(SODA))
        DrawLockOverlay(gCfg.sodaX - 13.f, gCfg.sodaY + 28.f, 8.f, 8.f);

    // Soda cup at the machine while filling or ready  (Diya)
    if (sodaMachineHasCup && SodaTexture) {
        float drawSodaX = sodaIsBeingDragged ? sodaDragX : gCfg.sodaX;
        float drawSodaY = sodaIsBeingDragged ? sodaDragY : gCfg.sodaY - 55.f;
        DrawQuad(SodaMesh, SodaTexture,
            drawSodaX, drawSodaY, 70.f, 110.f,
            sodaIsFilled ? 0.5f : 0.f, 0.f);
    }

    // Burger draw origin
    const float drawX = burgerIsBeingDragged ? burgerDragX : gCfg.stackBaseX;
    const float drawY = burgerIsBeingDragged ? burgerDragY : gCfg.stackBaseY;

    // Plate image underneath the burger
    if (PlateTexture)
        DrawQuad(SquareMesh, PlateTexture, drawX, drawY - 20.f, 20.f, 20.f);

    // Ingredient stack — single loop driven by kRenderOrder table
    float stackH = 0.f;
    for (const auto& ri : kRenderOrder) {
        if (!ingredientOnPlate[ri.type]) continue;

        if (ri.type == TOP_BUN) stackH -= 2.f; // slight burger compression

        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.f, 1.f, 1.f, 1.f);
        AEGfxTextureSet(Ingredients, ri.texU, ri.texV);

        float yOffset = 0.f;
        if (ri.type == BOTTOM_BUN) yOffset = -10.f;
        if (ri.type == ONION) yOffset = -15.f;
        AEMtx33Scale(&IngScale, gCfg.ingredientSize, gCfg.ingredientSize);
        AEMtx33Trans(&IngTranslate, drawX, drawY + yOffset + stackH);
        AEMtx33Concat(&IngTransform, &IngTranslate, &IngScale);
        AEGfxSetTransform(IngTransform.m);
        AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);

        stackH += ri.stackHeight;
    }

    // Soda beside the burger once placed on plate  (Diya)
    if (ingredientOnPlate[SODA] && SodaTexture)
        DrawQuad(SodaMesh, SodaTexture,
            drawX + 90.f, drawY + 25.f, 70.f, 110.f, 0.5f, 0.f);

    GarageDoor_Draw();

    // -----------------------------------------------------------------------
    // Pause menu overlay  (Teb)
    // -----------------------------------------------------------------------
    if (!isPaused) {
        Settings_Draw(GameFontId, BGMesh, PlateMesh);
        return;
    }

    // Dim backdrop
    {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetColorToMultiply(0.f, 0.f, 0.f, 0.6f);
        AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
        AEMtx33 s, t, m;
        AEMtx33Scale(&s, 1600.f, 900.f);
        AEMtx33Trans(&t, 0.f, 0.f);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);
    }

    // Panel constants
    const float panelCX = 0.f, panelCY = 0.f;
    const float panelW = 400.f, panelH = 560.f, panelR = 30.f;
    const float titleH = 70.f;
    const float btnW = 300.f, btnH = 55.f, btnR = 28.f;

    // Sync button rects for Update hit-testing
    btnResume = { panelCX, panelCY + 110.f, btnW, btnH };
    btnRestart = { panelCX, panelCY + 30.f, btnW, btnH };
    btnMenu = { panelCX, panelCY - 55.f, btnW, btnH };
    btnSettings = { panelCX, panelCY - 140.f, btnW, btnH };

    // Cream panel
    DrawRoundedRect(BGMesh, PlateMesh,
        panelCX, panelCY, panelW, panelH, panelR,
        0.98f, 0.96f, 0.90f);

    // Orange title bar
    {
        const float titleBarTop = panelCY + panelH * 0.5f;
        const float titleBarCY = titleBarTop - titleH * 0.5f;
        AEMtx33 s, t, m;
        AEGfxSetColorToMultiply(0.85f, 0.31f, 0.055f, 1.f);
        AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);

        AEMtx33Scale(&s, panelW - panelR * 2.f, titleH);
        AEMtx33Trans(&t, panelCX, titleBarCY);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Scale(&s, panelR * 2.f, titleH - panelR);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);
        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR, titleBarCY - panelR * 0.5f);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        AEMtx33Scale(&s, panelR * 0.5f, panelR * 0.5f);
        AEMtx33Trans(&t, panelCX - panelW * 0.5f + panelR, panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);
        AEMtx33Trans(&t, panelCX + panelW * 0.5f - panelR, panelCY + panelH * 0.5f - panelR);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(PlateMesh, AE_GFX_MDM_TRIANGLES);
    }

    // Buttons
    DrawRoundedRect(BGMesh, PlateMesh,
        btnResume.cx, btnResume.cy, btnResume.w, btnResume.h,
        btnR, 0.85f, 0.31f, 0.055f);
    DrawRoundedRect(BGMesh, PlateMesh,
        btnRestart.cx, btnRestart.cy, btnRestart.w, btnRestart.h,
        btnR, 0.17f, 0.17f, 0.17f);
    DrawRoundedRect(BGMesh, PlateMesh,
        btnMenu.cx, btnMenu.cy, btnMenu.w, btnMenu.h,
        btnR, 0.17f, 0.17f, 0.17f);
    DrawRoundedRect(BGMesh, PlateMesh,
        btnSettings.cx, btnSettings.cy, btnSettings.w, btnSettings.h,
        btnR, 0.17f, 0.17f, 0.17f);

    // Text
    if (GameFontId >= 0) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        const float titleBarCY = panelCY + panelH * 0.5f - titleH * 0.5f;
        s8 smallFont = (GameFontIdSmall >= 0) ? GameFontIdSmall : GameFontId;

        AEGfxPrint(GameFontId, "PAUSED",
            -0.13f, (titleBarCY - 14.f) / 450.f, 1.8f, 1.f, 1.f, 1.f, 1.f);
        AEGfxPrint(smallFont, "Press ESC to Resume",
            -0.10f, (titleBarCY - titleH * 0.5f - 30.f) / 450.f, 0.75f, 0.5f, 0.5f, 0.5f, 1.f);
        AEGfxPrint(GameFontId, "Resume",
            -0.07f, (btnResume.cy - 10.f) / 450.f, 1.f, 1.f, 1.f, 1.f, 1.f);
        AEGfxPrint(GameFontId, "Restart",
            -0.07f, (btnRestart.cy - 10.f) / 450.f, 1.f, 1.f, 1.f, 1.f, 1.f);
        AEGfxPrint(GameFontId, "Back to Main Menu",
            -0.15f, (btnMenu.cy - 10.f) / 450.f, 0.85f, 1.f, 1.f, 1.f, 1.f);
        AEGfxPrint(GameFontId, "Settings",
            -0.08f, (btnSettings.cy - 10.f) / 450.f, 1.f, 1.f, 1.f, 1.f, 1.f);
    }

    Settings_Draw(GameFontId, BGMesh, PlateMesh);
}

// ---------------------------------------------------------------------------
// Cleanup
// ---------------------------------------------------------------------------

void Game_Free() {
    Burger_Clear();
    Customer_Free();
    Points_Free();
    Level_Manager_Free();
    GarageDoor_Free();
    Sound_Free();
}

void Game_Unload() {
    auto UnloadTex = [](AEGfxTexture*& tex) {
        if (tex) { AEGfxTextureUnload(tex); tex = nullptr; }
        };
    auto FreeMesh = [](AEGfxVertexList*& m) {
        if (m) { AEGfxMeshFree(m); m = nullptr; }
        };

    UnloadTex(Ingredients);
    UnloadTex(SodaTexture);
    UnloadTex(LockTexture);
    UnloadTex(PlateTexture);
    UnloadTex(GameplayBG_back);
    UnloadTex(GameplayBG_front);
    for (auto& t : CustomerTexture) UnloadTex(t);

    FreeMesh(SquareMesh);
    FreeMesh(PlateMesh);
    FreeMesh(IngredientsMesh);
    FreeMesh(BGMesh);
    FreeMesh(SodaMesh);

    if (GameFontId >= 0) { AEGfxDestroyFont(GameFontId);      GameFontId = -1; }
    if (GameFontIdSmall >= 0) { AEGfxDestroyFont(GameFontIdSmall); GameFontIdSmall = -1; }

    GarageDoor_Unload();
    Points_Unload();
    Level_Manager_Unload();
    Sound_Unload();
    Settings_Unload();
}