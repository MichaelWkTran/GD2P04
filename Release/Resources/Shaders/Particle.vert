#version 430 core

layout (location = 0) in vec4 in_position;
out float vs_life;

void main()
{
	vs_life = in_position.w;
	gl_Position = vec4(in_position.xyz, 1.0f);
}