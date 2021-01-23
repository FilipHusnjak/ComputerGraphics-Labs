//
// Created by Filip on 4.8.2020..
//

#ifndef NEON_LOGICALDEVICE_H
#define NEON_LOGICALDEVICE_H

#include "PhysicalDevice.h"
#include <thread>

namespace Neon
{
class LogicalDevice : public std::enable_shared_from_this<LogicalDevice>
{
public:
	LogicalDevice(const LogicalDevice&) = delete;
	LogicalDevice(LogicalDevice&&) = delete;
	LogicalDevice& operator=(const LogicalDevice&) = delete;
	LogicalDevice& operator=(LogicalDevice&&) = delete;

	static std::shared_ptr<LogicalDevice> Create(const PhysicalDevice& physicalDevice);

	std::shared_ptr<LogicalDevice> GetSharedPtr()
	{
		return shared_from_this();
	}
	std::shared_ptr<const LogicalDevice> GetSharedPtr() const
	{
		return shared_from_this();
	}
	[[nodiscard]] const vk::Device& GetHandle() const
	{
		return m_Handle.get();
	}
	[[nodiscard]] vk::Queue GetGraphicsQueue() const
	{
		return m_GraphicsQueue;
	}
	[[nodiscard]] vk::Queue GetPresentQueue() const
	{
		return m_PresentQueue;
	}
	[[nodiscard]] vk::Queue GetComputeQueue() const
	{
		return m_ComputeQueue;
	}
	[[nodiscard]] vk::Queue GetTransferQueue() const
	{
		return m_TransferQueue;
	}

private:
	explicit LogicalDevice(const PhysicalDevice& physicalDevice);

private:
	vk::UniqueDevice m_Handle;
	vk::Queue m_GraphicsQueue;
	vk::Queue m_PresentQueue;
	vk::Queue m_ComputeQueue;
	vk::Queue m_TransferQueue;
};
} // namespace Neon

#endif //NEON_LOGICALDEVICE_H
