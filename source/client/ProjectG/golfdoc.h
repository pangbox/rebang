#pragma once

#include <bitset>
#include "encrypttypes.h"
#include "parttidlist.h"
#include <map>
#include <stack>

class WPolySoup;
struct sApproachResultData;
class CSky;
class CAutoTarget;

enum eTeamName
{
};

enum eApproachMissionType
{
};

enum eApproachPrizeType
{
};

enum eApproachWinnerType
{
};

#pragma pack(push, 1)
struct sWinningPrize
{
	unsigned long holeInOne;
	unsigned long albatross;
	unsigned long eagle;
	unsigned long birdie;
	unsigned long par;
	unsigned long bogey;
};

struct sPrizeInfo
{
	unsigned long typeId;
	unsigned short unknown;
	unsigned short quantity;
	int type;
};

struct sShotResult
{
	unsigned long oid;
	float pos[3];
	unsigned char result;
	unsigned char bunker;
	unsigned char ruleFlag;
	unsigned long pang;
	unsigned long bonusPang;
	unsigned long shotFlag;
	unsigned long actionFlag;
	unsigned short traceNum;
};
#pragma pack(pop)

struct sProperty
{
	float bound;
	float roll;
	float min[3];
	float max[3];
	unsigned char pass;
	unsigned char ground;

	sProperty()
	{
		boundSound[0] = 0;
		rollSound[0] = 0;
	}
	~sProperty() { }

	char boundSound[64];
	char rollSound[64];
	char stepSound[64];
};

class CPropertyInfo
{
public:
	CPropertyInfo() { m_defaultIndex = -1; }
	~CPropertyInfo();

	void Init(const TiXmlDocument& doc);
	int GetPropertyIndex(const char* texture);
	const sProperty& GetProperty(int index);

private:
	std::vector<sProperty> m_property;
	std::map<std::string, int> m_textureIndex;
	int m_defaultIndex;
};

class CMapInfo
{
public:
	void Init(const TiXmlDocument& doc, unsigned char hole);
	const char* FindItem(const std::string& key);

private:
	std::map<std::string, std::string> m_item;
};

struct sWave
{
	char texture[32];
	float width;
	float freq;
};

class CWaveInfo
{
public:
	CWaveInfo() { }
	~CWaveInfo() { m_wave.clear(); }

	void Load(const char* filename);

	char* GetTexture(int index) { return m_wave[index].texture; }
	float GetWidth(int index) { return m_wave[index].width; }
	float GetFreq(int index) { return m_wave[index].freq; }

private:
	std::map<int, sWave> m_wave;
};

struct sRiver
{
	float scrollVel;
	float tileHead;
	float tileSide;
	float depth;
};

class CRiverInfo
{
public:
	CRiverInfo() { }
	~CRiverInfo() { m_river.clear(); }

	void Load(const char* filename);

	float GetScrollVelocity(int index) { return m_river[index].scrollVel; }
	float GetHeadTile(int index) { return m_river[index].tileHead; }
	float GetSideTile(int index) { return m_river[index].tileSide; }
	float GetDepth(int index) { return m_river[index].depth; }

private:
	std::map<int, sRiver> m_river;
};

const float s_barGaugeStart = 105.0f;
ILNUM_GDH
const float s_barEnd = 500.0f;
static const float s_barTextY = 8.0f;
const float s_barStart = 140.0f;
static float s_powerBarWidth = s_barEnd - s_barStart;
static float s_barCenter = (s_barStart + s_barEnd) / 2;

struct sBar
{
	float power;
	float impact;
	float gauge;
	float gaugeStep;
	int grade;
	bool bFullPower;
	int dir;
	float center;
	float calipers;

	void Reset();
	float Persent() const { return (power - s_barStart) / s_powerBarWidth; }
	float Delta() const { return impact - center; }
};

struct sHoleData
{
	WVector tee;
	WVector pin;
	unsigned char par;
	unsigned char playerNum;
	unsigned char rank[4];
	unsigned char result;
	unsigned char carry;
	__int64 pang;
	__int64 basePang;
	unsigned long checkSum;
	unsigned long reserved;
};

struct sCameraData
{
	float distance;
	float height;
	float clearance;
	WVector target;
	float angle;
	float angleOffset;
	WMatrix mat;
	float pitch;
};

struct sPlayerData
{
	int state;
	unsigned char index;
	WVector startPos;
	WVector pos;
	WCrypticValue<unsigned char> stroke[18];
	WCrypticValue<unsigned char> putt[18];
	WCrypticValue<int> pang[18];
	WCrypticValue<int> skinsPang[18];
	unsigned long color;
	unsigned long uid;
	unsigned long oid;
	unsigned char itemNum;
	unsigned long item;
	unsigned char teeOrder;
	WCrypticValue<float> gauge;
	unsigned short unusedWord;
	unsigned char bunker;
	unsigned char timeout;
	unsigned char giveUp;
	unsigned long shotFlag;
	WFlags event;
	unsigned char flag;
	unsigned char team;
	unsigned char teammate;
	unsigned long shotTime;
	unsigned char order : 4;
	unsigned char rank : 4;
	unsigned char totalStroke;
	int score;
	WCrypticValue<int> totalPang;
	WCrypticValue<int> bonusPang;
	unsigned long holeNum;
	WCrypticValue<int> power;
	WCrypticValue<int> accuracy;
	WCrypticValue<int> control;
	WCrypticValue<int> spin;
	WCrypticValue<int> curve;
	unsigned char chatBlock;
	sWinningPrize prize;
	unsigned char extPrizeNum;
	sPrizeInfo extPrize[128];
	unsigned char holeInPose;
	unsigned char holeInCollision;
	__int64 skinsTotalPang;
	__int64 skinsSumPang;
	__int64 skinsHolePang;
	unsigned char level;
};

struct sTeamData
{
	int state;
	WCrypticValue<unsigned char> stroke[18];
	WCrypticValue<unsigned char> putt[18];
	WCrypticValue<int> pang[18];
	unsigned char totalStroke;
	int score;
	WCrypticValue<int> totalPang;
	WCrypticValue<int> bonusPang;
	WVector pos;
	unsigned long color;
	WCrypticValue<float> gauge;
	unsigned char bunker;
	unsigned char shooter;
	unsigned char club;
	unsigned char turn;
	unsigned char teeShooter;
	unsigned char holeInPose;
	unsigned char holeInCollision;
};

class CGolfDoc : public CTaskDoc
{
	DECLARE_OBJECT(CGolfDoc)

protected:
	CGolfDoc();

public:
	virtual ~CGolfDoc();

	void LoadXml(const char* filename, unsigned char hole, bool bReload);
	void UpdateBright(float bright);
	void SendReport();

	void MakeDefaultCamera(bool bApply);
	WMatrix GetDefaultCamera();
	WMatrix MakeTopViewCameraMatrix(bool bKeep);

	struct sTarget
	{
		WPuppet* pet;
		WVector pos;
		bool bShow;

		sTarget()
			: pet(NULL), bShow(false)
		{
		}
	};

	sPlayerData* GetPlayer(unsigned char index);
	sTeamData* GetTeam(unsigned char index);
	void MakePlayer(unsigned char num);
	void MakeTeam();
	void MakeHoleData();
	void ResetTeamVars();
	void RefreshPlayerNum();
	void SetCurrentPlayerNum(int num);
	unsigned char GetCurrentPlayerNum();
	void SetPlayerLevel(unsigned char index);
	void LogStrokeScore();

	unsigned char GetCurrentPlayer() { return m_currentPlayer; }
	unsigned char GetPlayerNum() { return m_playerNum; }
	sHoleData& GetHoleData() { return m_pHoleData[m_currentHole - 1]; }
	sHoleData& GetHoleData(unsigned char hole) { return m_pHoleData[hole - 1]; }
	unsigned char& GetUIAlpha() { return m_uiAlpha; }
	sBar& GetBar() { return m_bar; }
	sCameraData& GetCameraData() { return m_cameraData; }

	unsigned long& GameMode() { return m_gameMode; }

	void InitShotResultMsg();
	void ClearShotResultMsg();

	void NetGameEnd() { m_bNetGameEnd = true; }
	bool IsNetGameEnd() { return m_bNetGameEnd; }
	void SetTeeShotReady(bool bReady) { m_bTeeShotReady = bReady; }
	bool IsTeeShotReady() { return m_bTeeShotReady; }

	CPartTidList& GetPartTidList(unsigned char index);
	void SetPartTidList(unsigned char index, unsigned long charTid,
		unsigned char hairColor, unsigned char shirtColor,
		unsigned long* partTid, unsigned long* partId);

	void SetPause(bool bPause) { m_bPause = bPause; }
	bool IsPause() { return m_bPause; }
	void SetTeamGameWinner(eTeamName team) { m_teamGameWinner = team; }
	eTeamName GetTeamGameWinner() { return m_teamGameWinner; }

	void SetReport(bool bReport) { m_bReported = bReport; }
	bool IsReported() { return m_bReported; }

	void SetDay(bool bDay) { m_bDay = bDay; }
	bool GetDay() { return m_bDay; }

	void ShowChat(bool bShow) { m_bShowChat = bShow; }
	bool IsShowChat() { return m_bShowChat; }

	void SetSaveChatNum(int num) { m_saveChatNum = num; }
	int GetSaveChatNum() { return m_saveChatNum; }

	void SetApproachMissionDesc(const char* desc);
	char* GetApproachMissionDesc();
	void SetApproachMissionType(eApproachMissionType type);
	void SetApproachPrizeType(eApproachPrizeType type);
	void SetApproachWinnerType(eApproachWinnerType type);
	eApproachMissionType GetApproachMissionType() const;
	eApproachPrizeType GetApproachPrizeType() const;
	eApproachWinnerType GetApproachWinnerType() const;
	void SetApproachCurHoleResult(sApproachResultData& result);
	void SetApproachGameResult(sApproachResultData& result);
	void ClearApproachCurHoleResult();
	void ClearApproachGameResult();
	std::list<sApproachResultData*>& GetApproachCurHoleResult();
	std::list<sApproachResultData*>& GetApproachGameResult();
	sApproachResultData* GetAppMyCurHoleResult();
	sApproachResultData* GetAppMyGameResult();
	void AddApproachTotalPrize(unsigned long prize);
	unsigned long GetApproachTotalPrize() const;
	void ApproachDataAllClear();

	void SetStatUpdated(bool bUpdated) { m_bStatUpdated = bUpdated; }
	bool GetStatUpdated() const { return m_bStatUpdated; }

	bool GetApproachReloaded() { return m_bApproachReloaded; }
	void SetApproachReloaded(bool bReloaded)
	{
		m_bApproachReloaded = bReloaded;
	}

#pragma pack(push, 1)
	struct sGameResult
	{
		unsigned long oid;
		unsigned char rank;
		char score;
		unsigned char totalStroke;
		short exp;
		__int64 pang;
		__int64 bonusPang;
		__int64 skinsPang;
	};

	struct sGaugeInfo
	{
		unsigned char phase;
		float pos;
	};
#pragma pack(pop)

	WPolySoup* m_pPolySoup;
	CSky* m_pSky;
	CAutoTarget* m_pAutoTarget;
	CPropertyInfo m_propertyInfo;
	CMapInfo m_mapInfo;
	CWaveInfo m_waveInfo;
	CRiverInfo m_riverInfo;
	WList<char*> m_modelNameList;
	int m_tutorialMode;
	std::list<unsigned long> m_commandList;
	sTarget m_target;
	std::string m_xmlName;
	sPlayerData* m_pPlayerData;
	sTeamData* m_pTeamData;
	eTeamName m_teamGameWinner;
	unsigned char m_currentPlayerNum;
	unsigned char m_playerNum;
	sHoleData* m_pHoleData;
	unsigned char m_uiAlpha;
	sBar m_bar;
	sCameraData m_cameraData;
	unsigned long m_gameMode;
	bool m_bNetGameEnd;
	bool m_bPause;
	bool m_bTeeShotReady;
	bool m_bReported;
	bool m_bDay;
	bool m_bShowChat;
	int m_saveChatNum;
	bool m_bReplay;
	bool m_bShowDropPos;
	bool m_bLockGauge;
	int m_tutorialPower;
	bool m_bGodMode;
	float m_debugBar;
	unsigned char m_currentHole;
	unsigned long m_remainTime;
	unsigned long m_shotTime;
	unsigned char m_shotPhase;
	unsigned char m_currentPlayer;
	float m_bright;
	bool m_bDropRequest;
	bool m_bStopRequest;
	unsigned char m_maxGrade;
	bool m_bStopDlg;
	bool m_bNoStat;
	float m_timeScale;
	float m_maxTimeScale;
	unsigned long m_skinsWinner;
	unsigned long m_skinsDonator;
	unsigned short m_skinsState;
	__int64 m_skinsHolePang;
	__int64 m_skinsCarryPang;
	unsigned long m_skinsMultiple;
	unsigned char m_skinsCarryNum;
	__int64 m_skinsCoursePang;
	__int64 m_skinsForfeitPang;
	unsigned char m_unused1c0[0x168];
	sGameResult m_gameResult[4];
	sShotResult m_shotResult[4];
	sShotProtocol m_shotInfo;
	sGaugeInfo m_gaugeInfo;
	CPartTidList m_partTidList[4];
	bool m_bStatUpdated;
	bool m_bApproachReloaded;
	char m_approachMissionDesc[128];
	eApproachMissionType m_approachMissionType;
	eApproachPrizeType m_approachPrizeType;
	eApproachWinnerType m_approachWinnerType;
	std::list<sApproachResultData*> m_approachCurHoleResult;
	std::list<sApproachResultData*> m_approachGameResult;
	unsigned long m_approachTotalPrize;

	static std::stack<CGolfDoc*, std::list<CGolfDoc*> > m_GolfDocInstanceStack;
};
