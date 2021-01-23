#pragma once

#include <vulkan/vulkan.hpp>

namespace Neon
{
class VulkanShader
{
public:
	explicit VulkanShader(vk::Device device, vk::ShaderStageFlagBits stage);

	void LoadFromFile(const std::string& fileName);

	[[nodiscard]] vk::PipelineShaderStageCreateInfo GetShaderStageCreateInfo() const;

private:
	vk::Device m_Device;
	vk::UniqueShaderModule m_Module;
	vk::ShaderStageFlagBits m_Stage;
};
} // namespace Neon
