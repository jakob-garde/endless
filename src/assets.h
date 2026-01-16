#ifndef __ASSETS_H__
#define __ASSETS_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"


Array<SEffect> LoadSoundEffects(MArena *a_dest) {
    sounds = InitArray<SEffect>(a_dest, 64);

    // ...

    return sounds;
}

Array<Animation> LoadAssets(MArena *a_dest) {
    animations = InitArray<Animation>(a_dest, 64);

    // ...

    return animations;
}

void LoadMusic() {
    // music
    //music_track_x = LoadMusicStream("resources/some_music_track.mp3");

    float volume = 0.5f;
    SetMusicVolume(music_track_x, volume);
    f32 master_volume = 0.3f;
    SetMasterVolume(master_volume);

    SetMusicTrack(&music_track_x);
}



#endif
