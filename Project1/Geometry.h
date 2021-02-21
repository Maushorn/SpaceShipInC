#include <stdlib.h>
#include <math.h>

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vec3d;

typedef struct {
	float x, y;
} Vec2d;

typedef struct {
	Vec2d p[2];
} Line2D;

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

Vec3d Vector_AddVector(Vec3d v1, Vec3d v2) {
	Vec3d v = { .x = v1.x + v2.x, .y = v1.y + v2.y,.z = v1.z + v2.z,.w = v1.w + v2.w };
	return v;
}

Vec3d Vector_SubVector(Vec3d v1, Vec3d v2) {
	Vec3d v = { .x = v1.x - v2.x, .y = v1.y - v2.y,.z = v1.z - v2.z,.w = v1.w - v2.w };
	return v;
}

Vec3d Vector_Div(Vec3d v, float x) {
	Vec3d newVec;
	newVec.x = v.x / x;
	newVec.y = v.y / x;
	newVec.z = v.z / x;
	newVec.w = v.w / x;
	return newVec;
}

Vec3d Vector_Mul(Vec3d v, float x) {
	Vec3d newVec;
	newVec.x = v.x * x;
	newVec.y = v.y * x;
	newVec.z = v.z * x;
	newVec.w = v.w * x;
	return newVec;
}

void Matrix_MultiplyVector(Vec3d* i, Vec3d* o, Mat4x4* m) {
	o->x = i->x * m->m[0][0] + i->y * m->m[1][0] + i->z * m->m[2][0] + m->m[3][0];
	o->y = i->x * m->m[0][1] + i->y * m->m[1][1] + i->z * m->m[2][1] + m->m[3][1];
	o->z = i->x * m->m[0][2] + i->y * m->m[1][2] + i->z * m->m[2][2] + m->m[3][2];
	o->w = i->x * m->m[0][3] + i->y * m->m[1][3] + i->z * m->m[2][3] + m->m[3][3];
}

void Matrix_MultiplyTriangle(Triangle* i, Triangle* o, Mat4x4* m) {
	Matrix_MultiplyVector(&(i->p[0]), &(o->p[0]), m);
	Matrix_MultiplyVector(&(i->p[1]), &(o->p[1]), m);
	Matrix_MultiplyVector(&(i->p[2]), &(o->p[2]), m);
}

Mat4x4 Matrix_MultiplyMatrix(Mat4x4 m1, Mat4x4 m2) {
	Mat4x4 m = { 0 };
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			m.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	return m;
}

inline void RotationMatrixX(Mat4x4* matRotX, float fTheta) {
	matRotX->m[0][0] = 1;
	matRotX->m[1][1] = cosf(fTheta * 0.5f);
	matRotX->m[1][2] = sinf(fTheta * 0.5f);
	matRotX->m[2][1] = -sinf(fTheta * 0.5f);
	matRotX->m[2][2] = cosf(fTheta * 0.5f);
	matRotX->m[3][3] = 1;
}

inline void RotationMatrixY(Mat4x4* matRotY, float fTheta) {
	matRotY->m[0][0] = cosf(fTheta);
	matRotY->m[1][1] = 1;
	matRotY->m[2][0] = sinf(fTheta);
	matRotY->m[0][2] = -sinf(fTheta);
	matRotY->m[2][2] = cosf(fTheta);
	matRotY->m[3][3] = 1;

}

inline void RotationMatrixZ(Mat4x4* matRotZ, float fTheta) {
	matRotZ->m[0][0] = cosf(fTheta);
	matRotZ->m[0][1] = sinf(fTheta);
	matRotZ->m[1][0] = -sinf(fTheta);
	matRotZ->m[1][1] = cosf(fTheta);
	matRotZ->m[2][2] = 1;
	matRotZ->m[3][3] = 1;
}

inline void TranslateTriangle(Triangle* t, float value) {
	t->p[0].z = t->p[0].z + value;
	t->p[1].z = t->p[1].z + value;
	t->p[2].z = t->p[2].z + value;
}

inline void NormalVector(Triangle* t, Vec3d* normalVector) {
	Vec3d vectorA;
	vectorA.x = t->p[1].x - t->p[0].x;
	vectorA.y = t->p[1].y - t->p[0].y;
	vectorA.z = t->p[1].z - t->p[0].z;

	Vec3d vectorB;
	vectorB.x = t->p[2].x - t->p[0].x;
	vectorB.y = t->p[2].y - t->p[0].y;
	vectorB.z = t->p[2].z - t->p[0].z;

	normalVector->x = vectorA.y * vectorB.z - vectorA.z * vectorB.y;
	normalVector->y = vectorA.z * vectorB.x - vectorA.x * vectorB.z;
	normalVector->z = vectorA.x * vectorB.y - vectorA.y * vectorB.x;
}

inline void NormalizeVector(Vec3d* v) {
	float normalizeFactor = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= normalizeFactor;
	v->y /= normalizeFactor;
	v->z /= normalizeFactor;
}

float Vector_DotProduct(Vec3d* v1, Vec3d* v2) {
	return (v1->x * v2->x + v1->y * v2->y + v1->z * v2->z);
}

Vec3d Vector_CrossProduct(Vec3d* v1, Vec3d* v2) {
	Vec3d newVector = {
		.x = v1->y * v2->z - v1->z * v2->y,
		.y = v1->z * v2->x - v1->x * v2->z,
		.z = v1->x * v2->y - v1->y * v2->x,
		.w = 1
	};

	return newVector;
}

Mat4x4 Matrix_PointAt(Vec3d* pos, Vec3d* target, Vec3d* up) {
	//Calculate new forward direction
	Vec3d newForward = Vector_SubVector(*target, *pos);
	NormalizeVector(&newForward);

	//Calculate new up
	Vec3d a = Vector_Mul(newForward, Vector_DotProduct(up, &newForward));
	Vec3d newUp = Vector_SubVector(*up, a);
	NormalizeVector(&newUp);

	//Calculate new Right
	Vec3d newRight = Vector_CrossProduct(&newUp, &newForward);

	Mat4x4 m = { .m = {
		{newRight.x, newRight.y, newRight.z, 0},
		{newUp.x, newUp.y ,newUp.z, 0},
		{newForward.x ,newForward.y ,newForward.z ,0},
		{pos->x, pos->y, pos->z, 1}
		}
	};
	return m;
}


Mat4x4 Matrix_QuickInverse(Mat4x4* m) {
	Mat4x4 newMatrix = {
		.m = {
			{m->m[0][0],m->m[1][0],m->m[2][0], 0},
			{m->m[0][1],m->m[1][1],m->m[2][1], 0},
			{m->m[0][2],m->m[1][2],m->m[2][2], 0},
			{
				-(m->m[3][0] * m->m[0][0] + m->m[3][1] * m->m[0][1] + m->m[3][2] * m->m[0][2]),
				-(m->m[3][0] * m->m[1][0] + m->m[3][1] * m->m[1][1] + m->m[3][2] * m->m[1][2]),
				-(m->m[3][0] * m->m[2][0] + m->m[3][1] * m->m[2][1] + m->m[3][2] * m->m[2][2]),
				1
			}
		}
	};
	return newMatrix;
}