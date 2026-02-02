#include "pch.hpp"
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