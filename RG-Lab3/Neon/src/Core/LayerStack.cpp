#include "LayerStack.h"
#include "Layer.h"

LayerStack::~LayerStack()
{
	for (Neon::Layer* layer : m_Layers)
	{
		layer->OnDetach();
		delete layer;
	}
}

void LayerStack::PushLayer(Neon::Layer* layer)
{
	m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
	m_LayerInsertIndex++;
}

void LayerStack::PushOverlay(Neon::Layer* overlay)
{
	m_Layers.emplace_back(overlay);
}

void LayerStack::PopLayer(Neon::Layer* layer)
{
	auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
	if (it != m_Layers.begin() + m_LayerInsertIndex)
	{
		layer->OnDetach();
		m_Layers.erase(it);
		m_LayerInsertIndex--;
	}
}

void LayerStack::PopOverlay(Neon::Layer* overlay)
{
	auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
	if (it != m_Layers.end())
	{
		overlay->OnDetach();
		m_Layers.erase(it);
	}
}