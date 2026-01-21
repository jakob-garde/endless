#ifndef __WFC_H__
#define __WFC_H__


enum TileFlags {
    TF_PASSIVE = 0,

    TF_FLOWERS = 1 << 0,
    TF_ROCKS = 1 << 1,
    TF_GRASS = 1 << 2,
};

enum TileType {
    TT_SUPERPOSITION,
    TT_FLOWERS,
    TT_ROCKS,
    TT_GRASS,
};

struct Tile {
    TileType tpe;
    u32 flags;
    Frame frame;
    s32 x;
    s32 y;
};


Tile tiles_rand[CHUNK_W * CHUNK_H];
Tile tiles[CHUNK_W * CHUNK_H];

u8 mem_tmp[ARENA_CAP];
MArena _a_tmp;
MArena *a_tmp;
Animation ani;

void InitWFC() {
    _a_tmp = ArenaCreate(mem_tmp, ARENA_CAP);
    a_tmp = &_a_tmp;

    ani = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1);

    for (s32 i = 0; i < CHUNK_W; ++i) {
        for (s32 j = 0; j < CHUNK_H; ++j) {
            s32 idx = j * CHUNK_W + i;

            Tile tile = {};

            // get a random texture
            s32 rand = Rand(ani.frames.len - 1) + 1;

            // collapse into a certain state
            if (rand <= 2) {
                tile.tpe = TT_FLOWERS;
            }
            else if (rand >= 7) {
                tile.tpe = TT_ROCKS;
            }
            else {
                tile.tpe = TT_GRASS;
            }
            tile.frame = ani.frames.arr[ rand ];

            tiles_rand[idx] = tile;
        }
    }

    for (s32 i = 0; i < CHUNK_W; ++i) {
        for (s32 j = 0; j < CHUNK_H; ++j) {
            s32 idx = j * CHUNK_W + i;

            Tile tile = {};
            tile.tpe = TT_SUPERPOSITION;
            tile.flags |= TF_FLOWERS;
            tile.flags |= TF_ROCKS;
            tile.flags |= TF_GRASS;

            // get a random texture
            tile.frame = ani.frames.arr[ 0 ];

            tiles[idx] = tile;
        }
    }
}

void RunWFCIteration(MArena *a_tmp) {

    // find the min-entropy subset set
    Array<s32> least = InitArray<s32>(a_tmp, CHUNK_W * CHUNK_H);

    for (s32 i = 0; i < CHUNK_W * CHUNK_H; ++i) {
        Tile *t = tiles + i;

        if (t->tpe == TT_SUPERPOSITION) {
            least.Add(i);
        }
    }

    // collapse a random tile
    s32 idx = least.arr[Rand(least.len)];
    Tile *t = tiles + idx;
    s32 selector = Rand(3);

    if (t->flags & TF_ROCKS && selector == 2) {
        t->tpe = TT_GRASS;
        s32 rand_flower_idx = Rand(3) + 6;
        t->frame = ani.frames.arr[ rand_flower_idx ];
    }
    else if (t->flags & TF_FLOWERS && selector == 1) {
        t->tpe = TT_GRASS;
        s32 rand_flower_idx = Rand(2) + 1;
        t->frame = ani.frames.arr[ rand_flower_idx ];
    }
    else if (t->flags & TF_GRASS) {
        t->tpe = TT_GRASS;
        s32 rand_flower_idx = Rand(3) + 3;
        t->frame = ani.frames.arr[ rand_flower_idx ];
    }
}


void DrawWFC() {
    {
        s32 offset_x = 0 * TILE_SZ * CHUNK_W;
        s32 offset_y = 0 * TILE_SZ * CHUNK_H;

        for (s32 j = 0; j < CHUNK_H; ++j) {
            for (s32 i = 0; i < CHUNK_W; ++i) {
                s32 idx = j * CHUNK_W + i;
                Tile tile = tiles_rand[idx];

                f32 x = i * TILE_SZ + offset_x;
                f32 y = j * TILE_SZ + offset_y;

                Rectangle dest = {};
                dest.x = x;
                dest.y = y;
                dest.width = TILE_SZ;
                dest.height = TILE_SZ;
                DrawTexturePro(tile.frame.tex, tile.frame.source, dest, {}, 0, WHITE);
            }
        }
    }

    {
        s32 offset_x = 1 * TILE_SZ * CHUNK_W;
        s32 offset_y = 0 * TILE_SZ * CHUNK_H;

        for (s32 j = 0; j < CHUNK_H; ++j) {
            for (s32 i = 0; i < CHUNK_W; ++i) {
                s32 idx = j * CHUNK_W + i;
                Tile tile = tiles[idx];

                f32 x = i * TILE_SZ + offset_x;
                f32 y = j * TILE_SZ + offset_y;

                Rectangle dest = {};
                dest.x = x;
                dest.y = y;
                dest.width = TILE_SZ;
                dest.height = TILE_SZ;
                DrawTexturePro(tile.frame.tex, tile.frame.source, dest, {}, 0, WHITE);
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        RunWFCIteration(a_tmp);
    }

    ArenaClear(a_tmp);
}


#endif
