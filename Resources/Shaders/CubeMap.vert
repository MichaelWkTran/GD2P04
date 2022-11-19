/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: CubeMap.vert																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

layout (location = 0) in vec3 in_v3Position;

out vec3 vs_v3TextureCoord;

uniform vec3 uni_v3CameraPosition;
uniform mat4 uni_mat4CameraMatrix;

void main()
{
	vs_v3TextureCoord = in_v3Position;

	gl_Position = uni_mat4CameraMatrix * vec4(in_v3Position + uni_v3CameraPosition, 1.0);
}