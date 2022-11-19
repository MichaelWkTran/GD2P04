/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: StandaloneGameManager.cpp																						*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

float g_time = 0;

#include "StandaloneGameManager.h"
#include <Camera.h> //For accessing the camera
#include <TinyObjectLoader.h> //For loading models
#include <Lights.h> //For adding lights to the scene
#include <Shader.h> //For creating shaders
#include <Texture.h> //For creating textures
#include <CubeSkybox.h> //For creating a skybox
#include "Main.h" //For accessing viewport and keyboard input
#include <iostream> //For printing messages to console
#include <GenerateMesh.h>
#include "BonFireParticleSystem.h"
#include "ssAnimatedModel.h"
#include <MathUtils.h>
#include "GPUParticle.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CStandaloneGameManager()
//	 Purpose: To initalise variables and to setup the demo scene

CStandaloneGameManager::CStandaloneGameManager()
{
	//Set Key Inputs
	e_keyCallbackFunctions.emplace([](GLFWwindow* _window, int _key, int _scanCode, int _action, int _mods)
	{
		CStandaloneGameManager* pGameManager = ((CStandaloneGameManager*)&GetGameManager());
		if (_action != GLFW_PRESS) return;

		switch (_key)
		{
		case GLFW_KEY_1:
			pGameManager->m_particleSystem->m_visible = false;
			pGameManager->m_gpuParticleSystem->m_visible = false;
			break;
		case GLFW_KEY_2:
			pGameManager->m_particleSystem->m_visible = true;
			pGameManager->m_gpuParticleSystem->m_visible = false;
			break;
		case GLFW_KEY_3:
			pGameManager->m_particleSystem->m_visible = false;
			pGameManager->m_gpuParticleSystem->m_visible = true;
			break;
		}
	});

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
	(new ssAnimatedModel("../Resources/Models/theDude.DAE", *(new CShader("RiggedDiffuse.vert", "RiggedDiffuse.frag"))))->m_transform.SetScale(0.01f * glm::vec3(1.0f));
	m_particleSystem = new CBonfireParticleSystem;
	m_gpuParticleSystem = new CGPUParticleSystem();
	m_particleSystem->m_visible = false;
	m_gpuParticleSystem->m_visible = false;

	//Setup Lighting
	new CDirectionalLight;
	CLight::UpdateLightUniforms(*diffuse);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: To perform operations every frame such as enabling or disabling scissor tests

void CStandaloneGameManager::Update()
{
	//Update Objects
	UpdateObjectsInWorld();
	
	//Draw the scene
	DrawObjectsInWorld();
	
	//Delete objects marked for deletion
	DeleteObjectsInWorld();
}