// ---------------------------------------------------------------------------
// GameStateManager.hpp - Updates all game state functions in files. Load, Init, Update, Draw, Free, Unload
// Author: YUE JUN, DIYA, SYAKIL, ELIZABETH
// ---------------------------------------------------------------------------

#pragma once

// Function pointer type definition for state management functions
typedef void(*FP)(void);

// External declarations for global state tracking variables
extern int current, previous, next;  

// External declarations for state function pointers
extern FP fpLoad, fpInit, fpUpdate, fpDraw, fpFree, fpUnload;

// Function prototype for initializing the Game State Manager
void GSM_Init(int startingState);

// Function prototype for updating Game State Manager function pointers
void GSM_Update();