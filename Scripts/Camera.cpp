/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Camera.cpp																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "Camera.h"
#include "Main.h"
#include "GameManager.h"
#include "FreePlayerCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>


CCamera* CCamera::m_mainCamera = nullptr;

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CCamera()
//	 Purpose: Initalises variables of the created CCamera

CCamera::CCamera()
{
	//Set the main camera if not assigned yet
	if (m_mainCamera == nullptr) m_mainCamera = this;

	//Set camera viewport
	m_viewPort.x = e_viewPortW;
	m_viewPort.y = e_viewPortH;
	
	//Set camera view mode
	m_isPerspective = true;
	
	//Set view specefic variables
	m_fieldOfView = 45.0f;
	m_orthographicScale = 1;
	
	//Set far and near plane
	m_nearPlane = 0.1f;
	m_farPlane = 100.0f;

	//Set camera matrices
	m_viewMatrix = glm::mat4(1.0f);
	m_projectionMatrix = glm::mat4(1.0f);
	m_cameraMatrix = glm::mat4(1.0f);

	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: ~CCamera()
//	 Purpose: Ensures that m_pMainCamera never contains garbage data if it is destroyed

CCamera::~CCamera()
{
	//Set m_mainCamera to null if it is destroyed
	if (m_mainCamera == this) m_mainCamera = nullptr;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: MainCamera()
//	 Purpose: Gets the main camera if requested. If a main camera does not exist, create a free camera as the main camera
//	 Returns: The current main camera

CCamera& CCamera::MainCamera()
{
	//If a main camera does not exist, create a free camera
	if (m_mainCamera == nullptr) m_mainCamera = new CFreePlayerCamera();

	return *m_mainCamera;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetViewMatrix()
//	 Purpose: Gets the view matrix of the camera
//	 Returns: The view matrix of the camera

const glm::mat4 CCamera::GetViewMatrix() const
{
	return m_viewMatrix;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetProjectionMatrix()
//	 Purpose: Gets the projection matrix of the camera
//	 Returns: The projection matrix of the camera

const glm::mat4 CCamera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetCameraMatrix()
//	 Purpose: Gets the camera matrix of the camera
//	 Returns: The camera matrix of the camera

const glm::mat4 CCamera::GetCameraMatrix() const
{
	return m_cameraMatrix;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetViewPort()
//	 Purpose: Gets the viewport size of the camera
//	 Returns: The viewport size of the camera

const glm::uvec2 CCamera::GetViewPort() const
{
	return m_viewPort;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetViewPort()
//	 Purpose: Sets the viewport size of the camera where the passed in _uv2ViewPort is the new viewport

void CCamera::SetViewPort(const glm::uvec2 _viewPort)
{
	m_viewPort = _viewPort;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetProjection()
//	 Purpose: Gives a boolean that shows the view mode of the camera. If the boolean is true, the camera is in perspective mode, otherwise, it is in orthographic mode
//	 Returns: True if the camera is in perspective mode, otherwise false if it is in orthographic mode

const bool CCamera::GetProjection() const
{
	return m_isPerspective;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetProjection()
//	 Purpose: Sets the view mode of the camera. If the boolean given is true, the camera is set to perspective mode, otherwise, it is set to orthographic mode

void CCamera::SetProjection(const bool _isPerspective)
{
	m_isPerspective = _isPerspective;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetFieldOfView()
//	 Purpose: Gets the field of view of the camera. The function will return the field of view in radians if _bInRadians is true,
//			  otherwise it will return the field of view in degrees.
//	 Returns: The field of view in radians if _bInRadians is true, otherwise it will return the field of view in degrees.

const float CCamera::GetFieldOfView(const bool _inRadians)
{
	if (_inRadians) return glm::radians(m_fieldOfView);
	else return m_fieldOfView;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetFieldOfView()
//	 Purpose: Sets the new field of view of the camera, _fFieldOfView. If the field of view is in radians, then _bIsRadians must be set to true.

void CCamera::SetFieldOfView(const float _fieldOfView, const bool _isRadians)
{
	m_fieldOfView = _isRadians ? glm::degrees(_fieldOfView) : _fieldOfView;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetOrthographicScale()
//	 Purpose: Gets the orthographic scale of the camera.
//	 Returns: The orthographic scale of the camera

const float CCamera::GetOrthographicScale() const
{
	return m_orthographicScale;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetOrthographicScale()
//	 Purpose: Sets the new orthographic scale, _fOrthographicScale, of the camera.

void CCamera::SetOrthographicScale(const float _orthographicScale)
{
	m_orthographicScale = _orthographicScale;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetNearPlane()
//	 Purpose: Gets the near plane distance of the camera
//	 Returns: The near plane distance of the camera

const float CCamera::GetNearPlane() const
{
	return m_nearPlane;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetNearPlane()
//	 Purpose: Sets the new near plane distance, _fNearPlane, of the camera

void CCamera::SetNearPlane(const float _nearPlane)
{
	m_nearPlane = _nearPlane;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetFarPlane()
//	 Purpose: Gets the far plane distance of the camera
//	 Returns: The far plane distance of the camera

const float CCamera::GetFarPlane() const
{
	return m_farPlane;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetFarPlane()
//	 Purpose: Sets the new far plane distance of the camera, _fFarPlane.

void CCamera::SetFarPlane(const float _farPlane)
{
	m_farPlane = _farPlane;
	m_updateProjectionMatrix = true;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: void CCamera::Update()
//	 Purpose: Updates the projection, view and camera matricies

void CCamera::Update()
{
	if (m_updateProjectionMatrix)
	{
		if (m_isPerspective)
		{
			m_projectionMatrix = glm::perspective
			(
				glm::radians(m_fieldOfView),
				((float)m_viewPort.x) / ((float)m_viewPort.y),
				m_nearPlane, m_farPlane
			);
		}
		else
		{
			float halfViewPortW = (((float)m_viewPort.x) / 2) / m_orthographicScale;
			float halfViewPortH = (((float)m_viewPort.y) / 2) / m_orthographicScale;

			m_projectionMatrix = glm::ortho(halfViewPortW, -halfViewPortW, halfViewPortH, -halfViewPortH, m_nearPlane, m_farPlane);
		}

		m_updateProjectionMatrix = false;
	}

	//Update the view and camera matrices
	m_viewMatrix = glm::lookAt(m_transform.GetPosition(), m_transform.GetPosition() + m_transform.Forward(), glm::vec3(0.0f, 1.0f, 0.0f));
	m_cameraMatrix = m_projectionMatrix * m_viewMatrix;
}