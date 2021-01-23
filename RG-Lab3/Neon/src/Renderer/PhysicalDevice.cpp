//
// Created by Filip on 4.8.2020..
//

#include "Context.h"

std::unique_ptr<Neon::PhysicalDevice>
Neon::PhysicalDevice::Create(const vk::SurfaceKHR& surface,
							 const std::vector<const char*>& requiredExtensions,
							 const std::vector<vk::QueueFlagBits>& queueFlags)
{
	auto physicalDevice = new PhysicalDevice(surface, requiredExtensions, queueFlags);
	return std::unique_ptr<PhysicalDevice>(physicalDevice);
}

Neon::PhysicalDevice::PhysicalDevice(const vk::SurfaceKHR& surface,
									 const std::vector<const char*>& requiredExtensions,
									 const std::vector<vk::QueueFlagBits>& queueFlags)
{
	std::multimap<int, VkPhysicalDevice> candidates;
	std::vector<vk::PhysicalDevice> devices =
		Context::GetInstance().GetVkInstance().enumeratePhysicalDevices();
	for (auto& device : devices)
	{
		if (int score = IsDeviceSuitable(device, surface, requiredExtensions, queueFlags) > -1)
		{ candidates.insert(std::make_pair(score, device)); }
	}
	if (candidates.empty()) { throw std::runtime_error("Suitable physical device not found"); }
	m_Handle = candidates.rbegin()->second;
	m_Properties = m_Handle.getProperties();
	m_Features = m_Handle.getFeatures();
	m_MemoryProperties = m_Handle.getMemoryProperties();
	m_SupportedExtensions = m_Handle.enumerateDeviceExtensionProperties();
	m_RequiredExtensions = requiredExtensions;
	FindQueueFamilies(surface);
}

int Neon::PhysicalDevice::IsDeviceSuitable(const vk::PhysicalDevice& physicalDevice,
										   const vk::SurfaceKHR& surface,
										   const std::vector<const char*>& requiredExtensions,
										   const std::vector<vk::QueueFlagBits>& queueFlags)
{
	if (!CheckExtensionSupport(physicalDevice, requiredExtensions)) return -1;
	Neon::DeviceSurfaceProperties surfaceProperties =
		QueryDeviceSurfaceProperties(physicalDevice, surface);
	if (surfaceProperties.formats.empty() || surfaceProperties.presentModes.empty()) return -1;
	vk::PhysicalDeviceFeatures supportedFeatures = physicalDevice.getFeatures();
	if (!supportedFeatures.samplerAnisotropy) return -1;
	if (!CheckQueueFamilySupport(physicalDevice, surface, queueFlags)) return -1;
	vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
	return properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ? 10 : 1;
}

bool Neon::PhysicalDevice::CheckExtensionSupport(const vk::PhysicalDevice& physicalDevice,
												 std::vector<const char*> requiredExtensions)
{
	auto supportedExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	std::set<std::string> requiredExtensionsSet(requiredExtensions.begin(),
												requiredExtensions.end());
	for (const auto& supportedExtension : supportedExtensions)
	{
		requiredExtensionsSet.erase(supportedExtension.extensionName);
	}
	return requiredExtensionsSet.empty();
}

Neon::DeviceSurfaceProperties
Neon::PhysicalDevice::QueryDeviceSurfaceProperties(const vk::PhysicalDevice& physicalDevice,
												   const vk::SurfaceKHR& surface)
{
	Neon::DeviceSurfaceProperties surfaceProperties{};
	surfaceProperties.surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	surfaceProperties.formats = physicalDevice.getSurfaceFormatsKHR(surface);
	surfaceProperties.presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
	return surfaceProperties;
}

bool Neon::PhysicalDevice::CheckQueueFamilySupport(const vk::PhysicalDevice& physicalDevice,
												   const vk::SurfaceKHR& surface,
												   const std::vector<vk::QueueFlagBits>& queueFlags)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
		physicalDevice.getQueueFamilyProperties();
	std::vector<bool> flagsSupported(queueFlags.size(), false);

	for (const auto& queueFamilyProperty : queueFamilyProperties)
	{
		for (int i = 0; i < queueFlags.size(); i++)
		{
			flagsSupported[i] =
				flagsSupported[i] || (queueFlags[i] & queueFamilyProperty.queueFlags);
		}
	}
	for (bool supp : flagsSupported)
	{
		if (!supp) return false;
	}
	return true;
}

void Neon::PhysicalDevice::FindQueueFamilies(const vk::SurfaceKHR& surface)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
		m_Handle.getQueueFamilyProperties();
	uint32_t index = 0;

	for (const auto& queueFamilyProperty : queueFamilyProperties)
	{
		Neon::QueueFamily queueFamily{index};
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics)
		{ m_GraphicsQueueFamily = queueFamily; }
		if (m_Handle.getSurfaceSupportKHR(static_cast<uint32_t>(index), surface))
		{ m_PresentQueueFamily = queueFamily; }
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute)
		{ m_ComputeQueueFamily = queueFamily; }
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer)
		{ m_TransferQueueFamily = queueFamily; }
		if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eSparseBinding)
		{ m_SparseBindingQueueFamily = queueFamily; }
		index++;
	}
}
