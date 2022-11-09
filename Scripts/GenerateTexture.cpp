/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GenerateTexture.cpp																							*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

#include "GenerateTexture.h"
#include "MathUtils.h"

float Smooth(float, float, int);
float SmoothInterpolate(float, float, int);

#pragma region gt namespace methods

float gt::Noise(float _x, float _y, int _seed)
{
	//(n << 13) is the same thing as (n * 2^13)
	//the (x & 7fffffff) operation is equivaLent to (x mod 2147483648) 
	
	int noise = _x + _y * _seed; 
	noise = (noise << 13) ^ noise; 

	int t = (noise * (noise * noise * 15731 + 789221) + 1376312589) & 0x7fffffff;
	double dValue = 1.0 - double(t) * 0.931322574615478515625e-9;
	
	return (float)dValue;
}

float gt::PerlinNoise(float _x, float _y, unsigned int _octaves, float _zoom, float _persistance, int _seed)
{
	float total = 0.0f;

	for (unsigned int iterations = 0; iterations < _octaves - 1U; iterations++)
	{
		float frequency = powf(2.0f, (float)iterations) / _zoom;
		float amplitude = powf(_persistance, (float)iterations);

		total += SmoothInterpolate(_x * frequency, _y * frequency, _seed) * amplitude;
	}

	return total;
}

#pragma endregion

float Smooth(float _x, float _y, int _seed)
{
	float corners = gt::Noise(_x-1.0f, _y-1.0f, _seed) + gt::Noise(_x+1.0f, _y-1.0f, _seed) + gt::Noise(_x-1.0f, _y+1.0f, _seed) + gt::Noise(_x+1.0f, _y+1.0f, _seed); corners /= 16.0f;
	float sides = gt::Noise(_x-1.0f, _y, _seed) + gt::Noise(_x+1.0f, _y,	_seed) + gt::Noise(_x, _y-1.0f, _seed) + gt::Noise(_x , _y+1.0f, _seed); sides /= 8.0f;
	float centre = gt::Noise(_x, _y, _seed) / 4.0f;

	return corners + sides + centre;
}

float SmoothInterpolate(float _x, float _y, int _iSeed)
{
	float fractX = _x - int(_x);
	float fFractY = _y - int(_y);
	
	//Smooths
	float v1 = Smooth(int(_x),		  int(_y),		  _iSeed);
	float v2 = Smooth(int(_x) + 1.0f, int(_y),		  _iSeed);
	
	float v3 = Smooth(int(_x),		  int(_y) + 1.0f, _iSeed);
	float v4 = Smooth(int(_x) + 1.0f, int(_y) + 1.0f, _iSeed);
	
	//Interpolates
	float i1 = glm::Lerp(v1, v2, fractX);
	float i2 = glm::Lerp(v3, v4, fractX);
	
	return glm::Lerp(i1, i2, fFractY);
}