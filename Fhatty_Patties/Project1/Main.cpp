// ---------------------------------------------------------------------------
// includes
#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "GameStateManager.h"
#include "GameStateList.h"

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

    // GSM initialize - Initialize Game State Manager with starting state (Level 1)
    GSM_Initialize(GS_LEVEL1);
    GSM_Update();

    // Call the Load function for the current state
    if (fpLoad)
        fpLoad();

    // Call the Initialize function for the current state
    if (fpInitialize)
        fpInitialize();

    // Game Loop
    while (gGameRunning)
    {
        // Informing the system about the loop's start
        AESysFrameStart();

        // Check for exit
        if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
            gGameRunning = 0;

        // Call Update function
        if (fpUpdate)
            fpUpdate();

        // Call Draw function
        if (fpDraw)
            fpDraw();

        // Informing the system about the loop's end
        AESysFrameEnd();
    }

    // Clean up - call Free and Unload
    if (fpFree)
        fpFree();

    if (fpUnload)
        fpUnload();

    // Free the system
    AESysExit();

    return 0;
}