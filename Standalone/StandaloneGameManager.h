/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: StandaloneGameManager.h																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GameManager.h>
#include <Mesh.h>
#include <map>

class CTexture;
class CBonfireParticleSystem;
class CGPUParticleSystem;

class CStandaloneGameManager : public CGameManager
{
public:
	std::map<const char* /*Shader name*/, CShader*> m_shaders;
	CBonfireParticleSystem* m_particleSystem;
	CGPUParticleSystem* m_gpuParticleSystem;

	CStandaloneGameManager();
	void Update() override;
};

