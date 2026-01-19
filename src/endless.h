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
    TileCreate(&a_life, map, colormap_paletted_autumn, 1, 0);
    TileCreate(&a_life, map, colormap_paletted_autumn, 0, 1);
    TileCreate(&a_life, map, colormap_paletted_autumn, 1, 1);

    Entity c = {};
    c.tpe = ET_MAP_CROSS;
    c.disable_draw_frames = true;
    c.disabled = true;
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

    Texture tex_forest01 = LoadTexture("resources/forest1.png");
    Texture tex_forest02 = LoadTexture("resources/forest2.png");
    Texture tex_forest03 = LoadTexture("resources/forest3.png");
    Texture tex_pine01 = LoadTexture("resources/forest4.png");
    Texture tex_pine02 = LoadTexture("resources/forest5.png");
    Texture tex_meadow01 = LoadTexture("resources/forest6.png");
    Texture tex_meadow02 = LoadTexture("resources/forest7.png");
    tex_forest[0] = tex_forest01;
    tex_forest[1] = tex_forest02;
    tex_forest[2] = tex_forest03;
    tex_forest[3] = tex_pine01;
    tex_forest[4] = tex_pine02;
    tex_forest[5] = tex_meadow01;
    tex_forest[6] = tex_meadow02;
}


#endif
