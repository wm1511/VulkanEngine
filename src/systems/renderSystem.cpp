#include "renderSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>

namespace wme
{
	struct PushConstantsData
	{
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	RenderSystem::RenderSystem(WmeDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : wmeDevice{device}
	{
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(wmeDevice.device(), pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantsData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineInfo.pushConstantRangeCount = 1;
		pipelineInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(wmeDevice.device(), &pipelineInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		WmePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;
		wmePipeline = std::make_unique<WmePipeline>(
			wmeDevice,
			std::filesystem::path("shaders/vert.spv"),
			std::filesystem::path("shaders/frag.spv"),
			pipelineConfig);
	}

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, mg::WorldInfo& worldInfo)
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

		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.model == nullptr) 
				continue;

			if (glm::length(frameInfo.camera.getPosition() - obj.transform.translation) > worldInfo.renderDistance)
				continue;

			PushConstantsData push{};
			push.modelMatrix = obj.transform.mat4();
			push.normalMatrix = obj.transform.normalMatrix();

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstantsData),
				&push);
			obj.model->bind(frameInfo.commandBuffer);
			obj.model->draw(frameInfo.commandBuffer);
		}
	}
}
