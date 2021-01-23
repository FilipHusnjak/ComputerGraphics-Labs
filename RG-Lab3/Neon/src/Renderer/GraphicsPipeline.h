#pragma once

#include "Renderer/VulkanShader.h"
#include "VulkanShader.h"

#include <vulkan/vulkan.hpp>

namespace Neon
{
class GraphicsPipeline
{
public:
	GraphicsPipeline() = default;
	explicit operator vk::Pipeline() const
	{
		return m_Pipeline.get();
	}
	void Init(vk::Device device);
	void LoadVertexShader(const std::string& file);
	void LoadFragmentShader(const std::string& file);
	void CreatePipelineLayout(std::vector<vk::DescriptorSetLayout> descLayouts,
							  std::vector<vk::PushConstantRange> pushConstRanges);
	void CreatePipeline(vk::RenderPass renderPass, vk::SampleCountFlagBits samples,
						vk::Extent2D extent,
						std::vector<vk::VertexInputBindingDescription> bindingDesc,
						std::vector<vk::VertexInputAttributeDescription> attributeDesc,
						vk::CullModeFlagBits cullMode);

	[[nodiscard]] inline vk::PipelineLayout GetLayout() const
	{
		return m_Layout.get();
	}

private:
	vk::Device m_Device;
	std::vector<VulkanShader> m_Shaders;
	vk::UniquePipelineLayout m_Layout;
	vk::UniquePipeline m_Pipeline;
};
} // namespace Neon
