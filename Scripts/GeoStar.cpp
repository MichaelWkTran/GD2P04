#include "GeoStar.h"
#include "Shader.h"
#include "Texture.h"
#include "Lights.h"

CGeoStar::CGeoStar()
{
	//Create Mesh
	m_mesh = new CMesh<>();
	CMesh<>* mesh = (CMesh<>*)m_mesh;

	//Assign the mesh with one vertex
	mesh->SetVerticies(std::vector<stVertex>{ stVertex{} });
	
	//Set shaders
	mesh->m_shader = new CShader("Star.vert", "Star.geom", "Star.frag");
	mesh->m_shadowShader = new CShader("StarShadowMap.vert", "StarShadowMap.geom", "Empty.frag");

	//Setup texture
	mesh->m_textures.emplace(std::make_pair("", new CTexture("Eye.png")));

	//Set mesh draw method
	mesh->m_drawMethod = [](CMesh<>& _Mesh) { glDrawArrays(GL_POINTS, 0, 1); };
}

void CGeoStar::Draw()
{
	CMesh<>* mesh = (CMesh<>*)m_mesh;
	CLight::UpdateLightUniforms(*mesh->m_shader);
	CGameObject::Draw();
}
