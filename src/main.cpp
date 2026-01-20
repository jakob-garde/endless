#include "includes.h"


void Run() {
    Init("Endless Forest");
    InitEndless();

    // start
    game.SetState(GS_OVERWORLD);

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
