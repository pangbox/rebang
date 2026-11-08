#include "stdafx.h"
#include "voiceitem.h"
#include "gdvoiceitem.h"
#include "bonusticket.h"
#include "parttidlist.h"
#include "soundmanager.h"
#include "commonutil.h"

extern const char* g_plSound[];
static unsigned long wigParts[] = { 0x08000809, 0x08040811, 0x08080808,
	0x080c0003, 0x08100005, 0x08140003, 0x08180005 };

void sVOICECLUBINSERT::operator()(const sGD_VOICEITEM& item) const
{
	CVoiceItem::Instance()->SetSound(item.typeID, item.name, item.state,
		item.numOfSounds);
}

CVoiceItem::CVoiceItem()
{
}

CVoiceItem::~CVoiceItem()
{
}

bool CVoiceItem::PlaySoundA(unsigned long typeID, eWaveState state, int index)
{
	switch (typeID >> 26)
	{
	case 4:
		return ClubPlaySound(typeID, state, index);
	case 5:
		return AztecPlaySound(typeID, state);
	}
	return false;
}

void CVoiceItem::Initialize()
{
	CGDVoiceItem* data = (CGDVoiceItem*)CGameDataDB::Instance()->GetGameData(
		GAMEDATA_VOICEITEM, "VoiceClub");
	if (data)
		std::for_each(data->GetVoiceList().begin(), data->GetVoiceList().end(),
			sVOICECLUBINSERT());
}

void CVoiceItem::Destroy()
{
	std::multimap<unsigned long, cVoice*>::iterator it = m_voiceMap.begin();
	while (it != m_voiceMap.end())
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
			m_voiceMap.erase(it++);
		}
		else
			++it;
	}
	m_voiceMap.clear();
}

void CVoiceItem::SetSound(unsigned long typeID, std::string name,
	unsigned long state, unsigned char numOfSounds)
{
	cVoice* voice = new cVoice;
	voice->state = state;
	voice->name = name;
	voice->numOfSounds = numOfSounds;
	m_voiceMap.insert(
		std::multimap<unsigned long, cVoice*>::value_type(typeID, voice));
}

void CVoiceItem::SetSound(unsigned long typeID, std::string name,
	eWaveState state, unsigned char numOfSounds)
{
	cVoice* voice = new cVoice;
	voice->state = state;
	voice->name = name;
	voice->numOfSounds = numOfSounds;
	m_voiceMap.insert(
		std::multimap<unsigned long, cVoice*>::value_type(typeID, voice));
}

bool CVoiceItem::ClubPlaySound(unsigned long typeID, eWaveState state,
	int index)
{
	std::string name = "";
	cVoice* voice = NULL;
	std::multimap<unsigned long, cVoice*>::iterator it =
		m_voiceMap.find(typeID);
	if (it != m_voiceMap.end())
	{
		while (it != m_voiceMap.upper_bound(typeID))
		{
			cVoice* current = it->second;
			if (current->state & state)
			{
				voice = current;
				name = voice->name;
				break;
			}
			++it;
		}
	}
	if (name.empty())
		return false;
	if (index == -1)
		index = Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid &
			0x3ffffff;
	char buffer[128] = { 0 };
	switch (typeID & 0x3ffffff)
	{
	case 23:
		if (voice->numOfSounds > 1)
		{
			int sound;
			if (IsMassGame())
				sound = rand() % 2;
			else
				sound = CBonusTicket::Instance().GetRandomTicket(
							"CLUBSET_MIRACLE") %
					voice->numOfSounds;
			sprintf(buffer, name.c_str(), index, sound);
			if (!IsExistVoiceFile(buffer))
				sprintf(buffer, name.c_str(), index, 0);
			name = buffer;
		}
		else
		{
			sprintf(buffer, name.c_str(), index);
			name = buffer;
		}
		break;
	case 31:
		sprintf(buffer, name.c_str(), index);
		name = buffer;
		break;
	case 53:
		if (voice->numOfSounds > 1)
		{
			sprintf(buffer, name.c_str(), rand() % 4);
			name = buffer;
		}
		break;
	case 63:
	{
		int sound = 0;
		if (voice->numOfSounds > 1)
			sound = rand() % voice->numOfSounds;
		sprintf(buffer, name.c_str(), index, sound);
		name = buffer;
		if (!IsExistVoiceFile(buffer))
			return false;
	}
	break;
	case 68:
	case 69:
	{
		std::string format = name;
		if (index == 8)
		{
			std::string prefix = (typeID & 0x3ffffff) == 68 ? "negima_club_a_"
															: "negima_club_b_";
			str_Replace(format, "negima_club_", prefix);
		}
		int sound = 0;
		if (voice->numOfSounds > 1)
			sound = rand() % voice->numOfSounds;
		sprintf(buffer, format.c_str(), index, sound);
		name = buffer;
		if (!IsExistVoiceFile(buffer))
			return false;
	}
	break;
	}
	g_audio->PlaySfx(name.c_str());
	return true;
}

bool CVoiceItem::AztecPlaySound(unsigned long typeID, eWaveState state)
{
	std::string name = "";
	std::multimap<unsigned long, cVoice*>::iterator it =
		m_voiceMap.find(typeID);
	if (it != m_voiceMap.end())
	{
		while (it != m_voiceMap.upper_bound(typeID))
		{
			cVoice* current = it->second;
			if (current->state & state)
			{
				name = current->name;
				break;
			}
			++it;
		}
	}
	if (name.empty())
		return false;
	g_audio->PlaySfx(name.c_str());
	return true;
}

bool CVoiceItem::IsExistVoiceFile(char* filename)
{
	std::string source = "";
	std::string matched = "";
	source = filename;
	source += ".wav";
	matched = g_resrcmng->FindMatchFile(source.c_str());
	if (matched.compare(source) == 0)
	{
		matched.clear();
		source.clear();
		return false;
	}
	source.clear();
	matched.clear();
	return true;
}

bool WearWig(unsigned char player);

void CSoundItemManager::PlayCurrentSoundItem()
{
	unsigned long ball =
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall;
	if (!CVoiceItem::Instance()->PlaySoundA(ball, CVoiceItem::WAVESTATE_02, -1))
	{
		if (ball == 335544349)
			g_audio->PlaySfx("HVA_niceshot");
		else if (ball == 335544348)
		{
			int sound = rand() % 8;
			char buffer[255] = { 0 };
			sprintf(buffer, "nagashima00%d", sound + 1);
			g_audio->PlaySfx(buffer);
		}
	}
	if (!CVoiceItem::Instance()->PlaySoundA(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid,
			CVoiceItem::WAVESTATE_02, -1))
	{
		if (WearWig(GOLFDOC()->m_currentPlayer))
		{
			g_audio->PlaySfx(MakeStr("t_Ex8-%d", rand() % 4 + 1));
		}
		else
			g_audio->PlaySfx(g_plSound[6]);
	}
}

bool WearWig(unsigned char player)
{
	CPartTidList* parts = &GOLFDOC()->GetPartTidList(player);
	if (!parts)
		return false;
	unsigned int character = Doc()->m_userInfo[player].charInfo.tid & 0x3ffffff;
	if (character >= 7)
		return false;
	return parts->IsEquipped(wigParts[character], 0);
}
