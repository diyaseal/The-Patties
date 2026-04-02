
#ifndef PCH_H
#define PCH_H

// Add headers that you want to pre-compile here

// ------------------------------------------------------------
// LIBRARY
// ------------------------------------------------------------
#include <crtdbg.h> // To check for memory leaks
#include <cstdio> 
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "AEEngine.h"

// ------------------------------------------------------------
// HEADERS
// ------------------------------------------------------------
#include "GameStateList.hpp"  // Game state enumeration header
#include "GameStateManager.hpp"
#include "LevelManager.hpp"
#include "Customer.hpp"
#include "Loading.hpp"
#include "Game.hpp"
#include "Utils.hpp"
#include "StartCredits.hpp"
#include "EndCredits.hpp"
#include "MainMenu.hpp"
#include "Levels.hpp"
#include "Points.hpp"
#include "GraphicManager.hpp"
#include "ParticleManager.hpp"
#include "LevelComplete.hpp"
#include "GarageDoor.hpp"
#include "Sounds.hpp"
#include "Settings.hpp"
#include "Tutorial.hpp"

//enum of ingredient type used in game.cpp 
//on click order :Syakil / diya 
enum IngredientType {
    BOTTOM_BUN,
    TOP_BUN,
    PATTY,
    CHEESE,
    LETTUCE,
    TOMATO,
    ONION,
    CUCUMBER,
    SODA,
};

#endif //PCH_H