#include "playerinfodlg.h"
#include "fredit.h"
#include "golfdoc.h"
#include "shareddoc.h"
#include "../../shared/sharedtables.h"

IMPLEMENT_OBJECT(FrPlayerInfoDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrPlayerInfoDlg, FrForm)
ON_FRESH_VI("user", FRCMD_INIT, FrPlayerInfoDlg::OnUserInfoInit)
ON_FRESH_VI("character", FRCMD_INIT, FrPlayerInfoDlg::OnCharInfoInit)
ON_FRESH_VI("caddie", FRCMD_INIT, FrPlayerInfoDlg::OnCadInfoInit)
END_FRESH_MSGMAP()

FrPlayerInfoDlg::FrPlayerInfoDlg()
{
}

FrPlayerInfoDlg::~FrPlayerInfoDlg()
{
}

void FrPlayerInfoDlg::WritePlayerInfoDlg()
{
	char buffer[200];

	_snprintf(buffer, sizeof(buffer), "[\xc0\xaf\xc0\xfa]");
	m_pUserInfo->AddLine(buffer, 0, false);
	_snprintf(buffer, sizeof(buffer), "\xb4\xeb\xc8\xad\xb8\xed: %s",
		Doc()->m_userInfo[MyIndex()].info.sNick);
	m_pUserInfo->AddLine(buffer, 0, false);
	unsigned char* level = &Doc()->m_userInfo[MyIndex()].stat.Level;
	_snprintf(buffer, sizeof(buffer), "\xb7\xb9  \xba\xa7: %s",
		g_LevelTable[*level].name);
	m_pUserInfo->AddLine(buffer, 0, false);
	_snprintf(buffer, sizeof(buffer), "\xb0\xe6\xc7\xe8\xc4\xa1: %d",
		Doc()->m_userInfo[MyIndex()].stat.dwExp);
	m_pUserInfo->AddLine(buffer, 0, false);

	_snprintf(buffer, sizeof(buffer), "[\xc4\xb3\xb8\xaf\xc5\xcd(TID)]");
	m_pCharInfo->AddLine(buffer, 0, false);
	sCharacterInfo* character = &Doc()->m_userInfo[MyIndex()].charInfo;
	IFF_STRUCT::sChar* charItem = ItemManager()->FindChar(character->tid);
	if (charItem)
	{
		_snprintf(buffer, sizeof(buffer), "\xc0\xcc  \xb8\xa7: %s( %d )",
			charItem->c.Name, character->tid);
		m_pCharInfo->AddLine(buffer, 0, false);
	}

	unsigned char index = MyIndex();
	sPlayerData* player = GOLFDOC()->GetPlayer(index);
	if (player)
	{
		_snprintf(buffer, sizeof(buffer), "Power: %d", (int)player->power + 15);
		m_pCharInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "Control: %d", (int)player->control);
		m_pCharInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "Impact: %d", (int)player->accuracy);
		m_pCharInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "Spin: %d", (int)player->spin);
		m_pCharInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "Curve: %d", (int)player->curve);
		m_pCharInfo->AddLine(buffer, 0, false);
	}

	unsigned long* parts = character->tidParts;
	if (parts)
	{
		m_pCharInfo->AddLine("", 0, false);
		_snprintf(buffer, sizeof(buffer), "[Parts(TID)]");
		m_pCharInfo->AddLine(buffer, 0, false);
		for (int i = 0; i < 24; i++)
		{
			if (parts[i])
			{
				IFF_STRUCT::sPart* part = ItemManager()->FindPart(parts[i]);
				if (part)
				{
					_snprintf(buffer, sizeof(buffer), "%s( %d )", part->c.Name,
						parts[i]);
					m_pCharInfo->AddLine(buffer, 0, false);
				}
			}
		}
	}

	parts = character->tidAuxParts;
	if (parts)
	{
		m_pCharInfo->AddLine("", 0, false);
		_snprintf(buffer, sizeof(buffer), "[AuxParts(TID)]");
		m_pCharInfo->AddLine(buffer, 0, false);
		for (int i = 0; i < 5; i++)
		{
			if (parts[i])
			{
				IFF_STRUCT::sAuxPart* part =
					ItemManager()->FindAuxPart(parts[i]);
				if (part)
				{
					_snprintf(buffer, sizeof(buffer), "%s( %d )", part->c.Name,
						parts[i]);
					m_pCharInfo->AddLine(buffer, 0, false);
				}
			}
		}
	}

	m_pCharInfo->AddLine("", 0, false);
	_snprintf(buffer, sizeof(buffer), "[\xc0\xe5\xba\xf1(TID)]");
	m_pCharInfo->AddLine(buffer, 0, false);
	unsigned long club = Doc()->m_userInfo[MyIndex()].userEquip.guidClubSet;
	std::map<unsigned int, sItemInfo>::iterator it =
		Doc()->m_clubSetMap.find(club);
	IFF_STRUCT::sClubSet* clubItem = ItemManager()->FindClubSet(it->second.tid);
	if (clubItem)
	{
		_snprintf(buffer, sizeof(buffer), "%s( %d )", clubItem->c.Name, club);
		m_pCharInfo->AddLine(buffer, 0, false);
	}

	unsigned long ball = Doc()->m_userInfo[MyIndex()].userEquip.tidBall;
	IFF_STRUCT::sBall* ballItem = ItemManager()->FindBall(ball);
	if (ballItem)
	{
		_snprintf(buffer, sizeof(buffer), "%s( %d )", ballItem->c.Name, ball);
		m_pCharInfo->AddLine(buffer, 0, false);
	}

	_snprintf(buffer, sizeof(buffer), "[\xc4\xb3\xb5\xf0(TID)]");
	m_pCadInfo->AddLine(buffer, 0, false);
	sCaddieInfo* caddie = &Doc()->m_userInfo[MyIndex()].caddieInfo;
	IFF_STRUCT::sCaddie* caddieItem = ItemManager()->FindCaddie(caddie->tid);
	if (caddieItem)
	{
		_snprintf(buffer, sizeof(buffer), "\xc0\xcc  \xb8\xa7: %s( %d )",
			caddieItem->c.Name, caddie->tid);
		m_pCadInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "\xb7\xb9  \xba\xa7: %d",
			caddie->Level);
		m_pCadInfo->AddLine(buffer, 0, false);
		_snprintf(buffer, sizeof(buffer), "\xb0\xe6\xc7\xe8\xc4\xa1: %d",
			caddie->Exp);
		m_pCadInfo->AddLine(buffer, 0, false);
		if (caddie->tidPart)
		{
			IFF_STRUCT::sCadItem* part =
				ItemManager()->FindCadItem(caddie->tidPart);
			if (part)
			{
				m_pCadInfo->AddLine("", 0, false);
				_snprintf(buffer, sizeof(buffer), "[Parts(TID)]", part->c.Name,
					caddie->tidPart);
				m_pCadInfo->AddLine(buffer, 0, false);
				_snprintf(buffer, sizeof(buffer), "%s( %d )", part->c.Name,
					caddie->tidPart);
				m_pCadInfo->AddLine(buffer, 0, false);
			}
		}
	}
	else
	{
		m_pCadInfo->AddLine("\xbe\xf8\xc0\xbd", 0, false);
	}
}

void FrPlayerInfoDlg::OnUserInfoInit(int param)
{
	m_pUserInfo = DYNAMIC_CAST(FrEdit, param);
}

void FrPlayerInfoDlg::OnCharInfoInit(int param)
{
	m_pCharInfo = DYNAMIC_CAST(FrEdit, param);
}

void FrPlayerInfoDlg::OnCadInfoInit(int param)
{
	m_pCadInfo = DYNAMIC_CAST(FrEdit, param);
}
