// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2022 Media Design School
//
// File Name: MathUtils.cpp
// Description: Math utils declaration file
// Authors: Michael Wai Kit Tran

#include "MathUtils.h"
#include "assimp/matrix4x4.h"
#include <glm/gtc/constants.hpp>

using namespace std;

const float glm::Lerp(const float _a, const float _b, const float _t)
{
	return _a + ((_b - _a) * _t);
}

const float glm::CosineInterpolate(const float _a, const float _b, const float _t)
{
	return Lerp(_a, _b, (1.0f - cosf(_t * pi<float>())) / 2.0f);
}

const glm::mat4 glm::mat4_cast(const aiMatrix4x4 _value)
{
	https://github.com/mackron/GTGameEngine/blob/master/source/ModelDefinition_Assimp.cpp
	glm::mat4 converted(0);
	
	converted[0][0] = _value.a1; converted[1][0] = _value.a2; converted[2][0] = _value.a3; converted[3][0] = _value.a4;
	converted[0][1] = _value.b1; converted[1][1] = _value.b2; converted[2][1] = _value.b3; converted[3][1] = _value.b4;
	converted[0][2] = _value.c1; converted[1][2] = _value.c2; converted[2][2] = _value.c3; converted[3][2] = _value.c4;
	converted[0][3] = _value.d1; converted[1][3] = _value.d2; converted[2][3] = _value.d3; converted[3][3] = _value.d4;

	return converted;
}