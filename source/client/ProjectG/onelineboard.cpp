#include "onelineboard.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "golftask.h"
#include "capturedbg.h"

extern Fresh* g_pFresh;
enum
{
	FADE_NONE,
	FADE_OUT,
	FADE_IN,
};
COneLineBoard::COneLineBoard()
{
	m_fadeMode = FADE_NONE;
	m_waitMsgNum = 0;
	m_waitMsgTime = 0;
	m_pBgBitmap = NULL;
	m_bShowBg = true;
	m_fadeFactor = 1.0f;
	m_bAnimate = true;
}

COneLineBoard::~COneLineBoard()
{
	Clear();
}

void COneLineBoard::Clear()
{
	sequence_delete(m_msgList.begin(), m_msgList.end());
	m_msgList.clear();
}

void COneLineBoard::Process(float delta)
{
	if (m_fadeMode == FADE_OUT)
	{
		m_fadeFactor -= delta;

		if (m_fadeFactor < 0.0f)
			m_fadeFactor = 0.0f;
	}
	else if (m_fadeMode == FADE_IN)
	{
		m_fadeFactor += delta;

		if (m_fadeFactor > 1.0f)
		{
			m_fadeFactor = 1.0f;
			m_fadeMode = FADE_NONE;
		}
	}

	if (m_fadeMode != FADE_NONE)
		return;
	static DWORD lastTime = 0;
	DWORD time = GetTickCount();
	if (time - lastTime < 25)
		return;

	lastTime = time;

	m_bAnimate = true;

	if (CCapturedBg::Instance() && CCapturedBg::Instance()->IsCaptured())
	{
		m_bAnimate = false;
		return;
	}

	for (m_it = m_msgList.begin(); m_it != m_msgList.end();)
	{
		sOnelineMsg* pMsg = *m_it;

		pMsg->x -= 0.5f;

		if (pMsg->x + pMsg->width < m_rect.x)
		{
			delete pMsg;
			m_msgList.erase(m_it++);
		}
		else
		{
			++m_it;
		}
	}
}

void COneLineBoard::Display()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		DrawBg();

	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();

	float alpha = pDevice->GetAlpha();

	pDevice->SetAlpha(m_fadeFactor);

	if (IS_EXACTKINDOF(CGolfTask, AfxGetTask()))
	{
		pDevice->SetTextStyle(4);
		pDevice->SetTextColor(0xffffffff, 0xff000000);
	}
	else
	{
		pDevice->SetTextStyle(0);
		pDevice->SetTextColor(0xff000000, 0xffffffff);
	}

	FrEmoticon* pEmoticon = g_pFresh->GetManager()->GetEmoticon();
	WRect clip(m_rect.x, m_rect.y - 10.0f, m_rect.w, m_rect.h + 20.0f);
	pEmoticon->SetClippingArea(&clip);

	for (m_it = m_msgList.begin(); m_it != m_msgList.end(); ++m_it)
	{
		sOnelineMsg* pMsg = *m_it;

		if (m_bAnimate)
			pEmoticon->SetAnim(true);

		pEmoticon->SetAnimTime(pMsg->time);
		pEmoticon->PrintText(WPoint((float)(int)pMsg->x, m_rect.y + 5.0f), 0,
			pMsg->msg.c_str(), 0xffffffff);
		pEmoticon->SetAnim(false);
	}

	pEmoticon->SetClippingArea(NULL);

	pDevice->SetAlpha(alpha);
	pDevice->SetTextColor(0xffffffff, 0xffffffff);
	pDevice->SetTextStyle(0);
}

void COneLineBoard::AddOnelineMsg(const std::string& nick,
	const std::string& msg)
{
	FrEmoticon* pEmoticon = g_pFresh->GetManager()->GetEmoticon();
	std::list<sOnelineMsg*>::reverse_iterator it;

	sOnelineMsg* pMsg = new sOnelineMsg;
	pMsg->msg = msg + "  <" + nick + ">";
	pMsg->width = pEmoticon->GetTextWidth(pMsg->msg.c_str());

	if (!m_msgList.empty())
	{
		it = m_msgList.rbegin();
		sOnelineMsg* pLast = *it;

		pMsg->x = max(pLast->x + pLast->width + 50.0f, m_rect.Right());
	}
	else
	{
		pMsg->x = max(0.0f, m_rect.Right());
	}

	pMsg->time = timeGetTime();

	m_msgList.push_back(pMsg);
}

void COneLineBoard::LoadBg()
{
	if (m_pBgBitmap == NULL)
		m_pBgBitmap = g_pFresh->GetBitmap("oneline_back");
}

void COneLineBoard::FadeOut()
{
	m_fadeMode = FADE_OUT;
}

void COneLineBoard::FadeIn()
{
	m_fadeMode = FADE_IN;
}

float COneLineBoard::GetFadeFactor()
{
	if (m_fadeMode == FADE_NONE)
		return 1.0f;

	return m_fadeFactor;
}

void COneLineBoard::DrawBg()
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (!m_bShowBg)
			return;

		if (CCapturedBg::Instance()->IsCaptured())
			return;
	}

	if (m_pBgBitmap == NULL)
		return;
	FrGraphicInterface* pDevice = g_pFresh->GetManager()->GetGDI();
	if (pDevice == NULL)
		return;

	pDevice->DrawTexture(m_pBgBitmap,
		WRect(0.0f, 0.0f, (float)m_pBgBitmap->Width(),
			(float)m_pBgBitmap->Height()),
		WRect(m_rect.x - 2.0f, m_rect.y, (float)m_pBgBitmap->Width(),
			(float)m_pBgBitmap->Height()),
		0xffffffff, false);
}
