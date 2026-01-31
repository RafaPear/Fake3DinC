#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 144

Vector2 screen(Vector2 p){
    // Objetivo: x -> 0..WIDTH; y -> HEIGHT..0
    // x: -1..1 (+1) -> 0..2 (/2) -> 0..1 (*WIDTH) -> 0..WIDTH
    // y: -1..1 (+1) -> 0..2 (/2) -> 0..1 (1-) -> 1..0 (*HEIGHT) -> HEIGHT..0 
    float s = (WIDTH < HEIGHT) ? WIDTH : HEIGHT;
    return (Vector2){
        .x = (p.x + 1) * 0.5f * s + (WIDTH  - s) * 0.5f,
        .y = (1 - (p.y + 1) * 0.5f) * s + (HEIGHT - s) * 0.5f
    };
}

Vector2 project(Vector3 vec3){
    if (vec3.z <= 0.01f) vec3.z = 0.01f;
    
    return screen((Vector2) {
        .x = vec3.x / vec3.z,
        .y = vec3.y / vec3.z 
    });
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

void projectScreenPoints(Vector3 *pts, Vector2 *projected, size_t size){
    for(int i = 0; i < size; i++){
        projected[i] = project(pts[i]);
    }
}

void drawPoints(Vector2 *pts, size_t size, int radius, Color color){
    for (int i = 0; i < size; i++) {
        Vector2 p = pts[i];
        DrawCircle(p.x, p.y, radius, color);    
    }
}

void updatePoint(Vector3 *point, float speed){
    point->z += speed;
}

void updatePoints(Vector3 *pts, size_t size, float speed){
    for (int i = 0; i < size; i++) updatePoint(&pts[i], speed);
}

typedef struct Edge{
    Vector3 *start;
    Vector3 *end;
} Edge;

typedef struct ProjectedEdge {
    Vector2 start;
    Vector2 end;
} ProjectedEdge;

ProjectedEdge projectEdge(Edge edge){
    Vector2 start = project(*edge.start);
    Vector2 end = project(*edge.end);
    return (ProjectedEdge){
        .start = start,
        .end = end
    };
}

void updateEdge(Edge *edge, float speed){
    updatePoint(edge->start, speed);
    updatePoint(edge->end, speed);
}

void updateEdges(Edge *edges, float speed){
    for (int i = 0; i < 12; i++) updateEdge(&edges[i], speed);
}

void drawEdge(ProjectedEdge edge, Color color){
    DrawLine(edge.start.x, edge.start.y, edge.end.x, edge.end.y, color);    
}

void drawEdges(ProjectedEdge *edges, Color color){
    for (int i = 0; i < 12; i++) drawEdge(edges[i], color);
}

typedef struct Cube{
    Vector3 *vertices;
    Edge *edges;
    int side;  
} Cube;

typedef struct ProjectedCube {
    Vector2 *vertices;
    ProjectedEdge *edges;
} ProjectedCube;

Cube *createCube(Vector3 pos, int side){
    Vector3 *vertices = malloc(sizeof(Vector3)*8);
    Edge *edges = malloc(sizeof(Edge)*12);
    Cube *cube = malloc(sizeof(Cube));

    if (!vertices){
        printf("Could not create vertices array.\n");
        return NULL;
    }
    printf("Created vertices array.\n");
    
    if (!edges){
        printf("Could not create edges array.\n");
        return NULL;
    }
    printf("Created edges array.\n");
    
    if (!cube){
        printf("Could not create cube.\n");
        return NULL;
    }
    printf("Created cube.\n");
    
    cube->vertices = vertices;
    cube->edges = edges;
    cube->side = side;
    int halfSide = side/2;

    printf("Initalized Cube.\n");

    Vector3 mask[] = {
        {1, 1, 1},
        {-1, 1, 1},
        {1, -1, 1},
        {-1, -1, 1},
    };

    int edgesMask[12][2] = {
        {6, 2}, {6, 4}, {6, 7},
        {3, 1}, {3, 2}, {3, 7},
        {0, 1}, {0, 2}, {0, 4},
        {5, 1}, {5, 4}, {5, 7},
    };

    for(int i = 0; i < 4; i++){
        Vector3 entry = mask[i];
        entry.x = entry.x * halfSide + pos.x;
        entry.y = entry.y * halfSide + pos.y;
        entry.z = entry.z * halfSide + pos.z;
        vertices[i] = entry;
    }
    
    for(int i = 4; i < 8; i++){
        Vector3 entry = mask[i-4];
        entry.x = entry.x * halfSide + pos.x;
        entry.y = entry.y * halfSide + pos.y;
        entry.z = entry.z * -halfSide + pos.z;
        vertices[i] = entry;
    }

    printf("Populated vertices.\n");

    for(int i = 0; i < 12; i++){
        int a = edgesMask[i][0];
        int b = edgesMask[i][1];
        edges[i] = (Edge){ &vertices[a], &vertices[b] };
    }

    printf("Populated edges.\n");

    return cube;
}

bool freeCube(Cube *cube){
    if(!cube) return false;
    free(cube->vertices);
    free(cube->edges);
    return true;
}

// | x cos θ + z sin θ|   |x'|
// |         y        | = |y'|
// |−x sin θ + z cos θ|   |z'|
//
// x' = x*cos(t) + z*sin(t)
// y' = y
// z' = -x*sin(t) + z*cos(t)
void rotateCubeYAxisWorldSpace(Cube *cube, float angle){
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];
        float x = v->x;
        float z = v->z;

        v->x =  x * c + z * s;
        v->z = -x * s + z * c;
    }
}

// |x cos θ − y sin θ|   |x'|
// |x sin θ + y cos θ| = |y'|
// |        z        |   |z'|
//
// x' = x*cos(t) - y*sin(t)
// y' = x*sin(t) + y*cos(t)
// z' = z
void rotateCubeZAxisWorldSpace(Cube *cube, float angle){
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];
        float x = v->x;
        float y = v->y;

        v->x = x * c - y * s;
        v->y = x * s + y * c;
    }
}

// |        x        |   |x'|
// |y cos θ − z sin θ| = |y'|
// |y sin θ + z cos θ|   |z'|
//
// x' = x
// y' = y*cos(t) - z*sin(t)
// z' = y*sin(t) + z*cos(t)
void rotateCubeXAxisWorldSpace(Cube *cube, float angle){
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];
        float y = v->y;
        float z = v->z;

        v->y = y * c - z * s;
        v->z = y * s + z * c;
    }
}

// c = (1/N) * Σ v_i
// centro = média aritmética dos vértices
Vector3 cubeCenter(Cube *cube){
    Vector3 c = {0, 0, 0};

    for (int i = 0; i < 8; i++){
        c.x += cube->vertices[i].x;
        c.y += cube->vertices[i].y;
        c.z += cube->vertices[i].z;
    }

    c.x /= 8.0f;
    c.y /= 8.0f;
    c.z /= 8.0f;

    return c;
}

// v' = T⁻¹ · R_y(θ) · T · v
//
// 1) Translação para a origem:
//    x₀ = x - cx
//    z₀ = z - cz
//
// 2) Rotação (igual à world space):
//    x₁ =  x₀*cos(θ) + z₀*sin(θ)
//    z₁ = -x₀*sin(θ) + z₀*cos(θ)
//
// 3) Translação inversa:
//    x' = x₁ + cx
//    z' = z₁ + cz
void rotateCubeYAxisLocalSpace(Cube *cube, float angle){
    Vector3 c0 = cubeCenter(cube);
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float x0 = v->x - c0.x;
        float z0 = v->z - c0.z;

        float x1 =  x0 * c + z0 * s;
        float z1 = -x0 * s + z0 * c;

        v->x = x1 + c0.x;
        v->z = z1 + c0.z;
    }
}

// v' = T⁻¹ · R_z(θ) · T · v
//
// 1) Translação:
//    x₀ = x - cx
//    y₀ = y - cy
//
// 2) Rotação:
//    x₁ = x₀*cos(θ) - y₀*sin(θ)
//    y₁ = x₀*sin(θ) + y₀*cos(θ)
//
// 3) Translação inversa:
//    x' = x₁ + cx
//    y' = y₁ + cy
void rotateCubeZAxisLocalSpace(Cube *cube, float angle){
    Vector3 c0 = cubeCenter(cube);
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float x0 = v->x - c0.x;
        float y0 = v->y - c0.y;

        float x1 = x0 * c - y0 * s;
        float y1 = x0 * s + y0 * c;

        v->x = x1 + c0.x;
        v->y = y1 + c0.y;
        // z permanece inalterado
    }
}

// v' = T⁻¹ · R_x(θ) · T · v
//
// 1) Translação:
//    y₀ = y - cy
//    z₀ = z - cz
//
// 2) Rotação:
//    y₁ = y₀*cos(θ) - z₀*sin(θ)
//    z₁ = y₀*sin(θ) + z₀*cos(θ)
//
// 3) Translação inversa:
//    y' = y₁ + cy
//    z' = z₁ + cz
void rotateCubeXAxisLocalSpace(Cube *cube, float angle){
    Vector3 c0 = cubeCenter(cube);
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float y0 = v->y - c0.y;
        float z0 = v->z - c0.z;

        float y1 = y0 * c - z0 * s;
        float z1 = y0 * s + z0 * c;

        v->y = y1 + c0.y;
        v->z = z1 + c0.z;
        // x permanece inalterado
    }
}

ProjectedCube *createProjectedCube(){
    Vector2 *vertices = malloc(sizeof(Vector2)*8);
    ProjectedEdge *edges = malloc(sizeof(ProjectedEdge)*12);
    ProjectedCube *cube = malloc(sizeof(ProjectedCube));
    cube->vertices = vertices;
    cube->edges = edges;
    return cube;
}

bool freeProjectedCube(ProjectedCube *cube){
    if(!cube) return false;
    free(cube->vertices);
    free(cube->edges);
    return true;
}

void projectCube(Cube *cube, ProjectedCube *projected){
    for (int i = 0; i < 8; i++) projected->vertices[i] = project(cube->vertices[i]);
    for (int i = 0; i < 12; i++) projected->edges[i] = projectEdge(cube->edges[i]);
}

void updateCube(Cube *cube, float delta){
    updatePoints(cube->vertices, 8, delta);
    updateEdges(cube->edges, delta);
}

void drawCube(ProjectedCube *cube, bool drawVertices, bool drawLines, float radius, Color color){
    if (drawVertices) drawPoints(cube->vertices, 8, radius, color);
    if (drawLines) drawEdges(cube->edges, color);
}

int main(void) {
    printf("Initializing Window...\n");
    InitWindow(WIDTH, HEIGHT, "Fake 3D");

    SetTargetFPS(FPS);

    Cube *cube = createCube((Vector3){0, 0, 20}, 10);
    if (!cube) return -1;
    ProjectedCube *projected = createProjectedCube();
    float radius = 50;
    float angle = .01;
    
    while(!WindowShouldClose()){
        //radius -= SPEED;
        // rotateCubeZAxisLocalSpace(cube, angle);
        rotateCubeYAxisLocalSpace(cube, angle);
        // rotateCubeXAxisLocalSpace(cube, angle);
        //updateCube(cube, SPEED);
        projectCube(cube, projected);
        
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
