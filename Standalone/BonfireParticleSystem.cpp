#include "BonfireParticleSystem.h"
#include "Main.h"
#include <Shader.h>
#include <MathUtils.h>
#include <Camera.h>
#include <Texture.h>
#include <glm/gtc/type_ptr.hpp>

CBonfireParticleSystem* CBonfireParticleSystem::CParticle::m_particleSystem = nullptr;

#pragma region Particle System

CBonfireParticleSystem::CBonfireParticleSystem()
{
	CParticle::m_particleSystem = this;
	m_visible = true;
	m_position = {};
	m_particleShader = new CShader("Particle.vert", "Particle.geom", "Particle.frag");

	//Create Fire Particles
	m_fireParticleCount = 100;
	m_fireEmmisionRate = 1.0f;
	m_fireTexture = new CTexture("FireParticle.png", GL_RGBA);

	for (unsigned particleIndex = 0U; particleIndex < m_fireParticleCount; particleIndex++)
		m_fireParticles.push_back(CFireParticle());
	m_firePositions.resize(m_fireParticleCount);
	
	//Create Smoke Particles
	m_smokeParticleCount = 50;
	m_smokeEmmisionRate = 2.0f;
	m_smokeTexture = new CTexture("SmokeParticle.png", GL_RGBA);

	for (unsigned particleIndex = 0U; particleIndex < m_smokeParticleCount; particleIndex++)
		m_smokeParticles.push_back(CSmokeParticle());
	m_smokePositions.resize(m_smokeParticleCount);

	//Create Mesh Buffers
	glGenVertexArrays(1, &m_vertexArray);
	glGenBuffers(1, &m_vertexBuffer);

	//Assign Vertex Attributes
	glBindVertexArray(m_vertexArray); glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0 /*Position*/, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);
	glBindVertexArray(0); glBindBuffer(GL_ARRAY_BUFFER, 0);
}

CBonfireParticleSystem::~CBonfireParticleSystem()
{
	glDeleteVertexArrays(1, &m_vertexArray);
	glDeleteBuffers(1, &m_vertexBuffer);
}

void CBonfireParticleSystem::Update()
{
	if (!m_visible) return;

	//Update particles and get their position data
	for (int i = 0; i < (int)m_fireParticleCount; i++)
	{
		m_fireParticles[i].Update();
		m_firePositions[i] = glm::vec4(m_fireParticles[i].m_position, m_fireParticles[i].m_elapsedTime / m_fireEmmisionRate);
	}

	for (int i = 0; i < (int)m_smokeParticleCount; i++)
	{
		m_smokeParticles[i].Update();
		m_smokePositions[i] = glm::vec4(m_smokeParticles[i].m_position, m_smokeParticles[i].m_elapsedTime / m_smokeEmmisionRate);
	}
}

void CBonfireParticleSystem::Draw()
{
	if (!m_visible) return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	
	glUseProgram(*m_particleShader);

	//Assign Uniforms
	glUniformMatrix4fv(glGetUniformLocation(*m_particleShader, "uni_cameraMatrix"), 1, GL_FALSE, glm::value_ptr(GetMainCamera().GetCameraMatrix()));

	glUniform3f(glGetUniformLocation(*m_particleShader, "uni_cameraUp"),
		GetMainCamera().m_transform.Up().x, GetMainCamera().m_transform.Up().y, GetMainCamera().m_transform.Up().z);

	glUniform3f(glGetUniformLocation(*m_particleShader, "uni_cameraRight"),
		GetMainCamera().m_transform.Right().x, GetMainCamera().m_transform.Right().y, GetMainCamera().m_transform.Right().z);

	//Draw fire particles
	{
		//Buffer Vertex Data
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_fireParticleCount * sizeof(glm::vec4), m_firePositions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Set Texture
		glActiveTexture(GL_TEXTURE0);
		m_fireTexture->Bind();
		glUniform1i(glGetUniformLocation(*m_particleShader, "uni_texture"), 0);

		//Draw Particles
		glBindVertexArray(m_vertexArray);
		glDrawArrays(GL_POINTS, 0, m_fireParticleCount);
		glBindVertexArray(0);
	}

	//Draw smoke particles
	{
		//Buffer Vertex Data
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, m_smokeParticleCount * sizeof(glm::vec4), m_smokePositions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Set Texture
		glActiveTexture(GL_TEXTURE0);
		m_smokeTexture->Bind();
		glUniform1i(glGetUniformLocation(*m_particleShader, "uni_texture"), 0);

		//Draw Particles
		glBindVertexArray(m_vertexArray);
		glDrawArrays(GL_POINTS, 0, m_smokeParticleCount);
		glBindVertexArray(0);
	}
	
	CTexture::Unbind();
	glUseProgram(0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

#pragma endregion

#pragma region Fire Particle

CBonfireParticleSystem::CFireParticle::CFireParticle()
{
	m_elapsedTime = ((float)rand() / RAND_MAX) * m_particleSystem->m_fireEmmisionRate;
	ResetToInitialValues();
}

void CBonfireParticleSystem::CFireParticle::ResetToInitialValues()
{
	while (m_elapsedTime < 0) m_elapsedTime += m_particleSystem->m_fireEmmisionRate;
	m_position = m_particleSystem->m_position;

	float minSpreadSpeed = 0.5f; float maxSpreadSpeed = 1.0f;
	float minRiseSpeed = 2.0f; float maxRiseSpeed = 4.0f;

	m_velocity = glm::vec3(
		cosf(2.0f * glm::pi<float>() * glm::Lerp(minSpreadSpeed, maxSpreadSpeed, (float)rand() / RAND_MAX)),
		glm::Lerp(minRiseSpeed, maxRiseSpeed, (float)rand() / RAND_MAX),
		sinf(2.0f * glm::pi<float>() * glm::Lerp(minSpreadSpeed, maxSpreadSpeed, (float)rand() / RAND_MAX))
	);
}

void CBonfireParticleSystem::CFireParticle::Update()
{
	m_velocity.y += -1.0f * e_deltaTime;
	m_position += m_velocity * e_deltaTime;

	//Reset the particle if its elapsed time has finished
	m_elapsedTime -= e_deltaTime;
	if (m_elapsedTime <= 0.0f) ResetToInitialValues();
}

#pragma endregion

#pragma region Smoke Particle

CBonfireParticleSystem::CSmokeParticle::CSmokeParticle()
{
	m_elapsedTime = ((float)rand() / RAND_MAX) * m_particleSystem->m_smokeEmmisionRate;
	ResetToInitialValues();
}

void CBonfireParticleSystem::CSmokeParticle::ResetToInitialValues()
{
	while (m_elapsedTime < 0) m_elapsedTime += m_particleSystem->m_smokeEmmisionRate;
	m_position = m_particleSystem->m_position;

	float minSpreadSpeed = 1.0f; float maxSpreadSpeed = 2.0f;
	float minRiseSpeed = 3.0f; float maxRiseSpeed = 4.0f;
	
	m_velocity = glm::vec3(
		cosf(2.0f * glm::pi<float>() * glm::Lerp(minSpreadSpeed, maxSpreadSpeed, (float)rand() / RAND_MAX)),
		glm::Lerp(minRiseSpeed, maxRiseSpeed, (float)rand() / RAND_MAX),
		sinf(2.0f * glm::pi<float>() * glm::Lerp(minSpreadSpeed, maxSpreadSpeed, (float)rand() / RAND_MAX))
	);
}

void CBonfireParticleSystem::CSmokeParticle::Update()
{
	m_velocity.y += -1.0f * e_deltaTime;
	m_position += m_velocity * e_deltaTime;

	//Reset the particle if its elapsed time has finished
	m_elapsedTime -= e_deltaTime;
	if (m_elapsedTime <= 0.0f) ResetToInitialValues();
}

#pragma endregion