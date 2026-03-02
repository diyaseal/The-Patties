// ---------------------------------------------------------------------------
// includes
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Global Variables
// ---------------------------------------------------------------------------
int currentLevel = 1;
LevelData currentLevelData;
// ---------------------------------------------------------------------------
// Helper: Split string by delimiter
// ---------------------------------------------------------------------------
static std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        // Remove quotes if present
        if (!token.empty() && token.front() == '"') token.erase(0, 1);
        if (!token.empty() && token.back() == '"') token.pop_back();
        // Remove whitespace
        token.erase(0, token.find_first_not_of(" \t\n\r"));
        token.erase(token.find_last_not_of(" \t\n\r") + 1);

        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

// ---------------------------------------------------------------------------
// Load Level Data from CSV
// ---------------------------------------------------------------------------
bool Level_Manager_LoadLevel(int level) {
    std::ifstream file("Assets/Data/Data.csv");
    if (!file.is_open()) {
        printf("ERROR: Could not open Data.csv\n");
        return false;
    }

    std::string line;

    // Skip header row
    std::getline(file, line);

    // Read each data row
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;

        std::vector<std::string> cells = SplitString(line, ',');

        if (cells.size() < 6)
            continue;

        int csvLevel = std::stoi(cells[0]);

        if (csvLevel == level) {

            currentLevelData.level = csvLevel;

            currentLevelData.customerCount = std::stoi(cells[1]);

            currentLevelData.ingredients = SplitString(cells[2], '|');

            currentLevelData.durationMultiplier = std::stof(cells[3]);

            // POINTS (STARS)
            std::vector<std::string> pointsTokens =
                SplitString(cells[4], '|');

            if (pointsTokens.size() == 3) {
                currentLevelData.starPositions[0] = std::stof(pointsTokens[0]);
                currentLevelData.starPositions[1] = std::stof(pointsTokens[1]);
                currentLevelData.starPositions[2] = std::stof(pointsTokens[2]);
            }

            // STAR STATUS
            std::vector<std::string> starTokens =
                SplitString(cells[5], '|');

            if (starTokens.size() == 3) {
                currentLevelData.starStatus[0] = std::stoi(starTokens[0]);
                currentLevelData.starStatus[1] = std::stoi(starTokens[1]);
                currentLevelData.starStatus[2] = std::stoi(starTokens[2]);
            }

            printf("=== Level %d loaded from CSV ===\n", level);
            printf("  Customer count: %d\n", currentLevelData.customerCount);
            printf("  Ingredients (%zu): ", currentLevelData.ingredients.size());
            for (const auto& ing : currentLevelData.ingredients) {
                printf("%s ", ing.c_str());
            }
            printf("\n");
            printf("  Duration multiplier: %.2f\n", currentLevelData.durationMultiplier);
            printf("  Star positions: %.0f, %.0f, %.0f\n",
                currentLevelData.starPositions[0],
                currentLevelData.starPositions[1],
                currentLevelData.starPositions[2]);
            printf("  Star status: %d, %d, %d\n",
                currentLevelData.starStatus[0],
                currentLevelData.starStatus[1],
                currentLevelData.starStatus[2]);
            printf("================================\n");

            currentLevel = level;
            file.close();
            return true;
        }
    }

    file.close();
    printf("ERROR: Level %d not found in Data.csv\n", level);
    return false;
}

// ---------------------------------------------------------------------------
// Save star status back to CSV
// ---------------------------------------------------------------------------
static void Level_Manager_SaveStarStatus() {
    // Read entire file
    std::ifstream inFile("Data.csv");
    if (!inFile.is_open()) {
        printf("ERROR: Could not open Data.csv for reading\n");
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(inFile, line)) {
        lines.push_back(line);
    }
    inFile.close();

    // Update the line for current level
    for (size_t i = 1; i < lines.size(); i++) {  // Skip header
        std::stringstream ss(lines[i]);
        std::string cell;
        std::getline(ss, cell, ',');

        if (std::stoi(cell) == currentLevel) {
            // Rebuild the line with updated star status
            std::stringstream newLine;
            newLine << currentLevelData.level << ","
                << currentLevelData.customerCount << ",\"";

            // Ingredients
            for (size_t j = 0; j < currentLevelData.ingredients.size(); j++) {
                newLine << currentLevelData.ingredients[j];
                if (j < currentLevelData.ingredients.size() - 1) newLine << "|";
            }

            newLine << "\"," << currentLevelData.durationMultiplier << ",\""
                << (int)currentLevelData.starPositions[0] << "|"
                << (int)currentLevelData.starPositions[1] << "|"
                << (int)currentLevelData.starPositions[2] << "\",\""
                << currentLevelData.starStatus[0] << "|"
                << currentLevelData.starStatus[1] << "|"
                << currentLevelData.starStatus[2] << "\"";

            lines[i] = newLine.str();
            break;
        }
    }

    // Write back to file
    std::ofstream outFile("Data.csv");
    if (!outFile.is_open()) {
        printf("ERROR: Could not open Data.csv for writing\n");
        return;
    }

    for (const auto& l : lines) {
        outFile << l << "\n";
    }
    outFile.close();

    printf("Star status saved to CSV\n");
}

// ---------------------------------------------------------------------------
// Update star status based on points
// ---------------------------------------------------------------------------
static void Level_Manager_UpdateStarStatus(float currentPoints) {
    bool anyStarEarned = false;

    for (int i = 0; i < 3; i++) {
        if (currentPoints >= currentLevelData.starPositions[i]) {
            if (currentLevelData.starStatus[i] == 0) {
                currentLevelData.starStatus[i] = 1;  // Mark as earned
                printf("? Star %d earned!\n", i + 1);
                anyStarEarned = true;
            }
        }
    }

    if (anyStarEarned) {
        Level_Manager_SaveStarStatus();  // Save to CSV
    }
}

// ---------------------------------------------------------------------------
// Getter Functions
// ---------------------------------------------------------------------------
static LevelData Level_Manager_GetCurrentLevelData() {
    return currentLevelData;
}

static int Level_Manager_GetCurrentLevel() {
    return currentLevel;
}

static float Level_Manager_GetMaxPoints() {
    return currentLevelData.starPositions[2];  // Highest star position
}

static int Level_Manager_GetStarsEarned() {
    int stars = 0;
    for (int i = 0; i < 3; i++) {
        if (currentLevelData.starStatus[i] == 1) {
            stars++;
        }
    }
    return stars;
}

// ---------------------------------------------------------------------------
// Level Manager State Functions
// ---------------------------------------------------------------------------
static void Level_Manager_Init() {
    printf("Level_Manager_Init called\n");

    // Load level 1 by default
    Level_Manager_LoadLevel(1);
}

static void Level_Manager_Load() {
    printf("Level_Manager_Load called\n");
}

static void Level_Manager_Update() {
    // Any per-frame level management logic goes here
}

static void Level_Manager_Draw() {
    // Level manager doesn't draw anything
    // Points.cpp and Game.cpp handle rendering
}

static void Level_Manager_Free() {
    printf("Level_Manager_Free called\n");
}

static void Level_Manager_Unload() {
    printf("Level_Manager_Unload called\n");
    currentLevelData.ingredients.clear();
}