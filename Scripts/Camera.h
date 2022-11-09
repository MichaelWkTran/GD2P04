/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Camera.h																										*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "UpdatedObject.h"
#include "Transform.h"

/*The camera object allows the user to view the game world.*/
class CCamera : public CUpdatedObject
{
protected:
	glm::uvec2 m_viewPort;
	bool m_isPerspective;
	float m_fieldOfView; //Value is in degrees
	float m_orthographicScale;
	float m_nearPlane, m_farPlane;

	bool m_updateProjectionMatrix;
	glm::mat4 m_viewMatrix, m_projectionMatrix, m_cameraMatrix;

public:
	static CCamera* m_mainCamera;
	CTransform m_transform;

	CCamera();
	~CCamera();
	static CCamera& MainCamera();

	const glm::mat4 GetViewMatrix() const;
	const glm::mat4 GetProjectionMatrix() const;
	const glm::mat4 GetCameraMatrix() const;
	const glm::uvec2 GetViewPort() const; void SetViewPort(const glm::uvec2 _viewPort);
	const bool GetProjection() const; void SetProjection(const bool _perspective);
	const float GetFieldOfView(bool _inRadians = false); void SetFieldOfView(const float _fieldOfView, const bool _isRadians = false);
	const float GetOrthographicScale() const; void SetOrthographicScale(const float _orthographicScale);
	const float GetNearPlane() const; void SetNearPlane(const float _nearPlane);
	const float GetFarPlane() const; void SetFarPlane(const float _farPlane);

	virtual void Update() override;
};

#define GetMainCamera CCamera::MainCamera