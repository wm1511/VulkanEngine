#pragma once

#include "wmeDevice.hpp"
#include "wmeRenderer.hpp"
#include "wmeWindow.hpp"
#include "wmeGameObject.hpp"
#include "wmeDescriptors.hpp"

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
		void loadGameObjects();

		WmeWindow wmeWindow{ WIDTH, HEIGHT, "Test Vulkan" };
		WmeDevice wmeDevice{ wmeWindow };
		WmeRenderer wmeRenderer{ wmeWindow, wmeDevice };

		std::unique_ptr<WmeDescriptorPool> globalPool;
		WmeGameObject::Map gameObjects;
	};
}