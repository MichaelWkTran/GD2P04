/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Unlit.frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

in vec2 vs_v2TextureCoord;

uniform sampler2D uni_samp2DDiffuse0;
uniform vec3 uni_v3Colour = vec3(1.0f, 1.0f, 1.0f);

void main()
{
	fs_v4Colour = vec4(uni_v3Colour, 1.0f);
	if (textureSize(uni_samp2DDiffuse0, 0) != vec2(1,1)) fs_v4Colour *= texture(uni_samp2DDiffuse0, vs_v2TextureCoord);
}