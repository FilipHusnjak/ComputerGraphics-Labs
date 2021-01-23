#define VMA_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Allocator.h"

#include "Renderer/VulkanRenderer.h"

Neon::Allocator Neon::Allocator::s_Allocator;

Neon::Allocator::Allocator() noexcept { }

Neon::BufferAllocation::~BufferAllocation()
{
	Allocator::DestroyBufferAllocation(*this);
}

Neon::ImageAllocation::~ImageAllocation()
{
	Allocator::DestroyImageAllocation(*this);
}

Neon::TextureImage::~TextureImage()
{
	Allocator::DestroyTextureImage(*this);
}

void Neon::Allocator::Init(vk::PhysicalDevice physicalDevice, vk::Device device)
{
	s_Allocator.m_PhysicalDevice = physicalDevice;
	s_Allocator.m_LogicalDevice = device;
	VmaAllocatorCreateInfo allocatorInfo{{}, physicalDevice, device};
	allocatorInfo.flags |= (uint32_t)VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
	vmaCreateAllocator(&allocatorInfo, &s_Allocator.m_Allocator);
}

void Neon::Allocator::FlushStaging()
{
	s_Allocator.m_StagingBuffers.clear();
}

std::unique_ptr<Neon::BufferAllocation>
Neon::Allocator::CreateBuffer(const vk::DeviceSize& size, const vk::BufferUsageFlags& usage,
							  const VmaMemoryUsage& memoryUsage)
{
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;
	VkBufferCreateInfo bufferInfo{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
	bufferInfo.size = size;
	bufferInfo.usage = static_cast<VkBufferUsageFlags>(usage);
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	auto* bufferAllocation = new BufferAllocation();
	vmaCreateBuffer(s_Allocator.m_Allocator, &bufferInfo, &allocInfo, &bufferAllocation->m_Buffer,
					&bufferAllocation->m_Allocation, nullptr);
	return std::unique_ptr<BufferAllocation>(bufferAllocation);
}

std::unique_ptr<Neon::ImageAllocation>
Neon::Allocator::CreateImage(const uint32_t width, const uint32_t height,
							 const vk::SampleCountFlagBits& sampleCount, const vk::Format& format,
							 const vk::ImageTiling& tiling, const vk::ImageUsageFlags& usage,
							 const VmaMemoryUsage& memoryUsage)
{
	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = memoryUsage;
	VkImageCreateInfo imageInfo = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = static_cast<VkFormat>(format);
	imageInfo.extent = {width, height, 1};
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = static_cast<VkSampleCountFlagBits>(sampleCount);
	imageInfo.tiling = static_cast<VkImageTiling>(tiling);
	imageInfo.usage = static_cast<VkImageUsageFlags>(usage);
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	auto imageAllocation = new ImageAllocation();
	vmaCreateImage(s_Allocator.m_Allocator, &imageInfo, &allocInfo, &imageAllocation->m_Image,
				   &imageAllocation->m_Allocation, nullptr);
	return std::unique_ptr<ImageAllocation>(imageAllocation);
}

void Neon::Allocator::TransitionImageLayout(vk::Image image, vk::ImageAspectFlagBits aspect,
											vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::ImageSubresourceRange imgSubresourceRange{aspect, 0, 1, 0, 1};
	vk::ImageMemoryBarrier barrier{{},
								   {},
								   oldLayout,
								   newLayout,
								   VK_QUEUE_FAMILY_IGNORED,
								   VK_QUEUE_FAMILY_IGNORED,
								   image,
								   imgSubresourceRange};

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;
	if (oldLayout == vk::ImageLayout::eUndefined &&
		newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal &&
			 newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eGeneral)
	{
		barrier.srcAccessMask = vk::AccessFlagBits();
		barrier.dstAccessMask = vk::AccessFlagBits();

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eBottomOfPipe;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined &&
			 newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined &&
			 newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits();
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined &&
			 newLayout == vk::ImageLayout::eDepthStencilReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits();
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		assert(false);
	}
	auto commandBuffer = VulkanRenderer::BeginSingleTimeCommands();
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, {barrier});
	VulkanRenderer::EndSingleTimeCommands(commandBuffer);
}

std::unique_ptr<Neon::ImageAllocation>
Neon::Allocator::CreateTextureImage(const std::string& filename)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels =
		stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels)
	{
		texWidth = texHeight = 1;
		texChannels = 4;
		auto* color = new glm::u8vec4(255, 0, 255, 255);
		pixels = reinterpret_cast<stbi_uc*>(color);
	}

	return CreateTextureImage(pixels, texWidth, texHeight);
}

std::unique_ptr<Neon::ImageAllocation>
Neon::Allocator::CreateTextureImage(stbi_uc* pixels, int texWidth, int texHeight)
{
	vk::DeviceSize imageSize =
		static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * sizeof(glm::u8vec4);

	std::unique_ptr<BufferAllocation> stagingBufferAllocation =
		CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_GPU_TO_CPU);

	void* mappedData;
	vmaMapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation, &mappedData);
	memcpy(mappedData, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation);

	stbi_image_free(pixels);

	std::unique_ptr<ImageAllocation> imageAllocation =
		CreateImage(texWidth, texHeight, vk::SampleCountFlagBits::e1, vk::Format::eR8G8B8A8Srgb,
					vk::ImageTiling::eOptimal,
					vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
					VMA_MEMORY_USAGE_GPU_ONLY);

	TransitionImageLayout(imageAllocation->m_Image, vk::ImageAspectFlagBits::eColor,
						  vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	vk::ImageSubresourceLayers imgSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1};
	vk::BufferImageCopy region{
		0,
		0,
		0,
		imgSubresourceLayers,
		{0, 0, 0},
		vk::Extent3D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1}};

	auto commandBuffer = VulkanRenderer::BeginSingleTimeCommands();
	commandBuffer.copyBufferToImage(stagingBufferAllocation->m_Buffer, imageAllocation->m_Image,
									vk::ImageLayout::eTransferDstOptimal, {region});
	VulkanRenderer::EndSingleTimeCommands(commandBuffer);

	TransitionImageLayout(imageAllocation->m_Image, vk::ImageAspectFlagBits::eColor,
						  vk::ImageLayout::eTransferDstOptimal,
						  vk::ImageLayout::eShaderReadOnlyOptimal);

	s_Allocator.m_StagingBuffers.push_back(std::move(stagingBufferAllocation));
	return std::move(imageAllocation);
}

std::unique_ptr<Neon::ImageAllocation>
Neon::Allocator::CreateHdrTextureImage(const std::string& filename)
{
	int texWidth, texHeight, nrComponents;
	float* pixels = stbi_loadf(filename.c_str(), &texWidth, &texHeight, &nrComponents, 0);

	vk::DeviceSize imageSize =
		static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * sizeof(float) * 3;

	std::unique_ptr<BufferAllocation> stagingBufferAllocation =
		CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, VMA_MEMORY_USAGE_GPU_TO_CPU);

	void* mappedData;
	vmaMapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation, &mappedData);
	memcpy(mappedData, pixels, static_cast<size_t>(imageSize));
	vmaUnmapMemory(s_Allocator.m_Allocator, stagingBufferAllocation->m_Allocation);

	stbi_image_free(pixels);

	std::unique_ptr<ImageAllocation> imageAllocation =
		CreateImage(texWidth, texHeight, vk::SampleCountFlagBits::e1, vk::Format::eR32G32B32Sfloat,
					vk::ImageTiling::eLinear,
					vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
					VMA_MEMORY_USAGE_GPU_ONLY);

	TransitionImageLayout(imageAllocation->m_Image, vk::ImageAspectFlagBits::eColor,
						  vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	vk::ImageSubresourceLayers imgSubresourceLayers{vk::ImageAspectFlagBits::eColor, 0, 0, 1};
	vk::BufferImageCopy region{
		0,
		0,
		0,
		imgSubresourceLayers,
		{0, 0, 0},
		vk::Extent3D{static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1}};

	auto commandBuffer = VulkanRenderer::BeginSingleTimeCommands();
	commandBuffer.copyBufferToImage(stagingBufferAllocation->m_Buffer, imageAllocation->m_Image,
									vk::ImageLayout::eTransferDstOptimal, {region});
	VulkanRenderer::EndSingleTimeCommands(commandBuffer);

	TransitionImageLayout(imageAllocation->m_Image, vk::ImageAspectFlagBits::eColor,
						  vk::ImageLayout::eTransferDstOptimal,
						  vk::ImageLayout::eShaderReadOnlyOptimal);

	s_Allocator.m_StagingBuffers.push_back(std::move(stagingBufferAllocation));
	return std::move(imageAllocation);
}

void Neon::Allocator::FreeMemory(VmaAllocation allocation)
{
	vmaFreeMemory(s_Allocator.m_Allocator, allocation);
}

void Neon::Allocator::DestroyImageAllocation(Neon::ImageAllocation& imageAllocation)
{
	s_Allocator.m_LogicalDevice.destroyImage(imageAllocation.m_Image);
	FreeMemory(imageAllocation.m_Allocation);
}

void Neon::Allocator::DestroyBufferAllocation(Neon::BufferAllocation& bufferAllocation)
{
	s_Allocator.m_LogicalDevice.destroyBuffer(bufferAllocation.m_Buffer);
	FreeMemory(bufferAllocation.m_Allocation);
}

void Neon::Allocator::DestroyTextureImage(Neon::TextureImage& textureImage)
{
	s_Allocator.m_LogicalDevice.destroySampler(textureImage.m_Descriptor.sampler);
	s_Allocator.m_LogicalDevice.destroyImageView(textureImage.m_Descriptor.imageView);
}
