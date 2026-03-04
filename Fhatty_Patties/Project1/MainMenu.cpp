// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global variables for MainMenu

static float totalElapsedTime = 0.0f;            // Timer for screen duration
static s8 FontId = -1;                           // Font ID
// ---------------------------------------------------------------------------
// Using BurgerButton struct in hpp
static BurgerButton topBun;
static BurgerButton cheese;
static BurgerButton patty;
static BurgerButton lettuce;
static BurgerButton bottomBun;

// Array for easy iteration of the above variables
static BurgerButton* burgerButtons[] = { &bottomBun, &lettuce, &patty, &cheese, &topBun };
static const int ButtonCount = sizeof(BUTTON_DATA) / sizeof(BUTTON_DATA[0]);
static float buttonWidth = 850.0f, buttonHeight = 400.0f;
// ---------------------------------------------------------------------------
static AEGfxTexture* Wall = nullptr;
static AEMtx33 WallTransform, WallTranslate, WallScale;
static f32 WallPosY = 900;
static AEGfxVertexList* WallMesh = nullptr;
// --------------------------------------------------------------------------- 
// Game State Functions

void MainMenu_Load()
{
    // Load BG
    Wall = AEGfxTextureLoad("Assets/Assets/Transition_wall.png");
    WallMesh = SquareMesh(0.5, 0.5);

    //Load font
    FontId = AEGfxCreateFont("Assets/Font/Exo2-Regular.ttf", 40);
    if (FontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", FontId);

    // Load ingredient sprite sheet for button texture
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");

    if (Ingredients)
        printf("Ingredients texture loaded successfully\n");
    else 
        printf("ERROR: Failed to load Ingredients.png\n");

    // Assign the same mesh to all buttons (or create separate if needed)
    for (int i = 0; i < ButtonCount; i++) {
        burgerButtons[i]->mesh = IngredientMesh(0.5, 0.5);
    }
}

void MainMenu_Init()
{
    printf("Initialized");
    totalElapsedTime = 0.0f;

    // Button dimensions (adjust based on your asset sizes)
    float startY = -200.0f, spacing = 100.0f;

    for (int i = 0; i < ButtonCount; i++) {
        float yPos = startY + spacing * BUTTON_DATA[i].yOffset;

        burgerButtons[i]->texture = Ingredients;
        burgerButtons[i]->mesh = IngredientMesh(0.5, 0.5);
        burgerButtons[i]->name = BUTTON_DATA[i].name;
        burgerButtons[i]->x = 0.0f;
        burgerButtons[i]->y = yPos;
        burgerButtons[i]->targetY = yPos;
        burgerButtons[i]->width = buttonWidth;
        burgerButtons[i]->height = buttonHeight;
        burgerButtons[i]->floatOffset = 0.0f;
        burgerButtons[i]->floatSpeed = BUTTON_DATA[i].floatSpeed;
        burgerButtons[i]->floatAmount = BUTTON_DATA[i].floatAmount;
        burgerButtons[i]->isHovered = false;
        burgerButtons[i]->isClicked = false;
    }
}

void MainMenu_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();
    totalElapsedTime += dt;

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to WORLD coordinates (where 0,0 is screen center)
    float worldMouseX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
    float worldMouseY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

    if (WallPosY > 0)  // Only move if position is positive
    {
        WallPosY -= dt * 500.0f;  // Added speed multiplier for smoother movement
        if (WallPosY < 0)  // Clamp to prevent overshoot
            WallPosY = 0;
    }

    // Update each burger button
    for (int i = 0; i < ButtonCount; i++) {
        BurgerButton* btn = burgerButtons[i];

        // Update floating animation using sine wave
        btn->floatOffset = sinf(totalElapsedTime * btn->floatSpeed) * btn->floatAmount;

        // Check hover state
        float halfWidth = btn->width / 4.0f;
        float halfHeight = btn->height / 4.0f;
        float currentY = btn->targetY + btn->floatOffset;

        btn->isHovered = (worldMouseX >= btn->x - halfWidth &&
                          worldMouseX <= btn->x + halfWidth &&
                          worldMouseY >= currentY - halfHeight &&
                          worldMouseY <= currentY + halfHeight);

        // Check click
        if (btn->isHovered && AEInputCheckTriggered(AEVK_LBUTTON)) {
            btn->isClicked = true;

            // Handle button actions based on which burger part was clicked
            if (btn == &topBun) {
                printf("Top Bun clicked - Levels Menu!\n");
                next = GS_LEVELS;  // Or whatever state you want
            }
            else if (btn == &cheese) {
                printf("Cheese clicked - Options menu!\n");
                // next = GS_TUTORIAL;
            }
            else if (btn == &patty) {
                printf("Patty clicked - Load game!\n");
                // next = GS_CREDIT;
            }
            else if (btn == &lettuce) {
                printf("Lettuce clicked - High scores!\n");
                // next = GS_SETTINGS;
            }
            else if (btn == &bottomBun) {
                printf("Bottom Bun clicked - Quit!\n");
                next = GS_QUIT;
            }
        }
        else btn->isClicked = false;
    }
}

void MainMenu_Draw()
{
    // TransitionWall
    AEMtx33Scale(&WallScale, 1600, 900);
    AEMtx33Trans(&WallTranslate, 0, WallPosY);
    AEMtx33Concat(&WallTransform, &WallTranslate, &WallScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(WallTransform.m);
    AEGfxTextureSet(Wall, 0.f, 0.f);
    AEGfxMeshDraw(WallMesh, AE_GFX_MDM_TRIANGLES);

    // Draw burger buttons from bottom to top (for proper layering)
    for (int i = 0; i < ButtonCount; i++) {
        BurgerButton* btn = burgerButtons[i]; 

        if (btn->texture && btn->mesh) {
            // Calculate current Y position with float offset
            float currentY = btn->targetY + btn->floatOffset;

            // Set transparency and color based on hover state
            if (btn->isHovered) {
                AEGfxSetTransparency(1.0f);
                // Slightly brighter or tinted when hovered
                AEGfxSetColorToMultiply(1.2f, 1.2f, 1.2f, 1.0f);
                btn->width = buttonWidth * 1.2f;
                btn->height = buttonHeight * 1.2f;
            }
            else {
                AEGfxSetTransparency(0.95f);  // Slightly transparent when not hovered
                AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
                btn->width = buttonWidth;
                btn->height = buttonHeight;
            }

            // Set transform
            AEMtx33 scale, translate, transform;
            AEMtx33Scale(&scale, btn->width, btn->height);
            AEMtx33Trans(&translate, btn->x, currentY);
            AEMtx33Concat(&transform, &translate, &scale);

            AEGfxSetTransform(transform.m);
            AEGfxTextureSet(btn->texture, BUTTON_DATA[i].x, BUTTON_DATA[i].y);
            AEGfxMeshDraw(btn->mesh, AE_GFX_MDM_TRIANGLES);
        }
    }

    //Render Settings
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
}

void MainMenu_Free()
{
    printf("MainMenu Free called\n");
    // Free mesh (shared by all buttons)
    if (topBun.mesh) {
        AEGfxMeshFree(topBun.mesh);
        topBun.mesh = nullptr;
    }

    // Null out mesh pointers
    for (int i = 0; i < ButtonCount; i++) {
        burgerButtons[i]->mesh = nullptr;
    }

    if (WallMesh)
    {
        AEGfxMeshFree(WallMesh);
    }
}

void MainMenu_Unload()
{
    printf("MainMenu Unload called\n");

    // Unload only once
    if (topBun.texture) {
        AEGfxTextureUnload(topBun.texture);

        // Set all pointers to nullptr after unloading
        topBun.texture = nullptr;
        cheese.texture = nullptr;
        patty.texture = nullptr;
        lettuce.texture = nullptr;
        bottomBun.texture = nullptr;
    }

    if (Wall)
    {
        AEGfxTextureUnload(Wall);
        Wall = nullptr;
    }

    if (FontId >= 0)
    {
        AEGfxDestroyFont(FontId);
        FontId = -1;
    }
}
