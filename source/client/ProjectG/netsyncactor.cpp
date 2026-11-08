#include "netsyncactor.h"
#include "club.h"

static bool IsZero(float f)
{
	return f < g_EPSILON;
}

const WRTTI NetSyncActor::m_RTTI("NetSyncActor", &IActor::m_RTTI);

NetSyncActor::NetSyncActor()
	: m_bLastForce(false), m_bReplay(false), m_initialSeed(0)
{
	m_syncStartTime = m_shotStartTime = m_time = 0.0f;

	m_maxFrame = m_lastFrame = -1;

	int* pFrame = &m_lastFrame;
	*pFrame = 999999999;
}

NetSyncActor::~NetSyncActor()
{
}

void NetSyncActor::OnInit()
{
}

void NetSyncActor::OnDestroy()
{
}

void NetSyncActor::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x278:
	{
		float delay;
		SendMsg(this, "Player", 0x126, (int)&delay, 0, 0, 0);
		float time = delay + m_time;

		if (GolfClub().PowerShotType() == 0)
			time += 1.0f;

		((WSendPacket*)msg.param1)->Encode4(*(unsigned long*)&time);

		if (!OnlinePlay())
			m_syncStartTime = time;
	}
	break;

	case 0x279:
	{
		unsigned long value = ((WReceivedPacket*)msg.param1)->Decode4();
		m_syncStartTime = *(float*)&value;
	}
	break;

	case 0x27a:
		OnHoleOut();
		break;
	}
}

void NetSyncActor::NSA_Step(bool bForce, float step, int frame)
{
	if (m_lastFrame == 0 && frame == 0)
		return;

	if (!m_bLastForce)
	{
		if (bForce)
		{
			if (frame == 0)
			{
				if (m_lastFrame == -1)
				{
					m_shotStartTime = m_syncStartTime;
					NSC_OnFirstNewShotFrame(m_shotStartTime, frame);
				}
				else
				{
					m_bReplay = true;
					NSC_OnFirstReplayFrame(frame);
				}
			}
			else
			{
				m_bReplay = true;
				NSC_OnFirstReplayFrame(frame);
			}
		}
	}
	else
	{
		if (!bForce)
		{
			m_bReplay = false;
		}
		else
		{
			if (frame <= m_maxFrame)
			{
				m_bReplay = true;
				if (frame != m_lastFrame + 1)
					NSC_OnFirstReplayFrame(frame);
			}
			else
			{
				m_bReplay = false;
			}
		}
	}

	if (bForce)
		m_time = m_shotStartTime + frame * step;
	else
		m_time += step;

	NSC_OnStep(bForce, m_bReplay, step, m_time, frame);

	m_bLastForce = bForce;
	m_lastFrame = frame;
	if (frame == -1 || m_maxFrame < frame)
		m_maxFrame = frame;
}

void NetSyncActor::OnHoleOut()
{
}
