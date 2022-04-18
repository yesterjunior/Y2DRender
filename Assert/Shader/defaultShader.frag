#version 330 core

out vec4 Color;

in vec2 texcoord;
in vec4 color;

uniform int hasTexrure;
uniform sampler2D defaultTexture;

void main()
{
	if(hasTexrure == 1){
		Color = texture(defaultTexture, texcoord) * color;
	}
	else{
		Color = color;
	}
}