/* Start Header ************************************************************************/
/*!
\file:GameStateList.h
\author Diya Seal, diya.seal, 2502930
\par diya.seal@digipen.edu
\date 01, 24, 2026
\brief Header file containing game state enumeration definitions.
Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

// ----------------------------------------------------------------------------
// GameStateList.h - Game state enumeration definitions
// Contains all possible game states used by the Game State Manager
// ----------------------------------------------------------------------------

// Enumeration of all possible game states
enum GS_STATES
{
	GS_LEVEL1 = 0,  // Level 1 game state
	GS_LEVEL2,      // Level 2 game state

	GS_QUIT,        // Quit game state - signals program termination
	GS_RESTART      // Restart state - signals level restart
};