#include <raylib.h>
#include "projection.h"

typedef struct Edge{
    Vector3 *start;
    Vector3 *end;
} Edge;

typedef struct ProjectedEdge {
    Vector2 start;
    Vector2 end;
} ProjectedEdge;

typedef struct Cube{
    Vector3 *vertices;
    Edge *edges;
    int side;  
} Cube;

typedef struct ProjectedCube {
    Vector2 *vertices;
    ProjectedEdge *edges;
} ProjectedCube;

Cube *createCube(Vector3, int);
bool freeCube(Cube*);
void updateCube(Cube*, float);
void projectCube(Cube*, ProjectedCube*, Screen);
void drawCube(ProjectedCube*, bool, bool, float, Color);
void rotateCubeYAxisLocalSpace(Cube*, float);
void rotateCubeZAxisLocalSpace(Cube*, float);
void rotateCubeXAxisLocalSpace(Cube*, float);

ProjectedCube *createProjectedCube();
bool freeProjectedCube(ProjectedCube*);
