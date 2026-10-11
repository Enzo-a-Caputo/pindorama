#include "raylib.h"

constexpr int   SCREEN_WIDTH  = 1280;
constexpr int   SCREEN_HEIGHT = 720;
constexpr int   TARGET_FPS    = 60;
constexpr int   TILE_SIZE     = 16;

constexpr const char* GAME_TITLE = "Pindorama";

int main(){
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);

    //teste 
    Texture2D image = LoadTexture("assets/sprites/tiles/Path_Middle.png");
    //teste
    Vector2 imagePos{0.f, 0.f};


    SetTargetFPS(TARGET_FPS);

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGREEN);
        
        //teste
        DrawTextureEx(image, imagePos, 0.f, 4.f, WHITE);
        EndDrawing();
    }
//teste
    UnloadTexture(image);
    CloseWindow();

    return 0;
}