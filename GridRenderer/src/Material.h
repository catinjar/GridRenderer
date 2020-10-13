#pragma once

#include <string>

#include "ShaderProgram.h"
#include "ShaderMetaData.h"
#include "CameraController.h"
#include "Grid.h"

class Material
{
public:
	void Update();
	void Render(const CameraController& camera, const Grid& grid) const;
	void DrawUI();

private:
	void RecompileShaderProgram();

	bool isHotloadEnabled = true;

	std::string vertexShaderFilename = "shaders\\default.vert";
	std::string fragmentShaderFilename = "shaders\\default.frag";

	ShaderProgram shaderProgram = ShaderProgram(vertexShaderFilename, fragmentShaderFilename);
	ShaderMetaData shaderMetaData = ShaderMetaData(vertexShaderFilename, fragmentShaderFilename);

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
};