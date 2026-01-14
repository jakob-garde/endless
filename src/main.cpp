#include "raylib.h"

// generic
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

void Init() {
    // raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Asteroids");
    InitAudioDevice();
    ToggleFullscreen();
    SetTargetFPS(60);
    screen_w = GetScreenWidth();
    screen_h = GetScreenHeight();

    // always have this screen size:
    //screen_h = fmin(screen_h, 768);

    // usr
    RandInit();

    MArena a = ArenaCreate(arena_mem, ARENA_CAP);
    entities = InitArray<Entity>(&a, 256);
    //entities_next = InitArray<Entity>(&a, 256);
    animations = LoadAssets(&a);
    sounds = LoadSoundEffects(&a);

    // music
    /*
    music_track_x = LoadMusicStream("resources/some_music_track.mp3");

    float volume = 0.5f;
    SetMusicVolume(music_track_x, volume);
    f32 master_volume = 0.3f;
    SetMasterVolume(master_volume);

    SetMusicTrack(&music_track_x);
    */


    // start
    game.SetState(GS_GAME);
}

void Close() {
    UnloadTextures(animations);

    UnloadSounds(sounds);
    UnloadMusicStream(music_track_x);
    CloseAudioDevice();

    CloseWindow();
}

void FrameDrawAndSwap() {
    BeginDrawing();
    ClearBackground(BLACK);

    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        EntityDraw(animations, ent);

        // draw debug info
        if (debug) {
            EntityDrawDebug(ent);
        }
    }

    EndDrawing();

    // copy entitues to next frame
    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->deleted == false) {
            Entity *added = entities_next.Add(*ent);
        }
    }

    // swap entity buffers
    Array<Entity> swap = entities;
    entities = entities_next;
    entities_next = swap;
    entities_next.len = 0;
}

void FrameUpdate() {
    if (IsKeyPressed(KEY_M)) {
        music_enabled = !music_enabled;

        if (music_enabled) {
            PlayMusicStream(*music_track);
        }
        else {
            StopMusicStream(*music_track);
        }
    }
    if (IsKeyPressed(KEY_B)) {
        debug = !debug;
    }
    if (IsKeyPressed(KEY_P)) {
        pause = !pause;
    }

    if (pause) {
        return;
    }
    if (music_enabled) {
        UpdateMusicStream(*music_track);
    }

    // update
    f32 dt = GetFrameTimeMS();
    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->life_frames > 0) {
            ent->life_frames--;
            if (ent->life_frames == 0) {
                ent->deleted = true;
            }
        }

        ent->Update(dt);
    }
}


void Run() {
    Init();

    while (!WindowShouldClose()) {
        if (game.GetState() == GS_END) {
            FrameUpdate();
            FrameDrawAndSwap();
        }
        else {
            // ...
        }

    }

    Close();
}

int main(void) {
    Run();
}
