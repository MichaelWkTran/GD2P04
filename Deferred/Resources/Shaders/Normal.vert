#version 430 core

layout (location = 0) in vec3 in_v3Position;
layout (location = 1) in vec3 in_v3Normal;

out VS_GS_VERTEX
{
	out vec4 v4Position;
	out vec4 v4Normal;
} vs_out;

uniform mat4 uni_mat4Model;
uniform mat4 uni_mat4CameraMatrix;

void main()
{
	gl_Position = uni_mat4CameraMatrix * uni_mat4Model * vec4(in_v3Position, 1.0f);
	
	vs_out.v4Position = gl_Position;
	vs_out.v4Normal = uni_mat4CameraMatrix * uni_mat4Model * vec4(in_v3Normal, 1.0f) * 100.0f;
}
