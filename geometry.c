#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "geometry.h"
#include "raylib.h"

ProjectedEdge projectEdge(Edge edge, Screen screen){
    Vector2 start = project(*edge.start, screen);
    Vector2 end = project(*edge.end, screen);
    return (ProjectedEdge){
        .start = start,
        .end = end
    };
}

ProjectedFace projectFace(Face face, Screen screen){
    Vector2 topR = project(*face.topR, screen);
    Vector2 topL = project(*face.topL, screen);
    Vector2 bottomR = project(*face.bottomR, screen);
    Vector2 bottomL = project(*face.bottomL, screen);
    return (ProjectedFace){
        .topR    = topR,
        .topL    = topL,
        .bottomR = bottomR,
        .bottomL = bottomL,
        .color   = face.color
    };
}

void updateEdge(Edge *edge, Vector3 delta){
    updatePoint(edge->start, delta);
    updatePoint(edge->end, delta);
}

void updateEdges(Edge *edges, Vector3 delta){
    for (int i = 0; i < 12; i++) updateEdge(&edges[i], delta);
}

long clamp(long a, long min, long max){
    if (a < min) return min;
    if (a > max) return max;
    return a;
}

void updateFacesColor(Face *faces, Color baseColor){
    for (int i = 0; i < 6; i++){
        Face *face = &faces[i];
        int middleZ = (int)((face->topR->z + face->bottomL->z) * 2.0f);
        if (middleZ < 0) middleZ = 0;
        if (middleZ > 255) middleZ = 255;
        
        int r = baseColor.r - middleZ;
        int g = baseColor.g - middleZ;
        int b = baseColor.b - middleZ;
        
        face->color.r = (r < 1) ? 1 : (r > 255 ? 255 : r);
        face->color.g = (g < 1) ? 1 : (g > 255 ? 255 : g);
        face->color.b = (b < 1) ? 1 : (b > 255 ? 255 : b);
        face->color.a = baseColor.a;
    }
}

void drawEdge(ProjectedEdge edge, Color color){
    DrawLine(edge.start.x, edge.start.y, edge.end.x, edge.end.y, color);    
}

void drawEdges(ProjectedEdge *edges, Color color){
    for (int i = 0; i < 12; i++) drawEdge(edges[i], color);
}

Cube *createCube(Vector3 pos, int side, Color color){
    Vector3 *vertices = malloc(sizeof(Vector3)*8);
    Edge *edges = malloc(sizeof(Edge)*12);
    Face *faces = malloc(sizeof(Face)*6);
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

    if (!faces){
        printf("Could not create faces array.\n");
        return NULL;
    }
    printf("Created faces array.\n");
   
    if (!cube){
        printf("Could not create cube.\n");
        return NULL;
    }
    printf("Created cube.\n");
    
    cube->vertices = vertices;
    cube->edges = edges;
    cube->faces = faces;
    cube->side = side;
    cube->color = color;
    int halfSide = side/2;

    printf("Initalized Cube.\n");

    int edgesMask[12][2] = {
        {6, 2}, {6, 4}, {6, 7},
        {3, 1}, {3, 2}, {3, 7},
        {0, 1}, {0, 2}, {0, 4},
        {5, 1}, {5, 4}, {5, 7},
    };

    int facesMask[6][4] = {
      {3, 2, 1, 0}, {6, 7, 4, 5},
      {2, 6, 0, 4}, {7, 3, 5, 1},
      {7, 6, 3, 2}, {1, 0, 5, 4}  
    };

    for (int i = 0; i < 8; i++){
        vertices[i] = (Vector3){
            pos.x + ((i&1) ? halfSide : -halfSide),
            pos.y + ((i&2) ? halfSide : -halfSide),
            pos.z + ((i&4) ? halfSide : -halfSide),
        };
    }

    printf("Populated vertices.\n");

    for (int i = 0; i < 6; i++){
        int *idxs = facesMask[i];
        Face face = (Face){
            .topR    = &vertices[idxs[0]],
            .topL    = &vertices[idxs[1]],
            .bottomR = &vertices[idxs[2]],
            .bottomL = &vertices[idxs[3]],
        };
        unsigned char middleZ = (face.topR->z + face.bottomL->z);
        face.color = (Color){
            .r = color.r - middleZ,
            .g = color.g - middleZ,
            .b = color.b - middleZ,
            .a = color.a,
        };
        faces[i] = face;
    }

    printf("Populated faces.\n");

    for(int i = 0; i < 12; i++){
        int a = edgesMask[i][0];
        int b = edgesMask[i][1];
        edges[i] = (Edge){ &vertices[a], &vertices[b] };
    }

    printf("Populated edges.\n");

    cube->center = pos;

    return cube;
}

bool freeCube(Cube *cube){
    if(!cube) return false;
    free(cube->vertices);
    free(cube->edges);
    free(cube->faces);
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
void updateCubeCenter(Cube *cube){
    Vector3 c = {0, 0, 0};

    for (int i = 0; i < 8; i++){
        c.x += cube->vertices[i].x;
        c.y += cube->vertices[i].y;
        c.z += cube->vertices[i].z;
    }

    cube->center.x = c.x * 0.125f;
    cube->center.y = c.y * 0.125f;
    cube->center.z = c.z * 0.125f;
}

Vector3 cubeCenter(Cube *cube){
    return cube->center;
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
    Vector3 c0 = cube->center;
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float x0 = v->x - c0.x;
        float z0 = v->z - c0.z;

        v->x = x0 * c + z0 * s + c0.x;
        v->z = -x0 * s + z0 * c + c0.z;
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
    Vector3 c0 = cube->center;
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float x0 = v->x - c0.x;
        float y0 = v->y - c0.y;

        v->x = x0 * c - y0 * s + c0.x;
        v->y = x0 * s + y0 * c + c0.y;
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
    Vector3 c0 = cube->center;
    float c = cosf(angle);
    float s = sinf(angle);

    for (int i = 0; i < 8; i++){
        Vector3 *v = &cube->vertices[i];

        float y0 = v->y - c0.y;
        float z0 = v->z - c0.z;

        v->y = y0 * c - z0 * s + c0.y;
        v->z = y0 * s + z0 * c + c0.z;
    }
}

ProjectedCube *createProjectedCube(){
    Vector2 *vertices = malloc(sizeof(Vector2)*8);
    ProjectedEdge *edges = malloc(sizeof(ProjectedEdge)*12);
    ProjectedFace *faces = malloc(sizeof(ProjectedFace)*6);
    ProjectedCube *cube = malloc(sizeof(ProjectedCube));
    cube->vertices = vertices;
    cube->edges = edges;
    cube->faces = faces;
    return cube;
}

bool freeProjectedCube(ProjectedCube *cube){
    if(!cube) return false;
    free(cube->vertices);
    free(cube->edges);
    free(cube->faces);
    return true;
}

void projectCube(Cube *cube, ProjectedCube *projected, Screen screen){
    for (int i = 0; i < 8; i++) {
        projected->vertices[i] = project(cube->vertices[i], screen);
    }
    for (int i = 0; i < 12; i++) {
        projected->edges[i].start = project(*cube->edges[i].start, screen);
        projected->edges[i].end = project(*cube->edges[i].end, screen);
    }
    for (int i = 0; i < 6; i++) {
        projected->faces[i] = projectFace(cube->faces[i], screen);
    }
}

void updateCube(Cube *cube, Vector3 delta){
    if (delta.x != 0 || delta.y != 0 || delta.z != 0) {
        updatePoints(cube->vertices, 8, delta);
        updateEdges(cube->edges, delta);
        updateCubeCenter(cube);
    }
    updateFacesColor(cube->faces, cube->color);
}

void drawFaces(ProjectedFace *faces, Color color){
    for (int i = 0; i < 6; i++){
        ProjectedFace face = faces[i];    
        DrawTriangle(face.topR, face.topL, face.bottomL, face.color);
        DrawTriangle(face.bottomL, face.bottomR, face.topR, face.color);
    }
}

void drawCube(ProjectedCube *cube, bool wireframe, float radius, Color color){
    if (wireframe) {
        drawEdges(cube->edges, color);
        drawPoints(cube->vertices, 8, radius, color);
    }
    else drawFaces(cube->faces, color);
}
