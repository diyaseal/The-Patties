// ---------------------------------------------------------------------------
// Utils.hpp
// Description: Defines the Button struct and declares shared UI helper
//              functions for hover and click detection.
// ---------------------------------------------------------------------------
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

// Function to check if button is clicked
bool IsButtonClicked(const Button& button);
