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

		switch (uniform.dataType)
		{
		case ShaderDataType::Bool:
			glUniform1i(location, uniform.boolValue);
			break;

		case ShaderDataType::Int:
			glUniform1i(location, uniform.intValue);
			break;

		case ShaderDataType::Uint:
			glUniform1ui(location, uniform.intValue);
			break;

		case ShaderDataType::Float:
			glUniform1f(location, uniform.floatValue);
			break;

		case ShaderDataType::Double:
			glUniform1d(location, uniform.doubleValue);
			break;

		case ShaderDataType::bVec2:
			glUniform2i(location, uniform.boolVec2[0], uniform.boolVec2[1]);
			break;

		case ShaderDataType::bVec3:
			glUniform3i(location, uniform.boolVec3[0], uniform.boolVec3[1], uniform.boolVec3[2]);
			break;

		case ShaderDataType::bVec4:
			glUniform4i(location, uniform.boolVec4[0], uniform.boolVec4[1], uniform.boolVec4[2], uniform.boolVec4[3]);
			break;

		case ShaderDataType::iVec2:
			glUniform2iv(location, 1, &uniform.intVec2[0]);
			break;

		case ShaderDataType::iVec3:
			glUniform3iv(location, 1, &uniform.intVec3[0]);
			break;

		case ShaderDataType::iVec4:
			glUniform4iv(location, 1, &uniform.intVec4[0]);
			break;

		case ShaderDataType::uVec2:
			glUniform2uiv(location, 1, (GLuint*)(&uniform.intVec2[0]));
			break;

		case ShaderDataType::uVec3:
			glUniform3uiv(location, 1, (GLuint*)(&uniform.intVec3[0]));
			break;

		case ShaderDataType::uVec4:
			glUniform4uiv(location, 1, (GLuint*)(&uniform.intVec4[0]));
			break;

		case ShaderDataType::Vec2:
			glUniform2fv(location, 1, &uniform.vec2[0]);
			break;

		case ShaderDataType::Vec3:
			glUniform3fv(location, 1, &uniform.vec3[0]);
			break;

		case ShaderDataType::Vec4:
			glUniform4fv(location, 1, &uniform.vec4[0]);
			break;

		case ShaderDataType::dVec2:
			glUniform2d(location, uniform.vec2[0], uniform.vec2[1]);
			break;

		case ShaderDataType::dVec3:
			glUniform3d(location, uniform.vec3[0], uniform.vec3[1], uniform.vec3[2]);
			break;

		case ShaderDataType::dVec4:
			glUniform4d(location, uniform.vec4[0], uniform.vec4[1], uniform.vec4[2], uniform.vec4[3]);
			break;

		case ShaderDataType::Mat2x2:
		case ShaderDataType::Mat2:
			glUniformMatrix2fv(location, 1, false, &uniform.mat2x2[0][0]);
			break;

		case ShaderDataType::Mat2x3:
			glUniformMatrix2x3fv(location, 1, false, &uniform.mat2x3[0][0]);
			break;

		case ShaderDataType::Mat2x4:
			glUniformMatrix2x4fv(location, 1, false, &uniform.mat2x4[0][0]);
			break;

		case ShaderDataType::Mat3x2:
			glUniformMatrix3x2fv(location, 1, false, &uniform.mat3x2[0][0]);
			break;

		case ShaderDataType::Mat3x3:
		case ShaderDataType::Mat3:
			glUniformMatrix3fv(location, 1, false, &uniform.mat3x3[0][0]);
			break;

		case ShaderDataType::Mat4x2:
			glUniformMatrix4x2fv(location, 1, false, &uniform.mat4x2[0][0]);
			break;

		case ShaderDataType::Mat4x3:
			glUniformMatrix4x3fv(location, 1, false, &uniform.mat4x3[0][0]);
			break;

		case ShaderDataType::Mat4x4:
		case ShaderDataType::Mat4:
			glUniformMatrix4fv(location, 1, false, &uniform.mat4x4[0][0]);
			break;
		}
	}
}

void ShaderMetaData::DrawUI()
{
	ImGui::Text("Parameters");

	for (auto& uniform : uniforms)
	{
		switch (uniform.dataType)
		{
		case ShaderDataType::Bool:
			ImGui::Checkbox(uniform.name.c_str(), &uniform.boolValue);
			break;

		case ShaderDataType::Int:
			ImGui::DragInt(uniform.name.c_str(), &uniform.intValue);
			break;

		case ShaderDataType::Uint:
			ImGui::DragInt(uniform.name.c_str(), &uniform.intValue, 1, 0, std::numeric_limits<uint32_t>::max());
			break;

		case ShaderDataType::Float:
		case ShaderDataType::Double:
			ImGui::DragFloat(uniform.name.c_str(), &uniform.floatValue, 0.01f);
			break;

		case ShaderDataType::bVec2:
			ImGui::Text(uniform.name.c_str());
			ImGui::SameLine();
			ImGui::Checkbox("x##bVec", &uniform.boolVec2[0]);
			ImGui::SameLine();
			ImGui::Checkbox("y##bVec", &uniform.boolVec2[1]);
			break;

		case ShaderDataType::bVec3:
			ImGui::Text(uniform.name.c_str());
			ImGui::SameLine();
			ImGui::Checkbox("x##bVec", &uniform.boolVec3[0]);
			ImGui::SameLine();
			ImGui::Checkbox("y##bVec", &uniform.boolVec3[1]);
			ImGui::SameLine();
			ImGui::Checkbox("z##bVec", &uniform.boolVec3[2]);
			break;

		case ShaderDataType::bVec4:
			ImGui::Text(uniform.name.c_str());
			ImGui::SameLine();
			ImGui::Checkbox("x##bVec", &uniform.boolVec4[0]);
			ImGui::SameLine();
			ImGui::Checkbox("y##bVec", &uniform.boolVec4[1]);
			ImGui::SameLine();
			ImGui::Checkbox("z##bVec", &uniform.boolVec4[2]);
			ImGui::SameLine();
			ImGui::Checkbox("w##bVec", &uniform.boolVec4[3]);
			break;

		case ShaderDataType::iVec2:
			ImGui::DragInt2(uniform.name.c_str(), &uniform.intVec2[0]);
			break;

		case ShaderDataType::iVec3:
			ImGui::DragInt3(uniform.name.c_str(), &uniform.intVec3[0]);
			break;

		case ShaderDataType::iVec4:
			ImGui::DragInt4(uniform.name.c_str(), &uniform.intVec4[0]);
			break;

		case ShaderDataType::uVec2:
			ImGui::DragInt2(uniform.name.c_str(), &uniform.intVec2[0], 1, 0, std::numeric_limits<uint32_t>::max());
			break;

		case ShaderDataType::uVec3:
			ImGui::DragInt3(uniform.name.c_str(), &uniform.intVec3[0], 1, 0, std::numeric_limits<uint32_t>::max());
			break;

		case ShaderDataType::uVec4:
			ImGui::DragInt4(uniform.name.c_str(), &uniform.intVec4[0], 1, 0, std::numeric_limits<uint32_t>::max());
			break;

		case ShaderDataType::Vec2:
		case ShaderDataType::dVec2:
			ImGui::DragFloat2(uniform.name.c_str(), &uniform.vec2[0], 0.01f);
			break;

		case ShaderDataType::Vec3:
		case ShaderDataType::dVec3:
			ImGui::DragFloat3(uniform.name.c_str(), &uniform.vec3[0], 0.01f);
			break;

		case ShaderDataType::Vec4:
		case ShaderDataType::dVec4:
			ImGui::DragFloat4(uniform.name.c_str(), &uniform.vec4[0], 0.01f);
			break;

		case ShaderDataType::Mat2x2:
		case ShaderDataType::Mat2:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat2x2[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat2x3:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat2x3[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat2x4:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 2; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat2x4[i][j], 0.01f);
					ImGui::SameLine();
				}
				
				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat3x2:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat3x2[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat3x3:
		case ShaderDataType::Mat3:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat3x3[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat3x4:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat3x4[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat4x2:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 2; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat4x2[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat4x3:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat4x3[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Mat4x4:
		case ShaderDataType::Mat4:
			ImGui::PushID(uniform.name.c_str());
			ImGui::Text(uniform.name.c_str());
			ImGui::PushItemWidth(50.0f);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ImGui::Text("%ix%i", i, j);
					ImGui::SameLine();
					ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &uniform.mat4x4[i][j], 0.01f);
					ImGui::SameLine();
				}

				ImGui::NewLine();
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
			break;

		case ShaderDataType::Invalid:
			ImGui::Text("Uniform with name '%s' has invalid data type!", uniform.name.c_str());
			break;
		}
	}
}