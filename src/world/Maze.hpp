#pragma once

#include "Grid.hpp"

#include "../wmeGameObject.hpp"

namespace mg
{
	class Maze
	{
	public:
		Maze(WorldInfo& worldInfo, wme::WmeGameObject::Map& gameObjects);
		void quadsFromGen(const std::shared_ptr<wme::WmeModel>& model) const;
		void cubesFromGen(const std::shared_ptr<wme::WmeModel>& model) const;
		void cubesFrom1To2(const std::shared_ptr<wme::WmeModel>& model) const;
		void cubesFrom1To3(const std::shared_ptr<wme::WmeModel>& model) const;

	private:
		WorldInfo& worldInfo;
		wme::WmeGameObject::Map& gameObjects;
	};
}