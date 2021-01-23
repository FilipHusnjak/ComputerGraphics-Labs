#include "neopch.h"

#include "VulkanShader.h"

#include "FileTools.h"
#include "Tools/FileTools.h"

Neon::VulkanShader::VulkanShader(vk::Device device, vk::ShaderStageFlagBits stage)
	: m_Device(device)
	, m_Stage(stage)
{
}

void Neon::VulkanShader::LoadFromFile(const std::string& fileName)
{
	std::vector<char> code = ReadFile(fileName);
	vk::ShaderModuleCreateInfo createInfo{
		{}, code.size(), reinterpret_cast<const uint32_t*>(code.data())};
	m_Module = m_Device.createShaderModuleUnique(createInfo);
}

vk::PipelineShaderStageCreateInfo Neon::VulkanShader::GetShaderStageCreateInfo() const
{
	return {{}, m_Stage, m_Module.get(), "main"};
}
