#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Game.c"
#include "Geometry.h"

// Override base class with your custom functionality
class SpaceGame : public olc::PixelGameEngine
{
public:
	SpaceGame()
	{
		// Name your application
		sAppName = "SpaceGame";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};

int main()
{
	SpaceGame demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}