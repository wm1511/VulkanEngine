#pragma once

#include "wmeDevice.hpp"
#include "wmeRenderer.hpp"
#include "wmeWindow.hpp"
#include "wmeGameObject.hpp"
#include "wmeDescriptors.hpp"
#include "systems/pointLightSystem.hpp"

#include <memory>
#include <vector>

namespace wme
{
	class App
	{
	public:
		static constexpr int HEIGHT = 720;
		static constexpr int WIDTH = 1280;

		App();
		~App();
		App(const App&) = delete;
		App& operator= (const App&) = delete;

		void run();

	private:
		WmeWindow wmeWindow{ WIDTH, HEIGHT, "VkMaze" };
		WmeDevice wmeDevice{ wmeWindow };
		WmeRenderer wmeRenderer{ wmeWindow, wmeDevice };

		std::unique_ptr<WmeDescriptorPool> globalPool;
		WmeGameObject::Map gameObjects;
		mg::WorldInfo worldInfo;
	};
}