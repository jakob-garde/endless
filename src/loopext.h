#ifndef __LOOP_EXT_H__
#define __LOOP_EXT_H__


#include "memory.h"
#include "entities.h"
#include "globals.h"
#include "assets.h"
#include "color.h"

// app specific
#include "map.h"


const char *title = "Endless Forest";
Map *map;

Entity *cross;
Entity *paws;

void InitExt() {
    map = InitForestMap(&a_life);

    s32 grid_x = 0;
    s32 grid_y = 0;

    TileInit(&a_life, map, colormap_paletted_autumn, 0, 0);
    entities.Add( TileEntityCreate(0, 0) );

    TileInit(&a_life, map, colormap_paletted_jet, 1, 0);
    entities.Add( TileEntityCreate(1, 0) );

    TileInit(&a_life, map, colormap_paletted_jet, 0, 1);
    entities.Add( TileEntityCreate(0, 1) );

    TileInit(&a_life, map, colormap_paletted_autumn, 1, 1);
    entities.Add( TileEntityCreate(1, 1) );

    Entity c = {};
    c.tpe = ET_MAP_CROSS;
    c.disable_draw_frames = true;
    cross = entities.Add(c);

    Entity p = {};
    p.tpe = ET_MAP_PAWS;
    p.position = { 100, 100 };
    p.disable_draw_frames = true;
    paws = entities.Add(p);
}


void FrameEntityUpdateExt(Entity *ent, f32 dt) {
    // ...

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
}

Vector2 Vector2Normalize(Vector2 v, f32 new_len) {
    f32 norm = sqrt( v.x*v.x + v.y*v.y );    
    v.x = v.x / norm * new_len;
    v.y = v.y / norm * new_len;

    return v;
}

void UpdateExt(f32 dt) {
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

void FrameDrawExt() {
    // post-entity loop draw

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
}

void EntityDrawExt(Array<Animation> animations, Entity *ent) {
    if (ent->tpe == ET_MAP_TILE) {
        Tile *t = TileGet(map, ent->hash_key);
        assert(t);
        TileDraw(t, map->tile_sz_px);
    }
}


#endif
