#include "stdafx.h"
#include "mathconsts.h"
#include "quadtree.h"
#include "fx.h"
#include "fxbox.inl"
#include "avatar_main.h"
#include "avatar_avatar.h"
#include "ntavatar.h"
#include "clientsetting.h"
#include "fresh.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "mousecursor.h"
#include "chatmsg.h"
#include "uccmanager.h"
#include "woverlay.h"
#include "fxsequence.h"
#include "ucclibrary.h"
#include "balloontext.h"
#include "avatar_task.h"
#include "avataruserlistdlg.h"
#include "frcombobox.h"
#include "frframe.h"
#include "avatarchatlogdlg.h"
#include "frscrollbar.h"
#include "user_info.h"
#include "polysoup.h"
#include "sky.h"
#include "wind.h"
#include "messengerdlg.h"
#include "s5/utilities.h"
#include "quitwindow.h"
#include "frgraphicinterface.h"
#include "jukeboxdlg.h"
#include "shadow.h"

extern Fresh* g_pFresh;
bool g_SALoadingState = false;
extern WMatrix g_camera;
extern bool g_bQuit;
extern unsigned long g_CurrentTime;
extern LightSet g_lightset;

static const char s_commands[5][64] = { "\xb0\xc5\xc0\xce",
	"\xbf\xd5\xb8\xd3\xb8\xae", "\xb1\xa4\xbc\xd3", "\xb9\xdd\xc2\xa6\xc0\xcc",
	"" };

IMPLEMENT_ACTOR(ntAvatarChatMain, CTaskMain)

ntAvatarChatMain::ntAvatarChatMain(void)
{
	m_dispPriority = DISP_PRIORITY_4;

	m_pPVS = 0;
	m_pPVS = &::GetPVS();

	m_preservedMap = 0;
	m_preservedHole = 0;

	m_glowTime = 0.0f;
	m_camLookAt = WVector::ZERO;
	m_camAngle = WVector(-28.0f * g_DEGTORAD, 0.0f, 0.0f);
	m_camDist = 28.800001f;
	m_camTargetDist = 28.800001f;
	m_camDefaultDist = 28.800001f;
	m_camHeight = 4.6f;
	m_camTargetHeight = 4.6f;
	m_camYawVel = 0.0f;
	m_camPitchVel = 0.0f;
	m_camZoom = 1.0f;
	m_bTopView = false;
	m_topViewHeight = 256.0f;
	m_bGlow = false;

	m_glowTex = g_resrcmng->LoadTexture("~GlowBall.jpg", 0, 0, 0);
	m_bPortraitLoaded = false;

	new ntBalloonTextManager;
	ntBalloonTextManager::Instance()->Init();

	m_pFont = g_resrcmng->GetTitleFont();

	tagWTITLEFONT info;
	memset(&info, 0, sizeof(info));

	info.fonth = 16;
	info.fontw = 16;

	char* fontName = "[font_wind.jpg";
	info.filename = &fontName;
	info.numPages = 1;
	info.texw = 128;
	info.texh = 64;
	info.pCharSet = "1234567890ym%-.?/:, ";
	m_pFont->Create(&info);

	InitUI();

	m_bCameraLock = false;
	m_pickedUID = 0xFFFFFFFF;
}

ntAvatarChatMain::~ntAvatarChatMain(void)
{
	g_audio->StopSfx("AmbientSnd");

	ntBalloonTextManager::Instance()->Shutdown();
	if (ntBalloonTextManager::IsInstantiated())
	{
		delete ntBalloonTextManager::Instance();
	}

	if (g_resrcmng && m_glowTex)
	{
		g_resrcmng->Release(m_glowTex);
		m_glowTex = 0;
	}

	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = 0;
	}

	for (int i = 0; i < 10; i++)
	{
		ClearTexCache(m_pPortrait[i]);
		delete m_pPortrait[i];
		m_pPortrait[i] = 0;
	}
}

void ntAvatarChatMain::OnPreLoadInit(void)
{
	CTaskMain::OnPreLoadInit();
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());

	GOLFDOC()->MakePlayer(1);
}

void ntAvatarChatMain::OnLoad(void)
{
	sSlotInfo slotInfo;
	sCharacterInfo charInfo;

	CTaskMain::OnLoad();

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME))
	{
		SetCurMap(CSharedDoc::Instance()->m_roomInfo.mapType);

		LoadBg(CSharedDoc::Instance()->m_golfGame.pCourse
				? CSharedDoc::Instance()->m_golfGame.pCourse->Data
				: 0,
			CSharedDoc::Instance()->m_roomInfo.nHole);

		WPolySoup::sCamera* camera =
			GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera)
		{
			g_camera = camera->mat;
			g_view->SetFOV(camera->fov * g_DEGTORAD);
		}

		g_audio->StopBGM(false);
		if (GetCurMapProp() && GetCurMapProp()->AmbientSnd[0])
		{
			g_audio->StopSfx("AmbientSnd");
			g_audio->PlaySfx(GetCurMapProp()->AmbientSnd, 0, 1, "AmbientSnd", 0,
				0.5f, 200.0f);
		}

		CSceneManager::Instance()->SetWeather(0, false);

		ntAvatarManager::Instance()->FreeAllAvatars();
	}
	else
	{
		SetCurMap(COption::Instance()->dGetMap());

		LoadBg(CSharedDoc::Instance()->m_golfGame.pCourse
				? CSharedDoc::Instance()->m_golfGame.pCourse->Data
				: 0,
			COption::Instance()->dGetStartHole());

		WPolySoup::sCamera* camera =
			GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		if (camera)
		{
			g_camera = camera->mat;
			g_view->SetFOV(camera->fov * g_DEGTORAD);
		}

		g_audio->StopBGM(false);
		if (GetCurMapProp() && GetCurMapProp()->AmbientSnd[0])
		{
			g_audio->StopSfx("AmbientSnd");
			g_audio->PlaySfx(GetCurMapProp()->AmbientSnd, 0, 1, "AmbientSnd", 0,
				0.5f, 200.0f);
		}

		CSceneManager::Instance()->SetWeather(
			COption::Instance()->dGetWeather(), false);

		ntAvatarManager::Instance()->FreeAllAvatars();

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(
			MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
		if (point == 0)
		{
			point = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
		}

		WVector pos;
		pos = GetPVS().GetGroundPoint(point->pos, false, 0, false);

		memset(&slotInfo, 0, sizeof(slotInfo));
		slotInfo.location[2] = Random(0.0f, g_2_PI, -1);
		slotInfo.bMaster = 1;
		slotInfo.dwGuid = 0;
		slotInfo.dwTitle = 0;
		slotInfo.level = 1;
		rand();
		slotInfo.location[0] = pos.x;
		slotInfo.location[1] = pos.z;
		slotInfo.gender = 0;
		strcpy(slotInfo.sNick, "jufoot");
		slotInfo.tidChar =
			COption::Instance()->dGetDefaultCharacter() | 0x4000000;
		CSharedDoc::Instance()->m_slotList.push_back(slotInfo);

		memset(&charInfo, 0, sizeof(charInfo));
		charInfo.tid = slotInfo.tidChar;

		ntAvatarManager::Instance()->m_pMyAvatar =
			ntAvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);

		static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
			ntAvatarBurden(0, 0, charInfo, 0));

		for (int i = 1; i < COption::Instance()->dGetNumPlayers(); i++)
		{
			point = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
			if (point == 0)
			{
				point = GOLFDOC()->m_pPolySoup->FindPoint(
					MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
			}

			pos = point->pos;
			pos.x += Random(-16.0f, 16.0f, -1);
			pos.z += Random(-16.0f, 16.0f, -1);
			pos = GetPVS().GetGroundPoint(pos, false, 0, false);

			memset(&slotInfo, 0, sizeof(slotInfo));
			slotInfo.location[2] = Random(0.0f, g_2_PI, -1);
			slotInfo.bMaster = 0;
			slotInfo.dwGuid = i;
			slotInfo.dwTitle = 0;
			slotInfo.level = 1;
			rand();
			slotInfo.location[0] = pos.x;
			slotInfo.location[1] = pos.z;
			slotInfo.gender = 0;
			strcpy(slotInfo.sNick, MakeStr("dummy%2d", i));
			slotInfo.tidChar = rand() % 7 | 0x4000000;
			CSharedDoc::Instance()->m_slotList.push_back(slotInfo);

			memset(&charInfo, 0, sizeof(charInfo));
			charInfo.tid = slotInfo.tidChar;

			_util::GenerateRandomTidParts(charInfo.tid, charInfo.tidParts,
				charInfo.tidAuxParts);

			ntAvatarManager::Instance()->CreateAvatar(slotInfo, charInfo, this);

			static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
				ntAvatarBurden(i, i, charInfo, 0));
		}
	}

	GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());

	Wind().Init(0xFF, 0xFF, 0, 1);

	Wind().CheckSpecialWind();
	Wind().SetWind(WVector(1.0f, 0.0f, 1.0f));
	GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
	CSceneManager::Instance()->UpdateWind();

	m_pIcon[0] = g_resrcmng->GetOverlay("i_male.tga", 0);
	m_pIcon[1] = g_resrcmng->GetOverlay("i_female.tga", 0);
	m_pIcon[2] = g_resrcmng->GetOverlay("i_male_02.tga", 0);
	m_pIcon[3] = g_resrcmng->GetOverlay("i_female_02.tga", 0);
	m_pIcon[4] = g_resrcmng->GetOverlay("i_male_03.tga", 0);
	m_pIcon[5] = g_resrcmng->GetOverlay("i_female_03.tga", 0);

	m_pMannerIcon[0] = g_resrcmng->GetOverlay("i_male_manner.tga", 0);
	m_pMannerIcon[1] = g_resrcmng->GetOverlay("i_female_manner.tga", 0);

	m_pAngelIcon[0] = g_resrcmng->GetOverlay("i_male_angel.tga", 0);
	m_pAngelIcon[1] = g_resrcmng->GetOverlay("i_female_angel.tga", 0);

	m_pDive = g_resrcmng->GetOverlay("dive.tga", 0);
	m_pStateIcon1 = g_resrcmng->GetOverlay("state_icon_01.tga", 0);
	m_pStateIcon2 = g_resrcmng->GetOverlay("state_icon_02.tga", 0);
	m_pStateIcon3 = g_resrcmng->GetOverlay("state_icon_03.tga", 0);
	m_pStateIcon4 = g_resrcmng->GetOverlay("state_icon_04.tga", 0);
	m_pStateIcon5 = g_resrcmng->GetOverlay("state_icon_05.tga", 0);
	m_pStateIcon6 = g_resrcmng->GetOverlay("state_icon_06.tga", 0);
	m_pStateIconN1 = g_resrcmng->GetOverlay("stateicon_01_n.tga", 0);
	m_pStateIconN2 = g_resrcmng->GetOverlay("stateicon_02_n.tga", 0);
	m_pStateIconN3 = g_resrcmng->GetOverlay("stateicon_03_n.tga", 0);
	m_pStateIconN4 = g_resrcmng->GetOverlay("stateicon_04_n.tga", 0);

	m_pEmoticon = g_resrcmng->GetOverlay("[emoti_object.png", 0);

	for (unsigned int bar = 0; bar < 9; bar++)
	{
		m_pStateBar[bar] =
			g_resrcmng->GetOverlay(MakeStr("statebar_%02d.tga", bar + 1), 0);
	}

	m_shadowTex = g_resrcmng->LoadTexture("[shadow.jpg", 0, 0, 0);

	CSceneManager::Instance()->ToggleTransparent();

	if (CMessengerInfo::Instance()->GetMyStatus() != 3)
	{
		CMessengerInfo::Instance()->SetMyStatus((MSN::eStatusType)0);
	}
}

void ntAvatarChatMain::DoReLoad(void)
{
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	AfxGetTask()->Init(0);
}

void ntAvatarChatMain::OnInit(void)
{
	CTaskMain::OnInit();

	SetBGMNone();

	CalcUIOffset(false);

	if (CMouseCursor::IsInstantiated())
	{
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}

	CSceneManager::Instance()->UpdateFog(true);
	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, this,
		"\xbe\xc6\xb9\xd9\xc5\xb8\xb8\xde\xc0\xce", 0, 0);

	if (CChatMsg::Instance())
	{
		CChatMsg::Instance()->Close(true, true);
		CChatMsg::Instance()->EnableCaretMove(true);
	}

	if (!CChatMsg::Instance()->IsOpened())
	{
		g_pFresh->GetManager()->SetExclusiveKey(false);
	}

	FrJukeBoxDlg::Init();

	FrJukeBoxDlg::PlayWithMap(GetCurMap());

	g_input->Reset();
}

void ntAvatarChatMain::OnDestroy(void)
{
	CTaskMain::OnDestroy();

	if (CShadowManager::Instance())
	{
		delete CShadowManager::Instance();
	}

	for (int gender = 0; gender < 6; gender++)
	{
		if (g_resrcmng && m_pIcon[gender])
		{
			g_resrcmng->Release(m_pIcon[gender]);
			m_pIcon[gender] = 0;
		}
	}

	for (int manner = 0; manner < 2; manner++)
	{
		if (g_resrcmng && m_pMannerIcon[manner])
		{
			g_resrcmng->Release(m_pMannerIcon[manner]);
			m_pMannerIcon[manner] = 0;
		}
	}

	for (int angel = 0; angel < 2; angel++)
	{
		if (g_resrcmng && m_pAngelIcon[angel])
		{
			g_resrcmng->Release(m_pAngelIcon[angel]);
			m_pAngelIcon[angel] = 0;
		}
	}

	if (g_resrcmng && m_pDive)
	{
		g_resrcmng->Release(m_pDive);
		m_pDive = 0;
	}

	if (g_resrcmng && m_pStateIcon1)
	{
		g_resrcmng->Release(m_pStateIcon1);
		m_pStateIcon1 = 0;
	}

	if (g_resrcmng && m_pStateIcon2)
	{
		g_resrcmng->Release(m_pStateIcon2);
		m_pStateIcon2 = 0;
	}

	if (g_resrcmng && m_pStateIcon3)
	{
		g_resrcmng->Release(m_pStateIcon3);
		m_pStateIcon3 = 0;
	}

	if (g_resrcmng && m_pStateIcon4)
	{
		g_resrcmng->Release(m_pStateIcon4);
		m_pStateIcon4 = 0;
	}

	if (g_resrcmng && m_pStateIcon5)
	{
		g_resrcmng->Release(m_pStateIcon5);
		m_pStateIcon5 = 0;
	}

	if (g_resrcmng && m_pStateIcon6)
	{
		g_resrcmng->Release(m_pStateIcon6);
		m_pStateIcon6 = 0;
	}

	if (g_resrcmng && m_pStateIconN1)
	{
		g_resrcmng->Release(m_pStateIconN1);
		m_pStateIconN1 = 0;
	}

	if (g_resrcmng && m_pStateIconN2)
	{
		g_resrcmng->Release(m_pStateIconN2);
		m_pStateIconN2 = 0;
	}

	if (g_resrcmng && m_pStateIconN3)
	{
		g_resrcmng->Release(m_pStateIconN3);
		m_pStateIconN3 = 0;
	}

	if (g_resrcmng && m_pStateIconN4)
	{
		g_resrcmng->Release(m_pStateIconN4);
		m_pStateIconN4 = 0;
	}

	if (g_resrcmng && m_pEmoticon)
	{
		g_resrcmng->Release(m_pEmoticon);
		m_pEmoticon = 0;
	}

	for (int bar = 0; bar < 9; bar++)
	{
		if (g_resrcmng && m_pStateBar[bar])
		{
			g_resrcmng->Release(m_pStateBar[bar]);
			m_pStateBar[bar] = 0;
		}
	}

	if (g_resrcmng && m_shadowTex)
	{
		g_resrcmng->Release(m_shadowTex);
		m_shadowTex = 0;
	}

	int guids[30];
	int count = 0;
	for (int slot = 0;
		slot < 30 && count < ntAvatarManager::Instance()->m_avatarCount; slot++)
	{
		if (ntAvatarManager::Instance()->m_avatar[slot])
		{
			guids[count++] =
				ntAvatarManager::Instance()->m_avatar[slot]->m_guid;
		}
	}

	for (int msgIndex = 0; msgIndex < count; msgIndex++)
	{
		this << MsgObject(this, 0xE5, guids[msgIndex], 0, 0, 0, 0);
	}

	g_pFresh->GetManager()->SetExclusiveKey(true);
}

void ntAvatarChatMain::OnPreserveBack(const char* nextTask)
{
	if (strcmp(nextTask, "CRealMyRoomTask") == 0)
	{
		ntAvatarManager::Instance()->m_pMyAvatar->SetState(
			ntAvatarManager::Instance()->m_pMyAvatar->GetState() | 2);
	}
	else if (strcmp(nextTask, "CShopTask") == 0)
	{
		ntAvatarManager::Instance()->m_pMyAvatar->SetState(
			ntAvatarManager::Instance()->m_pMyAvatar->GetState() | 4);
	}

	ntAvatarManager::Instance()->m_pMyAvatar->Stop();

	g_audio->StopSfx("AmbientSnd");

	FrJukeBoxDlg::Preserve();

	m_preservedMap = GetCurMap();
	m_preservedHole = GOLFDOC()->m_currentHole;

	SetPVSIndex(1);

	ResetFrWndPointers();

	CTaskMain::OnPreserveBack(nextTask);
}

void ntAvatarChatMain::OnRestorePreserved(void)
{
	CTaskMain::OnRestorePreserved();

	g_lightset = m_light;

	SetPVSIndex(0);

	SetCurMap(m_preservedMap);
	GOLFDOC()->m_currentHole = m_preservedHole;
	COption::Instance()->vSetMinimumScreenSize(1, 800, 600);
	COption::Instance()->vApplyGameScreenSize();
	ntAvatarManager::Instance()->m_bActive = true;

	std::map<unsigned int, sCharacterInfo>::iterator it =
		CSharedDoc::Instance()->m_charMap.find(
			CSharedDoc::Instance()->m_myInfo.userEquip.guidChar);
	if (it != CSharedDoc::Instance()->m_charMap.end())
	{
		WSendPacket packet((enumClientPacket)0x0C);
		packet.Encode1(4);
		packet.Encode4(it->second.guid);
		packet.Send(TO_GAME);
	}

	ntAvatarManager::Instance()->ResetAvatarPositionAngle();

	if (CSharedDoc::Instance()->m_golfGame.pCourse &&
		CSharedDoc::Instance()->m_golfGame.pCourse->AmbientSnd[0])
	{
		g_audio->PlaySfx(CSharedDoc::Instance()->m_golfGame.pCourse->AmbientSnd,
			0, 1, "AmbientSnd", 0, 0.5f, 200.0f);
	}

	SetBGMNone();

	CalcUIOffset(false);

	if (CMouseCursor::IsInstantiated())
	{
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}

	FrJukeBoxDlg::Restore();

	ntAvatarManager::Instance()->m_pMyAvatar->SetState(
		ntAvatarManager::Instance()->m_pMyAvatar->GetState() & ~6);
}

void ntAvatarChatMain::LoadBg(const char* name, int hole)
{
	m_bgName = name;
	GOLFDOC()->m_currentHole = hole;

	if (CFx::Instance())
	{
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load(
			MakeStr("%s_%02d.gbin", name, GOLFDOC()->m_currentHole), true))
	{
		g_bQuit = true;
		return;
	}

	m_light = g_lightset;

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", name));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", name));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(name, 0.0f);
	GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));

	GOLFDOC()->m_pPolySoup->MakeDataStructure();

	if (GOLFDOC()->m_pSky)
	{
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	CSceneManager::Instance()->Load();

	GOLFDOC()->m_pPolySoup->EndLoad(false);

	if (CFx::Instance())
	{
		CFx::Instance()->SetActive(true);
	}
}

void ntAvatarChatMain::CheckUIPosition(void)
{
	static float s_barY = COption::Instance()->gGetBar_Y();

	if (COption::Instance()->gGetBar_Y() == s_barY)
	{
		return;
	}

	CalcUIOffset(false);
	s_barY = COption::Instance()->gGetBar_Y();

	g_pFresh->OpenLayout("AVATARCHAT_MAIN", this, false);

	if (ntAvatarManager::Instance()->m_pMyAvatar->GetState() & 0x80)
	{
		EnableUnderBar(false);
	}
}

void ntAvatarChatMain::ToggleTopViewMode(void)
{
	m_bTopView = !m_bTopView;
	if (m_bTopView)
	{
		GOLFDOC()->GameMode() = 0x200;

		Waabb box;
		m_pPVS->GetAABB(box);

		if (ntAvatarManager::Instance()->m_pMyAvatar)
		{
			m_topViewPos = ntAvatarManager::Instance()->m_pMyAvatar->m_pos;
		}
		else
		{
			m_topViewPos.x = (box.min.x + box.max.x) * 0.5f;
			m_topViewPos.y = box.min.y;
			m_topViewPos.z = (box.min.z + box.max.z) * 0.5f;
		}
		m_topViewHeight = 256.0f;

		CSceneManager::Instance()->SetFog(false);
	}
	else
	{
		GOLFDOC()->GameMode() = 0;
		CSceneManager::Instance()->SetFog(true);
	}
}

void ntAvatarChatMain::OnProcess(float elapsed)
{
	static WVector s_keyMoveDir = WVector::UNIT_POS_Z;
	static WVector s_cameraFront = WVector::UNIT_POS_X;
	static bool s_flagAF3E4C = false;
	static bool s_moving = false;
	static bool s_keyMoving = false;

	CheckUIPosition();

	CTaskMain::OnProcess(elapsed);

	GetPVS().PreProcess(elapsed, false);

	ntBalloonTextManager::Instance()->Process(elapsed);

	if (!ntAvatarManager::Instance()->m_pMyAvatar)
	{
		return;
	}

	if (g_pFresh->GetManager()->GetDesktop()->m_nFlags.GetFlag(0x8))
	{
		int leftButton = g_input->GetButton(LEFT_BUTTON);
		int rightButton = g_input->GetButton(RIGHT_BUTTON);
		int middleButton = g_input->GetButton(MIDDLE_BUTTON);
		WVector mouseDelta;
		WVector target;
		int groundIndex;
		mouseDelta = g_input->GetMouseDelta();

		WVector mouse = g_input->GetMousePoint();
		WVector rayFrom, rayDir;
		g_view->GetScreen2World(mouse.x, mouse.y, rayFrom, rayDir);

		if (m_pRightMenu)
		{
			if (!CMouseCursor::Instance()->InArea(m_pRightMenu->GetRect()))
			{
				if (leftButton == 1 || rightButton == 1)
				{
					m_pRightMenu->SetVisible(false);
				}
			}
		}

		float topViewSpeed = Max(0.1f, m_topViewHeight * 0.0010416667f);

		if (!s_keyMoving)
		{
			s_cameraFront = g_view->camera.xa;
			s_cameraFront = s_cameraFront.Normalize();

			s_keyMoveDir = WCrossProduct(s_cameraFront, WVector::UNIT_POS_Y);
			s_keyMoveDir = s_keyMoveDir.Normalize();
		}

		int area = CMouseCursor::Instance()->GetArea();
		switch (area)
		{
		case 0:
		{
			if (!m_bTopView)
			{
				ntAvatarManager::Instance()->m_pPickedAvatar =
					ntAvatarManager::Instance()->AvatarByRayIntersect(rayFrom,
						rayDir);
				ntAvatarManager::Instance()->m_pClickedAvatar =
					ntAvatarManager::Instance()->CheckClick(mouse.x, mouse.y);

				if (ntAvatarManager::Instance()->m_pPickedAvatar ||
					ntAvatarManager::Instance()->m_pClickedAvatar)
				{
					CMouseCursor::Instance()->SetCursor(1);
				}
				else
				{
					CMouseCursor::Instance()->SetCursor(0);
				}
			}

			if (m_pRightMenu && m_pRightMenu->IsVisible())
			{
				WRect menuRect(m_pRightMenu->GetRect());

				if (menuRect.IsInRect(mouse.x, mouse.y))
				{
					break;
				}
			}

			if (leftButton == 1)
			{
				ntAvatar* pClicked;

				if (!m_bTopView &&
					(pClicked = ntAvatarManager::Instance()->CheckClick(mouse.x,
						 mouse.y)) != 0)
				{
					strcpy(m_pickedNick, pClicked->GetNickName());
					m_pickedUID = pClicked->GetUID();

					if (pClicked->GetState() & 0x80)
					{
						if (pClicked !=
							ntAvatarManager::Instance()->m_pMyAvatar)
						{
							OnEnterTradeShop();
						}
						else
						{
							OnOpenTradeShop();
						}
					}
				}
				else
				{
					target = GetPVS().GetRayIntersection(rayFrom, rayDir);

					if (!ntAvatarManager::Instance()
							->m_pMyAvatar->IsHoldState())
					{
						target = GetPVS().GetGroundPoint(target, true,
							&groundIndex, true);

						if (groundIndex >= 0)
						{
							s_moving = true;
							ntAvatarManager::Instance()
								->m_pMyAvatar->SetTargetPosition(target);
							ntAvatarManager::Instance()->m_pMyAvatar->SetAction(
								0);
							ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
						}
					}
				}
			}

			if (rightButton == 1)
			{
				if (!m_bTopView)
				{
					if (ntAvatarManager::Instance()->m_pPickedAvatar)
					{
						strcpy(m_pickedNick,
							ntAvatarManager::Instance()
								->m_pPickedAvatar->GetNickName());
						m_pickedUID = ntAvatarManager::Instance()
										  ->m_pPickedAvatar->GetUID();
					}
					else
					{
						m_pickedNick[0] = 0;
						m_pickedUID = 0xFFFFFFFF;
						m_lastNick[0] = 0;
					}
				}
				s_flagAF3E4C = false;
			}
			else if (rightButton == 3)
			{
				if (!m_bTopView)
				{
					static WVector s_lastMouseDelta(0.0f, 0.0f, 0.0f);

					float turnY = (s_lastMouseDelta.y + mouseDelta.x) * 0.5f;
					float turnX = (mouseDelta.y + s_lastMouseDelta.y) * 0.5f;
					m_camAngle.y -= turnY * 0.008f;
					m_camAngle.x -= turnX * 0.008f;
					s_lastMouseDelta = mouseDelta;

					if (m_camAngle.x < -g_PI / 2)
					{
						m_camAngle.x = -g_PI / 2;
					}

					if (m_camAngle.x > g_PI / 4)
					{
						m_camAngle.x = g_PI / 4;
					}

					m_camPos = m_camAngle;
					m_camYawVel = 0.0f;
				}

				if (!WisZero(mouseDelta.x, 1e-5f) ||
					!WisZero(mouseDelta.y, 1e-5f))
				{
					s_flagAF3E4C = true;
				}

				g_input->FixPointer(true);
				CMouseCursor::Instance()->Toggle(false);
			}
			else if (rightButton == 2)
			{
				if (!m_bTopView && !s_flagAF3E4C &&
					ntAvatarManager::Instance()->m_pPickedAvatar &&
					strcmp(ntAvatarManager::Instance()
							   ->m_pPickedAvatar->GetNickName(),
						m_pickedNick) == 0)
				{
					PopupRightMenu((int)g_input->GetMousePoint().x,
						(int)g_input->GetMousePoint().y);
				}

				g_input->FixPointer(false);
				CMouseCursor::Instance()->Toggle(true);
			}
			else
			{
				g_input->FixPointer(false);
				CMouseCursor::Instance()->Toggle(true);
			}

			if (g_input->GetButton(LEFT_BUTTON) &&
				g_input->GetButton(RIGHT_BUTTON))
			{
				if (!m_bTopView && !CChatMsg::Instance()->IsOpenning())
				{
					target = ntAvatarManager::Instance()->m_pMyAvatar->m_pos;
					target += s_keyMoveDir * 4.1f;

					if (!WisEqual(target,
							ntAvatarManager::Instance()->m_pMyAvatar->m_pos,
							1e-5f) &&
						!ntAvatarManager::Instance()
							->m_pMyAvatar->IsHoldState())
					{
						s_moving = false;

						ntAvatarManager::Instance()
							->m_pMyAvatar->SetTargetPosition(target);
						ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
						ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
					}
				}
			}

			if (middleButton == 1)
			{
				ToggleTopViewMode();
			}

			if (!g_pFresh->GetManager()->GetWheelFocus())
			{
				if (!m_bTopView)
				{
					m_camTargetDist = Between(10.88f,
						m_camTargetDist -
							mouseDelta.z * g_input->GetSensitivity() * 0.021f,
						64.0f);
				}
				else
				{
					m_topViewHeight = Between(25.6f,
						m_topViewHeight -
							mouseDelta.z * g_input->GetSensitivity() *
								topViewSpeed * 0.4f,
						960.0f);
				}
			}
		}
		break;
		}

		if (m_bTopView)
		{
			if (g_input->GetMousePoint().x < 5.0f)
			{
				m_topViewPos.x -= topViewSpeed * elapsed * 800.0f;
			}

			if (g_view->GetWidth() - 6.0f < g_input->GetMousePoint().x)
			{
				m_topViewPos.x += topViewSpeed * elapsed * 800.0f;
			}

			if (g_input->GetMousePoint().y < 5.0f)
			{
				m_topViewPos.z += topViewSpeed * elapsed * 800.0f;
			}

			if (g_view->GetHeight() - 6.0f < g_input->GetMousePoint().y)
			{
				m_topViewPos.z -= topViewSpeed * elapsed * 800.0f;
			}
		}

		if (!g_pFresh->GetManager()->IsKeyExclusive())
		{
			if (g_input->GetDown("ENTER", true) ||
				g_input->GetDown("PADENTER", true))
			{
				if (CChatMsg::Instance()->IsClosed())
				{
					CChatMsg::Instance()->Open(true, true);
				}
			}

			bool shift;
			if (g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true))
			{
				shift = true;
			}
			else
			{
				shift = false;
			}
			bool control;
			if (g_input->Get("LCONTROL", true) ||
				g_input->Get("RCONTROL", true))
			{
				control = true;
			}
			else
			{
				control = false;
			}

			target = ntAvatarManager::Instance()->m_pMyAvatar->m_pos;

			if (!m_bTopView && !CChatMsg::Instance()->IsOpenning())
			{
				if (!control)
				{
					if (g_input->Get("\xbb\xf3", true))
					{
						target += s_keyMoveDir * 4.1f;
					}

					if (g_input->Get("\xc7\xcf", true))
					{
						target += -s_keyMoveDir * 4.1f;
					}

					if (g_input->Get("\xc1\xc2", true))
					{
						target += -s_cameraFront * 4.1f;
					}

					if (g_input->Get("\xbf\xec", true))
					{
						target += s_cameraFront * 4.1f;
					}

					target = GetPVS().GetGroundPoint(target, true, &groundIndex,
						true);
					if (groundIndex < 0)
					{
						target =
							ntAvatarManager::Instance()->m_pMyAvatar->m_pos;
					}
				}
				else
				{
					float turnY = 0.0f;
					float turnX = 0.0f;

					if (g_input->Get("\xbb\xf3", true))
					{
						turnX = elapsed * (-g_PI / 6);
					}

					if (g_input->Get("\xc7\xcf", true))
					{
						turnX += elapsed * (g_PI / 6);
					}

					if (g_input->Get("\xc1\xc2", true))
					{
						turnY = elapsed * (-g_PI / 6);
					}

					if (g_input->Get("\xbf\xec", true))
					{
						turnY += elapsed * (g_PI / 6);
					}

					if (!WisZero(turnY, 1e-5f) || !WisZero(turnX, 1e-5f))
					{
						m_camAngle.y -= turnY;
						m_camAngle.x -= turnX;

						if (m_camAngle.x < -g_PI / 2)
						{
							m_camAngle.x = -g_PI / 2;
						}

						if (m_camAngle.x > g_PI / 4)
						{
							m_camAngle.x = g_PI / 4;
						}

						m_camPos = m_camAngle;
						m_camYawVel = 0.0f;
					}
				}

				if (g_input->Get("HOME", true))
				{
					m_camTargetDist = Between(10.88f,
						m_camTargetDist - elapsed * 16.8f, 64.0f);
				}

				if (g_input->Get("END", true))
				{
					m_camTargetDist = Between(10.88f,
						m_camTargetDist + elapsed * 16.8f, 64.0f);
				}

				if (g_input->GetDown("PAGE_DOWN", true))
				{
					if (!ntAvatarManager::Instance()
							->m_pMyAvatar->IsHoldState())
					{
						if (ntAvatarManager::Instance()
								->m_pMyAvatar->GetAction() != 2)
						{
							if (ntAvatarManager::Instance()
									->m_pMyAvatar->GetAction() == 1)
							{
								ntAvatarManager::Instance()
									->m_pMyAvatar->SetAction(2);
							}
							else
							{
								ntAvatarManager::Instance()
									->m_pMyAvatar->Stop();
								ntAvatarManager::Instance()
									->m_pMyAvatar->SetAction(1);
							}
						}
					}
				}

				if (g_input->GetDown("PAGE_UP", true))
				{
					if (!ntAvatarManager::Instance()
							->m_pMyAvatar->IsHoldState())
					{
						if (ntAvatarManager::Instance()
									->m_pMyAvatar->GetAction() == 1 ||
							ntAvatarManager::Instance()
									->m_pMyAvatar->GetAction() == 2)
						{
							if (ntAvatarManager::Instance()
									->m_pMyAvatar->GetAction() == 1)
							{
								ntAvatarManager::Instance()
									->m_pMyAvatar->SetAction(0);
							}
							else
							{
								ntAvatarManager::Instance()
									->m_pMyAvatar->SetAction(1);
							}
						}
					}
				}
			}
			else
			{
				if (g_input->Get("\xbb\xf3", true) && !shift)
				{
					m_topViewPos.z += topViewSpeed * elapsed * 800.0f;
				}

				if (g_input->Get("\xc7\xcf", true) && !shift)
				{
					m_topViewPos.z -= topViewSpeed * elapsed * 800.0f;
				}

				if (g_input->Get("\xc1\xc2", true))
				{
					m_topViewPos.x -= topViewSpeed * elapsed * 800.0f;
				}

				if (g_input->Get("\xbf\xec", true))
				{
					m_topViewPos.x += topViewSpeed * elapsed * 800.0f;
				}

				if (g_input->Get("HOME", true) ||
					(shift && g_input->Get("\xbb\xf3", true)))
				{
					m_topViewHeight = Between(25.6f,
						m_topViewHeight - topViewSpeed * elapsed * 1200.0f,
						960.0f);
				}

				if (g_input->Get("END", true) ||
					(shift && g_input->Get("\xc7\xcf", true)))
				{
					m_topViewHeight = Between(25.6f,
						m_topViewHeight + topViewSpeed * elapsed * 1200.0f,
						960.0f);
				}
			}

			if (g_input->GetDown("PAD_0", true) ||
				g_input->GetDown("NUM_0", true))
			{
				ToggleTopViewMode();
			}

			if (!WisEqual(target,
					ntAvatarManager::Instance()->m_pMyAvatar->m_pos, 1e-5f) &&
				!ntAvatarManager::Instance()->m_pMyAvatar->IsHoldState())
			{
				s_moving = false;
				s_keyMoving = true;

				ntAvatarManager::Instance()->m_pMyAvatar->SetTargetPosition(
					target);
				ntAvatarManager::Instance()->m_pMyAvatar->SetAction(0);
				ntAvatarManager::Instance()->m_pMyAvatar->Follow(0);
			}
			else
			{
				s_keyMoving = false;
			}
		}
	}
	else
	{
		if (m_pRightMenu->IsVisible())
		{
			m_pRightMenu->SetVisible(false);
		}
		g_input->FixPointer(false);
		CMouseCursor::Instance()->Toggle(true);
	}

	if (ntAvatarManager::Instance()->m_pMyAvatar->IsTargetPositionChanged(
			false))
	{
		WVector dir;
		if (ntAvatarManager::Instance()->m_pMyAvatar->IsFollowing() &&
			ntAvatarManager::Instance()->m_pMyAvatar->GetFollowingAvatar())
		{
			ntAvatar* pMyAvatar = ntAvatarManager::Instance()->m_pMyAvatar;
			dir = pMyAvatar->GetFollowingAvatar()->m_pos - pMyAvatar->m_pos;
			s_moving = true;
		}
		else if (s_keyMoving)
		{
			dir = s_keyMoveDir;
		}
		else
		{
			dir =
				ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition() -
				ntAvatarManager::Instance()->m_pMyAvatar->m_pos;
			m_camYawVel = 0.0f;
		}

		dir.Normalize();

		m_camPitchVel =
			(float)atan2((double)dir.z, (double)dir.x) + g_PI * 1.5f;
		m_camPos = m_camAngle;
	}

	ntAvatarManager::Instance()->Process(elapsed);

	if (ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 1)
	{
		m_camYawVel += elapsed;
	}

	if (ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 2 ||
		ntAvatarManager::Instance()->m_pMyAvatar->GetMotion() == 3)
	{
		m_camTargetHeight = ntAvatarManager::Instance()
								->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0)
								.getvalue() *
			1.9f;
	}
	else
	{
		m_camTargetHeight = ntAvatarManager::Instance()
								->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0)
								.getvalue() *
			4.6f;
	}

	if (IsEqual(ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition().x,
			ntAvatarManager::Instance()->m_pMyAvatar->m_pos.x) &&
		IsEqual(ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition().z,
			ntAvatarManager::Instance()->m_pMyAvatar->m_pos.z))
	{
		s_moving = false;
	}

	m_bGlow = s_moving;

	ProcessCamera(elapsed);
	ProcessUI(elapsed);

	m_glowTime += elapsed;

	if (IsLocalContent((localContentType_t)0x57))
	{
		RefreshAllUccClothes();
	}
}

void ntAvatarChatMain::ProcessCamera(float elapsed)
{
	WMatrix camera;
	WVector pos;
	WVector dir;

	if (m_bCameraLock && g_SALoadingState)
	{
		m_camTargetDist = ntAvatarManager::Instance()
							  ->m_pMyAvatar->GetMySpItem((eSPAVILITYTYPE)0)
							  .getvalue() *
			28.800001f;
		m_bCameraLock = false;
	}

	if (m_bTopView)
	{
		g_view->SetClip(8.0f, 8192.0f, false);
		g_view->SetFOV(g_PI / 2);
		g_view->UpdateCamera();

		camera = RotMat(WVector(-g_PI / 2, 0.0f, 0.0f));
		camera.pivot = GetPVS().GetGroundPoint(m_topViewPos, false, 0, false);
		camera.pivot.y += m_topViewHeight;
	}
	else
	{
		g_view->SetClip(1.0f, 4096.0f, false);
		g_view->SetFOV(g_PI / 3);
		g_view->UpdateCamera();

		if (!(Wabs(m_camDist - m_camTargetDist) < 1e-5f))
		{
			float step = elapsed * 46.0f;
			if (m_camTargetDist > m_camDist)
			{
				m_camDist += step;
				if (m_camDist > m_camTargetDist)
				{
					m_camDist = m_camTargetDist;
				}
			}
			else
			{
				m_camDist -= step;
				if (m_camDist < m_camTargetDist)
				{
					m_camDist = m_camTargetDist;
				}
			}
		}

		camera = RotMat(WVector(m_camAngle.x, m_camAngle.y, m_camAngle.z));

		if (!(Wabs(m_camHeight - m_camTargetHeight) < 1e-5f))
		{
			float step = elapsed * 5.0f;
			if (m_camTargetHeight > m_camHeight)
			{
				m_camHeight += step;
				if (m_camHeight > m_camTargetHeight)
				{
					m_camHeight = m_camTargetHeight;
				}
			}
			else
			{
				m_camHeight -= step;
				if (m_camHeight < m_camTargetHeight)
				{
					m_camHeight = m_camTargetHeight;
				}
			}
		}

		const WVector& avatarPos =
			ntAvatarManager::Instance()->m_pMyAvatar->m_pos;
		pos = avatarPos;
		pos.y += m_camHeight;

		dir = camera.za * m_camDist;
		dir.Negate();
		float hit = GetPVS().RayIntersectAll(pos, dir, false);
		if (hit > m_camZoom && hit * m_camDist > m_camDefaultDist)
		{
			m_camDefaultDist += elapsed * 16.0f;
			if (m_camDefaultDist >= hit * m_camDist)
			{
				m_camDefaultDist = hit * m_camDist;
				m_camZoom = hit;
			}
		}
		else
		{
			m_camDefaultDist = hit * m_camDist;
			m_camZoom = hit;
		}

		dir.Normalize();
		WVector eye;
		eye = pos + dir * m_camDefaultDist;

		WVector ground = GetPVS().GetGroundPoint(eye, false, 0, false);
		float minY = ground.y + 3.5f;
		if (minY > eye.y)
		{
			eye.y = minY;
		}

		camera.pivot = eye;
	}

	g_camera = camera;
}

void ntAvatarChatMain::PrintText(float x, float y, const char* text,
	float spacing, int align, float scale, unsigned long color)
{
	if (text == 0)
	{
		return;
	}

	int len = strlen(text);
	if (len == 0)
	{
		return;
	}

	char* charSet = m_pFont->GetFontInfo()->pCharSet;

	float oldScale = m_pFont->GetScale();
	m_pFont->SetScale(scale);

	float width = 0.0f;
	const char* p = text;
	switch (align)
	{
	case 1:
	{
		while (*p)
		{
			int c = *p++;

			char* found = strchr(charSet, c);

			width += m_pFont->GetCharWidthA(g_view, found - charSet) +
				spacing * scale;
		}
		width -= spacing;
		width *= 0.5f;
		break;
	}
	case 2:
	{
		while (*p)
		{
			int c = *p++;

			char* found = strchr(charSet, c);

			width += m_pFont->GetCharWidthA(g_view, found - charSet) +
				spacing * scale;
		}
		width -= spacing;
		break;
	}
	}

	x -= width;

	p = text;
	while (*p)
	{
		int c = *p++;

		char* found = strchr(charSet, c);

		float charWidth =
			m_pFont->PutChar(g_view, x, y, found - charSet, 0, color);
		charWidth += spacing;
		x += charWidth;
	}

	m_pFont->SetScale(oldScale);
	m_pFont->Flush(g_view);
}

void ntAvatarChatMain::Display(void)
{
	if (ntAvatarManager::Instance()->m_pMyAvatar == 0)
	{
		return;
	}

	ntAvatarManager::Instance()->Render();

	if (!m_bGlow || m_uiMode == 2)
	{
		return;
	}

	WVector normal, tangent;
	normal = WVector(4.8f, 0.0f, 0.0f);
	tangent = WVector(0.0f, 0.0f, 4.8f);

	if (ntAvatarManager::Instance()->m_pMyAvatar->IsFollowing())
	{
		ntAvatar* pTarget = ntAvatarManager::Instance()->FindAvatarByNickName(
			ntAvatarManager::Instance()->m_pMyAvatar->m_followName);
		if (pTarget == 0)
		{
			return;
		}
		GetPVS().DrawDecal(m_glowTex, pTarget->m_pos, normal, tangent,
			(cosf(m_glowTime * 10.0f) + 1.0f) * 0.05f + 1.0f, 1.0f,
			W_POLY_BLEND_ADD, false);
	}
	else
	{
		GetPVS().DrawDecal(m_glowTex,
			ntAvatarManager::Instance()->m_pMyAvatar->GetTargetPosition(),
			normal, tangent, (cosf(m_glowTime * 10.0f) + 1.0f) * 0.05f + 1.0f,
			1.0f, W_POLY_BLEND_ADD, false);
	}
}

void ntAvatarChatMain::OnDisplay(void)
{
	ntAvatarManager::Instance()->RenderOverlay();

	if (m_uiMode != 2 && !CProjectG::Instance()->HidePI())
	{
		ntBalloonTextManager::Instance()->Render();
	}

	if (m_uiMode == 2 || CProjectG::Instance()->HideGUI() ||
		CProjectG::Instance()->HidePI())
	{
		return;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	const char* roomName =
		MakeStr("[%d] %s", CSharedDoc::Instance()->m_roomInfo.roomGuid,
			CSharedDoc::Instance()->m_roomInfo.title);

	pGDI->SetTextStyle(2);
	pGDI->SetTextColor(0xFFFFFFFF, 0xFF000000);
	pGDI->PrintText(WPoint(20.0f, 48.0f), 0, roomName, 0);

	if (IsLocalContent((localContentType_t)10) &&
		!(CSharedDoc::Instance()->m_myInfo.info.dwIdentity & 4))
	{
		std::map<unsigned long, sBriefUserInfo>::iterator found;
		std::list<sSlotInfo>::iterator end =
			CSharedDoc::Instance()->m_slotList.end();
		for (std::list<sSlotInfo>::iterator it =
				 CSharedDoc::Instance()->m_slotList.begin();
			it != end; ++it)
		{
			sSlotInfo player = *it;

			if (player.dwIdentity & 4)
			{
				found = CSharedDoc::Instance()->m_briefUserInfoMap.find(
					player.dwGuid);
			}
		}
	}

	PrintText(g_view->GetWidth() - 46.0f, 84.0f,
		MakeStr("%2d/%2d", CSharedDoc::Instance()->m_roomInfo.nUserNum,
			CSharedDoc::Instance()->m_roomInfo.nUserLimit),
		1.0f, 2, 1.0f, 0xFFFFFFFF);
}

void ntAvatarChatMain::HandleMsg(const MsgObject& msg)
{
	if ((unsigned long)msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.message)
	{
	case 0x69:

		if (m_pJukeBoxBtn)
		{
			m_pJukeBoxBtn->SetButtonImg(msg.param1 == 1 ? "btn_jukebox_o"
														: "btn_jukebox_n",
				FrButton::NORMAL);
		}
		break;

	case 0xE4:
	{
		const sSlotInfo* pSlotInfo = (const sSlotInfo*)msg.param1;
		const sCharacterInfo* pCharInfo = (const sCharacterInfo*)msg.param2;
		if (ntAvatarManager::Instance()->FindAvatarByNickName(
				pSlotInfo->sNick) == 0)
		{
			if (m_pUserListDlg)
			{
				m_pUserListDlg->UpdateUserList();
			}

			if (ntAvatarManager::Instance()->m_pMyAvatar)
			{
				if (IsLocalContent((localContentType_t)10))
				{
					std::map<unsigned long, sBriefUserInfo>::const_iterator it =
						CSharedDoc::Instance()->m_briefUserInfoMap.find(
							pSlotInfo->dwGuid);
					if ((sBriefUserInfo((*it).second).dwIdentity & 4))
					{
						if ((sBriefUserInfo((*it).second).state & 1))
						{
							std::string text(MakeStr(
								"%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
								pSlotInfo->sNick));
							CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00,
								false, false);
						}
					}
					else
					{
						std::string text(MakeStr(
							"%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
							pSlotInfo->sNick));
						CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false,
							false);
					}
				}
				else
				{
					std::string text(MakeStr(
						"%s \xb4\xd4\xc0\xcc \xc0\xd4\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						pSlotInfo->sNick));
					CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false,
						false);
				}
			}

			ntAvatar* pAvatar = ntAvatarManager::Instance()->CreateAvatar(
				*pSlotInfo, *pCharInfo, this);
			if (pAvatar == 0)
			{
				break;
			}

			static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
				ntAvatarBurden(pSlotInfo->dwUserUID, pSlotInfo->dwGuid,
					*pCharInfo, 0));

			unsigned long myGuid = CSharedDoc::Instance()->m_myInfo.info.dwGuid;
			if (pSlotInfo->dwGuid == myGuid)
			{
				ntAvatarManager::Instance()->m_pMyAvatar = pAvatar;
				CSharedDoc::Instance()->m_myInfo.userEquip.guidChar =
					pCharInfo->guid;
				CSharedDoc::Instance()->SendTradeCloseShop();

				WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(
					MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
				if (point == 0)
				{
					point = GOLFDOC()->m_pPolySoup->FindPoint(
						MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
				}

				WVector pos = point->pos;
				pos.x += Random(-16.0f, 16.0f, -1);
				pos.z += Random(-16.0f, 16.0f, -1);

				pos = GetPVS().GetGroundPoint(pos, false, 0, false);

				if (IsLocalContent((localContentType_t)10))
				{
					std::map<unsigned long, sBriefUserInfo>::const_iterator it =
						CSharedDoc::Instance()->m_briefUserInfoMap.find(
							pSlotInfo->dwGuid);
					if ((sBriefUserInfo((*it).second).dwIdentity & 4))
					{
						if ((sBriefUserInfo((*it).second).state & 1))
						{
							CFxSequence* pSeq = CFx::Instance()->OpenSequence(
								"Talk_Room.seq", true);
							if (pSeq)
							{
								pSeq->Pos() = pos + WVector(0.0f, 5.0f, 0.0f);
							}
						}
					}
					else
					{
						CFxSequence* pSeq = CFx::Instance()->OpenSequence(
							"Talk_Room.seq", true);
						if (pSeq)
						{
							pSeq->Pos() = pos + WVector(0.0f, 5.0f, 0.0f);
						}
					}
				}
				else
				{
					CFxSequence* pSeq =
						CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq)
					{
						pSeq->Pos() = pos + WVector(0.0f, 5.0f, 0.0f);
					}
				}

				ntAvatarManager::Instance()->m_pMyAvatar->SetPosition(pos);
				ntAvatarManager::Instance()->m_pMyAvatar->SetAngle(
					Random(0.0f, g_2_PI, -1));
				ntAvatarManager::Instance()->m_pMyAvatar->Stop();

				WSendPacket packet((enumClientPacket)0x63);
				packet.Encode1(4);
				packet.EncodeBuffer(
					&ntAvatarManager::Instance()->m_pMyAvatar->m_pos.x, 4);
				packet.EncodeBuffer(
					&ntAvatarManager::Instance()->m_pMyAvatar->m_pos.z, 4);
				packet.EncodeBuffer(
					&ntAvatarManager::Instance()->m_pMyAvatar->m_angle, 4);
				packet.Send(TO_GAME);

				ChangePortraitImage(
					ntAvatarManager::Instance()->m_pMyAvatar->m_charTypeId &
					0x3FFFFFF);
			}
			else
			{
				WVector pos =
					GetPVS().GetGroundPoint(WVector(pSlotInfo->location[0],
												0.0f, pSlotInfo->location[1]),
						false, 0, false);
				pAvatar->SetPosition(pos);
				pAvatar->SetAngle(pSlotInfo->location[2]);
			}
		}

		WSendPacket packet((enumClientPacket)0xed);
		packet.Encode4(pSlotInfo->dwGuid);
		packet.Send(TO_GAME);
		break;
	}

	case 0xE5:
	{
		ntAvatar* pAvatar =
			ntAvatarManager::Instance()->FindAvatarByGuid(msg.param1);
		if (pAvatar == 0)
		{
			break;
		}

		if (m_pUserListDlg)
		{
			m_pUserListDlg->UpdateUserList();
		}

		if (ntAvatarManager::Instance()->m_pMyAvatar)
		{
			if (strcmp(pAvatar->GetIDName(),
					ntAvatarManager::Instance()->m_pMyAvatar->GetIDName()) == 0)
			{
				CSharedDoc::Instance()->SendTradeCloseShop();
				ntAvatarManager::Instance()->m_pMyAvatar->SubState(0x7E);
				ntAvatarManager::Instance()->m_pMyAvatar = 0;
			}

			if (IsLocalContent((localContentType_t)10))
			{
				std::map<unsigned long, sBriefUserInfo>::const_iterator it =
					CSharedDoc::Instance()->m_briefUserInfoMap.find(
						pAvatar->GetGuid());
				if ((sBriefUserInfo((*it).second).dwIdentity & 4))
				{
					if ((sBriefUserInfo((*it).second).state & 1))
					{
						std::string text(MakeStr(
							"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
							pAvatar->GetNickName()));
						CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false,
							false);
					}
				}
				else
				{
					std::string text(MakeStr(
						"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
						pAvatar->GetNickName()));
					CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false,
						false);
				}
			}
			else
			{
				std::string text(MakeStr(
					"%s \xb4\xd4\xc0\xcc \xc5\xf0\xc0\xe5\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
					pAvatar->GetNickName()));
				CChatMsg::Instance()->AddChatMsg(text, 0xFFFF00, false, false);
			}
		}

		if (IsLocalContent((localContentType_t)10))
		{
			std::map<unsigned long, sBriefUserInfo>::const_iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(
					pAvatar->GetGuid());
			if (sBriefUserInfo((*it).second).dwIdentity & 4)
			{
				if (sBriefUserInfo((*it).second).state & 1)
				{
					CFxSequence* pSeq =
						CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq)
					{
						pSeq->Pos() =
							pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
					}
				}
			}
			else
			{
				CFxSequence* pSeq =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq)
				{
					pSeq->Pos() = pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
				}
			}
		}
		else
		{
			CFxSequence* pSeq =
				CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pSeq)
			{
				pSeq->Pos() = pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
			}
		}

		static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
			ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_charInfo,
				(CPetFrame*)-1));
		static_cast<ntAvatarChatTask*>(GetTask())->RemoveAvatarBurden(
			ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_charInfo,
				pAvatar->m_pPetFrame));
		ntAvatarManager::Instance()->DeleteAvatar(pAvatar->GetIDName());
		break;
	}

	case 0xE7:

		if (ntAvatarManager::Instance()->m_pMyAvatar)
		{
			strcpy(ntAvatarManager::Instance()->m_pMyAvatar->m_nickName,
				(const char*)msg.param1);
		}
		break;

	case 0xE6:
	{
		ntAvatar* pAvatar =
			ntAvatarManager::Instance()->FindAvatarByGuid(msg.param1);
		break;
	}

	case 0xE8:
	{
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByNickName(
			(const char*)msg.param1);
		if (pAvatar == 0)
		{
			break;
		}

		switch (msg.param2)
		{
		case 0:
		case 2:
			pAvatar->SubState(0x180);
			break;
		case 3:
			pAvatar->SubState(0x80);
			pAvatar->AddState(0x100);
			break;
		case 1:
			pAvatar->AddState(0x80);
			break;
		}
		break;
	}

	case 0xE9:
	{
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByIDName(
			(const char*)msg.param1);
		if (pAvatar)
		{
			pAvatar->SetTradeTitle((const char*)msg.param2);
		}
		break;
	}

	case 0x49:
	{
		int guid = msg.param1;
		const sCharacterInfo* pCharInfo = (const sCharacterInfo*)msg.param2;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar == 0)
		{
			break;
		}

		static_cast<ntAvatarChatTask*>(GetTask())->RemoveAvatarBurden(
			ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, pAvatar->m_charInfo,
				pAvatar->m_pPetFrame));

		pAvatar->SetCharactorInfo(*pCharInfo);

		static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
			ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid, *pCharInfo, 0));

		if (IsLocalContent((localContentType_t)10))
		{
			std::map<unsigned long, sBriefUserInfo>::const_iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(
					pAvatar->GetGuid());
			if ((sBriefUserInfo((*it).second).dwIdentity & 4))
			{
				if ((sBriefUserInfo((*it).second).state & 1))
				{
					CFxSequence* pSeq =
						CFx::Instance()->OpenSequence("Talk_Room.seq", true);
					if (pSeq)
					{
						pSeq->Pos() =
							pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
					}
				}
			}
			else
			{
				CFxSequence* pSeq =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq)
				{
					pSeq->Pos() = pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
				}
			}
		}
		else
		{
			CFxSequence* pSeq =
				CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (pSeq)
			{
				pSeq->Pos() = pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
			}
		}

		if (pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			CSharedDoc::Instance()->m_myInfo.userEquip.guidChar =
				pCharInfo->guid;
			ChangePortraitImage(
				ntAvatarManager::Instance()->m_pMyAvatar->m_charTypeId &
				0x3FFFFFF);
		}
		break;
	}

	case 0x4A:
	{
		int guid = msg.param1;
		const unsigned long* pCaddieId = (const unsigned long*)msg.param2;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			CSharedDoc::Instance()->m_myInfo.userEquip.guidCaddie = *pCaddieId;
		}
		break;
	}

	case 0x4B:
	{
		int guid = msg.param1;
		const unsigned long* pClubId = (const unsigned long*)msg.param2;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			CSharedDoc::Instance()->m_myInfo.userEquip.guidClubSet = *pClubId;
		}
		break;
	}

	case 0x4C:
	{
		int guid = msg.param1;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			CSharedDoc::Instance()->m_myInfo.userEquip.tidBall = msg.param2;
		}
		break;
	}

	case 0x4E:
	{
		int guid = msg.param1;
		const unsigned long* pMascotId = (const unsigned long*)msg.param2;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar && pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			CSharedDoc::Instance()->m_myInfo.userEquip.guidMascot = *pMascotId;
		}
		break;
	}

	case 0x24E:
	{
		eSPAVILITYTYPE type = (eSPAVILITYTYPE)msg.param2;
		int guid = msg.param1;
		float value = (float)msg.param3;
		ntAvatar* pAvatar = ntAvatarManager::Instance()->FindAvatarByGuid(guid);
		if (pAvatar == 0)
		{
			break;
		}

		if (type == 0)
		{
			if (ntAvatarManager::Instance()->m_pMyAvatar->m_guid == guid)
			{
				m_bCameraLock = true;
				g_SALoadingState = false;
			}
		}

		if (type != 3)
		{
			sCharacterInfo charInfo = pAvatar->m_charInfo;
			static_cast<ntAvatarChatTask*>(GetTask())->RemoveAvatarBurden(
				ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid,
					pAvatar->m_charInfo, pAvatar->m_pPetFrame));
			pAvatar->SetCharactorInfo(charInfo);
			static_cast<ntAvatarChatTask*>(GetTask())->AddAvatarBurden(
				ntAvatarBurden(pAvatar->m_uid, pAvatar->m_guid,
					pAvatar->m_charInfo, 0));
		}

		pAvatar->GetMySpItem(type).changeflag();
		pAvatar->GetMySpItem(type).setvalue(value);

		if (IsLocalContent((localContentType_t)10))
		{
			std::map<unsigned long, sBriefUserInfo>::const_iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(
					pAvatar->GetGuid());
			if ((sBriefUserInfo((*it).second).dwIdentity & 4))
			{
				if ((sBriefUserInfo((*it).second).state & 1))
				{
					StartSpEffect(pAvatar, type);
				}
			}
			else
			{
				StartSpEffect(pAvatar, type);
			}
		}
		else
		{
			StartSpEffect(pAvatar, type);
		}
		break;
	}

	case 0x0F:
	{
		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_slotList;
		for (std::list<sSlotInfo>::iterator it = players.begin();
			it != players.end(); ++it)
		{
			if (it->bMaster & 1)
			{
				m_masterOID = it->dwGuid;
				break;
			}
		}
		break;
	}

	case 0x5F:
	{
		if (msg.param2 != 0xFFFF)
		{
			break;
		}
		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_slotList;
		for (std::list<sSlotInfo>::iterator it = players.begin();
			it != players.end(); ++it)
		{
			if (it->dwGuid == (unsigned long)msg.param1)
			{
				it->bMaster |= 1;
				it->bReady = 1;
			}
			else
			{
				it->bMaster &= ~1;
			}
		}
		m_masterOID = msg.param1;
		break;
	}

	case 0x5B:
	{
		ntAvatar* pAvatar =
			ntAvatarManager::Instance()->FindAvatarByGuid(msg.param1);
		if (pAvatar == 0 || pAvatar == ntAvatarManager::Instance()->m_pMyAvatar)
		{
			break;
		}

		switch (msg.param2)
		{
		case 4:
		{
			WVector pos(*(const float*)msg.param3, 0.0f,
				*(const float*)msg.param4);
			pos = GetPVS().GetGroundPoint(pos, false, 0, false);
			pAvatar->SetPosition(pos);
			pAvatar->SetAngle(*(const float*)msg.param5);

			if (IsLocalContent((localContentType_t)10))
			{
				std::map<unsigned long, sBriefUserInfo>::const_iterator it =
					CSharedDoc::Instance()->m_briefUserInfoMap.find(msg.param1);
				if ((sBriefUserInfo((*it).second).dwIdentity & 4))
				{
					if ((sBriefUserInfo((*it).second).state & 1))
					{
						CFxSequence* pSeq = CFx::Instance()->OpenSequence(
							"Talk_Room.seq", true);
						if (pSeq)
						{
							pSeq->Pos() =
								pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
						}
					}
				}
			}
			else
			{
				CFxSequence* pSeq =
					CFx::Instance()->OpenSequence("Talk_Room.seq", true);
				if (pSeq)
				{
					pSeq->Pos() = pos + WVector(0.0f, 5.0f, 0.0f);
				}
			}
			break;
		}
		case 5:
			pAvatar->SetAction(msg.param3);
			break;
		case 6:
		{
			WVector target = pAvatar->GetTargetPosition();
			target.x += *(const float*)msg.param3;
			target.z += *(const float*)msg.param4;
			pAvatar->SetTargetPosition(target);
			pAvatar->SetPendingTargetAngle(*(const float*)msg.param5);
			break;
		}
		case 7:
			pAvatar->ChangeMotion(4, (const char*)msg.param3, 0.2f);
			break;
		case 8:
			pAvatar->SetState(msg.param3);
			break;
		case 9:
		{
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(msg.param1);
			if (it != CSharedDoc::Instance()->m_briefUserInfoMap.end())
			{
				if (m_pUserListDlg)
				{
					m_pUserListDlg->UpdateUserList();
				}
			}
			break;
		}
		}
		break;
	}

	case 0x3D:
	{
		std::list<sSlotInfo>& players = CSharedDoc::Instance()->m_slotList;
		std::list<sSlotInfo>::iterator it =
			std::find(players.begin(), players.end(), msg.param1);
		if (it == players.end())
		{
			break;
		}
		it->bSleep = (msg.param2 == 1);
		break;
	}

	case 0x62:

		if (m_pChatFrame)
		{
			m_pChatFrame->SetVisible(msg.param1 != 0);
		}

		if (m_pChatTarget)
		{
			m_pChatTarget->SetVisible(msg.param1 != 0);
		}

		if (m_pChatInput)
		{
			if (msg.param1)
			{
				m_pChatInput->SetVisible(true);
				m_pChatInput->SetKeyFocus(true);
			}
			else
			{
				m_pChatInput->SetVisible(false);
			}
			g_pFresh->GetManager()->SetExclusiveKey(msg.param1 != 0);
		}

		if (m_pEmoticonBtn)
		{
			m_pEmoticonBtn->SetVisible(msg.param1 != 0);
		}

		if (m_pLanguage)
		{
			m_pLanguage->SetVisible(msg.param1 != 0);
		}

		if (m_pReportBtn)
		{
			m_pReportBtn->SetVisible(msg.param1 != 0);
		}
		break;

	case 0x3F:

		*(const char**)msg.param1 =
			m_pChatInput ? m_pChatInput->GetLine(1, false) : 0;
		break;

	case 0x37:

		if (CUserInfo::Instance()->IsVisible())
		{
			CUserInfo::Instance()->Close();
		}

		if (CSharedDoc::Instance()->AddWhisperPartner(m_pChatTarget,
				(const char*)msg.param1))
		{
			m_pChatTarget->SetLine(1, (const char*)msg.param1, 0, false, 0);
		}

		if (CChatMsg::Instance()->IsClosed())
		{
			CChatMsg::Instance()->Open(true, true);
		}

		if (m_pChatInput)
		{
			m_pChatInput->SetKeyFocus(true);
		}
		break;

	case 0x65:

		if (m_pChatTarget)
		{
			if (CSharedDoc::Instance()->m_recentWhisperList.size() > 0)
			{
				m_pChatTarget->SetLine(1,
					CSharedDoc::Instance()->m_recentWhisperList.back().c_str(),
					0, false, 0);
			}
		}
		break;

	case 0x38:

		CSharedDoc::Instance()->AddWhisperPartner(m_pChatTarget,
			(const char*)msg.param1);
		break;

	case 0x47:

		CSharedDoc::Instance()->InitRecentWhisperList(m_pChatTarget);
		break;

	case 0x64:

		if (m_pChatView)
		{
			if (m_pChatView->GetLineNum() > 50)
			{
				m_pChatView->DeleteFirstLine();
				m_pChatView->GetScrollBar()->DelItem();
			}
			m_pChatView->AddLine((const char*)msg.param1, msg.param2, false);
			if (m_chatLineCount > 0)
			{
				m_pChatView->DeleteFirstLine();
				m_pChatView->GetScrollBar()->DelItem();
				m_chatLineCount--;
			}
		}

		if (m_pChatLogDlg)
		{
			static_cast<FrAvatarChatLogDlg*>(m_pChatLogDlg)
				->AddChatMsg((const char*)msg.param1, msg.param2);
		}
		break;

	case 0x63:

		if (m_pChatView)
		{
			m_pChatView->ClearLine();
			for (int i = 0; i < 8; i++)
			{
				m_pChatView->AddLine("", 0, false);
			}
		}
		break;

	case 0x60:
	{
		const char* nickName = (const char*)msg.param1;
		const char* text = (const char*)msg.param2;
		if (nickName == 0 && ntAvatarManager::Instance()->m_pMyAvatar)
		{
			nickName = ntAvatarManager::Instance()->m_pMyAvatar->GetNickName();
		}
		ntAvatar* pAvatar =
			ntAvatarManager::Instance()->FindAvatarByNickName(nickName);
		if (pAvatar == 0)
		{
			break;
		}

		if (SpCommand(text))
		{
			break;
		}
		int face = CSharedDoc::Instance()->m_chatManager.GetFaceIndex(text);
		if (face != 5)
		{
			pAvatar->SetChatFacial(face);
		}
		const char* motionName =
			CSharedDoc::Instance()->m_chatManager.GetMotionName(text,
				pAvatar->m_charTypeId);
		if (motionName && !pAvatar->IsHoldState())
		{
			pAvatar->ChangeMotion(4, motionName, 0.2f);
		}
		break;
	}

	case 0x00:

		if (g_pFresh)
		{
			g_pFresh->OpenLayout((const char*)msg.param1, this, false);
		}
		break;

	case 0x16:
		m_bLayoutFlag = msg.param1 != 0;
		break;

	case 0x17:
		*(int*)msg.param1 = m_bLayoutFlag;
		break;

	case 0x27:
		strcpy(m_layoutName, (const char*)msg.param1);
		break;

	case 0x28:
		*(char**)msg.param1 = m_layoutName;
		break;

	case 0x3B:

		switch ((unsigned char)msg.param1)
		{
		case 2:
			this << MsgObject(0, 0x23,
				(int)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 3:
			this << MsgObject(0, 0x23,
				(int)"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 4:
			this << MsgObject(0, 0x23,
				(int)"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xb0\xa1 \xc6\xb2\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 7:
		case 9:
			this << MsgObject(0, 0x23,
				(int)"\xb9\xe6\xb8\xb8\xb5\xe9\xb1\xe2 \xbf\xe4\xc3\xbb\xc0\xcc \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 8:
			this << MsgObject(0, 0x23,
				(int)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 5:
			this << MsgObject(0, 0x23,
				(int)"\xc7\xf6\xc0\xe7 \xb7\xb9\xba\xa7\xbf\xa1\xbc\xad\xb4\xc2 \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe9\xb0\xc5\xb3\xaa \xb9\xe6\xbf\xa1 \xc0\xd4\xc0\xe5\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;
		case 0xD:
			this << MsgObject(0, 0x23,
				(int)"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xd8\xbe\xdf \xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;
		case 0xF:
			this << MsgObject(0, 0x23,
				(int)"\xc6\xce\xb9\xe8\xc6\xb2\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
			break;
		case 0x11:
			this << MsgObject(0, 0x23,
				(int)"\xbe\xee\xc7\xc1\xb7\xce\xc4\xa1\xb0\xd4\xc0\xd3\xbf\xa1 \xc7\xca\xbf\xe4\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
				0, 0, 0, 0);
			break;
		}
		break;

	case 0x1F9:
	{
		if (CQuitWindow::Instance()->IsActive())
		{
			break;
		}

		bool hideGUI = m_uiMode == 2 || CProjectG::Instance()->HideGUI();
		if (g_pFresh)
		{
			g_pFresh->Toggle(!hideGUI);
		}

		bool hideBar = m_uiMode == 1 || CProjectG::Instance()->HideGUI();
		ShowOverAndUnderBar(!hideBar);

		bool hideChat = (m_uiMode == 2 || CProjectG::Instance()->HidePI()) &&
			!CChatMsg::Instance()->IsClosed();
		CChatMsg::Instance()->Toggle(!hideChat);

		ntAvatarManager::Instance()->m_bShowName =
			m_uiMode == 0 && !CProjectG::Instance()->HidePI();

		bool hideCursor = m_uiMode == 2 || CProjectG::Instance()->HideGUI() ||
			CProjectG::Instance()->HidePI();
		CMouseCursor::Instance()->Toggle(!hideCursor);

		bool hidePI = m_uiMode == 2 || CProjectG::Instance()->HidePI();
		if (m_pUserListBtn)
		{
			m_pUserListBtn->SetVisible(!hidePI);
		}

		if (m_pOverBarPangCookie)
		{
			m_pOverBarPangCookie->SetVisible(!hidePI);
		}
		break;
	}

	case 0x03:
	case 0x31:
	case 0x34:
	case 0x3C:
	case 0x211:
		break;

	default:
		HandleMsgCommon(msg);
		break;
	}
}

void ntAvatarChatMain::RefreshAllUccClothes(void)
{
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++)
	{
		if (!UccManager()->GetRefreshItem(item))
		{
			break;
		}

		int count = 0;
		for (int j = 0; j < 30; j++)
		{
			if (count >= ntAvatarManager::Instance()->m_avatarCount)
			{
				break;
			}
			ntAvatar* pAvatar = ntAvatarManager::Instance()->m_avatar[j];

			if (pAvatar == 0)
			{
				continue;
			}

			count++;
			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1F);
			if (pAvatar->GetPartTidList().m_tid[slot] != item.typeId)
			{
				continue;
			}
			unsigned long serial = pAvatar->GetPartTidList().m_uid[slot];

			sUccClothes* pClothes = UccManager()->FindClothes(serial);

			if (pClothes != 0 &&
				strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
			{
				pAvatar->RefreshUccClothes(pClothes->typeId, pClothes->id);
			}
		}
	}
}

CQuadTree& ntAvatarChatMain::GetPVS(void)
{
	return *m_pPVS;
}

void ntAvatarChatMain::SetLocalPVS(CQuadTree& pvs)
{
	m_pPVS = &pvs;
}

void ntAvatarChatMain::StartSpEffect(ntAvatar* pAvatar, eSPAVILITYTYPE type)
{
	CFxSequence* pSeq = NULL;
	bool bOn = pAvatar->GetMySpItem(type).getflag();

	switch (type)
	{
	case 0:
		if (bOn)
		{
			CFx* fx = CFx::Instance();
			pSeq = fx->OpenSequence("change_up-big.seq", true);

			break;
		}

		if (pAvatar->GetMySpItem((eSPAVILITYTYPE)1).getflag())
		{
			pSeq = CFx::Instance()->OpenSequence("change_up-middle.seq", true);
		}
		else
		{
			pSeq = CFx::Instance()->OpenSequence("change_up-small.seq", true);
		}

		break;

	case 1:
		if (bOn)
		{
			if (pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag())
			{
				pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
			}
			else
			{
				pSeq =
					CFx::Instance()->OpenSequence("change_up-middle.seq", true);
			}
		}
		else
		{
			if (!pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag())
			{
				pSeq =
					CFx::Instance()->OpenSequence("change_up-small.seq", true);
			}
			else
			{
				pSeq = CFx::Instance()->OpenSequence("change_up-big.seq", true);
			}
		}
		break;

	case 2:
		if (pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getflag())
		{
			CFx* fx = CFx::Instance();
			pSeq = fx->OpenSequence("change_up-big.seq", true);

			break;
		}

		if (pAvatar->GetMySpItem((eSPAVILITYTYPE)1).getflag())
		{
			pSeq = CFx::Instance()->OpenSequence("change_up-middle.seq", true);
		}
		else
		{
			pSeq = CFx::Instance()->OpenSequence("change_up-small.seq", true);
		}

		break;

	case 3:
	{
		float height =
			pAvatar->GetMySpItem((eSPAVILITYTYPE)0).getvalue() * 10.0f;
		pSeq = CFx::Instance()->OpenSequence("BFX_B2.seq", true);
		if (pSeq)
		{
			pSeq->Pos() = WVector(pAvatar->m_pos.x + 1.0f,
				pAvatar->m_pos.y + height, pAvatar->m_pos.z + 1.0f);
		}
		pSeq = CFx::Instance()->OpenSequence("BFX_B3.seq", true);
		if (pSeq)
		{
			pSeq->Pos() = WVector(pAvatar->m_pos.x + 1.0f,
				pAvatar->m_pos.y + height, pAvatar->m_pos.z + 1.0f);
		}
		g_audio->PlaySfx("Ball_B", 0, 0, 0, 0, 0.5f, 200.0f);
		return;
	}

	default:
		break;
	}

	if (pSeq)
	{
		pSeq->Pos() = pAvatar->m_pos + WVector(0.0f, 5.0f, 0.0f);
	}
}

bool ntAvatarChatMain::SpCommand(const char* command)
{
	bool handled = false;
	static unsigned long s_lastTime = timeGetTime() - 1000;
	int len = strlen(command + 1);

	if (len == 0 || command[0] != '/')
	{
		return false;
	}

	for (int i = 0; i < 5; i++)
	{
		if (strcmp(command + 1, s_commands[i]) == 0)
		{
			handled = true;

			if (i == 3)
			{
				if (timeGetTime() - s_lastTime >= 1000)
				{
					s_lastTime = timeGetTime();
				}
				else
				{
					CChatMsg::Instance()->AddChatMsg(
						"\xc1\xf6\xb1\xdd\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9",
						0xFFFF7878, true, false);
					return true;
				}
			}

			if (CanUseSPItem((eSPAVILITYTYPE)i))
			{
				ntAvatarManager::Instance()->m_pMyAvatar->UseSPItem(
					(eSPAVILITYTYPE)i);
			}
			else
			{
				CChatMsg::Instance()->AddChatMsg(
					"\xc7\xd8\xb4\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xc2\xf8\xbf\xeb\xc7\xcf\xc1\xf6 \xbe\xca\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9",
					0xFFFF7878, true, false);
			}
			break;
		}
	}

	return handled;
}

bool ntAvatarChatMain::CanUseSPItem(eSPAVILITYTYPE type)
{
	const sCharacterInfo& info =
		ntAvatarManager::Instance()->m_pMyAvatar->GetCharacterInfo();
	for (int i = 0; i < 24; i++)
	{
		if (info.tidParts[i] != 0)
		{
			IFF_STRUCT::sSpecialPrizeItem* pItem =
				ItemManager()->FindSPItem(info.tidParts[i], type);
			if (pItem != 0)
			{
				return true;
			}
		}
	}

	for (int j = 0; j < 5; j++)
	{
		if (info.tidAuxParts[j] != 0)
		{
			IFF_STRUCT::sSpecialPrizeItem* pItem =
				ItemManager()->FindSPItem(info.tidAuxParts[j], type);
			if (pItem != 0)
			{
				return true;
			}
		}
	}

	return false;
}
