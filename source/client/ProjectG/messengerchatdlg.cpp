#include "messengerchatdlg.h"
#include "messengerdlg.h"
#include "emoticondlg.h"
#include "buddymanager.h"
#include "golftask.h"
#include "chatmsg.h"
#include "projectg.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrMessengerChatDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMessengerChatDlg, FrForm)

ON_FRESH_VV("cancel", FRCMD_LBUTTONDOWN,
	FrMessengerChatDlg::OnMessenger_CloseLBtnDown)
ON_FRESH_VI("send", FRCMD_INIT, FrMessengerChatDlg::OnMessenger_ChatInputInit)
ON_FRESH_VV("send", FRCMD_LBUTTONDOWN,
	FrMessengerChatDlg::OnMessenger_ChatInputLBtnDown)
ON_FRESH_BI("send", FRCMD_ENTERKEY, FrMessengerChatDlg::OnChatInputEnterKey)
ON_FRESH_VI("receive", FRCMD_INIT, FrMessengerChatDlg::OnMessenger_ChatViewInit)
ON_FRESH_VI("emoticon", FRCMD_INIT,
	FrMessengerChatDlg::OnMessenger_EmoticonInit)
ON_FRESH_VV("emoticon", FRCMD_LBUTTONDOWN,
	FrMessengerChatDlg::OnMessenger_EmoticonLBtnDown)
ON_FRESH_VI("caption", FRCMD_INIT, FrMessengerChatDlg::OnMessenger_CaptionInit)

END_FRESH_MSGMAP()

FrMessengerChatDlg::FrMessengerChatDlg()
{
	m_pEmoticonDlg = NULL;
	m_pEmoticonBtn = NULL;
	m_unknown114 = 0;
	m_pChatInput = NULL;
	m_pChatView = NULL;
	m_uid = 0;
	m_lastName = "";
	m_vibrateTime = 0;
	m_vibrateElapsed = 0;
	m_bVibrate = false;
	m_vibrateDX = 2;
	m_vibrateDY = -2;
}

FrMessengerChatDlg::~FrMessengerChatDlg()
{
}

void FrMessengerChatDlg::OnMessenger_CloseLBtnDown()
{
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	std::list<sChatSlot>::iterator it;
	for (it = slots.begin(); it != slots.end(); ++it)
	{
		if ((*it).uid == m_uid)
		{
			(*it).pDlg = NULL;
			slots.erase(it);
			break;
		}
	}
	Close(true);
}

void FrMessengerChatDlg::OnMessenger_ChatInputInit(int param)
{
	m_pChatInput = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
		g_pFresh->GetManager()->SetExclusiveKey(true);
	if (m_pChatInput)
		m_pChatInput->SetCharLimit(36, true);
}

void FrMessengerChatDlg::OnMessenger_ChatInputLBtnDown()
{
	OnLButtonDown(g_pFresh->GetManager()->GetMousePos());
}

bool FrMessengerChatDlg::OnLButtonDown(const WPoint& pt)
{
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (g_input->GetButton(LEFT_BUTTON) == 1)
		{
			g_pFresh->GetManager()->SetExclusiveKey(true);
			MESSENGER()->SetGameFocus(false);
		}
	}
	m_pBaseFrm->SetBlink(false);
	return FrForm::OnLButtonDown(pt);
}

void FrMessengerChatDlg::OnProc(const float dt)
{
	if (m_bVibrate)
	{
		if (m_vibrateTime > m_vibrateElapsed)
		{
			WPoint pos(m_rect.x, m_rect.y);
			pos.x += m_vibrateDX;
			pos.y += m_vibrateDY;
			MoveWindow(pos);
			Swap(m_vibrateDX, m_vibrateDY);
			m_vibrateElapsed += dt;
		}
		else
		{
			m_bVibrate = false;
			m_vibrateTime = 0;
			m_vibrateElapsed = 0;
		}
	}
	if (g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false))
		if (g_input->GetDown("EMOTICON", false))
			OnMessenger_EmoticonLBtnDown();
}

void FrMessengerChatDlg::Vibration(float time)
{
	m_pBaseFrm->SetBlink(true);
}

void FrMessengerChatDlg::OnMessenger_ChatViewInit(int param)
{
	m_pChatView = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

bool FrMessengerChatDlg::OnChatInputEnterKey(int param)
{
	char* text = (char*)param;
	if (strlen(text))
	{
		std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
		std::list<sChatSlot>::iterator it;
		for (it = slots.begin(); it != slots.end(); it++)
		{
			if ((*it).uid == m_uid)
			{
				sFriend* buddy = Buddy()->GetBuddy(m_uid);
				if (!buddy)
					return false;
				if (buddy->IsBlock)
				{
					AddChatLine(buddy->NickName,
						"\xc2\xf7\xb4\xdc\xbb\xf3\xb4\xeb\xbf\xcd\xb4\xc2 \xb4\xeb\xc8\xad\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
					return false;
				}
				if (!buddy->IsLogOn)
				{
					AddChatLine(buddy->NickName,
						"\xb8\xde\xbd\xc3\xc1\xf6\xb8\xa6 \xc0\xfc\xb4\xde\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
					return false;
				}
				Doc()->m_chatManager.Filtering(text);
				WSendPacket packet(30);
				packet.Encode4(buddy->Uid);
				packet.EncodeStr(std::string(text));
				packet.Send(TO_MSN);
				Doc()->m_chatManager.FilteringHack(text, false);
				sMsnChat chat;
				chat.nick = Doc()->m_myInfo.info.sNick;
				chat.msg = text;
				(*it).chatList.push_back(chat);
				AddChatLine(chat.nick, chat.msg);
				break;
			}
		}
	}
	return true;
}

void FrMessengerChatDlg::AddChatLine(std::string name, std::string msg)
{
	if (m_pChatView)
	{
		if (m_lastName != name)
		{
			m_pChatView->AddLine(MakeStr("%s : ", name.c_str()), 0xff000000,
				false);
			m_lastName = name;
		}
		Doc()->m_chatManager.FilteringHack(msg.c_str(), false);
		wchar_t text[260];
		wchar_t line[260];
		char bytes[260];
		int length = MultiByteToWideChar(CP_ACP, 0, msg.c_str(),
			strlen(msg.c_str()) + 1, text, 260);
		int index = 0;
		int extra = 0;
		int last;
		if (length % 16 == 0)
			last = Max(length / 16 - 1, 0);
		else
			last = length / 16;
		do
		{
			memset(line, 0, sizeof(line));
			memset(bytes, 0, sizeof(bytes));
			int start = index * 16 + extra;
			int end = index * 16 + 16;
			if (length <= end)
				end = length - 1;
			bool open = true;
			for (int i = 0; i < 4; ++i)
			{
				if (text[end - i] == ')')
					open = false;
				if (text[end - i] == '(' && open)
					break;
			}
			for (int i = 0; i < 4; ++i)
			{
				if (text[end + i] == ')')
				{
					extra = i + 1;
					break;
				}
			}
			end += extra;
			wcsncpy(line, text + start, end - start);
			if (WideCharToMultiByte(CP_ACP, 0, line, -1, bytes, 260, NULL,
					NULL) > 1)
			{
				if (index)
					m_pChatView->AddLine(MakeStr("   \b%s", bytes), 0xff464646,
						false);
				else
					m_pChatView->AddLine(MakeStr(" > \b%s", bytes), 0xff464646,
						false);
			}
		} while (index++ < last);
	}
}

void FrMessengerChatDlg::OnMessenger_EmoticonInit(int param)
{
	m_pEmoticonBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMessengerChatDlg::OnMessenger_EmoticonLBtnDown()
{
	if (m_pEmoticonBtn && !m_pEmoticonDlg)
	{
		m_pEmoticonDlg = CreateForm<FrEmoticonDlg>(g_pFresh->GetManager(), this,
			"emoticon", this);
		if (m_pEmoticonDlg)
		{
			WPoint pos(0, 0);
			if (m_pEmoticonBtn)
			{
				pos.x = m_pEmoticonBtn->GetRect().x + 20;
				pos.y = m_pEmoticonBtn->GetRect().y + 20;
			}
			m_pEmoticonDlg->SetFixed(true);
			m_pEmoticonDlg->Open(
				(FRESH_PFN_RESULT)&FrMessengerChatDlg::OnEmoticonResult, pos,
				3);
			WRect rect = m_pEmoticonDlg->GetRect();
			m_pEmoticonDlg->Adjust(rect);
			m_pEmoticonDlg->MoveWindow(WPoint(rect.x, rect.y));
		}
	}
}

bool FrMessengerChatDlg::OnEmoticonResult(int result, FrForm* pForm)
{
	if (m_pEmoticonDlg && result == 1)
	{
		FrEmoticonDlg* dlg = DYNAMIC_CAST(FrEmoticonDlg, pForm);
		if (dlg)
		{
			const char* icon = dlg->GetSelectedIcon();
			if (icon)
			{
				FrEdit* input = m_pChatInput;
				if (input)
				{
					if (IS_KINDOF(CGolfTask, AfxGetTask()))
						g_pFresh->GetManager()->SetExclusiveKey(true);
					const char* front = input->GetEditText_Front();
					const char* comp = input->GetEditText_Comp();
					const char* end = input->GetEditText_End();
					if (CChatMsg::Instance()->GetMaskedFont()->GetTextWidth(
							g_view,
							MakeStr("%s%s%s%s", front, comp, icon, end)) <
						input->GetWidthLimit())
						input->SetLine(1,
							MakeStr("%s%s%s%s", front, comp, icon, end), 0,
							false, 0);
					input->SetKeyFocus(true);
				}
			}
		}
	}
	m_pEmoticonDlg = NULL;
	return true;
}

void FrMessengerChatDlg::OnMessenger_CaptionInit(int param)
{
	m_pCaption = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrMessengerChatDlg::SetUID(unsigned long uid)
{
	m_uid = uid;
	sFriend* buddy = Buddy()->GetBuddy(uid);
	if (buddy)
		m_pCaption->SetCaption(buddy->NickName);
}

bool FrMessengerChatDlg::Close(bool bResult)
{
	if (m_pEmoticonDlg && !bResult)
	{
		m_pEmoticonDlg->Close(true);
		return false;
	}
	std::list<sChatSlot>& slots = MESSENGER()->m_chatSlotList;
	std::list<sChatSlot>::iterator it;
	for (it = slots.begin(); it != slots.end(); it++)
	{
		if ((*it).uid == m_uid)
		{
			(*it).pDlg = NULL;
			break;
		}
	}
	for (it = slots.begin(); it != slots.end(); it++)
	{
		if ((*it).pDlg)
		{
			(*it).pDlg->FindNextTopFocus(false);
			break;
		}
	}
	FrForm::Close(true);
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		if (!CChatMsg::Instance()->IsOpened() && slots.size() == 1)
		{
			g_pFresh->GetManager()->SetExclusiveKey(false);
			MESSENGER()->SetGameFocus(true);
		}
	}
	return true;
}

void FrMessengerChatDlg::RestoreChat(sChatSlot& slot)
{
	sMsnChat chat;
	std::list<sMsnChat>::iterator it;
	for (it = slot.chatList.begin(); it != slot.chatList.end(); it++)
	{
		chat = *it;
		AddChatLine(chat.nick, chat.msg);
	}
	if (m_pChatInput)
		m_pChatInput->AddLine(slot.tempChat.c_str(), 0, false);
}

void FrMessengerChatDlg::SaveTempChat(sChatSlot& slot)
{
	if (m_pChatInput)
		slot.tempChat = m_pChatInput->GetLine(1, false);
}

bool FrMessengerChatDlg::GetEmoticonClientRect(WRect& rect) const
{
	if (m_pEmoticonDlg)
	{
		rect = m_pEmoticonDlg->GetRect();
		return true;
	}
	return false;
}
