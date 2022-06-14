#include "wmeRenderer.hpp"

#include <stdexcept>
#include <array>

namespace wme
{

	WmeRenderer::WmeRenderer(WmeWindow& window, WmeDevice& device) : wmeWindow{window}, wmeDevice{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	WmeRenderer::~WmeRenderer()
	{
		freeCommandBuffers();
	}

	VkCommandBuffer WmeRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Cannot start a frame while already in progress");

		auto result = wmeSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image");

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer");

		return commandBuffer;
	}

	void WmeRenderer::endFrame()
	{
		assert(isFrameStarted && "Cannot end a frame while not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");

		auto result = wmeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || wmeWindow.wasWindowResized())
		{
			wmeWindow.resetWindowResizedFlag();
			recreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image");

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % WmeSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void WmeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot start a render pass while not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot start a render pass on command buffer from another frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = wmeSwapChain->getRenderPass();
		renderPassInfo.framebuffer = wmeSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = wmeSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(wmeSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(wmeSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, wmeSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void WmeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Cannot end a render pass while not in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end a render pass on command buffer from another frame");

		vkCmdEndRenderPass(commandBuffer);
	}

	void WmeRenderer::createCommandBuffers()
	{
		commandBuffers.resize(WmeSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = wmeDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(wmeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers");
	}

	void WmeRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(wmeDevice.device(), wmeDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		commandBuffers.clear();
	}

	void WmeRenderer::recreateSwapChain()
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
		{
			std::shared_ptr<WmeSwapChain> oldSwapChain = std::move(wmeSwapChain);
			wmeSwapChain = std::make_unique<WmeSwapChain>(wmeDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormats(*wmeSwapChain.get()))
				throw std::runtime_error("Swap chain image or depth format has changed");
		}
	}
}
