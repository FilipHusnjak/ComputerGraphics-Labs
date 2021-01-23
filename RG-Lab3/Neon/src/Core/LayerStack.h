#pragma once

#include "Layer.h"

#include <vector>

class LayerStack
{
public:
	LayerStack() = default;
	~LayerStack();

	void PushLayer(Neon::Layer* layer);
	void PushOverlay(Neon::Layer* overlay);
	void PopLayer(Neon::Layer* layer);
	void PopOverlay(Neon::Layer* overlay);

	std::vector<Neon::Layer*>::iterator begin()
	{
		return m_Layers.begin();
	}

	std::vector<Neon::Layer*>::iterator end()
	{
		return m_Layers.end();
	}

	std::vector<Neon::Layer*>::reverse_iterator rbegin()
	{
		return m_Layers.rbegin();
	}

	std::vector<Neon::Layer*>::reverse_iterator rend()
	{
		return m_Layers.rend();
	}

	[[nodiscard]] std::vector<Neon::Layer*>::const_iterator begin() const
	{
		return m_Layers.begin();
	}

	[[nodiscard]] std::vector<Neon::Layer*>::const_iterator end() const
	{
		return m_Layers.end();
	}

	[[nodiscard]] std::vector<Neon::Layer*>::const_reverse_iterator rbegin() const
	{
		return m_Layers.rbegin();
	}

	[[nodiscard]] std::vector<Neon::Layer*>::const_reverse_iterator rend() const
	{
		return m_Layers.rend();
	}

private:
	std::vector<Neon::Layer*> m_Layers;
	unsigned int m_LayerInsertIndex = 0;
};