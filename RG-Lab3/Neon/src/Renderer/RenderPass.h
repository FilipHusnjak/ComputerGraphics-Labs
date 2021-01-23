#include <vulkan/vulkan.hpp>

namespace Neon
{
vk::RenderPass CreateRenderPass(const vk::Device& device, vk::Format colorAttachmentFormat,
								vk::SampleCountFlagBits samples, bool clearColor,
								vk::ImageLayout colorInitialLayout,
								vk::ImageLayout colorFinalLayout,
								vk::Format depthAttachmentFormat = vk::Format::eUndefined, bool clearDepth = true,
								vk::ImageLayout depthInitialLayout = vk::ImageLayout::eUndefined,
								vk::ImageLayout depthFinalLayout = vk::ImageLayout::eUndefined, bool resolve = false);
} // namespace Neon