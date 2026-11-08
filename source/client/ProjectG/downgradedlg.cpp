class FrDowngradeDlg;
#include "helpdlg.h"
#include "downgradedlg.h"
#include "projectg.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrDowngradeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrDowngradeDlg, FrForm)

ON_FRESH_VI("title", FRCMD_INIT, FrDowngradeDlg::OnTitleInit)
ON_FRESH_VI("help", FRCMD_INIT, FrDowngradeDlg::OnHelpBtnInit)
ON_FRESH_VV("help", FRCMD_LBUTTONUP, FrDowngradeDlg::OnHelpBtnUp)
ON_FRESH_VI("power", FRCMD_INIT, FrDowngradeDlg::OnPowerInit)
ON_FRESH_VV("power", FRCMD_LBUTTONUP, FrDowngradeDlg::OnPowerBtnUp)
ON_FRESH_VI("control", FRCMD_INIT, FrDowngradeDlg::OnControlInit)
ON_FRESH_VV("control", FRCMD_LBUTTONUP, FrDowngradeDlg::OnControlBtnUp)
ON_FRESH_VI("impact", FRCMD_INIT, FrDowngradeDlg::OnImpactInit)
ON_FRESH_VV("impact", FRCMD_LBUTTONUP, FrDowngradeDlg::OnImpactBtnUp)
ON_FRESH_VI("spin", FRCMD_INIT, FrDowngradeDlg::OnSpinInit)
ON_FRESH_VV("spin", FRCMD_LBUTTONUP, FrDowngradeDlg::OnSpinBtnUp)
ON_FRESH_VI("curve", FRCMD_INIT, FrDowngradeDlg::OnCurveInit)
ON_FRESH_VV("curve", FRCMD_LBUTTONUP, FrDowngradeDlg::OnCurveBtnUp)
ON_FRESH_VI("epower1", FRCMD_INIT, FrDowngradeDlg::OnPowerEdit1Init)
ON_FRESH_VI("econtrol1", FRCMD_INIT, FrDowngradeDlg::OnControlEdit1Init)
ON_FRESH_VI("eimpact1", FRCMD_INIT, FrDowngradeDlg::OnImpactEdit1Init)
ON_FRESH_VI("espin1", FRCMD_INIT, FrDowngradeDlg::OnSpinEdit1Init)
ON_FRESH_VI("ecurve1", FRCMD_INIT, FrDowngradeDlg::OnCurveEdit1Init)
ON_FRESH_VI("epower2", FRCMD_INIT, FrDowngradeDlg::OnPowerEdit2Init)
ON_FRESH_VI("econtrol2", FRCMD_INIT, FrDowngradeDlg::OnControlEdit2Init)
ON_FRESH_VI("eimpact2", FRCMD_INIT, FrDowngradeDlg::OnImpactEdit2Init)
ON_FRESH_VI("espin2", FRCMD_INIT, FrDowngradeDlg::OnSpinEdit2Init)
ON_FRESH_VI("ecurve2", FRCMD_INIT, FrDowngradeDlg::OnCurveEdit2Init)
ON_FRESH_VI("powerbar", FRCMD_INIT, FrDowngradeDlg::OnPowerBarInit)
ON_FRESH_VI("controlbar", FRCMD_INIT, FrDowngradeDlg::OnControlBarInit)
ON_FRESH_VI("impactbar", FRCMD_INIT, FrDowngradeDlg::OnImpactBarInit)
ON_FRESH_VI("spinbar", FRCMD_INIT, FrDowngradeDlg::OnSpinBarInit)
ON_FRESH_VI("curvebar", FRCMD_INIT, FrDowngradeDlg::OnCurveBarInit)
ON_FRESH_VI("help1", FRCMD_INIT, FrDowngradeDlg::OnHelpEdit1Init)
ON_FRESH_VI("help2", FRCMD_INIT, FrDowngradeDlg::OnHelpEdit2Init)

END_FRESH_MSGMAP()

FrDowngradeDlg::FrDowngradeDlg()
{
	memset(m_pBar, 0, sizeof(m_pBar));
	memset(m_pStatBtn, 0, sizeof(m_pStatBtn));
	memset(m_pLevelEdit, 0, sizeof(m_pLevelEdit));
	memset(m_pEnchantEdit, 0, sizeof(m_pEnchantEdit));
	m_clubId = 0;
	m_pHelpDlg = NULL;
}

void FrDowngradeDlg::SetInfo(unsigned long charTypeId, char* upgrade,
	unsigned long* parts, unsigned long* auxParts, int mode)
{
	m_charTypeId = charTypeId;
	m_pUpgrade = upgrade;
	m_pParts = parts;
	m_pAuxParts = auxParts;
	m_mode = mode;
	m_type = 0;
	if (!mode)
	{
		for (int i = 0; i < 5; ++i)
			if (m_pStatBtn[i])
				m_pStatBtn[i]->SetButtonImg("level_up", FrButton::NORMAL);
		if (m_pTitle)
			m_pTitle->SetBgImg("cha_up");
		if (m_pHelpEdit[0])
			m_pHelpEdit[0]->AddText(
				"\xb4\xc9\xb7\xc2\xc0\xbb \xc5\xb0\xbf\xef \xbc\xf6 \xc0\xd6\xb4\xc2 \xc0\xc7\xbb\xf3\xc0\xbb \xb0\xae\xc3\xdf\xbe\xee \xc0\xd4\xbe\xfa\xb4\xd9\xb8\xe9 \xc0\xcf\xc1\xa4\xb7\xae\xc0\xc7 \xc6\xce\xc0\xbb \xc1\xf6\xba\xd2\xc7\xcf\xbf\xa9 \xbf\xa9\xb7\xaf\xba\xd0\xc0\xc7 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
		if (m_pHelpEdit[1])
			m_pHelpEdit[1]->AddText(
				"\xb4\xd9\xb8\xa5 \xc0\xe5\xba\xf1\xbf\xcd \xc1\xb6\xc7\xd5\xbd\xc3 \xc6\xc4\xbf\xf6 \xb4\xc9\xb7\xc2\xc0\xcc 20 \xc0\xcc\xbb\xf3\xc0\xcf \xb0\xe6\xbf\xec \xc4\xc1\xc6\xae\xb7\xd1\xb0\xfa \xc1\xa4\xc8\xae\xb5\xb5 \xb4\xc9\xb7\xc2\xc0\xcc \xc1\xd9\xbe\xee\xb5\xe5\xb4\xc2 \xc6\xd0\xb3\xce\xc6\xbc\xb0\xa1 \xc0\xd6\xc0\xb8\xb4\xcf \xc6\xc4\xbf\xf6 \xb4\xc9\xb7\xc2 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb4\xc2 \xbd\xc5\xc1\xdf\xc8\xf7 \xb0\xe1\xc1\xa4\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				0, true);
	}
	else
	{
		if (m_pTitle)
			m_pTitle->SetBgImg("cha_dn");
		if (m_pHelpEdit[0])
			m_pHelpEdit[0]->AddText(
				"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb7\xce \xc4\xb3\xb8\xaf\xc5\xcd \xb4\xc9\xb7\xc2\xc0\xc7 \xb9\xeb\xb7\xb1\xbd\xba\xb0\xa1 \xb9\xab\xb3\xca\xc1\xae \xbb\xe7\xbf\xeb\xc7\xcf\xb1\xe2 \xbe\xee\xb7\xc1\xbf\xef \xb6\xa7, \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \xb4\xc9\xb7\xc2\xc4\xa1\xb8\xa6 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
		if (m_pHelpEdit[1])
			m_pHelpEdit[1]->AddText(
				"-\xb4\xc9\xb7\xc2 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xd8 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xc0\xe7\xc1\xb6\xc0\xfd\xc7\xd1 \xc8\xc4 \xb4\xd9\xbd\xc3 \xc4\xb3\xb8\xaf\xc5\xcd\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xb6\xa7\xb4\xc2 \xc6\xf2\xbb\xf3\xbd\xc3\xbf\xcd \xb0\xb0\xc0\xba \xc6\xce\xc0\xcc \xbc\xd2\xb8\xf0\xb5\xcb\xb4\xcf\xb4\xd9.",
				0, true);
	}
	SetLevelBar();
}

void FrDowngradeDlg::SetInfo(unsigned long clubId, int mode)
{
	m_clubId = clubId;
	m_mode = mode;
	m_type = 1;
	if (!mode)
	{
		for (int i = 0; i < 5; ++i)
			if (m_pStatBtn[i])
				m_pStatBtn[i]->SetButtonImg("level_up", FrButton::NORMAL);
		if (m_pTitle)
			m_pTitle->SetBgImg("club_up");
		if (m_pHelpEdit[0])
			m_pHelpEdit[0]->AddText(
				"\xb4\xc9\xb7\xc2\xc0\xbb \xc5\xb0\xbf\xef \xbc\xf6 \xc0\xd6\xb4\xc2 \xc5\xac\xb7\xb4\xc0\xbb \xbc\xd2\xc0\xaf\xc7\xcf\xb0\xed \xc0\xd6\xb4\xd9\xb8\xe9 \xc0\xcf\xc1\xa4\xb7\xae\xc0\xc7 \xc6\xce\xc0\xbb \xc1\xf6\xba\xd2\xc7\xcf\xbf\xa9 \xbf\xa9\xb7\xaf\xba\xd0\xc0\xc7 \xc5\xac\xb7\xb4\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
		if (m_pHelpEdit[1])
			m_pHelpEdit[1]->AddText(
				"\xb4\xd9\xb8\xa5 \xc0\xe5\xba\xf1\xbf\xcd \xc1\xb6\xc7\xd5\xbd\xc3 \xc6\xc4\xbf\xf6 \xb4\xc9\xb7\xc2\xc0\xcc 20 \xc0\xcc\xbb\xf3\xc0\xcf \xb0\xe6\xbf\xec \xc4\xc1\xc6\xae\xb7\xd1\xb0\xfa \xc1\xa4\xc8\xae\xb5\xb5 \xb4\xc9\xb7\xc2\xc0\xcc \xc1\xd9\xbe\xee\xb5\xe5\xb4\xc2 \xc6\xd0\xb3\xce\xc6\xbc\xb0\xa1 \xc0\xd6\xc0\xb8\xb4\xcf \xc6\xc4\xbf\xf6 \xb4\xc9\xb7\xc2 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb4\xc2 \xbd\xc5\xc1\xdf\xc8\xf7 \xb0\xe1\xc1\xa4\xc7\xcf\xbd\xc3\xb1\xe2 \xb9\xd9\xb6\xf8\xb4\xcf\xb4\xd9.",
				0, true);
	}
	else
	{
		if (m_pTitle)
			m_pTitle->SetBgImg("club_dn");
		if (m_pHelpEdit[0])
			m_pHelpEdit[0]->AddText(
				"\xc0\xdf\xb8\xf8\xb5\xc8 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb7\xce \xc5\xac\xb7\xb4 \xb4\xc9\xb7\xc2\xc0\xc7 \xb9\xeb\xb7\xb1\xbd\xba\xb0\xa1 \xb9\xab\xb3\xca\xc1\xae \xbb\xe7\xbf\xeb\xc7\xcf\xb1\xe2 \xbe\xee\xb7\xc1\xbf\xef \xb6\xa7, \xc5\xac\xb7\xb4\xc0\xc7 \xb4\xc9\xb7\xc2\xc4\xa1\xb8\xa6 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xbc\xf6 \xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9.",
				0, true);
		if (m_pHelpEdit[1])
			m_pHelpEdit[1]->AddText(
				"-\xb4\xc9\xb7\xc2 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc0\xcc\xbf\xeb\xc7\xd8 \xc5\xac\xb7\xb4\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xc0\xe7\xc1\xb6\xc0\xfd\xc7\xd1 \xc8\xc4 \xb4\xd9\xbd\xc3 \xc5\xac\xb7\xb4\xc0\xc7 \xb4\xc9\xb7\xc2\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xb6\xa7\xb4\xc2 \xc6\xf2\xbb\xf3\xbd\xc3\xbf\xcd \xb0\xb0\xc0\xba \xc6\xce\xc0\xcc \xbc\xd2\xb8\xf0\xb5\xcb\xb4\xcf\xb4\xd9.",
				0, true);
	}
	SetLevelBar();
}

void FrDowngradeDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrDowngradeDlg::OnHelpBtnInit(int param)
{
	m_pHelpBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnHelpBtnUp()
{
	if (!m_pHelpDlg)
	{
		m_pHelpDlg =
			CreateForm<FrHelpDlg>(g_pFresh->GetManager(), this, "help", NULL);
		m_pHelpDlg->Open((FRESH_PFN_RESULT)&FrDowngradeDlg::OnHelpDlgResult,
			true);
	}
}

bool FrDowngradeDlg::OnHelpDlgResult(int result, FrForm* form)
{
	m_pHelpDlg = NULL;
	return true;
}

void FrDowngradeDlg::OnHelpEdit1Init(int param)
{
	m_pHelpEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnHelpEdit2Init(int param)
{
	m_pHelpEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnPowerBarInit(int param)
{
	m_pBar[0] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[0]->SetRange(0, 50, false);
}

void FrDowngradeDlg::OnControlBarInit(int param)
{
	m_pBar[1] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[1]->SetRange(0, 30, false);
}

void FrDowngradeDlg::OnImpactBarInit(int param)
{
	m_pBar[2] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[2]->SetRange(0, 30, false);
}

void FrDowngradeDlg::OnSpinBarInit(int param)
{
	m_pBar[3] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[3]->SetRange(0, 30, false);
}

void FrDowngradeDlg::OnCurveBarInit(int param)
{
	m_pBar[4] = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	m_pBar[4]->SetRange(0, 30, false);
}

void FrDowngradeDlg::OnPowerInit(int param)
{
	m_pStatBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnControlInit(int param)
{
	m_pStatBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnImpactInit(int param)
{
	m_pStatBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnSpinInit(int param)
{
	m_pStatBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnCurveInit(int param)
{
	m_pStatBtn[4] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrDowngradeDlg::OnPowerBtnUp()
{
	OpenDowngrade(0);
}

void FrDowngradeDlg::OnControlBtnUp()
{
	OpenDowngrade(1);
}

void FrDowngradeDlg::OnImpactBtnUp()
{
	OpenDowngrade(2);
}

void FrDowngradeDlg::OnSpinBtnUp()
{
	OpenDowngrade(3);
}

void FrDowngradeDlg::OnCurveBtnUp()
{
	OpenDowngrade(4);
}

void FrDowngradeDlg::OpenDowngrade(unsigned char stat)
{
	unsigned char type = m_mode ? 2 : 0;
	if (m_type == 1)
		++type;
	AfxGetTask()->GetActor("RealMyRoom")
		<< MsgObject(NULL, 212, type, stat, 0, 0, 0);
}

void FrDowngradeDlg::OnPowerEdit1Init(int param)
{
	m_pLevelEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnControlEdit1Init(int param)
{
	m_pLevelEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnImpactEdit1Init(int param)
{
	m_pLevelEdit[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnSpinEdit1Init(int param)
{
	m_pLevelEdit[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnCurveEdit1Init(int param)
{
	m_pLevelEdit[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnPowerEdit2Init(int param)
{
	m_pEnchantEdit[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnControlEdit2Init(int param)
{
	m_pEnchantEdit[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnImpactEdit2Init(int param)
{
	m_pEnchantEdit[2] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnSpinEdit2Init(int param)
{
	m_pEnchantEdit[3] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::OnCurveEdit2Init(int param)
{
	m_pEnchantEdit[4] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrDowngradeDlg::SetLevelNumber(int index)
{
	if (!m_pLevelEdit[index])
		return;
	unsigned char value, limit;
	if (m_type == 0)
	{
		unsigned char item =
			ItemManager()->GetItemLevel((eLvlType)index, m_pParts, m_pAuxParts);
		value = item +
			ItemManager()->GetCharLevel((eLvlType)index, m_charTypeId,
				m_pUpgrade);
		limit = item +
			ItemManager()->GetCharCapacity((eLvlType)index, m_charTypeId,
				m_pParts, m_pAuxParts, Doc()->m_myInfo.stat.Level);
	}
	else
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(m_clubId);
		IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(it->second.tid);
		if (!club)
			return;
		value = club->Attr[index] + it->second.Common[index];
		limit = club->Slot[index];
	}
	m_pLevelEdit[index]->SetLine(1, MakeStr("%d", Min(value, limit)),
		0xffff0000, false, 0);
}

void FrDowngradeDlg::SetEnchantNumber(int index)
{
	if (!m_pEnchantEdit[index])
		return;
	if (m_type == 0)
	{
		unsigned short limit = ItemManager()->GetCharCapacity((eLvlType)index,
			0, m_pParts, m_pAuxParts, Doc()->m_myInfo.stat.Level);
		int value = m_pUpgrade ? m_pUpgrade[index] : 0;
		m_pEnchantEdit[index]->SetLine(1, MakeStr("%d/%d", value, limit),
			0xff000000, false, 0);
	}
	else
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(m_clubId);
		IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(it->second.tid);
		if (!club)
			return;
		unsigned short limit = club->Slot[index];
		m_pEnchantEdit[index]->SetLine(1,
			MakeStr("%d/%d", it->second.Common[index],
				limit - club->Attr[index]),
			0xff000000, false, 0);
	}
}

void FrDowngradeDlg::SetLevelBar()
{
	int limit;
	if (m_type == 0)
	{
		for (unsigned char i = 0; i < 5; ++i)
		{
			int item =
				ItemManager()->GetItemLevel((eLvlType)i, m_pParts, m_pAuxParts);
			limit = item +
				ItemManager()->GetCharCapacity((eLvlType)i, m_charTypeId,
					m_pParts, m_pAuxParts, Doc()->m_myInfo.stat.Level);
			int value = item +
				ItemManager()->GetCharLevel((eLvlType)i, m_charTypeId,
					m_pUpgrade);
			if (m_pBar[i])
			{
				m_pBar[i]->SetDestPos(value, false);
				m_pBar[i]->SetExpand(limit - value);
			}
			if (m_pStatBtn[i])
			{
				if (!m_mode)
					m_pStatBtn[i]->Enable(value < limit);
				else
					m_pStatBtn[i]->Enable(m_pUpgrade[i] > 0);
			}
			SetLevelNumber(i);
			SetEnchantNumber(i);
		}
	}
	else
	{
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(m_clubId);
		if (it == Doc()->m_clubSetMap.end())
			return;
		IFF_STRUCT::sClubSet* club = ItemManager()->FindClubSet(it->second.tid);
		if (!club)
			return;
		for (unsigned char i = 0; i < 5; ++i)
		{
			limit = club->Slot[i];
			int value = club->Attr[i] + it->second.Common[i];
			if (m_pBar[i])
			{
				m_pBar[i]->SetDestPos(value, false);
				m_pBar[i]->SetExpand(limit - value);
			}
			if (m_pStatBtn[i])
			{
				if (!m_mode)
					m_pStatBtn[i]->Enable(value < limit);
				else
					m_pStatBtn[i]->Enable(it->second.Common[i] > 0);
			}
			SetLevelNumber(i);
			SetEnchantNumber(i);
		}
	}
}
