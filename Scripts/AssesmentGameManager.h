/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: AssesmentGameManager.h																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "GameManager.h"
#include "Mesh.h"
#include <map>

class CTexture;

enum class FrameBufferEffect
{
	None,
	Rain,
	ChromaticAberration,
	CRT
};

class CAssesmentGameManager : public CGameManager
{
private:
	unsigned int m_frameBuffer;
	unsigned int m_renderBuffer;
	CMesh<> m_renderQuad;

public:
	std::map<const char* /*Shader name*/, CShader*> m_shaders;
	FrameBufferEffect m_frameBufferEffect;

	CAssesmentGameManager();
	~CAssesmentGameManager();
	void Update() override;
};

