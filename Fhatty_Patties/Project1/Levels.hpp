// ---------------------------------------------------------------------------
// Levels.hpp
// Description: Header file for the level selection screen.
//              Declares state management functions and data structures
//              for displaying unlockable levels with star ratings.
//              Allows players to choose which level to play.
// ---------------------------------------------------------------------------
#pragma once
void Levels_Load();
void Levels_Init();
void Levels_Update();
void Levels_Draw();
void Levels_Free();
void Levels_Unload();

// Level button structure
struct LevelButton {
    Button button;
    const char* ingredientName;
    int starsEarned;  // 0-3 stars
    bool Unlocked;
};

static const struct {
    const char* name;
    float x, y;
}INGREDIENT_DATA[] = {
    { "Cheese", 0.f, 1.f / 2.f },
    { "Lettuce", 2.f/4.f, 0.f },
    { "Tomato", 1.f/4.f, 0.f },
    { "Onion", 1.f/4.f, 1.f/2.f },
    { "Cucumber", 2.f/4.f, 1.f/2.f }
};