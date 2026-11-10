#pragma once

#include <map>
#include "actor.h"

class CSimpleUI;
class WOverlay;

class CItemWindow : public IActor
{
protected:
	CItemWindow();

public:
	virtual ~CItemWindow() { }

	DECLARE_OBJECT(CItemWindow)

	enum eState
	{
		STATE_OPENED,
		STATE_OPENING,
		STATE_CLOSING,
		STATE_CLOSED
	};

	virtual void OnLoad();
	virtual void OnInit();
	virtual void OnProcess(float delta);
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	void ResetUiPosition();
	static bool IsClosed();

protected:
	void ResetVars();
	void ExecuteNow(bool bSound);
	void OnOpened(float delta);
	void OnClosed(float delta);
	void OnLeft(float delta);
	void OnRight(float delta);
	void CheckInput();
	void Draw();
	unsigned long* GetTidItemSlot(unsigned char player);
	bool IsUsable(unsigned long typeId);
	bool UseItem(int slot);
	void NetUseItem(unsigned long uid, unsigned long typeId,
		unsigned long random);
	bool ProcessItem(unsigned char player, unsigned long typeId, bool bLocal,
		unsigned long random);

	static eState ms_state;

	std::map<unsigned long, WOverlay*> m_iconMap;
	WOverlay* m_pWindow;
	WRect m_dest;
	WRect m_clip;
	float m_pos;
	float m_openPos;
	float m_velocity;
	WPoint m_mouse;
	unsigned char m_failFrame;
	CSimpleUI* m_pFailUI;
	int m_overSlot;
	int m_useSlot;
	int m_selSlot;
	unsigned long m_usedTypeId;
	float m_reserved74;
	float m_reserved78;
	float m_effectTime;
	WPoint m_effectPos;
	WPoint m_effectVel;
	bool m_bEffect;
	bool m_bFailed;
	bool m_bEnable;
	bool m_bInputEnable;
};
