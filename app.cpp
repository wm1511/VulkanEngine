#include "app.hpp"

#include "keyboardController.hpp"
#include "wmeCamera.hpp"
#include "renderSystem.hpp"
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
	struct GlobalUbo
	{
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
	};

	App::App() { loadGameObjects(); }

	App::~App() {}

	void App::run()
	{
		WmeBuffer globalUboBuffer
		{
			wmeDevice,
			sizeof(GlobalUbo),
			WmeSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			wmeDevice.properties.limits.minUniformBufferOffsetAlignment
		};
		globalUboBuffer.map();

		RenderSystem renderSystem{wmeDevice, wmeRenderer.getSwapChainRenderPass()};
        WmeCamera camera{};
    
        auto viewerObject = WmeGameObject::createGameObject();
        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!wmeWindow.shouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(wmeWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			
            float aspectRatio = wmeRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 10.0f);
			
            if (auto commandBuffer = wmeRenderer.beginFrame())
			{
				int frameIndex = wmeRenderer.getFrameIndex();
				FrameInfo frameInfo
				{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				globalUboBuffer.writeToIndex(&ubo, frameIndex);
				globalUboBuffer.flushIndex(frameIndex);

				wmeRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo, gameObjects);
				wmeRenderer.endSwapChainRenderPass(commandBuffer);
				wmeRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(wmeDevice.device());
	}

	void App::loadGameObjects()
	{
		std::shared_ptr<WmeModel> wmeModel = WmeModel::createModelFromFile(wmeDevice, "models/porsche_911.obj");

        auto gameObj = WmeGameObject::createGameObject();
        gameObj.model = wmeModel;
        gameObj.transform.translation = { .0f, .0f, 3.0f };
        gameObj.transform.scale = { 1.0f, 1.0f, 1.0f };
		gameObj.transform.rotation = { 3.1415f, 3.1415f, .0f};
        gameObjects.push_back(std::move(gameObj));
	}
}
