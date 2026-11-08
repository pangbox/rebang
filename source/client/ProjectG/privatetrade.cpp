#include "stdafx.h"
#include "privatetrade.h"
#include "privatetradedlg.h"
#include "formbar.h"
#include "chatmsg.h"
#include "avatar_avatar.h"
#include "ntavatar.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;
extern WView* g_view;
extern unsigned long g_CurrentTime;

enum _ePrivateTradeState
{
	PTS_NONE = 0,
	PTS_WAIT = 2,
	PTS_TRADE = 3,
	PTS_CONFIRM = 4,
	PTS_PERMIT = 5,
	PTS_DONE = 6
};

class CPrivateTradeImpl : public FrCmdTarget
{
	friend class CPrivateTrade;

	void SetTraderNick(const std::string& nick) { m_traderNick = nick; }
	void SetTraderGuid(unsigned long guid) { m_traderGuid = guid; }
	const std::string& GetTraderNick() const { return m_traderNick; }
	const unsigned long GetTraderGuid() const { return m_traderGuid; }
	void SetState(_ePrivateTradeState state) { m_state = state; }
	_ePrivateTradeState GetState() const { return m_state; }

	CPrivateTradeImpl();
	virtual ~CPrivateTradeImpl();
	void Reset();
	void Init();
	void Process(float delta);
	void Display();
	void CloseWnd();
	void ChatMsg(const char* msg);
	bool OnResultTrade(int result, FrForm* form);
	void OnOpenTradeWnd();
	bool OnResultWait(int result, FrForm* form);
	FrFormBar* OpenFormBar();
	void OpenRequestWaitWnd(const std::string& nick);
	void OpenAcceptWaitWnd(const std::string& nick);

	_ePrivateTradeState m_state;
	float m_fTime;
	FrPrivateTrade* m_pTradeWnd;
	FrFormBar* m_pWaitWnd;
	unsigned long m_traderGuid;
	std::string m_traderNick;
};

CPrivateTradeImpl::CPrivateTradeImpl()
	: m_state(PTS_NONE),
	  m_fTime(1.0f),
	  m_pTradeWnd(NULL),
	  m_pWaitWnd(NULL),
	  m_traderGuid(0)
{
}

CPrivateTradeImpl::~CPrivateTradeImpl()
{
}

void CPrivateTradeImpl::Reset()
{
	m_pTradeWnd = NULL;
	m_pWaitWnd = NULL;
	m_state = PTS_NONE;
	m_fTime = 0;
	m_traderGuid = 0;
	m_traderNick = "";
}

void CPrivateTradeImpl::Init()
{
	m_state = PTS_NONE;
}

void CPrivateTradeImpl::Process(float delta)
{
}

void CPrivateTradeImpl::Display()
{
}

void CPrivateTradeImpl::OnOpenTradeWnd()
{
	CloseWnd();
	ntAvatar* avatar =
		ntAvatarManager::Instance()->FindAvatarByGuid(GetTraderGuid());
	if (avatar)
	{
		SetState(PTS_TRADE);
		if (!m_pTradeWnd)
		{
			unsigned long typeId = avatar->GetCharacterInfo().tid;
			m_pTradeWnd = CreateForm<FrPrivateTrade>(g_pFresh->GetManager(),
				this, "FrPrivateTrade");
			m_pTradeWnd->SetCompanion(GetTraderNick(), GetTraderGuid(), typeId);
			m_pTradeWnd->Open(
				(FRESH_PFN_RESULT)&CPrivateTradeImpl::OnResultTrade, FrESCAPE);
		}
	}
	else
	{
		ChatMsg(
			"\xb0\xc5\xb7\xa1 \xbb\xf3\xb4\xeb\xb0\xa1 \xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb0\xc5\xb7\xa1\xb0\xa1 \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
	}
}

void CPrivateTradeImpl::OpenRequestWaitWnd(const std::string& nick)
{
	if (m_pWaitWnd)
		return;

	SetState(PTS_WAIT);
	m_pWaitWnd = OpenFormBar();
	m_pWaitWnd->SetAutoCloseTimer(10);
	m_pWaitWnd->EnableButton(true);
	m_pWaitWnd->SetDesc(MakeStr(
		"%s\xb4\xd4\xbf\xa1\xb0\xd4 \xb0\xc5\xb7\xa1 \xbf\xe4\xc3\xbb\xc0\xcc \xbf\xd4\xbd\xc0\xb4\xcf\xb4\xd9.",
		nick.c_str()));
}

void CPrivateTradeImpl::OpenAcceptWaitWnd(const std::string& nick)
{
	if (m_pWaitWnd)
		return;

	SetState(PTS_WAIT);
	m_pWaitWnd = OpenFormBar();
	m_pWaitWnd->SetAutoCloseTimer(12);
	m_pWaitWnd->SetDesc(MakeStr(
		"%s\xb4\xd4\xbf\xa1\xb0\xd4 \xb0\xc5\xb7\xa1\xb8\xa6 \xbf\xe4\xc3\xbb\xc1\xdf \xc0\xd4\xb4\xcf\xb4\xd9.",
		nick.c_str()));
}

bool CPrivateTradeImpl::OnResultTrade(int result, FrForm* form)
{
	if (result == FrOK)
	{
		m_pTradeWnd = NULL;
		SetState(PTS_DONE);
		return true;
	}
	if (result == FrCANCEL)
	{
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(2);
		packet.Encode4(m_traderGuid);
		packet.EncodeStr(GetTraderNick());
		packet.Send((eSendTo)0);
		ChatMsg(
			"\xb0\xc5\xb7\xa1\xb0\xa1 \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
	}
	if (result != FrNONE)
		Reset();
	return true;
}

bool CPrivateTradeImpl::OnResultWait(int result, FrForm* form)
{
	if (result == FrOK)
	{
		m_pWaitWnd = NULL;
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(4);
		packet.Encode4(m_traderGuid);
		packet.EncodeStr(GetTraderNick());
		packet.Send((eSendTo)0);
		OnOpenTradeWnd();
		return true;
	}
	if (result == FrNO)
	{
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(1);
		packet.Encode4(m_traderGuid);
		packet.EncodeStr(GetTraderNick());
		packet.Send((eSendTo)0);
	}
	if (result != FrNONE)
		Reset();
	return true;
}

void CPrivateTradeImpl::ChatMsg(const char* msg)
{
	CChatMsg::Instance()->AddChatMsg(std::string(msg), 0xffff8000, false,
		false);
}

FrFormBar* CPrivateTradeImpl::OpenFormBar()
{
	FrFormBar* form =
		CreateForm<FrFormBar>(g_pFresh->GetManager(), this, "FORM_BAR");
	form->SetRange(0, 1000);
	WRect rect = form->GetRect();
	form->SetRect(WRect(g_view->GetWidth() - rect.w - 30, 80, rect.w, rect.h));
	rect.x = rect.y = 0;
	form->Attach(rect, "course_icon_o");
	form->Open((FRESH_PFN_RESULT)&CPrivateTradeImpl::OnResultWait, FrESCAPE);
	WPoint pos = form->GetRelativeBarPos();
	pos.y = 30;
	form->MoveBar(pos);
	return form;
}

void CPrivateTradeImpl::CloseWnd()
{
	if (m_pTradeWnd)
	{
		m_pTradeWnd->Close(FrNONE, true);
		m_pTradeWnd = NULL;
	}
	if (m_pWaitWnd)
	{
		m_pWaitWnd->Close(FrNONE, true);
		m_pWaitWnd = NULL;
	}
}

IMPLEMENT_ACTOR(CPrivateTrade, IActor)

CPrivateTrade::CPrivateTrade()
	: m_pImpl(NULL)
{
	m_pImpl = new CPrivateTradeImpl;
}

CPrivateTrade::~CPrivateTrade()
{
	delete m_pImpl;
}

void CPrivateTrade::OnLoad()
{
}

void CPrivateTrade::OnInit()
{
	m_pImpl->Init();
	m_pImpl->SetState(PTS_NONE);
}

void CPrivateTrade::OnReload()
{
	m_pImpl->Init();
}

void CPrivateTrade::OnProcess(float delta)
{
	m_pImpl->Process(delta);
}

void CPrivateTrade::OnDisplay()
{
	m_pImpl->Display();
}

void CPrivateTrade::OnDestroy()
{
	m_pImpl->CloseWnd();
}

void CPrivateTrade::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}
	if (msg.message != 583)
		return;
	switch (msg.param1)
	{
	case 0:
	{
		unsigned long guid = msg.param2;
		const char* nick = (const char*)msg.param3;
		if (CSharedDoc::Instance()->m_myInfo.info.dwGuid == guid)
			return;
		if (m_pImpl->GetState() != PTS_NONE)
			return;
		m_pImpl->SetState(PTS_WAIT);
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(0);
		packet.Encode4(guid);
		packet.EncodeStr(std::string(nick));
		packet.Send((eSendTo)0);
		m_pImpl->OpenAcceptWaitWnd(std::string(nick));
		m_pImpl->SetTraderGuid(guid);
		m_pImpl->SetTraderNick(nick);
		break;
	}
	case 1:
	{
		m_pImpl->SetState(PTS_TRADE);
		const _sPrivateTradeItem* items = (const _sPrivateTradeItem*)msg.param2;
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(5);
		packet.Encode4(m_pImpl->GetTraderGuid());
		packet.EncodeStr(m_pImpl->GetTraderNick());
		packet.EncodeBuffer(items, sizeof(*items));
		packet.Send((eSendTo)0);
		break;
	}
	case 2:
	{
		m_pImpl->SetState(PTS_CONFIRM);
		const _sPrivateTradeItem* items = (const _sPrivateTradeItem*)msg.param2;
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(6);
		packet.Encode4(m_pImpl->GetTraderGuid());
		packet.EncodeStr(m_pImpl->GetTraderNick());
		packet.EncodeBuffer(items, sizeof(*items));
		packet.Send((eSendTo)0);
		break;
	}
	case 3:
	{
		m_pImpl->SetState(PTS_PERMIT);
		const _sPrivateTradeItem* mine = (const _sPrivateTradeItem*)msg.param2;
		const _sPrivateTradeItem* theirs =
			(const _sPrivateTradeItem*)msg.param3;
		WSendPacket packet((enumClientPacket)219);
		packet.Encode1(7);
		packet.Encode4(m_pImpl->GetTraderGuid());
		packet.EncodeStr(m_pImpl->GetTraderNick());
		packet.EncodeBuffer(mine, sizeof(*mine));
		packet.EncodeBuffer(theirs, sizeof(*theirs));
		packet.Send((eSendTo)0);
		break;
	}
	}
}

void CPrivateTrade::OnPacket(WReceivedPacket* packet)
{
	switch (packet->Decode1())
	{
	case 0:
	{
		unsigned long guid = packet->Decode4();
		std::string nick = packet->DecodeStr();
		if (m_pImpl->GetState() != PTS_NONE)
		{
			WSendPacket reply((enumClientPacket)219);
			reply.Encode1(3);
			reply.Encode4(guid);
			reply.EncodeStr(nick);
			reply.Send((eSendTo)0);
		}
		else
		{
			m_pImpl->SetState(PTS_WAIT);
			m_pImpl->SetTraderGuid(guid);
			m_pImpl->SetTraderNick(nick);
			m_pImpl->OpenRequestWaitWnd(nick);
		}
		break;
	}
	case 2:
		if (m_pImpl->GetTraderGuid())
			m_pImpl->ChatMsg(
				"\xb0\xc5\xb7\xa1\xb0\xa1 \xc3\xeb\xbc\xd2 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pImpl->CloseWnd();
		m_pImpl->Reset();
		break;
	case 1:
		m_pImpl->ChatMsg(
			"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xb0\xc5\xb7\xa1\xb8\xa6 \xb0\xc5\xc0\xfd \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pImpl->CloseWnd();
		m_pImpl->Reset();
		break;
	case 3:
		m_pImpl->ChatMsg(
			"\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xba \xc7\xf6\xc0\xe7 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xb0\xfa \xb0\xc5\xb7\xa1\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.");
		m_pImpl->CloseWnd();
		m_pImpl->Reset();
		break;
	case 4:
		m_pImpl->OnOpenTradeWnd();
		break;
	case 5:
	{
		unsigned long guid = packet->Decode4();
		std::string nick = packet->DecodeStr();
		_sPrivateTradeItem items;
		packet->DecodeBuffer(&items, sizeof(items));
		FrPrivateTrade* form =
			(FrPrivateTrade*)g_pFresh->GetDesktop()->FindChildForm(
				"FrPrivateTrade");
		if (form)
			form->UpdateFrTradeItem(items);
		break;
	}
	case 6:
	{
		unsigned long guid = packet->Decode4();
		std::string nick = packet->DecodeStr();
		_sPrivateTradeItem items;
		packet->DecodeBuffer(&items, sizeof(items));
		FrPrivateTrade* form =
			(FrPrivateTrade*)g_pFresh->GetDesktop()->FindChildForm(
				"FrPrivateTrade");
		if (form)
			form->FinalFrWait(items);
		break;
	}
	case 7:
	{
		unsigned long guid = packet->Decode4();
		std::string nick = packet->DecodeStr();
		_sPrivateTradeItem items;
		packet->DecodeBuffer(&items, sizeof(items));
		FrPrivateTrade* form =
			(FrPrivateTrade*)g_pFresh->GetDesktop()->FindChildForm(
				"FrPrivateTrade");
		if (form)
			form->FinalFrOK(items);
		break;
	}
	case 9:
		packet->Decode1();
		if (m_pImpl->GetTraderGuid())
			m_pImpl->ChatMsg(
				"\xc7\xf6\xc0\xe7 \xb0\xc5\xb7\xa1\xb8\xa6 \xb4\xf5\xc0\xcc\xbb\xf3 \xc1\xf8\xc7\xe0 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pImpl->CloseWnd();
		m_pImpl->Reset();
		break;
	}
}
