//
// Created by Filip on 10.8.2020..
//

#include "SwapChain.h"

#define MAX_FRAMES_IN_FLIGHT 2

std::unique_ptr<Neon::SwapChain> Neon::SwapChain::Create(Window& window,
														 const vk::Instance& instance,
														 const vk::SurfaceKHR& surface,
														 const Neon::PhysicalDevice& physicalDevice,
														 const Neon::LogicalDevice& logicalDevice)
{
	auto swapChain = new Neon::SwapChain(window, instance, surface, physicalDevice, logicalDevice);
	return std::unique_ptr<Neon::SwapChain>(swapChain);
}

Neon::SwapChain::SwapChain(Window& window, const vk::Instance& instance,
						   const vk::SurfaceKHR& surface, const PhysicalDevice& physicalDevice,
						   const LogicalDevice& logicalDevice)
	: m_Window(window)
	, m_Instance(instance)
	, m_Surface(surface)
	, m_LogicalDevice(logicalDevice)
	, m_PhysicalDevice(physicalDevice)
{
	CreateSwapChainHandle();
	m_SwapChainFences.resize(m_SwapChainImageViews.size());

	vk::SemaphoreCreateInfo semaphoreInfo{};
	vk::FenceCreateInfo fenceInfo{vk::FenceCreateFlagBits::eSignaled};

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		m_ImageAcquiredSemaphores.push_back(
			logicalDevice.GetHandle().createSemaphoreUnique(semaphoreInfo));
		m_RenderFinishedSemaphores.push_back(
			logicalDevice.GetHandle().createSemaphoreUnique(semaphoreInfo));
		m_FrameFences.push_back(logicalDevice.GetHandle().createFenceUnique(fenceInfo));
	}
}

vk::Result Neon::SwapChain::AcquireNextImage()
{
	m_LogicalDevice.GetHandle().waitForFences(m_FrameFences[m_FrameIndex].get(), VK_TRUE,
											  UINT64_MAX);
	auto result = m_LogicalDevice.GetHandle().acquireNextImageKHR(
		m_Handle.get(), UINT64_MAX, m_ImageAcquiredSemaphores[m_FrameIndex].get(), nullptr);
	if (result.result == vk::Result::eErrorOutOfDateKHR)
	{
		CreateSwapChainHandle();
		return result.result;
	}
	else
	{
		assert(result.result == vk::Result::eSuccess ||
			   result.result == vk::Result::eSuboptimalKHR);
	}
	m_ImageIndex = result.value;
	if (m_SwapChainFences[m_ImageIndex] != vk::Fence())
	{
		m_LogicalDevice.GetHandle().waitForFences(m_SwapChainFences[m_ImageIndex], VK_TRUE,
												  UINT64_MAX);
	}
	m_SwapChainFences[m_ImageIndex] = m_FrameFences[m_FrameIndex].get();
	return result.result;
}

vk::Result Neon::SwapChain::Present(const vk::CommandBuffer& commandBuffer)
{
	vk::Semaphore waitSemaphores[] = {m_ImageAcquiredSemaphores[m_FrameIndex].get()};
	vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	vk::Semaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_FrameIndex].get()};
	vk::SubmitInfo submitInfo{1, waitSemaphores,  waitStages, 1, &commandBuffer,
							  1, signalSemaphores};

	m_LogicalDevice.GetHandle().resetFences({m_FrameFences[m_FrameIndex].get()});

	m_LogicalDevice.GetGraphicsQueue().submit({submitInfo}, m_FrameFences[m_FrameIndex].get());

	vk::PresentInfoKHR presentInfo{1, signalSemaphores, 1, &m_Handle.get(), &m_ImageIndex};

	auto result = m_LogicalDevice.GetPresentQueue().presentKHR(&presentInfo);

	if (m_Window.Resized())
	{
		CreateSwapChainHandle();
		result = vk::Result::eErrorOutOfDateKHR;
	}
	else if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		CreateSwapChainHandle();
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}
	m_Window.ResetResized();
	m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	return result;
}

void Neon::SwapChain::CreateSwapChainHandle()
{
	// FIXME: This sometimes does not work properly, because deviceSurfaceProperties.surfaceCapabilities.currentExtent
	// 	is not updated immediately after extent returned by glfwGetFramebufferSize function is positive
	int testW, testH;
	glfwGetFramebufferSize(m_Window.GetNativeWindow(), &testW, &testH);
	while (testW == 0 || testH == 0)
	{
		glfwGetFramebufferSize(m_Window.GetNativeWindow(), &testW, &testH);
		glfwWaitEvents();
	}

	m_LogicalDevice.GetHandle().waitIdle();
	auto deviceSurfaceProperties = m_PhysicalDevice.GetDeviceSurfaceProperties(m_Surface);
	std::vector<vk::SurfaceFormatKHR> availableSurfaceFormats = deviceSurfaceProperties.formats;
	assert(!availableSurfaceFormats.empty());
	vk::SurfaceFormatKHR surfaceFormat = availableSurfaceFormats[0];
	for (const auto& availableSurfaceFormat : availableSurfaceFormats)
	{
		if (availableSurfaceFormat.format == vk::Format::eB8G8R8A8Unorm &&
			availableSurfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{ surfaceFormat = availableSurfaceFormat; }
	}
	m_SwapChainImageFormat = surfaceFormat.format;

	std::vector<vk::PresentModeKHR> availablePresentModes = deviceSurfaceProperties.presentModes;
	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{ presentMode = availablePresentMode; }
	}

	if (deviceSurfaceProperties.surfaceCapabilities.currentExtent.width != UINT32_MAX)
	{ m_Extent = deviceSurfaceProperties.surfaceCapabilities.currentExtent; }
	else
	{
		int width, height;
		glfwGetFramebufferSize(m_Window.GetNativeWindow(), &width, &height);
		m_Extent = vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
		m_Extent.width = max(
			deviceSurfaceProperties.surfaceCapabilities.minImageExtent.width,
			min(deviceSurfaceProperties.surfaceCapabilities.maxImageExtent.width, m_Extent.width));
		m_Extent.height = max(deviceSurfaceProperties.surfaceCapabilities.minImageExtent.height,
							  min(deviceSurfaceProperties.surfaceCapabilities.maxImageExtent.height,
								  m_Extent.height));
	}

	uint32_t imageCount = deviceSurfaceProperties.surfaceCapabilities.minImageCount + 1;
	if (deviceSurfaceProperties.surfaceCapabilities.maxImageCount > 0 &&
		imageCount > deviceSurfaceProperties.surfaceCapabilities.maxImageCount)
	{ imageCount = deviceSurfaceProperties.surfaceCapabilities.maxImageCount; }
	vk::SwapchainCreateInfoKHR swapChainCreateInfo(
		{}, m_Surface, imageCount, m_SwapChainImageFormat, surfaceFormat.colorSpace, m_Extent, 1,
		vk::ImageUsageFlagBits::eColorAttachment, vk::SharingMode::eExclusive, 0, nullptr,
		deviceSurfaceProperties.surfaceCapabilities.currentTransform,
		vk::CompositeAlphaFlagBitsKHR::eOpaque, presentMode, true, nullptr);

	if (m_PhysicalDevice.GetPresentQueueFamily().m_Index !=
		m_PhysicalDevice.GetGraphicsQueueFamily().m_Index)
	{
		uint32_t queueFamilyIndices[] = {m_PhysicalDevice.GetGraphicsQueueFamily().m_Index,
										 m_PhysicalDevice.GetPresentQueueFamily().m_Index};
		swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapChainCreateInfo.queueFamilyIndexCount = 2;
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}

	m_SwapChainImageViews.clear();
	m_Handle.reset();

	m_Handle = m_LogicalDevice.GetHandle().createSwapchainKHRUnique(swapChainCreateInfo);
	auto swapChainImages = m_LogicalDevice.GetHandle().getSwapchainImagesKHR(m_Handle.get());
	m_SwapChainImageViews.reserve(swapChainImages.size());
	for (auto& swapChainImage : swapChainImages)
	{
		vk::ImageSubresourceRange subResourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		vk::ImageViewCreateInfo imageViewCreateInfo{
			{}, swapChainImage,	 vk::ImageViewType::e2D, m_SwapChainImageFormat,
			{}, subResourceRange};
		m_SwapChainImageViews.push_back(
			m_LogicalDevice.GetHandle().createImageViewUnique(imageViewCreateInfo));
	}
}
