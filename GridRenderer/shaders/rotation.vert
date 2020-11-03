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

uniform float angleX;
uniform float angleY;
uniform float angleZ;

mat4 rotationX(in float angle)
{
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY(in float angle)
{
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ(in float angle)
{
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

void main(void)
{
	param1 = inParam1;
	param2 = inParam2;
	param3 = inParam3;

	gl_PointSize = 2.0f;
	gl_Position = projection * view * model * rotationX(angleX) * rotationY(angleY) * rotationZ(angleZ) * inPosition;
}
