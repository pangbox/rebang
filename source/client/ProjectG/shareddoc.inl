inline CSharedDoc* Doc()
{
	return CSharedDoc::Instance();
}

inline CItemManager* ItemManager();

inline int GalleryMode();

inline CGolfDoc* GOLFDOC();

inline unsigned char PrevMap(unsigned char map)
{
	int cur = 17;
	for (int i = 0; i < 18; i++)
	{
		if (s_courseOrder[i] == map)
		{
			cur = i;
			break;
		}
	}

	for (int j = cur - 1; j >= 0; j--)
	{
		if (IsLocalContent(S3_ROOKIE_CHANNEL))
		{
			if ((Doc()->m_curChannel.Type & 0x800) &&
				!Doc()->CanUseRookieChannelMap(s_courseOrder[j]))
				continue;
		}
		return s_courseOrder[j];
	}

	return map;
}

inline unsigned char NextMap(unsigned char map, bool bLoop)
{
	int cur = 0;
	for (int i = 0; i < 18; i++)
	{
		if (s_courseOrder[i] == map)
		{
			cur = i;
			break;
		}
	}

	for (int j = cur + 1; j < 18; j++)
	{
		if (IsLocalContent(S3_ROOKIE_CHANNEL))
		{
			if ((Doc()->m_curChannel.Type & 0x800) &&
				!Doc()->CanUseRookieChannelMap(s_courseOrder[j]))
				continue;
		}
		return s_courseOrder[j];
	}

	return map;
}

inline unsigned char FirstMap()
{
	if (!(Doc()->m_curChannel.Type & 0x800))
		return 19;

	if (IsLocalContent(S3_ROOKIE_CHANNEL) && (Doc()->m_curChannel.Type & 0x800))
		return 11;

	int i;
	for (i = 0; i < 18; i++)
	{
		IFF_STRUCT::sCourse* pCourse =
			ItemManager()->FindCourse(s_courseOrder[i] | 0x28000000);
		if (pCourse->Difficulty < 3)
			break;
	}
	return s_courseOrder[i];
}

inline unsigned char LastMap(bool bAll)
{
	if (!(Doc()->m_curChannel.Type & 0x800))
	{
		if (bAll)
			return 9;
		return 127;
	}

	int i;
	for (i = 17; i >= 0; i--)
	{
		if (bAll && s_courseOrder[i] == 127)
			continue;
		IFF_STRUCT::sCourse* pCourse =
			ItemManager()->FindCourse(s_courseOrder[i] | 0x28000000);
		if (pCourse->Difficulty < 3)
			break;
	}
	return s_courseOrder[i];
}

inline sPlayerData* PLAYER(unsigned char index);
inline unsigned long MyGuid(bool bGallery);

inline bool IsMyTurn(bool bCheck)
{
	switch (Doc()->m_gameMode)
	{
	case 5:
		return PLAYER(GOLFDOC()->m_currentPlayer)->oid == MyGuid(false) ? true
																		: false;

	case 0:
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			if (GalleryMode())
				return bCheck;

			return PLAYER(GOLFDOC()->m_currentPlayer)->oid == MyGuid(false);
		}
		break;
	}

	return true;
}

inline bool IsOthersTurn(bool bCheck)
{
	return !IsMyTurn(bCheck);
}

inline int OfflinePlay()
{
	return Doc()->m_offlinePlay;
}

inline const char* GetCurMapName()
{
	if (Doc()->m_golfGame.pCourse)
		return Doc()->m_golfGame.pCourse->Data;

	return NULL;
}

inline IFF_STRUCT::sCourse* GetCurMapProp()
{
	return Doc()->m_golfGame.pCourse;
}

inline const char* GetGameTypeName(unsigned char gameType);

inline unsigned char GetGameType()
{
	return Doc()->m_golfGame.gameType;
}

inline void SetGameType(unsigned char gameType)
{
	Doc()->m_golfGame.gameType = gameType;
	if (gameType < GAME_TYPE_MAX)
		strcpy(Doc()->m_golfGame.gameTypeName, GetGameTypeName(gameType));
}

inline const sGameTypeInfo* GetGameTypeInfo(unsigned char gameType)
{
	if (gameType >= GAME_TYPE_MAX)
	{
		return NULL;
	}

	return &Doc()->m_gameTypeInfo[gameType];
}

inline const char* GetGameTypeName(unsigned char gameType)
{
	if (gameType >= GAME_TYPE_MAX)
	{
		return NULL;
	}

	return Doc()->m_gameTypeInfo[gameType].name;
}

inline unsigned char GetHoles()
{
	return Doc()->m_golfGame.holes;
}

inline void SetHoles(unsigned char holes)
{
	Doc()->m_golfGame.holes = holes;
}

inline unsigned char GetStartHole()
{
	return Doc()->m_holeOrder[0];
}

inline unsigned char GetHoleType()
{
	return Doc()->m_holeType;
}

inline void SetHoleType(unsigned char holeType)
{
	Doc()->m_holeType = holeType;
}

inline unsigned char GetEndHole()
{
	return Doc()->m_holeOrder[GetHoles() - 1];
}

inline unsigned char GetHoleIndex(unsigned char hole)
{
	for (unsigned char i = 0; i < 18; i++)
	{
		if (Doc()->m_holeOrder[i] == hole)
			return i + 1;
	}
	return 19;
}

inline unsigned char GetWeather()
{
	return Doc()->m_golfGame.weather;
}

inline void SetWeather(unsigned char weather)
{
	Doc()->m_golfGame.weather = weather;
}

inline unsigned char GetDifficulty()
{
	if (Doc()->m_golfGame.pCourse == NULL)
		return 1;
	return Doc()->m_golfGame.pCourse->Difficulty;
}

inline void SetShotTimeLimit(unsigned long limit)
{
	Doc()->m_golfGame.shotTimeLimit = limit;
}

inline unsigned long GetShotTimeLimit()
{
	return Doc()->m_golfGame.shotTimeLimit;
}

inline CItemManager* ItemManager()
{
	return &Doc()->m_itemManager;
}

inline unsigned char GetPlayerNum()
{
	return GOLFDOC()->GetPlayerNum();
}

inline sPlayerData* PLAYER(unsigned char index)
{
	return GOLFDOC()->GetPlayer(index);
}

inline sTeamData* TEAM(unsigned char index)
{
	return GOLFDOC()->GetTeam(index);
}

inline sRivalData* GUILDMATCHUP(unsigned char index)
{
	return &Doc()->m_rivalList[Doc()->GetIndex(Doc()->m_teamPlayerGuid[index])];
}

inline CGolfDoc* GOLFDOC()
{
	return Doc()->m_pGolfDoc;
}

inline WVector GetTeeShotPos()
{
	return GOLFDOC()->GetHoleData().tee;
}

inline WVector GetHoleCupPos()
{
	return GOLFDOC()->GetHoleData().pin;
}

inline unsigned char& UIAlpha()
{
	return GOLFDOC()->m_uiAlpha;
}

inline sBar& Bar()
{
	return GOLFDOC()->m_bar;
}

inline sCameraData& GameCamera()
{
	return GOLFDOC()->m_cameraData;
}

inline unsigned char CSharedDoc::GetIndex(unsigned long uid)
{
	if (uid == 0xffffffff)
	{
		if (IS_KINDOF(CGolfTask, AfxGetTask()))
			return GOLFDOC()->m_currentPlayer;
	}
	else if (OnlinePlay())
	{
		if (m_indexMap.find(uid) == m_indexMap.end())
			return 0xff;

		return m_indexMap[uid];
	}
	else
	{
		if (IS_KINDOF(CGolfTask, AfxGetTask()))
			return GOLFDOC()->m_currentPlayer;
	}

	return 0xff;
}

inline void SetGameOver(bool bGameOver)
{
	Doc()->m_bGameOver = bGameOver;
}

inline bool IsGameOver()
{
	return Doc()->m_bGameOver;
}

// HACK: stands in for an unknown PCH inline that deletes a std::string
inline void DeleteString(std::string* p)
{
	std::auto_ptr<std::string> holder(p);
}

inline unsigned long GetGameTimeLimit()
{
	return Doc()->m_golfGame.gameTimeLimit;
}

inline void SetGameTimeLimit(unsigned long limit)
{
	Doc()->m_golfGame.gameTimeLimit = limit;
}

inline unsigned long MyGuid(bool bGallery)
{
	if (GalleryMode() && bGallery)
		return Doc()->m_myInfo.info.dwGalleryGuid;
	return Doc()->m_myInfo.info.dwGuid;
}

inline unsigned long MyUID()
{
	return Doc()->m_myInfo.info.dwUID;
}

inline int IsMassGame()
{
	switch (Doc()->m_golfGame.gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
		return TRUE;
	}
	return FALSE;
}

inline int IsMassGame(unsigned char gameType)
{
	switch (gameType)
	{
	case GAME_TYPE_30S:
	case GAME_TYPE_30S_TEAM:
	case GAME_TYPE_GUILD_MATCH:
	case GAME_TYPE_APPROACH:
	case GAME_TYPE_NEW_APPROACH:
	case GAME_TYPE_USEMAX:
		return TRUE;
	}
	return FALSE;
}

inline int IsGuildGame()
{
	return Doc()->m_golfGame.gameType == GAME_TYPE_GUILD_MATCH;
}

inline char* MyId()
{
	return Doc()->m_myInfo.info.sID;
}

inline char* MyNick()
{
	return Doc()->m_myInfo.info.sNick;
}

inline unsigned char MyIndex()
{
	if (GalleryMode())
	{
		switch (Doc()->m_golfGame.gameType)
		{
		case GAME_TYPE_30S:
		case GAME_TYPE_30S_TEAM:
		case GAME_TYPE_GUILD_MATCH:
		case GAME_TYPE_APPROACH:
		case GAME_TYPE_NEW_APPROACH:
			return Doc()->GetIndex(Doc()->m_myInfo.info.dwGalleryGuid);
		default:
			return GOLFDOC()->m_currentPlayer;
		}
	}
	return Doc()->GetIndex(Doc()->m_myInfo.info.dwGuid);
}

inline int IsObserver(unsigned long identity)
{
	if (identity == 0xffffffff)
		identity = Doc()->m_myInfo.info.dwIdentity;

	return (identity & 0xe) == 0xe;
}

inline int IsAdministrator(unsigned long identity)
{
	if (identity == 0xffffffff)
		identity = Doc()->m_myInfo.info.dwIdentity;

	return (identity & 4) == 4;
}

inline __int64 MyPang()
{
	return Doc()->m_myInfo.stat.i64Pang;
}

inline __int64 MyCookie()
{
	return Doc()->m_cookie;
}

inline __int64 MyOutsideCash()
{
	return Doc()->m_cash;
}

inline __int64 MyBonusCash()
{
	return Doc()->m_bonusCash;
}

inline int GalleryMode()
{
	if (Doc()->m_myInfo.info.IsIdentity(2))
		return TRUE;

	return FALSE;
}

inline bool IsAngelWing(unsigned long typeId)
{
	switch (typeId)
	{
	case 0x08016800:
	case 0x08058800:
	case 0x08098800:
	case 0x080dc800:
	case 0x08118800:
	case 0x08160800:
	case 0x08190800:
	case 0x081e2800:
	case 0x08214800:
	case 0x08254800:
		return true;
	}

	return false;
}

inline bool IsMannerPlayer()
{
	unsigned long gameCount = Doc()->m_myInfo.stat.dwGameCount;
	if (gameCount < 1)
		gameCount = 1;

	float rate = (float)Doc()->m_myInfo.stat.dwNoMannerGameCount /
		(float)gameCount * 100.0f;

	if (rate < 3.0f)
		return true;

	return false;
}

inline unsigned char GetCurRoomMapType()
{
	return Doc()->m_roomInfo.mapType;
}

inline void SetMyOutsideCash(__int64 cash)
{
	Doc()->m_cash = cash;
}

inline unsigned char GetDiffFlag()
{
	IFF_STRUCT::sCourse* course = Doc()->m_golfGame.pCourse;
	if (!course)
		return 0;
	return course->DiffFlag;
}
