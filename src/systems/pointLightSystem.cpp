#include "pointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace wme
{
	PointLightSystem::PointLightSystem(WmeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : wmeDevice{device}
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(wmeDevice.device(), pipelineLayout, nullptr);
	}

	void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		/*VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantsData);*/

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineInfo.pushConstantRangeCount = 0;
		pipelineInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(wmeDevice.device(), &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		WmePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.attributeDescriptions.clear();
		pipelineConfig.bindingDescriptions.clear();
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		wmePipeline = std::make_unique<WmePipeline>(
			wmeDevice,
			std::filesystem::path("shaders/pointLightVert.spv"),
			std::filesystem::path("shaders/pointLightFrag.spv"),
			pipelineConfig);
	}

	void PointLightSystem::render(FrameInfo& frameInfo)
	{
		wmePipeline->bind(frameInfo.commandBuffer);

		vkCmdBindDescriptorSets(
			frameInfo.commandBuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			pipelineLayout,
			0,
			1,
			&frameInfo.globalDescriptorSet,
			0,
			nullptr);

		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}
