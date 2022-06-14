#pragma once

#include "wmeCamera.hpp"
#include "wmeDevice.hpp"
#include "wmePipeline.hpp"
#include "wmeGameObject.hpp"

#include <memory>
#include <vector>

namespace wme
{
	class RenderSystem
	{
	public:
		RenderSystem(WmeDevice& device, VkRenderPass renderPass);
		~RenderSystem();
		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator= (const RenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<WmeGameObject>& gameObjects, const WmeCamera& camera);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		WmeDevice& wmeDevice;
		std::unique_ptr<WmePipeline> wmePipeline;
		VkPipelineLayout pipelineLayout;
	};
}