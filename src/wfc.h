#ifndef __WFC_H__
#define __WFC_H__


//
//  lib


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
        Tile *tile = grid.tiles.arr + i;
        if (tile->entropy > entropy_max) {
            entropy_max = tile->entropy;
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


//
//  user


Grid grid;
Array<Frame> meadow_frames;

void InitWFC(s32 grid_size) {
    grid = InitGrid(&a_life, grid_size, grid_size);
    meadow_frames = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1).frames;
}

void RunWFCIteration() {
    s32 idx = SelectTile(grid);
    Collapse(grid, idx);
}

void RunWFC() {
    s32 tile_idx = SelectTile(grid);
    while (tile_idx >= 0) {
        Collapse(grid, tile_idx);
        tile_idx = SelectTile(grid);
    }
}


//
//  drawing


Frame GetFrame(TileType tpe) {
    // indices refer to the contents of the map tile sprite sheet (three of each variant)
    s32 rand_index = Rand(3);
    if (tpe == TT_FLOWERS) {
        return meadow_frames.arr[rand_index];
    }
    else if (tpe == TT_GRASS) {
        return meadow_frames.arr[rand_index + 3];
    }
    else if (tpe == TT_ROCKS) {
        return meadow_frames.arr[rand_index + 6];
    }
    return meadow_frames.arr[0];
}

#define WFC_TILE_SIZE 64
void DrawWFC() {
    ClearBackground(BLACK);

    for (s32 j = 0; j < grid.grid_h; ++j) {
        for (s32 i = 0; i < grid.grid_w; ++i) {
            s32 idx = j * grid.grid_w + i;
            Tile *tile = grid.tiles.arr + idx;

            Rectangle destination_rec = {};
            destination_rec.x = tile->x * WFC_TILE_SIZE;
            destination_rec.y = tile->y * WFC_TILE_SIZE;
            destination_rec.width = WFC_TILE_SIZE;
            destination_rec.height = WFC_TILE_SIZE;

            if (tile->is_collapsed == false) {
                tile->is_collapsed = true;
                tile->frame = GetFrame( (TileType) tile->options[0]);
            }

            DrawText(TextFormat("%d", tile->entropy), destination_rec.x + WFC_TILE_SIZE/2, destination_rec.y + WFC_TILE_SIZE/2, 16, WHITE);

            if (tile->entropy == 1) {
                DrawTexturePro(tile->frame.tex, tile->frame.source, destination_rec, {}, 0, WHITE);
            }
        }
    }
}


#endif
