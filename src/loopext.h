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

    TileInit(&a_life, map, colormap_paletted_autumn, 0, 0);
    entities.Add( TileEntityCreate(0, 0) );

    TileInit(&a_life, map, colormap_paletted_jet, 1, 0);
    entities.Add( TileEntityCreate(1, 0) );

    TileInit(&a_life, map, colormap_paletted_jet, 0, 1);
    entities.Add( TileEntityCreate(0, 1) );

    TileInit(&a_life, map, colormap_paletted_autumn, 1, 1);
    entities.Add( TileEntityCreate(1, 1) );
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
    if (ent->tpe == ET_MAP_TILE) {
        Tile *t = TileGet(map, ent->hash_key);
        assert(t);
        TileDraw(t, map->tile_sz_px);
    }
}


#endif
