#pragma once

AEGfxVertexList* CreateSquareMesh(float x, float y);
AEGfxVertexList* CreateCircleMesh(float radius, int segments);
AEGfxVertexList* CreateCircleMeshUV(float radius, int segments);
AEGfxVertexList* CreateIngredientMesh(float x, float y);
AEGfxVertexList* CreateTruckMesh(float x, float y);
AEGfxVertexList* CreateStarMesh(float outerRadius, float innerRadius);
AEGfxVertexList* CreateSodaMesh(float x, float y);
AEGfxVertexList* CreateCustomerMesh();
void DrawRoundedRect(AEGfxVertexList* squareMesh, AEGfxVertexList* circleMesh,
    float cx, float cy, float w, float h, float r,
    float red, float green, float blue, float alpha = 1.0f);