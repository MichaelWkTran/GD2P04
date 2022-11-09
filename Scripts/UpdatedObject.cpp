/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: UpdateObject.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "UpdatedObject.h"
#include "GameManager.h"

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CUpdatedObject()
//	 Purpose: Initalise variables from the updated object

CUpdatedObject::CUpdatedObject()
{
	m_bStartCalled = false;
	m_bDeleteUpdatedObject = false;
	GetGameManager().m_updatedObjects.emplace_back(this);
}