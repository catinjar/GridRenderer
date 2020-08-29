#pragma once

#include <glm\glm.hpp>

class ShaderProgram;
class Grid;

class CameraController
{
public:
	CameraController();

	void Update(const Grid& grid);
	void Input();
	void Scroll(float amount);

	void ApplyUniforms(const ShaderProgram& shaderProgram);

private:
	void KeyboardInput();
	void MouseInput();

	float m_phi;
	float m_theta;

	float m_phiDelta = 0.0f;
	float m_thetaDelta = 0.0f;

	float m_distanceFromOrigin;

	glm::mat4 m_projection;
	glm::mat4 m_view;

	glm::vec3 m_position;
	glm::vec3 m_look;
	glm::vec3 m_up;
	
	float m_cameraSpeed;
	
	glm::vec2 m_mousePosition = glm::vec2();
	float m_mouseSensitivity;
	float m_scrollSensitivity;
	
	float m_pitchDelta = 0.0f;
	float m_yawDelta = 0.0f;
};