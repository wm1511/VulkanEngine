#pragma once

#include "WorldInfo.hpp"

#include <random>
#include <vector>

namespace mg
{
	class Cell
	{
	public:
		Cell();

		void init(int x, int z);

		int x, z;
		bool up, left, right, down, visited;
	};

	class Generator
	{
	public:
		[[nodiscard]] int getSize() const { return mapSize; }
		std::vector<std::vector<Cell>>& getMap() { return map; }

		explicit Generator(const WorldInfo& worldInfo);

		std::vector<Cell>& operator[] (const int pos) { return map[pos]; }

	private:
		int mapSize;
		std::vector<std::vector<Cell>> map;

		[[nodiscard]] std::vector<Cell> getAdjacentUnvisited(const Cell& currentCell) const;
		void eraseWalls(const Cell& currentCell, const Cell& nextCell);
		void generate(int startX, int startZ);
	};
}