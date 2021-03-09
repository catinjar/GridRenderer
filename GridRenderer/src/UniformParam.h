#pragma once

#include <string>

#include <GL/glew.h>

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

	bool boolValue = true;
	int32_t intValue = 0;
	float_t floatValue = 1.0f;
	double_t doubleValue = 1.0;
	bool boolVec2[2] { true };
	bool boolVec3[3] { true };
	bool boolVec4[4] { true };
	int32_t intVec2[2] { 0 };
	int32_t intVec3[3] { 0 };
	int32_t intVec4[4] { 0 };
	float_t vec2[2] { 0.0f };
	float_t vec3[3] { 0.0f };
	float_t vec4[4] { 0.0f };
	float_t mat2x2[2][2] { { 0.0f } };
	float_t mat2x3[2][3] { { 0.0f } };
	float_t mat2x4[2][4] { { 0.0f } };
	float_t mat3x2[3][2] { { 0.0f } };
	float_t mat3x3[3][3] { { 0.0f } };
	float_t mat3x4[3][4] { { 0.0f } };
	float_t mat4x2[4][2] { { 0.0f } };
	float_t mat4x3[4][3] { { 0.0f } };
	float_t mat4x4[4][4] { { 0.0f } };

	std::string GetTypeName() const;
	void ApplyUniforms(GLuint location) const;
	void DrawUI();
};