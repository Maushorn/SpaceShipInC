#include <stdlib.h>
#include<stdio.h>
#include "Geometry.h"

Mesh* GetExampleMeshCube() {

	Mesh* meshCube = malloc(sizeof(Mesh));

	Triangle triangles[] = {
		//South
		{ { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } } },
		{ { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },
		//East
		{ { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } } },
		{ { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } } },
		//North
		{ { { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } } },
		{ { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } } },
		//West
		{ { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } } },
		{ { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
		//Top
		{ { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } } },
		{ { { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } } },
		//Bottom
		{ { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } } },
		{ { { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } } }
	};
	meshCube->trisCount = sizeof(triangles) / sizeof(Triangle);
	Triangle* trianglesPtr = calloc(meshCube->trisCount, sizeof(Triangle));

	for (int i = 0; i < meshCube->trisCount; i++)
	{
		*(trianglesPtr + i) = triangles[i];
	}
	meshCube->tris = trianglesPtr;

	return meshCube;
}

void MultiplyMatrixVector(Vec3d* i, Vec3d* o, Mat4x4* m) {
	o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
	o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
	o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
	float w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][2];

	if (w != 0.0f) {
		o->x /= w;
		o->y /= w;
		//I do not know if this is neccessary.
		o->z /= w;
	}
}
