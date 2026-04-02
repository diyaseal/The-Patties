// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"
// SYAKIL TOUCH
// ---------------------------------------------------------------------------
// Global variables for MainMenu

static float totalElapsedTime = 1.5f;            // Timer for screen duration
static s8 FontId = -1;                           // Font ID
static int pendingNextState = 0;
static float textScale = 0.0f;
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
static AEGfxVertexList* IngredientsMesh = nullptr;  // Ingredient mesh 

//Pop up confirmation for quit
static bool showQuitConfirm = false;

//setting menu
static bool settingsJustClosed = false;
// ---------------------------------------------------------------------------
// Drop animation state (indices match burgerButtons[] order: 0=bottomBun 4=topBun)
static float dropCurrentY[5];          // live Y used for rendering during drop
static float dropVelocity[5];          // velocity in px/s (negative = moving downward in world space)
static bool  dropLanded[5];            // true once the button has settled at targetY

// Staggered delays (seconds after wall finishes) bottom bun first, top bun last
static const float DROP_DELAY[5] = { 0.0f, 0.15f, 0.30f, 0.45f, 0.60f };
static const float DROP_START_Y = 1200.0f;  // well above screen top
static const float GRAVITY = 3500.0f;  // px/s
static const float BOUNCE_DAMPEN = 0.30f;    // fraction of velocity kept on bounce
static const float LAND_THRESHOLD = 50.0f;     // px � snap to rest when this close
// ---------------------------------------------------------------------------
static AEGfxTexture* BG = nullptr;
static AEMtx33 BGTransform, BGTranslate, BGScale;
static float BGAlpha = 0.0f;   // fades from 0 to 1 on entry
static const float BG_FADE_SPEED = 0.8f;
static AEGfxVertexList* BGMesh = nullptr;
// --------------------------------------------------------------------------- 
static AEGfxTexture* Truck = nullptr;
static AEMtx33 TruckTransform, TruckTranslate, TruckScale;
static AEGfxVertexList* TruckMesh = nullptr;
// Wheel animation 2 frames side by side in spritesheet, each frame = 0.5 UV width
static int   truckFrame = 0;        // current frame (0 or 1)
static float truckFrameTimer = 0.0f;     // counts up to TRUCK_FRAME_DURATION
static const float TRUCK_FRAME_DURATION = 1.0f / 8.0f;
static const float TRUCK_FRAME_UV[2] = { 0.0f, 1.f / 2.f };

static float truckX = 1400.0f;  // starts off-screen right
static float truckVelX = -900.0f;  // drives left at 900 px/s
static const float TRUCK_TARGET_X = -400.0f;  // final resting X
static const float TRUCK_Y = -125.0f;  // fixed vertical position
static const float TRUCK_DECEL = 2600.0f;  // braking force px/s�
static bool  truckParked = false;     // true once fully stopped
static float TruckFinishedTimer = 0.0f;
static bool truckDriveSoundPlayed = false;
// ---------------------------------------------------------------------------
static AEGfxTexture* Wall = nullptr;
static AEMtx33 WallTransform, WallTranslate, WallScale;
static float WallPosY = 900.0f;
static float SlideSpeed = 500.0f;
static AEGfxVertexList* WallMesh = nullptr;
static bool  wallTriggered = false;   // true once Top Bun is clicked
static AEGfxVertexList* CircleMesh = nullptr; 
// ---------------------------------------------------------------------------
// Game State Functions

void MainMenu_Load()
{
    // Load Truck
    Truck = AEGfxTextureLoad("Assets/Assets/Food_Truck.png");
    TruckMesh = CreateTruckMesh(0.5, 0.5);

    if (Truck)
        printf("Truck texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Food_truck.png\n");

    // Load BG
    BG = AEGfxTextureLoad("Assets/Assets/Loading_BG.png");
    BGMesh = CreateSquareMesh(0.5, 0.5);

    //Load font
    FontId = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 30);
    if (FontId < 0)
        printf("ERROR: Failed to load font!\n");
    else
        printf("SUCCESS: Font loaded, ID = %d\n", FontId);

    // Load ingredient sprite sheet for button texture
    Ingredients = AEGfxTextureLoad("Assets/Assets/Ingredients.png");
    IngredientsMesh = CreateIngredientMesh(0.5f, 0.5f);

    if (Ingredients)
        printf("Ingredients texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Ingredients.png\n");

    // Load Wall
    Wall = AEGfxTextureLoad("Assets/Assets/Transition_wall.png");
    WallMesh = CreateSquareMesh(0.5, 0.5);

    if (Wall)
        printf("Wall texture loaded successfully\n");
    else
        printf("ERROR: Failed to load Transition_wall.png\n");

    // Load Audio
    Sound_Load();
    Sound_PlayBGM(); //Load sound then play BGM
    Settings_Load();
}

void MainMenu_Init()
{
    printf("Initialized");

    // Button dimensions
    float startY = -200.0f, spacing = 100.0f;

    for (int i = 0; i < ButtonCount; i++) {
        float yPos = startY + spacing * BUTTON_DATA[i].yOffset;

        burgerButtons[i]->texture = Ingredients;
        burgerButtons[i]->mesh = IngredientsMesh;
        burgerButtons[i]->name = BUTTON_DATA[i].name;
        burgerButtons[i]->x = 300.0f;
        burgerButtons[i]->y = yPos;
        burgerButtons[i]->targetY = yPos;
        burgerButtons[i]->width = buttonWidth;
        burgerButtons[i]->height = buttonHeight;
        burgerButtons[i]->floatOffset = 0.0f;
        burgerButtons[i]->floatSpeed = BUTTON_DATA[i].floatSpeed;
        burgerButtons[i]->floatAmount = BUTTON_DATA[i].floatAmount;
        burgerButtons[i]->isHovered = false;
        burgerButtons[i]->isClicked = false;

        // Drop animation � start each button high above the screen
        dropCurrentY[i] = DROP_START_Y;
        dropVelocity[i] = 0.0f;
        dropLanded[i] = false;
    }

    /* Elizabeth - Set wall to reset upon calling page */
    WallPosY = 900.0f;
    wallTriggered = false;
    truckParked = false;
    truckX = 1400.0f;
    truckVelX = -900.0f;
    TruckFinishedTimer = 0.0f;
    BGAlpha = 0.0f;
    totalElapsedTime = 0.0f;

    pendingNextState = 0;
    truckDriveSoundPlayed = false;

    CircleMesh = CreateCircleMesh(2.0f, 32);
}

void MainMenu_Update()
{
    float dt = (float)AEFrameRateControllerGetFrameTime();

    // Track when settings closes to block bleed-through clicks
    static bool wasSettingsOpen = false;      
    bool isNowOpen = Settings_IsOpen();        

    Settings_Update(dt, FontId);

    // Detect the frame settings just closed
    if (wasSettingsOpen && !Settings_IsOpen())  
        settingsJustClosed = true;            
    wasSettingsOpen = Settings_IsOpen();       

    // If settings just closed this frame, skip input for one frame
    if (settingsJustClosed) {
        settingsJustClosed = false;
        return;
    }

    if (Settings_IsOpen()) return;

    totalElapsedTime += dt;

    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert to WORLD coordinates (where 0,0 is screen center)
    float worldMouseX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
    float worldMouseY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

    // Wall fade-in
    if (BGAlpha < 1.0f)  // If not fully transparent
    {
        BGAlpha += dt * BG_FADE_SPEED;  // Increase transparency with dt * speed
        if (BGAlpha > 1.0f)
            BGAlpha = 1.0f; // Clamp to prevent overshoot
    }

    // Truck Drive-In
    if (!truckParked)
    {
        //tebs: Play drive sound once when truck starts moving
        if (!truckDriveSoundPlayed) {
            Sound_PlaySFX_TruckMove();
            truckDriveSoundPlayed = true;
        }

        float distToTarget = truckX - TRUCK_TARGET_X;  

        if (distToTarget > 0.0f)
        {
            // Brake proportionally � ramp braking up as it gets close
            float brakeForce = TRUCK_DECEL * (1.0f - (distToTarget / (truckX - TRUCK_TARGET_X + 1.0f)));
            truckVelX += brakeForce * dt;  // velX is negative (leftward), brakeForce reduces magnitude
            if (truckVelX > 0.0f) truckVelX = 0.0f;  // don't overshoot backwards

            truckX += truckVelX * dt;

            // Snap to target if close enough or overshot
            if (truckX <= TRUCK_TARGET_X)
            {
                truckX = TRUCK_TARGET_X;
                truckVelX = 0.0f;
                truckParked = true;
                truckFrame = 0;
                Sound_StopSFX_TruckMove();
                Sound_PlaySFX_TruckBeep(); //tebs: truck beep beep wld played 
            }

            // for ELI
            // Animate wheels only while the truck is moving
            truckFrameTimer += dt;
            if (truckFrameTimer >= TRUCK_FRAME_DURATION)
            {
                truckFrameTimer -= TRUCK_FRAME_DURATION;
                truckFrame = (truckFrame + 1) % 2;  // toggle between frame 0 and 1
            }
        }
    }

    // Accumulate AFTER the truck parked
    if (truckX <= TRUCK_TARGET_X && TruckFinishedTimer == 0.0f)
        TruckFinishedTimer = 0.001f;  // kick it off so delay[0]=0.0f doesn't fire on frame 0
    else if (truckX <= TRUCK_TARGET_X)
        TruckFinishedTimer += dt;

    // Update each burger button + Drop animation
    for (int i = 0; i < ButtonCount; i++) {
        BurgerButton* btn = burgerButtons[i];

        // Drop Physics
        if (!dropLanded[i])
        {
            if (TruckFinishedTimer > DROP_DELAY[i])
            {
                // Apply gravity (world Y: positive = up, so subtract to fall)
                dropVelocity[i] -= GRAVITY * dt;
                dropCurrentY[i] += dropVelocity[i] * dt;

                float target = btn->targetY;

                if (dropCurrentY[i] <= target)
                {
                    // Bounce: reflect velocity and dampen
                    dropCurrentY[i] = target;
                    dropVelocity[i] = -dropVelocity[i] * BOUNCE_DAMPEN;

                    // Kill tiny residual bounces so it settles cleanly
                    if (fabsf(dropVelocity[i]) < LAND_THRESHOLD) //fabsf is the standard C / C++ function for getting the absolute value of a float negate -tve sign
                    {
                        dropVelocity[i] = 0.0f;
                        dropLanded[i] = true;
                    }
                }
            }
            // While still falling: no hover/click, just continue to next button
            continue;
        }

        // Floats once landed
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
            Sound_PlaySFX_OnClick();    
            // Handle button actions based on which burger part was clicked
            if (btn == &topBun) {
                printf("Top Bun clicked - Levels Menu!\n");
                wallTriggered = true;
                pendingNextState = GS_LEVELS;  //save where the page would go (Level page)
            }
            else if (btn == &cheese) {
                printf("Cheese clicked - Tutorial!\n");
                wallTriggered = true;
                pendingNextState = GS_TUTORIAL;
            }
            else if (btn == &patty) {
                printf("Patty clicked - Credits Page!\n");
                wallTriggered = true;
                pendingNextState = GS_END_CRED;
            }
            else if (btn == &lettuce) {
                printf("Lettuce clicked - Settings Menu!\n");
                Settings_Open();
            }
            else if (btn == &bottomBun) {
                printf("Bottom Bun clicked - Quit confirmation!\n");
                showQuitConfirm = true;
            }
        }
        else btn->isClicked = false;
    }

    if (wallTriggered && WallPosY > 0) {
        WallPosY -= SlideSpeed * dt;
        if (WallPosY <= 0)
        {
            WallPosY = 0;
            next = pendingNextState;
            /*wallTriggered = false;*/
        }
    }
    // Quit confirmation popup — block everything while open
    if (showQuitConfirm) {
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);
        float worldMouseX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
        float worldMouseY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

        float yesBtnX = -80.0f, yesBtnY = -30.0f;
        float noBtnX = 80.0f, noBtnY = -30.0f;
        float popBtnW = 130.0f, popBtnH = 50.0f;

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (worldMouseX >= yesBtnX - popBtnW / 2.0f &&
                worldMouseX <= yesBtnX + popBtnW / 2.0f &&
                worldMouseY >= yesBtnY - popBtnH / 2.0f &&
                worldMouseY <= yesBtnY + popBtnH / 2.0f) {
                showQuitConfirm = false;
                pendingNextState = GS_QUIT;
                wallTriggered = true;
            }
            if (worldMouseX >= noBtnX - popBtnW / 2.0f &&
                worldMouseX <= noBtnX + popBtnW / 2.0f &&
                worldMouseY >= noBtnY - popBtnH / 2.0f &&
                worldMouseY <= noBtnY + popBtnH / 2.0f) {
                showQuitConfirm = false;
            }
        }
        return;
    }
}

void MainMenu_Draw()
{
    // Faded BG
    AEMtx33Scale(&BGScale, 1600, 900);
    AEMtx33Trans(&BGTranslate, 0, 0);
    AEMtx33Concat(&BGTransform, &BGTranslate, &BGScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransparency(BGAlpha);
    AEGfxSetTransform(BGTransform.m);
    AEGfxTextureSet(BG, 0.f, 0.f);
    AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

    // Game Title
    if (FontId >= 0)
    {
        float halfW = AEGfxGetWindowWidth() * 0.5f;
        float halfH = AEGfxGetWindowHeight() * 0.5f;

        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        // Top-left position � tweak these world-space values to taste
        f32 titleX = -500.0f;   // left side
        f32 line1Y = 250.0f;   // "Fatty" upper line
        f32 line2Y = 150.0f;   // "Patty" lower line

        // Normalize to [-1, 1] screen space for AEGfxPrint
        f32 normX = titleX / halfW;
        f32 normLine1 = line1Y / halfH;
        f32 normLine2 = line2Y / halfH;

        f32 outlineOffset = 0.02f;  // in normalized screen units, tweak to taste

        // "Fatty" outline (draw in black/dark color, offset in 4 directions)
        AEGfxPrint(FontId, "Fatty", normX - outlineOffset, normLine1, 3.0f, 0.f, 0.f, 0.f, BGAlpha); 
        AEGfxPrint(FontId, "Fatty", normX + outlineOffset, normLine1, 3.0f, 0.f, 0.f, 0.f, BGAlpha); 
        AEGfxPrint(FontId, "Fatty", normX, normLine1 + outlineOffset, 3.0f, 0.f, 0.f, 0.f, BGAlpha); 
        AEGfxPrint(FontId, "Fatty", normX, normLine1 - outlineOffset, 3.0f, 0.f, 0.f, 0.f, BGAlpha); 

        // "Fatty" actual text on top
        AEGfxPrint(FontId, "Fatty", normX, normLine1, 3.0f, 1.f, 1.f, 1.f, BGAlpha);


        // "Patty" outline
        AEGfxPrint(FontId, "Patty", normX - outlineOffset, normLine2, 3.0f, 0.f, 0.f, 0.f, BGAlpha);
        AEGfxPrint(FontId, "Patty", normX + outlineOffset, normLine2, 3.0f, 0.f, 0.f, 0.f, BGAlpha);
        AEGfxPrint(FontId, "Patty", normX, normLine2 + outlineOffset, 3.0f, 0.f, 0.f, 0.f, BGAlpha);
        AEGfxPrint(FontId, "Patty", normX, normLine2 - outlineOffset, 3.0f, 0.f, 0.f, 0.f, BGAlpha);

        // "Patty" actual text on top
        AEGfxPrint(FontId, "Patty", normX, normLine2, 3.0f, 1.f, 1.f, 1.f, BGAlpha);
    }

    // Truck
    AEGfxSetTransparency(1.0f);
    AEMtx33Scale(&TruckScale, 350, 350);
    AEMtx33Trans(&TruckTranslate, truckX, TRUCK_Y);
    AEMtx33Concat(&TruckTransform, &TruckTranslate, &TruckScale);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetTransform(TruckTransform.m);
    AEGfxTextureSet(Truck, TRUCK_FRAME_UV[truckFrame], 0.f);  // X offset selects frame
    AEGfxMeshDraw(TruckMesh, AE_GFX_MDM_TRIANGLES);

    // prevents hover color bleeding onto background
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetTransparency(1.0f);

    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;

    // Draw burger buttons from bottom to top 
    for (int i = 0; i < ButtonCount; i++) {
        BurgerButton* btn = burgerButtons[i];

        // Calculate current Y position with float offset
            // Use drop position while falling, targetY + float once landed
        float currentY = dropLanded[i]
            ? btn->targetY + btn->floatOffset
            : dropCurrentY[i];

        if (btn->texture && btn->mesh) {
            // Set transparency and color based on hover state
            if (btn->isHovered) {
                AEGfxSetTransparency(1.0f);
                // Slightly brighter or tinted when hovered
                AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f); // brighten via add, stays within safe range
                AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
                btn->width = buttonWidth * 1.2f;
                btn->height = buttonHeight * 1.2f;
                textScale *= 1.2f;
            }
            else {
                AEGfxSetTransparency(0.85f);  // Slightly transparent when not hovered
                AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
                AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
                btn->width = buttonWidth;
                btn->height = buttonHeight;
                textScale = 1.5f;
            }

            // Set transform
            AEMtx33 scale, translate, transform;
            AEMtx33Scale(&scale, btn->width, btn->height);
            AEMtx33Trans(&translate, btn->x, currentY);
            AEMtx33Concat(&transform, &translate, &scale);
            //AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxSetTransform(transform.m);
            AEGfxTextureSet(btn->texture, BUTTON_DATA[i].x, BUTTON_DATA[i].y);
            AEGfxMeshDraw(btn->mesh, AE_GFX_MDM_TRIANGLES);

            // Draw label on top of button � only once it has landed
            if (FontId >= 0)
            {
                //full opacity regardless of hover state
                AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                AEGfxSetBlendMode(AE_GFX_BM_BLEND);

                // Centre the text on the button X, slight vertical offset to sit nicely
                f32 textX = (btn->x - 50) / halfW;   // nudge left to visually centre (adjust to taste)
                f32 textY = currentY / halfH;  // slight upward offset from button centre

                AEGfxPrint(FontId, btn->name, textX, textY, textScale, 0.f, 0.f, 0.f, 1.f);

                AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
                AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            }
        }
    }

    if (wallTriggered && Wall && WallMesh)
    {
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetTransparency(1.0f);

        AEMtx33Scale(&WallScale, 1600.0f, 900.0f);   // full screen width + a bit extra
        AEMtx33Trans(&WallTranslate, 0, WallPosY);
        AEMtx33Concat(&WallTransform, &WallTranslate, &WallScale);
        AEGfxSetTransform(WallTransform.m);
        AEGfxTextureSet(Wall, 0.f, 0.f);
        AEGfxMeshDraw(WallMesh, AE_GFX_MDM_TRIANGLES);
    }

    //Render back Settings
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetTransparency(1.0f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

	//Quit confirmation popup
    if (showQuitConfirm && FontId >= 0)
    {
        AEMtx33 s, t, m;
        float yesBtnX = -80.0f, yesBtnY = -30.0f;
        float noBtnX = 80.0f, noBtnY = -30.0f;
        float popBtnW = 130.0f, popBtnH = 50.0f;

        // Dark overlay
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.6f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, 1600.0f, 900.0f);
        AEMtx33Trans(&t, 0.0f, 0.0f);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(BGMesh, AE_GFX_MDM_TRIANGLES);

        // Popup panel, buttons
        DrawRoundedRect(BGMesh, CircleMesh,
            0.0f, 10.0f, 340.0f, 180.0f, 20.0f,
            0.98f, 0.96f, 0.90f, 1.0f);
        DrawRoundedRect(BGMesh, CircleMesh,
            yesBtnX, yesBtnY, popBtnW, popBtnH, 25.0f,
            0.85f, 0.31f, 0.055f, 1.0f);  // orange YES
        DrawRoundedRect(BGMesh, CircleMesh,
            noBtnX, noBtnY, popBtnW, popBtnH, 25.0f,
            0.17f, 0.17f, 0.17f, 1.0f);   // dark NO

        // Text
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxPrint(FontId, "Quit the game?", -0.15f, 0.09f, 0.85f, 0.2f, 0.2f, 0.2f, 1.0f);
        AEGfxPrint(FontId, "YES", -0.13f, -0.09f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(FontId, "NO", 0.08f, -0.09f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    // Draw settings panel on top of everything
    Settings_Draw(FontId, BGMesh, CircleMesh);  // BGMesh is a plain square mesh, perfect for this
}

void MainMenu_Free()
{
    printf("MainMenu Free called\n");
    showQuitConfirm = false;

    // Free mesh (shared by all buttons)
    if (topBun.mesh) {
        AEGfxMeshFree(topBun.mesh);
        topBun.mesh = nullptr;
    }

    // Null out mesh pointers
    for (int i = 0; i < ButtonCount; i++) {
        burgerButtons[i]->mesh = nullptr;
    }

    if (BGMesh)
    {
        AEGfxMeshFree(BGMesh);
        BGMesh = nullptr;
    }

    if (TruckMesh) {
        AEGfxMeshFree(TruckMesh);
        TruckMesh = nullptr;
    }

    if (WallMesh) {
        AEGfxMeshFree(WallMesh);
        WallMesh = nullptr;
    }

    Sound_Free(); //stop bgm when leaving

    if (CircleMesh) { 
        AEGfxMeshFree(CircleMesh); 
        CircleMesh = nullptr; 
    }
}

void MainMenu_Unload()
{
    printf("MainMenu Unload called\n");

    if (Ingredients) {
        AEGfxTextureUnload(Ingredients);
        Ingredients = nullptr;
        topBun.texture = cheese.texture = patty.texture = lettuce.texture = bottomBun.texture = nullptr;
    }

    if (BG)
    {
        AEGfxTextureUnload(BG);
        BG = nullptr;
    }

    if (Truck) {
        AEGfxTextureUnload(Truck);
        Truck = nullptr;
    }

    if (Wall) {
        AEGfxTextureUnload(Wall);
        Wall = nullptr;
    }

    if (FontId >= 0)
    {
        AEGfxDestroyFont(FontId);
        FontId = -1;
    }
    Sound_Unload();
    Settings_Unload();
}
