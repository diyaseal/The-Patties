#pragma once

AEGfxVertexList* CreateSquareMesh(float x, float y);
AEGfxVertexList* CreateCircleMesh(float radius, int segments);
AEGfxVertexList* CreateCircleMeshUV(float radius, int segments);
AEGfxVertexList* CreateIngredientMesh(float x, float y);
AEGfxVertexList* CreateTruckMesh(float x, float y);
AEGfxVertexList* CreateStarMesh(float outerRadius, float innerRadius);
AEGfxVertexList* CreateCustomerMesh();