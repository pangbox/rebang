#pragma once

#include <windows.h>
#include <list>

#include "taskmanager.h"

class ntAvatarBurden;

class ntAvatarChatTask : public CTask
{
	DECLARE_OBJECT(ntAvatarChatTask)

public:
	void ShowQuitDialog(void);
	void AddAvatarBurden(const ntAvatarBurden& burden);
	void RemoveAvatarBurden(const ntAvatarBurden& burden);

protected:
	ntAvatarChatTask(void);
	virtual ~ntAvatarChatTask(void);

	virtual void Register(void);
	virtual void Init(const char* backGround);
	virtual void PreLoadInit(void);
	virtual void Process(float dt);
	virtual void Display(void);
	virtual void Destroy(void);
	virtual int OnPacket(WReceivedPacket& packet);
	virtual void Load(void);
	virtual void PreserveBack(const char*);
	virtual void RestorePreserved(void);
	virtual void OnAnotherHand(int nWork, void* pData);

private:
	void DeleteBurdenPet(std::list<ntAvatarBurden>&,
		std::list<ntAvatarBurden>&);
	void KeepUniqueCompleteList(const ntAvatarBurden& burden);

	std::list<ntAvatarBurden> m_completeList;
	std::list<ntAvatarBurden> m_addList;
	std::list<ntAvatarBurden> m_removeList;
	CRITICAL_SECTION m_burdenLock;
	bool m_destroying;
};
