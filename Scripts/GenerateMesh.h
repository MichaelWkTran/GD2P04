/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GenerateMesh.h																								*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "Mesh.h"

namespace gm
{
	void GenerateCube(CMesh<>& _Mesh, glm::vec3 _v3Scale = glm::vec3(1.0f, 1.0f, 1.0f));
	void GenerateFlatCube(CMesh<>& _Mesh, glm::vec3 _v3Scale = glm::vec3(1.0f,1.0f,1.0f));
	void GeneratePlane(CMesh<>& _Mesh, glm::vec3 _v3Scale = glm::vec3(1.0f, 1.0f, 1.0f));
	
	//void GenerateGrid(CMesh<>& _Mesh, unsigned int _uiWidth, unsigned int _uiHeight, glm::vec3 _v3Scale = glm::vec3(1.0f, 1.0f, 1.0f));
	void GenerateSphere(CMesh<>& _Mesh, const float _fRadius, const int _iFidelity);
}