/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.Frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

layout (location = 0) out vec4 fs_gPosition;
layout (location = 1) out vec4 fs_gNormal;
layout (location = 2) out vec4 fs_gAlbedoSpec;
out vec4 fs_colour;

in vec3 vs_v3Position;
in vec3 vs_v3Normal;
in vec2 vs_v2TextureCoord;

//Unique Uniforms---------------------------------------------------------
uniform vec4 uni_v4Colour = vec4(1.0);
uniform float uni_fSpecularStrength = 0.3f;

//Texture Uniforms
uniform sampler2D uni_samp2DDiffuse0;
uniform sampler2D uni_samp2DSpecular0;

void main()
{
	fs_gPosition = vec4(vs_v3Position, 1.0f);
	fs_gNormal = vec4(vs_v3Normal, 1.0f);

	fs_gAlbedoSpec = vec4(1.0f);
	//if (textureSize(uni_samp2DDiffuse0,  0) != vec2(1.0f, 1.0f)) fs_gAlbedoSpec.rgb = texture(uni_samp2DDiffuse0, vs_v2TextureCoord).rgb;
	//if (textureSize(uni_samp2DSpecular0, 0) != vec2(1.0f, 1.0f)) fs_gAlbedoSpec.a   = texture(uni_samp2DSpecular0, vs_v2TextureCoord).r;
}