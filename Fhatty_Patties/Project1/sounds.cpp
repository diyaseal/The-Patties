#include "pch.hpp"

// ---------------------------------------------------------------------------
// Audio assets and groups
// ---------------------------------------------------------------------------

//BGM
static AEAudio bgmAudio;

//SFX
static AEAudio sfxOnClick;
static AEAudio sfxCorrect;
static AEAudio sfxAngryMale;
static AEAudio sfxAngryFemale;
static AEAudio sfxdoorClose;
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
    sfxOnClick = AEAudioLoadMusic("Assets/Audio/onClick.mp3");
    sfxAngryMale = AEAudioLoadMusic("Assets/Audio/mAngry.mp3");
    sfxAngryFemale = AEAudioLoadMusic("Assets/Audio/wAngry.mp3");
    sfxCorrect = AEAudioLoadMusic("Assets/Audio/custHappy.mp3");
    sfxdoorClose = AEAudioLoadSound("Assets/Audio/doorClose.wav");

    printf("Sound loaded\n");
}

// ---------------------------------------------------------------------------
// Sound_PlayBGM
// ---------------------------------------------------------------------------
void Sound_PlayBGM() {
    if (AEAudioIsValidAudio(bgmAudio) && AEAudioIsValidGroup(bgmGroup))
        AEAudioPlay(bgmAudio, bgmGroup, 0.5f, 1.0f, -1);
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
    AEAudioUnloadAudio(sfxCorrect);
    AEAudioUnloadAudio(sfxAngryMale);
    AEAudioUnloadAudio(sfxAngryFemale);
    AEAudioUnloadAudio(sfxOnClick);

    AEAudioUnloadAudioGroup(bgmGroup);
    AEAudioUnloadAudioGroup(sfxGroup);

    printf("Sound unloaded\n");
}