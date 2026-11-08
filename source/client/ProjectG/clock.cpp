#include "projectg.h"
#include "golfdoc.h"
#include "clock.h"

IMPLEMENT_ACTOR(CClock, IActor)

CClock::CClock()
{
	AddSkipList(0x84400);
}

void CClock::OnInit()
{
	Reset();
}

void CClock::OnProcess(float delta)
{
	if (GOLFDOC()->m_bPause || GOLFDOC()->m_bStopDlg)
		return;

	if (GOLFDOC()->m_tutorialMode < 15)
		return;

	GOLFDOC()->m_shotTime += (int)(delta * 1000.0f);

	if (Doc()->m_golfGame.gameType == 10)
	{
		if (Doc()->m_approachStartTime <= 0)
			return;
		if (g_CurrentTime >
			Doc()->m_approachStartTime + Doc()->m_golfGame.gameTimeLimit)
		{
			if (!m_bActive)
				return;
			m_bActive = false;
			g_audio->PlaySfx("\xc5\xb8\xc0\xd3", NULL, 1, "\xc5\xb8\xc0\xd3");

			g_audio->PlaySfx("approach_bell");
		}
		else if (g_CurrentTime > Doc()->m_approachStartTime +
				Doc()->m_golfGame.gameTimeLimit - 10000)
		{
			if (m_bActive)
				return;
			m_bActive = true;
			g_audio->PlaySfx("\xc5\xb8\xc0\xd3", NULL, 1, "\xc5\xb8\xc0\xd3");
		}
		return;
	}

	if (!m_bActive || !Doc()->m_golfGame.shotTimeLimit)
		return;

	if (GOLFDOC()->m_remainTime > (unsigned long)(delta * 1000.0f))
		GOLFDOC()->m_remainTime -= (unsigned long)(delta * 1000.0f);
	else
		GOLFDOC()->m_remainTime = 0;

	m_angle = ((1.0f -
				   (float)GOLFDOC()->m_remainTime /
					   (float)Doc()->m_golfGame.shotTimeLimit) *
					  278.0f +
				  12.0f) *
		g_DEGTORAD;

	if (GOLFDOC()->m_remainTime < 10000)
	{
		if (!GOLFDOC()->m_shotPhase && !g_audio->IsPlaying("\xc5\xb8\xc0\xd3"))
			g_audio->PlaySfx("\xc5\xb8\xc0\xd3", NULL, 1, "\xc5\xb8\xc0\xd3");
	}

	if (GOLFDOC()->m_remainTime > 20000)
	{
		m_color = 0xff147eff;
	}
	else if (GOLFDOC()->m_remainTime > 15000)
	{
		m_color = ULblend(0xff147eff, 0xfff014ff,
			(unsigned char)((20000 - GOLFDOC()->m_remainTime) * 255 / 5000));
	}
	else if (GOLFDOC()->m_remainTime > 10000)
	{
		m_color = 0xfff014ff;
	}
	else if (GOLFDOC()->m_remainTime > 5000)
	{
		m_color = ULblend(0xfff014ff, 0xffff1450,
			(unsigned char)((10000 - GOLFDOC()->m_remainTime) * 255 / 5000));
	}
	else
	{
		m_color = 0xffff1450;
	}

	SendMsg(this, "Screen", 425, m_color, (int)&m_angle, 0, 0);

	if (GOLFDOC()->m_shotPhase <= 0)

		if (!GOLFDOC()->m_remainTime)
		{
			m_bActive = false;

			if (!OnlinePlay())
			{
				Reset();
				SendMsg(this, "GolfRule", 120, 0x4000, 0, 0, 0);
			}
		}
}

void CClock::Reset()
{
	GOLFDOC()->m_remainTime = Doc()->m_golfGame.shotTimeLimit;
	GOLFDOC()->m_shotTime = 0;
	m_angle = 12.0f * g_DEGTORAD;
	m_color = 0xff147eff;
	m_bActive = false;

	SendMsg(this, "Screen", 425, m_color, (int)&m_angle, 0, 0);

	g_audio->StopSfx("\xc5\xb8\xc0\xd3");
}

void CClock::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x271a:
		CClock::OnInit();
		break;

	case 0x1e9:
		m_bActive = true;
		if (OnlinePlay() && GetShotTimeLimit() &&
			!(bool)((Doc()->m_myInfo.info.dwIdentity >> 1) & 1) &&
			IsMyTurn(true))
		{
			WSendPacket packet((enumClientPacket)0x22);
			packet.Send((eSendTo)0);
		}
		break;

	case 0x1ea:
		m_bActive = false;
		break;
	}
}
