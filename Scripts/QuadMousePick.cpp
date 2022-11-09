/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: QuadMousePick.cpp																								*/
/*  DATE: Aug 30th, 2022																								*/
/************************************************************************************************************************/

#include "QuadMousePick.h"
#include "GenerateMesh.h"
#include "Main.h"

CQuadMousePick::CQuadMousePick(glm::vec2 _v2Size)
{
	m_v2Size = _v2Size;
	//gm::GeneratePlane(m_Mesh, glm::vec3(m_v2Size.x, m_v2Size.y, 0.0f), false);
}

void CQuadMousePick::Update()
{

}
