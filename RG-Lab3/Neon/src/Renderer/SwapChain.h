//
// Created by Filip on 10.8.2020..
//

#ifndef NEON_SWAPCHAIN_H
#define NEON_SWAPCHAIN_H

#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "vulkan/vulkan.hpp"
#include <Window/Window.h>

namespace Neon
{
class SwapChain
{
public:
	SwapChain(const SwapChain&) = delete;
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;
	static std::unique_ptr<SwapChain> Create(Window& window, const vk::Instance& instance,
											 const vk::SurfaceKHR& surface,
											 const PhysicalDevice& physicalDevice,
											 const LogicalDevice& logicalDevice);
	[[nodiscard]] vk::Result AcquireNextImage();
	[[nodiscard]] vk::Result Present(const vk::CommandBuffer& commandBuffer);
	[[nodiscard]] const vk::SwapchainKHR& GetHandle() const
	{
		return m_Handle.get();
	}
	[[nodiscard]] size_t GetImageViewSize() const
	{
		return m_SwapChainImageViews.size();
	}
	[[nodiscard]] const vk::ImageView& GetImageView(size_t index) const
	{
		assert(index >= 0 && index < m_SwapChainImageViews.size());
		return m_SwapChainImageViews[index].get();
	}
	[[nodiscard]] vk::Format GetSwapChainImageFormat() const
	{
		return m_SwapChainImageFormat;
	}
	[[nodiscard]] const vk::Extent2D& GetExtent() const
	{
		return m_Extent;
	}
	[[nodiscard]] uint32_t GetImageIndex() const
	{
		return m_ImageIndex;
	}

private:
	SwapChain(Window& window, const vk::Instance& instance, const vk::SurfaceKHR& surface,
			  const PhysicalDevice& physicalDevice, const LogicalDevice& logicalDevice);
	void CreateSwapChainHandle();

private:
	Window& m_Window;
	const vk::Instance& m_Instance;
	const vk::SurfaceKHR& m_Surface;
	const LogicalDevice& m_LogicalDevice;
	const PhysicalDevice& m_PhysicalDevice;
	uint32_t m_FrameIndex = 0;
	uint32_t m_ImageIndex = -1;
	vk::UniqueSwapchainKHR m_Handle;
	vk::Extent2D m_Extent;
	vk::Format m_SwapChainImageFormat = vk::Format::eUndefined;
	std::vector<vk::UniqueImageView> m_SwapChainImageViews;
	std::vector<vk::UniqueSemaphore> m_ImageAcquiredSemaphores;
	std::vector<vk::UniqueSemaphore> m_RenderFinishedSemaphores;
	std::vector<vk::Fence> m_SwapChainFences;
	std::vector<vk::UniqueFence> m_FrameFences;
};
} // namespace Neon

#endif //NEON_SWAPCHAIN_H
