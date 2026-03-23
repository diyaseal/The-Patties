#pragma once

void Tutorial_Load();
void Tutorial_Init();
void Tutorial_Update();
void Tutorial_Draw();
void Tutorial_Free();
void Tutorial_Unload();

// ---------------------------------------------------------------------------
// Tutorial State Machine
// ---------------------------------------------------------------------------
enum TutorialPhase {
    TUTE_CUSTOMER_WALKING,  // Customer walks in, nothing interactive yet
    TUTE_SHOW_ORDER,        // Customer stops, patience frozen, show "check order" prompt
    TUTE_BUILD_BURGER,      // Player builds burger body (bottom bun -> patty -> top bun)
    TUTE_FILL_SODA,         // Burger done, player fills and drags soda to plate
    TUTE_SERVE,             // Plate complete, player drags to customer
    TUTE_COMPLETE           // Points shown, Main Menu / Levels buttons
};