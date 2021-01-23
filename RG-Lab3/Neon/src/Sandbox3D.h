#pragma once

#include "Core/Layer.h"

#include "Renderer/PerspectiveCameraController.h"

#include "Event.h"
#include "Layer.h"
#include "PerspectiveCameraController.h"
#include "Scene/Scene.h"

class Sandbox3D : public Neon::Layer
{
public:
	Sandbox3D();
	~Sandbox3D() override = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float ts) override;
	void OnImGuiRender() override;
	void OnEvent(Neon::Event& e) override;

private:
	Neon::PerspectiveCameraController m_CameraController;

	std::shared_ptr<Neon::Scene> m_ActiveScene;

	std::queue<float> m_Times;
	float m_TimePassed = 0.0f;
	int m_FrameCount = 0;

	glm::vec4 clearColor{1, 1, 1, 1.00f};
	bool pointLight = false;
	glm::vec3 lightDirection = {0, -1, 0};
	float lightIntensity = 0.6f;
	glm::vec3 lightPosition = {-0.896, 15.821, -12.353};
};