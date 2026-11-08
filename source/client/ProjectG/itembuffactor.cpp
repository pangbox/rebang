#include "stdafx.h"
#include "itembuffactor.h"
#include "carddlg.h"
#include "cardsystem.h"
#include "wlocalize.h"
extern Fresh* g_pFresh;

struct sAbilityFinder
{
	sAbilityFinder(unsigned long typeId)
		: m_typeId(typeId)
	{
	}
	bool operator()(sSCardAvilityPeriodInfo info)
	{
		return m_typeId == info.tid;
	}
	unsigned long m_typeId;
};

namespace
{
	extern const char* abilityNames[20];

	const char* MakeBuffMessage(const char* message, int duration,
		specialcardavility ability, int value)
	{
		const char* period =
			MakeStr("\xb1\xe2\xb0\xa3\x20\x3a\x20\x25\x64\xba\xd0", duration);
		const char* effect = MakeStr(
			"\xc8\xbf\xb0\xfa\x20\x3a\x20\x25\x73\x20\x2b\x25\x64\x25\x25",
			abilityNames[ability], value);
		return MakeStr("%s\n\\c0xff0000ff\\c%s\n%s\\c0xff000000\\c", message,
			period, effect);
	}
}

namespace
{
	FrForm* OpenResultDlg(unsigned long typeId, FrCmdTarget* target)
	{
		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), target,
			"notify_item", NULL);
		WRect formRect = form->GetRect();
		formRect.w += 55.0f;
		formRect.h += 40.0f;
		form->SetRect(formRect);
		FrButton* ok = DYNAMIC_CAST(FrButton, form->FindChildByName("ok"));
		if (ok)
		{
			WRect rect = ok->GetRect();
			rect.x = (formRect.w - rect.w) * 0.5f;
			rect.y = formRect.h - 80.0f;
			ok->SetClientRect(rect);
		}
		FrEdit* message =
			DYNAMIC_CAST(FrEdit, form->FindChildByName("message"));
		if (message)
		{
			WRect rect = message->GetRect();
			rect.w += 55.0f;
			rect.h += 40.0f;
			message->SetRect(rect);
		}
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
		const char* text = "";
		specialcardavility ability;
		int value, duration, accum;
		if (ItemManager()->GetBuffProperty(typeId, ability, value, duration,
				accum))
			text = MakeBuffMessage(
				"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb\x20\xbb\xe7\xbf\xeb\xc7\xcf\xbf\xa9\x20\xb4\xd9\xc0\xbd\x20\xc8\xbf\xb0\xfa\xb8\xa6\x20\xbe\xf2\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9\x21",
				duration, ability, value);
		FrArea* icon = DYNAMIC_CAST(FrArea, form->FindChildByName("itemicon"));
		FrEdit* name = DYNAMIC_CAST(FrEdit, form->FindChildByName("itemname"));
		if (icon)
		{
			if (common)
				icon->SetBgImg(common->Icon);
			else
				icon->SetBgImg("hide");
		}
		if (name)
		{
			if (common)
				name->AddText(common->Name, 0, true);
			else
				name->AddText("none", 0, true);
			name->AddText("\n", 0, true);
		}
		form->SetMessage(text, false);
		form->Open(NULL, 1);
		return form;
	}

	FrForm* OpenConfirmDlg(unsigned long typeId, FrCmdTarget* target,
		FRESH_PFN_RESULT callback)
	{
		FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), target,
			"notify_item_yesno", NULL);
		WRect formRect = form->GetRect();
		formRect.w += 55.0f;
		formRect.h += 40.0f;
		form->SetRect(formRect);
		FrButton* yes = DYNAMIC_CAST(FrButton, form->FindChildByName("yes"));
		if (yes)
		{
			WRect rect = yes->GetRect();
			rect.x = 55.0f;
			rect.y = formRect.h - 90.0f;
			yes->SetClientRect(rect);
		}
		FrButton* no = DYNAMIC_CAST(FrButton, form->FindChildByName("no"));
		if (no)
		{
			WRect rect = no->GetRect();
			rect.x = formRect.w - rect.w - 55.0f;
			rect.y = formRect.h - 90.0f;
			no->SetClientRect(rect);
		}
		FrEdit* message =
			DYNAMIC_CAST(FrEdit, form->FindChildByName("message"));
		if (message)
		{
			WRect rect = message->GetRect();
			rect.w += 55.0f;
			rect.h += 40.0f;
			message->SetRect(rect);
		}
		IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(typeId);
		const char* text = "";
		specialcardavility ability;
		int value, duration, accum;
		if (ItemManager()->GetBuffProperty(typeId, ability, value, duration,
				accum))
			text = MakeBuffMessage(
				"\xc0\xcc\x20\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb\x20\xbb\xe7\xbf\xeb\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee\x3f",
				duration, ability, value);
		FrArea* icon = DYNAMIC_CAST(FrArea, form->FindChildByName("itemicon"));
		FrEdit* name = DYNAMIC_CAST(FrEdit, form->FindChildByName("itemname"));
		if (icon)
		{
			if (common)
				icon->SetBgImg(common->Icon);
			else
				icon->SetBgImg("hide");
		}
		if (name)
		{
			if (common)
				name->AddText(common->Name, 0, true);
			else
				name->AddText("none", 0, true);
			name->AddText("\n", 0, true);
		}
		form->SetMessage(text, false);
		form->Open(callback, 1);
		return form;
	}
}

namespace
{
	const char* abilityNames[] = { "", "",
		K2L_Compatibility("\xc8\xb9\xb5\xe6\x20\xb0\xe6\xc7\xe8\xc4\xa1"), "",
		"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };
}

IMPLEMENT_ACTOR(CItemBuff, IActor)
std::list<sSCardAvilityPeriodInfo> CItemBuff::ms_BuffItemInfoList;
CItemBuff::CItemBuff()
{
	m_pButtonNormal = m_pButtonOver = NULL;
	m_useTypeId = 0;
}

void CItemBuff::OnLoad()
{
	m_pButtonNormal = g_pFresh->RegisterBitmap("trade_item_edit_bn_normal");
	m_pButtonOver = g_pFresh->RegisterBitmap("trade_item_edit_bn_over");
	m_buttonRect = WRect(103.0f, 66.0f, m_pButtonNormal->Width(),
		m_pButtonNormal->Height());
}

void CItemBuff::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
		StoreDelayedMsg(msg);
	else
		switch (msg.message)
		{
		case 38:
			OpenResultDlg(msg.param1, this);
			break;
		case 582:
			HandleBuffItemSubMsg(msg);
			break;
		}
}

void CItemBuff::HandleBuffItemSubMsg(const MsgObject& msg)
{
	switch (msg.param1)
	{
	case 2:
	{
		bool* handled = (bool*)msg.param2;
		if (TOOLTIP()->GetCurTipIndex() == TOOLTIP_USE)
			*handled = true;
	}
	break;
	case 1:
	{
		unsigned long typeId = msg.param5;
		bool* handled = (bool*)msg.param2;
		if (ItemManager()->IsItemBuff(typeId))
			*handled = true;
		else if (!*handled)
			return;
		ButtonDown((FrListItem*)msg.param3, *(WPoint*)msg.param4);
	}
	break;
	case 0:
	{
		unsigned long typeId = msg.param5;
		bool* handled = (bool*)msg.param2;
		if (ItemManager()->IsItemBuff(typeId))
			*handled = true;
		else if (!*handled)
			return;
		DrawMiniButton((FrListItem*)msg.param3, *(WPoint*)msg.param4);
	}
	break;
	}
}

void CItemBuff::DrawMiniButton(FrListItem* item, const WPoint& pos)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (item->underCursor && (m_buttonRect + item->pos).IsInRect(pos))
	{
		graphic->DrawTexture(m_pButtonOver, m_buttonRect + item->pos,
			0xffffffff, 0);
		TOOLTIP()->SetToolTip(TOOLTIP_USE);
	}
	else
	{
		graphic->DrawTexture(m_pButtonNormal, m_buttonRect + item->pos,
			0xffffffff, 0);
	}
}

void CItemBuff::ButtonDown(FrListItem* item, const WPoint& pos)
{
	if (item && (m_buttonRect + item->pos).IsInRect(pos))
		UseBuffItem((sItemInfo*)item->pData);
}

void CItemBuff::UseBuffItem(sItemInfo* item)
{
	if (item && ItemManager()->FindCommonItem(item->tid))
	{
		m_useTypeId = item->tid;
		bool compatible = true;
		unsigned long conflict = 0;
		IsCompatibleBuff(m_useTypeId, ms_BuffItemInfoList, compatible,
			conflict);
		IsCompatibleBuff(m_useTypeId, Doc()->m_cardAbilityList[0], compatible,
			conflict);
		if (!compatible)
		{
			IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(conflict);
			const char* text;
			if (common)
				text = MakeStr("%s\n%s",
					"\xb4\xd9\xc0\xbd\x20\xbe\xc6\xc0\xcc\xc5\xdb\xb0\xfa\x20\xc7\xd4\xb2\xb2\x20\xbb\xe7\xbf\xeb\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9\x2e",
					common->Name);
			else
				text = MakeStr("%s",
					"\xc7\xd4\xb2\xb2\x20\xbb\xe7\xbf\xeb\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xb4\xc2\x20\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e");
			GetActor("RealMyRoom")
				<< MsgObject(NULL, 35, (int)text, 0, 0, 0, 0);
			m_useTypeId = 0;
		}
		else
			OpenConfirmDlg(item->tid, this,
				(FRESH_PFN_RESULT)&CItemBuff::OnUseBuffItemDlgResult);
	}
}

void CItemBuff::IsCompatibleBuff(unsigned long typeId,
	const std::list<sSCardAvilityPeriodInfo>& buffList, bool& compatible,
	unsigned long& conflictTypeId)
{
	for (std::list<sSCardAvilityPeriodInfo>::const_iterator it =
			 buffList.begin();
		compatible && it != buffList.end(); ++it)
	{
		if (!ItemManager()->IsCompatibleBuff(typeId, (*it).tid))
		{
			conflictTypeId = (*it).tid;
			compatible = false;
		}
	}
}

bool CItemBuff::OnUseBuffItemDlgResult(int result, FrForm* form)
{
	if (result == 1 && m_useTypeId)
	{
		GetActor("RealMyRoom") << MsgObject(NULL, 1, 0, 0, 0, 0, 0);
		WSendPacket packet((enumClientPacket)218);
		packet.Encode4(m_useTypeId);
		packet.Send(TO_GAME);
	}
	m_useTypeId = 0;
	return true;
}

void CItemBuff::ResetBuffList()
{
	ms_BuffItemInfoList.clear();
}

void CItemBuff::ProcessPacket(WReceivedPacket& packet)
{
	if (packet.Decode2() != 390)
		return;
	switch (packet.Decode4())
	{
	case 2:
	{
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 2, 0, 0, 0, 0, 0);
		unsigned long typeId = 0;
		unsigned long result = packet.Decode4();
		if (result == 1)
		{
			packet.Decode4();
			sSCardAvilityPeriodInfo info;
			packet.DecodeBuffer(&info, sizeof(info));
			AddBuffItem(info);
			typeId = info.tid;
			CCardManager::Instance()->SetSPCardAlarm(true);
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 537, typeId, result, 0, 0, 0);
		}
		AfxGetTask()->GetActor("ItemBuff")
			<< MsgObject(NULL, 38, typeId, 0, 0, 0, 0);
	}
	break;
	case 1:
		RemoveBuffItem(packet.Decode4());
		break;
	case 0:
	{
		int count = packet.Decode1();
		while (count-- > 0)
		{
			sSCardAvilityPeriodInfo info;
			packet.DecodeBuffer(&info, sizeof(info));
			AddBuffItem(info);
		}
	}
	break;
	}
}

void CItemBuff::AddBuffItem(sSCardAvilityPeriodInfo& info)
{
	if (!ItemManager()->IsItemBuff(info.tid))
		return;
	std::list<sSCardAvilityPeriodInfo>::iterator it =
		std::find_if(ms_BuffItemInfoList.begin(), ms_BuffItemInfoList.end(),
			sAbilityFinder(info.tid));
	if (it != ms_BuffItemInfoList.end())
	{
		(*it).useEndTime = info.useEndTime;
		(*it).useStartTime.wSecond = HIWORD(g_CurrentTime);
		(*it).useStartTime.wMilliseconds = LOWORD(g_CurrentTime);
	}
	else
	{
		info.useStartTime.wSecond = HIWORD(g_CurrentTime);
		info.useStartTime.wMilliseconds = LOWORD(g_CurrentTime);
		ms_BuffItemInfoList.push_back(info);
	}
	CCardManager::Instance()->UpdateCardBuffDlg();
}

void CItemBuff::RemoveBuffItem(unsigned long typeId)
{
	for (std::list<sSCardAvilityPeriodInfo>::iterator it =
			 ms_BuffItemInfoList.begin();
		it != ms_BuffItemInfoList.end();)
	{
		if ((*it).tid == typeId)
			it = ms_BuffItemInfoList.erase(it);
		else
			++it;
	}
	CCardManager::Instance()->UpdateCardBuffDlg();
}

void CItemBuff::UpdateBuffDlg(FrSPCardBuffDlg* dlg)
{
	if (dlg)
	{
		dlg->SetBuffItem(Doc()->m_cardAbilityList[0], true);
		dlg->SetBuffItem(ms_BuffItemInfoList, false);
	}
}
