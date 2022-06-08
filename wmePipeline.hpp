#pragma once

#include "wmeDevice.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace wme
{
	struct PipelineConfigInfo 
	{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
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
		void operator = (const WmePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

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