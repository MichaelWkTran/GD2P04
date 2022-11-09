/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Mesh.h																										*/
/*  DATE: Aug 23rd, 2022																								*
/************************************************************************************************************************/

#pragma once
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ElementBuffer.h"
#include <map>
#include <set>
#include "Transform.h"

class CTexture;
class CShader;
class CCamera;

struct stVertex
{
	glm::vec3 v3Position;
	glm::vec3 v3Normal;
	glm::vec2 v2TextureUV;
	
	static void LinkAttributes()
	{
		glEnableVertexAttribArray(0 /*m_v3Position*/);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v3Position)));

		glEnableVertexAttribArray(1 /*m_v3Normal*/);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v3Normal)));

		glEnableVertexAttribArray(2 /*m_v2TextureCoord*/);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(stVertex), (void*)(offsetof(stVertex, stVertex::v2TextureUV)));
	}
};

class CBaseMesh
{
protected:
	bool m_updateVertexArray;
	static std::set<CBaseMesh*> meshes;

	CBaseMesh()
	{
		meshes.emplace(this);
		m_updateVertexArray = false;
		m_shader = nullptr;
		m_shadowShader = nullptr;
	};
	
public:
	static std::set<CBaseMesh*>::iterator GetMeshesBegin() { return meshes.begin(); }
	static std::set<CBaseMesh*>::iterator GetMeshesEnd() { return meshes.end(); }
	
	CVertexArray m_vertexArray;
	CElementBuffer m_elementBuffer;
	CTransform m_transform;
	std::map<const char* /*Uniform Name*/, CTexture*> m_textures;
	CShader* m_shader;
	CShader* m_shadowShader;
	
	~CBaseMesh()
	{
		meshes.erase(this);
	};
	const std::vector<unsigned int> GetIndicies() const;
	void SetIndicies(const std::vector<unsigned int> _indicies);
	virtual void Draw(const CCamera& _camera) = 0;
};

template <class T = stVertex>
class CMesh : public CBaseMesh
{
public:
	CVertexBuffer<T> m_vertexBuffer;
	void (*m_drawMethod)(CMesh<T>& _Mesh);

	CMesh();
	CMesh(std::vector<T>& _verticies, std::vector<unsigned int>& _indicies,
		std::map<const char*, CTexture*>& _textures,
		CShader* _pShader = nullptr);

	void UpdateVertexArray();
	const std::vector<T> GetVerticies() const;
	void SetVerticies(const std::vector<T> _verticies);
	
	virtual void Draw(const CCamera& _camera) override;
};