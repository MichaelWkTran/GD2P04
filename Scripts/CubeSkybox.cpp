/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: CubeSkybox.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "CubeSkybox.h"
#include "GenerateMesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CCubeSkybox()
//	 Purpose: Initalise the variables of the CCubeSkybox as well as setup the cube map texture.
//			  _fSize is the size of the skybox while _pTextureDirectories is the array of directories that lead to the images to be loaded. 

CCubeSkybox::CCubeSkybox(const float _fSize, const char* _pTextureDirectories[6])
{
	//Set Shader
	if (m_mesh.m_shader == nullptr) m_mesh.m_shader = new CShader("CubeMap.vert", "CubeMap.frag");
	m_mesh.m_shadowShader = nullptr;

	//Generate Mesh
	gm::GenerateFlatCube(m_mesh, glm::vec3(1.0f, 1.0f, 1.0f) * _fSize);

	//Invert Faces
	std::vector<unsigned int> vIndicies = m_mesh.GetIndicies();
	for (int i = 0; i < (int)vIndicies.size(); i += 3)
	{
		int iTemp = vIndicies[i + 1];
		vIndicies[i + 1] = vIndicies[i + 2];
		vIndicies[i + 2] = iTemp;
	}
	m_mesh.SetIndicies(vIndicies);

	//Set up texture
	CTexture* texture = new CTexture(GL_TEXTURE_CUBE_MAP);
	m_mesh.m_textures.emplace(std::make_pair("uni_sampCube", texture));
	texture->Bind();

	int iImageWidth, iImageHeight, iImageComponents;
	stbi_set_flip_vertically_on_load(false);
	for (int i = 0; i < 6; i++)
	{
		unsigned char* pImageData = stbi_load((std::string(CTexture::m_pDirective) + std::string(*(_pTextureDirectories + i))).c_str(), &iImageWidth, &iImageHeight, &iImageComponents, 0);
		if (pImageData == nullptr) { std::cout << stbi_failure_reason() << "\n"; }

		unsigned int uiLoadedComponents = (iImageComponents == 4) ? GL_RGBA : GL_RGB;

		glTexImage2D
		(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, uiLoadedComponents, iImageWidth, iImageHeight, 0,
			uiLoadedComponents, GL_UNSIGNED_BYTE, pImageData
		);
		
		stbi_image_free(pImageData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	CTexture::Unbind();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: UpdateLightUniforms()
//	 Purpose: Apply the skybox uniform to the given shader, _pShader 

void CCubeSkybox::UpdateLightUniforms(CShader* _pShader)
{
	//[Texture slot is zero]

	_pShader->Activate();
	 m_mesh.m_textures[0]->Uniform(*_pShader, "uni_sampSkybox");
	CTexture::Unbind();
	_pShader->Deactivate();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: UpdateLightUniforms()
//	 Purpose: Apply the skybox uniform to the given vector of shaders, _vShaders

void CCubeSkybox::UpdateLightUniforms(std::vector<CShader*> _vShaders)
{
	for (auto pShader : _vShaders) UpdateLightUniforms(pShader);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Draw()
//	 Purpose: Draw the cube map

void CCubeSkybox::Draw()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	m_mesh.Draw(GetMainCamera());
	glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}