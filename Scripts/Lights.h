/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: LightManager.h																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <set>

class CShader;
class CTexture;

class CLight
{
private:
	static CShader* m_shadowMapShader;
	static std::set<CLight*> m_lightsInWorld;

protected:
	unsigned int m_frameBuffer;
	glm::mat4 m_projectionMatrix;
	bool m_updateProjectionMatrix;

	virtual void UpdateProjectionMatrix() = 0;

public:
	CTexture* m_shadowMapTexture;
	static glm::vec4 m_ambientColour;
	static void UpdateLightUniforms(CShader& _shader);
	static void UpdateShadowUniforms(CShader& _shader, unsigned int _slot = 10);
	static void UpdateShadowMaps();
	static const std::set<CLight*>& GetLightsInWorld() { return m_lightsInWorld; }
	static CShader*& GetShadowMapShader();

	glm::vec4 m_lightColour;
	
	//Methods
	CLight(glm::vec4 _lightColour = glm::vec4(1.0f));
	virtual ~CLight();

	const glm::mat4 GetProjectionMatrix()
	{
		return m_projectionMatrix;
	}
	void SetProjection(const glm::mat4 _projection)
	{
		m_projectionMatrix = _projection;
		m_updateProjectionMatrix = true;
	}
	unsigned int GetFrameBuffer()
	{
		return m_frameBuffer;
	}
};

class CInfinitePointLight : public CLight
{
private:
	glm::vec3 m_position;

protected:
	virtual void UpdateProjectionMatrix() override
	{
		
	}

public:
	CInfinitePointLight(glm::vec3 _position = {}, glm::vec4 _lightColour = glm::vec4(1.0f)) : CLight(_lightColour)
	{
		m_position = _position;
	};

	const glm::vec3 GetPosition()
	{
		return m_position;
	}
	const void SetPosition(glm::vec3 _v3Position)
	{
		m_position = _v3Position;
		m_updateProjectionMatrix = true;
	}
};

class CPointLight : public CLight
{
private:
	glm::vec3 m_position;

protected:
	virtual void UpdateProjectionMatrix() override
	{

	}

public:
	float m_attenuationExponent;
	float m_attenuationLinear;
	float m_attenuationConstant;

	//Methods
	CPointLight
	(
		glm::vec3 _position = {}, glm::vec4 _lightColour = glm::vec4(1.0f),
		float _attenuationExponent = 3.0f,
		float _attenuationLinear = 0.7f,
		float _attenuationConstant = 1.0f
	) : CLight(_lightColour)
	{
		m_position = _position;
		m_attenuationExponent = 3.0f;
		m_attenuationLinear = 0.7f;
		m_attenuationConstant = 1.0f;
	}

	const glm::vec3 GetPosition()
	{
		return m_position;
	}
	const void SetPosition(glm::vec3 _position)
	{
		m_position = _position;
		m_updateProjectionMatrix = true;
	}
};

class CDirectionalLight : public CLight
{
private:
	glm::vec3 m_lightDirection;
	
protected:
	virtual void UpdateProjectionMatrix() override;

public:
	CDirectionalLight(glm::vec4 _lightColour = glm::vec4(1.0f), glm::vec3 _lightDirection = glm::vec3(-1.0f,-1.0f,1.0f))
		: CLight(_lightColour)
	{
		m_lightDirection = _lightDirection;
	}

	const glm::vec3 GetLightDirection()
	{
		return m_lightDirection;
	}
	void SetLightDirection(const glm::vec3 _lightDirection)
	{
		m_lightDirection = _lightDirection;
		m_updateProjectionMatrix = true;
	}
};

class CSpotLight : public CLight
{
private:
	glm::vec3 m_position;
	glm::vec3 m_lightDirection;

protected:
	virtual void UpdateProjectionMatrix() override
	{

	}

public:
	float m_outerCone;
	float m_innerCone;

	//Methods
	CSpotLight
	(
		glm::vec3 _position = {},
		glm::vec4 _lightColour = glm::vec4(1.0f),
		glm::vec3 _lightDirection = glm::vec3(0.0f, -1.0f, 0.0f),
		float _outerCone = 0.9f, float _innerCone = 0.95f
	) : CLight(_lightColour)
	{
		m_position = _position;
		m_lightDirection = _lightDirection;
		m_outerCone = _outerCone;
		m_innerCone = _innerCone;
	}

	const glm::vec3 GetPosition()
	{
		return m_position;
	}
	const void SetPosition(glm::vec3 _position)
	{
		m_position = _position;
		m_updateProjectionMatrix = true;
	}
	const glm::vec3 GetLightDirection()
	{
		return m_lightDirection;
	}
	void SetLightDirection(const glm::vec3 _lightDirection)
	{
		m_lightDirection = _lightDirection;
		m_updateProjectionMatrix = true;
	}
};