#ifndef __OVERWORLD_H__
#define __OVERWORLD_H__


#define MAP_TILES_CAP 255
#define CHUNK_W 64
#define CHUNK_H 64
#define CHUNK_LEN CHUNK_W * CHUNK_H
#define TILE_SZ 64


struct MapRegion {
    s32 map_y;
    s32 map_x;
    s32 width_tiles;
    s32 height_tiles;
    u64 hash_key;

    Array<Tile> tiles;
};

struct EndlessMap {
    HashMap chunks;
};

EndlessMap *EndlessMapInit(MArena *a_dest) {
    EndlessMap *map = ArenaAlloc<EndlessMap>(a_dest);
    map->chunks = InitMap(a_dest, MAP_TILES_CAP * 2);

    meadow_frames_DBG = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1).frames;
    forest_frames_DBG = InitAnimation(&a_life, "resources/forest.png", ET_BACKGROUND, 0, 1).frames;

    return map;
}

inline
u64 ChunkKey(s32 grid_x, s32 grid_y) {
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

MapRegion *ChunkGet(EndlessMap *map, s32 grid_x, s32 grid_y) {
    u64 key = ChunkKey( grid_x, grid_y);
    MapRegion *tile = (MapRegion*) MapGet(&map->chunks, key);
    return tile;
}

MapRegion *ChunkGet(EndlessMap *map, u64 hash_key) {
    MapRegion *tile = (MapRegion*) MapGet(&map->chunks, hash_key);
    return tile;
}

void ChunkCreate(MArena *a_dest, EndlessMap *map, s32 grid_x, s32 grid_y) {
    MapRegion *chunk = ArenaAlloc<MapRegion>(a_dest);
    chunk->map_x = grid_x;
    chunk->map_y = grid_y;
    chunk->hash_key = ChunkKey(chunk->map_x, chunk->map_y);

    WFCGrid grid = InitGrid(a_dest, CHUNK_W, CHUNK_H);
    RunWFC(grid);
    SetWFCGridTextures(grid);
    chunk->tiles = grid.tiles;

    MapPut(&map->chunks, chunk->hash_key, chunk);
}


#endif
