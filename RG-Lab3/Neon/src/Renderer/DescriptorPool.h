#pragma once

#include <vulkan/vulkan.hpp>

namespace Neon
{
class DescriptorPool
{
public:
	DescriptorPool(const DescriptorPool&) = delete;
	DescriptorPool(DescriptorPool&&) = delete;
	DescriptorPool& operator=(const DescriptorPool&) = delete;
	DescriptorPool& operator=(DescriptorPool&&) = delete;

	[[nodiscard]] const vk::DescriptorPool& GetHandle() const
	{
		return m_Handle.get();
	}

	static std::unique_ptr<DescriptorPool>
	Create(vk::Device device, const std::vector<vk::DescriptorSetLayoutBinding>& bindings,
		   uint32_t maxSets);

	static std::unique_ptr<DescriptorPool>
	Create(vk::Device device, const std::vector<vk::DescriptorPoolSize>& sizes, uint32_t maxSets);

private:
	explicit DescriptorPool(vk::Device device, const std::vector<vk::DescriptorPoolSize>& sizes,
							uint32_t maxSets);

private:
	vk::UniqueDescriptorPool m_Handle;
};
} // namespace Neon
