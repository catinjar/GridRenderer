#version 450 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in float inParam1;
layout(location = 2) in float inParam2;
layout(location = 3) in float inParam3;

out float param1;
out float param2;
out float param3;

uniform mat4 view;
uniform mat4 projection;

uniform bool boolValue;
uniform int intValue;
uniform uint uintValue;
uniform float floatValue;
uniform double doubleValue;
uniform bvec2 boolVec2;
uniform bvec3 boolVec3;
uniform bvec4 boolVec4;
uniform ivec2 intVec2;
uniform ivec3 intVec3;
uniform ivec4 intVec4;
uniform uvec2 uintVec2;
uniform uvec3 uintVec3;
uniform uvec4 uintVec4;
uniform vec2 vector2;
uniform vec3 vector3;
uniform vec4 vector4;
uniform dvec2 doubleVec2;
uniform dvec3 doubleVec3;
uniform dvec4 doubleVec4;
uniform mat2x2 matrix2x2;
uniform mat2x3 matrix2x3;
uniform mat2x4 matrix2x4;
uniform mat2 matrix2;
uniform mat3x2 matrix3x2;
uniform mat3x3 matrix3x3;
uniform mat3x4 matrix3x4;
uniform mat3 matrix3;
uniform mat4x2 matrix4x2;
uniform mat4x3 matrix4x3;
uniform mat4x4 matrix4x4;
uniform mat4 matrix4;

void main(void)
{
	param1 = inParam1;
	param2 = inParam2;
	param3 = inParam3;

	gl_PointSize = 2.0f;
	gl_Position = projection * view * inPosition;
}
