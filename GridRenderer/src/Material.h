#pragma once

#include <string>

#include "ShaderProgram.h"
#include "ShaderMetaData.h"
#include "CameraController.h"

class Grid;

class Material
{
public:
	void Update();
	void Render(const CameraController& camera, const Grid& grid) const;
	void DrawUI();

private:
	void RecompileShaderProgram();
	void ApplyDefaultUniforms() const;

	bool isHotloadEnabled = true;

	std::string vertexShaderFilename = "shaders\\default.vert";
	std::string fragmentShaderFilename = "shaders\\default.frag";

	ShaderProgram shaderProgram = ShaderProgram(vertexShaderFilename, fragmentShaderFilename);
	ShaderMetaData shaderMetaData = ShaderMetaData(vertexShaderFilename, fragmentShaderFilename);
};