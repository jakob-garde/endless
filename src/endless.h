#ifndef __ENDLESS_H__
#define __ENDLESS_H__


EndlessMap *map;
s32 location_sz = 12;
Entity paws = {};
Entity cross = {};
Array<Frame> meadow_frames;
Array<Frame> forest_frames;


void InitEndless() {
    map = EndlessMapInit(&a_life);

    meadow_frames_DBG = InitAnimation(&a_life, "resources/meadow.png", ET_BACKGROUND, 0, 1).frames;
    forest_frames_DBG = InitAnimation(&a_life, "resources/forest.png", ET_BACKGROUND, 0, 1).frames;
    ChunkCreate(&a_life, map, 0, 0);
    ChunkCreate(&a_life, map, 0, -1);
    ChunkCreate(&a_life, map, -1, 0);
    ChunkCreate(&a_life, map, -1, -1);

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
    MapRegion *chunk = NULL;
    do {
        chunk = (MapRegion*) MapNextVal(&map->chunks, &iter);
        
        if (chunk) {
            s32 offset_x = chunk->map_x * TILE_SZ * CHUNK_W;
            s32 offset_y = chunk->map_y * TILE_SZ * CHUNK_H;

            for (s32 i = 0; i < CHUNK_W; ++i) {
                for (s32 j = 0; j < CHUNK_H; ++j) {
                    s32 idx = j * CHUNK_W + i;
                    Tile *tile = chunk->tiles.arr + idx;

                    Rectangle destination_rec = {};
                    destination_rec.x = tile->x * TILE_SZ + offset_x;
                    destination_rec.y = tile->y * TILE_SZ + offset_y;
                    destination_rec.width = TILE_SZ;
                    destination_rec.height = TILE_SZ;

                    DrawTexturePro(tile->frame.tex, tile->frame.source, destination_rec, {}, 0, WHITE);
                }
            }
        }
    } while (chunk != NULL);

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
        //Rectangle rect = {cross.position.x - sz/2.0f, cross.position.y - sz/2.0f, (f32) sz, (f32) sz };
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
