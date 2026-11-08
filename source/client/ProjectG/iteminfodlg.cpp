#include "iteminfodlg.h"
#include "../../shared/globalnetworkdefine.h"
#include "uccmanager.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrItemInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrItemInfoDlg, FrForm)

ON_FRESH_VI("portrait", FRCMD_INIT, FrItemInfoDlg::OnPortraitInit)
ON_FRESH_VV("portrait", FRCMD_OWNERDRAW, FrItemInfoDlg::OnPortraitOwnerDraw)
ON_FRESH_VI("name", FRCMD_INIT, FrItemInfoDlg::OnNameInit)
ON_FRESH_VI("level", FRCMD_INIT, FrItemInfoDlg::OnLevelInit)
ON_FRESH_VI("static_power", FRCMD_INIT, FrItemInfoDlg::OnPowerStaticInit)
ON_FRESH_VI("static_control", FRCMD_INIT, FrItemInfoDlg::OnControlStaticInit)
ON_FRESH_VI("static_accuracy", FRCMD_INIT, FrItemInfoDlg::OnAccuracyStaticInit)
ON_FRESH_VI("static_spin", FRCMD_INIT, FrItemInfoDlg::OnSpinStaticInit)
ON_FRESH_VI("static_curve", FRCMD_INIT, FrItemInfoDlg::OnCurveStaticInit)
ON_FRESH_VI("epower1", FRCMD_INIT, FrItemInfoDlg::OnPowerEdit1Init)
ON_FRESH_VI("econtrol1", FRCMD_INIT, FrItemInfoDlg::OnControlEdit1Init)
ON_FRESH_VI("eaccuracy1", FRCMD_INIT, FrItemInfoDlg::OnAccuracyEdit1Init)
ON_FRESH_VI("espin1", FRCMD_INIT, FrItemInfoDlg::OnSpinEdit1Init)
ON_FRESH_VI("ecurve1", FRCMD_INIT, FrItemInfoDlg::OnCurveEdit1Init)
ON_FRESH_VI("epower2", FRCMD_INIT, FrItemInfoDlg::OnPowerEdit2Init)
ON_FRESH_VI("econtrol2", FRCMD_INIT, FrItemInfoDlg::OnControlEdit2Init)
ON_FRESH_VI("eaccuracy2", FRCMD_INIT, FrItemInfoDlg::OnAccuracyEdit2Init)
ON_FRESH_VI("espin2", FRCMD_INIT, FrItemInfoDlg::OnSpinEdit2Init)
ON_FRESH_VI("ecurve2", FRCMD_INIT, FrItemInfoDlg::OnCurveEdit2Init)
ON_FRESH_VI("powerbar", FRCMD_INIT, FrItemInfoDlg::OnPowerBarInit)
ON_FRESH_VI("controlbar", FRCMD_INIT, FrItemInfoDlg::OnControlBarInit)
ON_FRESH_VI("accuracybar", FRCMD_INIT, FrItemInfoDlg::OnAccuracyBarInit)
ON_FRESH_VI("spinbar", FRCMD_INIT, FrItemInfoDlg::OnSpinBarInit)
ON_FRESH_VI("curvebar", FRCMD_INIT, FrItemInfoDlg::OnCurveBarInit)

END_FRESH_MSGMAP()

FrItemInfoDlg::FrItemInfoDlg()
{
	m_pItem = NULL;
	m_pPortrait = NULL;
	m_pName = NULL;
	m_pLevel = NULL;
	memset(m_pBar, 0, sizeof(m_pBar));
	memset(m_pEdit1, 0, sizeof(m_pEdit1));
	memset(m_pEdit2, 0, sizeof(m_pEdit2));
	memset(m_pStatic, 0, sizeof(m_pStatic));
	m_pEnchantArrow[0] = g_pFresh->GetBitmap("trade_enchant_nothing");
	m_pEnchantArrow[1] = g_pFresh->GetBitmap("trade_enchant_upgrade");
}

FrItemInfoDlg::~FrItemInfoDlg()
{
}

void FrItemInfoDlg::OnPortraitInit(int param)
{
	m_pPortrait = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrItemInfoDlg::OnPortraitOwnerDraw()
{
	DrawEnchantArrow();
}

void FrItemInfoDlg::OnNameInit(int param)
{
	m_pName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnLevelInit(int param)
{
	m_pLevel = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrItemInfoDlg::OnPowerStaticInit(int param)
{
	m_pStatic[0] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrItemInfoDlg::OnControlStaticInit(int param)
{
	m_pStatic[1] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrItemInfoDlg::OnAccuracyStaticInit(int param)
{
	m_pStatic[2] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrItemInfoDlg::OnSpinStaticInit(int param)
{
	m_pStatic[3] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrItemInfoDlg::OnCurveStaticInit(int param)
{
	m_pStatic[4] = DYNAMIC_CAST(FrStatic, (FrWnd*)param);
}

void FrItemInfoDlg::OnPowerEdit1Init(int param)
{
	m_pEdit1[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnControlEdit1Init(int param)
{
	m_pEdit1[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnAccuracyEdit1Init(int param)
{
	m_pEdit1[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnSpinEdit1Init(int param)
{
	m_pEdit1[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnCurveEdit1Init(int param)
{
	m_pEdit1[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnPowerEdit2Init(int param)
{
	m_pEdit2[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnControlEdit2Init(int param)
{
	m_pEdit2[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnAccuracyEdit2Init(int param)
{
	m_pEdit2[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnSpinEdit2Init(int param)
{
	m_pEdit2[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnCurveEdit2Init(int param)
{
	m_pEdit2[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrItemInfoDlg::OnPowerBarInit(int param)
{
	m_pBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[0]->SetRange(0, 50, 0);
}

void FrItemInfoDlg::OnControlBarInit(int param)
{
	m_pBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[1]->SetRange(0, 30, 0);
}

void FrItemInfoDlg::OnAccuracyBarInit(int param)
{
	m_pBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[2]->SetRange(0, 30, 0);
}

void FrItemInfoDlg::OnSpinBarInit(int param)
{
	m_pBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[3]->SetRange(0, 30, 0);
}

void FrItemInfoDlg::OnCurveBarInit(int param)
{
	m_pBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[4]->SetRange(0, 30, 0);
}

bool FrItemInfoDlg::OnInit()
{
	SetControlsAbility(false);
	return true;
}

void FrItemInfoDlg::SetItemInfo(sTradeItem* item)
{
	if (!item)
		return;
	m_pItem = item;
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->dwTid);
	if (!common)
		return;
	sUccClothes* clothes =
		UccManager()->FindClothes(item->dwTid, item->UccIndex);
	if (m_pPortrait)
	{
		m_pPortrait->SetBgImg(common->Icon);
		if (IsLocalContent(S4_UCC) && IsUccClothes(item->dwTid))
		{
			const Bitmap* icon = GetUccItemIcon(item->dwGuid);
			if (icon)
				m_pPortrait->SetBgImg(icon);
		}
	}
	if (m_pName)
	{
		if (IsLocalContent(S4_UCC) && clothes && IsUccClothes(item->dwTid))
			m_pName->AddText(MakeStr("\xc0\xcc\xb8\xa7: %s\012\012",
								 GetUccItemName(item->dwGuid)),
				false, true);
		else if (IsAngelWing(common->TypeId) && !IsMannerPlayer())
			m_pName->AddText(
				MakeStr(
					"\xc0\xcc\xb8\xa7: %s \\c0xffff0000\\c(\xc2\xf8\xbf\xeb\xba\xd2\xb0\xa1)\\c0xff000000\\c\012\012",
					common->Name),
				false, true);
		else
			m_pName->AddText(
				MakeStr("\xc0\xcc\xb8\xa7: %s\012\012", common->Name), false,
				true);
	}
	if (m_pLevel && m_pName)
	{
		if (common->IsUnderLvl)
			m_pName->AddText(
				"\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf\012", false,
				true);
		else
			m_pName->AddText(
				"\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3\012", false,
				true);
		m_pLevel->SetBgImg(MakeStr("level_%03d", common->Level + 1));
	}
	if (m_pName)
	{
		std::list<sItemInfo>::iterator it;
		for (it = Doc()->m_cutinList.begin(); it != Doc()->m_cutinList.end();
			++it)
			if (it->tid == m_pItem->dwTid)
				break;
		if (it == Doc()->m_cutinList.end())
			for (it = Doc()->m_myItemList.begin();
				it != Doc()->m_myItemList.end(); ++it)
				if (it->tid == m_pItem->dwTid)
					break;
		if (it != Doc()->m_myItemList.end())
		{
			const unsigned char& type = it->ItemType;
			int days = it->HourRemain / 24;
			int hours = it->HourRemain % 24;
			const bool& expired = it->Expired;
			switch (type)
			{
			case 1:
				m_pName->AddText(
					MakeStr(
						"\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3",
						days, hours),
					false, true);
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if (expired)
					m_pName->AddText(
						"\\c0xffff0000\\c\xbb\xe7\xbf\xeb\xb1\xe2\xb0\xa3\\c0xff000000\\c\xc0\xcc \\c0xffff0000\\c\xb8\xb8\xb7\xe1\\c0xff000000\\c\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9.\012",
						false, true);
				else if (!days && !hours)
					m_pName->AddText(
						"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: 1\xbd\xc3\xb0\xa3\xb9\xcc\xb8\xb8\012",
						false, true);
				else
					m_pName->AddText(
						MakeStr(
							"\xb3\xb2\xc0\xba\xbd\xc3\xb0\xa3: %d\xc0\xcf %d\xbd\xc3\xb0\xa3\012",
							days, hours),
						false, true);
			}
		}
	}
	IFF_STRUCT::sDesc* desc = ItemManager()->FindDesc(m_pItem->dwTid);
	if (desc)
		SetMessage(desc->Desc, false);
	if ((m_pItem->dwTid >> 26) == 4)
	{
		SetControlsAbility(true);
		SetLevelBar();
	}
	else
		SetControlsAbility(false);
}

void FrItemInfoDlg::SetLevelBar()
{
	if (!m_pItem)
		return;
	IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(m_pItem->dwTid);
	if (!club)
		return;
	for (unsigned char i = 0; i < 5; ++i)
	{
		int limit = club->Slot[i];
		int value = club->Attr[i] + m_pItem->arrEnchant[i];
		if (m_pBar[i])
		{
			m_pBar[i]->SetDestPos(value, false);
			m_pBar[i]->SetExpand(limit - value);
		}
		SetLevelNumber(i);
		SetEnchantNumber(i);
	}
}

void FrItemInfoDlg::SetLevelNumber(int index)
{
	if (!m_pItem || !m_pEdit1[index])
		return;
	IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(m_pItem->dwTid);
	if (!club)
		return;
	unsigned char value = club->Attr[index] + m_pItem->arrEnchant[index];
	unsigned char limit = club->Slot[index];
	m_pEdit1[index]->SetLine(1, MakeStr("%d", Min(value, limit)), 0xffff0000,
		false, 0);
}

void FrItemInfoDlg::SetEnchantNumber(int index)
{
	if (!m_pItem || !m_pEdit2[index])
		return;
	IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(m_pItem->dwTid);
	if (!club)
		return;
	unsigned short slots = club->Slot[index];
	m_pEdit2[index]->SetLine(1,
		MakeStr("%d/%d", m_pItem->arrEnchant[index], slots - club->Attr[index]),
		0xff000000, false, 0);
}

void FrItemInfoDlg::SetControlsAbility(bool bEnable)
{
	for (int i = 0; i < 5; ++i)
	{
		if (m_pBar[i])
			m_pBar[i]->SetVisible(bEnable);
		if (m_pEdit1[i])
			m_pEdit1[i]->SetVisible(bEnable);
		if (m_pEdit2[i])
			m_pEdit2[i]->SetVisible(bEnable);
		if (m_pStatic[i])
			m_pStatic[i]->SetVisible(bEnable);
	}
}

void FrItemInfoDlg::DrawEnchantArrow()
{
	// HACK
	if (0)
		SetEnchantNumber(0);
	if (!m_pItem || (m_pItem->dwTid >> 26) != 4)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	WPoint pos(m_rect.x + 248.0f, m_rect.y + 229.0f);
	for (int i = 0; i < 5; ++i)
	{
		int arrow = m_pItem->arrEnchant[i] ? 1 : 0;
		if (m_pEnchantArrow[arrow])
		{
			gdi->DrawTexture(m_pEnchantArrow[arrow],
				WRect(pos.x, pos.y + i * 17, m_pEnchantArrow[arrow]->Width(),
					m_pEnchantArrow[arrow]->Height()),
				0xffffffff, 0);
		}
	}
}
