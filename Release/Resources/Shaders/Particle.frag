#version 430 core

in vec2 gs_textureCoordinate;
in float gs_life;
out vec4 fs_colour;
uniform sampler2D uni_texture;

void main()
{
	fs_colour = texture(uni_texture, gs_textureCoordinate);
	fs_colour.w *= gs_life;
}