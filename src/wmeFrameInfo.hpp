#pragma once

#include "wmeCamera.hpp"
#include "wmeGameObject.hpp"

#include "vulkan/vulkan.h"

namespace wme
{
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