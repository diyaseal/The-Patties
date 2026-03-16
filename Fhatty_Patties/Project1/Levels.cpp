// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// Global variables for Level
// ---------------------------------------------------------------------------
static float totalElapsedTime = 0.0f;            // Timer for screen duration
static s8 fontId = -1;                           // Font ID
// ---------------------------------------------------------------------------
static AEGfxTexture* LevelBG = nullptr;
static AEGfxVertexList* LevelMesh = nullptr;
static AEMtx33 LevelTransform, LevelTranslate, LevelScale;
// ---------------------------------------------------------------------------
static AEGfxTexture* Ingredients = nullptr;
static AEGfxVertexList* IngredientsMesh = nullptr;
static LevelButton  levelButtons[5];
static AEGfxTexture* Lock = nullptr;
static AEGfxVertexList* LockMesh = nullptr;

// Button dimensions
const float BUTTON_WIDTH = 250.0f;
const float BUTTON_HEIGHT = 250.0f;
const float BUTTON_SPACING = 300.0f;
const float START_X = -600.0f;
const float START_Y = 0.f;
// ---------------------------------------------------------------------------
// Star rating textures and mesh
static AEGfxTexture* starEmptyTex = nullptr;
static AEGfxTexture* starFilledTex = nullptr;
static AEGfxVertexList* starMesh = nullptr;

// Star transformation matrices
static AEMtx33 TempTransform, StarTransform, StarScale, StarRotate, StarTranslate;  
// ---------------------------------------------------------------------------
static AEGfxTexture* Wall = nullptr;
static AEMtx33 WallTransform, WallTranslate, WallScale;
static float WallPosY = 0.0f;
static float SlideSpeed = 500.0f;
static AEGfxVertexList* WallMesh = nullptr;
static float WallTimer = 0.0f;

//Sound
static bool pendingStateChange = false;
static int  pendingNextState = 0;
static float stateChangeTimer = 0.0f;
static const float STATE_CHANGE_DELAY = 0.3f;

// Game State Functions
// ---------------------------------------------------------------------------
void Levels_Load()
{
    //Load BG
    LevelMesh = CreateSquareMesh(0.5f, 0.5f);
    LevelBG = AEGfxTextureLoad("Assets/Assets/level_page.png");
    if (LevelBG)
        printf("LevelBG texture loaded successfully\n");
    else
        printf("ERROR: Failed to load LevelBG!\n");

    //Load font
    fontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (fontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", fontId);

    //Create a button mesh
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);
    starMesh = CreateStarMesh(0.5f, 0.2f);  // Outer radius, inner radius

    // Load ingredient textures (you'll need to create these assets)
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");

    for (int i = 0; i < 5; i++) {
        levelButtons[i].ingredientName = INGREDIENT_DATA[i].name;
    }

    //Load Lock
    LockMesh = CreateSquareMesh(0.5f, 0.5f);
    Lock = AEGfxTextureLoad("Assets/Assets/lock.png");
    if (LevelBG)
        printf("Lock texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Lock!\n");

    // Load Wall
    Wall = AEGfxTextureLoad("Assets/Assets/Transition_wall.png");
    WallMesh = CreateSquareMesh(0.5, 0.5);

    if (Wall)
        printf("Wall texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Transition_wall.png\n");

    //Load Sound
    Sound_Load();
}

void Levels_Init()
{
    totalElapsedTime = 0.0f;
    // Initialize level buttons in a grid layout
    for (int i = 0; i < 5; i++) {
        levelButtons[i].button.x = START_X + (i * BUTTON_SPACING);
        levelButtons[i].button.y = START_Y;
        levelButtons[i].button.width = BUTTON_WIDTH;
        levelButtons[i].button.height = BUTTON_HEIGHT;
        levelButtons[i].button.isHovered = false;
        levelButtons[i].button.isClicked = false;

        //ELI OVER HERE U CAN INPUT SAVED DATA STUFF
        // Demo stars (replace with actual saved data)
        Level_Manager_LoadLevel(i + 1);
        levelButtons[i].starsEarned = Level_Manager_GetStarsEarned();

        // Level 1: 3 stars, Level 2: 2 stars, Level 3: 1 star, etc.
        //levelButtons[i].starsEarned = 3 - i;
        //if (levelButtons[i].starsEarned < 0) levelButtons[i].starsEarned = 0;

        // First 3 levels unlocked, last 2 locked for demo
        levelButtons[i].Unlocked = (i < 3);
    }

    totalElapsedTime = 0.0f;
    WallTimer = 0.0f;  
    WallPosY = 0.0f;  
    pendingStateChange = false;
    pendingNextState = 0;
    stateChangeTimer = 0.0f;
}

void Levels_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    if (WallTimer < 1.0f)
        WallTimer += dt;

    // After 1 second, slide wall upward off screen
    if (WallTimer >= 1.0f && WallPosY < 900.0f)
    {
        WallPosY += SlideSpeed * dt;
        if (WallPosY >= 900.0f)
            WallPosY = 900.0f;  // clamp
    }

    // Check each level button
    for (int i = 0; i < 5; i++) {
        // ELI this line skips locked buttons so the click can only work for unlocked levels
        // if u need to edit base on your code msg me I wanna understand what you do
        if (!levelButtons[i].Unlocked) continue;  // Skip locked levels

        // Check hover state
        levelButtons[i].button.isHovered = IsButtonHovered(levelButtons[i].button);

        // Check if button is clicked
        if (IsButtonClicked(levelButtons[i].button)) {
            Sound_PlaySFX_OnClick();
            levelButtons[i].button.isClicked = true;
            // ELI also over here is the place to go to the next game state based on what is clicked when u wanna edit
            // go through with me too I wanna understand
            currentLevel = i + 1;  // 1-based level number
            pendingStateChange = true;      // ← don't switch immediately
            pendingNextState = GS_LOADING; 

            
        }
    }

    // Handle delayed state change so click sound can finish
    if (pendingStateChange) {
        stateChangeTimer += dt;
        if (stateChangeTimer >= STATE_CHANGE_DELAY) {
            next = pendingNextState;
            pendingStateChange = false;
            stateChangeTimer = 0.0f;
            next = GS_LOADING;  // Go to game
        }
    }
}

void Levels_Draw()
{
    // LevelBG
    AEMtx33Scale(&LevelScale, 1600, 900);
    AEMtx33Trans(&LevelTranslate, 0, 0);
    AEMtx33Concat(&LevelTransform, &LevelTranslate, &LevelScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(LevelTransform.m);
    AEGfxTextureSet(LevelBG, 0.f, 0.f);
    AEGfxMeshDraw(LevelMesh, AE_GFX_MDM_TRIANGLES);

    // ---------------------------------------------------------------------------
    // Draw all level buttons
    for (int i = 0; i < 5; i++) {
        AEMtx33 scale, translate, transform;

        /*-----------------
             DRAW STARS
        -------------------*/

        const f32 star_size = 35.0f;
        const f32 starY = levelButtons[i].button.y - 60.0f;
        const f32 starX = levelButtons[i].button.x - 85.0f;
        const f32 starSpace = 40.0f;

        for (int j = 1; j <= 3; j++)
        {
            if (j <= levelButtons[i].starsEarned)  // colors all earned stars
                AEGfxSetColorToMultiply(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
            else
                AEGfxSetColorToMultiply(0.4f, 0.4f, 0.4f, 1.0f); // Gray

            AEMtx33Scale(&StarScale, star_size, star_size);
            AEMtx33Trans(&StarTranslate, starX + (j * starSpace), starY);
            AEMtx33Concat(&StarTransform, &StarTranslate, &StarScale);
            AEGfxSetTransform(StarTransform.m);
            AEGfxMeshDraw(starMesh, AE_GFX_MDM_TRIANGLES);
        }
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f); // reset color after stars

        // Draw ingredient icon (if unlocked)
        if (levelButtons[i].Unlocked && Ingredients) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEMtx33Scale(&scale, levelButtons[i].button.width, levelButtons[i].button.height);
            AEMtx33Trans(&translate, levelButtons[i].button.x, levelButtons[i].button.y + 15); // Shift up for stars
            AEMtx33Concat(&transform, &translate, &scale);
            AEGfxSetTransform(transform.m);
            AEGfxTextureSet(Ingredients, INGREDIENT_DATA[i].x, INGREDIENT_DATA[i].y);
            AEGfxMeshDraw(IngredientsMesh, AE_GFX_MDM_TRIANGLES);
        }
        else if (!levelButtons[i].Unlocked) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEMtx33Scale(&scale, levelButtons[i].button.width, levelButtons[i].button.height);
            AEMtx33Trans(&translate, levelButtons[i].button.x, levelButtons[i].button.y + 15); // Shift up for stars
            AEMtx33Concat(&transform, &translate, &scale);
            AEGfxSetTransform(transform.m);
            AEGfxTextureSet(Lock, INGREDIENT_DATA[i].x, INGREDIENT_DATA[i].y);
            AEGfxMeshDraw(LockMesh, AE_GFX_MDM_TRIANGLES);
            // Draw lock icon or text for locked levels
            if (fontId >= 0) {
                AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                AEGfxPrint(fontId, "LOCKED",
                    (levelButtons[i].button.x / 800),
                    (levelButtons[i].button.y / 450) - 0.05f,
                    0.6f, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        
        // Draw level number
        if (fontId >= 0 && levelButtons[i].Unlocked) {
            char levelText[16];
            sprintf_s(levelText, "Level %d", i + 1);
            float normX = (levelButtons[i].button.x / 800.0f) - 0.1f;
            float normY = (levelButtons[i].button.y / 450.0f) + 0.15f;
            if (normX >= -1.0f && normX <= 1.0f && normY >= -1.0f && normY <= 1.0f) {
                AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                AEGfxPrint(fontId, levelText, normX, normY, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
    }

    // Wall
    if (WallPosY < 900.0f)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEMtx33Scale(&WallScale, 1600.0f, 900.0f);
        AEMtx33Trans(&WallTranslate, 0.0f, WallPosY);
        AEMtx33Concat(&WallTransform, &WallTranslate, &WallScale);
        AEGfxSetTransform(WallTransform.m);
        AEGfxTextureSet(Wall, 0.f, 0.f);
        AEGfxMeshDraw(WallMesh, AE_GFX_MDM_TRIANGLES);
    }
}

void Levels_Free()
{

}

void Levels_Unload()
{
    // Free the button mesh
    if (IngredientsMesh)
    {
        AEGfxMeshFree(IngredientsMesh);
        IngredientsMesh = nullptr;
    }
    if (starMesh) {
        AEGfxMeshFree(starMesh);
        starMesh = nullptr;
    }
    if (LevelBG) {
        AEGfxTextureUnload(LevelBG);
        LevelBG = nullptr;
    }
    if (LevelMesh) {
        AEGfxMeshFree(LevelMesh);
        LevelMesh = nullptr;
    }
    if (fontId >= 0)
    {
        AEGfxDestroyFont(fontId);
        fontId = -1;
    }
    Sound_Unload();
}
