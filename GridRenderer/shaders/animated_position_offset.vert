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
uniform mat4 model;

uniform vec3 offset1;
uniform vec3 offset2;

uniform vec4 sinTime;

void main(void)
{
	param1 = inParam1;
	param2 = inParam2;
	param3 = inParam3;

	vec3 offset = mix(offset1, offset2, sinTime.x * sinTime.x);

	gl_PointSize = 2.0f;
	gl_Position = projection * view * model * (inPosition + vec4(offset, 0.0));
}
