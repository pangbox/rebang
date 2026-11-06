#include "minatl.h"
#include "npc.h"
#include "golfball.h"
#include "quadtree.h"
#include "clientsetting.h"
#include "slicestr.h"
#include "fx.h"
#include "wbone.h"

const char* GetStrConst(const char* source, char* dest);

IMPLEMENT_ACTOR(CNPC, IActor)

void CNPC::OnDestroy()
{
	Destroy();
}

void CNPC::Destroy()
{
	for (std::list<CCreature*>::iterator it = m_creatureList.begin();
		it != m_creatureList.end(); it++)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_creatureList.clear();
}

void CNPC::OnInit()
{
	Destroy();

	for (int i = 0; i < GOLFDOC()->m_pPolySoup->GetAreaNum(); ++i)
	{
		WPolySoup::sArea* area = GOLFDOC()->m_pPolySoup->GetArea(i);
		if (area->type == 1)
			SetNPC(area->aabb, area->name);
	}

	for (std::vector<CNodeContainer>::const_iterator it =
			 GOLFDOC()->m_pPolySoup->m_node.begin();
		it != GOLFDOC()->m_pPolySoup->m_node.end(); ++it)
	{
		if (it->GetType() == CNodeContainer::TYPE_NPC)
			SetNPC(it->GetList(), it->GetName().c_str());
	}
}

void CNPC::OnProcess(float delta)
{
	for (std::list<CCreature*>::iterator it = m_creatureList.begin();
		it != m_creatureList.end(); it++)
	{
		(*it)->Process(delta);
	}
}

void CNPC::HandleMsg(const MsgObject& msg)
{
	std::list<CCreature*>::iterator it;

	switch (msg.message)
	{
	case 0x188:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			if ((*it)->GetType() == msg.param1)
			{
				*(int*)msg.param2 = 1;
				return;
			}
		}
		break;
	case 0x182:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			if (((*it)->m_flag & 1) && (*it)->GetType() != 9)
			{
				if ((*it)->GetActive())
					(*it)->SetActive(false);
			}
		}
		break;
	case 0x183:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			CCreature* creature = *it;
			if (creature->m_flag & 1)
			{
				Waabb area = creature->GetArea();
				if (area.IsInclude(GolfBall().m_pos))
				{
					(*it)->PlayEvent();
					return;
				}
			}
		}
		break;
	case 0x184:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			CCreature* creature = *it;
			if (creature->m_flag & 1)
			{
				Waabb area = creature->GetArea();
				if (area.IsInclude(GolfBall().m_pos))
				{
					*(int*)msg.param1 = 0;
					return;
				}
			}
		}
		break;
	case 0x186:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			if (!((*it)->m_flag & 1))
				(*it)->Record();
		}
		break;
	case 0x185:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			(*it)->Replay();
		}
		break;
	case 0x18a:
		for (it = m_creatureList.begin(); it != m_creatureList.end(); it++)
		{
			if ((*it)->GetType() == 9)
				(*it)->PlayEvent();
		}
		break;
	}
}

void CNPC::SetNPC(const std::vector<WVector>& node, const char* option)
{
	CCreature* creature;

	if (strstr(option, "santa"))
	{
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME))
		{
			if (Doc()->m_golfGame.gameType)
				return;

			if (Doc()->m_gameMode ||
				Doc()->m_holeRandom[GetHoleIndex(GOLFDOC()->m_currentHole) -
					1] % 100 >=
					3)
				return;
		}

		WSliceStr slice(option);
		float radius;
		if (!slice.Get("*r", "%f", &radius))
			radius = 32.0f;

		int i;
		for (i = 0; i < 3; ++i)
		{
			creature = new CSanta("NPC_Mozzi.pet");
			creature->SetRadius(radius);
			creature->SetArea(node, i * -0.05f, 0);
			creature->SetType(7);
			m_creatureList.push_back(creature);
		}

		creature = new CSanta("NPC_Santa.pet");
		creature->SetRadius(radius);
		creature->SetArea(node, (i + 1) * -0.05f, 0);
		creature->SetType(7);
	}
	else if (strstr(option, "flight"))
	{
		creature = new CFighter("NPC_Fighter.pet");
		creature->SetArea(node, 0, 0);
		creature->SetType(8);
	}
	else if (strstr(option, "ufo"))
	{
		creature = new CUfo("sand_house12.pet");
		creature->SetArea(node, 0, 0);
		creature->SetType(11);
	}
	else if (strstr(option, "penguinwalk"))
	{
		creature = new ntPenguin("NPC_penguin02.pet");
		creature->SetArea(node, 0, 0);
		creature->SetType(12);
	}
	else if (strstr(option, "drg"))
	{
		WSliceStr slice(option);
		float delay;
		if (!slice.Get("*d", "%f", &delay))
			delay = 0.0f;

		creature = new ntNpc_Dragon("NPC_dragon.pet", delay);
		creature->SetArea(node, 0, 0);
		creature->SetType(15);
	}
	else if (strstr(option, "ship"))
	{
		WSliceStr slice(option);
		float delay;
		if (!slice.Get("*d", "%f", &delay))
			delay = 0.0f;

		creature = new ntNpc_PolaShip("NPC_ship.pet", delay);
		creature->SetArea(node, 0, 0);
		creature->SetType(17);
	}
	else if (strstr(option, "vessel"))
	{
		char name[64] = "";
		float time = 0.0f;
		WSliceStr slice(option);
		int num = 2;
		if (strstr(option, "vessel4"))
			num = 4;

		if (!slice.Get("*t", "%f", &time))
			time = 1.0f;

		sprintf(name, "gs_ship_0%d.pet", num);
		creature = new CNPC_TRSHIP(name, time);
		creature->SetArea(node, 0, 0);
		creature->SetType(20);
	}
	else if (strstr(option, "walex"))
	{
		char name[64] = "";
		WSliceStr slice(option);
		int num = 1;
		float speed = 0.0f;
		if (strstr(option, "walex03"))
			num = 3;

		if (!slice.Get("*vel", "%f", &speed))
			speed = 1.0f;

		sprintf(name, "npc_alex0%d.pet", num);
		creature = new CNPC_TORTOISE(name, speed);
		creature->SetArea(node, 0, 0);
		creature->SetType(23);
	}
	else if (strstr(option, "herald"))
	{
		WSliceStr slice(option);
		float speed;
		if (!slice.Get("*vel", "%f", &speed))
			speed = 1.0f;

		if (strstr(option, "herald01"))
			creature = new CNPC_TORTOISE("NPC_Herald01.pet", speed);
		else
			creature = new CNPC_TORTOISE("NPC_Herald03.pet", speed);

		creature->SetArea(node, 0, 0);
		creature->SetType(23);
	}
	else if (strstr(option, "crn") || strstr(option, "gns") ||
		strstr(option, "pnd") || strstr(option, "toy"))
	{
		WSliceStr slice(option);
		char prefix[32] = "";
		char name[256] = "";
		float speed = 0.0f;
		bool bWalk = false;
		if (!slice.Get("*v", "%f", &speed))
			speed = 1.0f;

		if (strstr(option, "#"))
			bWalk = true;

		if (strstr(option, "crn"))
			strcpy(prefix, "NPC_crane");
		else if (strstr(option, "gns"))
			strcpy(prefix, "NPC_ginseng");
		else if (strstr(option, "pnd"))
			strcpy(prefix, "NPC_panda");
		else if (strstr(option, "toy"))
			strcpy(prefix, "NPC_toy");

		std::string str(option);
		std::string pet("");
		int pos = strstr(option, "*") ? str.find('*') : str.find('#');
		if (!pos)
			pos = str.length();

		pet = str.substr(7, pos - 7);
		sprintf(name, "%s%s.pet", prefix, pet.c_str());

		if (bWalk)
		{
			creature = new CNPC_TORTOISE(name, speed);
			creature->SetArea(node, 0, 0);
			creature->SetType(23);
		}
		else
		{
			creature = new CNPC_TRSHIP(name, speed);
			creature->SetArea(node, 0, 0);
			creature->SetType(20);
		}
	}
	else if (strstr(option, "npc_t"))
	{
		const char* pet = "";
		WSliceStr slice(option);
		float value = 1.0f;
		slice.Get("*v", "%f", &value);

		if (strstr(option, "npc_t0"))
			pet = "npc_insam_ani_N.pet";
		else if (strstr(option, "npc_t1"))
			pet = "npc_cat_ani02_N.pet";

		ntNpc_WizCityNode* walker = new ntNpc_WizCityNode(pet, value);
		value = 1.0f;
		slice.Get("*s", "%f", &value);
		walker->SetScale(value);

		creature = walker;
		creature->SetArea(node, 0, 0);
		creature->SetType(32);
	}
	else
	{
		const char* name = strstr(option, "npc_");
		if (!name)
			return;

		creature = new CDisposable(MakeStr("%s.pet", name + 4));
		creature->SetArea(node, 0, 0);
		creature->SetType(9);
	}

	m_creatureList.push_back(creature);
}

void CNPC::SetNPC(const Waabb& area, char* option)
{
	int type;
	int num;
	char pet[32];
	CCreature* creature = NULL;
	unsigned long flag = 0;

	WSliceStr slice(option);
	slice.Get("*type", "%d", &type);
	slice.Get("*pet", "%s", pet);

	switch (type)
	{
	case 0:
		if (slice.Get("*num", "%d", &num))
		{
			for (int i = 0; i < num; ++i)
			{
				creature = new CGull(pet);
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatureList.push_back(creature);
			}
			return;
		}

		creature = new CGull(pet);
		break;
	case 1:
		if (slice.Get("*num", "%d", &num))
		{
			for (int i = 0; i < num; ++i)
			{
				creature = new CButterfly(pet);
				creature->SetArea(area, 4);
				creature->SetType(type);
				m_creatureList.push_back(creature);
			}
			return;
		}

		creature = new CButterfly(pet);
		flag = 4;
		break;
	case 2:
		if (slice.Get("*num", "%d", &num))
			creature = new CDuck(pet, num + 1);
		else
			creature = new CDuck(pet, 0);
		flag = 1;
		break;
	case 3:
		creature = new CMole(pet);
		break;
	case 4:
		creature = new CDolphin(pet);
		break;
	case 5:
		creature = new CCrab(pet);
		break;
	case 6:
		if (slice.Get("*num", "%d", &num))
			creature = new CSparrow(pet, num);
		break;
	case 7:
		break;
	case 8:
		creature = new CFighter(pet);
		break;
	case 9:
		creature = new CDisposable(pet);
		break;
	case 10:
		break;
	case 13:
		if (slice.Get("*num", "%d", &num))
		{
			for (int i = 0; i < num; ++i)
			{
				creature = new ntPenguin_Sailor(pet);
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatureList.push_back(creature);
			}
			return;
		}

		creature = new ntPenguin_Sailor(pet);
		break;
	case 14:
		if (slice.Get("*num", "%d", &num))
			creature = new ntPenguin_Duck(pet, num + 1);
		else
			creature = new ntPenguin_Duck(pet, 0);
		flag = 1;
		break;
	case 16:
		if (slice.Get("*num", "%d", &num))
		{
			for (int i = 0; i < num; ++i)
			{
				creature = new ntNpc_BlackSparrow("NPC_BTsparrow.pet");
				creature->SetArea(area, 0);
				creature->SetType(type);
				m_creatureList.push_back(creature);
			}
			return;
		}

		creature = new ntNpc_BlackSparrow("NPC_BTsparrow.pet");
		break;
	case 18:
	{
		float speed = 2.0f;
		slice.Get("*vel", "%f", &speed);
		num = 1;
		slice.Get("*num", "%d", &num);
		for (int i = 0; i < num; ++i)
		{
			creature = new ntNpc_PolaPenguin1(pet, speed);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatureList.push_back(creature);
		}
		return;
	}
	case 19:
		num = 1;
		slice.Get("*num", "%d", &num);
		for (int i = 0; i < num; ++i)
		{
			creature = new ntNpc_PolaPenguin2(pet);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatureList.push_back(creature);
		}
		return;
	case 31:
	{
		num = 1;
		slice.Get("*num", "%d", &num);
		float scale = 1.0f;
		slice.Get("*s", "%f", &scale);
		for (int i = 0; i < num; ++i)
		{
			ntNpc_WizCityArea* walker = new ntNpc_WizCityArea(pet);
			walker->SetScale(scale);
			creature = walker;
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatureList.push_back(creature);
		}
		return;
	}
	case 21:
	{
		num = 1;
		slice.Get("*num", "%d", &num);
		float speed;
		slice.Get("*vel", "%f", &speed);
		for (int i = 0; i < num; ++i)
		{
			creature = new CNPC_BIRD(pet, speed);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatureList.push_back(creature);
		}
		return;
	}
	case 22:
		if (!slice.Get("num", "%d", &num))
			num = 1;

		for (int i = 0; i < num; ++i)
		{
			creature = new CNPC_PARROT(pet);
			creature->SetArea(area, 0);
			creature->SetType(type);
			m_creatureList.push_back(creature);
		}
		return;
	default:
		return;
	}

	creature->SetArea(area, flag);
	creature->SetType(type);
	m_creatureList.push_back(creature);
}

CCreature::~CCreature()
{
	if (g_resrcmng && m_addTex)
	{
		g_resrcmng->Release(m_addTex);
		m_addTex = 0;
	}
	if (g_resrcmng && m_pPet)
	{
		g_resrcmng->Release(m_pPet, false);
		m_pPet = NULL;
	}
	if (m_pHistory)
	{
		delete[] m_pHistory;
		m_pHistory = NULL;
	}
	m_nodeList.clear();
	m_recordList.clear();
	m_alphaList.clear();
}

void CCreature::Init(const char* name, unsigned long flag)
{
	m_addTex = 0;
	m_pPet = g_resrcmng->GetPuppet(name, false, true, false);
	if (!m_pPet)
		return;

	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, m_pPet, this, name,
		flag, NULL);
	m_roll = 0.0f;
	m_bRecord = false;
	m_alpha = 0xff;
	m_recordIndex = 0;
	m_rollSpeed = g_PI / 4.0f;
	SetMotion("NPC_Default", -1.0f);

	if (m_flag & 1)
	{
		m_bActive = false;
		m_alphaList.clear();

		int from, to;
		if (m_pPet->CheckFrameData((int)floorf(m_pMotion->s * 30.0f),
				(int)ceilf(m_pMotion->e * 30.0f), from, to))
		{
			for (int i = from; i <= to; ++i)
			{
				sAlpha alpha;
				const char* data = m_pPet->m_frame[i].data;
				if (sscanf(data, "*opa %f", &alpha.alpha))
				{
					alpha.alpha *= 2.55f;
					alpha.time = m_pPet->GetFrameDataTime(i);
					m_alphaList.push_back(alpha);
				}
			}
		}
	}
	else
	{
		m_bActive = true;
		m_rotSpeed = g_PI / 3.0f;
	}

	for (w_bound_box* box = m_pPet->m_bbList.Start(); box;
		box = m_pPet->m_bbList.Next())
	{
		char* option = box->info->option;
		if (!option)
			continue;

		char* fx = strstr(option, "*fx");
		if (!fx)
			continue;

		Attacher attacher;
		char command[6];
		char file[32] = "", bone[32] = "";
		sscanf(fx, "%s %s %s", command, file, bone);

		if (box->bone && !box->bone->IsInvisible())
		{
			char* ext = strrchr(file, '.');
			if (ext && strlen(ext) >= 4)
			{
				if (!stricmp(ext + 1, "spr") || !stricmp(ext + 1, "seq"))
				{
					Attacher attacher;
					attacher.mat[0] = &box->bone->GetMatrix();
					attacher.pos[0] = &box->info->spherePivot;
					if (!stricmp(ext + 1, "spr"))
						CFx::Instance()->Attach(attacher, file, false, NULL);
					else
						CFx::Instance()->Attach(attacher, file, false, NULL);
				}
			}
		}
	}
}

void CCreature::SetArea(const Waabb& area, unsigned long flag)
{
	m_area = area;
	if (m_flag & 1)
		return;

	m_mat = RotMat(Random(-180.0f, 180.0f) * g_DEGTORAD, 1);
	for (int i = 0; i < 3; ++i)
	{
		float r = Random(0.0f, 1.0f);
		m_mat.pivot.p[i] = (1.0f - r) * m_area.max.p[i] + r * m_area.min.p[i];
	}

	if (flag & 1)
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, false, NULL, true);
	if (flag & 2)
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, false, NULL, false);
	m_target = m_mat.pivot;

	float numX = ceilf((m_area.max.x - m_area.min.x) / m_cellSize);
	float numY;
	if (flag & 4)
		numY = ceilf((m_area.max.y - m_area.min.y) / m_cellSize);
	else
		numY = 1.0f;
	float numZ = ceilf((m_area.max.z - m_area.min.z) / m_cellSize);
	m_cellNumX = (int)numX;
	m_cellNumY = (int)numY;
	m_cellNumZ = (int)numZ;
	m_historyNum = m_cellNumY > 1 ? 4 : 2;
	m_cellLenX = (m_area.max.x - m_area.min.x) / numX;
	m_cellLenY = (m_area.max.y - m_area.min.y) / numY;
	m_cellLenZ = (m_area.max.z - m_area.min.z) / numZ;
	m_pHistory = new int[m_historyNum];

	int cell = (int)((m_target.x - m_area.min.x) / m_cellLenX) +
		m_cellNumX *
			((int)((m_target.z - m_area.min.z) / m_cellLenZ) +
				m_cellNumZ * (int)((m_target.y - m_area.min.y) / m_cellLenY));
	for (int i = 0; i < m_historyNum; ++i)
		m_pHistory[i] = cell;
}

void CCreature::SetArea(const std::vector<WVector>& node, float time,
	unsigned long flag)
{
	m_nodeList.clear();
	m_nodeList.reserve(node.size());
	m_nodeList.assign(node.begin(), node.end());

	m_mat.za = -1.0f * (m_nodeList[1] - m_nodeList[0]).Normalize();
	m_mat.ya = WVector::UNIT_POS_Y;
	m_mat.xa = WCrossProduct(m_mat.ya, m_mat.za);
	m_mat.pivot = node[0];
	m_target = m_nodeList[1];
	m_time = time;
}

void CCreature::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		if (m_flag & 1)
		{
			m_bActive = false;
			m_time = m_pMotion->s;
		}
		else
			m_time -= m_pMotion->e;
	}

	if (m_flag & 1)
	{
		std::list<sAlpha>::iterator it;
		for (it = m_alphaList.begin(); it != m_alphaList.end();)
		{
			std::list<sAlpha>::iterator prev = it;
			++it;
			if (it == m_alphaList.end())
			{
				m_alpha = (unsigned char)(*prev).alpha;
			}
			else if (m_time > (*prev).time && m_time < (*it).time)
			{
				float t = (m_time - (*prev).time) / ((*it).time - (*prev).time);
				m_alpha = (unsigned char)((1.0f - t) * (*prev).alpha +
					t * (*it).alpha);
				break;
			}
		}
	}
	else
		CheckTarget(dt);
}

void CCreature::Display()
{
	if (!COption::Instance()->vIsNpcEnabled() &&
		Doc()->m_pGolfDoc->m_currentHole != 19)
		return;
	if (!m_pPet || !m_bActive || m_time < 0.0f)
		return;

	if (m_flag & 1)
		m_pPet->SetAlpha(m_alpha);

	m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false),
		Abs(m_roll) > 0.0f ? RotMat(m_roll, 2) * m_mat : m_mat);
	m_pPet->Transform(g_view, 1.0f, 0);
	m_pPet->UpdateBSphere();
	if (g_view->InFrustum(m_pPet->GetBoundSphere()))
	{
		m_pPet->UpdateLightSource(&g_lightset, false, NULL);
		m_pPet->Render(g_view, true, 1.0f,
			g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
	}
}

void CCreature::DrawDebugLine()
{
	g_view->DrawSphere(m_pPet->GetBoundSphere(), 0xff00ff00, 20, 0);
	if (!(m_flag & 1))
	{
		g_view->DrawLine(m_mat.pivot, 0xffffff00, m_mat.pivot - m_mat.za * 5.0f,
			0xffffff00, 0);
		g_view->DrawLine(m_mat.pivot, 0xffff0000, m_target, 0xffff0000, 0);
	}
}

void CCreature::SetMotion(const char* name, float time)
{
	if (!m_pPet)
		return;

	w_motion_data* motion = m_pPet->FindMotionData(name);
	if (motion)
	{
		m_pMotion = motion;
		if (time >= 0.0f)
			m_time = Min(time + motion->s, motion->e);
		else
			m_time = Random(motion->s, motion->e);
	}
	else
		m_time = 0.0f;
}

void CCreature::ScriptProcess(const w_motion_data* motion, float prevTime,
	float curTime)
{
	if (!motion)
		return;

	if (prevTime <= motion->e && prevTime != curTime)
	{
		float time = curTime;
		prevTime *= 30.0f;
		curTime = Min(motion->e, curTime) * 30.0f;
		int first = Ceil(prevTime, 0.03f);
		int last = curTime == floorf(curTime)
			? float2int(curTime) - 1
			: float2int((float)floor((double)curTime));

		if (first <= last)
		{
			int from, to;
			if (m_pPet->CheckFrameData(first, last, from, to))
			{
				for (int i = from; i <= to; ++i)
					ParsePuppetScript(m_pPet->m_frame[i].data);
			}
		}

		if (time > motion->e)
		{
			prevTime = motion->s * 30.0f;
			curTime = Min(motion->e, time - motion->e) * 30.0f;
			first = Ceil(prevTime, 0.03f);
			last = curTime == floorf(curTime)
				? float2int(curTime) - 1
				: float2int((float)floor((double)curTime));
			if (first <= last)
			{
				int from, to;
				if (m_pPet->CheckFrameData(first, last, from, to))
				{
					for (int i = from; i <= to; ++i)
						ParsePuppetScript(m_pPet->m_frame[i].data);
				}
			}
		}
	}
}

void CCreature::ParsePuppetScript(const char* script)
{
	static char token[1024];

	if (!script)
		return;

	const char* p = script;
	while (p)
	{
		p = strchr(p, '*');
		if (!p)
			break;

		int len = 0;
		while (p[len])
		{
			if (p[++len] == ' ')
				break;
			if (p[len] == '(')
			{
				++len;
				break;
			}
		}
		if (len <= 1)
			break;

		strncpy(token, p, len);
		token[len] = '\0';
		p += len;
		if (!strcmpi(token, "*fx("))
		{
			char name[64];
			char file[64];
			char bone[64];
			int length;

			name[0] = '\0';
			p = GetStrConst(p, name);
			if (!(length = strlen(name)) || strlen(name) >= 64)
				return;

			if (name[0] == '@')
			{
				if (!m_pMotion || strcmp(name + 1, m_pMotion->name))
					return;

				p = GetStrConst(p, file);
				if (!(length = strlen(file)) || strlen(file) >= 64)
					return;
			}
			else
			{
				strcpy(file, name);
			}

			Attacher attacher;
			int i;
			for (i = 0; i < sizeof(attacher.mat) / sizeof(attacher.mat[0]); ++i)
			{
				p = GetStrConst(p, bone);
				if (!(length = strlen(bone)) || strlen(bone) >= 64)
					break;

				WBone* pBone = m_pPet->FindBone(bone);
				if (pBone)
				{
					attacher.mat[i] = &pBone->GetMatrix();
				}
				else
				{
					w_bound_box* box = m_pPet->FindBoundBox(bone);
					if (box)
					{
						if (box->bone)
							attacher.mat[i] = &box->bone->GetMatrix();
						attacher.pos[i] = &box->info->spherePivot;
					}
				}
			}
			if (i > 0)
				CFx::Instance()->Attach(attacher, file, true, NULL);
		}
	}
}

void CCreature::CheckTarget(float dt)
{
	if (WVectorLen(m_mat.pivot - m_target) < 1.0f)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			int pos[3], cell;
			int count = 0;
			do
			{
				++count;

				int delta[3];
				for (int i = 0; i < 3; ++i)
					delta[i] = rand() % 3 - 1;

				int last = m_pHistory[0];
				int prev[3];
				prev[0] = last % m_cellNumX;
				prev[1] = last / (m_cellNumZ * m_cellNumX);
				prev[2] = last % (m_cellNumZ * m_cellNumX) / m_cellNumX;
				pos[0] = Between(0, prev[0] + delta[0], m_cellNumX - 1);
				pos[1] = Between(0, prev[1] + delta[1], m_cellNumY - 1);
				pos[2] = Between(0, prev[2] + delta[2], m_cellNumZ - 1);
				cell = pos[0] + m_cellNumX * (pos[2] + m_cellNumZ * pos[1]);

				bool bVisited = false;
				for (int j = 0; j < m_historyNum; ++j)
				{
					if (cell == m_pHistory[j])
					{
						bVisited = true;
						break;
					}
				}
				if (!bVisited)
					break;
			} while (count < 5);

			for (int i = m_historyNum - 1; i >= 1; --i)
				m_pHistory[i] = m_pHistory[i - 1];
			m_pHistory[0] = cell;

			m_target.x =
				Random(0.0f, m_cellLenX) + pos[0] * m_cellLenX + m_area.min.x;
			if (m_cellNumY > 1)
				m_target.y = Random(0.0f, m_cellLenY) + pos[1] * m_cellLenY +
					m_area.min.y;
			m_target.z =
				Random(0.0f, m_cellLenZ) + pos[2] * m_cellLenZ + m_area.min.z;

			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			m_target = m_recordList[m_recordIndex];
			++m_recordIndex;
		}
	}

	if (m_cellNumY > 1)
	{
		m_mat.pivot += m_speed * (m_target - m_mat.pivot).Normalize() * dt;
	}
	else
	{
		float angle = CalcDeltaAngle(m_target - m_mat.pivot, -1.0f * m_mat.za);
		if (Abs(angle) > g_DEGTORAD)
		{
			if (Abs(angle) > m_rollSpeed)
			{
				if (Abs(m_roll) < m_maxRoll)
					m_roll -= angle * dt;
			}
			else
			{
				if (Abs(m_roll) > Abs(angle * dt))
					m_roll += (angle > 0.0f ? m_rollSpeed : -m_rollSpeed) * dt;
				else
					m_roll = 0.0f;
			}

			m_mat =
				RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
				m_mat;
		}
		else if (Abs(m_roll) > 0.0f)
		{
			if (Abs(m_roll) > dt * m_rollSpeed)
				m_roll += (m_roll > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
			else
				m_roll = 0.0f;
		}

		m_mat.pivot += -1.0f * m_mat.za * m_speed * dt;
	}
}

void CCreature::PlayEvent()
{
	m_time = 0.0f;
	m_bActive = true;
	m_alpha = 0xff;

	m_mat = RotMat(Random(-g_PI, g_PI), 1);
	m_mat.pivot = GetPVS().GetGroundPoint(
		m_radius * m_mat.za + GolfBall().m_pos, false, NULL, true);
}

void CCreature::Record()
{
	m_recordList.clear();
	m_recordList.reserve(10);
	m_recordList.push_back(m_target);
	m_bRecord = true;
	m_recTime = m_time;
	m_recMat = m_mat;
	m_recRoll = m_roll;
	m_bRecActive = m_bActive;
}

void CCreature::Replay()
{
	if (m_flag & 1)
	{
		m_bActive = false;
		return;
	}

	m_target = m_recordList[0];
	m_recordIndex = 1;
	m_bRecord = false;
	m_time = m_recTime;
	m_mat = m_recMat;
	m_roll = m_recRoll;
	m_bActive = m_bRecActive;
}

CGull::CGull(const char* name)
{
	m_speed = 10.0f;
	Init(name, 0);
	m_maxRoll = g_PI / 4.0f;
	m_cellSize = 48.0f;
}

CGull::~CGull()
{
}

CButterfly::CButterfly(const char* name)
{
	m_speed = 7.0f;
	Init(name, 0);
	m_maxRoll = 0.0f;
	m_cellSize = 16.0f;
}

CButterfly::~CButterfly()
{
}

CDuck::CDuck(const char* name, int num)
{
	m_pMat = NULL;
	m_speed = 2.0f;
	Init(name, 0);
	m_rotSpeed = g_PI / 6.0f;
	m_maxRoll = 0.0f;
	m_cellSize = 32.0f;
	m_num = num;
	if (num > 0)
		m_pMat = new WMatrix[num];
}

void CDuck::SetArea(const Waabb& area, unsigned long flag)
{
	CCreature::SetArea(area, flag);
	if (!m_num)
		return;

	m_pMat[0] = m_mat;
	for (int i = 1; i < m_num; ++i)
	{
		m_pMat[i] = m_pMat[i - 1];
		m_pMat[i].pivot = m_pMat[i - 1].za * 6.4f + m_pMat[i - 1].pivot;
	}
}

void CDuck::Process(float dt)
{
	CheckTarget(dt);
	if (!m_num)
		return;

	m_pMat[0] = m_mat;
	for (int i = 1; i < m_num; ++i)
	{
		WVector dist;
		dist = m_pMat[i - 1].pivot - m_pMat[i].pivot;
		if (dist.Magnitude() < 3.2f)
			continue;

		float angle = Between(dt * m_rotSpeed * -0.7f,
			CalcDeltaAngle(m_pMat[i - 1].pivot - m_pMat[i].pivot,
				m_pMat[i].za * -1.0f),
			dt * m_rotSpeed * 0.7f);
		m_pMat[i] = RotMat(angle, 1) * m_pMat[i];
		m_pMat[i].pivot += -1.0f * m_pMat[i].za * m_speed * dt;
	}
}

void CDuck::Display()
{
	if (!m_num)
	{
		CCreature::Display();
		return;
	}

	for (int i = 0; i < m_num; ++i)
	{
		m_pPet->ApplyBones(NULL, m_pMat[i] * (i == 0 ? 1.0f : 0.5f));
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();
		if (g_view->InFrustum(m_pPet->GetBoundSphere()))
		{
			m_pPet->UpdateLightSource(&g_lightset, false, NULL);
			m_pPet->Render(g_view, false, 1.0f,
				g_view->InFrustumSafe(m_pPet->GetBoundBox()), true, false);
		}
	}
}

CMole::CMole(const char* name)
{
	unsigned long& flag = m_flag;
	flag |= 1;
	Init(name, 0);
	m_radius = 6.4f;
}

void CMole::PlayEvent()
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->bunker >= 1)
	{
		CCreature::PlayEvent();
		if (GetPVS().GetGroundType(m_mat.pivot, true, NULL) != 3)
		{
			m_mat.za = GolfBall().m_pos - 0.5f * (m_area.max + m_area.min);
			m_mat.za.y = 0.0f;
			m_mat.za.Normalize();
			m_mat.xa = WCrossProduct(m_mat.ya, m_mat.za);
			m_mat.xa.Normalize();
			m_mat.pivot = GetPVS().GetGroundPoint(
				m_radius * m_mat.za + GolfBall().m_pos, false, NULL, true);
		}
	}
}

CDolphin::CDolphin(const char* name)
{
	unsigned long& flag = m_flag;
	flag |= 1;
	Init(name, 0);
	m_radius = 0.0f;
}

void CDolphin::PlayEvent()
{
	if (GolfBall().m_bOB && GolfBall().m_water == CGolfBall::WATER_SEA)
		CCreature::PlayEvent();
}

CCrab::CCrab(const char* name)
{
	unsigned long& flag = m_flag;
	flag |= 1;
	Init(name, 0);
	m_radius = -3.2f;
}

void CCrab::PlayEvent()
{
	if (PLAYER(GOLFDOC()->m_currentPlayer)->bunker >= 1)
		CCreature::PlayEvent();
}

CSparrow::CSparrow(const char* name, int num)
{
	m_pMat = NULL;
	m_pPet = g_resrcmng->GetPuppet(name, false, true, false);
	m_roll = 0.0f;
	m_bRecord = false;
	m_alpha = 0xff;
	m_bActive = false;
	m_maxNum = num;
	m_pMat = new WMatrix[num];
}

void CSparrow::PlayEvent()
{
	m_bActive = true;

	float speed = GolfBall().m_vel.Magnitude();
	m_num = (int)(m_maxNum / GolfBall().PreData(0)->vel.Magnitude() * speed);
	for (int i = 0; i < m_num; ++i)
	{
		m_pMat[i] = RotMat(Random(-180.0f, 180.0f) * g_DEGTORAD, 1);
		const WVector& max = m_area.max;
		m_pMat[i].pivot = (m_area.min + max) * 0.5f;
	}
}

void CSparrow::Display()
{
	if (!m_pPet || !m_bActive)
		return;

	for (int i = 0; i < m_num; ++i)
	{
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false),
			m_pMat[i]);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();
		if (g_view->InFrustum(m_pPet->GetBoundSphere()))
		{
			m_pPet->UpdateLightSource(&g_lightset, false, NULL);
			m_pPet->Render(g_view, false, 1.0f,
				g_view->InFrustumSafe(m_pPet->GetBoundBox()), true, false);
		}
	}
}

CSanta::CSanta(const char* name)
{
	Init(name, 0);
	m_nodeIndex = 1;
	m_speed = 120.0f;
	m_maxRoll = g_PI / 4.0f;
	m_rotSpeed = g_PI * 2.0f / 3.0f;
	m_velY = 0.0f;
	m_rollSpeed = g_PI / 2.0f;
	m_phase = 2;
}

CSanta::~CSanta()
{
}

void CSanta::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		m_time -= m_pMotion->e;
		if (rand() % 10 == 0 && !strcmpi(m_pMotion->name, "NPC_Default"))
			SetMotion("NPC_Default1", 0.0f);
		else
			SetMotion("NPC_Default", 0.0f);
	}
	CheckTarget(dt);
}

void CSanta::CheckTarget(float dt)
{
	WVector dir = m_target - m_mat.pivot;
	dir.y = 0.0f;

	if (dir.Magnitude() < Max(dt, 0.1f) * m_speed)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			if (m_nodeIndex == m_nodeList.size() - 1)
			{
				m_bActive = false;
				return;
			}

			unsigned char prev = m_nodeIndex;
			m_nodeIndex = (m_nodeIndex + 1) % m_nodeList.size();
			m_target = m_nodeList[m_nodeIndex];
			m_target += m_radius *
				RotMat(Random(-g_PI / 2.0f, g_PI / 2.0f, -1), 0).za *
				RotMat(Random(-g_PI, g_PI, -1), 1);

			dir = m_target - m_mat.pivot;
			dir.y = 0.0f;
			WVector node = m_nodeList[m_nodeIndex] - m_nodeList[prev];
			node.y = 0.0f;
			float distance = dir.Magnitude();
			m_speed = distance * 120.0f / node.Magnitude();
			m_velY = (m_target.y - m_mat.pivot.y) / distance * m_speed;
			m_maxRoll =
				Min(Abs(CalcDeltaAngle(dir, m_mat.za * -1.0f)), g_PI / 2.0f);
			m_phase = 1;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			if (m_recordIndex < m_recordList.size())
			{
				m_target = m_recordList[m_recordIndex];
				++m_recordIndex;
			}
			else
			{
				m_bActive = false;
			}
		}
	}

	float angle = CalcDeltaAngle(dir, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	switch (m_phase)
	{
	case 0:
		if (Abs(m_roll) > dt * m_rollSpeed)
		{
			m_roll += (m_roll > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		}
		else
		{
			m_roll = 0.0f;
			m_phase = 2;
		}
		break;
	case 1:
		if (Abs(m_roll) < m_maxRoll)
			m_roll += (angle > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		else
			m_phase = 0;
		break;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot.y += dt * m_velY;

	float distance = (g_camera.pivot - m_mat.pivot).Magnitude();
	if (distance < 640.0f)
	{
		if (!g_audio->IsPlaying("Santa"))
			g_audio->PlaySfx("santaboost", NULL, 1, "Santa");
		g_audio->SetSFXVolumeWhilePlaying("Santa",
			(640.0f - distance) / 640.0f);
	}
	else
	{
		g_audio->StopSfx("Santa");
	}
}

CFighter::CFighter(const char* name)
{
	Init(name, 0);
	m_nodeIndex = 1;
	m_speed = 320.0f;
	m_maxRoll = g_PI / 4.0f;
	m_rotSpeed = g_PI * 2.0f / 3.0f;
	m_velY = 0.0f;
	m_rollSpeed = g_PI / 2.0f;
	m_phase = 2;
}

CFighter::~CFighter()
{
}

void CFighter::CheckTarget(float dt)
{
	WVector dir = m_target - m_mat.pivot;
	dir.y = 0.0f;

	if (dir.Magnitude() < Max(dt, 0.1f) * m_speed)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			if (m_nodeIndex == m_nodeList.size() - 1)
			{
				m_bActive = false;
				return;
			}

			m_nodeIndex = (m_nodeIndex + 1) % m_nodeList.size();
			m_target = m_nodeList[m_nodeIndex];
			dir = m_target - m_mat.pivot;
			dir.y = 0.0f;
			float distance = dir.Magnitude();
			m_speed = distance;
			m_velY = (m_target.y - m_mat.pivot.y) / distance * m_speed;
			m_maxRoll =
				Min(Abs(CalcDeltaAngle(dir, m_mat.za * -1.0f)), g_PI / 2.0f);
			m_phase = 1;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			if (m_recordIndex < m_recordList.size())
			{
				m_target = m_recordList[m_recordIndex];
				++m_recordIndex;
			}
			else
			{
				m_bActive = false;
			}
		}
	}

	float angle = CalcDeltaAngle(dir, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	switch (m_phase)
	{
	case 0:
		if (Abs(m_roll) > dt * m_rollSpeed)
		{
			m_roll += (m_roll > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		}
		else
		{
			m_roll = 0.0f;
			m_phase = 2;
		}
		break;
	case 1:
		if (Abs(m_roll) < m_maxRoll)
			m_roll += (angle > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		else
			m_phase = 0;
		break;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot.y += dt * m_velY;
}

CUfo::CUfo(const char* name)
{
	Init(name, 0);
	m_nodeIndex = 1;
	m_speed = 2.857143f;
	m_maxRoll = g_PI / 4.0f;
	m_rotSpeed = g_PI * 2.0f / 3.0f;
	m_velY = 0.0f;
	m_rollSpeed = g_PI / 2.0f;
	m_phase = 2;
}

CUfo::~CUfo()
{
}

void CUfo::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		m_time -= m_pMotion->e;
		SetMotion("NPC_Default", 0.0f);
	}
	CheckTarget(dt);
}

void CUfo::CheckTarget(float dt)
{
	WVector dir = m_target - m_mat.pivot;
	dir.y = 0.0f;

	if (dir.Magnitude() < Max(dt, 0.1f) * m_speed)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			if (m_nodeIndex == m_nodeList.size() - 1)
			{
				m_bActive = false;
				return;
			}

			unsigned char prev = m_nodeIndex;
			m_nodeIndex = (m_nodeIndex + 1) % m_nodeList.size();
			m_target = m_nodeList[m_nodeIndex];
			m_target += m_radius *
				RotMat(Random(-g_PI / 2.0f, g_PI / 2.0f, -1), 0).za *
				RotMat(Random(-g_PI, g_PI, -1), 1);

			dir = m_target - m_mat.pivot;
			dir.y = 0.0f;
			WVector node = m_nodeList[m_nodeIndex] - m_nodeList[prev];
			node.y = 0.0f;
			float distance = dir.Magnitude();
			m_speed = distance * 120.0f / node.Magnitude();
			m_velY = (m_target.y - m_mat.pivot.y) / distance * m_speed;
			m_maxRoll =
				Min(Abs(CalcDeltaAngle(dir, m_mat.za * -1.0f)), g_PI / 2.0f);
			m_phase = 1;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			if (m_recordIndex < m_recordList.size())
			{
				m_target = m_recordList[m_recordIndex];
				++m_recordIndex;
			}
			else
			{
				m_bActive = false;
			}
		}
	}

	float angle = CalcDeltaAngle(dir, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	switch (m_phase)
	{
	case 0:
		if (Abs(m_roll) > dt * m_rollSpeed)
		{
			m_roll += (m_roll > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		}
		else
		{
			m_roll = 0.0f;
			m_phase = 2;
		}
		break;
	case 1:
		if (Abs(m_roll) < m_maxRoll)
			m_roll += (angle > 0.0f ? -m_rollSpeed : m_rollSpeed) * dt;
		else
			m_phase = 0;
		break;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot.y += dt * m_velY;
}

ntPenguin::ntPenguin(const char* name)
{
	Init(name, 0);
	m_nodeIndex = 1;
	m_speed = 2.857143f;
	m_maxRoll = g_PI / 4.0f;
	m_rotSpeed = g_PI * 2.0f / 3.0f;
	m_velY = 0.0f;
	m_rollSpeed = g_PI / 2.0f;
	m_phase = 2;
}

ntPenguin::~ntPenguin()
{
}

void ntPenguin::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		m_time -= m_pMotion->e;
		SetMotion("NPC_Default", 0.0f);
	}
	CheckTarget(dt);
}

void ntPenguin::CheckTarget(float dt)
{
	WVector dir = m_target - m_mat.pivot;
	dir.y = 0.0f;

	if (dir.Magnitude() < Max(dt, 0.1f) * m_speed)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			if (m_nodeIndex == m_nodeList.size() - 1)
				m_nodeIndex = 0;

			m_nodeIndex = (m_nodeIndex + 1) % m_nodeList.size();
			m_target = m_nodeList[m_nodeIndex];
			m_target += m_radius *
				RotMat(Random(-g_PI / 2.0f, g_PI / 2.0f, -1), 0).za *
				RotMat(Random(-g_PI, g_PI, -1), 1);

			dir = m_target - m_mat.pivot;
			dir.y = 0.0f;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			if (m_recordIndex < m_recordList.size())
			{
				m_target = m_recordList[m_recordIndex];
				++m_recordIndex;
			}
			else
			{
				m_recordIndex = 0;
			}
		}
	}

	float angle = CalcDeltaAngle(dir, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot.y += dt * m_velY;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

ntPenguin_Sailor::ntPenguin_Sailor(const char* name)
{
	m_speed = 2.0f;
	Init(name, 0);
	m_maxRoll = g_PI / 4.0f;
	m_cellSize = 48.0f;
	SetMotion("NPC_Act0", 0.0f);
	m_scale = Random(0.7f, 1.0f, -1);
	m_bWalk = true;
}

ntPenguin_Sailor::~ntPenguin_Sailor()
{
}

void ntPenguin_Sailor::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		if (m_flag & 1)
		{
			m_bActive = false;
			m_time = m_pMotion->s;
		}
		else
		{
			m_time -= m_pMotion->e - m_pMotion->s;
			switch (rand() % 10)
			{
			case 0:
				SetMotion("NPC_Act1", 0.0f);
				m_bWalk = false;
				break;
			case 1:
				SetMotion("NPC_Default", 0.0f);
				m_bWalk = false;
				break;
			default:
				SetMotion("NPC_Act0", 0.0f);
				m_bWalk = true;
				break;
			}
		}
	}

	if (m_flag & 1)
	{
		for (std::list<sAlpha>::iterator it = m_alphaList.begin();
			it != m_alphaList.end();)
		{
			std::list<sAlpha>::iterator prev = it;
			++it;
			if (it == m_alphaList.end())
			{
				m_alpha = (unsigned char)(*prev).alpha;
			}
			else if (m_time > (*prev).time && m_time < (*it).time)
			{
				float ratio =
					(m_time - (*prev).time) / ((*it).time - (*prev).time);
				m_alpha = (unsigned char)((1.0f - ratio) * (*prev).alpha +
					ratio * (*it).alpha);
				break;
			}
		}
	}
	else if (m_bWalk)
	{
		CheckTarget(dt);
	}
}

void ntPenguin_Sailor::Display()
{
	if (!COption::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 19)
		return;

	if (m_pPet && m_bActive && !(m_time < 0.0f))
	{
		if (m_flag & 1)
			m_pPet->SetAlpha(m_alpha);
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false),
			Abs(m_roll) > 0.0f ? RotMat(m_roll, 2) * m_mat * m_scale
							   : m_mat * m_scale);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();
		if (g_view->InFrustum(m_pPet->GetBoundSphere()))
		{
			m_pPet->UpdateLightSource(&g_lightset, false, NULL);
			m_pPet->Render(g_view, true, 1.0f,
				g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

void ntPenguin_Sailor::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
	{
		if (m_recordIndex < m_recordList.size() && !m_bRecord)
		{
			m_target = m_recordList[m_recordIndex];
			++m_recordIndex;
		}
		else
		{
			int tries = 0;
			int cell[3];
			int index;
			int i;
			bool bFound;
			do
			{
				++tries;
				int delta[3];
				for (i = 0; i < 3; ++i)
					delta[i] = rand() % 3 - 1;

				int pos[3];
				pos[0] = m_pHistory[0] % m_cellNumX;
				pos[1] = m_pHistory[0] / (m_cellNumZ * m_cellNumX);
				pos[2] = m_pHistory[0] % (m_cellNumZ * m_cellNumX) / m_cellNumX;

				cell[0] = Between(0, pos[0] + delta[0], m_cellNumX - 1);
				cell[1] = Between(0, pos[1] + delta[1], m_cellNumY - 1);
				cell[2] = Between(0, pos[2] + delta[2], m_cellNumZ - 1);
				index = (m_cellNumZ * cell[1] + cell[2]) * m_cellNumX + cell[0];

				bFound = false;
				for (i = 0; i < m_historyNum; ++i)
				{
					if (index == m_pHistory[i])
					{
						bFound = true;
						break;
					}
				}
			} while (bFound && tries < 5);

			for (i = m_historyNum - 1; i >= 1; --i)
				m_pHistory[i] = m_pHistory[i - 1];
			m_pHistory[0] = index;

			m_target.x = Random(0.0f, m_cellLenX, -1) + cell[0] * m_cellLenX +
				m_area.min.x;
			if (m_cellNumY > 1)
			{
				m_target.y = Random(0.0f, m_cellLenY, -1) +
					cell[1] * m_cellLenY + m_area.min.y;
			}
			m_target.z = Random(0.0f, m_cellLenZ, -1) + cell[2] * m_cellLenZ +
				m_area.min.z;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
	}

	if (m_cellNumY > 1)
	{
		m_mat.pivot += m_speed * (m_target - m_mat.pivot).Normalize() * dt;
		return;
	}

	float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

ntPenguin_Duck::ntPenguin_Duck(const char* name, int num)
	: m_pMat(NULL)
{
	m_speed = 2.0f;
	Init(name, 0);
	m_rotSpeed = g_PI / 6.0f;
	m_maxRoll = 0.0f;
	m_cellSize = 32.0f;
	m_num = num;
	if (m_num > 0)
	{
		m_pMat = new WMatrix[m_num];
		m_pScale = new float[m_num];
		m_pScale[0] = 1.0f;
		for (int i = 1; i < m_num; ++i)
			m_pScale[i] = Random(0.6f, 0.8f, -1);
	}
	SetMotion("NPC_Act0", 0.0f);
	m_bWalk = false;
}

ntPenguin_Duck::~ntPenguin_Duck()
{
	if (m_pMat)
	{
		delete[] m_pMat;
		m_pMat = NULL;
	}
	if (m_pScale)
	{
		delete[] m_pScale;
		m_pScale = NULL;
	}
}

void ntPenguin_Duck::SetArea(const Waabb& area, unsigned long flag)
{
	CCreature::SetArea(area, flag);

	if (m_num)
	{
		m_pMat[0] = m_mat;
		for (int i = 1; i < m_num; ++i)
		{
			m_pMat[i] = m_pMat[i - 1];
			m_pMat[i].pivot = m_pMat[i - 1].za * 6.4f + m_pMat[i - 1].pivot;
		}
	}
}

void ntPenguin_Duck::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		if (m_flag & 1)
		{
			m_bActive = false;
			m_time = m_pMotion->s;
		}
		else
		{
			m_time -= m_pMotion->e - m_pMotion->s;
			SetMotion("NPC_Act0", 0.0f);
			switch (rand() % 10)
			{
			case 0:
				SetMotion("NPC_Act1", 0.0f);
				m_bWalk = false;
				break;
			default:
				SetMotion("NPC_Act0", 0.0f);
				m_bWalk = true;
				break;
			}
		}
	}

	if (m_bWalk)
		CheckTarget(dt);

	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);

	if (!m_num)
		return;
	if (!m_bWalk)
		return;

	m_pMat[0] = m_mat;
	for (int i = 1; i < m_num; ++i)
	{
		WVector dir;
		dir = m_pMat[i - 1].pivot - m_pMat[i].pivot;
		if (dir.Magnitude() < 4.16f)
			continue;

		float angle = Between(dt * m_rotSpeed * -0.7f,
			CalcDeltaAngle(m_pMat[i - 1].pivot - m_pMat[i].pivot,
				m_pMat[i].za * -1.0f),
			dt * m_rotSpeed * 0.7f);
		m_pMat[i] = RotMat(angle, 1) * m_pMat[i];
		m_pMat[i].pivot += m_pMat[i].za * -1.0f * m_speed * dt;
		m_pMat[i].pivot =
			GetPVS().GetGroundPoint(m_pMat[i].pivot, true, NULL, false);
	}
}

void ntPenguin_Duck::Display()
{
	if (!COption::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 19)
		return;

	if (m_pPet && m_bActive && !(m_time < 0.0f))
	{
		if (m_num == 0)
		{
			CCreature::Display();
		}
		else
		{
			for (int i = 0; i < m_num; ++i)
			{
				m_pPet->ApplyBones(
					m_pPet->GetBones(NULL, m_time, 0, NULL, false),
					m_pMat[i] * m_pScale[i]);
				m_pPet->Transform(g_view, 1.0f, 0);
				m_pPet->UpdateBSphere();
				if (g_view->InFrustum(m_pPet->GetBoundSphere()))
				{
					m_pPet->UpdateLightSource(&g_lightset, false, NULL);
					m_pPet->Render(g_view, false, 1.0f,
						g_view->InFrustumSafe(m_pPet->GetBoundBox()), true,
						false);
				}
			}
		}
	}
}

ntNpc_Dragon::ntNpc_Dragon(const char* name, float delay)
{
	Init(name, 0);
	m_t = 0.0f;
	m_delay = delay;
	m_bWait = true;
}

ntNpc_Dragon::~ntNpc_Dragon()
{
}

void ntNpc_Dragon::Process(float dt)
{
	if (m_bActive && m_pPet)
	{
		m_time += dt;
		if (m_bWait)
		{
			if (!(m_time > m_delay))
				return;
			m_time = 0.0f;
			m_bWait = false;
			SetMotion("NPC_Act02", -1.0f);
		}

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
		{
			int rate = rand() % 101;
			if (rate < 5)
				SetMotion("NPC_Act02", 0.0f);
			else if (rate < 10)
				SetMotion("NPC_Act01", 0.0f);
			else
				SetMotion("NPC_Default", 0.0f);
		}

		m_t += dt * 100.0f;
		CheckTarget(dt);
	}
}

void ntNpc_Dragon::Display()
{
	if (!m_bActive || m_bWait == true)
		return;

	CCreature::Display();
}

void ntNpc_Dragon::CheckTarget(float dt)
{
	WVector target = m_spline.Evaluate(m_t + 0.1f);
	m_mat.pivot = m_spline.Evaluate(m_t);

	if (WisEqual(target, m_nodeList[m_nodeList.size() - 1], 1.0e-5f))
		m_t = 0.0f;

	m_mat = MakeCameraMatrix(target, m_mat.pivot);
}

void ntNpc_Dragon::SetArea(const std::vector<WVector>& node, float time,
	unsigned long flag)
{
	CCreature::SetArea(node, time, flag);

	WVector* point = (WVector*)_alloca(node.size() * sizeof(WVector));
	float* length = (float*)_alloca(node.size() * sizeof(float));

	point[0] = node[0];
	length[0] = 0.0f;

	for (unsigned int i = 1; i < node.size(); ++i)
	{
		point[i] = node[i];
		length[i] = length[i - 1] + (node[i] - node[i - 1]).Magnitude();
	}

	m_spline.Init(point, length, node.size());
}

ntNpc_BlackSparrow::ntNpc_BlackSparrow(const char* name)
{
	m_speed = 5.5f;
	Init(name, 0);
	m_maxRoll = 0.78539819f;
	m_cellSize = 48.0f;
	SetMotion("NPC_Default", 0.0f);
	m_bFly = true;
}

ntNpc_BlackSparrow::~ntNpc_BlackSparrow()
{
}

void ntNpc_BlackSparrow::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);

	if (m_time > m_pMotion->e)
	{
		if (m_flag & 1)
		{
			m_bActive = false;
			m_time = m_pMotion->s;
		}
		else
		{
			m_time -= m_pMotion->e - m_pMotion->s;

			if (rand() % 101 < 20)
			{
				SetMotion("NPC_Act01", 0.0f);
				m_bFly = false;
			}
			else
			{
				SetMotion("NPC_Default", 0.0f);
				m_bFly = true;
			}
		}
	}

	if (m_flag & 1)
	{
		for (std::list<sAlpha>::iterator it = m_alphaList.begin();
			it != m_alphaList.end();)
		{
			std::list<sAlpha>::iterator prev = it;
			++it;
			if (it == m_alphaList.end())
			{
				m_alpha = (unsigned char)(*prev).alpha;
			}
			else if (m_time > (*prev).time && m_time < (*it).time)
			{
				float ratio =
					(m_time - (*prev).time) / ((*it).time - (*prev).time);
				m_alpha = (unsigned char)((1.0f - ratio) * (*prev).alpha +
					ratio * (*it).alpha);
				break;
			}
		}
	}
	else if (m_bFly)
	{
		CheckTarget(dt);
	}
}

void ntNpc_BlackSparrow::Display()
{
	if (!COption::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 19)
		return;

	if (m_pPet && m_bActive && !(m_time < 0.0f))
	{
		if (m_flag & 1)
			m_pPet->SetAlpha(m_alpha);
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false),
			Abs(m_roll) > 0.0f ? RotMat(m_roll, 2) * m_mat * 1.0f
							   : m_mat * 1.0f);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();
		if (g_view->InFrustum(m_pPet->GetBoundSphere()))
		{
			m_pPet->UpdateLightSource(&g_lightset, false, NULL);
			m_pPet->Render(g_view, true, 1.0f,
				g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

void ntNpc_BlackSparrow::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
	{
		if (m_recordIndex < m_recordList.size() && !m_bRecord)
		{
			m_target = m_recordList[m_recordIndex];
			++m_recordIndex;
		}
		else
		{
			int tries = 0;
			int cell[3];
			int index;
			int i;
			bool bFound;
			do
			{
				++tries;
				int delta[3];
				for (i = 0; i < 3; ++i)
					delta[i] = rand() % 3 - 1;

				int pos[3];
				pos[0] = m_pHistory[0] % m_cellNumX;
				pos[1] = m_pHistory[0] / (m_cellNumZ * m_cellNumX);
				pos[2] = m_pHistory[0] % (m_cellNumZ * m_cellNumX) / m_cellNumX;

				cell[0] = Between(0, pos[0] + delta[0], m_cellNumX - 1);
				cell[1] = Between(0, pos[1] + delta[1], m_cellNumY - 1);
				cell[2] = Between(0, pos[2] + delta[2], m_cellNumZ - 1);
				index = (m_cellNumZ * cell[1] + cell[2]) * m_cellNumX + cell[0];

				bFound = false;
				for (i = 0; i < m_historyNum; ++i)
				{
					if (index == m_pHistory[i])
					{
						bFound = true;
						break;
					}
				}
			} while (bFound && tries < 5);

			for (i = m_historyNum - 1; i >= 1; --i)
				m_pHistory[i] = m_pHistory[i - 1];
			m_pHistory[0] = index;

			m_target.x = Random(0.0f, m_cellLenX, -1) + cell[0] * m_cellLenX +
				m_area.min.x;
			if (m_cellNumY > 1)
			{
				m_target.y = Random(0.0f, m_cellLenY, -1) +
					cell[1] * m_cellLenY + m_area.min.y;
			}
			m_target.z = Random(0.0f, m_cellLenZ, -1) + cell[2] * m_cellLenZ +
				m_area.min.z;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
	}

	if (m_cellNumY > 1)
	{
		m_mat.pivot += m_speed * (m_target - m_mat.pivot).Normalize() * dt;
		return;
	}

	float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

ntNpc_PolaShip::ntNpc_PolaShip(const char* name, float delay)
{
	Init(name, 0);
	m_t = 0.0f;
	m_delay = delay;
	m_bWait = true;
}

ntNpc_PolaShip::~ntNpc_PolaShip()
{
}

void ntNpc_PolaShip::Process(float dt)
{
	if (m_bActive && m_pPet)
	{
		m_time += dt;
		if (m_bWait)
		{
			if (!(m_time > m_delay))
				return;
			m_time = 0.0f;
			m_bWait = false;
			SetMotion("NPC_Act02", -1.0f);
		}

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
			m_time -= m_pMotion->e - m_pMotion->s;

		m_t += dt * 10.0f;
		CheckTarget(dt);
	}
}

void ntNpc_PolaShip::Display()
{
	if (!m_bActive || m_bWait == true)
		return;

	CCreature::Display();
}

void ntNpc_PolaShip::CheckTarget(float dt)
{
	WVector target = m_spline.Evaluate(m_t + 0.1f);
	m_mat.pivot = m_spline.Evaluate(m_t);

	if (WisEqual(target, m_nodeList[m_nodeList.size() - 1], 1.0e-5f))
		m_t = 0.0f;

	target.y = m_mat.pivot.y;
	m_mat = MakeCameraMatrix(target, m_mat.pivot);
}

void ntNpc_PolaShip::SetArea(const std::vector<WVector>& node, float time,
	unsigned long flag)
{
	CCreature::SetArea(node, time, flag);

	WVector* point = (WVector*)_alloca(node.size() * sizeof(WVector));
	float* length = (float*)_alloca(node.size() * sizeof(float));

	point[0] = node[0];
	length[0] = 0.0f;

	for (unsigned int i = 1; i < node.size(); ++i)
	{
		point[i] = node[i];
		length[i] = length[i - 1] + (node[i] - node[i - 1]).Magnitude();
	}

	m_spline.Init(point, length, node.size());
}

ntNpc_PolaPenguin1::ntNpc_PolaPenguin1(const char* name, float speed)
{
	m_speed = speed;
	Init(name, 0);
}

ntNpc_PolaPenguin1::~ntNpc_PolaPenguin1()
{
}

void ntNpc_PolaPenguin1::Process(float dt)
{
	if (m_pPet && m_bActive)
	{
		m_time += dt;
		if (m_time < 0.0f)
			return;

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
		{
			if (m_flag & 1)
			{
				m_bActive = false;
				m_time = m_pMotion->s;
			}
			else
			{
				m_time -= m_pMotion->e - m_pMotion->s;
			}
		}

		CheckTarget(dt);
	}
}

void ntNpc_PolaPenguin1::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
		FindTarget();

	float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-(dt * m_rotSpeed), angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

void ntNpc_PolaPenguin1::FindTarget()
{
	for (int i = 0; i < 3; ++i)
	{
		float rate = Random(0.0f, 1.0f);
		m_target.p[i] =
			m_area.min.p[i] * rate + m_area.max.p[i] * (1.0f - rate);
	}

	m_target = GetPVS().GetGroundPoint(m_target, false, NULL, false);
}

ntNpc_PolaPenguin2::ntNpc_PolaPenguin2(const char* name)
{
	m_speed = 2.0f;
	Init(name, 0);
	m_bWalk = true;
	SetMotion("NPC_Act0", 0.0f);
}

ntNpc_PolaPenguin2::~ntNpc_PolaPenguin2()
{
}

void ntNpc_PolaPenguin2::Process(float dt)
{
	if (m_pPet && m_bActive)
	{
		m_time += dt;
		if (m_time < 0.0f)
			return;

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
		{
			if (m_flag & 1)
			{
				m_bActive = false;
				m_time = m_pMotion->s;
			}
			else
			{
				m_time -= m_pMotion->e - m_pMotion->s;

				if (m_bWalk == true && IsSlidable() == true)
				{
					SetMotion("NPC_Default", 0.0f);
					m_bWalk = false;
				}
				else
				{
					SetMotion("NPC_Act0", 0.0f);
					m_bWalk = true;
				}
			}
		}

		CheckTarget(dt);
	}
}

void ntNpc_PolaPenguin2::Display()
{
	if (!COption::Instance()->vIsNpcEnabled() && GOLFDOC()->m_currentHole != 19)
		return;

	if (!m_pPet || !m_bActive || m_time < 0.0f)
		return;

	if (m_flag & 1)
		m_pPet->SetAlpha(m_alpha);

	WVector delta = m_pPet->GetDeltaVec(m_pMotion->name, m_time);
	delta = RotVec(delta, m_mat);

	m_mat.pivot -= delta;
	m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false), m_mat);
	m_pPet->Transform(g_view, 1.0f, 0);
	m_pPet->UpdateBSphere();
	m_mat.pivot += delta;

	if (g_view->InFrustum(m_pPet->GetBoundSphere()))
	{
		m_pPet->UpdateLightSource(&g_lightset, false, NULL);
		m_pPet->Render(g_view, true, 1.0f,
			g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
	}
}

void ntNpc_PolaPenguin2::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
		FindTarget();

	if (m_bWalk)
	{
		float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
		if (Abs(angle) > g_DEGTORAD)
		{
			m_mat =
				RotMat(Between(-(dt * m_rotSpeed), angle, dt * m_rotSpeed), 1) *
				m_mat;
		}

		m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
	}
	else
	{
		WVector delta;
		delta = m_pPet->GetDeltaVec(m_pMotion->name, m_time - dt, m_time);
		delta = RotVec(delta, m_mat);
		m_mat.pivot += delta;
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
	}
}

void ntNpc_PolaPenguin2::FindTarget()
{
	for (int i = 0; i < 3; ++i)
	{
		float rate = Random(0.0f, 1.0f);
		m_target.p[i] =
			m_area.min.p[i] * rate + m_area.max.p[i] * (1.0f - rate);
	}

	const WVector& target = m_target;
	while (m_area.IsInclude(target))
		m_target += (m_target - m_mat.pivot).Normalize() * 3.2f;

	m_target = GetPVS().GetGroundPoint(m_target, true, NULL, false);
}

bool ntNpc_PolaPenguin2::IsSlidable()
{
	float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
		return false;

	w_motion_data* motion = m_pPet->FindMotionData("NPC_Default");
	if (!motion)
		return false;

	WVector dir = m_target - m_mat.pivot;
	WVector move;
	move = m_pPet->GetDeltaVec("NPC_Default", motion->e);
	if (dir.Magnitude() < move.Magnitude())
		return false;

	return true;
}

CNPC_BIRD::CNPC_BIRD(const char* name, float speed)
{
	m_speed = speed;
	Init(name, 0);
}

CNPC_BIRD::~CNPC_BIRD()
{
}

void CNPC_BIRD::Process(float dt)
{
	if (m_pPet && m_bActive)
	{
		m_time += dt;
		if (m_time < 0.0f)
			return;

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
		{
			if (m_flag & 1)
			{
				m_bActive = false;
				m_time = m_pMotion->s;
			}
			else
			{
				m_time -= m_pMotion->e - m_pMotion->s;
			}
		}

		CheckTarget(dt);
	}
}

void CNPC_BIRD::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
		FindTarget();

	float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-(dt * m_rotSpeed), angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

void CNPC_BIRD::FindTarget()
{
	for (int i = 0; i < 3; ++i)
	{
		float rate = Random(0.0f, 1.0f);
		m_target.p[i] =
			m_area.min.p[i] * rate + m_area.max.p[i] * (1.0f - rate);
	}

	m_target = GetPVS().GetGroundPoint(m_target, false, NULL, false);
}

CNPC_PARROT::CNPC_PARROT(const char* name)
{
	m_speed = 10.0f;
	Init(name, 0);
	m_maxRoll = 0.78539819f;
	m_cellSize = 48.0f;
}

CNPC_PARROT::~CNPC_PARROT()
{
}

CNPC_TORTOISE::CNPC_TORTOISE(const char* name, float speed)
{
	Init(name, 0);
	m_nodeIndex = 1;
	m_speed = speed * 2.0f;
	m_maxRoll = 0.78539819f;
	m_rotSpeed = 2.0943952f;
	m_velY = 0.0f;
	m_rollSpeed = 1.5707964f;
	m_phase = 2;
}

CNPC_TORTOISE::~CNPC_TORTOISE()
{
}

void CNPC_TORTOISE::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	m_time += dt;
	if (m_time < 0.0f)
		return;

	ScriptProcess(m_pMotion, m_time - dt, m_time);
	if (m_time > m_pMotion->e)
	{
		m_time -= m_pMotion->e;
		SetMotion("NPC_Default", 0.0f);
	}
	CheckTarget(dt);
}

void CNPC_TORTOISE::CheckTarget(float dt)
{
	WVector dir = m_target - m_mat.pivot;
	dir.y = 0.0f;

	if (dir.Magnitude() < Max(dt, 0.1f) * m_speed)
	{
		if (m_recordIndex >= m_recordList.size() || m_bRecord)
		{
			if (m_nodeIndex == m_nodeList.size() - 1)
				m_nodeIndex = 0;

			m_nodeIndex = (m_nodeIndex + 1) % m_nodeList.size();
			m_target = m_nodeList[m_nodeIndex];
			m_target += m_radius *
				RotMat(Random(-g_PI / 2.0f, g_PI / 2.0f, -1), 0).za *
				RotMat(Random(-g_PI, g_PI, -1), 1);

			dir = m_target - m_mat.pivot;
			dir.y = 0.0f;
			if (m_bRecord)
				m_recordList.push_back(m_target);
		}
		else
		{
			if (m_recordIndex < m_recordList.size())
			{
				m_target = m_recordList[m_recordIndex];
				++m_recordIndex;
			}
			else
			{
				m_recordIndex = 0;
			}
		}
	}

	float angle = CalcDeltaAngle(dir, m_mat.za * -1.0f);
	if (Abs(angle) > g_DEGTORAD)
	{
		m_mat = RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
			m_mat;
	}

	m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
	m_mat.pivot.y += dt * m_velY;
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
}

CNPC_TRSHIP::CNPC_TRSHIP(const char* name, float speed)
{
	Init(name, 0);
	m_t = 0.0f;
	float clamped = Between(-1.0f, speed, 1.0f);
	if (clamped == speed)
		m_speed2 = 1.0f;
	else
		m_speed2 = speed;
	m_bStart = true;
}

CNPC_TRSHIP::~CNPC_TRSHIP()
{
}

void CNPC_TRSHIP::Process(float dt)
{
	if (m_bActive && m_pPet)
	{
		m_time += dt;
		if (m_bStart)
		{
			if (!(m_time > 0.0f))
				return;
			m_time = 0.0f;
			m_bStart = false;
			SetMotion("NPC_default", -1.0f);
		}

		ScriptProcess(m_pMotion, m_time - dt, m_time);

		if (m_time > m_pMotion->e)
			m_time -= m_pMotion->e - m_pMotion->s;

		m_t += dt * 10.0f;
		CheckTarget(dt);
	}
}

void CNPC_TRSHIP::Display()
{
	if (!m_bActive || m_bStart == true)
		return;

	CCreature::Display();
}

void CNPC_TRSHIP::CheckTarget(float dt)
{
	WVector target = m_spline.Evaluate(m_t + 0.1f);
	m_mat.pivot = m_spline.Evaluate(m_t);

	if (WisEqual(target, m_nodeList[m_nodeList.size() - 1], 1.0e-5f))
		m_t = 0.0f;

	target.y = m_mat.pivot.y;
	m_mat = MakeCameraMatrix(target, m_mat.pivot);
}

void CNPC_TRSHIP::SetArea(const std::vector<WVector>& node, float time,
	unsigned long flag)
{
	CCreature::SetArea(node, time, flag);

	WVector* point = (WVector*)_alloca(node.size() * sizeof(WVector));
	float* length = (float*)_alloca(node.size() * sizeof(float));

	point[0] = node[0];
	length[0] = 0.0f;

	for (unsigned int i = 1; i < node.size(); ++i)
	{
		point[i] = node[i];
		float dist = (node[i] - node[i - 1]).Magnitude();
		length[i] = length[i - 1] + (dist == 0.0f ? 1.0f : dist) / m_speed2;
	}

	m_spline.Init(point, length, node.size());
}

ntNpc_AreaWalker::ntNpc_AreaWalker(const char* name)
{
	m_bWalking = true;
	m_speed = 2.0f;
	Init(name, 0);
}

ntNpc_AreaWalker::~ntNpc_AreaWalker()
{
}

void ntNpc_AreaWalker::Process(float dt)
{
	if (!m_pPet || !m_bActive)
		return;

	float time = m_time + dt;
	m_time = time;
	if (time < 0.0f)
		return;

	ScriptProcess(m_pMotion, time - dt, time);
	if (m_time > m_pMotion->e)
	{
		if (m_flag & 1)
		{
			m_bActive = false;
			m_time = m_pMotion->s;
		}
		else
		{
			SelectNextMotion();
		}
	}

	CheckTarget(dt);
}

void ntNpc_AreaWalker::CheckTarget(float dt)
{
	if ((m_mat.pivot - m_target).Magnitude() < 1.0f)
		FindTarget();

	if (m_bWalking)
	{
		float angle = CalcDeltaAngle(m_target - m_mat.pivot, m_mat.za * -1.0f);
		if (Abs(angle) > g_DEGTORAD)
		{
			m_mat =
				RotMat(Between(-dt * m_rotSpeed, angle, dt * m_rotSpeed), 1) *
				m_mat;
		}
		m_mat.pivot += m_mat.za * -1.0f * m_speed * dt;
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
	}
	else
	{
		WVector delta;
		delta = m_pPet->GetDeltaVec(m_pMotion->name, m_time - dt, m_time);
		delta = RotVec(delta, m_mat);
		m_mat.pivot += delta;
		m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
	}
}

void ntNpc_AreaWalker::FindTarget()
{
	for (int i = 0; i < 3; ++i)
	{
		float rate = Random(0.0f, 1.0f);
		m_target.p[i] =
			m_area.min.p[i] * rate + m_area.max.p[i] * (1.0f - rate);
	}

	const WVector& target = m_target;
	while (m_area.IsInclude(target))
		m_target += (m_target - m_mat.pivot).Normalize() * 3.2f;

	m_target = GetPVS().GetGroundPoint(m_target, true, NULL, false);
}

void ntNpc_AreaWalker::AddWalkingMotion(const char* name)
{
	std::vector<std::string>::const_iterator it =
		std::find(m_walkingMotion.begin(), m_walkingMotion.end(), name);
	if (it == m_walkingMotion.end())
		m_walkingMotion.push_back(name);
}

void ntNpc_AreaWalker::SelectNextMotion()
{
	std::map<std::string, std::list<MotionMap> >::iterator it;
	it = m_motionMap.find(m_pMotion->name);
	if (it == m_motionMap.end())
		return;

	int rate = rand() % 100;
	const char* name = NULL;
	std::list<MotionMap>::const_iterator motion;
	const std::list<MotionMap>& list = (*it).second;
	for (motion = list.begin(); motion != list.end(); ++motion)
	{
		if ((*motion).rate > rate)
		{
			name = (*motion).name.c_str();
			break;
		}
	}

	if (name)
	{
		SetMotion(name, 0.0f);
		std::vector<std::string>::const_iterator walking = std::find(
			m_walkingMotion.begin(), m_walkingMotion.end(), m_pMotion->name);
		m_bWalking = walking != m_walkingMotion.end();
	}
}

ntNpc_WizCityArea::ntNpc_WizCityArea(const char* name)
	: ntNpc_AreaWalker(name)
{
	ntNpc_WizCityArea::ConstructNextMotion();
	AddWalkingMotion("NPC_Default");
	if (rand() % 2 < 1)
	{
		SetMotion("NPC_Default", -1.0f);
	}
	else
	{
		m_bWalking = false;
		SetMotion("NPC_Ato", -1.0f);
	}
}

void ntNpc_WizCityArea::Display()
{
	WMatrix mat = m_mat;
	m_mat *= m_scale;
	CCreature::Display();
	m_mat = mat;
}

void ntNpc_WizCityArea::ConstructNextMotion()
{
	MotionMap motion;
	std::list<MotionMap> list;
	motion.name = "NPC_Ato";
	motion.rate = 100;
	list.push_back(motion);
	m_motionMap.insert(std::make_pair("NPC_Default", list));

	motion.name = "NPC_Default";
	motion.rate = 100;
	list.clear();
	list.push_back(motion);
	m_motionMap.insert(std::make_pair("NPC_Ato", list));
}

ntNpc_NodeWalker::ntNpc_NodeWalker(const char* name, float speed)
{
	m_speed = speed;
	m_length = 0.0f;
	m_dist = 0.0f;
	Init(name, 0);
}

ntNpc_NodeWalker::~ntNpc_NodeWalker()
{
}

void ntNpc_NodeWalker::Process(float dt)
{
	m_dist += dt * m_speed;
	CCreature::Process(dt);
}

void ntNpc_NodeWalker::SetArea(const std::vector<WVector>& node, float time,
	unsigned long flag)
{
	CCreature::SetArea(node, time, flag);
	m_nodeList.push_back(node[0]);
	m_nodeList.push_back(node[1]);
	MakeSpline(m_nodeList, m_length, m_spline);
}

void ntNpc_NodeWalker::CheckTarget(float dt)
{
	WVector target = m_spline.Evaluate(m_dist + 0.1f);
	m_mat.pivot = m_spline.Evaluate(m_dist);
	if (m_dist > m_length)
		m_dist = 0.0f;

	target = GetPVS().GetGroundPoint(target, true, NULL, false);
	m_mat.pivot = GetPVS().GetGroundPoint(m_mat.pivot, true, NULL, false);
	if (target.y < m_mat.pivot.y)
		target.y = m_mat.pivot.y;

	m_mat = MakeCameraMatrix(target, m_mat.pivot);
}

void ntNpc_NodeWalker::MakeSpline(const std::vector<WVector>& node,
	float& length, ntCatmullRom& spline)
{
	int num = node.size();
	WVector* point = (WVector*)_alloca(num * sizeof(WVector));
	float* time = (float*)_alloca(num * sizeof(float));

	point[0] = node[0];
	time[0] = 0.0f;
	for (int i = 1; i < num; ++i)
	{
		point[i] = node[i];
		time[i] = time[i - 1] + (node[i] - node[i - 1]).Magnitude();
	}

	length = time[num - 2];
	spline.Init(point, time, num);
}

ntNpc_WizCityNode::ntNpc_WizCityNode(const char* name, float speed)
	: ntNpc_NodeWalker(name, speed)
{
	m_scale = 1.0f;
}

void ntNpc_WizCityNode::Display()
{
	WMatrix mat = m_mat;
	m_mat *= m_scale;
	CCreature::Display();
	m_mat = mat;
}

CDisposable::CDisposable(const char* name)
{
	unsigned long& flag = m_flag;
	flag |= 1;
	Init(name, 0);
}

void CDisposable::PlayEvent()
{
	CCreature::PlayEvent();

	m_mat.za = m_nodeList[0] - m_nodeList[1];
	m_mat.za.y = 0.0f;
	m_mat.za.Normalize();
	m_mat.ya = WVector::UNIT_POS_Y;
	m_mat.xa = WCrossProduct(m_mat.ya, m_mat.za);
	m_mat.pivot = GetPVS().GetGroundPoint(m_nodeList[0], true, NULL, true);
}

COnGameNet::COnGameNet(const char* name, unsigned char index)
{
	m_camTex = 0;
	Init(name, 0);
	m_speed = 0.5f;
	m_index = index;

	m_camTex = g_resrcmng->LoadTexture("camera.jpg", 0, 0,
		MakeStr("camera_%d.jpg", index));
	w_texlist* tex = g_resrcmng->FindTexture("camera.jpg");
	if (tex)
		m_pPet->ChangeTextureByHandle(m_camTex, tex->texhandle, NULL);
}

COnGameNet::~COnGameNet()
{
	if (g_resrcmng && m_camTex)
	{
		g_resrcmng->Release(m_camTex);
		m_camTex = 0;
	}
}

void COnGameNet::Process(float dt)
{
	if (GOLFDOC()->m_currentPlayer != m_index)
	{
		m_bActive = false;
		return;
	}

	m_bActive = true;
	CCreature::Process(dt);
}

void COnGameNet::CheckTarget(float dt)
{
	if (GolfBall().m_playFrame >= 0 && GOLFDOC()->GameMode() != 0x1000)
		return;

	AfxGetTask()->GetActor("Player")
		<< MsgObject(NULL, 292, m_index, (int)&m_headMat, 0, 0, 0);

	WVector dir = m_headMat.za;
	dir.y = 0.0f;
	dir.Normalize();
	if (GolfBall().m_groundType == 2)
		m_target = m_headMat.pivot - dir * (0.8f * 3.2f);
	else
		m_target = m_headMat.pivot - dir * (1.2f * 3.2f);
	m_target.y = GolfBall().m_pos.y + 1.2f * 3.2f;

	dir = m_target - m_mat.pivot;
	float distance = sqrtf((float)(dir * dir));
	float ratio = Min(distance / (2.0f * 3.2f), 1.0f);
	m_mat.za = (1.0f - ratio) * (GolfBall().m_pos - m_mat.pivot) + ratio * dir;
	m_mat.za.y = 0.0f;
	m_mat.za.Normalize();
	m_mat.za *= -1.0f;

	m_mat.ya = WVector::UNIT_POS_Y;
	m_mat.xa = WCrossProduct(m_mat.ya, m_mat.za);

	WVector pos = m_mat.pivot;
	m_mat.pivot = WVector::ZERO;
	m_mat = RotMat(-30.0f * g_DEGTORAD, 1) * m_mat;
	m_mat.pivot = pos;

	if (distance < 10.0 * 3.2f)
	{
		if (distance > 0.5)
			m_mat.pivot += dir * (distance * m_speed) * dt;
	}
	else
	{
		m_mat.pivot = m_target;
	}
}

ntCatmullRom::ntCatmullRom()
	: m_pPoint(NULL),
	  m_pTime(NULL),
	  m_pLength(NULL),
	  m_totalLength(0.0f),
	  m_num(0)
{
}

ntCatmullRom::~ntCatmullRom()
{
	Clean();
}

bool ntCatmullRom::Init(const WVector* point, const float* time, int num)
{
	if (m_num)
		return false;

	if (num < 4 || !point || !time)
		return false;

	m_pPoint = new WVector[num];
	m_pTime = new float[num];
	m_num = num;

	for (unsigned int i = 0; i < (unsigned int)num; ++i)
	{
		m_pPoint[i] = point[i];
		m_pTime[i] = time[i];
	}

	m_pLength = new float[num - 1];
	m_totalLength = 0.0f;
	for (unsigned int i = 0; i < (unsigned int)(num - 1); ++i)
	{
		m_pLength[i] = SegmentArcLength(i, 0.0f, 1.0f);
		m_totalLength += m_pLength[i];
	}

	return true;
}

void ntCatmullRom::Clean()
{
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
	if (m_pTime)
	{
		delete[] m_pTime;
		m_pTime = NULL;
	}
	if (m_pLength)
	{
		delete[] m_pLength;
		m_pLength = NULL;
	}
	m_totalLength = 0.0f;
	m_num = 0;
}

WVector ntCatmullRom::Evaluate(float t)
{
	if (m_num < 2)
		return WVector::ZERO;

	if (t <= m_pTime[0])
		return m_pPoint[0];
	else if (t >= m_pTime[m_num - 1])
		return m_pPoint[m_num - 1];

	int i;
	for (i = 0; i < m_num - 1; ++i)
	{
		if (t <= m_pTime[i + 1])
			break;
	}

	float t0 = m_pTime[i];
	float t1 = m_pTime[i + 1];
	float u = (t - t0) / (t1 - t0);

	if (i == 0)
	{
		WVector A = m_pPoint[0] - 2.0f * m_pPoint[1] + m_pPoint[2];
		WVector B = 4.0f * m_pPoint[1] - 3.0f * m_pPoint[0] - m_pPoint[2];
		return m_pPoint[0] + (0.5f * u) * (B + u * A);
	}
	else if (i >= m_num - 2)
	{
		i = m_num - 2;
		WVector A = m_pPoint[i - 1] - 2.0f * m_pPoint[i] + m_pPoint[i + 1];
		WVector B = m_pPoint[i + 1] - m_pPoint[i - 1];
		return m_pPoint[i] + (0.5f * u) * (B + u * A);
	}
	else
	{
		WVector A = 3.0f * m_pPoint[i] - m_pPoint[i - 1] -
			3.0f * m_pPoint[i + 1] + m_pPoint[i + 2];
		WVector B = 2.0f * m_pPoint[i - 1] - 5.0f * m_pPoint[i] +
			4.0f * m_pPoint[i + 1] - m_pPoint[i + 2];
		WVector C = m_pPoint[i + 1] - m_pPoint[i - 1];
		return m_pPoint[i] + (0.5f * u) * (C + u * (B + u * A));
	}
}

WVector ntCatmullRom::Velocity(float t)
{
	if (m_num < 2)
		return WVector::ZERO;

	if (t <= m_pTime[0])
		t = 0.0f;
	else if (t > m_pTime[m_num - 1])
		t = m_pTime[m_num - 1];

	int i;
	for (i = 0; i < m_num - 1; ++i)
	{
		if (t <= m_pTime[i + 1])
			break;
	}

	float t0 = m_pTime[i];
	float t1 = m_pTime[i + 1];
	float u = (t - t0) / (t1 - t0);

	if (i == 0)
	{
		WVector A = m_pPoint[0] - 2.0f * m_pPoint[1] + m_pPoint[2];
		WVector B = 4.0f * m_pPoint[1] - 3.0f * m_pPoint[0] - m_pPoint[2];
		return 0.5f * B + u * A;
	}
	else if (i >= m_num - 2)
	{
		i = m_num - 2;
		WVector A = m_pPoint[i - 1] - 2.0f * m_pPoint[i] + m_pPoint[i + 1];
		WVector B = m_pPoint[i + 1] - m_pPoint[i - 1];
		return 0.5f * B + u * A;
	}
	else
	{
		WVector A = 3.0f * m_pPoint[i] - m_pPoint[i - 1] -
			3.0f * m_pPoint[i + 1] + m_pPoint[i + 2];
		WVector B = 2.0f * m_pPoint[i - 1] - 5.0f * m_pPoint[i] +
			4.0f * m_pPoint[i + 1] - m_pPoint[i + 2];
		WVector C = m_pPoint[i + 1] - m_pPoint[i - 1];
		return 0.5f * C + u * (B + 1.5f * u * A);
	}
}

WVector ntCatmullRom::Acceleration(float t)
{
	if (m_num < 2)
		return WVector::ZERO;

	if (t <= m_pTime[0])
		t = 0.0f;
	else if (t > m_pTime[m_num - 1])
		t = m_pTime[m_num - 1];

	int i;
	for (i = 0; i < m_num - 1; ++i)
	{
		if (t <= m_pTime[i + 1])
			break;
	}

	float t0 = m_pTime[i];
	float t1 = m_pTime[i + 1];
	float u = (t - t0) / (t1 - t0);

	if (i == 0)
		return m_pPoint[0] - 2.0f * m_pPoint[1] + m_pPoint[2];
	else if (i >= m_num - 2)
	{
		i = m_num - 2;
		return m_pPoint[i - 1] - 2.0f * m_pPoint[i] + m_pPoint[i + 1];
	}
	else
	{
		WVector A = 3.0f * m_pPoint[i] - m_pPoint[i - 1] -
			3.0f * m_pPoint[i + 1] + m_pPoint[i + 2];
		WVector B = 2.0f * m_pPoint[i - 1] - 5.0f * m_pPoint[i] +
			4.0f * m_pPoint[i + 1] - m_pPoint[i + 2];
		return B + 3.0f * u * A;
	}
}

float ntCatmullRom::FindParameterByDistance(float t1, float dist)
{
	if (ArcLength(t1, m_pTime[m_num - 1]) < dist)
		return m_pTime[m_num - 1];

	float p = t1 + dist * ((m_pTime[m_num - 1] - m_pTime[0]) / m_totalLength);
	for (int i = 0; i < 32; ++i)
	{
		float func = ArcLength(t1, p) - dist;
		if (Wabs(func) < 0.001f)
			return p;
		p -= func / Velocity(p).Magnitude();
	}

	return g_HUGE;
}

float ntCatmullRom::ArcLength(float t1, float t2)
{
	if (t2 <= t1)
		return 0.0f;

	if (t1 < m_pTime[0])
		t1 = m_pTime[0];
	if (t2 > m_pTime[m_num - 1])
		t2 = m_pTime[m_num - 1];

	int seg1;
	for (seg1 = 0; seg1 < m_num - 1; ++seg1)
	{
		if (t1 <= m_pTime[seg1 + 1])
			break;
	}
	float u1 = (t1 - m_pTime[seg1]) / (m_pTime[seg1 + 1] - m_pTime[seg1]);

	int seg2;
	for (seg2 = 0; seg2 < m_num - 1; ++seg2)
	{
		if (t2 <= m_pTime[seg2 + 1])
			break;
	}
	float u2 = (t2 - m_pTime[seg2]) / (m_pTime[seg2 + 1] - m_pTime[seg2]);

	float result;
	if (seg1 == seg2)
	{
		result = SegmentArcLength(seg1, u1, u2);
	}
	else
	{
		result = SegmentArcLength(seg1, u1, 1.0f);
		for (int i = seg1 + 1; i < seg2; ++i)
			result += m_pLength[i];
		result += SegmentArcLength(seg2, 0.0f, u2);
	}

	return result;
}

float ntCatmullRom::SegmentArcLength(int seg, float u1, float u2)
{
	static const float x[] = { 0.0000000000f, 0.5384693101f, -0.5384693101f,
		0.9061798459f, -0.9061798459f };
	static const float c[] = { 0.5688888889f, 0.4786286705f, 0.4786286705f,
		0.2369268850f, 0.2369268850f };

	if (u2 <= u1)
		return 0.0f;

	if (u1 < 0.0f)
		u1 = 0.0f;
	if (u2 > 1.0f)
		u2 = 1.0f;

	float sum = 0.0f;
	WVector A, B, C;
	if (seg == 0)
	{
		A = m_pPoint[0] - 2.0f * m_pPoint[1] + m_pPoint[2];
		B = 4.0f * m_pPoint[1] - 3.0f * m_pPoint[0] - m_pPoint[2];
	}
	else if (seg >= m_num - 2)
	{
		seg = m_num - 2;
		A = m_pPoint[seg - 1] - 2.0f * m_pPoint[seg] + m_pPoint[seg + 1];
		B = m_pPoint[seg + 1] - m_pPoint[seg - 1];
	}
	else
	{
		A = 3.0f * m_pPoint[seg] - m_pPoint[seg - 1] -
			3.0f * m_pPoint[seg + 1] + m_pPoint[seg + 2];
		B = 2.0f * m_pPoint[seg - 1] - 5.0f * m_pPoint[seg] +
			4.0f * m_pPoint[seg + 1] - m_pPoint[seg + 2];
		C = m_pPoint[seg + 1] - m_pPoint[seg - 1];
	}

	for (int j = 0; j < 5; ++j)
	{
		float u = 0.5f * ((u2 - u1) * x[j] + u2 + u1);
		WVector derivative;
		if (seg == 0 || seg >= m_num - 2)
			derivative = 0.5f * B + u * A;
		else
			derivative = 0.5f * C + u * (B + 1.5f * u * A);
		sum += c[j] * derivative.Magnitude();
	}
	sum *= 0.5f * (u2 - u1);

	return sum;
}
