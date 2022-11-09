/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: FreePlayerCamera.cpp																							*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "FreePlayerCamera.h"
#include "Main.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CFreePlayerCamera()
//	 Purpose: Initalise the free camera variables. 

CFreePlayerCamera::CFreePlayerCamera()
{
	m_window = e_mainWindow;
	m_firstClick = false;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: To move the camera using the WASD and arrow keys and to rotate the camera using the mouth. 

void CFreePlayerCamera::Update()
{
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	float speed = 10.0f;
	float sensitivity = 30.0f;
	
	//Handles key inputs
	glm::vec3 velocity(0.0f, 0.0f, 0.0f);

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_UP)	   == GLFW_PRESS) velocity += speed * m_transform.Forward();
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_DOWN)  == GLFW_PRESS) velocity += speed * -m_transform.Forward();
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_LEFT)  == GLFW_PRESS) velocity += speed * -m_transform.Right();
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS) velocity += speed * m_transform.Right();
	if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)													  velocity += speed * up;
	if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)											  velocity += speed * -up;

	m_transform.SetPosition(m_transform.GetPosition() + (velocity * e_deltaTime));

	//Handles mouse inputs
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//Hides mouse cursor
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		//Prevents camera from jumping on the first click
		if (m_firstClick)
		{
			e_mousePosition = glm::vec2(e_viewPortW / 2, e_viewPortH / 2);
			glfwSetCursorPos(m_window, e_mousePosition.x, e_mousePosition.y);
			m_firstClick = false;
		}

		//Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		//and then "transforms" them into degrees 
		float rotX = sensitivity * (e_mousePosition.x - (e_viewPortW / 2)) * e_deltaTime;
		float rotY = sensitivity * (e_mousePosition.y - (e_viewPortH / 2)) * e_deltaTime;
		
		//Calculates upcoming vertical change in the Orientation
		glm::vec3 v3Orientation = glm::rotate(m_transform.Forward(), glm::radians(-rotY), glm::normalize(glm::cross(m_transform.Forward(), up)));;

		//Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(v3Orientation, up) - glm::radians(90.0f)) > glm::radians(85.0f))
			v3Orientation = m_transform.Forward();

		//Rotates the Orientation left and right
		v3Orientation = glm::rotate(v3Orientation, glm::radians(-rotX), up);

		//Set New Orientation
		m_transform.LookAt(m_transform.GetPosition() + v3Orientation);

		//Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(m_window, (e_viewPortW / 2), (e_viewPortH / 2));
		e_mousePosition = glm::vec2(e_viewPortW / 2, e_viewPortH / 2);
	}
	else if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		//Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//Makes sure the next time the camera looks around it doesn't jump
		m_firstClick = true;
	}

	CCamera::Update();
}
