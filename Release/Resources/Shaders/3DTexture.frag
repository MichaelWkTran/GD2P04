/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: 3DTexture.frag																								*/
/*  DATE: Aug 25th, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

in vec3 vs_v3Position;

uniform sampler3D uni_samp3DDiffuse0;
uniform vec3 uni_v3CameraPosition;
uniform vec3 uni_v3FogColour = vec3(0.0f, 0.0f, 0.0f);
uniform float uni_fFogStart = 20.0f;
uniform float uni_fFogRange = 10.0f;

vec4 MixFogColour(vec4 _v4Colour)
{
	float fCameraDistance = distance(vs_v3Position, uni_v3CameraPosition);

	float fFogContribution = (fCameraDistance - uni_fFogStart)/uni_fFogRange;
	fFogContribution = clamp(fFogContribution, 0.0, 1.0);

	return mix(_v4Colour, vec4(uni_v3FogColour, 1.0f), fFogContribution);
}

void main()
{
	fs_v4Colour = texture(uni_samp3DDiffuse0, (vs_v3Position/2.0f) + 0.5f);
	fs_v4Colour = vec4(MixFogColour(fs_v4Colour).rgb, 1.0f);
}