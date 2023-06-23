#include "app.hpp"

#include "world/Maze.hpp"
#include "keyboardController.hpp"
#include "mouseController.hpp"
#include "wmeCamera.hpp"
#include "systems/renderSystem.hpp"
#include "wmeBuffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <chrono>
#include <cassert>

namespace wme
{
	App::App() 
	{ 
		globalPool = WmeDescriptorPool::Builder(wmeDevice)
			.setMaxSets(WmeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, WmeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		const mg::Maze maze(worldInfo, gameObjects);
		std::shared_ptr<wme::WmeModel> quadModel = wme::WmeModel::createModelFromFile(wmeDevice, "models/quad.obj");
		std::shared_ptr<wme::WmeModel> cubeModel = wme::WmeModel::createModelFromFile(wmeDevice, "models/cube.obj");
		//maze.quadsFromGen(quadModel);
		maze.cubesFromGen(cubeModel);
		//maze.cubesFrom1to2(cubeModel);
		//maze.cubesFrom1to3(cubeModel);
	}

	App::~App() {}

	void App::run()
	{
		std::vector<std::unique_ptr<WmeBuffer>> uboBuffers(WmeSwapChain::MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = std::make_unique<WmeBuffer>(
				wmeDevice,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				wmeDevice.properties.limits.minUniformBufferOffsetAlignment);
				uboBuffers[i]->map();
		}

		auto globalSetLayout = WmeDescriptorSetLayout::Builder(wmeDevice)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(WmeSwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			WmeDescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		RenderSystem renderSystem{wmeDevice, wmeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
		PointLightSystem pointLightSystem{wmeDevice, wmeRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        WmeCamera camera{};
    
        auto viewerObject = WmeGameObject::createGameObject();

        KeyboardMovementController cameraKeyboardController{};
		MouseRotationController cameraMouseController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		double cursorX = wmeWindow.getCursorX();
		double cursorY = wmeWindow.getCursorY();

		while (!wmeWindow.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			if (cameraKeyboardController.controlCursorMode(wmeWindow.getGLFWwindow()) == GLFW_CURSOR_DISABLED)
			{
				cameraMouseController.rotateView(wmeWindow.getGLFWwindow(), cursorX, cursorY, frameTime, viewerObject);
				cameraKeyboardController.moveInPlaneXZ(wmeWindow.getGLFWwindow(), frameTime, viewerObject);
				camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			}
			
			float aspectRatio = wmeRenderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 100.0f);
			
			if (auto commandBuffer = wmeRenderer.beginFrame())
			{
				int frameIndex = wmeRenderer.getFrameIndex();
				FrameInfo frameInfo
				{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex],
					gameObjects
				};

				GlobalUbo ubo{};
				ubo.projection = camera.getProjection();
				ubo.view = camera.getView();
				ubo.inverseView = camera.getInverseView();
				pointLightSystem.update(worldInfo, frameInfo, ubo);

				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				wmeRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, worldInfo);
				pointLightSystem.render(frameInfo);
				wmeRenderer.endSwapChainRenderPass(commandBuffer);
				wmeRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(wmeDevice.device());
	}
}
