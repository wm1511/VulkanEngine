#pragma once

#include "wmeDevice.hpp"
#include "wmePipeline.hpp"
#include "wmeSwapChain.hpp"
#include "wmeWindow.hpp"

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
		void operator = (const App&) = delete;

		void run();

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		WmeWindow wmeWindow{ WIDTH, HEIGHT, "Test Vulkan" };
		WmeDevice wmeDevice{ wmeWindow };
		WmeSwapChain wmeSwapChain{ wmeDevice, wmeWindow.getExtent() };
		std::unique_ptr<WmePipeline> wmePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}