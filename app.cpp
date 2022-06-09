#include "app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>

namespace wme
{
	struct PushConstantsData
	{
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	App::App()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
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
			drawFrame();
		}
		vkDeviceWaitIdle(wmeDevice.device());
	}

	void App::loadModels()
	{
		std::vector<WmeModel::Vertex> vertices
		{
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}},
		};
		wmeModel = std::make_unique<WmeModel>(wmeDevice, vertices);
	}

	void App::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantsData);

		VkPipelineLayoutCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineInfo.setLayoutCount = 0;
		pipelineInfo.pSetLayouts = nullptr;
		pipelineInfo.pushConstantRangeCount = 1;
		pipelineInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(wmeDevice.device(), &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	void App::createPipeline()
	{
		assert(wmeSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		WmePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = wmeSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		wmePipeline = std::make_unique<WmePipeline>(
			wmeDevice,
			std::filesystem::path("shaders/vert.spv"),
			std::filesystem::path("shaders/frag.spv"),
			pipelineConfig);
	}

	void App::createCommandBuffers()
	{
		commandBuffers.resize(wmeSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = wmeDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(wmeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers");
	}

	void App::freeCommandBuffers()
	{
		vkFreeCommandBuffers(wmeDevice.device(), wmeDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void App::drawFrame()
	{
		uint32_t imageIndex;
		auto result = wmeSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image");

		recordCommandBuffer(imageIndex);
		result = wmeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || wmeWindow.wasWindowResized())
		{
			wmeWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image");
	}

	void App::recreateSwapChain()
	{
		auto extent = wmeWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = wmeWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(wmeDevice.device());
		if (wmeSwapChain == nullptr)
			wmeSwapChain = std::make_unique<WmeSwapChain>(wmeDevice, extent);
		else
			wmeSwapChain = std::make_unique<WmeSwapChain>(wmeDevice, extent, std::move(wmeSwapChain));

		if (wmeSwapChain->imageCount() != commandBuffers.size())
		{
			freeCommandBuffers();
			createCommandBuffers();
		}

		createPipeline();
	}

	void App::recordCommandBuffer(int imageIndex)
	{
		static int frame = 0;
		frame = (frame + 1) % 150;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = wmeSwapChain->getRenderPass();
		renderPassInfo.framebuffer = wmeSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = wmeSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(wmeSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(wmeSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, wmeSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		wmePipeline->bind(commandBuffers[imageIndex]);
		wmeModel->bind(commandBuffers[imageIndex]);

		for (int i = 0; i < 5; i++)
		{
			PushConstantsData push{};
			push.offset = { -0.75f + frame * 0.02f, -0.45f + i * frame * 0.005f };
			push.color = { 0.0f + 0.005f * frame, 0.0f, 0.2f + 0.2f * i };

			vkCmdPushConstants(
				commandBuffers[imageIndex], 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(PushConstantsData), 
				&push);

			wmeModel->draw(commandBuffers[imageIndex]);
		}


		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");
	}
}
