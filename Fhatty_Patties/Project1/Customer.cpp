#include "pch.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>

// ---------------------------------------------------------------------------
// Customer System Variables
// ---------------------------------------------------------------------------
std::vector<Customer> customers;
static const int MAX_CUSTOMERS = 1;        // Only 1 customer for prototype
static bool hasSpawned = false;            // Track if we've spawned the customer

//// Predefined positions
const float SPAWN_X = -700.0f;       // Customers spawn off-screen left
const float SPAWN_Y = 100.0f;
const float COUNTER_X = -200.0f;     // Where customers walk to (the counter)
const float COUNTER_Y = 100.0f;
const float EXIT_X = 800.0f;         // Off-screen right (where customer leaves)

static AEGfxVertexList* DurationMesh = nullptr;

struct Duration {
    f32 currentDuration{ 0.0f };
    f32 maxDuration{ 0.0f };

    // Standard bar
    f32 bar_width{ 0.0f };
    f32 bar_height{ 0.0f };
    f32 bar_pos_x{ 0.0f }, bar_pos_y{ 0.0f };

    AEMtx33 transform{}; // final transformation matrix for rendering
};

// Duration bar properties - tied to the customer
static Duration duration = { 0 };
static bool durationStarted = false;  // Only start decreasing when customer arrives
static const f32 DURATION_DECREASE_RATE = 10.0f;  // How fast the bar decreases per second

// ---------------------------------------------------------------------------
// Initialize Customer System
// ---------------------------------------------------------------------------
void Customer_Init() {
    customers.clear();
    hasSpawned = false;
    durationStarted = false;
    srand(static_cast<unsigned int>(time(nullptr)));
    printf("Customer system initialized\n");
}

// ---------------------------------------------------------------------------
// Spawn a New Customer
// ---------------------------------------------------------------------------
void Customer_Spawn() {
    if (customers.size() >= MAX_CUSTOMERS) {
        return;  // Don't spawn if we're at max capacity
    }

    Customer newCustomer;

    // Starting position (off-screen)
    newCustomer.x = SPAWN_X;
    newCustomer.y = SPAWN_Y;

    // Target position (at the counter)
    newCustomer.targetX = COUNTER_X;
    newCustomer.targetY = COUNTER_Y;

    // WALKING sprite (2nd row, 1st column)
    newCustomer.frameX = 1;
    newCustomer.frameY = 0;

    // Properties
    newCustomer.speed = 200.0f;  // Random speed between 100-150
    newCustomer.isWalking = true;
    newCustomer.mesh = nullptr;

    newCustomer.hasOrder = false;
    newCustomer.isWaitingForFood = false;
    //newCustomer.patience = 100.0f;
    //newCustomer.maxPatience = 100.0f;

    // Generate a random order (simple 3-ingredient burger for prototype)
    RandomOrder(&newCustomer);

    customers.push_back(newCustomer);
    printf("Customer spawned! Total customers: %zu\n", customers.size());
}
// ---------------------------------------------------------------------------
// Create a Random burger order from customer
// ---------------------------------------------------------------------------
void RandomOrder(Customer* customer) {
    customer->order.clear();

    // Always start with bottom bun
    customer->order.push_back(BOTTOM_BUN);

    // Random main ingredient (patty or cheese)
    if (rand() % 2 == 0) {
        customer->order.push_back(PATTY);
    }
    else {
        customer->order.push_back(CHEESE);
    }

    // Random veggie (optional)
    if (rand() % 2 == 0) {
        if (rand() % 2 == 0) {
            customer->order.push_back(LETTUCE);
        }
        else {
            customer->order.push_back(TOMATO);
        }
    }

    // Always end with top bun
    customer->order.push_back(TOP_BUN);

    customer->hasOrder = true;

    // Print order for debugging
    printf("Customer order: ");
    for (int ingredient : customer->order) {
        printf("%d ", ingredient);
    }
    printf("\n");
}

// ---------------------------------------------------------------------------
// Check if Customer Reached Target
// ---------------------------------------------------------------------------
bool Customer_IsAtTarget(Customer* customer) {
    float dx = customer->targetX - customer->x;
    float dy = customer->targetY - customer->y;
    float distanceSquared = dx * dx + dy * dy;
    return distanceSquared < 25.0f;  // Within 5 pixels
}

// ---------------------------------------------------------------------------
// Update All Customers
// ---------------------------------------------------------------------------
void Customer_Update(float dt) {
    // Spawn 1 customer at the start (prototype mode)
    if (!hasSpawned) {
        Customer_Spawn();
        hasSpawned = true;
    }

    // Update each customer
    for (int i = 0; i < (int)customers.size(); i++) {
        Customer& customer = customers[i];
        // Walking logic
        if (customer.isWalking) {
            // WALKING sprite
            customer.frameX = 1;
            customer.frameY = 0;
            customer.flipHorizontal = false; // Facing right

            // Calculate direction to target
            float dx = static_cast<float>(customer.targetX - customer.x);
            float dy = static_cast<float>(customer.targetY - customer.y);
            float distance = static_cast<float>(sqrt(dx * dx + dy * dy));

            if (distance > 0.01f) {
                // Normalize direction and move
                float moveX = static_cast<float>((dx / distance) * customer.speed * dt);
                float moveY = static_cast<float>((dy / distance) * customer.speed * dt);

                customer.x += moveX;
                customer.y += moveY;
            }

            // Check if reached target
            if (Customer_IsAtTarget(&customer)) {
                // Customer walked off-screen to the right, remove them
                if (customer.targetX >= EXIT_X) {
                    customers.erase(customers.begin() + i);
                    hasSpawned = false;  // Allow a new customer to spawn
                    printf("Customer left!\n");
                    break;  // Break since we modified the vector
                }

                // Customer reached the counter
                customer.isWalking = false;
                printf("Customer reached target position!\n");

                // IDLE sprite
                customer.frameX = 0;
                customer.frameY = 0;

                // START the duration bar when customer arrives
                durationStarted = true;
                duration.currentDuration = 100.0f;
                duration.maxDuration = 100.0f;

                // Customer is now waiting for food
                customer.isWaitingForFood = true;

                // Display the order (we'll add visual later)
                printf("Customer wants: ");
                for (int ing : customer.order) {
                    switch (ing) {
                    case BOTTOM_BUN: printf("Bottom Bun "); break;
                    case TOP_BUN: printf("Top Bun "); break;
                    case PATTY: printf("Patty "); break;
                    case CHEESE: printf("Cheese "); break;
                    case LETTUCE: printf("Lettuce "); break;
                    case TOMATO: printf("Tomato "); break;
                    }
                }
                printf("\n");
            }
        }
    }

    // Only decrease duration when customer has arrived
    if (durationStarted) {
        duration.currentDuration -= DURATION_DECREASE_RATE * dt;
        if (duration.currentDuration < 0.0f) {
            duration.currentDuration = 0.0f;
            durationStarted = false;  // Stop decreasing when it hits 0
            printf("Customer duration expired!\n");

            // Make customer walk towards the right (off-screen)
            for (Customer& customer : customers) {
                if (!customer.isWalking) {
                    customer.isWalking = true;
                    customer.targetX = EXIT_X;
                    customer.targetY = customer.y;  // Keep same Y
                    customer.flipHorizontal = true;  // Face right
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draw All Customers
// ---------------------------------------------------------------------------
void Customer_Draw(AEGfxTexture* customerTexture) {
    DurationMesh = SquareMesh(0.5f, 0.5f);

    for (Customer& customer : customers) {
        // Switch to texture rendering mode
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        // Set color to white (to show texture without tint)
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        // Pass the flip parameter
        AEGfxVertexList* frameMesh = CustomerMesh(
            static_cast<float>(customer.frameX),
            static_cast<float>(customer.frameY),
            2.0f,
            1.0f,
            customer.flipHorizontal
        );

        // Set texture (no offset needed since UVs are in the mesh)
        AEGfxTextureSet(customerTexture, 0.0f, 0.0f);

        // Create transformation matrices
        AEMtx33 scale, translate;
        AEMtx33Scale(&scale, 280.0f, 280.0f);
        AEMtx33Trans(&translate, customer.x + 50.0f, customer.y);
        AEMtx33Concat(&customer.transform, &translate, &scale);

        // Draw customer sprite
        AEGfxSetTransform(customer.transform.m);
        AEGfxMeshDraw(frameMesh, AE_GFX_MDM_TRIANGLES);

        // Free the temporary mesh
        AEGfxMeshFree(frameMesh);

        // ---------------------------------------------------------------
        // DRAW DURATION BAR only when customer has arrived
        // ---------------------------------------------------------------
        if (!customer.isWalking) {
            // Switch back to color render mode for the bar
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);

            // Position the bar above the customer's head
            duration.bar_pos_x = customer.x + 10.0f;           // Centered on customer
            duration.bar_pos_y = customer.y + 100.0f;  // Above the customer's head
            duration.bar_width = 140.0f;               // FIXED: width
            duration.bar_height = 10.0f;               // FIXED: was bar_width = 10.0f

            f32 durationPercentage = duration.currentDuration / duration.maxDuration;
            if (durationPercentage > 1.0f) durationPercentage = 1.0f;
            if (durationPercentage < 0.0f) durationPercentage = 0.0f;

            // Draw gray background bar
            {
                AEMtx33 barScale, barTranslate, barTransform;
                AEMtx33Scale(&barScale, duration.bar_width, duration.bar_height);
                AEMtx33Trans(&barTranslate, duration.bar_pos_x, duration.bar_pos_y);
                AEMtx33Concat(&barTransform, &barTranslate, &barScale);

                AEGfxSetColorToMultiply(0.3f, 0.3f, 0.3f, 1.0f); // Dark gray
                AEGfxSetTransform(barTransform.m);
                AEGfxMeshDraw(DurationMesh, AE_GFX_MDM_TRIANGLES);
            }

            // Draw colored duration fill
            if (durationPercentage > 0.0f) {
                f32 currentWidth = duration.bar_width * durationPercentage;
                f32 offsetX = -(duration.bar_width - currentWidth) / 2.0f;

                AEMtx33 barScale, barTranslate, barTransform;
                AEMtx33Scale(&barScale, currentWidth, duration.bar_height);
                AEMtx33Trans(&barTranslate, duration.bar_pos_x + offsetX, duration.bar_pos_y);
                AEMtx33Concat(&barTransform, &barTranslate, &barScale);

                // Color: green (100%) to red (0%)
                f32 red = 1.0f - durationPercentage;
                f32 green = durationPercentage;
                AEGfxSetColorToMultiply(red, green, 0.0f, 1.0f);

                AEGfxSetTransform(barTransform.m);
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
    printf("Customer system freed\n");
    if (DurationMesh) {
        AEGfxMeshFree(DurationMesh);
        DurationMesh = nullptr;
    }
}