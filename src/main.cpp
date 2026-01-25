#include "engine/includes.h"
#include "wfc.h"
#include "overworld.h"
#include "endless.h"


void Run() {
    Init("Endless Forest");
    InitEndless();

    // test
    WFCGrid test_grid = InitWFC(64);
    RunWFC(test_grid);

    // start
    game.SetState(GS_OVERWORLD);

    while (!WindowShouldClose()) {
        UpdateBase();
        DrawBegin();

        if (game.state == GS_OVERWORLD) {
            UpdateOverworld();
            DrawOverworld();

            if (IsKeyPressed(KEY_T)) {
                game.SetState(GS_TEST);
                cam.target.x = 0;
                cam.target.y = 0;
            }
        }
        else if (game.state == GS_LOCATION) {
            UpdateLocation();
            DrawLocation();
        }
        else if (game.state == GS_TEST) {

            if (IsKeyDown(KEY_UP)) {
                cam.target.y -= 8;
            }
            else if (IsKeyDown(KEY_DOWN)) {
                cam.target.y += 8;
            }
            else if (IsKeyDown(KEY_LEFT)) {
                cam.target.x -= 8;
            }
            else if (IsKeyDown(KEY_RIGHT)) {
                cam.target.x += 8;
            }

            cam.target.x += 1;
            cam.target.y += 1;

            if (IsKeyPressed(KEY_T)) {
                game.SetState(GS_OVERWORLD);
            }

            DrawWFC_DBG(test_grid);
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
