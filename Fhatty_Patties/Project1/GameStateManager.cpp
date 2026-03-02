/* Start Header ************************************************************************/
/*!
\file:GameStateManager.cpp
\author Diya Seal, diya.seal, 2502930
\par diya.seal@digipen.edu
\date 01, 24, 2026
\brief Implementation file for Game State Manager handling level transitions and function pointers.
Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "pch.hpp"

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
    case GS_SCH_CRED:
        fpLoad = School_Credits_Load;              // Add this line
        fpInit = School_Credits_Init;
        fpUpdate = School_Credits_Update;
        fpDraw = School_Credits_Draw;
        fpFree = School_Credits_Free;
        fpUnload = School_Credits_Unload;
        break;
    case GS_TEAM_CRED:
        fpLoad = Team_Credits_Load;              // Add this line
        fpInit = Team_Credits_Init;
        fpUpdate = Team_Credits_Update;
        fpDraw = Team_Credits_Draw;
        fpFree = Team_Credits_Free;
        fpUnload = Team_Credits_Unload;
        break;
    case GS_MAINMENU:
        fpLoad = MainMenu_Load;              // Add this line
        fpInit = MainMenu_Init;
        fpUpdate = MainMenu_Update;
        fpDraw = MainMenu_Draw;
        fpFree = MainMenu_Free;
        fpUnload = MainMenu_Unload;
		break;
    case GS_GAME:
        // Level 1 function pointers
        fpLoad = Game_Load;              // Add this line
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