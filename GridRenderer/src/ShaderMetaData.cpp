#include "ShaderMetaData.h"

#include <sstream>
#include <limits>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "third_party/imgui/imgui.h"

#include "Misc.h"

ShaderMetaData::ShaderMetaData(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	this->vertexShaderSource = vertexShaderSource;
	this->fragmentShaderSource = fragmentShaderSource;
}

void ShaderMetaData::ProcessShader(const std::string& shaderSource)
{
	std::istringstream stream(shaderSource);
	std::string line;

	while (std::getline(stream, line))
	{
		const auto tokens = misc::Split(line, ' ');

		if (tokens[0] == "uniform")
		{
			const auto name = tokens[2].substr(0, tokens[2].length() - 2); // '\r' and ';'

			if (name == "view" || name == "projection" || name == "model" || name == "time" || name == "cosTime" || name == "sinTime") // Reserved uniforms
				continue;

			uniforms.push_back(
			{
				.name = name,
				.dataType = GetDataType(tokens[1])
			});
		}
	}
}

void ShaderMetaData::ApplyUniforms(const ShaderProgram& shaderProgram) const
{
	for (const auto& uniform : uniforms)
	{
		const auto location = shaderProgram[uniform.name];
		uniform.ApplyUniforms(location);
	}
}

void ShaderMetaData::DrawUI()
{
	ImGui::Text("Parameters");

	for (auto& uniform : uniforms)
	{
		uniform.DrawUI();
	}
}