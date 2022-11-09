/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GameObject.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "GameObject.h"
#include "Camera.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GameObject()
//	 Purpose: To initalise variables of the game object

CGameObject::CGameObject()
{
	m_mesh = nullptr;
	m_visible = true;
}

CGameObject::~CGameObject()
{
	if (m_mesh == nullptr) delete m_mesh;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: TagExists()
//	 Purpose: Check whether the tag, _pTagName, exists in m_setTags 
//	 Returns: Whether the tag, _pTagName, exists in in m_setTags

const bool CGameObject::TagExists(const char* _tagName)
{
	if (m_tags.find(_tagName) != m_tags.end()) return true;
	else return false;
}

const CBaseMesh* CGameObject::GetMesh() const
{
	return m_mesh;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Draw()
//	 Purpose: To draw the mesh stored in the game object

void CGameObject::Draw()
{
	CMesh<>* mesh = (CMesh<>*)m_mesh;
	mesh->m_transform = m_transform;
	mesh->Draw(GetMainCamera());
}