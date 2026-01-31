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
#define SPEED .01

int main(void) {
    printf("Initializing Window...\n");
    InitWindow(WIDTH, HEIGHT, "Fake 3D");

    SetTargetFPS(FPS);

    Screen screen = {WIDTH, HEIGHT};

    Cube *cube = createCube((Vector3){0, 0, 20}, 10);
    if (!cube) return -1;
    ProjectedCube *projected = createProjectedCube();
    // float radius = 50;
    float angle = .01;
    
    while(!WindowShouldClose()){
        //radius -= SPEED;
        // rotateCubeZAxisLocalSpace(cube, angle);
        rotateCubeYAxisLocalSpace(cube, angle);
        // rotateCubeXAxisLocalSpace(cube, angle);
        //updateCube(cube, SPEED);
        projectCube(cube, projected, screen);
        
        BeginDrawing();
            ClearBackground(BLACK);
            drawCube(projected, false, true, 10, WHITE);
            DrawFPS(0, 0); 
        EndDrawing();
    }
    freeCube(cube);
    freeProjectedCube(projected);

    CloseWindow();
    
    return 0;
}
