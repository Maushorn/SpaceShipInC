#include<stdlib.h>
#include<stdio.h>
#include<SDL.h>
#include<SDL_render.h>
#include<SDL_video.h>
#include<math.h>
#include"Geometry.h"

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

Mesh* meshCube;
float fTheta = 0.0f;


/*
* Adds Mesh to the render pipeline.
*/
void AddMesh(Mesh* mesh) {
	meshes[meshCount] = mesh;
	meshCount++;
}

void Load(void) {
	fAspectRatio = ((float)height) / ((float)width);
	fFovRad = 1.0f / tanf(fFov * 0.5f);
	meshCube = GetExampleMeshCube();
	AddMesh(meshCube);

	//Set Projection Matrix
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fNear * fFar) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f; //Not really necessary, because all elements should be initiated with 0.

}

void Update(Uint32 delta) {
	fTheta += 0.001f * (float)delta;

	//Set z-Rotation Matrix
	matRotZ.m[0][0] = cosf(fTheta);
	matRotZ.m[0][1] = sinf(fTheta);
	matRotZ.m[1][0] = -sinf(fTheta);
	matRotZ.m[1][1] = cosf(fTheta);
	matRotZ.m[2][2] = 1;
	matRotZ.m[3][3] = 1;

	//Set x-Rotation Matrix
	matRotX.m[0][0] = 1;
	matRotX.m[1][1] = cosf(fTheta * 0.5f);
	matRotX.m[1][2] = sinf(fTheta * 0.5f);
	matRotX.m[2][1] = -sinf(fTheta * 0.5f);
	matRotX.m[2][2] = cosf(fTheta * 0.5f);
	matRotX.m[3][3] = 1;
}

void Draw(SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	Triangle projectedTriangle;

	for (size_t i = 0; i < meshCube->trisCount; i++)
	{
		Triangle rotatedZTriangle;
		MultiplyMatrixVector(&(meshCube->tris[i].p[0]), &rotatedZTriangle.p[0], &matRotZ);
		MultiplyMatrixVector(&(meshCube->tris[i].p[1]), &rotatedZTriangle.p[1], &matRotZ);
		MultiplyMatrixVector(&(meshCube->tris[i].p[2]), &rotatedZTriangle.p[2], &matRotZ);

		Triangle rotatedXTriangle;
		MultiplyMatrixVector(&rotatedZTriangle.p[0], &rotatedXTriangle.p[0], &matRotX);
		MultiplyMatrixVector(&rotatedZTriangle.p[1], &rotatedXTriangle.p[1], &matRotX);
		MultiplyMatrixVector(&rotatedZTriangle.p[2], &rotatedXTriangle.p[2], &matRotX);

		//Translate because otherwise the camera is inside the Cube
		Triangle translatedTriangle = rotatedXTriangle;
		translatedTriangle.p[0].z = rotatedXTriangle.p[0].z + 3.0f;
		translatedTriangle.p[1].z = rotatedXTriangle.p[1].z + 3.0f;
		translatedTriangle.p[2].z = rotatedXTriangle.p[2].z + 3.0f;

		//Calculating the normal vector (cross product).
		Vec3d vectorA = {
			.x = translatedTriangle.p[1].x - translatedTriangle.p[0].x,
			.y = translatedTriangle.p[1].y - translatedTriangle.p[0].y,
			.z = translatedTriangle.p[1].z - translatedTriangle.p[0].z
		};
		Vec3d vectorB = {
			.x = translatedTriangle.p[2].x - translatedTriangle.p[0].x,
			.y = translatedTriangle.p[2].y - translatedTriangle.p[0].y,
			.z = translatedTriangle.p[2].z - translatedTriangle.p[0].z
		};
		Vec3d normalVector = {
			.x = vectorA.y * vectorB.z - vectorA.z * vectorB.y,
			.y = vectorA.z * vectorB.x - vectorA.x * vectorB.z,
			.z = vectorA.x * vectorB.y - vectorA.y * vectorB.x
		};
		//normalize normalVector
		float normalizeFactor = sqrtf(normalVector.x * normalVector.x + normalVector.y * normalVector.y + normalVector.z * normalVector.z);
		normalVector.x /= normalizeFactor;
		normalVector.x /= normalizeFactor;
		normalVector.x /= normalizeFactor;

		Vec3d lookVector = {
			.x = translatedTriangle.p[0].x - cameraPosition.x,
			.y = translatedTriangle.p[0].y - cameraPosition.y,
			.z = translatedTriangle.p[0].z - cameraPosition.z
		};
		//normalize again
		float normalizeFactor2 = sqrtf(lookVector.x * lookVector.x + lookVector.y * lookVector.y + lookVector.z * lookVector.z);
		lookVector.x /= normalizeFactor2;
		lookVector.y /= normalizeFactor2;
		lookVector.z /= normalizeFactor2;

		if ((normalVector.x * lookVector.x + normalVector.y * lookVector.y + normalVector.z * lookVector.z) < 0) {
			//Project Triangle - Projection Space
			MultiplyMatrixVector(&(translatedTriangle.p[0]), &(projectedTriangle.p[0]), &matProj);
			MultiplyMatrixVector(&(translatedTriangle.p[1]), &(projectedTriangle.p[1]), &matProj);
			MultiplyMatrixVector(&(translatedTriangle.p[2]), &(projectedTriangle.p[2]), &matProj);

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

			//Draw Triangles
			SDL_RenderDrawLine(renderer, projectedTriangle.p[0].x, projectedTriangle.p[0].y, projectedTriangle.p[1].x, projectedTriangle.p[1].y);
			SDL_RenderDrawLine(renderer, projectedTriangle.p[1].x, projectedTriangle.p[1].y, projectedTriangle.p[2].x, projectedTriangle.p[2].y);
			SDL_RenderDrawLine(renderer, projectedTriangle.p[2].x, projectedTriangle.p[2].y, projectedTriangle.p[0].x, projectedTriangle.p[0].y);
		}
	}

	SDL_RenderPresent(renderer);

}

void Exit() {
	for (int i = 0; i < meshCount; i++)
	{
		free(meshes[i]->tris);
		free(meshes[i]);
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

				/* Example Code on how to draw
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