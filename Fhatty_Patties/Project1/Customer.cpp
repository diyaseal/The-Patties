#include "pch.hpp"
#include <vector>
#include <cstdlib>
#include <ctime>

// ---------------------------------------------------------------------------
// Customer System Variables
// ---------------------------------------------------------------------------
static std::vector<Customer> customers;
static const int MAX_CUSTOMERS = 1;        // Only 1 customer for prototype
static bool hasSpawned = false;            // Track if we've spawned the customer

// Predefined positions
static const float SPAWN_X = -700.0f;       // Customers spawn off-screen left
static const float SPAWN_Y = 100.0f;
static const float COUNTER_X = -200.0f;     // Where customers walk to (the counter)
static const float COUNTER_Y = 100.0f;

// ---------------------------------------------------------------------------
// Initialize Customer System
// ---------------------------------------------------------------------------
void Customer_Init() {
    customers.clear();
    hasSpawned = false;
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

    // Properties
    newCustomer.speed = 100.0f + (rand() % 50);  // Random speed between 100-150
    newCustomer.isWalking = true;

    // Create mesh (rectangle body)
    newCustomer.mesh = SquareMesh(1.0f, 2.0f);  // Tall rectangle for person shape

    customers.push_back(newCustomer);
    printf("Customer spawned! Total customers: %zu\n", customers.size());
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
    for (Customer& customer : customers) {
        // Walking logic
        if (customer.isWalking) {
            // Calculate direction to target
            float dx = customer.targetX - customer.x;
            float dy = customer.targetY - customer.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance > 0.01f) {
                // Normalize direction and move
                float moveX = (dx / distance) * customer.speed * dt;
                float moveY = (dy / distance) * customer.speed * dt;

                customer.x += moveX;
                customer.y += moveY;
            }

            // Check if reached target
            if (Customer_IsAtTarget(&customer)) {
                customer.isWalking = false;
                printf("Customer reached target position!\n");
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draw All Customers
// ---------------------------------------------------------------------------
void Customer_Draw() {
    for (Customer& customer : customers) {
        // Set customer color to blue
        AEGfxSetColorToMultiply(0.2f, 0.6f, 1.0f, 1.0f);

        // Create transformation matrices
        AEMtx33 scale, translate;
        AEMtx33Scale(&scale, 40.0f, 80.0f);  // 40x80 pixel rectangle
        AEMtx33Trans(&translate, customer.x, customer.y);
        AEMtx33Concat(&customer.transform, &translate, &scale);

        // Draw customer
        AEGfxSetTransform(customer.transform.m);
        AEGfxMeshDraw(customer.mesh, AE_GFX_MDM_TRIANGLES);
    }
}

// ---------------------------------------------------------------------------
// Free Customer System
// ---------------------------------------------------------------------------
void Customer_Free() {
    for (Customer& customer : customers) {
        if (customer.mesh) {
            AEGfxMeshFree(customer.mesh);
        }
    }
    customers.clear();
    printf("Customer system freed\n");
}