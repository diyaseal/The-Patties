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

    float highestPts;
};

// Global level data (shared)
extern LevelData currentLevelData;
extern int currentLevel;
extern const char* LEVEL_DATA_PATH;

// Function to load level
bool Level_Manager_LoadLevel(int level);

// Setter Functions
void Level_Manager_UpdateStarStatus(float currentPoints);
void Level_Manager_UpdateHighestPoints(float currentPoints);

// Getter Functions
LevelData Level_Manager_GetCurrentLevelData();
int Level_Manager_GetCurrentLevel();
float Level_Manager_GetMaxPoints();
int Level_Manager_GetCustomerCount();
float Level_Manager_GetStarPosition(int index);
int Level_Manager_GetStarsEarned();
float Level_Manager_GetHighestPoints();

// Level Manager State Functions
void Level_Manager_Init();
void Level_Manager_Load();
void Level_Manager_Update();
void Level_Manager_Draw();
void Level_Manager_Free();
void Level_Manager_Unload();

#endif