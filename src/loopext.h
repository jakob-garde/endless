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

void InitExt() {
    // ...

    map = InitForestMap(&a_life);

    s32 grid_x = 0;
    s32 grid_y = 0;

    {
        grid_x = 0;
        grid_y = 0;
        TileInit(&a_life, map, colormap_paletted_autumn, grid_x, grid_y);
        Entity tile = CreateEntity(ET_MAP_TILE, animations);
        tile.tpe = ET_MAP_TILE;
        tile.disable_draw_frames = true;
        tile.hash_key = TileKey(grid_x, grid_y);
        entities.Add(tile);
    }

    {
        grid_x = 1;
        grid_y = 0;
        TileInit(&a_life, map, colormap_paletted_autumn, grid_x, grid_y);
        Entity tile = CreateEntity(ET_MAP_TILE, animations);
        tile.tpe = ET_MAP_TILE;
        tile.disable_draw_frames = true;
        tile.hash_key = TileKey(grid_x, grid_y);
        entities.Add(tile);
    }
}


void UpdateExt(Entity *ent, f32 dt) {
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


void EntityDrawExt(Array<Animation> animations, Entity *ent) {
    // ...
    if (ent->tpe != ET_MAP_TILE) {
        return;
    }

    Tile *t = TileGet(map, ent->hash_key);
    s32 sz = map->tile_sz_px;

    s32 offset_x = t->grid_x * sz * TILE_W;
    s32 offset_y = t->grid_y * sz * TILE_H;

    for (s32 i = 0; i < TILE_W; ++i) {
        for (s32 j = 0; j < TILE_H; ++j) {
            // draw the square given colour

            Color col = t->colors.arr[i * TILE_W + j];
            DrawRectangle(i * sz + offset_x, j * sz + offset_y, sz, sz, col);
        }
    }
}


#endif
