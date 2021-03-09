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

	glm::mat4 GetProjection() const { return projection; }
	glm::mat4 GetView() const { return view; }

private:
	void MouseInput();

	float phi;
	float theta;

	float phiDelta = 0.0f;
	float thetaDelta = 0.0f;

	float distanceFromOrigin;

	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 position;
	glm::vec3 look;
	glm::vec3 up;
	
	float cameraSpeed;
	
	glm::vec2 mousePosition = glm::vec2();
	float mouseSensitivity;
	float scrollSensitivity;
	
	float pitchDelta = 0.0f;
	float yawDelta = 0.0f;
};