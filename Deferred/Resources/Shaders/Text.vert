/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Text.vert																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

layout (location = 0) in vec4 in_v4Vertex; // (vec2:Position, vec2:TexCoords)

out vec2 m_v2TextureCoord;

uniform mat4 uni_mat4Projection;

void main()
{
	gl_Position = uni_mat4Projection * vec4(in_v4Vertex.xy, 0.0f, 1.0f);
	m_v2TextureCoord = in_v4Vertex.zw;
}