/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: VertexArray.h																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

class CVertexArray
{
private:
	unsigned int m_uiID;

public:
	CVertexArray();
	CVertexArray(CVertexArray const&) = delete;
	~CVertexArray();

	const unsigned int& GetID();
	void Bind();
	static void Unbind();
};