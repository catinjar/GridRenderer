#version 450 core

layout(location = 0) in vec4 inPosition;

uniform mat4 view;
uniform mat4 projection;

void main(void)
{
  gl_PointSize = 2.0f;
  gl_Position = projection * view * inPosition;
}
