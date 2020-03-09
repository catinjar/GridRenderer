#pragma once

#include <sdl2\SDL.h>
#include <glm\glm.hpp>

class ShaderProgram;

class CameraController
{
public:
	CameraController();

	void Update();
	void Input();

	void ApplyUniforms(const ShaderProgram& shaderProgram);

private:
	void KeyboardInput();
	void MouseInput();

	glm::mat4 m_projection;
	glm::mat4 m_view;

	glm::vec3 m_position;
	glm::vec3 m_look;
	glm::vec3 m_up;
	
	float m_cameraSpeed;
	
	glm::vec2 m_mousePosition;
	float m_mouseSensitivity;
	
	float m_pitchDelta = 0.0f;
	float m_yawDelta = 0.0f;
};