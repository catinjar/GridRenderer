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

	position = glm::vec3(1.0f, 4.0f, 8.0f);
	look = glm::vec3(0.0f, -0.6f, -1.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);

	projection = glm::perspective(45.0f, (float)displayMode.w / (float)displayMode.h, 0.1f, 10000.0f);
	view = glm::lookAt(position, position + look, glm::vec3(0.0f, 1.0f, 0.0f));

	cameraSpeed = 0.05f;
	mouseSensitivity = 0.002f;
	scrollSensitivity = 0.1f;

	phi = glm::pi<float>() / 2.0f;
	theta = 0.0f;

	distanceFromOrigin = 3.0f;
}

void CameraController::Update(const Grid& grid)
{
	phi -= phiDelta;
	theta += thetaDelta;

	phiDelta *= 0.75f;
	thetaDelta *= 0.75f;

	phi = glm::clamp(phi, 0.001f, glm::pi<float>() - 0.001f);

	position.x = glm::sin(phi) * glm::cos(theta);
	position.y = glm::cos(phi);
	position.z = glm::sin(phi) * glm::sin(theta);

	position *= distanceFromOrigin;

	view = glm::lookAt(position, grid.GetCenter(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void CameraController::Input()
{
	MouseInput();
}

void CameraController::Scroll(float amount)
{
	distanceFromOrigin -= amount * scrollSensitivity;
}

void CameraController::MouseInput()
{
	int x, y;
	const Uint32 mouseState = SDL_GetMouseState(&x, &y);

	const float deltaX = x - mousePosition.x;
	const float deltaY = y - mousePosition.y;

	mousePosition.x = (float)x;
	mousePosition.y = (float)y;

	if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		phiDelta = deltaY * mouseSensitivity;
		thetaDelta = deltaX * mouseSensitivity;
	}
}

void CameraController::ApplyUniforms(const ShaderProgram& shaderProgram)
{
	glUniformMatrix4fv(shaderProgram["view"], 1, false, &view[0][0]);
	glUniformMatrix4fv(shaderProgram["projection"], 1, false, &projection[0][0]);
}