#version 430 core

layout (location = 0) in vec3 in_v3Position;

out vec4 vs_v4Position;

uniform mat4 uni_mat4CameraMatrix;
uniform mat4 uni_mat4Model;

void main()
{
	gl_Position = uni_mat4CameraMatrix * uni_mat4Model * vec4(in_v3Position, 1.0f);
	vs_v4Position = gl_Position;
}