#version 450 core

in float param1;
in float param2;
in float param3;

out vec4 outColor;

uniform vec3 color; 
uniform float threshold;

void main(void)
{
	if (param1 > threshold)
		discard;

	outColor = vec4(color.r, color.g, color.b, 1.0);
}
