#include "stdafx.h"
#include "chatmsg.h"
#include "golftask.h"
#include "lobbytask.h"
#include "avatar_task.h"
#include "realmyroomtask.h"
#include "binstr.h"
#include "clientsetting.h"
#include "fresh.h"
extern Fresh* g_pFresh;

CChatMsg::CChatMsg()
	: m_bPrinted(false),
	  m_bCaretMove(true),
	  m_nMaxBytes(0),
	  m_fMaxWidth(1000.f),
	  m_pMaskedFont(NULL),
	  m_bToggle(true)
{
	for (int i = 0; i < 3; ++i)
		m_pChatWindow[i] = NULL;
	m_iState = CHAT_WIN_CLOSED;
	Init();
}

CChatMsg::~CChatMsg()
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pChatWindow[i])
		{
			g_resrcmng->Release(m_pChatWindow[i]);
			m_pChatWindow[i] = NULL;
		}
	}

	if (g_resrcmng && m_pMaskedFont)
	{
		g_resrcmng->Release(m_pMaskedFont);
		m_pMaskedFont = NULL;
	}
}

void CChatMsg::Init()
{
	InitFont();
	m_bCompInProgress = false;
	m_bActive = false;
	m_bInsert = true;
	m_iPrevCode = 0;
	m_iCurChar = 0;
	m_iNumChars = 0;
	m_iCurByte = 0;
	m_iNumBytes = 0;
	m_iNumCopyChars = 0;
	m_iNumCopyBytes = 0;
	m_iNumSelBytes = 0;
	m_szChat[0] = 0;
	m_szComp[0] = 0;
	m_pChatWindow[0] = g_resrcmng->GetOverlay("{chat_l.png", 0);
	m_pChatWindow[1] = g_resrcmng->GetOverlay("{chat_m.png", 0);
	m_pChatWindow[2] = g_resrcmng->GetOverlay("{chat_r.png", 0);
	m_rcChatWinSrc[0] = WRect(0, 0, 139.f / 256.f, 1.f);
	m_rcChatWinSrc[1] = WRect(0, 0, 1.f, 1.f);
	m_fVel = 3000.f;
	m_rcChatWinSrc[2] = WRect(0, 0, 1.f, 1.f);
	m_bBackupUiAlpha = 255;
	m_bShowCaret = true;
	m_fCaretTime = 0;
	ResetUiPosition();
}

void CChatMsg::ResetUiPosition()
{
	float x, y;
	WRect rect;

	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		x = 70.f;
		y = g_view->GetHeight() - 106.f;
		rect.x = x;
		rect.y = (g_view->GetHeight() - 106.f);
	}
	else
	{
		x = 91.f;
		y = COption::Instance()->gGetBar_Y() + 9.f;
		rect.x = x;
		rect.y = (COption::Instance()->gGetBar_Y() + 9.f);
	}

	rect.w = 482.f;
	rect.h = 32.f;

	for (int i = 0; i < 3; ++i)
		if (m_pChatWindow[i])
			m_pChatWindow[i]->SetClippingArea(&rect);

	float pos;
	if (m_iState == CHAT_WIN_OPENED)
		pos = x;
	else
		pos = x - 462.f;

	rect.x = pos;
	rect.y = y;
	rect.w = 139.f;
	rect.h = 32.f;
	m_rcChatWinDst[0] = rect;
	x = m_rcChatWinDst[0].x + m_rcChatWinDst[0].w;
	rect.x = 0;
	rect.y = y;
	rect.w = 64.f;
	rect.h = 32.f;
	m_rcChatWinDst[2] = rect;
	rect.x = x;
	rect.y = y;
	rect.w = 462.f - m_rcChatWinDst[0].w - m_rcChatWinDst[2].w;
	rect.h = 32.f;
	m_rcChatWinDst[1] = rect;
	m_rcChatWinDst[2].x = m_rcChatWinDst[1].x + m_rcChatWinDst[1].w;
}

void CChatMsg::Reset()
{
	g_ime->Reset();
	m_bCompInProgress = false;
	m_iPrevCode = 0;
	m_iCurChar = 0;
	m_iNumChars = 0;
	m_iCurByte = 0;
	m_iNumBytes = 0;
	m_szComp[0] = 0;
	m_szChat[0] = 0;
}

void CChatMsg::ClearHistory()
{
	Doc()->m_chatLineList.clear();
	Doc()->m_chatHistory.clear();
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		AfxGetTask()->GetActor("GolfRule")
			<< MsgObject(NULL, 99, 0, 0, 0, 0, 0);
	if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 99, 0, 0, 0, 0, 0);
}

void CChatMsg::InitFont()
{
	m_pMaskedFont =
		g_resrcmng->GetFixedFont("fnt_\xb5\xb8\xbf\xf2\xc3\xbc_12.wft");
	if (m_pMaskedFont)
		m_pMaskedFont->SetMode(WFont::MASKED);
}

void CChatMsg::SetAlwaysActive()
{
	g_ime->SetActive(true);
	CInputManager::Instance()->SetInputMode(CInputManager::INPUT_CHAT);
	m_bActive = true;
	m_bShowCaret = true;
	m_fCaretTime = 0;
	m_iState = CHAT_WIN_OPENED;
	m_rcChatWinDst[0].x = 91.f;
	m_rcChatWinDst[1].x = m_rcChatWinDst[0].x + m_rcChatWinDst[0].w;
	m_rcChatWinDst[2].x = m_rcChatWinDst[1].x + m_rcChatWinDst[1].w;

	{
		IActor* actor = AfxGetTask()->GetActor("GolfRule");
		if (actor)
			actor << MsgObject(NULL, 98, 1, 0, 0, 0, 0);
	}

	{
		IActor* actor = AfxGetTask()->GetActor("AvatarChat");
		if (actor)
			actor << MsgObject(NULL, 98, 1, 0, 0, 0, 0);
	}

	ReplaceBackupUiAlpha();

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (GOLFDOC() && IsMyTurn(true))
			GOLFDOC()->GetUIAlpha() = 128;
	}
	else if (GOLFDOC())
	{
		GOLFDOC()->GetUIAlpha() = 128;
	}
}

bool CChatMsg::HasBeenPrinted()
{
	return !m_bToggle || m_bPrinted;
}

void CChatMsg::ReplaceBackupUiAlpha()
{
	if (GOLFDOC())
		m_bBackupUiAlpha = GOLFDOC()->GetUIAlpha();
}

void CChatMsg::SetActive(bool active, bool reset, bool unused)
{
	if (m_bActive == active)
		return;
	m_bActive = active;
	g_ime->SetActive(active);
	CInputManager::Instance()->SetInputMode(
		active ? CInputManager::INPUT_CHAT : CInputManager::INPUT_GAME);
	if (!m_bActive && reset)
		Reset();
}

void CChatMsg::ToggleInsertMode()
{
	m_bInsert = !m_bInsert;
}

void CChatMsg::Process(float elapsed)
{
	if (!m_bActive)
		return;
	ProcessResultString();
	ProcessCompString();
	if (m_iState != CHAT_WIN_OPENED)
		return;
	m_fCaretTime += .01f;
	if (m_fCaretTime > (m_bShowCaret ? .4f : .1f))
	{
		m_bShowCaret = !m_bShowCaret;
		m_fCaretTime = 0;
	}
}

void CChatMsg::ProcessWindow(float elapsed)
{
	float x = IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? 70.f : 91.f;

	if (m_iState == CHAT_WIN_LEFT || m_iState == CHAT_WIN_RIGHT)
	{
		if (m_iState == CHAT_WIN_LEFT)
		{
			m_rcChatWinDst[0].x -= elapsed * 3000.f;
			if (m_rcChatWinDst[0].x < x - 462.f)
			{
				m_rcChatWinDst[0].x = x - 462.f;
				m_iState = CHAT_WIN_CLOSED;
			}
		}
		else if (m_iState == CHAT_WIN_RIGHT)
		{
			m_rcChatWinDst[0].x += elapsed * m_fVel;
			if (m_rcChatWinDst[0].x > x)
			{
				m_fVel -= elapsed * 105000.f;
				if (m_rcChatWinDst[0].x > x + 20.f)
					m_rcChatWinDst[0].x = x + 20.f;
			}
			else if (m_fVel < 0)
			{
				m_rcChatWinDst[0].x = x;
				m_iState = CHAT_WIN_OPENED;
				{
					IActor* actor = AfxGetTask()->GetActor("GolfRule");
					if (actor)
					{
						MsgObject msg(NULL, 98, 1, 0, 0, 0, 0);
						actor << msg;
					}
				}
				{
					IActor* actor = AfxGetTask()->GetActor("AvatarChat");
					if (actor)
					{
						MsgObject msg(NULL, 98, 1, 0, 0, 0, 0);
						actor << msg;
					}
				}
			}
		}
		m_rcChatWinDst[1].x = m_rcChatWinDst[0].x + m_rcChatWinDst[0].w;
		m_rcChatWinDst[2].x = m_rcChatWinDst[1].x + m_rcChatWinDst[1].w;
	}

	m_bPrinted = false;

	if (m_bActive)
		g_input->GetButton(LEFT_BUTTON);
}

void CChatMsg::ProcessMacro(float elapsed)
{
	if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()) ||
		g_input->Get("LALT", true) || g_input->Get("RALT", true) ||
		!OnlinePlay())
		return;
	std::string msg;
	CTask* task = AfxGetTask();
	for (int i = 0; i < 8; ++i)
	{
		if (g_input->GetDown(MakeStr("FUNC_%d", i + 1), true))
		{
			if (IS_KINDOF(CGolfTask, task) || IS_KINDOF(ntAvatarChatTask, task))
			{
				Doc()->m_chatManager.SendChatMessage(
					COption::Instance()->mGetMacroText(i), NULL, false);
				if (!WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_GAME))
				{
					msg = Doc()
							  ->m_userInfo[GOLFDOC()->m_currentPlayer]
							  .info.sNick;
					msg += " : ";
					msg += COption::Instance()->mGetMacroText(i);
					AddChatMsg(msg, 0xffffcc00, false, false);
				}
			}
			else
			{
				Doc()->m_chatManager.SendChatMessage2(
					COption::Instance()->mGetMacroText(i), NULL, false);
				if (!WNetworkSystem::Instance()->IsConnected(
						WNetworkSystem::NET_GAME))
				{
					msg = Doc()->m_myInfo.info.sNick;
					msg += " : ";
					msg += COption::Instance()->mGetMacroText(i);
					AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 3,
						(int)msg.c_str(), 0xffffcc00, 0, 0, 0);
				}
			}
		}
	}
}

void CChatMsg::ProcessResultString()
{
	m_iConsolCode = 0;
	int code = g_ime->GetState(3, 0);
	if (code == '\\')
		return;
	if (code & 0x80)
	{
		if (!(g_ime->GetState(3, -2) & 0x80))
			m_bCompInProgress = false;
		InsertWCharacter(code);
		m_iNumSelChars = 0;
		m_iNumSelBytes = 0;
		m_fCaretTime = 0;
		m_bShowCaret = true;
	}
	else if (code)
	{
		switch (code)
		{
		case 8:
			m_iConsolCode = code;
			OnBackSpace();
			break;
		case 4:
			m_iConsolCode = code;
			if (m_szChat[0])
				OnMoveLeft();
			else if (m_iState == CHAT_WIN_OPENED &&
				!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
					"emoticon"))
				Close(true, true);
			break;
		case 5:
			m_iConsolCode = code;
			if (m_szChat[0])
				OnMoveRight();
			else if (m_iState == CHAT_WIN_OPENED &&
				!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
					"emoticon"))
				Close(true, true);
			break;
		case 6:
			m_iConsolCode = code;
			if (!m_szChat[0] && m_iState == CHAT_WIN_OPENED &&
				!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
					"emoticon"))
				Close(true, true);
			break;
		case 7:
			m_iConsolCode = code;
			if (!m_szChat[0] && m_iState == CHAT_WIN_OPENED &&
				!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
					"emoticon"))
				Close(true, true);
			break;
		case 26:
			OnMoveEnd();
			break;

		case 12:
			OnMoveHome();
			break;
		case 1:
			m_iConsolCode = code;
			OnDelete();
			break;

		default:
			if (code >= 32 && code <= 127)
			{
				if (m_iNumSelBytes)
					DeleteSelected();
				WriteCharacter(code);
			}
			break;
		}
		m_fCaretTime = 0;
		m_bShowCaret = true;
	}
}

void CChatMsg::ProcessResultString_JP()
{
	m_iConsolCode = 0;
	const char* text = (const char*)g_ime->GetState(3, 0);
	if (text)
	{
		int len = strlen(text);
		int i = 0;
		while (i < len)
		{
			if (text[i] & 0x80)
			{
				if (!*(char*)g_ime->GetState(3, -2))
					m_bCompInProgress = false;
				int wc = ((unsigned char)text[i + 1] << 8) |
					(unsigned char)text[i++];
				InsertWCharacter(wc);
				m_iNumSelChars = 0;
				m_iNumSelBytes = 0;
				m_fCaretTime = 0;
				m_bShowCaret = true;
			}
			else if (text[i])
			{
				if (text[i] == '\\')
					continue;
				switch (text[i])
				{
				case 8:
					m_iConsolCode = text[i];
					OnBackSpace();
					break;
				case 4:
					m_iConsolCode = text[i];
					if (m_szChat[0])
						OnMoveLeft();
					else if (m_iState == CHAT_WIN_OPENED &&
						!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
							"emoticon"))
						Close(true, true);
					break;
				case 5:
					m_iConsolCode = text[i];
					if (m_szChat[0])
						OnMoveRight();
					else if (m_iState == CHAT_WIN_OPENED &&
						!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
							"emoticon"))
						Close(true, true);
					break;
				case 6:
					m_iConsolCode = text[i];
					if (!m_szChat[0] && m_iState == CHAT_WIN_OPENED &&
						!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
							"emoticon"))
						Close(true, true);
					break;
				case 7:
					m_iConsolCode = text[i];
					if (!m_szChat[0] && m_iState == CHAT_WIN_OPENED &&
						!g_pFresh->GetManager()->GetDesktop()->FindChildForm(
							"emoticon"))
						Close(true, true);
					break;
				case 26:
					OnMoveEnd();
					break;

				case 12:
					OnMoveHome();
					break;
				case 1:
					m_iConsolCode = text[i];
					OnDelete();
					break;

				default:
				{
					int code = (unsigned char)text[i];
					if ((code >= 32 && code < 128) ||
						(code >= 160 && code < 224))
					{
						if (m_iNumSelBytes)
							DeleteSelected();
						WriteCharacter(code);
					}
					break;
				}
				}
				m_fCaretTime = 0;
				m_bShowCaret = true;
			}
			++i;
		}
	}
}

void CChatMsg::ProcessCompString()
{
	m_iCompCode = g_ime->GetState(3, -2);
	if (m_iCompCode == '\\')
	{
		m_iCompCode = 0;
		return;
	}

	if (m_iCompCode)
	{
		if (m_pMaskedFont->GetTextWidth(g_view, m_szChat) > m_fMaxWidth ||
			m_iNumBytes - m_iNumSelBytes > m_nMaxBytes - 2)
		{
			g_ime->Reset();
			if (m_iNumSelBytes)
				DeleteSelected();
			m_fCaretTime = 0;
			m_bShowCaret = true;
			return;
		}
		if (m_iCompCode != m_iPrevCode)
		{
			m_iPrevCode = m_iCompCode;
			m_szComp[0] = (char)m_iCompCode;
			m_szComp[1] = (char)(m_iCompCode >> 8);
			m_szComp[2] = 0;
			if (!m_bCompInProgress)
			{
				m_bCompInProgress = true;
				if (m_iNumSelBytes)
					DeleteSelected();
				if (!m_bInsert && m_iCurChar < m_iNumChars)
					DeleteCharacter();
			}
			m_fCaretTime = 0;
			m_bShowCaret = true;
		}
	}
	else
	{
		if (m_bCompInProgress)
		{
			*(short*)m_szComp = 0;
			m_bCompInProgress = false;
		}
		m_iPrevCode = 0;
	}
}

void CChatMsg::ProcessCompString_JP()
{
	const char* text = (const char*)g_ime->GetState(3, -2);
	if (!text)
		return;
	strcpy(m_szComp, text);
	if (m_szComp[0])
	{
		const void* attr = (const void*)g_ime->GetState(3, -3);
		const void* cls = (const void*)g_ime->GetState(3, -4);
		memcpy(m_bCompAttr, attr, 1024);
		memcpy(m_dwCompCls, cls, 1024);
		m_dwCompAttrLen = g_ime->GetState(3, -5);
		m_dwCompClsLen = g_ime->GetState(3, -6);
		m_pMaskedFont->GetTextWidth(g_view, m_szChat);
		m_pMaskedFont->GetTextWidth(g_view, m_szComp);
		if (!m_bCompInProgress)
		{
			m_bCompInProgress = true;
			if (m_iNumSelBytes)
				DeleteSelected();
			if (!m_bInsert && m_iCurChar < m_iNumChars)
				DeleteCharacter();
		}
		m_bShowCaret = true;
		m_fCaretTime = 0;
	}
	else if (m_bCompInProgress)
	{
		m_bCompInProgress = false;
	}
}

void CChatMsg::ProcessEditKey(float elapsed)
{
	if (!m_bCaretMove)
		return;

	if (g_input->GetDown("HOME", true))
	{
		if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))
		{
			m_iSelByteS = m_iCurByte;
			m_iSelByteE = 0;
			m_iNumSelBytes = m_iCurByte;
			m_iSelCharS = m_iCurChar;
			m_iSelCharE = 0;
			m_iNumSelChars = m_iCurChar;
		}
		OnMoveHome();
	}

	if (g_input->GetDown("END", true))
	{
		if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))
		{
			m_iSelByteS = m_iCurByte;
			m_iSelByteE = m_iNumBytes;
			m_iNumSelBytes = m_iNumBytes - m_iCurByte;
			m_iSelCharS = m_iCurChar;
			m_iSelCharE = m_iNumChars;
			m_iNumSelChars = m_iNumChars - m_iCurChar;
		}
		OnMoveEnd();
	}

	if (g_input->Get("LCONTROL", true))
	{
		if (g_input->GetDown("COPY", true))
		{
			if (m_iNumSelBytes)
				CopySelected();
		}
		else if (g_input->GetDown("CUT", true))
		{
			if (m_iNumSelBytes)
			{
				CopySelected();
				DeleteSelected();
			}
		}
		else if (g_input->GetDown("PASTE", true))
		{
			if (m_iNumSelBytes)
				DeleteSelected();
			PasteCopied();
		}
	}
}

void CChatMsg::OnMoveLeft()
{
	if (m_bCaretMove && m_iCurChar > 0)
	{
		m_iCurByte -= 1 + m_abWide[m_iCurChar - 1];
		--m_iCurChar;
	}
}

void CChatMsg::OnMoveRight()
{
	if (m_bCaretMove && m_iCurChar < m_iNumChars)
	{
		m_iCurByte += 1 + m_abWide[m_iCurChar];
		++m_iCurChar;
	}
}

void CChatMsg::OnMoveEnd()
{
	m_iCurByte = m_iNumBytes;
	m_iCurChar = m_iNumChars;
}

void CChatMsg::OnMoveHome()
{
	m_iCurByte = 0;
	m_iCurChar = 0;
}

void CChatMsg::OnDelete()
{
	if (m_iNumSelBytes)
		DeleteSelected();
	else if (m_iCurChar < m_iNumChars)
		DeleteCharacter();
}

void CChatMsg::OnBackSpace()
{
	if (m_iNumSelBytes)
	{
		DeleteSelected();
	}
	else if (m_iCurChar)
	{
		int size = 1 + m_abWide[m_iCurChar - 1];
		memmove(m_szChat + m_iCurByte - size, m_szChat + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		memmove(m_szExt + m_iCurByte - size, m_szExt + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		m_iNumBytes -= size;
		m_iCurByte -= size;
		memmove(m_abWide + m_iCurChar - 1, m_abWide + m_iCurChar,
			m_iNumChars - m_iCurChar);
		--m_iNumChars;
		--m_iCurChar;
	}
}

void CChatMsg::DeleteCharacter()
{
	int size = 1 + m_abWide[m_iCurChar];
	memmove(m_szChat + m_iCurByte, m_szChat + m_iCurByte + size,
		m_iNumBytes - m_iCurByte - size);
	memmove(m_szExt + m_iCurByte, m_szExt + m_iCurByte + size,
		m_iNumBytes - m_iCurByte - size);
	m_iNumBytes -= size;
	memmove(m_abWide + m_iCurChar, m_abWide + m_iCurChar + 1,
		m_iNumChars - m_iCurChar - 1);
	--m_iNumChars;
}

void CChatMsg::DeleteSelected()
{
	m_iNumBytes -= m_iNumSelBytes;
	m_iNumChars -= m_iNumSelChars;

	if (m_iSelByteS < m_iSelByteE)
	{
		memmove(m_szChat + m_iSelByteS, m_szChat + m_iSelByteE, m_iNumSelBytes);
		memmove(m_szExt + m_iSelByteS, m_szExt + m_iSelByteE, m_iNumSelBytes);
		memmove(m_abWide + m_iSelCharS, m_abWide + m_iSelCharE, m_iNumSelChars);
		m_iCurByte = m_iSelByteS;
		m_iCurChar = m_iSelCharS;
	}
	else
	{
		memmove(m_szChat + m_iSelByteE, m_szChat + m_iSelByteS, m_iNumSelBytes);
		memmove(m_szExt + m_iSelByteE, m_szExt + m_iSelByteS, m_iNumSelBytes);
		memmove(m_abWide + m_iSelCharE, m_abWide + m_iSelCharS, m_iNumSelChars);
		m_iCurByte = m_iSelByteE;
		m_iCurChar = m_iSelCharE;
	}

	m_iNumSelChars = 0;
	m_iNumSelBytes = 0;
}

void CChatMsg::CopySelected()
{
	if (m_iSelByteS < m_iSelByteE)
	{
		memmove(m_szCopied, m_szChat + m_iSelByteS, m_iNumSelBytes);
		memmove(m_szCopiedExt, m_szExt + m_iSelByteS, m_iNumSelBytes);
		memmove(m_abCopied, m_abWide + m_iSelCharS, m_iNumSelChars);
	}
	else
	{
		memmove(m_szCopied, m_szChat + m_iSelByteE, m_iNumSelBytes);
		memmove(m_szCopiedExt, m_szExt + m_iSelByteE, m_iNumSelBytes);
		memmove(m_abCopied, m_abWide + m_iSelCharE, m_iNumSelChars);
	}

	m_iNumCopyBytes = m_iNumSelBytes;
	m_iNumCopyChars = m_iNumSelChars;
}

void CChatMsg::PasteCopied()
{
	float width = m_pMaskedFont->GetTextWidth(g_view, m_szChat);
	float copied = m_pMaskedFont->GetTextWidth(g_view, m_szCopied);
	if (width + copied > m_fMaxWidth)
		return;
	if (m_iNumBytes + m_iNumCopyBytes < m_nMaxBytes)
	{
		memmove(m_szChat + m_iCurByte + m_iNumCopyBytes, m_szChat + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		memmove(m_szExt + m_iCurByte + m_iNumCopyBytes, m_szExt + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		memmove(m_abWide + m_iCurChar + m_iNumCopyChars, m_abWide + m_iCurChar,
			m_iNumChars - m_iCurChar);
		memmove(m_szChat + m_iCurByte, m_szCopied, m_iNumCopyBytes);
		memmove(m_szExt + m_iCurByte, m_szCopiedExt, m_iNumCopyBytes);
		memmove(m_abWide + m_iCurChar, m_abCopied, m_iNumCopyChars);
		m_iNumBytes += m_iNumCopyBytes;
		m_iNumChars += m_iNumCopyChars;
		m_iCurByte += m_iNumCopyBytes;
		m_iCurChar += m_iNumCopyChars;
	}
}

void CChatMsg::InsertWCharacter(int code)
{
	if (m_pMaskedFont->GetTextWidth(g_view, m_szChat) > m_fMaxWidth)
		return;

	if (m_iNumBytes < m_nMaxBytes)
	{
		memmove(m_szChat + m_iCurByte + 2, m_szChat + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		memmove(m_szExt + m_iCurByte + 2, m_szExt + m_iCurByte,
			m_iNumBytes - m_iCurByte);
		memmove(m_abWide + m_iCurChar + 1, m_abWide + m_iCurChar,
			m_iNumChars - m_iCurChar);
		m_szChat[m_iCurByte] = (char)code;
		m_szChat[m_iCurByte + 1] = (char)(code >> 8);
		m_szExt[m_iCurByte] = 1;
		m_szExt[m_iCurByte + 1] = 0;
		m_abWide[m_iCurChar] = true;
		++m_iNumChars;
		m_iNumBytes += 2;
		m_iCurByte += 2;
		++m_iCurChar;
		m_szChat[m_iNumBytes] = 0;
	}
}

void CChatMsg::WriteCharacter(int code)
{
	if (m_pMaskedFont->GetTextWidth(g_view, m_szChat) > m_fMaxWidth)
		return;

	if (m_iNumBytes < m_nMaxBytes)
	{
		if (m_bInsert)
		{
			memmove(m_szChat + m_iCurByte + 1, m_szChat + m_iCurByte,
				m_iNumBytes - m_iCurByte);
			memmove(m_szExt + m_iCurByte + 1, m_szExt + m_iCurByte,
				m_iNumBytes - m_iCurByte);
			memmove(m_abWide + m_iCurChar + 1, m_abWide + m_iCurChar,
				m_iNumChars - m_iCurChar);
			++m_iNumBytes;
			++m_iNumChars;
		}
		else
		{
			if (m_iNumSelBytes)
			{
				memmove(m_szChat + m_iCurByte + 1, m_szChat + m_iCurByte,
					m_iNumBytes - m_iCurByte);
				memmove(m_szExt + m_iCurByte + 1, m_szExt + m_iCurByte,
					m_iNumBytes - m_iCurByte);
				memmove(m_abWide + m_iCurChar + 1, m_abWide + m_iCurChar,
					m_iNumChars - m_iCurChar);
				++m_iNumBytes;
				++m_iNumChars;
			}
			if (m_iCurChar == m_iNumChars)
			{
				++m_iNumBytes;
				++m_iNumChars;
			}
			else if (m_abWide[m_iCurChar])
			{
				memmove(m_szChat + m_iCurByte + 1, m_szChat + m_iCurByte + 2,
					m_iNumBytes - m_iCurByte - 2);
				memmove(m_szExt + m_iCurByte + 1, m_szExt + m_iCurByte + 2,
					m_iNumBytes - m_iCurByte - 2);
				--m_iNumBytes;
			}
		}
		m_szChat[m_iCurByte] = (char)code;
		m_szExt[m_iCurByte] = 0;
		m_abWide[m_iCurChar] = false;
		++m_iCurByte;
		++m_iCurChar;
		m_szChat[m_iNumBytes] = 0;
	}
}

void CChatMsg::PrintInChatWindow()
{
}

const float chatHeight = 140.f;
static float lineHeight = chatHeight / 14.f;

unsigned long CChatMsg::ConvertToUIColor(unsigned long color)
{
	switch (color)
	{
	case 0xffff7878:
		return 0xffff0000;
	case 0xffffcc00:
		return (unsigned long)-4239104;
	case 0xffffffff:
		return 0xff000000;
	case 0xffd2a2ff:
		return (unsigned long)-8126209;
	case 0xff00c6ff:
		return (unsigned long)-16755457;
	case 0xff00ff00:
		return (unsigned long)-16735977;
	case 0xff5bffb0:
		return (unsigned long)-16738048;
	}

	return color;
}

void CChatMsg::RemoveColor(const std::string& text, std::string& out)
{
	char buffer[1024];
	strncpy(buffer, text.c_str(), 1023);
	cTokenV token;
	token.Init(buffer, strlen(buffer));
	char unused[64] = { 0 };
	strncpy(unused, buffer, 63);
	out = "";
	char* part;
	bool more;
	do
	{
		more = token.GetTokenFullSep(&part, "\\c", 2);
		out += part;
		if (more)
			token.GetTokenFullSep(&part, "\\c", 2);
	} while (more);
}

void CChatMsg::AddChatMsg(const std::string& text, unsigned long color,
	bool force, bool remove)
{
	m_lastMsgTime = g_CurrentTime;
	std::string msg;
	if (remove)
		RemoveColor(text, msg);
	else
		msg = text;

	if (IS_KINDOF(CLobbyTask, AfxGetTask()))
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 3, (int)msg.c_str(), color, 0, 0, 0);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 96,
			(int)m_chatFacialUser.c_str(), (int)msg.c_str(), 0, 0, 0);
	}
	else if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (GOLFDOC()->IsShowChat() || force)
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(NULL, 100, (int)msg.c_str(), color, 0, 0, 0);
		AfxGetTask()->GetActor("Screen") << MsgObject(NULL, 100,
			(int)msg.c_str(), ConvertToUIColor(color), 0, 0, 0);
	}
	else if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()) &&
		(GOLFDOC()->IsShowChat() || force))
	{
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 100, (int)msg.c_str(), color, force, 0, 0);
	}

	sChatLine line;
	line.text = msg;
	line.color = color;
	Doc()->m_chatLineList.push_back(line);
}

void CChatMsg::GetChatText_JP(std::string& front, std::string& comp,
	std::string& end, int maxBytes)
{
	m_szChat[m_iNumBytes] = 0;
	if (maxBytes == -1)
	{
		if (m_iCurByte < m_iNumBytes)
		{
			char temp = m_szChat[m_iCurByte];
			m_szChat[m_iCurByte] = 0;
			front = m_szChat;
			m_szChat[m_iCurByte] = temp;
			end = m_szChat + m_iCurByte;
		}
		else
		{
			front = m_szChat;
			end = "";
		}
		comp = m_bCompInProgress ? m_szComp : "";
	}
	else
	{
		int i;
		if (false)
			Min(0, 0);

		if (m_iCurByte < m_iNumBytes)
		{
			for (i = 0; i < maxBytes && i < m_iCurByte; ++i)
				if (m_szChat[i] & 0x80)
					++i;
		}
		else
		{
			for (i = 0; i < maxBytes && i < m_iNumBytes; ++i)
				if (m_szChat[i] & 0x80)
					++i;
		}

		char temp = m_szChat[i];
		m_szChat[i] = 0;
		front = m_szChat;
		maxBytes -= i;
		m_szChat[i] = temp;

		if (m_bCompInProgress && maxBytes > 0)
		{
			int len = strlen(m_szComp);
			for (i = 0; i < len; ++i)
				if (m_szComp[i] & 0x80)
					++i;
			temp = m_szComp[i];
			m_szComp[i] = 0;
			comp = m_szComp;
			m_szComp[i] = temp;
		}
		else
		{
			comp = "";
		}

		if (m_iCurByte < m_iNumBytes && maxBytes > 0)
		{
			i = m_iCurByte;
			int limit = i + maxBytes;
			for (; i < limit && i < m_iNumBytes; ++i)
				if (m_szChat[i] & 0x80)
					++i;
			temp = m_szChat[i];
			m_szChat[i] = 0;
			end = m_szChat + m_iCurByte;
			m_szChat[i] = temp;
		}
		else
		{
			end = "";
		}
	}
}

void CChatMsg::GetChatText(std::string& front, std::string& comp,
	std::string& end)
{
	m_szChat[m_iNumBytes] = 0;
	if (m_iCurByte < m_iNumBytes)
	{
		char temp = m_szChat[m_iCurByte];
		m_szChat[m_iCurByte] = 0;
		front = m_szChat;
		m_szChat[m_iCurByte] = temp;
		end = m_szChat + m_iCurByte;
	}
	else
	{
		front = m_szChat;
		end = "";
	}

	if (m_bCompInProgress)
		comp = m_szComp;
	else
		comp = "";
}

void CChatMsg::GetCompositionString(sCompStr* comp)
{
	comp->pCompStr = m_szComp;
	comp->pCompAttr = m_bCompAttr;
	comp->pCompCls = m_dwCompCls;
	comp->dwCompAttrLen = m_dwCompAttrLen;
	comp->dwCompClsLen = m_dwCompClsLen;
}

void CChatMsg::SetChatText(const char* text, bool keepCaret)
{
	if ((int)strlen(text) > m_nMaxBytes)
	{
		strcpy(m_szChat, text);
		m_iNumBytes = m_nMaxBytes = strlen(text);
	}
	else
	{
		strcpy(m_szChat, text);
		m_iNumBytes = strlen(m_szChat);
	}

	int byte = m_iCurByte, character = m_iCurChar;
	int chars = 0;
	if (m_iNumBytes)
	{
		int i = 0;
		do
		{
			if (m_szChat[i] & 0x80)
			{
				m_abWide[chars] = true;
				++i;
			}
			else
				m_abWide[chars] = false;
			++chars;
			if (keepCaret && m_iCurByte > i)
			{
				byte = i;
				character = chars;
			}
			++i;
		} while (i < m_iNumBytes);
	}

	m_iNumChars = chars;
	if (keepCaret)
	{
		m_iCurByte = m_iCurByte ? byte + 1 : 0;
		m_iCurByte = Min(m_iCurByte, m_iNumBytes);
		m_iCurChar = character;
	}
	else
	{
		m_iCurByte = m_iNumBytes;
		m_iCurChar = m_iNumChars;
	}

	if (keepCaret)
		m_iCurChar = Min(character, m_iNumChars);
	else
		m_iCurChar = Min(m_iCurChar, m_iNumChars);
}

void CChatMsg::DrawChatWindow()
{
	if (m_iState != CHAT_WIN_CLOSED && g_pFresh->IsVisible())
		for (int i = 0; i < 3; ++i)
			if (m_pChatWindow[i])
				m_pChatWindow[i]->Render(g_view, m_rcChatWinSrc[i],
					m_rcChatWinDst[i], 0, 0xffffffff, 0, NULL);
}

void CChatMsg::PrintInScreen()
{
	m_bPrinted = true;
	if (g_CurrentTime - m_lastMsgTime > 10000)
	{
		if (IS_KINDOF(CGolfTask, AfxGetTask()))
			AfxGetTask()->GetActor("GolfRule")
				<< MsgObject(NULL, 150, 0, 0, 0, 0, 0);
		m_lastMsgTime = g_CurrentTime;
	}
}

void CChatMsg::PrintOnChatRoom()
{
}

std::string CChatMsg::GetIncomingChatMsg() const
{
	std::string msg = m_szChat;
	if (m_iCompCode)
		msg += m_szComp;
	for (unsigned int i = 0; i < msg.length(); ++i)
		if (msg[i] != ' ')
			return msg;
	return msg = "";
}

void CChatMsg::Open(bool reset, bool unused)
{
	if (IsLocalContent((localContentType_t)79) && Doc()->m_playingMode == 2)
		return;
	SetActive(true, reset, unused);
	m_bShowCaret = true;
	m_fCaretTime = 0;
	m_iState = CHAT_WIN_RIGHT;
	ReplaceBackupUiAlpha();
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (GOLFDOC() && IsMyTurn(true))
			GOLFDOC()->GetUIAlpha() = 128;
	}
	else if (GOLFDOC())
	{
		GOLFDOC()->GetUIAlpha() = 128;
	}
}

void CChatMsg::Close(bool reset, bool unused)
{
	SetActive(false, reset, unused);
	if (m_iState != CHAT_WIN_CLOSED)
	{
		m_iState = CHAT_WIN_LEFT;
		m_fVel = 3000.f;
	}

	if (GOLFDOC())
		GOLFDOC()->GetUIAlpha() = m_bBackupUiAlpha;
	{
		IActor* actor = AfxGetTask()->GetActor("GolfRule");
		if (actor)
			actor << MsgObject(NULL, 98, 0, 0, 0, 0, 0);
	}
	{
		IActor* actor = AfxGetTask()->GetActor("AvatarChat");
		if (actor)
			actor << MsgObject(NULL, 98, 0, 0, 0, 0, 0);
	}
}

const char* CChatMsg::MakeShortID(const char* id, float width)
{
	static char result[256];
	float ellipsis =
		m_pMaskedFont->GetTextWidth(g_view, K2L_Compatibility("\xa1\xa6"));
	if (m_pMaskedFont->GetTextWidth(g_view, id) > width)
	{
		float current = 0;
		int i;
		for (i = 0;;)
		{
			if (id[i] & 0x80)
			{
				static float wide = m_pMaskedFont->GetTextWidth(g_view,
					K2L_Compatibility("\xb0\xa1"));
				current += wide;
				if (current > width - ellipsis)
					break;
				result[i] = id[i];
				result[i + 1] = id[i + 1];
				i += 2;
			}
			else
			{
				current += m_pMaskedFont->GetTextWidth(g_view, "A");
				if (current > width - ellipsis)
					break;
				result[i] = id[i];
				++i;
			}
		}
		result[i] = 0;
		strcat(result, K2L_Compatibility("\xa1\xa6"));
	}
	else
	{
		strcpy(result, id);
	}

	return result;
}

bool CChatMsg::IME_IsCandWinOpened()
{
	return (bool)g_ime->GetState(5, 0);
}

bool CChatMsg::IME_IsCompDone()
{
	return (bool)g_ime->GetState(6, 0);
}

bool CChatMsg::IME_IsImeOpened()
{
	return (bool)g_ime->GetState(7, 0);
}

bool CChatMsg::IME_IsCompInProgress()
{
	return (bool)g_ime->GetState(4, 0);
}

bool CChatMsg::IME_IsAlphaNumericMode()
{
	return g_ime->GetState(10, 0) ? false : true;
}

void CChatMsg::IME_SetAlphaNumericMode()
{
	g_ime->SetAlphaNumericMode();
}

void CChatMsg::IME_SetOpenStatus(bool open)
{
	g_ime->SetOpenStatus(open);
}

tagCANDIDATELIST* const CChatMsg::IME_GetCandList()
{
	return (tagCANDIDATELIST*)g_ime->GetState(9, 0);
}
