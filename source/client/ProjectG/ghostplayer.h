#pragma once

#include <list>

#include "actor.h"
#include "frcmdtarget.h"

class CGhostDocument;
class CGhostHoleData;
class CSequenceStateMachine;
class CStateUnit;
class FrForm;
class FrGhostAward;
class MsgObject;
class WOverlay;
struct GHOST_ACTION;
struct GHOST_SHOT_DATA;

class CGhostPlayer : public IActor, public FrCmdTarget
{
public:
	DECLARE_OBJECT(CGhostPlayer)

	class cMovieStyleDisplay
	{
	public:
		cMovieStyleDisplay(void);
		~cMovieStyleDisplay(void);

		void ResetHeight(void);
		void Process(float fDelta);
		void Render(void);

		float m_height;
		WOverlay* m_pOverlay;
	};

	CGhostPlayer(void);
	virtual ~CGhostPlayer(void);

	virtual void OnLoad(void);
	virtual void OnInit(void);
	virtual void OnReload(void);
	virtual void OnProcess(float fDelta);
	virtual void OnDisplay(void);
	virtual void OnDestroy(void);

protected:
	virtual void HandleMsg(const MsgObject& msg);

private:
	CStateUnit* CreateStateUnit(int type, GHOST_ACTION const* pAction);
	bool DoShot(GHOST_SHOT_DATA const* pShot);
	void SetWindAndWeather(unsigned char holeNum);
	void UseItem(unsigned long itemTypeId, unsigned long itemId);
	void SendAwardResult(void);
	bool ExceptionAlarmWaitCallBack(int result, FrForm* pForm);

	CGhostDocument* m_pGhostDoc;
	CGhostHoleData const* m_pHoleData;
	std::list<GHOST_SHOT_DATA*> m_shots;
	CSequenceStateMachine* m_pStateMachine;
	FrGhostAward* m_pAward;
	cMovieStyleDisplay* m_pMovieStyle;
	int m_totalShotCount;
	unsigned int m_holeShotCount;
	int m_errorCode;
	int m_shooting;
	int m_awardSent;
	unsigned long m_serverProperty;
};
