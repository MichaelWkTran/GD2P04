#version 430 core

layout (location = 0) in vec3 in_v3Position;

out VS_GS_VERTEX
{
	out vec4 v3Position;
	out mat4 mat4CameraMatrix;
} vs_out;

uniform mat4 uni_mat4CameraMatrix;

void main()
{
	gl_Position = uni_mat4CameraMatrix * vec4(in_v3Position, 1.0f);
	
	vs_out.v3Position = gl_Position;
	vs_out.mat4CameraMatrix = uni_mat4CameraMatrix;
}
