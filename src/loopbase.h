#ifndef __LOOP_BASE_H__
#define __LOOP_BASE_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"
#include "assets.h"
#include "loopext.h"


void Init() {
    // raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), "Asteroids");
    InitAudioDevice();
    ToggleFullscreen();
    SetTargetFPS(60);
    screen_w = GetScreenWidth();
    screen_h = GetScreenHeight();
    //screen_h = fmin(screen_h, 768);

    // usr
    RandInit();

    MArena a = ArenaCreate(arena_mem, ARENA_CAP);
    entities = InitArray<Entity>(&a, 256);
    //entities_next = InitArray<Entity>(&a, 256);
    animations = LoadAssets(&a);
    sounds = LoadSoundEffects(&a);

    // start
    game.SetState(GS_GAME);
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

        UpdateExt(ent, dt);
    }
}

void FrameDrawAndSwap() {
    BeginDrawing();
    ClearBackground(BLACK);

    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        EntityDraw(animations, ent);

        EntityDrawExt(animations, ent);

        // draw debug info
        if (debug) {
            EntityDrawDebug(ent);
        }
    }

    DrawText("Debug Text", 0, 0, 48, WHITE);
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

void Close() {
    UnloadTextures(animations);

    UnloadSounds(sounds);
    UnloadMusicStream(music_track_x);
    CloseAudioDevice();

    CloseWindow();
}

void Run() {
    Init();

    while (!WindowShouldClose()) {
        if (game.GetState() == GS_GAME) {
            FrameUpdate();
            FrameDrawAndSwap();
        }
        else {
            // ...
        }

    }

    Close();
}


#endif
