/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: DeferredGameManager.cpp																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

float g_time = 0;

#include "DeferredGameManager.h"
#include <Camera.h> //For accessing the camera
#include <TinyObjectLoader.h> //For loading models
#include <Lights.h> //For adding lights to the scene
#include <Shader.h> //For creating shaders
#include <Texture.h> //For creating textures
#include <CubeSkybox.h> //For creating a skybox
#include "Main.h" //For accessing viewport and keyboard input
#include <iostream> //For printing messages to console
#include <GenerateMesh.h>
#include <MathUtils.h>
#include <Texture.h>
#include <GameObject.h>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CDeferredGameManager()
//	 Purpose: To initalise variables and to setup the demo scene

CDeferredGameManager::CDeferredGameManager()
{
	//Set Key Inputs
	e_keyCallbackFunctions.emplace([](GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
	{
		CDeferredGameManager* pGameManager = ((CDeferredGameManager*)&GetGameManager());
		if (_action != GLFW_PRESS) return;
	
		//switch (_key)
		//{
		//	
		//}
	});

	//Create Gbuffer
	glGenFramebuffers(1, &m_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

	//Create Gbuffer textures
	auto generateBufferTexture = [](GLint _internalFormat, GLenum _format, GLenum _type, GLenum _colourAttachment) -> CTexture*
	{
		CTexture* bufferTexture = new CTexture();

		bufferTexture->Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, e_viewPortW, e_viewPortH, 0, _format, _type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, _colourAttachment, GL_TEXTURE_2D, *bufferTexture, 0);
		CTexture::Unbind();

		return bufferTexture;
	};
	CTexture* positionTex =	  generateBufferTexture(GL_RGB16F,			GL_RGBA,			GL_FLOAT,		  GL_COLOR_ATTACHMENT0);
	CTexture* normalTex =	  generateBufferTexture(GL_RGB16F,			GL_RGBA,			GL_FLOAT,		  GL_COLOR_ATTACHMENT1);
	CTexture* albedoSpecTex = generateBufferTexture(GL_RGBA,			GL_RGBA,		    GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT2);
	CTexture* depthTex =	  generateBufferTexture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT,		  GL_DEPTH_ATTACHMENT);

	//Give the GBuffer the colour attachments to use
	GLenum attachments[3]{GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	//Check the GBuffer is valid and unbind it
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer is is not complete";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Setup render quad mesh 
	m_renderQuad.SetVerticies({
	stVertex{glm::vec3(-1.0f, -1.0f, 0.0f), {}, glm::vec2(0.0f, 0.0f)},
	stVertex{glm::vec3(1.0f, -1.0f, 0.0f), {}, glm::vec2(1.0f, 0.0f)},
	stVertex{glm::vec3(-1.0f,  1.0f, 0.0f), {}, glm::vec2(0.0f, 1.0f)},
	stVertex{glm::vec3(1.0f,  1.0f, 0.0f), {}, glm::vec2(1.0f, 1.0f)}});
	m_renderQuad.m_shadowShader = nullptr;
	m_renderQuad.SetIndicies({ 0, 1, 2, 1, 3, 2 });

	//Create Render Pass Shader
	m_renderQuad.m_shader = new CShader("FrameBuffer.vert", "Deferred/LightingPass.frag");
	m_renderQuad.m_textures.emplace("uni_gPosition", positionTex);
	m_renderQuad.m_textures.emplace("uni_gNormal", normalTex);
	m_renderQuad.m_textures.emplace("uni_gAlbedoSpec", albedoSpecTex);
	m_renderQuad.m_textures.emplace("uni_gDepth", depthTex);

	//Create Shaders
	CShader* diffuse = new CShader("Diffuse.vert", "Deferred/Diffuse.frag");
	m_shaders.emplace("Diffuse", diffuse);

	//Setup Camera
	GetMainCamera().m_transform.Move(glm::vec3(0.0f, 0.0f, 5.0f));

	//Setup Lighting
	new CDirectionalLight(glm::vec4(1.0f, 1.0f, 1.0f, 0.3f));
	new CPointLight(glm::vec3( 0.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 2.0f), 6.0f, 1.4f);
	new CPointLight(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 2.0f), 6.0f, 1.4f);
	new CPointLight(glm::vec3( 3.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 2.0f), 6.0f, 1.4f);
	new CPointLight(glm::vec3( 3.0f, 0.0f, 2.0f), glm::vec4(1.0f, 1.0f, 0.0f, 2.0f));
	CLight::UpdateLightUniforms(*m_renderQuad.m_shader);

	//Create objects in world
	CGameObject* plane = new CGameObject;
	gm::GeneratePlane(*plane->GenerateMesh<CMesh<>>(), glm::vec3(10.0f));
	plane->GetMesh()->m_shader = diffuse;
	plane->m_transform.RotateEuler(glm::vec3(-90.0f, 0.0f, 0.0f));
	plane->m_transform.Move(glm::vec3(0.0f, -2.0f, 0.0f));

	CGameObject* model = new CGameObject;
	GetObjModelData(*model->GenerateMesh<CMesh<>>(), "../Resources/Models/AsymmetricalObject.obj");
	model->GetMesh()->m_shader = diffuse;
	((CMesh<>*)(model->GetMesh()))->m_drawMethod = [](CMesh<>& _Mesh)
	{
		glDrawArrays(GL_TRIANGLES, 0, _Mesh.m_vertexBuffer.GetVertices().size());
	};
	model->m_transform.SetScale(glm::vec3(2.0f));
	model->m_transform.Move(glm::vec3(-1.0, 0.0f, 0.0f));

	CGameObject* sphere = new CGameObject;
	gm::GenerateSphere(*sphere->GenerateMesh<CMesh<>>(), 1.0f, 20);
	sphere->GetMesh()->m_shader = diffuse;
	sphere->m_transform.Move(glm::vec3(1.0f, 0.0f, 0.0f));
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: To perform operations every frame such as enabling or disabling scissor tests

void CDeferredGameManager::Update()
{
	//Update Objects
	UpdateObjectsInWorld();

	//Draw all objects in the game world into gBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawObjectsInWorld();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Draw the lighting pass to the screen
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_renderQuad.Draw(GetMainCamera());
	glEnable(GL_DEPTH_TEST);

	//Delete objects marked for deletion
	DeleteObjectsInWorld();
}