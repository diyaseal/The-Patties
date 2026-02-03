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

    // WALKING sprite (2nd row, 1st column)
    newCustomer.frameX = 1; 
    newCustomer.frameY = 0; 

    // Properties
    newCustomer.speed = 100.0f + (rand() % 50);  // Random speed between 100-150
    newCustomer.isWalking = true;
    // newCustomer.flipHorizontal = true;  
    newCustomer.mesh = nullptr;

    // Initialize mesh to nullptr (we'll create it dynamically in Draw)
    newCustomer.mesh = nullptr;

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
            // WALKING sprite
            customer.frameX = 1; 
            customer.frameY = 0;
			customer.flipHorizontal = false; // Facing right

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

                // IDLE sprite (1st row, 2nd column)
                customer.frameX = 0;
                customer.frameY = 0; 
            }
        }
    }
}

// ---------------------------------------------------------------------------
// Draw All Customers
// ---------------------------------------------------------------------------
void Customer_Draw(AEGfxTexture* customerTexture) {
    for (Customer& customer : customers) {
        // Switch to texture rendering mode
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        // Set color to white (to show texture without tint)
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

        // Pass the flip parameter
        AEGfxVertexList* frameMesh = CustomerMesh(
            customer.frameX,
            customer.frameY,
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
    }
}

// ---------------------------------------------------------------------------
// Free Customer System
// ---------------------------------------------------------------------------
void Customer_Free() {
    // No need to free mesh here since we're creating/freeing it in Draw now
    customers.clear();
    printf("Customer system freed\n");
}