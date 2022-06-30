#pragma once

#include "wmeCamera.hpp"
#include "wmeGameObject.hpp"

#include "vulkan/vulkan.h"

namespace wme
{

#define MAX_LIGHTS 10

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{};
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverseView{ 1.f };
		glm::vec4 ambientLightColor{ 1.0f, 1.0f, 1.0f, .02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};

	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		WmeCamera& camera;
		VkDescriptorSet globalDescriptorSet;
		WmeGameObject::Map& gameObjects;
	};
}