#ifndef __OVERWORLD_H__
#define __OVERWORLD_H__


#define MAP_TILES_CAP 255
//#define CHUNK_W 16
//#define CHUNK_H 16
#define CHUNK_W 8
#define CHUNK_H 8
#define TILE_SZ 64


struct Chunk {
    s32 grid_x;
    s32 grid_y;
    Array<Color> colors;
    Array<f32> values;
};

struct EndlessMap {
    HashMap chunks;
};

EndlessMap *EndlessMapInit(MArena *a_dest) {
    EndlessMap *map = ArenaAlloc<EndlessMap>(a_dest);
    map->chunks = InitMap(a_dest, MAP_TILES_CAP * 2);

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

Chunk *ChunkGet(EndlessMap *map, s32 grid_x, s32 grid_y) {
    u64 key = ChunkKey( grid_x, grid_y);
    Chunk *tile = (Chunk*) MapGet(&map->chunks, key);
    return tile;
}

Chunk *ChunkGet(EndlessMap *map, u64 hash_key) {
    Chunk *tile = (Chunk*) MapGet(&map->chunks, hash_key);
    return tile;
}

void ChunkCreate(MArena *a_dest, EndlessMap *map, u8 colormap[64][4], s32 grid_x, s32 grid_y) {
    Chunk *tile = ArenaAlloc<Chunk>(a_dest);
    tile->colors = InitArray<Color>(a_dest, CHUNK_W * CHUNK_H);
    tile->values = InitArray<f32>(a_dest, CHUNK_W * CHUNK_H);
    tile->grid_x = grid_x;
    tile->grid_y = grid_y;

    for (s32 i = 0; i < CHUNK_W; ++i) {
        for (s32 j = 0; j < CHUNK_H; ++j) {
            f32 val = Rand01();
            Color col = ColorMapGet(val, colormap);
            tile->colors.arr[i * CHUNK_W + j] = col;
            tile->values.arr[i * CHUNK_W + j] = val;
        }
    }

    u64 key = ChunkKey(tile->grid_x, tile->grid_y);
    MapPut(&map->chunks, key, tile);
}


#endif
