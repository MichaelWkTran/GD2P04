/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Transform.h																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <glm/gtx/vector_angle.hpp>

class CTransform
{
private:
	glm::vec3 m_v3Position;
	glm::fquat m_fquatRotation;
	glm::vec3 m_v3Scale;
	glm::mat4x4 m_mat4Model;
	
	void UpdateModelMatrix();

public:
	CTransform();
	CTransform(CTransform const& _Other);
	CTransform& operator=(const CTransform& _Other);

	const glm::mat4x4 GetModel() const;
	
	const glm::vec3 GetPosition() const;
	void SetPosition(const glm::vec3 _v3Position);
	void Move(const glm::vec3 _v3Offset);
	
	const glm::fquat GetRotation() const;
	void SetRotation(const glm::fquat _fquatRotation);
	void Rotate(const glm::fquat _fquatOffset);
	const glm::vec3 GetRotationEuler(bool _bInDegrees = true) const;
	void SetRotationEuler(const glm::vec3 _v3Rotation, bool _bInDegrees = true);
	void RotateEuler(const glm::vec3 _v3Offset, bool _bInDegrees = true);
	void LookAt(const glm::vec3 _v3Target);
	
	const glm::vec3 GetScale() const;
	void SetScale(const glm::vec3 _v3Scale);

	const glm::vec3 Up() const;
	const glm::vec3 Forward() const;
	const glm::vec3 Right() const;
};