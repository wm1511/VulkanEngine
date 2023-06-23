#include "Maze.hpp"

namespace mg
{
	Maze::Maze(WorldInfo& worldInfo, wme::WmeGameObject::Map& gameObjects)
		: worldInfo(worldInfo), gameObjects(gameObjects)
	{
		auto userLight = wme::WmeGameObject::makePointLight(.8f, 1.0f, glm::vec3(1.0f));
		gameObjects.emplace(userLight.getId(), std::move(userLight));
	}

	void Maze::quadsFromGen(const std::shared_ptr<wme::WmeModel>& model) const
	{
		auto map = Generator(worldInfo).getMap();

		for (int i = 0; i < worldInfo.mazeSize; ++i)
			for (int j = 0; j < worldInfo.mazeSize; ++j)
			{
				auto floor = wme::WmeGameObject::createGameObject();
				floor.model = model;
				floor.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), 1.5f, 2 * worldInfo.unit * (j - worldInfo.startZ) };
				floor.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
				gameObjects.emplace(floor.getId(), std::move(floor));

				auto ceiling = wme::WmeGameObject::createGameObject();
				ceiling.model = model;
				ceiling.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), -.5f * worldInfo.unit, 2 * worldInfo.unit * (j - worldInfo.startZ) };
				ceiling.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
				ceiling.transform.rotation = { glm::pi<float>(), .0f, .0f };
				gameObjects.emplace(ceiling.getId(), std::move(ceiling));

				if (map[i][j].up)
				{
					auto front = wme::WmeGameObject::createGameObject();
					front.model = model;
					front.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) - worldInfo.unit + .01f };
					front.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
					front.transform.rotation = { -glm::half_pi<float>(), .0f, .0f };
					gameObjects.emplace(front.getId(), std::move(front));
				}

				if (map[i][j].right)
				{
					auto right = wme::WmeGameObject::createGameObject();
					right.model = model;
					right.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX) + worldInfo.unit + .01f, .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) };
					right.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
					right.transform.rotation = { .0f, .0f, glm::half_pi<float>() };
					gameObjects.emplace(right.getId(), std::move(right));
				}

				if (map[i][j].down)
				{
					auto down = wme::WmeGameObject::createGameObject();
					down.model = model;
					down.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) + worldInfo.unit };
					down.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
					down.transform.rotation = { glm::half_pi<float>(), .0f, .0f };
					gameObjects.emplace(down.getId(), std::move(down));
				}

				if (map[i][j].left)
				{
					auto left = wme::WmeGameObject::createGameObject();
					left.model = model;
					left.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX) - worldInfo.unit, .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) };
					left.transform.scale = { worldInfo.unit, worldInfo.unit, worldInfo.unit };
					left.transform.rotation = { .0f, .0f, -glm::half_pi<float>() };
					gameObjects.emplace(left.getId(), std::move(left));
				}
			}
	}

	void Maze::cubesFromGen(const std::shared_ptr<wme::WmeModel>& model) const
	{
		auto map = Generator(worldInfo).getMap();

		for (int i = 0; i < worldInfo.mazeSize; ++i)
			for (int j = 0; j < worldInfo.mazeSize; ++j)
			{
				auto floor = wme::WmeGameObject::createGameObject();
				floor.model = model;
				floor.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), 1.5f, 2 * worldInfo.unit * (j - worldInfo.startZ) };
				floor.transform.scale = { worldInfo.unit, 0.01f, worldInfo.unit };
				gameObjects.emplace(floor.getId(), std::move(floor));

				auto ceiling = wme::WmeGameObject::createGameObject();
				ceiling.model = model;
				ceiling.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), -.5f * worldInfo.unit, 2 * worldInfo.unit * (j - worldInfo.startZ) };
				ceiling.transform.scale = { worldInfo.unit, 0.01f, worldInfo.unit };
				ceiling.transform.rotation = { glm::pi<float>(), .0f, .0f };
				gameObjects.emplace(ceiling.getId(), std::move(ceiling));

				if (map[i][j].up)
				{
					auto front = wme::WmeGameObject::createGameObject();
					front.model = model;
					front.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) - worldInfo.unit };
					front.transform.scale = { 1.1f * worldInfo.unit, worldInfo.unit, 0.1f * worldInfo.unit };
					gameObjects.emplace(front.getId(), std::move(front));
				}

				if (map[i][j].right)
				{
					auto right = wme::WmeGameObject::createGameObject();
					right.model = model;
					right.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX) + worldInfo.unit, .5f, 2 * worldInfo.unit * (j - worldInfo.startZ)};
					right.transform.scale = { 0.1f * worldInfo.unit, worldInfo.unit, 1.1f * worldInfo.unit };
					gameObjects.emplace(right.getId(), std::move(right));
				}

				if (map[i][j].down)
				{
					auto down = wme::WmeGameObject::createGameObject();
					down.model = model;
					down.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX), .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) + worldInfo.unit };
					down.transform.scale = { 1.1f * worldInfo.unit, worldInfo.unit, 0.1f * worldInfo.unit };
					gameObjects.emplace(down.getId(), std::move(down));
				}

				if (map[i][j].left)
				{
					auto left = wme::WmeGameObject::createGameObject();
					left.model = model;
					left.transform.translation = { 2 * worldInfo.unit * (i - worldInfo.startX) - worldInfo.unit, .5f, 2 * worldInfo.unit * (j - worldInfo.startZ) };
					left.transform.scale = { 0.1f * worldInfo.unit, worldInfo.unit, 1.1f * worldInfo.unit };
					gameObjects.emplace(left.getId(), std::move(left));
				}
			}
	}

	void Maze::cubesFrom1To2(const std::shared_ptr<wme::WmeModel>& model) const
	{
		auto grid = Grid1To2(worldInfo).getGrid();

		for (size_t i = 0; i < grid.size(); ++i)
			for (size_t j = 0; j < grid.size(); ++j)
			{
				auto floor = wme::WmeGameObject::createGameObject();
				floor.model = model;
				floor.transform.translation = { worldInfo.unit * (i - worldInfo.startX - 1), 1.5f, worldInfo.unit * (j - worldInfo.startZ - 1) };
				floor.transform.scale = { 0.5f * worldInfo.unit, 0.01f, 0.5f * worldInfo.unit };
				gameObjects.emplace(floor.getId(), std::move(floor));

				auto ceiling = wme::WmeGameObject::createGameObject();
				ceiling.model = model;
				ceiling.transform.translation = { worldInfo.unit * (i - worldInfo.startX - 1), -.5f * worldInfo.unit, worldInfo.unit * (j - worldInfo.startZ - 1) };
				ceiling.transform.scale = { 0.5f * worldInfo.unit, 0.01f, 0.5f * worldInfo.unit };
				ceiling.transform.rotation = { glm::pi<float>(), .0f, .0f };
				gameObjects.emplace(ceiling.getId(), std::move(ceiling));

				if (grid[i][j].full)
				{
					auto cubeObj = wme::WmeGameObject::createGameObject();
					cubeObj.model = model;
					cubeObj.transform.translation = { worldInfo.unit * (i - 1), .5f, worldInfo.unit * (j - 1) };
					cubeObj.transform.scale = { 0.5f * worldInfo.unit, worldInfo.unit, 0.5f * worldInfo.unit };
					gameObjects.emplace(cubeObj.getId(), std::move(cubeObj));
				}
			}
	}

	void Maze::cubesFrom1To3(const std::shared_ptr<wme::WmeModel>& model) const
	{
		auto grid = Grid1To3(worldInfo).getGrid();

		for (size_t i = 0; i < grid.size(); ++i)
			for (size_t j = 0; j < grid.size(); ++j)
			{
				auto floor = wme::WmeGameObject::createGameObject();
				floor.model = model;
				floor.transform.translation = { worldInfo.unit * (i - worldInfo.startX - 1), 1.5f, worldInfo.unit * (j - worldInfo.startZ - 1) };
				floor.transform.scale = { 0.5f * worldInfo.unit, 0.01f, 0.5f * worldInfo.unit };
				gameObjects.emplace(floor.getId(), std::move(floor));

				auto ceiling = wme::WmeGameObject::createGameObject();
				ceiling.model = model;
				ceiling.transform.translation = { worldInfo.unit * (i - worldInfo.startX - 1), -.5f * worldInfo.unit, worldInfo.unit * (j - worldInfo.startZ - 1) };
				ceiling.transform.scale = { 0.5f * worldInfo.unit, 0.01f, 0.5f * worldInfo.unit };
				ceiling.transform.rotation = { glm::pi<float>(), .0f, .0f };
				gameObjects.emplace(ceiling.getId(), std::move(ceiling));

				if (grid[i][j].full)
				{
					auto cubeObj = wme::WmeGameObject::createGameObject();
					cubeObj.model = model;
					cubeObj.transform.translation = { worldInfo.unit * (i - 1), .5f, worldInfo.unit * (j - 1) };
					cubeObj.transform.scale = { 0.5f * worldInfo.unit, worldInfo.unit, 0.5f * worldInfo.unit };
					gameObjects.emplace(cubeObj.getId(), std::move(cubeObj));
				}
			}
	}
}
