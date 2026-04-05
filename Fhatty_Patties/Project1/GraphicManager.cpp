// ---------------------------------------------------------------------------
// GraphicManager.cpp
// Description: Implements mesh creation for all game objects and UI elements,
//              and a rounded rectangle drawing utility used across all screens.
// ---------------------------------------------------------------------------
#include "pch.hpp"
#include <cmath> 
 

AEGfxVertexList* CreateSquareMesh(float x, float y) {
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        //  change to x and y va
        -x, -y, 0xFFFFFFFF, 0.0f, 1.0f,  // Bottom-left
        x, -y, 0xFFFFFFFF, 1.0f, 1.0f,   // Bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 1.0f, 1.0f,   // Bottom-right
        x, y, 0xFFFFFFFF, 1.0f, 0.0f,    // Top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    return AEGfxMeshEnd();
}

// Soda mesh uses only the left half of the sprite sheet (U capped at 0.5)
AEGfxVertexList* CreateSodaMesh(float x, float y) { // (D) soda asset mesh 
    AEGfxMeshStart();

    AEGfxTriAdd(
        -x, -y, 0xFFFFFFFF, 0.0f, 1.0f,   // bottom-left
        x, -y, 0xFFFFFFFF, 0.5f, 1.0f,   // bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 0.5f, 1.0f,   // bottom-right
        x, y, 0xFFFFFFFF, 0.5f, 0.0f,   // top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // top-left
    );

    return AEGfxMeshEnd();
}

// Builds a circle from triangles fanning out from the centre
AEGfxVertexList* CreateCircleMesh(float radius, int segments)
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

// Same as CreateCircleMesh but maps UVs so textures render correctly on the circle
AEGfxVertexList* CreateCircleMeshUV(float radius, int segments)
{
    AEGfxMeshStart();
    const float cPI = 3.14159265359f;
    float angleStep = (2.0f * cPI) / segments;

    for (int i = 0; i < segments; ++i)
    {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float cx = 0.0f, cy = 0.0f;
        float x1 = radius * cosf(angle1);
        float y1 = radius * sinf(angle1);
        float x2 = radius * cosf(angle2);
        float y2 = radius * sinf(angle2);

        // Map position to UV: position is -0.5 to 0.5, UV is 0 to 1
        float u1 = x1 / radius * 0.5f + 0.5f;
        float v1 = y1 / radius * 0.5f + 0.5f;
        float u2 = x2 / radius * 0.5f + 0.5f;
        float v2 = y2 / radius * 0.5f + 0.5f;

        AEGfxTriAdd(
            cx, cy, 0xFFFFFFFF, 0.5f, 0.5f,  // center UV = middle of texture
            x1, y1, 0xFFFFFFFF, u1, v1,
            x2, y2, 0xFFFFFFFF, u2, v2
        );
    }
    return AEGfxMeshEnd();
}

// Ingredient mesh samples one cell (1/4 width, 1/2 height) from the sprite sheet
AEGfxVertexList* CreateIngredientMesh(float x, float y) {
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        //  change to x and y va
        -x, -y, 0xFFFFFFFF, 0.0f, 1.f/2.f,  // Bottom-left
        x, -y, 0xFFFFFFFF, 1.f/4.f, 1.f/2.f,   // Bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 1.f/4.f, 1.f/2.f,   // Bottom-right
        x, y, 0xFFFFFFFF, 1.f/4.f, 0.0f,    // Top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    return AEGfxMeshEnd();
}

// Truck mesh samples the left half of the sprite sheet (U capped at 0.5)
AEGfxVertexList* CreateTruckMesh(float x, float y) {
    AEGfxMeshStart();

    // Create a square using two triangles
    AEGfxTriAdd(
        //  change to x and y va
        -x, -y, 0xFFFFFFFF, 0.0f, 1.f / 1.f,  // Bottom-left
        x, -y, 0xFFFFFFFF, 1.f / 2.f, 1.f / 1.f,   // Bottom-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    AEGfxTriAdd(
        x, -y, 0xFFFFFFFF, 1.f / 2.f, 1.f / 1.f,   // Bottom-right
        x, y, 0xFFFFFFFF, 1.f / 2.f, 0.0f,    // Top-right
        -x, y, 0xFFFFFFFF, 0.0f, 0.0f    // Top-left
    );

    // Finish creating square mesh
    return AEGfxMeshEnd();
}

// Customer mesh samples the top-left cell (0.5 x 0.5) of the sprite sheet
AEGfxVertexList* CreateCustomerMesh() {
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.5f,  // Bottom-left
        0.5f, -0.5f, 0xFFFFFFFF, 0.5f, 0.5f,  // Bottom-right
        0.5f, 0.5f, 0xFFFFFFFF, 0.5f, 0.0f   // Top-right
    );
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.5f,  // Bottom-left
        0.5f, 0.5f, 0xFFFFFFFF, 0.5f, 0.0f,  // Top-right
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f   // Top-left
    );
    return AEGfxMeshEnd();
}

// Builds a 5-pointed star from 10 triangles (2 per point) fanning from centre
AEGfxVertexList* CreateStarMesh(float outerRadius, float innerRadius)
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

// Draws a rectangle with rounded corners using two overlapping slabs + 4 corner circles
void DrawRoundedRect(AEGfxVertexList* squareMesh, AEGfxVertexList* circleMesh,
    float cx, float cy, float w, float h, float r,
    float red, float green, float blue, float alpha)
{
    AEMtx33 s, t, m;
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetColorToMultiply(red, green, blue, alpha);

    // Horizontal slab
    AEMtx33Scale(&s, w, h - r * 2.0f);
    AEMtx33Trans(&t, cx, cy);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    // Vertical slab
    AEMtx33Scale(&s, w - r * 2.0f, h);
    AEMtx33Trans(&t, cx, cy);
    AEMtx33Concat(&m, &t, &s);
    AEGfxSetTransform(m.m);
    AEGfxMeshDraw(squareMesh, AE_GFX_MDM_TRIANGLES);

    // 4 corner circles
    float offX = w * 0.5f - r;
    float offY = h * 0.5f - r;
    float corners[4][2] = {
        { cx - offX, cy + offY },
        { cx + offX, cy + offY },
        { cx - offX, cy - offY },
        { cx + offX, cy - offY },
    };
    for (int i = 0; i < 4; i++) {
        AEMtx33Scale(&s, r / 2.0f, r / 2.0f);
        AEMtx33Trans(&t, corners[i][0], corners[i][1]);
        AEMtx33Concat(&m, &t, &s);
        AEGfxSetTransform(m.m);
        AEGfxMeshDraw(circleMesh, AE_GFX_MDM_TRIANGLES);
    }
}