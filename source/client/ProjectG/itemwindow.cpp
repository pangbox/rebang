#include "stdafx.h"
#include "s5/utilities.h"
#include "itemwindow.h"
#include "simpleoverlay.h"
#include "mousecursor.h"
#include "club.h"
#include "helpmsg.h"
#include "quitwindow.h"
#include "clientsetting.h"
#include "quadtree.h"
#include "cardsystem.h"
#include "wbone.h"

IMPLEMENT_ACTOR(CItemWindow, IActor)

CItemWindow::eState CItemWindow::ms_state = STATE_CLOSED;

CItemWindow::CItemWindow()
{
	m_pWindow = NULL;
	m_pFailUI = NULL;
	m_bEnable = true;
	m_dispPriority = DISP_PRIORITY_4;
}

void CItemWindow::OnDestroy()
{
	for (std::map<unsigned long, WOverlay*>::iterator it = m_iconMap.begin();
		it != m_iconMap.end(); it++)
	{
		if (g_resrcmng && (*it).second)
		{
			g_resrcmng->Release((*it).second);
			(*it).second = NULL;
		}
	}
	m_iconMap.clear();

	if (g_resrcmng && m_pWindow)
	{
		g_resrcmng->Release(m_pWindow);
		m_pWindow = NULL;
	}

	if (m_pFailUI)
	{
		delete m_pFailUI;
		m_pFailUI = NULL;
	}
}

void CItemWindow::OnLoad()
{
	if (GOLFDOC()->m_tutorialMode >= 15 || GOLFDOC()->m_tutorialMode == 5)
	{
		m_pWindow = g_resrcmng->GetOverlay("{Item_win.png", 0);
		m_pFailUI = new CSimpleUI("[emoti_object.png");
		m_pFailUI->Add("0", WRect(0, 128, 64, 64), false);
		m_pFailUI->Add("1", WRect(64, 128, 64, 64), false);
		m_pFailUI->Add("2", WRect(128, 128, 64, 64), false);
		m_pFailUI->Add("3", WRect(192, 128, 64, 64), false);
		m_pFailUI->Add("4", WRect(0, 192, 64, 64), false);
		m_pFailUI->Add("5", WRect(64, 192, 64, 64), false);
		m_pFailUI->Add("6", WRect(128, 192, 64, 64), false);
		m_pFailUI->Add("7", WRect(192, 192, 64, 64), false);
		ResetUiPosition();
	}
}

void CItemWindow::OnInit()
{
	ResetVars();
}

void CItemWindow::ResetVars()
{
	m_pos = 0;
	m_velocity = 0;
	ms_state = STATE_CLOSED;
	m_overSlot = -1;
	m_selSlot = -1;
	m_useSlot = -1;
	m_usedTypeId = 0;
	m_reserved74 = 200;
	m_reserved78 = 200;
	m_bEffect = false;
	m_bInputEnable = true;
}

void CItemWindow::OnProcess(float delta)
{
	if (GOLFDOC()->m_tutorialMode >= 15 || GOLFDOC()->m_tutorialMode == 5)
	{
		if (m_bEffect)
		{
			if (!(GOLFDOC()->m_gameMode & 8))
				m_bEffect = false;
			m_effectTime += delta;
			if (m_bFailed)
			{
				m_failFrame = (unsigned char)(m_effectTime * 10);
				if (m_failFrame == 8)
				{
					m_failFrame = 0;
					m_bEffect = false;
				}
			}
			else
			{
				if (m_effectTime < 0.8f)
				{
					m_effectVel.y += delta * 6;
					m_effectPos.x += m_effectVel.x;
					m_effectPos.y += m_effectVel.y;
				}
				else
				{
					WBone* bone;
					SendMsg(this, "Player", 273, (int)&bone, (int)"Bip01 Head",
						0, 0);
					WVector pos(bone->GetMatrix().pivot.x,
						bone->GetMatrix().pivot.y - 0.5f,
						bone->GetMatrix().pivot.z);
					WVector screen = g_view->Projection(pos);
					float dx = screen.x - m_effectPos.x;
					m_effectVel.x += dx * delta * 0.5f;
					m_effectVel.y += (screen.y - m_effectPos.y) * delta * 0.5f;
					float nextX = m_effectPos.x + m_effectVel.x;
					float nextY = m_effectPos.y + m_effectVel.y;
					if ((screen.x - nextX) * dx < 0)
						m_bEffect = false;
					m_effectPos.x = nextX;
					m_effectPos.y = nextY;
				}
			}
		}
		if (m_bEnable && IsMyTurn(true))
		{
			m_mouse.x = g_input->GetMousePoint().x;
			m_mouse.y = g_input->GetMousePoint().y;
			CheckInput();
			switch (ms_state)
			{
			case STATE_OPENED:
				OnOpened(delta);
				break;
			case STATE_OPENING:
				OnRight(delta);
				break;
			case STATE_CLOSING:
				OnLeft(delta);
				break;
			case STATE_CLOSED:
				OnClosed(delta);
				break;
			}
			if (ms_state != STATE_CLOSED)
				m_dest.x = m_pos - 444;
		}
	}
}

void CItemWindow::CheckInput()
{
	if (false)
		UseItem(0);

	if (!CQuitWindow::Instance()->IsActive())
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->itemNum)
		{
			if (GOLFDOC()->GameMode() & 0x308)
			{
				if (g_input->GetDown("ITEMSLOT", true) ||
					(CMouseCursor::Instance()->GetArea() ==
							CMouseCursor::AREA_ITEM &&
						CMouseCursor::Instance()->IsInButtonDownArea(
							LEFT_BUTTON) &&
						g_input->GetButton(LEFT_BUTTON) == 2))
				{
					CChatMsg::Instance()->Close(false, false);
					if ((!PLAYER(GOLFDOC()->m_currentPlayer)->item &&
							m_useSlot == -1) ||
						(Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe)
						ExecuteNow(true);
				}
			}
		}
	}
}

void CItemWindow::ExecuteNow(bool sound)
{
	switch (ms_state)
	{
	case STATE_OPENED:
		if (sound)
			g_audio->PlaySfx("play_menu_close");
	case STATE_OPENING:
		ms_state = STATE_CLOSING;
		m_selSlot = -1;
		m_overSlot = -1;
		CMouseCursor::Instance()->SetItemWindow(false);
		CMouseCursor::Instance()->SetAutoHide(true);
		SendMsg(this, "Screen", 432, 0, 0, 0, 0);
		if (CHelpMsg::Instance())
			CHelpMsg::Instance()->SetItemHelpMsg(0);
		break;
	case STATE_CLOSED:
		if (sound)
			g_audio->PlaySfx("play_menu_open");
		GolfClub().SetItemWindow(true);
		if (GolfClub().BarIs(0))
			GolfClub().SetBar(1);
	case STATE_CLOSING:
		ms_state = STATE_OPENING;
		m_openPos = PLAYER(GOLFDOC()->m_currentPlayer)->itemNum * 45.0f + 64.0f;
		m_velocity = 1500;
		SendMsg(this, "Screen", 431, 0, 0, 0, 0);
		CMouseCursor::Instance()->SetAutoHide(false);
		break;
	}
}

void CItemWindow::OnClosed(float delta)
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->item)
		CHelpMsg::Instance()->SetItemHelpMsg(
			PLAYER(GOLFDOC()->m_currentPlayer)->item);
}

void CItemWindow::OnRight(float delta)
{
	if (CChatMsg::Instance()->IsActive())
	{
		ms_state = STATE_CLOSING;
		CMouseCursor::Instance()->SetItemWindow(false);
	}

	m_pos += delta * m_velocity;
	if (m_pos > m_openPos)
	{
		m_velocity -= delta * 52500;
		if (m_pos > m_openPos + 20)
			m_pos = m_openPos + 20;
	}
	else if (m_velocity < 0)
	{
		m_pos = m_openPos;
		ms_state = STATE_OPENED;
		m_selSlot = 0;
		CMouseCursor::Instance()->SetItemWindow(true);
	}
}

void CItemWindow::OnLeft(float delta)
{
	m_pos -= delta * 1500;
	if (m_pos < 0)
	{
		m_pos = 0;
		ms_state = STATE_CLOSED;
		SendMsg(this, "Screen", 431, 0, 0, 0, 0);
		GolfClub().SetItemWindow(false);
	}
}

void CItemWindow::OnOpened(float delta)
{
	if (CQuitWindow::Instance()->IsActive())
		return;

	if (CChatMsg::Instance()->IsActive())
	{
		ms_state = STATE_CLOSING;
		CMouseCursor::Instance()->SetItemWindow(false);
	}

	if (m_useSlot < 0 && m_bInputEnable)
	{
		bool used = false;
		unsigned long* slots = GetTidItemSlot(GOLFDOC()->m_currentPlayer);
		m_overSlot = -1;
		m_usedTypeId = 0;
		if (CMouseCursor::Instance()->GetArea() == CMouseCursor::AREA_ITEMSLOT)
		{
			m_selSlot = float2int((m_mouse.x - 65 - 49) / 45);
			m_overSlot = m_selSlot;
			if (m_overSlot < 0 ||
				m_overSlot >= PLAYER(GOLFDOC()->m_currentPlayer)->itemNum)
			{
				if (CHelpMsg::Instance())
					CHelpMsg::Instance()->SetItemHelpMsg(0);
			}
			else if ((g_input->GetButton(LEFT_BUTTON) == 2 &&
						 CMouseCursor::Instance()->IsInButtonDownArea(
							 LEFT_BUTTON)) ||
				g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
					true))
			{
				used = UseItem(m_overSlot);
			}
			else if (CHelpMsg::Instance())
			{
				CHelpMsg::Instance()->SetItemHelpMsg(slots[m_overSlot]);
			}
			if (m_openPos + 65 - 15 < m_mouse.x &&
				g_input->GetButton(LEFT_BUTTON) == 2 &&
				CMouseCursor::Instance()->IsInButtonDownArea(LEFT_BUTTON))
			{
				CMouseCursor::Instance()->SetItemWindow(false);
				ms_state = STATE_CLOSING;
				g_audio->PlaySfx("play_menu_close");
			}
		}
		if (!used)
		{
			if (m_selSlot >= 0 &&
				m_selSlot < PLAYER(GOLFDOC()->m_currentPlayer)->itemNum)
			{
				if (CHelpMsg::Instance() && m_selSlot != m_overSlot)
					CHelpMsg::Instance()->SetItemHelpMsg(slots[m_selSlot]);
				if (g_input->GetDown("\xbd\xba\xc6\xe4\xc0\xcc\xbd\xba\xb9\xd9",
						true) &&
					UseItem(m_selSlot))
					return;
			}
			if (g_input->GetDown("\xc1\xc2", true))
			{
				WVector movement = g_input->GetMouseDelta();
				if (m_selSlot >= 1)
				{
					--m_selSlot;
					if (CMouseCursor::Instance()->GetArea() ==
							CMouseCursor::AREA_ITEMSLOT &&
						WisZero(movement.x, g_EPSILON) &&
						WisZero(movement.y, g_EPSILON))
						CMouseCursor::Instance()->ForceMove(
							g_input->GetMousePoint().x - 45,
							g_input->GetMousePoint().y, -1);
					if (CHelpMsg::Instance() && m_selSlot != m_overSlot)
						CHelpMsg::Instance()->SetItemHelpMsg(slots[m_selSlot]);
				}
			}
			if (g_input->GetDown("\xbf\xec", true))
			{
				WVector movement = g_input->GetMouseDelta();
				if (m_selSlot < PLAYER(GOLFDOC()->m_currentPlayer)->itemNum - 1)
				{
					++m_selSlot;
					if (CMouseCursor::Instance()->GetArea() ==
							CMouseCursor::AREA_ITEMSLOT &&
						WisZero(movement.x, g_EPSILON) &&
						WisZero(movement.y, g_EPSILON))
						CMouseCursor::Instance()->ForceMove(
							g_input->GetMousePoint().x + 45,
							g_input->GetMousePoint().y, -1);
					if (CHelpMsg::Instance() && m_selSlot != m_overSlot)
						CHelpMsg::Instance()->SetItemHelpMsg(slots[m_selSlot]);
				}
			}
		}
	}
}

bool CItemWindow::ProcessItem(unsigned char index, unsigned long typeId,
	bool local, unsigned long random)
{
	if (index >= GOLFDOC()->m_playerNum)
		return false;

	sPlayerData* player = GOLFDOC()->GetPlayer(index);
	unsigned long* slots = GetTidItemSlot(index);
	IFF_STRUCT::sItem* item = ItemManager()->FindItem(typeId);

	if (IsLocalContent((localContentType_t)112))
	{
		int buff = Doc()->GetBuffValue(index, 20);
		m_bFailed = (short)(random % 100) < (item ? item->COM[1] + buff : 100);
	}
	else if (IsLocalContent((localContentType_t)81))
	{
		if (item)
		{
			int prob = item->COM[1];
			CCardManager::Instance()->SetPlayerIndex(
				GOLFDOC()->m_currentPlayer);
			prob -= CCardManager::Instance()->GetCardSuccesssProbUp();
			m_bFailed = (short)(random % 100) < prob;
		}
		else
		{
			m_bFailed = true;
			return false;
		}
	}
	else
	{
		m_bFailed = (short)(random % 100) < (item ? item->COM[1] : 100);
	}

	if (index != GOLFDOC()->m_currentPlayer)
		return false;

	int slot;
	if (local)
	{
		if (m_useSlot < 0 || slots[m_useSlot] != typeId)
			return false;
		--player->itemNum;
		if (m_useSlot < player->itemNum)
			memcpy(slots + m_useSlot, slots + m_useSlot + 1,
				(player->itemNum - m_useSlot) * sizeof(unsigned long));
		slots[player->itemNum] = 0;
		slot = m_useSlot;
		m_useSlot = -1;
	}
	else
	{
		int i;
		for (i = 0; i < player->itemNum; ++i)
			if (slots[i] == typeId)
				break;
		if (i >= player->itemNum)
			return false;
		--player->itemNum;
		if (i < player->itemNum)
			memcpy(slots + i, slots + i + 1,
				(player->itemNum - i) * sizeof(unsigned long));
		slots[player->itemNum] = 0;
		slot = i;
	}

	if (GOLFDOC()->m_gameMode == 8)
	{
		float y = COption::Instance()->gGetBar_Y();
		m_effectPos.x = slot * 45.0f + 114;
		m_effectPos.y = y - 25 + 2;
		m_failFrame = 0;
		m_effectTime = 0;
		m_effectVel.x = 0;
		m_effectVel.y = -3;
		m_bEffect = true;
	}

	m_usedTypeId = typeId;
	if (m_bFailed)
		typeId = 0x1800002d;
	switch (typeId & 0x1ffffff)
	{
	case 39:
		if (IsLocalContent((localContentType_t)49))
			SendMsg(this, "Player", 286, 0, 0, 0, 0);
		break;
	case 37:
		if (Doc()->m_golfGame.gameType == 1)
		{
			float gauge = TEAM(PLAYER(index)->team)->gauge;
			TEAM(PLAYER(index)->team)->gauge = Between(0.0f, gauge + 33.0f,
				Doc()->GetComboGaugeLimit(PLAYER(index)->team));
		}
		else
			player->gauge = Between(0.0f, player->gauge + 33.0f,
				Doc()->GetComboGaugeLimit(index));
		break;
	case 5:
		SendMsg(this, "GuideLine", 418, 1, 0, 0, 0);
		break;
	case 9:
		PerformPowerBarSetting();
		break;
	case 4:
	case 16:
	case 34:
		SendMsg(this, "Player", 285, 0, 0, 0, 0);
		break;
	}

	player->item = typeId;
	if (local && OnlinePlay())
	{
		for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
			it != Doc()->m_myItemList.end(); it++)
		{
			if ((*it).tid == m_usedTypeId)
			{
				if ((*it).Common[0] > 1)
					--(*it).Common[0];
				else
					Doc()->m_myItemList.erase(it);
				break;
			}
		}
		if (!(Doc()->m_curChannel.Type & 0x88))
			memcpy(Doc()->m_myInfo.userEquip.tidItemSlot,
				Doc()->m_userInfo[index].userEquip.tidItemSlot, 40);
	}

	if (m_bFailed)
		g_audio->PlaySfx("play_fail_item");
	else if (m_usedTypeId == 0x1800000f)
		g_audio->PlaySfx(MakeStr("t_Ex9-%d", rand() % 4 + 1));
	else
		g_audio->PlaySfx("play_use_item");

	if (index == GOLFDOC()->m_currentPlayer && m_usedTypeId == 0x1800000e)
	{
		SendMsg(this, "GolfRule", 156, 0, 0, 0, 0);
		player->item = 0;
	}

	if (index == GOLFDOC()->m_currentPlayer && m_usedTypeId == 0x18000028)
		SendMsg(this, "Screen", 514, 0, 0, 0, 0);

	return true;
}

bool CItemWindow::IsUsable(unsigned long typeId)
{
	if (false)
		ProcessItem(0, 0, false, 0);
	if (!typeId || (typeId & 0xfc000000) != 0x18000000)
		return false;
	switch (typeId & 0x1ffffff)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 7:
	case 8:
	case 10:
	case 11:
	case 15:
	case 17:
	case 18:
	case 19:
	case 31:
	case 33:
		return GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2;
	case 12:
		if ((int)(float)PLAYER(GOLFDOC()->m_currentPlayer)->gauge < 33)
			return false;
	case 4:
	case 6:
	case 22:
	case 32:
	case 34:
	case 37:
	case 39:
		return (GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2 &&
				   GolfClub().GetType() != 3)
			? 1
			: 0;
	case 5:
		return GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) == 2;
	case 9:
		return true;
	case 13:
		return false;
	case 14:
		return GolfBall().m_bOB;
	case 16:
		return (GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2 &&
				   GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2 &&
				   GolfClub().GetType() != 3)
			? 1
			: 0;
	case 40:
		if (IsLocalContent((localContentType_t)47))
		{
			return (GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) != 2 &&
					   GetPVS().GetGroundType(GolfBall().m_pos, true, NULL) !=
						   0)
				? 1
				: 0;
		}
		return false;
	default:
		return false;
	}
}

bool CItemWindow::UseItem(int slot)
{
	if (Doc()->m_myInfo.info.IsIdentity(2) == true)
		return false;

	unsigned long typeId = GetTidItemSlot(GOLFDOC()->m_currentPlayer)[slot];

	if (!IsUsable(typeId))
	{
		if (CHelpMsg::Instance())
			CHelpMsg::Instance()->SetHelpMsg(17, 0);
		return false;
	}

	if (typeId == 0x18000009 && Doc()->m_gameMode != 2 &&
		Doc()->m_gameMode != 1 && S5::IsUsableCalipers() == true)
	{
		if (CHelpMsg::Instance())
			CHelpMsg::Instance()->SetHelpMsg(27, 0);
		return false;
	}

	CMouseCursor::Instance()->SetItemWindow(false);
	m_useSlot = slot;
	ms_state = STATE_CLOSING;

	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)23);
		packet.Encode4(typeId);
		packet.Send((eSendTo)0);
	}
	else if (Doc()->m_gameMode != 5)
	{
		SendMsg(this, "GolfRule", 146, 0, 0, 0, 0);
		ProcessItem(GOLFDOC()->m_currentPlayer, typeId, true, rand());
	}

	return true;
}

void CItemWindow::NetUseItem(unsigned long uid, unsigned long typeId,
	unsigned long random)
{
	if ((Doc()->m_myInfo.info.dwIdentity & 0xe) == 0xe &&
		ms_state != STATE_CLOSED)
	{
		CMouseCursor::Instance()->SetItemWindow(false);
		CHelpMsg::Instance()->SetItemHelpMsg(typeId);
		ms_state = STATE_CLOSING;
		m_selSlot = -1;
		m_overSlot = -1;
	}

	if (OnlinePlay())
	{
		for (unsigned char i = 0; i < GOLFDOC()->m_playerNum; ++i)
		{
			if (PLAYER(i)->state != 3 && PLAYER(i)->oid == uid)
			{
				bool gallery = false;
				ProcessItem(i, typeId, uid == MyGuid(gallery), random);
				break;
			}
		}
	}
	else
	{
		ProcessItem(GOLFDOC()->m_currentPlayer, typeId, true, random);
	}
}

void CItemWindow::Draw()
{
	if (false)
		CItemWindow::OnProcess(0);

	if (GOLFDOC()->m_tutorialMode >= 15 || GOLFDOC()->m_tutorialMode == 5)
	{
		if (ms_state != STATE_CLOSED)
		{
			if (m_pWindow)
				m_pWindow->Render(g_view, WRect(0, 0, 1, 1), m_dest, 0x2000000,
					-1, 0, 0);
			float y = COption::Instance()->gGetBar_Y();
			unsigned long* slots = GetTidItemSlot(GOLFDOC()->m_currentPlayer);
			WRect rect;
			rect.x = IsLocalContent((localContentType_t)81)
				? (10 - PLAYER(GOLFDOC()->m_currentPlayer)->itemNum) * 45.0f +
					m_dest.x + 49 - 4
				: (10 - PLAYER(GOLFDOC()->m_currentPlayer)->itemNum) * 45.0f +
					m_dest.x + 57;
			rect.h = 54;
			rect.w = 54;
			rect.y = y - 25 + 8;
			for (int i = 0; i < 10; ++i)
			{
				if (rect.x < 65 || !slots[i])
					continue;
				WOverlay* overlay = NULL;
				std::map<unsigned long, WOverlay*>::iterator it =
					m_iconMap.find(slots[i]);
				if (it != m_iconMap.end())
					overlay = (*it).second;
				else
				{
					IFF_STRUCT::sItem* item = ItemManager()->FindItem(slots[i]);
					if (item)
					{
						overlay = g_resrcmng->GetOverlay(
							MakeStr("%s.tga", item->c.Icon), 0);
						if (overlay)
							m_iconMap.insert(
								std::pair<const unsigned long, WOverlay*>(
									slots[i], overlay));
					}
				}
				if (overlay)
				{
					if (i == m_selSlot)
					{
						WRect sel(rect.x - 8, rect.y - 8, 70, 70);
						overlay->Render(g_view, WRect(0, 0, 1, 1), sel, 0, -1,
							0, 0);
					}
					else
						overlay->Render(g_view, WRect(0, 0, 1, 1), rect, 0, -1,
							0, 0);
				}
				rect.x += 45;
			}
		}
		if (m_bEffect)
		{
			if (m_bFailed)
			{
				if (m_pFailUI)
					m_pFailUI->Render(MakeStr("%d", m_failFrame), m_effectPos.x,
						m_effectPos.y, 1, 0);
			}
			else
			{
				WOverlay* overlay = NULL;
				std::map<unsigned long, WOverlay*>::iterator it =
					m_iconMap.find(m_usedTypeId);
				if (it != m_iconMap.end())
					overlay = (*it).second;
				else
				{
					IFF_STRUCT::sItem* item =
						ItemManager()->FindItem(m_usedTypeId);
					if (!item)
						return;
					overlay = g_resrcmng->GetOverlay(
						MakeStr("%s.tga", item->c.Icon), 0);
					if (!overlay)
						return;
					m_iconMap.insert(std::pair<const unsigned long, WOverlay*>(
						m_usedTypeId, overlay));
				}
				if (overlay)
					overlay->Render(g_view, WRect(0, 0, 1, 1),
						WRect((float)(int)m_effectPos.x,
							(float)(int)m_effectPos.y,
							(float)overlay->GetWidth(),
							(float)overlay->GetHeight()),
						0, -1, 0, 0);
			}
		}
	}
}

void CItemWindow::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 404:
		CheckInput();
		break;
	case 405:
		ms_state = STATE_OPENING;
		ExecuteNow(false);
		m_bInputEnable = true;
		break;
	case 406:
		ms_state = STATE_CLOSING;
		ExecuteNow(false);
		m_bInputEnable = false;
		break;
	case 409:
		*(int*)msg.param1 = ms_state;
		break;
	case 407:
		if (ms_state != STATE_CLOSED)
			*(int*)msg.param1 = 1;
		else
			*(int*)msg.param1 = 0;
		break;
	case 408:
		Draw();
		break;
	case 410:
		m_bEnable = true;
		break;
	case 411:
		m_bEnable = false;
		break;
	case 414:
		UseItem(msg.param1);
		break;
	case 415:
		NetUseItem(msg.param1, msg.param2, msg.param3);
		break;
	case 416:
		ResetVars();
		break;
	case 10011:
		ResetUiPosition();
		break;
	}
}

void CItemWindow::ResetUiPosition()
{
	float y = COption::Instance()->gGetBar_Y();

	if (m_pWindow)
	{
		m_clip.x = s_barStart - 75;
		m_clip.y = y - 25;
		m_clip.w = (float)m_pWindow->GetWidth();
		m_clip.h = (float)m_pWindow->GetHeight();
		m_pWindow->SetClippingArea(&m_clip);
	}

	m_dest.x = -444;
	m_dest.y = y - 25;
	m_dest.w = m_clip.w;
	m_dest.h = m_clip.h;
}

bool CItemWindow::IsClosed()
{
	if (ms_state == STATE_CLOSED)
		return true;
	return false;
}

unsigned long* CItemWindow::GetTidItemSlot(unsigned char player)
{
	IActor* tutorial = GetActor("Tutorial");

	if (tutorial && player == 0)
	{
		unsigned long* slots = NULL;
		tutorial << MsgObject(NULL, 338, (int)&slots, 0, 0, 0, 0);
		if (slots)
			return slots;
	}

	return Doc()->m_userInfo[player].userEquip.tidItemSlot;
}
