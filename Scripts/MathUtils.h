// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
// (c) 2022 Media Design School
//
// File Name: MathUtils.h
// Description: Math utils initialization file
// Authors: Michael Wai Kit Tran

#pragma once
#include <cmath>
#include <glm/glm.hpp>

template<typename TReal>
class aiMatrix4x4t;
typedef aiMatrix4x4t<float> aiMatrix4x4;

namespace glm
{
	template<typename T, qualifier Q>
	struct mat<3, 3, T, Q>;
	typedef mat<4, 4, f32, defaultp> mat4;

	const float Lerp(const float _a, const float _b, const float _t);
	const float CosineInterpolate(const float _a, const float _b, const float _t);
	const mat4 mat4_cast(const aiMatrix4x4 _value);
}