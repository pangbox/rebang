#include "s5/utilities.h"
#include "skinnedtheme.h"
#include "binstr.h"
#include "serverdlg.h"
#include "frdesktop.h"
#include "frscrollbar.h"
#include "logoutdlg.h"
#include "lobbytask.h"
#include "messengerdlg.h"
#include "logininfo.h"
#include "../../shared/localize.h"
#include "../../shared/encryption.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

class cProgressBar
{
public:
	struct sProgressImg
	{
		WRect rect;
		const Bitmap* pImg;
	};

	cProgressBar()
		: m_min(0), m_max(0), m_pos(0), m_marginX(0), m_marginY(0)
	{
	}
	~cProgressBar() { }

	void Initialize(const WRect& rect, float marginX, float marginY)
	{
		m_rect = rect;
		m_marginX = marginX;
		m_marginY = marginY;
	}

	void SetImg(const char* left, const char* middle, const char* right,
		const char* bar)
	{
		m_image[0].pImg = g_pFresh->GetBitmap(left);
		m_image[1].pImg = g_pFresh->GetBitmap(middle);
		m_image[2].pImg = g_pFresh->GetBitmap(right);
		m_image[3].pImg = g_pFresh->GetBitmap(bar);
		for (int i = 0; i < 4; ++i)
		{
			if (m_image[i].pImg)
				m_image[i].rect = WRect(m_rect.x, m_rect.y,
					m_image[i].pImg->Width(), m_image[i].pImg->Height());
		}

		m_image[2].rect.x = m_rect.x + m_rect.w - m_image[2].rect.w;
		m_image[1].rect.x = m_rect.x + m_image[0].rect.w;
		m_image[1].rect.w = m_rect.w - m_image[0].rect.w - m_image[2].rect.w;
		m_image[3].rect.x = m_rect.x + m_marginX;
		m_image[3].rect.y = m_rect.y + m_marginY;
		m_image[3].rect.w = m_rect.w - m_marginX * 2;
		m_image[3].rect.h = m_image[1].rect.h - m_marginY * 2;
	}

	void SetRange(int min, int max)
	{
		m_min = min;
		m_max = max;
	}

	void SetPos(int pos)
	{
		if (pos < m_min)
			m_pos = m_min;
		else
			m_pos = pos;
		if (pos > m_max)
			m_pos = m_max;
		else
			m_pos = pos;
	}

	void Display()
	{
		FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
		if (graphic)
		{
			int range = m_max - m_min;
			if (range)
			{
				WRect rect = m_image[3].rect;
				rect.w = m_pos * rect.w / range;
				if (m_image[3].pImg)
					graphic->DrawTexture(m_image[3].pImg, rect, -1, 0);
			}
			if (m_image[0].pImg)
				graphic->DrawTexture(m_image[0].pImg, m_image[0].rect, -1, 0);
			if (m_image[1].pImg)
				graphic->DrawTexture(m_image[1].pImg, m_image[1].rect, -1, 0);
			if (m_image[2].pImg)
				graphic->DrawTexture(m_image[2].pImg, m_image[2].rect, -1, 0);
		}
	}

private:
	sProgressImg m_image[4];
	WRect m_rect;
	int m_min;
	int m_max;
	int m_pos;
	float m_marginX;
	float m_marginY;
};

class FrAutoConnect : public FrForm
{
	DECLARE_OBJECT(FrAutoConnect)

	FrAutoConnect()
	{
		m_pProgressBar = NULL;
		m_pProgressBar = new cProgressBar;
		m_state = 1;
		m_retryTime = 0.0f;
		m_elapsed = 0.0f;
	}
	virtual ~FrAutoConnect()
	{
		if (m_pProgressBar)
		{
			delete m_pProgressBar;
			m_pProgressBar = NULL;
		}
	}

	void Retry() { m_state = 0; }

	virtual bool OnInit()
	{
		FrForm::OnInit();
		m_pProgressBar->Initialize(
			WRect(m_rect.x + 30.0f, m_rect.y + 60.0f, m_rect.w - 60.0f, 30.0f),
			3.0f, 3.0f);
		m_pProgressBar->SetImg("gauge_left_frm", "gauge_middle_frm",
			"gauge_right_frm", "gauge_bar_frm");
		m_pProgressBar->SetRange(0, 1000);
		return true;
	}

	virtual void OnProc(const float delta)
	{
		FrForm::OnProc(delta);
		if (m_pStaticMsg)
			m_pStaticMsg->SetCaption(MakeStr(
				"\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. %c",
				32));
		if (m_state != 2)
		{
			m_elapsed += delta;
			if (m_elapsed > 10.0f)
			{
				Close(FrOK, true);
				FrWnd* wnd = g_pFresh->GetDesktop()->FindChildForm("server");
				if (wnd)
				{
					FrServerDlg* server = (FrServerDlg*)wnd;
					if (IS_KINDOF(FrServerDlg, server))
					{
						server->OnNotify(
							"\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
							2);
						server->FailedToConnectServer();
					}
				}
			}
			else
			{
				m_pProgressBar->SetPos((int)(m_elapsed * 100.0f));
				if (m_state == 1)
				{
					if (m_elapsed > 1.0f)
					{
						m_elapsed = 1.0f;
						m_retryTime = (rand() % 1000 + 100) * 0.001f;
						m_state = 0;
					}
				}
				else if (m_elapsed > m_retryTime)
				{
					m_state = 2;
					m_retryTime = (rand() % 1000 + 100) * 0.001f + m_elapsed;
					std::string id = MyId();
					std::string password = Doc()->m_password;
					unsigned long signature = 0x77def61b;
					Decrypt(&signature, 4);
					WSendPacket packet((enumClientPacket)2);
					packet.EncodeStr(id);
					packet.Encode4(MyUID());
					packet.Encode4(CLoginInfo::Instance()->AuthUid());
					packet.Encode2(0x6696);
					packet.EncodeStr(Doc()->GetLoginAuthKey());
					packet.EncodeStr(std::string("645.00"));
					packet.Encode4(signature);
					packet.Encode4(CLoginInfo::Instance()->IsPcBang());
					packet.Send(TO_GAME);
					if (m_state == 2)
						return;
				}
			}
		}
	}

protected:
	void OnDrawArea()
	{
		if (g_pFresh->GetManager()->GetGDI())
			m_pProgressBar->Display();
	}

	void OnInitStaticMsg(int param)
	{
		m_pStaticMsg = DYNAMIC_CAST(FrStatic, param);
	}

	void OnInitBt1(int param)
	{
		FrButton* button = DYNAMIC_CAST(FrButton, param);
		button->SetVisible(false);
	}

	void OnInitBt2(int param)
	{
		FrButton* button = DYNAMIC_CAST(FrButton, param);
		button->SetVisible(false);
	}

protected:
	cProgressBar* m_pProgressBar;
	FrStatic* m_pStaticMsg;
	int m_state;
	float m_elapsed;
	float m_retryTime;

	DECLARE_FRESH_MSGMAP()
};

IMPLEMENT_OBJECT(FrAutoConnect, FrForm)
BEGIN_FRESH_MSGMAP(FrAutoConnect, FrForm)
ON_FRESH_VV("drawingArea", FRCMD_OWNERDRAW, FrAutoConnect::OnDrawArea)
ON_FRESH_VI("proc_msg", FRCMD_INIT, FrAutoConnect::OnInitStaticMsg)
ON_FRESH_VI("yes", FRCMD_INIT, FrAutoConnect::OnInitBt1)
ON_FRESH_VI("no", FRCMD_INIT, FrAutoConnect::OnInitBt2)
END_FRESH_MSGMAP()

IMPLEMENT_OBJECT(FrServerDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrServerDlg, FrForm)
ON_FRESH_VI("serverlist", FRCMD_INIT, FrServerDlg::OnServerListInit)
ON_FRESH_VI("serverlist", FRCMD_OWNERDRAW, FrServerDlg::OnServerListOwnerDraw)
ON_FRESH_VV("serverlist", FRCMD_LBUTTONDOWN, FrServerDlg::OnServerListBtnDown)
ON_FRESH_VI("channellist", FRCMD_INIT, FrServerDlg::OnChannelListInit)
ON_FRESH_VI("channellist", FRCMD_OWNERDRAW, FrServerDlg::OnChannelListOwnerDraw)
ON_FRESH_VV("channellist", FRCMD_LBUTTONDOWN, FrServerDlg::OnChannelListBtnDown)
ON_FRESH_VI("connectmessage", FRCMD_INIT, FrServerDlg::OnConnectMessageInit)
ON_FRESH_VI("connect_cancle", FRCMD_INIT, FrServerDlg::OnConnectCancleInit)
ON_FRESH_VV("connect_cancle", FRCMD_LBUTTONUP, FrServerDlg::OnConnectCancleUp)
ON_FRESH_VI("message", FRCMD_INIT, FrServerDlg::OnMessageInit)
ON_FRESH_VI("curserver", FRCMD_INIT, FrServerDlg::OnCurServerInit)
ON_FRESH_VI("cancel", FRCMD_INIT, FrServerDlg::OnCloseBtnInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrServerDlg::OnCloseLBtnUp)
END_FRESH_MSGMAP()

bool ServerListCompare(const void* left, const void* right)
{
	const sGameServerInfo* a = (const sGameServerInfo*)left;
	const sGameServerInfo* b = (const sGameServerInfo*)right;
	if (a->property & 0x80 || b->property & 0x80)
		return false;
	if (a->eventFlags & 0x40 || b->property & 0x40)
		return false;
	if (a->curUser > b->curUser)
		return true;
	return false;
}

FrServerDlg::FrServerDlg()
{
	m_pServerList = NULL;
	m_pChannelList = NULL;
	m_connectWaitTime = 0;
	m_reconnectDelay = 0;
	m_state = (gsLoginState_t)0;
	m_pMessage = NULL;
	m_pCurServer = NULL;
	m_pCloseBtn = NULL;
	m_bUnknown13c = false;
	m_pNotifyDlg = NULL;
	m_pWaitDlg = NULL;
	m_pLoginDlg = NULL;
	m_bAlertMessage = false;
	m_alertBlinkTime = 0;
	m_bConnecting = false;
	memset(m_pImgServerIcon, 0, sizeof(m_pImgServerIcon));
	m_pImgConnection = NULL;
	m_pFont = g_resrcmng->GetTitleFont();
	char* fontName = "[font_angel.jpg";
	m_fontInfo.filename = &fontName;
	m_fontInfo.flag = 0;
	m_fontInfo.fonth = 11;
	m_fontInfo.fontw = 6;
	m_fontInfo.numPages = 1;
	m_fontInfo.texw = 128;
	m_fontInfo.texh = 32;
	m_fontInfo.pCharSet = "1234567890";
	m_pFont->Create(&m_fontInfo);
	m_bFirstLogin = Doc()->m_curChannel.Uid == 0xff;
	m_loginReplyCount = 0;
	m_pAutoConnect = NULL;
	m_pProgressBar = NULL;
	m_pConnectCancle = NULL;
	memset(m_serverList, 0, sizeof(m_serverList));
}

FrServerDlg::~FrServerDlg()
{
	ClearServerList();
	if (m_pProgressBar)
	{
		delete m_pProgressBar;
		m_pProgressBar = NULL;
	}
	m_serverIconMap.clear();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrServerDlg::ClearServerList()
{
	m_pServerList->ClearItem();
	memset(m_serverList, 0, sizeof(m_serverList));
}

void FrServerDlg::SetState(gsLoginState_t state)
{
	m_state = state;
	if (state == 4)
		m_bConnecting = false;
}

void FrServerDlg::OnCancel()
{
	if (Doc()->m_curChannel.Uid == 0xff)
	{
		SetLoginMessageAlert(
			"\xc3\xa4\xb3\xce\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		OpenQuitDlg();
	}
	else
	{
		FrForm::Close(true);
	}
}

bool FrServerDlg::OnInit()
{
	std::map<unsigned int, std::string>& icons = m_serverIconMap;
	icons[0] = std::string("list_bongdal");
	m_serverIconMap[1] = std::string("list_pipin");
	m_serverIconMap[2] = std::string("list_titangboo");
	m_serverIconMap[3] = std::string("list_dolphiny");
	m_serverIconMap[4] = std::string("list_lolo");
	m_serverIconMap[5] = std::string("list_quma");
	m_serverIconMap[6] = std::string("list_tiki");
	m_serverIconMap[7] = std::string("list_cadie");
	m_serverIconMap[8] = std::string("list_caren");
	m_serverIconMap[9] = std::string("");
	m_serverIconMap[10] = std::string("");
	m_serverIconMap[11] = std::string("");
	m_serverIconMap[120] = std::string("");
	m_serverIconMap[128] = std::string("");
	m_connectWaitTime = 0;
	BuildServerList();
	BuildChannelList();
	EnableDrag(false);
	if (IsLocalContent(S3_TRADE_BLOCK))
	{
		bool blocked = false;
		std::string message;
		std::string time;
		if (Doc()->GetTimeBlock() > 0)
		{
			unsigned int days = Doc()->GetTimeBlock() / 24;
			unsigned int hours = Doc()->GetTimeBlock() % 24;
			if (days > 0)
				time += MakeStr("%d\xc0\xcf ", days);
			if (hours > 0)
				time += MakeStr("%d\xbd\xc3\xb0\xa3 ", hours);
			if (Doc()->GetFlagBlock() & 4)
			{
				message = MakeStr(
					"\\c0xffff0000\\c\xb0\xb3\xc0\xce \xbb\xf3\xc1\xa1 \xba\xed\xb7\xb0");
				blocked = true;
			}
			if (Doc()->GetFlagBlock() & 8)
			{
				if (blocked)
					message += MakeStr(", ");
				else
					message = MakeStr("\\c0xffff0000\\c");
				message += MakeStr("\xc3\xa4\xc6\xc3 \xba\xed\xb7\xb0");
				blocked = true;
			}
			if (Doc()->GetFlagBlock() & 16)
			{
				if (blocked)
					message += MakeStr(", ");
				else
					message = MakeStr("\\c0xffff0000\\c");
				message += MakeStr("\xc0\xfc\xb1\xa4\xc6\xc7 \xba\xed\xb7\xb0");
				blocked = true;
			}
			if (Doc()->GetFlagBlock() & 128)
			{
				if (blocked)
					message += MakeStr(", ");
				else
					message = MakeStr("\\c0xffff0000\\c");
				message += MakeStr(
					"\xc6\xed\xc1\xf6\xbe\xb2\xb1\xe2 \xba\xed\xb7\xb0");
				blocked = true;
			}
			message += MakeStr(
				"\\c0xff000000\\c \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9. \xba\xed\xb7\xb0 \xc7\xd8\xc1\xa6\xbd\xc3\xb0\xa3\xb1\xee\xc1\xf6 %s\xb3\xb2\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.\012\012",
				time.c_str());
			message += MakeStr(
				"\xb0\xed\xb0\xb4\xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7\xc7\xcf\xbf\xa9 \xc1\xd6\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
			if (blocked)
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 35, (int)message.c_str(), 0, 0, 0, 0);
		}
	}
	return true;
}

void FrServerDlg::OnProc(const float delta)
{
	if (m_bAlertMessage)
	{
		m_alertBlinkTime += delta;
		if (m_alertBlinkTime > 0.5f)
		{
			m_pMessage->SetVisible(!m_pMessage->IsVisible());
			m_alertBlinkTime = 0;
		}
	}
	switch (m_state)
	{
	case 0:
	case 4:
		m_reconnectDelay -= delta;
		if (m_reconnectDelay > 0)
			m_reconnectDelay = 0;
		break;
	case 1:
		m_reconnectDelay -= delta;
		if (m_reconnectDelay < 0)
			ConnectToServer();
		else
			m_bConnecting = true;
		break;
	case 2:
		if (!NET()->IsConnectionComplete(WNetworkSystem::NET_GAME) ||
			NET()->IsConnected(WNetworkSystem::NET_GAME))
			break;
	case 3:
		m_state = (gsLoginState_t)4;
		m_bConnecting = false;
		break;
	case 5:
		m_state = (gsLoginState_t)3;
		break;
	case 12:
	case 13:
		FailedToConnectServer();
		break;
	case 14:
		SetLoginMessage(
			"\xc0\xd4\xc0\xe5 \xc0\xda\xb0\xdd\xc0\xbb \xb9\xde\xc0\xba \xc0\xaf\xc0\xfa\xb8\xb8 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xbc\xad\xb9\xf6\xc0\xd4\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 15:
		SetLoginMessage(
			"\xc0\xd4\xc0\xe5 \xc1\xa6\xc7\xd1 \xbd\xc3\xb0\xa3\xc0\xcc \xc1\xf6\xb3\xaa\xbc\xad \xbc\xad\xb9\xf6\xc1\xa2\xbc\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 16:
		SetLoginMessage(
			"\xb4\xeb\xc8\xb8 \xc2\xfc\xb0\xa1\xc0\xda\xb0\xa1 \xbe\xc6\xb4\xcf\xb6\xf3\xbc\xad \xbc\xad\xb9\xf6\xc1\xa2\xbc\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 20:
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 32, 0, 0, 0, 0, 0);
		break;
	case 17:
		SetLoginMessage("PangYa is not available from your area.");
		FailedToConnectServer();
		break;
	case 9:
		SetLoginMessage(
			"\xbb\xe7\xbf\xeb\xb1\xdd\xc1\xf6\xb5\xc8 \xbe\xc6\xc0\xcc\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xed\xb0\xb4\xc1\xf6\xbf\xf8\xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7\xba\xce\xc5\xb9\xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 18:
		SetLoginMessage(
			"\xb0\xe8\xc1\xa4 \xc0\xcc\xc0\xfc\xc0\xcc \xb5\xc7\xbe\xee \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 8:
		SetLoginMessage(
			"\xbf\xb5\xb1\xb8\xc0\xfb\xc0\xb8\xb7\xce \xbb\xe7\xbf\xeb\xb1\xdd\xc1\xf6\xb5\xc8 \xbe\xc6\xc0\xcc\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9. \xb0\xed\xb0\xb4\xc1\xf6\xbf\xf8\xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7\xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.");
		FailedToConnectServer();
		break;
	case 19:
		SetLoginMessage(
			"\xc7\xf6\xc0\xe7 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xcf\xb1\xe2 \xbe\xee\xb7\xc1\xbf\xee \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9. \xc0\xe1\xbd\xc3 \xc8\xc4\xbf\xa1 \xb4\xd9\xbd\xc3 \xc1\xa2\xbc\xd3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		FailedToConnectServer();
		break;
	}
	if (m_pConnectMessage->IsVisible())
	{
		m_connectWaitTime += delta;
		if (m_connectWaitTime > 10.0f)
			m_pConnectCancle->Enable(true);
	}
}

void FrServerDlg::AddLoginReplyCount(int count)
{
	if (!m_loginReplyCount)
	{
		m_connectWaitTime = 0.0f;
		m_pConnectMessage->SetVisible(true);
		if (m_pConnectCancle)
			m_pConnectCancle->SetVisible(true);
	}
	if (count == 21)
	{
		m_connectWaitTime = 0.0f;
		m_loginReplyCount = 21;
		m_pConnectMessage->SetVisible(false);
		if (m_pConnectCancle)
		{
			m_pConnectCancle->SetVisible(false);
			m_pConnectCancle->Enable(false);
		}
	}
	else
	{
		m_loginReplyCount += 2;
		if (m_loginReplyCount > 21)
			m_loginReplyCount = 20;
		int percent = m_loginReplyCount * 100 / 21;
		m_pConnectMessage->SetCaption(
			"\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa4\xba\xb8\xb8\xa6 \xbf\xe4\xc3\xbb\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_pProgressBar->SetPos(percent);
	}
}

void FrServerDlg::BeginAutoLogin()
{
	if (m_pAutoConnect)
		m_pAutoConnect->Retry();
	else
	{
		m_pAutoConnect = CreateForm<FrAutoConnect>(g_pFresh->GetManager(), this,
			"GHOST_LOADING", NULL);
		m_pAutoConnect->EnableDrag(false);
		m_pAutoConnect->Open((FRESH_PFN_RESULT)&FrServerDlg::OnResultAutoRecon,
			0);
	}
}

bool FrServerDlg::OnResultAutoRecon(int result, FrForm* form)
{
	m_pAutoConnect = NULL;
	return true;
}

void FrServerDlg::FinishAutoLogin()
{
	if (m_pAutoConnect)
		m_pAutoConnect->Close(true);
}

bool FrServerDlg::CloseByEnterChannel(bool fade)
{
	if (m_bFirstLogin && !g_pFresh->IsCurrentLayout("SERVERLIST"))
	{
		if (!IS_KINDOF(CLobbyTask, AfxGetTask()))
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
		AfxPostMsg(NULL, "Lobby", 0, (int)"ROOMLIST", 0, 0, 0);
	}
	return FrForm::Close(fade);
}

void FrServerDlg::ConnectToServer(unsigned long serverUID)
{
	m_unknown110 = 1;
	int count = m_pServerList->GetCurrentItemSize(false);
	sGameServerInfo* server = NULL;
	for (int i = 1; i < count + 1; i++)
	{
		FrListItem* item = m_pServerList->GetItem(count - i);
		if (item && (server = (sGameServerInfo*)item->pData) && server->name[0])
		{
			if (!Doc()->m_curGameServer.guid)
				Doc()->m_curGameServer = m_curServer;
			else if (Doc()->m_curGameServer.guid == server->guid)
				continue;
			if (server->guid == serverUID)
				break;
		}
	}
	m_curServer = *server;
}

void FrServerDlg::ConnectToServer()
{
	m_pServerList->Enable(false);
	m_pChannelList->Enable(false);
	Doc()->m_gameServerAddr = m_curServer.addr;
	Doc()->m_gameServerPort = m_curServer.port;
	Doc()->m_gameServerUID = m_curServer.guid;
	if (NET()->IsConnected(WNetworkSystem::NET_GAME))
		NET()->ForceShutDown(WNetworkSystem::NET_GAME);
	NET()->SendMessage(WNetworkSystem::NET_GAME, NetworkUnit::eEVENT_NEXT);
	CMessengerInfo::Instance()->SetServerListWaitingState(false);
	m_bUnknown13c = false;
	m_connectWaitTime = 0;
	if (m_pConnectMessage)
		m_pConnectMessage->SetVisible(false);
	Doc()->m_curGameServer = m_curServer;
	m_state = (gsLoginState_t)2;
}

void FrServerDlg::BuildServerList()
{
	ClearServerList();
	unsigned int count = 0;
	for (std::list<sGameServerInfo>::iterator it =
			 Doc()->m_gameServerList.begin();
		it != Doc()->m_gameServerList.end(); ++it)
	{
		if (((*it).property & 0x80) && !((*it).property & 0x10))
		{
			m_pServerList->AddItem(&*it);
			++count;
		}
	}
	for (std::list<sGameServerInfo>::iterator it =
			 Doc()->m_gameServerList.begin();
		it != Doc()->m_gameServerList.end(); ++it)
	{
		if (((*it).property & 0x80) && !((*it).property & 0x10))
			continue;
		if (((*it).property & 0x10) &&
			!Doc()->m_myInfo.info.IsIdentity(0x400) &&
			IsLocalContent(S3_GM_TOOLKIT))
		{
			if (!Doc()->m_myInfo.info.IsIdentity(4))
				continue;
			strcat((*it).name, " [\xb8\xc1\xc5\xe4]");
		}
		m_pServerList->AddItem(&*it);
		++count;
	}
	if (count < 9)
		for (unsigned int i = 0; i < 9 - count; i++)
			m_pServerList->AddItem(&m_serverList[i]);
	m_pServerList->SortItem(ServerListCompare);
	m_pServerList->GetScrollBar()->ScrollToFirst();
}

bool FrServerDlg::OnLoginDlgResult(int result, FrForm* form)
{
	m_pLoginDlg = NULL;
	return true;
}

void FrServerDlg::OnServerListInit(int param)
{
	m_pServerList = DYNAMIC_CAST(FrListBox, param);
	if (m_pServerList)
	{
		m_pServerList->SetPushSound(NULL);
		m_pImgServerIcon[0] =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "icon_expevent");
		m_pImgServerIcon[1] =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "icon_pangevent");
		m_pImgServerIcon[2] =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "icon_angelwing");
		m_pImgServerIcon[3] =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "icon_community");
		m_pImgConnection =
			g_pFresh->GetManager()->GetBitmap("ITEMS", "icon_connection");
	}
}

void FrServerDlg::OnServerListBtnDown()
{
	if (!m_pServerList)
		return;
	FrListItem* item = m_pServerList->GetItemUnderCursor();
	if (!item)
		return;
	sGameServerInfo* server = (sGameServerInfo*)item->pData;
	if (!server || !server->name[0] ||
		Doc()->m_curGameServer.guid == server->guid || m_reconnectDelay > 0)
		return;
	if (!(Doc()->m_myInfo.info.dwIdentity & 0x8014))
	{
		if (server->property & 0x40)
		{
			if (Doc()->m_myInfo.stat.Level > 5)
			{
				SetLoginMessageAlert(
					"\xb7\xe7\xc5\xb0 \xb7\xb9\xba\xa7\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if (!Doc()->m_myInfo.stat.Level)
			{
				SetLoginMessageAlert(
					"\xb7\xe7\xc5\xb0"
					"F \xb7\xb9\xba\xa7\xc0\xba \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
		}
		if ((server->property & 4) && Doc()->m_myInfo.stat.Level < 6)
		{
			SetLoginMessageAlert(
				"\xba\xf1\xb1\xe2\xb3\xca \xb7\xb9\xba\xa7\xc0\xcc\xbb\xf3 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		if ((server->property & 8) && !Doc()->m_pcBang)
		{
			SetLoginMessageAlert(MakeStr(
				"\xb8\xb8%d\xbc\xbc \xc0\xcc\xbb\xf3\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xbc\xba\xc0\xce\xbc\xad\xb9\xf6\xc0\xd4\xb4\xcf\xb4\xd9.",
				19));
			return;
		}
	}
	if (server->curUser < server->maxUser - 200 ||
		(server->curUser < server->maxUser - 150 && rand() % 3 == 0) ||
		(Doc()->m_myInfo.info.dwIdentity & 0x14))
	{
		m_pAutoConnect = NULL;
		m_curServer = *server;
		m_reconnectDelay = 0;
		Doc()->ClearCurrentChannel(true);
		m_state = (gsLoginState_t)1;
		m_pChannelList->ClearItem();
		m_pCloseBtn->SetVisible(false);
		m_pMessage->SetVisible(false);
		m_bAlertMessage = false;
		g_audio->PlaySfx("ui_desktop_icon_click", 0, 0, 0, 0, 0.5f, 200.0f);
	}
	else
	{
		SetLoginMessageAlert(
			"\xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa6\xc7\xd1 \xc0\xce\xbf\xf8\xc0\xcc \xb8\xf0\xb5\xce \xc3\xa1\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_reconnectDelay = 1.0f;
	}
	Doc()->m_sendGiftList.clear();
	Doc()->m_mailList.clear();
	m_loginReplyCount = 0;
	if (m_pProgressBar)
		m_pProgressBar->SetPos(10);
	S5::UTIL::SetServerProperty(server->property);
}

bool FrServerDlg::IsNotDrawServerList(FrListItem* item)
{
	if (!m_pServerList)
		return true;
	if (!item)
		return true;
	if (!item->pData)
		return true;
	return false;
}

void FrServerDlg::DrawServerButton(const WPoint& point,
	const sGameServerInfo* server, bool selected)
{
	bool disabled = m_reconnectDelay > 0.0f;
	THEME_BUTTONS buttons;
	S5::THEME::GetServerSelectButton(server->property, buttons);
	std::string name;
	if (disabled)
		name = buttons.press;
	else if (selected)
		name = buttons.over;
	else
		name = buttons.normal;
	const Bitmap* image = g_pFresh->GetBitmap(name.c_str());
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->DrawTexture(image, WRect(point, image->Width(), image->Height()),
		0xffffffff, 0);
	if (strcmp(server->name, "") != 0)
	{
		std::map<unsigned int, std::string>::iterator it =
			m_serverIconMap.find(server->iconIndex);
		if (it != m_serverIconMap.end())
		{
			std::string& icon = it->second;
			if (icon.length())
			{
				image = g_pFresh->GetBitmap(icon.c_str());
				graphic->DrawTexture(image,
					WRect(point, image->Width(), image->Height()), 0xffffffff,
					0);
			}
			graphic->SetTextStyle(1);
			char text[32] = "";
			if (strlen(server->name) > 16)
			{
				mbscut(text, server->name, 16);
				strcat(text, "...");
			}
			else
				strncpy(text, server->name, 31);
			graphic->PrintText(WPoint(point.x + 51.0f, point.y + 10.0f), 0,
				text, NULL);
			graphic->SetTextStyle(0);
		}
	}
}

void FrServerDlg::OnServerListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (IsNotDrawServerList(item))
		return;
	sGameServerInfo* server = (sGameServerInfo*)item->pData;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	DrawServerButton(item->pos, server, item->underCursor);
	if (strcmp(server->name, "") != 0)
	{
		graphic->SetTextStyle(1);
		if (Doc()->m_curGameServer.guid == server->guid)
		{
			graphic->DrawTexture(m_pImgConnection,
				WRect(item->pos.x + 9.0f, item->pos.y + 3.0f,
					m_pImgConnection->Width(), m_pImgConnection->Height()),
				0xffffffff, 0);
			if (server->angelicWingsNum > 0)
			{
				WRect pos = GetRect();
				graphic->DrawTexture(m_pImgServerIcon[2],
					WRect(pos.x + 420.0f, pos.y + 83.0f,
						(float)m_pImgServerIcon[2]->Width(),
						(float)m_pImgServerIcon[2]->Height()),
					0xffffffff, 0);
				const char* text = MakeStr("%d", server->angelicWingsNum);
				m_pFont->GetTextWidth(g_view, text);
				m_pFont->Print(g_view, pos.x + 440.0f, pos.y + 90.0f, text, 0,
					0xffffffff, NULL);
			}
		}
		float x = 143.0f;
		if (server->eventFlags & 64)
		{
			graphic->DrawTexture(m_pImgServerIcon[3],
				WRect(item->pos.x + x, item->pos.y + 8.0f,
					m_pImgServerIcon[3]->Width(),
					m_pImgServerIcon[3]->Height()),
				0xffffffff, 0);
			x -= 15.0f;
		}
		if (server->eventFlags & 2)
		{
			graphic->DrawTexture(m_pImgServerIcon[1],
				WRect(item->pos.x + x, item->pos.y + 8.0f,
					m_pImgServerIcon[1]->Width(),
					m_pImgServerIcon[1]->Height()),
				0xffffffff, 0);
			x -= 15.0f;
		}
		if (server->eventFlags & 16)
		{
			graphic->DrawTexture(m_pImgServerIcon[0],
				WRect(item->pos.x + x, item->pos.y + 8.0f,
					m_pImgServerIcon[0]->Width(),
					m_pImgServerIcon[0]->Height()),
				0xffffffff, 0);
			x -= 15.0f;
		}
		if (server->eventFlags & 4)
		{
			graphic->DrawTexture(m_pImgServerIcon[0],
				WRect(item->pos.x + x, item->pos.y + 8.0f,
					m_pImgServerIcon[0]->Width(),
					m_pImgServerIcon[0]->Height()),
				0xa0ffffff, 0);
		}
		if (server->eventFlags & 8)
		{
			graphic->DrawTexture(m_pImgServerIcon[2],
				WRect(item->pos.x + 158.0f, item->pos.y - 5.0f + 8.0f,
					m_pImgServerIcon[2]->Width(),
					m_pImgServerIcon[2]->Height()),
				0xffffffff, 0);
		}
		if (m_pConnectMessage->IsVisible() && m_pProgressBar)
			m_pProgressBar->Display();
		graphic->SetTextStyle(0);
	}
}

void FrServerDlg::BuildChannelList()
{
	if (m_pWaitDlg)
		m_pWaitDlg->Close(true);
	if (m_pNotifyDlg)
		m_pNotifyDlg->Close(true);
	m_bConnecting = false;
	m_pChannelList->ClearItem();
	std::list<sChannelInfo>::iterator it = Doc()->m_channelList.begin();
	if (it != Doc()->m_channelList.end())
	{
		for (; it != Doc()->m_channelList.end(); ++it)
			m_pChannelList->AddItem(&*it);
		TurnOnChannels();
		m_pChannelList->GetScrollBar()->ScrollToFirst();
	}
	else
		TurnOffChannels();
}

void FrServerDlg::OnChannelListInit(int param)
{
	m_pChannelList = DYNAMIC_CAST(FrListBox, param);
	if (m_pChannelList)
		m_pImgNew = g_pFresh->GetManager()->GetBitmap("ITEMS", "mark_new");
}

void FrServerDlg::OnChannelListBtnDown()
{
	FrListItem* item = m_pChannelList->GetItemUnderCursor();
	if (!item)
		return;
	sChannelInfo* channel = (sChannelInfo*)item->pData;
	if (!channel)
		return;
	if (Doc()->m_curChannel.Uid == channel->Uid)
	{
		Close(true);
		return;
	}
	if (!(Doc()->m_myInfo.info.dwIdentity & 0x14))
	{
		if (Doc()->m_curGameServer.property & 0x20)
		{
			unsigned long games = Doc()->m_myInfo.stat.dwGameCount;
			if (games < 1)
				games = 1;
			if ((float)Doc()->m_myInfo.stat.dwNoMannerGameCount / games >= 0.4f)
			{
				SetLoginMessageAlert(
					"\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xb7\xfc\xc0\xcc 40%\xc0\xcc\xbb\xf3\xc0\xce \xc0\xaf\xc0\xfa\xb4\xc2 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x800) && Doc()->m_myInfo.stat.Level > 10)
			{
				SetLoginMessageAlert(
					"\xb7\xe7\xc5\xb0/\xba\xf1\xb1\xe2\xb3\xca\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x1000) &&
				(Doc()->m_myInfo.stat.Level < 11 ||
					Doc()->m_myInfo.stat.Level > 20))
			{
				SetLoginMessageAlert(
					"\xc1\xd6\xb4\xcf\xbe\xee/\xbd\xc3\xb4\xcf\xbe\xee\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x2000) && Doc()->m_myInfo.stat.Level < 21)
			{
				SetLoginMessageAlert(
					"\xbe\xc6\xb8\xb6\xc3\xdf\xbe\xee\xc0\xcc\xbb\xf3 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
		}
		else
		{
			if ((channel->Type & 0x800) && Doc()->m_myInfo.stat.Level > 5)
			{
				SetLoginMessageAlert(
					"\xb7\xe7\xc5\xb0\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x1000) &&
				(Doc()->m_myInfo.stat.Level < 6 ||
					Doc()->m_myInfo.stat.Level > 15))
			{
				SetLoginMessageAlert(
					"\xba\xf1\xb1\xe2\xb3\xca/\xc1\xd6\xb4\xcf\xbe\xee\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x2000) &&
				(Doc()->m_myInfo.stat.Level < 11 ||
					Doc()->m_myInfo.stat.Level > 20))
			{
				SetLoginMessageAlert(
					"\xc1\xd6\xb4\xcf\xbe\xee/\xbd\xc3\xb4\xcf\xbe\xee\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x80) && Doc()->m_myInfo.stat.Level < 6)
			{
				SetLoginMessageAlert(
					"\xba\xf1\xb1\xe2\xb3\xca \xc0\xcc\xbb\xf3 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x200) && Doc()->m_myInfo.stat.Level > 15)
			{
				SetLoginMessageAlert(
					"\xc1\xd6\xb4\xcf\xbe\xee \xc0\xcc\xc7\xcf\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
			if ((channel->Type & 0x400) && Doc()->m_myInfo.stat.Level < 11)
			{
				SetLoginMessageAlert(
					"\xc1\xd6\xb4\xcf\xbe\xee \xc0\xcc\xbb\xf3\xb8\xb8 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
				return;
			}
		}
	}
	m_pMessage->SetVisible(false);
	Doc()->m_selChannel = *channel;
	if (Doc()->m_bReturnLobbyNotice)
	{
		WSendPacket packet((enumClientPacket)0x83);
		packet.Encode1(channel->Uid);
		packet.Send(TO_GAME);
	}
	else
	{
		WSendPacket packet((enumClientPacket)4);
		packet.Encode1(channel->Uid);
		packet.Send(TO_GAME);
	}
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	if (NET()->IsConnected(WNetworkSystem::NET_MSN) &&
		CMessengerInfo::Instance()->GetComplete() == 4)
	{
		sChannelInfo& selected = Doc()->m_selChannel;
		Doc()->m_roomInfo.roomGuid = 0xffff;
		unsigned char& gameType = Doc()->m_roomInfo.gameType;
		gameType = 0xff;
		sUserPosition position;
		position.iServerGUID = Doc()->m_gameServerUID;
		position.iChannelUid = selected.Uid;
		wsprintf(position.csChannelName, selected.Name);
		position.iRoomIdx = 0xffff;
		position.iRoomType = -1;
		WSendPacket packet(0x23);
		packet.EncodeBuffer(&position, sizeof(position));
		packet.Send(TO_MSN);
	}
	m_pCloseBtn->SetVisible(false);
}

bool FrServerDlg::IsDisableEnterChannel(unsigned long flags,
	unsigned char level)
{
	if ((flags & 0x800) && Doc()->m_myInfo.stat.Level > 5)
		return true;
	if ((flags & 0x1000) &&
		(Doc()->m_myInfo.stat.Level < 6 || Doc()->m_myInfo.stat.Level > 15))
		return true;
	if ((flags & 0x2000) &&
		(Doc()->m_myInfo.stat.Level < 11 || Doc()->m_myInfo.stat.Level > 20))
		return true;
	if ((flags & 0x200) && Doc()->m_myInfo.stat.Level > 15)
		return true;
	if ((flags & 0x400) && Doc()->m_myInfo.stat.Level < 11)
		return true;
	return false;
}

void FrServerDlg::DrawChannelButton(const WPoint& point,
	const sChannelInfo* channel, bool selected)
{
	bool disabled =
		IsDisableEnterChannel(channel->Type, Doc()->m_myInfo.stat.Level);
	THEME_BUTTONS buttons;
	S5::THEME::GetChannelSelectButton(S5::UTIL::GetServerProperty(), buttons);
	std::string name;
	if (disabled)
		name = buttons.press;
	else if (selected)
		name = buttons.over;
	else
		name = buttons.normal;
	const Bitmap* image = g_pFresh->GetBitmap(name.c_str());
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->DrawTexture(image, WRect(point, image->Width(), image->Height()),
		0xffffffff, 0);
	if (!_stricmp(channel->Name, "\xb7\xe7\xc5\xb0") ||
		!_stricmp(channel->Name, "\xb7\xe7\xc5\xb0\xc3\xa4\xb3\xce"))
		graphic->DrawTexture(m_pImgNew,
			WRect(point, m_pImgNew->Width(), m_pImgNew->Height()), 0xffffffff,
			0);
	graphic->SetTextStyle(0);
	graphic->PrintText(WPoint(point.x + 76.0f, point.y + 15.0f), 0,
		channel->Name, NULL);
}

void FrServerDlg::OnChannelListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	sChannelInfo* channel = (sChannelInfo*)item->pData;
	if (!channel)
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	DrawChannelButton(item->pos, channel, item->underCursor);
	float x = item->pos.x + 76.0f;
	float y = item->pos.y + 31.0f;
	int unit = channel->Max_Num / 18;
	if (!unit)
		unit = 1;
	int count = min(18, (channel->Current_Num + unit - 1) / unit);
	WOverlay::DrawBox(g_view, WRect(x, y, 110.0f, 6.0f), 0, 0xff535353, 0.0f);
	for (int i = 0; i < count; ++i)
	{
		_WRECT rect = { x + i * 6 + 1.0f, y + 1.0f, 6.0f, 4.0f };
		WOverlay::DrawBox(g_view, rect, 0,
			static_cast<unsigned __int64>(channel->Max_Num) >
					channel->Current_Num
				? 0xfffeaf24
				: 0xffff0000,
			0.0f);
	}
	if (Doc()->m_curChannel.Uid == channel->Uid &&
		!_stricmp(Doc()->m_curChannel.Name, channel->Name))
		graphic->DrawTexture(m_pImgConnection,
			WRect(item->pos.x + 9.0f, item->pos.y + 3.0f,
				m_pImgConnection->Width(), m_pImgConnection->Height()),
			0xffffffff, 0);
}

void FrServerDlg::OnConnectMessageInit(int param)
{
	m_pConnectMessage = DYNAMIC_CAST(FrStatic, param);
	if (!m_pConnectMessage)
		return;
	m_pConnectMessage->SetVisible(false);
	if (!m_pProgressBar)
	{
		WRect progress = m_pConnectMessage->GetRect();
		WRect rect = progress;
		progress.y -= 25.0f;
		progress.w = 180.0f;
		progress.h = 30.0f;
		m_pProgressBar = new cProgressBar;
		m_pProgressBar->Initialize(progress, 3.0f, 3.0f);
		m_pProgressBar->SetImg("gauge_left_frm", "gauge_middle_frm",
			"gauge_right_frm", "gauge_bar_frm");
		m_pProgressBar->SetRange(0, 100);
		m_pProgressBar->SetPos(10);
		rect.x += 25.0f;
		m_pConnectMessage->SetRect(rect);
	}
}

void FrServerDlg::OnConnectCancleInit(int param)
{
	m_pConnectCancle = DYNAMIC_CAST(FrButton, param);
	m_pConnectCancle->Enable(false);
	m_pConnectCancle->SetVisible(false);
}

void FrServerDlg::OnConnectCancleUp()
{
	m_pConnectCancle->Enable(false);
	m_pConnectCancle->SetVisible(false);
	if (m_pConnectMessage->IsVisible())
		m_pConnectMessage->SetVisible(false);
	if (m_pProgressBar)
		m_pProgressBar->SetPos(0);
	FailedToConnectServer();
}

void FrServerDlg::OnMessageInit(int param)
{
	m_pMessage = DYNAMIC_CAST(FrStatic, param);
}

void FrServerDlg::OnCurServerInit(int param)
{
	m_pCurServer = DYNAMIC_CAST(FrStatic, param);
}

void FrServerDlg::OnCloseBtnInit(int param)
{
	m_pCloseBtn = DYNAMIC_CAST(FrButton, param);
	if (g_pFresh->IsCurrentLayout("SERVERLIST"))
		m_pCloseBtn->SetVisible(false);
}

void FrServerDlg::OnCloseLBtnUp()
{
	if (Doc()->m_curChannel.Uid == 0xff)
	{
		SetLoginMessageAlert(
			"\xc3\xa4\xb3\xce\xc0\xbb \xbc\xb1\xc5\xc3\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.");
		OpenQuitDlg();
	}
	else
	{
		FrForm::Close(true);
	}
}

void FrServerDlg::SetLoginMessage(const char* message)
{
	m_pMessage->SetCaption(message);
	m_pMessage->SetVisible(true);
	m_pMessage->SetTextColor(0xff000000, 0xffffffff);
	m_bAlertMessage = false;
}

void FrServerDlg::SetLoginMessageAlert(const char* message)
{
	m_pMessage->SetCaption(message);
	m_pMessage->SetVisible(true);
	m_pMessage->SetTextColor(0xffff0000, 0xffffffff);
	m_bAlertMessage = true;
}

void FrServerDlg::TurnOnChannels()
{
	m_pCurServer->SetCaption(Doc()->m_curGameServer.name);
	m_pMessage->SetVisible(false);
	m_pCurServer->SetVisible(true);
	m_pChannelList->SetVisible(true);
	m_pChannelList->Enable(true);
	m_pServerList->Enable(true);
}

void FrServerDlg::TurnOffChannels()
{
	m_pMessage->SetVisible(true);
	m_pCurServer->SetVisible(false);
	m_pChannelList->SetVisible(false);
	m_pServerList->Enable(true);
}

void FrServerDlg::FailedToConnectServer()
{
	if (NET()->IsConnected(WNetworkSystem::NET_GAME))
		NET()->ForceShutDown(WNetworkSystem::NET_GAME);
	Doc()->m_curGameServer.guid = 0;
	m_bConnecting = false;
	m_state = (gsLoginState_t)0;
	m_pCurServer->SetCaption(Doc()->m_curGameServer.name);
	m_pMessage->SetVisible(true);
	m_pCurServer->SetVisible(true);
	m_pChannelList->SetVisible(false);
	m_pServerList->Enable(true);
}

void FrServerDlg::OnNotify(const char* message, unsigned long style)
{
	if (m_pNotifyDlg)
		m_pNotifyDlg->Close(true);
	m_pNotifyDlg =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
	m_pNotifyDlg->SetMessage(message, false);
	m_pNotifyDlg->Open((FRESH_PFN_RESULT)&FrServerDlg::OnNotifyResult, style);
}

bool FrServerDlg::OnNotifyResult(int result, FrForm* form)
{
	m_pNotifyDlg = NULL;
	return true;
}

void FrServerDlg::OnWaitNotify(const char* message, unsigned long style)
{
	if (m_pWaitDlg)
		m_pWaitDlg->Close(true);
	m_pWaitDlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "wait", NULL);
	m_pWaitDlg->SetMessage(message, false);
	m_pWaitDlg->Open((FRESH_PFN_RESULT)&FrServerDlg::OnWaitNotifyResult, style);
}

bool FrServerDlg::OnWaitNotifyResult(int result, FrForm* form)
{
	m_pWaitDlg = NULL;
	return true;
}

void FrServerDlg::OpenQuitDlg()
{
	m_pQuitDlg =
		CreateForm<FrLogoutDlg>(g_pFresh->GetManager(), this, "quit", NULL);
	m_pQuitDlg->DisableServerSelect();
	m_pQuitDlg->Open((FRESH_PFN_RESULT)&FrServerDlg::OnQuitDlgResult, 1);
}

bool FrServerDlg::OnQuitDlgResult(int result, FrForm* form)
{
	m_pQuitDlg = NULL;
	return true;
}
