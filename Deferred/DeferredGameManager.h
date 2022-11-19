/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: DeferredGameManager.h																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GameManager.h>
#include <Mesh.h>
#include <map>

class CTexture;

class CDeferredGameManager : public CGameManager
{
private:
	unsigned int m_gBuffer;
	CMesh<> m_renderQuad;

public:
	std::map<const char* /*Shader name*/, CShader*> m_shaders;

	CDeferredGameManager();
	void Update() override;
};

