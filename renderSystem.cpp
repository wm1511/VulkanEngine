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
		glm::mat4 transform{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
	};

	RenderSystem::RenderSystem(WmeDevice& device, VkRenderPass renderPass) : wmeDevice{device}
	{
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem()
	{
		vkDestroyPipelineLayout(wmeDevice.device(), pipelineLayout, nullptr);
	}

	void RenderSystem::createPipelineLayout()
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

	void RenderSystem::renderGameObjects(FrameInfo& frameInfo, std::vector<WmeGameObject>& gameObjects)
	{
		wmePipeline->bind(frameInfo.commandBuffer);

		for (auto& obj : gameObjects)
		{
			auto projectionView = frameInfo.camera.getProjection() * frameInfo.camera.getView();

			PushConstantsData push{};
			auto modelMatrix = obj.transform.mat4();
			push.transform = projectionView * modelMatrix;
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
