#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>
#include "geometry.h"

#define WIDTH 800
#define HEIGHT 600
#define FPS 144
#define SPEED .05

int main(void) {
    printf("Initializing Window...\n");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_INTERLACED_HINT);
    InitWindow(WIDTH, HEIGHT, "Fake 3D");

    SetTargetFPS(FPS);

    Screen screen = {WIDTH, HEIGHT};

    Cube *cube = createCube((Vector3){0, 0, 20}, 10, RED);
    if (!cube) return -1;
    ProjectedCube *projected = createProjectedCube();
    float radius = 50;
    float angle = .01;
    Vector3 speed = {0, 0, 0};
    long count = 0xFF;

    printf("App Started\n");
    
    while(!WindowShouldClose()){
        screen.w = GetScreenWidth();
        screen.h = GetScreenHeight();
        // radius -= SPEED;
        // rotateCubeZAxisLocalSpace(cube, angle);
        rotateCubeYAxisLocalSpace(cube, angle);
        // rotateCubeXAxisLocalSpace(cube, angle);
        // if (count!=0) count--;
        updateCube(cube, speed);
        for (int i = 0; i < 8; i++){
            if (cube->vertices[i].z >= 80|| cube->vertices[i].z < 10) {
                speed.z = -speed.z;
                break;
            }
        }
        projectCube(cube, projected, screen);
        
        BeginDrawing();
            ClearBackground(BLACK);
            drawCube(projected, false, 10, WHITE);
            DrawFPS(0, 0); 
        EndDrawing();
    }
    freeCube(cube);
    freeProjectedCube(projected);

    CloseWindow();
    
    return 0;
}
