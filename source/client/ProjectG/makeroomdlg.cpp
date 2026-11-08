#include "s5/utilities.h"
#include "makeroomdlg.h"
#include "treasuredlg.h"
#include "treasure.h"
#include "inputmanager.h"
#include "mathconsts.h"

namespace nsInvalidCheck
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

extern Fresh* g_pFresh;
extern CInputManager* g_input;

IMPLEMENT_OBJECT(FrMakeRoomDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrMakeRoomDlg, FrForm)
ON_FRESH_VI("make_vs", FRCMD_INIT, FrMakeRoomDlg::OnMakeVsInit)
ON_FRESH_VV("make_vs", FRCMD_LBUTTONDOWN, FrMakeRoomDlg::OnMakeVsDown)
ON_FRESH_VI("make_mass", FRCMD_INIT, FrMakeRoomDlg::OnMakeMassInit)
ON_FRESH_VV("make_mass", FRCMD_LBUTTONDOWN, FrMakeRoomDlg::OnMakeMassDown)
ON_FRESH_VI("make_battle", FRCMD_INIT, FrMakeRoomDlg::OnMakeBattleInit)
ON_FRESH_VV("make_battle", FRCMD_LBUTTONDOWN, FrMakeRoomDlg::OnMakeBattleDown)
ON_FRESH_VI("make_chat", FRCMD_INIT, FrMakeRoomDlg::OnMakeChatInit)
ON_FRESH_VV("make_chat", FRCMD_LBUTTONDOWN, FrMakeRoomDlg::OnMakeChatDown)
ON_FRESH_VI("make_chaos", FRCMD_INIT, FrMakeRoomDlg::OnMakeChaosInit)
ON_FRESH_VV("make_chaos", FRCMD_LBUTTONDOWN, FrMakeRoomDlg::OnMakeChaosDown)
ON_FRESH_VI("picture", FRCMD_INIT, FrMakeRoomDlg::OnPictureInit)
ON_FRESH_VI("text", FRCMD_INIT, FrMakeRoomDlg::OnTextInit)
ON_FRESH_VI("title", FRCMD_INIT, FrMakeRoomDlg::OnTitleInit)
ON_FRESH_BI("title", FRCMD_ENTERKEY, FrMakeRoomDlg::OnTitleEnterKey)
ON_FRESH_VI("password", FRCMD_INIT, FrMakeRoomDlg::OnPasswordInit)
ON_FRESH_VI("mapprev", FRCMD_INIT, FrMakeRoomDlg::OnMapPrevInit)
ON_FRESH_VV("mapprev", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnMapPrevUp)
ON_FRESH_VI("mapnext", FRCMD_INIT, FrMakeRoomDlg::OnMapNextInit)
ON_FRESH_VV("mapnext", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnMapNextUp)
ON_FRESH_VI("mapprev_chat", FRCMD_INIT, FrMakeRoomDlg::OnMapPrevChatInit)
ON_FRESH_VV("mapprev_chat", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnMapPrevUp)
ON_FRESH_VI("mapnext_chat", FRCMD_INIT, FrMakeRoomDlg::OnMapNextChatInit)
ON_FRESH_VV("mapnext_chat", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnMapNextUp)
ON_FRESH_VI("gametype", FRCMD_INIT, FrMakeRoomDlg::OnGameTypeInit)
ON_FRESH_VV("gametype", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnGameTypeButtonUp)
ON_FRESH_VI("gametype", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnGameTypeOwnerDraw)
ON_FRESH_VI("userlimit", FRCMD_INIT, FrMakeRoomDlg::OnUserLimitInit)
ON_FRESH_VV("userlimit", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnUserLimitButtonUp)
ON_FRESH_VI("userlimit", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnUserLimitOwnerDraw)
ON_FRESH_VI("hole", FRCMD_INIT, FrMakeRoomDlg::OnHoleInit)
ON_FRESH_VV("hole", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnHoleButtonUp)
ON_FRESH_VI("hole", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnHoleOwnerDraw)
ON_FRESH_VI("timelimit", FRCMD_INIT, FrMakeRoomDlg::OnTimeLimitInit)
ON_FRESH_VV("timelimit", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnTimeLimitButtonUp)
ON_FRESH_VI("timelimit", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnTimeLimitOwnerDraw)
ON_FRESH_VI("course", FRCMD_INIT, FrMakeRoomDlg::OnCourseInit)
ON_FRESH_VV("course", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnCourseUp)
ON_FRESH_VI("course", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnCourseOwnerDraw)
ON_FRESH_VI("holetype", FRCMD_INIT, FrMakeRoomDlg::OnHoleTypeInit)
ON_FRESH_VI("holetype", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnHoleTypeOwnerDraw)
ON_FRESH_VI("course_chat", FRCMD_INIT, FrMakeRoomDlg::OnCourseChatInit)
ON_FRESH_VV("course_chat", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnCourseChatUp)
ON_FRESH_VI("hole_min", FRCMD_INIT, FrMakeRoomDlg::OnHoleMinInit)
ON_FRESH_VV("hole_min", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnHoleMinUp)
ON_FRESH_VI("hole_prev", FRCMD_INIT, FrMakeRoomDlg::OnHolePrevInit)
ON_FRESH_VV("hole_prev", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnHolePrevUp)
ON_FRESH_VI("hole_chat", FRCMD_INIT, FrMakeRoomDlg::OnHoleChatInit)
ON_FRESH_VI("hole_chat", FRCMD_OWNERDRAW, FrMakeRoomDlg::OnHoleChatOwnerDraw)
ON_FRESH_VI("hole_next", FRCMD_INIT, FrMakeRoomDlg::OnHoleNextInit)
ON_FRESH_VV("hole_next", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnHoleNextUp)
ON_FRESH_VI("hole_max", FRCMD_INIT, FrMakeRoomDlg::OnHoleMaxInit)
ON_FRESH_VV("hole_max", FRCMD_LBUTTONUP, FrMakeRoomDlg::OnHoleMaxUp)
ON_FRESH_VI("overgauge", FRCMD_INIT, FrMakeRoomDlg::OnOverGauge_Init)
END_FRESH_MSGMAP()

FrMakeRoomDlg::FrMakeRoomDlg()
{
	memset(m_pMakeBtn, 0, sizeof(m_pMakeBtn));
	if (Doc()->m_curChannel.Type & 0x80)
		m_gameType = GAME_TYPE_MATCH;
	else if (Doc()->m_curChannel.Type & 0x4000)
		m_gameType = GAME_TYPE_SKINS;
	else
		m_gameType = GAME_TYPE_STROKE;
	m_pMapPrev = NULL;
	m_pMapNext = NULL;
	m_pMapPrevChat = NULL;
	m_pMapNextChat = NULL;
	m_pMapSelectDlg = NULL;
	m_btnDelay = 0.7f;
}

FrMakeRoomDlg::~FrMakeRoomDlg()
{
}

bool FrMakeRoomDlg::OnInit()
{
	m_holeType = 0;
	m_chatHole = 1;
	m_gameTime = 0;
	m_shotTime = 0;
	m_courseX = m_pCourse->GetRect().x;
	m_pMapSelectO =
		g_pFresh->GetManager()->GetBitmap("MAKEROOM", "map_select_o");
	ChangeGameType(m_gameType, true);
	if (m_pMakeBtn[m_gameTypeGroup])
		m_pMakeBtn[m_gameTypeGroup]->SetStatus(FrButton::PRESSED);
	ChangeCourse(FirstMap());
	return true;
}

void FrMakeRoomDlg::OnMakeVsInit(int param)
{
	m_pMakeBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMakeRoomDlg::OnMakeVsDown()
{
	ChangeGameType(GAME_TYPE_STROKE, true);
}

void FrMakeRoomDlg::OnMakeMassInit(int param)
{
	m_pMakeBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMakeRoomDlg::OnMakeMassDown()
{
	ChangeGameType(GAME_TYPE_30S, true);
}

void FrMakeRoomDlg::OnMakeBattleInit(int param)
{
	m_pMakeBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMakeRoomDlg::OnMakeBattleDown()
{
	ChangeGameType(GAME_TYPE_NEW_APPROACH, true);
}

void FrMakeRoomDlg::OnMakeChatInit(int param)
{
	m_pMakeBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrMakeRoomDlg::OnMakeChatDown()
{
	ChangeGameType(GAME_TYPE_AVATARCHAT, true);
}

void FrMakeRoomDlg::OnMakeChaosInit(int param)
{
	m_pMakeBtn[4] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (!m_pMakeBtn[4])
		return;
	bool found = false;
	std::list<sItemInfo>::iterator it;
	for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
		++it)
		if ((*it).tid == 0x1a0000f7)
		{
			found = true;
			break;
		}
	if (!found)
		m_pMakeBtn[4]->SetVisible(false);
	if (Doc()->m_curGameServer.eventFlags & 0x40)
		m_pMakeBtn[4]->SetVisible(false);
	m_pMakeBtn[4]->EnableBlink(true);
}

void FrMakeRoomDlg::OnMakeChaosDown()
{
	AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 35,
		(int)MakeStr(
			"\xbd\xba\xc6\xf3\xbc\xc8 \xbc\xc5\xc7\xc3 \xb8\xf0\xb5\xe5 \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe9 \xb6\xa7, \x0a\\c0xffff0000\\c%s\\c0xff000000\\c\xc0\xcc \xbc\xd2\xb8\xf0 \xb5\xcb\xb4\xcf\xb4\xd9.",
			ItemManager()->GetItemName(0x1a0000f7)),
		0, 0, 0, 0));
	ChangeGameType(14, true);
}

void FrMakeRoomDlg::OnPictureInit(int param)
{
	m_pPicture = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pPicture)
		m_pPicture->SetMouseEvent(false);
}

void FrMakeRoomDlg::OnTextInit(int param)
{
	m_pText = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrMakeRoomDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	if (!m_pTitle)
		return;
	m_pTitle->SetCharLimit(20, false);
	SetDefaultTitle();
}

bool FrMakeRoomDlg::OnTitleEnterKey(int param)
{
	return false;
}

void FrMakeRoomDlg::OnPasswordInit(int param)
{
	FrEdit* password = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrMakeRoomDlg::OnMapPrevInit(int param)
{
	m_pMapPrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapPrev)
	{
		m_pMapPrev->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnMapPrevUp()
{
	if (m_pMapPrev && m_pMapPrevChat)
	{
		unsigned char course = PrevMap(m_course);
		ChangeCourse(course);
	}
}

void FrMakeRoomDlg::OnMapNextInit(int param)
{
	m_pMapNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapNext)
	{
		m_pMapNext->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnMapNextUp()
{
	if (m_pMapNext && m_pMapNextChat)
	{
		unsigned char course =
			NextMap(m_course, m_gameType == GAME_TYPE_AVATARCHAT);
		ChangeCourse(course);
	}
}

void FrMakeRoomDlg::OnMapPrevChatInit(int param)
{
	m_pMapPrevChat = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapPrevChat)
	{
		m_pMapPrevChat->SetPushDelay(0.0f);
		m_pMapPrevChat->SetVisible(false);
	}
}

void FrMakeRoomDlg::OnMapNextChatInit(int param)
{
	m_pMapNextChat = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pMapNextChat)
	{
		m_pMapNextChat->SetPushDelay(0.0f);
		m_pMapNextChat->SetVisible(false);
	}
}

bool FrMakeRoomDlg::MakeRoom(bool bQuick)
{
	if (Doc()->IsControlServerService(0x40) && m_gameType == GAME_TYPE_STROKE)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x40, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x80) && m_gameType == GAME_TYPE_TEAM)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x80, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x100) && m_gameType == GAME_TYPE_30S)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x100, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x2000000) && m_gameType == 14)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x2000000, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x200) &&
		m_gameType == GAME_TYPE_30S_TEAM)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x200, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x400) &&
		m_gameType == GAME_TYPE_GUILD_MATCH)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x400, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x800) && m_gameType == GAME_TYPE_SKINS)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x800, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x1000) &&
		m_gameType == GAME_TYPE_NEW_APPROACH)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x1000, 0, 0, 0, 0);
		return false;
	}
	if (Doc()->IsControlServerService(0x2000) &&
		m_gameType == GAME_TYPE_AVATARCHAT)
	{
		AfxGetTask()->GetActor("Lobby")
			<< MsgObject(NULL, 0x272, 0x2000, 0, 0, 0, 0);
		return false;
	}
	std::string title;
	FrWnd* titleWnd = FindChildByName("title");
	FrEdit* titleEdit = DYNAMIC_CAST(FrEdit, titleWnd);
	if (titleEdit)
		title = titleEdit->GetLine(1, false);
	if (m_gameType == GAME_TYPE_GUILD_MATCH && !Doc()->m_myInfo.info.dwGuildId)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb1\xe6\xb5\xe5 \xb0\xa1\xc0\xd4\xc0\xda\xb8\xb8 \xb9\xe6\xc0\xbb \xb8\xb8\xb5\xe5\xbd\xc7 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, 3);
		return false;
	}
	if (m_gameType == GAME_TYPE_AVATARCHAT && m_course == 0x7f)
		m_course = rand() % 11;
	if (title.empty() || title.find_first_not_of(' ') == std::string::npos)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb9\xe6 \xc1\xa6\xb8\xf1\xc0\xbb \xb3\xd6\xbe\xee\xc1\xd6\xbc\xbc\xbf\xe4",
			false);
		form->Open(NULL, 3);
		return false;
	}
	if (Doc()->m_chatManager.Filtering((char*)title.c_str()))
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb9\xe6 \xc1\xa6\xb8\xf1\xbf\xa1\xbc\xad \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xb4\xc2 \xb9\xae\xc0\xda\xb8\xa6 \xc6\xf7\xc7\xd4\xc7\xcf\xb0\xed \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, 3);
		return false;
	}
	FrWnd* passwordWnd = FindChildByName("password");
	FrEdit* passwordEdit = DYNAMIC_CAST(FrEdit, passwordWnd);
	std::string password;
	if (passwordEdit)
	{
		const char* text = passwordEdit->GetLine(1, false);
		password = text;
		if (!bQuick)
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 39, (int)text, 0, 0, 0, 0);
		else
			AfxGetTask()->GetActor("AvatarChat")
				<< MsgObject(NULL, 39, (int)text, 0, 0, 0, 0);
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
	if (m_gameType == GAME_TYPE_AVATARCHAT)
		m_hole = m_chatHole;
	if (IsLocalContent(S3_ROOKIE_RESTRICT) &&
		(Doc()->m_curChannel.Type & 0x800) && m_gameType == GAME_TYPE_SKINS)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xb7\xe7\xc5\xb0 \xc3\xa4\xb3\xce\xbf\xa1\xbc\xad\xb4\xc2 \xc6\xce\xb9\xe8\xc6\xb2 \xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, 3);
		return false;
	}
	nsInvalidCheck::_invalidPwd invalid = std::for_each(password.begin(),
		password.end(), nsInvalidCheck::_invalidPwd());
	if (invalid.bInvalid)
	{
		FrForm* form =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		form->SetMessage(
			"\xba\xf1\xb9\xd0\xb9\xf8\xc8\xa3\xbf\xa1 \xb0\xf8\xb9\xe9\xc0\xba \xbb\xe7\xbf\xeb\xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		form->Open(NULL, 3);
		return false;
	}
	WSendPacket packet((enumClientPacket)8);
	packet.Encode1(bQuick ? 1 : 0);
	packet.Encode4(m_shotTime);
	packet.Encode4(m_gameTime);
	packet.Encode1(m_userLimit);
	packet.Encode1(m_gameType);
	packet.Encode1(m_hole);
	packet.Encode1(m_course);
	packet.Encode1(m_holeType);
	packet.EncodeStr(title);
	packet.EncodeStr(password);
	packet.Send(TO_GAME);
	if (!bQuick)
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 22, 1, 0, 0, 0, 0);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	else
	{
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 22, 1, 0, 0, 0, 0);
		AfxGetTask()->GetActor("AvatarChat")
			<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
	}
	return true;
}

void FrMakeRoomDlg::SetDefaultTitle()
{
	m_pTitle->SetLine(1, Doc()->GetDefaultTitle(m_gameType), 0, false, 0);
}

void FrMakeRoomDlg::ChangeGameType(unsigned char type, bool bInit)
{
	if (m_gameType == GAME_TYPE_AVATARCHAT && type != GAME_TYPE_AVATARCHAT)
	{
		m_pMapPrev->SetVisible(true);
		m_pMapNext->SetVisible(true);
		m_pMapPrevChat->SetVisible(false);
		m_pMapNextChat->SetVisible(false);
		m_pCourseChat->SetVisible(false);
		m_pHoleChat->SetVisible(false);
		m_pHoleMin->SetVisible(false);
		m_pHolePrev->SetVisible(false);
		m_pHoleNext->SetVisible(false);
		m_pHoleMax->SetVisible(false);
		m_pHoleType->SetVisible(true);
		m_pCourse->SetVisible(true);
		m_pOverGauge->SetVisible(true);
	}
	if (m_gameType != GAME_TYPE_AVATARCHAT && type == GAME_TYPE_AVATARCHAT)
	{
		m_pMapPrev->SetVisible(false);
		m_pMapNext->SetVisible(false);
		m_pMapPrevChat->SetVisible(true);
		m_pMapNextChat->SetVisible(true);
		m_pCourseChat->SetVisible(true);
		m_pHoleChat->SetVisible(true);
		m_pHoleMin->SetVisible(true);
		m_pHolePrev->SetVisible(true);
		m_pHoleNext->SetVisible(true);
		m_pHoleMax->SetVisible(true);
		m_pHoleType->SetVisible(false);
		m_pCourse->SetVisible(false);
		m_pOverGauge->SetVisible(false);
		if (m_course == 0x7f)
			ChangeCourse(FirstMap());
	}
	else if (m_gameType == 14 && type != 14)
	{
		m_pCourse->SetMouseEvent(true);
		m_pMapPrev->SetVisible(true);
		m_pMapNext->SetVisible(true);
		m_pHoleType->SetVisible(true);
		m_pCourse->SetVisible(true);
		m_pOverGauge->SetVisible(true);
		if (m_course == 0x7f)
			ChangeCourse(FirstMap());
	}
	if (m_gameType == GAME_TYPE_NEW_APPROACH && type != GAME_TYPE_NEW_APPROACH)
	{
		m_pCourse->SetMouseEvent(true);
		ChangeCourse(FirstMap());
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
		m_pMapPrevChat->Enable(false);
		m_pMapNext->Enable(false);
		m_pMapNextChat->Enable(false);
	}
	else if (m_gameType != GAME_TYPE_NEW_APPROACH &&
		type == GAME_TYPE_NEW_APPROACH)
	{
		m_pCourse->SetMouseEvent(false);
		ChangeCourse(0x7f);
		m_pMapPrev->Enable(false);
		m_pMapPrevChat->Enable(false);
		m_pMapNext->Enable(false);
		m_pMapNextChat->Enable(false);
	}
	if (m_gameType != 14 && type == 14)
	{
		m_pCourse->SetMouseEvent(false);
		m_pMapPrev->SetVisible(false);
		m_pMapPrevChat->SetVisible(false);
		m_pMapNext->SetVisible(false);
		m_pMapNextChat->SetVisible(false);
		m_pHoleType->SetVisible(false);
		m_pCourse->SetVisible(false);
		m_pOverGauge->SetVisible(false);
		ChangeCourse(0x7f);
	}
	m_gameType = type;
	for (int i = 0; i < 5; ++i)
		m_pMakeBtn[i]->SetStatus(FrButton::NORMAL);
	if (IsLocalContent(S3_ROOKIE_RESTRICT) &&
		(Doc()->m_curChannel.Type & 0x800) && !Doc()->m_myInfo.stat.Level)
		m_pMakeBtn[2]->Enable(false);
	m_pUserLimit->ClearTab();
	m_pHole->ClearTab();
	m_pTimeLimit->ClearTab();
	switch (m_gameType)
	{
	case GAME_TYPE_STROKE:
		m_pPicture->Open("picture_stroke.tga");
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 0;
		m_userLimit = 2;
		m_hole = 3;
		m_shotTime = 40000;
		m_timeLimit = 40;
		m_pUserLimit->AddTab("select_button", 2, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 3, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 4, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 3, &WSize(37.5f, 20.0f));
		m_pHole->AddTab("select_button", 6, &WSize(37.5f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(37.5f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 60, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 90, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 120, &WSize(37.5f, 20.0f));
		if (!bInit)
			m_pMakeBtn[0]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_TEAM:
		m_pPicture->Open("picture_match.tga");
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 0;
		m_userLimit = 4;
		m_hole = 6;
		m_shotTime = 40000;
		m_timeLimit = 40;
		m_pUserLimit->AddTab("select_button", 2, &WSize(75.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 4, &WSize(75.0f, 20.0f));
		m_pHole->AddTab("select_button", 6, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(50.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 60, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 90, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 120, &WSize(37.5f, 20.0f));
		if (!bInit)
			m_pMakeBtn[0]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_30S:
		m_pPicture->Open("picture_30s.tga");
		m_pText->SetBgImg("text_mass");
		m_gameTypeGroup = 1;
		m_userLimit = 30;
		m_hole = 18;
		m_gameTime = 1800000;
		m_timeLimit = 30;
		if (IsLocalContent(S3_HUNDRED_MODE) &&
			(Doc()->m_myInfo.info.dwIdentity & 4))
		{
			m_pUserLimit->AddTab("select_button", 10, &WSize(25.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 20, &WSize(25.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 30, &WSize(25.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 100, &WSize(25.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 200, &WSize(25.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 250, &WSize(25.0f, 20.0f));
		}
		else
		{
			m_pUserLimit->AddTab("select_button", 10, &WSize(50.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
			m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		}
		m_pHole->AddTab("select_button", 9, &WSize(75.0f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(75.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 30, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 35, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 45, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 50, &WSize(30.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[1]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_30S_TEAM:
		m_pPicture->Open("picture_30steam.tga");
		m_pText->SetBgImg("text_mass");
		m_gameTypeGroup = 1;
		m_userLimit = 30;
		m_hole = 18;
		m_gameTime = 1800000;
		m_timeLimit = 30;
		m_pUserLimit->AddTab("select_button", 10, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(75.0f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(75.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 30, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 35, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 45, &WSize(30.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 50, &WSize(30.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[1]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_GUILD_MATCH:
		m_pPicture->Open("picture_guild.tga");
		m_pText->SetBgImg("text_mass");
		m_gameTypeGroup = 1;
		m_userLimit = 30;
		m_hole = 18;
		m_gameTime = 1800000;
		m_timeLimit = 30;
		m_pUserLimit->AddTab("select_button", 10, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(75.0f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(75.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 35, &WSize(50.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(50.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[1]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_SKINS:
		m_pPicture->Open("picture_battle.tga");
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 2;
		m_userLimit = 2;
		m_hole = 6;
		m_shotTime = 40000;
		m_timeLimit = 40;
		m_pUserLimit->AddTab("select_button", 2, &WSize(75.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 4, &WSize(75.0f, 20.0f));
		m_pHole->AddTab("select_button", 6, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 18, &WSize(50.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 60, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 90, &WSize(37.5f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 120, &WSize(37.5f, 20.0f));
		if (!bInit)
			m_pMakeBtn[2]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_NEW_APPROACH:
		m_pPicture->Open("picture_approach.tga");
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 2;
		m_userLimit = 6;
		m_hole = 3;
		m_gameTime = 40000;
		m_timeLimit = 40;
		m_holeType = 3;
		m_pUserLimit->AddTab("select_button", 6, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 3, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 6, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 9, &WSize(50.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(150.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[2]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_APPROACH:
		m_pPicture->Open("picture_approach.tga");
		m_pText->SetBgImg("text_vs");
		m_gameTypeGroup = 2;
		m_userLimit = 30;
		m_hole = 1;
		m_gameTime = 40000;
		m_timeLimit = 40;
		m_holeType = 2;
		m_pUserLimit->AddTab("select_button", 10, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		m_pHole->AddTab("select_button", 1, &WSize(150.0f, 20.0f));
		m_pTimeLimit->AddTab("select_button", 40, &WSize(150.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[2]->SetStatus(FrButton::PRESSED);
		break;
	case GAME_TYPE_AVATARCHAT:
		m_pPicture->Open("picture_chat.tga");
		m_pText->SetBgImg("text_chat");
		m_gameTypeGroup = 3;
		m_userLimit = 20;
		m_hole = 1;
		m_pUserLimit->AddTab("select_button", 10, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 20, &WSize(50.0f, 20.0f));
		m_pUserLimit->AddTab("select_button", 30, &WSize(50.0f, 20.0f));
		if (!bInit)
			m_pMakeBtn[3]->SetStatus(FrButton::PRESSED);
		break;
	case 14:
		m_pPicture->Open("picture_chaos.tga");
		m_pText->SetBgImg("text_chaos");
		m_gameTypeGroup = 4;
		m_userLimit = 30;
		m_hole = 18;
		m_gameTime = 1800000;
		m_timeLimit = 30;
		m_holeType = 4;
		if (!bInit)
			m_pMakeBtn[4]->SetStatus(FrButton::PRESSED);
		break;
	}
	m_pTimeLimit->SetVisible(m_gameType != GAME_TYPE_GUILD_MATCH);
	SetDefaultTitle();
	if (m_hole == 18 && m_gameType != GAME_TYPE_NEW_APPROACH &&
		m_gameType != 14 && (m_holeType == 1 || m_holeType == 2))
		m_holeType = 0;
	if (bInit)
	{
		m_pGameType->ClearTab();
		switch (m_gameTypeGroup)
		{
		case 0:
			m_pGameType->AddTab("select_button", 0, &WSize(75.0f, 20.0f));
			m_pGameType->AddTab("select_button", 1, &WSize(75.0f, 20.0f));
			break;
		case 1:
			m_pGameType->AddTab("select_button", 4, &WSize(50.0f, 20.0f));
			m_pGameType->AddTab("select_button", 5, &WSize(50.0f, 20.0f));
			m_pGameType->AddTab("select_button", 6, &WSize(50.0f, 20.0f));
			break;
		case 2:
			if (IsLocalContent(S3_ROOKIE_RESTRICT))
			{
				if (Doc()->m_curChannel.Type & 0x800)
				{
					m_pGameType->AddTab("select_button", 10,
						&WSize(150.0f, 20.0f));
				}
				else
				{
					m_pGameType->AddTab("select_button", 10,
						&WSize(75.0f, 20.0f));
					m_pGameType->AddTab("select_button", 7,
						&WSize(75.0f, 20.0f));
				}
			}
			else
			{
				m_pGameType->AddTab("select_button", 10, &WSize(75.0f, 20.0f));
				m_pGameType->AddTab("select_button", 7, &WSize(75.0f, 20.0f));
			}
			break;
		case 3:
			m_pGameType->AddTab("select_button", 2, &WSize(150.0f, 20.0f));
			break;
		case 4:
			m_pGameType->ClearTab();
			break;
		}
	}
	m_pGameType->Select(m_gameType);
	m_pUserLimit->Select(m_userLimit);
	m_pHole->Select(m_hole);
	m_pTimeLimit->Select(m_timeLimit);
}

void FrMakeRoomDlg::ChangeUserLimit(unsigned char value)
{
	m_userLimit = value;
}

void FrMakeRoomDlg::ChangeHole(unsigned char hole)
{
	if (m_hole == hole)
		return;
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
		{
			m_pTimeLimit->ClearTab();
			m_pTimeLimit->AddTab("select_button", 15, &WSize(37.5f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 20, &WSize(37.5f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 25, &WSize(37.5f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 30, &WSize(37.5f, 20.0f));
			m_timeLimit = 15;
			m_pTimeLimit->Select(15);
		}
		else if (hole == 18)
		{
			m_pTimeLimit->ClearTab();
			m_pTimeLimit->AddTab("select_button", 30, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 35, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 40, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 45, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 50, &WSize(30.0f, 20.0f));
			m_timeLimit = 30;
			m_pTimeLimit->Select(30);
			if (m_holeType == 1 || m_holeType == 2)
				m_holeType = 0;
		}
		break;
	case GAME_TYPE_GUILD_MATCH:
		if (hole == 9)
		{
			m_pTimeLimit->ClearTab();
			m_pTimeLimit->AddTab("select_button", 20, &WSize(37.5f, 20.0f));
			m_timeLimit = 20;
			m_pTimeLimit->Select(20);
		}
		else if (hole == 18)
		{
			m_pTimeLimit->ClearTab();
			m_pTimeLimit->AddTab("select_button", 30, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 35, &WSize(30.0f, 20.0f));
			m_pTimeLimit->AddTab("select_button", 40, &WSize(30.0f, 20.0f));
			m_timeLimit = 30;
			m_pTimeLimit->Select(30);
		}
		m_pTimeLimit->SetVisible(false);
		m_holeType = 0;
		break;
	case 14:
		m_pTimeLimit->ClearTab();
		m_pTimeLimit->SetVisible(false);
		m_timeLimit = 30;
		m_holeType = 2;
		break;
	}
}

void FrMakeRoomDlg::ChangeTimeLimit(unsigned long value)
{
	m_timeLimit = value;
}

void FrMakeRoomDlg::ChangeCourse(unsigned char course)
{
	unsigned long index =
		course > 0x7f && course != 0xfd ? course - 0x80 : course;
	IFF_STRUCT::sCourse* info = ItemManager()->FindCourse(index | 0x28000000);
	if (!info)
		return;
	if (course != 0x7f)
	{
		m_pCourse->Open(MakeStr("small_%s.tga", info->c.Icon));
		m_pCourseChat->Open(MakeStr("small_%s.tga", info->c.Icon));
		unsigned long gauge =
			CTHunter::Instance()->GetTHunterGauge((eMapType)course);
		if (gauge != 9999)
		{
			m_pOverGauge->ResetGauge();
			m_pOverGauge->SetDestPos(gauge < 701 ? 0 : gauge - 700, false);
		}
	}
	else
	{
		m_pCourse->Open("map_random_middle.tga");
		m_pCourseChat->Open("map_random_middle.tga");
		if (m_pOverGauge)
		{
			m_pOverGauge->ResetGauge();
			m_pOverGauge->SetDestPos(0, false);
		}
	}
	m_course = course;
	if (m_pMapPrev)
		m_pMapPrev->Enable(m_course != FirstMap());
	if (m_pMapPrevChat)
		m_pMapPrevChat->Enable(m_course != FirstMap());
	if (m_pMapNext)
		m_pMapNext->Enable(m_course != LastMap(false));
	if (m_pMapNextChat)
		m_pMapNextChat->Enable(m_course != LastMap(true));
}

void FrMakeRoomDlg::ChangeHoleType(eHoleType value)
{
	m_holeType = value;
}

void FrMakeRoomDlg::OnGameTypeInit(int param)
{
	m_pGameType = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pGameType)
		m_pGameType->SetSepWidth(1.0f);
}

void FrMakeRoomDlg::OnGameTypeButtonUp()
{
	TButtonItem* item = m_pGameType->GetSelected();
	if (item)
		ChangeGameType((unsigned char)item->index, false);
}

void FrMakeRoomDlg::OnGameTypeOwnerDraw(int param)
{
	TButtonItem* item = (TButtonItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
		gi->PrintText11(
			WPoint(item->rect.x + item->rect.w * 0.5f, item->rect.y + 5.0f), 1,
			s_gameTypeTitle[item->index], NULL);
}

void FrMakeRoomDlg::OnUserLimitInit(int param)
{
	m_pUserLimit = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pUserLimit)
		m_pUserLimit->SetSepWidth(1.0f);
}

void FrMakeRoomDlg::OnUserLimitButtonUp()
{
	TButtonItem* item = m_pUserLimit->GetSelected();
	if (item)
		ChangeUserLimit((unsigned char)item->index);
}

void FrMakeRoomDlg::OnUserLimitOwnerDraw(int param)
{
	TButtonItem* item = (TButtonItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
		gi->PrintText11(
			WPoint(item->rect.x + item->rect.w * 0.5f, item->rect.y + 5.0f), 1,
			MakeStr("%d", item->index), NULL);
}

void FrMakeRoomDlg::OnHoleInit(int param)
{
	m_pHole = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pHole)
		m_pHole->SetSepWidth(1.0f);
}

void FrMakeRoomDlg::OnHoleButtonUp()
{
	TButtonItem* item = m_pHole->GetSelected();
	if (item)
		ChangeHole((unsigned char)item->index);
}

void FrMakeRoomDlg::OnHoleOwnerDraw(int param)
{
	TButtonItem* item = (TButtonItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
		gi->PrintText11(
			WPoint(item->rect.x + item->rect.w * 0.5f, item->rect.y + 5.0f), 1,
			MakeStr("%d", item->index), NULL);
}

void FrMakeRoomDlg::OnTimeLimitInit(int param)
{
	m_pTimeLimit = DYNAMIC_CAST(FrTabButton, (FrWnd*)param);
	if (m_pTimeLimit)
		m_pTimeLimit->SetSepWidth(1.0f);
}

void FrMakeRoomDlg::OnTimeLimitButtonUp()
{
	TButtonItem* item = m_pTimeLimit->GetSelected();
	if (item)
		ChangeTimeLimit(item->index);
}

void FrMakeRoomDlg::OnTimeLimitOwnerDraw(int param)
{
	TButtonItem* item = (TButtonItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
		gi->PrintText11(
			WPoint(item->rect.x + item->rect.w * 0.5f, item->rect.y + 5.0f), 1,
			MakeStr("%d", item->index), NULL);
}

void FrMakeRoomDlg::OnCourseInit(int param)
{
	m_pCourse = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

bool FrMakeRoomDlg::OnMapSelectDlgResult(int result, FrForm* form)
{
	unsigned char map = 0;
	if (form)
		map = ((FrTreasureCourse*)form)->GetCurMapIdx();
	ChangeCourse(map);
	m_pMapSelectDlg = NULL;
	m_btnDelay = 0.7f;
	return true;
}

void FrMakeRoomDlg::OnCourseUp()
{
	if (m_gameType != GAME_TYPE_NEW_APPROACH &&
		m_gameType != GAME_TYPE_GUILD_MATCH && m_gameType != 14)
	{
		g_audio->PlaySfx("ui_icon_click");
		m_pMapSelectDlg = CreateForm<FrTreasureCourse>(g_pFresh->GetManager(),
			this, "treasuremap", NULL);
		if (m_pMapSelectDlg)
		{
			m_pMapSelectDlg->SetSelectMap(m_course);
			m_pMapSelectDlg->SetMapIndexChanged(false);
			m_pMapSelectDlg->LockRandom(m_gameType == GAME_TYPE_AVATARCHAT);
			m_pMapSelectDlg->EnableDrag(false);
			m_pMapSelectDlg->Open(
				(FRESH_PFN_RESULT)&FrMakeRoomDlg::OnMapSelectDlgResult, 3);
		}
	}
}

void FrMakeRoomDlg::OnCourseOwnerDraw(int param)
{
	if (m_gameType == GAME_TYPE_NEW_APPROACH ||
		m_gameType == GAME_TYPE_GUILD_MATCH)
		return;
	if (!m_pCourse || !m_pMapSelectO)
		return;
	WRect rect = m_pCourse->GetRect();
	FrWndManager* manager = g_pFresh->GetManager();
	if (rect.IsInRect(manager->GetMousePos()))
	{
		FrGraphicInterface* gi = manager->GetGDI();
		if (gi)
		{
			gi->DrawTexture(m_pMapSelectO,
				WRect(rect.x - 25.0f, rect.y - 11.0f,
					(float)m_pMapSelectO->Width(),
					(float)m_pMapSelectO->Height()),
				0xffffffff, 0);
		}
	}
}

void FrMakeRoomDlg::OnHoleTypeInit(int param)
{
	m_pHoleType = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (!m_pHoleType)
		return;
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
}

void FrMakeRoomDlg::DrawHoleTypeButton(FrGraphicInterface* gi, eHoleType type,
	WRect rect, bool bEnable)
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
	{
		color = 0xff909090;
	}
	gi->DrawTexture(bitmap, rect, color, 0);
}

void FrMakeRoomDlg::OnHoleTypeOwnerDraw(int param)
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
		DrawHoleTypeButton(gi, (eHoleType)0, front, false);
		DrawHoleTypeButton(gi, (eHoleType)1, back, false);
		DrawHoleTypeButton(gi, (eHoleType)2, random, false);
		DrawHoleTypeButton(gi, (eHoleType)3, shuffle, false);
	}
	else if (m_gameType == 14)
	{
		DrawHoleTypeButton(gi, (eHoleType)0, front, false);
		DrawHoleTypeButton(gi, (eHoleType)1, back, false);
		DrawHoleTypeButton(gi, (eHoleType)2, random, false);
		DrawHoleTypeButton(gi, (eHoleType)3, shuffle, false);
	}
	else if (m_hole == 18)
	{
		DrawHoleTypeButton(gi, (eHoleType)0, front, true);
		DrawHoleTypeButton(gi, (eHoleType)1, back, false);
		DrawHoleTypeButton(gi, (eHoleType)2, random, false);
		DrawHoleTypeButton(gi, (eHoleType)3, shuffle, true);
	}
	else
	{
		DrawHoleTypeButton(gi, (eHoleType)0, front, true);
		DrawHoleTypeButton(gi, (eHoleType)1, back, true);
		DrawHoleTypeButton(gi, (eHoleType)2, random, true);
		DrawHoleTypeButton(gi, (eHoleType)3, shuffle, true);
	}
}

void FrMakeRoomDlg::OnCourseChatInit(int param)
{
	m_pCourseChat = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pCourseChat)
		m_pCourseChat->SetVisible(false);
}

void FrMakeRoomDlg::OnCourseChatUp()
{
	OnCourseUp();
}

void FrMakeRoomDlg::OnHoleChatInit(int param)
{
	m_pHoleChat = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pHoleChat)
		m_pHoleChat->SetVisible(false);
}

void FrMakeRoomDlg::OnHoleChatOwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
	{
		WRect rect = m_pHoleChat->GetRect();
		gi->PrintText11(WPoint(rect.x + 17.0f, rect.y + 5.0f), 1,
			MakeStr("%d", m_chatHole), NULL);
	}
}

void FrMakeRoomDlg::OnHoleMinInit(int param)
{
	m_pHoleMin = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pHoleMin)
	{
		m_pHoleMin->SetVisible(false);
		m_pHoleMin->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnHoleMinUp()
{
	m_chatHole = 1;
}

void FrMakeRoomDlg::OnHolePrevInit(int param)
{
	m_pHolePrev = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pHolePrev)
	{
		m_pHolePrev->SetVisible(false);
		m_pHolePrev->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnHolePrevUp()
{
	if (m_chatHole > 1)
		--m_chatHole;
}

void FrMakeRoomDlg::OnHoleNextInit(int param)
{
	m_pHoleNext = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pHoleNext)
	{
		m_pHoleNext->SetVisible(false);
		m_pHoleNext->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnHoleNextUp()
{
	if (m_chatHole < 18)
		++m_chatHole;
}

void FrMakeRoomDlg::OnHoleMaxInit(int param)
{
	m_pHoleMax = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pHoleMax)
	{
		m_pHoleMax->SetVisible(false);
		m_pHoleMax->SetPushDelay(0.0f);
	}
}

void FrMakeRoomDlg::OnHoleMaxUp()
{
	m_chatHole = 18;
}

void FrMakeRoomDlg::OnOverGauge_Init(int param)
{
	m_pOverGauge = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pOverGauge)
	{
		m_pOverGauge->SetRange(0, 300, 0);
		m_pOverGauge->SetBarSpeed(200.0f);
	}
}

void FrMakeRoomDlg::OnProc(const float dt)
{
	FrForm::OnProc(dt);
	if (!m_pMapSelectDlg && m_btnDelay >= 0.0f)
		m_btnDelay -= dt;
}

bool FrMakeRoomDlg::IsBtnEnable()
{
	if (!m_pMapSelectDlg && m_btnDelay < 0.0f)
		return true;
	return false;
}
