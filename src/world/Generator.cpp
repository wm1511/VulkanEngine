#include "Generator.hpp"

#include <stack>
#include <list>
#include <cassert>

namespace mg
{
	Cell::Cell() : x{ 0 }, z{ 0 }, up{ true }, left{ true }, right{ true }, down{ true }, visited{ false } {}

	void Cell::init(const int x, const int z)
	{
		this->x = x;
		this->z = z;
	}

	Generator::Generator(const WorldInfo& worldInfo) : mapSize(worldInfo.mazeSize)
	{
		assert(mapSize > 1 && "Size must be greater than 1");
		assert(abs(worldInfo.startX) < mapSize && abs(worldInfo.startZ) < mapSize && "Start points must belong to the grid");
		map.resize(mapSize);
		for (int i = 0; i < mapSize; i++)
		{
			map[i].resize(mapSize);
			for (int j = 0; j < mapSize; j++)
				map[i][j].init(i, j);
		}
		generate(worldInfo.startX, worldInfo.startZ);
	}

	std::vector<Cell> Generator::getAdjacentUnvisited(const Cell& currentCell) const
	{
		std::vector<Cell> adjacent = {};

		if (currentCell.x > 0 && !map[currentCell.x - 1][currentCell.z].visited)
			adjacent.push_back(map[currentCell.x - 1][currentCell.z]);

		if (currentCell.x < mapSize - 1 && !map[currentCell.x + 1][currentCell.z].visited)
			adjacent.push_back(map[currentCell.x + 1][currentCell.z]);

		if (currentCell.z > 0 && !map[currentCell.x][currentCell.z - 1].visited)
			adjacent.push_back(map[currentCell.x][currentCell.z - 1]);

		if (currentCell.z < mapSize - 1 && !map[currentCell.x][currentCell.z + 1].visited)
			adjacent.push_back(map[currentCell.x][currentCell.z + 1]);

		return adjacent;
	}

	void Generator::eraseWalls(const Cell& currentCell, const Cell& nextCell)
	{
		const int directionX = nextCell.x - currentCell.x;
		const int directionY = nextCell.z - currentCell.z;

		if (directionX == 0 && directionY == -1)
		{
			map[currentCell.x][currentCell.z].up = false;
			map[nextCell.x][nextCell.z].down = false;
		}

		if (directionX == 0 && directionY == 1)
		{
			map[currentCell.x][currentCell.z].down = false;
			map[nextCell.x][nextCell.z].up = false;
		}

		if (directionX == -1 && directionY == 0)
		{
			map[currentCell.x][currentCell.z].left = false;
			map[nextCell.x][nextCell.z].right = false;
		}

		if (directionX == 1 && directionY == 0)
		{
			map[currentCell.x][currentCell.z].right = false;
			map[nextCell.x][nextCell.z].left = false;
		}
	}

	void Generator::generate(const int startX, const int startZ)
	{
		std::random_device randDevice;
		std::mt19937 generator(randDevice());
		std::uniform_int_distribution<std::mt19937::result_type> distribution(1, INT32_MAX);

		int unvisitedCount = mapSize * mapSize;

		auto stack = std::stack<Cell, std::list<Cell>>();

		Cell currentCell = map[startX][startZ];
		currentCell.visited = true;
		unvisitedCount--;

		while (unvisitedCount > 0)
		{
			std::vector<Cell> adjacentUnvisited = getAdjacentUnvisited(currentCell);

			if (!adjacentUnvisited.empty())
			{
				Cell nextCell = adjacentUnvisited[distribution(generator) % adjacentUnvisited.size()];
				stack.emplace(currentCell);
				eraseWalls(currentCell, nextCell);

				currentCell = nextCell;
				map[currentCell.x][currentCell.z].visited = true;
				unvisitedCount--;
			}
			else 
			{
				currentCell = stack.top();
				stack.pop();
			}
		}
	}
}