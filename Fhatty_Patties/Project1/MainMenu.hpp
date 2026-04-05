// ---------------------------------------------------------------------------
// MainMenu.hpp
// Description: Header file for the main menu screen.
//              Declares state management functions and data structures
//              for displaying an animated burger-themed main menu with
//              floating buttons for Play, Tutorial, Credits, Settings, and Quit.
// ---------------------------------------------------------------------------
#pragma once
void MainMenu_Load();
void MainMenu_Init();
void MainMenu_Update();
void MainMenu_Draw();
void MainMenu_Free();
void MainMenu_Unload();

// Burger Button Structure
struct BurgerButton {
    AEGfxTexture* texture;
    AEGfxVertexList* mesh;
    float x, y;              // Position
    float targetY;           // Target hover position
    float floatOffset;       // Current floating offset
    float floatSpeed;        // Speed of floating
    float floatAmount;       // Amount of float movement
    float width, height;     // Dimensions
    bool isHovered;
    bool isClicked;
    const char* name;        // Button identifier
};

// Struct to store data for initilization
static const struct {
    const char* name;
    float yOffset;      // Multiplier for spacing
    float floatSpeed;
    float floatAmount;
    float x, y;
} BUTTON_DATA[] = {
    { "Quit", -1.5f, 2.0f, 15.0f, 3.f / 4.f, 1.f / 2.f },
    { "Settings", 0.2f, 2.1f, 16.0f, 1.f/ 2.f, 0 },
    { "Credits", 1.7f, 1.9f, 14.0f, 0, 0 },
    { "Tutorial", 3.2f, 2.0f, 15.0f, 0, 1.f / 2.f },
    { "Play", 5.3f, 2.1f, 16.0f, 3.f / 4.f, 0 }
};
