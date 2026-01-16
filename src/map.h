#ifndef __MAP_H__
#define __MAP_H__


#include "raylib.h"
#include "memory.h"


#define MAP_TILES_CAP 255
#define TILE_W 64
#define TILE_H 64

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
    key += grid_x << 31 + grid_y;
    return key;
}

Tile *MapGetTile(Map *map, s32 grid_x, s32 grid_y) {
    u64 key = TileKey( grid_x, grid_y);
    Tile *tile = (Tile*) MapGet(&map->tile_map, key);
    return tile;
}

void MapInitTile(MArena *a_dest, Map *map, s32 grid_x, s32 grid_y) {
    Tile *tile = ArenaAlloc<Tile>(a_dest);
    tile->colors = InitArray<Color>(a_dest, TILE_W * TILE_H);
    tile->grid_x = grid_x;
    tile->grid_y = grid_y;

    u64 key = TileKey(tile->grid_x, tile->grid_y);
    MapPut(&map->tile_map, key, tile);
}

void MapDrawTile(Map *map, Vector2 camera_offset) {

}


#endif
