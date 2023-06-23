#include "Grid.hpp"

namespace mg
{
	GridCell::GridCell(const bool isFull) : full{ isFull } {}

	void Grid::prepareMap()
	{
		grid.resize(gridSize);
		for (int i = 0; i < gridSize; i++)
			grid[i].resize(gridSize);
	}

	Grid1To2::Grid1To2(WorldInfo& info) : worldInfo(info)
	{
		gridSize = 2 * worldInfo.mazeSize + 2;
		prepareMap();
		makeGrid();
	}

	void Grid1To2::prepareMap()
	{
		const std::vector<GridCell> full(gridSize, { GridCell(true) });
		grid.push_back(full);

		std::vector<GridCell> startEnd{};
		startEnd.push_back({ GridCell(true) });
		startEnd.resize(gridSize - 1);
		startEnd.push_back({ GridCell(true) });
		for (int i = 0; i < gridSize - 2; i++)
			grid.push_back(startEnd);
		grid.push_back(full);
	}

	void Grid1To2::makeGrid()
	{
		const auto map = Generator(worldInfo).getMap();

		for (size_t i = 0; i < map.size(); i++)
		{
			for (size_t j = 0; j < map.size(); j++)
			{
				if (map[j][i].down)
				{
					grid[2 * i + 3][2 * j + 1].full = true;
					grid[2 * i + 3][2 * j + 2].full = true;
				}
				if (map[j][i].right)
				{
					grid[2 * i + 1][2 * j + 3].full = true;
					grid[2 * i + 2][2 * j + 3].full = true;
				}
			}
		}
	}

	Grid1To3::Grid1To3(WorldInfo& info) : worldInfo(info)
	{
		gridSize = 3 * worldInfo.mazeSize;
		prepareMap();
		makeGrid();
	}

	void Grid1To3::makeGrid()
	{
		const auto map = Generator(worldInfo).getMap();

		for (size_t i = 0; i < map.size(); i++)
		{
			for (size_t j = 0; j < map.size(); j++)
			{
				grid[3 * i][3 * j].full = true;
				grid[3 * i + 2][3 * j].full = true;
				grid[3 * i][3 * j + 2].full = true;
				grid[3 * i + 2][3 * j + 2].full = true;

				if (map[i][j].up)
					grid[3 * i + 1][3 * j].full = true;
				if (map[i][j].left)
					grid[3 * i][3 * j + 1].full = true;
				if (map[i][j].down)
					grid[3 * i + 1][3 * j + 2].full = true;
				if (map[i][j].right)
					grid[3 * i + 2][3 * j + 1].full = true;
			}
		}
	}
}