#include "pch.hpp"

// ---------------------------------------------------------------------------
// Audio assets and groups
// ---------------------------------------------------------------------------

//BGM
static AEAudio bgmAudio;
static AEAudio bgmEndCreds;
static AEAudio bgmLevel;

//SFX
static AEAudio sfxOnClick;
static AEAudio sfxHover;

static AEAudio sfxCorrect;
static AEAudio sfxCompleteAll;
static AEAudio sfxLevelComplete;
static AEAudio sfxUnlock;
static AEAudio sfxLose;

static AEAudio sfxAngryMale;
static AEAudio sfxAngryFemale;
static AEAudio sfxdoorClose;
static AEAudio sfxStack;
static AEAudio sfxPour;

static AEAudio sfxTruckMove;
static AEAudio sfxTruckBeep;

//Groups
static AEAudioGroup bgmGroup;
static AEAudioGroup sfxGroup;


// ---------------------------------------------------------------------------
// Sound_Load
// ---------------------------------------------------------------------------
void Sound_Load() {
    bgmGroup = AEAudioCreateGroup();
    sfxGroup = AEAudioCreateGroup();

    bgmAudio = AEAudioLoadMusic("Assets/Audio/bouken.mp3");
    bgmEndCreds = AEAudioLoadMusic("Assets/Audio/bgmEnd.mp3");
    bgmLevel = AEAudioLoadMusic("Assets/Audio/boukencut.mp3");

    sfxOnClick = AEAudioLoadMusic("Assets/Audio/onClick.mp3");
    sfxHover = AEAudioLoadMusic("Assets/Audio/Hover.wav");

    sfxAngryMale = AEAudioLoadMusic("Assets/Audio/mAngry.mp3");
    sfxAngryFemale = AEAudioLoadMusic("Assets/Audio/wAngry.mp3");
    sfxStack = AEAudioLoadSound("Assets/Audio/foodStacking.wav");
    sfxPour = AEAudioLoadMusic("Assets/Audio/pour2.mp3");
    sfxLose = AEAudioLoadMusic("Assets/Audio/lose.mp3");

    sfxCorrect = AEAudioLoadMusic("Assets/Audio/custHappy.mp3");
    sfxCompleteAll = AEAudioLoadMusic("Assets/Audio/CompleteAll.wav");
    sfxLevelComplete = AEAudioLoadMusic("Assets/Audio/levelComplete.wav");
    sfxUnlock = AEAudioLoadSound("Assets/Audio/UnlockLevel.wav");

    sfxdoorClose = AEAudioLoadSound("Assets/Audio/doorClose.wav");
    sfxTruckMove = AEAudioLoadMusic("Assets/Audio/truckMove.mp3");
    sfxTruckBeep = AEAudioLoadMusic("Assets/Audio/truckBeep.mp3");
    printf("Sound loaded\n");

    // reapply saved volumes to the new groups
    Sound_SetMusicVolume(Settings_GetMusicVolume() * Settings_GetMasterVolume());
    Sound_SetSFXVolume(Settings_GetSFXVolume() * Settings_GetMasterVolume());
}

// ---------------------------------------------------------------------------
// Sound_PlayBGM
// ---------------------------------------------------------------------------
void Sound_PlayBGM() {
    if (AEAudioIsValidAudio(bgmAudio) && AEAudioIsValidGroup(bgmGroup))
        AEAudioPlay(bgmAudio, bgmGroup, 0.25f, 1.0f, -1);
}
void Sound_PlayBGMCreds() {
    if (AEAudioIsValidAudio(bgmEndCreds) && AEAudioIsValidGroup(bgmGroup))
        AEAudioPlay(bgmEndCreds, bgmGroup, 0.25f, 1.0f, -1);
}
void Sound_PlayBGMLevel() {
    if (AEAudioIsValidAudio(bgmLevel) && AEAudioIsValidGroup(bgmGroup))
        AEAudioPlay(bgmLevel, bgmGroup, 0.25f, 1.0f, -1);
}

// ---------------------------------------------------------------------------
// Sound_StopBGM
// ---------------------------------------------------------------------------
void Sound_StopBGM() {
    AEAudioStopGroup(bgmGroup);
}

// ---------------------------------------------------------------------------
// SFX
// ---------------------------------------------------------------------------
void Sound_PlaySFX_OnClick() {
    if (AEAudioIsValidAudio(sfxOnClick) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxOnClick, sfxGroup, 1.0f, 1.0f, 0);
}
void Sound_PlaySFX_Correct() {
    if (AEAudioIsValidAudio(sfxCorrect) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxCorrect, sfxGroup, 1.0f, 1.0f, 0);
}

void Sound_PlaySFX_Angry(bool isMale) {
    if (isMale) {
        if (AEAudioIsValidAudio(sfxAngryMale) && AEAudioIsValidGroup(sfxGroup))
            AEAudioPlay(sfxAngryMale, sfxGroup, 1.0f, 1.0f, 0);
    }
    else {
        if (AEAudioIsValidAudio(sfxAngryFemale) && AEAudioIsValidGroup(sfxGroup))
            AEAudioPlay(sfxAngryFemale, sfxGroup, 1.0f, 1.0f, 0);
    }
}
void Sound_PlaySFX_doorClose() {
    if (AEAudioIsValidAudio(sfxdoorClose) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxdoorClose, sfxGroup, 1.0f, 1.0f, 0);
}

void Sound_PlaySFX_TruckMove() {
    if (AEAudioIsValidAudio(sfxTruckMove) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxTruckMove, sfxGroup, 1.0f, 1.0f, 0);
}
void Sound_StopSFX_TruckMove() {
    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioStopGroup(sfxGroup);  // stops all SFX
}
void Sound_PlaySFX_TruckBeep() {
    if (AEAudioIsValidAudio(sfxTruckBeep) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxTruckBeep, sfxGroup, 1.0f, 1.0f, 0);
}
void Sound_PlaySFX_Pour() {
    if (AEAudioIsValidAudio(sfxPour) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxPour, sfxGroup, 1.5f, 1.0f, 0);
}
void Sound_StopSFX_Pour() {
    // Let it finish naturally since we can't stop individual sounds
    // Or if your engine supports: AEAudioStop(sfxPour);
}

void Sound_PlaySFX_LevelComplete() {

    if (AEAudioIsValidAudio(sfxLevelComplete) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxLevelComplete, sfxGroup, 1.0f, 1.0f, 0);
}

void Sound_PlaySFX_CompleteAll() {
    if (AEAudioIsValidAudio(sfxCompleteAll) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxCompleteAll, sfxGroup, 1.0f, 1.0f, 0);
}
void Sound_PlaySFX_Lose() {
    if (AEAudioIsValidAudio(sfxLose) && AEAudioIsValidGroup(sfxGroup))
        AEAudioPlay(sfxLose, sfxGroup, 1.0f, 1.0f, 0);
}

// ---------------------------------------------------------------------------
// Sound Settings
// ---------------------------------------------------------------------------
void Sound_SetMusicVolume(float volume) {
    if (AEAudioIsValidGroup(bgmGroup))
        AEAudioSetGroupVolume(bgmGroup, volume);
}

void Sound_SetSFXVolume(float volume) {
    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioSetGroupVolume(sfxGroup, volume);
}

void Sound_SetMasterVolume(float volume) {
    if (AEAudioIsValidGroup(bgmGroup))
        AEAudioSetGroupVolume(bgmGroup, volume * Settings_GetMusicVolume());
    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioSetGroupVolume(sfxGroup, volume * Settings_GetSFXVolume());
}

// ---------------------------------------------------------------------------
// Sound_Free
// ---------------------------------------------------------------------------
void Sound_Free() {
    Sound_StopBGM();
}

// ---------------------------------------------------------------------------
// Sound_Unload
// Use AEAudioReleaseGroup instead of AEUnloadAudioGroup
// ---------------------------------------------------------------------------
void Sound_Unload() {
    AEAudioStopGroup(bgmGroup);
    AEAudioStopGroup(sfxGroup);

    AEAudioUnloadAudio(bgmAudio);
    AEAudioUnloadAudio(bgmEndCreds);
    AEAudioUnloadAudio(bgmLevel);

    AEAudioUnloadAudio(sfxCorrect);
    AEAudioUnloadAudio(sfxCompleteAll);   
    AEAudioUnloadAudio(sfxLevelComplete); 
    AEAudioUnloadAudio(sfxUnlock);        
    AEAudioUnloadAudio(sfxAngryMale);
    AEAudioUnloadAudio(sfxAngryFemale);
    AEAudioUnloadAudio(sfxOnClick);
    AEAudioUnloadAudio(sfxHover);     
    AEAudioUnloadAudio(sfxStack);         
    AEAudioUnloadAudio(sfxPour);
    AEAudioUnloadAudio(sfxLose);

    AEAudioUnloadAudio(sfxdoorClose);
    AEAudioUnloadAudio(sfxTruckMove);
    AEAudioUnloadAudio(sfxTruckBeep);

    AEAudioUnloadAudioGroup(bgmGroup);
    AEAudioUnloadAudioGroup(sfxGroup);

    printf("Sound unloaded\n");
}