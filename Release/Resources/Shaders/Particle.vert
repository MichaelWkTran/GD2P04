#version 430 core

layout (location = 0) in vec3 in_v3Position;
out vec4 vs_v4Position;

void main()
{
	gl_Position = vec4(in_v3Position, 1.0f);
}