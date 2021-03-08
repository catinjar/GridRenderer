#pragma once

#include <string>

#include "ShaderProgram.h"
#include "ShaderMetaData.h"
#include "CameraController.h"
#include "NodeGraph.h"

class Grid;

class Material
{
public:
	Material() {}

	Material(const std::string& vertexSourceCode, const std::string& fragmentSourceCode) :
		vertexSourceCode(vertexSourceCode), fragmentSourceCode(fragmentSourceCode),
		shaderProgram(vertexSourceCode, fragmentSourceCode), shaderMetaData(vertexSourceCode, fragmentSourceCode)
	{}

	void Render(const CameraController& camera, const Grid& grid) const;
	void DrawUI();
	void SetSourceCode(const std::string& vertexSourceCode, const std::string& fragmentSourceCode);

	const ShaderProgram& GetShaderProgram() const { return shaderProgram; }
	const std::string& GetVertexSourceCode() const { return vertexSourceCode; }
	const std::string& GetFragmentSourceCode() const { return fragmentSourceCode; }

private:
	void ApplyDefaultUniforms(const CameraController& camera, const Grid& grid) const;

	std::string vertexSourceCode;
	std::string fragmentSourceCode;

	ShaderProgram shaderProgram;
	ShaderMetaData shaderMetaData;

	bool isInit = false;
};