/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GameManaeger.cpp																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#include "GameManager.h"
#include "Camera.h"
#include "Main.h"
#include <iostream>

CGameManager* CGameManager::m_singleton = nullptr;

//------------------------------------------------------------------------------------------------------------------------
// Procedure: CGameManager()
//	 Purpose: Ensure that the game manager is a singeton

CGameManager::CGameManager()
{
	//Ensure that the game manager is a singleton class
	if (m_singleton == nullptr) { m_singleton = this; }
	else if (m_singleton != this) delete this;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: GetSingleton()
//	 Purpose: Gets the current game manager, if none exists create one
//	 Returns: The current game manager

CGameManager& CGameManager::GetSingleton()
{
	//Create a game manager if none is created yet
	if (m_singleton == nullptr)
	{
		std::cout << "ERROR: The Game manager class must be created before requesting it. A default game manager is created instead";
		m_singleton = new CGameManager();
	}

	//Get the game manager
	return *m_singleton;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: ~CGameManager()
//	 Purpose: Delete all stored game objects when the game manager is destroyed

CGameManager::~CGameManager()
{
	Clear();
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: DestroyImmediate()
//	 Purpose: DestroyImmediate is used for destroying the given updated object, _pUpdatedObject, imediately outside of the update loop.

const std::deque<CUpdatedObject*>& CGameManager::GetUpdatedObjects() const
{
	return m_updatedObjects;
}

void CGameManager::DestroyImmediate(CUpdatedObject*&& _updatedObject)
{
	//Check whether the given pointer is valid
	if (_updatedObject == nullptr)
	{
		std::cout << "ERROR: Can not use DestroyImmediate on nullptr";
		return;
	}

	//Find the requested object and delete it
	for (int i = 0; i < (int)m_updatedObjects.size(); i++)
	{
		if (m_updatedObjects[i] != _updatedObject) continue;

		m_updatedObjects.erase(m_updatedObjects.begin() + i);
		delete _updatedObject;

		return;
	}
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: DestroyImmediate()
//	 Purpose: DestroyImmediate is used for destroying the given updated object, _pUpdatedObject, imediately outside of the update loop.

void CGameManager::DestroyImmediate(CUpdatedObject*& _updatedObject)
{
	//Check whether the given pointer is valid
	if (_updatedObject == nullptr)
	{
		std::cout << "ERROR: Can not use DestroyImmediate on nullptr";
		return;
	}

	//Find the requested object and delete it
	for (int i = 0; i < (int)m_updatedObjects.size(); i++)
	{
		if (m_updatedObjects[i] != _updatedObject) continue;

		m_updatedObjects.erase(m_updatedObjects.begin() + i);
		delete _updatedObject;

		//Ensure that the given pointer is nullptr
		_updatedObject = nullptr;

		return;
	}
}

void CGameManager::DestroyImmediate(unsigned int _index)
{
	CUpdatedObject* pUpdatedObject = m_updatedObjects[_index];

	m_updatedObjects.erase(m_updatedObjects.begin() + _index);
	delete pUpdatedObject;
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Clear()
//	 Purpose: Delete all updated objects

void CGameManager::Clear()
{
	const int iGameObjectsCount = m_updatedObjects.size();
	for (int i = 0; i < iGameObjectsCount; i++)
	{
		delete m_updatedObjects.front();
		m_updatedObjects.pop_front();
	}
}

void CGameManager::UpdateObjectsInWorld()
{
	for (auto& gameObject : m_updatedObjects) gameObject->Start();
	for (auto& gameObject : m_updatedObjects) gameObject->BeginUpdate();
	for (auto& gameObject : m_updatedObjects) gameObject->Update();
	for (auto& gameObject : m_updatedObjects) gameObject->EndUpdate();
}

void CGameManager::DrawObjectsInWorld()
{
	for (auto& gameObject : m_updatedObjects) gameObject->Draw();
}

void CGameManager::DeleteObjectsInWorld()
{
	//Search all objects in the world
	for (int i = 0; i < (int)m_updatedObjects.size(); i++)
	{
		//Skip objects not maked for deletion
		if (!m_updatedObjects[i]->GetDeleteUpdatedObject()) continue;

		//Delete object
		DestroyImmediate(i);
	}
}

//------------------------------------------------------------------------------------------------------------------------
// Procedure: Update()
//	 Purpose: Called every game frame and updates all game objects

void CGameManager::Update()
{
	UpdateObjectsInWorld();
	DrawObjectsInWorld();
	DeleteObjectsInWorld();
}