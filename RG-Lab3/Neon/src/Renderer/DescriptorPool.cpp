#include "DescriptorPool.h"

std::unique_ptr<Neon::DescriptorPool>
Neon::DescriptorPool::Create(const vk::Device device,
							 const std::vector<vk::DescriptorSetLayoutBinding>& bindings,
							 const uint32_t maxSets)
{
	std::vector<vk::DescriptorPoolSize> sizes;
	sizes.reserve(bindings.size());
	for (const auto& b : bindings)
	{
		sizes.emplace_back(b.descriptorType, b.descriptorCount * maxSets);
	}
	return Create(device, sizes, maxSets);
}

std::unique_ptr<Neon::DescriptorPool>
Neon::DescriptorPool::Create(const vk::Device device,
							 const std::vector<vk::DescriptorPoolSize>& sizes,
							 const uint32_t maxSets)
{
	auto descriptorPool = new DescriptorPool(device, sizes, maxSets);
	return std::unique_ptr<DescriptorPool>(descriptorPool);
}

Neon::DescriptorPool::DescriptorPool(const vk::Device device,
									 const std::vector<vk::DescriptorPoolSize>& sizes,
									 const uint32_t maxSets)
{
	vk::DescriptorPoolCreateInfo poolInfo = {};
	poolInfo.setPoolSizeCount(static_cast<uint32_t>(sizes.size()));
	poolInfo.setPPoolSizes(sizes.data());
	poolInfo.setMaxSets(maxSets);
	m_Handle = device.createDescriptorPoolUnique(poolInfo);
	assert(m_Handle);
}
