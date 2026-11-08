#include "golfdoc.h"
#include "golfrule.h"
#include "golfrulebase.h"
#include "clientsetting.h"
#include "polysoup.h"

CGolfRuleNewApproach::CGolfRuleNewApproach(CGolfRule* pGolfRule)
{
	m_pGolfRule = pGolfRule;

	GOLFDOC()->m_bNoStat = true;
}

CGolfRuleNewApproach::~CGolfRuleNewApproach()
{
}

void CGolfRuleNewApproach::SetReady()
{
	CGolfRuleBase::SetReady();

	GOLFDOC()->ApproachDataAllClear();
}

void CGolfRuleNewApproach::SetGrade()
{
}

WVector CGolfRuleNewApproach::GetRandomStartPos(unsigned long oid)
{
	Waabb area = GOLFDOC()->m_pPolySoup->GetExtraHoleStartArea();

	WVector pos;

	pos.x = ((oid + Doc()->m_holeRandom[oid % 18]) & 0x7f) *
			(area.max.x - area.min.x) * 0.0078125f +
		area.min.x;
	pos.y = 0.0f;
	pos.z = ((oid + Doc()->m_holeRandom[17 - oid % 18]) & 0x7f) *
			(area.max.z - area.min.z) * 0.0078125f +
		area.min.z;

	return pos;
}

void CGolfRuleNewApproach::SetPlayer()
{
	if (m_pGolfRule == NULL)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		GOLFDOC()->MakePlayer(1);

		std::list<sSlotInfo>& slotList = Doc()->m_slotList;
		Doc()->m_rivalList.reserve(slotList.size());

		for (std::list<sSlotInfo>::iterator it = slotList.begin();
			it != slotList.end(); ++it)
		{
			if ((*it).dwGuid == -1)
			{
				GOLFDOC()->GetPlayer(0)->state = 3;
				continue;
			}

			Doc()->SetIndex((*it).dwGuid);
			sRivalData rival;
			rival.hole = Doc()->m_holeOrder[0];
			rival.rank = 1;
			rival.state = 0;
			rival.totalStroke = 0;
			rival.totalScore = 0;
			rival.totalPang = 0;
			rival.totalBonusPang = 0;

			rival.uid = (*it).dwUserUID;
			rival.oid = (*it).dwGuid;
			rival.ballPos = GetRandomStartPos(rival.oid);

			rival.titleTypeId = (*it).dwTitle;
			rival.team = (*it).bTeam;
			rival.flag = (*it).gender;

			memset(rival.holeStroke, 0, sizeof(rival.holeStroke));
			rival.observerState = 1;
			rival.observerRank = 1;
			memset(rival.holeScore, 0, sizeof(rival.holeScore));
			memset(rival.holePang, 0, sizeof(rival.holePang));
			rival.order = (*it).connectionRank;
			rival.quitOrder = slotList.size();
			rival.finishOrder = slotList.size();
			rival.level = (*it).level;
			strcpy(rival.nickname, (*it).sNick);
			strcpy(rival.guildName, (*it).sGuild);
			rival.guildUID = (*it).GuildId;

			strcpy(rival.guildMark, (*it).szEmblemName);
			rival.guildPoint = 0;
			WVector diff;
			diff = WVector(GOLFDOC()->GetHoleData().pin) - rival.ballPos;

			rival.approachDistance = (int)floor(
				(double)((sqrtf(diff.x * diff.x + diff.z * diff.z) * 10.0f) *
					0.3125f));

			rival.approachResultDistance = -1;
			rival.approachTime = -1;
			rival.capability = (*it).dwIdentity;
			rival.mascotTypeId = (*it).tidMascot;
			rival.pangMastery = (*it).pangma[0];
			rival.pangNitro = (*it).pangma[1];
			Doc()->m_rivalList.push_back(rival);

			if ((*it).dwGuid == MyGuid(true))
			{
				sPlayerData* pPlayer = GOLFDOC()->GetPlayer(0);
				pPlayer->team = (*it).bTeam;
				m_pGolfRule->AddPlayer(0, (*it).sNick, (*it).dwUserUID,
					(*it).dwGuid, Doc()->m_userInfo[0].caddieInfo.tid);

				GOLFDOC()->GetHoleData().tee = rival.ballPos;
			}

			Doc()->ClearUserInfoTimeMap((*it).dwUserUID);
		}

		GOLFDOC()->RefreshPlayerNum();

		m_pGolfRule->SendHoleData();
	}
	else
	{
		Doc()->m_golfGame.shotTimeLimit = 0;
		COption::Instance()->dSetNumPlayers(1);
		GOLFDOC()->MakePlayer(1);

		m_pGolfRule->LoadShot("save/pangya_000.sav");

		GOLFDOC()->m_pPolySoup->LoadMapCheckData(
			m_pGolfRule->GetShotData()->hole);
	}
	Doc()->m_approachStartTime = 0;
}
