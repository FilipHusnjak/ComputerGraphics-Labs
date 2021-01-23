#include "VulkanRenderer.h"

#include "Allocator.h"
#include "Context.h"
#include "RenderPass.h"
#include "Window.h"

#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_vulkan.h>

#define MAX_DESCRIPTOR_SETS_PER_POOL 1024

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

Neon::VulkanRenderer Neon::VulkanRenderer::s_Instance;

Neon::VulkanRenderer::VulkanRenderer() noexcept { }

void Neon::VulkanRenderer::Init(Window* window)
{
	s_Instance.InitRenderer(window);
}

void Neon::VulkanRenderer::Shutdown()
{
	Neon::Context::GetInstance().GetLogicalDevice().GetHandle().waitIdle();
}

void Neon::VulkanRenderer::Begin()
{
	auto result = s_Instance.m_SwapChain->AcquireNextImage();
	if (result == vk::Result::eErrorOutOfDateKHR) { s_Instance.WindowResized(); }
	else
	{
		assert(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR);
		vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
		s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get().begin(beginInfo);
	}
}

void Neon::VulkanRenderer::End()
{
	auto& commandBuffer =
		s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get();
	commandBuffer.end();
	auto result = s_Instance.m_SwapChain->Present(commandBuffer);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{ s_Instance.WindowResized(); }
	else
	{
		assert(result == vk::Result::eSuccess);
	}
}

void Neon::VulkanRenderer::BeginScene(const std::vector<vk::UniqueFramebuffer>& frameBuffers,
									  const vk::Extent2D& extent, const glm::vec4& clearColor,
									  const Neon::PerspectiveCamera& camera,
									  const glm::vec4& clippingPlane, bool pointLight,
									  float lightIntensity, glm::vec3 lightDirection,
									  const glm::vec3& lightPosition)
{
	s_Instance.m_PushConstant.cameraPos = camera.GetPosition();
	s_Instance.m_PushConstant.view = camera.GetViewMatrix();
	s_Instance.m_PushConstant.projection = camera.GetProjectionMatrix();
	s_Instance.m_PushConstant.clippingPlane = clippingPlane;
	s_Instance.m_PushConstant.pointLight = pointLight;
	s_Instance.m_PushConstant.lightIntensity = lightIntensity;
	s_Instance.m_PushConstant.lightDirection = lightDirection;
	s_Instance.m_PushConstant.lightPosition = lightPosition;

	auto& commandBuffer =
		s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get();
	std::array<vk::ClearValue, 2> clearValues = {};
	memcpy(&clearValues[0].color.float32, &clearColor, sizeof(clearValues[0].color.float32));
	clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};
	vk::RenderPassBeginInfo renderPassInfo{
		s_Instance.m_OffscreenRenderPass.get(),
		frameBuffers[s_Instance.m_SwapChain->GetImageIndex()].get(),
		{{0, 0}, extent},
		static_cast<uint32_t>(clearValues.size()),
		clearValues.data()};
	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
}

void Neon::VulkanRenderer::EndScene()
{
	s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get().endRenderPass();
}

void Neon::VulkanRenderer::DrawImGui()
{
	auto& commandBuffer =
		s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get();

	vk::RenderPassBeginInfo renderPassInfo{
		s_Instance.m_ImGuiRenderPass.get(),
		s_Instance.m_ImGuiFrameBuffers[s_Instance.m_SwapChain->GetImageIndex()].get(),
		{{0, 0}, s_Instance.m_SwapChain->GetExtent()}};
	commandBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	ImGui_ImplVulkan_RenderDrawData(
		ImGui::GetDrawData(),
		s_Instance.m_CommandBuffers[s_Instance.m_SwapChain->GetImageIndex()].get());
	commandBuffer.endRenderPass();
}

vk::CommandBuffer Neon::VulkanRenderer::BeginSingleTimeCommands()
{
	auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	vk::CommandBufferAllocateInfo allocInfo{s_Instance.m_CommandPool.get(),
											vk::CommandBufferLevel::ePrimary, 1};
	vk::CommandBuffer commandBuffer = logicalDevice.allocateCommandBuffers(allocInfo)[0];
	vk::CommandBufferBeginInfo beginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit};
	commandBuffer.begin(beginInfo);
	return commandBuffer;
}

void Neon::VulkanRenderer::EndSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	const auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice();
	commandBuffer.end();
	vk::SubmitInfo submitInfo{0, nullptr, nullptr, 1, &commandBuffer};
	logicalDevice.GetGraphicsQueue().submit(submitInfo, nullptr);
	logicalDevice.GetGraphicsQueue().waitIdle();
	logicalDevice.GetHandle().freeCommandBuffers(s_Instance.m_CommandPool.get(), commandBuffer);
}

vk::ImageView Neon::VulkanRenderer::CreateImageView(vk::Image image, vk::Format format,
													const vk::ImageAspectFlags& aspectFlags)
{
	auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	vk::ImageSubresourceRange subResourceRange(aspectFlags, 0, 1, 0, 1);
	vk::ImageViewCreateInfo imageViewCreateInfo{{},		image, vk::ImageViewType::e2D,
												format, {},	   subResourceRange};
	return logicalDevice.createImageView(imageViewCreateInfo);
}

vk::UniqueImageView
Neon::VulkanRenderer::CreateImageViewUnique(vk::Image image, vk::Format format,
											const vk::ImageAspectFlags& aspectFlags)
{
	auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	vk::ImageSubresourceRange subResourceRange(aspectFlags, 0, 1, 0, 1);
	vk::ImageViewCreateInfo imageViewCreateInfo{{},		image, vk::ImageViewType::e2D,
												format, {},	   subResourceRange};
	return logicalDevice.createImageViewUnique(imageViewCreateInfo);
}

vk::Sampler Neon::VulkanRenderer::CreateSampler(const vk::SamplerCreateInfo& createInfo)
{
	return Neon::Context::GetInstance().GetLogicalDevice().GetHandle().createSampler(createInfo);
}

void Neon::VulkanRenderer::InitRenderer(Window* window)
{
	//TODO: swap chain image size should not effect number of descriptor sets and uniform buffers
	Neon::Context::GetInstance().Init();
	Neon::Context::GetInstance().CreateSurface(window);
	Neon::Context::GetInstance().CreateDevice({vk::QueueFlagBits::eGraphics});
	Neon::Context::GetInstance().InitAllocator();

	const auto& physicalDevice = Neon::Context::GetInstance().GetPhysicalDevice();
	const auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice();
	m_SwapChain =
		SwapChain::Create(*window, Context::GetInstance().GetVkInstance(),
						  Context::GetInstance().GetSurface(), physicalDevice, logicalDevice);

	m_OffscreenRenderPass = vk::UniqueRenderPass(
		Neon::CreateRenderPass(logicalDevice.GetHandle(), vk::Format::eR32G32B32A32Sfloat,
							   s_MsaaSamples, true, vk::ImageLayout::eGeneral,
							   vk::ImageLayout::eGeneral, vk::Format::eD32Sfloat, true,
							   vk::ImageLayout::eDepthStencilReadOnlyOptimal,
							   vk::ImageLayout::eDepthStencilReadOnlyOptimal, true),
		logicalDevice.GetHandle());

	CreateCommandPool();
	IntegrateImGui();
	CreateOffscreenRenderer();
	CreateImGuiRenderer();
	CreateCommandBuffers();

	///////////////////////////
	std::vector<vk::DescriptorPoolSize> sizes;
	sizes.emplace_back(vk::DescriptorType::eStorageBuffer,
					   1 * MAX_SWAP_CHAIN_IMAGES * MAX_DESCRIPTOR_SETS_PER_POOL);
	sizes.emplace_back(vk::DescriptorType::eCombinedImageSampler,
					   10 * MAX_SWAP_CHAIN_IMAGES * MAX_DESCRIPTOR_SETS_PER_POOL);
	m_DescriptorPools.push_back(DescriptorPool::Create(
		logicalDevice.GetHandle(), sizes, MAX_SWAP_CHAIN_IMAGES * MAX_DESCRIPTOR_SETS_PER_POOL));
	////////////////////////

	Neon::Context::GetInstance().GetLogicalDevice().GetHandle().waitIdle();
}

void Neon::VulkanRenderer::WindowResized()
{
	const auto& device = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	device.waitIdle();
	CreateOffscreenRenderer();
	CreateImGuiRenderer();
	device.waitIdle();
}

void Neon::VulkanRenderer::IntegrateImGui()
{
	const auto& logicalDevice = Neon::Context::GetInstance().GetLogicalDevice();
	const auto& physicalDevice = Neon::Context::GetInstance().GetPhysicalDevice();

	std::vector<vk::DescriptorPoolSize> poolSizes = {
		{vk::DescriptorType::eSampler, 1000},
		{vk::DescriptorType::eCombinedImageSampler, 1000},
		{vk::DescriptorType::eSampledImage, 1000},
		{vk::DescriptorType::eStorageImage, 1000},
		{vk::DescriptorType::eUniformTexelBuffer, 1000},
		{vk::DescriptorType::eStorageTexelBuffer, 1000},
		{vk::DescriptorType::eUniformBuffer, 1000},
		{vk::DescriptorType::eStorageBuffer, 1000},
		{vk::DescriptorType::eUniformBufferDynamic, 1000},
		{vk::DescriptorType::eStorageBufferDynamic, 1000},
		{vk::DescriptorType::eInputAttachment, 1000}};
	m_ImGuiDescriptorPool = DescriptorPool::Create(logicalDevice.GetHandle(), poolSizes,
												   1000 * static_cast<uint32_t>(poolSizes.size()));

	m_ImGuiRenderPass = vk::UniqueRenderPass(
		Neon::CreateRenderPass(logicalDevice.GetHandle(), m_SwapChain->GetSwapChainImageFormat(),
							   vk::SampleCountFlagBits::e1, false, vk::ImageLayout::eUndefined,
							   vk::ImageLayout::ePresentSrcKHR),
		logicalDevice.GetHandle());

	ImGui_ImplVulkan_InitInfo imguiInfo = {};
	imguiInfo.Instance = Neon::Context::GetInstance().GetVkInstance();
	imguiInfo.PhysicalDevice = physicalDevice.GetHandle();
	imguiInfo.Device = logicalDevice.GetHandle();
	imguiInfo.QueueFamily = physicalDevice.GetGraphicsQueueFamily().m_Index;
	imguiInfo.Queue = logicalDevice.GetGraphicsQueue();
	imguiInfo.PipelineCache = nullptr;
	imguiInfo.DescriptorPool = m_ImGuiDescriptorPool->GetHandle();
	imguiInfo.Allocator = nullptr;
	imguiInfo.MinImageCount = (uint32_t)m_SwapChain->GetImageViewSize();
	imguiInfo.ImageCount = (uint32_t)m_SwapChain->GetImageViewSize();
	imguiInfo.CheckVkResultFn = nullptr;
	ImGui_ImplVulkan_Init(&imguiInfo, m_ImGuiRenderPass.get());

	vk::CommandBuffer commandBuffer = BeginSingleTimeCommands();
	ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
	EndSingleTimeCommands(commandBuffer);

	m_ImGuiOffscreenTextureDescSet = ImGui_ImplVulkan_CreateTexture();
}

void Neon::VulkanRenderer::CreateOffscreenRenderer()
{
	const auto& extent = s_Instance.m_SwapChain->GetExtent();
	CreateFrameBuffers(extent, m_SampledOffscreenColorTextureImage,
					   m_SampledOffscreenDepthTextureImage, m_OffscreenColorTextureImage,
					   m_OffscreenDepthTextureImage, m_OffscreenFrameBuffers);
	ImGui_ImplVulkan_UpdateTexture(
		m_ImGuiOffscreenTextureDescSet, m_OffscreenColorTextureImage.m_Descriptor.sampler,
		m_OffscreenColorTextureImage.m_Descriptor.imageView,
		(VkImageLayout)m_OffscreenColorTextureImage.m_Descriptor.imageLayout);
}

void Neon::VulkanRenderer::CreateImGuiRenderer()
{
	const auto& device = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	m_ImGuiFrameBuffers.clear();
	m_ImGuiFrameBuffers.reserve(m_SwapChain->GetImageViewSize());
	const auto& extent = s_Instance.m_SwapChain->GetExtent();
	for (size_t i = 0; i < m_SwapChain->GetImageViewSize(); i++)
	{
		std::array<vk::ImageView, 1> attachments = {m_SwapChain->GetImageView(i)};
		vk::FramebufferCreateInfo framebufferInfo{{},
												  m_ImGuiRenderPass.get(),
												  static_cast<uint32_t>(attachments.size()),
												  attachments.data(),
												  extent.width,
												  extent.height,
												  1};
		m_ImGuiFrameBuffers.push_back(device.createFramebufferUnique(framebufferInfo));
	}
}

void Neon::VulkanRenderer::CreateCommandPool()
{
	const auto& device = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	const auto& physicalDevice = Neon::Context::GetInstance().GetPhysicalDevice();
	vk::CommandPoolCreateInfo poolInfo{vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
									   physicalDevice.GetGraphicsQueueFamily().m_Index};
	m_CommandPool = device.createCommandPoolUnique(poolInfo);
}

void Neon::VulkanRenderer::CreateCommandBuffers()
{
	const auto& device = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();
	m_CommandBuffers.resize(m_SwapChain->GetImageViewSize());
	vk::CommandBufferAllocateInfo allocInfo{m_CommandPool.get(), vk::CommandBufferLevel::ePrimary,
											static_cast<uint32_t>(m_CommandBuffers.size())};
	m_CommandBuffers = device.allocateCommandBuffersUnique(allocInfo);
}

void Neon::VulkanRenderer::CreateFrameBuffers(vk::Extent2D extent,
											  Neon::TextureImage& sampledColorTextureImage,
											  Neon::TextureImage& sampledDepthTextureImage,
											  Neon::TextureImage& colorTextureImage,
											  Neon::TextureImage& depthTextureImage,
											  std::vector<vk::UniqueFramebuffer>& frameBuffers)
{
	const auto& device = Neon::Context::GetInstance().GetLogicalDevice().GetHandle();

	sampledColorTextureImage.m_TextureAllocation = Allocator::CreateImage(
		extent.width, extent.height, VulkanRenderer::GetMsaaSamples(),
		vk::Format::eR32G32B32A32Sfloat, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransientAttachment,
		VMA_MEMORY_USAGE_GPU_ONLY);
	Neon::Allocator::TransitionImageLayout(sampledColorTextureImage.m_TextureAllocation->m_Image,
										   vk::ImageAspectFlagBits::eColor,
										   vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	sampledColorTextureImage.m_Descriptor.imageView = VulkanRenderer::CreateImageView(
		sampledColorTextureImage.m_TextureAllocation->m_Image, vk::Format::eR32G32B32A32Sfloat,
		vk::ImageAspectFlagBits::eColor);

	sampledDepthTextureImage.m_TextureAllocation =
		Neon::Allocator::CreateImage(extent.width, extent.height, VulkanRenderer::GetMsaaSamples(),
									 vk::Format::eD32Sfloat, vk::ImageTiling::eOptimal,
									 vk::ImageUsageFlagBits::eDepthStencilAttachment |
										 vk::ImageUsageFlagBits::eTransientAttachment,
									 VMA_MEMORY_USAGE_GPU_ONLY);
	Neon::Allocator::TransitionImageLayout(
		sampledDepthTextureImage.m_TextureAllocation->m_Image, vk::ImageAspectFlagBits::eDepth,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilReadOnlyOptimal);
	sampledDepthTextureImage.m_Descriptor.imageView =
		VulkanRenderer::CreateImageView(sampledDepthTextureImage.m_TextureAllocation->m_Image,
										vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth);

	colorTextureImage.m_TextureAllocation = Neon::Allocator::CreateImage(
		extent.width, extent.height, vk::SampleCountFlagBits::e1, vk::Format::eR32G32B32A32Sfloat,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
		VMA_MEMORY_USAGE_GPU_ONLY);
	Neon::Allocator::TransitionImageLayout(colorTextureImage.m_TextureAllocation->m_Image,
										   vk::ImageAspectFlagBits::eColor,
										   vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
	colorTextureImage.m_Descriptor.imageView = VulkanRenderer::CreateImageView(
		colorTextureImage.m_TextureAllocation->m_Image, vk::Format::eR32G32B32A32Sfloat,
		vk::ImageAspectFlagBits::eColor);
	colorTextureImage.m_Descriptor.sampler = VulkanRenderer::CreateSampler(vk::SamplerCreateInfo());
	colorTextureImage.m_Descriptor.imageLayout = vk::ImageLayout::eGeneral;

	depthTextureImage.m_TextureAllocation = Neon::Allocator::CreateImage(
		extent.width, extent.height, vk::SampleCountFlagBits::e1, vk::Format::eD32Sfloat,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
		VMA_MEMORY_USAGE_GPU_ONLY);
	Neon::Allocator::TransitionImageLayout(
		depthTextureImage.m_TextureAllocation->m_Image, vk::ImageAspectFlagBits::eDepth,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilReadOnlyOptimal);
	depthTextureImage.m_Descriptor.imageView =
		VulkanRenderer::CreateImageView(depthTextureImage.m_TextureAllocation->m_Image,
										vk::Format::eD32Sfloat, vk::ImageAspectFlagBits::eDepth);
	depthTextureImage.m_Descriptor.sampler = VulkanRenderer::CreateSampler(vk::SamplerCreateInfo());
	depthTextureImage.m_Descriptor.imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;

	frameBuffers.clear();
	frameBuffers.reserve(MAX_SWAP_CHAIN_IMAGES);
	for (size_t i = 0; i < MAX_SWAP_CHAIN_IMAGES; i++)
	{
		std::vector<vk::ImageView> attachments = {sampledColorTextureImage.m_Descriptor.imageView,
												  sampledDepthTextureImage.m_Descriptor.imageView,
												  colorTextureImage.m_Descriptor.imageView,
												  depthTextureImage.m_Descriptor.imageView};

		vk::FramebufferCreateInfo framebufferInfo;
		framebufferInfo.setRenderPass(VulkanRenderer::GetOffscreenRenderPass());
		framebufferInfo.setAttachmentCount(static_cast<uint32_t>(attachments.size()));
		framebufferInfo.setPAttachments(attachments.data());
		framebufferInfo.setWidth(extent.width);
		framebufferInfo.setHeight(extent.height);
		framebufferInfo.setLayers(1);

		frameBuffers.push_back(device.createFramebufferUnique(framebufferInfo));
	}
}
