/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GameObject.h																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "UpdatedObject.h"
#include "Transform.h"
#include "Mesh.h"
#include <set>

class CGameObject : public CUpdatedObject
{
protected:
	//Rendering variables
	CBaseMesh* m_mesh;

public:
	//Refrencing variables
	std::set<const char*> m_tags;

	//Transformation variables
	CTransform m_transform;
	
	//Rendering variables
	bool m_visible;

	//Methods
	CGameObject();
	~CGameObject();
	const bool TagExists(const char* _tagName);
	const CBaseMesh* GetMesh() const;
	virtual void Draw() override;
};