/* Start Header ************************************************************************/
/*!
\file:pch.h
\author Diya Seal, diya.seal, 2502930
\par diya.seal@digipen.edu
\date 01, 24, 2026
\brief Precompiled header file for faster compilation.
Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#ifndef PCH_H
#define PCH_H

// ----------------------------------------------------------------------------
// Precompiled header file - includes frequently used headers
// This file is compiled once and reused for faster build times
// ----------------------------------------------------------------------------

// Add headers that you want to pre-compile here
#include <crtdbg.h> // To check for memory leaks
#include <cstdio> 
#include "AEEngine.h"
#include "GameStateList.hpp"  // Game state enumeration header
#include "GameStateManager.hpp"
#include "game.hpp"
#include "School_Credits.hpp"
#include "Team_Credits.hpp"
#include "MainMenu.hpp"
#include "GraphicManager.hpp"
//#include "Points.hpp"
#include "Utils.hpp"

#endif //PCH_H