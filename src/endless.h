#ifndef __LOOP_ENDLESS_H__
#define __LOOP_ENDLESS_H__


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
    map = EndlessMapInit(&a_life);

    s32 grid_x = 0;
    s32 grid_y = 0;

    TileCreate(&a_life, map, colormap_paletted_autumn, 0, 0);
    TileCreate(&a_life, map, colormap_paletted_jet, 1, 0);
    TileCreate(&a_life, map, colormap_paletted_jet, 0, 1);
    TileCreate(&a_life, map, colormap_paletted_autumn, 1, 1);

    Entity c = {};
    c.tpe = ET_MAP_CROSS;
    c.disable_draw_frames = true;
    cross = entities.Add(c);

    Entity p = {};
    p.tpe = ET_MAP_PAWS;
    p.position = { 100, 100 };
    p.disable_draw_frames = true;
    paws = entities.Add(p);

    // fixed, but random, locations

    s32 tile_sz_px = map->tile_sz_px * TILE_W;

    Entity l = {};
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(tile_sz_px), (f32) Rand(tile_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(tile_sz_px), (f32) Rand(tile_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(tile_sz_px), (f32) Rand(tile_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }
}


#endif
