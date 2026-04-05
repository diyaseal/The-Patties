// ---------------------------------------------------------------------------
// GameStateManager.cpp
// Description: Updates all game state functions in files. Load, Init, Update, 
// Draw, Free, Unload
// Author: DIYA
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
// Global state variables - track previous, current, and next game states
int current = 0, previous = 0, next = 0;

// Function pointers for the six state operations
FP fpLoad = nullptr, fpInit = nullptr, fpUpdate = nullptr,
fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;

// ----------------------------------------------------------------------------
// Initializes the Game State Manager with a starting state
// \param startingState - The initial game state (usually GS_LEVEL1)
// ----------------------------------------------------------------------------
void GSM_Init(int startingState)
{
    printf("GSM:Initialize\n");
    // Set all state trackers to the starting state
    current = previous = next = startingState;
}

// ----------------------------------------------------------------------------
// Updates function pointers based on the current game state
// Called whenever the state changes to set up the correct function calls
// ----------------------------------------------------------------------------
void GSM_Update()
{
    printf("GSM:Update\n");
    // Set function pointers based on the current game state
    switch (current)
    {
    case GS_ST_CRED:
        fpLoad = Start_Credits_Load;              
        fpInit = Start_Credits_Init;
        fpUpdate = Start_Credits_Update;
        fpDraw = Start_Credits_Draw;
        fpFree = Start_Credits_Free;
        fpUnload = Start_Credits_Unload;
        break;
    case GS_MAINMENU:
        fpLoad = MainMenu_Load;              
        fpInit = MainMenu_Init;
        fpUpdate = MainMenu_Update;
        fpDraw = MainMenu_Draw;
        fpFree = MainMenu_Free;
        fpUnload = MainMenu_Unload;
		break;
    case GS_LEVELS:
        fpLoad = Levels_Load;             
        fpInit = Levels_Init;
        fpUpdate = Levels_Update;
        fpDraw = Levels_Draw;
        fpFree = Levels_Free;
        fpUnload = Levels_Unload;
        break;
    case GS_LOADING:
        fpLoad = Loading_Load;
        fpInit = Loading_Init;
        fpUpdate = Loading_Update;
        fpDraw = Loading_Draw;
        fpFree = Loading_Free;
        fpUnload = Loading_Unload;
        break;
    case GS_GAME:
        fpLoad = Game_Load;              
        fpInit = Game_Init;
        fpUpdate = Game_Update;
        fpDraw = Game_Draw;
        fpFree = Game_Free;
        fpUnload = Game_Unload;
        break;
    case GS_LVLCMP:
        fpLoad = LevelComplete_Load;
        fpInit = LevelComplete_Init;
        fpUpdate = LevelComplete_Update;
        fpDraw = LevelComplete_Draw;
        fpFree = LevelComplete_Free;
        fpUnload = LevelComplete_Unload;
        break;
    case GS_END_CRED:
        fpLoad = End_Credits_Load;
        fpInit = End_Credits_Init;
        fpUpdate = End_Credits_Update;
        fpDraw = End_Credits_Draw;
        fpFree = End_Credits_Free;
        fpUnload = End_Credits_Unload;
        break;
    case GS_TUTORIAL:
        fpLoad = Tutorial_Load;
        fpInit = Tutorial_Init;
        fpUpdate = Tutorial_Update;
        fpDraw = Tutorial_Draw;
        fpFree = Tutorial_Free;
        fpUnload = Tutorial_Unload;
        break;
    case GS_RESTART:
        // No function pointers for restart - handled in main loop
        break;
    case GS_QUIT:
        // No function pointers for quit - handled in main loop
        break;
    default:
        // Should never reach here with valid states
        break;
    }
}