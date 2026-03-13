
// ---------------------------------------------------------------------------
//	Customer Struct
// ----------------------------------------------------------------------------
//struct Customer {
//    float x, y;                    // Position
//    float targetX, targetY;        // Target position (where they walk to)
//    float speed;                   // Walking speed
//    bool isWalking;                // Is the customer currently walking?
//
//    // Order system
//    std::vector<int> order;        // What the customer wants (list of ingredient IDs)
//    bool hasOrder;                 // Does customer have an order?
//    bool isWaitingForFood;         // Is customer waiting at counter for food?
//    float patience;                // Current patience (0-100)
//    float maxPatience;             // Maximum patience
//
//    int frameX;
//    int frameY;
//	bool flipHorizontal;         // For sprite flipping
//
//    AEGfxVertexList* mesh;         // The rectangle mesh
//    AEMtx33 transform;             // Transformation matrix
//};

struct Customer {
    // Initialize all variables to default values
    float x{ 0.0f }, y{ 0.0f };                    // Position
    float targetX{ 0.0f }, targetY{ 0.0f };        // Target position
    float speed{ 0.0f };                         // Walking speed
    bool isWalking{ false };                     // Is walking?

    // Order system
    std::vector<int> order{};                  // Customer's order
    bool hasOrder{ false };                      // Does customer have order?
    bool isWaitingForFood{ false };              // Waiting at counter?
    float patience{ 0.0f };                      // Current patience
    float maxPatience{ 0.0f };                   // Maximum patience

    // Sprite fields
    int frameX{ 0 };
    int frameY{ 0 };
    bool flipHorizontal{ false };                // For sprite flipping

    //Gender
    bool isMale{ false };

    //customer emotions
    bool isSatisfied{ false };
    bool isAngry{ false };
    bool isLeaving{ false };
    float emotionTimer{ 0.0f };
    int slotIndex{ -1 };
    int textureIndex{ 0 }; //to find which sprite sheet customer i want to use

    AEGfxVertexList* mesh{ nullptr };            // The rectangle mesh
    AEMtx33 transform{};                       // Transformation matrix

    // Constructor to ensure proper initialization
    //Customer() = default;
};

// ---------------------------------------------------------------------------
// Customer System Functions
// ---------------------------------------------------------------------------
void Customer_Init();
void Customer_Spawn();
void RandomOrder(Customer* customer);
void DrawThoughtBubble(float customerX, float customerY, const std::vector<int>& order);
void Customer_Update(float dt);
void Customer_Draw(AEGfxTexture* customerTexture[]);
void Customer_Satisfy(int index);
bool Customer_IsAtTarget(Customer* customer);
bool Customer_IsLevelComplete();  
void Customer_Free();

// Helper functions
bool Customer_IsAtTarget(Customer* customer);

