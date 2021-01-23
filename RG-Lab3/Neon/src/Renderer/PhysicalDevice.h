//
// Created by Filip on 4.8.2020..
//

#ifndef NEON_PHYSICALDEVICE_H
#define NEON_PHYSICALDEVICE_H

#include <vector>
#include <vulkan\vulkan.hpp>

namespace Neon
{
struct DeviceSurfaceProperties
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};
struct QueueFamily
{
	uint32_t m_Index;
	QueueFamily()
		: m_Index(-1){};
	explicit QueueFamily(uint32_t index)
		: m_Index(index){};
};
class PhysicalDevice
{
public:
	PhysicalDevice(const PhysicalDevice&) = delete;
	PhysicalDevice(PhysicalDevice&&) = delete;
	PhysicalDevice& operator=(const PhysicalDevice&) = delete;
	PhysicalDevice& operator=(PhysicalDevice&&) = delete;

	static std::unique_ptr<PhysicalDevice>
	Create(const vk::SurfaceKHR& surface, const std::vector<const char*>& requiredExtensions,
		   const std::vector<vk::QueueFlagBits>& queueFlags);

	[[nodiscard]] const vk::PhysicalDevice& GetHandle() const
	{
		return m_Handle;
	}
	[[nodiscard]] const vk::PhysicalDeviceProperties& GetProperties() const
	{
		return m_Properties;
	}
	[[nodiscard]] DeviceSurfaceProperties
	GetDeviceSurfaceProperties(const vk::SurfaceKHR& surface) const
	{
		return QueryDeviceSurfaceProperties(m_Handle, surface);
	}
	[[nodiscard]] QueueFamily GetGraphicsQueueFamily() const
	{
		return m_GraphicsQueueFamily;
	}
	[[nodiscard]] QueueFamily GetPresentQueueFamily() const
	{
		return m_PresentQueueFamily;
	}
	[[nodiscard]] QueueFamily GetComputeQueueFamily() const
	{
		return m_ComputeQueueFamily;
	}
	[[nodiscard]] QueueFamily GetTransferQueueFamily() const
	{
		return m_TransferQueueFamily;
	}
	[[nodiscard]] QueueFamily GetSparseBindingQueueFamily() const
	{
		return m_SparseBindingQueueFamily;
	}
	[[nodiscard]] const std::vector<const char*>& GetRequiredExtensions() const
	{
		return m_RequiredExtensions;
	}

private:
	PhysicalDevice(const vk::SurfaceKHR& surface,
				   const std::vector<const char*>& requiredExtensions,
				   const std::vector<vk::QueueFlagBits>& queueFlags);
	[[nodiscard]] static int IsDeviceSuitable(const vk::PhysicalDevice& physicalDevice,
											  const vk::SurfaceKHR& surface,
											  const std::vector<const char*>& requiredExtensions,
											  const std::vector<vk::QueueFlagBits>& queueFlags);
	[[nodiscard]] static DeviceSurfaceProperties
	QueryDeviceSurfaceProperties(const vk::PhysicalDevice& physicalDevice,
								 const vk::SurfaceKHR& surface);
	[[nodiscard]] static bool CheckExtensionSupport(const vk::PhysicalDevice& physicalDevice,
													std::vector<const char*> requiredExtensions);
	[[nodiscard]] static bool
	CheckQueueFamilySupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface,
							const std::vector<vk::QueueFlagBits>& queueFlags);

	void FindQueueFamilies(const vk::SurfaceKHR& surface);

private:
	vk::PhysicalDevice m_Handle;
	vk::PhysicalDeviceProperties m_Properties;
	vk::PhysicalDeviceFeatures m_Features;
	vk::PhysicalDeviceMemoryProperties m_MemoryProperties;
	std::vector<vk::ExtensionProperties> m_SupportedExtensions;
	std::vector<const char*> m_RequiredExtensions;
	QueueFamily m_GraphicsQueueFamily;
	QueueFamily m_PresentQueueFamily;
	QueueFamily m_ComputeQueueFamily;
	QueueFamily m_TransferQueueFamily;
	QueueFamily m_SparseBindingQueueFamily;
};
} // namespace Neon

#endif //NEON_PHYSICALDEVICE_H
