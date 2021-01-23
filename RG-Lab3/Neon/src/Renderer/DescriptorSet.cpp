#include "DescriptorSet.h"

void Neon::DescriptorSet::Init(vk::Device device)
{
	m_Device = device;
}

void Neon::DescriptorSet::Create(vk::DescriptorPool pool,
								 const std::vector<vk::DescriptorSetLayoutBinding>& bindings)
{
	vk::DescriptorSetLayoutCreateInfo layoutInfo({}, static_cast<uint32_t>(bindings.size()),
												 bindings.data());
	m_Layout = m_Device.createDescriptorSetLayoutUnique(layoutInfo);
	m_Bindings = bindings;
	assert(m_Layout && m_Layout);

	vk::DescriptorSetAllocateInfo allocInfo(pool, 1, &m_Layout.get());
	m_Set = m_Device.allocateDescriptorSets(allocInfo)[0];
}

vk::WriteDescriptorSet Neon::DescriptorSet::CreateWrite(const size_t binding,
														const vk::DescriptorBufferInfo* info,
														const uint32_t arrayElement)
{
	return {{},
			m_Bindings[binding].binding,
			arrayElement,
			m_Bindings[binding].descriptorCount,
			m_Bindings[binding].descriptorType,
			nullptr,
			info};
}

vk::WriteDescriptorSet Neon::DescriptorSet::CreateWrite(const size_t binding,
														const vk::DescriptorImageInfo* info,
														const uint32_t arrayElement)
{
	return {{},
			m_Bindings[binding].binding,
			arrayElement,
			m_Bindings[binding].descriptorCount,
			m_Bindings[binding].descriptorType,
			info};
}

void Neon::DescriptorSet::Update(std::vector<vk::WriteDescriptorSet>& descriptorWrites)
{
	assert(descriptorWrites.size() <= m_Bindings.size());
	for (auto& wr : descriptorWrites)
	{
		wr.setDstSet(m_Set);
	}
	m_Device.updateDescriptorSets(descriptorWrites, nullptr);
}
