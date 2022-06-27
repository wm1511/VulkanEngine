#pragma once

#include "wmeDevice.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace wme
{
	struct PipelineConfigInfo 
	{
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator = (const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};


	class WmePipeline
	{
	public:
		WmePipeline(
			WmeDevice& device, 
			const std::filesystem::path& vertPath, 
			const std::filesystem::path& fragPath,
			const PipelineConfigInfo& configInfo);
		~WmePipeline();
		WmePipeline(const WmePipeline&) = delete;
		WmePipeline& operator = (const WmePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::filesystem::path& filePath);
		void createGraphicsPipeline(
			const std::filesystem::path& vertPath, 
			const std::filesystem::path& fragPath, 
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		WmeDevice& wmeDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}