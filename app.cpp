#include "app.hpp"
#include "renderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <cassert>

namespace wme
{
	App::App() { loadGameObjects(); }

	App::~App() {}

	void App::run()
	{
		RenderSystem renderSystem{wmeDevice, wmeRenderer.getSwapChainRenderPass()};
		while (!wmeWindow.shouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = wmeRenderer.beginFrame())
			{
				wmeRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(commandBuffer, gameObjects);
				wmeRenderer.endSwapChainRenderPass(commandBuffer);
				wmeRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(wmeDevice.device());
	}

	void App::loadGameObjects()
	{
		std::vector<WmeModel::Vertex> vertices
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		};
		auto wmeModel = std::make_shared<WmeModel>(wmeDevice, vertices);

		auto triangle = WmeGameObject::createGameObject();
		triangle.model = wmeModel;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.0f, 0.5f };
		triangle.transform2d.rotation = -0.25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}
}
