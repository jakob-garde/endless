#ifndef __WFC_H__
#define __WFC_H__

/*
enum TileFlags {
    TF_PASSIVE = 0,

    TF_FLOWERS = 1 << 0,
    TF_ROCKS = 1 << 1,
    TF_GRASS = 1 << 2,
};
*/

enum TileType {
    TT_SUPERPOSITION,
    TT_FLOWERS,
    TT_ROCKS,
    TT_GRASS,

    TT_CNT,
};


// CPP:
//      methods
//      initialization using {} (set non-zero values)
//      ZII (combined with the above exceptions)


struct Tile {
    TileType tpe_collapsed;
    bool options_mem[TT_CNT];
    s32 options_cnt = TT_CNT;
    bool is_collapsed;
    Frame frame;
    s32 x;
    s32 y;

    void InitSuperposition() {
        Enable(TT_SUPERPOSITION);
        for (s32 i = 0; i < options_cnt; ++i) {
            options_mem[i] = true;
        }
        options_mem[0] = false;
    }

    s32 Entropy() {
        s32 entropy = 0;
        for (s32 i = 0; i < options_cnt; ++i) {
            if (options_mem[i]) {
                entropy++;
            }
        }
        assert(entropy <= 3);
        return entropy;
    }

    void Enable(TileType tpe) {
        is_collapsed = false;
        options_mem[tpe] = true;
    }

    void Disable(TileType tpe) {
        assert(tpe != TT_SUPERPOSITION);
        options_mem[tpe] = false;
    }

    void Collapse(TileType tpe) {
        assert(tpe < TT_CNT);

        is_collapsed = true;
        for (s32 i = 0; i < options_cnt; ++i) {
            options_mem[i] = false;
        }
        options_mem[tpe] = true;
        tpe_collapsed = tpe;
    }

    TileType CollapseRandom() {
        is_collapsed = true;
        s32 entropy = Entropy();
        s32 dest = Rand(entropy);

        TileType tpe = TT_SUPERPOSITION;
        for (s32 i = 0; i < entropy; ++i) {
            s32 j = i;
            while (options_mem[j] == false) {
                j++;
                assert(j < options_cnt);
            }
            if (i == dest) {
                tpe = (TileType) j;
                assert(tpe != TT_SUPERPOSITION);
                assert(tpe != TT_CNT);
                Collapse(tpe);
                break;
            }
        }
        assert(tpe != TT_SUPERPOSITION);
        return tpe;
    }
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

    for (s32 j = 0; j < CHUNK_H; ++j) {
        for (s32 i = 0; i < CHUNK_W; ++i) {
            s32 idx = j * CHUNK_W + i;

            Tile tile = {};
            tile.x = i;
            tile.y = j;

            // get a random texture
            s32 rand = Rand(ani.frames.len - 1) + 1;

            // collapse into a certain state
            if (rand <= 2) {
                tile.Collapse(TT_FLOWERS);
            }
            else if (rand >= 7) {
                tile.Collapse(TT_ROCKS);
            }
            else {
                tile.Collapse(TT_GRASS);
            }
            tile.frame = ani.frames.arr[ rand ];

            tiles_rand[idx] = tile;
        }
    }

    for (s32 j = 0; j < CHUNK_H; ++j) {
        for (s32 i = 0; i < CHUNK_W; ++i) {
            s32 idx = j * CHUNK_W + i;

            Tile tile = {};
            tile.x = i;
            tile.y = j;
            tile.InitSuperposition();

            // get a random texture
            tile.frame = ani.frames.arr[ 0 ];

            tiles[idx] = tile;
        }
    }
}


s32 TileIdx(s32 x, s32 y) {
    return x + CHUNK_W * y;
}

s32 TileX(s32 idx) {
    return idx % CHUNK_W;
}

s32 TileY(s32 idx) {
    return idx / CHUNK_W - 1;
}

TileType tpe_selected;

void RunWFCIteration(MArena *a_tmp) {

    // find the min-entropy subset set
    Array<s32> least = InitArray<s32>(a_tmp, CHUNK_LEN);

    s32 entropy_min = TT_CNT;
    for (s32 i = 0; i < CHUNK_LEN; ++i) {
        Tile *t = tiles + i;

        s32 entropy = t->Entropy();
        //if ((t->is_collapsed == false) && (entropy_min > entropy)) {
        //    entropy_min = entropy;
        //}
        if ((t->is_collapsed == false)) {
            entropy_min = entropy;
        } 
    }
    if (entropy_min == TT_CNT) {
        assert(1 == 0 && "finished or error");
    }

    for (s32 i = 0; i < CHUNK_LEN; ++i) {
        Tile *t = tiles + i;

        if ((t->is_collapsed == false) && (t->Entropy() == entropy_min)) {
            least.Add(i);
        }
    }

    // collapse a random tile from the least-set
    s32 idx = least.arr[Rand(least.len)];
    Tile *t = tiles + idx;

    // collapse and set animation frame 
    {
        TileType tpe = t->CollapseRandom();
        tpe_selected = tpe;

        if (tpe == TT_FLOWERS) {
            s32 rand_flower_idx = Rand(2) + 1;
            t->frame = ani.frames.arr[ rand_flower_idx ];
        }
        else if (tpe == TT_GRASS) {
            s32 rand_flower_idx = Rand(3) + 3;
            t->frame = ani.frames.arr[ rand_flower_idx ];
        }
        else if (tpe == TT_ROCKS) {
            s32 rand_flower_idx = Rand(3) + 6;
            t->frame = ani.frames.arr[ rand_flower_idx ];
        }
    }

    // update neighbouring tiles
    Tile *left = NULL;
    Tile *right = NULL;
    Tile *above = NULL;
    Tile *below = NULL;

    if (t->x > 0 && t->x < CHUNK_W) {
        left = tiles + TileIdx(t->x - 1, t->y);
        right = tiles + TileIdx(t->x + 1, t->y);
    }
    else if (t->x == 0) {
        right = tiles + TileIdx(t->x + 1, t->y);
    }
    else if (t->x == CHUNK_W) {
        left = tiles + TileIdx(t->x - 1, t->y);
    }

    if (t->y > 0 && t->y < CHUNK_H) {
        above = tiles + TileIdx(t->x, t->y - 1);
        below = tiles + TileIdx(t->x, t->y + 1);
    }
    else if (t->y == 0) {
        below = tiles + TileIdx(t->x, t->y + 1);
    }
    else if (t->y == CHUNK_H) {
        above = tiles + TileIdx(t->x, t->y - 1);
    }

    if (t->tpe_collapsed == TT_ROCKS) {
        if (left) {
            left->Disable(TT_FLOWERS);
        }
        if (right) {
            right->Disable(TT_FLOWERS);
        }
        if (above) {
            above->Disable(TT_FLOWERS);
        }
        if (below) {
            below->Disable(TT_FLOWERS);
        }
    }
    else if (t->tpe_collapsed == TT_FLOWERS) {
        if (left) {
            left->Disable(TT_ROCKS);
        }
        if (right) {
            right->Disable(TT_ROCKS);
        }
        if (above) {
            above->Disable(TT_ROCKS);
        }
        if (below) {
            below->Disable(TT_ROCKS);
        }
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

                if (tile.is_collapsed == false) {
                    s32 entropy = tile.Entropy();
                    DrawText( TextFormat("%d", entropy), x + TILE_SZ/2, y + TILE_SZ/2, 24, BLACK);
                }
            }
        }
    }

    //DrawText( TextFormat("%d", tpe_selected), 20, 20, 48, BLACK);

    if (IsKeyPressed(KEY_SPACE)) {
        RunWFCIteration(a_tmp);
    }

    ArenaClear(a_tmp);
}


#endif
