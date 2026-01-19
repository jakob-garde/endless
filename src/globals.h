#ifndef __ASTEROIDS_TYPES_H__
#define __ASTEROIDS_TYPES_H__

#include "raylib.h"


#include "memory.h"
#include "entities.h"


#define ARENA_CAP 1024 * 1024 * 64
u8 arena_mem[ARENA_CAP];
MArena a_life;


enum GameState {
    GS_TITLE,
    GS_OVERWORLD,
    GS_LOCATION,
    GS_TRANSITION,
    GS_END,
};

struct Game {
public:
    GameState state;
    s32 phase_elapsed;

    void SetState(GameState stt) {
        state = stt;
        phase_elapsed = 0;
    }
    GameState GetState() {
        return state;
    }
};
Game game;
Camera2D cam;

bool pause;
bool debug;
bool music_enabled;

Array<Animation> animations;
Array<Entity> entities;
Array<Entity> entities_next;
Array<SEffect> sounds;

Music music_track_x;
Music *music_track;
void SetMusicTrack(Music *track) {
    if (music_track) {
        StopMusicStream(*music_track);
    }
    if (music_enabled) {
        PlayMusicStream(*track);
    }
    music_track = track;
}

f32 screen_w;
f32 screen_h;


s32 location_sz = 12;
Texture tex_forest[7];



// helpers

inline
f32 GetFrameTimeMS() {
    f32 result = GetFrameTime() * 1000;
    if (result == 0) {
        // TODO: fix this hack:
        //      a hack, since the first frame of the game, dt == 0, and we
        //      are counting on elapsed counters to be advanced every frame
        result = 1;
    }
    return result;
}

s32 AnimationGetFirstByType(EntityType tpe) {
    for (s32 i = 0; i < animations.len; ++i) {
        Animation ani = animations.arr[i];
        if (ani.tpe == tpe) {
            return i;
        }
    }
    return {};
}

Array<s32> AnimationGetAllByType(MArena *a_tmp, EntityType tpe) {
    Array<s32> result = {};
    result.InitForExpand(a_tmp);

    for (s32 i = 0; i < animations.len; ++i) {
        Animation ani = animations.arr[i];
        if (ani.tpe == tpe) {
            result.Expand(a_tmp, i);
        }
    }
    return result;
}

Entity *FindFirstEntityByType(EntityType tpe, Array<Entity> entities) {
    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;
        if (ent->tpe == tpe) {
            return ent;
        }
    }
    return NULL;
}

Vector2 Vector2Normalize(Vector2 v, f32 new_len) {
    f32 norm = sqrt( v.x*v.x + v.y*v.y );    
    v.x = v.x / norm * new_len;
    v.y = v.y / norm * new_len;

    return v;
}

#endif
