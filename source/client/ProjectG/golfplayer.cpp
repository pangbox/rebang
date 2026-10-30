#include "minatl.h"
#include "golfdoc.h"
#include "uccmanager.h"
#include "wbone.h"
#include "club.h"
#include "partsinfo.h"
#include "fx.h"
#include "golfplayer.h"
#include "taileffect.h"
#include "scenemanager.h"
#include "texturepart.h"
#include "golfball.h"
#include "headicon.h"
#include "inputmanager.h"
#include "quadtree.h"
#include "polysoup.h"
#include "parttidlist.h"
#include "soundmanager.h"
#include "slicestr.h"
#include "quitwindow.h"
#include "itemwindow.h"
#include "../../shared/localize.h"
#include "helpmsg.h"
#include "projectg.h"
#include "mathconsts.h"

const char* GetStrConst(const char* str, char* out);
const char* GetCharsTillSpace(const char* str, char* out);
int Ceil(float value, float epsilon);

IMPLEMENT_ACTOR(CGolfPlayer, IActor)

static struct sTailTexture
{
	unsigned long typeId;
	const char* texture;
} s_tailTexture[] = {
	{ 0x10000009, "~swinglight02.jpg"      },
	{ 0x1000000c, "~swinglight01_pink.jpg" },
	{ 0x1000000a, "~swinglight01_blue.jpg" },
};

const char* s_shotName[] = {
	"\xbf\xec\xb5\xe5\xbc\xa6",
	"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6",
	"\xbf\xec\xb5\xe5\xbc\xa6\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
	"\xbf\xec\xb5\xe5\xbc\xa6\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
	"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
	"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xc6\xc4\xbf\xf6",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xc6\xc4\xbf\xf6\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
	"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xc6\xc4\xbf\xf6\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
	"\xbb\xf7\xb5\xe5\x9f\x87\xc1\xf6\xbc\xa6",
	"\xbb\xf7\xb5\xe5\x9f\x87\xc1\xf6\xbc\xa6\xc6\xc4\xbf\xf6",
	"\xc6\xdb\xc6\xc3\xbc\xa6",
	"\xc2\xaa\xc0\xba\xc6\xdb\xc6\xc3",
	"\xb3\xa1",
};

WMatrix convMat(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
WMatrix convMatInv(0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

CGolfPlayer::CGolfPlayer()
	: m_clubPuppet(NULL),
	  m_petFrame(NULL),
	  m_petInfo(NULL),
	  m_tail(NULL),
	  m_bDisplay(true),
	  m_bClubMat(false)
{
	m_bCombo = false;
	m_comboTime = 0;

	m_reserved320 = 0;
	m_powerShotTime = 0;
	m_bPowerShot = false;
	m_bForcePowerShot = false;
	m_powerShotState = 0;

	m_headBone = NULL;
	m_safeTeeSeq = NULL;

	m_bSkipScript = false;
}

void CGolfPlayer::OnDestroy()
{
	GolfClub().ClearClubSet();

	std::list<sTexturePart*>::iterator it;
	for (it = m_texPartList.begin(); it != m_texPartList.end(); ++it)
		if (*it)
		{
			delete *it;
			*it = NULL;
		}

	m_texPartList.clear();

	if (m_tail)
	{
		delete m_tail;
		m_tail = NULL;
	}

	if (m_petFrame)
	{
		delete[] m_petFrame;
		m_petFrame = NULL;
	}
	if (m_petInfo)
	{
		delete[] m_petInfo;
		m_petInfo = NULL;
	}
}

void CGolfPlayer::RegisterClub(int player)
{
	const char* motionName[4] = {
		"\xbf\xec\xb5\xe5\xbc\xa6\xc1\xd8\xba\xf1",
		"\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6\xc1\xd8\xba\xf1",
		"\xbb\xf7\xb5\xe5\x9f\x87\xc1\xf6\xbc\xa6\xc1\xd8\xba\xf1",
		"\xc6\xdb\xc6\xc3\xbc\xa6\xc1\xd8\xba\xf1",
	};

	CPartTidList& myTids =
		GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);

	WMatrix mat;
	mat.Reset();
	GolfClub().ClearClubSet();

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (player != -1)
		{
			if (i != player)
				continue;
		}

		if (GOLFDOC()->GetPlayer(i)->state == 3)
			continue;

		std::vector<WPuppet*>& pets = m_petFrame[i].GetAniPetList();

		sClubSet* clubSet =
			GolfClub().AddClubSet(Doc()->m_userInfo[i].clubInfo.tid);

		CPartTidList& tids = GOLFDOC()->GetPartTidList(i);
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tids.m_charTid);

		std::vector<WPuppet*>::iterator it;
		for (it = pets.begin(); it != pets.end(); it++)
		{
			WPuppet* pet = *it;

			for (int type = 0; type < 4; type++)
			{
				m_club[i][type].ballPos.Reset();
				m_club[i][type].gripPos = 0;

				w_motion_data* motion = pet->FindMotionData(motionName[type]);
				if (!motion)
					continue;

				int frame = (int)(motion->s * 30.0f + 0.5f);

				int from = -1, to;
				pet->CheckFrameData(frame, frame, from, to);
				if (from <= -1)
					continue;

				WSliceStr slice(pet->GetFrameData(from));
				slice.Get("*ball_dist", "%f", &m_club[i][type].ballPos.x);
				slice.Get("*grip_pos", "%f", &m_club[i][type].gripPos);

				m_club[i][type].ballPos.y = 0;
				m_club[i][type].ballPos.z = 0.4f;

				if (clubSet && pChar)
				{
					w_bound_box* ball =
						clubSet->puppet[type]->m_bbList.Find("Ball");
					w_bound_box* target =
						clubSet->puppet[type]->m_bbList.Find("Ball_Target");

					if (ball && target)
					{
						clubSet->puppet[type]->ApplyBones(NULL, mat);
						clubSet->puppet[type]->Transform(NULL, 1.0f, 0);

						WVector diff;
						diff =
							(ball->info->spherePivot * ball->bone->GetMatrix() -
								target->info->spherePivot *
									target->bone->GetMatrix()) *
							pChar->ClubScale;

						m_club[i][type].ballPos.x += diff.x;
						m_club[i][type].ballPos.z += diff.z;
					}
				}
			}
		}
	}
}

void CGolfPlayer::OnPreLoadInit()
{
	m_lightSet.type = 2;

	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, NULL, NULL,
		"\xc5\xac\xb7\xb4", 0x28, NULL);

	CSceneManager::Instance()->RegisterElement(OBJ_EFFECT, NULL, m_tail, "tail",
		0, NULL);
	CSceneManager::Instance()->SetVisible("tail", false);

	m_petInfo = new sPetInfo[4];

	for (int i = 0; i < 4; i++)
	{
		CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, NULL, NULL,
			MakeStr("PLAYER_%d", i), 0x18, NULL);
		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i), false);
	}

	m_tail = new CTailEffect("~swinglight01.jpg");
}

void CGolfPlayer::OnLoad()
{
	if (CHeadIcon::Instance())
		CHeadIcon::Instance()->SetLevel();

	if (m_petFrame)
	{
		delete[] m_petFrame;
		m_petFrame = NULL;
	}
	m_petFrame = new CPetFrame[GOLFDOC()->GetPlayerNum()];

	for (int i = 0; i < 4; i++)
	{
		if (i < GOLFDOC()->GetPlayerNum())
		{
			if (GOLFDOC()->GetPlayer(i)->state != 3)
			{
				CPartTidList* tids = &GOLFDOC()->GetPartTidList(i);

				ShowAbilityParts(tids);

				if (!m_petFrame[i].Build(tids,
						Doc()->m_userInfo[i].charInfo.tidAuxParts, true, true,
						i))
				{
					tids->SetDefaultTids();
					m_petFrame[i].Build(tids,
						Doc()->m_userInfo[i].charInfo.tidAuxParts, true, false,
						i);
				}

				m_petFrame[i].CalcBVolume();
				SetMotion("\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7", 0.0f, i);

				m_petInfo[i].powerShot = 0;
				m_petInfo[i].nextMotion[0] = 0;

				CSceneManager::Instance()->SetPuppet(MakeStr("PLAYER_%d", i),
					m_petFrame[i].GetBonePet());

				m_extendedEffect.RegistExtenedEffect(&m_petFrame[i]);
			}
		}
		else
		{
			m_petInfo[i].bVisible = false;
			CSceneManager::Instance()->SetPuppet(MakeStr("PLAYER_%d", i), NULL);
		}
	}

	if (Doc()->m_golfGame.gameType == 1)
	{
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (i == MyIndex() ||
				(PLAYER(MyIndex())->teammate != 0xff &&
					i == PLAYER(MyIndex())->teammate) ||
				!WNetworkSystem::Instance()->IsConnected(
					WNetworkSystem::NET_GAME))
			{
				MakeStr("PLAYER_%d", i);
			}
		}
	}

	RegisterClub(-1);
	CheckShotScript();
}

void CGolfPlayer::OnReload()
{
	OnLoad();
}

bool CGolfPlayer::ShowAbilityParts(CPartTidList* partList)
{
	if (m_abilityParts.empty())
	{
		AddAbilityParts(0x080a2800, 0x080a4000);
		AddAbilityParts(0x080a2801, 0x080a4001);
		AddAbilityParts(0x080a2802, 0x080a4002);

		AddAbilityParts(0x08122800, 0x08124000);
		AddAbilityParts(0x08122801, 0x08124001);
		AddAbilityParts(0x08122802, 0x08124002);

		AddAbilityParts(0x08026800, 0x08024800);
		AddAbilityParts(0x08026801, 0x08024801);
		AddAbilityParts(0x08026802, 0x08024802);

		AddAbilityParts(0x08064800, 0x08066800);
		AddAbilityParts(0x08064801, 0x08066801);
		AddAbilityParts(0x08064802, 0x08066802);
		AddAbilityParts(0x08064803, 0x08066803);

		AddAbilityParts(0x081ea800, 0x081ec800);

		AddAbilityParts(0x080e4800, 0x080e6800);
		AddAbilityParts(0x080e4801, 0x080e6801);
		AddAbilityParts(0x080e4802, 0x080e6802);

		AddAbilityParts(0x081a4800, 0x081a6800);
		AddAbilityParts(0x081a4800, 0x081a8800);
		AddAbilityParts(0x081a4801, 0x081a6801);
		AddAbilityParts(0x081a4801, 0x081a8801);

		AddAbilityParts(0x0816e801, 0x08152801);
		AddAbilityParts(0x0816e802, 0x08152802);
		AddAbilityParts(0x0816e803, 0x08152803);

		AddAbilityParts(0x08228800, 0x08226800);
		AddAbilityParts(0x08228801, 0x08226801);
		AddAbilityParts(0x08228802, 0x08226802);
		AddAbilityParts(0x08228803, 0x08226803);
	}

	bool bShown = false;
	int count = GetAbilityPartsSize();

	for (int i = 0; i < count; i++)
	{
		unsigned long from = m_abilityParts[i].first;
		unsigned long to = m_abilityParts[i].second;

		if ((partList->m_charTid & 0x3ffffff) == ((from >> 18) & 0xff))
		{
			if (partList->IsEquipped(from, 0) == true)
			{
				partList->SetTid(to, 0, NULL, 0);
				bShown = true;
			}
		}
	}

	return bShown;
}

void CGolfPlayer::CheckShotScript()
{
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (PLAYER(i)->state == 3)
			continue;

		for (int j = 0; j < 16; j++)
		{
			w_motion_data* motion =
				m_petFrame[i].FindMotionData(s_shotName[j], true, NULL);

			if (!motion)
				continue;

			const float fps = 30.0f;
			int start = (int)ceilf(motion->s * fps);
			float last = motion->e * fps;
			int end = WisEqual(floorf(last), last, g_EPSILON)
				? -1 - (int)(motion->e * -fps)
				: (int)floorf(motion->e * fps);

			for (int frame = start; frame < end; frame++)
			{
				int index;
				if (m_petFrame[i].GetCurAniPet()->CheckFrameData(frame, frame,
						index, index))
				{
					char* data =
						m_petFrame[i].GetCurAniPet()->GetFrameData(index);

					if (strstr(data, "*shot"))
					{
						m_petInfo[i].shotTime[j] =
							(frame - start) * (1.0f / 30.0f);
						break;
					}
				}
			}
		}
	}
}

void CGolfPlayer::OnInit()
{
	m_footBone[0] = NULL;
	m_footBone[1] = NULL;
	m_motionName[0] = 0;
	m_swingType[0] = 0;
	m_bTail = false;

	m_dir = 3.14159265f;
	m_mat.Reset();
	m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_lightSet.nearOne = g_lightset.nearOne;
}

void CGolfPlayer::InputProcess(float delta)
{
	if (g_input->Get(0x45))
	{
		if (m_headAngle < 1.0471976f)
		{
			m_headAngle += delta * 12.566371f;
			m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x200;
		}
	}

	if (g_input->Get(0x51))
	{
		if (m_headAngle > -1.0471976f)
		{
			m_headAngle -= delta * 12.566371f;
			m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x200;
		}
	}
}

void CGolfPlayer::OnProcess(float delta)
{
	switch (m_petInfo[GOLFDOC()->m_currentPlayer].powerShot)
	{
	case 3:
	{
		float ratio;
		if (m_petInfo[GOLFDOC()->m_currentPlayer].motion)
		{
			ratio = (delta + m_petInfo[GOLFDOC()->m_currentPlayer].time -
						m_petInfo[GOLFDOC()->m_currentPlayer].motion->s) /
				(m_petInfo[GOLFDOC()->m_currentPlayer].motion->e -
					m_petInfo[GOLFDOC()->m_currentPlayer].motion->s);

			if (ratio > 1.0f)
			{
				ratio = 1.0f;
				m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 4;

				SendMsg(this, "Spot", 0xf5, 0, 0, 0, 0);

				if (OnlinePlay())
				{
					int type = GolfClub().PowerShotType();
					WSendPacket packet((enumClientPacket)0x15);
					packet.Encode1(type);
					packet.Send((eSendTo)0);
				}
			}

			SendMsg(this, "Screen", 0x1aa, (int)&ratio, 0, 0, 0);
		}
	}
	break;

	case 0:
	case 4:
		if (GOLFDOC()->GameMode() & 0xa4000)
		{
			m_bOldTurn = false;
		}
		else
		{
			if (m_bOldTurn && !m_bTurn)
			{
				if (m_swingType[0])
				{
					char* name;
					switch (GolfClub().PowerShotType())
					{
					case 0:
						name = MakeStr("%s\xc1\xd8\xba\xf1", m_swingType);
						break;
					case 1:
						name = MakeStr(
							"%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1\xb3\xa1",
							m_swingType);
						break;
					case 2:
					case 3:
						name = MakeStr("%s\xc6\xc4\xbf\xf6"
									   "2\xc1\xd8\xba\xf1\xb3\xa1",
							m_swingType);
						break;
					}
					SetMotion(name, 0.0f, -1);
				}
			}
			m_bOldTurn = m_bTurn;
		}
		m_bTurn = false;
		break;
	}

	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (!m_petInfo[i].bVisible || GOLFDOC()->GetPlayer(i)->state == 3)
			continue;

		m_petInfo[i].time += delta;

		if (Abs(m_petInfo[i].alphaVel) > 0.0f)
			m_petInfo[i].alpha = Between(0.0f,
				m_petInfo[i].alpha + delta * m_petInfo[i].alphaVel, 1.0f);

		ScriptProcess(m_petInfo[i].motion, m_petInfo[i].time - delta,
			m_petInfo[i].time, i, false, false);

		if (m_petInfo[i].motion && m_petInfo[i].time > m_petInfo[i].motion->e)
		{
			if (m_petInfo[i].motion->s != m_petInfo[i].motion->e)
			{
				if (m_petInfo[i].powerShot == 2)
				{
					m_bTail = false;
					CSceneManager::Instance()->SetVisible("tail", false);
				}

				if (m_petInfo[i].nextMotion[0])
				{
					if (m_bMotionEnd)
					{
						m_bMotionEnd = false;
						this << MsgObject(NULL, 0x10e, 0, 0, 0, 0, 0);
						continue;
					}

					SetMotion(m_petInfo[i].nextMotion,
						m_petInfo[i].time - m_petInfo[i].motion->e, i);
					m_petInfo[i].nextMotion[0] = 0;
					continue;
				}

				char name[64];
				sprintf(name, "%s\xc1\xd8\xba\xf1", m_swingType);

				if (!strcmp(m_petInfo[i].motion->name,
						"\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7") &&
					m_swingType[0])
				{
					if (strcmp(name, m_petInfo[i].motion->nextmotion))
						SetMotion(name,
							m_petInfo[i].time - m_petInfo[i].motion->e, i);
					else
						SetMotion(m_petInfo[i].motion->nextmotion,
							m_petInfo[i].time - m_petInfo[i].motion->e, i);
				}
				else if (i == GOLFDOC()->m_currentPlayer &&
					!strcmp(m_petInfo[i].motion->name, name) &&
					(m_bTurn || GolfClub().PowerShotType() ||
						GOLFDOC()->GameMode() != 8 || m_cupDist < 9.6f))
					SetMotion(m_petInfo[i].motion->name,
						m_petInfo[i].time - m_petInfo[i].motion->e, i);
				else
					SetMotion(m_petInfo[i].motion->nextmotion,
						m_petInfo[i].time - m_petInfo[i].motion->e, i);
			}
			else
			{
				m_petInfo[i].time = m_petInfo[i].motion->s;
				m_petInfo[i].flags &= ~2;
			}
		}
	}

	if (GolfClub().TypeUpdated())
	{
		ParseBoundingBoxScript(GOLFDOC()->m_currentPlayer, true, false, true);
		SetSwingType();
		SearchGround();

		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x21;
	}

	if (m_bTail)
	{
		m_tail->Process(delta);

		if (m_clubNear && m_clubFar)
		{
			WVector nearPos, farPos;
			nearPos =
				m_clubNear->info->spherePivot * m_clubNear->bone->GetMatrix();
			farPos =
				m_clubFar->info->spherePivot * m_clubFar->bone->GetMatrix();

			m_tail->PutSpot(nearPos, farPos);
		}
	}

	if (GolfClub().GetType() == 3)
	{
		if (GolfBall().m_groundType == 2 && GOLFDOC()->m_tutorialMode >= 4 &&
			GolfBall().m_playFrame < 0)
		{
			if (g_input->GetDown("LALT", true) ||
				g_input->GetDown("RALT", true))
				m_bAlpha = !m_bAlpha;
		}
	}
	else
		PowerShotInput();

	if (IsLocalContent(S4_UCC))
		RefreshAllUccClothes();
}

void CGolfPlayer::OnDisplay()
{
	if ((GOLFDOC()->GameMode() & 0x20000) ||
		((Doc()->m_golfGame.gameType == 1 || Doc()->m_golfGame.gameType == 3 ||
			 Doc()->m_golfGame.gameType == 7) &&
			(GOLFDOC()->GameMode() & 0x3000)))
	{
		ScoreEvent();
		return;
	}

	if (!m_bDisplay)
		return;

	if (m_petInfo[GOLFDOC()->m_currentPlayer].flags & 1)
	{
		if (GOLFDOC()->GameMode() & 0x3000)
		{
			WMatrix& mat = m_petInfo[GOLFDOC()->m_currentPlayer].mat;

			mat.za = m_mat.za;
			mat.ya = WVector(0.0f, 1.0f, 0.0f);
			mat.xa = WCrossProduct(mat.ya, mat.za);
			mat.pivot =
				GetPVS().GetGroundPoint(m_pos * m_mat, false, NULL, false);
		}
		else if (GOLFDOC()->GameMode() & 0x4000)
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.za = m_mat.za;
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.ya = WVector::UNIT_POS_Y;
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.xa =
				WCrossProduct(m_petInfo[GOLFDOC()->m_currentPlayer].mat.ya,
					m_petInfo[GOLFDOC()->m_currentPlayer].mat.za);
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot =
				GetPVS().GetGroundPoint(m_pos +
						m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
								.ballPos *
							m_mat,
					false, NULL, false);
		}
		else
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].mat = m_mat;
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot = m_pos +
				m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
						.ballPos *
					m_mat;
		}

		const char* texture = GetPVS().GetTextureName(
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot);

		if (texture &&
			(!stricmp(texture, "igrass05.dds") ||
				!stricmp(texture, "igrass04_01.dds")))
			m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot.y -= 0.32000002f;
	}

	if (m_petInfo[GOLFDOC()->m_currentPlayer].flags & 3)
	{
		m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->ApplyBones(
			m_petFrame[GOLFDOC()->m_currentPlayer].GetCurAniPet()->GetBones(
				NULL, m_petInfo[GOLFDOC()->m_currentPlayer].time, 0, NULL,
				false),
			m_petInfo[GOLFDOC()->m_currentPlayer].mat);
		m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->UpdateBound(true,
			true);
	}

	if (GOLFDOC()->GameMode() & 0x1002)
	{
		WVector pos = GetHeadMatrix().pivot;
		pos.y = 0.0f;
		pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
		m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot.y = pos.y;
	}

	if (!(GOLFDOC()->GameMode() & 0x4000) &&
		(m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0xb))
	{
		for (int i = 0; i < 2; i++)
		{
			if (m_footBone[i])
				m_footBone[i]->SetMatrix(convMatInv * GetFootMatrix(i), NULL);
		}
	}

	if (m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x203)
	{
		if (m_headBone)
			m_headBone->SetMatrix(convMatInv * GetHeadMatrix(), NULL);
	}

	if (CHeadIcon::Instance())
	{
		WBone* head =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBone(
				"Bip01 Head");
		WVector pos = head->GetMatrix().pivot;
		CHeadIcon::Instance()->SetPlayerPos(GOLFDOC()->m_currentPlayer, pos);
	}

	if (m_petInfo[GOLFDOC()->m_currentPlayer].flags & 3)
		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x30;

	WPuppet* pet = m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet();

	sClubSet* clubSet = GolfClub().FindClubSet(
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid);

	WPuppet* club = clubSet->puppet[GolfClub().GetType()];

	if (g_view->InFrustum(pet->GetBoundSphere()))
	{
		if ((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x10) ? true : false)
		{
			if ((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x40) ? true
																	 : false)
				SetLight();

			m_petInfo[GOLFDOC()->m_currentPlayer].flags &= ~0x10;
			m_petFrame[GOLFDOC()->m_currentPlayer].UpdateLightSource(
				&m_lightSet, false, NULL);
		}

		if (m_bBlocked || m_bAlpha)
		{
			m_petFrame[GOLFDOC()->m_currentPlayer].SetAlpha(
				(unsigned char)(m_petInfo[GOLFDOC()->m_currentPlayer].alpha *
					64.0f));
			club->SetAlpha(
				(unsigned char)(m_petInfo[GOLFDOC()->m_currentPlayer].alpha *
					64.0f));
		}
		else
		{
			m_petFrame[GOLFDOC()->m_currentPlayer].SetAlpha(
				(unsigned char)(m_petInfo[GOLFDOC()->m_currentPlayer].alpha *
					255.0f));
			club->SetAlpha(
				(unsigned char)(m_petInfo[GOLFDOC()->m_currentPlayer].alpha *
					255.0f));
		}

		if (!(Wabs(m_petInfo[GOLFDOC()->m_currentPlayer].alpha) < g_EPSILON))
			club->GetAlpha(NULL);

		sPetInfo* info = &m_petInfo[GOLFDOC()->m_currentPlayer];

		if (info->pendingTexPart && info->pendingTexPart != info->texPart)
		{
			sTexturePart* part = info->pendingTexPart;
			info->texPart = part;

			if (!part->bitmap)
				m_petFrame[GOLFDOC()->m_currentPlayer]
					.GetBonePet()
					->ChangeTexturePart(
						m_petFrame[GOLFDOC()->m_currentPlayer].GetFaceTex(),
						part->part, &part->bitmap, &part->rect);
			else
				m_petFrame[GOLFDOC()->m_currentPlayer]
					.GetBonePet()
					->ChangeTexturePart(
						m_petFrame[GOLFDOC()->m_currentPlayer].GetFaceTex(),
						part->bitmap, part->rect);

			m_petFrame[GOLFDOC()->m_currentPlayer].SetMakeup();
		}
	}

	if (!((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x80) ? true : false))
	{
		CPartTidList& tids =
			GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(tids.m_charTid);

		float scale;
		if (pChar)
			scale = pChar->ClubScale;
		else
			scale = 1.0f;

		WMatrix mat;
		if ((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x100) ? true
																  : false)
		{
			mat.xa = m_clubMat.xa * scale;
			mat.ya = m_clubMat.ya * scale;
			mat.za = m_clubMat.za * scale;
			mat.pivot = m_clubMat.pivot;
		}
		else
		{
			w_bound_box* box = m_petFrame[GOLFDOC()->m_currentPlayer]
								   .GetBonePet()
								   ->FindBoundBox("club");

			if (box)
			{
				mat = box->bone->GetMatrix();
				mat.pivot = box->info->spherePivot * box->bone->GetMatrix();
				mat.pivot += mat.xa *
					m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
						.gripPos;
				mat.xa *= scale;
				mat.ya *= scale;
				mat.za *= scale;
			}
			else
				mat.Reset();
		}

		club->ApplyBones(NULL, mat);
		club->Transform(NULL, 1.0f, 0);
		club->UpdateBSphere();

		if (GolfClub().TypeUpdated())
			m_clubPuppet = club;

		if ((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x20) ? true : false)
		{
			if ((m_petInfo[GOLFDOC()->m_currentPlayer].flags & 0x40) ? true
																	 : false)
				SetLight();

			m_petInfo[GOLFDOC()->m_currentPlayer].flags &= ~0x20;
		}

		club->UpdateLightSource(&m_lightSet, false, NULL);
		CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4", club);
	}

	if (m_bTail)
		m_tail->Display();

	m_petInfo[GOLFDOC()->m_currentPlayer].flags &= ~0x209;

	m_extendedEffect.Update(&m_petFrame[GOLFDOC()->m_currentPlayer]);
}

void CGolfPlayer::ScoreEvent()
{
	for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
	{
		if (!m_petInfo[i].bVisible || PLAYER(i)->state == 3)
			continue;

		m_petFrame[i].GetBonePet()->ApplyBones(
			m_petFrame[i].GetCurAniPet()->GetBones(NULL, m_petInfo[i].time, 0,
				NULL, false));
		m_petFrame[i].GetBonePet()->UpdateBound(true, false);

		if (g_view->InFrustum(m_petFrame[i].GetBonePet()->GetBoundSphere()))
			m_petFrame[i].UpdateLightSource(&m_lightSet, false, NULL);

		if (CHeadIcon::Instance())
		{
			WBone* head = m_petFrame[i].GetBonePet()->FindBone("Bip01 Head");
			WVector pos = head->GetMatrix().pivot;
			CHeadIcon::Instance()->SetPlayerPos(i, pos);
		}

		sPetInfo* pet = &m_petInfo[i];

		if (pet->pendingTexPart && pet->pendingTexPart != pet->texPart)
		{
			sTexturePart* part = pet->texPart = pet->pendingTexPart;

			if (part->bitmap == NULL)
			{
				m_petFrame[i].GetBonePet()->ChangeTexturePart(
					m_petFrame[i].GetFaceTex(), part->part, &part->bitmap,
					&part->rect);
			}
			else
			{
				m_petFrame[i].GetBonePet()->ChangeTexturePart(
					m_petFrame[i].GetFaceTex(), part->bitmap, part->rect);
			}

			m_petFrame[i].SetMakeup();
		}

		if (m_clubPuppet == NULL)
			return;

		if (!(GOLFDOC()->GameMode() & 0x3000))
			continue;

		if (Doc()->m_golfGame.gameType == 1)
		{
			if (i != TEAM(PLAYER(MyIndex())->team)->shooter)
				continue;

			if ((bool)((m_petInfo[i].flags >> 7) & 1))
				continue;
		}

		if ((bool)((m_petInfo[i].flags >> 8) & 1))
		{
			m_clubPuppet->ApplyBones(NULL, m_clubMat);
		}
		else
		{
			w_bound_box* box = m_petFrame[i].GetBonePet()->FindBoundBox("club");
			WMatrix mat;
			mat = box->bone->GetMatrix();
			mat.pivot = box->info->spherePivot * box->bone->GetMatrix();
			sClub& club = m_club[i][GolfClub().GetType()];
			mat.pivot += club.gripPos * mat.xa;
			m_clubPuppet->ApplyBones(NULL, mat);
		}

		m_clubPuppet->Transform(NULL, 1.0f, 0);
		m_clubPuppet->UpdateBSphere();
		m_clubPuppet->UpdateLightSource(&m_lightSet, false, NULL);
	}
}

WMatrix CGolfPlayer::GetHeadMatrix()
{
	WMatrix mat;
	WBone* bone = m_headBone;
	if (bone == NULL)
	{
		mat.Reset();
	}
	else
	{
		if (Wabs(m_headAngle) < g_EPSILON)
		{
			mat = convMat * bone->GetMatrix();
		}
		else
		{
			mat = RotMat(m_headAngle, 1);
			mat = mat * convMat * bone->GetMatrix();
		}
	}

	return mat;
}

WMatrix CGolfPlayer::GetFootMatrix(int foot)
{
	WMatrix mat;
	WBone* bone = m_footBone[foot];
	if (bone == NULL)
	{
		mat.Reset();
	}
	else
	{
		if (Wabs(m_footAngle) < g_EPSILON)
		{
			mat = convMat * bone->GetMatrix();
		}
		else
		{
			mat = RotMat(m_footAngle, 0);
			mat = mat * convMat * bone->GetMatrix();
		}
	}

	return mat;
}

void CGolfPlayer::ParseBoundingBoxScript(int player, bool b1, bool b2, bool b3)
{
	WVector unused[1];

	// HACK! Force the desired stack slots
	struct
	{
		union
		{
			int frameOffset;
			WList<w_bound_box*>* clubList;
		};
		w_bound_box* box;
	} cursor;
	union
	{
		CFxSpray* spray;
		CFxSequence* sequence;
		const char* cur;
	} effectStorage;
	union
	{
		WList<w_bound_box*>* bbList;
		CFxSpray* spray;
		CFxSequence* sequence;
	} listStorage;
	char fxTag[] = "*fx(";
	char clubTag[4];
	sClubSet* clubSet =
		GolfClub().FindClubSet(Doc()->m_userInfo[player].clubInfo.tid);
	Attacher attacher;

	if (b1)
	{
		for (unsigned int i = 0; i < m_spray.size(); i++)
			CFx::Instance()->CloseSpray(m_spray[i]);

		for (unsigned int j = 0; j < m_sequence.size(); j++)
			CFx::Instance()->CloseSequence(m_sequence[j], true);

		m_spray.clear();
		m_sequence.clear();
	}

	cursor.frameOffset = player * sizeof(CPetFrame);
	listStorage.bbList =
		&(*(CPetFrame*)((char*)m_petFrame + cursor.frameOffset))
			 .GetBonePet()
			 ->m_bbList;
	for (cursor.box = listStorage.bbList->Start(); cursor.box;
		cursor.box = listStorage.bbList->Next())
	{
		if (cursor.box->info->option == NULL || cursor.box->bone == NULL ||
			cursor.box->bone->IsInvisible())
			continue;

		const char* found = strstr(cursor.box->info->option, fxTag);
		const char* cur;

		for (; found; found = strstr(cur, fxTag))
		{
			char name[64];
			cur = GetStrConst(found + strlen(fxTag), name);

			int len = strlen(name);

			if (len == 0 || len >= 64)
				continue;

			char* ext = strrchr(name, '.');
			if (ext == NULL || strlen(ext) < 4)
				continue;

			char cond[64];
			cur = GetStrConst(cur, cond);

			len = strlen(cond);
			if (len == 0 || len >= 64)
				continue;

			bool bNeed = false;

			while (len != 0 && cond[0] == '&')
			{
				bNeed = true;
				unsigned long tid = atoi(&cond[1]);

				if ((*(CPetFrame*)((char*)m_petFrame + cursor.frameOffset))
						.GetPartList()
						.FindPart(tid))
				{
					bNeed = false;
					break;
				}

				for (int k = 0; k < 5; k++)
				{
					if (Doc()
							->m_userInfo[GOLFDOC()->m_currentPlayer]
							.charInfo.tidAuxParts[k] == tid)
					{
						bNeed = false;
						break;
					}
				}

				if (!bNeed)
					break;

				if (clubSet && clubSet->pClubSet &&
					(clubSet->pClubSet->ClubIds[GolfClub().GetType()] == tid ||
						clubSet->pClubSet->c.TypeId == tid))
				{
					bNeed = false;
					break;
				}

				cur = GetStrConst(cur, cond);

				len = strlen(cond);
				if (len == 0 || len >= 64)
					break;
			}

			if (bNeed)
				continue;

			attacher.mat[0] = &cursor.box->bone->GetMatrix();
			attacher.pos[0] = &cursor.box->info->spherePivot;

			if (stricmp(ext + 1, "spr") == 0)
			{
				effectStorage.spray = (CFxSpray*)CFx::Instance()->Attach(
					attacher, name, false, NULL);
				m_spray.push_back(effectStorage.spray);
			}
			else if (stricmp(ext + 1, "seq") == 0)
			{
				effectStorage.sequence = (CFxSequence*)CFx::Instance()->Attach(
					attacher, name, false, NULL);
				m_sequence.push_back(effectStorage.sequence);
			}
		}
	}

	if (clubSet)
	{
		cursor.clubList = &clubSet->puppet[GolfClub().GetType()]->m_bbList;
		strcpy(clubTag, "*fx");
		w_bound_box* box = cursor.clubList->Start();
		WList<w_bound_box*>* nextList;
		while (box)
		{
			const char* found;
			if (box->info->option == NULL || box->bone == NULL ||
				box->bone->IsInvisible())
				goto reloadClubList;

			found = strstr(box->info->option, clubTag);

			if (found)
			{
				do
				{
					char name[64];
					effectStorage.cur =
						GetCharsTillSpace(found + strlen(clubTag), name);

					int len = strlen(name);

					if (len == 0 || len >= 64)
						continue;

					char* ext = strrchr(name, '.');
					if (ext == NULL || strlen(ext) < 4)
						continue;

					attacher.mat[0] = &box->bone->GetMatrix();
					attacher.pos[0] = &box->info->spherePivot;

					if (stricmp(ext + 1, "spr") == 0)
					{
						listStorage.spray = (CFxSpray*)CFx::Instance()->Attach(
							attacher, name, false, NULL);
						m_spray.push_back(listStorage.spray);
					}
					else if (stricmp(ext + 1, "seq") == 0)
					{
						listStorage.sequence =
							(CFxSequence*)CFx::Instance()->Attach(attacher,
								name, false, NULL);
						m_sequence.push_back(listStorage.sequence);
					}
				} while ((found = strstr(effectStorage.cur, clubTag)) != NULL);
			}
			else
			{
				goto reloadClubList;
			}
			nextList = cursor.clubList;
			// HACK!
			goto nextClubBox;
reloadClubList:
			nextList = *(WList<w_bound_box*>* volatile*)&cursor.clubList;
nextClubBox:
			box = nextList->Next();
		}
	}
}

void CGolfPlayer::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.message)
	{
	case 417:
		if (!m_bOldTurn)
		{
			if (GolfClub().PowerShotType() && GolfClub().GetType() != 3)
			{
				if (GolfClub().PowerShotType() == 1 ||
					GolfClub().GetType() != 0)
					SetMotion(
						MakeStr("%s\xb0\xd4\xb0\xc9\xc0\xbd\xc6\xc4\xbf\xf6",
							m_swingType),
						0.0f, -1);
				else
					SetMotion(
						MakeStr("%s\xb0\xd4\xb0\xc9\xc0\xbd\xc6\xc4\xbf\xf6"
								"2",
							m_swingType),
						0.0f, -1);
			}
			else
			{
				float speed = 0.0f;
				SendMsg(this, "Screen", 426, (int)&speed, 0, 0, 0);
				m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 0;
				SetMotion(MakeStr("%s\xb0\xd4\xb0\xc9\xc0\xbd", m_swingType),
					0.0f, -1);
			}
		}

		if (IsLocalContent(S3_SAFETEE_ITEM))
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			{
				WVector pos = GetPVS().GetGroundPoint(
					m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot, false,
					NULL, false);
				if (m_safeTeeSeq)
					m_safeTeeSeq->m_pos = pos;
			}
		}

		m_bTurn = true;
		{
			float angle = *(float*)msg.param1;
			m_mat = RotMat(m_dir = angle + m_dir, 1);
		}
		SearchGround();

		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x40;

		if (IsMyTurn(false))
		{
			if (m_bBlocked)
			{
				float zero = 0.0f;
				SendMsg(this, "Letter", 397, (int)"NoPlay", (int)&zero, 1, 0);
				SendMsg(this, "Caddie", 481, 1, 0, 0, 0);
			}
			else
			{
				SendMsg(this, "Letter", 396, (int)"NoPlay", 0, 0, 0);
				SendMsg(this, "Caddie", 481, 0, 0, 0, 0);
			}
		}
		break;

	case 276:
		m_bAlpha = msg.param1 == 1;
		break;

	case 272:
		*(float*)msg.param1 = m_dir - g_PI;
		break;

	case 10005:
		*(const char**)msg.param1 =
			m_petInfo[GOLFDOC()->m_currentPlayer].motion->name;
		break;

	case 10006:
		m_bSkipScript = msg.param3 == 1;
		SetMotion((const char*)msg.param1, (float)msg.param2, -1);
		m_bSkipScript = false;
		break;

	case 270:
		if (!m_bMotionEnd)
		{
			if (m_petInfo[GOLFDOC()->m_currentPlayer].powerShot == 2)
				break;

			m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 2;
			m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion[0] = 0;
			SetMotion(m_motionName, 0.0f, -1);

			if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) &&
				GOLFDOC()->m_bar.grade < 2)
				break;

			WVector delta;
			delta = GolfBall().PreData(GolfBall().m_renderNum)->pos -
				GetHoleCupPos();
			delta.y = 0.0f;
			float endDist = delta.Magnitude();

			delta = GolfBall().PreData(0)->pos - GetHoleCupPos();
			delta.y = 0.0f;
			float startDist = delta.Magnitude();

			switch (GolfClub().GetType())
			{
			case 0:
				if (!GolfBall().m_bOB && endDist > 416.0f)
					SendMsg(this, "Player", 291, 0, 0, 0, 0);
				break;

			case 3:
				if ((GetGameType() == 7 && GOLFDOC()->m_skinsState == 2) ||
					(GOLFDOC()->m_skinsCarryNum &&
						GOLFDOC()->m_skinsState == 3))
				{
					if (startDist > 32.0f && endDist < 3.2f)
					{
						SendMsg(this, "GolfRule", 154, 0, 0, 0, 0);
						strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
							"\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2");
					}
				}
				else
				{
					if (startDist > 32.0f)
					{
						int chance;
						if (startDist > 96.0f)
							chance = rand() % 4;
						else if (startDist > 64.0f)
							chance = rand() % 3;
						else
							chance = rand() % 2;

						if (endDist < 3.2f && chance >= 1)
						{
							SendMsg(this, "GolfRule", 154, 0, 0, 0, 0);
							strcpy(m_petInfo[GOLFDOC()->m_currentPlayer]
									   .nextMotion,
								"\xc6\xdb\xc6\xc3\xc8\xc4\xb4\xeb\xb1\xe2");
						}
					}
				}
				break;
			}
		}
		break;

	case 271:
		SetSwingType();
		break;

	case 10008:
		m_bTurn = false;
		m_bOldTurn = false;
		{
			float angle = *(float*)msg.param1;
			m_mat = RotMat(m_dir = angle + g_PI, 1);
		}
		SearchGround();
		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x248;
		break;

	case 10009:
		m_pos = *(WVector*)msg.param1;
		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x249;
		break;

	case 295:
		if (msg.param2 == 1)
			*(WVector*)msg.param1 =
				m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot;
		else
			*(WVector*)msg.param1 = m_pos;
		break;

	case 285:
	{
		SetMotion(MakeStr("%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1", m_swingType),
			0.0f, -1);
		m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 4;
		GolfClub().SetPowerShot(1);
		SendMsg(this, "Spot", 245, 0, 0, 0, 0);

		m_bPowerShot = true;
		m_powerShotState = 0;
		m_bForcePowerShot = true;
		float speed = 1.0f;
		SendMsg(this, "Screen", 426, (int)&speed, 0, 0, 0);
		break;
	}

	case 286:
		if (IsLocalContent(S3_STRENGTH))
		{
			SetMotion(MakeStr("%s\xc6\xc4\xbf\xf6"
							  "2\xc1\xd8\xba\xf1",
						  m_swingType),
				0.0f, -1);
			m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 4;
			GolfClub().SetPowerShot(3);
			SendMsg(this, "Spot", 245, 0, 0, 0, 0);

			m_bPowerShot = true;
			m_powerShotState = 0;
			m_bForcePowerShot = true;
			float speed = 1.0f;
			SendMsg(this, "Screen", 426, (int)&speed, 0, 0, 0);
		}
		break;

	case 292:
		*(WMatrix*)msg.param2 = m_petFrame[msg.param1]
									.GetBonePet()
									->FindBone("Bip01 Head")
									->GetMatrix();
		break;

	case 226:
	{
		for (unsigned int i = 0; (int)i < GOLFDOC()->GetPlayerNum(); i++)
		{
			const char* name = MakeStr("PLAYER_%d", i);

			if (PLAYER(i)->state == 3)
			{
				if (m_petInfo[i].bVisible)
				{
					m_petInfo[i].bVisible = false;
					m_petFrame[i].GetBonePet()->HideBone(NULL);
					CSceneManager::Instance()->SetVisible(name, false);
				}
			}
			else
			{
				m_petInfo[i].bVisible = i == GOLFDOC()->m_currentPlayer;

				if (m_petInfo[i].bVisible)
					m_petFrame[i].GetBonePet()->ShowBone(NULL);
				else
					m_petFrame[i].GetBonePet()->HideBone(NULL);

				CSceneManager::Instance()->SetVisible(name,
					m_petInfo[i].bVisible);

				if (IsCurMapSnow())
				{
					Attacher attacher;

					w_bound_box* box =
						m_petFrame[i].GetBonePet()->FindBoundBox("Breathe");

					if (box)
					{
						WBone* bone = box->bone;

						attacher.mat[0] = &bone->GetMatrix();
						attacher.pos[0] = &box->info->spherePivot;
						CFx::Instance()->Attach(attacher, "breath.seq", true,
							NULL);
					}
				}
			}

			if (m_petInfo[i].bVisible &&
				!Doc()->m_userInfo[i].info.angelicWings &&
				Doc()->m_userInfo[i].info.angelicWingsEffect)
			{
				Attacher attacher;

				WBone* bone =
					m_petFrame[i].GetBonePet()->FindBone("Bip01 R Finger1");

				if (bone)
				{
					attacher.mat[0] = &bone->GetMatrix();
					CFx::Instance()->Attach(attacher, "High-Rank.seq", true,
						NULL);
				}

				bone = m_petFrame[i].GetBonePet()->FindBone("Bip01 L Finger1");

				if (bone)
				{
					attacher.mat[0] = &bone->GetMatrix();
					CFx::Instance()->Attach(attacher, "High-Rank.seq", true,
						NULL);
				}
			}
		}

		m_bClubMat = false;
		m_bAlpha = false;
		m_bTail = false;
		m_bMotionEnd = false;
		CSceneManager::Instance()->SetVisible("tail", false);

		SetSwingType();

		m_pos = GolfBall().m_pos - WVector(0.0f, GolfBall().m_radius, 0.0f);

		Waabb box;
		box.min = GolfBall().m_pos - WVector(16.0f, 0.0f, 16.0f);
		box.max = GolfBall().m_pos + WVector(16.0f, 0.0f, 16.0f);
		GetPVS().GetObjList(box);

		m_bTurn = false;
		m_bOldTurn = false;
		float angle = GOLFDOC()->m_cameraData.angle;
		m_mat = RotMat(m_dir = angle + g_PI, 1);
		SearchGround();

		m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x40;
		m_petInfo[GOLFDOC()->m_currentPlayer].flags &= ~0x180;

		m_clubNear =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBoundBox(
				"club_near");
		m_clubFar =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBoundBox(
				"club_far");

		m_tail->Reset();

		m_footBone[0] =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBoneByTail(
				"L Foot");
		m_footBone[1] =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBoneByTail(
				"R Foot");

		m_headAngle = 0.0f;
		m_headBone =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBone(
				"Bip01 Head");

		m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion[0] = 0;

		WVector delta =
			GetHoleCupPos() - PLAYER(GOLFDOC()->m_currentPlayer)->pos;
		delta.y = 0.0f;
		m_cupDist = delta.Magnitude();

		sClubSet* clubSet = GolfClub().FindClubSet(
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid);

		m_clubPuppet = clubSet->puppet[GolfClub().GetType()];
		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);

		ParseBoundingBoxScript(GOLFDOC()->m_currentPlayer, true, false, true);

		m_bPowerShot = false;
		m_bForcePowerShot = false;
		break;
	}

	case 10007:
		if ((const char*)msg.param1)
			strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
				(const char*)msg.param1);
		else
			strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
				MakeStr("%s\xc1\xd8\xba\xf1", m_swingType));
		break;

	case 273:
		*(WBone**)msg.param1 =
			m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBone(
				(const char*)msg.param2);
		break;

	case 275:
		*(float*)msg.param1 = Between(-0.34906584f, m_footAngle, 0.34906584f);
		break;

	case 287:
		if (msg.param1)
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 4;

			float speed = 1.0f;
			GolfClub().SetPowerShot(msg.param1);
			SendMsg(this, "Spot", 245, 0, 0, 0, 0);
			SendMsg(this, "Screen", 426, (int)&speed, 0, 0, 0);

			char* motion;
			if (GolfClub().PowerShotType() == 1)
				motion = MakeStr("%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1\xb3\xa1",
					m_swingType);
			else
				motion = MakeStr("%s\xc6\xc4\xbf\xf6"
								 "2\xc1\xd8\xba\xf1\xb3\xa1",
					m_swingType);

			SetMotion(motion, 0.0f, -1);
		}
		else
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 0;

			float speed = 0.0f;
			GolfClub().SetPowerShot(0);
			SendMsg(this, "Spot", 245, 0, 0, 0, 0);
			SendMsg(this, "Screen", 426, (int)&speed, 0, 0, 0);

			SetMotion(MakeStr("%s\xc1\xd8\xba\xf1", m_swingType), 0.0f, -1);
		}
		break;

	case 277:
		GolfBall().m_standGround = HasStandGround(*(WVector*)msg.param1);
		break;

	case 294:
		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) &&
			GOLFDOC()->m_bar.grade < 2)
		{
			if (GOLFDOC()->m_bar.impact > GOLFDOC()->m_bar.center)
				strcpy(m_motionName,
					MakeStr("%s%s\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
						m_swingType,
						GolfClub().PowerShotType() ? "\xc6\xc4\xbf\xf6" : ""));
			else
				strcpy(m_motionName,
					MakeStr("%s%s\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
						m_swingType,
						GolfClub().PowerShotType() ? "\xc6\xc4\xbf\xf6" : ""));
		}
		else if (GolfClub().PowerShotType())
		{
			strcpy(m_motionName, MakeStr("%s\xc6\xc4\xbf\xf6", m_swingType));
		}
		else if (GolfClub().GetType() == 3 && m_cupDist < 3.2f &&
			Abs(CalcDeltaAngle(GolfBall().m_pos - GetHoleCupPos(),
				m_petInfo[GOLFDOC()->m_currentPlayer].mat.za)) < 0.5235988f)
		{
			strcpy(m_motionName, "\xc2\xaa\xc0\xba\xc6\xdb\xc6\xc3");
		}
		else
		{
			strcpy(m_motionName, m_swingType);
		}

		for (unsigned int i = 0; i < 16; i++)
		{
			if (strcmp(s_shotName[i], m_motionName) == 0)
			{
				*(float*)msg.param1 =
					m_petInfo[GOLFDOC()->m_currentPlayer].shotTime[i];
				break;
			}
		}
		break;

	case 119:
	{
		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) &&
			GOLFDOC()->m_bar.grade < 2)
		{
			if (GOLFDOC()->m_bar.impact > GOLFDOC()->m_bar.center)
				strcpy(m_motionName,
					MakeStr("%s%s\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae",
						m_swingType,
						GolfClub().PowerShotType() ? "\xc6\xc4\xbf\xf6" : ""));
			else
				strcpy(m_motionName,
					MakeStr("%s%s\xb4\xca\xc0\xba\xc7\xea\xbd\xba\xc0\xae",
						m_swingType,
						GolfClub().PowerShotType() ? "\xc6\xc4\xbf\xf6" : ""));
		}
		else if (GolfClub().PowerShotType())
		{
			if (GolfClub().PowerShotType() == 1 || GolfClub().GetType() != 0)
				strcpy(m_motionName,
					MakeStr("%s\xc6\xc4\xbf\xf6", m_swingType));
			else
				strcpy(m_motionName,
					MakeStr("%s\xc6\xc4\xbf\xf6"
							"2",
						m_swingType));
		}
		else if (GolfClub().GetType() == 3 && m_cupDist < 3.2f &&
			Abs(CalcDeltaAngle(GolfBall().m_pos - GetHoleCupPos(),
				m_petInfo[GOLFDOC()->m_currentPlayer].mat.za)) < 0.5235988f)
		{
			strcpy(m_motionName, "\xc2\xaa\xc0\xba\xc6\xdb\xc6\xc3");
		}
		else
		{
			strcpy(m_motionName, m_swingType);
		}

		char* motion;
		if (GolfClub().PowerShotType())
		{
			if (GolfClub().PowerShotType() == 1)
				motion = MakeStr("%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1\xb3\xa1",
					m_swingType);
			else
				motion = MakeStr("%s\xc6\xc4\xbf\xf6"
								 "2\xc1\xd8\xba\xf1\xb3\xa1",
					m_swingType);
		}
		else
		{
			motion = MakeStr("%s\xc1\xd8\xba\xf1", m_swingType);
		}

		w_motion_data* data =
			m_petFrame[GOLFDOC()->m_currentPlayer].FindMotionData(motion, true,
				NULL);

		if (data->e - data->s > 1.0f)
		{
			m_bMotionEnd = true;
			SetMotion(data->name, data->e - data->s - 1.0f, -1);
			strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
				m_motionName);
		}
		else
		{
			m_bMotionEnd = false;
		}
		break;
	}

	case 279:
	{
		unsigned long tid = msg.param1;
		if ((tid & 0xfc000000) == 0x70000000)
		{
			Doc()
				->m_userInfo[GOLFDOC()->m_currentPlayer]
				.charInfo.tidAuxParts[(tid >> 21) & 0x1f] =
				Doc()->m_userInfo[GOLFDOC()->m_currentPlayer]
						.charInfo.tidAuxParts[(tid >> 21) & 0x1f] != tid
				? tid
				: 0;

			GOLFDOC()->SetPlayerLevel(GOLFDOC()->m_currentPlayer);
			SendMsg(this, "Spot", 245, 0, 0, 0, 0);
			ParseBoundingBoxScript(GOLFDOC()->m_currentPlayer, true, false,
				true);
		}
		else
		{
			CPartTidList& tids =
				GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);

			if (ItemManager()->FindPart(tid))
			{
				unsigned long changed =
					m_petFrame[GOLFDOC()->m_currentPlayer].SetPart(&tids, tid,
						0);

				if (ShowAbilityParts(&tids))
					m_petFrame[GOLFDOC()->m_currentPlayer].Build(&tids,
						Doc()
							->m_userInfo[GOLFDOC()->m_currentPlayer]
							.charInfo.tidAuxParts,
						true, true, GOLFDOC()->m_currentPlayer);

				m_petFrame[GOLFDOC()->m_currentPlayer]
					.GetBonePet()
					->xUpdateTnLBuffers(true);

				if (changed)
				{
					m_petFrame[GOLFDOC()->m_currentPlayer].CalcBVolume();
					m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x13;
				}

				ParseBoundingBoxScript(GOLFDOC()->m_currentPlayer, true, false,
					true);
			}
		}
		break;
	}

	case 355:
		m_bDisplay = true;
		m_petInfo[GOLFDOC()->m_currentPlayer].bVisible = true;
		m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->ShowBone(NULL);
		CSceneManager::Instance()->SetVisible(
			MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer), true);
		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
		break;

	case 356:
		m_bDisplay = false;
		m_petInfo[GOLFDOC()->m_currentPlayer].bVisible = false;
		m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->HideBone(NULL);
		CSceneManager::Instance()->SetVisible(
			MakeStr("PLAYER_%d", GOLFDOC()->m_currentPlayer), false);
		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", false);

		for (unsigned int i = 0; i < m_spray.size(); i++)
			CFx::Instance()->CloseSpray(m_spray[i]);
		for (unsigned int j = 0; j < m_sequence.size(); j++)
			CFx::Instance()->CloseSequence(m_sequence[j], true);

		m_spray.clear();
		m_sequence.clear();
		break;

	case 280:
		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (PLAYER(i)->state != 3)
				m_petFrame[GOLFDOC()->m_currentPlayer]
					.GetBonePet()
					->SetRenderMode(msg.param1);
		}
		break;

	case 281:
	{
		std::string motion;
		char charName[16];

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			if (GetGameType() != 1 && PLAYER(i)->state == 3)
				continue;

			const char* pointName;
			if (GetGameType() == 1)
			{
				if (PLAYER(i)->teammate != 0xff)
				{
					if (GOLFDOC()->GetTeamGameWinner() == 2)
					{
						if (PLAYER(i)->team == 0)
							pointName = i < PLAYER(i)->teammate
								? "\xc4\xbf\xc7\xc3_1\xb5\xee"
								: "\xc4\xbf\xc7\xc3_2\xb5\xee";
						else
							pointName = i < PLAYER(i)->teammate
								? "\xc4\xbf\xc7\xc3_3\xb5\xee"
								: "\xc4\xbf\xc7\xc3_4\xb5\xee";
					}
					else if (PLAYER(i)->team ==
						(unsigned char)GOLFDOC()->GetTeamGameWinner())
					{
						pointName = i < PLAYER(i)->teammate
							? "\xc4\xbf\xc7\xc3_1\xb5\xee"
							: "\xc4\xbf\xc7\xc3_2\xb5\xee";
					}
					else
					{
						pointName = i < PLAYER(i)->teammate
							? "\xc4\xbf\xc7\xc3_3\xb5\xee"
							: "\xc4\xbf\xc7\xc3_4\xb5\xee";
					}
				}
				else if (GOLFDOC()->GetTeamGameWinner() == 2)
				{
					if (PLAYER(i)->team == 0)
						pointName = "\xc4\xbf\xc7\xc3_1\xb5\xee";
					else
						pointName = "\xc4\xbf\xc7\xc3_3\xb5\xee";
				}
				else
				{
					if (PLAYER(i)->team ==
						(unsigned char)GOLFDOC()->GetTeamGameWinner())
						pointName = "\xc4\xbf\xc7\xc3_1\xb5\xee";
					else
						pointName = "\xc4\xbf\xc7\xc3_3\xb5\xee";
				}
			}
			else
			{
				pointName = MakeStr("%d\xc0\xce_%d\xb5\xee",
					GOLFDOC()->GetCurrentPlayerNum(), PLAYER(i)->order);
			}

			WPolySoup::sPoint* point =
				GOLFDOC()->m_pPolySoup->FindPoint(pointName);

			m_petFrame[i].GetBonePet()->ShowBone(NULL);
			m_petFrame[i].GetBonePet()->HideBone("Bone01");
			m_petFrame[i].SetAlpha(0xff);
			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i),
				true);

			m_petInfo[i].mat.Reset();
			m_petInfo[i].mat.pivot =
				GetPVS().GetGroundPoint(point->pos, false, NULL, false);

			m_petInfo[i].powerShot = 0;
			m_petInfo[i].nextMotion[0] = 0;

			if (GetGameType() != 1)
			{
				if (PLAYER(i)->rank == 1)
					motion = "1\xb5\xee\xb8\xf0\xbc\xc7";
				else if (PLAYER(i)->rank == GOLFDOC()->GetPlayerNum())
					motion = "3\xb5\xee\xb8\xf0\xbc\xc7";
				else if (PLAYER(i)->rank == 2)
					motion = "2\xb5\xee\xb8\xf0\xbc\xc7";
				else
					motion = "3\xb5\xee\xb8\xf0\xbc\xc7";
			}
			else
			{
				if (PLAYER(i)->team ==
						(unsigned char)GOLFDOC()->GetTeamGameWinner() ||
					GOLFDOC()->GetTeamGameWinner() == 2)
					motion = "1\xb5\xee\xb8\xf0\xbc\xc7";
				else
					motion = "3\xb5\xee\xb8\xf0\xbc\xc7";
			}

			m_petInfo[i].motion =
				m_petFrame[i].FindMotionData(motion.c_str(), true, NULL);
			m_petInfo[i].time = m_petInfo[i].motion->s;
			m_petInfo[i].bVisible = true;

			m_petFrame[i].GetBonePet()->ApplyBones(
				m_petFrame[i].GetCurAniPet()->GetBones(NULL, m_petInfo[i].time,
					0, NULL, false),
				m_petInfo[i].mat);

			m_lightSet.nearOne = g_lightset.nearOne;
			m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
			SetLight();

			sprintf(charName, "char%d", i);

			m_petFrame[i].SetInvisible(false);

			ScriptProcess(m_petInfo[i].motion, m_petInfo[i].motion->s,
				m_petInfo[i].motion->s, i, true, false);
		}

		CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4", NULL);
	}
	break;

	case 284:
	{
		unsigned long tid = msg.param1;
		sClubSet* clubSet = GolfClub().AddClubSet(tid);

		if (clubSet)
		{
			Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid = tid;
			m_clubPuppet = clubSet->puppet[GolfClub().GetType()];

			if (msg.param2)
				RegisterClub(0);
			CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4",
				m_clubPuppet);

			m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x269;

			m_ballDist =
				m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
					.ballPos.x;
		}
		break;
	}

	case 73:
	{
		CPartTidList& tids = GOLFDOC()->GetPartTidList(0);
		tids.SetTids(&Doc()->m_userInfo[0].charInfo,
			Doc()->m_userInfo[0].stat.Level);
		if (!m_petFrame->Build(&tids, Doc()->m_userInfo[0].charInfo.tidAuxParts,
				true, true, -1))
		{
			tids.SetDefaultTids();
			m_petFrame->Build(&tids, Doc()->m_userInfo[0].charInfo.tidAuxParts,
				true, false, -1);
		}
		m_petFrame->CalcBVolume();
		CSceneManager::Instance()->SetPuppet("PLAYER_0",
			m_petFrame->GetBonePet());
		RegisterClub(0);
		break;
	}

	case 409:
		*(int*)msg.param1 = m_petInfo[GOLFDOC()->m_currentPlayer].powerShot;
		break;

	case 289:
	{
		m_petFrame[MyIndex()].GetBonePet()->ShowBone(NULL);
		m_petFrame[MyIndex()].GetBonePet()->HideBone("Bone01");
		m_petFrame[MyIndex()].SetAlpha(0xff);

		m_petInfo[MyIndex()].mat.Reset();
		m_petInfo[MyIndex()].mat.pivot =
			GetPVS().GetGroundPoint(*(WVector*)msg.param1, false, NULL, false);

		m_petInfo[MyIndex()].powerShot = 0;
		m_petInfo[MyIndex()].nextMotion[0] = 0;

		m_petInfo[MyIndex()].motion = m_petFrame[MyIndex()].FindMotionData(
			(const char*)msg.param2, true, NULL);

		if (m_petInfo[MyIndex()].motion)
			m_petInfo[MyIndex()].time = m_petInfo[MyIndex()].motion->s;
		else
			m_petInfo[MyIndex()].time = 0.0f;

		m_petInfo[MyIndex()].bVisible = true;

		m_petFrame[MyIndex()].GetBonePet()->ApplyBones(
			m_petFrame[MyIndex()].GetCurAniPet()->GetBones(NULL,
				m_petInfo[MyIndex()].time, 0, NULL, false),
			m_petInfo[MyIndex()].mat);

		m_petInfo[MyIndex()].flags |= 0x48;

		CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", MyIndex()),
			true);

		sClubSet* clubSet =
			GolfClub().FindClubSet(Doc()->m_userInfo[MyIndex()].clubInfo.tid);

		WPuppet* club = clubSet->puppet[GolfClub().GetType()];

		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
		CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4", club);

		m_clubPuppet = club;
		m_petFrame[MyIndex()].SetInvisible(false);

		ScriptProcess(m_petInfo[MyIndex()].motion,
			m_petInfo[MyIndex()].motion->s, m_petInfo[MyIndex()].motion->s,
			MyIndex(), true, false);

		m_petInfo[1 - MyIndex()].bVisible = false;
		CSceneManager::Instance()->SetVisible(
			MakeStr("PLAYER_%d", 1 - MyIndex()), false);

		m_lightSet.nearOne = g_lightset.nearOne;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);
		break;
	}

	case 290:
	{
		MyIndex();
		bool bShowClub = msg.param1 != 0;

		m_petFrame[MyIndex()].GetBonePet()->ShowBone(NULL);
		m_petFrame[MyIndex()].GetBonePet()->HideBone("Bone01");
		m_petFrame[MyIndex()].SetAlpha(0xff);

		m_petInfo[MyIndex()].mat.Reset();
		m_petInfo[MyIndex()].mat.pivot =
			GetPVS().GetGroundPoint(*(WVector*)msg.param1, false, NULL, false);

		m_petInfo[MyIndex()].powerShot = 0;
		m_petInfo[MyIndex()].nextMotion[0] = 0;

		m_petInfo[MyIndex()].motion = m_petFrame[MyIndex()].FindMotionData(
			(const char*)msg.param2, true, NULL);

		if (m_petInfo[MyIndex()].motion)
			m_petInfo[MyIndex()].time = m_petInfo[MyIndex()].motion->s;
		else
			m_petInfo[MyIndex()].time = 0.0f;

		m_petInfo[MyIndex()].bVisible = true;

		m_petFrame[MyIndex()].GetBonePet()->ApplyBones(
			m_petFrame[MyIndex()].GetCurAniPet()->GetBones(NULL,
				m_petInfo[MyIndex()].time, 0, NULL, false),
			m_petInfo[MyIndex()].mat);

		m_petInfo[MyIndex()].flags |= 0x48;

		for (int i = 0; i < GOLFDOC()->GetPlayerNum(); i++)
		{
			m_petInfo[i].bVisible = i == MyIndex();
			CSceneManager::Instance()->SetVisible(MakeStr("PLAYER_%d", i),
				i == MyIndex());
		}

		CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", bShowClub);

		if (bShowClub)
		{
			sClubSet* clubSet = GolfClub().FindClubSet(
				Doc()->m_userInfo[MyIndex()].clubInfo.tid);

			if (clubSet)
			{
				WPuppet* club = clubSet->puppet[GolfClub().GetType()];

				if (club)
				{
					CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4",
						club);
					m_clubPuppet = club;
				}
				else
				{
					m_clubPuppet = NULL;
				}
			}
			else
			{
				m_clubPuppet = NULL;
			}
		}
		else
		{
			m_clubPuppet = NULL;
		}

		m_petFrame[MyIndex()].SetInvisible(false);

		ScriptProcess(m_petInfo[MyIndex()].motion,
			m_petInfo[MyIndex()].motion->s, m_petInfo[MyIndex()].motion->s,
			MyIndex(), true, false);

		m_lightSet.nearOne = g_lightset.nearOne;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);
		break;
	}

	case 288:
	{
		const char* motion[2] = { (const char*)msg.param1,
			(const char*)msg.param2 };
		WVector pos;
		pos = *(WVector*)msg.param3;

		for (int i = 0; i < 2; i++)
		{
			int player;

			if (PLAYER(MyIndex())->teammate != 0xff)
			{
				player = i ? PLAYER(MyIndex())->teammate : MyIndex();
			}
			else
			{
				if (i == 1)
					break;

				player = MyIndex();
			}

			CPetFrame& frame = m_petFrame[player];
			sPetInfo& info = m_petInfo[player];

			frame.GetBonePet()->ShowBone(NULL);
			frame.GetBonePet()->HideBone("Bone01");
			frame.SetAlpha(0xff);

			info.mat.Reset();
			info.mat.pivot = GetPVS().GetGroundPoint(
				WVector((i == 0 ? -3.0f : 3.0f) + pos.x, pos.y, pos.z), false,
				NULL, false);

			info.powerShot = 0;
			info.nextMotion[0] = 0;
			info.motion = frame.FindMotionData(motion[i], true, NULL);

			if (info.motion)
				info.time = info.motion->s;
			else
				info.time = 0.0f;

			info.bVisible = true;
			frame.GetBonePet()->ApplyBones(
				frame.GetCurAniPet()->GetBones(NULL, info.time, 0, NULL, false),
				info.mat);

			m_petInfo[player].flags |= 0x48;

			const char* name = MakeStr("PLAYER_%d", player);
			CSceneManager::Instance()->SetVisible(name, true);

			if (player == TEAM(PLAYER(MyIndex())->team)->shooter)
			{
				sClubSet* clubSet = GolfClub().FindClubSet(
					Doc()->m_userInfo[player].clubInfo.tid);

				if (!clubSet)
					continue;

				WPuppet* club;

				if (i == GOLFDOC()->m_currentPlayer)
					club = clubSet->puppet[GolfClub().GetType()];
				else
					club = clubSet->puppet[TEAM(PLAYER(MyIndex())->team)->club];

				if (!club)
					continue;

				CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
				CSceneManager::Instance()->SetPuppet("\xc5\xac\xb7\xb4", club);

				m_clubPuppet = club;
			}

			m_petFrame[player].SetInvisible(false);

			ScriptProcess(m_petInfo[player].motion, m_petInfo[player].motion->s,
				m_petInfo[player].motion->s, player, true, false);
		}

		for (int j = 0; j < GOLFDOC()->GetPlayerNum(); j++)
		{
			if (j != MyIndex())
			{
				if ((PLAYER(MyIndex())->teammate != 0xff &&
						j != PLAYER(MyIndex())->teammate) ||
					PLAYER(MyIndex())->teammate == 0xff)
				{
					m_petInfo[j].bVisible = false;
					CSceneManager::Instance()->SetVisible(
						MakeStr("PLAYER_%d", j), false);
				}
			}
		}

		m_lightSet.nearOne = g_lightset.nearOne;
		m_lightSet.ambient = ModifyColor(g_lightset.ambient, 1.0f);
		SetLight(MyIndex());

		ParseBoundingBoxScript(MyIndex(), true, false, true);

		if (PLAYER(MyIndex())->teammate != 0xff)
			ParseBoundingBoxScript(PLAYER(MyIndex())->teammate, false, false,
				false);
		break;
	}

	case 291:
	{
		CPartTidList& tids =
			GOLFDOC()->GetPartTidList(GOLFDOC()->m_currentPlayer);
		if (GolfClub().PowerShotType())
		{
			if ((tids.m_charTid & 0x3ffffff) == 4)
			{
				if (GolfClub().PowerShotType() == 1)
					strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
						"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6\xc8\xc4\xb8\xf0\xbc\xc7");
				else
					strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
						"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6"
						"2\xc8\xc4\xb8\xf0\xbc\xc7");
			}
			else
			{
				if (GolfClub().PowerShotType() == 1)
					strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
						"\xbf\xec\xb5\xe5\xbc\xa6\xc8\xc4\xb8\xf0\xbc\xc7");
				else
					strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
						"\xbf\xec\xb5\xe5\xbc\xa6\xc6\xc4\xbf\xf6"
						"2\xc8\xc4\xb8\xf0\xbc\xc7");
			}
		}
		else
		{
			strcpy(m_petInfo[GOLFDOC()->m_currentPlayer].nextMotion,
				"\xbf\xec\xb5\xe5\xbc\xa6\xc8\xc4\xb8\xf0\xbc\xc7");
		}
		break;
	}

	case 293:
		if (msg.param1 == 0)
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0x80;
			CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", false);
		}
		else
		{
			m_petInfo[GOLFDOC()->m_currentPlayer].flags &= ~0x80;
			CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
		}
		break;

	case 296:
		if (m_petInfo && m_petInfo[GOLFDOC()->m_currentPlayer].motion &&
			(strstr(m_petInfo[GOLFDOC()->m_currentPlayer].motion->name,
				 "\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee") ||
				strstr(m_petInfo[GOLFDOC()->m_currentPlayer].motion->name,
					"\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee") ||
				strstr(m_petInfo[GOLFDOC()->m_currentPlayer].motion->name,
					"\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7") ||
				strstr(m_petInfo[GOLFDOC()->m_currentPlayer].motion->name,
					"\xb5\xee\xb8\xf0\xbc\xc7")))
			*(WVector*)msg.param1 = WVector::ZERO;
		else
			*(WVector*)msg.param1 =
				m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
					.ballPos;
		break;

	case 297:
		*(int*)msg.param1 = m_bCombo;
		*(float*)msg.param2 = m_comboTime;
		break;

	case 504:
	{
		int player = msg.param1;
		if (player < GOLFDOC()->GetPlayerNum())
		{
			CFxSequence* seq =
				CFx::Instance()->OpenSequence("Talk_Room.seq", true);
			if (seq)
			{
				const WVector& pivot = m_petInfo[player].mat.pivot;
				seq->m_pos = WVector(pivot.x, pivot.y + 5.0f, pivot.z);
			}
		}
		break;
	}

	case 343:
		if (msg.param1 != -1)
			PLAYER(0)->power = msg.param1;
		if (msg.param2 != -1)
			PLAYER(0)->control = msg.param2;
		if (msg.param3 != -1)
			PLAYER(0)->curve = msg.param3;
		break;

	case 515:
		if (IsLocalContent(S3_SAFETEE_ITEM))
		{
			if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000028)
			{
				if (msg.param1)
				{
					m_safeTeeSeq = CFx::Instance()->OpenSequence(
						"safetee_Avatar.seq", true);

					WVector pos;
					pos = GetPVS().GetGroundPoint(
						m_petInfo[GOLFDOC()->m_currentPlayer].mat.pivot, false,
						NULL, false);
					if (m_safeTeeSeq)
						m_safeTeeSeq->m_pos = pos;
				}
				else
				{
					CFx::Instance()->CloseSequence(m_safeTeeSeq, false);
				}
			}
			else
			{
				CFx::Instance()->CloseSequence(m_safeTeeSeq, false);
			}
		}
		break;
	}
}

void CGolfPlayer::SetMotion(const char* name, float time, int player)
{
	if (player < 0)
		player = GOLFDOC()->m_currentPlayer;

	if (IsLocalContent(S4_RANDOM_CHAR_MOTION))
	{
		int count =
			m_petFrame[player].GetMotionCountIncludeName(name, s_shotName[16]);
		if (count > 1)
		{
			char motionName[128] = "";
			sprintf(motionName, "%s%02d", name, rand() % count + 1);

			m_petInfo[player].motion =
				m_petFrame[player].FindMotionData(motionName, true, NULL);

			if (!m_petInfo[player].motion)
				m_petInfo[player].motion =
					m_petFrame[player].FindMotionData(name, true, NULL);
		}
		else
		{
			m_petInfo[player].motion =
				m_petFrame[player].FindMotionData(name, true, NULL);
		}
	}
	else
	{
		m_petInfo[player].motion =
			m_petFrame[player].FindMotionData(name, true, NULL);
	}

	if (!m_petInfo[player].motion)
		return;

	m_petInfo[player].alpha = 1.0f;
	m_petInfo[player].alphaVel = 0;

	if (time < 0)
		m_petInfo[player].time = Max(m_petInfo[player].motion->s,
			time + m_petInfo[player].motion->e);
	else
		m_petInfo[player].time = Min(time + m_petInfo[player].motion->s,
			m_petInfo[player].motion->e);

	m_petInfo[player].flags |= 0x33;
	m_bClubMat = (m_petInfo[player].flags >> 8) & 1;
	m_petInfo[player].flags &= ~0x180;

	m_petFrame[player].SetInvisible(false);

	ScriptProcess(m_petInfo[player].motion, m_petInfo[player].motion->s,
		m_petInfo[player].motion->s, player, true, time < 0);

	if ((m_petFrame[player].GetTidChar() & 0x3ffffff) == 6 &&
		!(Wabs(time) < g_EPSILON) &&
		!strcmpi(name, "\xb5\xee\xc0\xe5\xb8\xf0\xbc\xc7"))
	{
		WBone* bone = m_petFrame[player].GetBonePet()->FindBone("G_Bird");
		if (bone)
			bone->SetInvisible(false, true);
	}

	m_animFx[player].clear();
}

void CGolfPlayer::SetSwingType()
{
	switch (GolfClub().GetType())
	{
	case 0:
		strcpy(m_swingType, "\xbf\xec\xb5\xe5\xbc\xa6");
		break;
	case 1:
		strcpy(m_swingType, "\xbe\xc6\xc0\xcc\xbe\xf0\xbc\xa6");
		break;
	case 2:
		strcpy(m_swingType, "\xbb\xf7\xb5\xe5\x9f\x87\xc1\xf6\xbc\xa6");
		break;
	case 3:
		strcpy(m_swingType, "\xc6\xdb\xc6\xc3\xbc\xa6");
		break;
	}

	char* name;
	if (GolfClub().PowerShotType())
	{
		if (GolfClub().PowerShotType() == 1)
			name = MakeStr("%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1\xb3\xa1",
				m_swingType);
		else
			name = MakeStr("%s\xc6\xc4\xbf\xf6"
						   "2\xc1\xd8\xba\xf1\xb3\xa1",
				m_swingType);
	}
	else
	{
		name = MakeStr("%s\xc1\xd8\xba\xf1", m_swingType);
	}

	SetMotion(name, 0, -1);

	m_ballDist =
		m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()].ballPos.x;
	m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 0;
}

void CGolfPlayer::ScriptProcess(w_motion_data* motion, float from, float to,
	int player, bool bAll, bool bSkip)
{
	if (!motion)
		return;

	if ((!(from <= motion->e) || from == to) && !bAll)
		return;

	int first, last;
	from *= 30.0f;
	if (bAll)
	{
		first = Ceil(from, 0.03f);
		last = first;
	}
	else
	{
		const float fps = 30.0f;
		to = Min(motion->e, to) * fps;
		first = Ceil(from, 0.03f);
		last =
			WisEqual(floorf(to), to, g_EPSILON) ? (int)to - 1 : (int)floorf(to);

		if (first == (int)floor((double)(motion->s * fps + 0.5f)))
			first++;
	}

	if (first <= last)
	{
		int start, end;
		if (m_petFrame[player].GetCurAniPet()->CheckFrameData(first, last,
				start, end))
		{
			for (int i = start; i <= end; i++)
			{
				ParsePuppetScript(
					m_petFrame[player].GetCurAniPet()->GetFrameData(i), player,
					bSkip);
			}
		}
	}
}

void CGolfPlayer::CloseBoundingBoxFx(int player, unsigned long typeId)
{
	if (!typeId || player > 4)
		return;

	std::vector<sCondBBSpray>::iterator spray;
	for (spray = m_condBBSpray[player].begin();
		spray != m_condBBSpray[player].end();)
	{
		if ((*spray).typeId == typeId)
		{
			CFx::Instance()->CloseSpray((*spray).spray);
			m_condBBSpray[player].erase(spray);
		}
		else
		{
			spray++;
		}
	}

	std::vector<sCondBBSequence>::iterator sequence;
	for (sequence = m_condBBSequence[player].begin();
		sequence != m_condBBSequence[player].end();)
	{
		if ((*sequence).typeId == typeId)
		{
			CFx::Instance()->CloseSequence((*sequence).sequence, true);
			m_condBBSequence[player].erase(sequence);
		}
		else
		{
			sequence++;
		}
	}
}

void CGolfPlayer::ParsePuppetScript(char* script, int player, bool bSkip)
{
	if (!script)
		return;

	static char* s_command[] = {
		"*snd(",
		"*shot",
		"*tail",
		"*showclub",
		"*hideclub",
		"*swing",
		"*lockclub",
		"*fx(",
		"*ptex(",
		"*showbone(",
		"*hidebone(",
		"*playif(",
		"*opa(",
		"*showvis(",
		"*hidevis(",
	};
	static char s_motion[256];
	static char s_cond[1024];
	static char s_name[64];
	static char s_token[1024];

	const char* p = script;
	int len;
	if (p != script)
		goto inner;
	do
	{
		p = strchr(p, '*');

		if (!p)
			break;

inner:
		len = 0;

		while (p[len])
		{
			if (p[++len] == ' ')
				break;
			if (p[len] == '(')
			{
				len++;
				break;
			}
		}

		if (len <= 1)
			break;

		strncpy(s_token, p, len);
		s_token[len] = '\0';
		p += len;

		for (unsigned int i = 0; i < sizeof(s_command) / sizeof(s_command[0]);
			i++)
		{
			if (strcmpi(s_token, s_command[i]))
				continue;

			switch (i)
			{
			case 0:
			{
				p = GetStrConst(p, s_name);

				unsigned long size;
				if (!(size = strlen(s_name)) || strlen(s_name) >= 64)
					break;

				WVector pos;
				pos = m_pos - g_camera.pivot;
				g_audio->PlaySfx(s_name, &pos, 5, NULL, NULL, 0.5f, 200.0f);
				break;
			}
			case 1:
				PlayShotSfx(player);

				if (GOLFDOC()->GameMode() & 0x40)
					SendMsg(this, "GolfRule", 0x78, 0x80, 0, 0, 0);
				break;
			case 2:
			{
				int j;
				const struct sTailTexture* tail = s_tailTexture;
				for (j = 0; j < 3; j++, tail++)
				{
					if (Doc()
							->m_userInfo[GOLFDOC()->m_currentPlayer]
							.clubInfo.tid == tail->typeId)
					{
						m_tail->ChangeTexture(tail->texture);
						break;
					}
				}

				if (j == 3)
					m_tail->ChangeTexture("~swinglight01.jpg");

				m_bTail = true;
				CSceneManager::Instance()->SetVisible("tail", true);
				break;
			}
			case 3:
				m_petInfo[player].flags &= ~0x80;
				CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4", true);
				break;
			case 4:
				m_petInfo[player].flags |= 0x80;
				CSceneManager::Instance()->SetVisible("\xc5\xac\xb7\xb4",
					false);
				break;
			case 5:
				break;
			case 6:
				if (!m_bClubMat)
				{
					w_bound_box* box =
						m_petFrame[player].GetBonePet()->FindBoundBox("club");
					m_clubMat = box->bone->GetMatrix();
					m_clubMat.pivot =
						box->info->spherePivot * box->bone->GetMatrix();
					if (false)
						break;
					m_clubMat.pivot +=
						m_club[player][GolfClub().GetType()].gripPos *
						m_clubMat.xa;
				}

				m_petInfo[player].flags |= 0x100;
				break;
			case 7:
			{
				if (bSkip)
					break;

				char name[64];
				name[0] = '\0';
				p = GetStrConst(p, name);

				unsigned long size;
				if (!(size = strlen(name)) || strlen(name) >= 64)
					break;

				char file[64];
				if (name[0] == '@')
				{
					if (!m_petInfo[player].motion ||
						strcmp(name + 1, m_petInfo[player].motion->name))
						break;

					p = GetStrConst(p, file);

					unsigned long size;
					if (!(size = strlen(file)) || strlen(file) >= 64)
						break;
				}
				else
				{
					strcpy(file, name);
				}

				if (!stricmp(file, "*playbbfx"))
				{
					ParseBoundingBoxScript(player, true, true, true);
					break;
				}

				char token[64];
				p = GetStrConst(p, token);

				int length = strlen(token);
				if (!length || length >= 64)
					break;

				bool bFound = false;
				sClubSet* clubSet = GolfClub().FindClubSet(
					Doc()->m_userInfo[player].clubInfo.tid);

				bool bAnim = false;
				unsigned long value;

				while (token[0] == '&')
				{
					bAnim = true;
					value = atoi(token + 1);

					if (!bFound)
					{
						if (m_petFrame[player].GetPartList().FindPart(value))
						{
							bFound = true;
						}
						else if (clubSet && clubSet->pClubSet &&
							clubSet->pClubSet->ClubIds[GolfClub().GetType()] ==
								value)
						{
							bFound = true;
						}
					}

					p = GetStrConst(p, token);

					length = strlen(token);
					if (!length || length >= 64)
						break;
				}

				if (bAnim)
				{
					if (!bFound)
						break;

					CloseBoundingBoxFx(player, value);
					m_animFx[player].push_back(value);
				}

				char bone[64];
				strcpy(bone, token);

				if (!(size = strlen(bone)) || strlen(bone) >= 64)
					break;

				Attacher attacher;
				int k;
				for (k = 0; k < sizeof(attacher.mat) / sizeof(attacher.mat[0]);
					k++)
				{
					if (k != 0)
					{
						p = GetStrConst(p, bone);

						unsigned long size;
						if (!(size = strlen(bone)) || strlen(bone) >= 64)
							break;
					}

					WBone* pBone =
						m_petFrame[player].GetBonePet()->FindBone(bone);
					if (pBone)
					{
						attacher.mat[k] = &pBone->GetMatrix();
					}
					else
					{
						w_bound_box* box =
							m_petFrame[player].GetBonePet()->FindBoundBox(bone);
						if (box)
						{
							if (box->bone)
								attacher.mat[k] = &box->bone->GetMatrix();
							attacher.pos[k] = &box->info->spherePivot;
						}
						else if (clubSet &&
							clubSet->puppet[GolfClub().GetType()])
						{
							WBone* clubBone =
								clubSet->puppet[GolfClub().GetType()]->FindBone(
									bone);
							if (clubBone)
							{
								attacher.mat[k] = &clubBone->GetMatrix();
							}
							else
							{
								box = clubSet->puppet[GolfClub().GetType()]
										  ->FindBoundBox(bone);
								if (box)
								{
									if (box->bone)
										attacher.mat[k] =
											&box->bone->GetMatrix();
									attacher.pos[k] = &box->info->spherePivot;
								}
							}
						}
					}
				}

				if (k > 0)
					CFx::Instance()->Attach(attacher, file, true, NULL);
				break;
			}
			case 8:
			{
				char name[64];
				char tex[1024];
				name[0] = '\0';
				tex[0] = '\0';
				p = GetStrConst(p, name);

				unsigned long size;
				if (!(size = strlen(name)) || strlen(name) >= 64)
					break;

				if (name[0] == '@')
				{
					if (!m_petInfo[player].motion ||
						strcmpi(name + 1, m_petInfo[player].motion->name))
						break;

					p = GetStrConst(p, tex);

					unsigned long size;
					if (!(size = strlen(tex)) || strlen(tex) >= 1024)
						break;
				}
				else
				{
					strcpy(tex, p);
				}

				p = GetStrConst(p, tex);

				if (!(size = strlen(tex)) || strlen(tex) >= 1024)
					break;

				SetPendingTexPart(player, tex);
				break;
			}
			case 9:
			case 10:
			{
				char bone[64] = "";
				int k = 0;
				while (k < 64)
				{
					p = GetStrConst(p, bone);

					unsigned long size;
					if (!(size = strlen(bone)) || strlen(bone) >= 64)
						break;

					WBone* pBone =
						m_petFrame[player].GetBonePet()->FindBone(bone);
					if (pBone)
						pBone->SetInvisible(false, i != 9);

					if (!p || *p == ')')
						break;

					k++;
				}
				break;
			}
			case 11:
			{
				int offset = 0;
				s_motion[0] = '\0';
				s_cond[0] = '\0';
				p = GetStrConst(p, s_cond);
				p = GetStrConst(p, s_motion);

				if (!s_cond[0] || !s_motion[0])
					break;

				if (m_bSkipScript)
					break;

				while (true)
				{
					char* semi = strchr(s_cond + offset, ';');
					int n;
					if (semi)
					{
						n = semi - (s_cond + offset);
					}
					else
					{
						n = strlen(s_cond + offset);

						if (!n)
							break;
					}

					char petName[40];
					strncpy(petName, s_cond + offset, n);
					petName[n] = '\0';
					strcat(petName, ".mpet");

					if (m_petFrame[player].HasMPet(petName))
					{
						SetMotion(s_motion, 0, player);
						m_petInfo[player].nextMotion[0] = '\0';
						break;
					}

					if (!semi)
						break;

					offset += n + 1;
				}
				break;
			}
			case 12:
			{
				char name[64];
				char args[64];
				name[0] = '\0';
				args[0] = '\0';
				p = GetStrConst(p, name);

				unsigned long size;
				if (!(size = strlen(name)) || strlen(name) >= 64)
					break;

				if (name[0] == '@')
				{
					if (!m_petInfo[player].motion ||
						strcmpi(name + 1, m_petInfo[player].motion->name))
						break;

					p = GetStrConst(p, args);

					unsigned long size;
					if (!(size = strlen(args)) || strlen(args) >= 64)
						break;
				}
				else
				{
					strcpy(args, name);
				}

				if (!(size = strlen(args)) || strlen(args) >= 64)
					break;

				float from, to;
				float time = 0;
				sscanf(args, "%f %f %f", &from, &to, &time);
				m_petInfo[player].alpha = from;
				time *= 1.0f / 30.0f;

				if (time > 0)
					m_petInfo[player].alphaVel = (to - from) / time;
				else
					m_petInfo[player].alphaVel = 0;
				break;
			}
			case 13:
			case 14:
				ParseScript_ShowVis(player, i == 13, p);
				break;
			}
		}
	} while (p);

	m_bClubMat = false;
}

void CGolfPlayer::ParseScript_ShowVis(int player, bool bShow,
	const char* script)
{
	char name[256] = "";
	int count = 0;

	do
	{
		script = GetStrConst(script, name);

		if (!strlen(name) || strlen(name) >= sizeof(name))
			break;

		std::list<std::string>* group =
			WSingleton<CSharedDoc>::Instance()->GetVisGroup(name);

		if (group)
		{
			for (std::list<std::string>::iterator it = group->begin();
				it != group->end(); ++it)
			{
				WBone* bone =
					m_petFrame[player].GetBonePet()->FindBone((*it).c_str());

				if (bone)
					bone->SetInvisible(false, !bShow);
			}
		}

		if (!script || *script == ')')
			break;
	} while (++count < 64);
}

void CGolfPlayer::PlayShotSfx(int player)
{
	if (player < 0 || player >= GOLFDOC()->GetPlayerNum())
		return;

	if (strstr(m_petInfo[player].motion->name, "\xc7\xea\xbd\xba\xc0\xae"))
	{
		if (GolfClub().GetType() == 0)
		{
			if (!strstr(m_motionName,
					"\xba\xfc\xb8\xa5\xc7\xea\xbd\xba\xc0\xae"))
				g_audio->PlaySfx("swing_vain");
			else
				g_audio->PlaySfx("swing_miss_wood");
		}
		else if (GolfClub().GetType() == 1)
		{
			g_audio->PlaySfx("swing_miss_iron");
		}

		if ((GolfClub().GetType() == 0 || GolfClub().GetType() == 1) &&
			GOLFDOC()->GetBar().grade < 2)
			SendMsg(this, "Gallery", 0x103, 0, 0, 0, 0);
		return;
	}

	int score = Doc()->m_golfGame.gameType == 1
		? TEAM(PLAYER(player)->team)->stroke[GOLFDOC()->m_currentHole - 1]
		: PLAYER(player)->stroke[GOLFDOC()->m_currentHole - 1];

	if (GolfClub().PowerShotType())
		g_audio->PlaySfx("swing_powershot_effect");

	float accuracy = 0.0f;
	SendMsg(this, "GolfRule", 0x8b, (int)&accuracy, 0, 0, 0);
	int strength = 1;
	if (!(accuracy > 0.9f))
		strength = 2;

	int special = 0;
	IFF_STRUCT::sClubSet* clubSet = ItemManager()->FindClubSet(
		Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].clubInfo.tid);
	if (clubSet)
	{
		if ((clubSet->c.TypeId & 0x3ffffff) == 8)
			special = 1;
	}

	if (GolfBall().m_initSpecial & 0x10)
	{
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x1400000b)
			g_audio->PlaySfx("HG_SpecialShot_1");
		else
			g_audio->PlaySfx("swing_tomahawk");
		g_audio->PlaySfx("fly_tomahawk");
	}
	else if (GolfBall().m_initSpecial & 0x20)
	{
		if (Doc()->m_userInfo[GOLFDOC()->m_currentPlayer].userEquip.tidBall ==
			0x1400000b)
			g_audio->PlaySfx("HG_SpecialShot_1");
		else
			g_audio->PlaySfx("swing_cobra");
		g_audio->PlaySfx("fly_cobra");
	}

	switch (GolfClub().GetType())
	{
	case 0:
		if (special == 0)
		{
			if (score == 1)
			{
				if (strength == 1)
					g_audio->PlaySfx("swing_tee_s");
				else
					g_audio->PlaySfx("swing_tee_w");
			}
			else
			{
				if (strength == 1)
					g_audio->PlaySfx("swing_drive_s");
				else
					g_audio->PlaySfx("swing_drive_w");
			}
		}
		else if (special == 1)
		{
			g_audio->PlaySfx("picohan_wood");
		}

		if (GolfBall().m_initSpecial & 0x10)
		{
			g_audio->PlaySfx("swing_tomahawk");
			g_audio->PlaySfx("fly_tomahawk");
		}
		else if ((GolfBall().m_initSpecial & 0x20) ||
			(GolfBall().m_initSpecial & 0x40))
		{
			g_audio->PlaySfx("swing_cobra");
			g_audio->PlaySfx("fly_cobra");
		}
		break;

	case 1:
		if (special == 0)
		{
			if (strength == 1)
				g_audio->PlaySfx("swing_normal_s");
			else
				g_audio->PlaySfx("swing_normal_w");
		}
		else if (special == 1)
		{
			g_audio->PlaySfx("picohan_iron");
		}

		if (GolfBall().m_initSpecial & 0x10)
		{
			g_audio->PlaySfx("swing_tomahawk");
			g_audio->PlaySfx("fly_tomahawk");
		}

		switch (GolfBall().m_groundType)
		{
		case 1:
			g_audio->PlaySfx(MakeStr(
				"\xbe\xc6\xc0\xcc\xbe\xf0_\xc6\xe4\xbe\xee\xbf\xfe\xc0\xcc%d",
				strength));
			break;
		case 3:
			g_audio->PlaySfx(
				MakeStr("\xbe\xc6\xc0\xcc\xbe\xf0_\xb8\xf0\xb7\xa1\xb9\xe7%d",
					strength));
			g_audio->PlaySfx(
				"\xba\xa1\xc4\xbf\xb8\xf0\xb7\xa1\xb3\xaf\xb8\xb2");
			break;
		case 4:
			g_audio->PlaySfx(
				MakeStr("\xbe\xc6\xc0\xcc\xbe\xf0_\xb3\xaa\xb9\xb5\xc0\xd9%d",
					strength));
			break;
		}
		break;

	case 2:
		if (special == 0)
			g_audio->PlaySfx("swing_putting");
		else if (special == 1)
			g_audio->PlaySfx("mogura");

		if (GolfBall().m_initSpecial & 0x10)
		{
			g_audio->PlaySfx("swing_tomahawk");
			g_audio->PlaySfx("fly_tomahawk");
		}

		switch (GolfBall().m_groundType)
		{
		case 1:
			g_audio->PlaySfx(MakeStr(
				"\xbe\xc6\xc0\xcc\xbe\xf0_\xc6\xe4\xbe\xee\xbf\xfe\xc0\xcc%d",
				strength));
			break;
		case 3:
			g_audio->PlaySfx(
				MakeStr("\xbe\xc6\xc0\xcc\xbe\xf0_\xb8\xf0\xb7\xa1\xb9\xe7%d",
					strength));
			g_audio->PlaySfx(
				"\xba\xa1\xc4\xbf\xb8\xf0\xb7\xa1\xb3\xaf\xb8\xb2");
			break;
		case 4:
			g_audio->PlaySfx(
				MakeStr("\xbe\xc6\xc0\xcc\xbe\xf0_\xb3\xaa\xb9\xb5\xc0\xd9%d",
					strength));
			break;
		}
		break;

	case 3:
		if (special == 0)
			g_audio->PlaySfx("swing_putting");
		else if (special == 1)
			g_audio->PlaySfx("harisen_putt");
		break;
	}
}

void CGolfPlayer::SetPendingTexPart(int player, char* part)
{
	std::list<sTexturePart*>::iterator it;
	it = std::find_if(m_texPartList.begin(), m_texPartList.end(),
		sFindTexPart(part));

	if (it != m_texPartList.end())
	{
		if (*it != m_petInfo[player].pendingTexPart)
			m_petInfo[player].pendingTexPart = *it;
	}
	else
	{
		sTexturePart* texPart = new sTexturePart;

		strcpy(texPart->part, part);
		m_texPartList.push_back(texPart);

		m_petInfo[player].pendingTexPart = texPart;
	}
}

void CGolfPlayer::SearchGround()
{
	WVector dir;
	WVector ground;
	WVector next;
	float t;
	float dist[2];

	dir = m_ballDist * RotMat(m_dir, 1).xa;

	for (t = 0.0f; t < 1.0f; t += 0.1f)
	{
		ground = GetPVS().GetGroundPoint(m_pos + dir * t, true, NULL, false);
		next = GetPVS().GetGroundPoint(m_pos + dir * (t + 0.1f), true, NULL,
			false);

		dist[0] = WVectorLen(ground - m_pos);
		dist[0] -= m_ballDist;
		dist[1] = WVectorLen(next - m_pos);
		dist[1] -= m_ballDist;
		if (dist[0] * dist[1] <= 0.0f)
			break;
	}

	for (float s = t; s < t + 0.1f; s += 0.01f)
	{
		ground = GetPVS().GetGroundPoint(m_pos + dir * s, true, NULL, false);
		next = GetPVS().GetGroundPoint(m_pos + dir * (s + 0.01f), true, NULL,
			false);

		dist[0] = WVectorLen(ground - m_pos) - m_ballDist;
		dist[1] = WVectorLen(next - m_pos) - m_ballDist;
		if (dist[0] * dist[1] <= 0.0f)
		{
			m_mat.xa = next - m_pos;
			m_mat.xa.Normalize();

			m_mat.ya = WCrossProduct(m_mat.za, m_mat.xa);

			if (GOLFDOC()->m_bNoStat)
			{
				m_bBlocked = false;
			}
			else if (Abs(m_mat.xa.y) > (float)sin(20.0 * g_DEGTORAD))
			{
				m_bBlocked = true;
			}
			else
			{
				(void)Abs(m_ballDist);
				const WVector& ballPos =
					m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
						.ballPos;
				const float margin = 1.6f;
				m_bBlocked = GetPVS().OverlappedWithObject(
					m_pos + ballPos * m_mat, margin);
			}

			SendMsg(this, "GolfRule", 7, !m_bBlocked, 0, 0, 0);
			SendMsg(this, "GolfRule", 0x77, !m_bBlocked, 0, 0, 0);
			SendMsg(this, "GolfRule", 7, !m_bBlocked, 0, 0, 0);
			m_footAngle = asinf(m_mat.xa.y) * -1.0f;
			goto done;
		}
	}

	m_mat = RotMat(m_dir, 1);
	m_bBlocked = true;
	SendMsg(this, "GolfRule", 0x77, 0, 0, 0, 0);
	m_footAngle = 0.0f;

done:
	m_petInfo[GOLFDOC()->m_currentPlayer].flags |= 0xb;
}

bool CGolfPlayer::SearchGround(const WVector& pos, float angle)
{
	WVector dir;
	WVector ground;
	WVector next;
	int index;
	float t;

	dir = m_ballDist * RotMat(angle, 1).xa;

	// HACK: emission order workaround
	GetPlayerNum();

	for (t = 0.0f; t < 1.0f; t += 0.1f)
	{
		ground = GetPVS().GetGroundPoint(pos + dir * t, true, NULL, false);
		float n = t + 0.1f;
		next = GetPVS().GetGroundPoint(pos + dir * n, true, &index, false);

		if (index < 0)
			return false;

		float dist[2];
		dist[0] = WVectorLen(ground - pos) - m_ballDist;
		dist[1] = WVectorLen(next - pos) - m_ballDist;
		if (dist[0] * dist[1] <= 0.0f)
			break;
	}

	for (float s = t; s < t + 0.1f; s += 0.01f)
	{
		ground = GetPVS().GetGroundPoint(pos + dir * s, true, NULL, false);
		next =
			GetPVS().GetGroundPoint(pos + dir * (s + 0.01f), true, NULL, false);

		float dist[2];
		dist[0] = WVectorLen(ground - pos) - m_ballDist;
		dist[1] = WVectorLen(next - pos) - m_ballDist;
		if (dist[0] * dist[1] <= 0.0f)
		{
			WMatrix mat = RotMat(m_dir, 1);

			mat.xa = next - pos;
			mat.xa.Normalize();

			mat.ya = WCrossProduct(mat.za, mat.xa);

			const float half = 16.0f;
			Waabb box;
			box.min = GolfBall().PreData(GolfBall().m_renderNum)->pos -
				WVector(half, 0.0f, half);
			box.max = GolfBall().PreData(GolfBall().m_renderNum)->pos +
				WVector(half, 0.0f, half);
			GetPVS().GetObjList(box);

			GetPlayerNum();

			if (!(Abs(mat.xa.y) > (float)sin(20.0 * g_DEGTORAD)) &&
				!GetPVS().OverlappedWithObject(pos +
						m_club[GOLFDOC()->m_currentPlayer][GolfClub().GetType()]
								.ballPos *
							mat,
					1.6f))
				return true;

			return false;
		}
	}

	return false;
}

unsigned char CGolfPlayer::HasStandGround(const WVector& pos)
{
	float angle = 0.0f;
	unsigned char count = 0;

	for (int i = 0; i < 8; i++, angle += 3.1415927f / 4.0f)
	{
		if (SearchGround(pos - WVector(0.0f, GolfBall().GetRadius(), 0.0f),
				angle))
			count++;
	}

	return count;
}

void CGolfPlayer::SetLight()
{
	WBone* pBone =
		m_petFrame[GOLFDOC()->m_currentPlayer].GetBonePet()->FindBoneByTail(
			"Head");
	m_lightSet.diffuse = ModifyColor(g_lightset.diffuse,
		GetPVS().GetShadowIntensity(pBone->m_matrix.pivot));
}

void CGolfPlayer::SetLight(int player)
{
	if (player >= 0 && player < 4)
	{
		WBone* pBone = m_petFrame[player].GetBonePet()->FindBoneByTail("Head");

		if (pBone)
			m_lightSet.diffuse = ModifyColor(g_lightset.diffuse,
				GetPVS().GetShadowIntensity(pBone->m_matrix.pivot));
	}
}

void CGolfPlayer::ResetComboState()
{
	m_bCombo = false;
	m_comboTime = 0;
}

void CGolfPlayer::StartComboState()
{
	m_bCombo = true;
	m_comboTime = 0;
}

void CGolfPlayer::EndComboState()
{
	WVector unused;

	m_comboLevel = (int)(m_comboTime / 33.0f);
	m_bCombo = false;
	m_comboTime = 0;
}

bool CGolfPlayer::Decide_PowerShot()
{
	if ((m_comboLevel == 1 &&
			PLAYER(GOLFDOC()->m_currentPlayer)->gauge >= 33.0f) ||
		(m_comboLevel == 2 &&
			PLAYER(GOLFDOC()->m_currentPlayer)->gauge >= 66.0f))
	{
		return true;
	}

	if ((m_comboLevel == 1 && Doc()->m_golfGame.gameType == 1 &&
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge >= 33.0f) ||
		(m_comboLevel == 2 && Doc()->m_golfGame.gameType == 1 &&
			TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)->gauge >= 66.0f))
	{
		return true;
	}

	if (m_comboLevel == 1 &&
		(PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000004 ||
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000010 ||
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000022))
	{
		return true;
	}

	if (m_comboLevel == 2 &&
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000027)
	{
		return true;
	}

	return false;
}

void CGolfPlayer::Shot_PowerShot()
{
	if ((GOLFDOC()->GameMode() & 0x208) && Doc()->m_playingMode == 1)
	{
		if (m_bForcePowerShot == true && m_comboLevel == 2 &&
			PLAYER(GOLFDOC()->m_currentPlayer)->item != 0x18000027)
		{
			if (CHelpMsg::Instance())
			{
				g_audio->PlaySfx("play_unplayable");
				CHelpMsg::Instance()->SetHelpMsg(0x1a, NULL);
			}
			return;
		}

		if (Decide_PowerShot())
		{
			if (!m_bTurn)
			{
				if (m_swingType[0])
				{
					if (m_petInfo[GOLFDOC()->m_currentPlayer].powerShot == 3)
					{
						g_audio->StopSfx("powershot_ready");
						m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 0;
						SetMotion(MakeStr("%s\xc1\xd8\xba\xf1", m_swingType),
							0.0f, -1);

						float ratio = 0.0f;
						SendMsg(this, "Screen", 0x1aa, (int)&ratio, 0, 0, 0);
					}
					else
					{
						g_audio->PlaySfx("powershot_ready", NULL, 0,
							"powershot_ready");
						m_petInfo[GOLFDOC()->m_currentPlayer].powerShot = 3;

						if (IsLocalContent(S3_STRENGTH))
						{
							if (PLAYER(GOLFDOC()->m_currentPlayer)->item ==
								0x18000027)
								GolfClub().SetPowerShot(3);
							else
								GolfClub().SetPowerShot(m_comboLevel);
						}
						else
						{
							GolfClub().SetPowerShot(m_comboLevel);
						}

						unsigned char type = GolfClub().PowerShotType();
						const char* name;
						if (type == 2 || type == 3)
							name = MakeStr(
								"%s\xc6\xc4\xbf\xf6\x32\xc1\xd8\xba\xf1",
								m_swingType);
						else
							name = MakeStr("%s\xc6\xc4\xbf\xf6\xc1\xd8\xba\xf1",
								m_swingType);
						SetMotion(name, 0.0f, -1);

						if (Doc()->m_golfGame.gameType == 3)
							GolfClub().SetCalcRandomPower(false);

						IObject* pObj[4];
						SendMsg(this, "GolfRule", 0x24a, (int)"predict_normal",
							(int)&pObj[0], 0, 0);
						SendMsg(this, "GolfRule", 0x24a,
							(int)"predict_tomahwak", (int)&pObj[1], 0, 0);
						SendMsg(this, "GolfRule", 0x24a, (int)"predict_cobra",
							(int)&pObj[2], 0, 0);
						SendMsg(this, "GolfRule", 0x24a, (int)"predict_spike",
							(int)&pObj[3], 0, 0);

						FrButton* pButton[4];
						pButton[0] = DYNAMIC_CAST(FrButton, pObj[0]);
						pButton[1] = DYNAMIC_CAST(FrButton, pObj[1]);
						pButton[2] = DYNAMIC_CAST(FrButton, pObj[2]);
						pButton[3] = DYNAMIC_CAST(FrButton, pObj[3]);

						if (pButton[0])
							pButton[0]->SetStatus(FrButton::NORMAL);
						if (pButton[1])
							pButton[1]->SetStatus(FrButton::PRESSED);
						if (pButton[2])
							pButton[2]->SetStatus(FrButton::NORMAL);
						if (pButton[3])
							pButton[3]->SetStatus(FrButton::NORMAL);

						SendMsg(this, "ShotPreview", -1,
							(int)"CHANGE_PREDICT_TYPE", 1, 0, 0);
					}
				}
			}
		}
		else
		{
			if (CHelpMsg::Instance())
			{
				g_audio->PlaySfx("play_unplayable");
				CHelpMsg::Instance()->SetHelpMsg(1, NULL);
			}
		}
	}
}

void CGolfPlayer::Cancel_PowerShot()
{
	if (GolfClub().PowerShotType())
	{
		g_audio->StopSfx("powershot_ready");
		g_audio->PlaySfx("powershot_cancel");

		float ratio = 0.0f;
		GolfClub().SetPowerShot(0);
		SendMsg(this, "Spot", 0xf5, 0, 0, 0, 0);
		SendMsg(this, "Screen", 0x1aa, (int)&ratio, 0, 0, 0);

		SetSwingType();

		if (OnlinePlay())
		{
			WSendPacket packet((enumClientPacket)0x15);
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}
	}
}

void CGolfPlayer::PowerShotInput()
{
	if (IsMyTurn(false) && !CQuitWindow::Instance()->IsActive() &&
		CItemWindow::IsClosed())
	{
		if (m_powerShotState == 0)
		{
			if (g_input->GetUp("LALT") || g_input->GetUp("RALT"))
			{
				if (m_bPowerShot)
				{
					Cancel_PowerShot();
					m_bPowerShot = false;
				}
				else
				{
					if (IsLocalContent(S3_STRENGTH) &&
						PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000027)
					{
						m_powerShotState = 2;
						m_powerShotTime = timeGetTime();
					}
					else
					{
						m_powerShotState = 1;
						m_powerShotTime = timeGetTime();
					}
				}
			}
		}
		else
		{
			if (m_powerShotState == 1)
			{
				if (g_input->GetDown("LALT", true) ||
					g_input->GetDown("RALT", true))
				{
					if (timeGetTime() < m_powerShotTime + 500)
					{
						m_powerShotState = 2;
					}
					else if (m_bPowerShot)
					{
						Cancel_PowerShot();
						m_bPowerShot = false;
						m_powerShotState = 0;
					}
				}
			}
			else if (m_powerShotState == 2)
			{
				if (g_input->GetUp("LALT") || g_input->GetUp("RALT"))
				{
					m_comboLevel = 2;
					Shot_PowerShot();
					m_bPowerShot = true;
					m_powerShotState = 0;
				}
			}
		}
	}

	if (m_powerShotState == 1 && !m_bPowerShot &&
		timeGetTime() > m_powerShotTime + 500)
	{
		m_comboLevel = 1;
		Shot_PowerShot();
		m_bPowerShot = true;
		if (m_powerShotState == 1 && m_comboLevel == 1)
		{
			m_powerShotState = 0;
		}
		else
		{
			m_powerShotState = 0;
		}
	}
}

bool CGolfPlayer::IsAnimFxRunning(int player, unsigned long typeId)
{
	if (!typeId || player > 4)
		return false;

	std::vector<unsigned long>::iterator it;
	for (it = m_animFx[player].begin(); it != m_animFx[player].end(); it++)
	{
		if (*it == typeId)
			return true;
	}

	return false;
}

void CGolfPlayer::RefreshAllUccClothes()
{
	sUccRefreshItem item;

	for (int i = 0; i < 10; ++i)
	{
		if (!UccManager()->GetRefreshItem(item))
			break;

		for (int j = 0; j < GetPlayerNum(); ++j)
		{
			CPartTidList& tids = GOLFDOC()->GetPartTidList(j);

			unsigned char slot = (unsigned char)((item.typeId >> 13) & 0x1f);
			if (tids.m_tid[slot] == item.typeId)
			{
				unsigned long uid = tids.m_uid[slot];

				sUccClothes* pClothes = UccManager()->FindClothes(uid);

				if (pClothes && !strcmpi(pClothes->uccIndex, item.uccIndex))
				{
					m_petFrame[j].SetPart(&tids, pClothes->typeId,
						pClothes->id);
					m_petFrame[j].SetPart(&tids, pClothes->typeId,
						pClothes->id);

					m_petFrame[j].GetBonePet()->xUpdateTnLBuffers(true);
				}
			}
		}
	}
}

void CGolfPlayer::AddAbilityParts(unsigned long from, unsigned long to)
{
	m_abilityParts.push_back(std::make_pair(from, to));
}
