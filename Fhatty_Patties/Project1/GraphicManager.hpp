#pragma once


AEGfxVertexList* SquareMesh(float x, float y);
AEGfxVertexList* CircleMesh(float radius, int segments);
AEGfxVertexList* IngredientMesh(float x, float y);
AEGfxVertexList* StarMesh(float outerRadius, float innerRadius);
AEGfxVertexList* CustomerMesh(float frameX, float frameY, float column, float row, bool flipHorizontal);