#include "changeroominfodlg.h"
#include "makeroomdlg.h"
#include "treasuredlg.h"
#include "inputmanager.h"

extern Fresh* g_pFresh;

namespace
{
	struct _invalidPwd
	{
		bool bInvalid;

		_invalidPwd()
			: bInvalid(false)
		{
		}
		void operator()(char c)
		{
			if (c == ' ')
				bInvalid = true;
		}
	};
}

IMPLEMENT_OBJECT(FrChangeRoomInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrChangeRoomInfoDlg, FrForm)
ON_FRESH_VI("text", FRCMD_INIT, FrChangeRoomInfoDlg::OnTextInit)
ON_FRESH_VI("title", FRCMD_INIT, FrChangeRoomInfoDlg::OnTitleInit)
ON_FRESH_VV("title", FRCMD_ENTERKEY, FrChangeRoomInfoDlg::OnOkBtnUp)
ON_FRESH_VI("password", FRCMD_INIT, FrChangeRoomInfoDlg::OnPasswordInit)
ON_FRESH_VV("password", FRCMD_ENTERKEY, FrChangeRoomInfoDlg::OnOkBtnUp)
ON_FRESH_VI("mapprev", FRCMD_INIT, FrChangeRoomInfoDlg::OnMapPrevInit)
ON_FRESH_VV("mapprev", FRCMD_LBUTTONUP, FrChangeRoomInfoDlg::OnMapPrevUp)
ON_FRESH_VI("mapnext", FRCMD_INIT, FrChangeRoomInfoDlg::OnMapNextInit)
ON_FRESH_VV("mapnext", FRCMD_LBUTTONUP, FrChangeRoomInfoDlg::OnMapNextUp)
ON_FRESH_VI("gametype", FRCMD_INIT, FrChangeRoomInfoDlg::OnGameTypeInit)
ON_FRESH_VI("gametype", FRCMD_OWNERDRAW,
	FrChangeRoomInfoDlg::OnGameTypeOwnerDraw)
ON_FRESH_VI("userlimit", FRCMD_INIT, FrChangeRoomInfoDlg::OnUserLimitInit)
ON_FRESH_VI("userlimit", FRCMD_OWNERDRAW,
	FrChangeRoomInfoDlg::OnUserLimitOwnerDraw)
ON_FRESH_VI("hole", FRCMD_INIT, FrChangeRoomInfoDlg::OnHoleInit)
ON_FRESH_VI("hole", FRCMD_OWNERDRAW, FrChangeRoomInfoDlg::OnHoleOwnerDraw)
ON_FRESH_VI("timelimit", FRCMD_INIT, FrChangeRoomInfoDlg::OnTimeLimitInit)
ON_FRESH_VI("timelimit", FRCMD_OWNERDRAW,
	FrChangeRoomInfoDlg::OnTimeLimitOwnerDraw)
ON_FRESH_VI("course", FRCMD_INIT, FrChangeRoomInfoDlg::OnCourseInit)
ON_FRESH_VV("course", FRCMD_LBUTTONUP, FrChangeRoomInfoDlg::OnCourseUp)
ON_FRESH_VI("course", FRCMD_OWNERDRAW, FrChangeRoomInfoDlg::OnCourseOwnerDraw)
ON_FRESH_VI("holetype", FRCMD_INIT, FrChangeRoomInfoDlg::OnHoleTypeInit)
ON_FRESH_VI("holetype", FRCMD_OWNERDRAW,
	FrChangeRoomInfoDlg::OnHoleTypeOwnerDraw)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrChangeRoomInfoDlg::OnOkBtnUp)
END_FRESH_MSGMAP()

FrChangeRoomInfoDlg::FrChangeRoomInfoDlg()
{
	m_pTitle = NULL;
	m_pPassword = NULL;
	m_pText = NULL;
	m_pMapCtrl = NULL;
	m_pMapPrev = NULL;
	m_pMapNext = NULL;
	m_pGameType = NULL;
	m_pUserLimit = NULL;
	m_pHole = NULL;
	m_pTimeLimit = NULL;
	m_pCourse = NULL;
	m_pHoleType = NULL;
	m_pSelectBtnN =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_n");
	m_pSelectBtnO =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_o");
	m_pSelectBtnD =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "select_button_d");
	m_pHoleTypeBtn[0][0] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_front_n");
	m_pHoleTypeBtn[0][1] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_front_d");
	m_pHoleTypeBtn[1][0] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_back_n");
	m_pHoleTypeBtn[1][1] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_back_d");
	m_pHoleTypeBtn[2][0] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_random_n");
	m_pHoleTypeBtn[2][1] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_random_d");
	m_pHoleTypeBtn[3][0] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_shuffle_n");
	m_pHoleTypeBtn[3][1] =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "hole_shuffle_d");
	m_pMapSelectO =
		g_pFresh->GetManager()->GetBitmap("MAKEROOM", "map_select_o");
	m_pMapSelectDlg = NULL;
	m_roomInfo = Doc()->m_roomInfo;
	m_gameTypeGroup = 0;
	m_gameType = m_roomInfo.gameType;
	m_hole = m_roomInfo.nHole;
	m_userLimit = m_roomInfo.nUserLimit;
	m_gameTime = m_roomInfo.gameTimeLimit;
	m_shotTime = m_roomInfo.shotTimeLimit;
	m_course = m_roomInfo.mapType;
	m_holeType = m_roomInfo.holeType;
	switch (m_gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
	case GAME_TYPE_MATCH:
	case GAME_TYPE_SKINS:
		m_timeLimit = m_roomInfo.shotTimeLimit / 1000;
		break;
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
		m_timeLimit = m_roomInfo.gameTimeLimit / 60000;
		break;
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		m_timeLimit = m_roomInfo.gameTimeLimit / 1000;
		break;
	}
	m_btnDelay = 0.7f;
}

FrChangeRoomInfoDlg::~FrChangeRoomInfoDlg()
{
}

bool FrChangeRoomInfoDlg::OnInit()
{
	ChangeGameType(m_roomInfo.gameType, false, true);
	ChangeCourse(m_roomInfo.mapType);
	if (m_gameType == GAME_TYPE_NEW_APPROACH ||
		m_gameType == GAME_TYPE_GUILD_MATCH)
	{
		if (m_pMapPrev)
			m_pMapPrev->Enable(false);
		if (m_pMapNext)
			m_pMapNext->Enable(false);
	}
	return true;
}

void FrChangeRoomInfoDlg::OnTextInit(int param)
{
	m_pText = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pTitle)
	{
		m_pTitle->SetCharLimit(20, false);
		m_pTitle->SetLine(1, m_roomInfo.title, 0, false, 0);
	}
}

void FrChangeRoomInfoDlg::OnPasswordInit(int param)
{
	m_pPassword = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (m_pPassword)
	{
		m_pPassword->SetCharLimit(7, false);
		m_pPassword->SetLine(1, m_roomInfo.password, 0, false, 0);
	}
}

void FrChangeRoomInfoDlg::OnMapPrevInit(int param)
{
	m_pMapPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapPrev)
	{
		m_pMapPrev->SetPushDelay(0.0f);
		m_pMapPrev->SetVisible(true);
	}
}

void FrChangeRoomInfoDlg::OnMapPrevUp()
{
	if (m_pMapPrev)
		ChangeCourse(PrevMap(m_course));
}

void FrChangeRoomInfoDlg::OnMapNextInit(int param)
{
	m_pMapNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapNext)
	{
		m_pMapNext->SetPushDelay(0.0f);
		m_pMapNext->SetVisible(true);
	}
}

void FrChangeRoomInfoDlg::OnMapNextUp()
{
	if (m_pMapNext)
		ChangeCourse(NextMap(m_course, m_gameType == GAME_TYPE_AVATARCHAT));
}

void FrChangeRoomInfoDlg::OnGameTypeInit(int param)
{
	m_pGameType = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::DrawGameTypeButton(FrGraphicInterface* gi, int type,
	WRect rect, bool bEnable)
{
	const Bitmap* bitmap = m_pSelectBtnN;
	unsigned long color = 0xffffffff;
	if (bEnable)
	{
		if (m_gameType == type)
			bitmap = m_pSelectBtnD;
		else
		{
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (IsBtnEnable() && rect.IsInRect(mouse))
			{
				if (g_input->GetButton(LEFT_BUTTON) == 2)
					ChangeGameType(type, false, false);
				bitmap = m_pSelectBtnO;
			}
		}
	}
	else
	{
		color = 0xff909090;
	}
	gi->DrawTexture(bitmap, rect, color, 0);
	gi->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1,
		s_gameTypeTitle[type], NULL);
}

void FrChangeRoomInfoDlg::OnGameTypeOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect rect = m_pGameType->GetRect();
	switch (m_gameTypeGroup)
	{
	case 0:
	{
		WRect r1(rect.x, rect.y, rect.w / 2.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 2.0f, rect.y, rect.w / 2.0f, rect.h);
		DrawGameTypeButton(gi, GAME_TYPE_STROKE, r1, true);
		DrawGameTypeButton(gi, GAME_TYPE_TEAM, r2, true);
		break;
	}
	case 1:
	{
		int developer;
		if (IsLocalContent(S3_HUNDRED_MODE) &&
			(developer = (Doc()->m_myInfo.info.dwIdentity & 4) == 4) &&
			Doc()->m_roomInfo.nUserLimit >= 100)
		{
			WRect button(rect.x, rect.y, rect.w, rect.h);
			DrawGameTypeButton(gi, GAME_TYPE_30S, button, false);
		}
		else
		{
			bool guild = m_gameType == GAME_TYPE_GUILD_MATCH;
			WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
			WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f,
				rect.h);
			WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f,
				rect.h);
			DrawGameTypeButton(gi, GAME_TYPE_30S, r1, !guild);
			DrawGameTypeButton(gi, GAME_TYPE_30S_TEAM, r2, !guild);
			DrawGameTypeButton(gi, GAME_TYPE_GUILD_MATCH, r3, guild);
		}
		break;
	}
	case 3:
		DrawGameTypeButton(gi, GAME_TYPE_AVATARCHAT, rect, true);
		break;
	case 2:
	{
		WRect battle(rect.x, rect.y, rect.w / 2.0f, rect.h);
		WRect approach(rect.x + rect.w / 2.0f, rect.y, rect.w / 2.0f, rect.h);
		switch (m_gameType)
		{
		case GAME_TYPE_SKINS:
			DrawGameTypeButton(gi, GAME_TYPE_SKINS, battle, true);
			DrawGameTypeButton(gi, GAME_TYPE_NEW_APPROACH, approach, false);
			break;
		case GAME_TYPE_NEW_APPROACH:
			DrawGameTypeButton(gi, GAME_TYPE_SKINS, battle, false);
			DrawGameTypeButton(gi, GAME_TYPE_NEW_APPROACH, approach, true);
			break;
		}
		break;
	}
	}
}

void FrChangeRoomInfoDlg::OnUserLimitInit(int param)
{
	m_pUserLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::DrawUserLimitButton(FrGraphicInterface* gi, int limit,
	WRect rect)
{
	const Bitmap* bitmap = m_pSelectBtnN;
	if (m_userLimit == limit)
		bitmap = m_pSelectBtnD;
	else
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mouse))
		{
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				ChangeUserLimit(limit);
			bitmap = m_pSelectBtnO;
		}
	}
	gi->DrawTexture(bitmap, rect, 0xffffffff, 0);
	gi->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1,
		MakeStr("%d", limit), NULL);
}

void FrChangeRoomInfoDlg::OnUserLimitOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect rect = m_pUserLimit->GetRect();
	switch (m_gameType)
	{
	case GAME_TYPE_STROKE:
	{
		WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		DrawUserLimitButton(gi, 2, r1);
		DrawUserLimitButton(gi, 3, r2);
		DrawUserLimitButton(gi, 4, r3);
		break;
	}
	case GAME_TYPE_TEAM:
	{
		WRect r1(rect.x, rect.y, rect.w / 2.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 2.0f, rect.y, rect.w / 2.0f, rect.h);
		DrawUserLimitButton(gi, 2, r1);
		DrawUserLimitButton(gi, 4, r2);
		break;
	}
	case GAME_TYPE_SKINS:
	{
		WRect r1(rect.x, rect.y, rect.w / 2.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 2.0f, rect.y, rect.w / 2.0f, rect.h);
		DrawUserLimitButton(gi, 2, r1);
		DrawUserLimitButton(gi, 4, r2);
		break;
	}
	case GAME_TYPE_AVATARCHAT:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	{
		WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		DrawUserLimitButton(gi, 10, r1);
		DrawUserLimitButton(gi, 20, r2);
		DrawUserLimitButton(gi, 30, r3);
		break;
	}
	case GAME_TYPE_NEW_APPROACH:
	{
		WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		DrawUserLimitButton(gi, 6, r1);
		DrawUserLimitButton(gi, 20, r2);
		DrawUserLimitButton(gi, 30, r3);
		break;
	}
	case GAME_TYPE_30S:
	{
		if (IsLocalContent(S3_HUNDRED_MODE) &&
			(Doc()->m_myInfo.info.dwIdentity & 4) &&
			Doc()->m_roomInfo.nUserLimit >= 100)
		{
			WRect button(rect.x, rect.y, rect.w, rect.h);
			DrawUserLimitButton(gi, Doc()->m_roomInfo.nUserLimit, button);
		}
		else
		{
			WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
			WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f,
				rect.h);
			WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f,
				rect.h);
			DrawUserLimitButton(gi, 10, r1);
			DrawUserLimitButton(gi, 20, r2);
			DrawUserLimitButton(gi, 30, r3);
		}
		break;
	}
	}
}

void FrChangeRoomInfoDlg::OnHoleInit(int param)
{
	m_hole = m_roomInfo.nHole;
	m_pHole = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::DrawHoleButton(FrGraphicInterface* gi, int hole,
	WRect rect)
{
	const Bitmap* bitmap = m_pSelectBtnN;
	if (m_hole == hole)
		bitmap = m_pSelectBtnD;
	else
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mouse))
		{
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				ChangeHole(hole);
			bitmap = m_pSelectBtnO;
		}
	}
	gi->DrawTexture(bitmap, rect, 0xffffffff, 0);
	gi->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1,
		MakeStr("%d", hole), NULL);
}

void FrChangeRoomInfoDlg::OnHoleOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect rect = m_pHole->GetRect();
	switch (m_gameType)
	{
	case GAME_TYPE_STROKE:
	{
		WRect r1(rect.x, rect.y, rect.w / 4.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		WRect r4(rect.x + rect.w * 3.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		DrawHoleButton(gi, 3, r1);
		DrawHoleButton(gi, 6, r2);
		DrawHoleButton(gi, 9, r3);
		DrawHoleButton(gi, 18, r4);
		break;
	}
	case GAME_TYPE_TEAM:
	case GAME_TYPE_SKINS:
	{
		WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		DrawHoleButton(gi, 6, r1);
		DrawHoleButton(gi, 9, r2);
		DrawHoleButton(gi, 18, r3);
		break;
	}
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
	{
		WRect r1(rect.x, rect.y, rect.w / 3.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 3.0f, rect.y, rect.w / 3.0f, rect.h);
		DrawHoleButton(gi, 3, r1);
		DrawHoleButton(gi, 6, r2);
		DrawHoleButton(gi, 9, r3);
		break;
	}
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	{
		WRect r1(rect.x, rect.y, rect.w / 2.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 2.0f, rect.y, rect.w / 2.0f, rect.h);
		DrawHoleButton(gi, 9, r1);
		DrawHoleButton(gi, 18, r2);
		break;
	}
	}
}

void FrChangeRoomInfoDlg::OnTimeLimitInit(int param)
{
	m_pTimeLimit = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::DrawTimeLimitButton(FrGraphicInterface* gi, int time,
	WRect rect)
{
	const Bitmap* bitmap = m_pSelectBtnN;
	if (m_timeLimit == time)
		bitmap = m_pSelectBtnD;
	else
	{
		const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
		if (IsBtnEnable() && rect.IsInRect(mouse))
		{
			if (g_input->GetButton(LEFT_BUTTON) == 2)
				ChangeTimeLimit(time);
			bitmap = m_pSelectBtnO;
		}
	}
	gi->DrawTexture(bitmap, rect, 0xffffffff, 0);
	gi->PrintText11(WPoint(rect.x + rect.w * 0.5f, rect.y + 5.0f), 1,
		MakeStr("%d", time), NULL);
}

void FrChangeRoomInfoDlg::OnTimeLimitOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect rect = m_pTimeLimit->GetRect();
	switch (m_gameType)
	{
	case GAME_TYPE_GUILD_MATCH:
		return;
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		DrawTimeLimitButton(gi, 40, rect);
		break;
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
	case GAME_TYPE_SKINS:
	{
		WRect r1(rect.x, rect.y, rect.w / 4.0f, rect.h);
		WRect r2(rect.x + rect.w * 1.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		WRect r3(rect.x + rect.w * 2.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		WRect r4(rect.x + rect.w * 3.0f / 4.0f, rect.y, rect.w / 4.0f, rect.h);
		DrawTimeLimitButton(gi, 40, r1);
		DrawTimeLimitButton(gi, 60, r2);
		DrawTimeLimitButton(gi, 90, r3);
		DrawTimeLimitButton(gi, 120, r4);
		break;
	}
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
		if (m_hole == 9)
		{
			WRect r1(rect.x, rect.y, rect.w / 4.0f, rect.h);
			WRect r2(rect.x + rect.w * 1.0f / 4.0f, rect.y, rect.w / 4.0f,
				rect.h);
			WRect r3(rect.x + rect.w * 2.0f / 4.0f, rect.y, rect.w / 4.0f,
				rect.h);
			WRect r4(rect.x + rect.w * 3.0f / 4.0f, rect.y, rect.w / 4.0f,
				rect.h);
			DrawTimeLimitButton(gi, 15, r1);
			DrawTimeLimitButton(gi, 20, r2);
			DrawTimeLimitButton(gi, 25, r3);
			DrawTimeLimitButton(gi, 30, r4);
		}
		else if (m_hole == 18)
		{
			WRect r1(rect.x, rect.y, rect.w / 5.0f, rect.h);
			WRect r2(rect.x + rect.w * 1.0f / 5.0f, rect.y, rect.w / 5.0f,
				rect.h);
			WRect r3(rect.x + rect.w * 2.0f / 5.0f, rect.y, rect.w / 5.0f,
				rect.h);
			WRect r4(rect.x + rect.w * 3.0f / 5.0f, rect.y, rect.w / 5.0f,
				rect.h);
			WRect r5(rect.x + rect.w * 4.0f / 5.0f, rect.y, rect.w / 5.0f,
				rect.h);
			DrawTimeLimitButton(gi, 30, r1);
			DrawTimeLimitButton(gi, 35, r2);
			DrawTimeLimitButton(gi, 40, r3);
			DrawTimeLimitButton(gi, 45, r4);
			DrawTimeLimitButton(gi, 50, r5);
		}
		break;
	}
}

void FrChangeRoomInfoDlg::OnCourseInit(int param)
{
	m_pCourse = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

bool FrChangeRoomInfoDlg::OnMapSelectDlgResult(int result, FrForm* form)
{
	unsigned char map = 0;
	if (form)
		map = ((FrTreasureCourse*)form)->GetCurMapIdx();
	ChangeCourse(map);
	m_pMapSelectDlg = NULL;
	m_btnDelay = 0.7f;
	return true;
}

void FrChangeRoomInfoDlg::OnCourseUp()
{
	if (m_gameType != GAME_TYPE_NEW_APPROACH &&
		m_gameType != GAME_TYPE_GUILD_MATCH)
	{
		g_audio->PlaySfx("ui_icon_click");
		m_pMapSelectDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(),
			this, "treasuremap", NULL);
		if (m_pMapSelectDlg)
		{
			m_pMapSelectDlg->SetSelectMap(m_course);
			m_pMapSelectDlg->SetMapIndexChanged(false);
			m_pMapSelectDlg->EnableDrag(false);
			m_pMapSelectDlg->Open(
				(FRESH_PFN_RESULT)&FrChangeRoomInfoDlg::OnMapSelectDlgResult,
				3);
		}
	}
}

void FrChangeRoomInfoDlg::OnCourseOwnerDraw(int param)
{
	if (m_gameType == GAME_TYPE_NEW_APPROACH ||
		m_gameType == GAME_TYPE_GUILD_MATCH)
		return;
	if (m_pCourse && m_pMapSelectO)
	{
		WRect rect = m_pCourse->GetRect();
		FrWndManager* manager = g_pFresh->GetManager();
		if (rect.IsInRect(manager->GetMousePos()))
		{
			FrGraphicInterface* gi = manager->GetGDI();
			if (gi)
			{
				gi->DrawTexture(m_pMapSelectO,
					WRect(rect.x - 25.0f, rect.y - 15.0f,
						(float)m_pMapSelectO->Width(),
						(float)m_pMapSelectO->Height()),
					0xffffffff, 0);
			}
		}
	}
}

void FrChangeRoomInfoDlg::OnHoleTypeInit(int param)
{
	m_pHoleType = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrChangeRoomInfoDlg::DrawHoleTypeButton(FrGraphicInterface* gi,
	eHoleType type, WRect rect, bool bEnable)
{
	const Bitmap* bitmap = m_pHoleTypeBtn[type][0];
	unsigned long color = 0xffffffff;
	if (bEnable)
	{
		if (m_holeType == type)
			bitmap = m_pHoleTypeBtn[type][1];
		else
		{
			const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
			if (IsBtnEnable() && rect.IsInRect(mouse))
			{
				if (g_input->GetButton(LEFT_BUTTON) == 2)
					ChangeHoleType(type);
			}
		}
	}
	else
		color = 0xff909090;
	gi->DrawTexture(bitmap, rect, color, 0);
}

void FrChangeRoomInfoDlg::OnHoleTypeOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect rect = m_pHoleType->GetRect();
	float x = rect.x;
	float width = m_pHoleTypeBtn[0][0]->Width();
	float height = m_pHoleTypeBtn[0][0]->Height();
	WRect front(x, rect.y, width, height);
	x += width;
	width = m_pHoleTypeBtn[1][0]->Width();
	WRect back(x, rect.y, width, height);
	x += width;
	width = m_pHoleTypeBtn[2][0]->Width();
	WRect random(x, rect.y, width, height);
	x += width;
	width = m_pHoleTypeBtn[3][0]->Width();
	WRect shuffle(x, rect.y, width, height);
	if (m_gameType == GAME_TYPE_NEW_APPROACH)
	{
		DrawHoleTypeButton(gi, FRONT_HOLE, front, false);
		DrawHoleTypeButton(gi, BACK_HOLE, back, false);
		DrawHoleTypeButton(gi, START_RANDOM, random, false);
		DrawHoleTypeButton(gi, ALL_RANDOM, shuffle, false);
	}
	else if (m_hole == 18)
	{
		DrawHoleTypeButton(gi, FRONT_HOLE, front, true);
		DrawHoleTypeButton(gi, BACK_HOLE, back, false);
		DrawHoleTypeButton(gi, START_RANDOM, random, false);
		DrawHoleTypeButton(gi, ALL_RANDOM, shuffle, true);
	}
	else
	{
		DrawHoleTypeButton(gi, FRONT_HOLE, front, true);
		DrawHoleTypeButton(gi, BACK_HOLE, back, true);
		DrawHoleTypeButton(gi, START_RANDOM, random, true);
		DrawHoleTypeButton(gi, ALL_RANDOM, shuffle, true);
	}
}

void FrChangeRoomInfoDlg::OnOkBtnUp()
{
	if (m_roomInfo.nUserNum > 1 &&
		!(bool)((Doc()->m_myInfo.info.dwIdentity >> 2) & 1))
	{
		std::list<sSlotInfo>::iterator it;
		for (it = Doc()->m_slotList.begin(); it != Doc()->m_slotList.end();
			++it)
		{
			if (!(*it).bMaster && !(*it).bReady)
				break;
		}
		if (it == Doc()->m_slotList.end())
		{
			OnFreshOkay();
			return;
		}
	}
	std::string title = m_pTitle->GetLine(1, false);
	if (title.empty() || title.find_first_not_of(' ') == std::string::npos)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb9\xe6 \xc1\xa6\xb8\xf1\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
			false);
		form->Open(NULL, 3);
		return;
	}
	if (Doc()->m_chatManager.Filtering((char*)title.c_str()))
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb9\xe6 \xc1\xa6\xb8\xf1\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb9\xae\xc0\xda\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, 3);
		return;
	}
	if (IsMassGame(m_gameType))
	{
		m_shotTime = 0;
		if (m_gameType == GAME_TYPE_NEW_APPROACH)
			m_gameTime = m_timeLimit * 1000;
		else
			m_gameTime = m_timeLimit * 60000;
	}
	else
	{
		m_shotTime = m_timeLimit * 1000;
		m_gameTime = 0;
	}
	unsigned char changes = 0;
	if (m_gameType != m_roomInfo.gameType)
		++changes;
	if (m_hole != m_roomInfo.nHole)
		++changes;
	if (m_shotTime != m_roomInfo.shotTimeLimit)
		++changes;
	if (m_gameTime != m_roomInfo.gameTimeLimit)
		++changes;
	if (m_userLimit != m_roomInfo.nUserLimit)
		++changes;
	if (strcmp(m_roomInfo.title, m_pTitle->GetLine(1, false)))
		++changes;
	if (strcmp(m_roomInfo.password, m_pPassword->GetLine(1, false)))
		++changes;
	if (m_holeType != m_roomInfo.holeType)
		++changes;
	if (m_course != m_roomInfo.mapType)
		++changes;
	if (changes)
	{
		if (Doc()->m_curChannel.Type & 2)
			++changes;
		WSendPacket packet((enumClientPacket)10);
		packet.Encode2(0xffff);
		packet.Encode1(changes);
		if (strcmp(m_roomInfo.title, m_pTitle->GetLine(1, false)))
		{
			packet.Encode1(0);
			packet.EncodeStr(std::string(m_pTitle->GetLine(1, false)));
		}
		if (strcmp(m_roomInfo.password, m_pPassword->GetLine(1, false)))
		{
			std::string password = "";
			packet.Encode1(1);
			packet.EncodeStr(std::string(m_pPassword->GetLine(1, false)));
			password = m_pPassword->GetLine(1, false);
			_invalidPwd invalid =
				std::for_each(password.begin(), password.end(), _invalidPwd());
			if (invalid.bInvalid)
			{
				FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify", NULL);
				form->SetMessage(
					"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xbf\xa1 \xb0\xf8\xb9\xe9\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				form->Open(NULL, 3);
				return;
			}
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 39,
				(int)m_pPassword->GetLine(1, false), 0, 0, 0, 0);
		}
		if (m_hole != m_roomInfo.nHole)
		{
			packet.Encode1(4);
			packet.Encode1(m_hole);
		}
		if (m_shotTime != m_roomInfo.shotTimeLimit)
		{
			packet.Encode1(6);
			packet.Encode1(m_shotTime / 1000);
		}
		if (m_gameTime != m_roomInfo.gameTimeLimit)
		{
			packet.Encode1(8);
			if (m_gameType == GAME_TYPE_NEW_APPROACH)
				packet.Encode1(m_gameTime / 1000);
			else
				packet.Encode1(m_gameTime / 60000);
		}
		if (m_userLimit != m_roomInfo.nUserLimit)
		{
			packet.Encode1(7);
			packet.Encode1(m_userLimit);
		}
		if (m_gameType != m_roomInfo.gameType)
		{
			packet.Encode1(2);
			packet.Encode1(m_gameType);
		}
		if (Doc()->m_curChannel.Type & 2)
		{
			packet.Encode1(5);
			packet.Encode1(m_roomInfo.holeType);
		}
		if (m_holeType != m_roomInfo.holeType)
		{
			packet.Encode1(5);
			packet.Encode1(m_holeType);
		}
		if (m_course != m_roomInfo.mapType)
		{
			packet.Encode1(3);
			packet.Encode1(m_course);
		}
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	OnFreshOkay();
}

void FrChangeRoomInfoDlg::ChangeGameType(int type, bool bUnused,
	bool bKeepValues)
{
	if (m_gameType == GAME_TYPE_AVATARCHAT && type != GAME_TYPE_AVATARCHAT)
	{
		m_pMapCtrl->SetVisible(true);
		m_pMapPrev->SetVisible(true);
		m_pMapNext->SetVisible(true);
		m_pHoleType->SetVisible(true);
		m_pCourse->SetVisible(true);
	}
	else if (m_gameType != GAME_TYPE_AVATARCHAT && type == GAME_TYPE_AVATARCHAT)
	{
		m_pMapCtrl->SetVisible(false);
		m_pMapPrev->SetVisible(false);
		m_pMapNext->SetVisible(false);
		m_pHoleType->SetVisible(false);
		m_pCourse->SetVisible(false);
	}
	else if (m_gameType == GAME_TYPE_GUILD_MATCH &&
		type != GAME_TYPE_GUILD_MATCH)
	{
		m_pCourse->SetMouseEvent(true);
		ChangeCourse(FirstMap());
	}
	else if (m_gameType != GAME_TYPE_GUILD_MATCH &&
		type == GAME_TYPE_GUILD_MATCH)
	{
		m_pCourse->SetMouseEvent(false);
		ChangeCourse(0x7f);
		m_pMapPrev->Enable(false);
		m_pMapNext->Enable(false);
	}
	else if (m_gameType == GAME_TYPE_NEW_APPROACH &&
		type != GAME_TYPE_NEW_APPROACH)
	{
		m_pCourse->SetMouseEvent(true);
		ChangeCourse(FirstMap());
	}
	else if (m_gameType != GAME_TYPE_NEW_APPROACH &&
		type == GAME_TYPE_NEW_APPROACH)
	{
		m_pCourse->SetMouseEvent(false);
		ChangeCourse(0x7f);
		m_pMapPrev->Enable(false);
		m_pMapNext->Enable(false);
	}
	m_gameType = type;
	switch (type)
	{
	case GAME_TYPE_STROKE:
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 0;
		if (!bKeepValues)
		{
			m_userLimit = 2;
			m_hole = 3;
			m_shotTime = 40000;
			m_timeLimit = 40;
		}
		break;
	case GAME_TYPE_TEAM:
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 0;
		if (!bKeepValues)
		{
			m_userLimit = 4;
			m_hole = 6;
			m_shotTime = 40000;
			m_timeLimit = 40;
		}
		break;
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
		m_pText->SetBgImg("text_mass");
		m_gameTypeGroup = 1;
		break;
	case GAME_TYPE_GUILD_MATCH:
		m_pText->SetBgImg("text_mass");
		m_gameTypeGroup = 1;
		break;
	case GAME_TYPE_SKINS:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 2;
		break;
	case GAME_TYPE_AVATARCHAT:
		m_pText->SetBgImg("text_chat");
		m_gameTypeGroup = 3;
		break;
	}
}

void FrChangeRoomInfoDlg::ChangeUserLimit(int limit)
{
	m_userLimit = limit;
}

void FrChangeRoomInfoDlg::ChangeHole(int hole)
{
	m_hole = hole;
	switch (m_gameType)
	{
	case GAME_TYPE_STROKE:
	case GAME_TYPE_TEAM:
	case GAME_TYPE_SKINS:
		if (hole == 18 && (m_holeType == 1 || m_holeType == 2))
			m_holeType = 0;
		break;
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
		if (hole == 9)
			m_timeLimit = 15;
		else if (hole == 18)
		{
			m_timeLimit = 30;
			if (m_holeType == 1 || m_holeType == 2)
				m_holeType = 0;
		}
		break;
	case GAME_TYPE_GUILD_MATCH:
		if (hole == 9)
			m_timeLimit = 20;
		else if (hole == 18)
		{
			m_timeLimit = 30;
			m_holeType = 0;
		}
		break;
	}
}

void FrChangeRoomInfoDlg::ChangeTimeLimit(int time)
{
	m_timeLimit = time;
}

void FrChangeRoomInfoDlg::ChangeCourse(int course)
{
	unsigned long index =
		course > 0x7f && course != 0xfd ? course - 0x80 : course;
	IFF_STRUCT::sCourse* info = ItemManager()->FindCourse(index | 0x28000000);
	if (info)
	{
		if (course != 0x7f)
			m_pCourse->Open(MakeStr("small_%s.tga", info->c.Icon));
		else
			m_pCourse->Open("map_random_middle.tga");
		m_course = course;
		if (m_pMapPrev)
			m_pMapPrev->Enable(m_course != FirstMap());
		if (m_pMapNext)
			m_pMapNext->Enable(m_course != LastMap(false));
	}
}

void FrChangeRoomInfoDlg::ChangeHoleType(eHoleType type)
{
	m_holeType = type;
}

void FrChangeRoomInfoDlg::OnProc(const float dt)
{
	FrForm::OnProc(dt);
	if (!m_pMapSelectDlg && m_btnDelay >= 0.0f)
		m_btnDelay -= dt;
}

bool FrChangeRoomInfoDlg::IsBtnEnable()
{
	if (!m_pMapSelectDlg && m_btnDelay < 0.0f)
		return true;
	return false;
}
