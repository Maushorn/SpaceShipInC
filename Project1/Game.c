#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <math.h>
#include <string.h>
#include "Geometry.h"
#include "Triangle.h"

int width = 1920;
int height = 1080;

Mesh* meshes[100] = { 0 };
int meshCount = 0;

Mat4x4 matrix = { 0 };
float fNear = 0.1f;
float fFar = 1000.0f;
float fFov = 90.0f;
float fAspectRatio;
float fFovRad;
Mat4x4 matProj = { 0 };
Mat4x4 matRotZ = { 0 };
Mat4x4 matRotX = { 0 };
Vec3d cameraPosition = { 0 };
Vec3d lookDirection = { 0, 0, 1 };
float Yaw = 0.0f;
Vec3d directionalLight = { .x = 0.0, .y = 0.0, .z = 1.0, .w = 1.0 };

Mesh* meshCube;
float fTheta = 0.0f;
//TODO: Make this dynamically creatable at runtime
Mesh spaceShip;
Triangle* ptrSpaceShipTriangles;

/*
* Adds Mesh to the render pipeline.
*/
void AddMesh(Mesh* mesh) {
	meshes[meshCount] = mesh;
	meshCount++;
}

int SortTriangles(Triangle t1, Triangle t2) {
	return ((t1.p[0].z + t1.p[1].z + t1.p[2].z) < (t2.p[0].z + t2.p[1].z + t2.p[2].z));
}

void LoadMeshFromFile() {
	FILE* file;
	//TODO Improve performance with teapot
	fopen_s(&file, "SpaceShip.obj", "r");
	char buffer[64];
	Triangle triangleBuffer[128];
	float vertexBuffer[4000][3];
	int vertexCount = 0;
	int faceBuffer[8000][3];
	int faceCount = 0;


	while (fgets(buffer, 64, file) != NULL) {
		Vec3d tmpVertex;
		int start = 2;
		int end = 0;
		int nextStart = start;
		int j = 0;

		if (buffer[0] == 'v') {
			for (size_t i = 2; i < 64; i++)
			{
				if (buffer[i] == ' ' || buffer[i] == '\n') {
					end = i - 1;
					nextStart = i + 1;
					float value = atof(&(buffer[start]));
					//Add Vertex
					vertexBuffer[vertexCount][j] = value;

					printf("%f", value);
					printf("\n");
					j++;
					if (buffer[i] == '\n' || j > 2) {
						vertexCount++;
						break;
					}
					start = nextStart;
				}

			}
			printf("\n");
		}

		if (buffer[0] == 'f') {
			for (size_t i = 2; i < 64; i++)
			{
				if (buffer[i] == ' ' || buffer[i] == '\n') {
					end = i - 1;
					nextStart = i + 1;
					int value = atoi(&(buffer[start]));
					//Add Face
					faceBuffer[faceCount][j] = value;

					printf("%f", value);
					printf("\n");
					j++;
					if (buffer[i] == '\n' || j > 2) {
						faceCount++;
						break;
					}
					start = nextStart;
				}

			}
		}



		strchr(buffer, ' ');
	}

	ptrSpaceShipTriangles = calloc(sizeof(Triangle), faceCount);
	for (size_t i = 0; i < faceCount; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{


			ptrSpaceShipTriangles[i].p[j].x = vertexBuffer[faceBuffer[i][j] - 1][0];
			ptrSpaceShipTriangles[i].p[j].y = vertexBuffer[faceBuffer[i][j] - 1][1];
			ptrSpaceShipTriangles[i].p[j].z = vertexBuffer[faceBuffer[i][j] - 1][2];


		}
	}
	spaceShip.tris = ptrSpaceShipTriangles;
	spaceShip.trisCount = faceCount;
	fclose(file);
}

void Load(void) {
	fAspectRatio = ((float)height) / ((float)width);
	fFovRad = 1.0f / tanf(fFov * 0.5f);
	meshCube = GetExampleMeshCube();
	LoadMeshFromFile();
	meshCube = &spaceShip;
	AddMesh(meshCube);
	//AddMesh(&spaceShip);

	//Set Projection Matrix
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fNear * fFar) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f; //Not really necessary, because all elements should be initiated with 0.

}

void Update(int delta) {
	//Rotate
	//fTheta += 0.001f * (float)delta;

	const Uint8* state = SDL_GetKeyboardState(NULL);
	Vec3d forward = Vector_Mul(lookDirection, delta * 0.01f);

	//Controls
	if (state[SDL_SCANCODE_W]) {
		cameraPosition = Vector_AddVector(cameraPosition, forward);
	}
	if (state[SDL_SCANCODE_S]) {
		cameraPosition = Vector_SubVector(cameraPosition, forward);
	}
	if (state[SDL_SCANCODE_E]) {
		cameraPosition.y -= 0.02f;
	}
	if (state[SDL_SCANCODE_Q]) {
		cameraPosition.y += 0.02f;
	}
	if (state[SDL_SCANCODE_A]) {
		Yaw -= delta * 0.001f;
	}if (state[SDL_SCANCODE_D]) {
		Yaw += delta * 0.001f;
	}

	//Set z-Rotation Matrix
	RotationMatrixZ(&matRotZ, fTheta);

	//Set x-Rotation Matrix
	RotationMatrixX(&matRotX, fTheta);

}

void Draw(SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	Triangle triangles[8000] = { 0 };
	int triangleCount = 0;
	Mat4x4 worldMatrix = Matrix_MultiplyMatrix(matRotX, matRotZ);


	Vec3d up = { 0, 1 ,0 };
	//Vec3d target = Vector_AddVector(cameraPosition, lookDirection);
	Vec3d target = { 0, 0, 1 };
	Mat4x4 rotationMatrix = { 0 };
	RotationMatrixY(&rotationMatrix, Yaw);
	Matrix_MultiplyVector(&target, &lookDirection, &rotationMatrix);
	target = Vector_AddVector(cameraPosition, lookDirection);

	Mat4x4 cameraMatrix = Matrix_PointAt(&cameraPosition, &target, &up);
	Mat4x4 viewMatrix = Matrix_QuickInverse(&cameraMatrix);

	for (size_t i = 0; i < meshCube->trisCount; i++)
	{
		Triangle rotatedTriangle = GetDefaultTriangle();
		Matrix_MultiplyTriangle(&(meshCube->tris[i]), &rotatedTriangle, &worldMatrix);

		//Translate because otherwise the camera is inside the Cube
		Triangle translatedTriangle = GetDefaultTriangle();
		translatedTriangle = rotatedTriangle;
		TranslateTriangle(&translatedTriangle, 10.0f);

		//Calculating the normal vector (cross product).
		Vec3d normalVector;
		NormalVector(&translatedTriangle, &normalVector);
		//normalize normalVector
		NormalizeVector(&normalVector);
		Vec3d lookVector;
		lookVector = Vector_SubVector(translatedTriangle.p[0], cameraPosition);
		//normalize again
		NormalizeVector(&lookVector);
		//if face points toward camera
		if ((normalVector.x * lookVector.x + normalVector.y * lookVector.y + normalVector.z * lookVector.z) < 0) {
			triangles[triangleCount] = translatedTriangle;
			triangleCount++;
		}
	}

	qsort(triangles, triangleCount, sizeof(Triangle), SortTriangles);

	for (size_t i = 0; i < triangleCount; i++)
	{
		//Calculate directional lighting
		Vec3d normalVector;
		NormalVector(&triangles[i], &normalVector);
		NormalizeVector(&normalVector);
		int light = -(
			normalVector.x * directionalLight.x +
			normalVector.y * directionalLight.y +
			normalVector.z * directionalLight.z
			) * 255.0f;
		if (light < 50) light = 50;

		//Convert World Space to View Space
		Triangle viewTriangle = GetDefaultTriangle();
		Matrix_MultiplyTriangle(&triangles[i], &viewTriangle, &viewMatrix);

		//Clip viewed Triangle against near plane, this could form two additional triangles.
		int clippedTriangleCount = 0;
		Triangle clippedTriangles[2];
		clippedTriangles[0] = GetDefaultTriangle();
		clippedTriangles[1] = GetDefaultTriangle();
		Vec3d plane_p = { .x = 0.0f, .y = 0.0f, .z = 2.0f, .w = 1.0f };
		Vec3d plane_n = { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f };
		clippedTriangleCount = Triangle_ClipAgainstPlane(&plane_p, &plane_n, &viewTriangle, &clippedTriangles[0], &clippedTriangles[1]);

		for (size_t j = 0; j < clippedTriangleCount; j++)
		{

			//Project Triangle - Projection Space
			Triangle projectedTriangle = GetDefaultTriangle();
			Matrix_MultiplyTriangle(&clippedTriangles[j], &projectedTriangle, &matProj);
			//Matrix_MultiplyTriangle(&viewTriangle, &projectedTriangle, &matProj);

			//Normalize Coordinates
			for (size_t k = 0; k < 3; k++)
			{
				projectedTriangle.p[k] = Vector_Div(projectedTriangle.p[k], projectedTriangle.p[k].w);
			}

			//Scale into view
			projectedTriangle.p[0].x += 1.0f; projectedTriangle.p[0].y += 1.0f;
			projectedTriangle.p[1].x += 1.0f; projectedTriangle.p[1].y += 1.0f;
			projectedTriangle.p[2].x += 1.0f; projectedTriangle.p[2].y += 1.0f;

			projectedTriangle.p[0].x *= (0.5f * (float)width);
			projectedTriangle.p[0].y *= (0.5f * (float)height);
			projectedTriangle.p[1].x *= (0.5f * (float)width);
			projectedTriangle.p[1].y *= (0.5f * (float)height);
			projectedTriangle.p[2].x *= (0.5f * (float)width);
			projectedTriangle.p[2].y *= (0.5f * (float)height);






			//Clip viewed Triangle against edge of screen.
			Triangle clippedTrianglesToDraw[64] = { 0 };
			int clippedTrianglesToDrawCount = 0;

			//upper border
			Vec3d edgePlane0_p = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
			Vec3d edgePlane0_n = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f };
			//lower border
			Vec3d edgePlane1_p = { .x = 0.0f, .y = (float)height - 1.0f, .z = 0.0f, .w = 1.0f };
			Vec3d edgePlane1_n = { .x = 0.0f, .y = -1.0f, .z = 0.0f, .w = 1.0f };
			//left border
			Vec3d edgePlane2_p = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
			Vec3d edgePlane2_n = { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
			//right border
			Vec3d edgePlane3_p = { .x = (float)width - 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
			Vec3d edgePlane3_n = { .x = -1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };

			Triangle trianglesToClip[64] = { 0 };
			trianglesToClip[0] = projectedTriangle;
			int trianglesToClipCount = 1;
			Triangle clipGroup[64] = { 0 };

			Triangle clipped[2] = { 0 };
			int clipGroupCount = 0;



			for (size_t p = 0; p < 4; p++)
			{
				clipGroupCount = 0;

				for (size_t q = 0; q < trianglesToClipCount; q++)
				{


					int localClippedCount = 0;
					int allClippedCount = 0;
					Triangle allLocalClippedEdgeTriangles[64];

					switch (p) {
					case 0:
						localClippedCount = Triangle_ClipAgainstPlane(
							&edgePlane0_p,
							&edgePlane0_n,
							&trianglesToClip[q],
							&clipped[0],
							&clipped[1]
						);
						break;
					case 1:
						localClippedCount = Triangle_ClipAgainstPlane(
							&edgePlane1_p,
							&edgePlane1_n,
							&trianglesToClip[q],
							&clipped[0],
							&clipped[1]
						);
						break;
					case 2:
						localClippedCount = Triangle_ClipAgainstPlane(
							&edgePlane2_p,
							&edgePlane2_n,
							&trianglesToClip[q],
							&clipped[0],
							&clipped[1]
						);
						break;
					case 3:
						localClippedCount = Triangle_ClipAgainstPlane(
							&edgePlane3_p,
							&edgePlane3_n,
							&trianglesToClip[q],
							&clipped[0],
							&clipped[1]
						);
						break;
					}

					for (size_t i = 0; i < localClippedCount; i++)
					{
						clipGroup[clipGroupCount] = clipped[i];
						clipGroupCount++;

					}
				}

				for (size_t q = 0; q < clipGroupCount; q++)
				{

					if (p < 3) {
						trianglesToClip[q] = clipGroup[q];
					}
					else { //Draw after all 4 planes where clipped against
						//FillTriangle(renderer, &projectedTriangle);
						SDL_SetRenderDrawColor(renderer, light, light, light, SDL_ALPHA_OPAQUE);
						FillTriangle(
							clipGroup[q].p[0].x, clipGroup[q].p[0].y,
							clipGroup[q].p[1].x, clipGroup[q].p[1].y,
							clipGroup[q].p[2].x, clipGroup[q].p[2].y,
							renderer
						);
						//Draw Triangles
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
						SDL_RenderDrawLine(renderer, clipGroup[q].p[0].x, clipGroup[q].p[0].y, clipGroup[q].p[1].x, clipGroup[q].p[1].y);
						SDL_RenderDrawLine(renderer, clipGroup[q].p[1].x, clipGroup[q].p[1].y, clipGroup[q].p[2].x, clipGroup[q].p[2].y);
						SDL_RenderDrawLine(renderer, clipGroup[q].p[2].x, clipGroup[q].p[2].y, clipGroup[q].p[0].x, clipGroup[q].p[0].y);
						SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
					}
				}
				trianglesToClipCount = clipGroupCount;




				//FillTriangle(renderer, &projectedTriangle);
				/*
				SDL_SetRenderDrawColor(renderer, light, light, light, SDL_ALPHA_OPAQUE);
				FillTriangle(
					projectedTriangle.p[0].x, projectedTriangle.p[0].y,
					projectedTriangle.p[1].x, projectedTriangle.p[1].y,
					projectedTriangle.p[2].x, projectedTriangle.p[2].y,
					renderer
				);
				*/

				//Draw Triangles
				/*
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawLine(renderer, projectedTriangle.p[0].x, projectedTriangle.p[0].y, projectedTriangle.p[1].x, projectedTriangle.p[1].y);
				SDL_RenderDrawLine(renderer, projectedTriangle.p[1].x, projectedTriangle.p[1].y, projectedTriangle.p[2].x, projectedTriangle.p[2].y);
				SDL_RenderDrawLine(renderer, projectedTriangle.p[2].x, projectedTriangle.p[2].y, projectedTriangle.p[0].x, projectedTriangle.p[0].y);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				*/
			}

		}
	}

	SDL_RenderPresent(renderer);

}

void Exit() {
	for (int i = 0; i < meshCount; i++)
	{
		free(meshes[i]->tris);
		//free(meshes[i]);
		//free(ptrSpaceShipTriangles);
	}
}

int main(void) {

	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;

		Load();
		Uint32 delta = 0;
		Uint32 lastNumberOfTicks = 0;
		Uint32 currentNumberOfTicks = 0;

		if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) == 0) {
			SDL_bool done = SDL_FALSE;
			while (!done) {
				SDL_Event event;

				currentNumberOfTicks = SDL_GetTicks();
				delta = currentNumberOfTicks - lastNumberOfTicks;
				lastNumberOfTicks = currentNumberOfTicks;

				Update(delta);

				Draw(renderer);

				// Example Code on how to draw
				/**
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
				SDL_RenderClear(renderer);

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawLine(renderer, 320, 200, 300, 240);
				SDL_RenderDrawLine(renderer, 300, 240, 340, 240);
				SDL_RenderDrawLine(renderer, 340, 240, 320, 200);
				SDL_RenderPresent(renderer);
				*/

				while (SDL_PollEvent(&event)) {
					if (event.type == SDL_QUIT) {
						done = SDL_TRUE;
					}
				}
			}
		}
		if (renderer) {
			SDL_DestroyRenderer(renderer);
		}
		if (window) {
			SDL_DestroyWindow(window);
		}
		Exit();
	}
	SDL_Quit();

	return 0;
}