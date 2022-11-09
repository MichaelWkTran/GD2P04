/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: Transform.cpp																							    	*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CTransform()
//	 Purpose: Initalise variables for the CTransform

CTransform::CTransform()
{
    m_v3Position = glm::vec3(0.0f, 0.0f, 0.0f);
    m_fquatRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    m_v3Scale = glm::vec3(1.0f, 1.0f, 1.0f);
    UpdateModelMatrix();
}

CTransform::CTransform(CTransform const& _Other)
{
    m_v3Position = _Other.GetPosition();
    m_fquatRotation = _Other.GetRotation();
    m_v3Scale = _Other.GetScale();
    m_mat4Model = _Other.GetModel();
}

CTransform& CTransform::operator=(const CTransform& _Other)
{
    m_v3Position = _Other.GetPosition();
    m_fquatRotation = _Other.GetRotation();
    m_v3Scale = _Other.GetScale();
    m_mat4Model = _Other.GetModel();

    return *this;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: UpdateModelMatrix()
//	 Purpose: Update the model matrix of the transform to incorporate the scale, position, and rotation

void CTransform::UpdateModelMatrix()
{
    m_mat4Model = glm::scale(glm::mat4x4(1), m_v3Scale);
    m_mat4Model = glm::translate(m_mat4Model, m_v3Position);
    m_mat4Model *= glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), Forward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetModel()
//	 Purpose: Gets the model matrix of the CTransform.
//	 Returns: The model matrix of the CTransform.

const glm::mat4x4 CTransform::GetModel() const
{
    return m_mat4Model;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetPosition()
//	 Purpose: Get the position of the transform
//	 Returns: The position of the transform

const glm::vec3 CTransform::GetPosition() const
{
    return m_v3Position;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetPosition()
//	 Purpose: Sets the new position of the transform, _v3Position

void CTransform::SetPosition(const glm::vec3 _v3Position)
{
    m_v3Position = _v3Position;
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Move()
//	 Purpose: Move the position of the transform by _v3Offset

void CTransform::Move(const glm::vec3 _v3Offset)
{
    m_v3Position += _v3Offset;
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetRotation()
//	 Purpose: Get the current quaternion rotation of the transform
//	 Returns: The current quaternion rotation of the transform

const glm::quat CTransform::GetRotation() const
{
    return m_fquatRotation;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetRotation()
//	 Purpose: Set the new rotation, _fquatRotation, to the transform
//	 Returns: The new rotation, _fquatRotation, to the transform

void CTransform::SetRotation(const glm::fquat _fquatRotation)
{
    m_fquatRotation = _fquatRotation;
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Rotate()
//	 Purpose: Rotate the transform by _fquatOffset

void CTransform::Rotate(const glm::fquat _fquatOffset)
{
    m_fquatRotation *= _fquatOffset;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetRotationEuler()
//	 Purpose: Get the rotation of the transform in euler angles. if _bInDegrees is true, the returned angles are in degrees, otherwise they are in radians. 
//	 Returns: The rotation of the transform in euler angles. if _bInDegrees is true, the returned angles are in degrees, otherwise they are in radians. 

const glm::vec3 CTransform::GetRotationEuler(bool _bInDegrees) const
{
    glm::vec3 v3Output = glm::eulerAngles(m_fquatRotation);
    if (_bInDegrees) v3Output = glm::degrees(v3Output);

    return v3Output;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetRotationEuler()
//	 Purpose: Set the rotation of the transform by euler angles, _v3Rotation. If _bInDegrees is true, the assigned angles is in degress, otherwise the angles is in radians. 

void CTransform::SetRotationEuler(const glm::vec3 _v3Rotation, bool _bInDegrees)
{
    m_fquatRotation = _bInDegrees ? glm::quat(glm::radians(_v3Rotation)) : glm::quat(_v3Rotation);
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: RotateEuler()
//	 Purpose: Rotate the transform by the euler angles, _v3Offset.  If _bInDegrees is true, the assigned angles is in degress, otherwise the angles is in radians. 

void CTransform::RotateEuler(const glm::vec3 _v3Offset, bool _bInDegrees)
{
    glm::quat fquatOffset = _bInDegrees ? glm::quat(glm::radians(_v3Offset)) : glm::quat(_v3Offset);
    m_fquatRotation *= fquatOffset;
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: LookAt()
//	 Purpose: Have the transform loop at a target, _v3Target

void CTransform::LookAt(const glm::vec3 _v3Target)
{
    m_fquatRotation = glm::quatLookAt(_v3Target - m_v3Position, glm::vec3(0.0f, 1.0f, 0.0f));
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetScale()
//	 Purpose: Get the scale of the transform
//	 Returns: The scale of the transform

const glm::vec3 CTransform::GetScale() const
{
    return m_v3Scale;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: SetScale()
//	 Purpose: Set the new scale of the transform, _v3Scale.

void CTransform::SetScale(const glm::vec3 _v3Scale)
{
    m_v3Scale = _v3Scale;
    UpdateModelMatrix();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Up()
//	 Purpose: Get the y axis of the transform in local space

const glm::vec3 CTransform::Up() const
{
    return m_fquatRotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Forward()
//	 Purpose: Get the z axis of the transform in local space

const glm::vec3 CTransform::Forward() const
{
    return m_fquatRotation * glm::vec3(0.0f, 0.0f, -1.0f);
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Right()
//	 Purpose: Get the x axis of the transform in local space

const glm::vec3 CTransform::Right() const
{
    return m_fquatRotation * glm::vec3(1.0f, 0.0f, 0.0f);
}