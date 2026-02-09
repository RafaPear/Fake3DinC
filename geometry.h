#include <raylib.h>
#include "projection.h"

typedef struct Edge{
  Vector3 *start;
  Vector3 *end;
} Edge;

typedef struct Face{
  Vector3 *topR;
  Vector3 *topL;
  Vector3 *bottomR;
  Vector3 *bottomL;
  Color    color;
} Face;

typedef struct Cube{
  Vector3 *vertices;
  Edge *edges;
  Face *faces;
  Vector3 center;
  int side;  
  Color color;
} Cube;

typedef struct ProjectedEdge {
  Vector2 start;
  Vector2 end;
} ProjectedEdge;

typedef struct ProjectedFace {
  Vector2 topR;
  Vector2 topL;
  Vector2 bottomR;
  Vector2 bottomL;
  Color   color;
} ProjectedFace;

typedef struct ProjectedCube {
  Vector2 *vertices;
  ProjectedEdge *edges;
  ProjectedFace *faces;
} ProjectedCube;

Cube *createCube(Vector3, int, Color);
bool freeCube(Cube*);
void updateCube(Cube*, Vector3);
void projectCube(Cube*, ProjectedCube*, Screen);
void drawCube(ProjectedCube*, bool, float, Color);
void rotateCubeYAxisLocalSpace(Cube*, float);
void rotateCubeZAxisLocalSpace(Cube*, float);
void rotateCubeXAxisLocalSpace(Cube*, float);
void updateCubeCenter(Cube*);

ProjectedCube *createProjectedCube();
bool freeProjectedCube(ProjectedCube*);
