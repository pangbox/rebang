#pragma once

#include "actor.h"
#include "../../shared/mtrand1p1.h"

class NetSyncable
{
public:
	virtual void NSC_OnStep(bool bForce, bool bReplay, float step, float time,
		int frame) = 0;
	virtual void NSC_OnFirstNewShotFrame(float time, int frame) = 0;
	virtual void NSC_OnFirstReplayFrame(int frame) = 0;
};

class NetSyncActor : public IActor, public NetSyncable
{
	DECLARE_OBJECT(NetSyncActor)

public:
	NetSyncActor();
	virtual ~NetSyncActor();

	virtual void OnInit();
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	virtual void NSA_Step(bool bForce, float step, int frame);
	virtual void OnHoleOut();

	MTRand1p1& NSA_GetRandomGenerator() { return m_random; }

protected:
	void NSA_SetInitialRandomSeed(unsigned long seed) { m_initialSeed = seed; }

	float m_time;
	float m_shotStartTime;
	float m_syncStartTime;
	int m_lastFrame;
	int m_maxFrame;
	MTRand1p1 m_random;
	bool m_bLastForce;
	bool m_bReplay;
	unsigned long m_initialSeed;
};
