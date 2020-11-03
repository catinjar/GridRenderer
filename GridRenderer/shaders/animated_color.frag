#version 450 core

in float param1;
in float param2;
in float param3;

out vec4 outColor;

uniform vec4 color1;
uniform vec4 color2;

uniform vec4 cosTime;

void main(void)
{
	outColor = mix(color1, color2, cosTime.x * cosTime.x);
}
