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

ShaderDataType ShaderMetaData::GetDataType(const std::string& typeStr)
{
	if (typeStr == "bool")
		return ShaderDataType::Bool;
	else if (typeStr == "int")
		return ShaderDataType::Int;
	else if (typeStr == "uint")
		return ShaderDataType::Uint;
	else if (typeStr == "float")
		return ShaderDataType::Float;
	else if (typeStr == "double")
		return ShaderDataType::Double;
	else if (typeStr == "bvec2")
		return ShaderDataType::bVec2;
	else if (typeStr == "bvec3")
		return ShaderDataType::bVec3;
	else if (typeStr == "bvec4")
		return ShaderDataType::bVec4;
	else if (typeStr == "ivec2")
		return ShaderDataType::iVec2;
	else if (typeStr == "ivec3")
		return ShaderDataType::iVec3;
	else if (typeStr == "ivec4")
		return ShaderDataType::iVec4;
	else if (typeStr == "uvec2")
		return ShaderDataType::uVec2;
	else if (typeStr == "uvec3")
		return ShaderDataType::uVec3;
	else if (typeStr == "uvec4")
		return ShaderDataType::uVec4;
	else if (typeStr == "vec2")
		return ShaderDataType::Vec2;
	else if (typeStr == "vec3")
		return ShaderDataType::Vec3;
	else if (typeStr == "vec4")
		return ShaderDataType::Vec4;
	else if (typeStr == "dvec2")
		return ShaderDataType::dVec2;
	else if (typeStr == "dvec3")
		return ShaderDataType::dVec3;
	else if (typeStr == "dvec4")
		return ShaderDataType::dVec4;
	else if (typeStr == "mat2x2")
		return ShaderDataType::Mat2x2;
	else if (typeStr == "mat2")
		return ShaderDataType::Mat2;
	else if (typeStr == "mat2x3")
		return ShaderDataType::Mat2x3;
	else if (typeStr == "mat2x4")
		return ShaderDataType::Mat2x4;
	else if (typeStr == "mat3x2")
		return ShaderDataType::Mat3x2;
	else if (typeStr == "mat3x3")
		return ShaderDataType::Mat3x3;
	else if (typeStr == "mat3")
		return ShaderDataType::Mat3;
	else if (typeStr == "mat3x4")
		return ShaderDataType::Mat3x4;
	else if (typeStr == "mat4x2")
		return ShaderDataType::Mat4x2;
	else if (typeStr == "mat4x3")
		return ShaderDataType::Mat4x3;
	else if (typeStr == "mat4x4")
		return ShaderDataType::Mat4x4;
	else if (typeStr == "mat4")
		return ShaderDataType::Mat4;

	return ShaderDataType::Invalid;
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