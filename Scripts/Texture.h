/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Texture.h																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <map>
#include <string>

class CShader;

class CTexture
{
private:
	unsigned int m_uiID;

public:
	static const char* m_pDirective;
	GLenum m_GLeTarget;
	
	CTexture(GLenum&& _GLeTarget = GL_TEXTURE_2D);
	CTexture(std::string _pImage, GLenum&& _GLeFormat = GL_RGB, GLenum&& _GLePixelType = GL_UNSIGNED_BYTE);
	CTexture(CTexture const&) = delete;
	CTexture& operator=(const CTexture&) = delete;
	~CTexture();

	operator int() const;
	explicit operator int* ();
	const unsigned int GetID() const;
	void Uniform(unsigned int _uiShaderID, std::string _strUniformName, unsigned int _uiSlot = 0);
	void Bind() const;
	static void Unbind();
};