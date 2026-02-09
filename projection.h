#include <raylib.h>
#include <stdlib.h>

typedef struct Screen{
  int w;
  int h;
} Screen;

Vector2 projectScreen(Vector2 p, Screen screen);
Vector2 project(Vector3 vec3, Screen screen);
Vector2 cutToVec3(Vector3 vec3);

void projectScreenPoints(Vector3 *pts, Vector2 *projected, size_t size, Screen screen);
void drawPoints(Vector2 *pts, size_t size, int radius, Color color);
void updatePoint(Vector3 *point, Vector3 delta);
void updatePoints(Vector3 *pts, size_t size, Vector3 delta);
