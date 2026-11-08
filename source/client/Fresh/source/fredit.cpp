#include <stdlib.h>
#include <string.h>
#include <stack>
#include <windows.h>
#include <mmsystem.h>
#include "chatmsg.h"
#include "frelement.h"
#include "fredit.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "inputmanager.h"
#include "wvideo.h"
#include "commonutil.h"
#include "../../../shared/token.h"

IMPLEMENT_OBJECT(FrEdit, FrWnd)

extern int __fastcall float2int(float);

FrEdit::FrEdit()
	: m_pItem(NULL)
{
	m_lines = 0;
	m_lineHeight = 16;
	m_leftMargin = 0;
	m_topMargin = 0;
	m_font = 0;
	m_fontColor = 0xff808080;
	m_fontColor2 = 0xffc0c0c0;
	m_bgColor = 0;
	m_borderColor = 0;
	m_editProperty.Set(ES_EMOTICON);
	m_focusOff = false;
	m_multiLine = true;
	m_align = leftAlign;
	m_vCenterAlign = false;
	m_charLimit = 255;
	m_widthLimit = 1000.0f;
	m_password = false;
	m_bCaretMove = true;
	m_selectedLine = 1;
	m_oldEditText = "\x01\x02"
					"azrael"
					"\x01\x02";
	m_caret = 0.0f;
	m_caretDelta = 1;
	m_AutoLine = false;
	m_bSymmetry = true;
	m_dwCaretColor = 0xff808080;
	m_dwCaretColor2 = 0xffc0c0c0;
	m_bScrollUpdate = false;
	m_OldCaretPos = 0;
}

FrEdit::~FrEdit()
{
	ClearLine();
}

void FrEdit::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();
	std::map<std::string, std::string>& param = item.m_param;
	eFrStyle keyEvent;
	if (param.find("readonly") != param.end() && param["readonly"] == "true")
	{
		EnableEditStyle(ES_READ_ONLY);
		keyEvent = FWS_NONE;
	}
	else
		keyEvent = FWS_KEYEVENT;
	if (param.find("focusoff") != param.end() && param["focusoff"] == "true")
	{
		m_focusOff = true;
		keyEvent = FWS_NONE;
	}
	else if (!IsEditStyle(ES_READ_ONLY))
	{
		m_focusOff = false;
		keyEvent = FWS_KEYEVENT;
	}
	if (param.find("lineheight") != param.end())
		sscanf(param["lineheight"].c_str(), "%d", &m_lineHeight);
	if (param.find("leftmargin") != param.end())
		sscanf(param["leftmargin"].c_str(), "%d", &m_leftMargin);
	if (param.find("topmargin") != param.end())
		sscanf(param["topmargin"].c_str(), "%d", &m_topMargin);
	if (param.find("font") != param.end())
		sscanf(param["font"].c_str(), "%x", &m_font);
	if (param.find("fontcolor") != param.end())
		sscanf(param["fontcolor"].c_str(), "%x", &m_fontColor);
	if (param.find("fontcolor2") != param.end())
		sscanf(param["fontcolor2"].c_str(), "%x", &m_fontColor2);
	if (param.find("bgcolor") != param.end())
		sscanf(param["bgcolor"].c_str(), "%x", &m_bgColor);
	if (param.find("bordercolor") != param.end())
		sscanf(param["bordercolor"].c_str(), "%x", &m_borderColor);
	if (param.find("multiline") != param.end())
		m_multiLine = atoi(param["multiline"].c_str()) != 0;
	if (param.find("align") != param.end())
		m_align = (eAlign)atoi(param["align"].c_str());
	if (param.find("vcenteralign") != param.end())
		m_vCenterAlign = strcmpi(param["vcenteralign"].c_str(), "true") == 0;
	if (param.find("charlimit") != param.end())
		m_charLimit = atoi(param["charlimit"].c_str());
	if (param.find("widthlimit") != param.end())
		sscanf(param["widthlimit"].c_str(), "%f", &m_widthLimit);
	if (param.find("password") != param.end())
		m_password = strcmpi(param["password"].c_str(), "true") == 0;
	if (param.find("caretmove") != param.end())
		m_bCaretMove = strcmpi(param["caretmove"].c_str(), "true") == 0;
	if (param.find("symmetry") != param.end())
		m_bSymmetry = strcmpi(param["symmetry"].c_str(), "true") == 0;
	if (param.find("caretcolor") != param.end())
		sscanf(param["caretcolor"].c_str(), "%x", &m_dwCaretColor);
	if (param.find("caretcolor2") != param.end())
		sscanf(param["caretcolor2"].c_str(), "%x", &m_dwCaretColor2);
	m_pItem = &item;
	_RectangleSHORT& rc = item.m_rect;
	WRect rect(rc.left, rc.top, rc.Width(), rc.Height());
	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager,
		keyEvent | FWS_VISIBLE, rect, pParent);
	int rows = m_lineHeight ? ((int)m_rect.h - m_topMargin) / m_lineHeight : 0;
	if (!m_pScrBar)
	{
		m_pScrBar = new FrScrollBar;
		m_pScrBar->Init(pManager, this, 0, rows, 1, false);
		m_pScrBar->FollowBottom(true);
	}
}

void FrEdit::SetReadOnly(bool readOnly)
{
	if (readOnly)
		m_dwStyle.Disable(FWS_KEYEVENT);
	else
		m_dwStyle.Enable(FWS_KEYEVENT);
}

const char* FrEdit::InsertLine(int lineIndex, const char* text,
	unsigned long color, bool marginAlign)
{
	if (m_lineList.size() <= lineIndex)
		return NULL;
	if (!m_multiLine && m_lineList.size() >= 1)
		return NULL;
	FrLine* pItem = new FrLine;
	pItem->text = text;
	pItem->marginAlign = marginAlign;
	pItem->bIncludeEnterLine = true;
	pItem->animStart = timeGetTime();
	LimitText(pItem->text);
	if (color)
		pItem->text = MakeStr("\\c0x%08x\\c", color) + pItem->text +
			MakeStr("\\c0x%08x,0x%08x\\c", m_fontColor, m_fontColor2);
	std::list<FrLine*>::iterator it = m_lineList.begin();
	std::advance(it, lineIndex);
	if (it != m_lineList.end())
		m_lineList.insert(it, pItem);
	else
		m_lineList.push_back(pItem);
	if (m_pScrBar)
		m_pScrBar->AddItem();
	++m_lines;
	SetWindowTextA(pItem->text.c_str());
	return pItem->text.c_str();
}

int FrEdit::DeleteLine(int line)
{
	std::list<FrLine*>::iterator it = m_lineList.begin();
	std::advance(it, line);
	if (it != m_lineList.end())
	{
		delete *it;
		m_lineList.erase(it);
		return line;
	}
	return line + 1;
}

const char* FrEdit::AddLine(const char* text, unsigned long color,
	bool marginAlign)
{
	if (!m_multiLine && m_lineList.size() >= 1)
		return NULL;
	FrLine* pItem = new FrLine;
	pItem->text = text;
	pItem->marginAlign = marginAlign;
	pItem->animStart = timeGetTime();
	LimitText(pItem->text);
	if (color)
		pItem->text = MakeStr("\\c0x%08x\\c", color) + pItem->text +
			MakeStr("\\c0x%08x,0x%08x\\c", m_fontColor, m_fontColor2);
	m_lineList.push_back(pItem);
	if (m_pScrBar)
		m_pScrBar->AddItem();
	++m_lines;
	SetWindowTextA(pItem->text.c_str());
	return pItem->text.c_str();
}

const char* FrEdit::SetLine(int line, const char* text, unsigned long color,
	bool marginAlign, unsigned long animTime)
{
	if ((int)m_lineList.size() < line)
	{
		while ((int)m_lineList.size() < line)
			if (!AddLine("", 0, false))
				break;
		line = m_lineList.size();
	}
	for (std::list<FrLine*>::iterator it = m_lineList.begin();
		it != m_lineList.end(); ++it, --line)
	{
		if (line <= 1)
		{
			FrLine* pItem = *it;
			pItem->text = text;
			pItem->marginAlign = marginAlign;
			LimitText(pItem->text);
			pItem->animStart = animTime;
			if (color)
				pItem->text = MakeStr("\\c0x%08x\\c", color) + pItem->text +
					MakeStr("\\c0x%08x,0x%08x\\c", m_fontColor, m_fontColor2);
			SetWindowTextA(pItem->text.c_str());
			return pItem->text.c_str();
		}
	}
	return NULL;
}

const char* FrEdit::GetLine(int line, bool makeNewLine)
{
	if (makeNewLine && (int)m_lineList.size() < line)
	{
		while ((int)m_lineList.size() < line)
			if (!AddLine("", 0, false))
				break;
		line = m_lineList.size();
	}
	for (std::list<FrLine*>::iterator it = m_lineList.begin();
		it != m_lineList.end(); ++it, --line)
		if (line < 2)
			return (*it)->text.c_str();
	return "";
}

void FrEdit::ForcedEdit(const char* text, unsigned long color)
{
	CChatMsg::Instance()->SetChatText(text, false);
	SetLine(m_selectedLine, text, color, false, 0);
}

int FrEdit::GetLineNum()
{
	return m_lineList.size();
}

void FrEdit::ClearLine()
{
	sequence_delete(m_lineList.begin(), m_lineList.end());
	m_lineList.clear();
	if (m_pScrBar)
		m_pScrBar->ClearItem();
	m_lines = 0;
}

void FrEdit::DeleteFirstLine()
{
	std::list<FrLine*>::iterator it = m_lineList.begin();
	if (it != m_lineList.end())
	{
		delete *it;
		m_lineList.erase(it);
	}
}

void FrEdit::DeleteEndLine()
{
	std::list<FrLine*>::iterator it = m_lineList.end();
	it--;
	if (it != m_lineList.end())
	{
		delete *it;
		m_lineList.erase(it);
	}
}

int GetStep(const char* text, int len)
{
	if (Custom_IsLeadByte(*text))
		return 2;
	if (len > 1 && text[0] == '\\' && text[1] == 'c')
	{
		for (int i = 2; i < len - 1; ++i)
			if (text[i] == '\\' && text[i + 1] == 'c')
				return i + 2;
		return len;
	}
	return 1;
}

void FrEdit::AddText(const char* text, bool spaceAlign, bool marginAlign)
{
	if (!text)
		return;
	int length = strlen(text);
	if (!length)
		return;
	cTokenV token;
	token.Init(text, length);
	char* line;
	bool found = token.GetToken(&line, "\n", 1);
	int len = strlen(line);
	while (found || len)
	{
		if (len > 0 && line[len - 1] == '\r')
			line[len - 1] = 0;
		int step = GetStep(line, len);
		int pos = step;
		int spacePos = -1;
		while (true)
		{
			char temp = line[pos];
			line[pos] = 0;
			std::string noColor;
			RemoveColornLastSpace(line, noColor);
			float textWidth;
			if (IsEmoticonStyle())
				textWidth =
					WndManager()->GetEmoticon()->GetTextWidth(noColor.c_str());
			else
				textWidth = GDI()->GetTextExtend(noColor.c_str());
			line[pos] = temp;
			if (textWidth >= m_rect.w -
					m_leftMargin * (m_bSymmetry ? 2.0f : 1.0f) -
					(m_pScrBar ? m_pScrBar->GetRect().w : 0.0f))
			{
				if (pos - step == 0)
					break;
				if (spaceAlign && spacePos >= 0)
				{
					line[spacePos] = 0;
					AddLine(line, 0, marginAlign);
					len -= spacePos + 1;
					memmove(line, line + spacePos + 1, len);
					line[len] = 0;
					pos -= spacePos + 1;
					spacePos = -1;
				}
				else
				{
					char temp = line[pos - step];
					line[pos - step] = 0;
					AddLine(line, 0, marginAlign);
					line[pos - step] = temp;
					if (temp == ' ' || temp == '\t')
					{
						len -= pos - step + 1;
						memmove(line, line + pos - step + 1, len);
						pos = step - 1;
					}
					else
					{
						len -= pos - step;
						memmove(line, line + pos - step, len);
						pos = step;
					}
					line[len] = 0;
				}
			}
			else
			{
				if (pos >= len)
				{
					AddLine(line, 0, false);
					break;
				}
				if (temp == ' ')
					spacePos = pos;
				step = GetStep(line + pos, len - pos);
				pos += step;
			}
		}
		found = token.GetToken(&line, "\n", 1);
		len = strlen(line);
	}
	for (std::list<FrLine*>::iterator it = m_lineList.begin();
		it != m_lineList.end(); ++it)
		(*it)->bIncludeEnterLine = true;
	SetWindowTextA(GetLine(1, false));
}

unsigned long FrEdit::GetAnimTime(int line)
{
	for (std::list<FrLine*>::iterator it = m_lineList.begin();
		it != m_lineList.end(); ++it, --line)
		if (line < 2)
			return (*it)->animStart;
	return 0;
}

bool FrEdit::IsEmoticonStyle()
{
	if (m_editProperty.GetFlag(ES_EMOTICON) &&
		m_editProperty.GetFlag(ES_READ_ONLY))
		return true;
	return false;
}

void FrEdit::SetCharLimit(int limit, bool onlyShrink)
{
	m_charLimit = onlyShrink ? Min(limit, m_charLimit) : limit;
}

void FrEdit::LimitText(std::string& text)
{
	if ((int)text.size() > m_charLimit)
	{
		int pos;
		for (pos = 0; pos < m_charLimit; ++pos)
		{
			if (text[pos] & 0x80)
			{
				if (pos + 1 == m_charLimit)
					break;
				++pos;
			}
		}
		text.resize(pos);
	}
}

void FrEdit::RemoveColornLastSpace(const char* text, std::string& out)
{
	if (!text)
		return;
	cTokenV token;
	if (!token.Init(text, strlen(text)))
		return;
	out = "";
	char* part;
	bool found;
	do
	{
		found = token.GetTokenFullSep(&part, "\\c", 2);
		out += part;
		if (found)
			token.GetTokenFullSep(&part, "\\c", 2);
	} while (found);
	if (!out.empty() &&
		(out[out.length() - 1] == ' ' || out[out.length() - 1] == '\t'))
		out.resize(out.length() - 1);
}

void FrEdit::MakePassword(const char* text, std::string& out)
{
	out = "";
	for (int i = 0; text[i]; ++i)
	{
		if (text[i] & 0x80)
			++i;
		out += '*';
	}
}

void FrEdit::OnProc(float deltaTime)
{
	m_caret += m_caretDelta * deltaTime;
	if (m_caret > 0.4f)
	{
		m_caret = 0.4f;
		m_caretDelta = -1;
	}
	else if (m_caret < -0.1f)
	{
		m_caret = -0.1f;
		m_caretDelta = 1;
	}
}

void FrEdit::OnDraw()
{
	FrWnd::OnDraw();
	if (!m_pItem)
		return;
	GDI()->Box(m_rect, FrALPHA(m_bgColor, m_wndAlpha2 * m_wndAlpha));
	GDI()->LineBox(m_rect, FrALPHA(m_borderColor, m_wndAlpha2 * m_wndAlpha));
	if (WndManager()->HidePrivacy() && m_nFlags.GetFlag(0x1000))
		return;
	WRect content;
	if (m_borderColor >> 24)
		content = WRect(m_rect.x + 1.0f, m_rect.y + 1.0f, m_rect.w - 1.0f,
			m_rect.h - 1.0f);
	else
		content = m_rect;
	GDI()->SetClippingArea(&content);
	FrEmoticon* emo = WndManager()->GetEmoticon();
	int curLine = 1;
	int topRow;
	if (m_pScrBar)
		topRow = m_pScrBar->GetCurTopRow_Int();
	else
		topRow = 0;
	int yPos;
	if (m_vCenterAlign && topRow == 0)
	{
		int diff = (int)m_rect.h - m_lines * m_lineHeight - m_topMargin;
		yPos = Max(diff, 0) >> 1;
	}
	else
		yPos = 0;
	std::list<FrLine*>::iterator it = m_lineList.begin();
	for (; topRow > 0 && it != m_lineList.end(); --topRow, ++it, ++curLine)
	{
	}
	GDI()->SetTextStyle(m_font);
	if (IsEnabled())
		GDI()->SetTextColor(m_fontColor, m_fontColor2);
	else
		GDI()->SetTextColor(FrALPHA(m_fontColor, 0.4f), 0xffffffff);
	if (m_bScrollUpdate)
	{
		if (curLine > m_selectedLine && curLine > 1)
		{
			int diff = curLine - m_selectedLine;
			for (int i = 0; i < diff; ++i)
				m_pScrBar->ScrollUp(-1);
		}
		else
		{
			int tempyPos = yPos;
			std::list<FrLine*>::iterator tempIt = it;
			int i = 0;
			for (; yPos + m_lineHeight + m_topMargin - 1 < m_rect.h &&
				it != m_lineList.end();
				++it, yPos += m_lineHeight)
				++i;
			it = tempIt;
			yPos = tempyPos;
			if (curLine + i <= m_selectedLine)
			{
				int diff = m_selectedLine - i - curLine;
				for (int j = 0; j < diff + 1; ++j)
					m_pScrBar->ScrollDown(-1);
			}
		}
		m_bScrollUpdate = false;
	}
	for (; !(yPos + m_lineHeight + m_topMargin - 1 < m_rect.h)
			? false
			: it != m_lineList.end();
		yPos += m_lineHeight, ++it, ++curLine)
	{
		FrLine* line = *it;
		emo->SetAnim(false);
		emo->SetAnimTime((*it)->animStart);
		std::string noColor;
		RemoveColornLastSpace(line->text.c_str(), noColor);
		int xOffset = 0;
		int spaceWidth = (int)GDI()->GetTextExtend(" ");
		int oddWidth = 0;
		int compLeftOffset = 0;
		if (!line->text.empty() || m_align == rightAlign)
		{
			int diff = (int)(m_rect.w -
				m_leftMargin * (m_bSymmetry ? 2.0f : 1.0f) -
				(IsEmoticonStyle() ? emo->GetTextWidth(noColor.c_str())
								   : GDI()->GetTextExtend(noColor.c_str())) -
				(m_pScrBar && m_multiLine ? m_pScrBar->GetRect().w : 0.0f));
			switch (m_align)
			{
			case leftAlign:
				xOffset = 0;
				if ((*it)->marginAlign)
				{
					cTokenV token;
					token.Init(noColor.c_str(), noColor.length());
					int wordNum = token.GetTokenNum(0, " \t", 2) - 1;
					if (wordNum)
					{
						spaceWidth += diff / wordNum;
						oddWidth = diff % wordNum;
					}
				}
				break;
			case centerAlign:
				xOffset = diff / 2;
				break;
			case rightAlign:
				xOffset = diff;
				break;
			}
			float width;
			if (m_password || !IsEditStyle(ES_READ_ONLY))
				width = GDI()->GetTextExtend(m_editText[ETT_FRONT].c_str()) +
					GDI()->GetTextExtend(m_editText[ETT_COMP].c_str());
			else
				width = emo->GetTextWidth(m_editText[ETT_FRONT].c_str());
			if (!IsEditStyle(ES_READ_ONLY) &&
				width > m_rect.w - m_leftMargin * 2)
				compLeftOffset = (int)(width - (m_rect.w - m_leftMargin * 2));
			int wordOffset = 0;
			cTokenV token_color;
			if (token_color.Init(line->text.c_str(), line->text.length()))
			{
				bool exist_color;
				do
				{
					char* text;
					exist_color = token_color.GetTokenFullSep(&text, "\\c", 2);
					if (!text || !*text)
					{
						if (!exist_color)
							break;
					}
					else
					{
						if (m_password)
						{
							std::string password;
							MakePassword(text, password);
							WPoint pos(m_leftMargin + (float)wordOffset +
									xOffset + m_rect.x,
								yPos + m_rect.y + m_topMargin);
							wordOffset += (int)GDI()->PrintText(pos, 0,
								password.c_str(), NULL);
						}
						else
						{
							if (token_color.GetPos() == token_color.GetLen() &&
								(text[strlen(text) - 1] == ' ' ||
									text[strlen(text) - 1] == '\t'))
								text[strlen(text) - 1] = 0;
							cTokenV token_space;
							token_space.Init(text, strlen(text));
							int wordNum = token_space.GetTokenNum(0, " \t", 2);
							for (int i = 0; i < wordNum; ++i)
							{
								bool exist_space =
									token_space.GetToken(&text, " \t", 2);
								if (text && *text == '\b')
								{
									emo->SetAnim(true);
									++text;
								}
								if (text && *text)
								{
									WPoint pos(m_leftMargin +
											(float)wordOffset + xOffset +
											m_rect.x - compLeftOffset,
										yPos + m_rect.y + m_topMargin);
									wordOffset += IsEmoticonStyle()
										? (int)emo->PrintText(pos, 0, text,
											  0xffffffff)
										: (int)GDI()->PrintText(pos, 0, text,
											  NULL);
								}
								if (exist_space)
								{
									wordOffset += spaceWidth;
									if (token_color.GetPos() ==
											token_color.GetLen() &&
										i == wordNum - 2)
										wordOffset += oddWidth;
								}
							}
						}
					}
					if (!exist_color)
						break;
					token_color.GetTokenFullSep(&text, "\\c", 2);
					int colorNo = 0;
					unsigned long diffuse[2] = { m_fontColor, m_fontColor2 };
					cTokenV token_ctrl;
					if (token_ctrl.Init(text, strlen(text)))
					{
						bool exist_ctrl;
						do
						{
							exist_ctrl = token_ctrl.GetToken(&text, ",", 1);
							if (text && *text)
							{
								if (strlen(text) > 1)
									sscanf(text, "%x", &diffuse[colorNo++]);
								else
								{
									switch (*text)
									{
									case 'N':
									case 'n':
										GDI()->SetTextStyle(0);
										break;
									case 'T':
									case 't':
										GDI()->SetTextStyle(1);
										break;
									case 'O':
									case 'o':
										GDI()->SetTextStyle(2);
										break;
									case 'S':
									case 's':
										GDI()->SetTextStyle(4);
										break;
									}
								}
							}
						} while (exist_ctrl);
						if (colorNo)
							GDI()->SetTextColor(diffuse[0], diffuse[1]);
					}
				} while (exist_color);
			}
		}
		if (m_caret > 0.0f && curLine == m_selectedLine &&
			m_nFlags.GetFlag(0x10) && m_dwStyle.GetFlag(FWS_KEYEVENT))
		{
			float w = !m_password && IsEmoticonStyle()
				? emo->GetTextWidth(m_editText[ETT_FRONT].c_str())
				: GDI()->GetTextExtend(m_editText[ETT_FRONT].c_str());
			if (!m_editText[ETT_COMP].empty())
				w += !m_password && IsEmoticonStyle()
					? emo->GetTextWidth(m_editText[ETT_COMP].c_str())
					: GDI()->GetTextExtend(m_editText[ETT_COMP].c_str());
			WPoint pe, ps;
			ps.x = pe.x =
				(w + m_leftMargin + xOffset) + m_rect.x - compLeftOffset;
			ps.y = m_topMargin + (float)yPos + m_rect.y;
			pe.y =
				GDI()->GetFontHeight() + (float)m_topMargin + yPos + m_rect.y;
			GDI()->Line(ps, pe, m_dwCaretColor, m_dwCaretColor2, 0);
		}
		emo->SetAnim(false);
	}
	GDI()->SetClippingArea(NULL);
}

void FrEdit::OnResize()
{
	if (m_pScrBar)
		m_pScrBar->Resize(
			m_lineHeight ? ((int)m_rect.h - m_topMargin) / m_lineHeight : 0, 1,
			false);
}

void FrEdit::OnMouseMove(const WPoint& point)
{
}

bool FrEdit::OnLButtonDown(const WPoint& point)
{
	if (!m_dwStyle.GetFlag(FWS_KEYEVENT) && !m_focusOff)
		return true;
	SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, NULL);
	return false;
}

bool FrEdit::OnLButtonUp(const WPoint& point)
{
	if (!m_dwStyle.GetFlag(FWS_KEYEVENT) && !m_focusOff)
		return true;
	SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, NULL);
	return false;
}

const char* FrEdit::OnSelectText(const FrInputState* istate)
{
	if (!m_dwStyle.GetFlag(FWS_KEYEVENT))
		return NULL;
	if (!istate)
		return GetLine(m_selectedLine, false);
	int MoveRightCount = 0;
	int xPos;
	int width;
	if (m_multiLine)
	{
		if (m_lineHeight)
		{
			int topRow = m_pScrBar ? m_pScrBar->GetCurTopRow_Int() : 0;
			m_selectedLine =
				float2int(istate->mousePos.y - m_rect.y - m_topMargin) /
					m_lineHeight +
				topRow + 1;
		}
		else
			m_selectedLine = 0;
	}
	else
		m_selectedLine = 1;
	xPos = (int)(istate->mousePos.x - m_rect.x - m_leftMargin);
	bool move;
	if (xPos < 0)
		move = true;
	else if ((float)xPos < GDI()->GetTextExtend(GetLine(m_selectedLine, false)))
	{
		char editText[1024] = { 0 };
		strcpy(editText, GetLine(m_selectedLine, false));
		char* pos = editText;
		do
		{
			char temp = *pos;
			*pos = 0;
			width = (int)GDI()->GetTextExtend(editText);
			*pos = temp;
			if (*pos & 0x80)
				pos += 2;
			else
				++pos;
			++MoveRightCount;
		} while (*pos && width < xPos);
		move = true;
	}
	else
		move = false;
	const char* text = GetLine(m_selectedLine, false);
	SetWindowTextA(text);
	CChatMsg* im = istate->im;
	if (!im)
		return text;
	{
		im->Reset();
		if (move)
		{
			im->SetChatText(text, false);
			im->ResetCurCaretPos();
			for (int i = 0; i < MoveRightCount - 1; ++i)
				im->CurCaretMoveRight();
		}
		else
			im->SetChatText(text, false);
		m_OldCaretPos = istate->im->GetCurCaretPos();
	}
	return text;
}

void FrEdit::EnableKeyFocus(FrInputState& input)
{
	FrWnd::EnableKeyFocus(input);
	if (input.im)
	{
		input.im->EnableCaretMove(m_bCaretMove);
		input.im->SetBufLen(m_charLimit);
		input.im->SetBufWidth(m_widthLimit);
	}
}

void FrEdit::CursorInputProcess()
{
	if (!m_multiLine)
		return;
	CChatMsg* im = CChatMsg::Instance();
	bool bKeepCaretPos = true;
	bool bLinechanged = false;
	switch (im->GetConsolKeyCode())
	{
	case 6:
		m_selectedLine = --m_selectedLine > 1 ? m_selectedLine : 1;
		bLinechanged = true;
		break;
	case 7:
		m_selectedLine = m_lineList.size() > m_selectedLine ? ++m_selectedLine
															: m_selectedLine;
		bLinechanged = true;
		break;
	case 4:
		if (m_selectedLine > 1 && im->GetCurCaretPos() == m_OldCaretPos &&
			im->GetCurCaretPos() == 0)
		{
			--m_selectedLine;
			bLinechanged = true;
			bKeepCaretPos = false;
		}
		m_OldCaretPos = im->GetCurCaretPos();
		break;
	case 5:
		if (IsCursorEndPosition() && im->GetCurCaretPos() == m_OldCaretPos &&
			m_selectedLine + 1 <= m_lineList.size())
		{
			++m_selectedLine;
			im->SetChatText(GetLine(m_selectedLine, false), false);
			im->ResetCurCaretPos();
			m_bScrollUpdate = true;
		}
		m_OldCaretPos = im->GetCurCaretPos();
		break;
	case 8:
		if (im->GetCurCaretPos() == 0 &&
			m_OldCaretPos == im->GetCurCaretPos() && m_selectedLine > 1)
		{
			std::list<FrLine*>::iterator it = m_lineList.begin();
			std::advance(it, m_selectedLine - 2);
			if (it != m_lineList.end())
			{
				im->SetChatText((*it)->text.c_str(), false);
				(*it)->text += m_editText[ETT_FRONT] + m_editText[ETT_END];
				im->SetChatText((*it)->text.c_str(), true);
				m_pScrBar->DelItem();
			}
			m_selectedLine = DeleteLine(m_selectedLine - 1);
			bKeepCaretPos = false;
		}
		else
			bLinechanged = PullNextLine();
		m_OldCaretPos = im->GetCurCaretPos();
		m_bScrollUpdate = true;
		break;
	case 1:
		if (IsCursorEndPosition())
		{
			std::list<FrLine*>::iterator it = m_lineList.begin();
			std::advance(it, m_selectedLine);
			if (it != m_lineList.end())
			{
				std::string nextLineText = (*it)->text;
				--it;
				(*it)->text += nextLineText;
				DeleteLine(m_selectedLine);
				m_pScrBar->DelItem();
				bLinechanged = true;
			}
		}
		else
			bLinechanged = PullNextLine();
		break;
	}
	if (bLinechanged)
	{
		const char* text = GetLine(m_selectedLine, false);
		if (text)
		{
			SetWindowTextA(text);
			im->SetChatText(text, bKeepCaretPos);
			m_OldCaretPos = im->GetCurCaretPos();
		}
		m_bScrollUpdate = true;
	}
}

void FrEdit::LimitTextMultiLine()
{
	if (m_multiLine)
	{
		int limit = m_charLimit;
		CChatMsg* im = CChatMsg::Instance();
		if (GetWholeTextLength(true) > limit - 1)
			im->SetBufLen(im->GetNumBytes());
		else
			im->SetBufLen(limit);
	}
}

int FrEdit::GetWholeTextLength(bool bIncludeReturnLine)
{
	std::string SumText;
	int iteratorLineIndex = 1;
	for (std::list<FrLine*>::iterator it = m_lineList.begin();
		it != m_lineList.end(); ++it, ++iteratorLineIndex)
	{
		if (iteratorLineIndex == m_selectedLine)
		{
			std::string str1, str2, str3;
			CChatMsg::Instance()->GetChatText(str1, str2, str3);
			SumText += str1 + str2 + str3;
		}
		else
			SumText += (*it)->text;
		if (bIncludeReturnLine)
			SumText += '\n';
	}
	return SumText.length();
}

void FrEdit::AutoCutNextLineProcess()
{
	if (!m_multiLine)
		return;
	CChatMsg* im = CChatMsg::Instance();
	char editText[1024] = { 0 };
	for (std::list<FrLine*>::iterator lineIt = m_lineList.begin();
		lineIt != m_lineList.end(); ++lineIt)
	{
		std::list<FrLine*>::iterator next = lineIt;
		++next;
		if (GDI()->GetTextExtend((*lineIt)->text.c_str()) >= m_rect.w - 20.0f &&
			next == m_lineList.end())
			AddLine("", 0, false);
	}
	std::list<FrLine*>::iterator it = m_lineList.begin();
	int iteratorLineIndex = 1;
	for (; it != m_lineList.end(); ++it, ++iteratorLineIndex)
	{
		if (m_selectedLine > iteratorLineIndex)
			continue;
		{
			bool bCurSelectLine = false;
			if (m_selectedLine == iteratorLineIndex)
			{
				bCurSelectLine = true;
				im->GetChatText(m_editText[ETT_FRONT], m_editText[ETT_COMP],
					m_editText[ETT_END]);
				strcpy(editText,
					(m_editText[ETT_FRONT] + m_editText[ETT_COMP] +
						m_editText[ETT_END])
						.c_str());
			}
			else
				strcpy(editText, (*it)->text.c_str());
			float tw = GDI()->GetTextExtend(editText);
			if (tw < m_rect.w - 20.0f)
				continue;
			{
				int length = 0;
				std::stack<int> lastWordByte;
				while (editText[length])
				{
					if (editText[length] & 0x80)
					{
						lastWordByte.push(2);
						length += 2;
					}
					else
					{
						lastWordByte.push(1);
						++length;
					}
				}
				int cutIndex = length;
				float width;
				do
				{
					cutIndex -= lastWordByte.top();
					lastWordByte.pop();
					char temp = editText[cutIndex];
					editText[cutIndex] = 0;
					width = GDI()->GetTextExtend(editText);
					editText[cutIndex] = temp;
				} while (lastWordByte.size() > 0 && width >= m_rect.w - 20.0f);
				std::string NextLineItemText = &editText[cutIndex];
				if ((*it)->bIncludeEnterLine)
				{
					if (cutIndex >= 0)
						editText[cutIndex] = 0;
					(*it)->text = editText;
					(*it)->bIncludeEnterLine = false;
					if (bCurSelectLine)
						im->SetChatText(editText, true);
					InsertLine(iteratorLineIndex, NextLineItemText.c_str(), 0,
						false);
					break;
				}
				++it;
				if (it == m_lineList.end())
				{
					--it;
					AddLine(NextLineItemText.c_str(), 0, false);
					if (IsCursorEndPosition())
					{
						++m_selectedLine;
						if (!IsStringCompress())
							im->SetChatText(NextLineItemText.c_str(), false);
						else
							im->SetChatText("", false);
					}
					if (cutIndex >= 0)
						editText[cutIndex] = 0;
					(*it)->text = editText;
					break;
				}
				if (!IsCursorEndPosition() || !IsStringCompress())
					(*it)->text = NextLineItemText + (*it)->text;
				--it;
				if (bCurSelectLine)
				{
					strcpy(editText,
						(m_editText[ETT_FRONT] + m_editText[ETT_END]).c_str());
					if (IsStringCompress())
						cutIndex -= m_editText[ETT_COMP].size();
				}
				if (cutIndex >= 0)
					editText[cutIndex] = 0;
				(*it)->text = editText;
				if (bCurSelectLine)
				{
					if (IsCursorEndPosition())
					{
						++m_selectedLine;
						im->SetChatText(GetLine(m_selectedLine, false), false);
						im->ResetCurCaretPos();
						if (!IsStringCompress())
							im->CurCaretMoveRight();
						m_bScrollUpdate = true;
					}
					else
						im->SetChatText(editText, true);
				}
			}
		}
	}
}

bool FrEdit::IsCursorEndPosition()
{
	if (m_editText[ETT_END].length() == 0)
		return true;
	return false;
}

bool FrEdit::IsStringCompress()
{
	if (m_editText[ETT_COMP].length() != 0)
		return true;
	return false;
}

bool FrEdit::PullNextLine()
{
	std::list<FrLine*>::iterator it = m_lineList.begin();
	std::advance(it, m_selectedLine - 1);
	bool bIncludeEnterLine = false;
	std::string NextLineText;
	if (!(*it)->bIncludeEnterLine)
	{
		++it;
		if (it != m_lineList.end())
		{
			bIncludeEnterLine = (*it)->bIncludeEnterLine;
			NextLineText = (*it)->text;
		}
		--it;
		(*it)->bIncludeEnterLine = bIncludeEnterLine;
		std::string str1, str2, str3;
		CChatMsg::Instance()->GetChatText(str1, str2, str3);
		(*it)->text = str1 + str2 + str3 + NextLineText;
		DeleteLine(m_selectedLine);
		m_pScrBar->DelItem();
		return true;
	}
	return false;
}

void FrEdit::OnKeyFocus(CChatMsg* im)
{
	if (im->IsActive() && m_dwStyle.GetFlag(FWS_KEYEVENT))
	{
		char editText[1024];
		memset(editText, 0, sizeof(editText));
		CursorInputProcess();
		AutoCutNextLineProcess();
		LimitTextMultiLine();
		im->GetChatText(m_editText[ETT_FRONT], m_editText[ETT_COMP],
			m_editText[ETT_END]);
		im->Process(0.01f);
		strcpy(editText, m_editText[ETT_FRONT].c_str());
		strcat(editText, m_editText[ETT_COMP].c_str());
		strcat(editText, m_editText[ETT_END].c_str());
		if (m_password)
		{
			if (!g_ime->IsAlphaNumericMode())
				g_ime->SetAlphaNumericMode(true);
			int length = strlen(editText);
			if (length && editText[length - 1] == ' ')
				im->SetChatText(editText, false);
		}
		if (g_input->GetDown("ENTER", true) == 1 ||
			g_input->GetDown("PADENTER", true) == 1)
		{
			g_input->ExclusiveGetDownUseDone("ENTER");
			g_input->ExclusiveGetDownUseDone("PADENTER");
			if (m_multiLine)
			{
				if (GetWholeTextLength(true) > m_charLimit - 1)
					return;
				InsertLine(m_selectedLine - 1,
					(m_editText[ETT_FRONT] + m_editText[ETT_COMP]).c_str(), 0,
					false);
			}
			if (SendCmdToOwnerTarget(FRCMD_ENTERKEY, (int)editText, NULL))
			{
				im->Reset();
				if (m_multiLine)
				{
					++m_selectedLine;
					im->SetChatText(m_editText[ETT_END].c_str(), false);
					std::list<FrLine*>::iterator it = m_lineList.begin();
					std::advance(it, m_selectedLine - 1);
					if (it != m_lineList.end())
						(*it)->text = m_editText[ETT_END].c_str();
					if (it != m_lineList.begin())
					{
						--it;
						(*it)->bIncludeEnterLine = true;
					}
					im->ResetCurCaretPos();
					m_bScrollUpdate = true;
				}
				m_editText[ETT_FRONT] = "";
				m_editText[ETT_COMP] = "";
				m_editText[ETT_END] = "";
			}
		}
		if (m_oldEditText != editText)
		{
			m_oldEditText = editText;
			std::string text = SetLine(m_selectedLine, editText, 0, false, 0);
			SetWindowTextA(text.c_str());
		}
	}
}
