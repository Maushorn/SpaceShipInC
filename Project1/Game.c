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
	fopen_s(&file, "SpaceShip.obj", "r");
	char buffer[64];
	Triangle triangleBuffer[128];
	float vertexBuffer[64][3];
	int vertexCount = 0;
	int faceBuffer[128][3];
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
	fTheta += 0.001f * (float)delta;

	//Set z-Rotation Matrix
	RotationMatrixZ(&matRotZ, fTheta);

	//Set x-Rotation Matrix
	RotationMatrixX(&matRotX, fTheta);

}

void Draw(SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	Triangle triangles[512] = { 0 };
	int triangleCount = 0;
	Mat4x4 worldMatrix = Matrix_MultiplyMatrix(matRotX, matRotZ);

	for (size_t i = 0; i < meshCube->trisCount; i++)
	{
		Triangle rotatedTriangle;
		Matrix_MultiplyTriangle(&(meshCube->tris[i]), &rotatedTriangle, &worldMatrix);

		//Translate because otherwise the camera is inside the Cube
		Triangle translatedTriangle;
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
		float light = -(
			normalVector.x * directionalLight.x +
			normalVector.y * directionalLight.y +
			normalVector.z * directionalLight.z
			) * 255.0f;
		SDL_SetRenderDrawColor(renderer, light, light, light, SDL_ALPHA_OPAQUE);

		//Project Triangle - Projection Space
		Triangle projectedTriangle;
		Matrix_MultiplyTriangle(&(triangles[i]), &projectedTriangle, &matProj);

		//Normalize Coordinates
		for (size_t i = 0; i < 3; i++)
		{
			projectedTriangle.p[i] = Vector_Div(projectedTriangle.p[i], projectedTriangle.p[i].w);
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

		//FillTriangle(renderer, &projectedTriangle);

		FillTriangle(
			projectedTriangle.p[0].x, projectedTriangle.p[0].y,
			projectedTriangle.p[1].x, projectedTriangle.p[1].y,
			projectedTriangle.p[2].x, projectedTriangle.p[2].y,
			renderer
		);


		//Draw Triangles
/*
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawLine(renderer, projectedTriangle.p[0].x, projectedTriangle.p[0].y, projectedTriangle.p[1].x, projectedTriangle.p[1].y);
		SDL_RenderDrawLine(renderer, projectedTriangle.p[1].x, projectedTriangle.p[1].y, projectedTriangle.p[2].x, projectedTriangle.p[2].y);
		SDL_RenderDrawLine(renderer, projectedTriangle.p[2].x, projectedTriangle.p[2].y, projectedTriangle.p[0].x, projectedTriangle.p[0].y);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		*/
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