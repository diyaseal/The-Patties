#include "pch.hpp"

// ---------------------------------------------------------------------------
// Static variables — owns fonts AND its own meshes
// ---------------------------------------------------------------------------
static s8                fontId = -1;
static s8                fontSmall = -1;
static AEGfxVertexList* pMesh = nullptr;
static AEGfxVertexList* pStarMesh = nullptr;

static Button continueButton = {};
static Button restartButton = {};
static Button nextLevelButton = {};

static AEGfxTexture* pBgTexture = nullptr;

static float       popupScale = 0.0f;
static const float POPUP_GROW_SPEED = 3.0f;
//static const int   DISPLAY_SCORE = 1000;

static AEGfxVertexList* pCircleMesh = nullptr;

// Confirmation popup
static bool showConfirmPopup = false;
static bool confirmIsRestart = false; // true = restart, false = level select

// ---------------------------------------------------------------------------
// DrawRect
// ---------------------------------------------------------------------------
static void DrawRect(float cx, float cy, float w, float h,
    float r, float g, float b, float a,
    float scaleMultiplier = 1.0f)
{
    if (!pMesh) return;
    AEMtx33 s, t, m;
    AEMtx33Scale(&s, w * scaleMultiplier, h * scaleMultiplier);
    AEMtx33Trans(&t, cx, cy);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetColorToMultiply(r, g, b, a);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
}

// ---------------------------------------------------------------------------
// DrawStar
// ---------------------------------------------------------------------------
static void DrawStar(float cx, float cy, float size,
    float red, float green, float blue, float alpha)
{
    if (!pStarMesh) return;
    AEMtx33 s, t, r, temp, m;
    AEMtx33Scale(&s, size, size);
    AEMtx33RotDeg(&r, 180.0f);
    AEMtx33Trans(&t, cx, cy);

    AEMtx33Concat(&temp, &r, &s);
    AEMtx33Concat(&m, &t, &temp);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetColorToMultiply(red, green, blue, alpha);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(pStarMesh, AE_GFX_MDM_TRIANGLES);
}

// ---------------------------------------------------------------------------
// DrawBackground — door texture fullscreen
// ---------------------------------------------------------------------------
static void DrawBackground()
{
    if (!pMesh || !pBgTexture) return;
    AEMtx33 s, t, m;
    AEMtx33Scale(&s, 1700.0f, 1000.0f);
    AEMtx33Trans(&t, 0.0f, 0.0f);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxTextureSet(pBgTexture, 0.0f, 0.0f);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
}


// ---------------------------------------------------------------------------
// LevelComplete_Load — allocates fonts + meshes
// ---------------------------------------------------------------------------
void LevelComplete_Load()
{
    printf("LevelComplete: Load\n");

    if (pMesh != nullptr)
    {
        printf("WARNING: LevelComplete_Load called twice!\n");
        return;
    }

    Sound_Load();

    fontId = AEGfxCreateFont("Assets/Font/Starbim.ttf", 48);
    fontSmall = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 32);

    if (fontId < 0 || fontSmall < 0)
        printf("ERROR: LevelComplete failed to load fonts!\n");

    if (!pMesh)
        pMesh = CreateSquareMesh(0.5f, 0.5f);
    if (!pStarMesh)
        pStarMesh = CreateStarMesh(0.5f, 0.2f);
    if (!pCircleMesh)
        pCircleMesh = CreateCircleMesh(2.0f, 32);


    pBgTexture = AEGfxTextureLoad("Assets/Assets/Transition_wall.png");
    if (!pBgTexture)
        printf("ERROR: LevelComplete failed to load background texture!\n");

    
}

// ---------------------------------------------------------------------------
// LevelComplete_Init — state only, NO allocations
// ---------------------------------------------------------------------------
void LevelComplete_Init()
{
    printf("LevelComplete: Init\n");

    popupScale = 0.0f;  // reset animation

    continueButton.x = -260.0f; continueButton.y = -130.0f;
    continueButton.width = 200.0f; continueButton.height = 65.0f;
    continueButton.isHovered = continueButton.isClicked = false;

    restartButton.x = 0.0f;    restartButton.y = -130.0f;
    restartButton.width = 200.0f; restartButton.height = 65.0f;
    restartButton.isHovered = restartButton.isClicked = false;

    nextLevelButton.x = 260.0f;  nextLevelButton.y = -130.0f;
    nextLevelButton.width = 200.0f; nextLevelButton.height = 65.0f;
    nextLevelButton.isHovered = nextLevelButton.isClicked = false;

    bool canProgress = (currentPoints >= Level_Manager_GetStarPosition(0));

    if (canProgress && Level_Manager_GetCurrentLevel() == 5)
        Sound_PlaySFX_CompleteAll();
    else if (canProgress)
        Sound_PlaySFX_LevelComplete();
    else
        Sound_PlaySFX_Lose();       // ← no stars, can't progress
}

// ---------------------------------------------------------------------------
// LevelComplete_Update
// ---------------------------------------------------------------------------
void LevelComplete_Update()
{
    f64 dt = AEFrameRateControllerGetFrameTime();

    if (popupScale < 1.0f) {
        popupScale += POPUP_GROW_SPEED * static_cast<float>(dt);
        if (popupScale > 1.0f) popupScale = 1.0f;
        return;
    }

    // Block main buttons while confirm popup is open
    if (showConfirmPopup) {
        // Get mouse position
        s32 mouseX, mouseY;
        AEInputGetCursorPosition(&mouseX, &mouseY);
        float worldMouseX = (float)mouseX - 800.0f;
        float worldMouseY = -(float)mouseY + 450.0f;

        // Confirm popup button positions
        float yesBtnX = -80.0f, yesBtnY = -30.0f;
        float noBtnX = 80.0f, noBtnY = -30.0f;
        float popBtnW = 130.0f, popBtnH = 50.0f;

        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            // YES — confirm action
            if (worldMouseX >= yesBtnX - popBtnW / 2.0f &&
                worldMouseX <= yesBtnX + popBtnW / 2.0f &&
                worldMouseY >= yesBtnY - popBtnH / 2.0f &&
                worldMouseY <= yesBtnY + popBtnH / 2.0f) {
                showConfirmPopup = false;
                if (confirmIsRestart)
                    next = GS_GAME;
                else
                    next = GS_LEVELS;
            }
            // NO — cancel, close popup
            if (worldMouseX >= noBtnX - popBtnW / 2.0f &&
                worldMouseX <= noBtnX + popBtnW / 2.0f &&
                worldMouseY >= noBtnY - popBtnH / 2.0f &&
                worldMouseY <= noBtnY + popBtnH / 2.0f) {
                showConfirmPopup = false;
            }
        }
        return; // block everything else while popup is open
    }

    // Normal button logic
    continueButton.isHovered = IsButtonHovered(continueButton);
    restartButton.isHovered = IsButtonHovered(restartButton);

    // Level Select — show confirm
    if (IsButtonClicked(continueButton)) {
        showConfirmPopup = true;
        confirmIsRestart = false;
    }
    // Restart — show confirm
    if (IsButtonClicked(restartButton)) {
        showConfirmPopup = true;
        confirmIsRestart = true;
    }

    nextLevelButton.isHovered = IsButtonHovered(nextLevelButton);
    if (currentPoints > Level_Manager_GetStarPosition(0)) {
        if (IsButtonClicked(nextLevelButton) && Level_Manager_GetCurrentLevel() != 5) {
            currentLevel = Level_Manager_GetCurrentLevel() + 1;
            next = GS_GAME;
        }
        else if (IsButtonClicked(nextLevelButton) && Level_Manager_GetCurrentLevel() == 5) {
            next = GS_END_CRED;
        }
    }
}

// ---------------------------------------------------------------------------
// LevelComplete_Draw
// ---------------------------------------------------------------------------
void LevelComplete_Draw()
{
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    float s = popupScale;

    // 1. Striped door texture background
    DrawBackground();

    // 2. Pink border
    DrawRect(0.0f, 20.0f, 830.0f, 570.0f, 0.85f, 0.55f, 0.85f, 1.0f, s);

    // 3. Black title bar
    DrawRect(0.0f, 185.0f, 800.0f, 195.0f, 0.12f, 0.12f, 0.12f, 1.0f, s);

    // 4. Light grey body
    DrawRect(0.0f, -65.0f, 800.0f, 350.0f, 0.82f, 0.82f, 0.82f, 1.0f, s);

    // 5. Dark score row
    DrawRect(0.0f, 55.0f, 770.0f, 75.0f, 0.25f, 0.25f, 0.25f, 1.0f, s);

	// 6. Button layout — check if player earned enough points to unlock next level
    bool showNextLevel = (currentPoints >= Level_Manager_GetStarPosition(0));

    if (showNextLevel) {
        continueButton.x = -260.0f;
        restartButton.x = 0.0f;
        nextLevelButton.x = 260.0f;
    }
    else {
        continueButton.x = -130.0f;
        restartButton.x = 130.0f;
    }

    DrawRect(continueButton.x, continueButton.y,
        continueButton.width, continueButton.height,
        continueButton.isHovered ? 0.6f : 0.85f, 0.1f, 0.1f, 1.0f, s);

    DrawRect(restartButton.x, restartButton.y,
        restartButton.width, restartButton.height,
        0.1f, 0.1f, restartButton.isHovered ? 0.6f : 0.9f, 1.0f, s);

    if (showNextLevel) {
        DrawRect(nextLevelButton.x, nextLevelButton.y,
            nextLevelButton.width, nextLevelButton.height,
            0.1f, nextLevelButton.isHovered ? 0.4f : 0.6f, 0.1f, 1.0f, s);
    }

    // 9. Stars — only after fully scaled in
    if (s >= 1.0f)
    {
        int starsEarned = currentStars; // returns 0, 1, 2, or 3

        for (int i = 0; i < 3; i++)
        {
            float starX = -60.0f + (i * 60.0f); // -60, 0, 60
            float starY = 55.0f;

            if (i < starsEarned)
                DrawStar(starX, starY, 50.0f, 1.0f, 0.85f, 0.0f, 1.0f); // yellow
            else
                DrawStar(starX, starY, 50.0f, 0.4f, 0.4f, 0.4f, 1.0f);  // grey
        }
    }

    // 10. Text — only after fully scaled in
    if (fontId >= 0 && s >= 1.0f)
    {
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);

        AEGfxPrint(fontId, "A LEVEL COMPLETE! A", -0.43f, 0.28f, 1.2f, 1.0f, 0.85f, 0.0f, 1.0f);
        AEGfxPrint(fontSmall, "SCORE", -0.28f, 0.09f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

        char scoreStr[16];
        sprintf_s(scoreStr, "%d", static_cast<int>(currentPoints));
        AEGfxPrint(fontSmall, scoreStr, 0.22f, 0.09f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

        if (showNextLevel) {
            AEGfxPrint(fontSmall, "LEVEL SELECT", -0.43f, -0.30f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(fontSmall, "RESTART", -0.07f, -0.30f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(fontSmall, "NEXT LEVEL", 0.23f, -0.30f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);
        }
        else {
            AEGfxPrint(fontSmall, "LEVEL SELECT", -0.27f, -0.30f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);
            AEGfxPrint(fontSmall, "RESTART", 0.09f, -0.30f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        AEGfxPrint(fontSmall, "HIGH SCORE: ", -0.11f, -0.10f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
        char    highScoreStr[16];
        sprintf_s(highScoreStr, "%d", static_cast<int>(Level_Manager_GetHighestPoints()));
        AEGfxPrint(fontSmall, highScoreStr, 0.08f, -0.10f, 0.8f, 0.0f, 0.0f, 0.0f, 1.0f);
    }

    // Confirm popup
    if (showConfirmPopup && fontSmall >= 0)
    {
        float popBtnW = 130.0f, popBtnH = 50.0f;
        float yesBtnX = -80.0f, yesBtnY = -30.0f;
        float noBtnX = 80.0f, noBtnY = -30.0f;

        // Dark overlay (flat is fine)
        DrawRect(0.0f, 0.0f, 1600.0f, 900.0f, 0.0f, 0.0f, 0.0f, 0.5f);

        // Popup panel — rounded cream
        DrawRoundedRect(pMesh, pCircleMesh,
            0.0f, 10.0f, 340.0f, 180.0f, 20.0f,
            0.98f, 0.96f, 0.90f, 1.0f);

        // YES button — rounded orange
        DrawRoundedRect(pMesh, pCircleMesh,
            yesBtnX, yesBtnY, popBtnW, popBtnH, 25.0f,
            0.85f, 0.31f, 0.055f, 1.0f);

        // NO button — rounded dark
        DrawRoundedRect(pMesh, pCircleMesh,
            noBtnX, noBtnY, popBtnW, popBtnH, 25.0f,
            0.17f, 0.17f, 0.17f, 1.0f);

        // Text
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        const char* msg = confirmIsRestart ? "Restart level?" : "Go to Level Select?";
        AEGfxPrint(fontSmall, msg, -0.18f, 0.09f, 0.85f, 0.2f, 0.2f, 0.2f, 1.0f);
        AEGfxPrint(fontSmall, "YES", -0.12f, -0.09f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(fontSmall, "NO", 0.09f, -0.09f, 0.85f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
}

// ---------------------------------------------------------------------------
// LevelComplete_Free — reset state, NO deallocations
// ---------------------------------------------------------------------------
void LevelComplete_Free()
{
    printf("LevelComplete: Free\n");
    continueButton = {};
    restartButton = {};
    nextLevelButton = {};
    showConfirmPopup = false;
}

// ---------------------------------------------------------------------------
// LevelComplete_Unload — frees everything Load allocated
// ---------------------------------------------------------------------------
void LevelComplete_Unload()
{
    printf("LevelComplete: Unload\n");

    if (pMesh) { AEGfxMeshFree(pMesh);        pMesh = nullptr; }
    if (pStarMesh) { AEGfxMeshFree(pStarMesh);    pStarMesh = nullptr; }
    if (fontId >= 0) { AEGfxDestroyFont(fontId);    fontId = -1; }
    if (fontSmall >= 0) { AEGfxDestroyFont(fontSmall); fontSmall = -1; }
    if (pBgTexture) { AEGfxTextureUnload(pBgTexture); pBgTexture = nullptr; }
    if (pCircleMesh) { AEGfxMeshFree(pCircleMesh); pCircleMesh = nullptr; }

    Sound_Unload();
}