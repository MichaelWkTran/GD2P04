/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Main.cpp																						        		*/
/*  DATE: Aug 27th, 2022																								*/
/************************************************************************************************************************/

#include "Main.h"
#include <GameManager.h>
#include <Camera.h>
#include <iostream> 

#pragma region Window

unsigned int e_viewPortW = 800, e_viewPortH = 800;
GLFWwindow* e_mainWindow = nullptr;

#pragma endregion

#pragma region Time

float e_previousTimestep = 0.0f;
float e_deltaTime = 0.0f;
float e_maxDeltatime = 1.0f / 60.0f;

#pragma endregion

#pragma region Input

std::set<void(*)(GLFWwindow*, int, int, int, int)> e_keyCallbackFunctions;
char e_codePoint = 0;
bool e_codePointFound = false;

std::set<void(*)(GLFWwindow*, int, int, int)> e_mouseCallbackFunctions;
glm::vec2 e_mousePosition;
glm::vec2 e_mouseNDCPosition;
glm::vec3 e_mouseRayDirection;

void UpdateMousePosition()
{
    double posX, posY;
    glfwGetCursorPos(e_mainWindow, &posX, &posY);

    e_mousePosition.x = (float)posX;
    e_mousePosition.y = (float)posY;

    //Update Mouse NDC Position
    e_mouseNDCPosition = glm::vec2
    (
        (2.0f * e_mousePosition.x) / ((float)e_viewPortW - 1.0f),
        (1.0f - (2.0f * e_mousePosition.y)) / (float)e_viewPortH
    );

    glm::vec4 clipCoord = glm::vec4(e_mouseNDCPosition.x, e_mouseNDCPosition.y, -1.0f, 1.0f);

    //Homogeneous Clip Space to Eye space
    glm::mat4 invProjMatrix = glm::inverse(GetMainCamera().GetProjectionMatrix());
    glm::vec4 eyeCoords = invProjMatrix * clipCoord;

    // Manually set z and w to mean forward direction and a vector and not a point.
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

    //Eyespace to World space
    glm::mat4 invViewMatrix = glm::inverse(GetMainCamera().GetViewMatrix());
    glm::vec4 rayWorld = invViewMatrix * eyeCoords;
    e_mouseRayDirection = glm::normalize(rayWorld);
}

void UpdateInputPressed()
{
    e_codePointFound = false;
}

void TextInput(GLFWwindow* _pWindow, unsigned int _iCodePoint)
{
    e_codePointFound = true;
    e_codePoint = _iCodePoint;
}

#pragma endregion

#include "StandaloneGameManager.h"

int main()
{
#pragma region Setup GLFW, GLEW, and Viewport

    //Initialize and Configure GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Set up Window
    e_mainWindow = glfwCreateWindow(e_viewPortW, e_viewPortH, "LearnOpenGL", NULL, NULL);
    if (e_mainWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(e_mainWindow);

    //Initializing GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    //Setup Window Viewport
    glViewport(0, 0, (int)e_viewPortW, (int)e_viewPortH);
    glfwSetFramebufferSizeCallback(e_mainWindow, [](GLFWwindow* _pMainWindow, int _iWidth, int _iHeight)
        {
            glViewport(0, 0, _iWidth, _iHeight);
            //e_uViewPortW = (int)_iWidth;
            //e_uViewPortH = (int)_iHeight;
            //GetMainCamera().SetViewPort(glm::uvec2(_iWidth, _iHeight));
        });

    //Set up Culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    //Set up Anti-Aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);

#pragma endregion

    srand((unsigned int)time(0));

    //Set up Game Manager
    new CStandaloneGameManager;

    //Set Input Callbacks
    glfwSetKeyCallback
    (
        e_mainWindow,
        [](GLFWwindow* _pWindow, int _iKey, int _iScanCode, int _iAction, int _iMods)
        {
            for (auto& i : e_keyCallbackFunctions) i(_pWindow, _iKey, _iScanCode, _iAction, _iMods);
        }
    );

    glfwSetMouseButtonCallback
    (
        e_mainWindow,
        [](GLFWwindow* _pWindow, int _iButton, int _iAction, int _iMods)
        {
            for (auto& i : e_mouseCallbackFunctions) i(_pWindow, _iButton, _iAction, _iMods);
        }
    );

    //Game Loop
    while (!glfwWindowShouldClose(e_mainWindow))
    {
        //Update Deltatime
        float currentTimestep = (float)glfwGetTime();
        e_deltaTime = currentTimestep - e_previousTimestep;
        if (e_deltaTime > e_maxDeltatime) e_deltaTime = e_maxDeltatime;
        e_previousTimestep = currentTimestep;

        //Update Window Size
        {
            int x, y;
            glfwGetWindowSize(e_mainWindow, &x, &y);
            e_viewPortW = x;
            e_viewPortH = y;
        }

        //Inputs
        UpdateMousePosition();

        //Clear Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Update GameManager
        GetGameManager().Update();

        //Check and call events and swap the buffers
        glfwSwapBuffers(e_mainWindow);
        glfwPollEvents();
    }

    glfwDestroyWindow(e_mainWindow);
    glfwTerminate();
    return 0;
}