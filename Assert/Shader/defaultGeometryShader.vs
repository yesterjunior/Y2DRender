#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec4 Color;

out vec4 color;

void main()
{
	gl_Position = vec4(Position, 1.0f);
	color = Color;
}