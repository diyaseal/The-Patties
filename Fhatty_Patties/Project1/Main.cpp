// ---------------------------------------------------------------------------
// includes
#include "pch.hpp"

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

   // _CrtSetBreakAlloc(12304);

    int gGameRunning = 1;

    // Using custom window procedure
    AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

    // Changing the window title
    AESysSetWindowTitle("The Fatty Patties");

    // reset the system modules
    AESysReset();

    // GSM initialize - Initialize Game State Manager with starting state (Level 1)
    GSM_Init(GS_SCH_CRED);
    AEFrameRateControllerReset();
    GSM_Update();

    // Call the Load function for the current state
    if (fpLoad)
        fpLoad();

    // Call the Initialize function for the current state
    if (fpInit)
        fpInit();

    // Game Loop
    while (gGameRunning)
    {
        // Informing the system about the loop's start
        AEFrameRateControllerStart();
        AESysFrameStart();

        // Check for exit
        if (0 == AESysDoesWindowExist())
            gGameRunning = 0;

        // Call Update function
        if (fpUpdate)
            fpUpdate();

        // Call Draw function
        if (fpDraw)
            fpDraw();

    // Clean up - call Free and Unload
    if (next != current)
    {
        // Clean up old state
        if (fpFree)
            fpFree();

        if (fpUnload)
            fpUnload();

        previous = current;
        current = next;

        if (current == GS_QUIT) {
            gGameRunning = 0;
        }

        // Update GSM function pointers
        GSM_Update();

        // Load & init new state
        if (fpLoad)
            fpLoad();
        if (fpInit)
            fpInit();
    }

    // Informing the system about the loop's end
    AESysFrameEnd();

    }

    if (fpFree)
        fpFree();

    if (fpUnload)
        fpUnload();
    //MainMenu_Free();
    //MainMenu_Unload();
    // Free the system
    AESysExit();

    return 0;
}