#include "pointLightSystem.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <map>

namespace wme
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

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
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

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

	void PointLightSystem::createPipeline(VkRenderPass renderPass)
	{
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		WmePipeline::defaultPipelineConfigInfo(pipelineConfig);
		WmePipeline::enableAlphaBlending(pipelineConfig);
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

	void PointLightSystem::update(mg::WorldInfo& worldInfo, FrameInfo& frameInfo, GlobalUbo& ubo)
	{
		auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, { 0.f, -1.f, 0.f });
		int lightIndex = 0;
		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr) 
				continue;

			assert(lightIndex < MAX_LIGHTS && "Point lights count exceed maximum specified");

			if (!lightIndex)
			{
				ubo.pointLights[lightIndex].position = glm::vec4(frameInfo.camera.getPosition().x, frameInfo.camera.getPosition().y + .5f, frameInfo.camera.getPosition().z, 1.0f);
				float green = (frameInfo.camera.getPosition().x + worldInfo.startX + frameInfo.camera.getPosition().z + worldInfo.startZ) / (4.f * worldInfo.unit * worldInfo.mazeSize);
				float red = 1.f - green;
				float blue = .5f;
				ubo.pointLights[lightIndex].color = glm::vec4(glm::vec3(red, green, blue), obj.pointLight->lightIntensity);
			}
			else
			{
				obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

				ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
				ubo.pointLights[lightIndex].color = glm::vec4(glm::vec3(1.0f), obj.pointLight->lightIntensity);
			}

			lightIndex++;
		}
		ubo.numLights = lightIndex;
	}

	void PointLightSystem::render(FrameInfo& frameInfo)
	{
		std::map<float, WmeGameObject::id_t> sorted;
		for (auto& kv : frameInfo.gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr)
				continue;

			auto offset = frameInfo.camera.getPosition() - obj.transform.translation;
			float distSquared = glm::dot(offset, offset);
			sorted[distSquared] = obj.getId();
		}

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

		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			auto& obj = frameInfo.gameObjects.at(it->second);

			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform.translation, 1.0f);
			push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			push.radius = obj.transform.scale.x;

			vkCmdPushConstants(
				frameInfo.commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push);

			vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
		}
	}
}
