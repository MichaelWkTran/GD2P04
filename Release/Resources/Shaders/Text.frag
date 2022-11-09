/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Text.frag																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 FragColor;

in vec2 m_v2TextureCoord; 

uniform sampler2D uni_samp2DTextTexture;
uniform vec3 uni_v3Color; 

out vec4 FinalColor; 

void main()
{
	float fAlpha = texture(uni_samp2DTextTexture, m_v2TextureCoord).r;
	FragColor = vec4(uni_v3Color, fAlpha); 
}