#version 450 core

in float param1;
in float param2;
in float param3;

out vec4 outColor;

uniform vec3 defaultColor;

void main(void)
{
	outColor = vec4(defaultColor.r, defaultColor.g, defaultColor.b, 1.0);
}
