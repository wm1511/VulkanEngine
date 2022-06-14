#pragma once

#include "wmeDevice.hpp"
#include "wmeSwapChain.hpp"
#include "wmeWindow.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace wme
{
	class WmeRenderer
	{
	public:
		WmeRenderer(WmeWindow& window, WmeDevice& device);
		~WmeRenderer();
		WmeRenderer(const WmeRenderer&) = delete;
		WmeRenderer& operator= (const WmeRenderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() const 
		{ 
			assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const
		{
			assert(isFrameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

		VkRenderPass getSwapChainRenderPass() const { return wmeSwapChain->getRenderPass(); }

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		WmeWindow& wmeWindow;
		WmeDevice& wmeDevice;
		std::unique_ptr<WmeSwapChain> wmeSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};
}