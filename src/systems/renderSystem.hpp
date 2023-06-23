#pragma once

#include "..\world\WorldInfo.hpp"
#include "..\wmeCamera.hpp"
#include "..\wmeDevice.hpp"
#include "..\wmeFrameInfo.hpp"
#include "..\wmePipeline.hpp"
#include "..\wmeGameObject.hpp"

#include <memory>
#include <vector>

namespace wme
{
	class RenderSystem
	{
	public:
		RenderSystem(WmeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~RenderSystem();
		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator= (const RenderSystem&) = delete;

		void renderGameObjects(FrameInfo& frameInfo, mg::WorldInfo& worldInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		WmeDevice& wmeDevice;
		std::unique_ptr<WmePipeline> wmePipeline;
		VkPipelineLayout pipelineLayout;
	};
}