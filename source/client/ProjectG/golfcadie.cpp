#include "stdafx.h"

#include "networksystem.h"
#include "golfcadie.h"

#include "effect.h"
#include "petbody.h"
#include "shadow.h"
#include "wscene.h"
#include "../../shared/globalgamedefine.h"
#include "shareddoc.h"
#include "golfdoc.h"
#include "golfball.h"
#include "chatmsg.h"
#include "wpuppet.h"
#include "wbone.h"
#include "fxinterpreter.h"
#include "fx.h"
#include "randomhouse.h"
#include "commonutil.h"
#include "slicestr.h"
#include "quadtree.h"
#include "tinyxml.h"
#include "wresrcmng.h"
#include "westpak.h"
#include "camerawork.h"
#include "wind.h"
#include "club.h"
#include "soundmanager.h"
#include "polysoup.h"
#include "scenemanager.h"
#include "inputmanager.h"

#include <algorithm>

extern unsigned long g_CurrentTime;
extern LightSet g_lightset;
extern WMatrix g_camera;

int WisZero(const float& value, float eps);

// HACK!
static const float constants[3] = { 3.1415927410125732f, 0.01745329238474369f,
	3.4028234663852886e+38f };

sTalk talkTable[] = {
	{ "",			   false },
    { "click1",         false },
    { "click2",         false },
	{ "turnleft",       false },
    { "turnright",      false },
    { "great1",         false },
	{ "great2",         false },
    { "club1",          false },
    { "club2",          false },
	{ "shottime",       true  },
    { "danger",         false },
    { "ob",             false },
	{ "wind",           true  },
    { "putting",        false },
    { "bunker",         true  },
	{ "transparent",    true  },
    { "rival",          true  },
    { "powershot",      true  },
	{ "ob_caution",     true  },
    { "leader",         true  },
    { "approach",       true  },
	{ "bestscore",      true  },
    { "slopeup",        true  },
    { "turbulence",     true  },
	{ "noisy",          true  },
    { "trouble",        true  },
    { "noitem",         false },
	{ "reitem",         true  },
    { "bestdist",       true  },
    { "ob_sequence",    true  },
	{ "ob_water",       true  },
    { "dormie_if_win",  true  },
    { "dormie_if_draw", true  },
	{ "dormie_shot",    true  },
    { "eagle",          true  },
    { "itemdrop",       true  }
};

IMPLEMENT_ACTOR(CGolfCaddie, IActor)

bool XmlCaddieTalk::LoadSituation(const char* situation,
	const parameters& params, std::vector<attributes>& result)
{
	TiXmlDocument doc;
	if (!doc.LoadFileEx("caddie_talk.xml"))
		return false;

	TiXmlNode* node = doc.FirstChild("situation");
	while (node)
	{
		TiXmlElement* element = node->ToElement();
		if (element->Attribute("name") == situation)
			break;

		node = node->NextSibling();
	}

	if (!node)
		return false;

	TiXmlNode* param = node->FirstChild("param");
	while (param)
	{
		TiXmlElement* element = param->ToElement();
		std::vector<std::string>::const_iterator it = params.m_values.begin();
		attributes attr;
		while (it != params.m_values.end())
		{
			std::string value = element->Attribute((*it).c_str());

			attr.m_values[*it] = value;

			++it;
		}
		if (!attr.m_values.empty())
			result.push_back(attr);

		param = param->NextSibling();
	}

	return true;
}

void cCaddieTalkTeeShot::Load(void)
{
	parameters params;
	params.add("course");
	params.add("hole");
	params.add("caddie");
	params.add("talk");

	std::vector<attributes> result;
	if (!XmlCaddieTalk::LoadSituation("tee_shot", params, result))
		return;

	std::vector<attributes>::iterator it = result.begin();
	while (it != result.end())
	{
		sTalkData data;

		data.m_course = atoi((*it).get("course"));
		data.m_hole = atoi((*it).get("hole"));
		data.m_caddie = atoi((*it).get("caddie"));
		data.m_msg = (*it).get("talk");

		m_talkData.push_back(data);
		++it;
	}
}

bool cCaddieTalkTeeShot::FindMsg(void)
{
	if (GolfBall().m_groundType)
	{
		return false;
	}

	std::map<unsigned int, sCaddieInfo>::iterator itCaddie =
		CSharedDoc::Instance()->m_caddieMap.find(
			CSharedDoc::Instance()->m_myInfo.userEquip.guidCaddie);
	if (itCaddie == CSharedDoc::Instance()->m_caddieMap.end())
	{
		return false;
	}

	sTalkData key;
	key.m_caddie = itCaddie->second.tid & 0x3FFFFFF;
	key.m_hole = GOLFDOC()->m_currentHole;
	key.m_course = GetCurMap();

	for (std::vector<sTalkData>::iterator it = m_talkData.begin();
		it != m_talkData.end(); ++it)
	{
		if (*it == key)
		{
			m_talkMsg = (*it).m_msg;

			m_talkData.erase(it);
			return true;
		}
	}

	return false;
}

CCaddieTalk::~CCaddieTalk(void)
{
	std::for_each(m_talks.begin(), m_talks.end(), Delete_Object());
}

void CCaddieTalk::Release(void)
{
	std::for_each(m_talks.begin(), m_talks.end(), Delete_Object());
}

void CCaddieTalk::Init(void)
{
	m_isTalking = 0;

	ICaddieTalk* talk = new cCaddieTalkTeeShot;
	m_talks.push_back(talk);

	std::list<ICaddieTalk*>::iterator it = m_talks.begin();
	while (it != m_talks.end())
	{
		(*it)->Load();
		++it;
	}
}

const ICaddieTalk* CCaddieTalk::FindTalk(void)
{
	if (m_isTalking)
		return 0;

	std::list<ICaddieTalk*>::iterator it = m_talks.begin();
	while (it != m_talks.end())
	{
		if ((*it)->FindMsg())
		{
			m_isTalking = 1;
			return *it;
		}
		++it;
	}

	return 0;
}

void CGolfCaddie::OnDestroy(void)
{
	GetFxInterpreter()->DetachFxAll();

	m_fxScript = -1;
	for (std::list<sCaddie*>::iterator it = m_caddies.begin();
		it != m_caddies.end(); it++)
	{
		if ((*it)->m_pBody)
		{
			delete (*it)->m_pBody;
			(*it)->m_pBody = 0;
		}
		if (*it)
		{
			delete *it;
			*it = 0;
		}
	}

	m_caddies.clear();

	if (m_pRing)
	{
		delete m_pRing;
		m_pRing = 0;
	}
	if (m_pCaddieTalk)
	{
		delete m_pCaddieTalk;
		m_pCaddieTalk = 0;
	}
}

void CGolfCaddie::OnPreLoadInit(void)
{
	CSceneManager::Instance()->RegisterElement((eObjType)4, 0, this,
		"\xc4\xb3\xb5\xf0", 0x18, 0);
}

void CGolfCaddie::OnLoad(void)
{
	OnDestroy();

	if (GOLFDOC()->m_tutorialMode < 15)
	{
		CSharedDoc::Instance()->m_userInfo[0].caddieInfo.guid = 0xFFFFFFFF;
		CSharedDoc::Instance()->m_userInfo[0].caddieInfo.tid = 0x1C000001;
	}

	for (unsigned char i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
			continue;

		unsigned long typeId =
			CSharedDoc::Instance()->m_userInfo[i].caddieInfo.tid;
		if (!typeId)
			continue;

		if (GOLFDOC()->m_tutorialMode < 15)
		{
			AddCaddie(typeId, 0, true);
		}
		else
		{
			AddCaddie(typeId,
				CSharedDoc::Instance()->m_userInfo[i].caddieInfo.tidPart,
				false);
		}
	}

	if (!m_pCaddieTalk)
	{
		m_pCaddieTalk = new CCaddieTalk;
		m_pCaddieTalk->Init();
	}
}

void CGolfCaddie::OnReload(void)
{
	OnLoad();
}

CGolfCaddie::sCaddie* CGolfCaddie::AddCaddie(unsigned long typeId,
	unsigned long cadItemTypeId, bool find)
{
	sCaddie* caddie;
	std::list<sCaddie*>::iterator it;

	if (find)
	{
		sCaddie* found =
			(it = std::find_if(m_caddies.begin(), m_caddies.end(),
				 sFindCaddie(typeId, cadItemTypeId))) != m_caddies.end()
			? *it
			: 0;

		if ((caddie = found) != 0)
			return found;
	}

	IFF_STRUCT::sCaddie* pIffCaddie = ItemManager()->FindCaddie(typeId);

	if (pIffCaddie)
	{
		caddie = new sCaddie;

		caddie->m_showRing = false;
		caddie->m_caddieView = true;
		caddie->m_pIffCaddie = pIffCaddie;

		if (cadItemTypeId)
		{
			caddie->m_pBody = new CPetBody(
				MakeStr("*pet %s_ex.pet *shadow 1", pIffCaddie->Data), false);

			IFF_STRUCT::sCadItem* pCadItem =
				ItemManager()->FindCadItem(cadItemTypeId);
			if (pCadItem)
			{
				if (strlen(pCadItem->FaceTex))
					caddie->m_pBody->SetPendingTexPart("face",
						pCadItem->FaceTex);
				if (strlen(pCadItem->BodyTex))
					caddie->m_pBody->SetPendingTexPart("body",
						pCadItem->BodyTex);
			}
		}
		else
		{
			caddie->m_pBody = new CPetBody(
				MakeStr("*pet %s.pet *shadow 1", pIffCaddie->Data), false);
		}

		caddie->m_pBody->SetPos(WVector::ZERO);
		caddie->m_pBody->SetCustomGround("center");
		caddie->m_pBody->SetLight();

		LoadTalkText(caddie);
		LoadMotionLevel(caddie);
		ReadAttribute(caddie);

		m_caddies.push_back(caddie);
	}

	return caddie;
}

CGolfCaddie::sCaddie* CGolfCaddie::FindCaddie(unsigned long typeId,
	unsigned long cadItemTypeId)
{
	std::list<sCaddie*>::iterator it;
	it = std::find_if(m_caddies.begin(), m_caddies.end(),
		sFindCaddie(typeId, cadItemTypeId));

	if (it != m_caddies.end())
		return *it;

	return 0;
}

void CGolfCaddie::OnInit(void)
{
	m_pCaddie = FindCaddie(CSharedDoc::Instance()
							   ->m_userInfo[GOLFDOC()->m_currentPlayer]
							   .caddieInfo.tid,
		CSharedDoc::Instance()
			->m_userInfo[GOLFDOC()->m_currentPlayer]
			.caddieInfo.tidPart);

	m_unknown30 = 0;
	m_runDelay = 0;
	m_angle = 0;
	m_talkTime = 0;
	m_dodging = false;
	m_visible = true;
	m_unknown64 = 0;
	m_unknown60 = 0;
}

void CGolfCaddie::LoadTalkText(sCaddie* caddie)
{
	caddie->m_talks.clear();

	char fileName[32];
	fileName[0] = 0;
	sprintf(fileName, "%s.talk", caddie->m_pIffCaddie->Data);

	cFile* file = g_resrcmng->GetCFile(fileName, -1);

	if (!file)
		return;

	int length = file->Length();

	char* buffer;
	buffer = new char[length + 1];

	file->Read(buffer, length);
	buffer[length] = 0;
	CloseCFile(file);

	char* p = buffer;
	while (p && *p && strstr(p, ":"))
	{
		char name[256] = "";
		char level[256] = "";
		char motion[256] = "";

		char* next = GetBehaviorToken(name, p);
		if (!next || !*next || !name[0])
			break;

		next = GetBehaviorToken(level, next);
		if (!next || !*next || !level[0])
			break;

		p = GetBehaviorEndToken(motion, next);

		sBehavior behavior;
		behavior.m_level = atoi(level);
		behavior.m_motion = motion;

		caddie->m_talks[name] = behavior;
	}

	delete[] buffer;
}

void CGolfCaddie::LoadMotionLevel(sCaddie* caddie)
{
	caddie->m_motions.clear();

	char fileName[32];
	fileName[0] = 0;
	sprintf(fileName, "%s.mot", caddie->m_pIffCaddie->Data);

	cFile* file = g_resrcmng->GetCFile(fileName, -1);

	if (!file)
		return;

	int length = file->Length();

	char* buffer;
	buffer = new char[length + 1];

	file->Read(buffer, length);
	buffer[length] = 0;
	CloseCFile(file);

	char* p = buffer;
	while (p && *p && strstr(p, ":"))
	{
		char level[256] = "";
		char name[256] = "";

		char* end = GetBehaviorToken(level, p);
		if (!end || !*end || !level[0])
			break;

		p = GetBehaviorEndToken(name, end);

		sBehavior& behavior = caddie->m_motions[name];
		behavior.m_level = atoi(level);
	}

	delete[] buffer;
}

bool CGolfCaddie::IsGoodMotion(sCaddie* caddie, const char* motion)
{
	std::map<std::string, sBehavior>::iterator it =
		caddie->m_motions.find(motion);
	if (it == caddie->m_motions.end())
		return true;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (m_pCaddie->m_pInfo)
		{
			if (m_pCaddie->m_pInfo->Level < (*it).second.m_level)
				return false;
		}
		else
		{
			if ((*it).second.m_level > 0)
				return false;
		}
	}

	return true;
}

bool CGolfCaddie::ReadAttribute(sCaddie* caddie)
{
	WPuppet* pet = caddie->m_pBody->GetPet();
	if (pet)
	{
		w_motion_data* motion = pet->FindMotionData("\xb4\xde\xb8\xae\xb1\xe2");
		if (!motion)
			return false;

		int frame = (int)(motion->s * 30.0f + 0.5f);

		int index = -1;
		pet->CheckFrameData(frame, frame, index, frame);
		if (index > -1)
		{
			WSliceStr slice(pet->GetFrameData(index));

			char ringTexture[16];
			ringTexture[0] = '\0';
			caddie->m_showRing = slice.Get("*ring", "%s", ringTexture);
			if (caddie->m_showRing)
			{
				if (!m_pRing)
					m_pRing = new CRing(ringTexture);

				m_pRing->SetCenter(m_position + WVector(0.0f, 0.3f, 0.0f));
				m_pRing->SetActive(true);
			}

			slice.Get("*stop_dist", "%f", &caddie->m_stopDist);
			caddie->m_stopDist = (float)(caddie->m_stopDist * 3.2f);

			slice.Get("*start_delay", "%f", &caddie->m_startDelay);

			slice.Get("*pet_speed", "%f", &caddie->m_petSpeed);

			bool noCaddieView = slice.Get("*no_caddie_view", "");

			caddie->m_petSpeed *= 3.2f;
			caddie->m_caddieView = !noCaddieView;
			return true;
		}
	}

	return false;
}

char* CGolfCaddie::GetBehaviorToken(char* token, char* src)
{
	// HACK!
	if (false)
		GetBehaviorEndToken(0, 0);

	while (*src == ' ' || *src == '\n')
		src++;

	while (*src)
	{
		if (*src == ' ')
		{
			src++;
			continue;
		}

		if (*src & 0x80)
			*token++ = *src++;

		*token++ = *src++;
		if (*src == ':')
		{
			*token = '\0';
			return src + 1;
		}
	}

	return 0;
}

char* CGolfCaddie::GetBehaviorEndToken(char* token, char* src)
{
	while (*src == ' ')
		src++;

	*token++ = *src++;
	while (*src != '\r' && *src != '\0')
		*token++ = *src++;
	*token = '\0';
	if (*src != '\0')
		src++;

	return src;
}

void CGolfCaddie::OnProcess(float delta)
{
	if (!m_pCaddie || !m_visible)
		return;

	if ((GOLFDOC()->GameMode() & 0x80) && m_runDelay > 0.0f)
	{
		m_runDelay -= delta;

		if (WisZero(m_runDelay, g_EPSILON))
			m_runDelay = -g_EPSILON;
	}

	if (m_runDelay < 0.0f)
	{
		m_angle = CalcDeltaAngle(m_pCaddie->m_pBody->GetDirection(),
			WVector::UNIT_NEG_Z);
		m_angle += CalcDeltaAngle(m_destination - m_position,
			m_pCaddie->m_pBody->GetDirection());

		m_pCaddie->m_pBody->SetRotate(m_angle);

		m_runDelay = 0.0f;
		if (m_unknown30 != 4)
		{
			m_unknown30 = 4;
			if (GOLFDOC()->m_tutorialMode == 15)
				m_pCaddie->m_pBody->SetMotion("\xb4\xde\xb8\xae\xb1\xe2", true,
					0.0f, false);
			else
				m_pCaddie->m_pBody->SetMotion(
					"\xc6\xa9\xc5\xcd\xb8\xae\xbe\xf3\xb4\xde\xb8\xae\xb1\xe2",
					true, 0.0f, false);

			m_runDust = CFx::Instance()->MakeRunDust(m_position,
				m_pCaddie->m_pBody->GetVel());

			if (m_pRing && m_pCaddie->m_showRing)
				m_pRing->SetActive(false);
		}

		m_pCaddie->m_pBody->TakeOff(m_destination);
	}

	if (m_unknown30 == 4)
	{
		Run();
	}
	else if (GolfBall().m_playFrame < 0)
	{
		switch (CSharedDoc::Instance()->m_golfGame.shotTimeLimit)
		{
		case 0:
			m_idleTime -= delta;
			if (m_idleTime < 0.0f)
			{
				m_idleTime = 20.0f;
				if (IsGoodMotion(m_pCaddie, "\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2"))
				{
					m_unknown30 = 1;
					m_pCaddie->m_pBody->SetMotion(
						"\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2", false, 0.0f, false);
				}
			}

			if (m_unknown30 == 1 &&
				strcmp(m_pCaddie->m_pBody->GetMotionName(),
					"\xb1\xe2\xba\xbb") == 0)
				m_unknown30 = 0;
			break;
		case 40000:
			if (GOLFDOC()->m_remainTime < 10000 && m_unknown30 == 1)
			{
				if (IsGoodMotion(m_pCaddie, "\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2"))
				{
					m_unknown30 = 3;
					m_pCaddie->m_pBody->SetMotion(
						"\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2", false, 0.0f, false);
				}
			}
			else if (GOLFDOC()->m_remainTime < 20000 && m_unknown30 == 0)
			{
				if (IsGoodMotion(m_pCaddie, "\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2"))
				{
					m_unknown30 = 1;
					m_pCaddie->m_pBody->SetMotion(
						"\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2", false, 0.0f, false);
				}
			}
			else if (GOLFDOC()->m_remainTime > 20000)
			{
				m_unknown30 = 0;
			}
			break;
		case 60000:
			if (GOLFDOC()->m_remainTime < 10000 && m_unknown30 == 2)
			{
				if (IsGoodMotion(m_pCaddie, "\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2"))
				{
					m_unknown30 = 3;
					m_pCaddie->m_pBody->SetMotion(
						"\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2", false, 0.0f, false);
				}
			}
			else if (GOLFDOC()->m_remainTime < 20000 && m_unknown30 == 0)
			{
				if (IsGoodMotion(m_pCaddie, "\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2"))
				{
					m_unknown30 = 2;
					m_pCaddie->m_pBody->SetMotion(
						"\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2", false, 0.0f, false);
				}
			}
			else if (GOLFDOC()->m_remainTime < 40000 && m_unknown30 == 0)
			{
				if (IsGoodMotion(m_pCaddie, "\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2"))
				{
					m_unknown30 = 1;
					m_pCaddie->m_pBody->SetMotion(
						"\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2", false, 0.0f, false);
				}
			}
			else if (GOLFDOC()->m_remainTime > 40000)
			{
				m_unknown30 = 0;
			}
			break;
		}
	}

	m_pCaddie->m_pBody->Process(delta);

	if (m_pRing && m_pCaddie->m_showRing)
		m_pRing->Process(delta);

	if ((GOLFDOC()->GameMode() & 0xFFFFFFFC) &&
		GOLFDOC()->m_tutorialMode >= 2 && !GOLFDOC()->m_shotPhase &&
		GolfBall().m_playFrame < 0)
		TalkProcess(delta);
}

void CGolfCaddie::TalkProcess(float delta)
{
	bool timeOver = g_CurrentTime - m_talkResetTime > 1500;
	WVector dir = RotMat(m_talkTime, 1).za;

	if (GOLFDOC()->m_tutorialMode <= 6)
	{
		if (Abs(m_unknown60) > 1)
		{
			if (m_unknown60 > 0)
				SetBubble(1, false, 0);
			else
				SetBubble(2, false, 0);
		}

		WPolySoup::sPoint* point = GOLFDOC()->m_pPolySoup->FindPoint(
			MakeStr("\xb3\xab\xc7\xcf\xc1\xf6\xc1\xa1%d",
				GOLFDOC()->m_tutorialMode + 1));

		if (!point)
			return;

		float angle = CalcDeltaAngle(GOLFDOC()->GetCameraData().mat.za,
			point->pos - GolfBall().m_pos);
		float dist = (point->pos - GolfBall().m_pos).Magnitude();
		float range = GolfClub().GetRange(0);
		dist -= range * 3.2f;

		if (timeOver && !m_talkFlag)
		{
			if (angle < -0.5235988f)
			{
				m_talkFlag = true;
				SetBubble(3, false, 0);
			}
			else if (angle > 0.5235988f)
			{
				m_talkFlag = true;
				SetBubble(4, false, 0);
			}

			if (Abs(angle) < 0.17453292f)
			{
				if (m_talked[3] || m_talked[4])
				{
					if (Abs(dist) >
						(GOLFDOC()->m_tutorialMode == 2 ? 89.6f : 44.8f))
						SetBubble(5, false, 0);
					else
						SetBubble(6, false, 0);

					m_talked[3] = false;
					m_talked[4] = false;
				}
				else if (GolfClub().Updated() && Abs(dist) < 48.0f)
				{
					SetBubble(6, false, 0);
				}

				if (Abs(dist) >
					(GOLFDOC()->m_tutorialMode == 2 ? 89.6f : 44.8f))
				{
					if (dist > 0.0f)
						SetBubble(7, false, 0);
					else
						SetBubble(8, false, 0);
				}
			}
		}
		return;
	}

	if (!m_bubbleTalkType && m_pCaddie)
	{
		const ICaddieTalk* talk = m_pCaddieTalk->FindTalk();
		if (talk)
		{
			WBone* bone =
				m_pCaddie->m_pBody->GetPet()->FindBone("Bip01 HeadNub");

			if (bone)
			{
				WVector pos = bone->GetMatrix().pivot;
				const char* msg = talk->GetTalkMsg();
				if (msg)
					SendMsg(this, "Bubble", 0x13, (intptr_t)msg, (intptr_t)&pos,
						0, 0);
			}
		}
	}

	if (CSharedDoc::Instance()->m_golfGame.shotTimeLimit &&
		GOLFDOC()->m_remainTime < 10000)
		SetBubble(9, false, 0);

	if (timeOver && !m_talkFlag)
	{
		if (GolfClub().GetType() != 3 &&
			Abs(CalcDeltaAngle(dir, m_position - GolfBall().m_pos)) <
				0.08726646f)
		{
			m_talkFlag = true;
			SetBubble(10, false, 0);
		}

		int mesh = GetPVS().GetMeshIndex(
			GolfBall().m_pos + dir * (GolfClub().GetRange(0) * 3.2f), true);

		if (GolfClub().GetType() != 3 && mesh < 0)
		{
			m_talkFlag = true;
			SetBubble(11, false, 0);
		}

		if (m_unknown64)
		{
			m_talkFlag = true;
			SetBubble(0x19, false, 0);
		}
	}

	if (GolfBall().m_groundType == 2)
	{
		if (timeOver && !m_talkFlag &&
			Abs(CalcDeltaAngle(dir, GetHoleCupPos() - GolfBall().m_pos)) >
				1.0471976f)
		{
			m_talkFlag = true;
			SetBubble(13, false, 0);
		}

		if (CSharedDoc::Instance()->m_golfGame.shotTimeLimit &&
			GOLFDOC()->m_remainTime < 30000 &&
			m_pCaddie->m_pInfo->tid == 0x1C000000)
			SetBubble(15, false, 0);

		WVector cup = GOLFDOC()->GetHoleData().pin;
		if (GolfBall().m_pos.y - GolfBall().GetRadius() + 1.6f < cup.y &&
			!PLAYER(GOLFDOC()->m_currentPlayer)
				->putt[GOLFDOC()->m_currentHole - 1] &&
			CSharedDoc::Instance()->m_golfGame.shotTimeLimit &&
			GOLFDOC()->m_remainTime <
				CSharedDoc::Instance()->m_golfGame.shotTimeLimit - 5000)
			SetBubble(0x16, false, 0);
	}
	else
	{
		if (CSharedDoc::Instance()->m_golfGame.shotTimeLimit &&
			GOLFDOC()->m_remainTime < 35000 &&
			Wind().GetGlobalIntensity() > 4.0f)
			SetBubble(12, false, 0);

		if (GolfBall().m_groundType == 3)
			SetBubble(14, false, 0);
	}

	if (!CSharedDoc::Instance()->m_golfGame.gameType &&
		GOLFDOC()->m_currentHole > 5 &&
		GOLFDOC()
				->GetHoleData(GOLFDOC()->m_currentHole - 1)
				.rank[GOLFDOC()->m_currentPlayer] == 1)
	{
		int i;
		for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (GOLFDOC()->GetPlayer(i)->state != 3 &&
				i != GOLFDOC()->m_currentPlayer)
			{
				if (GOLFDOC()
						->GetHoleData(GOLFDOC()->m_currentHole - 1)
						.rank[i] == 1)
					break;
			}
		}

		int hole;
		for (hole = GOLFDOC()->m_currentHole - 1;
			hole > GOLFDOC()->m_currentHole - 6; hole--)
		{
			if (GOLFDOC()->GetHoleData(hole).rank[GOLFDOC()->m_currentPlayer] !=
					1 ||
				GOLFDOC()->GetHoleData(hole).rank[i] != 1)
				break;
		}

		if (hole == GOLFDOC()->m_currentHole - 6)
			SetBubble(0x10, false, 0);
	}

	if (GOLFDOC()->GetHoleData().par == 5 &&
		PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)1 &&
		PLAYER(GOLFDOC()->m_currentPlayer)->gauge >= 33.0f)
	{
		WVector cup = GOLFDOC()->GetHoleData().pin;
		WVector toCup = cup - GolfBall().m_pos;
		toCup.y = 0.0f;

		if (toCup * Wind().GetGlobalWind() > 0.0f &&
			GetPVS().GetMeshIndex(GolfBall().m_pos +
					toCup * ((GolfClub().GetRange("1W") + 10.0f) * 3.2f),
				true) > 0 &&
			2.0f * (GolfClub().GetRange("1W") * 3.2f) +
					PLAYER(GOLFDOC()->m_currentPlayer)->gauge * 0.969697f >
				toCup.Magnitude())
			SetBubble(0x11, false, 0);
	}

	if (CSharedDoc::Instance()->m_golfGame.shotTimeLimit &&
		GOLFDOC()->m_remainTime <
			CSharedDoc::Instance()->m_golfGame.shotTimeLimit - 5000 &&
		GolfBall().m_groundType != 2 && GolfClub().GetApproachMode() < 4 &&
		PLAYER(GOLFDOC()->m_currentPlayer)
				->stroke[GOLFDOC()->m_currentHole - 1] <=
			GOLFDOC()->GetHoleData().par)
		SetBubble(0x14, false, 0);

	if (GolfClub().GetType() != 3 && Wind().IsInTurbulence(g_camera.pivot))
	{
		m_turbulenceTime += delta;

		if (m_turbulenceTime > 2.0f)
		{
			float wind = Wind().GetWind(g_camera.pivot).Magnitude();

			if (wind > 11.0f)
				SetBubble(0x17, false, 5);
			else if (wind > 9.0f)
				SetBubble(0x17, false, 4);
			else if (wind > 7.0f)
				SetBubble(0x17, false, 3);
			else if (wind > 5.0f)
				SetBubble(0x17, false, 2);
			else if (wind > 3.0f)
				SetBubble(0x17, false, 1);
		}
	}
	else
	{
		m_turbulenceTime = 0.0f;
	}

	if (IsMyTurn(false) && g_input->GetDown("ITEMSLOT", true) &&
		!(CSharedDoc::Instance()->m_curChannel.Type & 8))
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->itemNum == 0)
			SetBubble(0x1A, false, 0);
		else if (PLAYER(GOLFDOC()->m_currentPlayer)->item)
			SetBubble(0x1B, false, 0);
	}

	WVector shot = PLAYER(GOLFDOC()->m_currentPlayer)->pos -
		PLAYER(GOLFDOC()->m_currentPlayer)->startPos;

	if (CSharedDoc::Instance()
				->m_holeStatistics[GOLFDOC()->m_currentPlayer]
				.fLongest > 960.0f &&
		Wabs(sqrtf(shot.x * shot.x + shot.z * shot.z) -
			CSharedDoc::Instance()
				->m_holeStatistics[GOLFDOC()->m_currentPlayer]
				.fLongest) < g_EPSILON)
		SetBubble(0x1C, false, 0);

	if (CSharedDoc::Instance()->m_golfGame.gameType == 1)
	{
		if (GetHoleIndex(GOLFDOC()->m_currentHole) < GetHoles())
		{
			if (TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->score >=
				TEAM(1 - PLAYER(GOLFDOC()->m_currentPlayer)->team)->score +
					GetHoles() - GetHoleIndex(GOLFDOC()->m_currentHole))
			{
				SetBubble(
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->score !=
							TEAM(1 - PLAYER(GOLFDOC()->m_currentPlayer)->team)
									->score +
								GetHoles() -
								GetHoleIndex(GOLFDOC()->m_currentHole)
						? 0x20
						: 0x1F,
					false, 0);

				if (GolfBall().m_groundType == 2 &&
					TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
							->stroke[GOLFDOC()->m_currentHole - 1] <
						TEAM(1 - PLAYER(GOLFDOC()->m_currentPlayer)->team)
							->stroke[GOLFDOC()->m_currentHole - 1])
					SetBubble(0x21, false, 0);
			}
		}

		if (TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
				->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
		{
			for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
			{
				if (PLAYER(i)->team !=
						PLAYER(GOLFDOC()->m_currentPlayer)->team &&
					GOLFDOC()->GetPlayer(i)->flag == 1)
				{
					SetBubble(0x12, false, 0);
					return;
				}
			}
		}
	}
	else
	{
		if (CSharedDoc::Instance()->m_golfGame.gameType == 0 ||
			CSharedDoc::Instance()->m_golfGame.gameType == 3)
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)
					->stroke[GOLFDOC()->m_currentHole - 1] == (unsigned char)1)
			{
				for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
				{
					if (GOLFDOC()->GetPlayer(i)->flag == 1 && i != MyIndex())
					{
						SetBubble(0x12, false, 0);
						break;
					}
				}
			}

			if (PLAYER(GOLFDOC()->m_currentPlayer)
						->stroke[GOLFDOC()->m_currentHole - 1] ==
					(unsigned char)1 &&
				GOLFDOC()->GetPlayerNum() > 1)
			{
				int i;
				for (i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
				{
					if (GOLFDOC()->GetPlayer(i)->state != 3 &&
						i != GOLFDOC()->m_currentPlayer)
					{
						if (PLAYER(GOLFDOC()->m_currentPlayer)->score + 2 >
							GOLFDOC()->GetPlayer(i)->score)
							break;
					}
				}

				if (i == GOLFDOC()->GetPlayerNum() &&
					GOLFDOC()->m_currentHole % 4 == 0)
					SetBubble(0x13, false, 0);
			}

			if (PLAYER(GOLFDOC()->m_currentPlayer)->flag == 2 &&
				GOLFDOC()->m_currentHole > 1 &&
				GOLFDOC()
						->GetHoleData(GOLFDOC()->m_currentHole - 1)
						.rank[GOLFDOC()->m_currentPlayer] ==
					GOLFDOC()->GetCurrentPlayerNum())
				SetBubble(0x1D, false, 0);
		}

		if (CSharedDoc::Instance()->m_golfGame.holes == 18 &&
			GOLFDOC()->m_currentHole ==
				CSharedDoc::Instance()->m_holeOrder[17] &&
			GolfBall().m_groundType == 2 &&
			CSharedDoc::Instance()
					->m_userInfo[GOLFDOC()->m_currentPlayer]
					.stat.cBestScore[GetDifficulty() - 1] != 0x7F &&
			(unsigned char)(PLAYER(GOLFDOC()->m_currentPlayer)->totalStroke +
				PLAYER(GOLFDOC()->m_currentPlayer)
					->stroke[GOLFDOC()->m_currentHole - 1]) <
				(unsigned char)(CSharedDoc::Instance()
									->m_userInfo[GOLFDOC()->m_currentPlayer]
									.stat.cBestScore[GetDifficulty() - 1] +
					72))
			SetBubble(0x15, false, 0);

		if (PLAYER(GOLFDOC()->m_currentPlayer)->flag == 2)
		{
			int result = 0;
			SendMsg(this, "NPC", 0x188, 4, (intptr_t)&result, 0, 0);
			if (result)
				SetBubble(0x1E, false, 0);
		}
	}
}

void CGolfCaddie::SetBubble(unsigned char type, bool chatOnly,
	unsigned char param)
{
	sBehavior* behavior = SetTalkToBubble(type, chatOnly, param);
	if (!behavior)
		return;

	m_bubbleTalkType = type;
	WVector pos = m_pCaddie->m_pBody->GetPet()
					  ->FindBone("Bip01 HeadNub")
					  ->GetMatrix()
					  .pivot;
	SendMsg(this, "Bubble", 0x13, (intptr_t)behavior->m_motion.c_str(),
		(intptr_t)&pos, 0, 0);
}

void CGolfCaddie::SetBubble(unsigned char type, const char* msg)
{
	if (m_pCaddie && !m_talked[type])
	{
		m_talked[type] = true;

		std::map<std::string, sBehavior>::iterator it =
			m_pCaddie->m_talks.find(talkTable[type].m_name);
		if (it == m_pCaddie->m_talks.end())
			return;

		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			if (m_pCaddie->m_pInfo)
			{
				if (m_pCaddie->m_pInfo->Level < (*it).second.m_level)
					return;
			}
			else if ((*it).second.m_level > 0)
				return;
		}

		m_bubbleTalkType = type;
		WVector pos = m_pCaddie->m_pBody->GetPet()
						  ->FindBone("Bip01 HeadNub")
						  ->GetMatrix()
						  .pivot;
		SendMsg(this, "Bubble", 0x13, (intptr_t)msg, (intptr_t)&pos, 0, 0);
	}
}

CGolfCaddie::sBehavior* CGolfCaddie::SetTalkToBubble(unsigned char type,
	bool chatOnly, unsigned char param)
{
	if (!m_pCaddie || m_talked[type])
		return 0;

	if (!chatOnly && m_bubbleTalkType)
		return 0;

	if (GOLFDOC()->m_tutorialMode != 15)
		return 0;

	m_talked[type] = true;
	std::map<std::string, sBehavior>::iterator it =
		m_pCaddie->m_talks.find(MakeStr("%s%d", talkTable[type].m_name,
			!param ? CRandomHouse::Instance()->GetRandom() % 5 + 1 : param));
	if (it == m_pCaddie->m_talks.end())
		return 0;

	sBehavior* behavior = &(*it).second;
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
	{
		if (m_pCaddie->m_pInfo)
		{
			if (m_pCaddie->m_pInfo->Level < behavior->m_level)
				return 0;
		}
		else
		{
			if (behavior->m_level > 0)
				return 0;
		}
	}

	return behavior;
}

void CGolfCaddie::SetCaddieTalk_ChatOnly(unsigned char type)
{
	sBehavior* behavior = SetTalkToBubble(type, true, 0);
	if (!behavior)
		return;

	std::string msg;
	IFF_STRUCT::sCaddie* iffCaddie =
		ItemManager()->FindCaddie(CSharedDoc::Instance()
				->m_userInfo[GOLFDOC()->m_currentPlayer]
				.caddieInfo.tid);
	if (iffCaddie)
	{
		msg = iffCaddie->c.Name;
		msg += " : ";
		msg += behavior->m_motion;
		CChatMsg::Instance()->AddChatMsg(msg, 0xff00ff00, true, false);
	}
}

void CGolfCaddie::Display(void)
{
	if (!ShouldDisplay())
		return;

	m_pCaddie->m_pBody->Display();

	if (m_pRing && m_pCaddie->m_showRing)
		m_pRing->Display();
}

void CGolfCaddie::DisplayShadow(void)
{
	if (!ShouldDisplay())
		return;

	CShadowManager::Instance()->RegisterCaster_New("caddie", 0x10, "sun",
		g_lightset);

	m_pCaddie->m_pBody->DisplayShadow("caddie");
}

void CGolfCaddie::ResetTalk(void)
{
	m_bubbleTalkType = 0;
	for (int i = 0; i < 36; i++)
		m_talked[i] = false;

	m_talkFlag = false;
	m_talkResetTime = g_CurrentTime;
}

void CGolfCaddie::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 0x1DF:
		m_unknown60 = msg.param1;
		break;
	case 0x1DB:
		ResetTalk();
		break;
	case 0x1DC:
		SetBubble((unsigned char)msg.param1, true, 0);
		break;
	case 0x1DE:
		SetCaddieTalk_ChatOnly((unsigned char)msg.param1);
		break;
	case 0x1DD:
		if (!talkTable[m_bubbleTalkType].m_once)
			m_talked[m_bubbleTalkType] = false;

		m_bubbleTalkType = 0;
		m_pCaddieTalk->OnStop();
		break;
	case 0x2718:
		m_talkTime = *(const float*)msg.param1;
		break;
	case 0x1A1:
		m_talkFlag = false;
		m_talkResetTime = g_CurrentTime;
		m_talkTime += *(const float*)msg.param1;
		break;
	case 0x1D6:
		Dodge();
		break;
	case 0x1D7:
	{
		int result = 1;
		SendMsg(this, "NPC", 0x184, (intptr_t)&result, 0, 0, 0);

		if (m_pCaddie && !m_dodging && result)
		{
			if (msg.param1)
			{
				m_runDelay = -1.0f;

				WPolySoup::sPoint* point =
					GOLFDOC()->m_pPolySoup->FindPoint((const char*)msg.param1);

				m_destination =
					GetPVS().GetGroundPoint(point->pos, false, 0, false) -
					GolfBall().m_pos;

				m_destination.y = 0.0f;
				m_pCaddie->m_pBody->SetMoveScale(6.0f);

				m_destination += GolfBall().m_pos;

				CCameraWork::Instance()->SetCaddieChase(true);
				return;
			}

			if (IsGoodMotion(m_pCaddie, "\xb4\xde\xb8\xae\xb1\xe2"))
			{
				m_runDelay = m_pCaddie->m_startDelay;

				m_destination =
					GolfBall().PreData(GolfBall().m_renderNum)->pos -
					GolfBall().PreData(0)->pos;
				m_destination.y = 0.0f;

				float time = (m_destination.Magnitude() +
								 (float)(GolfBall().m_renderNum -
									 GolfBall().m_bounceFrame - 150) *
									 0.6f) /
					m_pCaddie->m_petSpeed;
				m_pCaddie->m_pBody->SetMoveScale(
					time / (GolfBall().m_totalTime - 1.0f));

				m_destination =
					GolfBall().PreData(GolfBall().m_renderNum)->pos -
					m_destination.Normalize() * 9.6f;

				LookUp(&GolfBall().m_pos, false);

				CCameraWork::Instance()->SetCaddieChase(true);
				return;
			}
		}

		CCameraWork::Instance()->SetCaddieChase(false);
		break;
	}
	case 0x121:
		m_visible = true;
		m_unknown30 = 0;
		m_idleTime = 20.0f;
		m_runDelay = 0.0f;
		m_dodging = false;
		ResetTalk();

		m_pCaddie = FindCaddie(
			CSharedDoc::Instance()->m_userInfo[MyIndex()].caddieInfo.tid,
			CSharedDoc::Instance()->m_userInfo[MyIndex()].caddieInfo.tidPart);

		if (m_pCaddie)
		{
			m_pCaddie->m_pInfo =
				&CSharedDoc::Instance()->m_userInfo[MyIndex()].caddieInfo;

			m_pCaddie->m_pBody->Reset();
			m_pCaddie->m_pBody->SetPos(*(const WVector*)msg.param1);
			m_pCaddie->m_pBody->SetRotate(5.4977875f);
			m_pCaddie->m_pBody->SetMotion((const char*)msg.param2, false, 0.0f,
				false);
			m_pCaddie->m_pBody->Update();
			m_pCaddie->m_pBody->SetLight();
			m_pCaddie->m_pBody->LookUp(0, true);

			if (m_pRing && m_pCaddie->m_showRing)
			{
				m_pRing->SetCenter(m_position + WVector(0.0f, 0.3f, 0.0f));
				m_pRing->SetActive(true);
			}

			CSceneManager::Instance()->SetVisible("\xc4\xb3\xb5\xf0", true);
			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0",
				m_pCaddie->m_pBody->GetPet());

			m_unknown64 = 0;
		}
		else
		{
			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0", 0);
		}
		break;
	case 0x13:
		m_visible = true;
		m_unknown30 = 0;
		m_idleTime = 20.0f;
		m_runDelay = 0.0f;
		m_dodging = false;
		ResetTalk();

		m_pCaddie = FindCaddie(CSharedDoc::Instance()
								   ->m_userInfo[GOLFDOC()->m_currentPlayer]
								   .caddieInfo.tid,
			CSharedDoc::Instance()
				->m_userInfo[GOLFDOC()->m_currentPlayer]
				.caddieInfo.tidPart);

		if (m_pCaddie)
		{
			m_pCaddie->m_pInfo = &CSharedDoc::Instance()
									  ->m_userInfo[GOLFDOC()->m_currentPlayer]
									  .caddieInfo;

			SearchGround();

			m_pCaddie->m_pBody->Reset();
			m_pCaddie->m_pBody->SetPos(m_position);
			m_pCaddie->m_pBody->SetRotate(m_angle);
			m_pCaddie->m_pBody->SetMotion("\xb1\xe2\xba\xbb", false, 0.0f,
				false);
			m_pCaddie->m_pBody->Update();
			m_pCaddie->m_pBody->SetLight();
			m_pCaddie->m_pBody->LookUp(0, true);

			GetFxInterpreter()->DetachFx(m_fxScript);
			m_fxScript = GetFxInterpreter()->ParseBoundingBoxScript_fx(
				m_pCaddie->m_pBody->GetPet(), FxInterpreter::eSF_FX);

			GetPVS().SetCaddieBox(m_pCaddie->m_pBody->GetBoundBox());

			if (m_pRing && m_pCaddie->m_showRing)
			{
				m_pRing->SetCenter(m_position + WVector(0.0f, 0.3f, 0.0f));
				m_pRing->SetActive(true);
			}

			if (IsCurMapSnow())
			{
				Attacher attacher;

				w_bound_box* box =
					m_pCaddie->m_pBody->GetPet()->FindBoundBox("Breathe");

				if (box)
				{
					attacher.mat[0] = &box->bone->GetMatrix();

					attacher.pos[0] = &box->info->spherePivot;
					CFx::Instance()->Attach(attacher, "breath.seq", true, 0);
				}
			}

			CSceneManager::Instance()->SetVisible("\xc4\xb3\xb5\xf0", true);
			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0",
				m_pCaddie->m_pBody->GetPet());

			m_unknown64 = 0;
		}
		else
		{
			GetFxInterpreter()->DetachFxAll();
			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0", 0);
		}
		break;
	case 0x2719:
		m_position = *(const WVector*)msg.param1;
		if (m_pCaddie)
		{
			m_pCaddie->m_pBody->SetPos(m_position);

			if (m_pRing && m_pCaddie->m_showRing)
				m_pRing->SetCenter(m_position + WVector(0.0f, 0.3f, 0.0f));
		}
		break;
	case 0x1E2:
	{
		float angle = m_angle = *(const float*)msg.param1;
		if (m_pCaddie)
			m_pCaddie->m_pBody->SetRotate(angle);
		break;
	}
	case 0x1D8:
		LookUp((WVector*)msg.param1, msg.param2 != 0);
		break;
	case 0x1D9:
		DontLookUp();
		break;
	case 0xF4:
		*(WVector*)msg.param1 = m_position;
		break;
	case 0x2716:
		if (m_pCaddie)
		{
			if (IsGoodMotion(m_pCaddie, (const char*)msg.param1))
				m_pCaddie->m_pBody->SetMotion((const char*)msg.param1, false,
					0.0f, false);
		}
		break;
	case 0x2717:
		if (m_pCaddie)
		{
			if (IsGoodMotion(m_pCaddie, (const char*)msg.param1))
				m_pCaddie->m_pBody->SetNextMotion((const char*)msg.param1,
					false);
		}
		break;
	case 0x1DA:
		g_audio->PlaySfx("caddie_XXX", 0, 0, 0, 0, 0.5f, 200.0f);
		if (CSharedDoc::Instance()->m_golfGame.gameType == 1)
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge = 0.0f;
		else
			PLAYER(GOLFDOC()->m_currentPlayer)->gauge = 0.0f;
		break;
	case 0x4A:
	{
		unsigned long typeId = msg.param1;
		unsigned long cadItemTypeId = msg.param3;
		sCaddie* caddie = 0;

		if (typeId && cadItemTypeId &&
			(typeId & 0x3FFFFFF) != ((cadItemTypeId >> 21) & 0x1F))
			break;

		ResetTalk();

		if (typeId)
			caddie = AddCaddie(typeId, cadItemTypeId, true);

		m_pCaddie = caddie;
		if (caddie)
		{
			caddie->m_pInfo = &CSharedDoc::Instance()->m_userInfo[0].caddieInfo;
			m_pCaddie->m_pInfo->tid = typeId;
			m_pCaddie->m_pInfo->Level = (unsigned char)(msg.param2 < 0
					? 0
					: (msg.param2 > 3 ? 3 : msg.param2));

			SearchGround();

			m_pCaddie = caddie;
			m_pCaddie->m_pBody->Reset();
			m_pCaddie->m_pBody->SetPos(m_position);
			m_pCaddie->m_pBody->SetRotate(m_angle);
			m_pCaddie->m_pBody->SetMotion("\xb1\xe2\xba\xbb", false, 0.0f,
				false);
			m_pCaddie->m_pBody->Update();
			m_pCaddie->m_pBody->SetLight();
			m_pCaddie->m_pBody->LookUp(0, true);
			GetPVS().SetCaddieBox(m_pCaddie->m_pBody->GetBoundBox());

			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0",
				m_pCaddie->m_pBody->GetPet());

			if (m_pRing && m_pCaddie->m_showRing)
			{
				m_pRing->SetCenter(m_position + WVector(0.0f, 0.3f, 0.0f));
				m_pRing->SetActive(true);
			}
		}
		else
		{
			CSharedDoc::Instance()->m_userInfo[0].caddieInfo.tid = 0;
			CSceneManager::Instance()->SetPuppet("\xc4\xb3\xb5\xf0", 0);
		}
		break;
	}
	case 0x163:
		m_visible = true;
		break;
	case 0x164:
		m_visible = false;
		break;
	case 0x118:
		if (m_pCaddie)
			m_pCaddie->m_pBody->GetPet()->SetRenderMode(msg.param1);
		break;
	case 0x1E0:
		*(sCaddie**)msg.param1 = m_pCaddie;
		break;
	case 0x1E1:
		m_unknown64 = msg.param1 == 1;
		break;
	case 0x1E3:
	{
		unsigned long typeId = msg.param1;
		unsigned long cadItemTypeId = msg.param2;
		if (!ItemManager()->FindCaddie(typeId))
			break;

		IFF_STRUCT::sCadItem* pCadItem =
			ItemManager()->FindCadItem(cadItemTypeId);
		if (!pCadItem)
			break;

		for (std::list<sCaddie*>::iterator it = m_caddies.begin();
			it != m_caddies.end(); it++)
		{
			sCaddie* caddie = *it;
			if (caddie == NULL || caddie->m_pInfo == NULL)
			{
				continue;
			}

			if (caddie->m_pInfo->tid == typeId &&
				caddie->m_pInfo->tidPart == cadItemTypeId)
			{
				if (strlen(pCadItem->FaceTex))
					caddie->m_pBody->SetPendingTexPart("face",
						pCadItem->FaceTex);
				if (strlen(pCadItem->BodyTex))
					caddie->m_pBody->SetPendingTexPart("body",
						pCadItem->BodyTex);
			}
		}
		break;
	}
	}
}

void CGolfCaddie::SearchGround(void)
{
	for (int i = 0; i < 4; i++)
	{
		m_angle = GOLFDOC()->GetCameraData().angle +
			(i & 1 ? 1.0f : -1.0f) * 0.6981317f + (i & 2 ? constants[0] : 0.0f);
		m_position = GetPVS().GetGroundPoint(
			GolfBall().m_pos + RotMat(m_angle, 1).za * 22.4f, true, 0, false);

		if (GetPVS().GetMeshIndex(m_position, true) < 0)
			continue;

		if (GetPVS().GetMeshNormal(m_position).y <
			(float)cos(0.3490658476948738))
			continue;

		if (!GetPVS().OverlappedWithObject(m_position, 1.6f))
			break;
	}
}

bool CGolfCaddie::ShouldDisplay(void)
{
	if (!m_visible || !m_pCaddie)
		return false;

	float dist = (g_camera.pivot - m_position).Magnitude();
	if (dist > 640.0f)
		return false;

	return true;
}

void CGolfCaddie::Dodge(void)
{
	if (!m_pCaddie)
		return;

	WVector delta = m_position - GolfBall().PreData(0)->pos;
	float angle = CalcDeltaAngle(delta, GolfBall().PreData(0)->vel);

	switch (GetPVS().CollideBetweenBallandCaddie())
	{
	case 2:
	case 3:
		if (angle > 0.0f)
		{
			if (IsGoodMotion(m_pCaddie,
					"\xbf\xde\xc2\xca\xc0\xb8\xb7\xce\xc7\xc7\xc7\xcf\xb1\xe2"))
			{
				m_dodging = true;
				m_pCaddie->m_pBody->SetMotion(
					"\xbf\xde\xc2\xca\xc0\xb8\xb7\xce\xc7\xc7\xc7\xcf\xb1\xe2",
					false, 0.0f, false);
			}
		}
		else
		{
			if (IsGoodMotion(m_pCaddie,
					"\xbf\xc0\xb8\xa5\xc2\xca\xc0\xb8\xb7\xce\xc7\xc7\xc7\xcf\xb1\xe2"))
			{
				m_dodging = true;
				m_pCaddie->m_pBody->SetMotion(
					"\xbf\xc0\xb8\xa5\xc2\xca\xc0\xb8\xb7\xce\xc7\xc7\xc7\xcf\xb1\xe2",
					false, 0.0f, false);
			}
		}
		break;
	case 1:
		if (IsGoodMotion(m_pCaddie,
				"\xb3\xf4\xc0\xba\xb0\xf8\xc7\xc7\xc7\xcf\xb1\xe2"))
		{
			m_dodging = true;
			m_pCaddie->m_pBody->SetMotion(
				"\xb3\xf4\xc0\xba\xb0\xf8\xc7\xc7\xc7\xcf\xb1\xe2", false, 0.0f,
				false);
		}
		break;
	case 0:
		m_dodging = false;
		LookUp(&GolfBall().m_pos, true);
		break;
	}
}

void CGolfCaddie::Run(void)
{
	if (!m_pCaddie)
		return;

	m_position = m_pCaddie->m_pBody->GetPos();

	WVector dir = m_destination - m_position;
	WVector vel = m_pCaddie->m_pBody->GetVel();

	float dot = dir.x * vel.x + dir.z * vel.z;
	float stopDist = Wabs(GolfBall().m_initCurve) < g_EPSILON
		? m_pCaddie->m_stopDist
		: m_pCaddie->m_stopDist * 2.0f;

	if (sqrtf(dir.x * dir.x + dir.z * dir.z) < stopDist ||
		(GolfBall().m_playFrame > 0 &&
			(GolfBall().m_renderNum - GolfBall().m_playFrame < 50 ||
				dot < 0.0f)))
	{
		m_pCaddie->m_pBody->SetMoveScale(1.0f);
		if (GOLFDOC()->m_tutorialMode == 15)
			m_pCaddie->m_pBody->SetMotion("\xb1\xde\xc1\xa4\xb0\xc5", true,
				0.0f, false);
		else
			m_pCaddie->m_pBody->SetMotion(
				"\xc6\xa9\xc5\xcd\xb8\xae\xbe\xf3\xb1\xde\xc1\xa4\xb0\xc5",
				true, 0.0f, false);

		m_unknown30 = 0;
		CFx::Instance()->RemoveRunDust(m_runDust);

		if (GolfBall().m_renderNum > 0)
		{
			CGolfBall& ball = GolfBall();
			unsigned char ground = GetPVS().GetGroundType(
				ball.PreData(ball.m_renderNum)->pos, true, 0);

			if (ball.m_bOB || ground >= 3)
				m_pCaddie->m_pBody->SetNextMotion("\xba\xa1\xc4\xbf", false);

			dot = sqrtf(vel.x * vel.x + vel.z * vel.z);
			if (Wabs(dot) < g_EPSILON)
			{
				CGolfBall& ball = GolfBall();
				WVector pos;
				pos =
					ball.PreData(ball.m_renderNum)->pos - ball.PreData(0)->pos;
				pos.Normalize();
				WVector offset = pos * 5.0f;

				pos = ball.PreData(ball.m_renderNum)->pos - offset;
				pos = GetPVS().GetGroundPoint(pos, false, 0, false);

				m_pCaddie->m_pBody->SetPos(pos);
				m_position = pos;
			}
		}
	}

	m_pCaddie->m_pBody->SetLight();
}

void CGolfCaddie::LookUp(WVector* target, bool immediate)
{
	if (m_pCaddie)
		m_pCaddie->m_pBody->LookUp(target, immediate);
}

void CGolfCaddie::DontLookUp(void)
{
	if (m_pCaddie)
		m_pCaddie->m_pBody->LookUp(0, true);
}

void CGolfCaddie::UseSpecialAbility(void)
{
	// HACK!
	if (false)
		Run();
}
