/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: FreePlayerCamera.h																							*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "Camera.h"

class CFreePlayerCamera : public CCamera
{
public:
	GLFWwindow* m_window;
	bool m_firstClick;

	CFreePlayerCamera();
	
	virtual void Update() override;
};

