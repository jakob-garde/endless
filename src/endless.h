#ifndef __LOOP_ENDLESS_H__
#define __LOOP_ENDLESS_H__


EndlessMap *map;
Texture tex_forest[7];
s32 location_sz = 12;
Entity paws = {};
Entity cross = {};


void InitEndless() {
    map = EndlessMapInit(&a_life);

    s32 grid_x = 0;
    s32 grid_y = 0;

    ChunkCreate(&a_life, map, colormap_paletted_autumn, 0, 0);
    ChunkCreate(&a_life, map, colormap_paletted_autumn, 1, 0);
    ChunkCreate(&a_life, map, colormap_paletted_autumn, 0, 1);
    ChunkCreate(&a_life, map, colormap_paletted_autumn, 1, 1);

    cross.tpe = ET_MAP_CROSS;
    cross.disable_draw_frames = true;
    cross.disabled = true;

    paws.tpe = ET_MAP_PAWS;
    paws.position = { 100, 100 };
    paws.disable_draw_frames = true;

    // fixed, but random, locations

    s32 chunk_sz_px = TILE_SZ * CHUNK_W;

    Entity l = {};
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(chunk_sz_px), (f32) Rand(chunk_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(chunk_sz_px), (f32) Rand(chunk_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }
    {
        l.tpe = ET_MAP_LOCATION;
        l.position  = { (f32) Rand(chunk_sz_px), (f32) Rand(chunk_sz_px) };
        l.disable_draw_frames = true;
        entities.Add(l);
    }

    Texture tex_forest01 = LoadTexture("resources/forest1.png");
    Texture tex_forest02 = LoadTexture("resources/forest2.png");
    Texture tex_forest03 = LoadTexture("resources/forest3.png");
    Texture tex_pine01 = LoadTexture("resources/forest4.png");
    Texture tex_pine02 = LoadTexture("resources/forest5.png");
    Texture tex_meadow01 = LoadTexture("resources/forest6.png");
    Texture tex_meadow02 = LoadTexture("resources/forest7.png");
    tex_forest[0] = tex_forest01;
    tex_forest[1] = tex_forest02;
    tex_forest[2] = tex_forest03;
    tex_forest[3] = tex_pine01;
    tex_forest[4] = tex_pine02;
    tex_forest[5] = tex_meadow01;
    tex_forest[6] = tex_meadow02;
}

void UpdateOverworld() {
    f32 dt = GetFrameTimeMS();

    paws.Update(dt);
    cross.Update(dt);

    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->life_frames > 0) {
            ent->life_frames--;
            if (ent->life_frames == 0) {
                ent->deleted = true;
            }
        }
        ent->Update(dt);

        if (game.state == GS_OVERWORLD) {
            if (CheckCollisionPointCircle(paws.position, ent->position, location_sz) && IsKeyPressed(KEY_ENTER)) {
                game.SetState(GS_LOCATION);
                return;
            }
        }
    }

    if (IsKeyPressed(KEY_UP)) {
        cam.target.y -= TILE_SZ;
    }
    else if (IsKeyPressed(KEY_DOWN)) {
        cam.target.y += TILE_SZ;
    }
    else if (IsKeyPressed(KEY_LEFT)) {
        cam.target.x -= TILE_SZ;
    }
    else if (IsKeyPressed(KEY_RIGHT)) {
        cam.target.x += TILE_SZ;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        cross.position = GetMousePosition();
        cross.position.x += cam.target.x;
        cross.position.y += cam.target.y;
        cross.disabled = false;

        paws.velocity.x = cross.position.x - paws.position.x;
        paws.velocity.y = cross.position.y - paws.position.y;
        paws.velocity = Vector2Normalize(paws.velocity, 0.1f);
    }

    if (CheckCollisionPointCircle(paws.position, cross.position, 4)) {
        paws.velocity = {};
        cross.disabled = true;
    }
}

void UpdateLocation() {
    if (IsKeyPressed(KEY_ENTER)) {
        game.SetState(GS_OVERWORLD);
        return;
    }
}

void DrawOverworld() {
    ClearBackground(BLACK);

    // draw all map tiles
    MapIter iter = {};
    Chunk *t = NULL;
    do {
        t = (Chunk*) MapNextVal(&map->chunks, &iter);
        if (t) {
            s32 offset_x = t->grid_x * TILE_SZ * CHUNK_W;
            s32 offset_y = t->grid_y * TILE_SZ * CHUNK_H;

            for (s32 i = 0; i < CHUNK_W; ++i) {
                for (s32 j = 0; j < CHUNK_H; ++j) {
                    // draw the square given colour
                    //Color col = t->colors.arr[i * TILE_W + j];
                    //DrawRectangle(i * square_sz + offset_x, j * square_sz + offset_y, square_sz, square_sz, col);

                    // blit a texture into it
                    f32 x = i * TILE_SZ + offset_x;
                    f32 y = j * TILE_SZ + offset_y;
                    f32 val7 = t->values.arr[i * CHUNK_W + j] * 7;
                    s32 idx = ((s32) floor(val7)) % 7;
                    DrawTextureEx(tex_forest[idx], {x, y}, 0, 4, WHITE);
                }
            }
        }
    } while (t != NULL);

    // draw entities
    for (s32 i = 0; i < entities.len; ++i) {
        Entity *ent = entities.arr + i;

        if (ent->disable_draw_frames == false) {
            EntityDrawFrame(animations, ent);
        }

        if (ent->tpe == ET_MAP_LOCATION) {
            DrawCircleV(ent->position, location_sz, COLOR_GREEN);
        }

        if (debug) {
            EntityDrawDebug(ent);
        }
    }

    // draw the map cross
    if (cross.disabled == false) {
        s32 sz = 16;
        Rectangle rect = {cross.position.x - sz/2.0f, cross.position.y - sz/2.0f, (f32) sz, (f32) sz };
        DrawLineEx( { cross.position.x - sz/2.0f, cross.position.y - sz/2.0f }, { cross.position.x + sz/2.0f, cross.position.y + sz/2.0f }, 8, COLOR_WHITE );
        DrawLineEx( { cross.position.x + sz/2.0f, cross.position.y - sz/2.0f }, { cross.position.x - sz/2.0f, cross.position.y + sz/2.0f }, 8, COLOR_WHITE );
        DrawLineEx( { cross.position.x - sz/2.4f, cross.position.y - sz/2.4f }, { cross.position.x + sz/2.4f, cross.position.y + sz/2.4f }, 4, COLOR_BLACK );
        DrawLineEx( { cross.position.x + sz/2.4f, cross.position.y - sz/2.4f }, { cross.position.x - sz/2.4f, cross.position.y + sz/2.4f }, 4, COLOR_BLACK );
    }

    // draw the paws
    if (paws.disabled == false) {
        DrawCircleV( paws.position, 8, COLOR_WHITE );
        DrawCircleV( paws.position, 4, COLOR_BLACK );
    }

    // draw the connector
    // TODO: impl.
}

void DrawLocation() {
    ClearBackground(WHITE);
    DrawText("Location", 200, 200,  48, BLACK);
}


#endif
