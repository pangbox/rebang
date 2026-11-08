#include "projectg.h"
#include "golfdoc.h"
#include "golfball.h"
#include "capturedbg.h"
#include "chatmsg.h"
#include "talkbox.h"
#include "bubble.h"

extern WMatrix g_camera;

IMPLEMENT_ACTOR(CBubble, IActor)

CBubble::CBubble()
{
	m_dispPriority = DISP_PRIORITY_4;
	m_bubble = NULL;
	m_bShow = true;

	AddSkipList(0x854c2);
}

void CBubble::OnDestroy()
{
	if (m_bubble)
	{
		for (int i = 0; i < 2; i++)
		{
			if (m_bubble[i].pTalkBox)
			{
				delete m_bubble[i].pTalkBox;
				m_bubble[i].pTalkBox = NULL;
			}
		}

		if (m_bubble)
		{
			delete[] m_bubble;
			m_bubble = NULL;
		}
	}
}

void CBubble::OnLoad()
{
	m_bubble = new sBubble[2];

	for (int i = 0; i < 2; i++)
	{
		m_bubble[i].pTalkBox = new CTalkBox;
		m_bubble[i].bTalking = false;
	}
}

void CBubble::OnProcess(float delta)
{
	if (!m_bShow)
		return;

	if (CCapturedBg::Instance()->IsCaptured())
		return;

	for (int i = 0; i < 2; i++)
	{
		if (m_bubble[i].pTalkBox->IsActive())
		{
			m_bubble[i].pTalkBox->SetTransparency(1.0f -
				Between(0.0f,
					((m_bubble[i].pos - g_camera.pivot).Magnitude() - 64.0f) /
						256.0f,
					1.0f));
			m_bubble[i].pTalkBox->Process(delta);
		}
	}

	if (!m_bubble[0].pTalkBox->IsActive() && m_bubble[0].bTalking)
	{
		m_bubble[0].bTalking = false;
		SendMsg(this, "Caddie", 477, 0, 0, 0, 0);
	}
}

void CBubble::OnDisplay()
{
	if (!m_bShow)
		return;

	if ((GOLFDOC()->m_gameMode & 0x200) ||
		CCapturedBg::Instance()->IsCaptured())
		return;

	if (m_bubble[1].pTalkBox->IsActive())
	{
		WVector pos = g_view->Projection(m_bubble[1].pos);

		if (0.0f < pos.z && pos.z < 1.0f)
			m_bubble[1].pTalkBox->Render(g_view, pos.x, pos.y, 1);
	}

	if (m_bubble[0].pTalkBox->IsActive())
	{
		WVector pos = g_view->Projection(m_bubble[0].pos);

		if (0.0f < pos.z && pos.z < 1.0f)
		{
			float offX, offY;
			int align;
			if (GolfBall().m_groundType == 2)
			{
				offX = -20.0f;
				offY = -10.0f;
				align = 2;
			}
			else
			{
				offX = 0.0f;
				offY = 0.0f;
				align = 1;
			}

			m_bubble[0].pTalkBox->Render(g_view, pos.x + offX, pos.y + offY,
				align);
		}
	}
}

void CBubble::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 19:
	{
		std::string str;
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].caddieInfo.tid);
		if (pCaddie)
		{
			str = pCaddie->c.Name;
			str += " : ";
			str += (const char*)msg.param1;
			CChatMsg::Instance()->AddChatMsg(str, 0xff00ff00, true, false);
		}
		else
		{
			str = (const char*)msg.param1;
		}

		m_bubble[0].pTalkBox->SetText(str.c_str(), 3.0f);
		m_bubble[0].pos = *(WVector*)msg.param2;
		m_bubble[0].bTalking = true;
		break;
	}

	case 501:
		m_bubble[1].pTalkBox->SetText((const char*)msg.param1, 3.0f);
		m_bubble[1].pos = *(WVector*)msg.param2;
		m_bubble[1].bTalking = true;
		break;

	case 355:
		m_bShow = true;
		break;

	case 356:
		m_bShow = false;
		break;
	}
}
