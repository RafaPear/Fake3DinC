#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "geometry.h"

ProjectedEdge projectEdge(Edge edge, Screen screen){
    Vector2 start = project(*edge.start, screen);
    Vector2 end = project(*edge.end, screen);
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

    int edgesMask[12][2] = {
        {6, 2}, {6, 4}, {6, 7},
        {3, 1}, {3, 2}, {3, 7},
        {0, 1}, {0, 2}, {0, 4},
        {5, 1}, {5, 4}, {5, 7},
    };

    for (int i = 0; i < 8; i++){
        vertices[i] = (Vector3){
            pos.x + ((i&1) ? halfSide : -halfSide),
            pos.y + ((i&2) ? halfSide : -halfSide),
            pos.z + ((i&4) ? halfSide : -halfSide),
        };
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

void projectCube(Cube *cube, ProjectedCube *projected, Screen screen){
    for (int i = 0; i < 8; i++) projected->vertices[i] = project(cube->vertices[i], screen);
    for (int i = 0; i < 12; i++) projected->edges[i] = projectEdge(cube->edges[i], screen);
}

void updateCube(Cube *cube, float delta){
    updatePoints(cube->vertices, 8, delta);
    updateEdges(cube->edges, delta);
}

void drawCube(ProjectedCube *cube, bool drawVertices, bool drawLines, float radius, Color color){
    if (drawVertices) drawPoints(cube->vertices, 8, radius, color);
    if (drawLines) drawEdges(cube->edges, color);
}
