/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Diffuse.Frag																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#version 430 core

out vec4 fs_v4Colour;

uniform sampler2D renderTexture;
in vec2 vs_v2TextureCoord;
uniform float iTime;

vec2 iResolution = vec2(800.0f, 800.0f);

void main()
{
    vec2 uv = gl_FragCoord.xy / iResolution.xy;

	float amount = 0.0;
	
	amount = (1.0 + sin(iTime*6.0)) * 0.5;
	amount *= 1.0 + sin(iTime*16.0) * 0.5;
	amount *= 1.0 + sin(iTime*19.0) * 0.5;
	amount *= 1.0 + sin(iTime*27.0) * 0.5;
	amount = pow(amount, 3.0);

	amount *= 0.05;
	
    vec3 col;
    col.r = texture( renderTexture, vec2(uv.x+amount,uv.y) ).r;
    col.g = texture( renderTexture, uv ).g;
    col.b = texture( renderTexture, vec2(uv.x-amount,uv.y) ).b;

	col *= (1.0 - amount * 0.5);
	
    fs_v4Colour = vec4(col,1.0);
}