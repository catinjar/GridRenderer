#include "UniformParam.h"

#include "third_party/imgui/imgui.h"

std::string UniformParam::GetTypeName() const
{
	switch (dataType)
	{
		case ShaderDataType::Bool:		return "bool";
		case ShaderDataType::Int:		return "int";
		case ShaderDataType::Uint:		return "uint";
		case ShaderDataType::Float:		return "float";
		case ShaderDataType::Double:	return "double";
		case ShaderDataType::bVec2:		return "bvec2";
		case ShaderDataType::bVec3:		return "bvec3";
		case ShaderDataType::bVec4:		return "bvec4";
		case ShaderDataType::iVec2:		return "ivec2";
		case ShaderDataType::iVec3:		return "ivec3";
		case ShaderDataType::iVec4:		return "ivec4";
		case ShaderDataType::uVec2:		return "uvec2";
		case ShaderDataType::uVec3:		return "uvec3";
		case ShaderDataType::uVec4:		return "uvec4";
		case ShaderDataType::Vec2:		return "vec2";
		case ShaderDataType::Vec3:		return "vec3";
		case ShaderDataType::Vec4:		return "vec4";
		case ShaderDataType::Color:		return "vec4";
		case ShaderDataType::dVec2:		return "dvec2";
		case ShaderDataType::dVec3:		return "dvec3";
		case ShaderDataType::dVec4:		return "dvec4";
		case ShaderDataType::Mat2x2:	return "mat2x2";
		case ShaderDataType::Mat2:		return "mat2";
		case ShaderDataType::Mat2x3:	return "mat2x3";
		case ShaderDataType::Mat2x4:	return "mat2x4";
		case ShaderDataType::Mat3x2:	return "mat3x2";
		case ShaderDataType::Mat3x3:	return "mat3x3";
		case ShaderDataType::Mat3:		return "mat3";
		case ShaderDataType::Mat4x2:	return "mat4x2";
		case ShaderDataType::Mat4x3:	return "mat4x3";
		case ShaderDataType::Mat4x4:	return "mat4x4";
		case ShaderDataType::Mat4:		return "mat4";
	}
}

void UniformParam::ApplyUniforms(GLuint location) const
{
	switch (dataType)
	{
	case ShaderDataType::Bool:
		glUniform1i(location, boolValue);
		break;

	case ShaderDataType::Int:
		glUniform1i(location, intValue);
		break;

	case ShaderDataType::Uint:
		glUniform1ui(location, intValue);
		break;

	case ShaderDataType::Float:
		glUniform1f(location, floatValue);
		break;

	case ShaderDataType::Double:
		glUniform1d(location, doubleValue);
		break;

	case ShaderDataType::bVec2:
		glUniform2i(location, boolVec2[0], boolVec2[1]);
		break;

	case ShaderDataType::bVec3:
		glUniform3i(location, boolVec3[0], boolVec3[1], boolVec3[2]);
		break;

	case ShaderDataType::bVec4:
		glUniform4i(location, boolVec4[0], boolVec4[1], boolVec4[2], boolVec4[3]);
		break;

	case ShaderDataType::iVec2:
		glUniform2iv(location, 1, &intVec2[0]);
		break;

	case ShaderDataType::iVec3:
		glUniform3iv(location, 1, &intVec3[0]);
		break;

	case ShaderDataType::iVec4:
		glUniform4iv(location, 1, &intVec4[0]);
		break;

	case ShaderDataType::uVec2:
		glUniform2uiv(location, 1, (GLuint*)(&intVec2[0]));
		break;

	case ShaderDataType::uVec3:
		glUniform3uiv(location, 1, (GLuint*)(&intVec3[0]));
		break;

	case ShaderDataType::uVec4:
		glUniform4uiv(location, 1, (GLuint*)(&intVec4[0]));
		break;

	case ShaderDataType::Vec2:
		glUniform2fv(location, 1, &vec2[0]);
		break;

	case ShaderDataType::Vec3:
		glUniform3fv(location, 1, &vec3[0]);
		break;

	case ShaderDataType::Vec4:
	case ShaderDataType::Color:
		glUniform4fv(location, 1, &vec4[0]);
		break;

	case ShaderDataType::dVec2:
		glUniform2d(location, vec2[0], vec2[1]);
		break;

	case ShaderDataType::dVec3:
		glUniform3d(location, vec3[0], vec3[1], vec3[2]);
		break;

	case ShaderDataType::dVec4:
		glUniform4d(location, vec4[0], vec4[1], vec4[2], vec4[3]);
		break;

	case ShaderDataType::Mat2x2:
	case ShaderDataType::Mat2:
		glUniformMatrix2fv(location, 1, false, &mat2x2[0][0]);
		break;

	case ShaderDataType::Mat2x3:
		glUniformMatrix2x3fv(location, 1, false, &mat2x3[0][0]);
		break;

	case ShaderDataType::Mat2x4:
		glUniformMatrix2x4fv(location, 1, false, &mat2x4[0][0]);
		break;

	case ShaderDataType::Mat3x2:
		glUniformMatrix3x2fv(location, 1, false, &mat3x2[0][0]);
		break;

	case ShaderDataType::Mat3x3:
	case ShaderDataType::Mat3:
		glUniformMatrix3fv(location, 1, false, &mat3x3[0][0]);
		break;

	case ShaderDataType::Mat4x2:
		glUniformMatrix4x2fv(location, 1, false, &mat4x2[0][0]);
		break;

	case ShaderDataType::Mat4x3:
		glUniformMatrix4x3fv(location, 1, false, &mat4x3[0][0]);
		break;

	case ShaderDataType::Mat4x4:
	case ShaderDataType::Mat4:
		glUniformMatrix4fv(location, 1, false, &mat4x4[0][0]);
		break;
	}
}

void UniformParam::DrawUI()
{
	switch (dataType)
	{
	case ShaderDataType::Bool:
		ImGui::Checkbox(name.c_str(), &boolValue);
		break;

	case ShaderDataType::Int:
		ImGui::DragInt(name.c_str(), &intValue);
		break;

	case ShaderDataType::Uint:
		ImGui::DragInt(name.c_str(), &intValue, 1, 0, std::numeric_limits<uint32_t>::max());
		break;

	case ShaderDataType::Float:
	case ShaderDataType::Double:
		ImGui::DragFloat(name.c_str(), &floatValue, 0.01f);
		break;

	case ShaderDataType::bVec2:
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Checkbox("x##bVec", &boolVec2[0]);
		ImGui::SameLine();
		ImGui::Checkbox("y##bVec", &boolVec2[1]);
		break;

	case ShaderDataType::bVec3:
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Checkbox("x##bVec", &boolVec3[0]);
		ImGui::SameLine();
		ImGui::Checkbox("y##bVec", &boolVec3[1]);
		ImGui::SameLine();
		ImGui::Checkbox("z##bVec", &boolVec3[2]);
		break;

	case ShaderDataType::bVec4:
		ImGui::Text(name.c_str());
		ImGui::SameLine();
		ImGui::Checkbox("x##bVec", &boolVec4[0]);
		ImGui::SameLine();
		ImGui::Checkbox("y##bVec", &boolVec4[1]);
		ImGui::SameLine();
		ImGui::Checkbox("z##bVec", &boolVec4[2]);
		ImGui::SameLine();
		ImGui::Checkbox("w##bVec", &boolVec4[3]);
		break;

	case ShaderDataType::iVec2:
		ImGui::DragInt2(name.c_str(), &intVec2[0]);
		break;

	case ShaderDataType::iVec3:
		ImGui::DragInt3(name.c_str(), &intVec3[0]);
		break;

	case ShaderDataType::iVec4:
		ImGui::DragInt4(name.c_str(), &intVec4[0]);
		break;

	case ShaderDataType::uVec2:
		ImGui::DragInt2(name.c_str(), &intVec2[0], 1, 0, std::numeric_limits<uint32_t>::max());
		break;

	case ShaderDataType::uVec3:
		ImGui::DragInt3(name.c_str(), &intVec3[0], 1, 0, std::numeric_limits<uint32_t>::max());
		break;

	case ShaderDataType::uVec4:
		ImGui::DragInt4(name.c_str(), &intVec4[0], 1, 0, std::numeric_limits<uint32_t>::max());
		break;

	case ShaderDataType::Vec2:
	case ShaderDataType::dVec2:
		ImGui::DragFloat2(name.c_str(), &vec2[0], 0.01f);
		break;

	case ShaderDataType::Vec3:
	case ShaderDataType::dVec3:
		ImGui::DragFloat3(name.c_str(), &vec3[0], 0.01f);
		break;

	case ShaderDataType::Vec4:
	case ShaderDataType::dVec4:
		ImGui::DragFloat4(name.c_str(), &vec4[0], 0.01f);
		break;

	case ShaderDataType::Mat2x2:
	case ShaderDataType::Mat2:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat2x2[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat2x3:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat2x3[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat2x4:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 2; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat2x4[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat3x2:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat3x2[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat3x3:
	case ShaderDataType::Mat3:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat3x3[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat3x4:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat3x4[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat4x2:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 2; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat4x2[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat4x3:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat4x3[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Mat4x4:
	case ShaderDataType::Mat4:
		ImGui::PushID(name.c_str());
		ImGui::Text(name.c_str());
		ImGui::PushItemWidth(50.0f);
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				ImGui::Text("%ix%i", i, j);
				ImGui::SameLine();
				ImGui::DragFloat(("##" + std::to_string(i) + "x" + std::to_string(j)).c_str(), &mat4x4[i][j], 0.01f);
				ImGui::SameLine();
			}

			ImGui::NewLine();
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		break;

	case ShaderDataType::Color:
		ImGui::ColorPicker3(name.c_str(), &vec4[0]);
		break;

	case ShaderDataType::Invalid:
		ImGui::Text("Uniform with name '%s' has invalid data type!", name.c_str());
		break;
	}
}