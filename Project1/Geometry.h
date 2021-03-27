#include <stdlib.h>
#include <math.h>

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vec3d;

typedef struct {
	float u, v;
} Vec2d;

typedef struct {
	Vec2d p[2];
} Line2D;

typedef struct {
	Vec3d p[3];
	Vec2d t[3];
} Triangle;

typedef struct {
	Triangle* tris;
	int trisCount;
} Mesh;

typedef struct {
	float m[4][4];
}Mat4x4;

typedef struct {
	Vec3d color[];
}SVGSprite;

Triangle GetDefaultTriangle() {
	Triangle t;
	for (size_t i = 0; i < 3; i++)
	{
		t.p[i].x = 0.0f;
		t.p[i].y = 0.0f;
		t.p[i].z = 0.0f;
		t.p[i].w = 1.0f;
	}
	return t;
}

Mesh* GetExampleMeshCube() {

	Mesh* meshCube = malloc(sizeof(Mesh));

	Triangle triangles[] = {
		//South
		{.p = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } } },
		{.p = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } } },
		//East
		{.p = { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } }  },
		{.p = { { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }  },
		//North
		{.p = { { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } }  },
		{.p = { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }  },
		//West
		{.p = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } }  },
		{.p = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }  },
		//Top
		{.p = { { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } }  },
		{.p = { { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }  },
		//Bottom
		{.p = { { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } }, .t = { { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } }  },
		{.p = { { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } }, .t = { { 0.0f, 1.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f } }  }
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
	Matrix_MultiplyVector(&i->p[0], &o->p[0], m);
	Matrix_MultiplyVector(&i->p[1], &o->p[1], m);
	Matrix_MultiplyVector(&i->p[2], &o->p[2], m);
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

//Function returns a Vector if a line crosses the plane
Vec3d Vector_IntersectPlane(Vec3d* plane_p, Vec3d* plane_n, Vec3d* lineStart, Vec3d* lineEnd, float* t) {
	NormalizeVector(plane_n);
	float plane_d = -Vector_DotProduct(plane_n, plane_p);
	float ad = Vector_DotProduct(lineStart, plane_n);
	float bd = Vector_DotProduct(lineEnd, plane_n);
	*t = (-plane_d - ad) / (bd - ad);
	Vec3d lineStartToEnd = Vector_SubVector(*lineEnd, *lineStart);
	Vec3d lineToIntersect = Vector_Mul(lineStartToEnd, *t);
	return Vector_AddVector(*lineStart, lineToIntersect);
}

//Calculate the distance between a point an a plane.
inline float Vector_PlaneDistance(Vec3d* p, Vec3d* plane_point, Vec3d* plane_normal) {
	return Vector_DotProduct(plane_normal, p) - Vector_DotProduct(plane_normal, plane_point);
}

//Returns amount of resulting triangles.
int Triangle_ClipAgainstPlane(Vec3d* plane_p, Vec3d* plane_n, Triangle* in_tri, Triangle* out_tri1, Triangle* out_tri2) {
	//Make sure plane normal is normalized
	NormalizeVector(plane_n);

	Vec3d* insidePoints[3]; int insidePointCount = 0;
	Vec3d* outsidePoints[3]; int outsidePointCount = 0;

	Vec2d* insideTextures[3]; int insideTextureCount = 0;
	Vec2d* outsideTextures[3]; int outsideTextureCount = 0;

	//Get signed distance of each point in triangle to plane
	float d0 = Vector_PlaneDistance(&in_tri->p[0], plane_p, plane_n);
	float d1 = Vector_PlaneDistance(&in_tri->p[1], plane_p, plane_n);
	float d2 = Vector_PlaneDistance(&in_tri->p[2], plane_p, plane_n);

	if (d0 >= 0) {
		insidePoints[insidePointCount++] = &in_tri->p[0]; insideTextures[insideTextureCount++] = &in_tri->t[0];
	}
	else {
		outsidePoints[outsidePointCount++] = &in_tri->p[0];  outsideTextures[outsideTextureCount++] = &in_tri->t[0];
	}
	if (d1 >= 0) {
		insidePoints[insidePointCount++] = &in_tri->p[1]; insideTextures[insideTextureCount++] = &in_tri->t[1];
	}
	else {
		outsidePoints[outsidePointCount++] = &in_tri->p[1]; outsideTextures[outsideTextureCount++] = &in_tri->t[1];
	}
	if (d2 >= 0) {
		insidePoints[insidePointCount++] = &in_tri->p[2]; insideTextures[insideTextureCount++] = &in_tri->t[2];
	}
	else {
		outsidePoints[outsidePointCount++] = &in_tri->p[2]; outsideTextures[outsideTextureCount++] = &in_tri->t[2];
	}

	if (insidePointCount == 0) {
		//All points are outside the plane.
		return 0;
	}
	if (insidePointCount == 3) {
		//All points are inside the plane.
		*out_tri1 = *in_tri;
		return 1;
	}

	float t;
	float* tPtr = &t;
	if (insidePointCount == 1 && outsidePointCount == 2) {
		//Ther inside point is valid, so we keep that
		out_tri1->p[0] = *insidePoints[0];
		out_tri1->t[0] = *insideTextures[0];
		//But the two new points are at the locations where the original triangle intersects with the plane.
		out_tri1->p[1] = Vector_IntersectPlane(plane_p, plane_n, insidePoints[0], outsidePoints[0], tPtr);
		out_tri1->p[2] = Vector_IntersectPlane(plane_p, plane_n, insidePoints[0], outsidePoints[1], tPtr);
		//Apply this to texture coordinates as well
		out_tri1->t[1].u = t * (outsideTextures[0]->u - insideTextures[0]->u) + insideTextures[0]->u;
		out_tri1->t[1].v = t * (outsideTextures[0]->v - insideTextures[0]->v) + insideTextures[0]->v;
		out_tri1->t[2].u = t * (outsideTextures[1]->u - insideTextures[0]->u) + insideTextures[0]->u;
		out_tri1->t[2].v = t * (outsideTextures[1]->v - insideTextures[0]->v) + insideTextures[0]->v;
		return 1;
	}
	if (insidePointCount == 2 && outsidePointCount == 1) {

		//Calculate first new triangle
		out_tri1->p[0] = *insidePoints[0];
		out_tri1->p[1] = *insidePoints[1];
		out_tri1->p[2] = Vector_IntersectPlane(plane_p, plane_n, insidePoints[0], outsidePoints[0], tPtr);
		//Apply this to texture
		out_tri1->t[0] = *insideTextures[0];
		out_tri1->t[1] = *insideTextures[1];
		out_tri1->t[2].u = t * (outsideTextures[0]->u - insideTextures[0]->u) + insideTextures[0]->u;
		out_tri1->t[2].v = t * (outsideTextures[0]->v - insideTextures[0]->v) + insideTextures[0]->v;

		//Calculate second new triangle
		out_tri2->p[0] = *insidePoints[1];
		out_tri2->p[1] = Vector_IntersectPlane(plane_p, plane_n, insidePoints[1], outsidePoints[0], tPtr);
		out_tri2->p[2] = out_tri1->p[2];
		//Apply this to texture
		out_tri2->t[0] = *insideTextures[1];
		out_tri2->t[1].u = t * (outsideTextures[0]->u - insideTextures[1]->u) + insideTextures[1]->u;
		out_tri2->t[1].v = t * (outsideTextures[0]->v - insideTextures[1]->v) + insideTextures[1]->v;
		out_tri2->t[2] = out_tri1->t[2];



		return 2;
	}
	return 0;
}

void TexturedTriangle(
	int x1, int y1, float u1, float v1,
	int x2, int y2, float u2, float v2,
	int x3, int y3, float u3, float v3,
	SVGSprite* tex
) {

}