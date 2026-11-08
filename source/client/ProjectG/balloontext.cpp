#include "mathconsts.h"
#include "projectg.h"
#include "balloontext.h"
#include "avatar_avatar.h"
#include "ntavatar.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"

extern Fresh* g_pFresh;

int g_balloonEmoticonEnd = ')';

ntBalloonText::ntBalloonText(unsigned long uid, const char* text,
	const float* const color, float lifeTime)
{
	m_uid = uid;
	m_bUpdated = false;
	m_x = 0;
	m_color[0] = color[0];
	m_color[1] = color[1];
	m_color[2] = color[2];
	m_color[3] = color[3];
	m_lifeTime = lifeTime;
	m_elapsed = 0.f;
	SetText(text);
}

ntBalloonText::~ntBalloonText()
{
}

void ntBalloonText::SetText(const char* text)
{
	if (text && *text)
	{
		const char* start;
		if (*text == '!')
		{
			start = text + 1;
			m_bThink = true;
		}
		else
		{
			start = text;
			m_bThink = false;
		}
		const char* stripped = StripSpecialText(start);
		if (*stripped)
		{
			m_bUpdated = true;
			m_text = stripped;
			m_elapsed = 0.f;
		}
	}
}

const char* ntBalloonText::StripSpecialText(const char* text) const
{
	static char buffer[1024];
	int length = strlen(text);
	int i = 0;
	int count = 0;
	while (i < length)
	{
		char c = text[i++];
		if (c == '/')
		{
			i++;
			while ((unsigned char)text[i] > ' ')
				i++;
			i++;
		}
		else
			buffer[count++] = c;
	}
	buffer[count] = 0;
	return buffer;
}

bool ntBalloonText::Process(float delta)
{
	m_elapsed += delta;
	return IsVisible();
}

void ntBalloonText::Render(WView* view)
{
	char name[32];
	std::string lines[32];
	ntAvatar* avatar = ntAvatarManager::Instance()->FindAvatarByGuid(m_uid);
	if (!avatar)
		return;
	int headX, headY;
	if (!avatar->GetBoneScreenCoord("Bip01 HeadNub", &headX, &headY))
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrEmoticon* emoticon = g_pFresh->GetManager()->GetEmoticon();
	emoticon->SetAnim(true);
	if (!IsVisible())
	{
		if (gdi)
			gdi->SetScale(1.f);
		return;
	}
	float scale;
	if (m_elapsed < .1f)
		scale = m_elapsed * 10.f;
	else if (m_lifeTime - m_elapsed < .1f)
		scale = (m_lifeTime - m_elapsed) * 10.f;
	else
		scale = 1.f;
	int lineCount = 0;
	int maxWidth = 0;
	int i = 0;
	int count = 0;
	int length = strlen(m_text.c_str());
	bool leadByte = false;
	bool icon = false;
	int iconLength = 0;
	char buffer[1024];
	buffer[0] = 0;
	if (length > 0)
	{
		while (i < length)
		{
			char c = m_text.c_str()[i++];
			if (c == '\\')
			{
				c = m_text.c_str()[i++];
				if (c == 'n')
					lineCount++;
				else if (!c)
					break;
			}
			else
			{
				if (c == '(')
				{
					const char* end =
						strchr(m_text.c_str() + i, g_balloonEmoticonEnd);
					unsigned int n = end - m_text.c_str() - i;
					if (end && n < 32)
					{
						memset(name, 0, 31);
						name[31] = 0;
						memcpy(name, m_text.c_str() + i, n);
						if (emoticon->GetIconIndex(name) != -1)
						{
							icon = true;
							iconLength = n + 2;
						}
					}
				}
				buffer[count++] = c;
				if (!(c & 0x80) || leadByte)
				{
					buffer[count] = 0;
					if (icon)
					{
						if (count + iconLength > 20)
						{
							int bytes = leadByte ? 2 : 1;
							buffer[count - bytes] = 0;
							i -= bytes;
							count = 0;
							lines[lineCount++] = buffer;
						}
						icon = false;
					}
					else if (count > 20)
					{
						int bytes = leadByte ? 2 : 1;
						buffer[count - bytes] = 0;
						i -= bytes;
						count = 0;
						lines[lineCount++] = buffer;
					}
					ntBalloonTextManager::Instance()->m_pFont->SetScale(scale);
					int width =
						ntBalloonTextManager::Instance()->m_pFont->GetTextWidth(
							g_view, buffer);
					if (width > maxWidth)
						maxWidth = width;
					if (leadByte)
						leadByte = false;
				}
				else
					leadByte = true;
			}
		}
		if (buffer[0])
			lines[static_cast<unsigned __int64>(lineCount++)] = buffer;
	}
	int height = lineCount * scale * 15.f;
	if (m_bUpdated)
	{
		m_x = headX;
		m_bUpdated = false;
	}
	int left = m_x;
	int width = 0;
	if (headX >= left)
		width = static_cast<const volatile int&>(maxWidth);
	if (headX < left)
		m_x = headX;
	else if (headX > left + width)
		m_x = headX - width;
	headY += (int)((1.f - scale) * 10.f);
	unsigned long color = (((MAKEWORD((unsigned char)(m_color[0] * 255.f),
								 (unsigned char)(m_color[3] * 255.f))
								<< 8) |
							   (unsigned char)(m_color[1] * 255.f))
							  << 8) |
		(unsigned char)(m_color[2] * 255.f);
	int textX;
	int textY;
	RECT rect;

	if (!m_bThink)
	{
		textX = m_x;
		textY = headY - ntBalloonTextManager::Instance()->m_bottomHeight -
			ntBalloonTextManager::Instance()->m_pTail->GetHeight() - height + 5;
		int left = textX - ntBalloonTextManager::Instance()->m_leftWidth;
		rect.left = left +
			(textX - left - ntBalloonTextManager::Instance()->m_leftWidth) *
				scale;
		rect.top = headY - ntBalloonTextManager::Instance()->m_bottomHeight -
			ntBalloonTextManager::Instance()->m_pTail->GetHeight() - height -
			ntBalloonTextManager::Instance()->m_topHeight + 5;
		rect.right = maxWidth + ntBalloonTextManager::Instance()->m_leftWidth +
			ntBalloonTextManager::Instance()->m_rightWidth;
		rect.bottom = ntBalloonTextManager::Instance()->m_topHeight + height +
			ntBalloonTextManager::Instance()->m_bottomHeight;
		WOverlay::DrawFrameOverlay9(view,
			ntBalloonTextManager::Instance()->m_pFrame, rect, color);
		ntBalloonTextManager::Instance()->m_pTail->RenderWithShear(view,
			WRect(0, 0, 1.f, 1.f),
			WRect(headX -
					ntBalloonTextManager::Instance()->m_pTail->GetWidth() / 2,
				((float)headY -
					ntBalloonTextManager::Instance()->m_pTail->GetHeight()),
				ntBalloonTextManager::Instance()->m_pTail->GetWidth(),
				ntBalloonTextManager::Instance()->m_pTail->GetHeight()),
			0.f, 0x80000, color);
	}
	else
	{
		textX = m_x;
		textY = headY -
			ntBalloonTextManager::Instance()->m_pThinkTail->GetHeight() -
			height - 19;
		int left = textX - ntBalloonTextManager::Instance()->m_leftWidth;
		rect.left = left + (textX - left - 24) * scale;
		rect.top = headY -
			ntBalloonTextManager::Instance()->m_pThinkTail->GetHeight() -
			height - 43;
		rect.right = maxWidth + 48;
		rect.bottom = height + 48;
		WOverlay::DrawFrameOverlay1(view,
			ntBalloonTextManager::Instance()->m_pThinkFrame, 24.f, 24.f, 24.f,
			24.f, rect, color);
		ntBalloonTextManager::Instance()->m_pThinkTail->RenderWithShear(view,
			WRect(0, 0, 1.f, 1.f),
			WRect(headX -
					ntBalloonTextManager::Instance()->m_pThinkTail->GetWidth() /
						2,
				((float)headY -
					ntBalloonTextManager::Instance()
						->m_pThinkTail->GetHeight()),
				ntBalloonTextManager::Instance()->m_pThinkTail->GetWidth(),
				ntBalloonTextManager::Instance()->m_pThinkTail->GetHeight()),
			0.f, 0x80000, color);
	}
	for (int line = 0; line < lineCount; line++)
	{
		if (gdi)
		{
			gdi->SetScale(scale);
			unsigned long oldColor = gdi->GetTextColor();
			gdi->SetTextStyle(0);
			gdi->SetTextColor((unsigned char)(m_color[3] * 255.f) << 24,
				0xffffffff);
			emoticon->PrintText(WPoint(textX, line * scale * 15.f + textY), 0,
				lines[line].c_str(),
				((unsigned char)(m_color[3] * 255.f) << 24) | 0xffffff);
			gdi->SetScale(1.f);
			gdi->SetTextColor(oldColor, 0xffffffff);
		}
		else
		{
			ntBalloonTextManager::Instance()->m_pFont->SetScale(scale);
			ntBalloonTextManager::Instance()->m_pFont->Print(view, textX,
				line * scale * 15.f + textY, lines[line].c_str(), 0, 0xff000000,
				NULL);
		}
	}
	ntBalloonTextManager::Instance()->m_pFont->Flush(view);
}

void ntBalloonTextManager::Init()
{
	m_pFont = g_resrcmng->GetFixedFont("fnt_\xb5\xb8\xbf\xf2\xc3\xbc_12.wft");
	if (m_pFont)
		m_pFont->SetMode(WFont::MASKED);
	for (int i = 0; i < 9; i++)
		m_pFrame[i] =
			g_resrcmng->GetOverlay(MakeStr("chat_frame_%02d.tga", i + 1), 0);
	m_pThinkFrame = g_resrcmng->GetOverlay("framethink_02.tga", 0);
	m_pThinkTail = g_resrcmng->GetOverlay("framethink_01.tga", 0);
	m_leftWidth = m_pFrame[3]->GetWidth();
	m_topHeight = m_pFrame[1]->GetHeight();
	m_rightWidth = m_pFrame[5]->GetWidth();
	m_bottomHeight = m_pFrame[7]->GetHeight();
	m_pTail = g_resrcmng->GetOverlay("chat_frame_00.tga", 0);
}

void ntBalloonTextManager::Shutdown()
{
	DeleteAll();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
	for (int i = 0; i < 9; i++)
		if (g_resrcmng && m_pFrame[i])
		{
			g_resrcmng->Release(m_pFrame[i]);
			m_pFrame[i] = NULL;
		}
	if (g_resrcmng && m_pTail)
	{
		g_resrcmng->Release(m_pTail);
		m_pTail = NULL;
	}
	if (g_resrcmng && m_pThinkFrame)
	{
		g_resrcmng->Release(m_pThinkFrame);
		m_pThinkFrame = NULL;
	}
	if (g_resrcmng && m_pThinkTail)
	{
		g_resrcmng->Release(m_pThinkTail);
		m_pThinkTail = NULL;
	}
}

void ntBalloonTextManager::DeleteAll()
{
	std::list<ntBalloonText*>::iterator it;
	for (it = m_list.begin(); it != m_list.end();)
	{
		delete *it;
		it = m_list.erase(it);
	}
	m_list.clear();
}

void ntBalloonTextManager::Process(float delta)
{
	std::list<ntBalloonText*>::iterator it;
	for (it = m_list.begin(); it != m_list.end(); ++it)
		(*it)->Process(delta);
}

void ntBalloonTextManager::Render()
{
	std::list<ntBalloonText*>::iterator it;
	for (it = m_list.begin(); it != m_list.end(); it++)
		(*it)->Render(g_view);
}

ntBalloonText* ntBalloonTextManager::CreateBalloonText(unsigned long uid,
	const char* text, const float* const color, float lifeTime)
{
	ntBalloonText* balloon = new ntBalloonText(uid, text, color, lifeTime);
	m_list.push_back(balloon);
	return balloon;
}

void ntBalloonTextManager::DeleteBalloonText(ntBalloonText* balloon)
{
	std::list<ntBalloonText*>::iterator it =
		std::find(m_list.begin(), m_list.end(), balloon);
	if (it != m_list.end())
	{
		m_list.erase(it);
		delete balloon;
	}
}
