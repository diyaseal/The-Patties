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
#include "pch.h"
#include "GameStateManager.h"
#include "GameStateList.h"
#include "game.h"
#include <cstdio>  

// Global state variables - track previous, current, and next game states
int current = 0, previous = 0, next = 0;

// Function pointers for the six state operations
FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr,
fpDraw = nullptr, fpFree = nullptr, fpUnload = nullptr;

// ----------------------------------------------------------------------------
// Initializes the Game State Manager with a starting state
// \param startingState - The initial game state (usually GS_LEVEL1)
// ----------------------------------------------------------------------------
void GSM_Initialize(int startingState)
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
    case GS_LEVEL1:
        // Level 1 function pointers
        fpLoad = Game_Load;              // Add this line
        fpInitialize = Game_Initialize;
        fpUpdate = Game_Update;
        fpDraw = Game_Draw;
        fpFree = Game_Free;
        fpUnload = Game_Unload;
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