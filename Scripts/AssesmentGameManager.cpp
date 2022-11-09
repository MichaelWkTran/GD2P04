/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: AssesmentGameManager.cpp																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

float g_time = 0;

#include "AssesmentGameManager.h"
#include "Camera.h" //For accessing the camera
#include "TinyObjectLoader.h" //For loading models
#include "Lights.h" //For adding lights to the scene
#include "Shader.h" //For creating shaders
#include "Texture.h" //For creating textures
#include "CubeSkybox.h" //For creating a skybox
#include "Main.h" //For accessing viewport and keyboard input
#include <iostream> //For printing messages to console
#include "GeoStar.h"
#include "GeoSphere.h"
#include "GenerateMesh.h"
#include "TessModel.h"
#include "ParticleSystem.h"
#include "ssAnimatedModel.h"
#include "MathUtils.h"
#include "GPUParticle.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CAssesmentGameManager()
//	 Purpose: To initalise variables and to setup the demo scene

CAssesmentGameManager::CAssesmentGameManager()
{
	//Set Key Inputs
	e_keyCallbackFunctions.emplace([](GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
	{
		CAssesmentGameManager* pGameManager = ((CAssesmentGameManager*)&GetGameManager());
		if (_action != GLFW_PRESS) return;

		switch (_key)
		{
		//Change Frame Buffer Effect
		case GLFW_KEY_1:
			pGameManager->m_frameBufferEffect = FrameBufferEffect::None;
			break;
		case GLFW_KEY_2:
			pGameManager->m_frameBufferEffect = FrameBufferEffect::Rain;
			break;
		case GLFW_KEY_3:
			pGameManager->m_frameBufferEffect = FrameBufferEffect::ChromaticAberration;
			break;
		case GLFW_KEY_4:
			pGameManager->m_frameBufferEffect = FrameBufferEffect::CRT;
			break;
		}
	});

	//Setup frame and render buffer
	{
		//Create frame buffer
		glGenFramebuffers(1, &m_frameBuffer);

		//Bind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

		//Create and attach texture to frame buffer
		CTexture* renderTexture = new CTexture();
		m_renderQuad.m_textures.emplace("renderTexture", renderTexture);
		renderTexture->Bind();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, e_viewPortW, e_viewPortH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *renderTexture, 0);

		renderTexture->Unbind();

		//Create Render buffer
		glGenRenderbuffers(1, &m_renderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);

		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, e_viewPortW, e_viewPortH);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR: Framebuffer is is not complete";

		//Unbind frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Setup render quad mesh 
		m_renderQuad.SetVerticies({
		stVertex{glm::vec3(-1.0f, -1.0f, 0.0f), {}, glm::vec2(0.0f, 0.0f)},
		stVertex{glm::vec3( 1.0f, -1.0f, 0.0f), {}, glm::vec2(1.0f, 0.0f)},
		stVertex{glm::vec3(-1.0f,  1.0f, 0.0f), {}, glm::vec2(0.0f, 1.0f)},
		stVertex{glm::vec3( 1.0f,  1.0f, 0.0f), {}, glm::vec2(1.0f, 1.0f)}
		});
		m_renderQuad.m_shadowShader = nullptr;
		m_renderQuad.SetIndicies({0, 1, 2, 1, 3, 2});

		//Setup default frame buffer effect
		m_frameBufferEffect = FrameBufferEffect::None;

		//Create Shaders
		m_shaders.emplace("FrameBuffer",		 new CShader("FrameBuffer.vert", "FrameBuffer.frag"));
		m_shaders.emplace("Rain",				 new CShader("FrameBuffer.vert", "Rain.frag"));
		m_shaders.emplace("ChromaticAberration", new CShader("FrameBuffer.vert", "ChromaticAberration.frag"));
		m_shaders.emplace("CRT",				 new CShader("FrameBuffer.vert", "CRT.frag"));

		//Set texture to render quad mesh
		m_renderQuad.m_textures.emplace("noise", new CTexture("Noise.png", GL_RGB, GL_UNSIGNED_BYTE));
	}

	//Create Shaders
	CShader* diffuse = new CShader("Diffuse.vert", "Diffuse.frag");
	diffuse->m_defaultUniform = [](CShader& _shader)
	{
		//Light Uniforms
		_shader.Uniform1f("uni_fSpecularStrength", 0.3f);
		_shader.Uniform1f("uni_fShininess", 16.0f);

		_shader.Uniform1f("uni_fRimExponent", 16.0f);
		_shader.Uniform1f("uni_fShininess", 16.0f);
		_shader.Uniform4f("uni_v4RimColour", 1.0f, 1.0f, 1.0f, 0.0f);

		_shader.Uniform1f("uni_fReflectionStrength", 0.0f);
	};
	m_shaders.emplace("Diffuse", diffuse);

	//Setup Camera
	GetMainCamera().SetFarPlane(4000.0f);
	GetMainCamera().m_transform.Move(glm::vec3(0, 0, 5));

	//Draw Skybox
	{
		const char* cubeMapDirectories[6]
		{
			"Cubemaps/MountainOutpost/Right.jpg",
			"Cubemaps/MountainOutpost/Left.jpg",
			"Cubemaps/MountainOutpost/Up.jpg",
			"Cubemaps/MountainOutpost/Down.jpg",
			"Cubemaps/MountainOutpost/Back.jpg",
			"Cubemaps/MountainOutpost/Front.jpg"
		};
	
		new CCubeSkybox(2000.0f, cubeMapDirectories);
	}
	//(new ssAnimatedModel("Resources/Models/theDude.DAE", *(new CShader("RiggedDiffuse.vert", "RiggedDiffuse.frag"))))->m_transform.SetScale(0.01f * glm::vec3(1.0f));
	//(new CGeoSphere())->m_transform.SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));
	//(new CGeoStar())->m_transform.SetPosition(glm::vec3(-4.0f, 0.0f, 0.0f));
	//(new CTessModel())->m_transform.SetPosition(glm::vec3(0.0f, -50.0f, 0.0f));
	//new CParticleSystem;
	//new CGPUParticleSystem();

	//Setup Lighting
	new CDirectionalLight;
	CLight::UpdateLightUniforms(*diffuse);
}

CAssesmentGameManager::~CAssesmentGameManager()
{
	glDeleteFramebuffers(1, &m_frameBuffer);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: To perform operations every frame such as enabling or disabling scissor tests

void CAssesmentGameManager::Update()
{
	//Update Objects
	UpdateObjectsInWorld();

	//Update the shadows
	CLight::UpdateShadowMaps();
	
	//Draw the scene to the frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawObjectsInWorld();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Draw the renderTexture from the frame buffer to the screen
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	switch (m_frameBufferEffect)
	{
	case FrameBufferEffect::None:
		m_renderQuad.m_shader = (*m_shaders.find("FrameBuffer")).second;
		break;
	case FrameBufferEffect::Rain:
		m_renderQuad.m_shader = (*m_shaders.find("Rain")).second;
		g_time += e_deltaTime;
		m_renderQuad.m_shader->Uniform1f("iTime", g_time);
		break;
	case FrameBufferEffect::ChromaticAberration:
		m_renderQuad.m_shader = (*m_shaders.find("ChromaticAberration")).second;
		g_time += e_deltaTime;
		m_renderQuad.m_shader->Uniform1f("iTime", g_time);
		break;
	case FrameBufferEffect::CRT:
		m_renderQuad.m_shader = (*m_shaders.find("CRT")).second;
		g_time += e_deltaTime;
		m_renderQuad.m_shader->Uniform1f("iTime", g_time);
		break;
	}
	
	m_renderQuad.Draw(GetMainCamera());
	
	glEnable(GL_DEPTH_TEST);

	//Delete objects marked for deletion
	DeleteObjectsInWorld();
}