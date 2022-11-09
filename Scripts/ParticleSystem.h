#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "UpdatedObject.h"
#include "Mesh.h"

class CTexture;
class CShader;
class CParticle;

class CParticleSystem : public CUpdatedObject
{
	friend CParticle;

private:
	static CShader* m_particleShader;
	static CTexture* m_particleTexture;
	unsigned int m_particleCount;
	std::vector<CParticle> m_particles;

	//Mesh Properties
	std::vector<glm::vec3> m_particlePositions;
	unsigned int m_vertexArray;
	unsigned int m_vertexBuffer;

public:
	glm::vec3 m_position;
	
	CParticleSystem(unsigned int _particleCount = 4000U);
	~CParticleSystem();

	virtual void Update() override;
	virtual void Draw() override;
};

class CParticle
{
public:
	CParticleSystem* m_particleSystem;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_elapsedTime;

	CParticle(CParticleSystem* _particleSystem);
	void ResetToInitialValues();
	void Update();
};