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
	class PointLightSystem
	{
	public:
		PointLightSystem(WmeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();
		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator= (const PointLightSystem&) = delete;

		void update(mg::WorldInfo& worldInfo, FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		WmeDevice& wmeDevice;
		std::unique_ptr<WmePipeline> wmePipeline;
		VkPipelineLayout pipelineLayout;
	};
}