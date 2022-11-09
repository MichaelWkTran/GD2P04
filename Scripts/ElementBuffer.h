/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: ElementBuffer.h																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <vector>

class CElementBuffer
{
private:
	unsigned int m_GLuID;
	std::vector<unsigned int> m_vIndicies;

public:
	CElementBuffer();
	CElementBuffer(const std::vector<unsigned int> _vIndicies);
	CElementBuffer(CElementBuffer const&) = delete;
	~CElementBuffer();

	const unsigned int& GetID();
	const std::vector<unsigned int> GetIndicies() const;
	void SetIndicies(const std::vector<unsigned int> _vIndicies);
	void Bind();
	static void Unbind();
};

