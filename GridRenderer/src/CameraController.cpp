#include "CameraController.h"

#include <iostream>

#include <sdl2\SDL.h>
#include <GL/glew.h>

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>

#include "ShaderProgram.h"
#include "Grid.h"

CameraController::CameraController()
{
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);

	m_position = glm::vec3(1.0f, 4.0f, 8.0f);
	m_look = glm::vec3(0.0f, -0.6f, -1.0f);
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);

	m_projection = glm::perspective(45.0f, (float)displayMode.w / (float)displayMode.h, 0.1f, 10000.0f);
	m_view = glm::lookAt(m_position, m_position + m_look, glm::vec3(0.0f, 1.0f, 0.0f));

	m_cameraSpeed = 0.05f;
	m_mouseSensitivity = 0.002f;
	m_scrollSensitivity = 0.1f;

	m_phi = glm::pi<float>() / 2.0f;
	m_theta = 0.0f;

	m_distanceFromOrigin = 3.0f;
}

void CameraController::Update(const Grid& grid)
{
	/*const glm::vec3 pitchAxis = glm::cross(m_look, m_up);
	const glm::quat pitchQuat = glm::angleAxis(m_pitchDelta, pitchAxis);
	const glm::quat yawQuat = glm::angleAxis(m_yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::quat temp = glm::normalize(pitchQuat * yawQuat);

	m_look = glm::rotate(temp, m_look);
	m_up = glm::rotate(temp, m_up);

	m_view = glm::lookAt(m_position, m_position + m_look, glm::vec3(0.0f, 1.0f, 0.0f));

	m_pitchDelta *= 0.75f;
	m_yawDelta *= 0.75f;*/

	m_phi -= m_phiDelta;
	m_theta += m_thetaDelta;

	m_phiDelta *= 0.75f;
	m_thetaDelta *= 0.75f;

	m_phi = glm::clamp(m_phi, 0.001f, glm::pi<float>() - 0.001f);

	m_position.x = glm::sin(m_phi) * glm::cos(m_theta);
	m_position.y = glm::cos(m_phi);
	m_position.z = glm::sin(m_phi) * glm::sin(m_theta);

	m_position *= m_distanceFromOrigin;

	m_view = glm::lookAt(m_position, grid.GetCenter(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void CameraController::Input()
{
	KeyboardInput();
	MouseInput();
}

void CameraController::Scroll(float amount)
{
	m_distanceFromOrigin -= amount * m_scrollSensitivity;
}

void CameraController::KeyboardInput()
{
	/*const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

	const glm::vec3 right = glm::normalize(glm::cross(m_up, m_look));

	if (keyboardState[SDL_SCANCODE_W])
		m_position += m_look * m_cameraSpeed;

	if (keyboardState[SDL_SCANCODE_S])
		m_position -= m_look * m_cameraSpeed;

	if (keyboardState[SDL_SCANCODE_A])
		m_position += right * m_cameraSpeed;

	if (keyboardState[SDL_SCANCODE_D])
		m_position -= right * m_cameraSpeed;*/
}

void CameraController::MouseInput()
{
	int x, y;
	const Uint32 mouseState = SDL_GetMouseState(&x, &y);

	const float deltaX = x - m_mousePosition.x;
	const float deltaY = y - m_mousePosition.y;

	m_mousePosition.x = (float)x;
	m_mousePosition.y = (float)y;

	if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		//m_yawDelta = -deltaX * m_mouseSensitivity;
		//m_pitchDelta = -deltaY * m_mouseSensitivity;
		m_phiDelta = deltaY * m_mouseSensitivity;
		m_thetaDelta = deltaX * m_mouseSensitivity;
	}
}

void CameraController::ApplyUniforms(const ShaderProgram& shaderProgram)
{
	glUniformMatrix4fv(shaderProgram["view"], 1, false, &m_view[0][0]);
	glUniformMatrix4fv(shaderProgram["projection"], 1, false, &m_projection[0][0]);
}