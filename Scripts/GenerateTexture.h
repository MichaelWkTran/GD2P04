/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GenerateTexture.cpp																							*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

namespace gt
{
	float Noise(float _x, float _y, int _iSeed = 0);
	float PerlinNoise(float _x, float _y, unsigned int _uiOctaves = 4, float _fZoom = 64.0f, float _fPersistance = 0.5f, int _iSeed = 0);
}