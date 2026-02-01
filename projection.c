#include "projection.h"
#include "raylib.h"

Vector2 projectScreen(Vector2 p, Screen screen){
    // Objetivo: x -> 0..WIDTH; y -> HEIGHT..0
    // x: -1..1 (+1) -> 0..2 (/2) -> 0..1 (*WIDTH) -> 0..WIDTH
    // y: -1..1 (+1) -> 0..2 (/2) -> 0..1 (1-) -> 1..0 (*HEIGHT) -> HEIGHT..0 
    float s = (screen.w < screen.h) ? screen.w : screen.h;
    return (Vector2){
        .x = (p.x + 1) * 0.5f * s + (screen.w  - s) * 0.5f,
        .y = (1 - (p.y + 1) * 0.5f) * s + (screen.h - s) * 0.5f
    };
}

Vector2 project(Vector3 vec3, Screen screen){
    if (vec3.z <= 0.01f) vec3.z = 0.01f;
    
    return projectScreen((Vector2) {
        .x = vec3.x / vec3.z,
        .y = vec3.y / vec3.z 
    }, screen);
}

Vector2 cutToVec3(Vector3 vec3){
    return (Vector2){
        .x = vec3.x,
        .y = vec3.y
    };
}

#define VERTICES_SIZE 8
#define AXIS_SIZE 12
#define SPEED .01

void projectScreenPoints(Vector3 *pts, Vector2 *projected, size_t size, Screen screen){
    for(size_t i = 0; i < size; i++){
        projected[i] = project(pts[i], screen);
    }
}

void drawPoints(Vector2 *pts, size_t size, int radius, Color color){
    Color colors[8] = {
      GREEN, BLUE, RED, YELLOW,
      GRAY, PINK, PURPLE, BROWN  
    };

    for (size_t i = 0; i < size; i++) {
        Vector2 p = pts[i];
        DrawCircle(p.x, p.y, radius, colors[i]);    
    }
}

void updatePoint(Vector3 *point, float speed){
    point->z += speed;
}

void updatePoints(Vector3 *pts, size_t size, float speed){
    for (size_t i = 0; i < size; i++) updatePoint(&pts[i], speed);
}

