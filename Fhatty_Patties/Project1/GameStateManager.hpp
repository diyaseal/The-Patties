/* Start Header ************************************************************************/
/*!
\file:GameStateManager.h
\author Diya Seal, diya.seal, 2502930
\par diya.seal@digipen.edu
\date 01, 24, 2026
\brief Header file for Game State Manager handling level transitions and function pointers.
Copyright (C) 2026 DigiPen Institute of Technology.  // Fixed year from 20xx to 2026
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

// ----------------------------------------------------------------------------
// GameStateManager.h - Header for Game State Manager functionality
// ----------------------------------------------------------------------------

// Function pointer type definition for state management functions
typedef void(*FP)(void);

//// External declarations for global state tracking variables
extern int current, previous, next;  // Current, previous, and next game states

// External declarations for state function pointers
extern FP fpLoad, fpInit, fpUpdate, fpDraw, fpFree, fpUnload;

// Function prototype for initializing the Game State Manager
void GSM_Init(int startingState);

// Function prototype for updating Game State Manager function pointers
void GSM_Update();