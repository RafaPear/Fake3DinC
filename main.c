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

int main(int argc, char **argv) {
    int targetFPS = FPS;
    bool fpsLimitEnabled = true;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-fps-limit") == 0) {
            fpsLimitEnabled = false;
        } else if (strcmp(argv[i], "--fps") == 0 && i + 1 < argc) {
            targetFPS = atoi(argv[++i]);
            fpsLimitEnabled = true;
        }
    }
    printf("Initializing Window...\n");
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_INTERLACED_HINT);
    InitWindow(WIDTH, HEIGHT, "Fake 3D");

    if (fpsLimitEnabled) {
        SetTargetFPS(targetFPS);
    }

    Screen screen = {WIDTH, HEIGHT};

    Cube *cube = createCube((Vector3){0, 0, 20}, 10, RED);
    if (!cube) return -1;
    ProjectedCube *projected = createProjectedCube();
    float angularSpeed = 0.5f;
    Vector3 speed = {0, 0, 0};

    printf("App Started\n");
    printf("FPS Limit: %s\n", fpsLimitEnabled ? "Enabled" : "Disabled");
    
    while(!WindowShouldClose()){
        float deltaTime = GetFrameTime();
        
        if (IsKeyPressed(KEY_V)) {
            fpsLimitEnabled = !fpsLimitEnabled;
            if (fpsLimitEnabled) {
                SetTargetFPS(targetFPS);
            } else {
                SetTargetFPS(0);
            }
            printf("FPS Limit: %s\n", fpsLimitEnabled ? "Enabled" : "Disabled");
        }
        
        screen.w = GetScreenWidth();
        screen.h = GetScreenHeight();
        
        rotateCubeYAxisLocalSpace(cube, angularSpeed * deltaTime);
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
            DrawText(fpsLimitEnabled ? TextFormat("FPS Limit: %d", targetFPS) : "FPS Limit: OFF", 0, 20, 20, GREEN);
        EndDrawing();
    }
    freeCube(cube);
    freeProjectedCube(projected);

    CloseWindow();
    
    return 0;
}
