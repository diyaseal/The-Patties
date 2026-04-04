#include "pch.hpp"

bool IsButtonHovered(const BurgerButton& button)
{
    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Check hover state
    float currentY = button.targetY + button.floatOffset;

    // Convert mouse position to world coordinates
    float worldX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
    float worldY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

    // Check if mouse is within button bounds
    return (worldX >= button.x - button.width / 2.0f &&
            worldX <= button.x + button.width / 2.0f &&
            worldY >= currentY - button.height / 2.0f &&
            worldY <= currentY + button.height / 2.0f);
}

// Check if mouse is hovering over a button
bool IsButtonHovered(const Button& button)
{
    // Get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // Convert mouse position to world coordinates
    float worldX = (float)mouseX - AEGfxGetWindowWidth() / 2.0f;
    float worldY = -(float)mouseY + AEGfxGetWindowHeight() / 2.0f;

    // Check if mouse is within button bounds
    return (worldX >= button.x - button.width / 2.0f &&
        worldX <= button.x + button.width / 2.0f &&
        worldY >= button.y - button.height / 2.0f &&
        worldY <= button.y + button.height / 2.0f);
}

// Check if button is clicked
//bool IsButtonClicked(const BurgerButton& button)
//{
//    return AEInputCheckTriggered(AEVK_LBUTTON);
//}

// Check if button is clicked
bool IsButtonClicked(const Button& button)
{
    return IsButtonHovered(button) && AEInputCheckTriggered(AEVK_LBUTTON);
}