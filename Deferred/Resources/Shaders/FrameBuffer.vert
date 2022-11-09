/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.vert																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

layout (location = 0) in vec3 in_v3Position;
layout (location = 2) in vec2 in_v2TextureCoord;

out vec2 vs_v2TextureCoord;

void main()
{
	vs_v2TextureCoord = in_v2TextureCoord;
	gl_Position = vec4(in_v3Position, 1.0f);
}