// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include <math.h>   // Added for cosf and sinf

// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	int gGameRunning = 1;

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

	// Changing the window title
	AESysSetWindowTitle("UI Port Assignment - With Health System!");

	// reset the system modules
	AESysReset();

	// -------------------------------------------------------------------
	// CREATE THE SQUARE MESH (FOR HEALTH BAR AND ICONS)
	// -------------------------------------------------------------------
	AEGfxVertexList* pMesh = 0;  // Pointer to store square mesh

	// Start creating mesh for squares
	AEGfxMeshStart();

	// White mesh for color multiplication (SQUARE)
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,  // White
		0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,   // White
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f    // White
	);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,   // White
		0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,    // White
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f    // White
	);

	// Finish creating square mesh
	pMesh = AEGfxMeshEnd();

	// -------------------------------------------------------------------
	// CREATE CIRCLE MESHES
	// -------------------------------------------------------------------
	AEGfxVertexList* pMeshCircleRed = 0;
	AEGfxVertexList* pMeshCircleGreen = 0;
	AEGfxVertexList* pMeshCircleBlue = 0;
	AEGfxVertexList* pMeshCircleAoE = 0;  

	const int circleSegments = 32; 

	// Create red circle mesh
	AEGfxMeshStart();
	for (int i = 0; i < circleSegments; i++) {
		float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;

		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFF0000, 0.0f, 0.0f, // Center (red)
			cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFFFF0000, 0.0f, 0.0f,
			cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFFFF0000, 0.0f, 0.0f
		);
	}
	pMeshCircleRed = AEGfxMeshEnd();

	// Create green circle mesh
	AEGfxMeshStart();
	for (int i = 0; i < circleSegments; i++) {
		float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;

		AEGfxTriAdd(
			0.0f, 0.0f, 0xFF00FF00, 0.0f, 0.0f, // Center (green)
			cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFF00FF00, 0.0f, 0.0f,
			cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFF00FF00, 0.0f, 0.0f
		);
	}
	pMeshCircleGreen = AEGfxMeshEnd();

	// Create blue circle mesh (for player)
	AEGfxMeshStart();
	for (int i = 0; i < circleSegments; i++) {
		float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;

		AEGfxTriAdd(
			0.0f, 0.0f, 0xFF0000FF, 0.0f, 0.0f, // Center (blue)
			cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFF0000FF, 0.0f, 0.0f,
			cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFF0000FF, 0.0f, 0.0f
		);
	}
	pMeshCircleBlue = AEGfxMeshEnd();

	// Create white circle mesh for AoE (we'll color it with transparency)
	AEGfxMeshStart();
	for (int i = 0; i < circleSegments; i++) {
		float angle1 = (float)i / circleSegments * 2.0f * 3.14159f;
		float angle2 = (float)(i + 1) / circleSegments * 2.0f * 3.14159f;

		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFFFF, 0.0f, 0.0f, // Center (white)
			cosf(angle1) * 0.5f, sinf(angle1) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
			cosf(angle2) * 0.5f, sinf(angle2) * 0.5f, 0xFFFFFFFF, 0.0f, 0.0f
		);
	}
	pMeshCircleAoE = AEGfxMeshEnd();

	// -------------------------------------------------------------------
	// GAME VARIABLES
	// -------------------------------------------------------------------
	AEMtx33 transform, scale, rotate, translate;

	// Player variables
	float playerX = 0.0f;
	float playerY = 0.0f;
	float playerRadius = 50.0f;  // Half of 100 (player size)
	float playerSpeed = 200.0f;

	// Health system
	float maxHealth = 100.0f;
	float currentHealth = 100.0f;
	float minHealth = 0.0f;

	// Red circle (damages player) - LEFT side
	float redCircleX = -400.0f;
	float redCircleY = -50.0f;
	float redCircleRadius = 150.0f;  // Half of 300
	float redCircleAoE = 200.0f;     // Area of effect radius
	float damagePerSecond = 20.0f;

	// Green circle (heals player) - RIGHT side
	float greenCircleX = 400.0f;
	float greenCircleY = -50.0f;
	float greenCircleRadius = 150.0f;
	float greenCircleAoE = 200.0f;
	float healPerSecond = 15.0f;

	// UI variables
	float healthBarMaxWidth = 1300.0f;
	int maxHealthIcons = 10;
	float iconSpacing = 120.0f;

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();

		// Check for exit
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;

		// Get delta time
		f32 dt = (f32)AEFrameRateControllerGetFrameTime();

		// -------------------------------------------------------------------
		// INPUT HANDLING - Player Movement
		// -------------------------------------------------------------------
		if (AEInputCheckCurr(AEVK_W)) {
			playerY += playerSpeed * dt;
		}
		if (AEInputCheckCurr(AEVK_S)) {
			playerY -= playerSpeed * dt;
		}
		if (AEInputCheckCurr(AEVK_A)) {
			playerX -= playerSpeed * dt;
		}
		if (AEInputCheckCurr(AEVK_D)) {
			playerX += playerSpeed * dt;
		}

		// -------------------------------------------------------------------
		// COLLISION DETECTION & HEALTH MANAGEMENT
		// -------------------------------------------------------------------

		// Check collision with RED circle (damage zone)
		float distToRed = sqrtf((playerX - redCircleX) * (playerX - redCircleX) +
			(playerY - redCircleY) * (playerY - redCircleY));
		if (distToRed < redCircleAoE) {
			currentHealth -= damagePerSecond * dt;
			if (currentHealth < minHealth) {
				currentHealth = minHealth;
			}
		}

		// Check collision with GREEN circle (healing zone)
		float distToGreen = sqrtf((playerX - greenCircleX) * (playerX - greenCircleX) +
			(playerY - greenCircleY) * (playerY - greenCircleY));
		if (distToGreen < greenCircleAoE) {
			currentHealth += healPerSecond * dt;
			if (currentHealth > maxHealth) {
				currentHealth = maxHealth;
			}
		}

		// Calculate health percentage
		float healthPercent = currentHealth / maxHealth;

		// Calculate number of health icons to show (0 to 10)
		int iconsToShow = (int)(healthPercent * maxHealthIcons + 0.5f); // Round to nearest

		// -------------------------------------------------------------------
		// RENDERING SETUP
		// -------------------------------------------------------------------
		AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);

		// -------------------------------------------------------------------
		// DRAW RED CIRCLE AoE (semi-transparent)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, redCircleAoE * 2.0f, redCircleAoE * 2.0f);
		AEMtx33Trans(&translate, redCircleX, redCircleY);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(0.2f);  // Very transparent
		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);  // Red tint
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMeshCircleAoE, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW GREEN CIRCLE AoE (semi-transparent)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, greenCircleAoE * 2.0f, greenCircleAoE * 2.0f);
		AEMtx33Trans(&translate, greenCircleX, greenCircleY);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(0.2f);  // Very transparent
		AEGfxSetColorToMultiply(0.0f, 1.0f, 0.0f, 1.0f);  // Green tint
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMeshCircleAoE, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW RED CIRCLE (solid)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, redCircleRadius * 2.0f, redCircleRadius * 2.0f);
		AEMtx33Trans(&translate, redCircleX, redCircleY);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(1.0f);  // Fully opaque
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMeshCircleRed, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW GREEN CIRCLE (solid)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, greenCircleRadius * 2.0f, greenCircleRadius * 2.0f);
		AEMtx33Trans(&translate, greenCircleX, greenCircleY);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(1.0f);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMeshCircleGreen, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW PLAYER CIRCLE (blue)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, playerRadius * 2.0f, playerRadius * 2.0f);
		AEMtx33Trans(&translate, playerX, playerY);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(1.0f);
		AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMeshCircleBlue, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW HEALTH BAR BACKGROUND (dark red - full width)
		// -------------------------------------------------------------------
		AEMtx33Scale(&scale, healthBarMaxWidth, 50.0f);
		AEMtx33Trans(&translate, 0.0f, 300.0f);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetTransparency(1.0f);
		AEGfxSetColorToMultiply(0.3f, 0.0f, 0.0f, 1.0f);  // Dark red background
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW HEALTH BAR FOREGROUND (bright red - scales with health)
		// -------------------------------------------------------------------
		float currentBarWidth = healthBarMaxWidth * healthPercent;
		float barOffsetX = -(healthBarMaxWidth - currentBarWidth) / 2.0f;  // Align left

		AEMtx33Scale(&scale, currentBarWidth, 50.0f);
		AEMtx33Trans(&translate, barOffsetX, 300.0f);
		AEMtx33Concat(&transform, &translate, &scale);

		AEGfxSetColorToMultiply(1.0f, 0.0f, 0.0f, 1.0f);  // Bright red
		AEGfxSetTransform(transform.m);
		AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

		// -------------------------------------------------------------------
		// DRAW HEALTH ICONS (maroon rectangles - only show based on health)
		// -------------------------------------------------------------------
		AEGfxSetColorToMultiply(0.5f, 0.0f, 0.0f, 1.0f); // Maroon
		for (int i = 0; i < iconsToShow; i++) {
			AEMtx33Scale(&scale, 100.0f, 50.0f);
			AEMtx33Trans(&translate, -550.0f + (i * iconSpacing), 200.0f);
			AEMtx33Concat(&transform, &translate, &scale);
			AEGfxSetTransform(transform.m);
			AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
		}

		// Informing the system about the loop's end
		AESysFrameEnd();
	}

	// Clean up
	AEGfxMeshFree(pMesh);
	AEGfxMeshFree(pMeshCircleRed);
	AEGfxMeshFree(pMeshCircleGreen);
	AEGfxMeshFree(pMeshCircleBlue);
	AEGfxMeshFree(pMeshCircleAoE);

	// free the system
	AESysExit();
}