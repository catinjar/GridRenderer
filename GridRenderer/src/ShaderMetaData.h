#pragma once

#include <string>
#include <vector>

#include "ShaderProgram.h"
#include "UniformParam.h"

class ShaderMetaData
{
public:
	ShaderMetaData() {}
	ShaderMetaData(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

	void ApplyUniforms(const ShaderProgram& shaderProgram) const;
	void DrawUI();

private:
	std::string vertexShaderSource;
	std::string fragmentShaderSource;

	void ProcessShader(const std::string& shaderSource);
	ShaderDataType GetDataType(const std::string& typeStr);

	std::vector<UniformParam> uniforms;
};