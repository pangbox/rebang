#include "stdafx.h"
#include "mathconsts.h"

#include "networksystem.h"
#include "realmyroommain.h"
#include "tooltip.h"
#include "anotherhand.h"
#include "objectfactory.h"
#include "actor.h"
#include "taskmanager.h"
#include "shareddoc.h"
#include "golfdoc.h"
#include "clientsetting.h"
#include "shadow.h"
#include "fresh.h"
#include "wresrcmng.h"
#include "avatar_task.h"
#include "frwnd.h"
#include "wview.h"
#include "realmyroom_avatar.h"
#include "realmyroomdata.h"
#include "mousecursor.h"
#include "ccardbook.h"
#include "fx.h"
#include "fxbox.inl"
#include "../../shared/localize.h"
#include "inputmanager.h"
#include "woverlay.h"
#include "frwndmanager.h"
#include "transferableitem.h"
#include "quadtree.h"
#include "uccmanager.h"
#include "exhibition.h"
#include "partsinfo.h"
#include "sky.h"
#include "commonutil.h"
#include "camerawork.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "realmyroomtask.h"
#include "wind.h"
#include "../../shared/sharedtables.h"
#include "s5/utilities.h"
#include "frdesktop.h"
#include "messengerdlg.h"
#include "fredit.h"
#include "standinginline.h"

extern bool g_bQuit;

IMPLEMENT_ACTOR(CRealMyRoomMain, CTaskMain)

CRealMyRoomMain::CRealMyRoomMain(void)
{
	m_dispPriority = DISP_PRIORITY_4;

	m_glowTime = 0.0f;
	m_camLookAt = WVector::ZERO;
	m_camAngle = WVector(0.0f, 0.0f, 0.0f);
	m_fov = 28.800001f;
	m_targetFov = 28.800001f;
	m_defaultFov = 28.800001f;

	m_motionTime = 0.0f;
	m_reserved248 = 0;
	m_camDistance = 1.0f;
	m_camHeight = 4.6f;
	m_avatarCamHeight = 4.6f;

	m_startPos = WVector::ZERO;
	m_bTopView = false;
	m_topViewFov = 256.0f;

	m_camLimit[0] = -40.0f;
	m_camLimit[1] = 40.0f;
	m_camLimit[2] = -25.0f;
	m_camLimit[3] = 25.0f;
	m_camLimitType = 0;

	m_newYearMoneyUid = 0;

	m_pTitleFont = g_resrcmng->GetTitleFont();
	m_bAvatarMove = false;
	m_glowBallTex = g_resrcmng->LoadTexture("~GlowBall.jpg", 0, 0, 0);

	tagWTITLEFONT font;
	char* fileNames[1] = { "[font_wind.jpg" };
	memset(&font, 0, sizeof(font));

	font.filename = fileNames;
	font.fonth = 16;
	font.fontw = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:, ";
	m_pTitleFont->Create(&font);

	m_pLayout = NULL;
	m_pFurnitureCamera = NULL;
	m_pDefaultCamera = NULL;
	m_pDefaultCamera2 = NULL;
	m_pRoomCamera = NULL;
	m_pLimitPoint = NULL;

	m_pPointFx = NULL;
	m_bCamTarget = false;

	m_camDisplayType = 1;

	m_nearClip = g_view->GetClipNearValue();
	m_farClip = g_view->GetClipFarValue();

	m_pRightMenu = NULL;
	if (IsLocalContent((localContentType_t)0x51))
	{
		m_cardTypeId = 0;
		LoadCardImages();
	}

	m_pCardBook = new CCardBook;

	m_pCardPackList = NULL;

	m_cutinImg.clear();

	m_reserved2b8 = -1;
}

CRealMyRoomMain::~CRealMyRoomMain(void)
{
	Net_EquipDresserParts();
	Net_EquipCharacter();
	Net_EquipCaddie();
	Net_EquipItem();
	Net_EquipGear();
	Net_EquipSkin();

	Net_EquipMascot();

	if (IsLocalContent((localContentType_t)0x51))
	{
		DeleteCardImages();
		CSharedDoc::Instance()->SendEquipItemSlot();
	}
	else
	{
		CSharedDoc::Instance()->SendEquipItemSlotByMascot();
	}

	HidePet(true);

	g_resrcmng->UnlockAllByThread(m_anotherHand.GetThreadID());
	m_anotherHand.BreakWorkingThread();
	this << MsgObject(NULL, 2, 0, 0, 0, 0, 0);

	if (m_pPointFx)
	{
		CFx::Instance()->CloseSequence(m_pPointFx, false);
	}

	if (g_resrcmng && m_glowBallTex)
	{
		g_resrcmng->Release(m_glowBallTex);
		m_glowBallTex = 0;
	}

	if (g_resrcmng && m_pTitleFont)
	{
		g_resrcmng->Release(m_pTitleFont);
		m_pTitleFont = NULL;
	}

	CSceneManager::Instance()->ReleaseElement();

	if (m_pCardBook)
	{
		delete m_pCardBook;
		m_pCardBook = NULL;
	}

	DeleteCutinImages();
}

void CRealMyRoomMain::OnPreLoadInit(void)
{
	CTaskMain::OnPreLoadInit();

	if (!IS_EXACTKINDOF(ntAvatarChatTask,
			CTaskManager::Instance()->GetPreservedTask()))
	{
		new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());
	}

	CSharedDoc::Instance()->m_gameMode = 0;

	InitMyRoomControls();
	InitMyRoomDialogs();
	InitMyRoomVariables();

	GOLFDOC()->MakePlayer(1);
}

void CRealMyRoomMain::DoReLoad(void)
{
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	AfxGetTask()->Init(0);
}

void CRealMyRoomMain::OnInit(void)
{
	CTaskMain::OnInit();

	SetBGMNone();

	CSceneManager::Instance()->UpdateFog(true);

	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, this,
		"\xb8\xae\xbe\xf3\xb8\xb6\xc0\xcc\xb7\xeb", 0, 0);

	g_input->Reset();

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		m_camLookAt = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos;
		m_camLookAt.y += 4.6f;
		m_camLookAt.z -= 46.0f;
	}

	m_charInfoMap = CSharedDoc::Instance()->m_charMap;
	m_userEquip = CSharedDoc::Instance()->m_myInfo.userEquip;

	if (CMouseCursor::Instance() != NULL)
	{
		CMouseCursor::Instance()->SetActive(true);
		CMouseCursor::Instance()->SetMode(2);
	}

	CCameraWork::Instance()->SelectCamera(200);
	CCameraWork::Instance()->SetManualType(0);

	g_camera.Rotate(-0.17453292f, 0);

	m_pDefaultCamera->mat = g_camera;
	m_pDefaultCamera->mat.pivot.y = g_camera.pivot.y - 5.0f;
	m_pDefaultCamera->mat.Rotate(0.34906584f, 1);
	m_pDefaultCamera->mat.pivot.x -= 5.0f;

	m_pDefaultCamera->fov = g_view->GetFOV();
	m_savedCamMat = g_camera;
	m_savedCamFov = g_view->GetFOV();
	m_savedCamMat.pivot.y -= 5.0f;
	g_camera.pivot.y -= 7.0f;
	m_uiHideMode = 0;
	g_view->SetFOV(g_PI / 4);
}

void CRealMyRoomMain::OnLoad(void)
{
	WPolySoup::sCamera camera;
	sSlotInfo slotInfo;
	sCharacterInfo charInfo;
	WVector pos;

	CCameraWork::Instance()->Load();

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME))
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
		m_pDefaultCamera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		m_pDefaultCamera2 = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");

		m_targetFov = m_fov = camera->fov;

		m_pRoomCamera = GOLFDOC()->m_pPolySoup->FindCamera(
			"\xc4\xab\xb8\xde\xb6\xf3\xb3\xa1\xc1\xa1");
		m_pFurnitureCamera =
			GOLFDOC()->m_pPolySoup->FindCamera("\xc3\xca\xb1\xe2\xb0\xaa");
		m_pLimitPoint =
			GOLFDOC()->m_pPolySoup->FindPoint("\xbd\xc3\xc0\xdb\xc1\xa1");

		for (int i = 0; i < 4; i++)
		{
			char name[256];
			sprintf(name, "savecamera%d", i);

			m_pSaveCamera[i] = GOLFDOC()->m_pPolySoup->FindCamera(name);
		}
		m_pUICamera = GOLFDOC()->m_pPolySoup->FindCamera("uicamera0");

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(
			MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
		if (point == 0)
		{
			point = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xb4\xeb\xc8\xad\xb9\xe6\xbd\xc3\xc0\xdb"));
		}

		WVector pos = point->pos;
		pos = GetPVS().GetGroundPoint(pos, false, 0, false);
		GetPVS().SetAvatarPosition(pos, 2.0f);
		m_startPos = pos;

		m_pPointFx = CFx::Instance()->OpenSequence("room_point.seq", true);
		if (m_pPointFx)
		{
			m_pPointFx->m_pos = WVector(pos.x, pos.y, pos.z);
		}

		m_camTarget = pos;

		CSceneManager::Instance()->SetWeather(0, false);

		CRealMyRoom_AvatarManager::Instance()->FreeAllAvatars();
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
		m_pDefaultCamera = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");
		m_pDefaultCamera2 = GOLFDOC()->m_pPolySoup->FindCamera("DefaultCamera");

		m_targetFov = m_fov = camera->fov;

		m_pRoomCamera = GOLFDOC()->m_pPolySoup->FindCamera(
			"\xc4\xab\xb8\xde\xb6\xf3\xb3\xa1\xc1\xa1");
		m_pFurnitureCamera =
			GOLFDOC()->m_pPolySoup->FindCamera("\xc3\xca\xb1\xe2\xb0\xaa");
		m_pLimitPoint =
			GOLFDOC()->m_pPolySoup->FindPoint("\xbd\xc3\xc0\xdb\xc1\xa1");

		for (int i = 0; i < 4; i++)
		{
			char name[256];
			sprintf(name, "savecamera%d", i);

			m_pSaveCamera[i] = GOLFDOC()->m_pPolySoup->FindCamera(name);
		}
		m_pUICamera = GOLFDOC()->m_pPolySoup->FindCamera("uicamera0");

		CSceneManager::Instance()->SetWeather(
			COption::Instance()->dGetWeather(), false);

		CRealMyRoom_AvatarManager::Instance()->FreeAllAvatars();

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(
			MakeStr("\xb8\xb6\xc0\xcc\xb7\xeb\xbd\xc3\xc0\xdb\xc1\xa1"));
		if (point == 0)
		{
			point = GOLFDOC()->m_pPolySoup->FindPoint(
				MakeStr("\xbd\xc3\xc0\xdb\xc1\xa1"));
		}

		pos = point->pos;
		pos = GetPVS().GetGroundPoint(pos, false, 0, false);
		m_startPos = pos;

		GetPVS().SetAvatarPosition(pos, 2.0f);

		m_camTarget = pos;
		m_pPointFx = CFx::Instance()->OpenSequence("room_point.seq", true);

		if (m_pPointFx)
		{
			// HACK!
			m_pPointFx->m_pos = WVector(pos.x, (const volatile float&)pos.y,
				(const volatile float&)pos.z);
		}

		memset(&slotInfo, 0, sizeof(slotInfo));

		slotInfo.location[2] = 0.0f;
		slotInfo.bMaster = 1;
		slotInfo.dwGuid = 0;
		slotInfo.dwTitle = 0;
		slotInfo.level = 1;
		rand();
		slotInfo.location[0] = pos.x;
		slotInfo.location[1] = pos.z;
		slotInfo.gender = 0;
		strcpy(slotInfo.sNick, "bianca");
		slotInfo.tidChar =
			COption::Instance()->dGetDefaultCharacter() | 0x4000000;
		CSharedDoc::Instance()->m_slotList.push_back(slotInfo);

		memset(&charInfo, 0, sizeof(charInfo));
		charInfo.tid = slotInfo.tidChar;

		_util::GenerateRandomTidParts(charInfo.tid, charInfo.tidParts,
			charInfo.tidAuxParts);

		CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar =
			CRealMyRoom_AvatarManager::Instance()->CreateAvatar(slotInfo,
				charInfo, this);
		static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(
			CRealMyRoom_AvatarBurden(0, charInfo, 0));

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
			slotInfo.location[2] = Random(0.0f, g_PI * 2, -1);
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

			CRealMyRoom_AvatarManager::Instance()->CreateAvatar(slotInfo,
				charInfo, this);
			static_cast<CRealMyRoomTask*>(GetTask())->AddAvatarBurden(
				CRealMyRoom_AvatarBurden(i, charInfo, 0));
		}

		CRealMyRoomData::Instance()->OnLoad();
	}

	m_light = g_lightset;

	GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());

	Wind().Init(0xFF, 0xFF, 0, 1);

	Wind().CheckSpecialWind();
	Wind().SetWind(WVector(1.0f, 0.0f, 1.0f));
	GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
	CSceneManager::Instance()->UpdateWind();

	m_shadowTex = g_resrcmng->LoadTexture("[shadow.jpg", 0, 0, 0);

	CSceneManager::Instance()->ToogleNoOverlapCamera();

	Waabb box;
	GetPVS().GetAABB(box);

	float top = box.max.y;
	WPolySoup::sModel* wall = GOLFDOC()->m_pPolySoup->FindModel("wall_a.pet");
	if (wall)
	{
		top = wall->aabb.max.y;
	}

	m_roomBound[0] = WVector(box.max.x, top, box.min.z);

	m_roomBound[1] = WVector(box.max.x, box.min.y, box.min.z);
	m_roomBound[2] = WVector(box.max.x, (box.min.y + top) * 0.5f, box.min.z);

	m_roomBound[3] = WVector(box.min.x, top, box.min.z);
	m_roomBound[4] = WVector(box.min.x, box.min.y, box.min.z);
	m_roomBound[5] = WVector(box.min.x, (box.min.y + top) * 0.5f, box.min.z);

	m_roomBound[6] = WVector((box.min.x + box.max.x) * 0.5f, top, box.min.z);
	m_roomBound[7] =
		WVector((box.min.x + box.max.x) * 0.5f, box.min.y, box.min.z);

	m_pSexMark[0] = g_resrcmng->GetOverlay("i_male.tga", 0);
	m_pSexMark[1] = g_resrcmng->GetOverlay("i_female.tga", 0);
	m_pSexMark[2] = g_resrcmng->GetOverlay("i_male_02.tga", 0);
	m_pSexMark[3] = g_resrcmng->GetOverlay("i_female_02.tga", 0);
	m_pSexMark[4] = g_resrcmng->GetOverlay("i_male_03.tga", 0);
	m_pSexMark[5] = g_resrcmng->GetOverlay("i_female_03.tga", 0);

	m_pMannerMark[0] = g_resrcmng->GetOverlay("i_male_manner.tga", 0);
	m_pMannerMark[1] = g_resrcmng->GetOverlay("i_female_manner.tga", 0);

	m_pAngelMark[0] = g_resrcmng->GetOverlay("i_male_angel.tga", 0);
	m_pAngelMark[1] = g_resrcmng->GetOverlay("i_female_angel.tga", 0);

	CTaskMain::OnLoad();
	LoadMyRoomImages();
}

void CRealMyRoomMain::OnDestroy(void)
{
	CTaskMain::OnDestroy();

	if (!IS_EXACTKINDOF(ntAvatarChatTask,
			CTaskManager::Instance()->GetPreservedTask()))
	{
		if (CShadowManager::Instance())
		{
			delete CShadowManager::Instance();
		}
	}

	int i;
	for (i = 0; i < 6; i++)
	{
		if (g_resrcmng && m_pSexMark[i])
		{
			g_resrcmng->Release(m_pSexMark[i]);
			m_pSexMark[i] = 0;
		}
	}

	for (i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_pMannerMark[i])
		{
			g_resrcmng->Release(m_pMannerMark[i]);
			m_pMannerMark[i] = 0;
		}
	}

	for (i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_pAngelMark[i])
		{
			g_resrcmng->Release(m_pAngelMark[i]);
			m_pAngelMark[i] = 0;
		}
	}

	if (g_resrcmng && m_shadowTex)
	{
		g_resrcmng->Release(m_shadowTex);
		m_shadowTex = 0;
	}

	int guids[30];
	int count = 0;
	for (i = 0;
		i < 30 && count < CRealMyRoom_AvatarManager::Instance()->m_avatarCount;
		i++)
	{
		if (CRealMyRoom_AvatarManager::Instance()->m_avatar[i])
		{
			guids[count++] =
				CRealMyRoom_AvatarManager::Instance()->m_avatar[i]->m_guid;
		}
	}

	for (i = 0; i < count; i++)
	{
		this << MsgObject(this, 0xE5, guids[i], 0, 0, 0, 0);
	}

	g_pFresh->GetManager()->SetExclusiveKey(true);
}

void CRealMyRoomMain::OnProcess(float elapsed)
{
	static bool s_bRotating;
	static bool s_bDragging;
	static bool s_bMoving;
	static bool s_bKeyMove;
	static WVector s_keyMoveDir = WVector::UNIT_POS_Z;
	static WVector s_cameraFront = WVector::UNIT_POS_X;

	CTaskMain::OnProcess(elapsed);

	GetPVS().PreProcess(elapsed, false);

	if (!CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		return;
	}

	if (g_pFresh->GetManager()->GetDesktop()->m_nFlags.GetFlag(0x8))
	{
		int leftButton = g_input->GetButton(LEFT_BUTTON);
		int rightButton = g_input->GetButton(RIGHT_BUTTON);
		int middleButton = g_input->GetButton(MIDDLE_BUTTON);
		WVector mouseDelta;
		mouseDelta = g_input->GetMouseDelta();

		WVector mouse = g_input->GetMousePoint();
		WVector rayFrom, rayDir;
		WVector target;
		int groundIndex;
		WVector wallHit;
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

		float topViewSpeed = Max(0.1f, m_topViewFov * 0.0010416667f);

		if (!s_bKeyMove)
		{
			s_cameraFront = g_view->camera.xa;
			s_cameraFront = s_cameraFront.Normalize();

			s_keyMoveDir = WCrossProduct(s_cameraFront, WVector::UNIT_POS_Y);
			s_keyMoveDir = s_keyMoveDir.Normalize();
		}

		if (m_pFurnitureListNum && CMouseCursor::Instance()->GetArea() &&
			m_pFurnitureListNum->IsVisible())
		{
			m_pFurnitureListNum->SetKeyEvent(false);
		}

		int area = CMouseCursor::Instance()->GetArea();
		switch (area)
		{
		case 0:
		{
			if (!m_bTopView)
			{
				CRealMyRoom_AvatarManager::Instance()->m_pPickedAvatar =
					CRealMyRoom_AvatarManager::Instance()->AvatarByRayIntersect(
						rayFrom, rayDir);
				CRealMyRoom_AvatarManager::Instance()->m_pClickedAvatar =
					CRealMyRoom_AvatarManager::Instance()->CheckClick(mouse.x,
						mouse.y);
				CRealMyRoomData::Instance()->m_pCameraModel =
					CRealMyRoomData::Instance()->ModelByRayIntersectOBB(
						g_camera.pivot);

				if (CRealMyRoom_AvatarManager::Instance()->m_pPickedAvatar ||
					CRealMyRoom_AvatarManager::Instance()->m_pClickedAvatar)
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
				if (!m_bTopView &&
					CRealMyRoom_AvatarManager::Instance()->CheckClick(mouse.x,
						mouse.y))
				{
				}
				else
				{
					target = GetPVS().GetRayIntersection(rayFrom, rayDir);
					target = GetPVS().GetGroundPoint(target, true, &groundIndex,
						true);
					wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

					WVector decalX;
					decalX = WVector(4.8f, 0.0f, 0.0f);
					WVector decalZ;
					decalZ = WVector(0.0f, 0.0f, 4.8f);

					GetPVS().DrawDecal(m_glowBallTex, target, decalX, decalZ,
						0.5f, 1.0f, W_POLY_BLEND_ADD, false);

					if (CRealMyRoomData::Instance()->m_pSelModel &&
						(CRealMyRoomData::Instance()
								->m_pSelModel->m_pFurniture->typeId &
							0x3FF800) != 0x7000)
					{
						CRealMyRoomData::Instance()->SetTargetPosition(wallHit,
							1);
					}

					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
						!CRealMyRoom_AvatarManager::Instance()
							->m_pMyAvatar->IsHoldState())
					{
						GetPVS().GetGroundPoint(target, true, &groundIndex,
							true);

						if (groundIndex >= 0 &&
							!CRealMyRoomData::Instance()->m_pSelModel)
						{
							s_bMoving = true;
							if (target.z > m_pLimitPoint->pos.z)
							{
								CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar->SetTargetPosition(target);
								CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar->SetAction(0);
								CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar->Follow(0);
							}
						}
					}

					if (GetFlags(8))
					{
						if (CRealMyRoomData::Instance()->GetState() == 1 ||
							CRealMyRoomData::Instance()->GetState() == 3)
						{
							if (!CRealMyRoomData::Instance()->m_pSelModel ||
								(CRealMyRoomData::Instance()
										->m_pSelModel->m_pFurniture->typeId &
									0x3FF800) != 0x7000)
							{
								CRealMyRoomData::Instance()->m_pSelModel =
									CRealMyRoomData::Instance()
										->ModelByRayIntersect(rayFrom, rayDir);
							}
						}
					}
				}
			}
			else if (leftButton == 3)
			{
				if (CRealMyRoomData::Instance()->GetState() == 2)
				{
					s_bDragging = true;
					target = GetPVS().GetRayIntersection(rayFrom, rayDir);

					target = GetPVS().GetGroundPoint(target, true, &groundIndex,
						true);
					wallHit = GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

					Waabb aabb;
					GetPVS().GetAABB(aabb);

					if (CRealMyRoomData::Instance()->m_pSelModel && GetFlags(8))
					{
						CRealMyRoomData::Instance()->SetTargetPosition(wallHit,
							3);
					}

					CMouseCursor::Instance()->SetMode(0);
				}
			}
			else if (leftButton == 2)
			{
				if (CRealMyRoomData::Instance()->GetState() == 2)
				{
					if (s_bDragging)
					{
						target = GetPVS().GetRayIntersection(rayFrom, rayDir);
						target = GetPVS().GetGroundPoint(target, true,
							&groundIndex, true);
						wallHit =
							GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

						if (CRealMyRoomData::Instance()->m_pSelModel)
						{
							CRealMyRoomData::Instance()->SetTargetPosition(
								wallHit, 2);
						}

						CRealMyRoomData::Instance()->m_pSelModel = 0;

						CMouseCursor::Instance()->SetMode(2);

						s_bDragging = false;
						CRealMyRoomData::Instance()->SetState(0);
					}
				}
			}
			else
			{
				if (CRealMyRoomData::Instance()->GetState() == 2)
				{
					if (CRealMyRoomData::Instance()->m_pSelModel &&
						(CRealMyRoomData::Instance()
								->m_pSelModel->m_pFurniture->typeId &
							0x3FF800) == 0x7000)
					{
						s_bDragging = true;
						target = GetPVS().GetRayIntersection(rayFrom, rayDir);

						target = GetPVS().GetGroundPoint(target, true,
							&groundIndex, true);
						wallHit =
							GetPVS().GetRayIntersectionWall(rayFrom, rayDir);

						Waabb aabb;
						GetPVS().GetAABB(aabb);

						if (CRealMyRoomData::Instance()->m_pSelModel &&
							GetFlags(8))
						{
							CRealMyRoomData::Instance()->SetTargetPosition(
								wallHit, 3);
						}

						CMouseCursor::Instance()->SetMode(0);
					}
				}
			}

			if (rightButton == 1)
			{
				if (!m_bTopView)
				{
					if (GetFlags(8) || GetFlags(2) ||
						g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
					{
						CRealMyRoomData::Instance()->m_pSelModel =
							CRealMyRoomData::Instance()->ModelByRayIntersect(
								rayFrom, rayDir);
					}
				}
				s_bRotating = false;
			}
			else if (rightButton == 3)
			{
				if (!m_bTopView)
				{
					static WVector s_lastMouseDelta(0.0f, 0.0f, 0.0f);

					float turnX = (mouseDelta.y + s_lastMouseDelta.y) * 0.5f;

					m_camAngle.y -= turnX * 0.008f;
					s_lastMouseDelta = mouseDelta;

					if (!m_camLimitType)
					{
						if (m_camAngle.x < -g_PI / 2)
						{
							m_camAngle.x = -g_PI / 2;
						}

						if (m_camAngle.x > g_PI / 4)
						{
							m_camAngle.x = g_PI / 4;
						}
					}
					else
					{
						if (m_camAngle.x < m_camLimit[2] * g_DEGTORAD)
						{
							m_camAngle.x = m_camLimit[2] * g_DEGTORAD;
						}

						if (m_camAngle.x > m_camLimit[3] * g_DEGTORAD)
						{
							m_camAngle.x = m_camLimit[3] * g_DEGTORAD;
						}

						if (m_camAngle.y < m_camLimit[0] * g_DEGTORAD)
						{
							m_camAngle.y = m_camLimit[0] * g_DEGTORAD;
						}

						if (m_camAngle.y > m_camLimit[1] * g_DEGTORAD)
						{
							m_camAngle.y = m_camLimit[1] * g_DEGTORAD;
						}
					}

					m_savedCamAngle = m_camAngle;
					m_motionTime = 0.0f;
				}

				if (!WisZero(mouseDelta.x, 1e-5f) ||
					!WisZero(mouseDelta.y, 1e-5f))
				{
					s_bRotating = true;
				}

				g_input->FixPointer(true);
				CMouseCursor::Instance()->Toggle(false);
			}
			else if (rightButton == 2)
			{
				if (CRealMyRoomData::Instance()->m_pSelModel)
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
					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
					{
						target = CRealMyRoom_AvatarManager::Instance()
									 ->m_pMyAvatar->m_pos;
						target += s_keyMoveDir * 4.1f;

						if (!WisEqual(target,
								CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar->m_pos,
								1e-5f) &&
							!CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->IsHoldState())
						{
							s_bMoving = false;

							CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->SetTargetPosition(target);
							CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->SetAction(0);
							CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->Follow(0);
						}
					}
				}
			}

			if (middleButton == 1)
			{
				ToggleTopViewMode();
			}

			if (!g_pFresh->GetManager()->GetWheelFocus())
			{
				if (CRealMyRoomData::Instance()->GetState() == 3)
				{
					CRealMyRoomData::Instance()->SetRotate(mouseDelta.z);
				}
				else if (!m_bTopView)
				{
					m_targetFov = Between(33.28f,
						m_targetFov -
							mouseDelta.z * g_input->GetSensitivity() * 0.021f,
						60.8f);
				}
				else
				{
					m_topViewFov = Between(51.2f,
						m_topViewFov -
							mouseDelta.z * g_input->GetSensitivity() *
								topViewSpeed * 0.4f,
						80.0f);
				}
			}
		}
		break;
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

			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
			{
				target =
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos;
			}

			if (!CChatMsg::Instance()->IsOpenning())
			{
				if (!control)
				{
					if (g_input->Get("\xbb\xf3", true))
					{
						target.z += s_keyMoveDir.z * 4.1f;
					}

					if (g_input->Get("\xc7\xcf", true))
					{
						target.z += (s_keyMoveDir * -4.1f).z;
					}

					if (g_input->Get("\xc1\xc2", true))
					{
						target.x += (s_cameraFront * -4.1f).x;
					}

					if (g_input->Get("\xbf\xec", true))
					{
						target.x += s_cameraFront.x * 4.1f;
					}

					if (target.z > -8.0f)
					{
						target = GetPVS().GetGroundPoint(target, true,
							&groundIndex, true);
						if (groundIndex < 0)
						{
							if (CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar)
							{
								target = CRealMyRoom_AvatarManager::Instance()
											 ->m_pMyAvatar->m_pos;
							}
						}
					}
					else
					{
						target.z = -8.0f;
					}
				}
				else if (m_bTopView)
				{
					if (g_input->Get("\xbb\xf3", true))
					{
						m_topViewOffset.z += s_keyMoveDir.z;
						if (m_topViewOffset.z > 3.0f)
						{
							m_topViewOffset.z = 3.0f;
						}
					}

					if (g_input->Get("\xc7\xcf", true))
					{
						m_topViewOffset.z += (-s_keyMoveDir).z;
						if (m_topViewOffset.z < -3.0f)
						{
							m_topViewOffset.z = -3.0f;
						}
					}

					if (g_input->Get("\xc1\xc2", true))
					{
						m_topViewOffset.x += (-s_cameraFront).x;
						if (m_topViewOffset.x < -3.0f)
						{
							m_topViewOffset.x = -3.0f;
						}
					}

					if (g_input->Get("\xbf\xec", true))
					{
						m_topViewOffset.x += s_cameraFront.x;
						if (m_topViewOffset.x > 3.0f)
						{
							m_topViewOffset.x = 3.0f;
						}
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

						m_savedCamAngle = m_camAngle;
						m_motionTime = 0.0f;
					}
				}

				if (g_input->Get("HOME", true))
				{
					m_targetFov =
						Between(33.28f, m_targetFov - elapsed * 16.8f, 60.8f);
				}

				if (g_input->Get("END", true))
				{
					m_targetFov =
						Between(33.28f, m_targetFov + elapsed * 16.8f, 60.8f);
				}

				if (g_input->GetDown("PAGE_DOWN", true))
				{
					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
					{
						if (!CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->IsHoldState())
						{
							if (CRealMyRoom_AvatarManager::Instance()
									->m_pMyAvatar->GetAction() != 2)
							{
								if (CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->GetAction() == 1)
								{
									CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->SetAction(2);
								}
								else
								{
									CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->Stop();
									CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->SetAction(1);
								}
							}
						}
					}
				}

				if (g_input->GetDown("PAGE_UP", true))
				{
					if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
					{
						if (!CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->IsHoldState())
						{
							if (CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->GetAction() == 1 ||
								CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->GetAction() == 2)
							{
								if (CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->GetAction() == 1)
								{
									CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->SetAction(0);
								}
								else
								{
									CRealMyRoom_AvatarManager::Instance()
										->m_pMyAvatar->SetAction(1);
								}
							}
						}
					}
				}
			}
			else
			{
				if (g_input->Get("HOME", true) ||
					(shift && g_input->Get("\xbb\xf3", true)))
				{
					m_topViewFov = Between(25.6f,
						m_topViewFov - topViewSpeed * elapsed * 1200.0f,
						960.0f);
				}

				if (g_input->Get("END", true) ||
					(shift && g_input->Get("\xc7\xcf", true)))
				{
					m_topViewFov = Between(25.6f,
						m_topViewFov + topViewSpeed * elapsed * 1200.0f,
						960.0f);
				}
			}

			if (g_input->GetDown("PAD_0", true) ||
				g_input->GetDown("NUM_0", true))
			{
				ToggleTopViewMode();
			}

			if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
				!WisEqual(target,
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos,
					1e-5f) &&
				!CRealMyRoom_AvatarManager::Instance()
					->m_pMyAvatar->IsHoldState())
			{
				s_bMoving = false;
				s_bKeyMove = true;

				if (target.z > m_pLimitPoint->pos.z)
				{
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetTargetPosition(target);
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->SetAction(0);
					CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->Follow(
						0);
				}
			}
			else
			{
				s_bKeyMove = false;
			}
		}
	}
	else
	{
		if (CSharedDoc::Instance()->m_myInfo.info.dwUID ==
			CSharedDoc::Instance()->m_replayTargetUID)
		{
			if (m_pRightMenu->IsVisible())
			{
				m_pRightMenu->SetVisible(false);
			}
		}

		g_input->FixPointer(false);
		CMouseCursor::Instance()->Toggle(true);
	}

	CRealMyRoom_AvatarManager::Instance()->Process(elapsed);
	ProcessCamera(elapsed);
	CRealMyRoomData::Instance()->Process(elapsed);

	CTransferableItem::Instance()->Process(elapsed);

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetMotion() ==
			1)
		{
			m_motionTime += elapsed;
		}

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetMotion() ==
				2 ||
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetMotion() ==
				3)
		{
			m_avatarCamHeight = 1.9f;
		}
		else
		{
			m_avatarCamHeight = 4.6f;
		}

		if (IsEqual(CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->m_targetPos.x,
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos.x) &&
			IsEqual(CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->m_targetPos.z,
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos.z))
		{
			s_bMoving = false;
		}
	}

	m_bAvatarMove = s_bMoving;

	if (m_anotherHand.IsWorking(0))
	{
		Sleep(10);
	}
	else if (m_pExhibition)
	{
		m_anotherHand.Enter();

		bool rotate;
		switch (m_mainTab)
		{
		case 1:
		case 4:
			rotate = false;
			break;
		default:
			rotate = true;
			break;
		}

		m_pExhibition->Process(elapsed, rotate);
		m_anotherHand.Leave();
	}

	if (g_input->GetDown("ESCAPE", true) == 1 &&
		!g_pFresh->GetManager()->HasEscKeyWindow())
	{
		CMessengerInfo::Instance()->Close(false);
	}

	if (m_mainTab == 4)
	{
		for (int i = 0; i < 2; i++)
		{
			float speed = i == 0 ? 138.0f : 228.0f;
			switch (m_skinScrollDir[i])
			{
			case 0:
				m_skinScroll[i] = 0.0f;
				break;
			case 1:
				m_skinScroll[i] -= elapsed * speed * 0.25f;
				break;
			case 2:
				m_skinScroll[i] += elapsed * speed * 0.25f;
				break;
			}
		}
	}

	CIconToolTip* pToolTip = CIconToolTip::GetInstance();
	switch (pToolTip->GetCurTipIndex())
	{
	case 0x1A:
	case 0x1B:
	case 0x1C:
	case 0x1E:
	case 0x1F:
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x27:
	case 0x3E:
		OnBarHoverOff(0);
		break;
	}

	bool handled = false;
	SendMsg(this, "ItemBuff", 0x246, 2, (intptr_t)&handled, 0, 0);
	if (handled)
	{
		OnBarHoverOff(0);
	}

	if (IsLocalContent((localContentType_t)0x57))
	{
		RefreshAllUccClothes();
	}

	if (CSharedDoc::Instance()->m_myInfo.info.dwUID ==
		CSharedDoc::Instance()->m_replayTargetUID)
	{
		if ((g_input->Get("LCONTROL", true) ||
				g_input->Get("RCONTROL", true)) &&
			g_input->GetDown("HIDE", true) && !GetFlags(8))
		{
			SelectMenu(0);
		}

		if ((g_input->Get("LCONTROL", true) ||
				g_input->Get("RCONTROL", true)) &&
			g_input->GetDown("FURNITURE_UI", true))
		{
			SelectMenu(2);
		}
	}

	g_input->GetDown("ENTER", true);
}

void CRealMyRoomMain::Display(void)
{
	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar == NULL)
	{
		return;
	}

	CRealMyRoom_AvatarManager::Instance()->Render();
	CRealMyRoomData::Instance()->Render();

	CTransferableItem::Instance()->Render();

	if (m_bAvatarMove)
	{
		WVector right, forward;
		right = WVector(4.8f, 0.0f, 0.0f);
		forward = WVector(0.0f, 0.0f, 4.8f);

		if (CRealMyRoom_AvatarManager::Instance()
				->m_pMyAvatar->m_followName[0] != '\0')
		{
			CRealMyRoom_Avatar* pAvatar =
				CRealMyRoom_AvatarManager::Instance()->FindAvatarByNickName(
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->m_followName);
			if (pAvatar)
			{
				GetPVS().DrawDecal(m_glowBallTex, pAvatar->m_pos, right,
					forward, (cos(m_glowTime * 10.0f) + 1.0f) * 0.05f + 1.0f,
					1.0f, W_POLY_BLEND_ADD, false);
			}
		}
		else
		{
			GetPVS().DrawDecal(m_glowBallTex,
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_targetPos,
				right, forward, (cos(m_glowTime * 10.0f) + 1.0f) * 0.05f + 1.0f,
				1.0f, W_POLY_BLEND_ADD, false);
		}
	}

	DrawMouseCursor();
}

void CRealMyRoomMain::DrawMouseCursor(void)
{
	int cursor = CMouseCursor::Instance()->GetCursor();
	static int s_oldCursor = cursor;

	if (m_pRightMenu && m_pRightMenu->IsVisible())
	{
		CMouseCursor::Instance()->SetCursor(s_oldCursor);
		return;
	}

	if (CRealMyRoomData::Instance()->GetState() == 2 &&
		CRealMyRoomData::Instance()->m_pSelModel)
	{
		if (CRealMyRoomData::Instance()->m_bCollision)
		{
			CMouseCursor::Instance()->SetCursor(9);
		}
		else
		{
			CMouseCursor::Instance()->SetCursor(8);
		}

		return;
	}

	if (CRealMyRoomData::Instance()->GetState() == 3 &&
		CRealMyRoomData::Instance()->m_pSelModel)
	{
		if (CRealMyRoomData::Instance()->m_bCollision)
		{
			CMouseCursor::Instance()->SetCursor(11);
		}
		else
		{
			CMouseCursor::Instance()->SetCursor(10);
		}

		return;
	}

	if (cursor != s_oldCursor)
	{
		CMouseCursor::Instance()->SetCursor(s_oldCursor);
	}

	s_oldCursor = CMouseCursor::Instance()->GetCursor();
}

void CRealMyRoomMain::OnDisplay(void)
{
	CRealMyRoom_AvatarManager::Instance()->RenderOverlay();

	g_view->DrawLine(m_roomBound[0], 0xFFFFFFFF, m_roomBound[1], 0xFFFFFFFF, 0);
	g_view->DrawLine(m_roomBound[3], 0xFFFFFFFF, m_roomBound[4], 0xFFFFFFFF, 0);
}

void CRealMyRoomMain::LoadBg(const char* name, int hole)
{
	m_bgName = name;
	GOLFDOC()->m_currentHole = hole;

	if (CFx::Instance())
	{
		CFx::Instance()->SetActive(false);
	}

	if (!GOLFDOC()->m_pPolySoup->Load("myroom_01_01.gbin", false))
	{
		g_bQuit = true;
		return;
	}

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", name));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", name));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(name, 0.0f);

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

	CTransferableItem::Instance()->OnLoad();

	if (CFx::Instance())
	{
		CFx::Instance()->SetActive(true);
	}
}

void CRealMyRoomMain::CheckUIPosition(void)
{
	static float s_barY = COption::Instance()->gGetBar_Y();

	if (COption::Instance()->gGetBar_Y() == s_barY)
	{
		return;
	}

	CalcUIOffset(false);
	s_barY = COption::Instance()->gGetBar_Y();

	g_pFresh->OpenLayout("REALMYROOM", this, false);
}

void CRealMyRoomMain::ProcessCamera(float elapsed)
{
	static int data_a888d0 = 1;
	WMatrix mat;
	WVector target;
	WVector dir;
	WVector eye;
	float limit;
	float fov;

	limit = 8.0f;
	g_view->SetClip(1.0f, m_farClip, false);

	switch (m_camDisplayType)
	{
	case 0:
		mat.Reset();

		mat = g_camera;
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos;
		}

		dir = target - m_camTarget;

		target.y = g_camera.pivot.y;
		m_camTarget.y = target.y;
		dir *= elapsed * 4.0f;

		{
			float dist = (target - m_camTarget).Magnitude();

			float dz = fabsf(g_camera.pivot.z - target.z);
			if (dz > limit)
			{
				limit = dz;
			}

			if (dist == 0.0f)
			{
				m_bCamTarget = false;
			}
			else if (dist > 3.0f / limit || m_bCamTarget == true)
			{
				m_bCamTarget = true;
				m_camTarget.x += dir.x;
				m_camTarget.z += dir.z;
			}
		}

		m_camTarget.y = g_camera.pivot.y;

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
			CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->GetHide() &&
			target.z < m_startPos.z)
		{
			g_camera.pivot.y =
				m_savedCamMat.pivot.y - fabsf(target.z - m_startPos.z);
			m_camTarget.y = 8.0f;
		}

		if (!WisEqual(m_fov, m_targetFov, 0.00001f))
		{
			float step = elapsed * 46.0f;

			if (m_targetFov > m_fov)
			{
				m_fov += step;
				if (m_fov > m_targetFov)
				{
					m_fov = m_targetFov;
				}
			}
			else
			{
				m_fov -= step;
				if (m_fov < m_targetFov)
				{
					m_fov = m_targetFov;
				}
			}
		}

		g_view->SetFOV(m_fov * g_DEGTORAD);

		if (m_camAngle.y < -25.0f)
		{
			m_camAngle.y = -25.0f;
		}
		else if (m_camAngle.y > 23.0f)
		{
			m_camAngle.y = 23.0f;
		}

		m_camTarget.y += m_camAngle.y;
		g_camera = MakeCameraMatrix(g_camera.pivot, m_camTarget);
		g_view->SetCamera(g_camera);
		break;

	case 1:
		g_view->SetCamera(g_camera);
		break;

	case 3:
		mat.Reset();

		g_view->SetClip(8.0f, 8192.0f, false);

		g_view->SetFOV(m_topViewFov * g_DEGTORAD);

		mat = RotMat(WVector(-g_PI / 2, 0.0f, 0.0f));

		target = m_topViewOffset;
		eye = m_savedCamMat.pivot;

		eye.z += 5.0f;
		eye.y = 25.0f;
		mat = MakeCameraMatrix(eye, target);

		g_camera = mat;
		g_view->SetCamera(mat);
		break;

	case 4:
		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			target = CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar->m_pos;
		}

		target.y = m_camTargetPos.y;
		dir = m_camTargetPos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f)
		{
			fov = g_view->GetFOV();
			if (m_camTargetFov == fov)
			{
				SetCameraDisplayType(0);
				return;
			}

			CalculationCameraMat(mat, target, WVector::ZERO, g_camera.pivot,
				m_camTargetFov, fov, elapsed * 5.0f);
			g_camera = mat;
			g_view->SetCamera(g_camera);
			break;
		}
		else
		{
			float curFov = g_view->GetFOV();
			CalculationCameraMat(mat, target, dir, g_camera.pivot, curFov,
				curFov, elapsed * 5.0f);
		}

		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;

	case 2:
		target = WVector(0.0f, 0.0f, 5.0f);
		dir = m_camTargetPos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f)
		{
			data_a888d0++;
			if (data_a888d0 == 4)
			{
				m_camTargetPos = m_pSaveCamera[0]->mat.pivot;
			}
			else if (data_a888d0 == 5)
			{
				SetCameraDisplayType(0);

				g_view->SetClip(m_nearClip, m_farClip, false);
				data_a888d0 = 1;
			}
			else
			{
				m_camTargetPos = m_pSaveCamera[data_a888d0]->mat.pivot;
			}
			break;
		}

		CalculationCameraMat(mat, target, dir, g_camera.pivot, m_camTargetFov,
			g_view->GetFOV(), elapsed * 8.0f);
		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;

	case 5:
		g_view->SetFOV(m_topViewFov);

		target = WVector(0.0f, 0.0f, 5.0f);
		dir = m_camTargetPos - g_camera.pivot;
		if (dir.Magnitude() < 1.0f)
		{
			mat = MakeCameraMatrix(g_camera.pivot, m_topViewOffset);
		}
		else
		{
			float curFov = g_view->GetFOV();
			CalculationCameraMat(mat, target, dir, g_camera.pivot, curFov,
				curFov, elapsed * 8.0f);
		}

		g_camera = mat;
		g_view->SetCamera(g_camera);
		break;
	}

	g_view->UpdateCamera();
}

void CRealMyRoomMain::CalculationCameraMat(WMatrix& mat, WVector at,
	WVector dir, WVector pos, float targetFov, float curFov, float speed)
{
	mat = g_camera;

	float fov = curFov;

	dir.Normalize();
	dir *= speed;

	pos += dir;

	if (!(Wabs(curFov - targetFov) < 0.00001f))
	{
		float step = speed * 46.0f;

		if (targetFov > curFov)
		{
			fov = curFov + step;
			if (fov > targetFov)
			{
				fov = targetFov;
			}
		}
		else
		{
			fov = curFov - step;
			if (fov < targetFov)
			{
				fov = targetFov;
			}
		}
	}

	g_view->SetFOV(fov);

	mat = MakeCameraMatrix(pos, at);
}

void CRealMyRoomMain::ToggleTopViewMode(void)
{
	if (GetFlags(1))
	{
		return;
	}

	m_bTopView = !m_bTopView;

	if (m_bTopView)
	{
		GOLFDOC()->GameMode() = 0x200;

		m_topViewFov = 70.0f;
		m_topViewOffset = WVector::ZERO;
		m_camDisplayType = 3;
	}
	else
	{
		GOLFDOC()->GameMode() = 0;

		SetCameraDisplayType(0);
	}
}

void CRealMyRoomMain::SetCameraDisplayType(int type)
{
	static int s_camSel;
	if (type != 0)
	{
		switch (type)
		{
		case 1:
			g_camera = m_pDefaultCamera->mat;
			g_view->SetFOV(m_pDefaultCamera->fov);
			break;

		case 2:
			g_camera = m_pSaveCamera[0]->mat;
			g_view->SetFOV(0.7330383f);

			m_camTargetPos = m_pSaveCamera[1]->mat.pivot;
			m_camTargetFov = 0.7330383f;
			break;

		case 3:
			m_topViewFov = 70.0f;
			m_topViewOffset = WVector::ZERO;
			break;

		case 4:
			if (m_pUICamera)
			{
				g_camera = m_pUICamera->mat;
				g_view->SetFOV(m_pDefaultCamera->fov);
			}
			else
			{
				g_camera = m_savedCamMat;
				g_view->SetFOV(m_savedCamFov);
			}

			m_camTargetPos = m_savedCamMat.pivot;
			m_camTargetFov = m_savedCamFov;
			break;

		case 5:
			if (m_pSaveCamera[s_camSel])
			{
				g_camera = m_pSaveCamera[s_camSel]->mat;
			}
			else
			{
				g_camera = m_savedCamMat;
			}

			g_view->SetFOV(70.0f);

			s_camSel++;

			m_topViewFov = 70.0f;
			m_topViewOffset = WVector(0.0f, 0.0f, 5.0f);

			if (s_camSel >= 4)
			{
				s_camSel = 0;
				SetCameraDisplayType(4);
				return;
			}

			if (m_pSaveCamera[s_camSel])
			{
				m_camTargetPos = m_pSaveCamera[s_camSel]->mat.pivot;
			}
			else
			{
				m_camTargetPos = m_savedCamMat.pivot;
			}
			break;
		}
	}
	else
	{
		if (m_camDisplayType != 4)
		{
			g_camera = m_savedCamMat;
		}
		g_view->SetFOV(m_savedCamFov);
	}

	m_camDisplayType = type;
}

void CRealMyRoomMain::RefreshAllUccClothes(void)
{
	sUccRefreshItem item;

	for (int i = 0; i < 10; i++)
	{
		if (!UccManager()->GetRefreshItem(item))
		{
			break;
		}

		if (m_pExhibition && m_pExhibition->GetPetFrame())
		{
			CPartTidList& tidList = Doc()->GetMyPartTidList();

			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1F);
			if (tidList.m_tid[slot] == item.typeId)
			{
				unsigned long serial = tidList.m_uid[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
				{
					m_pExhibition->GetPetFrame()->SetPart(
						&Doc()->GetMyPartTidList(), pClothes->typeId,
						pClothes->id);
					m_pExhibition->GetPetFrame()->SetPart(
						&Doc()->GetMyPartTidList(), pClothes->typeId,
						pClothes->id);
				}
			}
		}

		if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			CRealMyRoom_Avatar* pMyAvatar =
				CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar;

			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1F);
			if (pMyAvatar->GetPartTidList().m_tid[slot] == item.typeId)
			{
				unsigned long serial = pMyAvatar->GetPartTidList().m_uid[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(serial);

				if (pClothes && strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
				{
					CRealMyRoom_AvatarManager::Instance()
						->m_pMyAvatar->RefreshUccClothes(pClothes->typeId,
							pClothes->id);
				}
			}
		}

		for (int j = 0;
			j < CRealMyRoom_AvatarManager::Instance()->m_avatarCount; j++)
		{
			CRealMyRoom_Avatar* pAvatar =
				CRealMyRoom_AvatarManager::Instance()->m_avatar[j];

			if (pAvatar)
			{
				unsigned char slot =
					(unsigned char)((item.typeId >> 13) & 0x1F);
				if (pAvatar->GetPartTidList().m_tid[slot] == item.typeId)
				{
					unsigned long serial =
						pAvatar->GetPartTidList().m_uid[slot];

					sUccClothes* pClothes = UccManager()->FindClothes(serial);

					if (pClothes &&
						strcmpi(pClothes->uccIndex, item.uccIndex) == 0)
					{
						pAvatar->RefreshUccClothes(pClothes->typeId,
							pClothes->id);
					}
				}
			}
		}
	}
}
