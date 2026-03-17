#include "pch.hpp"

// ---------------------------------------------------------------------------
// Internal state
// ---------------------------------------------------------------------------
static bool  settingsOpen = false;

// Volume levels 0.0 – 1.0
static float masterVolume = 1.0f;
static float musicVolume = 1.0f;
static float sfxVolume = 1.0f;

// Screen mode: 0 = Windowed, 1 = Fullscreen
static int   screenMode = 0;

static s8 settingsFontId = -1;
// ---------------------------------------------------------------------------
// Panel layout constants  (world-space, screen centre = 0,0)
// ---------------------------------------------------------------------------
static const float PANEL_W = 480.0f;
static const float PANEL_H = 320.0f;
static const float PANEL_X = 0.0f;
static const float PANEL_Y = -20.0f;

// Each slider row
struct SliderRow {
    const char* label;
    float* value;          // pointer to the volume float
    float       trackX;         // left edge of track in world space
    float       trackY;         // centre Y of track
    float       trackW;         // total track width
};

static const float TRACK_W = 260.0f;
static const float TRACK_LEFT = PANEL_X - TRACK_W * 0.5f + 30.0f;

static SliderRow sliders[3] = {
    { "Master", &masterVolume, TRACK_LEFT, PANEL_Y + 60.0f,  TRACK_W },  
    { "Music",  &musicVolume,  TRACK_LEFT, PANEL_Y - 20.0f,   TRACK_W },  
    { "SFX",    &sfxVolume,    TRACK_LEFT, PANEL_Y - 100.0f,  TRACK_W },  
};

// Which slider is being dragged (-1 = none)
static int   dragSlider = -1;


// Close (X) button – top-right corner of panel
static const float CLOSE_X = PANEL_X + PANEL_W * 0.5f - 28.0f;
static const float CLOSE_Y = PANEL_Y + PANEL_H * 0.5f - 28.0f;
static const float CLOSE_R = 18.0f;   // radius used as half-size for hit test

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static bool HitBox(float mx, float my, float cx, float cy, float hw, float hh) {
    return mx >= cx - hw && mx <= cx + hw &&
        my >= cy - hh && my <= cy + hh;
}

static void ApplyVolumes() {
    Sound_SetMusicVolume(musicVolume * masterVolume);
    Sound_SetSFXVolume(sfxVolume * masterVolume);
}


float Settings_GetMusicVolume() { return musicVolume; }
float Settings_GetSFXVolume() { return sfxVolume; }
float Settings_GetMasterVolume() { return masterVolume; }
// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------
void Settings_Init() {
    settingsOpen = false;
    dragSlider = -1;
}

bool Settings_IsOpen() { return settingsOpen; }
void Settings_Open() { settingsOpen = true;  dragSlider = -1; }
void Settings_Close() { settingsOpen = false; dragSlider = -1; }

void Settings_Load() {
    settingsFontId = AEGfxCreateFont("Assets/Font/Fredoka-Bold.ttf", 28);
    if (settingsFontId < 0)
        settingsFontId = AEGfxCreateFont("Assets/Font/HollyBerryPop-LVxvE.ttf", 28);
}
// ---------------------------------------------------------------------------
void Settings_Update(float dt, s8 /*fontId – unused in Update*/) {
    if (!settingsOpen) return;

    // Mouse in world space
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float ww = (float)AEGfxGetWindowWidth();
    float wh = (float)AEGfxGetWindowHeight();
    float wmx = (float)mx - ww * 0.5f;
    float wmy = -(float)my + wh * 0.5f;

    // -----------------------------------------------------------------------
    // CLOSE button
    // -----------------------------------------------------------------------
    if (AEInputCheckTriggered(AEVK_LBUTTON)) {
        if (HitBox(wmx, wmy, CLOSE_X, CLOSE_Y, CLOSE_R, CLOSE_R)) {
            Settings_Close();
            return;
        }

        // -------------------------------------------------------------------
        // Start dragging a slider?
        // -------------------------------------------------------------------
        for (int i = 0; i < 3; i++) {
            float thumbX = sliders[i].trackX + sliders[i].trackW * (*sliders[i].value);
            float thumbY = sliders[i].trackY;
            if (HitBox(wmx, wmy, thumbX, thumbY, 16.0f, 16.0f)) {
                dragSlider = i;
                break;
            }
            // Also allow clicking anywhere on the track to jump
            if (HitBox(wmx, wmy,
                sliders[i].trackX + sliders[i].trackW * 0.5f,
                sliders[i].trackY,
                sliders[i].trackW * 0.5f, 12.0f)) {
                float newVal = (wmx - sliders[i].trackX) / sliders[i].trackW;
                *sliders[i].value = AEClamp(newVal, 0.0f, 1.0f);
                ApplyVolumes();
                dragSlider = i;
                break;
            }
        }
    }

    // -----------------------------------------------------------------------
    // Continue dragging
    // -----------------------------------------------------------------------
    if (dragSlider >= 0 && AEInputCheckPrev(AEVK_LBUTTON)) {
        float newVal = (wmx - sliders[dragSlider].trackX) / sliders[dragSlider].trackW;
        *sliders[dragSlider].value = AEClamp(newVal, 0.0f, 1.0f);
        ApplyVolumes();
    }

    // Release
    if (AEInputCheckReleased(AEVK_LBUTTON)) {
        dragSlider = -1;
    }

    // ESC closes the panel
    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        Settings_Close();
    }
}

// ---------------------------------------------------------------------------
void Settings_Draw(s8 fontId, AEGfxVertexList* squareMesh, AEGfxVertexList* circleMesh) {
    if (!settingsOpen || !squareMesh) return;

    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    // Helper lambda-style inline rounded rect
    // We reuse the same pattern as the pause menu
    auto DrawRR = [&](float cx, float cy, float w, float h, float r,
        float red, float grn, float blu, float a = 1.0f) {
            AEMtx33 s, t, m;
            AEGfxSetColorToMultiply(red, grn, blu, a);
            AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

            AEMtx33Scale(&s, w, h - r * 2.0f);
            AEMtx33Trans(&t, cx, cy);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

            AEMtx33Scale(&s, w - r * 2.0f, h);
            AEMtx33Trans(&t, cx, cy);
            AEMtx33Concat(&m, &t, &s);
            AEGfxSetTransform(m.m);
            AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

            float offX = w * 0.5f - r, offY = h * 0.5f - r;
            float corners[4][2] = {
                {cx - offX, cy + offY}, {cx + offX, cy + offY},
                {cx - offX, cy - offY}, {cx + offX, cy - offY}
            };
            for (int i = 0; i < 4; i++) {
                AEMtx33Scale(&s, r / 2.0f, r / 2.0f);
                AEMtx33Trans(&t, corners[i][0], corners[i][1]);
                AEMtx33Concat(&m, &t, &s);
                AEGfxSetTransform(m.m);
                AEGfxMeshDraw(circleMesh, AE_GFX_MDM_TRIANGLES);
            }
        };

    // -------------------------------------------------------------------
    // 1. Dark backdrop
    // -------------------------------------------------------------------
    AEMtx33 s, t, m;
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.55f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEMtx33Scale(&s, 1600.0f, 900.0f);
    AEMtx33Trans(&t, 0.0f, 0.0f);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // 2. Rounded panel (cream)
    // -------------------------------------------------------------------
    float panelR = 24.0f;
    DrawRR(PANEL_X, PANEL_Y, PANEL_W, PANEL_H, panelR, 0.98f, 0.95f, 0.88f);

    // -------------------------------------------------------------------
    // 3. Title bar — orange, only top corners rounded
    // -------------------------------------------------------------------
    float titleH = 52.0f;
    float titleCY = PANEL_Y + PANEL_H * 0.5f - titleH * 0.5f;
    float titleBarBottom = PANEL_Y + PANEL_H * 0.5f - titleH;

    // Centre strip
    AEGfxSetColorToMultiply(0.85f, 0.30f, 0.10f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEMtx33Scale(&s, PANEL_W - panelR * 2.0f, titleH);
    AEMtx33Trans(&t, PANEL_X, titleCY);
    AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    // Side strips (stop short of top corners)
    AEMtx33Scale(&s, panelR * 2.0f, titleH - panelR);
    AEMtx33Trans(&t, PANEL_X - PANEL_W * 0.5f + panelR, titleCY - panelR * 0.5f);
    AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33Trans(&t, PANEL_X + PANEL_W * 0.5f - panelR, titleCY - panelR * 0.5f);
    AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    // Top-left and top-right orange corner circles
    AEMtx33Scale(&s, panelR / 2.0f, panelR / 2.0f);
    AEMtx33Trans(&t, PANEL_X - PANEL_W * 0.5f + panelR, PANEL_Y + PANEL_H * 0.5f - panelR);
    AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
    AEGfxMeshDraw(circleMesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33Trans(&t, PANEL_X + PANEL_W * 0.5f - panelR, PANEL_Y + PANEL_H * 0.5f - panelR);
    AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
    AEGfxMeshDraw(circleMesh, AE_GFX_MDM_TRIANGLES);

    // -------------------------------------------------------------------
    // 4. Sliders with CIRCLE thumbs
    // -------------------------------------------------------------------
    for (int i = 0; i < 3; i++) {
        float tx = sliders[i].trackX;
        float ty = sliders[i].trackY;
        float tw = sliders[i].trackW;
        float val = *sliders[i].value;

        // Track background
        AEGfxSetColorToMultiply(0.30f, 0.28f, 0.26f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, tw, 8.0f);
        AEMtx33Trans(&t, tx + tw * 0.5f, ty);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

        // Fill
        if (val > 0.0f) {
            AEGfxSetColorToMultiply(1.0f, 0.72f, 0.10f, 1.0f);
            AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
            float fillW = tw * val;
            AEMtx33Scale(&s, fillW, 8.0f);
            AEMtx33Trans(&t, tx + fillW * 0.5f, ty);
            AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
            AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);
        }

        // CIRCLE thumb — scale = radius/2 because circleMesh radius=2.0
        float thumbX = tx + tw * val;
        float thumbR = 14.0f;
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEMtx33Scale(&s, thumbR / 2.0f, thumbR / 2.0f);
        AEMtx33Trans(&t, thumbX, ty);
        AEMtx33Concat(&m, &t, &s); AEGfxSetTransform(m.m);
        AEGfxMeshDraw(circleMesh, AE_GFX_MDM_TRIANGLES);

        // Label
        if (settingsFontId >= 0) {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxSetBlendMode(AE_GFX_BM_BLEND);
            float labelX = (tx - 110.0f) / halfW;
            float labelY = ty / halfH;
            AEGfxPrint(settingsFontId, const_cast<char*>(sliders[i].label),
                labelX, labelY, 1.0f, 0.15f, 0.10f, 0.05f, 1.0f);
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        }
    }

    // -------------------------------------------------------------------
    // 5. Close button (rounded red square)
    // -------------------------------------------------------------------
    DrawRR(CLOSE_X, CLOSE_Y, CLOSE_R * 2.0f, CLOSE_R * 2.0f, CLOSE_R,
        0.85f, 0.18f, 0.10f);

    if (settingsFontId >= 0) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        float xNorm = (CLOSE_X - 7.0f) / halfW;
        float yNorm = (CLOSE_Y - 12.0f) / halfH;
        AEGfxPrint(settingsFontId, "X", xNorm, yNorm, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    }

    // -------------------------------------------------------------------
    // 6. Title text
    // -------------------------------------------------------------------
    if (settingsFontId >= 0) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        float titleNormX = (PANEL_X - 60.0f) / halfW;
        float titleNormY = (PANEL_Y + PANEL_H * 0.5f - 30.0f) / halfH;
        AEGfxPrint(settingsFontId, "Settings", titleNormX, titleNormY,
            1.2f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    }

    // Reset
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetTransparency(1.0f);
}

void Settings_Unload() {
    if (settingsFontId >= 0) {
        AEGfxDestroyFont(settingsFontId);
        settingsFontId = -1;
    }
}