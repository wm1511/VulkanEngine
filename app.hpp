#pragma once

#include "wmeDevice.hpp"
#include "wmePipeline.hpp"
#include "wmeSwapChain.hpp"
#include "wmeWindow.hpp"
#include "wmeModel.hpp"

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
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		WmeWindow wmeWindow{ WIDTH, HEIGHT, "Test Vulkan" };
		WmeDevice wmeDevice{ wmeWindow };
		std::unique_ptr<WmeSwapChain> wmeSwapChain;
		std::unique_ptr<WmePipeline> wmePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<WmeModel> wmeModel;
	};
}