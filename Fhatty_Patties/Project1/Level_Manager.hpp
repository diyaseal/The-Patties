#ifndef LEVEL_MANAGER_HPP
#define LEVEL_MANAGER_HPP

#include "pch.hpp"

// Structure holding level info
struct LevelData {

    int level = 0;
    int customerCount = 0;

    std::vector<std::string> ingredients;

    float durationMultiplier = 1.0f;

    float starPositions[3] = { 0.0f, 0.0f, 0.0f };
    int   starStatus[3] = { 0, 0, 0 };
};

// Global level data (shared)
extern LevelData currentLevelData;
extern int currentLevel;

// Function to load level
bool Level_Manager_LoadLevel(int level);

#endif