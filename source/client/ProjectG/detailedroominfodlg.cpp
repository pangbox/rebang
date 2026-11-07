#include "minatl.h"
#include "detailedroominfodlg.h"
#include "passworddlg.h"
#include "user_info.h"
#include "netresourcemanager.h"
#include "projectg.h"

extern Fresh* g_pFresh;

static bool CompareIdDesc(const void* left, const void* right)
{
	std::map<unsigned long, sBriefUserInfo>& users = Doc()->m_briefUserInfoMap;
	std::map<unsigned long, sBriefUserInfo>::iterator ia =
		users.find(((sRoomUserInfo*)left)->uid);
	std::map<unsigned long, sBriefUserInfo>::iterator ib =
		users.find(((sRoomUserInfo*)right)->uid);
	if (ia == Doc()->m_briefUserInfoMap.end())
		return true;
	if (ib == Doc()->m_briefUserInfoMap.end())
		return true;
	bool result = strcmp(ia->second.sNick, ib->second.sNick) < 0;
	return result;
}

static bool CompareStatusDesc(const void* left, const void* right)
{
	sRoomUserInfo* a = (sRoomUserInfo*)left;
	sRoomUserInfo* b = (sRoomUserInfo*)right;
	if (a->hole > b->hole)
		return true;
	if (a->hole < b->hole)
		return false;
	return CompareIdDesc(left, right);
}

static bool CompareLevelAsc(const void* left, const void* right)
{
	const sRoomUserInfo* a = (const sRoomUserInfo*)left;
	const sRoomUserInfo* b = (const sRoomUserInfo*)right;
	if (Doc()->m_curChannel.Type & 0x80)
	{
		if (a->ladderPoint > b->ladderPoint)
			return false;
		if (a->ladderPoint < b->ladderPoint)
			return true;
	}
	else
	{
		if (a->level > b->level)
			return false;
		if (a->level < b->level)
			return true;
	}
	return CompareIdDesc(left, right);
}

static bool CompareIdAsc(const void* left, const void* right)
{
	std::map<unsigned long, sBriefUserInfo>& users = Doc()->m_briefUserInfoMap;
	std::map<unsigned long, sBriefUserInfo>::iterator ia =
		users.find(((sRoomUserInfo*)left)->uid);
	std::map<unsigned long, sBriefUserInfo>::iterator ib =
		users.find(((sRoomUserInfo*)right)->uid);
	if (ia == Doc()->m_briefUserInfoMap.end())
		return true;
	if (ib == Doc()->m_briefUserInfoMap.end())
		return true;
	bool result = strcmp(ia->second.sNick, ib->second.sNick) >= 0;
	return result;
}

static bool CompareStatusAsc(const void* left, const void* right)
{
	sRoomUserInfo* a = (sRoomUserInfo*)left;
	sRoomUserInfo* b = (sRoomUserInfo*)right;
	if (a->hole > b->hole)
		return false;
	if (a->hole < b->hole)
		return true;
	return CompareIdDesc(left, right);
}

static bool CompareLevelDesc(const void* left, const void* right)
{
	// HACK
	if (false)
		new FrDtRoomDlg;
	const sRoomUserInfo* a = (const sRoomUserInfo*)left;
	const sRoomUserInfo* b = (const sRoomUserInfo*)right;
	if (Doc()->m_curChannel.Type & 0x80)
	{
		if (a->ladderPoint > b->ladderPoint)
			return true;
		if (a->ladderPoint < b->ladderPoint)
			return false;
	}
	else
	{
		if (a->level > b->level)
			return true;
		if (a->level < b->level)
			return false;
	}
	return CompareIdDesc(left, right);
}

IMPLEMENT_OBJECT(FrDtRoomDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrDtRoomDlg, FrForm)

ON_FRESH_VI("gamedesc", FRCMD_INIT, FrDtRoomDlg::OnGameDescInit)
ON_FRESH_VI("join", FRCMD_INIT, FrDtRoomDlg::OnJoinBtnInit)
ON_FRESH_VV("join", FRCMD_LBUTTONUP, FrDtRoomDlg::OnJoinBtnUp)
ON_FRESH_VI("userinfo", FRCMD_INIT, FrDtRoomDlg::OnUserInfoBtnInit)
ON_FRESH_VV("userinfo", FRCMD_LBUTTONUP, FrDtRoomDlg::OnUserInfoBtnUp)
ON_FRESH_VI("level", FRCMD_INIT, FrDtRoomDlg::OnLevelHdrInit)
ON_FRESH_VV("level", FRCMD_LBUTTONDOWN, FrDtRoomDlg::OnLevelHdrDown)
ON_FRESH_VI("id", FRCMD_INIT, FrDtRoomDlg::OnIdHdrInit)
ON_FRESH_VV("id", FRCMD_LBUTTONDOWN, FrDtRoomDlg::OnIdHdrDown)
ON_FRESH_VI("status", FRCMD_INIT, FrDtRoomDlg::OnStatusHdrInit)
ON_FRESH_VV("status", FRCMD_LBUTTONDOWN, FrDtRoomDlg::OnStatusHdrDown)
ON_FRESH_VI("userlist", FRCMD_INIT, FrDtRoomDlg::OnUserListInit)
ON_FRESH_VI("userlist", FRCMD_OWNERDRAW, FrDtRoomDlg::OnUserListOwnerDraw)
ON_FRESH_VV("userlist", FRCMD_LBUTTONDOWN, FrDtRoomDlg::OnUserListLBtnDown)
ON_FRESH_VV("userlist", FRCMD_RBUTTONUP, FrDtRoomDlg::OnUserListRBtnUp)

END_FRESH_MSGMAP()

FrDtRoomDlg::FrDtRoomDlg()
	: m_pGameDesc(NULL),
	  m_pJoinBtn(NULL),
	  m_pUserInfoBtn(NULL),
	  m_pGalleryBtn(NULL),
	  m_pUserList(NULL)
{
	m_pSelUser = NULL;
	m_pIdHdr = NULL;
	m_pLevelHdr = NULL;
	m_pStatusHdr = NULL;
	m_sortHeader = eHEADER_ID;
}

bool FrDtRoomDlg::OnInit()
{
	// HACK
	if (false)
		OnHeaderLBtnDown(eHEADER_LEVEL);
	m_sortHeader = (eHeader)3;
	if (m_pUserList)
	{
		m_sortHeader = eHEADER_LEVEL;
		m_pUserList->SortItem(CompareLevelDesc);
	}
	return true;
}

void FrDtRoomDlg::SetRoomInfo(const sRoomInfo& info)
{
	m_roomInfo = info;
}

void FrDtRoomDlg::OnGameDescInit(int param)
{
	m_pGameDesc = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
	char text[256];
	const char* typeName;
	switch (Doc()->m_roomDetail.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
	case 14:
	{
		IFF_STRUCT::sMatch* match =
			ItemManager()->FindMatch(Doc()->m_roomDetail.matchTypeId);
		if (match)
			typeName = match->Name;
		else
			typeName = Doc()->m_gameTypeInfo[Doc()->m_roomDetail.gameType].name;
	}
	break;
	default:
		typeName = Doc()->m_gameTypeInfo[Doc()->m_roomDetail.gameType].name;
		break;
	}
	sprintf(text, "\xb0\xd4\xc0\xd3\xc5\xb8\xc0\xd4 : %s%s", typeName,
		Doc()->m_roomDetail.gameType == 5 ? "\xc6\xc0\xc0\xfc" : "");
	m_pGameDesc->SetLine(1, text, 0, false, 0);
	if (Doc()->m_roomDetail.map == 255)
	{
		const char* source = "\xc4\xda\xbd\xba : RANDOM";
		int i = 0;
		do
		{
			text[i] = source[i];
		} while (source[i++]);
	}
	else
	{
		unsigned char map = Doc()->m_roomDetail.map;
		IFF_STRUCT::sCourse* course = Doc()->m_itemManager.FindCourse(
			(map <= 127 || map == 253 ? map : map - 128) | 0x28000000);
		if (course)
			sprintf(text, "\xc4\xda\xbd\xba : %s", course->c.Name);
		else
		{
			const char* source = "\xc4\xda\xbd\xba : \xb8\xf0\xb8\xa7";
			int i = 0;
			do
			{
				text[i] = source[i];
			} while (source[i++]);
		}
	}
	m_pGameDesc->SetLine(2, text, 0, false, 0);
	if (Doc()->m_roomDetail.holeOrder == 2 ||
		Doc()->m_roomDetail.holeOrder == 3)
		sprintf(text,
			"\xbd\xc3\xc0\xdb\xc8\xa6 : \xb7\xa3\xb4\xfd \xc8\xa6 / \xc8\xa6 \xbc\xf6 : %d \xc8\xa6",
			Doc()->m_roomDetail.holes);
	else
		sprintf(text,
			"\xbd\xc3\xc0\xdb\xc8\xa6 : %d \xc8\xa6 / \xc8\xa6 \xbc\xf6 : %d \xc8\xa6",
			Doc()->m_roomDetail.holeOrder ? 10 : 1, Doc()->m_roomDetail.holes);
	m_pGameDesc->SetLine(3, text, 0, false, 0);
	if (Doc()->m_roomDetail.gameType == 2)
		text[0] = 0;
	else
	{
		switch (Doc()->m_roomDetail.gameType)
		{
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 14:
			if (Doc()->m_roomDetail.gameType == 10)
				goto strokeTime;
			sprintf(text,
				"\xb0\xd4\xc0\xd3\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : %d \xba\xd0",
				Doc()->m_roomDetail.gameTime / 60000);
			break;
		default:
strokeTime:
			if (!Doc()->m_roomDetail.gameTime)
			{
				const char* source =
					"\xc5\xb8\xb1\xb8\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : \xbe\xf8\xc0\xbd";
				int i = 0;
				do
				{
					text[i] = source[i];
				} while (source[i++]);
			}
			else
				sprintf(text,
					"\xc5\xb8\xb1\xb8\xc1\xa6\xc7\xd1\xbd\xc3\xb0\xa3 : %d \xc3\xca",
					Doc()->m_roomDetail.gameTime / 1000);
			break;
		}
	}
	m_pGameDesc->SetLine(4, text, 0, false, 0);
}

void FrDtRoomDlg::OnJoinBtnInit(int param)
{
	m_pJoinBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pJoinBtn && (Doc()->m_curChannel.Type & 0x80))
		m_pJoinBtn->Enable(false);
}

void FrDtRoomDlg::OnJoinBtnUp()
{
	int developer = (Doc()->m_myInfo.info.dwIdentity & 14) == 14;
	if (developer)
	{
		OnGalleryBtnUp();
		return;
	}
	if (m_roomInfo.bAvailable)
	{
		if (!Doc()->m_myInfo.info.IsIdentity(4))
		{
			if (m_roomInfo.nUserNum == m_roomInfo.nUserLimit)
			{
				AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
					(int)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
					0, 0, 0, 0);
				return;
			}
			if (m_roomInfo.gameType == 5)
			{
				unsigned long uid = MyGuid(false);
				std::map<unsigned long, sBriefUserInfo>::iterator it =
					Doc()->m_briefUserInfoMap.find(uid);
				if (it != Doc()->m_briefUserInfoMap.end() &&
					it->second.gender > 1)
				{
					AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
						(int)"\xb0\xad\xc1\xa6 \xc1\xbe\xb7\xe1\xc0\xb2\xc0\xcc \xb3\xf4\xc0\xba \xc0\xaf\xc0\xfa\xb4\xc2 30\xc0\xce \xc6\xc0\xc0\xfc \xb0\xd4\xc0\xd3\xc0\xbb \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, 0, 0, 0);
					return;
				}
			}
		}
		if (m_roomInfo.gameType == 6 && !Doc()->m_myInfo.info.IsIdentity(0x14))
		{
			std::string text;
			unsigned long guild = Doc()->m_myInfo.info.dwGuildId;
			bool denied = false;
			if (!guild)
			{
				text =
					"\xb1\xe6\xb5\xe5\xbf\xa1 \xb0\xa1\xc0\xd4\xc7\xcf\xbc\xc5\xbe\xdf \xb1\xe6\xb5\xe5\xb4\xeb\xc0\xfc\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9.";
				denied = true;
			}
			if (m_roomInfo.GuildInfo.nGuildID[0] != guild &&
				m_roomInfo.GuildInfo.nGuildID[0] &&
				m_roomInfo.GuildInfo.nGuildID[1] != guild &&
				m_roomInfo.GuildInfo.nGuildID[1])
			{
				text =
					"\xc5\xb8\xb1\xe6\xb5\xe5\xc0\xc7 \xb4\xeb\xc0\xfc\xc0\xd4\xb4\xcf\xb4\xd9.";
				denied = true;
			}
			if (denied)
			{
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 35, (int)text.c_str(), 0, 0, 0, 0);
				return;
			}
		}
		if (m_roomInfo.bPublic || Doc()->m_myInfo.info.IsIdentity(0x14))
		{
			if (m_pJoinBtn)
				m_pJoinBtn->Enable(false);
			{
				WSendPacket packet((enumClientPacket)9);
				packet.Encode2(m_roomInfo.roomGuid);
				packet.EncodeStr(std::string(""));
				packet.Send(TO_GAME);
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 22, 0, 0, 0, 0, 0);
				AfxGetTask()->GetActor("Lobby")
					<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
			}
			Close((eFormRet)1, true);
		}
		else
		{
			FrPasswordDlg* dialog = CreateForm<FrPasswordDlg>(
				g_pFresh->GetManager(), this, "password", NULL);
			dialog->Open((FRESH_PFN_RESULT)&FrDtRoomDlg::OnPasswordDlgResult,
				3);
		}
	}
	else
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
			0, 0, 0, 0);
	}
}

void FrDtRoomDlg::OnGalleryBtnInit(int param)
{
	m_pGalleryBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDtRoomDlg::OnGalleryBtnUp()
{
	if (m_roomInfo.bAvailable)
	{
		if (m_roomInfo.nGalleryNum != m_roomInfo.nGalleryLimit ||
			Doc()->m_myInfo.info.IsIdentity(4))
		{
			if (m_roomInfo.bPublic || Doc()->m_myInfo.info.IsIdentity(4))
			{
				if (m_pGalleryBtn)
					m_pGalleryBtn->Enable(false);
				{
					WSendPacket packet((enumClientPacket)62);
					packet.Encode2(m_roomInfo.roomGuid);
					packet.EncodeStr(std::string(""));
					packet.Send(TO_GAME);
					AfxGetTask()->GetActor("Lobby")
						<< MsgObject(NULL, 22, 0, 0, 0, 0, 0);
					AfxGetTask()->GetActor("Lobby")
						<< MsgObject(NULL, 1, 0, 0, 0, 0, 0);
					Close((eFormRet)1, true);
				}
			}
			else
			{
				FrPasswordDlg* dialog = CreateForm<FrPasswordDlg>(
					g_pFresh->GetManager(), this, "password", NULL);
				dialog->Open(
					(FRESH_PFN_RESULT)&FrDtRoomDlg::OnPasswordDlgResult, 3);
			}
		}
		else
		{
			AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
				(int)"\xc1\xa4\xbf\xf8\xc0\xcc \xc3\xca\xb0\xfa \xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
				0, 0, 0, 0);
		}
	}
	else
	{
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 35,
			(int)"\xb0\xd4\xc0\xd3 \xc1\xf8\xc7\xe0\xc1\xdf\xc0\xce \xb9\xe6\xc0\xd4\xb4\xcf\xb4\xd9",
			0, 0, 0, 0);
	}
}

void FrDtRoomDlg::EnableJoinBtn(bool enable)
{
	if (m_pJoinBtn)
		m_pJoinBtn->Enable(enable && !(Doc()->m_curChannel.Type & 0x80));
}

void FrDtRoomDlg::OnProc(const float dt)
{
	std::list<void*>::iterator it;
	for (it = m_delList.begin(); it != m_delList.end();)
	{
		m_pUserList->DelItem(*it);
		it = m_delList.erase(it);
	}
}

void FrDtRoomDlg::OnUserInfoBtnInit(int param)
{
	m_pUserInfoBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pUserInfoBtn->Enable(false);
}

void FrDtRoomDlg::OnUserInfoBtnUp()
{
	if (CUserInfo::Instance())
	{
		FrListItem* item = m_pUserList->GetSelected();
		if (item)
		{
			sRoomUserInfo* user = (sRoomUserInfo*)item->pData;
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				Doc()->m_briefUserInfoMap.find(user->uid);
			if (it != Doc()->m_briefUserInfoMap.end())
				CUserInfo::Instance()->SetInfo(it->second.dwUid, user->uid,
					true, true, false, false, it->second.sNick);
		}
	}
}

void FrDtRoomDlg::OnLevelHdrInit(int param)
{
	m_pLevelHdr = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDtRoomDlg::OnLevelHdrDown()
{
	OnHeaderLBtnDown(eHEADER_LEVEL);
}

void FrDtRoomDlg::OnIdHdrInit(int param)
{
	m_pIdHdr = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDtRoomDlg::OnIdHdrDown()
{
	OnHeaderLBtnDown(eHEADER_ID);
}

void FrDtRoomDlg::OnStatusHdrInit(int param)
{
	m_pStatusHdr = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDtRoomDlg::OnStatusHdrDown()
{
	OnHeaderLBtnDown(eHEADER_STATUS);
}

void FrDtRoomDlg::OnHeaderLBtnDown(eHeader header)
{
	if (m_pUserList)
	{
		if (header == m_sortHeader)
		{
			m_sortHeader = (eHeader)(header + 3);
			switch (header)
			{
			case eHEADER_LEVEL:
				m_pUserList->SortItem(CompareLevelAsc);
				break;
			case eHEADER_ID:
				m_pUserList->SortItem(CompareIdAsc);
				break;
			case eHEADER_STATUS:
				m_pUserList->SortItem(CompareStatusAsc);
				break;
			}
		}
		else
		{
			m_sortHeader = header;
			switch (header)
			{
			case eHEADER_LEVEL:
				m_pUserList->SortItem(CompareLevelDesc);
				break;
			case eHEADER_ID:
				m_pUserList->SortItem(CompareIdDesc);
				break;
			case eHEADER_STATUS:
				m_pUserList->SortItem(CompareStatusDesc);
				break;
			}
		}
	}
}

void FrDtRoomDlg::OnUserListInit(int param)
{
	m_pUserList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pUserList->UseRightButton(true);
	m_pIcon[0] = m_pUserList->GetBitmap("i_male");
	m_pIcon[1] = m_pUserList->GetBitmap("i_female");
	m_pIcon[2] = m_pUserList->GetBitmap("i_male_02");
	m_pIcon[3] = m_pUserList->GetBitmap("i_female_02");
	m_pIcon[4] = m_pUserList->GetBitmap("i_male_03");
	m_pIcon[5] = m_pUserList->GetBitmap("i_female_03");
	m_pIcon[6] = m_pUserList->GetBitmap("i_male_manner");
	m_pIcon[7] = m_pUserList->GetBitmap("i_female_manner");
	m_pIcon[8] = m_pUserList->GetBitmap("i_male_angel");
	m_pIcon[9] = m_pUserList->GetBitmap("i_female_angel");
	m_pUserList->ClearItem();
	CSharedDoc* doc;
	std::list<sRoomUserInfo>& users = Doc()->m_roomUserList;
	std::list<sRoomUserInfo>::iterator it = users.begin();
	for (; it != users.end(); it++)
	{
		doc = Doc();
		if (!doc->m_myInfo.info.IsIdentity(4) && ((*it).capability & 0x14))
		{
			std::map<unsigned long, sBriefUserInfo>::iterator found =
				doc->m_briefUserInfoMap.find((*it).uid);
			if (found != doc->m_briefUserInfoMap.end())
			{
				if (!(sBriefUserInfo(found->second).state & 1))
					continue;
			}
		}
		m_pUserList->AddItem(&*it);
	}
}

void FrDtRoomDlg::OnUserListOwnerDraw(int param)
{
	if (!param)
		return;
	FrListItem* item = (FrListItem*)param;
	sRoomUserInfo*& user = (sRoomUserInfo*&)param;
	user = (sRoomUserInfo*)item->pData;
	if (!user)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	std::map<unsigned long, sBriefUserInfo>::iterator it =
		Doc()->m_briefUserInfoMap.find(user->uid);
	if (it == Doc()->m_briefUserInfoMap.end())
	{
		m_delList.push_back(item->pData);
		return;
	}
	sBriefUserInfo& info = it->second;
	unsigned long nickColor =
		((unsigned char)info.dwIdentity & 0x14) ? 0x50ffffff : 0xffffffff;
	if (item->underCursor)
	{
		gdi->SetTextColor(0xffffffff, 0xff808080);
		gdi->SetTextStyle(2);
	}
	else
	{
		if (info.bSleep)
			gdi->SetTextColor(0xff808080, 0xffffffff);
		else
			gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(0);
	}
	const Bitmap* bitmap;
	if (info.angelicWings)
		bitmap = m_pIcon[8 + (info.gender % 2)];
	else if (info.manner)
		bitmap = m_pIcon[6 + (info.gender % 2)];
	else
		bitmap = m_pIcon[info.gender];
	if (user == m_pSelUser)
	{
		_WRECT selection = { item->pos.x, item->pos.y,
			(float)m_pUserList->GetItemWidth(),
			(float)m_pUserList->GetItemHeight() + 2.0f };
		gdi->Box(selection, 0x999b9dff, 0, 0.0f);
	}
	gdi->DrawTexture(bitmap,
		WRect(item->pos.x + 58.0f, item->pos.y, (float)bitmap->Width(),
			(float)bitmap->Height()),
		0xffffffff, 0);
	const Bitmap* badge;
	if (Doc()->m_curChannel.Type & 0x80)
		badge = g_pFresh->GetManager()->GetBitmap("LEVELS",
			MakeStr("ladder_%03d", user->ladderPoint / 100));
	else
	{
		IFF_STRUCT::sSkin* skin;
		if (info.dwTitle && (skin = ItemManager()->FindSkin(info.dwTitle)))
			badge = g_pFresh->GetBitmap(skin->c.Icon);
		else
			badge = g_pFresh->GetManager()->GetBitmap("LEVELS",
				MakeStr("level_%03d", user->level + 1));
	}
	if (badge)
	{
		gdi->DrawTexture(badge,
			WRect(item->pos.x + 28.0f - float2int(badge->Width() * 0.5f),
				item->pos.y + 13.0f - float2int(badge->Height() * 0.5f),
				(float)badge->Width(), (float)badge->Height()),
			0xffffffff, 0);
	}
	if (info.m_GuildId && !info.IsIdentity(0x14))
	{
		const Bitmap* emblem =
			NetResourceManager::Instance()->GetEmblemByName(info.szEmblemName);
		if (emblem)
		{
			gdi->DrawTexture(emblem,
				WRect(item->pos.x + 75.0f, item->pos.y + 1.0f,
					(float)emblem->Width(), (float)emblem->Height()),
				0xffffffff, 0);
		}
	}
	if (!CProjectG::Instance()->HidePrivacy())
		g_pFresh->GetManager()->PrintText(
			WPoint(item->pos.x + 98.0f, item->pos.y + 7.0f), 0, info.sNick,
			-1.0f, nickColor);
	sRoomUserInfo* current = user;
	if (current->hole == 0)
		gdi->Print(WPoint(item->pos.x + 207.0f, item->pos.y + 7.0f), 1,
			info.bSleep ? "\xc0\xe1\xbc\xf6\xc1\xdf"
						: "\xb4\xeb\xb1\xe2\xc1\xdf");
	else if (current->hole == 19)
		gdi->Print(WPoint(item->pos.x + 207.0f, item->pos.y + 7.0f), 1, "End");
	else
		gdi->Print(WPoint(item->pos.x + 207.0f, item->pos.y + 7.0f), 1,
			"%d \xc8\xa6", current->hole);
}

void FrDtRoomDlg::OnUserListLBtnDown()
{
	FrListItem* item = m_pUserList->GetItemUnderCursor();
	if (item)
	{
		sRoomUserInfo* user = (sRoomUserInfo*)item->pData;
		m_pUserList->SelectItem(item, true);
		m_pSelUser = user;
		if (m_pUserInfoBtn)
			m_pUserInfoBtn->Enable(true);
	}
}

void FrDtRoomDlg::OnUserListRBtnUp()
{
	FrListItem* item = m_pUserList->GetItemUnderCursor();
	if (item)
	{
		sRoomUserInfo* user = (sRoomUserInfo*)item->pData;
		m_pUserList->SelectItem(item, true);
		m_pSelUser = user;
		if (m_pUserInfoBtn)
			m_pUserInfoBtn->Enable(true);
	}
	if (m_pSelUser && CUserInfo::Instance())
	{
		std::map<unsigned long, sBriefUserInfo>::iterator it =
			Doc()->m_briefUserInfoMap.find(m_pSelUser->uid);
		if (it != Doc()->m_briefUserInfoMap.end())
			CUserInfo::Instance()->SetInfo(it->second.dwUid, m_pSelUser->uid,
				true, true, false, false, it->second.sNick);
	}
}

bool FrDtRoomDlg::OnPasswordDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrWnd* child = form->FindChildByName("password");
		FrEdit* password = DYNAMIC_CAST(FrEdit, child);
		std::string text;
		if (password)
		{
			const char* line = password->GetLine(1, false);
			text = line;
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 39, (int)line, 0, 0, 0, 0);
		}
		FrWnd* ok = form->FindChildByName("ok");
		if (ok)
			ok->Enable(false);
		WSendPacket packet((enumClientPacket)9);
		packet.Encode2(m_roomInfo.roomGuid);
		packet.EncodeStr(text);
		packet.Send(TO_GAME);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 22, 0, 0, 0, 0, 0);
		AfxGetTask()->GetActor("Lobby") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		Close((eFormRet)1, true);
	}
	return true;
}
