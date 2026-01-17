#ifndef __LOOP_BASE_H__
#define __LOOP_BASE_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"
#include "assets.h"
#include "loopext.h"


void Init(const char *title) {
    // raylib
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(GetMonitorWidth(0), GetMonitorHeight(0), title);
    InitAudioDevice();
    ToggleFullscreen();
    SetTargetFPS(60);
    screen_w = GetScreenWidth();
    screen_h = GetScreenHeight();

    // usr
    RandInit();

    a_life = ArenaCreate(arena_mem, ARENA_CAP);
    entities = InitArray<Entity>(&a_life, 256);
    entities_next = InitArray<Entity>(&a_life, 256);
    animations = LoadAssets(&a_life);
    sounds = LoadSoundEffects(&a_life);

    cam.offset = {};
    cam.zoom = 1;

    //
    InitExt();

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

        UpdateEntityExt(ent, dt);
    }

    UpdateExt(dt);
}

void FrameDrawAndSwap() {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(cam);

    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->disable_draw_frames == false) {
            EntityDrawFrame(animations, ent);
        }

        DrawEntityExt(animations, ent);

        if (debug) {
            EntityDrawDebug(ent);
        }
    }

    DrawExt();

    EndMode2D();

    DrawText("Debug Text", 0, 0, 48, WHITE);

    EndDrawing();

    // copy entitues to next frame
    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->deleted == false) {
            Entity *added = entities_next.Add(*ent);


            // TODO: we should have a system for this
            if (added->tpe == ET_MAP_CROSS) {
                cross = added;
            }
            else if (added->tpe == ET_MAP_PAWS) {
                paws = added;
            }
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
    Init(title);

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
