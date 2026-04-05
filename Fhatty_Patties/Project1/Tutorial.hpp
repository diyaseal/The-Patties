// ---------------------------------------------------------------------------
// Tutorial.hpp
// Description: Header file for the tutorial game state.
//              Guides the player through basic gameplay mechanics:
//              - Customer arrival and order checking
//              - Building a burger with correct ingredient order
//              - Filling and serving soda
//              - Serving the completed meal to the customer
//              Reuses all rendering and customer systems from game.cpp
//              but adds a guided state machine on top.
// ---------------------------------------------------------------------------

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