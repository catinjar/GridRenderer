#pragma once

#include <string>
#include <vector>

#include "ShaderProgram.h"

enum class ShaderDataType
{
	Bool,
	Int,
	Uint,
	Float,
	Double,
	bVec2,
	bVec3,
	bVec4,
	iVec2,
	iVec3,
	iVec4,
	uVec2,
	uVec3,
	uVec4,
	Vec2,
	Vec3,
	Vec4,
	dVec2,
	dVec3,
	dVec4,
	Mat2x2,
	Mat2x3,
	Mat2x4,
	Mat3x2,
	Mat3x3,
	Mat3x4,
	Mat4x2,
	Mat4x3,
	Mat4x4,
	Mat2,
	Mat3,
	Mat4,
	Color,
	Invalid
};

struct UniformParam
{
	std::string name;
	ShaderDataType dataType;
	
	bool boolValue;
	int32_t intValue;
	float_t floatValue;
	double_t doubleValue;
	bool boolVec2[2];
	bool boolVec3[3];
	bool boolVec4[4];
	int32_t intVec2[2];
	int32_t intVec3[3];
	int32_t intVec4[4];
	float_t vec2[2];
	float_t vec3[3];
	float_t vec4[4];
	float_t mat2x2[2][2];
	float_t mat2x3[2][3];
	float_t mat2x4[2][4];
	float_t mat3x2[3][2];
	float_t mat3x3[3][3];
	float_t mat3x4[3][4];
	float_t mat4x2[4][2];
	float_t mat4x3[4][3];
	float_t mat4x4[4][4];

	void ApplyUniforms(GLuint location) const;
	void DrawUI();
};

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