#include "clientsetting.h"
#include "logininfo.h"
#include "logindlg.h"
#include "generichttpclient.h"
#include "projectg.h"
#include "shareddoc.h"
#include "frstatic.h"
#include "frdesktop.h"
#include "binstr.h"
#include <process.h>

namespace
{
	struct _ValidId
	{
		bool bInvalid;

		_ValidId()
			: bInvalid(false)
		{
		}
		void operator()(char c)
		{
			if (!isalnum(c))
				bInvalid = true;
		}
	};

	class cNTAUTHPARAM
	{
	public:
		cNTAUTHPARAM(const std::string& _id, const std::string& _pwd)
			: id(_id), pwd(_pwd)
		{
		}

		std::string id;
		std::string pwd;
	};
}

extern Fresh* g_pFresh;

extern bool g_bQuit;
eLoginState FrLoginDlg::m_connState = (eLoginState)0;
unsigned __stdcall HttpGetNtreevUserAuthKey(void* param);
unsigned __stdcall HttpGetNhnUserAuthKey(void* param);

IMPLEMENT_OBJECT(FrLoginDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrLoginDlg, FrForm)

ON_FRESH_VI("id", FRCMD_INIT, FrLoginDlg::OnIdInit)
ON_FRESH_BI("id", FRCMD_ENTERKEY, FrLoginDlg::OnEnterKey)
ON_FRESH_VI("password", FRCMD_INIT, FrLoginDlg::OnPasswordInit)
ON_FRESH_BI("password", FRCMD_ENTERKEY, FrLoginDlg::OnEnterKey)
ON_FRESH_VI("auto_id", FRCMD_INIT, FrLoginDlg::OnAutoIdInit)
ON_FRESH_VV("auto_id", FRCMD_LBUTTONUP, FrLoginDlg::OnAutoIdBtnUp)
ON_FRESH_VI("view", FRCMD_INIT, FrLoginDlg::OnViewInit)
ON_FRESH_VI("login", FRCMD_INIT, FrLoginDlg::OnLoginInit)
ON_FRESH_VV("login", FRCMD_LBUTTONUP, FrLoginDlg::OnLoginBtnUp)
ON_FRESH_VI("join", FRCMD_INIT, FrLoginDlg::OnJoinBtnInit)
ON_FRESH_VI("find", FRCMD_INIT, FrLoginDlg::OnFindBtnInit)
ON_FRESH_VV("join", FRCMD_LBUTTONUP, FrLoginDlg::OnJoinBtnUp)
ON_FRESH_VV("find", FRCMD_LBUTTONUP, FrLoginDlg::OnFindBtnUp)
ON_FRESH_VI("logo1", FRCMD_INIT, FrLoginDlg::OnParanAreaInit)
ON_FRESH_VI("logo2", FRCMD_INIT, FrLoginDlg::OnParanAreaInit)
ON_FRESH_VI("bkframe1", FRCMD_INIT, FrLoginDlg::OnHideAreaInit)
ON_FRESH_VV("bkframe1", FRCMD_OWNERDRAW, FrLoginDlg::OnHideAreaDraw)
ON_FRESH_VI("ID_EDIT_NOTICE", FRCMD_INIT, FrLoginDlg::OnLoginNoticeInit)
ON_FRESH_VI("icon1", FRCMD_INIT, FrLoginDlg::OnHideAreaInit)
ON_FRESH_VI("icon2", FRCMD_INIT, FrLoginDlg::OnHideAreaInit)
ON_FRESH_VI("box1", FRCMD_INIT, FrLoginDlg::OnHideAreaInit)
ON_FRESH_VI("box2", FRCMD_INIT, FrLoginDlg::OnHideAreaInit)
ON_FRESH_VI("s_id", FRCMD_INIT, FrLoginDlg::OnHideStaticInit)
ON_FRESH_VI("s_passwd", FRCMD_INIT, FrLoginDlg::OnHideStaticInit)
ON_FRESH_VI("s_save", FRCMD_INIT, FrLoginDlg::OnHideStaticInit)

END_FRESH_MSGMAP()

FrLoginDlg::FrLoginDlg()
{
	m_pView = NULL;
	m_pLogin = NULL;
	m_failCount = 0;
	m_connState = (eLoginState)0;
	m_reserved170 = 0;
	m_reserved168 = 0;
	m_pJoin = NULL;
	m_pFind = NULL;
	Doc()->SetNeedFirstLoginAlarm(true);
}

FrLoginDlg::~FrLoginDlg()
{
}

void FrLoginDlg::SetNoticeMsg(const char* text)
{
	if (m_pView)
		m_pView->AddLine(text, 0, 0);
	EnableLoginControls(true);
}

void FrLoginDlg::SetState(eLoginState state)
{
	m_connState = state;
}

eLoginState FrLoginDlg::GetState()
{
	return m_connState;
}

unsigned __stdcall HttpGetNtreevUserAuthKey(void* param)
{
	cNTAUTHPARAM* auth = (cNTAUTHPARAM*)param;
	GenericHTTPClient client;
	client.InitilizePostArguments();
	client.AddPostArguments("id", auth->id.c_str(), FALSE);
	client.AddPostArguments("pwd", auth->pwd.c_str(), FALSE);
	client.AddPostArguments("gamecode", 9UL);
	FrWnd* wnd = g_pFresh->GetDesktop()->FindChildForm("login");
	FrLoginDlg* dlg = DYNAMIC_CAST(FrLoginDlg, wnd);
	int success = client.Request(
		"http://qa.pangya.gametree.co.kr/Secure/Login/LoginForGame.aspx", 3,
		"MERONG(0.9/;p)");
	const char* response = client.QueryHTTPResponse();
	if (success && strlen(response) <= 520)
	{
		cHttpNtreevAuthResult result(response);
		if (result.GetResult())
		{
			std::string key = result.GetFieldValue("AuthKey");
			std::string member = result.GetFieldValue("MemberNo");
			std::string pcBang = result.GetFieldValue("PCBangNo");

			if (dlg)
			{
				LOGININFO()->SetPw(key);
				LOGININFO()->SetAuthUid(strtoul(member.c_str(), NULL, 10));
				LOGININFO()->EnablePcBang(pcBang == "0" ? FALSE : TRUE);
				dlg->BeginConnect();
			}
			else
			{
				FrForm* notify = CreateForm<FrForm>(g_pFresh->GetManager(),
					NULL, "notify", NULL);
				notify->SetMessage(
					"\xb7\xce\xb1\xd7\xc0\xce \xc0\xce\xc1\xf5 \xbd\xc3\xb0\xa3 \xc3\xca\xb0\xfa. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xcf\xbc\xbc\xbf\xe4.",
					false);
			}
		}
		else
		{
			std::string message = result.GetMessageA();
			if (dlg)
			{
				dlg->SetNoticeMsg(message.c_str());
				dlg->EnableLoginControls(true);
			}
			else
			{
				FrForm* notify = CreateForm<FrForm>(g_pFresh->GetManager(),
					NULL, "notify", NULL);
				notify->SetMessage(message.c_str(), false);
				notify->Open(NULL, FrESCAPE | FrENTER);
			}
		}
		delete auth;
	}
	else
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc0\xa5\xbc\xad\xba\xf1\xbd\xba \xc0\xce\xc1\xf5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		if (dlg)
			dlg->EnableLoginControls(true);
		delete auth;
	}
	return 0;
}

unsigned __stdcall HttpGetNhnUserAuthKey(void* param)
{
	cNTAUTHPARAM* auth = (cNTAUTHPARAM*)param;
	GenericHTTPClient client;
	client.InitilizePostArguments();
	client.AddPostArguments("id", auth->id.c_str(), FALSE);
	client.AddPostArguments("pwd", auth->pwd.c_str(), FALSE);
	client.AddPostArguments("gamecode", 9UL);
	FrWnd* wnd = g_pFresh->GetDesktop()->FindChildForm("login");
	FrLoginDlg* dlg = DYNAMIC_CAST(FrLoginDlg, wnd);
	int success = client.Request(
		"http://qa.pangya.gametree.co.kr/Secure/Login/LoginForGame.aspx", 3,
		"MERONG(0.9/;p)");
	const char* response = client.QueryHTTPResponse();
	if (success && strlen(response) <= 520)
	{
		cHttpNtreevAuthResult result(response);
		if (result.GetResult())
		{
			std::string key = result.GetFieldValue("AuthKey");
			std::string member = result.GetFieldValue("MemberNo");
			std::string pcBang = result.GetFieldValue("PCBangNo");
			std::string siteMember = result.GetFieldValue("SiteMemberNo");
			std::string zipCode = result.GetFieldValue("ZipCode");
			std::string birthday = result.GetFieldValue("Birthday");
			std::string siteCode = result.GetFieldValue("SiteCode");
			std::string sex = result.GetFieldValue("Sex");
			if (dlg)
			{
				LOGININFO()->SetZipCode(zipCode);
				LOGININFO()->SetBirthDay(birthday);
				LOGININFO()->SetGender(sex.find('M') == std::string::npos);
				NhnLogin* nhn = (NhnLogin*)LOGININFO()->QueryInterface();
				nhn->SetNHNMemberNumber(_atoi64(siteMember.c_str()));
				LOGININFO()->SetPw(key);
				LOGININFO()->SetAuthUid(strtoul(member.c_str(), NULL, 10));
				LOGININFO()->EnablePcBang(pcBang == "0" ? FALSE : TRUE);
				dlg->BeginConnect();
			}
			else
			{
				FrForm* notify = CreateForm<FrForm>(g_pFresh->GetManager(),
					NULL, "notify", NULL);
				notify->SetMessage(
					"\xb7\xce\xb1\xd7\xc0\xce \xc0\xce\xc1\xf5 \xbd\xc3\xb0\xa3 \xc3\xca\xb0\xfa. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xcf\xbc\xbc\xbf\xe4.",
					false);
			}
		}
		else
		{
			std::string message = result.GetMessageA();
			if (dlg)
			{
				dlg->SetNoticeMsg(message.c_str());
				dlg->EnableLoginControls(true);
			}
			else
			{
				FrForm* notify = CreateForm<FrForm>(g_pFresh->GetManager(),
					NULL, "notify", NULL);
				notify->SetMessage(message.c_str(), false);
				notify->Open(NULL, FrESCAPE | FrENTER);
			}
		}
		delete auth;
	}
	else
	{
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
			(int)"\xc0\xa5\xbc\xad\xba\xf1\xbd\xba \xc0\xce\xc1\xf5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0));
		if (dlg)
			dlg->EnableLoginControls(true);
		delete auth;
	}
	return 0;
}

void FrLoginDlg::BeginClientLogin()
{
	if (m_pId && m_pId->IsVisible())
	{
		char id[260] = "";
		strcpy(id, m_pId->GetLine(1, 0));
		m_id = TrimA(id, " \t\n\r\0");
		if (m_id.empty())
		{
			if (m_pView)
				m_pView->AddLine(
					"\xbe\xc6\xc0\xcc\xb5\xf0\xb8\xa6 \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0);
			if (m_pId)
				m_pId->SetKeyFocus(true);
			return;
		}
	}
	if (m_pPassword && m_pPassword->IsVisible())
	{
		char password[260] = "";
		strcpy(password, m_pPassword->GetLine(1, 0));
		m_passwd = TrimA(password, " \t\n\r\0");
		if (m_passwd.empty())
		{
			if (m_pView)
				m_pView->AddLine(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb8\xa6 \xc0\xd4\xb7\xc2\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0);
			if (m_pPassword)
				m_pPassword->SetKeyFocus(true);
			return;
		}
	}
	_ValidId valid = std::for_each(m_id.begin(), m_id.end(), _ValidId());
	if (valid.bInvalid && m_pView)
	{
		m_pView->AddLine(
			"\xbe\xc6\xc0\xcc\xb5\xf0\xb4\xc2 \xbf\xb5\xb9\xae\xc0\xda\xbf\xcd \xbc\xfd\xc0\xda, '_'\xb8\xb8 \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 \xbc\xf6 ",
			0, 0);
		m_pView->AddLine("\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.", 0, 0);
	}
	else
	{
		LOGININFO()->SetId(m_id);
		LOGININFO()->SetPw(m_passwd);
		cNTAUTHPARAM* param = new cNTAUTHPARAM(m_id, m_passwd);
		if (!_beginthreadex(NULL, 0, HttpGetNtreevUserAuthKey, param, 0, NULL))
		{
			if (m_pView)
			{
				m_pView->AddLine(
					"\xb7\xce\xb1\xd7\xc0\xce\xbf\xa1 \xc0\xe5\xbe\xd6\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5 \xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.",
					0, 0);
				m_pView->AddLine(
					"\xb9\xae\xc1\xa6\xb0\xa1 \xc1\xf6\xbc\xd3 \xb5\xc7\xb8\xe9 \xb0\xb3\xb9\xdf\xc6\xc0\xbf\xa1 \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
					0, 0);
			}
			delete param;
		}
		else
			EnableLoginControls(false);
	}
}

bool FrLoginDlg::PrepareWebLogin()
{
	return strlen(LOGINID()) && strlen(LOGINPW());
}

void FrLoginDlg::EnableLoginControls(bool enable)
{
	m_pId->Enable(enable);
	m_pPassword->Enable(enable);
	m_pLogin->Enable(enable);
}

void FrLoginDlg::BeginConnect()
{
	m_defaultRetCode = FrNONE;
	m_id = CLoginInfo::Instance()->Id();
	m_passwd = CLoginInfo::Instance()->Passwd();
	char* id = Doc()->m_myInfo.info.sID;
	memset(id, 0, sizeof(Doc()->m_myInfo.info.sID));
	strncpy(id, m_id.c_str(), 21);
	Doc()->m_password = m_passwd;
	NET()->Init(WNetworkSystem::NET_MAX);
	if (m_pView)
		m_pView->AddLine(MakeStr("%s \xc1\xa2\xbc\xd3 \xc1\xdf...|", LOGINID()),
			0, 0);
	m_connState = (eLoginState)1;
	NET()->SendMessage(WNetworkSystem::NET_LOGIN, (NetworkUnit::eEvent)0);
}

void FrLoginDlg::Login()
{
	if (m_pLogin && m_pLogin->IsEnabled())
	{
		if (CLoginInfo::Instance()->IsWebLogin())
		{
			if (PrepareWebLogin())
			{
				EnableLoginControls(false);
				BeginConnect();
			}
			else if (m_pView)
				m_pView->AddLine(
					"\xb7\xce\xb1\xd7\xc0\xce \xbd\xc3\xbd\xba\xc5\xdb \xc1\xa4\xba\xf1\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, 0);
		}
		else
			BeginClientLogin();
	}
}

bool FrLoginDlg::OnJoinDlgResult(int result, FrForm*)
{
	if (result == FrOK)
	{
		ShellExecuteA(NULL, NULL,
			"http://qa.www.gametree.co.kr/SignUp/Join.aspx?rsn=9", NULL, NULL,
			SW_MAXIMIZE);
		if (!COption::Instance()->vIsWindowed())
			g_bQuit = true;
	}
	return true;
}

bool FrLoginDlg::OnDisconPrevSession(int result, FrForm*)
{
	if (result == FrOK)
	{
		CProjectG::Instance()->m_bMoveLoginServer = false;
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0, (int)"SERVERLIST", 0, 0, 0, 0);
		WSendPacket packet(4);
		packet.Send(TO_LOGIN);
	}
	else
	{
		m_pId->SetLine(1, "", 0, 0, 0);
		m_pId->ClearLine();
		m_pPassword->SetLine(1, "", 0, 0, 0);
		m_pPassword->ClearLine();
		m_pId->SetKeyFocus(true);
		m_connState = (eLoginState)5;
		SetFadeout(true);
	}
	return true;
}

bool FrLoginDlg::OnFindDlgResult(int result, FrForm*)
{
	if (result == FrOK)
	{
		ShellExecuteA(NULL, NULL,
			"http://qa.www.gametree.co.kr/Secure/MyPage/Member/FindId.aspx",
			NULL, NULL, SW_MAXIMIZE);
		if (!COption::Instance()->vIsWindowed())
			g_bQuit = true;
	}
	return true;
}

void FrLoginDlg::OnProc(float delta)
{
	bool autoLogin = false;
	if (m_connState == 0)
	{
		if (CLoginInfo::Instance()->IsWebLogin())
			autoLogin = true;
		if ((m_connState == 0 && CProjectG::Instance()->m_bMoveLoginServer) ||
			autoLogin)
			Login();
	}
	if (NET())
	{
		WNetworkSystem* network = NET();
		switch (m_connState)
		{
		case 1:
		{
			int lines = m_pView->GetLineNum();
			if (lines)
			{
				char spin[3] = { '-', '/', '|' };
				const char* line = m_pView->GetLine(lines, 0);
				unsigned int length = strlen(line);
				char text[1024];
				UINT_PTR offset = (UINT_PTR)text - (UINT_PTR)line;
				const char* cursor = line;
				char ch;
				do
				{
					ch = *cursor;
					*(char*)(offset + (UINT_PTR)cursor) = ch;
					++cursor;
				} while (ch);
				if (text[length - 1] == '-' || text[length - 1] == '/' ||
					text[length - 1] == '|')
				{
					static float rotation = 0.0f;
					int step = (int)rotation;
					rotation += delta * 10.0f;
					text[length - 1] = spin[step % 3];
					m_pView->SetLine(lines, text, 0, 0, 0);
				}
			}
			if (network->IsConnectionComplete(WNetworkSystem::NET_LOGIN) &&
				!network->IsConnected(WNetworkSystem::NET_LOGIN))
			{
				m_connState = (eLoginState)5;
				if (m_pView)
				{
					m_pView->AddLine(
						"\xb7\xce\xb1\xd7\xc0\xce \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0);
					EnableLoginControls(true);
				}
			}
			break;
		}
		case 4:
			if (m_pView)
			{
				m_pView->AddLine("\xb7\xce\xb1\xd7\xc0\xce \xbc\xba\xb0\xf8.",
					0, 0);
				m_pView->AddLine(
					"\xbc\xad\xb9\xf6\xb8\xae\xbd\xba\xc6\xae\xb8\xa6 \xb9\xde\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0);
			}
			if (m_pAutoId->GetStatus() == FrButton::PRESSED)
				COption::Instance()->gSetLastLoginID(m_id.c_str());
			m_connState = (eLoginState)3;
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			break;
		case 6:
			EnableLoginControls(true);
			ResetInput(
				"\xbe\xc6\xc0\xcc\xb5\xf0\xb0\xa1 \xc0\xdf\xb8\xf8\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 8:
		{
			m_connState = (eLoginState)3;
			if (IsLocalContent((localContentType_t)90))
			{
				if (CProjectG::Instance()->m_bMoveLoginServer == true)
				{
					WSendPacket packet(4);
					packet.Send(TO_LOGIN);
					CProjectG::Instance()->m_bMoveLoginServer = false;
				}
				else
				{
					FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "notify_okcancel", NULL);
					if (dlg)
					{
						dlg->SetMessage(
							"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xb5\xf0\xb0\xa1 \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.\012\xc0\xcc\xc0\xfc \xc1\xa2\xbc\xd3\xc0\xbb \xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
							false);
						dlg->Open(
							(FRESH_PFN_RESULT)&FrLoginDlg::OnDisconPrevSession,
							0);
					}
				}
			}
			else
			{
				FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_okcancel", NULL);
				if (dlg)
				{
					dlg->SetMessage(
						"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xb5\xf0\xb0\xa1 \xbb\xe7\xbf\xeb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.\012\xc0\xcc\xc0\xfc \xc1\xa2\xbc\xd3\xc0\xbb \xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
						false);
					dlg->Open(
						(FRESH_PFN_RESULT)&FrLoginDlg::OnDisconPrevSession, 0);
				}
			}
			break;
		}
		case 7:
			EnableLoginControls(true);
			++m_failCount;
			if (m_pView)
			{
				m_pView->AddLine(
					"\xbe\xc6\xc0\xcc\xb5\xf0\xb3\xaa \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc0\xdf\xb8\xf8 \xc0\xd4\xb7\xc2\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0);
				m_pView->AddLine(
					"\\c0xffff0000,0xffffffff\\c'Caps Lock'\\c0xff000000,0xffffffff\\c \xc5\xb0\xb0\xa1 \xb4\xad\xb7\xaf\xc1\xae \xc0\xd6\xb4\xc2\xc1\xf6 \xc8\xae\xc0\xce\xc7\xd8\xba\xb8\xbc\xbc\xbf\xe4.",
					0, 0);
				m_connState = (eLoginState)5;
				if (m_failCount > 4)
					g_bQuit = true;
				m_pPassword->SetLine(1, "", 0, 0, 0);
				m_pPassword->ClearLine();
				m_pPassword->SetKeyFocus(true);
			}
			break;
		case 12:
			EnableLoginControls(true);
			ResetInput(
				"\xbc\xad\xb9\xf6\xb9\xae\xc1\xa6\xb7\xce \xb7\xce\xb1\xd7\xc0\xce \xc7\xd2\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			m_connState = (eLoginState)5;
			break;
		case 16:
			EnableLoginControls(true);
			m_pView->AddLine(
				"\xb0\xa1\xc0\xd4\xb5\xc8 \xc1\xd6\xb9\xce \xb9\xf8\xc8\xa3\xb0\xa1 \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0);
			m_pView->AddLine(
				"\\c0xffff0000,0xffffffff\\c'\xb0\xed\xb0\xb4 \xc1\xf6\xbf\xf8'\\c0xff000000,0xffffffff\\c \xc0\xc7 \xb8\xde\xc0\xcf \xb9\xae\xc0\xc7\xb8\xa6 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				0, 0);
			m_connState = (eLoginState)5;
			break;
		case 23:
			EnableLoginControls(true);
			ResetInput(
				"\xb0\xe8\xc1\xa4 \xc0\xcc\xc0\xfc\xc0\xcc \xb5\xc7\xbe\xee \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 9:
		case 10:
			EnableLoginControls(true);
			if (m_pView)
			{
				m_pView->AddText(MakeStr("\\c0xffff0000,0xffffffff\\c%s",
									 Doc()->m_chatModeName.c_str()),
					0, true);
				m_pView->AddText(
					"\\c0xff000000,0xffffffff\\c\xb0\xed\xb0\xb4 \xc1\xf6\xbf\xf8 \xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
					0, true);
				m_connState = (eLoginState)5;
			}
			break;
		case 17:
			EnableLoginControls(true);
			ResetInput(
				"\xc8\xb8\xbf\xf8\xc0\xbb \xc5\xbb\xc5\xf0\xc7\xd1 \xbe\xc6\xc0\xcc\xb5\xf0\xc0\xd4\xb4\xcf\xb4\xd9.");
			break;
		case 15:
			EnableLoginControls(true);
			if (m_pView)
			{
				m_pView->AddText(
					"(\xbe\xcb\xb8\xb2)-----------------------------------", 0,
					true);
				m_pView->AddText(
					"\\c0xffff0000,0xffffffff\\c\xb8\xb8 14\xbc\xbc \xb9\xcc\xb8\xb8\\c0xffffffff,0xff000000\\c\xc0\xc7 \xc8\xb8\xbf\xf8\xc0\xba \\c0xffff0000,0xffffffff\\c\xba\xce\xb8\xf0\xb4\xd4\xc0\xc7 \xb5\xbf\xc0\xc7\\c0xffffffff,0xff000000\\c\xb0\xa1 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9. \xc7\xf6\xc0\xe7 \xba\xce\xb8\xf0\xb4\xd4 \xb5\xbf\xc0\xc7 \xb0\xa1\xc0\xd4\xc0\xcc \xb5\xc7\xbe\xee \xc0\xd6\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \\c0xffff0000,0xffffffff\\c\xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\\c0xffffffff,0xff000000\\c\xbf\xa1 \xb7\xce\xb1\xd7\xc0\xce\xc7\xd1 \xc8\xc4 \\c0xffff0000,0xffffffff\\c\xba\xce\xb8\xf0\xb5\xbf\xc0\xc7\xbc\xad \xc0\xe7\xb9\xdf\xbc\xdb\\c0xffffffff,0xff000000\\c\xc0\xbb \xb4\xa9\xb8\xa3\xb8\xe9 \xba\xce\xb8\xf0\xb4\xd4\xb5\xbf\xc0\xc7 \xbd\xc5\xc3\xbb\xc0\xcc \xc0\xcc\xb7\xe7\xbe\xee\xc1\xf6\xb0\xed \xb9\xd9\xb7\xce \xb0\xd4\xc0\xd3\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
					0, true);
				m_pView->AddText("----------------------------------------", 0,
					true);
				m_connState = (eLoginState)5;
			}
			break;
		case 14:
			EnableLoginControls(true);
			if (m_pView)
			{
				m_pView->AddText(
					"(\xbe\xcb\xb8\xb2)----------------------------------", 0,
					true);
				m_pView->AddText(
					"\\c0xffff0000,0xffffffff\\c\xb8\xb8 14\xbc\xbc \xb9\xcc\xb8\xb8\\c0xffffffff,0xff000000\\c\xc0\xc7 \xc8\xb8\xbf\xf8\xc0\xba \\c0xffff0000,0xffffffff\\c\xba\xce\xb8\xf0\xb4\xd4\xc0\xc7 \xb5\xbf\xc0\xc7\\c0xffffffff,0xff000000\\c\xb0\xa1 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9. \xba\xce\xb8\xf0\xb4\xd4 \xb5\xbf\xc0\xc7\xbc\xad\xb0\xa1 \xbe\xc6\xc1\xf7 \xbd\xc2\xc0\xce\xb5\xc7\xc1\xf6 \xbe\xca\xc0\xba \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, true);
				m_pView->AddText(
					"\012\\c0xff0000ff,0xffffffff\\c\xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1 \xb9\xe6\xb9\xae\xc7\xcf\xbf\xa9 \xb9\xae\xc0\xc7\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.\\c0xffffffff,0xff000000\\c",
					0, true);
				m_pView->AddText("---------------------------------------", 0,
					true);
				m_connState = (eLoginState)5;
			}
			break;
		case 11:
			EnableLoginControls(true);
			if (m_pView)
			{
				m_pView->AddLine(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 5\xc8\xb8 \xc0\xdf\xb8\xf8 \xc0\xd4\xb7\xc2\xb5\xc7\xbe\xee",
					0, 0);
				m_pView->AddLine(
					"1\xbd\xc3\xb0\xa3\xb5\xbf\xbe\xc8 \xbc\xad\xb9\xf6\xbf\xa1 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9!",
					0, 0);
				m_pView->AddLine("------------------------------------", 0, 0);
				m_pView->AddLine(
					"====== \xc6\xce\xbe\xdf\xb8\xa6 \xc1\xbe\xb7\xe1\xc7\xcf\xb0\xda\xbd\xc0\xb4\xcf\xb4\xd9. ======",
					0, 0);
				m_pView->AddLine("------------------------------------", 0, 0);
			}
			ResetInput(NULL);
			g_bQuit = true;
			break;
		case 18:
			EnableLoginControls(true);
			ResetInput(
				"\xc0\xd4\xc0\xe5 \xc0\xda\xb0\xdd\xc0\xbb \xb9\xde\xc0\xba \xc0\xaf\xc0\xfa\xb8\xb8 \xc1\xa2\xbc\xd3\xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.");
			break;
		case 13:
			EnableLoginControls(true);
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 0, (int)"CREATE", 0, 0, 0, 0);
			break;
		case 19:
			EnableLoginControls(true);
			if (m_pView)
				m_pView->AddLine(
					"\xc1\xa1\xb0\xcb\xc0\xb8\xb7\xce \xc0\xce\xc7\xd8 \xb7\xce\xb1\xd7\xc0\xce \xc1\xdf\xc1\xf6 \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0);
			ResetInput(NULL);
			break;
		case 24:
			EnableLoginControls(true);
			if (m_pView)
				m_pView->AddLine(
					"\xb0\xed\xb0\xb4\xbc\xbe\xc5\xcd 1:1 \xb9\xae\xc0\xc7\xb7\xce \xc1\xa2\xbc\xf6 \xba\xce\xc5\xb9\xb5\xe5\xb8\xb3\xb4\xcf\xb4\xd9.",
					0, 0);
			ResetInput(NULL);
			break;
		case 20:
			EnableLoginControls(true);
			m_pView->AddText("PangYa is not available from your area.", 0,
				true);
			m_pView->AddText("We are truly sorry for any inconvenience.", 0,
				true);
			ResetInput(NULL);
			m_connState = (eLoginState)5;
			break;
		case 22:
			EnableLoginControls(true);
			ResetInput(
				"\xc0\xce\xc1\xf5\xc0\xcc \xbf\xcf\xb7\xe1\xb5\xc7\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.\xb5\xee\xb7\xcf \xb8\xde\xc0\xcf\xc0\xbb \xc8\xae\xc0\xce\xc7\xd8 \xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.");
			break;
		case 21:
			EnableLoginControls(true);
			m_pView->AddText(
				"\xc0\xd4\xc0\xe5\xb1\xc7\xc0\xcc \xbe\xf8\xbe\xee\xbc\xad \xb7\xce\xb1\xd7\xc0\xce \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
			m_pView->AddText(
				"\xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xb9\xab\xb7\xe1 \xc0\xd4\xc0\xe5\xb1\xc7\xc0\xbb \xb9\xdf\xb1\xde \xb9\xde\xc0\xb8\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
			m_connState = (eLoginState)5;
			break;
		case 25:
			if (IsLocalContent((localContentType_t)129))
			{
				EnableLoginControls(true);
				if (m_pView)
				{
					m_pView->AddText(
						"\xc0\xdf\xb8\xf8\xb5\xc8 \xc0\xaf\xc0\xfa\xc1\xa4\xba\xb8\xb8\xa6 \xc0\xd0\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, true);
					m_pView->AddText(
						"\xc0\xe7 \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbd\xc3\xb0\xc5\xb3\xaa, \xb0\xed\xb0\xb4 \xc1\xf6\xbf\xf8 \xbc\xbe\xc5\xcd\xb7\xce \xb9\xae\xc0\xc7 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
						0, true);
				}
				ResetInput(NULL);
				m_connState = (eLoginState)5;
			}
			break;
		case 26:
			EnableLoginControls(true);
			if (m_pView)
				m_pView->AddText(
					"CBT\xb1\xe2\xb0\xa3\xc0\xba \xbd\xc5\xb1\xd4\xb5\xee\xb7\xcf\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, true);
			ResetInput(NULL);
			m_connState = (eLoginState)5;
			break;
		case 27:
			EnableLoginControls(true);
			m_connState = (eLoginState)5;
			break;
		}
	}
}

void FrLoginDlg::OnIdInit(int param)
{
	m_pId = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pId->m_nFlags.Enable(FWF_PRIVACY);
	m_pId->SetCharLimit(22, true);
	m_pId->SetLine(1, COption::Instance()->gGetLastLoginID(), 0, 0, 0);
	if (CProjectG::Instance()->m_bMoveLoginServer)
		m_pId->SetLine(1, CProjectG::Instance()->m_moveLoginId.c_str(), 0, 0,
			0);
	const char* line = m_pId->GetLine(1, 0);
	if (!line || !strlen(line))
		m_pId->SetKeyFocus(true);
	if (CLoginInfo::Instance()->IsWebLogin())
	{
		m_pId->SetLine(1, LOGINID(), 0, 0, 0);
		m_pId->SetVisible(false);
	}
}

void FrLoginDlg::OnPasswordInit(int param)
{
	m_pPassword = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pPassword)
	{
		if (CLoginInfo::Instance()->IsWebLogin())
			m_pPassword->SetLine(1, LOGINPW(), 0, 0, 0);
		else if (CProjectG::Instance()->m_bMoveLoginServer)
			m_pPassword->SetLine(1,
				CProjectG::Instance()->m_moveLoginPassword.c_str(), 0, 0, 0);
		const char* line = m_pId->GetLine(1, 0);
		if (line && strlen(line))
			m_pPassword->SetKeyFocus(true);
		if (CLoginInfo::Instance()->IsWebLogin())
			m_pPassword->SetVisible(false);
	}
}

bool FrLoginDlg::OnEnterKey(int)
{
	return false;
}

void FrLoginDlg::OnAutoIdInit(int param)
{
	m_pAutoId = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	const char* id = COption::Instance()->gGetLastLoginID();
	if (id && strlen(id))
		m_pAutoId->SetStatus(FrButton::PRESSED);
	if (CLoginInfo::Instance()->IsWebLogin())
		m_pAutoId->SetVisible(false);
}

void FrLoginDlg::OnAutoIdBtnUp()
{
	if (m_pAutoId->GetStatus() != FrButton::PRESSED)
		COption::Instance()->gSetLastLoginID("");
}

void FrLoginDlg::OnViewInit(int param)
{
	m_pView = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pView)
		m_pView->AddText(
			"\xc6\xce\xbe\xdf\xc0\xc7 \xbc\xbc\xb0\xe8\xbf\xa1 \xbf\xc0\xbd\xc5 \xb0\xcd\xc0\xbb \xc8\xaf\xbf\xb5\xc7\xd5\xb4\xcf\xb4\xd9",
			0, true);
}

const char* FrLoginDlg::QueryWelcomeMessage()
{
	return NULL;
}

void FrLoginDlg::OnLoginInit(int param)
{
	m_pLogin = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (IsLocalContent((localContentType_t)16))
		if (CLoginInfo::Instance()->IsWebLogin())
			m_pLogin->SetVisible(false);
}

void FrLoginDlg::OnLoginBtnUp()
{
	Login();
}

void FrLoginDlg::OnJoinBtnInit(int param)
{
	m_pJoin = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pJoin && CLoginInfo::Instance()->IsWebLogin())
		m_pJoin->SetVisible(false);
}

void FrLoginDlg::OnFindBtnInit(int param)
{
	m_pFind = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pFind && CLoginInfo::Instance()->IsWebLogin())
		m_pFind->SetVisible(false);
}

void FrLoginDlg::OnJoinBtnUp()
{
	FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
		"notify_okcancel", NULL);
	if (dlg)
	{
		dlg->SetMessage(
			"\xc6\xce\xbe\xdf \xbe\xc6\xc0\xcc\xb5\xf0\xb8\xa6 \xb8\xb8\xb5\xe5\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		dlg->Open((FRESH_PFN_RESULT)&FrLoginDlg::OnJoinDlgResult, 0);
	}
}

void FrLoginDlg::OnFindBtnUp()
{
	FrForm* dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
		"notify_okcancel", NULL);
	if (dlg)
	{
		dlg->SetMessage(
			"\xc6\xce\xbe\xdf \xbe\xc6\xc0\xcc\xb5\xf0, \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb8\xa6 \xc3\xa3\xc0\xb8\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		dlg->Open((FRESH_PFN_RESULT)&FrLoginDlg::OnFindDlgResult, 0);
	}
}

void FrLoginDlg::ResetInput(const char* text)
{
	if (m_pView && text)
		m_pView->AddLine(text, 0, 0);
	if (m_pPassword)
	{
		m_pPassword->SetLine(1, "", 0, 0, 0);
		m_pPassword->ClearLine();
	}
	if (m_pId && m_pId->m_nFlags.GetFlag(FWF_KEYFOCUS))
		m_pId->SetKeyFocus(true);
	else if (m_pPassword && m_pPassword->m_nFlags.GetFlag(FWF_KEYFOCUS))
		m_pPassword->SetKeyFocus(true);
	m_connState = (eLoginState)5;
}

bool FrLoginDlg::OnInit()
{
	CProjectG::Instance()->m_bMsnOffline = false;
	SetMessage("\xc5\xd7\xbd\xba\xc6\xae \xb1\xb8\xb9\xae", false);
	return true;
}

void FrLoginDlg::OnHideStaticInit(int param)
{
	FrStatic* wnd = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (CLoginInfo::Instance()->IsWebLogin())
		wnd->SetVisible(false);
}

void FrLoginDlg::OnParanAreaInit(int param)
{
	FrArea* wnd = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (!CLoginInfo::Instance()->IsWebLogin())
		wnd->SetVisible(false);
}

void FrLoginDlg::OnHideAreaInit(int param)
{
	FrArea* wnd = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (CLoginInfo::Instance()->IsWebLogin())
		wnd->SetVisible(false);
}

void FrLoginDlg::OnHideAreaDraw()
{
}

void FrLoginDlg::OnLoginNoticeInit(int param)
{
	FrEdit* wnd = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	wnd->AddText(
		"\xa1\xd8\xc1\xdf\xbf\xe4 : \xc7\xd1\xba\xfb\xbc\xd2\xc7\xc1\xc6\xae \xc6\xce\xbe\xdf \xc8\xb8\xbf\xf8\xc0\xcc\xbd\xc5\xb0\xa1\xbf\xe4?\012\xc6\xce\xbe\xdf \xc8\xa8\xc6\xe4\xc0\xcc\xc1\xf6\xbf\xa1\xbc\xad \xb9\xdd\xb5\xe5\xbd\xc3 \xc8\xb8\xbf\xf8 \xc1\xa4\xba\xb8 \xc0\xcc\xc0\xfc \xbd\xc5\xc3\xbb\xc0\xbb \xc7\xcf\xbd\xc5 \xc8\xc4\xbf\xa1 \xb7\xce\xb1\xd7\xc0\xce\xc7\xcf\xbc\xc5\xbe\xdf \xc1\xa4\xbb\xf3\xc0\xfb\xc0\xce \xb0\xd4\xc0\xd3\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.",
		0, true);
}

void FrLoginDlg::OnHideBtnInit(int param)
{
	FrButton* wnd = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (CLoginInfo::Instance()->IsWebLogin())
		wnd->SetVisible(false);
}
