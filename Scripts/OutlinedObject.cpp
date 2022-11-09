/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: OutlinedObject.cpp																							*/
/*  DATE: Aug 30th, 2022																								*/
/************************************************************************************************************************/

#include "OutlinedObject.h"
#include "Camera.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AssesmentGameManager.h"
#include "Main.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: COutlinedObject()
//	 Purpose: Initalises the variables of the outlined object.
//			  _fOutLineThickness is the thickness of the outline rendered around the object.
//			  _v3OutlineColour is the colour of that outline. 

COutlinedObject::COutlinedObject(float _fOutLineThickness, glm::vec3 _v3OutlineColour)
{
	m_fOutLineThickness = _fOutLineThickness;
	m_v3OutlineColour = _v3OutlineColour;
	m_fRotationSpeed = 10.0f;
}


//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: Used to rotate the object every frame on its y axis. 

void COutlinedObject::Update()
{
	m_Transform.RotateEuler(glm::vec3(0.0f, m_fRotationSpeed * e_fDeltatime, 0.0f));
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Draw()
//	 Purpose: Draws the object's mesh as well as an outline if m_bStencilTestEnabled is enabled in CAssesmentGameManager

void COutlinedObject::Draw()
{
	CAssesmentGameManager* pGameManager = dynamic_cast<CAssesmentGameManager*>(&GetGameManager());
	bool bEnableStencil = (pGameManager == nullptr) ? true : pGameManager->m_bStencilTestEnabled;

	if (bEnableStencil)
	{
		//Enable stencil and set stencil operation
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		//1st pass - Set current stencil value
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}
	
	//Draw regular sized mesh
	m_Mesh.Draw(GetMainCamera());

	if (bEnableStencil)
	{
		//2nd pass
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		//Set Outline Uniforms
		CShader* pOutlineShader = CShader::Find("Outline");
		if (pOutlineShader == nullptr) pOutlineShader = new CShader("Outline", "Outline.vert", "Outline.frag", "", [](CShader& _Shader) { _Shader.Uniform3f("uni_v3Colour", 1.0f, 1.0f, 1.0f); });

		glm::mat4 mat4Model = glm::scale(m_Transform.GetModel(), (1.0f + m_fOutLineThickness) * glm::vec3(1.0f, 1.0f, 1.0f));
		pOutlineShader->UniformMatrix4fv("uni_mat4Model", 1, GL_FALSE, mat4Model);
		pOutlineShader->UniformMatrix4fv("uni_mat4CameraMatrix", 1, GL_FALSE, GetMainCamera().GetCameraMatrix());
		pOutlineShader->Uniform3f("uni_v3Colour", m_v3OutlineColour);
		pOutlineShader->Uniform3f("uni_v3CameraPosition", GetMainCamera().m_Transform.GetPosition());
		//Draw outline
		m_Mesh.BindVertexArray();
		pOutlineShader->Activate();
		m_Mesh.m_pDrawMethod(m_Mesh);
		pOutlineShader->Deactivate();
		m_Mesh.UnbindVertexArray();
	}

	//Disable stencil test
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
}