#include "projectg.h"
#include "woverlay.h"
#include "golfdoc.h"
#include "logo.h"

IMPLEMENT_ACTOR(CLogo, IActor)
CLogo::CLogo()
	: m_pLogo(NULL)
{
	m_dispPriority = DISP_PRIORITY_5;
}

void CLogo::OnDestroy()
{
	if (m_pLogo)
	{
		g_resrcmng->Release(m_pLogo);
		m_pLogo = NULL;
	}
}

void CLogo::OnLoad()
{
	if (GOLFDOC()->m_tutorialMode < 15)
		return;

	m_pLogo = g_resrcmng->GetOverlay(MakeStr("[logo_%d.png", GetCurMap()), 0);
}

void CLogo::OnInit()
{
	m_alpha = 0;
}

void CLogo::OnProcess(float delta)
{
	if (!m_pLogo)
		return;

	m_alpha = Min(1.0f, m_alpha + delta / 3.0f);
}

void CLogo::OnDisplay()
{
	if (!m_pLogo)
		return;

	WRect dst;
	dst.x = (int)((g_view->GetWidth() - m_pLogo->GetWidth()) * 0.5f);
	dst.y = 0;
	dst.w = m_pLogo->GetWidth();
	dst.h = m_pLogo->GetHeight();
	unsigned long color = ((unsigned char)(m_alpha * 255.0f) << 24) | 0xffffff;
	m_pLogo->Render(g_view, WRect(0, 0, 1, 1), dst, 0, color, 0, 0);
}

void CLogo::HandleMsg(const MsgObject& msg)
{
	if (msg.message == 0x18b)
	{
		OnDestroy();
	}
}
