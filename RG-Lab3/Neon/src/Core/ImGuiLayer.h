#pragma once

#include "Core/Layer.h"
#include "Layer.h"

namespace Neon
{
class ImGuiLayer : public Layer
{
public:
	ImGuiLayer() = default;
	~ImGuiLayer() override = default;

	void OnAttach() override;
	void OnDetach() override;

	static void Begin();
	static void End();
};
} // namespace Neon