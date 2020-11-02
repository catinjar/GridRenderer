#version 450 core

in float param1;
in float param2;
in float param3;

out vec4 outColor;

uniform vec3 defaultColor;
uniform vec4 sinTime;

void main(void)
{
	outColor = vec4(defaultColor.r * sinTime.x, defaultColor.g * sinTime.x, defaultColor.b * sinTime.x, 1.0);
}
