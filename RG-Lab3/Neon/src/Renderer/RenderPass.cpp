#include "RenderPass.h"
#include "Context.h"

vk::RenderPass Neon::CreateRenderPass(const vk::Device& device, vk::Format colorAttachmentFormat,
									  vk::SampleCountFlagBits samples, bool clearColor,
									  vk::ImageLayout colorInitialLayout,
									  vk::ImageLayout colorFinalLayout,
									  vk::Format depthAttachmentFormat, bool clearDepth,
									  vk::ImageLayout depthInitialLayout,
									  vk::ImageLayout depthFinalLayout, bool resolve)
{
	std::vector<vk::AttachmentDescription2KHR> attachments;
	vk::AttachmentDescription2KHR colorAttachment{{},
												  colorAttachmentFormat,
												  samples,
												  clearColor ? vk::AttachmentLoadOp::eClear
															 : vk::AttachmentLoadOp::eDontCare,
												  vk::AttachmentStoreOp::eStore,
												  vk::AttachmentLoadOp::eDontCare,
												  vk::AttachmentStoreOp::eDontCare,
												  colorInitialLayout,
												  colorFinalLayout};
	attachments.push_back(colorAttachment);
	vk::AttachmentReference2KHR colorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal};

	vk::SubpassDescription2KHR subpassDescription{
		{}, vk::PipelineBindPoint::eGraphics, {}, 0, nullptr, 1, &colorAttachmentRef};

	bool hasDepth = (depthAttachmentFormat != vk::Format::eUndefined);
	vk::AttachmentDescription2KHR depthAttachment;
	vk::AttachmentReference2KHR depthAttachmentRef;
	if (hasDepth)
	{
		depthAttachment = {{},
						   depthAttachmentFormat,
						   samples,
						   clearDepth ? vk::AttachmentLoadOp::eClear : vk::AttachmentLoadOp::eLoad,
						   vk::AttachmentStoreOp::eStore,
						   vk::AttachmentLoadOp::eDontCare,
						   vk::AttachmentStoreOp::eDontCare,
						   depthInitialLayout,
						   depthFinalLayout};
		attachments.push_back(depthAttachment);
		depthAttachmentRef = {1, vk::ImageLayout::eDepthStencilAttachmentOptimal};
		subpassDescription.setPDepthStencilAttachment(&depthAttachmentRef);
	}

	vk::SubpassDependency2KHR subpassDependency{VK_SUBPASS_EXTERNAL,
												0,
												vk::PipelineStageFlagBits::eColorAttachmentOutput,
												vk::PipelineStageFlagBits::eColorAttachmentOutput,
												vk::AccessFlagBits{},
												vk::AccessFlagBits::eColorAttachmentRead |
													vk::AccessFlagBits::eColorAttachmentWrite};

	vk::AttachmentDescription2KHR colorAttachmentResolve;
	vk::AttachmentReference2KHR colorAttachmentResolveReference;
	vk::AttachmentDescription2KHR depthAttachmentResolve;
	vk::AttachmentReference2KHR depthAttachmentResolveReference;
	vk::SubpassDescriptionDepthStencilResolveKHR depthStencilResolve = {
		vk::ResolveModeFlagBitsKHR::eSampleZero, vk::ResolveModeFlagBitsKHR::eSampleZero};
	if (resolve)
	{
		colorAttachmentResolve = {{},
								  colorAttachmentFormat,
								  vk::SampleCountFlagBits::e1,
								  vk::AttachmentLoadOp::eDontCare,
								  vk::AttachmentStoreOp::eStore,
								  vk::AttachmentLoadOp::eDontCare,
								  vk::AttachmentStoreOp::eDontCare,
								  colorInitialLayout,
								  colorFinalLayout};
		colorAttachmentResolveReference = {2, vk::ImageLayout::eColorAttachmentOptimal};
		subpassDescription.setPResolveAttachments(&colorAttachmentResolveReference);
		depthAttachmentResolve = {{},
								  depthAttachmentFormat,
								  vk::SampleCountFlagBits::e1,
								  vk::AttachmentLoadOp::eDontCare,
								  vk::AttachmentStoreOp::eStore,
								  vk::AttachmentLoadOp::eDontCare,
								  vk::AttachmentStoreOp::eDontCare,
								  depthInitialLayout,
								  depthFinalLayout};
		depthAttachmentResolveReference = {3, vk::ImageLayout::eDepthStencilAttachmentOptimal};
		depthStencilResolve.setPDepthStencilResolveAttachment(&depthAttachmentResolveReference);
		subpassDescription.setPNext(&depthStencilResolve);

		attachments.push_back(colorAttachmentResolve);
		attachments.push_back(depthAttachmentResolve);
	}
	vk::RenderPassCreateInfo2KHR renderPassInfo = {{},
												   static_cast<uint32_t>(attachments.size()),
												   attachments.data(),
												   1,
												   &subpassDescription,
												   1,
												   &subpassDependency};
	vk::DispatchLoaderDynamic dldi(Context::GetInstance().GetVkInstance(),
								   Context::GetInstance().GetLogicalDevice().GetHandle());
	dldi.vkCreateRenderPass2KHR = (PFN_vkCreateRenderPass2KHR)vkGetDeviceProcAddr(
		Context::GetInstance().GetLogicalDevice().GetHandle(), "vkCreateRenderPass2KHR");
	return device.createRenderPass2KHR(renderPassInfo, nullptr, dldi);
}
