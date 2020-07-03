#include "raylib.h"

#include <stdexcept>
#include <string>

template <typename T>
inline T Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

template <typename T>
inline T Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

struct Tile;
class World
{
public:
	World(int width, int height);
	~World();

	const Tile& At(int i, int j) const;
	Tile& At(int i, int j);

	void Tick();
	void Draw(int x, int y, int ts);

	const int WIDTH;
	const int HEIGHT;
	long Clock() const;

private:
	Tile* tiles;
	long clock;
};

#include "sandtypes.h"

World::World(int width, int height) : WIDTH(width), HEIGHT(height), tiles((Tile*)operator new(WIDTH * HEIGHT * sizeof(Tile))), clock(0)
{
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			At(i, j) = Air(-1);
		}
	}
}

World::~World()
{
	operator delete(tiles, WIDTH * HEIGHT * sizeof(Tile));
}

const Tile& World::At(int i, int j) const
{
#ifndef NDEBUG
	if (i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT)
	{
		throw std::out_of_range("Invalid tile position");
	}
#endif
	return tiles[i + j * WIDTH];
}

Tile& World::At(int i, int j)
{
#ifndef NDEBUG
	if (i < 0 || i >= WIDTH || j < 0 || j >= HEIGHT)
	{
		throw std::out_of_range("Invalid tile position");
	}
#endif
	return tiles[i + j * WIDTH];
}

void World::Tick()
{
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			Tile& t = At(i, j);
			if (t.data.Base.clock >= 0 && t.data.Base.clock <= clock)
			{
				t.Tick(i, j, *this);
			}
		}
	}
	clock++;
}

void World::Draw(int x, int y, int ts)
{
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < HEIGHT; j++)
		{
			if (At(i, j).data.Base.clock >= 0)
			{
				DrawRectangle(x + ts * i, (HEIGHT - 1) * ts - (y + ts * j), ts, ts, At(i, j).TypeColor());
			}
		}
	}
}

long World::Clock() const
{
	return clock;
}

int main(int argc, char** argv)
{
	constexpr int width = 640;
	constexpr int height = 480;
	constexpr int ts = 8;

	InitWindow(width, height, "rsand");

	int rate = GetMonitorRefreshRate(0);
	int interval = 1 + rate / 60;
	int tick = 0;

	SetTargetFPS(rate);

	World world(width / ts, height / ts);

	for (int i = 0; i < world.WIDTH; i++)
	{
		for (int j = 0; j < world.HEIGHT; j++)
		{
			world.At(i, j) = Water(world.Clock());
		}
	}

	while (!WindowShouldClose())
	{
		tick++;
		if (tick % interval == 0)
		{
			tick = 0;
			world.Tick();
			if (rand() % 2 == 0)
			{
				world.At(21 + rand() % 3, world.HEIGHT - 21 - rand() % 3) = Water(world.Clock());
			}
		}
		if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			int x = GetMouseX() / ts;
			int y = (height - 1 - GetMouseY()) / ts;
			if (x >= 0 && x < world.WIDTH && y >= 0 && y < world.HEIGHT)
			{
				if (IsKeyDown(KEY_A))
				{
					world.At(x, y) = Stone(world.Clock(), 10);
				}
				else if (IsKeyDown(KEY_S))
				{
					world.At(x, y) = Sand(world.Clock());
				}
				else if (IsKeyDown(KEY_W))
				{
					world.At(x, y) = Water(world.Clock());
				}
				else if (IsKeyDown(KEY_D))
				{
					world.At(x, y) = Plant(world.Clock(), 0);
				}
				else if (IsKeyDown(KEY_Q))
				{
					world.At(x, y) = Stone(world.Clock(), -1);
				}
				else if (IsKeyDown(KEY_F))
				{
					world.At(x, y) = Fire(world.Clock(), 50);
				}
				else
				{
					world.At(x, y) = Air(-1);
				}
			}
		}

		BeginDrawing();

		ClearBackground(RAYWHITE);
		world.Draw(0, 0, ts);

		EndDrawing();
	}
}
