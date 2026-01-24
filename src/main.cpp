#include "engine/includes.h"
#include "overworld.h"
#include "wfc.h"
#include "endless.h"


void Run() {
    Init("Endless Forest");
    InitEndless();

    // test
    InitWFC(64);
    RunWFC();
    game.SetState(GS_TEST);

    // start
    //game.SetState(GS_OVERWORLD);

    while (!WindowShouldClose()) {
        UpdateBase();
        DrawBegin();

        if (game.state == GS_OVERWORLD) {
            UpdateOverworld();
            DrawOverworld();
        }
        else if (game.state == GS_LOCATION) {
            UpdateLocation();
            DrawLocation();
        }
        else if (game.state == GS_TEST) {

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

            DrawWFC();
        }
        else {
            assert(1 == 0 && "game mode not implemented");
        }

        DrawEnd();
        Swap();
    }

    Close();
}


int main(void) {
    Run();
}
