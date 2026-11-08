#include "golfdoc.h"
#include "golfrule.h"
#include "golfrulebase.h"
#include "golfball.h"
#include "quadtree.h"
#include "clientsetting.h"
#include "polysoup.h"
#include "../../shared/localize.h"

CGolfRuleTutorial::CGolfRuleTutorial(CGolfRule* pGolfRule)
{
	m_pGolfRule = pGolfRule;
}

CGolfRuleTutorial::~CGolfRuleTutorial()
{
}

void CGolfRuleTutorial::SetPlayer()
{
	if (m_pGolfRule == NULL)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		m_pGolfRule->SendHoleData();

		GOLFDOC()->MakePlayer(1);
		Doc()->SetIndex(Doc()->m_myInfo.info.dwGuid);
		SetEquipCharCaddieInfoForTutorial();

		switch (Doc()->m_golfGame.gameType)
		{
		case 11:
			m_pGolfRule->AddPlayer(0, Doc()->m_myInfo.info.sNick,
				Doc()->m_myInfo.info.dwUID, Doc()->m_myInfo.info.dwGuid,
				0x1c000001);
			GOLFDOC()->m_tutorialMode = 2;
			break;

		case 12:
			m_pGolfRule->AddPlayer(0, Doc()->m_myInfo.info.sNick,
				Doc()->m_myInfo.info.dwUID, Doc()->m_myInfo.info.dwGuid,
				0x1c000001);
			{
				int a, b, mode = 1, c, d;
				IActor* pActor =
					AfxGetPreservedTask()->GetActor("TutorialMain");
				if (pActor)
				{
					pActor << MsgObject(NULL, 335, (int)&a, (int)&b, (int)&mode,
						(int)&c, (int)&d);
				}
				GOLFDOC()->m_tutorialMode = mode;
			}
			break;
		}

		GOLFDOC()->RefreshPlayerNum();
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
}

void CGolfRuleTutorial::SetGrade()
{
}

void CGolfRuleTutorial::SetReady()
{
	GetPVS().ResetHoleData();
	if (GOLFDOC()->m_tutorialMode != 9 && GOLFDOC()->m_tutorialMode < 15)
		GolfBall().Reset(GolfBall().m_pos);

	GolfBall().m_meshIndex = GetPVS().GetMeshIndex(GolfBall().m_pos, false);
	m_pGolfRule->ResetData(GolfBall().m_pos);

	GOLFDOC()->m_currentPlayer = m_pGolfRule->GetFirstTurn();
	PLAYER(GOLFDOC()->m_currentPlayer)->stroke[GOLFDOC()->m_currentHole - 1] =
		1;
	PLAYER(GOLFDOC()->m_currentPlayer)->state = 0;
	m_pGolfRule->SendMsg(m_pGolfRule, "Logo", 395, 0, 0, 0, 0);
}

void CGolfRuleTutorial::SetEquipCharCaddieInfoForTutorial()
{
	if (!IsLocalContent(S3_TUTORIAL_RENEWAL))
		return;

	memset(&Doc()->m_userInfo[0].charInfo, 0, sizeof(sCharacterInfo));
	memset(&Doc()->m_userInfo[0].caddieInfo, 0, sizeof(sCaddieInfo));
	memset(&Doc()->m_userInfo[0].clubInfo, 0, sizeof(sClubInfo));
	memset(&Doc()->m_userInfo[0].mascotInfo, 0, sizeof(sMascotInfo));
	memset(&Doc()->m_userInfo[0].userEquip, 0, sizeof(sUserEquip));

	Doc()->m_userInfo[0].info = Doc()->m_myInfo.info;
	Doc()->m_userInfo[0].stat = Doc()->m_myInfo.stat;
	Doc()->m_userInfo[0].userEquip.tidSkin[5] =
		Doc()->m_myInfo.userEquip.tidSkin[5];

	sCharacterInfo* pCharInfo = &Doc()->m_userInfo[0].charInfo;
	if (Doc()->m_userInfo[0].info.gender % 2)
	{
		pCharInfo->tid = 0x4000001;
	}
	else
	{
		pCharInfo->tid = 0x4000000;
	}

	Doc()->m_itemManager.GetDefCombo(pCharInfo->tid, pCharInfo->tidParts);

	Doc()->m_userInfo[0].clubInfo.tid = 0x10000000;

	Doc()->m_userInfo[0].userEquip.tidBall = 0x14000000;

	Doc()->m_userInfo[0].caddieInfo.Level = 2;
}
