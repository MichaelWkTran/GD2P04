/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Mesh.h																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Lights.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::set<CBaseMesh*> CBaseMesh::meshes;

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CMesh()
//	 Purpose: Initalises variables of the mesh

template<class T>
inline CMesh<T>::CMesh()
{
	m_shader = nullptr;
	m_shadowShader = CLight::GetShadowMapShader();
	m_drawMethod = [](CMesh<T>& _Mesh)
	{
		glDrawElements(GL_TRIANGLES, _Mesh.GetIndicies().size(), GL_UNSIGNED_INT, 0);
	};
	m_updateVertexArray = false;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CMesh()
//	 Purpose: Initalises variables of the mesh._vVertices is the number of vertices of the mesh.
//			  _vIndices is the number of indices in the element buffer.
//			  _mapTextures is the textures stored in the mesh.
//			  _pShader is the shader used by the mesh. 


template<class T>
CMesh<T>::CMesh(std::vector<T>& _vVerticies, std::vector<unsigned int>& _vIndicies, std::map<const char*, CTexture*>& _textures, CShader* _pShader)
{
	m_vertexBuffer.SetVertices(_vVerticies);
	m_elementBuffer.SetIndicies(_vIndicies);
	m_textures = _textures;
	m_shader = _pShader;
	m_shadowShader = CLight::GetShadowMapShader();
	m_drawMethod = [](CMesh<T>& _Mesh)
	{
		glDrawElements(GL_TRIANGLES, _Mesh.GetIndicies().size(), GL_UNSIGNED_INT, 0);
	};
	m_updateVertexArray = true;
}

template<class T>
void CMesh<T>::UpdateVertexArray()
{
	m_vertexArray.Bind(); m_vertexBuffer.Bind(); m_elementBuffer.Bind();
	T::LinkAttributes();
	m_vertexArray.Unbind(); m_vertexBuffer.Unbind(); m_elementBuffer.Unbind();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetVerticies()
//	 Purpose: Gets the vertices of the mesh.
//	 Returns: The vertices of the mesh.

template<class T>
inline const std::vector<T> CMesh<T>::GetVerticies() const
{
	return m_vertexBuffer.GetVertices();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetVerticies()
//	 Purpose: Sets the vertices of the mesh via _vVertices.

template<class T>
inline void CMesh<T>::SetVerticies(const std::vector<T> _vVerticies)
{
	m_vertexBuffer.SetVertices(_vVerticies);
	m_updateVertexArray = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetIndicies()
//	 Purpose: Gets the indices stored in the mesh.
//	 Returns: The indices stored in the mesh.

const std::vector<unsigned int> CBaseMesh::GetIndicies() const
{
	return m_elementBuffer.GetIndicies();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetIndicies()
//	 Purpose: Sets the indices stored in the mesh via _vIndices.

void CBaseMesh::SetIndicies(const std::vector<unsigned int> _vIndicies)
{
	m_elementBuffer.SetIndicies(_vIndicies);
	m_updateVertexArray = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Draw()
//	 Purpose: Draws the mesh using the camera matrix from _Camera

template<class T>
inline void CMesh<T>::Draw(const CCamera& _Camera)
{
	//Dont draw if no shader is assigned or m_VertexBuffer have no verticies
	if (m_shader == nullptr || m_vertexBuffer.GetVertices().empty()) return void();

	//Update vertex array if its vertex data has changed
	if (m_updateVertexArray)
	{
		m_updateVertexArray = false;
		UpdateVertexArray();
	}

	//Set GameObject Uniform
	m_shader->UniformMatrix4fv("uni_mat4Model", 1, GL_FALSE, m_transform.GetModel());

	//Set Mesh Uniforms
	m_shader->ResetUniforms();
	m_shader->Uniform3f("uni_v3CameraPosition", _Camera.m_transform.GetPosition());
	m_shader->UniformMatrix4fv("uni_mat4CameraMatrix", 1, GL_FALSE, _Camera.GetCameraMatrix());

	//Set Shadow Uniforms
	if (m_shadowShader != nullptr) CLight::UpdateShadowUniforms(*m_shader, m_textures.size() + 1U);

	//Set Texture Uniforms
	unsigned int slot = 0;
	for (auto& texture : m_textures)
	{
		if (texture.second == nullptr) continue;
		texture.second->Uniform(*m_shader, texture.first, slot);
		slot++;
	}

	//Draw Mesh
	m_shader->Activate();
	m_vertexArray.Bind();
	m_drawMethod(*this);
	m_vertexArray.Unbind();
	m_shader->Deactivate();
	CTexture::Unbind();
}

template class CMesh<stVertex>;