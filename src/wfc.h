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

struct Tile {
    s8 options[TT_CNT]; // by-tile
    s32 entropy;
    s32 x;
    s32 y;
    s32 idx;

    bool is_collapsed;
    Frame frame;
};

inline
Tile InitTile(s32 grid_stride, s32 idx) {
    Tile tile = {};
    tile.x = idx % grid_stride;
    tile.y = idx / grid_stride;
    tile.idx = idx;
    tile.entropy = TT_CNT;
    for (s32 i = 0; i < TT_CNT; ++i) {
        tile.options[i] = i;
    }
    return tile;
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

void Collapse(Grid grid, s32 tile_idx) {
    // collapse to a random, available option
    Tile *tile = grid.tiles.arr + tile_idx;
    s8 tpe = tile->options[Rand(tile->entropy)];
    memset(tile->options, 0, TT_CNT);
    tile->options[0] = tpe;
    tile->entropy = 1;

    // type transition matrix
    s8 *red = reduction_matrix[tpe]; 

    // kernel
    s32 reduction_kernel_x[4] = { -1, 1, 0, 0 }; // left right up down
    s32 reduction_kernel_y[4] = { 0, 0, -1, 1 };

    // iterate reduction matrix
    for (s32 i = 0; i < TT_CNT; ++i) {
        u8 reduce = red[i];
        if (reduce == -1) {
            break;
        }

        // apply the kernel to loop over neighbours
        for (s32 j = 0; j < 4; ++j) {
            s32 neighbour_x = (tile->x + reduction_kernel_x[j]) % grid.grid_w;
            s32 neighbour_y = (tile->y + reduction_kernel_y[j] / grid.grid_w) % grid.grid_h;
            s32 neighbour_idx = neighbour_y * grid.grid_w + neighbour_x;
            Tile *neighbour = grid.tiles.arr + neighbour_idx;

            if (tile->entropy > 1) {
                // search for the option in the tile's list of available options
                for (s32 k = 0; k < TT_CNT; ++i) {
                    if (neighbour->options[k] == tpe) {
                        // swap
                        neighbour->options[i] = neighbour->options[neighbour->entropy-1];
                        neighbour->options[neighbour->entropy] = 0;
                        // update
                        neighbour->entropy -= 1;

                        assert(neighbour->entropy > 0);
                        break;
                    }
                }
            }
        }
    }
}

Grid grid;
Animation ani;

void InitWFC() {
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
        s32 rand = Rand(2) + 1;
        return ani.frames.arr[rand];
    }
    else if (tpe == TT_GRASS) {
        s32 rand = Rand(3) + 3;
        return ani.frames.arr[rand];
    }
    else if (tpe == TT_ROCKS) {
        s32 rand = Rand(3) + 6;
        return ani.frames.arr[rand];
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
            Tile *t = grid.tiles.arr + idx;

            Rectangle dest = {};
            dest.x = t->x * tile_sz;
            dest.y = t->y * tile_sz;
            dest.width = tile_sz;
            dest.height = tile_sz;

            f32 x = i * tile_sz + offset_x;
            f32 y = j * tile_sz + offset_y;

            if (t->is_collapsed == false) {
                t->is_collapsed = true;
                t->frame = GetFrame( (TileType) t->options[0]);
            }

            DrawText(TextFormat("%d", t->entropy), dest.x + tile_sz/2, dest.y + tile_sz/2, 16, WHITE);

            if (t->entropy == 1) {
                DrawTexturePro(t->frame.tex, t->frame.source, dest, {}, 0, WHITE);
            }
        }
    }

    if (IsKeyPressed(KEY_SPACE)) {
        RunWFCIteration();
    }
}


#endif
