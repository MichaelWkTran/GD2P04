/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: UpdatedObject.h																								*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
class CGameManager;

class CUpdatedObject
{
private:
	bool m_bStartCalled;
	bool m_bDeleteUpdatedObject;

public:
	CUpdatedObject();
	
	const bool GetDeleteUpdatedObject() const
	{
		return m_bDeleteUpdatedObject;
	}
	void DeleteUpdatedObject()
	{
		m_bDeleteUpdatedObject = true;
	}

	virtual void Start()
	{
		if (m_bStartCalled) return;
		m_bStartCalled = true;
	};
	virtual void BeginUpdate() {}
	virtual void Update() {}
	virtual void EndUpdate() {}
	virtual void Draw() {};
};