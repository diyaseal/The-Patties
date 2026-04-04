// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include "pch.hpp"

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
int currentLevel;
LevelData currentLevelData;
const char* LEVEL_DATA_PATH = "Assets/Data/Data.csv";

// ---------------------------------------------------------------------------
// Helper Function: Split string by delimiter
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
    std::ifstream file(LEVEL_DATA_PATH);
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

        if (cells.size() < 7)
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

            currentLevelData.highestPts = static_cast<float>(std::stoi(cells[6]));

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
            printf("  Current highest points: %f\n", currentLevelData.highestPts);
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
static void Level_Manager_SaveProgress() {
    // Read entire file
    std::ifstream inFile(LEVEL_DATA_PATH);
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
                << static_cast<int>(currentLevelData.starPositions[0]) << "|"
                << static_cast<int>(currentLevelData.starPositions[1]) << "|"
                << static_cast<int>(currentLevelData.starPositions[2]) << "\",\""
                << currentLevelData.starStatus[0] << "|"
                << currentLevelData.starStatus[1] << "|"
                << currentLevelData.starStatus[2] << "\","
                << currentLevelData.highestPts;

            lines[i] = newLine.str();
            break;
        }
    }

    // Write back to file
    std::ofstream outFile(LEVEL_DATA_PATH);
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
// Setter Functions
// ---------------------------------------------------------------------------
void Level_Manager_UpdateStarStatus(float cPoints) {

    printf("Checking stars: points=%.0f\n", cPoints);

    bool anyStarEarned = false;

    for (int i = 0; i < 3; i++) {

        printf("Threshold %.0f | status %d\n",
            currentLevelData.starPositions[i],
            currentLevelData.starStatus[i]);

        if (cPoints >= currentLevelData.starPositions[i]) {

            if (currentLevelData.starStatus[i] == 0) {

                currentLevelData.starStatus[i] = 1;
                printf("Star %d earned!\n", i + 1);
                anyStarEarned = true;
            }
        }
    }

    if (anyStarEarned) {
        printf("Saving stars to CSV...\n");
        Level_Manager_SaveProgress();
    }
}

void Level_Manager_UpdateHighestPoints(float cPoints) {

    if (cPoints > Level_Manager_GetHighestPoints()) {

        printf("New record! %f (old: %f)\n",
            cPoints,
            Level_Manager_GetHighestPoints());

        currentLevelData.highestPts = cPoints;

        Level_Manager_UpdateStarStatus(cPoints);
        Level_Manager_SaveProgress(); // save to CSV
    }
}

// ---------------------------------------------------------------------------
// Getter Functions
// ---------------------------------------------------------------------------
LevelData Level_Manager_GetCurrentLevelData() {
    return currentLevelData;
}

int Level_Manager_GetCurrentLevel() {
    return currentLevel;
}

float Level_Manager_GetDurationMul() {
    return currentLevelData.durationMultiplier;
}

float Level_Manager_GetMaxPoints() {
    return currentLevelData.starPositions[2];  // Highest star position
}

int Level_Manager_GetCustomerCount() {
    return currentLevelData.customerCount;
}

float Level_Manager_GetStarPosition(int index) {
    return currentLevelData.starPositions[index];
}

int Level_Manager_GetStarsEarned() {
    int stars = 0;
    for (int i = 0; i < 3; i++) {
        if (currentLevelData.starStatus[i] == 1) {
            stars++;
        }
    }
    return stars;
}

float Level_Manager_GetHighestPoints() {
    return currentLevelData.highestPts;
}

// ---------------------------------------------------------------------------
// Level Manager State Functions
// ---------------------------------------------------------------------------
void Level_Manager_Init() {
    if (currentLevel < 5) {
        Level_Manager_LoadLevel(currentLevel);
    }
    else if (currentLevel >= 5) {
        Level_Manager_LoadLevel(5);
    }
    printf("Level_Manager_Init called\n");
}

void Level_Manager_Load() {
    printf("Level_Manager_Load called\n");
}

void Level_Manager_Update() {

}

void Level_Manager_Draw() {

}

void Level_Manager_Free() {
    printf("Level_Manager_Free called\n");
}

void Level_Manager_Unload() {
    printf("Level_Manager_Unload called\n");
    currentLevelData.ingredients.clear();
}