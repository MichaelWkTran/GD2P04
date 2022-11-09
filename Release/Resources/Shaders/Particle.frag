#version 430 core

in vec2 v2TextureCoordinate;
out vec4 fs_v4Colour;

uniform vec4 uni_v4Colour = vec4(1.0);
uniform sampler2D uni_samp2DDiffuse0;

void main()
{
	fs_v4Colour = uni_v4Colour * texture(uni_samp2DDiffuse0, v2TextureCoordinate);
}