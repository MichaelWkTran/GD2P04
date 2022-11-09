/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: ElementBuffer.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "ElementBuffer.h"
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CElementBuffer()
//	 Purpose: Generate an element buffer.

CElementBuffer::CElementBuffer()
{
    glGenBuffers(1, &m_GLuID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CElementBuffer()
//	 Purpose: Generate an element buffer while giving indices to the buffer via _vIndices

CElementBuffer::CElementBuffer(const std::vector<unsigned int> _vIndicies)
{
    glGenBuffers(1, &m_GLuID);
    SetIndicies(_vIndicies);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: ~CElementBuffer()
//	 Purpose: Delete the element buffer

CElementBuffer::~CElementBuffer()
{
    glDeleteBuffers(1, &m_GLuID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetID()
//	 Purpose: Gets the ID of the element buffer
//	 Returns: The ID of the element buffer

const unsigned int& CElementBuffer::GetID()
{
    return m_GLuID;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetIndicies()
//	 Purpose: Gets the indices stored in the buffer
//	 Returns: The indices stored in the buffer

const std::vector<unsigned int> CElementBuffer::GetIndicies() const
{
    return m_vIndicies;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetIndicies()
//	 Purpose: Sets the indices stored in the buffer via _vIndices

void CElementBuffer::SetIndicies(const std::vector<unsigned int> _vIndicies)
{
    m_vIndicies = _vIndicies;

    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vIndicies.size() * sizeof(unsigned int), m_vIndicies.data(), GL_STATIC_DRAW);
    Unbind();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Bind()
//	 Purpose: Binds the element buffer

void CElementBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLuID);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Unbind()
//	 Purpose: Unbinds the element buffer

void CElementBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}