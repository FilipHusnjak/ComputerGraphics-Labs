//
// Created by Filip on 21.8.2020..
//

#include "Components.h"
#include <Renderer/VulkanRenderer.h>

Neon::WaterRenderer::WaterRenderer()
{
	VulkanRenderer::CreateFrameBuffers(
		refractionReflectionResolution, m_RefractionSampledColorTextureImage,
		m_RefractionSampledDepthTextureImage, m_RefractionColorTextureImage,
		m_RefractionDepthTextureImage, m_RefractionFrameBuffers);
	VulkanRenderer::CreateFrameBuffers(
		refractionReflectionResolution, m_ReflectionSampledColorTextureImage,
		m_ReflectionSampledDepthTextureImage, m_ReflectionColorTextureImage,
		m_ReflectionDepthTextureImage, m_ReflectionFrameBuffers);
}
