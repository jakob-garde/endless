#ifndef __WFC_H__
#define __WFC_H__


//
//  lib


enum TileType {
    TT_GRASS,
    TT_FLOWERS,
    TT_ROCKS,

    /*
    TT_FOREST_UP,
    TT_FOREST_DOWN,
    TT_FOREST_LEFT,
    TT_FOREST_RIGHT,
    TT_FOREST_INNTER,
    */

    TT_CNT,
};

u8 adjacency_matrix[4][TT_CNT][TT_CNT] = {
    { // left
        { 1, 1, 1 }, // grass left 
        { 1, 1, 0 }, // flowers left
        { 1, 0, 1 }, // rocks left
    },

    { // right
        { 1, 1, 1 }, 
        { 1, 1, 0 },
        { 1, 0, 1 },
    },

    { // up
        { 1, 1, 1 }, 
        { 1, 1, 0 },
        { 1, 0, 1 },
    },

    { // down
        { 1, 1, 1 }, 
        { 1, 1, 0 },
        { 1, 0, 1 },
    },
};

struct Tile {
    bool options[TT_CNT]; // by-tile
    s32 entropy;
    s32 x;
    s32 y;
    s32 idx;

    TileType collapsed_type;
    Frame frame;
};

inline
Tile InitTile(s32 grid_stride, s32 idx) {
    Tile tile = {};
    tile.x = idx % grid_stride;
    tile.y = idx / grid_stride;
    tile.idx = idx;
    tile.entropy = TT_CNT;
    tile.collapsed_type = TT_CNT;
    for (s32 i = 0; i < TT_CNT; ++i) {
        tile.options[i] = true;
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
    // returns the first tile with maximum entropy

    // find max entropy
    s32 entropy_max = 0;
    for (s32 i = 0; i < grid.grid_h * grid.grid_w; ++i) {
        Tile *tile = grid.tiles.arr + i;
        if (tile->entropy > entropy_max) {
            entropy_max = tile->entropy;
            if (entropy_max == TT_CNT) {
                break;
            }
        }
    }

    // no more iterations needed
    if (entropy_max > 1) {
        for (s32 i = 0; i < grid.grid_h * grid.grid_w; ++i) {
            Tile *t = grid.tiles.arr + i;
            if (t->entropy == entropy_max) {
                return i;
            }
        }
    }

    return -1;
}

inline
TileType SelectRandomOption(bool *options, s32 tile_entropy) {
    s32 select_target = Rand(tile_entropy);
    s32 select_idx = 0;
    for (s32 i = 0; i < TT_CNT; ++i) {
        if (options[i]) {
            if (select_idx == select_target) {
                return (TileType) i;
            }
            else {
                select_idx++;
            }
        }
    }

    assert(1 == 0);
    return TT_CNT;
}

void TileCollapse(Tile *tile) {
    tile->collapsed_type = SelectRandomOption(tile->options, tile->entropy);
    memset(tile->options, 0, TT_CNT);
    tile->options[tile->collapsed_type] = 1;
    tile->entropy = 1;
}

void Collapse(Grid grid, s32 tile_idx) {
    Tile *tile = grid.tiles.arr + tile_idx;

    // collapse to a random, available option
    TileCollapse(tile);

    // kernel
    s32 adjacency_kernel_x[4] = { 0, 0, -1, 1 }; // up down left right
    s32 adjacency_kernel_y[4] = { -1, 1, 0, 0 };

    // iterate reduction matrix for tile type
    for (s32 adjacent_tpe = 0; adjacent_tpe < TT_CNT; ++adjacent_tpe) {

        // apply the kernel to loop over neighbours
        for (s32 j = 0; j < 4; ++j) {
            u8 *adjacency = adjacency_matrix[j][tile->collapsed_type]; 

            if (adjacency[adjacent_tpe] == true) {
                continue;
            }

            s32 neighbour_x = (tile->x + adjacency_kernel_x[j]) % grid.grid_w;
            if (neighbour_x == -1) { neighbour_x = grid.grid_w - 1; }
            s32 neighbour_y = (tile->y + adjacency_kernel_y[j]) % grid.grid_h;
            if (neighbour_y == -1) { neighbour_y = grid.grid_h - 1; }
            s32 neighbour_idx = neighbour_y * grid.grid_w + neighbour_x;

            Tile *neighbour = grid.tiles.arr + neighbour_idx;
            assert( (neighbour->x != tile->x) || (neighbour->y != tile->y) );

            if (neighbour->entropy == 1) {
                continue;
            }
            else {
                if (neighbour->options[adjacent_tpe]) {
                    neighbour->options[adjacent_tpe] = false;
                    neighbour->entropy--;

                    if (neighbour->entropy == 1) {
                        TileCollapse(neighbour);
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
Array<Frame> forest_frames;

void InitWFC(s32 grid_size) {
    grid = InitGrid(&a_life, grid_size, grid_size);
    meadow_frames = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1).frames;
    forest_frames = InitAnimation(&a_life, "resources/forest.png", ET_BACKGROUND, 0, 1).frames;
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


Frame GetTileFrameByType(TileType tpe) {
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
void DrawWFC_DBG() {
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

            // set frame
            if (tile->entropy == 1 && tile->frame.source.width == 0) {
                tile->frame = GetTileFrameByType((TileType) tile->collapsed_type);
            }
            DrawText(TextFormat("%d", tile->entropy), destination_rec.x + WFC_TILE_SIZE/2, destination_rec.y + WFC_TILE_SIZE/2, 16, WHITE);

            if (tile->entropy == 1) {
                DrawTexturePro(tile->frame.tex, tile->frame.source, destination_rec, {}, 0, WHITE);
            }
        }
    }
}


#endif
