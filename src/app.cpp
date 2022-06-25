#include "app.hpp"

#include "keyboardController.hpp"
#include "mouseController.hpp"
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
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, .02f };
		glm::vec3 lightPosition{ -1.0f };
		alignas(16) glm::vec4 lightColor{ 1.0f };
	};

	App::App() 
	{ 
		globalPool = WmeDescriptorPool::Builder(wmeDevice)
			.setMaxSets(WmeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, WmeSwapChain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
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
        WmeCamera camera{};
    
        auto viewerObject = WmeGameObject::createGameObject();
        KeyboardMovementController cameraKeyboardController{};
		MouseRotationController cameraMouseController{};

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto dt = 0.01f;

		double currentCursorX = wmeWindow.getCursorX();
		double currentCursorY = wmeWindow.getCursorY();

		while (!wmeWindow.shouldClose())
		{
			glfwPollEvents();

			cameraKeyboardController.controlCursorMode(wmeWindow.getGLFWwindow());

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			while (frameTime > 0.0f)
			{
				float deltaTime = std::min(frameTime, dt);

				double shiftX = currentCursorX - wmeWindow.getCursorX();
				double shiftY = currentCursorY - wmeWindow.getCursorY();
				currentCursorX = wmeWindow.getCursorX();
				currentCursorY = wmeWindow.getCursorY();

				if (glfwGetInputMode(wmeWindow.getGLFWwindow(), GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
				{
					cameraMouseController.rotateView(wmeWindow.getGLFWwindow(), shiftX, shiftY, frameTime, viewerObject);
					cameraKeyboardController.moveInPlaneXZ(wmeWindow.getGLFWwindow(), frameTime, viewerObject);
				}

				frameTime -= deltaTime;
			}

			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);
			
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
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				wmeRenderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderGameObjects(frameInfo);
				wmeRenderer.endSwapChainRenderPass(commandBuffer);
				wmeRenderer.endFrame();
			}
		}
		vkDeviceWaitIdle(wmeDevice.device());
	}

	void App::loadGameObjects()
	{
		std::shared_ptr<WmeModel> wmeModel = WmeModel::createModelFromFile(wmeDevice, "models/smooth_vase.obj");
        auto gameObj = WmeGameObject::createGameObject();
        gameObj.model = wmeModel;
        gameObj.transform.translation = { .0f, .5f, 1.f };
        gameObj.transform.scale = { 1.0f, 1.0f, 1.0f };
		gameObj.transform.rotation = { .0f, .0f, .0f};
        gameObjects.emplace(gameObj.getId(), std::move(gameObj));
		
		wmeModel = WmeModel::createModelFromFile(wmeDevice, "models/quad.obj");
        auto floor = WmeGameObject::createGameObject();
		floor.model = wmeModel;
		floor.transform.translation = { .0f, .5f, .0f };
		floor.transform.scale = { 5.0f, 1.0f, 5.0f };
		floor.transform.rotation = { .0f, .0f, .0f};
        gameObjects.emplace(floor.getId(), std::move(floor));

		/*wmeModel = WmeModel::createModelFromFile(wmeDevice, "models/R8.obj");
		auto car = WmeGameObject::createGameObject();
		car.model = wmeModel;
		car.transform.translation = { 2.0f, .5f, .0f };
		car.transform.scale = { 1.0f, 1.0f, 1.0f };
		car.transform.rotation = { .0f, .0f, glm::pi<float>() };
		gameObjects.emplace(car.getId(), std::move(car));*/
	}
}
