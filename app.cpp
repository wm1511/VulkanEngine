#include "app.hpp"

#include <stdexcept>

namespace wme
{
	App::App()
	{
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	App::~App()
	{
		vkDestroyPipelineLayout(wmeDevice.device(), pipelineLayout, nullptr);

	}

	void App::run()
	{
		while (!wmeWindow.shouldClose())
		{
			glfwPollEvents();
		}
	}

	void App::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineInfo.setLayoutCount = 0;
		pipelineInfo.pSetLayouts = nullptr;
		pipelineInfo.pushConstantRangeCount = 0;
		pipelineInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(wmeDevice.device(), &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	void App::createPipeline()
	{
		auto pipelineConfig = WmePipeline::defaultPipelineConfigInfo(wmeSwapChain.width(), wmeSwapChain.height());
		pipelineConfig.renderPass = wmeSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		wmePipeline = std::make_unique<WmePipeline>(
			wmeDevice,
			std::filesystem::path("shaders/vert.spv"),
			std::filesystem::path("shaders/frag.spv"),
			pipelineConfig);
	}

	void App::createCommandBuffers()
	{

	}

	void App::drawFrame()
	{

	}
}
