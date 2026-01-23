#ifndef __WFC_H__
#define __WFC_H__


enum TileType {
    TT_GRASS,
    TT_FLOWERS,
    TT_ROCKS,
    // ...

    TT_CNT,
};

s8 reduction_matrix[TT_CNT][TT_CNT] = {
    {
        -1,
        -1,
        -1
    },
    {
        (s8) TT_ROCKS,
        -1,
        -1 },
    {
        (s8) TT_FLOWERS,
        -1,
        -1
    }
};

// TODO: we should write a convenient init function for the reduction matrix
//      because that is much easier to edit and expand.


//u8 reduce_left[TT_CNT];
//u8 reduce_right[TT_CNT];
//u8 reduce_up[TT_CNT];
//u8 reduce_down[TT_CNT];


struct Tile {
    s8 options[TT_CNT]; // by-tile
    s32 entropy;
    s32 x;
    s32 y;
    s32 idx;

    s32 Left(s32 grid_w, s32 grid_h) {
        s32 x_new;
        if (x == 0) {
            x_new = grid_w - 1;
        }
        else {
            x_new = x - 1;
        }
        return y * grid_w + x_new;
    }

    s32 Right(s32 grid_w, s32 grid_h) {
        s32 x_new;
        if (x == grid_w - 1) {
            x_new = 0;
        }
        else {
            x_new = x + 1;
        }
        return y * grid_w + x_new;
    }

    s32 Up(s32 grid_w, s32 grid_h) {
        s32 y_new;
        if (y == 0) {
            y_new = grid_h - 1;
        }
        else {
            y_new = y - 1;
        }
        return y_new * grid_w + x;
    }

    s32 Down(s32 grid_w, s32 grid_h) {
        s32 y_new;
        if (y == grid_h - 1) {
            y_new = 0;
        }
        else {
            y_new = y + 1;
        }
        return y_new * grid_w + x;
    }

    void Reduce(s8 tpe) {
        if (entropy == 1) {
            return;
        }
        for (s32 i = 0; i < TT_CNT; ++i) {
            if (options[i] == tpe) {
                // swap
                options[i] = options[entropy-1];
                options[entropy] = 0;
                entropy--;
                assert(entropy > 0);
                return;
            }
        }
    }
};

inline
Tile InitTile(s32 grid_stride, s32 idx) {
    Tile t = {};
    t.x = idx % grid_stride;
    t.y = idx / grid_stride;
    t.idx = idx;
    t.entropy = TT_CNT;
    for (s32 i = 0; i < TT_CNT; ++i) {
        t.options[i] = i;
    }
    return t;
}

struct Grid {
    Array<Tile> tiles;
    s32 grid_w;
    s32 grid_h;
};

Grid InitGrid(MArena *a_dest, s32 w, s32 h) {
    Grid grid = {};
    grid.grid_w = w;
    grid.grid_h = h;
    grid.tiles = InitArray<Tile>(a_dest, w * h);
    for (s32 i = 0; i < w * h; ++i) {
        grid.tiles.Add( InitTile(w, i) );
    }
    return grid;
}

s32 SelectTile(Grid grid) {
    // just the first one, not a random one !
    s32 entropy_max = 0;
    for (s32 i = 0; i < grid.grid_h * grid.grid_w; ++i) {
        Tile *t = grid.tiles.arr + i;
        if (t->entropy > entropy_max) {
            entropy_max = t->entropy;
        }
    }

    // no more iterations needed
    if (entropy_max == 1) {
        return -1;
    }
    else {
        for (s32 i = 0; i < grid.grid_h * grid.grid_w; ++i) {
            Tile *t = grid.tiles.arr + i;
            if (t->entropy == entropy_max) {
                return i;
            }
        }
    }

    return -1;
}

void Collapse(Grid grid, s32 idx) {
    // collapse this tile
    Tile *t = grid.tiles.arr + idx;
    s8 tpe = t->options[Rand(t->entropy)];

    memset(t->options, 0, TT_CNT);
    t->options[0] = tpe;
    t->entropy = 1;

    // apply transition matrix to neighbours
    s8 *red = reduction_matrix[tpe];
    Tile *s = NULL;
    for (s32 i = 0; i < TT_CNT; ++i) {
        u8 reduce = red[i];
        if (reduce == -1) {
            break;
        }

        s = grid.tiles.arr + t->Left(grid.grid_w, grid.grid_h);
        s->Reduce(reduce);
        s = grid.tiles.arr + t->Right(grid.grid_w, grid.grid_h);
        s->Reduce(reduce);
        s = grid.tiles.arr + t->Up(grid.grid_w, grid.grid_h);
        s->Reduce(reduce);
        s = grid.tiles.arr + t->Down(grid.grid_w, grid.grid_h);
        s->Reduce(reduce);
    }
}

u8 mem_tmp[ARENA_CAP];
MArena _a_tmp;
MArena *a_tmp;
Grid grid;

Animation ani;
void InitWFC() {
    _a_tmp = ArenaCreate(mem_tmp, ARENA_CAP);
    a_tmp = &_a_tmp;

    grid = InitGrid(&a_life, 8, 8);
    ani = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1);
}

void RunWFCIteration() {
    s32 idx = SelectTile(grid);
    Collapse(grid, idx);
}

void RunWFC() {
    s32 idx = SelectTile(grid);
    while (idx >= 0) {
        Collapse(grid, idx);
        idx = SelectTile(grid);
    }
}

Frame GetFrame(TileType tpe) {
    if (tpe == TT_FLOWERS) {
        return ani.frames.arr[1];
    }
    else if (tpe == TT_GRASS) {
        return ani.frames.arr[4];
    }
    else if (tpe == TT_ROCKS) {
        return ani.frames.arr[6];
    }
    return ani.frames.arr[0];
}

void DrawWFC() {
    ClearBackground(BLACK);

    s32 tile_sz = 64;

    s32 offset_x = 0 * tile_sz * grid.grid_w;
    s32 offset_y = 0 * tile_sz * grid.grid_h;

    for (s32 j = 0; j < grid.grid_h; ++j) {
        for (s32 i = 0; i < grid.grid_w; ++i) {
            s32 idx = j * grid.grid_w + i;
            Tile tile = grid.tiles.arr[idx];

            Rectangle dest = {};
            dest.x = tile.x * tile_sz;
            dest.y = tile.y * tile_sz;
            dest.width = tile_sz;
            dest.height = tile_sz;

            f32 x = i * tile_sz + offset_x;
            f32 y = j * tile_sz + offset_y;

            DrawText(TextFormat("%d", tile.entropy), dest.x + tile_sz/2, dest.y + tile_sz/2, 16, WHITE);

            if (tile.entropy == 1) {
                Frame frame = GetFrame((TileType) tile.options[0]);
                DrawTexturePro(frame.tex, frame.source, dest, {}, 0, WHITE);
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        RunWFCIteration();
    }

    ArenaClear(a_tmp);
}


#endif
