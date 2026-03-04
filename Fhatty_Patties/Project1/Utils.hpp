// Button structure
#include "MainMenu.hpp"

struct Button
{
    float x, y;         // Position
    float width, height; // Size
    bool isHovered;    // Hover state
    bool isClicked;    // Click state
};

// Function to check if mouse is hovering over a button
bool IsButtonHovered(const BurgerButton& button);

bool IsButtonHovered(const Button& button);

bool IsButtonClicked(const BurgerButton& button);

// Function to check if button is clicked
bool IsButtonClicked(const Button& button);
