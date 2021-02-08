#include <stdlib.h>

typedef struct {
	float x, y, z;
} Vec3d;

typedef struct {
	float x, y;
} Vec2d;

typedef struct {
	Vec2d p[2];
} Line2D;

/*
int IsLine2DHorizontal(Line2D* line) {
	if (line->p[0].y == line->p[3].y)
		return 1;
	else
		return 0;
}
*/

typedef struct {
	Vec3d p[3];
} Triangle;

typedef struct {
	Triangle* tris;
	int trisCount;
} Mesh;

typedef struct {
	float m[4][4];
}Mat4x4;

void MultiplyMatrixVector(Vec3d* in, Vec3d* out, Mat4x4* matrix);

Mesh* GetExampleMeshCube();
