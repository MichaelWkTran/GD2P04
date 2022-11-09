/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.vert																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

const int MAX_JOINTS = 100;//max joints allowed in a skeleton
const int MAX_WEIGHTS = 4;//max number of joints that can affect a vertex

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 boneIds;
layout (location = 4) in vec4 weights;

out vec2 vs_v2TextureCoord;
out vec3 vs_v3Normal;
out vec3 vs_v3Position;
uniform mat4 vp;
uniform mat4 model;
uniform mat4 jointTransforms[MAX_JOINTS];

void main(void)
{
	vec4 totalLocalPosition = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	for(int i = 0; i < MAX_WEIGHTS; i++)
	{
		totalLocalPosition += (jointTransforms[boneIds[i]] * vec4(position, 1.0)) * weights[i];
		totalNormal += 		  (jointTransforms[boneIds[i]] * vec4(normal,   1.0)) * weights[i];
	}
	gl_Position = vp * model * totalLocalPosition;
	
	vs_v2TextureCoord = texCoord;
	vs_v3Position = vec3(model * vec4(position, 1.0f));
	vs_v3Normal = normalize(mat3(transpose(inverse(model))) * normal);
}