#include "headicon.h"
#include "projectg.h"
#include "golfdoc.h"
#include "netresourcemanager.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"

extern Fresh* g_pFresh;

CHeadIcon::CHeadIcon()
{
	m_bVisible = true;
	for (int i = 0; i < 4; ++i)
	{
		m_head[i].icon = 0xffff;
		m_head[i].time = 0.f;
		m_head[i].level = NULL;
	}
	m_emoticon = g_resrcmng->GetOverlay("[emoti_object.png", 0);
	m_dstRect.w = 64.f;
	m_dstRect.h = 64.f;
	m_srcRect.w = .25f;
	m_srcRect.h = .25f;
	m_itemIcon.clear();
	for (unsigned char item = 0; item < 44; ++item)
	{
		if (item == 13)
			continue;
		IFF_STRUCT::sItem* info = ItemManager()->FindItem(0x18000000 | item);
		if (info)
		{
			WOverlay* overlay =
				g_resrcmng->GetOverlay(MakeStr("%s.tga", info->c.Icon), 0);
			if (overlay)
			{
				unsigned long typeId = info->c.TypeId;
				m_itemIcon[typeId] = overlay;
			}
		}
	}
}

CHeadIcon::~CHeadIcon()
{
	if (g_resrcmng && m_emoticon)
	{
		g_resrcmng->Release(m_emoticon);
		m_emoticon = NULL;
	}
	for (int i = 0; i < 4; ++i)
		if (g_resrcmng && m_head[i].level)
		{
			g_resrcmng->Release(m_head[i].level);
			m_head[i].level = NULL;
		}
	for (std::map<unsigned long, WOverlay*>::iterator it = m_itemIcon.begin();
		it != m_itemIcon.end(); it++)
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	m_itemIcon.clear();
}

void CHeadIcon::SetLevel()
{
	int count = Min((int)GOLFDOC()->m_playerNum, 4);
	for (int i = 0; i < count; ++i)
	{
		if (g_resrcmng && m_head[i].level)
		{
			g_resrcmng->Release(m_head[i].level);
			m_head[i].level = NULL;
		}
		if (Doc()->m_curChannel.Type & 0x80)
			m_head[i].level = g_resrcmng->GetOverlay(
				MakeStr("ladder_%03d.tga",
					Doc()->m_userInfo[i].stat.dwLadderPoint / 100),
				0);
		else if (Doc()->m_userInfo[i].userEquip.tidSkin[5])
		{
			IFF_STRUCT::sSkin* skin = ItemManager()->FindSkin(
				Doc()->m_userInfo[i].userEquip.tidSkin[5]);
			if (skin)
				m_head[i].level =
					g_resrcmng->GetOverlay(MakeStr("%s.tga", skin->c.Icon), 0);
			else
				m_head[i].level = g_resrcmng->GetOverlay(
					MakeStr("level_%03d.tga",
						Doc()->m_userInfo[i].stat.Level + 1),
					0);
		}
		else
			m_head[i].level = g_resrcmng->GetOverlay(
				MakeStr("level_%03d.tga", Doc()->m_userInfo[i].stat.Level + 1),
				0);
	}
}

void CHeadIcon::SetPlayerPos(int index, WVector& pos)
{
	if (index >= 0 && (unsigned int)index < 4)
	{
		WVector screenPos = g_view->Projection(pos);
		sPlayerHead& head = m_head[index];
		WVector diff = pos - g_view->camera.pivot;
		head.screenPos.x = screenPos.x;
		head.screenPos.y = screenPos.y;
		head.screenPos.z = screenPos.z;
		head.distance = diff.Magnitude();
	}
}

bool CHeadIcon::SetIcon(unsigned long uid, unsigned short icon)
{
	unsigned char index = Doc()->GetIndex(uid);
	if (index < 4)
	{
		sPlayerHead& head = m_head[index];
		head.icon = icon;
		head.time = 0.f;
		return true;
	}
	return false;
}

bool CHeadIcon::SendIcon(unsigned short icon)
{
	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)24);
		packet.Encode2(icon);
		packet.Send(TO_GAME);
		return SetIcon(MyGuid(false), icon);
	}
	return SetIcon(GOLFDOC()->m_currentPlayer, icon);
}

void CHeadIcon::Process(float delta)
{
	int count = Min((int)GOLFDOC()->m_playerNum, 4);
	for (int i = 0; i < count; ++i)
		if (GOLFDOC()->GetPlayer(i)->state != 3)
		{
			m_head[i].time += delta;
			if (m_head[i].time > 3.f && m_head[i].icon != 29)
			{
				m_head[i].icon = 0xffff;
				m_head[i].time = 0.f;
			}
		}
}

void CHeadIcon::DisplayOverPlayer()
{
	if (!m_bVisible || CProjectG::Instance()->HidePrivacy())
		return;
	if (GOLFDOC()->m_gameMode & 0x20000)
	{
		for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; ++i)
			if ((Doc()->m_golfGame.gameType != 0 &&
					Doc()->m_golfGame.gameType != 3 &&
					Doc()->m_golfGame.gameType != 7) ||
				GOLFDOC()->GetPlayer(i)->state != 3)
				DisplayOverPlayer(i, true);
	}
	else if (GOLFDOC()->m_gameMode & 0x1000)
	{
		if (Doc()->m_golfGame.gameType == 1)
		{
			DisplayOverPlayer(MyIndex(), false);
			if (PLAYER(MyIndex())->teammate != 0xff)
				DisplayOverPlayer(PLAYER(MyIndex())->teammate, false);
		}
		else if (Doc()->m_golfGame.gameType == 3)
			DisplayOverPlayer(MyIndex(), false);
		else if (Doc()->m_golfGame.gameType == 7)
			DisplayOverPlayer(MyIndex(), false);
		else
			DisplayOverPlayer(GOLFDOC()->m_currentPlayer, false);
	}
	else
		DisplayOverPlayer(GOLFDOC()->m_currentPlayer, false);
}

void CHeadIcon::DisplayOverPlayer(unsigned char index, bool bBesideName)
{
	sPlayerHead& head = m_head[index];
	if (head.screenPos.z < 0.f)
		return;
	float distance = bBesideName ? head.distance : head.distance;
	if (head.screenPos.z <= 0.f || 1.f <= head.screenPos.z || distance > 640.f)
		return;
	{
		float scale;
		if (head.icon < 0xffff)
		{
			int column = head.icon % 4;
			int row = head.icon % 16 / 4;
			m_srcRect.x = column * .25f;
			m_srcRect.y = row * .25f;
			scale = 1.f / distance;
			m_dstRect.x = float2int(head.screenPos.x - 60.f - 200.f * scale);
			m_dstRect.y = float2int(head.screenPos.y - 52.f - 800.f * scale);
			m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xffffffff, 0,
				0);
		}
		else if (PLAYER(index)->item)
		{
			m_srcRect.y = 0.f;
			scale = 1.f / distance;
			m_dstRect.x = float2int(head.screenPos.x - 60.f - 200.f * scale);
			m_dstRect.y = float2int(head.screenPos.y - 52.f - 800.f * scale);
			if (PLAYER(index)->item == 0x1800002d)
			{
				m_srcRect.x = .5f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xffffffff,
					0, 0);
			}
			else if (PLAYER(index)->item != 0x1800000e)
			{
				m_srcRect.x = 0.f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xffffffff,
					0, 0);
				WOverlay* item = m_itemIcon[PLAYER(index)->item];
				WRect dst;
				dst.x = m_dstRect.x + 28.f;
				dst.y = m_dstRect.y + 30.f;
				dst.w = 42.f;
				dst.h = 42.f;
				WRect src(0, 0, 1.f, 1.f);
				item->Render(g_view, src, dst, 0, 0xffffffff, 0, 0);
			}
		}
	}
	if ((GOLFDOC()->m_gameMode == 2048 && Doc()->m_golfGame.gameType == 10) ||
		GOLFDOC()->m_gameMode == 512)
		return;
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		FrEmoticon* emoticon = g_pFresh->GetManager()->GetEmoticon();
		if (!gdi)
			return;
		gdi->SetAlpha(1.f);
		const char* nickname;
		if (Doc()->m_userInfo[index].info.sNick[0])
			nickname = Doc()->m_userInfo[index].info.sNick;
		else
			nickname = "???";
		float nameWidth;
		if (emoticon)
			nameWidth = emoticon->GetTextWidth(nickname);
		else
			nameWidth = gdi->GetTextExtend(nickname);
		bool showEmblem = Doc()->m_userInfo[index].info.dwGuildId &&
			!(Doc()->m_userInfo[index].info.dwIdentity & 0x14);
		WRect level;
		level.w = head.level ? head.level->GetWidth() : 0.f;
		level.h = head.level ? head.level->GetHeight() : 0.f;
		float width;
		if (bBesideName)
		{
			width = level.w;
			if (showEmblem)
				width += 27.f;
		}
		else
			width = level.w + nameWidth + 5.f;
		level.x = floorf(head.screenPos.x - width * .5f);
		level.y = floorf(head.screenPos.y - 30.f - 1200.f / distance);
		if (IsMassGame())
		{
			const Bitmap* rank = NULL;
			if (Doc()->m_golfGame.gameType != 10)
			{
				if (Doc()->m_golfGame.gameType == 4)
					rank = g_pFresh->GetManager()->GetBitmap("FRAMES4",
						"30in_rank");
				else if (Doc()->m_golfGame.gameType == 6)
					rank = g_pFresh->GetManager()->GetBitmap("FRAMES4",
						"30in_rank_02");
				else if (Doc()->m_golfGame.gameType == 5)
				{
					if (Doc()->m_rivalList[MyIndex()].team == 0)
						rank = g_pFresh->GetManager()->GetBitmap("FRAMES4",
							"30in_team_redteam_rank");
					else
						rank = g_pFresh->GetManager()->GetBitmap("FRAMES4",
							"30in_team_blueteam_rank");
				}
			}
			if (rank)
			{
				if (Doc()->m_golfGame.gameType == 6)
					gdi->DrawTexture(rank,
						WRect(level.x - 30.f, level.y - 1.f,
							(float)rank->Width(), (float)rank->Height()),
						0xffffffff, 0);
				else
					gdi->DrawTexture(rank,
						WRect(level.x - 19.f, level.y - 1.f + 2.f,
							(float)rank->Width(), (float)rank->Height()),
						0xffffffff, 0);
			}
			gdi->SetTextStyle(0);
			gdi->SetTextColor(0xffffffff, 0xffffffff);
			if (Doc()->m_golfGame.gameType == 6)
				gdi->PrintText(WPoint(level.x - 15.f, level.y + 2.f), 1,
					MakeStr("%d \xc1\xb6", Doc()->m_guildTeam), 0);
			else if (Doc()->m_golfGame.gameType != 10)
				gdi->PrintText(WPoint(level.x - 10.f, level.y + 2.f), 1,
					MakeStr("%d", Doc()->m_rivalList[MyIndex()].rank), 0);
		}
		if (head.level && !Doc()->m_offlinePlay)
		{
			WRect dst;
			dst.x = level.x;
			dst.y = level.y + 8.f - level.h * .5f;
			dst.w = level.w;
			dst.h = level.h;
			head.level->Render(g_view, WRect(0, 0, 1.f, 1.f), dst, 0,
				0xffffffff, 0, 0);
		}
		float textX, textY;
		textX = level.x + level.w + 5.f;
		textY = level.y;
		if (showEmblem)
		{
			const Bitmap* emblem = NetResManager()->GetEmblemByName(
				Doc()->m_userInfo[index].info.szEmblemName);
			if (emblem)
			{
				gdi->DrawTexture(emblem,
					WRect(textX, level.y - 6.f, (float)emblem->Width(),
						(float)emblem->Height()),
					0xffffffff, 0);
				textX += 27.f;
			}
		}
		if (bBesideName)
		{
			textX = head.screenPos.x - nameWidth * .5f;
			textY = level.y + 20.f;
		}
		gdi->SetTextStyle(2);
		gdi->SetTextColor(0xffffffff, 0xff000000);
		WPoint pos;
		pos.x = float2int(textX);
		pos.y = float2int(textY);
		if (emoticon)
			emoticon->PrintText(pos, 0, nickname, 0xffffffff);
		else
			gdi->PrintText(pos, 0, nickname, 0);
		gdi->SetAlpha(1.f);
	}
}

void CHeadIcon::DisplayBesideBoard(float x, float y)
{
	if (!(GOLFDOC()->m_gameMode & 0xfffff77e))
		return;
	y -= 64.f;
	for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; ++i)
	{
		if (PLAYER(i)->state == 3)
			continue;
		sPlayerHead& head = m_head[i];
		if (PLAYER(i)->chatBlock)
		{
			m_srcRect.x = .75f;
			m_srcRect.y = 0.f;
			if (Doc()->m_golfGame.gameType == 1)
			{
				m_dstRect.x = PLAYER(i)->team == 0 ? x : y;
				if (PLAYER(i)->teammate == 0xff)
					m_dstRect.y = 10.f;
				else
					m_dstRect.y = i < PLAYER(i)->teammate ? 10.f : 60.f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xccffffff,
					0, PLAYER(i)->team == 0);
			}
			else
			{
				m_dstRect.x = (i & 2) ? y : x;
				m_dstRect.y = (i & 1) ? 81.f : 10.f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xccffffff,
					0, (i & 2) == 0);
			}
		}
		else
		{
			if (i == GOLFDOC()->m_currentPlayer || head.icon >= 0xffff)
				continue;
			int column = head.icon % 4;
			int row = head.icon % 16 / 4;
			m_srcRect.x = column * .25f;
			m_srcRect.y = row * .25f;
			if (Doc()->m_golfGame.gameType == 1)
			{
				m_dstRect.x = PLAYER(i)->team == 0 ? x : y;
				if (PLAYER(i)->teammate == 0xff)
					m_dstRect.y = 10.f;
				else
					m_dstRect.y = i < PLAYER(i)->teammate ? 10.f : 60.f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xccffffff,
					0, PLAYER(i)->team == 0);
			}
			else
			{
				m_dstRect.x = (i & 2) ? y : x;
				m_dstRect.y = (i & 1) ? 81.f : 10.f;
				m_emoticon->Render(g_view, m_srcRect, m_dstRect, 0, 0xccffffff,
					0, (i & 2) == 0);
			}
		}
	}
}
