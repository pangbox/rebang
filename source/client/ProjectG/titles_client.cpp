#include "titles_client.h"
#include "../../shared/sharedtitledoc.inl"

void RegisterTitles(TitleManager* titlemanager)
{
	titlemanager->Register(TITLE_PANGYASHOT, new Title21);
	titlemanager->Register(TITLE_STRAIGHTROAD, new Title22);
	titlemanager->Register(TITLE_GREENMASTER, new Title23);
	titlemanager->Register(TITLE_COURSEMASTER, new Title24);
	titlemanager->Register(TITLE_GOLD, new Title25);
	titlemanager->Register(TITLE_SILVER, new Title26);
	titlemanager->Register(TITLE_BRONZE, new Title27);
	titlemanager->Register(TITLE_MANNERANGEL, new Title28);
	titlemanager->Register((eTitle)357, new Title357);
	titlemanager->Register((eTitle)358, new Title358);
	titlemanager->Register((eTitle)359, new Title359);
	titlemanager->Register((eTitle)360, new Title360);
	titlemanager->Register((eTitle)361, new Title361);
}

IMPLEMENT_TITLE(21)
{
	if (!CheckLevel(Doc()->m_myInfo.stat.Level, BEGINNER_E))
	{
		return false;
	}

	if (!CheckPangyaPercent(Doc()->m_myInfo.stat.dwPangya,
			Doc()->m_myInfo.stat.dwDrive, 70))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(22)
{
	if (!CheckLevel(Doc()->m_myInfo.stat.Level, BEGINNER_E))
	{
		return false;
	}

	if (!CheckFairawayPercent(Doc()->m_myInfo.stat.dwFairway,
			Doc()->m_myInfo.stat.dwHole, Doc()->m_myInfo.stat.dwMatchHole, 70))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(23)
{
	if (!CheckLevel(Doc()->m_myInfo.stat.Level, BEGINNER_E))
	{
		return false;
	}

	if (!CheckPuttinPercent(Doc()->m_myInfo.stat.dwPuttIn,
			Doc()->m_myInfo.stat.dwPutt, 80))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(24)
{
	std::map<unsigned char, sMapStatistics> MapStat;
	MapStat.clear();

	std::map<unsigned long, sUserInfoTime>::iterator it;
	int find = Doc()->FindUserInfoTimeUID(MyUID(), it);
	if (find)
	{
		sUserInfoTime* pInfo = &it->second;
		if (pInfo)
		{
			sMapStatistics* pMapStat = pInfo->oldSeason.classicMapStat;
			if (pMapStat)
			{
				for (int i = 0; i < 20; i++)
				{
					if (pMapStat[i].cBestScore != 127)
					{
						MapStat[i] = pMapStat[i];
					}
				}
			}
		}
	}

	if (!CheckCourseMaster(Doc()->m_myInfo, Doc()->GetMyPastMapStat(0),
			MapStat))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(25)
{
	if (!CheckGoldMedal(Doc()->m_myInfo.trophy, 10))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(26)
{
	if (!CheckSilverMedal(Doc()->m_myInfo.trophy, 10))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(27)
{
	if (!CheckBronzeMedal(Doc()->m_myInfo.trophy, 10))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(28)
{
	if (!CheckLevel(Doc()->m_myInfo.stat.Level, BEGINNER_E))
	{
		return false;
	}

	if (!CheckNoMannerPercent(Doc()->m_myInfo.stat.dwNoMannerGameCount,
			Doc()->m_myInfo.stat.dwGameCount, 3))
	{
		return false;
	}

	return true;
}

IMPLEMENT_TITLE(357)
{
	if (!CheckPang(Doc()->m_myInfo.stat.i64Pang, 1000000))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(358)
{
	if (!CheckPang(Doc()->m_myInfo.stat.i64Pang, 3000000))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(359)
{
	if (!CheckPang(Doc()->m_myInfo.stat.i64Pang, 5000000))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(360)
{
	if (!CheckHoleinCount(Doc()->m_myInfo.stat.wHoleInOne, 300))
	{
		return false;
	}
	return true;
}

IMPLEMENT_TITLE(361)
{
	if (!CheckHoleinCount(Doc()->m_myInfo.stat.wHoleInOne, 500))
	{
		return false;
	}
	return true;
}
