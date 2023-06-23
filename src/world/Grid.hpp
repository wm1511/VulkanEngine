#pragma once

#include "Generator.hpp"

namespace mg
{
	class GridCell
	{
	public:
		explicit GridCell(bool isFull = false);

		bool full;
	};

	class Grid
	{
	public:
		virtual ~Grid() = default;

		virtual std::vector<std::vector<GridCell>>& getGrid() { return grid; }

	protected:
		virtual void makeGrid() = 0;
		virtual void prepareMap();

		int gridSize{};
		std::vector<std::vector<GridCell>> grid;
	};

	class Grid1To2 final : public Grid
	{
	public:
		explicit Grid1To2(WorldInfo& info);

	private:
		void makeGrid() override;
		void prepareMap() override;

		WorldInfo& worldInfo;
	};

	class Grid1To3 final : public Grid
	{
	public:
		explicit Grid1To3(WorldInfo& info);

	private:
		void makeGrid() override;

		WorldInfo& worldInfo;
	};
}