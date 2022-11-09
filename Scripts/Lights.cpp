/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: LightManager.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "Lights.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Main.h"
#include "GameManager.h"
#include "GameObject.h"
#include <queue>

CShader* CLight::m_shadowMapShader = nullptr;
std::set<CLight*> CLight::m_lightsInWorld;
glm::vec4 CLight::m_ambientColour = glm::vec4(1.0f, 1.0f, 1.0f, 0.2f);

void CLight::UpdateLightUniforms(CShader& _shader)
{
	unsigned int infinitePointLightIndex = 0U;
	unsigned int pointLightIndex		 = 0U;
	unsigned int directionalLightIndex   = 0U;
	unsigned int spotLightIndex			 = 0U;

	//Set light type uniforms
	for (auto& light : m_lightsInWorld)
	{
		std::string uniformStructName = "";

		if (auto object = dynamic_cast<CInfinitePointLight*>(light))
		{
			uniformStructName = std::string("uni_InfinitePointLight") + '['+std::to_string(infinitePointLightIndex)+']';

			//Set the light uniforms
			_shader.Uniform3f(uniformStructName + ".v3LightPosition", object->GetPosition());
			
			//Update light index
			infinitePointLightIndex++;
		}
		else if (auto object = dynamic_cast<CPointLight*>(light))
		{
			uniformStructName = std::string("uni_PointLight") + '[' + std::to_string(infinitePointLightIndex) + ']';

			//Set the light uniforms
			_shader.Uniform3f(uniformStructName + ".v3LightPosition", object->GetPosition());
			_shader.Uniform1f(uniformStructName + ".fAttenuationExponent", object->m_attenuationExponent);
			_shader.Uniform1f(uniformStructName + ".fAttenuationLinear", object->m_attenuationLinear);
			_shader.Uniform1f(uniformStructName + ".fAttenuationConstant", object->m_attenuationConstant);

			//Update light index
			pointLightIndex++;
		}
		else if (auto object = dynamic_cast<CDirectionalLight*>(light))
		{
			uniformStructName = std::string("uni_DirectionalLight")  + '[' + std::to_string(infinitePointLightIndex) + ']';

			//Set the light uniforms
			_shader.Uniform3f(uniformStructName + ".v3LightDirection", object->GetLightDirection());

			//Update light index
			directionalLightIndex++;
		}
		else if (auto object = dynamic_cast<CSpotLight*>(light))
		{
			uniformStructName = std::string("uni_SpotLight") + '[' + std::to_string(infinitePointLightIndex) + ']';

			//Set the light uniforms
			_shader.Uniform3f(uniformStructName + ".v3LightPosition", object->GetPosition());
			_shader.Uniform3f(uniformStructName + ".v3LightDirection", object->GetLightDirection());
			_shader.Uniform1f(uniformStructName + ".fOuterCone", object->m_outerCone);
			_shader.Uniform1f(uniformStructName + ".fInnerCone", object->m_innerCone);

			//Update light index
			spotLightIndex++;
		}
		else
		{
			continue;
		}
		
		//Set CLight uniforms
		_shader.Uniform4f(uniformStructName + ".v4LightColour", light->m_lightColour);
	}

	//Set the uniforms for how may lights are in the world for each type
	_shader.Uniform1i("uni_iInfinitePointLightNum", infinitePointLightIndex);
	_shader.Uniform1i("uni_iPointLightNum", pointLightIndex);
	_shader.Uniform1i("uni_iDirectionalLightNum", directionalLightIndex);
	_shader.Uniform1i("uni_iSpotLightNum", spotLightIndex);

	//Set uni_v4AmbientColour
	_shader.Uniform4f("uni_v4AmbientColour", m_ambientColour);
}

void CLight::UpdateShadowUniforms(CShader& _shader, unsigned int _uiSlot)
{
	if (m_lightsInWorld.size() <= 0U) return;

	unsigned int infinitePointLightIndex = 0U;
	unsigned int pointLightIndex = 0U;
	unsigned int directionalLightIndex = 0U;
	unsigned int spotLightIndex = 0U;

	for (auto& pLight : m_lightsInWorld)
	{
		//Get Uniform Struct name
		std::string uniformStructName = "";
		if (auto object = dynamic_cast<CInfinitePointLight*>(pLight))
		{
			uniformStructName = std::string("uni_InfinitePointLight") + '[' + std::to_string(infinitePointLightIndex) + ']';
			infinitePointLightIndex++;
		}
		else if (auto object = dynamic_cast<CPointLight*>(pLight))
		{
			uniformStructName = std::string("uni_PointLight") + '[' + std::to_string(infinitePointLightIndex) + ']';
			pointLightIndex++;
		}
		else if (auto object = dynamic_cast<CDirectionalLight*>(pLight))
		{
			uniformStructName = std::string("uni_DirectionalLight") + '[' + std::to_string(infinitePointLightIndex) + ']';
			directionalLightIndex++;
		}
		else if (auto object = dynamic_cast<CSpotLight*>(pLight))
		{
			uniformStructName = std::string("uni_SpotLight") + '[' + std::to_string(infinitePointLightIndex) + ']';
			spotLightIndex++;
		}

		//Set Shadow Uniforms
		_shader.UniformMatrix4fv(uniformStructName + ".mat4VPMatrix", 1, GL_FALSE, pLight->GetProjectionMatrix());
		pLight->m_shadowMapTexture->Uniform(_shader, uniformStructName + ".samp2DShadowMap", _uiSlot);
	}
}

void CLight::UpdateShadowMaps()
{
	if (m_lightsInWorld.size() <= 0U) return;

	//Set the shader of the meshes creating shadows
	std::queue<CShader*> originalShaders;
	for (auto i = CBaseMesh::GetMeshesBegin(); i != CBaseMesh::GetMeshesEnd(); i++)
	{
		if ((*i)->m_shadowShader == nullptr) continue;

		//Store original shader and set the mesh shader as the shadow map shader
		originalShaders.emplace((*i)->m_shader);
		(*i)->m_shader = (*i)->m_shadowShader;
	}

	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	//Draw the light framebuffers
	for (auto& light : m_lightsInWorld)
	{
		//Update the projection matricies for all the lights
		light->UpdateProjectionMatrix();

		//Set light viewport and buffer
		glViewport(0, 0, e_viewPortW, e_viewPortH);
		glBindFramebuffer(GL_FRAMEBUFFER, light->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);

		//Set shadow map shader projection uniform
		GetShadowMapShader()->UniformMatrix4fv("uni_mat4LightProjection", 1, false, light->GetProjectionMatrix());

		//Draw scene
		for (auto i = CBaseMesh::GetMeshesBegin(); i != CBaseMesh::GetMeshesEnd(); i++)
		{
			if ((*i)->m_shadowShader == nullptr) continue;
			(*i)->Draw(GetMainCamera());
		}
	}

	//Unbind frame buffer and return viewport to original properties
	CTexture::Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0U);
	glViewport(0, 0, e_viewPortW, e_viewPortH);

	//Restore the original shader to the mesh
	for (auto i = CBaseMesh::GetMeshesBegin(); i != CBaseMesh::GetMeshesEnd(); i++)
	{
		if ((*i)->m_shadowShader == nullptr) continue;
		
		//Store original shader and set the mesh shader as the shadow map shader
		(*i)->m_shader = originalShaders.front();
		originalShaders.pop();
	}
}

CShader*& CLight::GetShadowMapShader()
{
	if (m_shadowMapShader == nullptr) m_shadowMapShader = new CShader("ShadowMap.vert", "Empty.frag");
	return m_shadowMapShader;
}

CLight::CLight(glm::vec4 _lightColour)
{
	m_lightsInWorld.emplace(this);
	
	//Initialise Light
	m_lightColour = _lightColour;
	m_projectionMatrix = glm::mat4(1.0f);
	m_updateProjectionMatrix = true;

	//Depth Texture
	m_shadowMapTexture = new CTexture();
	m_shadowMapTexture->Bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, e_viewPortW, e_viewPortH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float fClampColour[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, fClampColour);

	//Depth Frame Buffer
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *m_shadowMapTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_shadowMapTexture->Unbind();

	//Check Frame Buffer Validation
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) printf("FB error, status: 0x%x\n", status);
	
}

CLight::~CLight()
{
	delete m_shadowMapTexture;
	glDeleteFramebuffers(1U, &m_frameBuffer);
}

void CDirectionalLight::UpdateProjectionMatrix()
{
	//Check whether the projection matrix needed to be updated and update it
	if (m_updateProjectionMatrix == false) return;

	glm::mat4 projectionMatrix = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	glm::mat4 viewMatrix = glm::lookAt(20.0f * -m_lightDirection, {}, glm::vec3(0.0f, 1.0f, 0.0f));
	m_projectionMatrix = projectionMatrix * viewMatrix;
}
