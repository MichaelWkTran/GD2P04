#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <UpdatedObject.h>
#include <Mesh.h>

class CTexture;
class CShader;

class CBonfireParticleSystem : public CUpdatedObject
{
	class CFireParticle;
	class CSmokeParticle;
	
private:
	unsigned int m_fireParticleCount;
	unsigned int m_smokeParticleCount;
	std::vector<CFireParticle> m_fireParticles;
	std::vector<CSmokeParticle> m_smokeParticles;
	
	//Mesh Properties
	std::vector<glm::vec4> m_firePositions;
	std::vector<glm::vec4> m_smokePositions;
	unsigned int m_vertexArray;
	unsigned int m_vertexBuffer;

	//Texture shader properties
	CShader* m_particleShader;
	CTexture* m_fireTexture;
	CTexture* m_smokeTexture;

	//Classes
	class CParticle
	{
	public:
		static CBonfireParticleSystem* m_particleSystem;
		
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		float m_elapsedTime;

		virtual void ResetToInitialValues() = 0;
		virtual void Update() = 0;
	};

	class CFireParticle : public CParticle
	{
	public:
		CFireParticle();
		void ResetToInitialValues() override;
		void Update() override;
	};

	class CSmokeParticle : public CParticle
	{
	public:
		CSmokeParticle();
		void ResetToInitialValues() override;
		void Update() override;
	};

public:
	bool m_visible;
	glm::vec3 m_position;
	float m_smokeEmmisionRate;
	float m_fireEmmisionRate;

	CBonfireParticleSystem();
	~CBonfireParticleSystem();

	virtual void Update() override;
	virtual void Draw() override;
};