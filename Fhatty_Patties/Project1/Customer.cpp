#include "pch.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>

// ---------------------------------------------------------------------------
// Customer System Variables
// ---------------------------------------------------------------------------
std::vector<Customer> customers;
extern AEGfxTexture* Ingredients;  // for thought bubble ingredient icons (Diya's code)
extern AEGfxTexture* SodaTexture; // (D) for soda icon in thought bubble
static int totalCustomersSpawned = 0;  // tracks lifetime total
static int CUSTOMERS_PER_LEVEL = 0;  // game ends after this many
static bool levelcompletelogged = false;

// ---------------------------------------------------------------------------
// Counter Slot System
// ---------------------------------------------------------------------------
static const int MAX_SLOTS = 4;
static bool slotOccupied[MAX_SLOTS] = { false };

// ---------------------------------------------------------------------------
// Position Constants
// ---------------------------------------------------------------------------
const float SPAWN_X = -700.0f;
const float SPAWN_Y = 100.0f;
const float EXIT_X = 800.0f;
const float SLOT_START_X = -400.0f;
const float SLOT_SPACING = 250.0f;
const float COUNTER_Y = 100.0f;

// ---------------------------------------------------------------------------
// Mesh DIYA
// ---------------------------------------------------------------------------
static AEGfxVertexList* bubbleMesh = nullptr;
static AEGfxVertexList* ingredientMesh = nullptr;
static AEGfxVertexList* sodaMesh = nullptr; // (D) square mesh for soda icon
static AEGfxVertexList* DurationMesh = nullptr;
AEGfxVertexList* frameMesh = nullptr;
// ---------------------------------------------------------------------------
// Patience Bar
// ---------------------------------------------------------------------------
static f32 DURATION_DECREASE_RATE = 8.0f;

// ---------------------------------------------------------------------------
// Emotion Display
// ---------------------------------------------------------------------------
static const float EMOTION_DISPLAY_TIME = 1.5f;

// ---------------------------------------------------------------------------
// Spawn Timer
// ---------------------------------------------------------------------------
static float spawnTimer = 0.0f;
static float spawnInterval = 4.0f;  // seconds between spawns

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// (D) Helper: Check if ingredient is unlocked for current level
// ---------------------------------------------------------------------------
static bool IsIngredientUnlocked(IngredientType ingredient) {
    for (const std::string& ing : currentLevelData.ingredients) {
        switch (ingredient) {
        case BOTTOM_BUN:
            if ( ing == "bottom_bun") return true;
            break;
        case TOP_BUN:
            if (ing == "top_bun") return true;
            break;
        case PATTY:
            if (ing == "PATTY" || ing == "Patty" || ing == "patty") return true;
            break;
        case CHEESE:
            if (ing == "CHEESE" || ing == "Cheese" || ing == "cheese") return true;
            break;
        case LETTUCE:
            if (ing == "LETTUCE" || ing == "Lettuce" || ing == "lettuce") return true;
            break;
        case TOMATO:
            if (ing == "TOMATO" || ing == "Tomato" || ing == "tomato") return true;
            break;
        case ONION:
            if (ing == "ONION" || ing == "Onion" || ing == "onion") return true;
            break;
        case CUCUMBER:
            if (ing == "CUCUMBER" || ing == "Cucumber" || ing == "cucumber") return true;
            break;
        case SODA:
            if (ing == "SODA" || ing == "Soda" || ing == "soda") return true;
            break;
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// Helper: FindFreeSlot
// ---------------------------------------------------------------------------
static int FindFreeSlot() {
    for (int i = 0; i < MAX_SLOTS; i++) {
        if (!slotOccupied[i]) return i;
    }
    return -1;
}
// ---------------------------------------------------------------------------
// Initialize Customer System
// ---------------------------------------------------------------------------
void Customer_Init() {
    customers.clear();
    for (int i = 0; i < MAX_SLOTS; i++) slotOccupied[i] = false;
    CUSTOMERS_PER_LEVEL = Level_Manager_GetCustomerCount();
    spawnTimer = 0.0f;
    totalCustomersSpawned = 0;
    srand(static_cast<unsigned int>(time(nullptr)));

    DurationMesh = CreateSquareMesh(0.5f, 0.5f);
    bubbleMesh = CreateCircleMesh(0.5f, 32); // (D) rounder thought bubble
    ingredientMesh = CreateIngredientMesh(0.7f,0.7f);
    sodaMesh = CreateSodaMesh(0.7f, 0.7f); // (D) soda icon uses full-square UVs

    printf("Customer system initialized\n");

    Customer_Spawn();  // spawn first customer immediately
    levelcompletelogged = false;

    frameMesh = CreateCustomerMesh();
}

// ---------------------------------------------------------------------------
// Spawn a New Customer
// ---------------------------------------------------------------------------
void Customer_Spawn() {
    if (totalCustomersSpawned >= CUSTOMERS_PER_LEVEL) {
        return;  // Don't spawn if we've reached the total for the level
    }

    int slot = FindFreeSlot();
    if (slot == -1) return;

    Customer newCustomer;
    newCustomer.x = SPAWN_X;
    newCustomer.y = SPAWN_Y;
    newCustomer.targetX = SLOT_START_X + slot * SLOT_SPACING;
    newCustomer.targetY = COUNTER_Y;
    newCustomer.slotIndex = slot;
    slotOccupied[slot] = true;

    newCustomer.frameX = 1;      // side view
    newCustomer.frameY = 1;      // happy row
    newCustomer.flipHorizontal = false;

    newCustomer.speed = 180.0f + (rand() % 60);
    newCustomer.isWalking = true;
    newCustomer.patience = 100.0f;
    newCustomer.maxPatience = 100.0f;
    newCustomer.textureIndex = rand() % 4;  // random character
    newCustomer.isMale = (newCustomer.textureIndex == 0);  // only syakil is male

    totalCustomersSpawned++;
    RandomOrder(&newCustomer);
    customers.push_back(newCustomer);
    printf("Customer spawned in slot %d! Total: %zu\n", slot, customers.size());
}
// ---------------------------------------------------------------------------
// Create a Random burger order from customer
// ---------------------------------------------------------------------------
void RandomOrder(Customer* customer) {
    customer->order.clear();

    // (D) Customer can only ask for ingredients unlocked for the current level
    if (IsIngredientUnlocked(BOTTOM_BUN)) customer->order.push_back(BOTTOM_BUN);
    if (IsIngredientUnlocked(PATTY)) customer->order.push_back(PATTY);

    if (IsIngredientUnlocked(CHEESE) && rand() % 2 == 0) customer->order.push_back(CHEESE);
    if (IsIngredientUnlocked(LETTUCE) && rand() % 2 == 0) customer->order.push_back(LETTUCE);
    if (IsIngredientUnlocked(TOMATO) && rand() % 2 == 0) customer->order.push_back(TOMATO);
    if (IsIngredientUnlocked(ONION) && rand() % 2 == 0) customer->order.push_back(ONION);
    if (IsIngredientUnlocked(CUCUMBER) && rand() % 2 == 0) customer->order.push_back(CUCUMBER);

    if (IsIngredientUnlocked(TOP_BUN)) customer->order.push_back(TOP_BUN);

    // (D) Soda is a side item and appears after the burger in the order bubble
    if (IsIngredientUnlocked(SODA) && rand() % 2 == 0) customer->order.push_back(SODA);

    customer->hasOrder = true;
}
// ---------------------------------------------------------------------------
// Draw Thought Bubble with Ingredients :Diya
// ---------------------------------------------------------------------------
static void DrawThoughtBubble(float customerX, float customerY, const std::vector<int>& order) {
    if (order.empty()) return; // (D) nothing to draw

    float bubbleX = customerX + 55.0f;
    float bubbleY = customerY + 180.0f;

    // (D) Wrap icons so late-game orders stay inside the bubble
    const float iconSize = 32.0f;
    const float iconSpacing = 38.0f;
    const int maxPerRow = 4;

    int count = (int)order.size();
    int rows = (count + maxPerRow - 1) / maxPerRow;
    if (rows < 1) rows = 1;

    float bubbleWidth = (rows > 1) ? 195.0f : 175.0f;
    float bubbleHeight = (rows > 1) ? 135.0f : 100.0f;

    // (D) Proper outline: bigger black oval behind smaller white oval
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);

    AEMtx33 bubbleScale, bubbleTrans, bubbleTransform;
    AEMtx33Scale(&bubbleScale, bubbleWidth + 8.0f, bubbleHeight + 8.0f);
    AEMtx33Trans(&bubbleTrans, bubbleX, bubbleY);
    AEMtx33Concat(&bubbleTransform, &bubbleTrans, &bubbleScale);
    AEGfxSetTransform(bubbleTransform.m);
    AEGfxMeshDraw(bubbleMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 0.95f);
    AEMtx33Scale(&bubbleScale, bubbleWidth, bubbleHeight);
    AEMtx33Trans(&bubbleTrans, bubbleX, bubbleY);
    AEMtx33Concat(&bubbleTransform, &bubbleTrans, &bubbleScale);
    AEGfxSetTransform(bubbleTransform.m);
    AEGfxMeshDraw(bubbleMesh, AE_GFX_MDM_TRIANGLES);

    for (int i = 0; i < count; i++) {
        int row = i / maxPerRow;
        int col = i % maxPerRow;

        int itemsThisRow = maxPerRow;
        if (row == rows - 1) {
            itemsThisRow = count - row * maxPerRow;
            if (itemsThisRow <= 0) itemsThisRow = maxPerRow;
        }

        float rowWidth = (itemsThisRow - 1) * iconSpacing;
        float startX = bubbleX - rowWidth / 2.0f;
        float iconX = startX + col * iconSpacing;
        float iconY = bubbleY + 18.0f - row * 38.0f;

        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        bool useSodaMesh = false;
        switch (order[i]) {
        case BOTTOM_BUN: AEGfxTextureSet(Ingredients, 3.f / 4.f, 1.f / 2.f); break;
        case TOP_BUN:    AEGfxTextureSet(Ingredients, 3.f / 4.f, 0.f); break;
        case PATTY:      AEGfxTextureSet(Ingredients, 0.f, 0.f); break;
        case CHEESE:     AEGfxTextureSet(Ingredients, 0.f, 1.f / 2.f); break;
        case LETTUCE:    AEGfxTextureSet(Ingredients, 2.f / 4.f, 0.f); break;
        case TOMATO:     AEGfxTextureSet(Ingredients, 1.f / 4.f, 0.f); break;
        case ONION:      AEGfxTextureSet(Ingredients, 1.f / 4.f, 1.f / 2.f); break; // (D)
        case CUCUMBER:   AEGfxTextureSet(Ingredients, 2.f / 4.f, 1.f / 2.f); break; // (D)
        case SODA:       AEGfxTextureSet(SodaTexture, 1.f / 2.f, 0.f); // (D) use right half of soda sheet
        useSodaMesh = true;// (D) draw soda with sodaMesh, not ingredientMesh
        break;

        }

        AEMtx33 iconScale, iconTrans, iconTransform;
        AEMtx33Scale(&iconScale, iconSize, iconSize);
        AEMtx33Trans(&iconTrans, iconX, iconY);
        AEMtx33Concat(&iconTransform, &iconTrans, &iconScale);

        AEGfxSetTransform(iconTransform.m);
        AEGfxMeshDraw(useSodaMesh ? sodaMesh : ingredientMesh, AE_GFX_MDM_TRIANGLES);
    }

    // Tail circles
    float tailX = customerX + 25.0f;
    float tailY = customerY + 138.0f;
    float tailSize = 18.0f;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 1.0f);
    AEMtx33 tailScale, tailTrans, tailTransform;
    AEMtx33Scale(&tailScale, tailSize + 4.0f, tailSize + 4.0f);
    AEMtx33Trans(&tailTrans, tailX, tailY);
    AEMtx33Concat(&tailTransform, &tailTrans, &tailScale);
    AEGfxSetTransform(tailTransform.m);
    AEGfxMeshDraw(bubbleMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 0.95f);
    AEMtx33Scale(&tailScale, tailSize, tailSize);
    AEMtx33Trans(&tailTrans, tailX, tailY);
    AEMtx33Concat(&tailTransform, &tailTrans, &tailScale);
    AEGfxSetTransform(tailTransform.m);
    AEGfxMeshDraw(bubbleMesh, AE_GFX_MDM_TRIANGLES);
}

// ---------------------------------------------------------------------------
// Customer_IsAtTarget
// ---------------------------------------------------------------------------
bool Customer_IsAtTarget(Customer* customer) {
    float dx = customer->targetX - customer->x;
    float dy = customer->targetY - customer->y;
    return (dx * dx + dy * dy) < 25.0f;
}

// ---------------------------------------------------------------------------
// StartLeaving (internal)
// ---------------------------------------------------------------------------
static void StartLeaving(Customer& customer) {
    customer.isLeaving = true;
    customer.isWalking = true;
    customer.isWaitingForFood = false;
    customer.targetX = EXIT_X;
    customer.targetY = customer.y;
    customer.flipHorizontal = false;

    if (customer.isSatisfied) {
        customer.frameX = 1;
        customer.frameY = 1;
        printf("Customer leaving HAPPY\n");
    }
    else {
        customer.frameX = 1;
        customer.frameY = 0;
        printf("Customer leaving ANGRY\n");
    }
}

// ---------------------------------------------------------------------------
// Update All Customers
// ---------------------------------------------------------------------------
void Customer_Update(float dt) {
    spawnTimer += dt;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;
        Customer_Spawn();
    }

    // Check if all customers have been served/left and we hit the limit
    if (totalCustomersSpawned >= CUSTOMERS_PER_LEVEL && customers.empty()) {
        if (!levelcompletelogged) {
            printf("Level complete! All %d customers served.\n", CUSTOMERS_PER_LEVEL);
            levelcompletelogged = true;
        }
    }

    for (int i = 0; i < (int)customers.size(); i++) {
        Customer& c = customers[i];

        // STATE 1: WALKING
        if (c.isWalking) {
            float dx = c.targetX - c.x;
            float dy = c.targetY - c.y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > 0.01f) {
                c.x += (dx / dist) * c.speed * dt;
                c.y += (dy / dist) * c.speed * dt;
            }

            if (Customer_IsAtTarget(&c)) {
                if (c.isLeaving) {
                    if (c.slotIndex >= 0) slotOccupied[c.slotIndex] = false;
                    customers.erase(customers.begin() + i);
                    printf("Customer left!\n");
                    i--;
                    continue;
                }

                c.isWalking = false;
                c.isWaitingForFood = true;
                c.frameX = 0;
                c.frameY = 1;
                c.flipHorizontal = false;

                printf("Customer wants: ");
                for (int ing : c.order) {
                    switch (ing) {
                    case BOTTOM_BUN: printf("Bottom Bun "); break;
                    case TOP_BUN:    printf("Top Bun ");    break;
                    case PATTY:      printf("Patty ");      break;
                    case CHEESE:     printf("Cheese ");     break;
                    case LETTUCE:    printf("Lettuce ");    break;
                    case TOMATO:     printf("Tomato ");     break;
                    case ONION:      printf("Onion ");      break; // (D)
                    case CUCUMBER:   printf("Cucumber ");   break; // (D)
                    case SODA:       printf("Soda ");       break; // (D)
                    }
                }
                printf("\nCustomer arrived at slot %d\n", c.slotIndex);
            }
            continue;
        }

        // STATE 2: SHOWING EMOTION before leaving
        if (c.isAngry || c.isSatisfied) {
            c.emotionTimer += dt;
            if (c.emotionTimer >= EMOTION_DISPLAY_TIME) {
                StartLeaving(c);
            }
            continue;
        }

        // STATE 3: WAITING FOR FOOD - patience drains
        if (c.isWaitingForFood) {
            c.patience -= DURATION_DECREASE_RATE * dt * Level_Manager_GetDurationMul();

            if (c.patience <= 0.0f) {
                c.patience = 0.0f;
                c.isAngry = true;
                c.isWaitingForFood = false;
                c.frameX = 0;
                c.frameY = 0;
                c.flipHorizontal = false;
                Sound_PlaySFX_Angry(c.isMale);
                printf("Customer ANGRY! Patience ran out (slot %d)\n", c.slotIndex);
            }
        }
    }
}

bool Customer_IsLevelComplete() {
    return totalCustomersSpawned >= CUSTOMERS_PER_LEVEL && customers.empty();
}

// ---------------------------------------------------------------------------
// Customer_Satisfy
// Called from game.cpp when player serves correct burger
// ---------------------------------------------------------------------------
void Customer_Satisfy(int index) {
    if (index < 0 || index >= (int)customers.size()) return;
    Customer& customer = customers[index];
    if (!customer.isWaitingForFood) return;

    customer.isSatisfied = true;
    customer.isWaitingForFood = false;
    customer.emotionTimer = 0.0f;
    customer.frameX = 0;
    customer.frameY = 1;
    customer.flipHorizontal = false;
    Sound_PlaySFX_Correct();
    printf("Customer SATISFIED! (slot %d)\n", customer.slotIndex);
}

// ---------------------------------------------------------------------------
// Draw All Customers
// ---------------------------------------------------------------------------
void Customer_Draw(AEGfxTexture* customerTexture[]) {

    for (Customer& customer : customers) {
        // Switch to texture rendering mode
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        // Set color to white (to show texture without tint)
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        // Set texture (no offset needed since UVs are in the mesh)
        float offsetU = customer.frameX * 0.5f;  // 2 columns → each cell = 0.5
        float offsetV = customer.frameY * 0.5f;  // 2 rows    → each cell = 0.5
        AEGfxTextureSet(customerTexture[customer.textureIndex], offsetU, offsetV);

        // Create transformation matrices
        AEMtx33 scale, translate;
        AEMtx33Scale(&scale, 280.0f, 280.0f);
        AEMtx33Trans(&translate, customer.x + 50.0f, customer.y);
        AEMtx33Concat(&customer.transform, &translate, &scale);

        // Draw customer sprite
        AEGfxSetTransform(customer.transform.m);
        AEGfxMeshDraw(frameMesh, AE_GFX_MDM_TRIANGLES);

        // ---------------------------------------------------------------
        // DRAW THOUGHT BUBBLE when customer is waiting for food :Diya
        // ---------------------------------------------------------------
        if (customer.isWaitingForFood && customer.hasOrder) {
            DrawThoughtBubble(customer.x + 50.0f, customer.y, customer.order);
        }

        // ---------------------------------------------------------------
        // DRAW PATIENCE BAR only when customer has arrived
        // ---------------------------------------------------------------
        if (customer.isWaitingForFood) {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);

            const float bar_w = 140.0f, bar_h = 10.0f;
            const float bar_x = customer.x + 10.0f;
            const float bar_y = customer.y + 160.0f;


            // Grey Background Bar
            {
                AEMtx33 s, t, m;
                AEMtx33Scale(&s, bar_w, bar_h);
                AEMtx33Trans(&t, bar_x, bar_y);
                AEMtx33Concat(&m, &t, &s);
                AEGfxSetColorToMultiply(0.3f, 0.3f, 0.3f, 1.0f);
                AEGfxSetTransform(m.m);
                AEGfxMeshDraw(DurationMesh, AE_GFX_MDM_TRIANGLES);
            }

            // Calculate customer patience percentage
            f32 pct = customer.patience / customer.maxPatience;
            if (pct < 0.0f) pct = 0.0f;
            if (pct > 1.0f) pct = 1.0f;
            if (pct > 0.0f) {
                f32 fillW = bar_w * pct;
                f32 offsetX = -(bar_w - fillW) / 2.0f;

                AEMtx33 s, t, m;
                AEMtx33Scale(&s, fillW, bar_h);
                AEMtx33Trans(&t, bar_x + offsetX, bar_y);
                AEMtx33Concat(&m, &t, &s);
                AEGfxSetColorToMultiply(1.0f - pct, pct, 0.0f, 1.0f);
                AEGfxSetTransform(m.m);
                AEGfxMeshDraw(DurationMesh, AE_GFX_MDM_TRIANGLES);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Free Customer System
// ---------------------------------------------------------------------------
void Customer_Free() {
    customers.clear();
    for (int i = 0; i < MAX_SLOTS; i++) slotOccupied[i] = false;
    if (frameMesh) {
        AEGfxMeshFree(frameMesh);
        frameMesh = nullptr;
    }
    if (DurationMesh) {
        AEGfxMeshFree(DurationMesh);
        DurationMesh = nullptr;
    }
    if (bubbleMesh) {
        AEGfxMeshFree(bubbleMesh);
        bubbleMesh = nullptr;
    }
    if (ingredientMesh) {
        AEGfxMeshFree(ingredientMesh);
        ingredientMesh = nullptr;
    }
    if (sodaMesh) {
        AEGfxMeshFree(sodaMesh);
        sodaMesh = nullptr;
    }
    printf("Customer system freed\n");
}