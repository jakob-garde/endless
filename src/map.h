#ifndef __MAP_H__
#define __MAP_H__


#include "raylib.h"
#include "memory.h"


#define MAP_TILES_CAP 255
#define TILE_W 16
#define TILE_H 16

struct Tile {
    s32 grid_x;
    s32 grid_y;
    Array<Color> colors;
};

struct Map {
    HashMap tile_map;
    s32 tile_sz_px;
};

Map *InitForestMap(MArena *a_dest) {
    Map *map = ArenaAlloc<Map>(a_dest);
    map->tile_map = InitMap(a_dest, MAP_TILES_CAP * 2);
    map->tile_sz_px = 32;

    return map;
}

inline
u64 TileKey(s32 grid_x, s32 grid_y) {
    u64 key = ((u64) 1) << 63; // avoid 0,0 resulting in key == 0
    u32 half_largest_u32 = ((u32) -1) / 2;
    if (grid_x < 0) {
        grid_x = grid_x + half_largest_u32; 
        assert(grid_x >= 0);
    }
    if (grid_y < 0) {
        grid_y = grid_y + half_largest_u32; 
        assert(grid_y >= 0);
    }
    key += (grid_x << 31) + grid_y;
    return key;
}

Tile *TileGet(Map *map, s32 grid_x, s32 grid_y) {
    u64 key = TileKey( grid_x, grid_y);
    Tile *tile = (Tile*) MapGet(&map->tile_map, key);
    return tile;
}

Tile *TileGet(Map *map, u64 hash_key) {
    Tile *tile = (Tile*) MapGet(&map->tile_map, hash_key);
    return tile;
}

void TileInit(MArena *a_dest, Map *map, u8 colormap[64][4], s32 grid_x, s32 grid_y) {
    Tile *tile = ArenaAlloc<Tile>(a_dest);
    tile->colors = InitArray<Color>(a_dest, TILE_W * TILE_H);
    tile->grid_x = grid_x;
    tile->grid_y = grid_y;

    for (s32 i = 0; i < TILE_W; ++i) {
        for (s32 j = 0; j < TILE_H; ++j) {
            Color col = ColorMapGet(Rand01(), colormap);
            tile->colors.arr[i * TILE_W + j] = col;
        }
    }

    u64 key = TileKey(tile->grid_x, tile->grid_y);
    MapPut(&map->tile_map, key, tile);
}

Entity TileEntityCreate(s32 grid_x, s32 grid_y) {
    Entity tile = CreateEntity(ET_MAP_TILE, animations);
    tile.tpe = ET_MAP_TILE;
    tile.disable_draw_frames = true;
    tile.hash_key = TileKey(grid_x, grid_y);

    return tile;
}

inline
void TileDraw(Tile *t, s32 square_sz) {
    s32 offset_x = t->grid_x * square_sz * TILE_W;
    s32 offset_y = t->grid_y * square_sz * TILE_H;

    for (s32 i = 0; i < TILE_W; ++i) {
        for (s32 j = 0; j < TILE_H; ++j) {
            // draw the square given colour

            Color col = t->colors.arr[i * TILE_W + j];
            DrawRectangle(i * square_sz + offset_x, j * square_sz + offset_y, square_sz, square_sz, col);
        }
    }
}


#endif
