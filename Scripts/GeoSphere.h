#pragma once
#include "GameObject.h"

class CGeoSphere : public CGameObject
{
public:
	static CShader* m_normalShader;

	CGeoSphere();

	virtual void Draw() override;
};

