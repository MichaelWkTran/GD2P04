/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: GameManager.h																									*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include <deque>
#include "UpdatedObject.h"

/*
CGameManager managages the game rules. This includes the creation and deleting of updated objects as well as updating the objects every frame.
Though this object have be used on its own, it have be used to make subclasses that add rules such as score. 
*/
class CGameManager
{
	friend CUpdatedObject;

private:
	static CGameManager* m_singleton;

protected:
	std::deque<CUpdatedObject*> m_updatedObjects;
	~CGameManager();

public:
	//Game Manager Methods
	CGameManager();
	CGameManager(const CGameManager&) = delete;
	CGameManager& operator= (const CGameManager&) = delete;
	
	const std::deque<CUpdatedObject*>& GetUpdatedObjects() const;
	void DestroyImmediate(CUpdatedObject*&& _updatedObject);
	void DestroyImmediate(CUpdatedObject*& _updatedObject);
	void DestroyImmediate(unsigned int _index);
	void Clear();

	void UpdateObjectsInWorld();
	void DrawObjectsInWorld();
	void DeleteObjectsInWorld();

	virtual void Update();

	//Get Set Methods
	static CGameManager& GetSingleton();
};

#define GetGameManager CGameManager::GetSingleton