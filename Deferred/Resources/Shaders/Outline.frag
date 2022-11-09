/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Unlit.frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

in vec3 vs_v3Position;

uniform vec3 uni_v3Colour = vec3(1.0f, 1.0f, 1.0f);
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
	fs_v4Colour = vec4(uni_v3Colour, 1.0f);
	fs_v4Colour = MixFogColour(fs_v4Colour);
}