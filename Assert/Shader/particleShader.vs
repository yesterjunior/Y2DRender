#version 330 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 Texcoord;

layout(location = 2) in vec3 Transform;
layout(location = 3) in vec4 Color;

out vec2 texcoord;
out vec4 color;

void main()
{
	vec2 newpos = Position.xy + Transform.xy;
	gl_Position = vec4(newpos.xy, 0.0f, 1.0f);
	color = Color;
	texcoord = Texcoord;
}