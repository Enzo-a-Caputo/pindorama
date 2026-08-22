#include "raylib.h"

constexpr int   SCREEN_WIDTH  = 1280;
constexpr int   SCREEN_HEIGHT = 720;
constexpr int   TARGET_FPS    = 60;
constexpr int   TILE_SIZE     = 16;

constexpr const char* GAME_TITLE = "Pindorama";

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
    SetTargetFPS(TARGET_FPS);

    while(!WindowShouldClose()){
        BeginDrawing();
            ClearBackground(DARKGREEN);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}