#include "noticeboard.h"
#include "chatmsg.h"
#include "projectg.h"
#include "golftask.h"
#include "onelineboard.h"
#include "binstr.h"

CNoticeBoard::CNoticeBoard()
{
	m_pBoardLeft = g_resrcmng->GetOverlay("notice_l.tga", 0);
	m_pBoardMiddle = g_resrcmng->GetOverlay("notice_m.tga", 0);
	m_pBoardRight = g_resrcmng->GetOverlay("notice_r.tga", 0);
	m_pFont = CChatMsg::Instance()->GetMaskedFont();
	m_curNotice = m_noticeList.end();
	m_textPosX = -5555.0f;
	m_boardPosY = -50.0f;
}

CNoticeBoard::~CNoticeBoard()
{
	m_curNotice = m_noticeList.end();
	sequence_delete(m_noticeList.begin(), m_noticeList.end());
	if (g_resrcmng && m_pBoardLeft)
	{
		g_resrcmng->Release(m_pBoardLeft);
		m_pBoardLeft = NULL;
	}
	if (g_resrcmng && m_pBoardMiddle)
	{
		g_resrcmng->Release(m_pBoardMiddle);
		m_pBoardMiddle = NULL;
	}
	if (g_resrcmng && m_pBoardRight)
	{
		g_resrcmng->Release(m_pBoardRight);
		m_pBoardRight = NULL;
	}
}

void CNoticeBoard::Process(float delta)
{
	delta /= CProjectG::Instance()->GetGameSpeed();
	if (m_curNotice == m_noticeList.end())
	{
		m_curNotice = m_noticeList.begin();
		if (m_curNotice == m_noticeList.end())
		{
			if (m_boardPosY > -50.0f)
			{
				m_boardPosY -= delta * 30.0f;
				if (m_boardPosY < -50.0f)
					m_boardPosY = -50.0f;
			}
			if (IS_KINDOF(CGolfTask, AfxGetTask()))
				COneLineBoard::Instance()->FadeIn();
			return;
		}
	}
	if (m_boardPosY < -0.1f)
	{
		m_boardPosY += delta * 30.0f;
		if (!(m_boardPosY > 0.0f))
			return;
	}
	m_boardPosY = 0.0f;
	sNotice* notice = *m_curNotice;
	if (m_textPosX < -5000.0f)
		m_textPosX = g_view->GetWidth() * 0.5f + 150.0f;
	m_textPosX -= delta * 30.0f;
	if (m_textPosX + notice->width < g_view->GetWidth() * 0.5f - 150.0f)
	{
		m_textPosX = g_view->GetWidth() * 0.5f + 150.0f;
		--notice->count;
		if (notice->count < 1)
		{
			delete *m_curNotice;
			m_curNotice = m_noticeList.erase(m_curNotice);
		}
		else
			++m_curNotice;
	}
}

void CNoticeBoard::Display()
{
	if (m_pBoardLeft && m_pBoardMiddle && m_pBoardRight)
	{
		float alpha = (m_boardPosY + 50.0f) * 0.02f;
		if (!WisEqual(alpha, 0.0f, g_EPSILON))
		{
			WRect src(0, 0, 1, 1);
			WRect dst(g_view->GetWidth() * 0.5f - 150.0f - 20.0f, m_boardPosY,
				m_pBoardLeft->GetWidth(), m_pBoardLeft->GetHeight());
			m_pBoardLeft->Render(g_view, src, dst, 0, 0xa0ffffff, 0, 0);
			dst.x += dst.w;
			dst.w = 300.0f - m_pBoardRight->GetWidth() + 20.0f;
			dst.h = (float)m_pBoardMiddle->GetHeight();
			m_pBoardMiddle->Render(g_view, src, dst, 0, 0xa0ffffff, 0, 0);
			dst.x += dst.w;
			dst.w = (float)m_pBoardRight->GetWidth();
			dst.h = (float)m_pBoardRight->GetHeight();
			m_pBoardRight->Render(g_view, src, dst, 0, 0xa0ffffff, 0, 0);
		}
	}
	if (m_curNotice != m_noticeList.end())
	{
		sNotice* notice = *m_curNotice;
		if (m_pFont)
		{
			WRect clip(g_view->GetWidth() * 0.5f - 150.0f, 0.0f, 300.0f, 40);
			m_pFont->SetClippingArea(&clip);
			int x = (int)m_textPosX;
			m_pFont->Print(g_view, (float)x, 7.0f, notice->text.c_str(),
				0x80000, notice->color, NULL);
			m_pFont->Flush(g_view);
			m_pFont->SetClippingArea(NULL);
		}
	}
}

bool CNoticeBoard::GetColornText(const char* src, std::string& text,
	unsigned long& color)
{
	color = 0xffffffff;
	cTokenV token;
	char* prefix;
	bool more;
	if (token.Init(src, strlen(src)))
	{
		more = token.GetTokenFullSep(&prefix, "\\c", 2);
		if ((prefix && *prefix) || more)
		{
			if (more)
			{
				char* colorText;
				token.GetTokenFullSep(&colorText, "\\c", 2);
				unsigned long parsedColor = 0xffffffff;
				cTokenV colors;
				if (colors.Init(colorText, strlen(colorText)))
				{
					char* component;
					do
					{
						more = colors.GetToken(&component, ",", 1);
						if (component && *component && strlen(component) > 1)
							sscanf(component, "%x", &parsedColor);
					} while (more);
				}
				color = parsedColor;
				std::string source(src);
				text = source.substr(source.rfind("\\c") + 2);
			}
			else
				text = src;
			return true;
		}
	}
	return false;
}

void CNoticeBoard::AddNotice(const char* text, int count, bool bImmediately)
{
	sNotice* notice = new sNotice;
	notice->color = 0xffffffff;
	if (!GetColornText(text, notice->text, notice->color))
		notice->text = text;
	notice->count = count;
	notice->width = m_pFont->GetTextWidth(g_view, text);
	int size = m_noticeList.size();
	m_noticeList.push_back(notice);
	if (size == 0 && m_noticeList.size() == 1 && m_boardPosY < -0.1f)
		g_audio->PlaySfx("notice_dingdong", NULL, 0, NULL, NULL, 0.5f, 200.0f);
	if (bImmediately)
	{
		m_curNotice = m_noticeList.end();
		--m_curNotice;
		m_textPosX = g_view->GetWidth() * 0.5f + 150.0f;
	}
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		COneLineBoard::Instance()->FadeOut();
}
