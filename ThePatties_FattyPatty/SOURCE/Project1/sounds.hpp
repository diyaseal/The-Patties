// ---------------------------------------------------------------------------
// Sounds.hpp - Header File for Sounds Functionality
// Author: YUE JUN, SYAKIL
// ---------------------------------------------------------------------------
void Sound_Load();

void Sound_PlayBGM();
void Sound_PlayBGMCreds();
void Sound_PlayBGMLevel();
void Sound_StopBGM();

void Sound_PlaySFX_OnClick();
void Sound_PlaySFX_Correct();   // burger served correctly
void Sound_PlaySFX_Angry(bool isMale);     // customer angry
void Sound_PlaySFX_doorClose();
void Sound_PlaySFX_TruckMove();
void Sound_StopSFX_TruckMove();
void Sound_PlaySFX_TruckBeep();
void Sound_PlaySFX_Pour();
void Sound_StopSFX_Pour();
void Sound_PlaySFX_LevelComplete();
void Sound_PlaySFX_CompleteAll();
void Sound_PlaySFX_Lose();

void Sound_SetMasterVolume(float volume);
void Sound_SetMusicVolume(float volume);
void Sound_SetSFXVolume(float volume);

void Sound_Free();
void Sound_Unload();