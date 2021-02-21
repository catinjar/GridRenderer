#version 450 core

in float param1;
in float param2;
in float param3;

out vec4 outColor;

void main(void)
{
	float color = param1;
	outColor = vec4(color, color, color, 1.0);
}
