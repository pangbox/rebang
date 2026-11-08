#include "mathconsts.h"
#include "levelupitemdlg.h"
#include "standinginline.h"
#include "s5/utilities.h"
#include "frgroupbox.h"
#include "skinnedtheme.h"
#include "lobbytask.h"
#include "avatar_task.h"
#include "ghosttask.h"
#include "shoptask.h"
#include "tutorialtask.h"
#include "realmyroomtask.h"
#include "browser.h"
#include <wininet.h>
#include "s5/classicserver/classicserver.h"
#include "tooltip.h"
#include "binstr.h"
#include "itembuffactor.h"
#include "uccmanager.h"
#include "../../shared/s5/sharedutilities.h"
#include "scenemanager.h"
#include "caddieinfodlg.h"
#include "initdelegate.h"
#include "projectg.h"
#include "recycledlg.h"
#include "scratchdlg.h"
#include "bongdarishopdlg.h"
#include "offlinetradedlg.h"
#include "loginrsdlg.h"
#include "serverdlg.h"
#include "logoutdlg.h"
#include "giftalarmdlg.h"
#include "realmyroom_authoritydlg.h"
#include "mousecursor.h"
#include "avatar_avatar.h"
#include "messengerdlg.h"
#include "userinfoform.h"
#include "clientsetting.h"
#include "onelinereqdlg.h"
#include "onelineboard.h"
#include "quitwindow.h"
#include "noticedlg.h"
#include "kbdmanualdlg.h"
#include "giftdlg.h"
#include "buddymanager.h"
#include "cardsystem.h"
#include "carddlg.h"
#include "treasure.h"
#include "playerdlg.h"
#include "tikimagicboxdlg.h"
#include "itemstorage.h"
#include "gatewayactor.h"
#include "logininfo.h"
#include "birthdayeventdlg.h"
#include "optiondlg.h"

IMPLEMENT_ACTOR(CTaskMain, IActor)

BEGIN_FRESH_MSGMAP(CTaskMain, FrCmdTarget)
END_FRESH_MSGMAP()

bool CTaskMain::IsActiveRecycleDlg()
{
	return m_pRecycleDlg ? true : false;
}

bool CTaskMain::IsActiveScratchDlg()
{
	return m_pScratchDlg ? true : false;
}

void CTaskMain::OnPreLoadInit()
{
	for (int i = 0; i < 21; i++)
		(&m_pUnderBarBack)[i] = NULL;

	InitUnderBar();
	InitOverBar();
	InitBongdariShop();
	InitMessenger();
	InitControls();
}

void CTaskMain::OnInit()
{
	for (int i = 0; i < 21; i++)
		(&m_pUnderBarBack)[i] = NULL;

	InitUnderBar();
	InitOverBar();
	InitBongdariShop();
	InitMessenger();
	InitControls();

	m_pRMREnterSetting = NULL;

	for (int i = 0; i < 5; i++)
		m_pMissionDlg[i] = NULL;
}

void CTaskMain::OnLoad()
{
	CalcUIOffset(true);

	static char* s_titleFontFiles[] = { "[font_wind.jpg" };

	WTITLEFONT info;
	memset(&info, 0, sizeof(info));
	info.filename = s_titleFontFiles;
	info.fontw = 16;
	info.fonth = 16;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:,";
	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_pTitleFont->Create(&info);
}

void CTaskMain::OnProcess(float deltaTime)
{
	ProcessOverBar();

	if (g_input->GetDown("FUNC_12", true))
	{
		if (m_pKbdManualDlg)
			m_pKbdManualDlg->Close(true);
		else
		{
			m_pKbdManualDlg = CreateForm<FrKbdManualDlg>(g_pFresh->GetManager(),
				this, "kbd_manual");
			if (m_pKbdManualDlg)
			{
				m_pKbdManualDlg->Open(
					(FRESH_PFN_RESULT)&CTaskMain::OnKbdManualDlgResult, 1);
				m_pKbdManualDlg->EnableDrag(false);
			}
		}
	}

	if (g_input->GetDown("ESCAPE", true) &&
		!g_pFresh->GetManager()->HasEscKeyWindow())
	{
		if (!IS_EXACTKINDOF(CGolfTask, AfxGetTask()) &&
			!IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			OnUnderBar_BackUp();
	}

	if (m_fKbdManualTime > -0.1f)
	{
		m_fKbdManualTime += deltaTime;

		if (m_fKbdManualTime >
			(IS_KINDOF(CLobbyTask, AfxGetTask()) ? 1.0f : 0.5f))
		{
			if (m_pServerDlg == NULL)
			{
				if (m_pWaitDlg == NULL)
				{
					m_pWaitDlg = CreateForm<FrForm>(g_pFresh->GetManager(),
						this, "wait");
					m_pWaitDlg->SetMessage(
						"\xbc\xad\xb9\xf6\xc0\xc7 \xc0\xc0\xb4\xe4\xc0\xbb \xb1\xe2\xb4\xd9\xb8\xae\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
						false);
					m_pWaitDlg->Open(
						(FRESH_PFN_RESULT)&CTaskMain::OnWaitDlgResult, 0);

					FrButton* pCancel = DYNAMIC_CAST(FrButton,
						m_pWaitDlg->FindChildByName("cancel"));
					if (pCancel)
						pCancel->Enable(false);
				}
				else if (m_fKbdManualTime > 15.0f)
				{
					FrButton* pCancel = DYNAMIC_CAST(FrButton,
						m_pWaitDlg->FindChildByName("cancel"));
					if (pCancel)
						pCancel->Enable(true);

					m_fKbdManualTime = -10.0f;
				}
			}
			else
				m_fKbdManualTime = -10.0f;
		}
	}
}

void CTaskMain::OnDestroy()
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void CTaskMain::OnPreserveBack(const char* layout)
{
	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}
}

void CTaskMain::OnRestorePreserved()
{
	CTaskMain::OnLoad();
	CTaskMain::OnInit();
}

void CTaskMain::CalcUIOffset(bool bDefault)
{
	if (bDefault)
	{
		m_screenWidth = 800.0f;
		m_screenHeight = 600.0f;
	}
	else
	{
		m_screenWidth = g_view->GetWidth();
		m_screenHeight = g_view->GetHeight();
	}

	m_offsetX = (m_screenWidth - 800.0f) * 0.5f;
	m_offsetY = m_screenHeight - 600.0f;
}

void CTaskMain::SetCenterOffset(FrWnd* pWnd, float x, float y)
{
	WRect rect = pWnd->GetRect();

	rect.x += x;
	rect.y += y;

	pWnd->SetRect(rect);
}

void CTaskMain::SetLeftOffset(FrWnd* pWnd, float x)
{
	WRect rect = pWnd->GetRect();

	rect.x = x - (800.0f - rect.x);

	pWnd->SetRect(rect);
}

void CTaskMain::InitMessenger()
{
	m_pMessengerBubbleEdit = NULL;
	memset(m_pMessengerBubbleBmp, 0, sizeof(m_pMessengerBubbleBmp));
	m_pMessengerAlarmBtn = NULL;
	m_pMessengerGiftAlarmBtn = NULL;

	m_pTreasureAlarm = NULL;

	m_pMessengerBtn = NULL;
	m_pMessengerBubble = NULL;
}

void CTaskMain::InitBongdariShop()
{
	m_bBongdariShop0 = false;
	m_bBongdariShop1 = false;
	m_bBongdariShop2 = false;
}

void CTaskMain::InitControls()
{
	memset(&m_pOverBar, 0, sizeof(FrGroupBox*) * 3);
	m_pGiftDlg = NULL;
	m_pOptionDlg = NULL;
	m_pKbdManualDlg = NULL;
	m_pServerDlg = NULL;
	m_pLoginRsDlg = NULL;
	m_pRecycleDlg = NULL;
	m_pTikiMagicBoxDlg = NULL;
	m_pScratchDlg = NULL;
	m_pOnelineReqDlg = NULL;
	m_pWaitDlg = NULL;
	m_pNoticeDlg = NULL;
	m_pDlg150 = NULL;
	m_pQuitDlg = NULL;
	m_pSpecialCardDlg = NULL;
	m_pSubEquipBar = NULL;

	m_pMaketDlg = NULL;

	m_fKbdManualTime = -10.0f;
}

void CTaskMain::InitUnderBar()
{
	m_pUnderBarTabBtn = NULL;
	m_pOverBarMascot = NULL;
	m_pUnderBarTabToolTip = NULL;
	m_bUnderTabOn = false;

	m_pMaketDlg = NULL;
}

void CTaskMain::OnUnderBarInit(int param)
{
	m_pUnderBar = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
	if (m_pUnderBar)
		SetCenterOffset(m_pUnderBar, m_offsetX, m_offsetY);
}

void CTaskMain::OnUnderTab_Init(int param)
{
	m_pUnderTab = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
	if (m_pUnderTab)
	{
		m_pUnderTab->SetVisible(false);
		SetCenterOffset(m_pUnderTab, m_offsetX, m_offsetY);
	}
}

void CTaskMain::OnOverBar_Init(int param)
{
	m_pOverBar = DYNAMIC_CAST(FrGroupBox, (FrWnd*)param);
	if (m_pOverBar)
		S5::THEME::ChangeOverbarTheme(S5::THEME::GetCurrentSkinnedTheme(),
			m_pOverBar);
}

void CTaskMain::OnUnderBar_BackInit(int param)
{
	m_pUnderBarBack = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarBack)
	{
		m_pUnderBarBack->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarBack->EnableHover(true, 0.0f);
	}
}

void CTaskMain::OnUnderBar_BackUp()
{
	Doc()->SetRmrMode(2);

	BOOL bExitRoom = FALSE;

	if (g_pFresh->IsCurrentLayout("FAMILY"))
	{
		Doc()->m_gameMode = 0;
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0,
			0, 0);
	}
	else if (g_pFresh->IsCurrentLayout("TOPPAGE") ||
		g_pFresh->IsCurrentLayout("SERVERLIST"))
	{
		m_pQuitDlg =
			CreateForm<FrLogoutDlg>(g_pFresh->GetManager(), this, "quit");
		m_pQuitDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnQuitDlgResult, 1);
	}
	else if (g_pFresh->IsCurrentLayout("ROOMLIST"))
	{
		SendRoomListExitPacket();
		m_waitedLayout = "TOPPAGE";
	}
	else if (g_pFresh->IsCurrentLayout("GAMEROOM") ||
		g_pFresh->IsCurrentLayout("GAMEROOM_EXT"))
	{
		SendRoomExitPacket();
		m_waitedLayout = "ROOMLIST";
	}
	else if (g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"))
	{
		SendMsg(NULL, "Lobby", 0x1f7, 0, 0, 0, 0);
	}
	else if (g_pFresh->IsCurrentLayout("SHOPMAIN"))
	{
		if (IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("ntAvatarChatTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "AvatarChat", 0,
				(int)"AVATARCHAT_MAIN", 0, 0, 0);
		}
		else if (IS_EXACTKINDOF(CGhostTask,
					 CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("CGhostTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
			CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0,
				0, 0);
			bExitRoom = TRUE;
		}
		else if (Doc()->m_bRebuildUnderBar)
		{
			Doc()->m_bRebuildUnderBar = false;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
		else
		{
			if (Doc()->m_bReturnLobbyNotice)
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				bExitRoom = TRUE;
			}
			else
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"TOPPAGE", 0, 0, 0);
			}

			TurnOn3DBackground();
		}
	}
	else if (g_pFresh->IsCurrentLayout("RealMyRoom"))
	{
		if (IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("ntAvatarChatTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "AvatarChat", 0,
				(int)"AVATARCHAT_MAIN", 0, 0, 0);
		}
		else if (IS_EXACTKINDOF(CGhostTask,
					 CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("CGhostTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
			CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0,
				0, 0);
			bExitRoom = TRUE;
		}
		else if (Doc()->m_bRebuildUnderBar)
		{
			Doc()->m_bRebuildUnderBar = false;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
		else
		{
			if (Doc()->m_bReturnLobbyNotice)
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				bExitRoom = TRUE;
			}
			else
			{
				if (IsLocalContent(S4_ITEM_STORAGE) == true)
				{
					CItemStorage* pStorage = CItemStorage::Instance();
					if (pStorage)
					{
						if (!pStorage->IsVisible())
						{
							CTaskManager::Instance()->ChangeTask("CLobbyTask",
								"CGolfDoc", false);
							CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
								(int)"TOPPAGE", 0, 0, 0);
						}
					}
				}
				else
				{
					CTaskManager::Instance()->ChangeTask("CLobbyTask",
						"CGolfDoc", false);
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
						(int)"TOPPAGE", 0, 0, 0);
				}
			}

			TurnOn3DBackground();
		}
	}
	else if (g_pFresh->IsCurrentLayout("AVATARCHAT_MAIN"))
	{
		if (Doc()->m_bReturnLobbyNotice)
		{
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
			SendRoomExitPacket();
			m_waitedLayout = "ROOMLIST";
			bExitRoom = TRUE;
		}
		else
		{
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"TOPPAGE",
				0, 0, 0);
		}
	}
	else if (g_pFresh->IsCurrentLayout("TUTORIAL_MAIN"))
	{
		Doc()->m_gameMode = 0;
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0,
			0, 0);
	}
	else if (g_pFresh->IsCurrentLayout("UCCROOM"))
	{
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
		CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0,
			0, 0);
	}
	else if (g_pFresh->IsCurrentLayout("REALMYROOM"))
	{
		if (IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("ntAvatarChatTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "AvatarChat", 0,
				(int)"AVATARCHAT_MAIN", 0, 0, 0);
		}
		else if (IS_EXACTKINDOF(CGhostTask,
					 CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("CGhostTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
			CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0,
				0, 0);
			bExitRoom = TRUE;
		}
		else if (Doc()->m_bRebuildUnderBar)
		{
			Doc()->m_bRebuildUnderBar = false;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
		else
		{
			if (Doc()->m_bReturnLobbyNotice)
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				bExitRoom = TRUE;
			}
			else
			{
				if (IsLocalContent(S4_ITEM_STORAGE) == true)
				{
					CItemStorage* pStorage = CItemStorage::Instance();
					if (pStorage)
					{
						if (!pStorage->IsVisible())
						{
							CTaskManager::Instance()->ChangeTask("CLobbyTask",
								"CGolfDoc", false);
							CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
								(int)"TOPPAGE", 0, 0, 0);
						}
					}
				}
				else
				{
					CTaskManager::Instance()->ChangeTask("CLobbyTask",
						"CGolfDoc", false);
					CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
						(int)"TOPPAGE", 0, 0, 0);
				}
			}

			TurnOn3DBackground();
		}
	}

	if (g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
	{
		if (IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("ntAvatarChatTask", "CGolfDoc",
				false);
			CTaskManager::Instance()->PostMsg(NULL, "AvatarChat", 0,
				(int)"AVATARCHAT_MAIN", 0, 0, 0);
		}
		else if (IS_EXACTKINDOF(CGhostTask,
					 CTaskManager::Instance()->GetPreservedTask()))
		{
			CTaskManager::Instance()->ChangeTask("CGhostTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
			CTaskManager::Instance()->PostMsg(NULL, "Ghost", 0, (int)"GHOST", 0,
				0, 0);
			bExitRoom = TRUE;
		}
		else if (Doc()->m_bRebuildUnderBar)
		{
			Doc()->m_bRebuildUnderBar = false;
			CTaskManager::Instance()->ChangeTask("CTutorialTask", "", false);
			CTaskManager::Instance()->PostMsg(NULL, "TutorialMain", 0,
				(int)"TUTORIAL_MAIN", 0, 0, 0);
		}
		else
		{
			if (Doc()->m_bReturnLobbyNotice)
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"ROOMLIST", 0, 0, 0);
				bExitRoom = TRUE;
			}
			else
			{
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
				CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0,
					(int)"TOPPAGE", 0, 0, 0);
			}

			TurnOn3DBackground();
		}
	}

	if (IsLocalContent(S4_MATCHING_SYSTEM) && bExitRoom)
	{
		WSendPacket send((enumClientPacket)0xb9);
		send.Encode1(1);
		send.Send(TO_GAME);
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_FrontInit(int param)
{
	m_pUnderBarFront = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarFront)
	{
		m_pUnderBarFront->EnableHover(true, 0.0f);
		m_pUnderBarFront->Enable(false);
	}
}

void CTaskMain::OnUnderBar_FrontUp()
{
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_ShopInit(int param)
{
	m_pUnderBarShop = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarShop)
	{
		if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
			m_pUnderBarShop->Enable(false);
		m_pUnderBarShop->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarShop->EnableHover(true, 0.0f);
		if (IsLocalContent((localContentType_t)0x56) &&
			g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
			m_pUnderBarShop->Enable(false);
	}
}

void CTaskMain::OnUnderBar_ShopUp()
{
	if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
	{
	}
	else if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CShopTask", "", true);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
		ntAvatarManager::Instance()->SetActive(false);
	}
	else if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
	{
		CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
	}
	else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
	{
		Doc()->m_bRebuildUnderBar = true;
		CTaskManager::Instance()->ChangeTask("CShopTask", "", false);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
	}
	else if (g_pFresh->IsCurrentLayout("GHOST"))
	{
		CTaskManager::Instance()->ChangeTask("CShopTask", "", true);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
	}
	else if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
	{
		int page;
		int item;
		CProjectG::Instance()->FromGolfToLobbySettings();
		SendMsg(NULL, "RealMyRoom", 0xc3, (int)&page, (int)&item, 0, 0);
		if (page >= 8)
			page = 0;
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0xc4, page, item, 0, 0);
		CTaskManager::Instance()->ChangeTask("CShopTask", "",
			CTaskManager::Instance()->GetPreservedTask() != NULL);
		CTaskManager::Instance()->PostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0,
			0, 0);
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_MyRoomInit(int param)
{
	m_pUnderBarMyRoom = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarMyRoom)
	{
		if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
			m_pUnderBarMyRoom->Enable(false);
		m_pUnderBarMyRoom->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarMyRoom->EnableHover(true, 0.0f);
		if (IsLocalContent((localContentType_t)0x56) &&
			g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
			m_pUnderBarMyRoom->Enable(false);
	}
}

void CTaskMain::OnUnderBar_MyRoomUp()
{
	if (IsLocalContent((localContentType_t)0x56) &&
		IsLocalContent((localContentType_t)0x61))
	{
		Doc()->SetRmrMode(1);
		Doc()->m_replayTargetUID = MyUID();
		MakeRealMyRoom();
		OnBarHoverOff(0);
		return;
	}

	if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc6, 1, 0, 0, 0);
	else if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", true);
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0,
			(int)"RealMyRoom", 0, 0, 0);
		ntAvatarManager::Instance()->SetActive(false);
	}
	else
	{
		if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", false);
		else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
		{
			Doc()->m_bRebuildUnderBar = true;
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", false);
		}
		else if (g_pFresh->IsCurrentLayout("GHOST"))
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", true);
		else
		{
			if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
			{
				int page;
				int item;
				SendMsg(NULL, "Shop", 0xc3, (int)&page, (int)&item, 0, 0);
				CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc4,
					page, item, 0, 0);
			}
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
		}
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0,
			(int)"RealMyRoom", 0, 0, 0);
	}
	OnBarHoverOff(0);

	if (IsLocalContent((localContentType_t)0x59))
	{
		WSendPacket send((enumClientPacket)0xb9);
		send.Encode1(2);
		send.Send(TO_GAME);
	}
}

void CTaskMain::OnUnderBar_ItemStorageInit(int param)
{
	m_pUnderBarItemStorage = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarItemStorage)
	{
		m_pUnderBarItemStorage->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarItemStorage->EnableHover(true, 0.0f);
		m_bGiftUp = false;
		if (IsLocalContent((localContentType_t)0x56) &&
			g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
			m_pUnderBarItemStorage->Enable(false);
	}
}

void CTaskMain::OnUnderBar_ItemStorageUp()
{
	if (IsLocalContent((localContentType_t)0x71))
	{
		Doc()->SetRmrMode(4);
		Doc()->m_replayTargetUID = MyUID();
		MakeRealMyRoom();
		OnBarHoverOff(0);
	}
}

void CTaskMain::OnUnderBar_GiftInit(int param)
{
	m_pUnderBarGift = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarGift)
	{
		m_pUnderBarGift->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarGift->EnableHover(true, 0.0f);
		m_bGiftUp = false;
		if (IsLocalContent((localContentType_t)0x56) &&
			g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
			m_pUnderBarGift->Enable(false);
	}
}

void CTaskMain::OnUnderBar_GiftUp()
{
	if (IsLocalContent((localContentType_t)0x56) &&
		IsLocalContent((localContentType_t)0x61))
	{
		if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		{
			m_bGiftUp = true;
			return;
		}
		Doc()->SetRmrMode(3);
		Doc()->m_replayTargetUID = MyUID();
		MakeRealMyRoom();
		OnBarHoverOff(0);
	}
	else if (IsLocalContent((localContentType_t)0x5b))
	{
		WSendPacket send((enumClientPacket)0x92);
		send.Encode4(0);
		send.Send(TO_GAME);
	}
	else if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
	{
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
	}
	else if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		CProjectG::Instance()->FromGolfToLobbySettings();
		CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", true);
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0,
			(int)"RealMyRoom", 0, 0, 0);
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
		ntAvatarManager::Instance()->SetActive(false);
	}
	else
	{
		if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
		{
			Doc()->m_bRebuildUnderBar = true;
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "", false);
		}
		else
			CTaskManager::Instance()->ChangeTask("CRealMyRoomTask", "",
				CTaskManager::Instance()->GetPreservedTask() != NULL);
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0,
			(int)"RealMyRoom", 0, 0, 0);
		CTaskManager::Instance()->PostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_CookieInit(int param)
{
	m_pUnderBarCookie = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarCookie)
	{
		m_pUnderBarCookie->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarCookie->EnableHover(true, 0.0f);
	}
}

void CTaskMain::OnUnderBar_CookieUp()
{
	if (Doc()->m_needParentAgree)
	{
		this << MsgObject(NULL, 0x23,
			(int)"14\xbc\xbc \xb9\xcc\xb8\xb8\xc0\xce \xba\xd0\xc0\xba\n"
				 "\xc8\xb8\xbf\xf8\xb0\xa1\xc0\xd4(1) \xb9\xd7 "
				 "\xb0\xe1\xc1\xa6(2)\xbf\xa1 \xb4\xeb\xc7\xd1\n"
				 "\xba\xce\xb8\xf0\xb4\xd4\xc0\xc7 \xbd\xc2\xc0\xce\xc0\xcc "
				 "\xc0\xd6\xbe\xee\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
	}
	else
	{
		COption::Instance()->vApplyLobbyScreenSize();
		CBrowser::Instance()->InitializePostArgument();
		switch (Doc()->m_provType)
		{
		case 2:
		{
			CBrowser::Instance()->AddPostArgument("sn", "9");
			CLoginInfo* login = CLoginInfo::Instance();
			CBrowser* browser = CBrowser::Instance();
			browser->AddPostArgument("mn", MakeStr("%d", login->AuthUid()));
			CBrowser::Instance()->OpenPost(
				"https://qa.www.gametree.co.kr/InGame/Gateway.aspx",
				IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? true : false,
				true);
			break;
		}
		case 4:
		{
			NhnLogin* login =
				(NhnLogin*)CLoginInfo::Instance()->QueryInterface();
			BOOL ok = InternetSetCookieA("https://alpha-bill.hangame.com/", 0,
				login->GetSiteCookie());
			if (!ok)
			{
				GetLastError();
				MessageBoxA(0, "Cookie Setting Fail!", "Error", 0);
				return;
			}
			CBrowser::Instance()->AddPostArgument("CHNL", "HG");
			CBrowser::Instance()->OpenPost(
				"https://alpha-bill.hangame.com/pay/charge.nhn",
				IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? true : false,
				true);
			break;
		}
		default:
			return;
		}
	}

	if (IsLocalContent((localContentType_t)0x59))
	{
		WSendPacket send((enumClientPacket)0xb9);
		send.Encode1(2);
		send.Send(TO_GAME);
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_GameInit(int param)
{
	m_pUnderBarGame = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarGame)
	{
		m_pUnderBarGame->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarGame->EnableHover(true, 0.0f);

		m_pUnderBarGame->Enable(true);

		if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ||
			IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()) ||
			IS_EXACTKINDOF(CGhostTask, AfxGetTask()) ||
			IS_EXACTKINDOF(CGhostTask,
				CTaskManager::Instance()->GetPreservedTask()))
			m_pUnderBarGame->Enable(false);
	}
}

void CTaskMain::OnUnderBar_GameUp()
{
	if (!IS_KINDOF(ntAvatarChatTask, AfxGetTask()))
	{
		if (!Doc()->m_bReturnLobbyNotice)
		{
			WSendPacket send((enumClientPacket)0x81);
			send.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		else
		{
			if (!IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
				CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
					false);
			CTaskManager::Instance()->PostMsg(NULL, "Lobby", 0, (int)"ROOMLIST",
				0, 0, 0);
		}
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_ServerInit(int param)
{
	m_pUnderBarServer = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarServer)
	{
		m_pUnderBarServer->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarServer->EnableHover(true, 0.0f);
		if (IS_KINDOF(ntAvatarChatTask, AfxGetTask()) ||
			IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
			m_pUnderBarServer->Enable(false);
		else
			m_pUnderBarServer->Enable(true);
	}
}

void CTaskMain::OnUnderBar_ServerUp()
{
	if (!IS_KINDOF(ntAvatarChatTask, AfxGetTask()) &&
		!IS_EXACTKINDOF(ntAvatarChatTask,
			CTaskManager::Instance()->GetPreservedTask()))
	{
		IActor* actor = AfxGetTask()->GetActor("RealMyRoom");
		if (actor)
			actor << MsgObject(this, 0x68, 0, 0, 0, 0, 0);

		CloseAllDialogs();

		WSendPacket send((enumClientPacket)0x43);
		send.Send(TO_GAME);

		m_pServerDlg =
			CreateForm<FrServerDlg>(g_pFresh->GetManager(), this, "server");
		m_pServerDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnServerDlgResult,
			WPoint(m_pServerDlg->GetRect().x, 50.0f), 17);
		m_pServerDlg->SetIconRect(m_pUnderBarServer->GetRect());
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_ExitInit(int param)
{
	m_pUnderBarExit = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarExit)
	{
		m_pUnderBarExit->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarExit->EnableHover(true, 0.0f);
	}
}

void CTaskMain::OnUnderBar_ExitUp()
{
	CloseAllDialogs();

	if (g_pFresh->IsCurrentLayout("AVATARCHAT_MAIN"))
	{
		if (CQuitWindow::IsInstantiated())
			CQuitWindow::Instance()->Process();
	}
	else
	{
		m_pQuitDlg =
			CreateForm<FrLogoutDlg>(g_pFresh->GetManager(), this, "quit");
		m_pQuitDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnQuitDlgResult, 17);

		if (m_pUnderBarExit)
			m_pQuitDlg->SetIconRect(m_pUnderBarExit->GetRect());

		if (g_pFresh->IsCurrentLayout("GAMEROOM") ||
			g_pFresh->IsCurrentLayout("GAMEROOM_EXT"))
			m_pQuitDlg->DisableServerSelect();

		if (!g_pFresh->IsCurrentLayout("TOPPAGE"))
			m_pQuitDlg->MoveCursor("server");
		else
			m_pQuitDlg->MoveCursor("logout");
	}
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_TabToolTipInit(int param)
{
	m_pUnderBarTabToolTip = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pUnderBarTabToolTip)
		m_pUnderBarTabToolTip->SetVisible(
			COption::Instance()->gGetUnderExtBtn() == 0);
}

void CTaskMain::OnUnderBar_TabBtnInit(int param)
{
	m_pUnderBarTabBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarTabBtn)
	{
		m_pUnderBarTabBtn->SetPushDelay(0.0f);
		if (COption::Instance()->gGetUnderExtBtn() == 1)
			m_pUnderBarTabBtn->EnableBlink(false);
	}
}

void CTaskMain::OnUnderBar_TabBtnUp()
{
	if (m_bUnderTabOn)
		TurnOffUnderTabBar();
	else
		TurnOnUnderTabBar();

	if (m_pUnderBarTabToolTip && m_pUnderBarTabToolTip->IsVisible())
	{
		COption::Instance()->gSetUnderExtBtn(1);
		m_pUnderBarTabToolTip->SetVisible(false);
		m_pUnderBarTabBtn->EnableBlink(false);
	}
	OnBarHoverOff(0);
}

void CTaskMain::TurnOnUnderTabBar()
{
	if (m_bUnderTabOn)
	{
		return;
	}

	m_bUnderTabOn = true;
	m_pUnderTab->SetVisible(true);
	m_pUnderBarMagicBox->Enable(IsLocalContent(S3_RECYCLE));

	m_pUnderBarScratch->Enable(IsLocalContent(S3_SCRATCH));

	m_pUnderBarRanking->Enable(!IsLocalCountry(0x10));

	WRect rect = m_pUnderBarTabBtn->GetRect();
	rect.y -= 53.0f;

	m_pUnderBarTabBtn->SetRect(rect);

	std::string name;
	m_pUnderBarTabBtn->GetWindowName(name);
	if (stricmp(name.c_str(), "tabbtn_b") == 0)
	{
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_bd_n", FrButton::NORMAL);
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_bd_o", FrButton::OVER);
	}
	else
	{
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_d_n", FrButton::NORMAL);
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_d_o", FrButton::OVER);
	}
}

void CTaskMain::TurnOffUnderTabBar()
{
	if (!m_bUnderTabOn)
	{
		return;
	}

	m_bUnderTabOn = false;
	m_pUnderTab->SetVisible(false);

	WRect rect = m_pUnderBarTabBtn->GetRect();
	rect.y += 53.0f;

	m_pUnderBarTabBtn->SetRect(rect);

	std::string name;
	m_pUnderBarTabBtn->GetWindowName(name);
	if (stricmp(name.c_str(), "tabbtn_b") == 0)
	{
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_bu_n", FrButton::NORMAL);
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_bu_o", FrButton::OVER);
	}
	else
	{
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_u_n", FrButton::NORMAL);
		m_pUnderBarTabBtn->SetButtonImg("underbar_tab_u_o", FrButton::OVER);
	}
}

void CTaskMain::OnUnderBar_MagicBoxInit(int param)
{
	m_pUnderBarMagicBox = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarMagicBox)
	{
		m_pUnderBarMagicBox->EnableHover(true, 0.0f);

		WRect rect = m_pUnderBarMagicBox->GetRect();
		rect.x += 74.0f;
		m_pUnderBarMagicBox->SetRect(rect);
	}
}

void CTaskMain::OnUnderBar_MagicBoxUp()
{
	if (Doc()->IsControlServerService(0x8000))
	{
		this << MsgObject(this, 0x23,
			(int)"\xb8\xb6\xb9\xfd\xbb\xf3\xc0\xda \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (Doc()->GetFlagBlock() & 0x40)
	{
		this << MsgObject(this, 0x23,
			(int)"\\c0xffff0000\\c \xc4\xab\xb5\xe5\xc1\xa4\xba\xb8 \xc0\xcc\xbb\xf3\xc0\xb8\xb7\xce \xb8\xb6\xb9\xfd\xbb\xf3\xc0\xda\\c0xff000000\\c\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (IsLocalContent(S3_RECYCLE))
	{
		if (m_pRecycleDlg == NULL)
		{
			m_pRecycleDlg = CreateForm<FrRecycleDlg>(g_pFresh->GetManager(),
				this, "recycledlg", NULL);
			m_pRecycleDlg->Open(
				(FRESH_PFN_RESULT)&CTaskMain::OnRecycleDlgResult, 0xc1);

			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
				ntAvatarManager::Instance()->SetActive(false);
		}
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_TikiMagicBoxInit(int param)
{
}

void CTaskMain::OnUnderBar_TikiMagicBoxUp()
{
	if (IsLocalContent((localContentType_t)0x79) == true)
	{
		if (m_pTikiMagicBoxDlg == NULL)
		{
			m_pTikiMagicBoxDlg = CreateForm<FrTikiMagicBoxDlg>(
				g_pFresh->GetManager(), this, "tikimagicbox", NULL);
			m_pTikiMagicBoxDlg->Open(
				(FRESH_PFN_RESULT)&CTaskMain::OnTikiMagicBoxDlgResult, 0xc1);

			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
				ntAvatarManager::Instance()->SetActive(false);
		}
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_ScratchInit(int param)
{
	m_pUnderBarScratch = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarScratch)
	{
		m_pUnderBarScratch->EnableHover(true, 0.0f);

		WRect rect = m_pUnderBarScratch->GetRect();
		rect.x += 74.0f;
		m_pUnderBarScratch->SetRect(rect);
	}
}

void CTaskMain::OnUnderBar_ScratchUp()
{
	if (Doc()->IsControlServerService(0x4000))
	{
		this << MsgObject(this, 0x23,
			(int)"\xbd\xba\xc5\xa9\xb7\xa1\xc4\xa1 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (IsLocalContent(S3_SCRATCH))
	{
		if (Doc()->m_specialShotMark != 2)
		{
			if (m_pScratchDlg == NULL)
			{
				m_pScratchDlg = CreateForm<FrScratchDlg>(g_pFresh->GetManager(),
					this, "scratchdlg", NULL);
				m_pScratchDlg->Open(
					(FRESH_PFN_RESULT)&CTaskMain::OnScratchDlgResult, 0xc1);

				if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
					ntAvatarManager::Instance()->SetActive(false);
			}
		}
		else
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			WRect rect = pForm->GetRect();
			pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
			pForm->SetTimeLimit(12.0f);
			pForm->SetMessage(
				"\xc7\xf6\xc0\xe7 \xc7\xd1 \xc1\xbe\xb7\xf9\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb \xba\xb8\xc0\xaf \xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xc7\xd8\xb4\xe7 \xc4\xc1\xc5\xd9\xc3\xf7\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 0);
		}
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_BongdariShopInit(int param)
{
	m_pUnderBarBongdariShop = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarBongdariShop)
	{
		m_pUnderBarBongdariShop->SetPushSound("ui_desktop_icon_click");
		m_pUnderBarBongdariShop->EnableHover(true, 0.0f);

		WRect rect = m_pUnderBarBongdariShop->GetRect();
		rect.x += 74.0f;
		m_pUnderBarBongdariShop->SetRect(rect);
	}
}

void CTaskMain::OnUnderBar_BongdariShopUp()
{
	if (Doc()->IsControlServerService(0x10))
	{
		this << MsgObject(this, 0x23,
			(int)"\xba\xc0\xb4\xd9\xb8\xae\xbc\xa5 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (Doc()->m_specialShotMark != 2)
	{
		if (!m_bBongdariShop1)
		{
			FrWnd* pWnd =
				g_pFresh->GetDesktop()->FindChildByName("bongdari_shop");
			if (pWnd || DYNAMIC_CAST(FrWnd, pWnd))
			{
				pWnd->Close(true);
			}
			else
			{
				WSendPacket packet((enumClientPacket)0x95);
				packet.Send(TO_GAME);

				Doc()->m_pendingMenu = 1;
			}
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		WRect rect = pForm->GetRect();
		pForm->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
		pForm->SetTimeLimit(12.0f);
		pForm->SetMessage(
			"\xc7\xf6\xc0\xe7 \xc7\xd1 \xc1\xbe\xb7\xf9\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb \xba\xb8\xc0\xaf \xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xc7\xd8\xb4\xe7 \xc4\xc1\xc5\xd9\xc3\xf7\xb8\xa6 \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 0);
	}

	m_bBongdariShop0 = false;
	m_bBongdariShop1 = false;
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_MaketInit(int param)
{
	m_pUnderBarMaket = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarMaket)
	{
		m_pUnderBarMaket->EnableHover(true, 0.0f);

		m_pUnderBarMaket->SetVisible(false);
	}
}

void CTaskMain::OnUnderBar_MaketUp()
{
	if (!IsLocalContent(S4_OFFLINE_SHOP))
	{
		this << MsgObject(this, 0x23,
			(int)"\xc6\xce\xbe\xdf\xb8\xb6\xc4\xcf \xb9\xd7 \xc1\xf8\xbf\xad\xb4\xeb \xb1\xe2\xb4\xc9\xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (Doc()->IsControlServerService(0x80000))
	{
		this << MsgObject(this, 0x23,
			(int)"\xc6\xce\xbe\xdf\xb8\xb6\xc4\xcf \xb9\xd7 \xc1\xf8\xbf\xad\xb4\xeb \xb1\xe2\xb4\xc9\xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	if (m_pUnderBarMaket == NULL)
		return;

	CloseAllDialogs();

	if (IsLocalContent(S4_OFFLINE_SHOP))
	{
		WSendPacket packet((enumClientPacket)0xca);
		packet.Encode2(1);
		packet.Send(TO_GAME);
	}

	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));

	OnBarHoverOff(0);
}

void CTaskMain::CreateMaketDlg()
{
	if (m_pMaketDlg == NULL)
	{
		m_pMaketDlg = CreateForm<COfflineTradeDlg>(g_pFresh->GetManager(), this,
			"offlinetraderanking", NULL);
		m_pMaketDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnMaketListDlgResult,
			0x41);
	}
}

void CTaskMain::OnUnderBar_RankingInit(int param)
{
	m_pUnderBarRanking = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarRanking)
	{
		m_pUnderBarRanking->EnableHover(true, 0.0f);

		WRect rect = m_pUnderBarRanking->GetRect();
		rect.x += 74.0f;
		m_pUnderBarRanking->SetRect(rect);
	}
}

void CTaskMain::OnUnderBar_RankingUp()
{
	if (Doc()->IsControlServerService(0x10000))
	{
		this << MsgObject(this, 0x23,
			(int)"\xb7\xa9\xc5\xb7 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	CloseAllDialogs();

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		WSendPacket packet((enumClientPacket)0x47);
		packet.Send(TO_GAME);
	}
	else
	{
		WNetworkSystem::Instance()->Init(WNetworkSystem::NET_RANK);

		WNetworkSystem::Instance()->SendMessage(WNetworkSystem::NET_RANK,
			NetworkUnit::eEVENT_NEXT);
	}

	if (m_pLoginRsDlg == NULL)
	{
		m_pLoginRsDlg = CreateForm<FrLoginRsDlg>(g_pFresh->GetManager(), this,
			"login_gs", NULL);
		FrLoginRsDlg::SetState(LOGINRS_CONNECTING);
		m_pLoginRsDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnLoginRsDlgResult,
			0x41);
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_GuildInit(int param)
{
	m_pUnderBarGuild = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarGuild)
	{
		m_pUnderBarGuild->EnableHover(true, 0.0f);

		m_pUnderBarGuild->SetVisible(false);
	}
}

void CTaskMain::OnUnderBar_GuildUp()
{
	CloseAllDialogs();
	COption::Instance()->vApplyLobbyScreenSize();

	char szUrl[1024] = { 0 };
	sprintf(szUrl,
		"http://www.pangya.com/new3/800x600/index.asp?userid=%s&pwd=%s",
		MyUID(), LOGINPW());
	CBrowser::Instance()->Open(szUrl,
		IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? true : false, true);
	OnBarHoverOff(0);
}

void CTaskMain::OnUnderBar_NoticeInit(int param)
{
	m_pUnderBarNotice = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pUnderBarNotice)
	{
		m_pUnderBarNotice->EnableHover(true, 0.0f);

		WRect rect = m_pUnderBarNotice->GetRect();
		rect.x += 24.5f;
		m_pUnderBarNotice->SetRect(rect);
	}
}

void CTaskMain::OnUnderBar_NoticeUp()
{
	CloseAllDialogs();

	m_pNoticeDlg = CreateForm<FrNoticeDlg>(g_pFresh->GetManager(), this,
		"notice_popup", NULL);
	m_pNoticeDlg->LoadInitFile("notice_popup.ini");
	m_pNoticeDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnNoticeDlgResult, 0x11);

	if (m_pUnderBarNotice && m_pNoticeDlg)
		m_pNoticeDlg->SetIconRect(m_pUnderBarNotice->GetRect());

	OnBarHoverOff(0);
	Doc()->m_bShowNotice = false;
}

void CTaskMain::InitOverBar()
{
	m_pOverBarTab = NULL;

	m_pOverBarChar = NULL;
	m_pOverBarCaddie = NULL;
	m_pOverBarAztec = NULL;
	m_pOverBarClub = NULL;
	m_pOverBarCharSel = NULL;
	m_pOverBarCaddieSel = NULL;

	m_pOverBarClubSel = NULL;
	m_pOverBarAztecSel = NULL;
	m_pOverBarMascotSel = NULL;
	m_pOverBarSite = NULL;
	m_pOverBarPangCookie = NULL;
	m_pOverBarTitle = NULL;
	m_pOverBarOneLineView = NULL;
	m_pOverBarOneLineReqBtn = NULL;
	m_bShowSubSelection = false;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	m_bOverBarFlag0 = false;
	m_bOverBarFlag1 = false;
}

void CTaskMain::ProcessOverBar()
{
	if (g_input->GetButton(LEFT_BUTTON) == 2)
		m_bOverBarFlag0 = true;

	if (m_bOverBarFlag1 && m_bOverBarFlag0)
	{
		if (g_input->GetButton(LEFT_BUTTON) == 1)
		{
			m_bOverBarFlag0 = false;
			m_bOverBarFlag1 = false;
			SendSubSelectionPacket((PANGYA_ITEM_GROUP)0x20);
		}
	}
}

void CTaskMain::SendSubSelectionPacket(PANGYA_ITEM_GROUP group)
{
	CloseAllSubSelection();

	int packetType;
	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		packetType = 12;
	else
		packetType = 11;

	if (group == PIG_MAX)
		group = (PANGYA_ITEM_GROUP)(m_subSelTypeId >> 26);

	switch (group)
	{
	case PIG_CHAR:
	{
		if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()) &&
			Doc()->m_roomInfo.roomGuid != 0xffff &&
			!IsMassGame(Doc()->m_roomInfo.holeType))
			packetType = 12;

		WSendPacket packet(packetType);
		packet.Encode1(4);
		packet.Encode4(m_subSelItemId);
		packet.Send(TO_GAME);
		break;
	}
	case PIG_CADDIE:
	{
		WSendPacket packet(packetType);
		packet.Encode1(1);
		packet.Encode4(m_subSelItemId);
		packet.Send(TO_GAME);
		break;
	}
	case PIG_CLUBSET:
	{
		WSendPacket packet(packetType);
		packet.Encode1(3);
		packet.Encode4(m_subSelItemId);
		packet.Send(TO_GAME);
		break;
	}
	case PIG_BALL:
	{
		WSendPacket packet(packetType);
		packet.Encode1(2);
		packet.Encode4(m_subSelTypeId);
		packet.Send(TO_GAME);
		break;
	}
	case PIG_MASCOT:
	{
		WSendPacket packet(packetType);
		packet.Encode1(5);
		packet.Encode4(m_subSelItemId);
		packet.Send(TO_GAME);
		break;
	}
	}
}

void CTaskMain::ShowOverAndUnderBar(bool bShow)
{
	static bool s_bShow = true;
	static bool s_bVisible[3];

	if (s_bShow != bShow)
	{
		s_bShow = bShow;

		for (int i = 0; i < 3; i++)
		{
			if ((&m_pOverBar)[i])
			{
				if (bShow)
					(&m_pOverBar)[i]->SetVisible(s_bVisible[i]);
				else
				{
					s_bVisible[i] = (&m_pOverBar)[i]->IsVisible();
					(&m_pOverBar)[i]->SetVisible(false);
				}
			}
		}

		if (m_pUnderBarMaket)
			m_pUnderBarMaket->SetVisible(false);
	}
}

void CTaskMain::EnableOverAndUnderBar(bool bEnable)
{
	static bool s_bEnable = true;
	static bool s_bEnabled[35];

	FrWnd* wnds[] = { m_pOverBarChar, m_pOverBarCaddie, m_pOverBarAztec,
		m_pOverBarClub, m_pOverBarCharSel, m_pOverBarCaddieSel,
		m_pOverBarAztecSel, m_pOverBarClubSel, m_pOverBarMascotSel,
		m_pOverBarPangCookie, m_pOverBarTitle, m_pOverBarOneLineView,
		m_pOverBarOneLineReqBtn, m_pUnderBarBack, m_pUnderBarFront,
		m_pUnderBarShop, m_pUnderBarMyRoom, m_pUnderBarItemStorage,
		m_pUnderBarGift, m_pUnderBarCookie, m_pUnderBarBongdariShop,
		m_pUnderBarGame, m_pUnderBarServer, m_pUnderBarExit, m_pUnderBarTabBtn,
		m_pOverBar, m_pUnderBar, m_pUnderTab, m_pUnderBarMagicBox,
		m_pUnderBarScratch, m_pUnderBarRanking, m_pUnderBarGuild,
		m_pUnderBarNotice, m_pOverBarMascot, m_pUnderBarMaket };

	if (s_bEnable == bEnable)
		return;

	s_bEnable = bEnable;

	for (unsigned int i = 0; i < sizeof(wnds) / sizeof(wnds[0]); i++)
	{
		if (wnds[i])
		{
			if (bEnable)
			{
				wnds[i]->Enable(s_bEnabled[i]);
			}
			else
			{
				s_bEnabled[i] = wnds[i]->IsEnabled();
				wnds[i]->Enable(false);
			}
		}
	}

	if (m_pUnderBarMaket)
		m_pUnderBarMaket->SetVisible(false);
}

void CTaskMain::EnableUnderBar(bool bEnable)
{
	m_pUnderBarShop->Enable(bEnable);
	m_pUnderBarMyRoom->Enable(bEnable);
	m_pUnderBarItemStorage->Enable(bEnable);
	m_pUnderBarGift->Enable(bEnable);
	m_pUnderBarCookie->Enable(bEnable);
	m_pUnderBarBongdariShop->Enable(bEnable);
	m_pUnderBarGame->Enable(bEnable);
	m_pUnderBarServer->Enable(bEnable);
	m_pUnderBarTabBtn->Enable(bEnable);
}

void CTaskMain::ShowAllSubSelection()
{
	m_bShowSubSelection = true;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;
}

void CTaskMain::CloseAllSubSelection()
{
	if (m_pOverBarCharSel)
		m_pOverBarCharSel->SetVisible(false);
	if (m_pOverBarCaddieSel)
		m_pOverBarCaddieSel->SetVisible(false);
	if (m_pOverBarCaddieSel)
		m_pOverBarCaddieSel->SetVisible(false);
	if (m_pOverBarAztecSel)
		m_pOverBarAztecSel->SetVisible(false);
	if (m_pOverBarClubSel)
		m_pOverBarClubSel->SetVisible(false);
	if (m_pOverBarMascotSel)
		m_pOverBarMascotSel->SetVisible(false);
	m_bShowSubSelection = false;
}

void CTaskMain::OnOverBar_Bar1Init(int param)
{
	m_pOverBarBar1 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarBar1)
	{
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
			m_pOverBarBar1->SetBgImg("classic_over_bar_01");
	}
}

void CTaskMain::OnOverBar_Bar2Init(int param)
{
	m_pOverBarBar2 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarBar2)
	{
		float width = m_pOverBarBar3->GetRect().x -
			m_pOverBarBar1->GetRect().x - m_pOverBarBar1->GetRect().w;

		WRect rect = m_pOverBarBar2->GetRect();
		rect.w = width;
		m_pOverBarBar2->SetRect(rect);
	}
}

void CTaskMain::OnOverBar_Bar3Init(int param)
{
	m_pOverBarBar3 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarBar3)
	{
		SetLeftOffset(m_pOverBarBar3, m_screenWidth);
	}
}

void CTaskMain::OnOverBar_TitleInit(int param)
{
	m_pOverBarTitle = DYNAMIC_CAST(FrArea, param);
}

void CTaskMain::OnOverBar_Frame1Init(int param)
{
	m_pOverBarFrame1 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarFrame1)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarFrame1, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarFrame1, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_Frame2Init(int param)
{
	m_pOverBarFrame2 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarFrame2)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarFrame2, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarFrame2, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_Frame3Init(int param)
{
	m_pOverBarFrame3 = DYNAMIC_CAST(FrArea, param);
	if (m_pOverBarFrame3)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarFrame3, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarFrame3, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_TabInit(int param)
{
	m_pOverBarTab = DYNAMIC_CAST(FrTabButton, param);
	if (m_pOverBarTab)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarTab, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarTab, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_TabDown()
{
	TButtonItem* pItem = m_pOverBarTab->GetItemUnderCursor();
	if (!pItem)
		return;

	if (CSharedDoc::Instance()->m_myInfo.info.IsIdentity(4))
	{
	}
}

void CTaskMain::OnOverBar_CharInit(int param)
{
	m_pOverBarChar = DYNAMIC_CAST(FrViewer, param);
	if (m_pOverBarChar)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarChar, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarChar, m_offsetX, 0.0f);

		m_pOverBarChar->Enable(!g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"));
	}
}

void CTaskMain::OnOverBar_CharDown()
{
	WRect rect = m_pOverBarCharSel->GetRect();

	WPoint pos(rect.x, rect.y + rect.h);
	OpenSubEquipBar(pos, PIG_CHAR, 3);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CTaskMain::OnOverBar_CharOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	unsigned long color = m_pOverBarChar->IsEnabled() ? 0xffffffff : 0x50b0b0b0;
	WRect rect = m_pOverBarChar->GetRect();
	const Bitmap* pBitmap = NULL;

	if (Doc()->m_myInfo.userEquip.guidChar == 0)
		pBitmap = g_pFresh->GetBitmap("bar_no");
	else
	{
		CHAR_MAP_IT it =
			Doc()->m_charMap.find(Doc()->m_myInfo.userEquip.guidChar);
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
	}

	if (pBitmap)
		pGDI->DrawTexture(pBitmap, rect, color, false);

	_DrawSelection(m_pOverBarChar, rect, TOOLTIP_CHARACTER, pGDI);
}

void CTaskMain::_DrawSelection(FrViewer* pViewer, WRect& rect,
	eBarButton button, FrGraphicInterface* pGDI)
{
	if (pGDI == NULL)
		pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	WPoint mouse(g_input->GetMousePoint().x, g_input->GetMousePoint().y);
	if (pViewer->IsEnabled() && rect.IsInRect(mouse))
	{
		const Bitmap* pBitmap = g_pFresh->GetBitmap("select_frame");
		if (pBitmap)
			pGDI->DrawTexture(pBitmap,
				WRect(rect.x - 3.0f, rect.y - 3.0f, rect.w + 6.0f,
					rect.h + 6.0f),
				0xffffffff, false);

		CIconToolTip::GetInstance()->SetToolTip(button);
	}
	else
	{
		if (TOOLTIP()->IsCurToolTip(button))
			OnBarHoverOff(0);
	}
}

void CTaskMain::NotifyServerControlMsg(int type)
{
	const char* msg;
	if (_util::IsAvatarChatServer(0x40))
		msg = _util::GetAvatarServerServiceMessage(type);
	else
		msg = _util::GetNormalServerServiceMessage(type);

	if (msg)
		this << MsgObject(NULL, 0x23, (int)msg, 0, 0, 0, 0);
}

void CTaskMain::OnOverBar_CaddieInit(int param)
{
	m_pOverBarCaddie = DYNAMIC_CAST(FrViewer, param);
	if (m_pOverBarCaddie)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarCaddie, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarCaddie, m_offsetX, 0.0f);

		m_pOverBarCaddie->Enable(!g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"));
	}
}

void CTaskMain::OnOverBar_CaddieDown()
{
	WRect rect = m_pOverBarCaddieSel->GetRect();

	WPoint pos(rect.x, rect.y + rect.h);
	OpenSubEquipBar(pos, PIG_CADDIE, 3);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CTaskMain::OnOverBar_CaddieOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	unsigned long color =
		m_pOverBarCaddie->IsEnabled() ? 0xffffffff : 0x50b0b0b0;
	WRect rect = m_pOverBarCaddie->GetRect();
	const Bitmap* pBitmap = NULL;
	bool bExpired = false;

	if (Doc()->m_myInfo.userEquip.guidCaddie == 0)
		pBitmap = g_pFresh->GetBitmap("bar_no");
	else
	{
		CADDIE_MAP_IT it =
			Doc()->m_caddieMap.find(Doc()->m_myInfo.userEquip.guidCaddie);
		sCaddieInfo& info = (*it).second;
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(info.tid);
		if (pItem)
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));

		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(info.tid);
		if (pCaddie)
			if (info.Rent_flag && info.Remain_Date <= 0 && info.tidPart == 0 &&
				pCaddie->MonthlyFee != 0)
				bExpired = true;
	}

	if (pBitmap)
		pGDI->DrawTexture(pBitmap, rect, color, false);

	if (bExpired)
	{
		pGDI->Line(WPoint(rect.x, rect.y + rect.h * 0.5f - 1.0f),
			WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f - 1.0f),
			0xffff0000, 0xffff0000, 0);
		pGDI->Line(WPoint(rect.x, rect.y + rect.h * 0.5f),
			WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f), 0xffff0000,
			0xffff0000, 0);
		pGDI->Line(WPoint(rect.x, rect.y + rect.h * 0.5f + 1.0f),
			WPoint(rect.x + rect.w - 1.0f, rect.y + rect.h * 0.5f + 1.0f),
			0xffff0000, 0xffff0000, 0);
	}

	_DrawSelection(m_pOverBarCaddie, rect, TOOLTIP_CADDIE, pGDI);
}

void CTaskMain::OnOverBar_ClubInit(int param)
{
	m_pOverBarClub = DYNAMIC_CAST(FrViewer, param);
	if (m_pOverBarClub)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarClub, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarClub, m_offsetX, 0.0f);

		m_pOverBarClub->Enable(!g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"));
	}
}

void CTaskMain::OnOverBar_ClubDown()
{
	WRect rect = m_pOverBarClubSel->GetRect();

	WPoint pos(rect.x, rect.y + rect.h);
	OpenSubEquipBar(pos, PIG_CLUBSET, 3);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CTaskMain::OnOverBar_ClubOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	unsigned long color = m_pOverBarClub->IsEnabled() ? 0xffffffff : 0x50b0b0b0;
	WRect rect = m_pOverBarClub->GetRect();
	const Bitmap* pBitmap = NULL;

	if (Doc()->m_myInfo.userEquip.guidClubSet == 0)
		pBitmap = g_pFresh->GetBitmap("bar_no");
	else
	{
		CLUBSET_MAP_IT it =
			Doc()->m_clubSetMap.find(Doc()->m_myInfo.userEquip.guidClubSet);
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
	}

	if (pBitmap)
		pGDI->DrawTexture(pBitmap, rect, color, false);

	_DrawSelection(m_pOverBarClub, rect, TOOLTIP_CLUB, pGDI);
}

void CTaskMain::OnOverBar_AztecInit(int param)
{
	m_pOverBarAztec = DYNAMIC_CAST(FrViewer, param);
	if (m_pOverBarAztec)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarAztec, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarAztec, m_offsetX, 0.0f);

		m_pOverBarAztec->Enable(!g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"));
	}
}

void CTaskMain::OnOverBar_AztecDown()
{
	WRect rect = m_pOverBarAztecSel->GetRect();

	WPoint pos(rect.x, rect.y + rect.h);
	OpenSubEquipBar(pos, PIG_BALL, 3);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CTaskMain::OpenSubEquipBar(const WPoint& pos, PANGYA_ITEM_GROUP group,
	int subType)
{
	if (m_pSubEquipBar == NULL)
	{
		m_pSubEquipBar = CreateForm<FrSubEquipBar>(g_pFresh->GetManager(), this,
			"EQUIPBAR_SUB", NULL);
		if (m_pSubEquipBar)
		{
			m_pSubEquipBar->Open(
				(FRESH_PFN_RESULT)&CTaskMain::OnResultSubEquipBar,
				(FrSubEquipBar::eCategory)group, pos,
				(FrSubEquipBar::eSubType)subType);
		}
	}
}

bool CTaskMain::OnResultSubEquipBar(int result, FrForm* pForm)
{
	if (result == 1)
	{
		IFF_ITEM_COMMON* pItem = m_pSubEquipBar->GetResult();
		PANGYA_ITEM_GROUP group = PIG_MAX;
		if (pItem)
		{
			PANGYA_ITEM_GROUP itemGroup =
				(PANGYA_ITEM_GROUP)(pItem->TypeId >> 26);
			if (itemGroup == PIG_BALL)
			{
				m_subSelTypeId = Doc()->m_myInfo.userEquip.tidBall =
					pItem->TypeId;
				m_subSelItemId = 0;
			}
			else
			{
				m_subSelTypeId = pItem->TypeId;
				m_subSelItemId = m_pSubEquipBar->GetLastSelectedGuid();
				if (!m_pSubEquipBar->SetClientEquipInfo(itemGroup,
						m_subSelItemId))
				{
					m_pSubEquipBar = NULL;
					return true;
				}
			}
		}
		else
		{
			switch (m_pSubEquipBar->GetCategory())
			{
			case FrSubEquipBar::CATEGORY_MASCOT:
				Doc()->m_myInfo.userEquip.guidMascot = 0;
				group = PIG_MASCOT;
				break;
			case FrSubEquipBar::CATEGORY_CADDIE:
				Doc()->m_myInfo.userEquip.guidCaddie = 0;
				group = PIG_CADDIE;
				break;
			}
			m_subSelItemId = 0;
			m_subSelTypeId = 0;
		}

		SendSubSelectionPacket(group);
	}

	m_pSubEquipBar = NULL;
	return true;
}

void CTaskMain::OnOverBar_AztecOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	unsigned long color =
		m_pOverBarAztec->IsEnabled() ? 0xffffffff : 0x50b0b0b0;
	WRect rect = m_pOverBarAztec->GetRect();
	const Bitmap* pBitmap = NULL;

	if (Doc()->m_myInfo.userEquip.tidBall == 0)
		pBitmap = g_pFresh->GetBitmap("bar_no");
	else
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem(Doc()->m_myInfo.userEquip.tidBall);
		if (pItem)
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
	}

	if (pBitmap)
		pGDI->DrawTexture(pBitmap, rect, color, false);

	_DrawSelection(m_pOverBarAztec, rect, TOOLTIP_AZTEC, pGDI);
}

void CTaskMain::OnOverBar_MascotInit(int param)
{
	m_pOverBarMascot = DYNAMIC_CAST(FrViewer, param);
	if (m_pOverBarMascot)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarMascot, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarMascot, m_offsetX, 0.0f);

		m_pOverBarMascot->Enable(!g_pFresh->IsCurrentLayout("GAMEROOM_EXTRES"));
	}
}

void CTaskMain::OnOverBar_MascotDown()
{
	WRect rect = m_pOverBarMascotSel->GetRect();

	WPoint pos(rect.x, rect.y + rect.h);
	OpenSubEquipBar(pos, PIG_MASCOT, 3);

	if (m_pSubEquipBar)
	{
		WRect barRect = m_pSubEquipBar->GetRect();
		barRect.x = rect.x;
		m_pSubEquipBar->SetRect(barRect);
	}
}

void CTaskMain::OnOverBar_MascotOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	unsigned long color =
		m_pOverBarMascot->IsEnabled() ? 0xffffffff : 0x50b0b0b0;
	WRect rect = m_pOverBarMascot->GetRect();
	const Bitmap* pBitmap = NULL;

	if (Doc()->m_myInfo.userEquip.guidMascot == 0)
		pBitmap = g_pFresh->GetBitmap("bar_no");
	else
	{
		MASCOT_MAP_IT it =
			Doc()->m_mascotMap.find(Doc()->m_myInfo.userEquip.guidMascot);
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
	}

	if (pBitmap)
		pGDI->DrawTexture(pBitmap, rect, color, false);

	_DrawSelection(m_pOverBarMascot, rect, TOOLTIP_MASCOT, pGDI);
}

void CTaskMain::OnOverBar_CharSelInit(int param)
{
	m_pOverBarCharSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pOverBarCharSel)
	{
		m_pOverBarCharSel->SetVisible(false);

		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarCharSel, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarCharSel, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_CharSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_charMap.size();
	if (count <= 1)
		return;

	float x = m_pOverBarCharSel->GetRect().x;
	float y = m_pOverBarCharSel->GetRect().y;
	float w;

	const Bitmap* pBitmap = g_pFresh->GetBitmap("select_bar_01");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetBitmap("select_bar_02");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = (float)count * 24.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetBitmap("select_bar_03");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	y += 7.0f;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	std::map<unsigned int, sCharacterInfo>::iterator it;
	for (it = Doc()->m_charMap.begin(); it != Doc()->m_charMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			WRect dest(startX, y, 24.0f, 26.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

			if (dest.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap = g_pFresh->GetBitmap("select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
							dest.h + 6.0f),
						0xffffffff, 0);

				if (Doc()->m_myInfo.userEquip.guidChar != (*it).second.guid)
				{
					m_subSelTypeId = (*it).second.tid;
					m_subSelItemId = (*it).second.guid;
				}
			}

			startX += 27.0f;
		}
	}
}

void CTaskMain::OnOverBar_CaddieSelInit(int param)
{
	m_pOverBarCaddieSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pOverBarCaddieSel)
	{
		m_pOverBarCaddieSel->SetVisible(false);

		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarCaddieSel, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarCaddieSel, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_CaddieSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_caddieMap.size();
	if (count < 1)
		return;

	if (Doc()->m_myInfo.userEquip.guidCaddie)
		count++;

	float x = m_pOverBarCaddieSel->GetRect().x;
	float y = m_pOverBarCaddieSel->GetRect().y;
	float w;

	const Bitmap* pBitmap = g_pFresh->GetBitmap("select_bar_01");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetBitmap("select_bar_02");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = (float)count * 24.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetBitmap("select_bar_03");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	y += 7.0f;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	std::map<unsigned int, sCaddieInfo>::iterator it;
	for (it = Doc()->m_caddieMap.begin(); it != Doc()->m_caddieMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			WRect dest(startX, y, 24.0f, 26.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

			IFF_STRUCT::sCaddie* pCaddie =
				ItemManager()->FindCaddie((*it).second.tid);
			if (pCaddie)
			{
				if ((*it).second.Rent_flag && (*it).second.Remain_Date <= 0 &&
					(*it).second.tidPart == 0 && pCaddie->MonthlyFee)
				{
					pGDI->Line(WPoint(dest.x, dest.y + dest.h * 0.5f - 1.0f),
						WPoint(dest.x + dest.w - 1.0f,
							dest.y + dest.h * 0.5f - 1.0f),
						0xffff0000, 0xffff0000, 0);
					pGDI->Line(WPoint(dest.x, dest.y + dest.h * 0.5f),
						WPoint(dest.x + dest.w - 1.0f, dest.y + dest.h * 0.5f),
						0xffff0000, 0xffff0000, 0);
					pGDI->Line(WPoint(dest.x, dest.y + dest.h * 0.5f + 1.0f),
						WPoint(dest.x + dest.w - 1.0f,
							dest.y + dest.h * 0.5f + 1.0f),
						0xffff0000, 0xffff0000, 0);
				}
			}

			if (dest.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap = g_pFresh->GetBitmap("select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
							dest.h + 6.0f),
						0xffffffff, 0);

				if (Doc()->m_myInfo.userEquip.guidCaddie != (*it).second.guid)
				{
					m_subSelTypeId = (*it).second.tid;
					m_subSelItemId = (*it).second.guid;
				}
			}

			startX += 27.0f;
		}
	}

	if (Doc()->m_myInfo.userEquip.guidCaddie)
	{
		WRect dest(startX, y, 24.0f, 26.0f);

		pBitmap = g_pFresh->GetBitmap("s_no");
		if (pBitmap)
			pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

		if (dest.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			pBitmap = g_pFresh->GetBitmap("select_frame");
			if (pBitmap)
				pGDI->DrawTexture(pBitmap,
					WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
						dest.h + 6.0f),
					0xffffffff, 0);

			m_subSelTypeId = (*Doc()->m_caddieMap.begin()).second.tid;
			m_subSelItemId = 0;
		}
	}
}

void CTaskMain::OnOverBar_ClubSelInit(int param)
{
	m_pOverBarClubSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pOverBarClubSel)
	{
		m_pOverBarClubSel->SetVisible(false);

		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarClubSel, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarClubSel, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_ClubSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_clubSetMap.size();
	if (count <= 1)
		return;

	float x = m_pOverBarClubSel->GetRect().x;
	float y = m_pOverBarClubSel->GetRect().y;
	float w;

	const Bitmap* pBitmap = g_pFresh->GetBitmap("select_bar_01");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetBitmap("select_bar_02");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = (float)count * 24.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetBitmap("select_bar_03");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	y += 7.0f;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	std::map<unsigned int, sItemInfo>::iterator it;
	for (it = Doc()->m_clubSetMap.begin(); it != Doc()->m_clubSetMap.end();
		++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 24.0f, 26.0f),
					0xffffffff, 0);

			bool bUsable = Doc()->CheckUnderLevelClubSet((*it).second.tid);
			if (!bUsable)
			{
				WRect rc(startX, y, 24.0f, 26.0f);
				pGDI->Line(WPoint(rc.x, rc.y + rc.h * 0.5f - 1.0f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f - 1.0f),
					0xffff0000, 0xffff0000, 0);
				pGDI->Line(WPoint(rc.x, rc.y + rc.h * 0.5f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f), 0xffff0000,
					0xffff0000, 0);
				pGDI->Line(WPoint(rc.x, rc.y + rc.h * 0.5f + 1.0f),
					WPoint(rc.x + rc.w - 1.0f, rc.y + rc.h * 0.5f + 1.0f),
					0xffff0000, 0xffff0000, 0);
			}

			if (WRect(startX, y, 24.0f, 26.0f)
					.IsInRect(WPoint(g_input->GetMousePoint().x,
						g_input->GetMousePoint().y)))
			{
				pBitmap = g_pFresh->GetBitmap("select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 30.0f, 32.0f),
						0xffffffff, 0);

				if (Doc()->m_myInfo.userEquip.guidClubSet != (*it).second.guid)
				{
					if (bUsable == true)
					{
						m_subSelTypeId = (*it).second.tid;
						m_subSelItemId = (*it).second.guid;
					}
				}
			}

			startX += 27.0f;
		}
	}
}

void CTaskMain::OnOverBar_AztecSelInit(int param)
{
	m_pOverBarAztecSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pOverBarAztecSel)
	{
		m_pOverBarAztecSel->SetVisible(false);

		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarAztecSel, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarAztecSel, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_AztecSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	if (ItemManager()->FindCommonItem(Doc()->m_myInfo.userEquip.tidBall) ==
		NULL)
		return;

	int count = Doc()->m_ballList.size();
	if (count <= 1)
		return;

	float x = m_pOverBarAztecSel->GetRect().x;
	float y = m_pOverBarAztecSel->GetRect().y;
	float w;

	const Bitmap* pBitmap = g_pFresh->GetBitmap("select_bar_01");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetBitmap("select_bar_02");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = (float)count * 24.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetBitmap("select_bar_03");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	y += 7.0f;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	std::list<sItemInfo>::iterator it;
	for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem((*it).tid);
		if (pItem)
		{
			WRect rc(startX, y, 24.0f, 26.0f);

			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 24.0f, 26.0f),
					0xffffffff, 0);

			if (rc.IsInRect(WPoint(g_input->GetMousePoint().x,
					g_input->GetMousePoint().y)))
			{
				pBitmap = g_pFresh->GetBitmap("select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 30.0f, 32.0f),
						0xffffffff, 0);

				if (Doc()->m_myInfo.userEquip.tidBall != (*it).tid)
				{
					m_subSelTypeId = (*it).tid;
					m_subSelItemId = (*it).guid;
				}
			}

			startX += 27.0f;
		}
	}
}

void CTaskMain::OnOverBar_MascotSelInit(int param)
{
	m_pOverBarMascotSel = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pOverBarMascotSel)
	{
		m_pOverBarMascotSel->SetVisible(false);

		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarMascotSel, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarMascotSel, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_MascotSelOwnerDraw(int param)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	int count = Doc()->m_mascotMap.size();
	if (count < 1)
		return;

	if (Doc()->m_myInfo.userEquip.guidMascot)
		count++;

	float x = m_pOverBarMascotSel->GetRect().x;
	float y = m_pOverBarMascotSel->GetRect().y;
	float w;

	const Bitmap* pBitmap = g_pFresh->GetBitmap("select_bar_01");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	float startX = x;
	pBitmap = g_pFresh->GetBitmap("select_bar_02");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = (float)count * 24.0f + (float)(count - 1) * 3.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	x += w;
	pBitmap = g_pFresh->GetBitmap("select_bar_03");
	if (pBitmap)
	{
		const BITMAPINFO* info = pBitmap->bi;
		w = 8.0f;
		pGDI->DrawTexture(pBitmap,
			WRect(x, y, w, (float)(unsigned)info->bmiHeader.biHeight),
			0xffffffff, 0);
	}

	y += 7.0f;
	m_subSelTypeId = 0;
	m_subSelItemId = 0;

	std::map<unsigned int, sMascotInfo>::iterator it;
	for (it = Doc()->m_mascotMap.begin(); it != Doc()->m_mascotMap.end(); ++it)
	{
		IFF_ITEM_COMMON* pItem =
			ItemManager()->FindCommonItem((*it).second.tid);
		if (pItem)
		{
			pBitmap = g_pFresh->GetBitmap(MakeStr("s_%s", pItem->Icon));
			if (pBitmap)
				pGDI->DrawTexture(pBitmap, WRect(startX, y, 24.0f, 26.0f),
					0xffffffff, 0);

			if (WRect(startX, y, 24.0f, 26.0f)
					.IsInRect(WPoint(g_input->GetMousePoint().x,
						g_input->GetMousePoint().y)))
			{
				pBitmap = g_pFresh->GetBitmap("select_frame");
				if (pBitmap)
					pGDI->DrawTexture(pBitmap,
						WRect(startX - 3.0f, y - 3.0f, 30.0f, 32.0f),
						0xffffffff, 0);

				if (Doc()->m_myInfo.userEquip.guidMascot != (*it).second.guid)
				{
					m_subSelTypeId = (*it).second.tid;
					m_subSelItemId = (*it).second.guid;
				}
			}

			startX += 27.0f;
		}
	}

	if (Doc()->m_myInfo.userEquip.guidMascot)
	{
		WRect dest(startX, y, 24.0f, 26.0f);

		pBitmap = g_pFresh->GetBitmap("s_no");
		if (pBitmap)
			pGDI->DrawTexture(pBitmap, dest, 0xffffffff, 0);

		if (dest.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			pBitmap = g_pFresh->GetBitmap("select_frame");
			if (pBitmap)
				pGDI->DrawTexture(pBitmap,
					WRect(dest.x - 3.0f, dest.y - 3.0f, dest.w + 6.0f,
						dest.h + 6.0f),
					0xffffffff, 0);

			m_subSelTypeId = (*Doc()->m_mascotMap.begin()).second.tid;
			m_subSelItemId = 0;
		}
	}
}

void CTaskMain::OnOverBar_SiteInit(int param)
{
	m_pOverBarSite = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
	if (m_pOverBarSite)
	{
		UpdateSiteInfo();

		SetCenterOffset(m_pOverBarSite, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_OneLineReqBtnInit(int param)
{
	m_pOverBarOneLineReqBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOverBarOneLineReqBtn)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarOneLineReqBtn, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarOneLineReqBtn, m_offsetX, 0.0f);
	}
}

void CTaskMain::OnOverBar_OneLineReqBtnUp()
{
	if (Doc()->IsControlServerService(0x20000))
	{
		this << MsgObject(this, 0x23,
			(int)"\xc0\xfc\xb1\xa4\xc6\xc7 \xb0\xfc\xb7\xc3 \xba\xce\xba\xd0 "
				 "\xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
			0, 0, 0, 0);
		return;
	}

	char allowed = 0;
	IActor* actor = AfxGetTask()->GetActor(s_gatewayActor[1]);
	actor << MsgObject(NULL, 621, 16, (int)&allowed, 0, 0, 0);
	if (!allowed)
	{
		actor << MsgObject(NULL, 621, 17, 0, 0, 0, 0);
		return;
	}

	CloseAllDialogs();

	if (Doc()->GetFlagBlock() & 0x10)
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetTimeLimit(12.0f);
		pForm->SetMessage(
			"\xc7\xf6\xc0\xe7 \xc0\xfc\xb1\xa4\xc6\xc7 \xc0\xcc\xbf\xeb "
			"\xc1\xa6\xc0\xe7\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. "
			"\xc0\xfc\xb1\xa4\xc6\xc7\xc0\xbb \xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc7 "
			"\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		pForm->Open(NULL, 0);
	}
	else
	{
		m_pOnelineReqDlg = CreateForm<FrOnelineReqDlg>(g_pFresh->GetManager(),
			this, "oneline_req");
		m_pOnelineReqDlg->Open(
			(FRESH_PFN_RESULT)&CTaskMain::OnOnelineReqDlgResult, 0x11);
		if (!g_pFresh->GetManager()->IsKeyExclusive())
			g_pFresh->GetManager()->SetExclusiveKey(true);

		m_pOnelineReqDlg->SetIconRect(m_pOverBarOneLineReqBtn->GetRect());

		OnBarHoverOff(0);
	}
}

void CTaskMain::OnOverBar_OneLineViewInit(int param)
{
	m_pOverBarOneLineView = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pOverBarOneLineView)
	{
		if (m_screenWidth < 800.0f)
			SetCenterOffset(m_pOverBarOneLineView, -140.0f, 0.0f);
		else
			SetCenterOffset(m_pOverBarOneLineView, m_offsetX, 0.0f);

		WSingleton<COneLineBoard>::Instance()->SetRect(
			m_pOverBarOneLineView->GetRect());
	}
}

void CTaskMain::OnOverBar_OneLineViewOwnerDraw(int param)
{
	WSingleton<COneLineBoard>::Instance()->Display();
}

void CTaskMain::OnOverBar_OptionInit(int param)
{
	m_pOverBarOption = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOverBarOption)
	{
		m_pOverBarOption->EnableHover(true, 0.0f);

		SetLeftOffset(m_pOverBarOption, m_screenWidth);
	}
}

void CTaskMain::OnOverBar_OptionUp()
{
	CloseAllDialogs();

	m_pOptionDlg =
		CreateForm<FrOptionDlg>(g_pFresh->GetManager(), this, "option");
	m_pOptionDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnOptionDlgResult, 0x11);
	m_pOptionDlg->SetIconRect(m_pOverBarOption->GetRect());
	m_pOptionDlg->SetApplyScrSize(
		IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? true : false);

	OnBarHoverOff(0);
}

void CTaskMain::OnOverBar_MyInfoInit(int param)
{
	m_pOverBarMyInfo = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOverBarMyInfo)
	{
		m_pOverBarMyInfo->EnableHover(true, 0.0f);

		SetLeftOffset(m_pOverBarMyInfo, m_screenWidth);
	}
}

void CTaskMain::OnOverBar_MyInfoUp()
{
	CloseAllDialogs();

	WSingleton<CUserInfo>::Instance()->SetInfo(MyUID(), MyGuid(false), true,
		true, false, false, MyNick());

	OnBarHoverOff(0);
}

void CTaskMain::OnOverBar_PangCookieInit(int param)
{
	m_pOverBarPangCookie = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pOverBarPangCookie)
	{
		SetLeftOffset(m_pOverBarPangCookie, m_screenWidth);

		memset(m_pPangCookieBmp, 0, sizeof(Bitmap*) * 2);
		m_pPangCookieBmp[0] = g_pFresh->GetBitmap("pang_bar_treecash");
		m_pPangCookieBmp[1] = g_pFresh->GetBitmap("pang_bar_frame_tree");
		m_pPangCookieBmp[2] = g_pFresh->GetBitmap("pang_bar_hancoin");
		m_pPangCookieBmp[3] = g_pFresh->GetBitmap("pang_bar_frame_han");
	}
}

void CTaskMain::OnOverBar_PangCookieOwnerDraw(int param)
{
	if (CProjectG::Instance()->HidePrivacy())
		return;

	char buf[32] = { 0 };
	const WRect& rect = m_pOverBarPangCookie->GetRect();

	MakePriceStrA(buf, Doc()->m_myInfo.stat.i64Pang);
	Print(rect.x + 113.0f, rect.y + 6.0f, buf, 2.0f, 2, 1.0f, 0xffffd18e);

	__int64 cookie = S5::ExchangeOwnCashToCookie(Doc()->m_cash, 0, 100);
	if (cookie < 0)
		cookie = 0;
	MakePriceStrA(buf, cookie);
	if (Doc()->m_bonusCash)
	{
		char bonus[32] = { 0 };
		strcat(buf, "/");
		MakePriceStrA(bonus, Doc()->m_bonusCash);
		strcat(buf, bonus);
	}
	Print(rect.x + 218.0f, rect.y + 6.0f, buf, 2.0f, 2, 1.0f, 0xffffd18e);

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	const Bitmap* pIcon;
	const Bitmap* pFrame;
	switch (CLoginInfo::Instance()->ProvType())
	{
	case 2:
		pIcon = m_pPangCookieBmp[0];
		pFrame = m_pPangCookieBmp[1];
		break;
	case 4:
		pIcon = m_pPangCookieBmp[2];
		pFrame = m_pPangCookieBmp[3];
		break;
	default:
		return;
	}

	if (pIcon == NULL || pFrame == NULL)
		return;

	float iconW = (float)pIcon->Width();
	WRect iconRect(rect.x + rect.w - (iconW + 5.0f), rect.y + 3.0f, iconW,
		(float)pIcon->Height());
	pGDI->DrawTexture(pIcon, iconRect, 0xffffffff, 0);

	iconRect.x -= 100.0f;
	iconRect.w += 150.0f;
	if (!CMouseCursor::Instance()->InArea(iconRect))
		return;

	pGDI->DrawTexture(pFrame,
		WRect(iconRect.x - 75.0f, iconRect.y + 10.0f, (float)pFrame->Width(),
			(float)pFrame->Height()),
		0xffffffff, 0);
	MakePriceStrA(buf, MyOutsideCash());
	Print(iconRect.x + 75.0f, iconRect.y + 14.0f, buf, 2.0f, 2, 1.0f,
		0xffffd18e);
}

void CTaskMain::OnBarHoverOn(int param)
{
	m_hoverBtn = (int)DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_hoverBtn == 0)
		return;

	int i = 0;
	FrButton** ppButton = &m_pUnderBarBack;
	for (; i < 21; i++, ppButton++)
	{
		if (m_hoverBtn == (int)*ppButton)
		{
			TOOLTIP()->SetToolTip((eBarButton)i);
			return;
		}
	}
}

void CTaskMain::OnBarHoverOff(int param)
{
	FrButton* pButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (pButton == NULL || (int)pButton == m_hoverBtn)
	{
		m_hoverBtn = 0;
		TOOLTIP()->ClearToolTip();
	}
}

void CTaskMain::OnBarTooltipOwnerDraw(int param)
{
	TOOLTIP()->Render();
}

void CTaskMain::OnOverBar_SpecialCardInit(int param)
{
	m_pOverBarSpecialCard = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOverBarSpecialCard == NULL)
		return;

	m_pOverBarSpecialCard->EnableHover(true, 0.0f);
	m_pOverBarSpecialCard->EnableBlink(
		WSingleton<CCardManager>::Instance()->ShowSPCardAlarm());

	SetLeftOffset(m_pOverBarSpecialCard, m_screenWidth);
}

void CTaskMain::OnOverBar_SpecialCardUp()
{
	if (m_pSpecialCardDlg)
	{
		m_pSpecialCardDlg->Close(FrCANCEL, true);
	}
	else
	{
		m_pOverBarSpecialCard->EnableBlink(false);
		WSingleton<CCardManager>::Instance()->SetSPCardAlarm(false);

		m_pSpecialCardDlg = CreateForm<FrSPCardBuffDlg>(g_pFresh->GetManager(),
			this, "spcardbuff", NULL);
		WSingleton<CCardManager>::Instance()->SetCardBuffDlg(m_pSpecialCardDlg);
		CItemBuff::UpdateBuffDlg(m_pSpecialCardDlg);
		m_pSpecialCardDlg->SetVisible(false);
		m_pSpecialCardDlg->SetFixed(true);

		WPoint pos;
		pos = m_pSpecialCardDlg->GetPosition();
		pos.x = pos.x * g_view->GetWidth() * 0.00125f;
		pos.y = pos.y * g_view->GetHeight() * (1.0f / 600.0f);
		m_pSpecialCardDlg->EnableDrag(false);
		m_pSpecialCardDlg->Open(
			(FRESH_PFN_RESULT)&CTaskMain::OnSpecialCardFormResult, pos, 1);

		if (m_pSpecialCardDlg->GetSpecialCardNum() > 0)
		{
			m_pSpecialCardDlg->SetVisible(true);
		}
		else
		{
			m_pSpecialCardDlg->Close(FrCANCEL, true);

			WPoint pt;
			pt.x = m_pOverBarSpecialCard->GetRect().x - 86.0f;
			pt.y = m_pMessengerBubbleEdit->GetRect().y;
			AfxGetTask()->GetMainActor() << MsgObject(NULL, 0xa8,
				(int)"\xbb\xe7\xbf\xeb\xc1\xdf\xc0\xce "
					 "\xc8\xbf\xb0\xfa\xb0\xa1 "
					 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				1, (int)&pt, 0, 0);
		}
	}

	OnBarHoverOff(0);
}

void CTaskMain::OnOverBar_MessengerInit(int param)
{
	m_pOverBarMessenger = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOverBarMessenger == NULL)
		return;

	m_pOverBarMessenger->EnableHover(true, 0.0f);

	MESSENGER()->m_alarmRect = m_pOverBarMessenger->GetRect();
	if (!Doc()->m_bMessengerInit)
		m_pOverBarMessenger->EnableBlink(false);

	SetLeftOffset(m_pOverBarMessenger, m_screenWidth);
}

void CTaskMain::OnOverBar_MessengerUp()
{
	CloseAllDialogs();
	MESSENGER()->Open(true);
	OnBarHoverOff(0);
}

void CTaskMain::OnTreasureAlarmInit(int param)
{
	m_pTreasureAlarm = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTreasureAlarm == NULL)
		return;

	if (!WSingleton<CTHunter>::Instance()->IsUsedTikiReport())
	{
		m_pTreasureAlarm->SetVisible(false);
		m_pTreasureAlarm->Enable(false);
		SetLeftOffset(m_pTreasureAlarm, m_screenWidth);
	}
}

void CTaskMain::OnTreasureAlarmUp()
{
	m_pTreasureAlarm->SetVisible(false);
	m_pTreasureAlarm->Enable(false);
	WSingleton<CTHunter>::Instance()->SetUsedTikiReport(false);

	CTreasureAlarmDlg* pDlg = CreateForm<CTreasureAlarmDlg>(
		g_pFresh->GetManager(), this, "treasure_alarm", NULL);
	pDlg->Open(NULL, 0x11);
	pDlg->SetTopmost(true);
	pDlg->FindNextTopFocus(false);
	pDlg->SetIconRect(pDlg->GetRect());

	OnBarHoverOff(0);
}

void CTaskMain::OnMessengerBubbleInit(int param)
{
	m_pMessengerBubble = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	SetLeftOffset(m_pMessengerBubble, m_screenWidth);
	for (int i = 0; i < 3; i++)
		m_pMessengerBubbleBmp[i] =
			(Bitmap*)g_pFresh->GetBitmap(MakeStr("login_%02d", i));
}

void CTaskMain::OnMessengerBubbleOwnerDraw(int param)
{
	if (m_pMessengerBubbleEdit == NULL)
		return;

	if (!m_pMessengerBubbleEdit->IsVisible())
		return;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (pGDI == NULL)
		return;

	if (m_pMessengerBubbleEdit->GetLineNum() == 0)
		return;

	WRect rect = m_pMessengerBubbleEdit->GetRect();
	rect.x -= 2.0f;
	rect.y -= 8.0f;
	rect.h = m_pMessengerBubbleEdit->GetLineNum() * 14.0f;

	if (m_pMessengerBubbleBmp[0])
	{
		pGDI->DrawTexture(m_pMessengerBubbleBmp[0],
			WRect(rect.x, rect.y, (float)m_pMessengerBubbleBmp[0]->Width(),
				(float)m_pMessengerBubbleBmp[0]->Height()),
			0xffffffff, 0);
		rect.y += (float)m_pMessengerBubbleBmp[0]->Height();
	}

	if (m_pMessengerBubbleBmp[1])
	{
		pGDI->DrawTexture(m_pMessengerBubbleBmp[1],
			WRect(rect.x, rect.y, (float)m_pMessengerBubbleBmp[1]->Width(),
				rect.h),
			0xffffffff, 0);
		rect.y += rect.h;
	}

	if (m_pMessengerBubbleBmp[2])
	{
		pGDI->DrawTexture(m_pMessengerBubbleBmp[2],
			WRect(rect.x, rect.y, (float)m_pMessengerBubbleBmp[2]->Width(),
				(float)m_pMessengerBubbleBmp[2]->Height()),
			0xffffffff, 0);
	}
}

void CTaskMain::OnMessengerBubbleEditInit(int param)
{
	m_pMessengerBubbleEdit = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	m_pMessengerBubbleEdit->SetVisible(false);

	SetLeftOffset(m_pMessengerBubbleEdit, m_screenWidth);
}

void CTaskMain::OnMessengerAlarmBtnInit(int param)
{
	m_pMessengerAlarmBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	SetLeftOffset(m_pMessengerAlarmBtn, m_screenWidth);
	if (m_pMessengerAlarmBtn && !Doc()->m_bMessengerAlarm)
		m_pMessengerAlarmBtn->SetVisible(false);
}

void CTaskMain::OnMessengerAlarmBtnUp()
{
	MESSENGER()->AlarmBtnUp(0xffffffff);
}

void CTaskMain::OnMessengerGiftAlarmBtnInit(int param)
{
	m_pMessengerGiftAlarmBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pMessengerGiftAlarmBtn->SetVisible(false);
	SetLeftOffset(m_pMessengerGiftAlarmBtn, m_screenWidth);
}

void CTaskMain::OnMessengerGiftAlarmBtnUp()
{
	m_pMessengerGiftAlarmBtn->SetVisible(false);

	FrGiftAlarmDlg* pDlg = CreateForm<FrGiftAlarmDlg>(g_pFresh->GetManager(),
		this, "gift_alarm", NULL);
	pDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnNotifyDlgResult, 0x11);
	pDlg->SetTopmost(true);
	pDlg->FindNextTopFocus(false);
	pDlg->SetIconRect(m_pMessengerGiftAlarmBtn->GetRect());

	OnBarHoverOff(0);
}

void CTaskMain::CloseAllDialogs()
{
	if (m_pNoticeDlg)
	{
		m_pNoticeDlg->Close(true);
	}

	if (m_pDlg150)
	{
		m_pDlg150->Close(true);
	}

	if (!m_levelupForms.empty())
	{
		int count = m_levelupForms.size();
		for (int i = 0; i < count; i++)
		{
			if (m_levelupForms[i])
			{
				m_levelupForms[i]->Close(FrNONE, true);
				m_levelupForms[i] = NULL;
			}
		}
		m_levelupForms.clear();
	}
}

bool CTaskMain::OnWaitDlgResult(int result, FrForm* pForm)
{
	if (result == FrCANCEL)
	{
		CMouseCursor::Instance()->SetActive(false);

		if (!IS_KINDOF(CLobbyTask, AfxGetTask()))
		{
			CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc",
				false);
		}

		PostMsg(this, "Lobby", 0, (int)"LOGIN", 0, 0, 0);
	}

	m_pWaitDlg = NULL;
	return true;
}

bool CTaskMain::OnServerDlgResult(int result, FrForm* pForm)
{
	m_pServerDlg = NULL;
	if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()) && Doc()->m_bTutorialResume)
		GetActor("Lobby") << MsgObject(NULL, 0x56, 0, 0, 0, 0, 0);
	return true;
}

bool CTaskMain::OnQuitDlgResult(int result, FrForm* pForm)
{
	m_pQuitDlg = NULL;
	return true;
}

bool CTaskMain::OnOptionDlgResult(int result, FrForm* pForm)
{
	m_pOptionDlg = NULL;
	return true;
}

bool CTaskMain::OnNotifyDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK && IsLocalContent((localContentType_t)0x17) &&
		!Doc()->IsHaveHalloweenItem())
	{
		CMouseCursor::Instance()->SetActive(false);
		CTaskManager::Instance()->ChangeTask("CShopTask", "",
			IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) ? true : false);
		AfxPostMsg(NULL, "Shop", 0, (int)"CHARSHOP", 0, 0, 0);
	}

	Doc()->m_sendGiftList.clear();
	return true;
}

bool CTaskMain::OnNotifyOneDlgResult(int result, FrForm* pForm)
{
	m_pNotifyOneDlg = NULL;
	return true;
}

bool CTaskMain::OnRecycleDlgResult(int result, FrForm* pForm)
{
	m_pRecycleDlg = NULL;

	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		WSingleton<ntAvatarManager>::Instance()->SetActive(true);

	return true;
}

bool CTaskMain::OnTikiMagicBoxDlgResult(int result, FrForm* pForm)
{
	m_pTikiMagicBoxDlg = NULL;

	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		WSingleton<ntAvatarManager>::Instance()->SetActive(true);

	return true;
}

bool CTaskMain::OnBongdariDlgResult(int result, FrForm* pForm)
{
	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		WSingleton<ntAvatarManager>::Instance()->SetActive(true);

	if (IS_KINDOF(CRealMyRoomTask, AfxGetTask()))
	{
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
	}

	return true;
}

bool CTaskMain::OnScratchDlgResult(int result, FrForm* pForm)
{
	m_pScratchDlg = NULL;

	if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
		WSingleton<ntAvatarManager>::Instance()->SetActive(true);

	return true;
}

bool CTaskMain::OnLoginRsDlgResult(int result, FrForm* pForm)
{
	m_pLoginRsDlg = NULL;
	return true;
}

bool CTaskMain::OnMaketListDlgResult(int result, FrForm* pForm)
{
	m_pMaketDlg = NULL;
	return true;
}

bool CTaskMain::OnOnelineReqDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		const char* line = ((FrOnelineReqDlg*)pForm)->GetInputString();

		if (Doc()->m_chatManager.Filtering((char*)line))
		{
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pNotify->SetMessage(
				"\xc0\xfb\xb4\xe7\xc7\xcf\xc1\xf6\xbe\xca\xc0\xba "
				"\xb4\xdc\xbe\xee\xb0\xa1 "
				"\xc6\xf7\xc7\xd4\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf"
				"\xb4\xd9.",
				false);
			pNotify->Open(NULL, 3);
			m_pOnelineReqDlg = NULL;
			return true;
		}

		if (*line == '\0')
		{
			FrForm* pNotify = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pNotify->SetMessage("\xba\xb8\xb3\xbe \xb3\xbb\xbf\xeb\xc0\xcc "
								"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pNotify->Open(NULL, 3);
			m_pOnelineReqDlg = NULL;
			return true;
		}

		WSingleton<COneLineBoard>::Instance()->SetMessage(line);

		WSendPacket send((enumClientPacket)0x67);
		send.Send(TO_GAME);
	}

	m_pOnelineReqDlg = NULL;
	return true;
}

bool CTaskMain::OnNoticeDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		unsigned long typeId = m_pNoticeDlg->GetTypeId();
		switch (typeId >> 26)
		{
		case PIG_CHAR:
			Doc()->m_shopName = "CHARSHOP";
			break;

		case PIG_PART:
			Doc()->m_shopName = "CHARSHOP";
			break;

		case PIG_SETITEM:
			Doc()->m_shopName = "CHARSHOP";
			break;

		case PIG_CLUBSET:
			Doc()->m_shopName = "ITEMSHOP_CLUBSET";
			break;

		case PIG_BALL:
			Doc()->m_shopName = "ITEMSHOP_BALL";
			break;

		case PIG_ITEM:
			if (typeId & 0x2000000)
				Doc()->m_shopName = "ITEMSHOP_NOEQUIP";
			else
				Doc()->m_shopName = "ITEMSHOP_EQUIP";
			break;

		case PIG_CADDIE:
			Doc()->m_shopName = "CADDIESHOP";
			break;

		case PIG_SKIN:
			Doc()->m_shopName = "SKINSHOP";
			break;

		case PIG_QUEST:
			break;

		default:
			Doc()->m_shopName = "";
			break;
		}

		Doc()->m_shopName = "";

		if (Doc()->m_underBarMask == 0x1f && Doc()->m_shopName != "")
		{
			HandleMsg(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			WSendPacket send((enumClientPacket)0x3d);
			send.Send(TO_GAME);
		}

		if (IsLocalContent(S4_UCC) && m_pNoticeDlg->GetTypeId() == 100)
		{
			CTaskManager::Instance()->ChangeTask("CShopTask", "", true);
			AfxPostMsg(NULL, "Shop", 0, (int)"SHOPMAIN", 0, 0, 0);
			AfxPostMsg(NULL, "Shop", 0xc6, 3,
				UccManager()->GetWinnerCharacterTypeId(), 0, 0);
		}

		if (IsLocalCountry(1) && m_pNoticeDlg->GetTypeId() == 101)
			ShellExecuteA(NULL, NULL, "http://qa.pangya.gametree.co.kr/", NULL,
				NULL, SW_SHOWMAXIMIZED);

		m_pNoticeDlg = NULL;
		return true;
	}

	m_pNoticeDlg = NULL;
	return true;
}

bool CTaskMain::OnKbdManualDlgResult(int result, FrForm* pForm)
{
	m_pKbdManualDlg = NULL;
	return true;
}

bool CTaskMain::OnLevelupFormResult(int result, FrForm* pForm)
{
	if (!m_levelupForms.empty())
	{
		m_levelupForms[m_levelupForms.size() - 1] = NULL;
		m_levelupForms.erase(m_levelupForms.end() - 1);
	}

	return true;
}

bool CTaskMain::OnOnelineReqChkOutDlgResult(int result, FrForm* pForm)
{
	if (result == FrOK)
	{
		WSendPacket send((enumClientPacket)0x66);
		send.EncodeStr(WSingleton<COneLineBoard>::Instance()->GetMessage());
		send.Send(TO_GAME);
	}

	return true;
}

bool CTaskMain::OnSpecialCardFormResult(int result, FrForm* pForm)
{
	WSingleton<CCardManager>::Instance()->SetCardBuffDlg(NULL);
	m_pSpecialCardDlg = NULL;

	return true;
}

float CTaskMain::Print(float x, float y, const char* str, float gap, int align,
	float scale, unsigned long color)
{
	float startX = x;

	if (str == NULL)
		return 0.0f;

	if (strlen(str) == 0)
		return 0.0f;

	const char* charSet = m_pTitleFont->GetFontInfo()->pCharSet;
	float oldScale = m_pTitleFont->GetScale();
	m_pTitleFont->SetScale(scale);

	float width = 0.0f;
	const char* p = str;
	switch (align)
	{
	case 1:
		while (*p)
		{
			int c = *p++;
			const char* found = strchr(charSet, c);
			width += m_pTitleFont->GetCharWidth(g_view, found - charSet) +
				gap * scale;
		}
		width = (width - gap) * 0.5f;
		break;

	case 2:
		while (*p)
		{
			int c = *p++;
			const char* found = strchr(charSet, c);
			width += m_pTitleFont->GetCharWidth(g_view, found - charSet) +
				gap * scale;
		}
		width -= gap;
		break;
	}

	x -= width;

	for (p = str; *p;)
	{
		int c = *p++;
		const char* found = strchr(charSet, c);
		float w =
			m_pTitleFont->PutChar(g_view, x, y, found - charSet, 0, color);
		w += gap;
		x += w;
	}

	m_pTitleFont->SetScale(oldScale);
	m_pTitleFont->Flush(g_view);

	switch (align)
	{
	case 0:
		startX = x - startX;
		break;
	case 1:
	case 2:
		startX = width;
		break;
	}

	return startX;
}

const char* CTaskMain::MakeStatusString(int type, int value, int slot)
{
	const char* names[5] = { K2L_Compatibility("\xc6\xc4\xbf\xf6"),
		K2L_Compatibility("\xc4\xc1\xc6\xae\xb7\xd1"),
		K2L_Compatibility("\xc1\xa4\xc8\xae\xb5\xb5"),
		K2L_Compatibility("\xbd\xba\xc7\xc9"),
		K2L_Compatibility("\xc4\xbf\xba\xea") };

	if (value)
	{
		if (slot)
			return MakeStr("%s+%d/%s+%d", names[type], value,
				"\xbd\xbd\xb7\xd4", slot);
		return MakeStr("%s+%d", names[type], value);
	}

	if (slot)
		return MakeStr("%s%s+%d", names[type], "\xbd\xbd\xb7\xd4", slot);

	return "";
}

const char* CTaskMain::MakeStatusString2(int cur, int max, int minus)
{
	if (cur)
	{
		if (max)
		{
			if (minus)
				return MakeStr("%d(-%d)/%d", cur, minus, max);
			return MakeStr("%d/%d", cur, max);
		}
		return MakeStr("%d", cur);
	}

	if (max)
		return MakeStr("%d", max);

	return "";
}

const char* CTaskMain::MakeStatusString3(int value, int max, int minus)
{
	if (minus)
		return MakeStr("%d(-%d)/%d", value, minus, max);

	return MakeStr("%d/%d", value, max);
}

const char* CTaskMain::MakeCardStatusString(int type, int value)
{
	const char* names[5] = { K2L_Compatibility(
								 "\xc6\xc4\xbf\xf6\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc4\xc1\xc6\xae\xb7\xd1\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc1\xa4\xc8\xae\xb5\xb5\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xbd\xba\xc7\xc9\xbd\xbd\xb7\xd4"),
		K2L_Compatibility("\xc4\xbf\xba\xea\xbd\xbd\xb7\xd4") };

	if (value)
		return MakeStr("%s+%d", names[type], value);

	return "";
}

const char* randomBGMList[] = { "frog.mp3", "daydream.mp3" };

static int s_bgmMode = 0;

void CTaskMain::SetBGMNone()
{
	if (s_bgmMode == 0)
		return;

	g_audio->StopBGM(true);

	s_bgmMode = 0;
}

void CTaskMain::SetRandomBGM()
{
	if (s_bgmMode == 1)
		return;

	g_audio->SetFadeOutTime(1.0f);

	g_audio->PlayBGM(randomBGMList[rand() % 2], false, true);
	s_bgmMode = 1;
}

void CTaskMain::SetTimeVariableBGM()
{
	if (s_bgmMode == 2)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);

	g_audio->SetFadeOutTime(1.0f);

	if (time.wHour == 22 || time.wHour == 23)
		g_audio->PlayBGM("tea_time_vibra.mp3", false, true);
	else
		g_audio->PlayBGM("tea_time_piano.mp3", false, true);

	s_bgmMode = 2;
}

void CTaskMain::SetConstantBGM(const char* name, bool bLoop, bool bForce)
{
	if (s_bgmMode == 3 && !bForce)
		return;

	g_audio->SetFadeOutTime(1.0f);
	g_audio->PlayBGM(name, bLoop, true);
	s_bgmMode = 3;
}

void CTaskMain::TurnOn3DBackground()
{
	SetWeather(0);

	if (WSingleton<CSceneManager>::IsInstantiated())
		WSingleton<CSceneManager>::Instance()->SetLobbyWeather(false);

	Doc()->m_bBackgroundVideo = false;
}

void CTaskMain::TurnOff3DBackground()
{
	Doc()->m_bBackgroundVideo = true;
}

void CTaskMain::CheckNaviButtons()
{
	if (m_pUnderBarBack)
		m_pUnderBarBack->Enable(
			CTaskManager::Instance()->IsNaviBackwardEnable());

	if (m_pUnderBarFront)
		m_pUnderBarFront->Enable(
			CTaskManager::Instance()->IsNaviForwardEnable());
}

void CTaskMain::HandleMsgCommon(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x270:
		CInitDelegate::ExecuteInitFunctions();
		break;

	case 0x271:
		CInitDelegate::ExecuteReleaseFunctions();
		break;

	case 0x26b:
	{
		void* pData = (void*)msg.param1;
		AfxGetTask()->PushMsg(pData);
	}
	break;

	case 1:
		m_fKbdManualTime = 0.0f;
		break;

	case 2:
		m_fKbdManualTime = -30.0f;
		if (m_pWaitDlg)
		{
			m_pWaitDlg->Close(FrOK, true);
			m_pWaitDlg = NULL;
		}
		break;

	case 0xec:
		OpenWaitedLayout();
		break;

	case 0x19:
		if (m_pServerDlg)
			m_pServerDlg->BuildServerList();
		break;

	case 0x1b:
		if (m_pServerDlg)
			m_pServerDlg->BuildChannelList();
		break;

	case 0x23:
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		pForm->SetMessage((const char*)msg.param1, false);
		pForm->Open(NULL, 3);
		pForm->SetTopmost(true);
		pForm->FindNextTopFocus(false);
	}
	break;

	case 0x24:
		if (m_pNotifyOneDlg == NULL)
		{
			m_pNotifyOneDlg =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			m_pNotifyOneDlg->SetMessage((const char*)msg.param1, false);
			m_pNotifyOneDlg->Open(
				(FRESH_PFN_RESULT)&CTaskMain::OnNotifyOneDlgResult, 3);
		}
		break;

	case 0x26:
	{
		unsigned long typeId = msg.param2;
		IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_item");

		if (pForm)
		{
			FrArea* pIcon =
				DYNAMIC_CAST(FrArea, pForm->FindChildByName("itemicon"));
			FrEdit* pName =
				DYNAMIC_CAST(FrEdit, pForm->FindChildByName("itemname"));

			if (pIcon)
			{
				if (pItem)
				{
					pIcon->SetBgImg(pItem->Icon);
				}
				else if (typeId)
				{
					WRect rect = pIcon->GetRect();
					pIcon->SetRect(
						WRect(rect.x + 15.0f, rect.y + 7.0f, rect.w, rect.h));
					pIcon->SetBgImg("item1_16");
				}
				else
				{
					pIcon->SetBgImg("hide");
				}
			}

			if (pName)
			{
				if (pItem)
					pName->AddText(pItem->Name, false, true);
				else if (typeId)
					pName->AddText(MakeStr("%d\xc6\xce", typeId), false, true);
				else
					pName->AddText("none", false, true);

				pName->AddText("\n", false, true);
			}

			pForm->SetMessage((const char*)msg.param1, false);
			pForm->Open(NULL, 2);
		}
	}
	break;

	case 0x48:
		if (m_pGiftDlg)
			m_pGiftDlg->UpdateFriendList();
		break;

	case 0x50:
		if (m_pGiftDlg)
			m_pGiftDlg->SetReceiverInfo(msg.param1,
				*(sPangYaUserInfo*)msg.param2);

		if (CUserInfo::Instance()->IsVisible() == true)
		{
			FrUserInfoForm* pForm = USERINFODLG();
			if (pForm)
			{
				if (pForm->IsOpenAddFriendDlg())
				{
					sPangYaUserInfo* pInfo = (sPangYaUserInfo*)msg.param2;
					pForm->SetRequestFriendData(pInfo->sNick, pInfo->dwUID);
				}
			}
		}
		break;

	case 0x51:
	{
		FrUserInfoForm* pForm = USERINFODLG();
		if (pForm)
		{
			if (CUserInfo::Instance()->IsVisible() == true)
			{
				if (pForm->IsOpenAddFriendDlg())
					pForm->SetRequestFriendData((const char*)msg.param1,
						msg.param2);
			}
		}
	}

	case 0x5c:
	{
		FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			"caddie_info");
		FrCaddieInfoDlg* pDlg =
			pWnd ? (FrCaddieInfoDlg*)pWnd : DYNAMIC_CAST(FrCaddieInfoDlg, pWnd);
		if (pDlg)
			pDlg->EndContract(msg.param1);
	}
	break;

	case 0xa8:
		if (m_pMessengerBubbleEdit)
		{
			if (msg.param2)
			{
				const char* text = (const char*)msg.param1;
				if (msg.param3)
				{
					MESSENGER()->Notify2(text);

					WPoint pos = *(WPoint*)msg.param3;
					m_pMessengerBubbleEdit->MoveWindow(pos);
				}
				else
				{
					WPoint pos;
					pos.x = MESSENGER()->m_alarmRect.x - 86.0f;
					pos.y = m_pMessengerBubbleEdit->GetRect().y;
					m_pMessengerBubbleEdit->MoveWindow(pos);

					m_pMessengerBubbleEdit->ClearLine();
					m_pMessengerBubbleEdit->AddText(text, false, true);
					m_pMessengerBubbleEdit->SetVisible(true);
				}
			}
			else
			{
				m_pMessengerBubbleEdit->SetVisible(false);
			}
		}
		break;

	case 0xae:
		MESSENGER()->Close(true);
		break;

	case 0xa9:
		if (m_pMessengerAlarmBtn)
		{
			if (msg.param1)
				m_pMessengerAlarmBtn->SetVisible(true);
			else
				m_pMessengerAlarmBtn->SetVisible(false);
		}
		break;

	case 0xac:
		if (m_pOverBarMessenger)
		{
			if (msg.param1)
				m_pOverBarMessenger->EnableBlink(true);
			else
				m_pOverBarMessenger->EnableBlink(false);
		}
		break;

	case 0xaa:
		if (m_pMessengerGiftAlarmBtn)
		{
			if (msg.param1)
			{
				if (!(IsLocalContent(S3_TUTORIAL_RENEWAL) &&
						AfxGetPreservedTask() &&
						IS_EXACTKINDOF(CTutorialTask, AfxGetPreservedTask())))
					m_pMessengerGiftAlarmBtn->SetVisible(true);
			}
			else
			{
				m_pMessengerGiftAlarmBtn->SetVisible(false);
			}
		}
		break;

	case 0x6c:
		if (m_pTreasureAlarm)
		{
			if (msg.param1 == 1)
				m_pTreasureAlarm->SetVisible(true);
			else
				m_pTreasureAlarm->SetVisible(false);

			if (msg.param1 == 1)
			{
				m_pTreasureAlarm->Enable(true);
				m_pTreasureAlarm->SetVisible(true);
			}
			else
			{
				m_pTreasureAlarm->SetVisible(false);
				m_pTreasureAlarm->Enable(false);
			}
		}
		break;

	case 0x1fa:
		for (int i = 0; i < 6; i++)
		{
			unsigned long typeId = Doc()->m_myInfo.userEquip.tidSkin[i];

			if ((typeId & 0xfc000000) == 0x38000000)
			{
				std::list<sItemInfo>::iterator it;
				for (it = Doc()->m_cutinList.begin();
					it != Doc()->m_cutinList.end(); ++it)
				{
					if ((*it).tid == typeId)
					{
						(*it).ItemType = 2;
						break;
					}
				}
			}
		}
		break;

	case 0x1fb:
		OpenInformation(msg.param1);
		break;

	case 0xad:
	{
		unsigned long guid = msg.param1;
		unsigned long typeId = 0;
		std::list<sItemInfo>::iterator it;
		for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
			++it)
		{
			if ((*it).guid == guid)
			{
				typeId = (*it).tid;
				break;
			}
		}

		if (it != Doc()->m_myItemList.end())
		{
			if ((*it).ItemType)
			{
				if (ItemManager()->GetCouponKind((*it).tid) == 3)
				{
					(*it).Common[0]--;
					if ((*it).Common[0] <= 0)
						Doc()->m_myItemList.erase(it);
				}
				else if (ItemManager()->GetCouponKind((*it).tid))
				{
					Doc()->m_myItemList.erase(it);
				}
				else
				{
					if ((*it).HourRemain <= 0)
						Doc()->m_myItemList.erase(it);
				}
			}
			else
			{
				(*it).Common[0]--;
				if ((*it).Common[0] <= 0)
					Doc()->m_myItemList.erase(it);
			}
		}

		if (IsLocalContent(S3_BONGDARISHOP))
		{
			if (typeId && ItemManager()->GetCouponKind(typeId) == 2)
			{
				FrWnd* pWnd =
					g_pFresh->GetManager()->GetDesktop()->FindChildByName(
						"bongdari_shop");
				FrBongdariShopDlg* pDlg = pWnd
					? (FrBongdariShopDlg*)pWnd
					: DYNAMIC_CAST(FrBongdariShopDlg, pWnd);
				if (pDlg)
				{
					pDlg->InitCouponBongdariShop();
					pDlg->SetDataFee(1);
				}

				IActor* pActor = AfxGetTask()->GetActor("RealMyRoom");
				if (pActor)
					pActor << MsgObject(NULL, 0xe1, 0, 0, 0, 0, 0);
			}
		}
	}
	break;

	case 0xaf:
		m_bBongdariShop2 = msg.param1 != 0;
		break;

	case 0x5d:
		COneLineBoard::Instance()->SetWaitMsgNum(msg.param1);
		COneLineBoard::Instance()->SetWaitMsgTime(msg.param2);

		if (msg.param1 >= 1024)
		{
			this << MsgObject(this, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xbf\xb9\xbe\xe0\xb5\xc8 "
					 "\xb8\xde\xbd\xc3\xc1\xf6\xb0\xa1 \xb3\xca\xb9\xab "
					 "\xb8\xb9\xbd\xc0\xb4\xcf\xb4\xd9. "
					 "\n\xc0\xe1\xbd\xc3\xc8\xc4\xbf\xa1 \xb4\xd9\xbd\xc3 "
					 "\xc7\xd8\xc1\xd6\xbd\xca\xbd\xc3\xbf\xc0.",
				0, 0, 0, 0);
		}
		else
		{
			FrOnelineReqChkOutDlg* pDlg = CreateForm<FrOnelineReqChkOutDlg>(
				g_pFresh->GetManager(), this, "oneline_chkout");
			if (pDlg)
				pDlg->Open(
					(FRESH_PFN_RESULT)&CTaskMain::OnOnelineReqChkOutDlgResult,
					0x11);
		}
		break;

	case 0xea:
		OnUnderBar_ServerUp();
		break;

	case 0xeb:
		UpdateSiteInfo();
		break;

	case 0x1c:
		this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);

		Doc()->m_curChannel = Doc()->m_selChannel;
		Doc()->m_ignoreList.clear();
		UpdateSiteInfo();

		if (g_pFresh->IsCurrentLayout("SERVERLIST"))
		{
			AfxPostMsg(NULL, "Lobby", 0, (int)"TOPPAGE", 0, 0, 0);
			AfxPostMsg(NULL, "Lobby", 0x35, 0, 0, 0, 0);
		}
		break;

	case 0x20:
		if (m_pServerDlg)
		{
			m_pServerDlg->Close(FrOK, true);

			m_pServerDlg = NULL;
		}
		break;

	case 0x22:
		if (m_pServerDlg)
			m_pServerDlg->SetLoginMessageAlert((const char*)msg.param1);
		break;

	case 0x200:
	{
		CloseAllDialogs();

		FrBongdariShopDlg* pDlg = CreateForm<FrBongdariShopDlg>(
			g_pFresh->GetManager(), this, "bongdari_shop");
		if (pDlg)
		{
			pDlg->Open((FRESH_PFN_RESULT)&CTaskMain::OnBongdariDlgResult, 0x41);

			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
				ntAvatarManager::Instance()->SetActive(false);
		}
	}
	break;

	case 0x12d:
	{
		FrLevelupItemForm* pForm = CreateForm<FrLevelupItemForm>(
			g_pFresh->GetManager(), this, "levelupitem");

		if (pForm)
		{
			pForm->SetData(*(sLevelUpDone*)msg.param1);
			pForm->Open((FRESH_PFN_RESULT)&CTaskMain::OnLevelupFormResult, 3);

			m_levelupForms.push_back(pForm);
		}
	}
	break;

	case 0x216:
	{
		if (Doc()->GetNowGoWithMode() == 1)
		{
			int uid = Doc()->GetGoWithFriendUID();
			if (uid == -1)
				return;

			sFriend* pFriend = Buddy()->GetBuddy(uid);
			if (pFriend == NULL)
				return;

			Doc()->SetDirectMoveRoomInfo(pFriend->userPosition.iChannelUid,
				pFriend->userPosition.iRoomIdx,
				pFriend->userPosition.iServerGUID);
		}

		sGameServerInfo info;
		unsigned long serverUID = 0xffffffff;
		unsigned char channel = 0xff;
		unsigned short room = 0xffff;
		Doc()->GetDirectMoveRoomInfo(&channel, &room, &serverUID);

		for (GAMESERVER_LIST_IT it = Doc()->m_gameServerList.begin();
			it != Doc()->m_gameServerList.end(); ++it)
		{
			info = *it;
			if (info.guid == serverUID)
				break;
		}

		if (info.maxUser == 0)
			return;

		int mode = Doc()->GetNowGoWithMode();
		if (mode == 1)
		{
			if (channel == 0xff)
				return;
		}
		else if (mode == 2 || mode == 3)
		{
			if (channel == 0xff || room == 0xffff)
				return;
		}

		Doc()->m_gameServerAddr = info.addr;
		Doc()->m_gameServerPort = info.port;
		Doc()->m_gameServerUID = info.guid;

		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
			WNetworkSystem::Instance()->ForceShutDown(WNetworkSystem::NET_GAME);

		WNetworkSystem::Instance()->SendMessage(WNetworkSystem::NET_GAME,
			(NetworkUnit::eEvent)0);

		Doc()->m_curGameServer = info;
		Doc()->SetDirectMoveRoomInfo(channel, room, serverUID);
	}
	break;

	case 0x222:
		if (IsLocalContent(S4_REAL_MYROOM))
			MakeRealMyRoom();
		break;

	case 0x21f:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			if (Doc()->m_rmrAuthority.ownerUID == MyUID() ||
				!Doc()->m_rmrAuthority.bPrivate)
			{
				ChangeRealMyRoom();
			}
			else
			{
				int state = 0;
				AfxGetTask()->SendMsgToMainActor(
					MsgObject(NULL, 0x231, (int)&state, 2, 3, 0, 0));

				if (state == 1)
				{
					CloseAllDialogs();
					if (m_pRMREnterSetting == NULL)
					{
						m_pRMREnterSetting =
							CreateForm<CRealMyRoom_EnterSetting>(
								g_pFresh->GetManager(), this, "enter_setting");
						FrForm::EnableTail(false);
						m_pRMREnterSetting->Open(
							(FRESH_PFN_RESULT)&CTaskMain::OnRMR_ResultSetting,
							0x81);
						m_pRMREnterSetting->SetFadeout(false);
					}
				}
				else
				{
					this << MsgObject(this, 0x23,
						(int)"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 "
							 "\xb4\xeb\xb1\xe2\xb9\xe6\xbf\xa1 \xc0\xd6\xbe\xee "
							 "\xb4\xd9\xb8\xa5 \xc0\xaf\xc0\xfa\xc0\xc7 "
							 "\xb9\xe6\xbf\xa1 \xb9\xe6\xb9\xae\xc7\xd2 \xbc\xf6 "
							 "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
				}
			}
		}
		break;

	case 0x228:
	{
		int type = msg.param1;
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");

		switch (type)
		{
		case 0x122:
			if (msg.param2)
				pForm->SetMessage("\xb8\xb6\xc0\xcc\xb7\xeb\xc0\xc7 "
								  "\xb1\xc7\xc7\xd1\xc0\xbb "
								  "\xbc\xb3\xc1\xa4\xc7\xcf\xbf\xb4\xbd\xc0"
								  "\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage("\xb8\xb6\xc0\xcc\xb7\xeb\xc0\xc7 "
								  "\xb1\xc7\xc7\xd1\xc0\xbb "
								  "\xbc\xb3\xc1\xa4\xc7\xcf\xb4\xc2\xb5\xa5 "
								  "\xbd\xc7\xc6\xd0\xc7\xcf\xbf\xb4\xbd\xc0"
								  "\xb4\xcf\xb4\xd9.",
					false);
			break;

		case 0x124:
			if (msg.param2)
				pForm->SetMessage("\xc7\xf6\xc0\xe7 \xb9\xe8\xc4\xa1\xb8\xa6 "
								  "\xc0\xfa\xc0\xe5\xc7\xcf\xbf\xb4\xbd\xc0"
								  "\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage("\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 "
								  "\xb0\xa1\xb1\xb8\xb8\xa6 \xb3\xf5\xc1\xf6 "
								  "\xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.. ",
					false);
			break;

		case 0x125:
			pForm->SetMessage(
				"\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 \xc0\xd6\xb4\xc2 "
				"\xb0\xa1\xb1\xb8 \xc1\xa4\xba\xb8\xb8\xa6 "
				"\xb0\xa1\xc1\xae\xbf\xc0\xc1\xf6 "
				"\xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x123:
			pForm->SetMessage("\xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1 "
							  "\xb5\xe9\xbe\xee\xb0\xa5 \xbc\xf6 "
							  "\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x141:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xc7\xd4\xc0\xc7 "
				"\xc6\xed\xc1\xf6\xb5\xe9\xc0\xbb "
				"\xc0\xd0\xbe\xee\xbf\xc0\xc1\xf6 "
				"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x143:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xc0\xc7 "
				"\xb3\xbb\xbf\xeb\xc0\xbb "
				"\xc0\xd0\xbe\xee\xbf\xc0\xc1\xf6 "
				"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x145:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xbf\xa1 "
				"\xc3\xb7\xba\xce\xb5\xc8 "
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
				"\xbf\xc5\xb1\xe2\xc1\xf6 "
				"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x15d:
			pForm->SetMessage(
				"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb "
				"\xc1\xf6\xbf\xec\xc1\xf6 "
				"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			break;

		case 0x13f:
		{
			int result = msg.param2;

			if (result == 1)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc6\xed\xc1\xf6 "
					"\xbc\xf6\xbd\xc5\xc0\xbb "
					"\xb0\xc5\xba\xce\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 2)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe "
					"\xc0\xaf\xc0\xfa\xb8\xa6 \xc3\xa3\xc0\xbb \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 3)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc "
					"\xbc\xd2\xc0\xaf\xc7\xd1 "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 4)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbc\xd2\xc0\xaf\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb "
					"\xbc\xf6\xb7\xae\xba\xb8\xb4\xd9 \xb4\xf5 "
					"\xb8\xb9\xc0\xba \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
					"\xba\xb8\xb3\xbb\xb7\xc1\xb0\xed "
					"\xbd\xc3\xb5\xb5\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9..",
					false);
			else if (result == 5)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbc\xd2\xc0\xaf\xc7\xcf\xc1\xf6 \xbe\xca\xc0\xba "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb "
					"\xba\xb8\xb3\xbb\xb7\xc1\xb0\xed "
					"\xbd\xc3\xb5\xb5\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 6)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xcc \xc0\xcc\xb9\xcc "
					"\xc6\xed\xc1\xf6\xb7\xce \xb9\xde\xc0\xba "
					"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 7)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc3\xb7\xba\xce\xc7\xd2 "
					"\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 8)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xc6\xed\xc1\xf6 \xb9\xdf\xbc\xdb\xc0\xbb \xc7\xd2 \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 9)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 "
					"\xbf\xec\xc6\xed\xc7\xd4\xc0\xcc \xb0\xa1\xb5\xe6 "
					"\xc2\xf7\xbc\xad \xc7\xf6\xc0\xe7\xb4\xc2 "
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xb9\xde\xc0\xbb \xbc\xf6 "
					"\xbe\xf8\xb4\xc2 \xbb\xf3\xc5\xc2\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 10)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xc7\xd4 \xb1\xe2\xb4\xc9\xc0\xc7 "
					"\xba\xce\xba\xd0 \xc1\xa1\xb0\xcb\xc1\xdf\xc0\xd4\xb4\xcf"
					"\xb4\xd9.",
					false);
			else if (result == 11)
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb \xba\xb8\xb3\xbe \xbc\xf6 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. "
					"\n\xbb\xf3\xb4\xeb\xb9\xe6\xc0\xc7 \xc4\xb3\xb5\xf0\xb0\xa1 "
					"\xc8\xde\xb0\xa1\xc1\xdf\xc0\xcc\xb0\xc5\xb3\xaa, "
					"\xba\xb8\xc0\xaf\xc7\xd1 \xc4\xb3\xb5\xf0\xb0\xa1 "
					"\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else if (result == 20)
				pForm->SetMessage(
					"\xc3\xb3\xb8\xae\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9. "
					"\xc0\xe1\xbd\xc3\xb8\xb8 "
					"\xb1\xe2\xb4\xd9\xb8\xae\xbc\xbc\xbf\xe4.",
					false);
			else if (result == 22)
				pForm->SetMessage(
					"\xb8\xde\xc0\xcf \xc3\xb7\xba\xce \xb0\xa1\xb4\xc9\xc7\xd1 "
					"\xc3\xd6\xb4\xeb \xb0\xb3\xbc\xf6\xb8\xa6 "
					"\xc3\xca\xb0\xfa\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
			else
				pForm->SetMessage(
					"\xbf\xec\xc6\xed\xb9\xb0\xc0\xbb "
					"\xba\xb8\xb3\xbb\xc1\xf6 "
					"\xb8\xf8\xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
		}
		break;
		}

		pForm->Open(NULL, 0);
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 2, 0, 0, 0, 0, 0);
	}
	break;

	case 0x231:
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			unsigned char mode = (unsigned char)msg.param2;
			eBarButton button = (eBarButton)msg.param3;

			bool state = GetBtnState(mode, button);

			*(int*)msg.param1 = state;
		}
		else
		{
			*(bool*)msg.param3 = false;
		}
		break;

	case 0x252:
		if (IsLocalContent((localContentType_t)0x8e))
		{
			std::string layout = "BirthdayEventMissionSkin1";
			if (IsLocalContent((localContentType_t)0x9b))
				layout = "BirthdayEventMissionSkin2";

			for (int i = 0; i < 5; i++)
			{
				if (m_pMissionDlg[i] == NULL)
				{
					m_pMissionDlg[i] = CreateForm<FrBirthdayEventMissionDlg>(
						g_pFresh->GetManager(), this, layout.c_str());
					if (m_pMissionDlg[i])
					{
						unsigned long* pMission = (unsigned long*)msg.param1;
						m_pMissionDlg[i]->SetCompletedMissionInfo(*pMission);

						for (int j = 0; j <= i; j++)
						{
							if (m_pMissionDlg[j])
								m_pMissionDlg[j]->CalculatePos(j, i + 1);
						}

						m_pMissionDlg[i]->Open((FRESH_PFN_RESULT)&CTaskMain::
												   CompletedMission_Result,
							0);
						m_pMissionDlg[i]->m_nFlags.Disable(FWF_FADING_EX);
						m_pMissionDlg[i]->SetFadeout(false);
						m_pMissionDlg[i]->SetFadeOutFlag(true);
						break;
					}
				}
			}
		}
		break;

	case 0x272:
		NotifyServerControlMsg(msg.param1);
		break;
	}
}

bool CTaskMain::CompletedMission_Result(int result, FrForm* pForm)
{
	for (int i = 0; i < 5; i++)
	{
		if (m_pMissionDlg[i] == pForm)
		{
			m_pMissionDlg[i] = NULL;
			break;
		}
	}
	return true;
}

void CTaskMain::SendRoomExitPacket()
{
	WSendPacket send(15);
	send.Encode1(0);
	send.Encode2(0xffff);
	send.Encode8(0);
	send.Encode8(0);
	send.Send(TO_GAME);
	Doc()->m_myInfo.roomIndex = 0xffff;
	Doc()->m_roomInfo.roomGuid = 0xffff;
	this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
}

void CTaskMain::SendRoomListExitPacket()
{
	WSendPacket send(130);
	send.Send(TO_GAME);
	this << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
}

void CTaskMain::OpenWaitedLayout()
{
	if (!IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
		CTaskManager::Instance()->ChangeTask("CLobbyTask", "CGolfDoc", false);
	AfxPostMsg(NULL, "Lobby", 0, (int)m_waitedLayout.c_str(), 0, 0, 0);
}

void CTaskMain::UpdateSiteInfo()
{
	FrStatic* pSite = m_pOverBarSite;
	if (pSite)
	{
		if (OfflinePlay())
			pSite->SetCaption("OFFLINE PLAY");
		else if (strlen(Doc()->m_curChannel.Name))
			m_pOverBarSite->SetCaption(MakeStr("%s > %s",
				Doc()->m_curGameServer.name, Doc()->m_curChannel.Name));
		else
			pSite->SetCaption(Doc()->m_curGameServer.name);
	}
}

void CTaskMain::OpenInformation(unsigned long typeId)
{
	FrWnd* pWnd =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (pWnd)
		return;

	__rtti_obj = NULL;
	FrForm* pForm =
		CreateForm<FrForm>(g_pFresh->GetManager(), this, "information", NULL);

	IFF_ITEM_COMMON* pItem = ItemManager()->FindCommonItem(typeId);

	FrArea* pPortrait =
		DYNAMIC_CAST(FrArea, pForm->FindChildByName("portrait"));
	if (pPortrait)
	{
		if (pItem)
			pPortrait->SetBgImg(pItem->Icon);
		else
			pPortrait->SetBgImg("hide");
	}

	if (pItem)
	{
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		FrArea* pLevel = DYNAMIC_CAST(FrArea, pForm->FindChildByName("level"));
		if (pName && pLevel)
		{
			if (pItem->IsUnderLvl)
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xc7\xcf",
						pItem->Name),
					false, true);
			}
			else
			{
				pLevel->SetBgImg(MakeStr("level_%03d", pItem->Level + 1));
				pName->AddText(
					MakeStr(
						"\xc0\xcc\xb8\xa7: %s\n\n\xb7\xb9\xba\xa7:              \xc0\xcc\xbb\xf3",
						pItem->Name),
					false, true);
			}
		}

		IFF_STRUCT::sDesc* pDesc = ItemManager()->FindDesc(pItem->TypeId);
		if (pDesc)
			pForm->SetMessage(pDesc->Desc, false);
	}
	else
	{
		FrEdit* pName = DYNAMIC_CAST(FrEdit, pForm->FindChildByName("name"));
		if (pName)
			pName->AddText(
				"\xc0\xcc\xb8\xa7: \xba\xd2\xb8\xed\n\n\xb7\xb9\xba\xa7: \xba\xd2\xb8\xed",
				false, true);
		pForm->SetMessage(
			"\xba\xa3\xc0\xcf\xbf\xa1 \xbd\xce\xc0\xce \xc1\xa4\xc3\xbc\xba\xd2\xb8\xed\xc0\xc7 \xc1\xb8\xc0\xe7",
			false);
	}

	pForm->Open(NULL, 3);
}

void CTaskMain::MakeRealMyRoom()
{
	if (!IsLocalContent((localContentType_t)0x56))
		return;

	if (Doc()->m_replayTargetUID != MyUID())
	{
		int result = 0;
		AfxGetTask()->SendMsgToMainActor(
			MsgObject(NULL, 0x231, (int)&result, 2, 3, 0, 0));
		if (!result)
		{
			this << MsgObject(this, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 \xb4\xeb\xb1\xe2\xb9\xe6\xbf\xa1 \xc0\xd6\xbe\xee \xb4\xd9\xb8\xa5 \xc0\xaf\xc0\xfa\xc0\xc7 \xb9\xe6\xbf\xa1 \xb9\xe6\xb9\xae\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			return;
		}
		else
		{
			WSendPacket send((enumClientPacket)0xad);
			send.Encode4(MyUID());
			send.Encode4(Doc()->m_replayTargetUID);
			send.Send(TO_GAME);
			return;
		}
	}

	if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
	{
		int result = 0;
		AfxGetTask()->SendMsgToMainActor(
			MsgObject(NULL, 0x231, (int)&result, 2, 3, 0, 0));
		if (!result && !IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()) &&
			!IS_EXACTKINDOF(ntAvatarChatTask,
				CTaskManager::Instance()->GetPreservedTask()))
		{
			if (Doc()->GetPlayingMode() == 3)
			{
				Doc()->SetPlayingMode((eReplayModeType)1);
				WSendPacket send((enumClientPacket)0xad);
				send.Encode4(MyUID());
				send.Encode4(Doc()->m_replayTargetUID);
				send.Send(TO_GAME);
			}
			else
			{
				this << MsgObject(this, 0x23,
					(int)"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 \xb4\xeb\xb1\xe2\xb9\xe6\xbf\xa1 \xc0\xd6\xbe\xee \xb4\xd9\xb8\xa5 \xc0\xaf\xc0\xfa\xc0\xc7 \xb9\xe6\xbf\xa1 \xb9\xe6\xb9\xae\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					0, 0, 0, 0);
				return;
			}
		}
		else
		{
			WSendPacket send((enumClientPacket)0xad);
			send.Encode4(MyUID());
			send.Encode4(Doc()->m_replayTargetUID);
			send.Send(TO_GAME);
		}
	}
	else
	{
		FrForm* pForm =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		pForm->SetMessage(
			"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae\xc1\xdf\xbf\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xb8\xa5 \xb9\xe6\xc0\xb8\xb7\xce \xc0\xcc\xb5\xbf\xc7\xcf\xb1\xe6 \xbf\xf8\xc7\xcf\xbd\xc3\xb8\xe9 \xb7\xce\xba\xf1\xb7\xce \xc0\xcc\xb5\xbf\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
			false);
		pForm->Open(NULL, 3);
		return;
	}

	switch (Doc()->GetRmrMode())
	{
	case 1:
		if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		{
			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", true);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				ntAvatarManager::Instance()->SetActive(false);
			}
			else
			{
				if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
				{
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", false);
				}
				else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
				{
					Doc()->m_bRebuildUnderBar = true;
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", false);
				}
				else if (g_pFresh->IsCurrentLayout("GHOST"))
				{
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", true);
				}
				else
				{
					if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
					{
						int param1, param2;
						SendMsg(NULL, "Shop", 0xc3, (int)&param1, (int)&param2,
							0, 0);
						AfxPostMsg(NULL, "RealMyRoom", 0xc4, param1, param2, 0,
							0);
					}
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc",
						CTaskManager::Instance()->GetPreservedTask() != NULL);
				}
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
			}
		}
		break;
	case 3:
		if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		{
			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", true);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
				ntAvatarManager::Instance()->SetActive(false);
			}
			else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
			{
				Doc()->m_bRebuildUnderBar = true;
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", false);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
			}
			else
			{
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc",
					CTaskManager::Instance()->GetPreservedTask() != NULL);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 0, 0, 0, 0);
			}
		}
		break;
	case 4:
		if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		{
			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", true);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 6, 0, 0, 0);
				ntAvatarManager::Instance()->SetActive(false);
			}
			else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
			{
				Doc()->m_bRebuildUnderBar = true;
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", false);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 6, 0, 0, 0);
			}
			else
			{
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc",
					CTaskManager::Instance()->GetPreservedTask() != NULL);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOM", 0, 0, 0);
				AfxPostMsg(NULL, "RealMyRoom", 0xc6, 6, 0, 0, 0);
			}
		}
		break;
	case 5:
		if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		{
			ChangeRealMyRoom();
		}
		else
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pForm->SetMessage(
				"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae\xc1\xdf\xbf\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xb8\xa5 \xb9\xe6\xc0\xb8\xb7\xce \xc0\xcc\xb5\xbf\xc7\xcf\xb1\xe6 \xbf\xf8\xc7\xcf\xbd\xc3\xb8\xe9 \xb7\xce\xba\xf1\xb7\xce \xc0\xcc\xb5\xbf\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
				false);
			pForm->Open(NULL, 3);
		}
		break;
	}

	if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
		return;

	AfxGetTask()->GetActor("RealMyRoom")
		<< MsgObject(NULL, 0x16, 1, 0, 0, 0, 0);
	AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
}

void CTaskMain::ChangeRealMyRoom()
{
	if (!IsLocalContent((localContentType_t)0x56))
		return;

	switch (Doc()->GetRmrMode())
	{
	case 1:
	{
		WSendPacket send((enumClientPacket)0xaf);
		send.Encode4(Doc()->m_replayTargetUID);
		send.Encode1(1);
		send.Send(TO_GAME);

		if (IsLocalContent((localContentType_t)0x66))
		{
			WSendPacket send((enumClientPacket)0xc4);
			send.Encode4(Doc()->m_replayTargetUID);
			send.Send(TO_GAME);
		}

		if (IsLocalContent((localContentType_t)0x59))
		{
			WSendPacket send((enumClientPacket)0xb9);
			send.Encode1(2);
			send.Send(TO_GAME);
		}

		Doc()->SetRmrMode(0);
	}
	break;
	case 3:
	{
		WSendPacket send((enumClientPacket)0xaf);
		send.Encode4(Doc()->m_replayTargetUID);
		send.Encode1(1);
		send.Send(TO_GAME);

		if (IsLocalContent((localContentType_t)0x59))
		{
			WSendPacket send((enumClientPacket)0xb9);
			send.Encode1(2);
			send.Send(TO_GAME);
		}

		Doc()->SetRmrMode(0);
	}
	break;
	case 4:
	{
		WSendPacket send((enumClientPacket)0xaf);
		send.Encode4(Doc()->m_replayTargetUID);
		send.Encode1(1);
		send.Send(TO_GAME);

		if (IsLocalContent((localContentType_t)0x59))
		{
			WSendPacket send((enumClientPacket)0xb9);
			send.Encode1(2);
			send.Send(TO_GAME);
		}

		Doc()->SetRmrMode(0);
	}
	break;
	case 5:
		if (Doc()->m_rmrAuthority.bPrivate == true &&
			m_pRMREnterSetting == NULL)
		{
			FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
				"notify", NULL);
			pForm->SetMessage(
				"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3 \xc8\xae\xc0\xce \xc3\xa2\xc0\xcc \xb4\xdd\xc7\xf4\xbc\xad \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb8\xa6 \xc8\xae\xc0\xce\xc7\xcf\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			pForm->Open(NULL, 3);
		}
		else if (Doc()->m_rmrAuthority.bPrivate == true)
		{
			std::string password;
			password = m_pRMREnterSetting->GetPassWord();

			if (strcmp(password.c_str(), Doc()->m_rmrAuthority.password))
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				pForm->SetMessage(
					"\xc0\xd4\xb7\xc2\xc7\xcf\xbd\xc5 \xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc6\xb2\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9. \xc8\xae\xc0\xce\xc7\xcf\xbd\xc3\xb0\xed \xb4\xd9\xbd\xc3 \xb9\xe6\xb9\xae\xc7\xcf\xbc\xbc\xbf\xe4.",
					false);
				pForm->Open(NULL, 3);
			}
			else
			{
				if (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
				{
					if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
					{
						CProjectG::Instance()->FromGolfToLobbySettings();
						CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
							"CGolfDoc", true);
						AfxPostMsg(NULL, "RealMyRoom", 0,
							(int)"REALMYROOMVISITOR", 0, 0, 0);
						ntAvatarManager::Instance()->SetActive(false);
					}
					else
					{
						if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
						{
							CTaskManager::Instance()->ChangeTask(
								"CRealMyRoomTask", "CGolfDoc", false);
						}
						else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
						{
							Doc()->m_bRebuildUnderBar = true;
							CTaskManager::Instance()->ChangeTask(
								"CRealMyRoomTask", "CGolfDoc", false);
						}
						else if (g_pFresh->IsCurrentLayout("GHOST"))
						{
							CTaskManager::Instance()->ChangeTask(
								"CRealMyRoomTask", "CGolfDoc", true);
						}
						else
						{
							if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
							{
								int param1, param2;
								SendMsg(NULL, "Shop", 0xc3, (int)&param1,
									(int)&param2, 0, 0);
								AfxPostMsg(NULL, "RealMyRoom", 0xc4, param1,
									param2, 0, 0);
							}
							CTaskManager::Instance()->ChangeTask(
								"CRealMyRoomTask", "CGolfDoc",
								CTaskManager::Instance()->GetPreservedTask() !=
									NULL);
						}
						AfxPostMsg(NULL, "RealMyRoom", 0,
							(int)"REALMYROOMVISITOR", 0, 0, 0);
					}
				}

				WSendPacket send((enumClientPacket)0xaf);
				send.Encode4(Doc()->m_replayTargetUID);
				send.Encode1(1);
				send.Send(TO_GAME);

				if (IsLocalContent((localContentType_t)0x66))
				{
					WSendPacket send((enumClientPacket)0xc4);
					send.Encode4(Doc()->m_replayTargetUID);
					send.Send(TO_GAME);
				}

				if (IsLocalContent((localContentType_t)0x59))
				{
					WSendPacket send((enumClientPacket)0xb9);
					send.Encode1(2);
					send.Send(TO_GAME);
				}

				Doc()->SetRmrMode(0);
			}
		}
		else
		{
			if (IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()))
			{
				FrForm* pForm = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				pForm->SetMessage(
					"\xc7\xf6\xc0\xe7 \xc0\xaf\xc0\xfa\xb0\xa1 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb9\xe6\xc0\xbb \xb9\xe6\xb9\xae\xc1\xdf\xbf\xa1 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xb8\xa5 \xb9\xe6\xc0\xb8\xb7\xce \xc0\xcc\xb5\xbf\xc7\xcf\xb1\xe6 \xbf\xf8\xc7\xcf\xbd\xc3\xb8\xe9 \xb7\xce\xba\xf1\xb7\xce \xc0\xcc\xb5\xbf\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4.",
					false);
				pForm->Open(NULL, 3);
				break;
			}

			if (IS_EXACTKINDOF(ntAvatarChatTask, AfxGetTask()))
			{
				CProjectG::Instance()->FromGolfToLobbySettings();
				CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
					"CGolfDoc", true);
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOMVISITOR", 0,
					0, 0);
				ntAvatarManager::Instance()->SetActive(false);
			}
			else
			{
				if (IS_EXACTKINDOF(CLobbyTask, AfxGetTask()))
				{
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", false);
				}
				else if (IS_EXACTKINDOF(CTutorialTask, AfxGetTask()))
				{
					Doc()->m_bRebuildUnderBar = true;
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", false);
				}
				else if (g_pFresh->IsCurrentLayout("GHOST"))
				{
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc", true);
				}
				else
				{
					if (IS_EXACTKINDOF(CShopTask, AfxGetTask()))
					{
						int param1, param2;
						SendMsg(NULL, "Shop", 0xc3, (int)&param1, (int)&param2,
							0, 0);
						AfxPostMsg(NULL, "RealMyRoom", 0xc4, param1, param2, 0,
							0);
					}
					CTaskManager::Instance()->ChangeTask("CRealMyRoomTask",
						"CGolfDoc",
						CTaskManager::Instance()->GetPreservedTask() != NULL);
				}
				AfxPostMsg(NULL, "RealMyRoom", 0, (int)"REALMYROOMVISITOR", 0,
					0, 0);
			}

			WSendPacket send((enumClientPacket)0xaf);
			send.Encode4(Doc()->m_replayTargetUID);
			send.Encode1(1);
			send.Send(TO_GAME);

			if (IsLocalContent((localContentType_t)0x66))
			{
				WSendPacket send((enumClientPacket)0xc4);
				send.Encode4(Doc()->m_replayTargetUID);
				send.Send(TO_GAME);
			}

			if (IsLocalContent((localContentType_t)0x59))
			{
				WSendPacket send((enumClientPacket)0xb9);
				send.Encode1(2);
				send.Send(TO_GAME);
			}

			Doc()->SetRmrMode(0);
		}
		m_pRMREnterSetting = NULL;
		break;
	}
}

bool CTaskMain::OnRMR_ResultSetting(int result, FrForm* pForm)
{
	if (pForm)
		FrForm::EnableTail(true);
	if (result == 1)
	{
		Doc()->SetRmrMode(5);
		ChangeRealMyRoom();
	}
	else
	{
		m_pRMREnterSetting = NULL;
	}
	return true;
}

// HACK: volatile qualifier is a hack
unsigned long CTaskMain::GetCategoryFlag(volatile int type)
{
	switch (type)
	{
	case 0:
	case 13:
	case 22:
		return 0;
	case 3:
		return 0x10;
	case 4:
		return 0x2c;
	case 5:
		return 0x40;
	case 26:
		return 0x380;
	case 1:
	case 14:
		return 1;
	case 2:
	case 15:
		return 2;
	case 16:
		return 3;
	case 21:
		return 0x3bf;
	case 28:
		return 7;
	case 29:
		return 9;
	case 30:
		return 8;
	case 34:
		return 6;
	}
	return type;
}

bool CTaskMain::GetBtnState(unsigned char bar, eBarButton button)
{
	if (!IsLocalContent((localContentType_t)0x56))
		return false;

	FrButton* pButton = (&m_pUnderBarBack)[button];
	if (!pButton)
		return false;
	if (!pButton->IsVisible())
		return false;
	if (!pButton->IsEnabled())
		return false;

	FrGroupBox* pBar = (&m_pOverBar)[bar];
	if (!pBar)
		return false;
	if (!pBar->IsVisible())
		return false;
	if (!pBar->IsEnabled())
		return false;

	return true;
}
