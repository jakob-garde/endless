#include "engine/includes.h"
#include "overworld.h"
#include "wfc.h"
#include "endless.h"


void Run() {
    Init("Endless Forest");
    InitEndless();

    // test
    InitWFC();
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
