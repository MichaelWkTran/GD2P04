#version 430 core

in vec2 v2TextureCoordinate;

out vec4 fs_v4Colour;

uniform sampler2D uni_samp2DDiffuse0;

void main()
{
	fs_v4Colour = texture(uni_samp2DDiffuse0, v2TextureCoordinate);
}