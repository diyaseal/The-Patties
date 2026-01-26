// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "GameStateManager.h"

// ---------------------------------------------------------------------------
// main
void (*gameInit)();
void (*gameUpdate)();
void (*gameExit)();

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

	// GSM initialize - Initialize Game State Manager with starting state (Level 1)
	GSM_Initialize(GS_LEVEL1);

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();


		gameUpdate();
		// Check for exit
		if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			gGameRunning = 0;

		// -------------------------------------------------------------------
		// RENDERING SETUP
		// -------------------------------------------------------------------
		AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
		AEGfxSetRenderMode(AE_GFX_RM_COLOR);
		AEGfxSetBlendMode(AE_GFX_BM_BLEND);


		// Informing the system about the loop's end
		AESysFrameEnd();
	}

	// Clean up


	// free the system
	AESysExit();
}


void InitState(init, update, exit) {
	gameExit = exit;
	gameInit = init;
	gameUpdate = update;
	gameinit();
}

void ChangeState(init, update, exit) {
	gameExit();
	gameExit = exit;
	gameInit = init;
	gameUpdate = update;
	gameinit();
}