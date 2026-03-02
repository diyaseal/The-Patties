
#ifndef PCH_H
#define PCH_H

// Add headers that you want to pre-compile here
#include <crtdbg.h> // To check for memory leaks
#include <cstdio> 
#include <vector>
#include "AEEngine.h"
#include "Customer.hpp"
#include "GameStateList.hpp"  // Game state enumeration header
#include "GameStateManager.hpp"
#include "game.hpp"
#include "utils.hpp"
#include "School_Credits.hpp"
#include "Team_Credits.hpp"
#include "MainMenu.hpp"
#include "Points.hpp"
#include "GraphicManager.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

};

#endif //PCH_H