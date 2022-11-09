/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: CubeMap.frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

in vec3 vs_v3TextureCoord;

uniform samplerCube uni_sampCube;

void main()
{
	fs_v4Colour = texture(uni_sampCube, vs_v3TextureCoord); //vec4(1.0f,1.0f,1.0f,1.0f);// 
}