/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: QuadMousePick.h																								*/
/*  DATE: Aug 30th, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "GameObject.h"

class CQuadMousePick : CGameObject
{
private:
	glm::vec2 m_v2Size;

public:
	bool m_bIsIntersecting;

	CQuadMousePick(glm::vec2 _v2Size);

	void Update() override;
};