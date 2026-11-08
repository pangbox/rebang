#include "projectg.h"
#include "wresrcmng.h"
#include "wfont.h"
#include "woverlay.h"
#include "chatmsg.h"
#include "talkbox.h"

CTalkBox::CTalkBox()
{
	m_pFont = NULL;
	m_pOverlay = NULL;
	m_transparency = 1.0f;
	m_maxLineLen = 30;

	m_pOverlay = g_resrcmng->GetOverlay("[chat_box.jpg", 0);

	m_pFont = CChatMsg::Instance()->GetMaskedFont();
}

CTalkBox::~CTalkBox()
{
	if (g_resrcmng && m_pOverlay)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}
	m_lines.clear();
}

void CTalkBox::SetText(const char* text, float time)
{
	std::string str;

	if (text && *text)
	{
		m_lines.clear();

		m_lineNum = 0;

		strcpy(m_text, text);

		sLine line;
		line.color = 0;
		line.text[0] = 0;
		int i = 0;
		int len = 0;
		while (m_text[i])
		{
			bool bHan = false;

			if (m_text[i] == '/')
			{
				i++;

				if (m_text[i] == 'c')
				{
					i++;
					line.color = 0;

					for (int shift = 20; shift >= 0; shift -= 4, i++)
					{
						char c = m_text[i];
						if (c >= '0' && c <= '9')
							line.color |= (c - '0') << shift;
						else if (c >= 'A' && c <= 'F')
							line.color |= (c - 'A' + 10) << shift;
						else if (c >= 'a' && c <= 'f')
							line.color |= (c - 'a' + 10) << shift;
					}
					line.newLine = 0;
				}
				else if (m_text[i] == 'n')
				{
					i++;
					m_lineNum++;
					line.newLine = 1;
				}
				line.text[len] = 0;
				line.width = m_pFont->GetTextWidth(g_view, line.text);
				m_lines.push_back(line);
				len = 0;
				str += line.text;
			}
			else
			{
				if (m_text[i] & 0x80)
				{
					bHan = true;
					line.text[len++] = m_text[i++];
				}
				line.text[len++] = m_text[i++];
				if (len < m_maxLineLen && m_text[i])
					continue;
				if (m_text[i] == ' ')
				{
					line.text[len++] = ' ';
					i++;
				}
				else if (bHan && len >= m_maxLineLen)
				{
					i -= 2;
					line.text[len - 2] = 0;
				}
				line.text[len] = 0;
				line.width = m_pFont->GetTextWidth(g_view, line.text);
				line.newLine = 1;
				m_lines.push_back(line);
				m_lineNum++;
				len = 0;
				str += line.text;
			}
		}

		m_time = time;
	}
}

void CTalkBox::Process(float elapsed)
{
	m_time -= elapsed;
}

void CTalkBox::Render(WView* view, float x, float y, int align)
{
	if (m_lines.size() == 0 || m_transparency == 0.0f)
		return;

	unsigned long color = 0;
	unsigned long alpha = (int)(m_transparency * 255.0f) << 24;
	float maxWidth = 0.0f;
	float width = 0.0f;
	std::list<sLine>::iterator it;
	for (it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		width += (*it).width;
		if ((*it).newLine)
		{
			if (maxWidth < width)
				maxWidth = width;
			width = 0.0f;
		}
	}
	WRect uv;
	WRect tail;
	tail = WRect(0.0f, 0.0f, 10.0f, 20.0f);
	WPoint pos;
	float shear;
	switch (align)
	{
	case 0:
		pos.x = x;
		tail.x = x + 6.0f;
		shear = -(tail.w * 0.5f + 6.0f);
		break;
	case 1:
		pos.x = x - 6.0f - maxWidth * 0.5f;
		tail.x = x - tail.w * 0.5f;
		shear = 0.0f;
		break;
	case 2:
		pos.x = x - 12.0f - maxWidth;
		tail.x = x - 6.0f - tail.w;
		shear = tail.w * 0.5f + 6.0f;
		break;
	default:
		shear = 0.0f;
		break;
	}
	float height;
	float lineGap;
	float textTop;
	switch (m_lineNum)
	{
	case 1:
		uv = WRect(0.0f, 0.0f, 0.5f, 0.234375f);
		height = 30.0f;
		textTop = 10.0f;
		lineGap = 0.0f;
		break;
	case 2:
		height = 40.0f;
		textTop = 7.0f;
		lineGap = 6.0f;
		uv = WRect(0.0f, 0.375f, 0.5f, 0.3125f);
		break;
	case 3:
		height = 48.0f;
		textTop = 4.0f;
		lineGap = 5.0f;
		uv = WRect(0.5f, 0.0f, 0.5f, 0.375f);
		break;
	default:
		height = 30.0f;
		textTop = 10.0f;
		lineGap = 0.0f;
		break;
	}
	tail.y = y - tail.h;
	pos.y = tail.y - height + 1.0f;
	m_pos = pos;
	float lineHeight = lineGap + 10.0f;
	unsigned long diffuse = alpha | 0xffffff;
	m_pOverlay->Render(view, WRect(uv.x, uv.y, 0.046875f, uv.h),
		WRect(pos.x, pos.y, 6.0f, height), 0x80000, diffuse, 0, 0);
	m_pOverlay->Render(view,
		WRect(uv.x + 0.046875f, uv.y, uv.w - 0.09375f, uv.h),
		WRect(pos.x + 6.0f, pos.y, maxWidth, height), 0x80000, diffuse, 0, 0);
	m_pOverlay->Render(view,
		WRect(uv.w + uv.x - 0.046875f, uv.y, 0.046875f, uv.h),
		WRect(pos.x + maxWidth + 6.0f, pos.y, 6.0f, height), 0x80000, diffuse,
		0, 0);
	m_pOverlay->RenderWithShear(view,
		WRect(0.5f, 0.375f, tail.w * 0.0078125f, tail.h * 0.0078125f), tail,
		shear, 0x80000, diffuse);
	WPoint pt;
	pt.x = pos.x + 6.0f;
	pt.y = pos.y + textTop;
	for (it = m_lines.begin(); it != m_lines.end(); ++it)
	{
		m_pFont->Print(view, pt.x, pt.y, (*it).text, 0, alpha | color, 0);
		color = (*it).color;
		if ((*it).newLine)
		{
			pt.x = pos.x + 6.0f;
			pt.y += lineHeight;
		}
		else
			pt.x += (*it).width;
	}
	m_pFont->Flush(view);
}
