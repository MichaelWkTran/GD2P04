/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Unlit.vert																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

layout (location = 0) in vec3 in_v3Position;

uniform mat4 uni_mat4LightProjection;
uniform mat4 uni_mat4Model;

void main()
{
	gl_Position = uni_mat4LightProjection * uni_mat4Model * vec4(in_v3Position, 1.0);
}