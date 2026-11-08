#pragma once

#include "mailboxpage.h"
#include "taskmanager.h"

class CRealMyRoom_AvatarBurden;

class CRealMyRoomTask : public CTask
{
	DECLARE_OBJECT(CRealMyRoomTask)

protected:
	CRealMyRoomTask();
	virtual ~CRealMyRoomTask();

public:
	void AddAvatarBurden(const CRealMyRoom_AvatarBurden& burden);
	void RemoveAvatarBurden(const CRealMyRoom_AvatarBurden& burden);

protected:
	virtual void Register();
	virtual void Init(const char* wallPaper);
	virtual void PreLoadInit();
	virtual void Process(float delta);
	virtual void Display();
	virtual void Destroy();
	virtual int OnPacket(WReceivedPacket& packet);
	virtual void Load();
	virtual void RestorePreserved();
	virtual void OnAnotherHand(int key, void* param);

private:
	void DeleteBurdenPet(std::list<CRealMyRoom_AvatarBurden>& burdens,
		std::list<CRealMyRoom_AvatarBurden>& pets);

	std::list<CRealMyRoom_AvatarBurden> m_completeList;
	std::list<CRealMyRoom_AvatarBurden> m_requestList;
	std::list<CRealMyRoom_AvatarBurden> m_deleteList;
	_RTL_CRITICAL_SECTION m_cs;
	bool m_bExit;
};
