/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: VertexBuffer.h																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

template <class VertexType>
class CVertexBuffer
{
protected:
	unsigned int m_uiID;
	std::vector<VertexType> m_vVertices;

public:
	CVertexBuffer();
	CVertexBuffer(const std::vector<VertexType> _vVerticies);
	CVertexBuffer(CVertexBuffer const&) = delete;
	~CVertexBuffer();

	const unsigned int& GetID();
	const std::vector<VertexType> GetVertices() const;
	void SetVertices(const std::vector<VertexType> _vVertices);
	void Bind();
	static void Unbind();
};

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Generates a vertex buffer.

template<class VertexType>
inline CVertexBuffer<VertexType>::CVertexBuffer()
{
	glGenBuffers(1, &m_uiID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Generates a vertex buffer and assigns it with vertex data via _vVerticies

template<class VertexType>
inline CVertexBuffer<VertexType>::CVertexBuffer(const std::vector<VertexType> _vVerticies)
{
	glGenBuffers(1, &m_uiID);
	SetVertices(_vVerticies);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Deletes the vertex buffer

template<class VertexType>
inline CVertexBuffer<VertexType>::~CVertexBuffer()
{
	glDeleteBuffers(1, &m_uiID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Gets the vertex buffer ID
//	 Returns: The vertex buffer ID

template<class VertexType>
inline const unsigned int& CVertexBuffer<VertexType>::GetID()
{
	return m_uiID;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Gets the vertices stored in the vertex buffer
//	 Returns: The vertices stored in the vertex buffer.

template<class VertexType>
inline const std::vector<VertexType> CVertexBuffer<VertexType>::GetVertices() const
{
	return m_vVertices;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Sets the vertices stored in the vertex buffer via _vVertices.

template<class VertexType>
inline void CVertexBuffer<VertexType>::SetVertices(const std::vector<VertexType> _vVertices)
{
	m_vVertices = _vVertices;

	Bind();
	glBufferData(GL_ARRAY_BUFFER, m_vVertices.size() * sizeof(VertexType), m_vVertices.data(), GL_STATIC_DRAW);
	Unbind();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Binds the vertex buffer.

template<class VertexType>
inline void CVertexBuffer<VertexType>::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_uiID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Unbinds the vertex buffer

template<class VertexType>
inline void CVertexBuffer<VertexType>::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}