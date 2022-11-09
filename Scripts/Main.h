/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Main.h																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <set>

//Window
extern unsigned int e_viewPortW, e_viewPortH;
extern GLFWwindow* e_mainWindow;

//Time
extern float e_previousTimestep;
extern float e_deltaTime;
extern float e_maxDeltatime;

//Input
extern std::set<void(*)(GLFWwindow*,int,int,int,int)> e_keyCallbackFunctions;
extern char e_codePoint;
extern bool e_codePointFound;

extern std::set<void(*)(GLFWwindow*, int, int, int)> e_mouseCallbackFunctions;
extern glm::vec2 e_mousePosition;
extern glm::vec2 e_mouseNDCPosition;

void UpdateMousePosition();
void UpdateInputPressed();
void TextInput(GLFWwindow* _window, unsigned int _codePoint);