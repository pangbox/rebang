#include "stdafx.h"
#include "quitwindow.h"

#include "mousecursor.h"
#include "optiondlg.h"
#include "messengerdlg.h"
#include "avatar_task.h"
#include "treasure.h"
#include "projectg.h"
#include "../../shared/localize.h"
extern Fresh* g_pFresh;
extern bool g_bQuit;

class FrQuitForm : public FrForm
{
public:
	DECLARE_OBJECT(FrQuitForm)
	FrQuitForm();

	void EnableOptionBtn(bool bEnable);

	WRect GetOptionBtnRect() { return m_pOptionBtn->GetRect(); }

protected:
	virtual void OnProc(const float delta);
	virtual void OnDraw();

	void OnOptionBtnInit(int param);
	void OnNextBtnInit(int param);
	void OnToLobbyBtnInit(int param);
	void OnExitBtnInit(int param);
	void OnOptionBtnUp();
	void OnNextBtnUp();
	void OnToLobbyBtnUp();
	void OnExitBtnUp();
	void OnCloseBtnUp();
	bool OnToLobbyConfirmResult(int result, FrForm* pForm);
	bool OnExitConfirmResult(int result, FrForm* pForm);

	FrButton* m_pOptionBtn;
	FrButton* m_pNextBtn;
	FrButton* m_pToLobbyBtn;
	FrButton* m_pExitBtn;
	FrButton* m_pHoverBtn;
	float m_time;

	DECLARE_FRESH_MSGMAP()
};

IMPLEMENT_OBJECT(FrQuitForm, FrForm)
FrQuitForm::FrQuitForm()
{
	m_pOptionBtn = NULL;
	m_pNextBtn = NULL;
	m_pToLobbyBtn = NULL;
	m_pExitBtn = NULL;
	m_pHoverBtn = NULL;
	m_time = 0.0f;
	if (GOLFDOC() && GOLFDOC()->m_gameMode == 512 &&
		COption::Instance()->gGetTerrainTooltip())
		CMouseCursor::Instance()->SetTerrainToolTip(false);
}

BEGIN_FRESH_MSGMAP(FrQuitForm, FrForm)
ON_FRESH_VI("option", FRCMD_INIT, FrQuitForm::OnOptionBtnInit)
ON_FRESH_VV("option", FRCMD_LBUTTONUP, FrQuitForm::OnOptionBtnUp)
ON_FRESH_VI("next", FRCMD_INIT, FrQuitForm::OnNextBtnInit)
ON_FRESH_VV("next", FRCMD_LBUTTONUP, FrQuitForm::OnNextBtnUp)
ON_FRESH_VI("tolobby", FRCMD_INIT, FrQuitForm::OnToLobbyBtnInit)
ON_FRESH_VV("tolobby", FRCMD_LBUTTONUP, FrQuitForm::OnToLobbyBtnUp)
ON_FRESH_VI("exit", FRCMD_INIT, FrQuitForm::OnExitBtnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, FrQuitForm::OnExitBtnUp)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrQuitForm::OnCloseBtnUp)
END_FRESH_MSGMAP()

void FrQuitForm::OnCloseBtnUp()
{
	Close(FrNONE, true);
}

void FrQuitForm::EnableOptionBtn(bool enable)
{
	if (m_pOptionBtn)
		m_pOptionBtn->Enable(enable);
}

bool CQuitWindow::IsActive()
{
	return m_pQuitForm ? true : false;
}

void FrQuitForm::OnDraw()
{
	unsigned long color = ((unsigned char)(m_time * 64.0f) << 24) | 0x404040;
	g_pFresh->GetManager()->GetGDI()->Box(g_pFresh->GetDesktop()->GetRect(),
		color, 0, 0.0f);
}

void FrQuitForm::OnExitBtnInit(int param)
{
	m_pExitBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

bool FrQuitForm::OnExitConfirmResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (OnlinePlay())
		{
			WSendPacket packet((enumClientPacket)15);
			packet.Encode1(1);
			packet.Encode2(0xffff);
			packet.Encode8(0);
			packet.Encode8(0);
			packet.Send(TO_GAME);
		}
		Close(FrNONE, true);
		g_bQuit = true;
	}
	return true;
}

void FrQuitForm::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	m_time += delta;
	if (m_time > 2.0f)
		m_time = 2.0f;
	if (m_pOptionBtn && m_pOptionBtn->GetStatus() == FrButton::OVER &&
		m_pHoverBtn != m_pOptionBtn)
	{
		m_pHoverBtn = m_pOptionBtn;
		SetDesc(
			"\xc6\xce\xbe\xdf\x20\xbc\xb3\xc1\xa4\xc0\xbb\x20\xba\xaf\xb0\xe6\xc7\xd5\xb4\xcf\xb4\xd9\x2e");
	}
	if (m_pNextBtn && m_pNextBtn->GetStatus() == FrButton::OVER)
	{
		if (m_pHoverBtn != m_pNextBtn)
		{
			m_pHoverBtn = m_pNextBtn;
			SetDesc(
				"\xb4\xd9\xc0\xbd\x20\xb4\xdc\xb0\xe8\xb7\xce\x20\xc0\xcc\xb5\xbf\xc7\xd5\xb4\xcf\xb4\xd9\x2e");
		}
	}
	else if (m_pToLobbyBtn && m_pToLobbyBtn->GetStatus() == FrButton::OVER)
	{
		if (m_pHoverBtn != m_pToLobbyBtn)
		{
			m_pHoverBtn = m_pToLobbyBtn;
			if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				SetDesc(
					"\xb4\xeb\xc8\xad\xb9\xe6\xc0\xbb\x20\xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9\x2e");
				if (CMessengerInfo::Instance()->GetMyStatus() !=
					(MSN::eStatusType)3)
					CMessengerInfo::Instance()->SetMyStatus(
						(MSN::eStatusType)4);
			}
			else
				SetDesc(
					"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xb7\xce\xba\xf1\xb7\xce\x20\xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9\x2e");
		}
	}
	else if (m_pExitBtn && m_pExitBtn->GetStatus() == FrButton::OVER &&
		m_pHoverBtn != m_pExitBtn)
	{
		m_pHoverBtn = m_pExitBtn;
		SetDesc(
			"\xc6\xce\xbe\xdf\x20\xc7\xc1\xb7\xce\xb1\xd7\xb7\xa5\xc0\xbb\x20\xb3\xa1\xb3\xbb\xb0\xed\x20\xc0\xa9\xb5\xb5\xbf\xec\xb7\xce\x20\xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9\x2e");
	}
}

void FrQuitForm::OnOptionBtnInit(int param)
{
	m_pOptionBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (GOLFDOC()->m_tutorialMode < 15)
		m_pOptionBtn->SetVisible(false);
}

void FrQuitForm::OnNextBtnInit(int param)
{
	m_pNextBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNextBtn &&
		(Doc()->m_golfGame.gameType == 11 || GOLFDOC()->m_tutorialMode >= 8 ||
			CQuitWindow::Instance()->HasOptionWindow()))
		m_pNextBtn->SetVisible(false);
}

void FrQuitForm::OnToLobbyBtnInit(int param)
{
	m_pToLobbyBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pToLobbyBtn)
	{
		if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
			m_pToLobbyBtn->SetButtonImg("i_exit", FrButton::NORMAL);
		else
			m_pToLobbyBtn->SetButtonImg("i53", FrButton::NORMAL);
		if (Doc()->m_offlinePlay)
			m_pToLobbyBtn->Enable(false);
	}
}

void FrQuitForm::OnNextBtnUp()
{
	Close(false);
	IActor* actor = AfxGetTask()->GetActor("Tutorial");
	if (actor)
		actor << MsgObject(NULL, 331, 0, 0, 0, 0, 0);
}

void FrQuitForm::OnExitBtnUp()
{
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		FrForm* dlg;
		if (OnlinePlay())
		{
			if (GalleryMode())
			{
				dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_yesno");
				dlg->SetMessage(
					"\xb0\xb6\xb7\xaf\xb8\xae\xb8\xa6\x20\xb3\xa1\xb3\xbb\xb0\xed\x20\xc6\xce\xbe\xdf\xb8\xa6\x20\xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
					false);
			}
			else if (Doc()->m_golfGame.gameType == 2)
			{
				dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_yesno");
				dlg->SetMessage(
					"\xb4\xeb\xc8\xad\xb9\xe6\xbf\xa1\xbc\xad\x20\xb3\xaa\xb0\xa1\xb0\xed\x20\xc6\xce\xbe\xdf\xb8\xa6\x20\xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
					false);
			}
			else
			{
				dlg =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "tolobby");
				dlg->SetMessage(
					"\xb0\xd4\xc0\xd3\xc0\xcc\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbf\xcf\xc0\xfc\xc8\xf7\x20\xc1\xbe\xb7\xe1\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb5\xc7\xb1\xe2\x20\xc0\xfc\xbf\xa1\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb8\xe9\x2c\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xb0\xe6\xc7\xe8\xc4\xa1\x20\xc8\xb9\xb5\xe6\xb0\xfa\x20\xb0\xa2\xc1\xbe\x20\xb1\xe2\xb7\xcf\xc0\xcc\x20\xb9\xab\xc8\xbf\xc3\xb3\xb8\xae\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb5\xcb\xb4\xcf\xb4\xd9\x2e\x0a\x0a\x20\x09\x09\x09\x09\x09\x09\x09\xb6\xc7\xc7\xd1\x20\xb8\xb9\xc0\xba\x20\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xb7\xce\x20\xc0\xce\xc7\xd8\x20\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc\x20\xb4\xd9\xc0\xbd\xb0\xfa\x20\xb0\xb0\xc0\xba\x20\xbc\xf6\xc4\xa1\xb0\xa1\x20\xb5\xc7\xb8\xe9\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbc\xba\xba\xb0\x20\xc7\xa5\xbd\xc3\x20\xbe\xc6\xc0\xcc\xc4\xdc\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xc0\xcc\x20\xc5\xb9\xc7\xd8\xc1\xfc\xb0\xfa\x20\xb5\xbf\xbd\xc3\xbf\xa1\x20\x09\x09\x09\x09\x09\x09\x09\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbb\xe7\xbf\xeb\x20\xb1\xe2\xb4\xc9\xbf\xa1\x20\xb4\xeb\xc7\xd1\x20\xc1\xa6\xbe\xe0\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xc0\xcc\x20\xbb\xfd\xb1\xe9\xb4\xcf\xb4\xd9\x2e\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xc6\xce\xbe\xdf\xb8\xa6\x20\xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
					false);
			}
		}
		else
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xc6\xce\xbe\xdf\xb8\xa6\x20\xc1\xbe\xb7\xe1\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				false);
		}
		dlg->Open((FRESH_PFN_RESULT)&FrQuitForm::OnExitConfirmResult, FrESCAPE);
	}
	else
		g_bQuit = true;
}

CQuitWindow::CQuitWindow()
{
	m_pQuitForm = NULL;
	m_pOptionDlg = NULL;
	m_bCursorActive = true;
}

CQuitWindow::~CQuitWindow()
{
}

bool CQuitWindow::OnOptionResult(int result, FrForm* form)
{
	m_pOptionDlg = NULL;
	if (m_pQuitForm)
		m_pQuitForm->EnableOptionBtn(true);
	CMouseCursor::Instance()->Enable(CMouseCursor::AREA_AUXMENU_OPTION, true);
	return true;
}

void FrQuitForm::OnOptionBtnUp()
{
	EnableOptionBtn(false);
	if (CQuitWindow::Instance())
		CQuitWindow::Instance()->CreateOptionWindow();
}

bool CQuitWindow::OnQuitFormResult(int result, FrForm* form)
{
	COption::Instance()->SetQuitWindowed(false);
	if (GOLFDOC() && GOLFDOC()->m_gameMode == 512 &&
		COption::Instance()->gGetTerrainTooltip())
		CMouseCursor::Instance()->SetTerrainToolTip(true);
	if (GOLFDOC()->m_tutorialMode < 15)
		CMouseCursor::Instance()->SetActive(m_bCursorActive);
	m_pQuitForm = NULL;
	return true;
}

void CQuitWindow::CreateOptionWindow()
{
	if (m_pQuitForm)
		m_pQuitForm->EnableOptionBtn(false);
	if (!m_pOptionDlg)
	{
		CMouseCursor::Instance()->Enable(CMouseCursor::AREA_AUXMENU_OPTION,
			false);
		m_pOptionDlg =
			CreateForm<FrOptionDlg>(g_pFresh->GetManager(), this, "option");
		m_pOptionDlg->Open((FRESH_PFN_RESULT)&CQuitWindow::OnOptionResult,
			FrESCAPE | FrICON);
		m_pOptionDlg->SetApplyScrSize(true);
		m_pOptionDlg->SetCloseResult(FrCANCEL);
		if (m_pQuitForm)
			m_pOptionDlg->SetIconRect(m_pQuitForm->GetOptionBtnRect());
		else
			m_pOptionDlg->SetIconRect(
				WRect(g_view->GetWidth() - 64.0f, 181.0f, 64.0f, 24.0f));
	}
}

bool FrQuitForm::OnToLobbyConfirmResult(int result, FrForm* form)
{
	if (result == 1)
	{
		if (OnlinePlay())
		{
			if (Doc()->m_golfGame.gameType == 0 &&
				GOLFDOC()->GetCurrentPlayerNum() == 1 && !(GalleryMode()))
			{
				WSendPacket packet((enumClientPacket)55);
				packet.Send(TO_GAME);
				if (CTHunter::Instance())
					CTHunter::Instance()->SetReturnLobby(true);
			}
			else
			{
				Doc()->m_bGameOver = true;
				WSendPacket packet((enumClientPacket)15);
				packet.Encode1(1);
				packet.Encode2(0xffff);
				sRivalData* rival = NULL;
				if (IsLocalContent(S3_CADDIE_REPORT) &&
					(Doc()->m_roomInfo.gameType == 4 ||
						Doc()->m_roomInfo.gameType == 5))
				{
					for (std::vector<sRivalData>::iterator it =
							 Doc()->m_rivalList.begin();
						it != Doc()->m_rivalList.end(); ++it)
						if ((*it).oid == Doc()->m_myInfo.info.dwGuid)
						{
							rival = &(*it);
							break;
						}
				}
				if (rival)
				{
					packet.Encode8(rival->totalPang);
					packet.Encode8(rival->totalBonusPang);
				}
				else
				{
					packet.Encode8(0);
					packet.Encode8(0);
				}
				packet.Send(TO_GAME);
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 26, 0, 0, 0,
					0);
			}
		}
		else
		{
			CProjectG::Instance()->FromGolfToLobbySettings();
			if (Doc()->m_gameMode == 1)
			{
				CTaskManager::Instance()->ChangeTask("CFamilyTask", "", false);
				CTaskManager::Instance()->PostMsg(NULL, "Family", 0,
					(int)"FAMILY", 0, 0, 0);
			}
			else if (Doc()->m_gameMode == 5)
			{
				CTaskManager::Instance()->ChangeTask("CGhostTask", "",
					CTaskManager::Instance()->GetPreservedTask() != NULL);
				CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0,
					(int)"GHOST", 0, 0, 0);
			}
			else if (Doc()->m_golfGame.gameType == 11 ||
				Doc()->m_golfGame.gameType == 12)
			{
				Doc()->m_gameMode = 0;
				CTaskManager::Instance()->ChangeTask("CTutorialTask", "",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
					(int)"TUTORIAL_MAIN", 0, 0, 0);
			}
		}
		Close(FrNONE, true);
	}
	return true;
}

void CQuitWindow::Process()
{
	if (!(GOLFDOC()->m_gameMode & 0x2030) &&
		!g_pFresh->GetManager()->HasEscKeyWindow() && !m_pQuitForm)
	{
		m_pQuitForm =
			CreateForm<FrQuitForm>(g_pFresh->GetManager(), this, "quitdlg");
		m_pQuitForm->Open((FRESH_PFN_RESULT)&CQuitWindow::OnQuitFormResult,
			FrESCAPE);
		m_pQuitForm->MoveCursor("tolobby");
		COption::Instance()->SetQuitWindowed(true);
		if (GOLFDOC()->m_tutorialMode < 15)
		{
			m_bCursorActive = CMouseCursor::Instance()->IsActive();
			CMouseCursor::Instance()->SetActive(true);
		}
	}
}

void FrQuitForm::OnToLobbyBtnUp()
{
	if (!WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		g_bQuit = true;
		return;
	}
	{
		if (Doc()->m_golfGame.gameType == 2)
		{
			OnToLobbyConfirmResult(1, NULL);
			return;
		}
		FrForm* dlg;
		if (Doc()->m_gameMode == 1)
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xb4\xeb\xb1\xe2\xbd\xc7\xb7\xce\x20\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				false);
		}
		else if (Doc()->m_gameMode == 5)
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xb4\xeb\xb1\xe2\xbd\xc7\xb7\xce\x20\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f\x20\x20\x20\x20\x20\x20\x20\x28\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xba\x20\xb1\xe2\xb7\xcf\xb5\xc7\xc1\xf6\x20\xbe\xca\xc1\xf6\xb8\xb8\x2c\x20\xbb\xe7\xbf\xeb\xb5\xc8\x20\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba\x20\xb5\xb9\xb7\xc1\x20\xb9\xde\xc1\xf6\x20\xb8\xf8\xc7\xd5\xb4\xcf\xb4\xd9\x2e\x29",
				false);
		}
		else if (GalleryMode())
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xb6\xb7\xaf\xb8\xae\xb8\xa6\x20\xb3\xa1\xb3\xbb\xb0\xed\x20\xb7\xce\xba\xf1\xb7\xce\x20\xb3\xaa\xb0\xa9\xb4\xcf\xb4\xd9\x2e",
				false);
		}
		else if (Doc()->m_golfGame.gameType == 11 ||
			Doc()->m_golfGame.gameType == 12)
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xb7\xce\xba\xf1\xb7\xce\x20\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f\x20",
				false);
		}
		else if ((Doc()->m_golfGame.gameType == 0 &&
					 GOLFDOC()->GetCurrentPlayerNum() == 1) ||
			Doc()->m_golfGame.gameType == 10)
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify_yesno");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xbb\x20\xb1\xd7\xb8\xb8\xc7\xcf\xb0\xed\x20\xb7\xce\xba\xf1\xb7\xce\x20\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				false);
		}
		else
		{
			dlg = CreateForm<FrForm>(g_pFresh->GetManager(), this, "tolobby");
			dlg->SetMessage(
				"\xb0\xd4\xc0\xd3\xc0\xcc\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbf\xcf\xc0\xfc\xc8\xf7\x20\xc1\xbe\xb7\xe1\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb5\xc7\xb1\xe2\x20\xc0\xfc\xbf\xa1\x20\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb8\xe9\x2c\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xb0\xe6\xc7\xe8\xc4\xa1\x20\xc8\xb9\xb5\xe6\xb0\xfa\x20\xb0\xa2\xc1\xbe\x20\xb1\xe2\xb7\xcf\xc0\xcc\x20\xb9\xab\xc8\xbf\xc3\xb3\xb8\xae\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xb5\xcb\xb4\xcf\xb4\xd9\x2e\x0a\x0a\x20\x09\x09\x09\x09\x09\x09\x09\xb6\xc7\xc7\xd1\x20\xb8\xb9\xc0\xba\x20\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xb7\xce\x20\xc0\xce\xc7\xd8\x20\xb0\xad\xc1\xa6\xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc\x20\xb4\xd9\xc0\xbd\xb0\xfa\x20\xb0\xb0\xc0\xba\x20\xbc\xf6\xc4\xa1\xb0\xa1\x20\xb5\xc7\xb8\xe9\x20\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbc\xba\xba\xb0\x20\xc7\xa5\xbd\xc3\x20\xbe\xc6\xc0\xcc\xc4\xdc\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xc0\xcc\x20\xc5\xb9\xc7\xd8\xc1\xfc\xb0\xfa\x20\xb5\xbf\xbd\xc3\xbf\xa1\x20\x09\x09\x09\x09\x09\x09\x09\x5c\x63\x30\x78\x66\x66\x66\x66\x30\x30\x30\x30\x5c\x63\xbb\xe7\xbf\xeb\x20\xb1\xe2\xb4\xc9\xbf\xa1\x20\xb4\xeb\xc7\xd1\x20\xc1\xa6\xbe\xe0\x5c\x63\x30\x78\x66\x66\x30\x30\x30\x30\x30\x30\x5c\x63\xc0\xcc\x20\xbb\xfd\xb1\xe9\xb4\xcf\xb4\xd9\x2e\x0a\x0a\x0a\x0a\x0a\x0a\x0a\x0a\xb0\xd4\xc0\xd3\xc0\xbb\x20\xc6\xf7\xb1\xe2\xc7\xcf\xb0\xed\x20\xb7\xce\xba\xf1\xb7\xce\x20\xb3\xaa\xb0\xa1\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				false);
		}
		dlg->Open((FRESH_PFN_RESULT)&FrQuitForm::OnToLobbyConfirmResult,
			FrESCAPE);
	}
}
