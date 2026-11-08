#include "enchantdlg.h"
#include "frwnd.h"
#include "frarea.h"
#include "frbutton.h"
#include "frgaugebar.h"
#include "fredit.h"
#include "fresh.h"
#include "soundmanager.h"
#include "projectg.h"
#include "../../shared/localize.h"

IMPLEMENT_OBJECT(FrEnchantDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrEnchantDlg, FrForm)
ON_FRESH_VI("yes", FRCMD_INIT, FrEnchantDlg::OnYesInit)
ON_FRESH_VV("yes", FRCMD_LBUTTONUP, FrEnchantDlg::OnYesBtnUp)
ON_FRESH_VI("no", FRCMD_INIT, FrEnchantDlg::OnNoInit)
ON_FRESH_VV("no", FRCMD_LBUTTONUP, FrEnchantDlg::OnNoBtnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrEnchantDlg::OnOKInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrEnchantDlg::OnOKBtnUp)
ON_FRESH_VI("gauge", FRCMD_INIT, FrEnchantDlg::OnGaugeBarInit)
ON_FRESH_VI("etext", FRCMD_INIT, FrEnchantDlg::OnTextEditInit)
ON_FRESH_VI("title", FRCMD_INIT, FrEnchantDlg::OnTitleInit)
ON_FRESH_VI("base", FRCMD_INIT, FrEnchantDlg::OnBaseInit)
END_FRESH_MSGMAP()

FrEnchantDlg::FrEnchantDlg()
{
	m_stat = 0;
	m_statName = NULL;
	m_bResultShown = false;
	m_price = 0;
	m_resultCode = 0;
}

void FrEnchantDlg::OnTitleInit(int param)
{
	m_pTitle = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrEnchantDlg::OnBaseInit(int param)
{
	m_pBase = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrEnchantDlg::SetType(unsigned char type, unsigned char stat)
{
	m_type = type;
	m_stat = stat;
	switch (stat)
	{
	case 0:
		m_statColor = 0xffff0000;
		m_statName = "\xc6\xc4\xbf\xf6";
		break;
	case 1:
		m_statColor = 0xffffa100;
		m_statName = "\xc4\xc1\xc6\xae\xb7\xd1";
		break;
	case 2:
		m_statColor = 0xff14cf00;
		m_statName = "\xc1\xa4\xc8\xae\xb5\xb5";
		break;
	case 3:
		m_statColor = 0xff00c9ca;
		m_statName = "\xbd\xba\xc7\xc9";
		break;
	case 4:
		m_statColor = 0xff941de5;
		m_statName = "\xc4\xbf\xba\xea";
		break;
	}
	IFF_STRUCT::sEnchant* enchant = NULL;
	if (type & 1)
	{
		m_itemId = Doc()->m_myInfo.userEquip.guidClubSet;
		if (m_pTitle)
			m_pTitle->SetBgImg((type & 2) ? "club_dn" : "club_up");
		if (m_pBase)
			m_pBase->SetBgImg("club_up_base");
		std::map<unsigned int, sItemInfo>::iterator it =
			Doc()->m_clubSetMap.find(m_itemId);
		if (it != Doc()->m_clubSetMap.end())
			enchant = ItemManager()->FindEnchant(
				((m_stat | 0x340) << 20) | it->second.Common[m_stat]);
	}
	else
	{
		m_itemId = Doc()->m_myInfo.userEquip.guidChar;
		if (m_pTitle)
			m_pTitle->SetBgImg((type & 2) ? "cha_dn" : "cha_up");
		if (m_pBase)
			m_pBase->SetBgImg("cha_up_base");
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.find(m_itemId);
		if (it != Doc()->m_charMap.end())
			enchant = ItemManager()->FindEnchant(
				((m_stat | 0x340) << 20) | it->second.PCL[m_stat]);
	}
	if (enchant)
	{
		if (m_type & 2)
			m_price = 0;
		else
			m_price = enchant->ReqdPang;
		if (m_pText)
			m_pText->ClearLine();
		if (Doc()->m_myInfo.stat.i64Pang >= m_price)
		{
			if (m_pText)
			{
				m_pText->SetClientRect(WRect(45, 45, 200, 100));
				if (m_type & 2)
				{
					m_pText->AddLine(
						MakeStr(
							"%s\xc0\xc7 \\c0xffff0000\\c%s\xb4\xc9\xb7\xc2\\c0xff000000\\c\xc0\xbb",
							(m_type & 1) ? "\xc0\xe5\xba\xf1"
										 : "\xc4\xb3\xb8\xaf\xc5\xcd",
							m_statName),
						0, false);
					m_pText->AddLine(
						"\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
						0, false);
				}
				else
				{
					m_pText->AddLine(
						MakeStr(
							"%s\xc0\xc7 \\c0xffff0000\\c%s\xb4\xc9\xb7\xc2\\c0xff000000\\c\xc0\xbb %s",
							(m_type & 1) ? "\xc0\xe5\xba\xf1"
										 : "\xc4\xb3\xb8\xaf\xc5\xcd",
							m_statName,
							"\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"),
						0, false);
					m_pText->AddLine(
						MakeStr(
							"\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 \\c0xffff0000\\c%I64d\xc6\xce\\c0xff000000\\c\xc0\xcc \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
							m_price),
						0, false);
					m_pText->AddLine(
						MakeStr(
							"%s \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
							(m_type & 2)
								? "\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"
								: "\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"),
						0, false);
				}
			}
			if (m_pYesBtn)
				m_pYesBtn->SetVisible(true);
			if (m_pNoBtn)
				m_pNoBtn->SetVisible(true);
		}
		else
		{
			if (m_pText)
			{
				m_pText->SetClientRect(WRect(45, 55, 200, 100));
				m_pText->AddLine(
					MakeStr(
						"\\c0xffff0000\\c%s\xb4\xc9\xb7\xc2\\c0xff000000\\c\xc0\xbb %s",
						m_statName,
						(m_type & 2)
							? "\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"
							: "\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5"),
					0, false);
				m_pText->AddLine(
					MakeStr(
						"\xc7\xcf\xb1\xe2 \xc0\xa7\xc7\xd8\xbc\xad\xb4\xc2 \\c0xffff0000\\c%I64d\xc6\xce\\c0xff000000\\c\xc0\xcc \xb4\xf5 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9.",
						m_price - Doc()->m_myInfo.stat.i64Pang),
					0, false);
			}
			if (m_pOKBtn)
				m_pOKBtn->SetVisible(true);
		}
	}
}

void FrEnchantDlg::OnProc(const float dt)
{
	if (m_pGaugeBar)
	{
		if (WisEqual(m_pGaugeBar->GetPos(), 100.0f, g_EPSILON) &&
			!m_bResultShown && m_resultCode)
		{
			m_bResultShown = true;
			if (m_pOKBtn)
				m_pOKBtn->Enable(true);
			if (m_pText)
			{
				m_pText->ClearLine();
				switch (m_resultCode)
				{
				case 1:
					m_pText->SetClientRect(WRect(45, 60, 200, 80));
					if (m_type & 1)
						m_pText->AddLine(
							MakeStr(
								"\xc0\xe5\xba\xf1 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
								m_statName),
							0, false);
					else
						m_pText->AddLine(
							MakeStr(
								"\xc4\xb3\xb8\xaf\xc5\xcd \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
								m_statName),
							0, false);
					g_audio->PlaySfx("enchant_up");
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(NULL, 211, m_type & 1, 0, 0, 0, 0);
					break;
				case 2:
					m_pText->SetClientRect(WRect(45, 60, 200, 80));
					if (m_type & 1)
						m_pText->AddLine(
							MakeStr(
								"\xc0\xe5\xba\xf1 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
								m_statName),
							0, false);
					else
						m_pText->AddLine(
							MakeStr(
								"\xc4\xb3\xb8\xaf\xc5\xcd \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbc\xba\xb0\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
								m_statName),
							0, false);
					g_audio->PlayBGM("enchant_down.mp3", true, true);
					AfxGetTask()->GetActor("RealMyRoom")
						<< MsgObject(NULL, 211, m_type & 1, 0, 0, 0, 0);
					break;
				case 6:
					if (m_type & 2)
						m_pText->AddLine(
							"\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
							0, false);
					else
						m_pText->AddLine(
							"\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
							0, false);
					break;
				case 3:
					if (m_type & 2)
						m_pText->AddLine(
							"\xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc0\xa7\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
							0, false);
					else
						m_pText->AddLine(
							"\xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xb8\xa6 \xc0\xa7\xc7\xd1 \xc6\xce\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9.",
							0, false);
					break;
				case 4:
					m_pText->AddLine(
						"\xb4\xf5 \xc0\xcc\xbb\xf3 \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5\xc7\xd2 \xbc\xf6 \xc0\xd6\xb4\xc2 \xbd\xbd\xb7\xd4\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, false);
					break;
				case 5:
					m_pText->AddLine(
						"\xb4\xf5 \xc0\xcc\xbb\xf3 \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
						0, false);
					break;
				}
			}
		}
	}
}

void FrEnchantDlg::OnYesInit(int param)
{
	m_pYesBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pYesBtn)
		m_pYesBtn->SetVisible(false);
}

void FrEnchantDlg::OnYesBtnUp()
{
	if (m_pYesBtn)
		m_pYesBtn->SetVisible(false);
	if (m_pNoBtn)
		m_pNoBtn->SetVisible(false);
	if (m_pOKBtn)
	{
		m_pOKBtn->SetVisible(true);
		m_pOKBtn->Enable(false);
	}
	if (m_pGaugeBar)
	{
		m_pGaugeBar->SetVisible(true);
		m_pGaugeBar->SetDestPos(100, false);
		m_pGaugeBar->SetColor(m_statColor);
		m_pGaugeBar->SetBarSpeed(300.0f);
	}
	if (m_pText)
	{
		m_pText->ClearLine();
		m_pText->SetClientRect(WRect(45, 55, 220, 100));
		if (m_type & 1)
		{
			if (m_type & 2)
				m_pText->AddLine(
					"\xc0\xe5\xba\xf1\xc0\xc7 \xbc\xd3\xbc\xba\xc0\xbb \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, false);
			else
				m_pText->AddLine(
					"\xc0\xe5\xba\xf1\xc0\xc7 \xbc\xd3\xbc\xba\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, false);
		}
		else
		{
			if (m_type & 2)
				m_pText->AddLine(
					"\xc4\xb3\xb8\xaf\xc5\xcd \xb4\xc9\xb7\xc2\xc0\xbb \xb4\xd9\xbf\xee\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, false);
			else
				m_pText->AddLine(
					"\xc4\xb3\xb8\xaf\xc5\xcd \xb4\xc9\xb7\xc2\xc0\xbb \xbe\xf7\xb1\xd7\xb7\xb9\xc0\xcc\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					0, false);
		}
		m_pText->AddLine(
			"\xc0\xe1\xbd\xc3\xb8\xb8 \xb1\xe2\xb4\xd9\xb7\xc1 \xc1\xd6\xbc\xbc\xbf\xe4.",
			0, false);
	}
	WSendPacket packet(0x4b);
	packet.Encode1(m_type);
	packet.Encode1(m_stat);
	packet.Encode4(m_itemId);
	if (IsLocalContent(S4_FIX_ENCHANT))
	{
		std::map<unsigned int, sCharacterInfo>::iterator it =
			Doc()->m_charMap.find(m_itemId);
		packet.EncodeBuffer(&it->second, sizeof(sCharacterInfo));
	}
	packet.Send(TO_GAME);
}

void FrEnchantDlg::OnNoInit(int param)
{
	m_pNoBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pNoBtn)
		m_pNoBtn->SetVisible(false);
}

void FrEnchantDlg::OnNoBtnUp()
{
	Close(FrNONE, true);
}

void FrEnchantDlg::OnOKInit(int param)
{
	m_pOKBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pOKBtn)
		m_pOKBtn->SetVisible(false);
}

void FrEnchantDlg::OnOKBtnUp()
{
	Close(FrOK, true);
}

void FrEnchantDlg::OnGaugeBarInit(int param)
{
	m_pGaugeBar = DYNAMIC_CAST(FrGaugeBar, (FrWnd*)param);
	if (m_pGaugeBar)
	{
		m_pGaugeBar->SetVisible(false);
		m_pGaugeBar->SetBarSpeed(10.0f);
	}
}

void FrEnchantDlg::OnTextEditInit(int param)
{
	m_pText = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}
