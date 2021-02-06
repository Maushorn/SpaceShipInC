#include <stdlib.h>

typedef struct {
	float x, y, z;
} Vec3d;

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
