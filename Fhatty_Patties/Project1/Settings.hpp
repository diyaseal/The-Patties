// ---------------------------------------------------------------------------
// Settings.hpp
// Description: Header file for the settings/options menu system.
//              Declares functions for managing game settings including
//              master volume, music volume, and SFX volume controls.
//              Settings persist between game sessions via file I/O.
// ---------------------------------------------------------------------------

#pragma once

void Settings_Init();
void Settings_Load();
void Settings_Unload();
void Settings_Update(float dt, s8 fontId);
void Settings_Draw(s8 fontId, AEGfxVertexList* squareMesh, AEGfxVertexList* circleMesh);
bool Settings_IsOpen();
void Settings_Open();
void Settings_Close();

float Settings_GetMusicVolume();
float Settings_GetSFXVolume();
float Settings_GetMasterVolume();

