
// ---------------------------------------------------------------------------
//	Customer Struct
// ----------------------------------------------------------------------------
struct Customer {
    float x, y;                    // Position
    float targetX, targetY;        // Target position (where they walk to)
    float speed;                   // Walking speed
    bool isWalking;                // Is the customer currently walking?
    AEGfxVertexList* mesh;         // The rectangle mesh
    AEMtx33 transform;             // Transformation matrix
};

// ---------------------------------------------------------------------------
// Customer System Functions
// ---------------------------------------------------------------------------
void Customer_Init();
void Customer_Spawn();
void Customer_Update(float dt);
void Customer_Draw();
void Customer_Free();

// Helper functions
bool Customer_IsAtTarget(Customer* customer);

