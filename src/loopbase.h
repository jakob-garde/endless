#ifndef __LOOP_BASE_H__
#define __LOOP_BASE_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"
#include "assets.h"
#include "endless.h"


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
    game.SetState(GS_OVERWORLD);
}

void Update() {
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

        if (game.state == GS_OVERWORLD && ent->tpe == ET_MAP_LOCATION) {
            if (CheckCollisionPointCircle(paws->position, ent->position, location_sz) && IsKeyPressed(KEY_ENTER)) {
                game.SetState(GS_LOCATION);
                return;
            }
        }
    }

    if (game.state == GS_OVERWORLD) {
        if (IsKeyPressed(KEY_UP)) {
            cam.target.y -= map->tile_sz_px;
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            cam.target.y += map->tile_sz_px;
        }
        else if (IsKeyPressed(KEY_LEFT)) {
            cam.target.x -= map->tile_sz_px;
        }
        else if (IsKeyPressed(KEY_RIGHT)) {
            cam.target.x += map->tile_sz_px;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            cross->position = GetMousePosition();
            cross->position.x += cam.target.x;
            cross->position.y += cam.target.y;
            cross->disabled = false;

            paws->velocity.x = cross->position.x - paws->position.x;
            paws->velocity.y = cross->position.y - paws->position.y;
            paws->velocity = Vector2Normalize(paws->velocity, 0.1f);
        }

        if (CheckCollisionPointCircle(paws->position, cross->position, 4)) {
            paws->velocity = {};
            cross->disabled = true;
        }
    }
    else if (game.state == GS_LOCATION) {
        if (IsKeyPressed(KEY_ENTER)) {
            game.SetState(GS_OVERWORLD);
            return;
        }
    }
}

void DrawAndSwap() {
    BeginDrawing();

    BeginMode2D(cam);

    if (game.state == GS_OVERWORLD) {
        ClearBackground(BLACK);

        // draw all map tiles
        MapIter iter = {};
        Tile *t = NULL;
        do {
            t = (Tile*) MapNextVal(&map->tile_map, &iter);
            if (t) {
                TileDraw(t, map->tile_sz_px);
            }
        } while (t != NULL);

        // draw entities
        for (s32 i = 0; i < entities.len; ++i) {
            Entity *ent = entities.arr + i;

            if (ent->disable_draw_frames == false) {
                EntityDrawFrame(animations, ent);
            }

            if (ent->tpe == ET_MAP_LOCATION) {
                DrawCircleV(ent->position, location_sz, COLOR_GREEN);
            }

            if (debug) {
                EntityDrawDebug(ent);
            }
        }

        // draw the map cross
        if (cross->disabled == false) {
            s32 sz = 16;
            Rectangle rect = { cross->position.x - sz/2.0f, cross->position.y - sz/2.0f, (f32) sz, (f32) sz };
            DrawLineEx( { cross->position.x - sz/2.0f, cross->position.y - sz/2.0f }, { cross->position.x + sz/2.0f, cross->position.y + sz/2.0f }, 8, COLOR_WHITE );
            DrawLineEx( { cross->position.x + sz/2.0f, cross->position.y - sz/2.0f }, { cross->position.x - sz/2.0f, cross->position.y + sz/2.0f }, 8, COLOR_WHITE );
            DrawLineEx( { cross->position.x - sz/2.4f, cross->position.y - sz/2.4f }, { cross->position.x + sz/2.4f, cross->position.y + sz/2.4f }, 4, COLOR_BLACK );
            DrawLineEx( { cross->position.x + sz/2.4f, cross->position.y - sz/2.4f }, { cross->position.x - sz/2.4f, cross->position.y + sz/2.4f }, 4, COLOR_BLACK );
        }

        // draw the paws
        if (paws->disabled == false) {
            DrawCircleV( paws->position, 8, COLOR_WHITE );
            DrawCircleV( paws->position, 4, COLOR_BLACK );
        }

        // draw the connector
        // TODO: impl.
    }
    else if (game.state == GS_LOCATION) {
        ClearBackground(WHITE);
        DrawText("Location", 200, 200,  48, BLACK);
    }

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
        if (game.state == GS_OVERWORLD) {
            Update();
            DrawAndSwap();
        }
        else if (game.state == GS_LOCATION) {
            Update();
            DrawAndSwap();
        }
        else {
            // ...
        }

    }

    Close();
}


#endif
