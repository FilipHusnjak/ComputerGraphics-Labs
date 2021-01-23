#pragma once

#include <queue>

#include <vk_mem_alloc.h>

#include <stb_image.h>
#include <vulkan/vulkan.hpp>

namespace Neon
{
struct BufferAllocation
{
	~BufferAllocation();
	VkBuffer m_Buffer{};
	VmaAllocation m_Allocation{};
};
struct ImageAllocation
{
	~ImageAllocation();
	VkImage m_Image{};
	VmaAllocation m_Allocation{};
};
struct TextureImage
{
	TextureImage() = default;
	TextureImage(vk::DescriptorImageInfo descriptor,
				 std::unique_ptr<ImageAllocation>& textureAllocation)
		: m_Descriptor(descriptor)
		, m_TextureAllocation(std::move(textureAllocation))
	{
	}
	~TextureImage();
	TextureImage(TextureImage&& other) noexcept
	{
		*this = std::move(other);
	}
	TextureImage& operator=(TextureImage&& other) noexcept
	{
		m_Descriptor = other.m_Descriptor;
		other.m_Descriptor = vk::DescriptorImageInfo{};
		m_TextureAllocation = std::move(other.m_TextureAllocation);
		return *this;
	}
	vk::DescriptorImageInfo m_Descriptor{};
	std::unique_ptr<ImageAllocation> m_TextureAllocation{};
};
class Allocator
{
public:
	Allocator(const Allocator&) = delete;
	Allocator(Allocator&&) = delete;
	Allocator& operator=(const Allocator&) = delete;
	Allocator& operator=(Allocator&&) = delete;

	static void Init(vk::PhysicalDevice physicalDevice, vk::Device device);
	static void FlushStaging();
	static std::unique_ptr<BufferAllocation> CreateBuffer(const vk::DeviceSize& size,
														  const vk::BufferUsageFlags& usage,
														  const VmaMemoryUsage& memoryUsage);

	static std::unique_ptr<ImageAllocation>
	CreateImage(uint32_t width, uint32_t height, const vk::SampleCountFlagBits& sampleCount,
				const vk::Format& format, const vk::ImageTiling& tiling,
				const vk::ImageUsageFlags& usage, const VmaMemoryUsage& memoryUsage);

	static void TransitionImageLayout(vk::Image image, vk::ImageAspectFlagBits aspect,
									  vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	static std::unique_ptr<ImageAllocation> CreateTextureImage(const std::string& filename);
	static std::unique_ptr<ImageAllocation> CreateTextureImage(stbi_uc* pixels, int texWidth,
															   int texHeight);

	static std::unique_ptr<ImageAllocation> CreateHdrTextureImage(const std::string& filename);

	template<typename T>
	static void UpdateAllocation(const VmaAllocation& allocation, const T& data)
	{
		void* mappedData;
		vmaMapMemory(s_Allocator.m_Allocator, allocation, &mappedData);
		memcpy(mappedData, &data, sizeof(T));
		vmaUnmapMemory(s_Allocator.m_Allocator, allocation);
	}

	template<typename T>
	static void UpdateAllocation(const VmaAllocation& allocation, const std::vector<T>& data)
	{
		void* mappedData;
		vmaMapMemory(s_Allocator.m_Allocator, allocation, &mappedData);
		memcpy(mappedData, data.data(), data.size() * sizeof(T));
		vmaUnmapMemory(s_Allocator.m_Allocator, allocation);
	}

	template<typename T>
	static std::unique_ptr<BufferAllocation>
	CreateDeviceLocalBuffer(const vk::CommandBuffer& commandBuffer, const std::vector<T>& data,
							const vk::BufferUsageFlags& usage)
	{
		vk::DeviceSize bufferSize = sizeof(data[0]) * data.size();

		std::unique_ptr<BufferAllocation> stagingBufferAllocation = CreateBuffer(
			bufferSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_GPU_TO_CPU);

		void* mappedData;
		vmaMapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation, &mappedData);
		memcpy(mappedData, data.data(), (size_t)bufferSize);
		vmaUnmapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation);

		std::unique_ptr<BufferAllocation> resultBufferAllocation = CreateBuffer(
			bufferSize, vk::BufferUsageFlagBits::eTransferDst | usage, VMA_MEMORY_USAGE_GPU_ONLY);

		vk::BufferCopy copyRegion{0, 0, bufferSize};

		commandBuffer.copyBuffer(stagingBufferAllocation->m_Buffer,
								 resultBufferAllocation->m_Buffer, 1, &copyRegion);
		s_Allocator.m_StagingBuffers.push_back(std::move(stagingBufferAllocation));
		return std::move(resultBufferAllocation);
	}

	static void FreeMemory(VmaAllocation allocation);
	static void DestroyImageAllocation(ImageAllocation& imageAllocation);
	static void DestroyBufferAllocation(BufferAllocation& bufferAllocation);
	static void DestroyTextureImage(TextureImage& textureImage);

private:
	Allocator() noexcept;

private:
	static Allocator s_Allocator;
	VmaAllocator m_Allocator{};
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;
	std::vector<std::unique_ptr<BufferAllocation>> m_StagingBuffers;
};
} // namespace Neon
