/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.Frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

uniform sampler2D renderTexture;
in vec2 vs_v2TextureCoord;

void main()
{
	fs_v4Colour = texture(renderTexture, vs_v2TextureCoord);
}