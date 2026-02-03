#include "pch.hpp "
#include <cmath> 


AEGfxVertexList* SquareMesh(float x, float y) {
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        //  change to x and y va
        -x, -y, 0xFFFFFFFF, 0.0f, 0.0f,  // Bottom-left
        x, -y, 0xFFFFFFFF, 0.0f, 0.0f,   // Bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 0.0f, 0.0f,   // Bottom-right
        x, y, 0xFFFFFFFF, 0.0f, 0.0f,    // Top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    return AEGfxMeshEnd();
}


AEGfxVertexList* CircleMesh(float radius, int segments)
{
    AEGfxMeshStart();

    const float cPI = 3.14159265359f;
    float angleStep = (2.0f * cPI) / segments;

    for (int i = 0; i < segments; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        // Center point
        float cx = 0.0f;
        float cy = 0.0f;

        // First outer point
        float x1 = radius * cosf(angle1);
        float y1 = radius * sinf(angle1);

        // Second outer point
        float x2 = radius * cosf(angle2);
        float y2 = radius * sinf(angle2);

        AEGfxTriAdd(
            cx, cy, 0xFFFFFFFF, 0.0f, 0.0f,
            x1, y1, 0xFFFFFFFF, 0.0f, 0.0f,
            x2, y2, 0xFFFFFFFF, 0.0f, 0.0f
        );
    }

    return AEGfxMeshEnd();
}

AEGfxVertexList* IngredientMesh(float x, float y) {
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        //  change to x and y va
        -x, -y, 0xFFFFFFFF, 0.0f, 1/2.f,  // Bottom-left
        x, -y, 0xFFFFFFFF, 1/4.f, 1/2.f,   // Bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 1/4.f, 1/2.f,   // Bottom-right
        x, y, 0xFFFFFFFF, 1/4.f, 0.0f,    // Top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    return AEGfxMeshEnd();
}

AEGfxVertexList* CustomerMesh(float frameX, float frameY, float column, float row, bool flipHorizontal) {
    AEGfxMeshStart();
    //for protoype, can remove once we have the customer with same spaces
	const float UV_PADDING = 0.010f; // Small padding to prevent texture bleeding
        
    float uMin = (frameX / column) + UV_PADDING;
    float uMax = ((frameX + 1) / column) - UV_PADDING;
    float vMin = (frameY / row) + UV_PADDING;
    float vMax = ((frameY + 1) / row) - UV_PADDING;

    // If flipping, swap uMin and uMax
    if (flipHorizontal) {
        float temp = uMin;
        uMin = uMax;
        uMax = temp;
    }

    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, uMin,vMax,  // Bottom-left
        0.5f, -0.5f, 0xFFFFFFFF, uMax, vMax,  // Bottom-right
        0.5f, 0.5f, 0xFFFFFFFF, uMax, vMin   // Top-right
    );

    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, uMin,vMax,  // Bottom-left
        0.5f, 0.5f, 0xFFFFFFFF, uMax,vMin,  // Top-right
        -0.5f, 0.5f, 0xFFFFFFFF, uMin, vMin   // Top-left
    );

    return AEGfxMeshEnd();
}
AEGfxVertexList* StarMesh(float outerRadius, float innerRadius)
{
    AEGfxMeshStart();

    const float cPI = 3.14159265359f;
    const int numPoints = 5; // 5-pointed star

    // Center point
    float cx = 0.0f;
    float cy = 0.0f;

    // Calculate angle between points (360 / 5 = 72 degrees)
    float angleStep = (2.0f * cPI) / numPoints;

    // Start from top point (rotate -90 degrees to point upward)
    float startAngle = -cPI / 2.0f;

    // Create 5 triangles from center to outer points
    for (int i = 0; i < numPoints; ++i)
    {
        // Outer point angle
        float outerAngle = startAngle + (i * angleStep);

        // Inner point angles (between outer points)
        float innerAngle1 = outerAngle + (angleStep / 2.0f);
        float innerAngle2 = outerAngle - (angleStep / 2.0f);

        // Outer point coordinates
        float outerX = outerRadius * cosf(outerAngle);
        float outerY = outerRadius * sinf(outerAngle);

        // Inner point coordinates (for the valley between star points)
        float innerX1 = innerRadius * cosf(innerAngle1);
        float innerY1 = innerRadius * sinf(innerAngle1);

        float innerX2 = innerRadius * cosf(innerAngle2);
        float innerY2 = innerRadius * sinf(innerAngle2);

        // Create triangle from center to outer point
        // Triangle 1: center -> inner point (previous valley) -> outer point
        AEGfxTriAdd(
            cx, cy, 0xFFFFFFFF, 0.0f, 0.0f,           // Center
            innerX2, innerY2, 0xFFFFFFFF, 0.0f, 0.0f, // Previous valley
            outerX, outerY, 0xFFFFFFFF, 0.0f, 0.0f    // Outer point
        );

        // Triangle 2: center -> outer point -> inner point (next valley)
        AEGfxTriAdd(
            cx, cy, 0xFFFFFFFF, 0.0f, 0.0f,           // Center
            outerX, outerY, 0xFFFFFFFF, 0.0f, 0.0f,   // Outer point
            innerX1, innerY1, 0xFFFFFFFF, 0.0f, 0.0f  // Next valley
        );
    }

    return AEGfxMeshEnd();
}
