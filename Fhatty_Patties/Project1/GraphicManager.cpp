#include "pch.h"

AEGfxVertexList* CreateSquareMesh(float x, float y) {
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

