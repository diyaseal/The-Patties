#pragma once

// ----------------------------------------------------------------------------
// GameStateList.h - Game state enumeration definitions
// Contains all possible game states used by the Game State Manager
// ----------------------------------------------------------------------------

// Enumeration of all possible game states
enum GS_STATES
{
	GS_ST_CRED = 0, // Start Credits
	GS_MAINMENU,	// Main Menu 
	GS_LEVELS,		// Levels Directory
	GS_GAME,		// Game
	GS_LVLCMP,		// Level complete state
	GS_END_CRED,    // End Credits
	GS_LOADING,		// Loading
	GS_TUTORIAL,	// Tutorial

	GS_QUIT,        // Quit game state - signals program termination
	GS_RESTART      // Restart state - signals level restart
};