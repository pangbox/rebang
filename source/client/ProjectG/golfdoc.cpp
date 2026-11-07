#include "minatl.h"
#include "golfdoc.h"
#include "scenemanager.h"
#include "fx.h"
#include "capturedbg.h"
#include "polysoup.h"
#include "sky.h"
#include "autotarget.h"
#include "pangfbi.h"
#include "golfball.h"
#include "club.h"
#include "quadtree.h"
#include "exception.h"
#include "cardsystem.h"
#include "mathconsts.h"
#include <stdio.h>

int CalcPowerPenalty(int upgrade);

IMPLEMENT_OBJECT(CGolfDoc, CTaskDoc)

std::stack<CGolfDoc*, std::list<CGolfDoc*> > CGolfDoc::m_GolfDocInstanceStack;

CGolfDoc::CGolfDoc()
	: m_pPolySoup(NULL),
	  m_pSky(NULL),
	  m_pAutoTarget(NULL),
	  m_modelNameList(8, 8),
	  m_pPlayerData(NULL),
	  m_pTeamData(NULL),
	  m_pHoleData(NULL),
	  m_gameMode(0),
	  m_bNetGameEnd(false),
	  m_bTeeShotReady(false)
{
	Doc()->m_pGolfDoc = this;

	m_GolfDocInstanceStack.push(this);

	m_tutorialMode = 15;
	m_currentPlayerNum = 0;
	m_playerNum = 0;
	m_shotPhase = 0;
	m_currentPlayer = 0;
	m_uiAlpha = 0xff;
	GetCameraData().clearance = 0.5f;
	GetCameraData().angleOffset = 0.34906584f;
	m_bright = 1.0f;
	m_bPause = false;
	m_bGodMode = false;
	m_debugBar = 0.0f;
	m_bReported = false;
	m_bStopRequest = false;
	m_bDropRequest = false;
	m_maxGrade = 0;
	m_bStopDlg = false;
	m_saveChatNum = -1;
	m_bReplay = false;
	m_bNoStat = false;
	m_bShowChat = true;
	m_bShowDropPos = true;
	m_bLockGauge = false;
	m_timeScale = 1.0f;
	m_maxTimeScale = 15.0f;
	m_bDay = true;
	m_skinsWinner = -1;
	m_skinsDonator = -1;
	m_skinsState = 0;
	m_skinsHolePang = 0;
	m_skinsCoursePang = 0;
	m_skinsCarryPang = 0;
	m_skinsForfeitPang = 0;
	m_skinsMultiple = 0;
	m_skinsCarryNum = 0;
	MakeHoleData();

	CStackableSingleton<CSceneManager>::PushInstance();
	CStackableSingleton<CCapturedBg>::PushInstance();

	new CSceneManager;
	new CCapturedBg;

	if (CFx::IsInstantiated())
		CSceneManager::Instance()->RegisterElement(OBJ_EFFECT, NULL,
			CFx::Instance(), NULL, 0, NULL);

	m_pPolySoup = new WPolySoup(this);
	m_pPolySoup->SetResourceManager(g_resrcmng);

	memset(m_gameResult, 0, sizeof(m_gameResult));
	memset(m_shotResult, 0, sizeof(m_shotResult));
	memset(&m_shotInfo, 0, sizeof(m_shotInfo));
	memset(&m_gaugeInfo, 0, sizeof(m_gaugeInfo));

	m_bStatUpdated = true;
	m_bApproachReloaded = false;
	m_approachMissionType = (eApproachMissionType)0;
	m_approachPrizeType = (eApproachPrizeType)0;
	m_approachWinnerType = (eApproachWinnerType)0;
	m_approachCurHoleResult.clear();
	m_approachGameResult.clear();
	m_approachTotalPrize = 0;
}

CGolfDoc::~CGolfDoc()
{
	ClearApproachCurHoleResult();
	ClearApproachGameResult();

	SendReport();

	m_GolfDocInstanceStack.pop();
	if (m_GolfDocInstanceStack.empty())
	{
		Doc()->m_pGolfDoc = NULL;
	}
	else
	{
		Doc()->m_pGolfDoc = m_GolfDocInstanceStack.top();
	}

	if (m_pPolySoup)
	{
		delete m_pPolySoup;
		m_pPolySoup = NULL;
	}
	if (m_pSky)
	{
		delete m_pSky;
		m_pSky = NULL;
	}
	if (m_pAutoTarget)
	{
		delete m_pAutoTarget;
		m_pAutoTarget = NULL;
	}

	if (m_pPlayerData)
	{
		delete[] m_pPlayerData;
		m_pPlayerData = NULL;
	}
	if (m_pTeamData)
	{
		delete[] m_pTeamData;
		m_pTeamData = NULL;
	}
	if (m_pHoleData)
	{
		delete[] m_pHoleData;
		m_pHoleData = NULL;
	}

	for (char* name = m_modelNameList.Start(); name;
		name = m_modelNameList.Next())
	{
		if (name)
			delete[] name;
	}
	m_modelNameList.Reset();

	if (CCapturedBg::IsInstantiated())
		delete CCapturedBg::Instance();
	if (CSceneManager::IsInstantiated())
		delete CSceneManager::Instance();

	CStackableSingleton<CCapturedBg>::PopInstance();
	CStackableSingleton<CSceneManager>::PopInstance();

	if (!CTaskManager::Instance()->GetPreservedTask())
		g_resrcmng->ReleaseAllPets();
}

void CGolfDoc::UpdateBright(float bright)
{
	if (WisEqual(m_bright, bright, g_EPSILON))
		return;

	m_bright = bright;

	g_lightset.diffuse = ModifyColor(g_lightset.diffuse, GOLFDOC()->m_bright);
	g_lightset.ambient = ModifyColor(g_lightset.ambient, GOLFDOC()->m_bright);
	g_lightset.ambient2 = ModifyColor(g_lightset.ambient2, GOLFDOC()->m_bright);

	if (CSceneManager::IsInstantiated())
		CSceneManager::Instance()->UpdateBright();

	if (m_pPolySoup)
		m_pPolySoup->UpdateBright();

	if (m_pSky)
		m_pSky->UpdateBright();
}

void CGolfDoc::SendReport()
{
	if (m_bReported)
	{
		char path[MAX_PATH] = { 0 };
		strcpy(path, g_executeDirectory);

		char buffer[1024] = { 0 };
		FILE* fp = fopen(MakeStr("%s\\chat_history.txt", path), "wt");
		if (fp)
		{
			SYSTEMTIME time;
			GetLocalTime(&time);

			sprintf(buffer,
				"\xbd\xc5\xb0\xed\xbd\xc3\xb0\xa3 : %d:%02d:%02d %02d:%02d:%02d\n",
				time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
				time.wSecond);
			fwrite(buffer, strlen(buffer), 1, fp);

			sprintf(buffer,
				"------------------------------------------------------------\n");
			fwrite(buffer, strlen(buffer), 1, fp);

			sprintf(buffer,
				"\xbe\xc6\xc0\xcc\xb5\xf0 : \xb4\xeb\xc8\xad\xb8\xed\n");
			fwrite(buffer, strlen(buffer), 1, fp);

			if (IsMassGame())
			{
				for (std::vector<sRivalData>::iterator it =
						 Doc()->m_rivalList.begin();
					it != Doc()->m_rivalList.end(); ++it)
				{
					sprintf(buffer, "%s\n", (*it).nickname);
					fwrite(buffer, strlen(buffer), 1, fp);
				}
			}
			else
			{
				for (unsigned char i = 0; i < m_playerNum; i++)
				{
					sprintf(buffer, "\t%s\n", Doc()->m_userInfo[i].info.sNick);
					fwrite(buffer, strlen(buffer), 1, fp);
				}
			}

			sprintf(buffer,
				"------------------------------------------------------------\n");
			fwrite(buffer, strlen(buffer), 1, fp);

			std::list<sChatLine>::iterator it;
			for (it = Doc()->m_chatLineList.begin();
				it != Doc()->m_chatLineList.end(); ++it)
			{
				fwrite(it->text.c_str(), strlen(it->text.c_str()), 1, fp);
				fwrite("\n", 1, 1, fp);
			}

			fclose(fp);

			_snprintf(buffer, 1023, "%s %d %s\\chat_history.txt", MyNick(), 4,
				path);

			WSingleton<CPangFBI>::Instance()->ProcessReport("crime_log",
				buffer);
		}
	}
}

WMatrix CGolfDoc::MakeTopViewCameraMatrix(bool bKeep)
{
	WMatrix mat;
	mat.za = WVector::UNIT_NEG_Y;
	mat.ya = GameCamera().mat.za;
	mat.ya.Normalize();
	mat.xa = WCrossProduct(mat.ya, mat.za);

	WVector dir = GOLFDOC()->GetHoleData().pin;
	dir -= GOLFDOC()->GetPlayer(GOLFDOC()->m_currentPlayer)->pos;
	float dist = sqrtf(dir.z * dir.z + dir.x * dir.x);

	if (g_input->Get("LCONTROL", true) || g_input->Get("RCONTROL", true))
	{
		mat.pivot = (GetHoleCupPos() + GetTeeShotPos()) * 0.5f;
		mat.pivot.y = Max(GetHoleCupPos().y, GetTeeShotPos().y) +
			(Abs(GetHoleCupPos().z - GetTeeShotPos().z) + 480.0f) * 0.5f /
				(tanf(g_view->GetFOV() * 0.5f) * g_view->GetRatio());
	}
	else if ((g_input->Get("LSHIFT", true) || g_input->Get("RSHIFT", true) ||
				 g_input->Get(1)) &&
		IsMyTurn(true))
	{
		int index;
		mat.pivot = GolfBall().m_pos +
			GolfClub().GetRange() * 3.2f * GameCamera().mat.za;
		mat.pivot.y =
			GetPVS().GetGroundPoint(mat.pivot, true, &index, true).y + 64.0f;
		if (index < 0)
		{
			mat.pivot.y = GolfBall().m_pos.y + 64.0f;
		}
	}
	else if (GolfBall().m_groundType == 2 &&
		GolfClub().GetRange() * 3.2f > dist)
	{
		if (dist < 16.0f)
			mat.pivot = GolfBall().m_pos +
				(GameCamera().mat.za * 0.2f + WVector::UNIT_POS_Y) * 32.0f;
		else
			mat.pivot = GolfBall().m_pos +
				(GameCamera().mat.za * 0.4f + WVector(0.0f, 2.0f, 0.0f)) * dist;
	}
	else if (GolfClub().GetApproachMode() <= 3)
	{
		mat.pivot = GolfBall().m_pos +
			(GameCamera().mat.za * 0.5f + WVector(0.0f, 1.4f, 0.0f)) *
				Max(32.0f, dist);
	}
	else
	{
		mat.pivot = GolfBall().m_pos +
			(GameCamera().mat.za * 0.5f + WVector(0.0f, 1.4f, 0.0f)) *
				(GolfClub().GetRange() * 3.2f);
	}

	if (bKeep)
	{
		if (mat.pivot.y < g_camera.pivot.y ||
			(mat.pivot - g_camera.pivot).Magnitude() > 32.0f)
			return g_camera;
	}

	return mat;
}

WMatrix CGolfDoc::GetDefaultCamera()
{
	WMatrix mat;

	WVector pos, target;
	pos = m_cameraData.mat.pivot;
	target = m_cameraData.mat.pivot + m_cameraData.mat.za;
	target.y += tanf(m_cameraData.pitch);

	mat = MakeCameraMatrix(pos, target);

	return mat;
}

void CGolfDoc::MakeDefaultCamera(bool bApply)
{
	WVector dir, tag;

	if (GolfBall().m_groundType == 2)
	{
		m_cameraData.distance = 15.0f;
		m_cameraData.height = 7.0f;
	}
	else
	{
		m_cameraData.distance = 20.0f;
		m_cameraData.height = 5.0f;
	}

	dir = RotMat(m_cameraData.angle, 1).za;
	dir.y = 0.0f;
	dir.Normalize();

	if (IsMyTurn(true) ||
		(IsLocalContent(S4_REPLAY_SYSTEM) && Doc()->m_playingMode == 2))
	{
		m_cameraData.target = GolfBall().m_pos;
	}
	else
	{
		m_cameraData.target = dir * 15.0f + GolfBall().m_pos;
		m_cameraData.distance += 20.0f;

		if (GolfBall().m_groundType == 2)
			m_cameraData.height += 3.0f;

		dir = dir * RotMat(m_cameraData.angleOffset, 1);
	}

	IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].charInfo.tid);
	if (pChar)
	{
		m_cameraData.distance *= pChar->ClubScale;
		m_cameraData.height *= pChar->ClubScale;
	}

	tag = m_cameraData.target + dir;

	m_cameraData.mat = MakeCameraMatrix(m_cameraData.target, tag);
	m_cameraData.mat.pivot -= m_cameraData.distance * m_cameraData.mat.za;
	m_cameraData.mat.pivot.y = Max(
		GetPVS().GetGroundPoint(m_cameraData.mat.pivot, false, NULL, false).y +
			m_cameraData.clearance,
		m_cameraData.target.y + m_cameraData.height);

	if (bApply)
	{
		tag.y += tanf(m_cameraData.pitch);
		g_camera = MakeCameraMatrix(m_cameraData.target, tag);
		g_camera.pivot = m_cameraData.mat.pivot;
	}

	g_view->SetFOV(55.0f * g_DEGTORAD);
}

sPlayerData* CGolfDoc::GetPlayer(unsigned char index)
{
	if (!m_pPlayerData)
		return NULL;

	switch (Doc()->m_golfGame.gameType)
	{
	case 4:
	case 5:
	case 6:
	case 9:
	case 10:
		return m_pPlayerData;
	}

	if (index >= m_playerNum)
		return NULL;

	return &m_pPlayerData[index];
}

sTeamData* CGolfDoc::GetTeam(unsigned char index)
{
	if (!m_pTeamData)
		return NULL;

	return &m_pTeamData[index];
}

void CGolfDoc::MakeTeam()
{
	m_teamGameWinner = (eTeamName)2;
	if (m_pTeamData)
	{
		delete[] m_pTeamData;
		m_pTeamData = NULL;
	}
	m_pTeamData = new sTeamData[2];
	memset(m_pTeamData, 0, sizeof(sTeamData) * 2);
}

void CGolfDoc::MakePlayer(unsigned char num)
{
	m_currentPlayerNum = num;
	m_playerNum = num;

	if (m_pPlayerData)
	{
		delete[] m_pPlayerData;
		m_pPlayerData = NULL;
	}
	m_pPlayerData = new sPlayerData[m_playerNum];
	memset(m_pPlayerData, 0, sizeof(sPlayerData) * m_playerNum);
}

int __cdecl CompareRank(const void* arg1, const void* arg2)
{
	sPlayerData* p1 = (sPlayerData*)arg1;
	sPlayerData* p2 = (sPlayerData*)arg2;
	unsigned char hole = GOLFDOC()->m_currentHole;

	if ((unsigned char)p1->stroke[hole - 1] < p2->stroke[hole - 1])
		return 0;

	if (p1->stroke[hole - 1] == p2->stroke[hole - 1])
	{
		if (p1->state == 1 && p2->state != 1)
			return 0;
	}

	return 1;
}

void CGolfDoc::ResetTeamVars()
{
	if (!m_pTeamData)
		return;

	for (int i = 0; i < 2; i++)
	{
		m_pTeamData[i].color = (i == 0 ? 0xffff0000 : 0xff0060ff) & 0x00ffffff;
		m_pTeamData[i].state = 2;
		m_pTeamData[i].gauge = 0.0f;
		m_pTeamData[i].totalStroke = 0;
		m_pTeamData[i].score = 0;
		m_pTeamData[i].totalPang = 0;
		m_pTeamData[i].bonusPang = 0;
		m_pTeamData[i].shooter = 0;
		m_pTeamData[i].club = 0;
		m_pTeamData[i].turn = 0;
		m_pTeamData[i].teeShooter = 0;
		m_pTeamData[i].holeInPose = 0;
		m_pTeamData[i].holeInCollision = 0;

		for (int j = 0; j < 18; j++)
		{
			m_pTeamData[i].stroke[j] = 0;
			m_pTeamData[i].putt[j] = 0;
			m_pTeamData[i].pang[j] = 0;
		}

		if (OnlinePlay() && HackingManager::IsInstantiated())
		{
			for (int j = 0; j < 18; j++)
			{
				HackingManager::Instance()->AddCrypticByteValue(
					&m_pTeamData[i].stroke[j]);
				HackingManager::Instance()->AddCrypticByteValue(
					&m_pTeamData[i].putt[j]);
				HackingManager::Instance()->AddCrypticIntValue(
					&m_pTeamData[i].pang[j]);
			}

			HackingManager::Instance()->AddCrypticIntValue(
				&m_pTeamData[i].totalPang);
			HackingManager::Instance()->AddCrypticIntValue(
				&m_pTeamData[i].bonusPang);
			HackingManager::Instance()->AddCrypticFloatValue(
				&m_pTeamData[i].gauge);
		}
	}
}

void CGolfDoc::MakeHoleData()
{
	if (m_pHoleData)
	{
		delete[] m_pHoleData;
		m_pHoleData = NULL;
	}
	m_pHoleData = new sHoleData[18];
	memset(m_pHoleData, 0, sizeof(sHoleData) * 18);
}

void CGolfDoc::LoadXml(const char* filename, unsigned char hole, bool bReload)
{
	TiXmlDocument doc;

	cFile* file = g_resrcmng->GetCFile(filename, 0xffff);
	if (file)
	{
		char* buffer = new char[file->Length() + 1];
		file->Read(buffer, file->Length());
		buffer[file->Length()] = 0;
		CloseCFile(file);

		if (doc.LoadFileFromMemoryEx(buffer))
		{
			if (bReload || m_xmlName.empty() || m_xmlName != filename)
				m_propertyInfo.Init(doc);
			m_mapInfo.Init(doc, hole);
			m_xmlName = filename;

			delete[] buffer;
			return;
		}

		delete[] buffer;
	}

	throw WAppException(
		"\xc5\xd8\xbd\xba\xc3\xc4 \xbc\xd3\xbc\xba \xc6\xc4\xc0\xcf\xc0\xbb "
		"\xb7\xce\xb5\xf9 \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9");
}

void CGolfDoc::RefreshPlayerNum()
{
	m_currentPlayerNum = 0;

	for (unsigned char i = 0; i < m_playerNum; ++i)
	{
		if (m_pPlayerData[i].state == 3)
			continue;

		++m_currentPlayerNum;
	}
}

void CGolfDoc::SetCurrentPlayerNum(int num)
{
	m_currentPlayerNum = num;
}

unsigned char CGolfDoc::GetCurrentPlayerNum()
{
	return m_currentPlayerNum;
}

void CGolfDoc::SetPlayerLevel(unsigned char index)
{
	sPlayerData* player = GetPlayer(index);

	unsigned long charTid = Doc()->m_userInfo[index].charInfo.tid;
	const char* charPCL = Doc()->m_userInfo[index].charInfo.PCL;
	unsigned long* parts = Doc()->m_userInfo[index].charInfo.tidParts;
	unsigned long* auxParts = Doc()->m_userInfo[index].charInfo.tidAuxParts;
	unsigned long clubTid = Doc()->m_userInfo[index].clubInfo.tid;
	short* clubPCL = Doc()->m_userInfo[index].clubInfo.PCL;
	unsigned long caddieTid = Doc()->m_userInfo[index].caddieInfo.tid;
	unsigned char upgrade = Doc()->m_userInfo[index].stat.Level;

	if (Doc()->m_offlinePlay)
	{
		player->power = rand() % 20 + 10;
		player->control = rand() % 10 + 15;
		player->accuracy = rand() % 5 + 5;
		player->spin = rand() % 5 + 5;
		player->curve = rand() % 5 + 5;
	}
	else
	{
		if (IsLocalContent(S4_CARD_SYSTEM))
			CCardManager::Instance()->SetPlayerIndex(index);

		player->power = ItemManager()->GetLevel(LVL_POWER, charTid, charPCL,
			parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
		player->control = ItemManager()->GetLevel(LVL_CONTROL, charTid, charPCL,
			parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
		player->accuracy = ItemManager()->GetLevel(LVL_ACCURACY, charTid,
			charPCL, parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
		player->spin = ItemManager()->GetLevel(LVL_SPIN, charTid, charPCL,
			parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
		player->curve = ItemManager()->GetLevel(LVL_CURVE, charTid, charPCL,
			parts, auxParts, clubTid, clubPCL, caddieTid, upgrade);
	}

	player->power = Between<int>(0, player->power, 100) - 15;

	player->control -=
		Max<int>(0, player->power - (CalcPowerPenalty(upgrade) + 5));
	player->accuracy -=
		Max<int>(0, player->power - (CalcPowerPenalty(upgrade) + 5));

	player->accuracy = Between<int>(0, player->accuracy, 30);
	player->control = Between<int>(0, player->control, 30);
	player->spin = Between<int>(1, player->spin, 30);
	player->curve = Between<int>(1, player->curve, 30);
}

CPartTidList& CGolfDoc::GetPartTidList(unsigned char index)
{
	return m_partTidList[index];
}

void CGolfDoc::SetPartTidList(unsigned char index, unsigned long charTid,
	unsigned char hairColor, unsigned char shirtColor, unsigned long* parts,
	unsigned long* auxParts)
{
	sCharacterInfo info;
	info.tid = charTid;
	info.hairClr = hairColor;
	info.shirtsClr = shirtColor;
	memcpy(info.tidParts, parts, sizeof(info.tidParts));
	if (auxParts)
		memcpy(info.ItemIdList, auxParts, sizeof(info.ItemIdList));

	CPartTidList& list = m_partTidList[index];
	if (!list.SetTids(&info, 0xff))
		list.SetDefaultTids();

	if (!list.IsComboValid())
		list.SetDefaultTids();
}

void CGolfDoc::ClearShotResultMsg()
{
	memset(m_shotResult, 0, sizeof(m_shotResult));
}

void CGolfDoc::InitShotResultMsg()
{
	WVector pos =
		GetHoleData().tee + WVector(0.0f, GolfBall().GetRadius(), 0.0f);

	for (int i = 0; i < m_playerNum; ++i)
	{
		m_shotResult[i].pos[0] = pos.x;
		m_shotResult[i].pos[1] = pos.y;
		m_shotResult[i].pos[2] = pos.z;
		m_shotResult[i].bunker = 0;
	}
}

void CGolfDoc::LogStrokeScore()
{
}

void CWaveInfo::Load(const char* filename)
{
	cFile* file = g_resrcmng->GetCFile(filename, 0xffff);
	if (!file)
		return;

	m_wave.clear();

	char buf[1024];
	int index;
	sWave wave;

	int len = file->Length();
	file->Read(buf, len);
	buf[len] = 0;

	char* p = buf;
	char* line = p;

	while (*line)
	{
		while (*p && *p != '\n')
			++p;
		if (*p)
			++p;

		if (line[0] == '/' && line[1] == '/')
		{
			line = p;
			continue;
		}

		if (sscanf(line, "%d %s %f %f", &index, wave.texture, &wave.width,
				&wave.freq) == 4)
		{
			std::map<int, sWave>::iterator it = m_wave.find(index);
			if (it == m_wave.end())
			{
				wave.freq *= 0.001f;
				m_wave[index] = wave;
			}
		}

		line = p;
	}

	CloseCFile(file);
}

void CRiverInfo::Load(const char* filename)
{
	cFile* file = g_resrcmng->GetCFile(filename, 0xffff);
	if (!file)
		return;

	m_river.clear();

	char buf[1024];
	int index;
	sRiver river;

	int len = file->Length();
	file->Read(buf, len);
	buf[len] = 0;

	char* p = buf;
	char* line = p;

	while (*line)
	{
		while (*p && *p != '\n')
			++p;
		if (*p)
			++p;

		if (line[0] == '/' && line[1] == '/')
		{
			line = p;
			continue;
		}

		if (sscanf(line, "%d %f %f %f %f", &index, &river.scrollVel,
				&river.tileHead, &river.tileSide, &river.depth) == 5)
		{
			std::map<int, sRiver>::iterator it = m_river.find(index);
			if (it == m_river.end())
				m_river[index] = river;
		}

		line = p;
	}

	CloseCFile(file);
}

void CMapInfo::Init(const TiXmlDocument& doc, unsigned char hole)
{
	TiXmlNode* node = doc.FirstChild("round")->FirstChild("project");
	if (!node)
		return;

	m_item.clear();
	node = node->FirstChild("wep");
	for (unsigned char i = 0; i < hole; ++i)
		node = node->NextSibling();

	std::string name = node->ToElement()->Attribute("name");
	TiXmlNode* item = node->FirstChild();
	while (item)
	{
		std::string key = item->ToElement()->Value();
		std::string value = item->FirstChild()->Value();
		m_item[key] = value;
		item = item->NextSibling();
	}
}

const char* CMapInfo::FindItem(const std::string& key)
{
	if (m_item.find(key) == m_item.end())
		return NULL;
	return m_item[key].c_str();
}

CPropertyInfo::~CPropertyInfo()
{
}

void CPropertyInfo::Init(const TiXmlDocument& doc)
{
	TiXmlNode* round = doc.FirstChild("round");
	if (!round)
		throw WAppException(std::string("app exception"));

	TiXmlNode* declare = round->FirstChild("declare");
	if (!declare)
		return;

	m_property.clear();
	m_textureIndex.clear();

	struct sField
	{
		const char* name;
		const char* format;
		void* data;
	};

	int count = 0;
	declare->ToElement()->Attribute("count", &count);
	if (count)
		m_property.reserve(count);

	m_defaultIndex = -1;
	int index = 0;

	sProperty prop;
	char ground[128];
	sField fields[] = {
		{ "bound",       "%f", &prop.bound     },
		{ "roll",        "%f", &prop.roll      },
		{ "min",         "%f", &prop.min[0]    },
		{ "max",         "%f", &prop.max[0]    },
		{ "min2",        "%f", &prop.min[1]    },
		{ "max2",        "%f", &prop.max[1]    },
		{ "min3",        "%f", &prop.min[2]    },
		{ "max3",        "%f", &prop.max[2]    },
		{ "pass",        "%d", &prop.pass      },
		{ "pet",         "%s", ground          },
		{ "bound_sound", "%s", prop.boundSound },
		{ "roll_sound",  "%s", prop.rollSound  },
	};

	TiXmlNode* defaultNode = declare->FirstChild("class");
	while (defaultNode)
	{
		++m_defaultIndex;
		std::string name = defaultNode->ToElement()->Attribute("name");
		if (name == "default")
			break;

		defaultNode = defaultNode->NextSibling("class");
	}

	TiXmlNode* classNode = declare->FirstChild("class");
	do
	{
		TiXmlElement* classElement = classNode->ToElement();
		if (stricmp(classElement->Value(), "class"))
			break;

		std::string className = classElement->Attribute("name");
		if (className.empty())
			break;

		memset(ground, 0, sizeof(ground));
		for (int i = 0; i < 12; ++i)
		{
			TiXmlNode* node = classNode->FirstChild(fields[i].name);
			if (!node)
				node = defaultNode->FirstChild(fields[i].name);

			std::string value = node->ToElement()->FirstChild()->Value();
			sscanf(value.c_str(), fields[i].format, fields[i].data);
		}

		if (strstr(ground, "green"))
			prop.ground = 2;
		else if (strstr(ground, "fairway"))
			prop.ground = 1;
		else if (strstr(ground, "bunker"))
			prop.ground = 3;
		else if (strstr(ground, "rough"))
			prop.ground = 4;
		else if (strstr(ground, "snow"))
			prop.ground = 5;
		else if (strstr(ground, "road"))
			prop.ground = 6;
		else if (strstr(ground, "ice"))
			prop.ground = 7;
		else if (strstr(ground, "water"))
			prop.ground = 9;
		else if (strstr(ground, "sand"))
			prop.ground = 11;
		else if (strstr(ground, "booster"))
			prop.ground = 13;
		else if (strstr(ground, "special"))
			prop.ground = 12;
		else if (strstr(ground, "wb"))
			prop.ground = 14;
		else if (strstr(ground, "vector"))
			prop.ground = 8;

		char* ext;
		if ((ext = strstr(prop.boundSound, ".wav")) != NULL)
			*ext = 0;
		if ((ext = strstr(prop.rollSound, ".wav")) != NULL)
			*ext = 0;

		int type = prop.ground;
		if (prop.ground == 0 || prop.ground == 4)
		{
			if (GetCurMap() == 10)
				type = 11;
			else
				type = IsCurMapSnow() ? 5 : 2;
		}

		switch (type)
		{
		case 5:
			strcpy(prop.stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_snow");
			break;
		case 7:
			strcpy(prop.stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_ice");
			break;
		case 3:
		case 11:
			strcpy(prop.stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_sand");
			break;
		case 9:
			strcpy(prop.stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_water");
			break;
		default:
			strcpy(prop.stepSound, "\xb9\xdf\xc0\xda\xb1\xb9_green");
			break;
		}

		m_property.push_back(prop);

		TiXmlNode* textures = classNode->FirstChild("textures");
		if (textures)
		{
			TiXmlNode* item = textures->FirstChild("item");
			while (item)
			{
				std::string texture = item->ToElement()->FirstChild()->Value();
				item = item->NextSibling();
				m_textureIndex[texture] = index;
			}
		}

		++index;

		classNode = classNode->NextSibling("class");
	} while (classNode);
}

int CPropertyInfo::GetPropertyIndex(const char* texture)
{
	std::map<std::string, int>::iterator it = m_textureIndex.find(texture);
	if (it == m_textureIndex.end())
		return m_defaultIndex;

	return m_textureIndex[texture];
}

const sProperty& CPropertyInfo::GetProperty(int index)
{
	if (index < 0 || index >= m_property.size())
		index = m_defaultIndex;

	return m_property[index];
}

void sBar::Reset()
{
	center = s_barStart;
	power = s_barStart;
	impact = s_barStart;
	gauge = s_barGaugeStart;
	gaugeStep = s_barGaugeStart;
	bFullPower = false;
	dir = 0;
	calipers = s_barStart;
}

void CGolfDoc::SetApproachMissionDesc(const char* desc)
{
	strcpy(m_approachMissionDesc, desc);
}

char* CGolfDoc::GetApproachMissionDesc()
{
	return m_approachMissionDesc;
}

void CGolfDoc::SetApproachMissionType(eApproachMissionType type)
{
	m_approachMissionType = type;
}

void CGolfDoc::SetApproachPrizeType(eApproachPrizeType type)
{
	m_approachPrizeType = type;
}

void CGolfDoc::SetApproachWinnerType(eApproachWinnerType type)
{
	m_approachWinnerType = type;
}

eApproachMissionType CGolfDoc::GetApproachMissionType() const
{
	return m_approachMissionType;
}

eApproachPrizeType CGolfDoc::GetApproachPrizeType() const
{
	return m_approachPrizeType;
}

eApproachWinnerType CGolfDoc::GetApproachWinnerType() const
{
	return m_approachWinnerType;
}

void CGolfDoc::SetApproachCurHoleResult(sApproachResultData& result)
{
	sApproachResultData* pResult = new sApproachResultData;

	if (pResult)
	{
		*pResult = result;
		m_approachCurHoleResult.push_back(pResult);
	}
}

void CGolfDoc::SetApproachGameResult(sApproachResultData& result)
{
	sApproachResultData* pResult = new sApproachResultData;
	memset(pResult, 0, sizeof(sApproachResultData));

	if (pResult)
	{
		*pResult = result;

		std::list<sApproachResultData*>::iterator it;
		for (it = m_approachGameResult.begin();
			it != m_approachGameResult.end(); ++it)
		{
			if ((*it)->dwGUID == result.dwGUID)
				break;
		}

		if (it == m_approachGameResult.end())
		{
			m_approachGameResult.push_back(pResult);
		}
		else
		{
			delete pResult;

			sApproachResultData* pData = *it;
			pData->byRank = result.byRank;
			pData->dwGUID = result.dwGUID;
			pData->byRankPrize += result.byRankPrize;
			pData->byLuckPrize += result.byLuckPrize;
			pData->dwPrizeCount += result.dwPrizeCount;
			pData->dwRemainDistance += result.dwRemainDistance;
			pData->dwRemainTime += result.dwRemainTime;
		}
	}
}

void CGolfDoc::ClearApproachCurHoleResult()
{
	std::list<sApproachResultData*>::iterator it;
	for (it = m_approachCurHoleResult.begin();
		it != m_approachCurHoleResult.end(); ++it)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_approachCurHoleResult.clear();
}

void CGolfDoc::ClearApproachGameResult()
{
	std::list<sApproachResultData*>::iterator it;
	for (it = m_approachGameResult.begin(); it != m_approachGameResult.end();
		++it)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_approachGameResult.clear();
}

std::list<sApproachResultData*>& CGolfDoc::GetApproachCurHoleResult()
{
	return m_approachCurHoleResult;
}

sApproachResultData* CGolfDoc::GetAppMyCurHoleResult()
{
	sApproachResultData* pResult = NULL;

	std::list<sApproachResultData*>::iterator it;
	for (it = m_approachCurHoleResult.begin();
		it != m_approachCurHoleResult.end(); ++it)
	{
		sApproachResultData* pData = *it;
		if (pData)
		{
			if (pData->dwGUID == Doc()->m_myInfo.info.dwGuid)
			{
				pResult = pData;
				break;
			}
			else
			{
				pResult = NULL;
			}
		}
	}

	return pResult;
}

std::list<sApproachResultData*>& CGolfDoc::GetApproachGameResult()
{
	return m_approachGameResult;
}

sApproachResultData* CGolfDoc::GetAppMyGameResult()
{
	sApproachResultData* pResult = NULL;

	std::list<sApproachResultData*>::iterator it;
	for (it = m_approachGameResult.begin(); it != m_approachGameResult.end();
		++it)
	{
		sApproachResultData* pData = *it;
		if (pData)
		{
			if (pData->dwGUID == Doc()->m_myInfo.info.dwGuid)
			{
				pResult = pData;
				break;
			}
			else
			{
				pResult = NULL;
			}
		}
	}

	return pResult;
}

void CGolfDoc::AddApproachTotalPrize(unsigned long prize)
{
	m_approachTotalPrize += prize;
}

unsigned long CGolfDoc::GetApproachTotalPrize() const
{
	return m_approachTotalPrize;
}

void CGolfDoc::ApproachDataAllClear()
{
	m_bApproachReloaded = false;
	m_approachMissionType = (eApproachMissionType)0;
	m_approachPrizeType = (eApproachPrizeType)0;
	m_approachWinnerType = (eApproachWinnerType)0;

	m_approachCurHoleResult.clear();
	m_approachGameResult.clear();

	m_approachTotalPrize = 0;
}
