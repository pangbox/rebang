#include "stdafx.h"
#include "golfbg.h"
#include "clientsetting.h"
#include "shadow.h"
#include "autotarget.h"
#include "sky.h"
#include "polysoup.h"
#include "quadtree.h"
#include "golfball.h"
#include "wind.h"
#include "fx.h"
#include "taskmanager.h"

extern bool g_bQuit;

IMPLEMENT_ACTOR(CGolfBg, IActor)

CGolfBg::CGolfBg()
{
	(void)&g_HUGE;
	m_dispPriority = DISP_PRIORITY_1;
	m_renderWeather = 0;
}

void CGolfBg::OnDestroy()
{
	if (CShadowManager::Instance())
		delete CShadowManager::Instance();
}

void CGolfBg::DoReLoad()
{
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);
	if (CFx::Instance())
		CFx::Instance()->SetActive(false);
	int hole = GOLFDOC()->m_currentHole;
	if (Doc()->m_roomInfo.realGameType == 14)
	{
		GOLFDOC()->m_pAutoTarget->Reset();
		if (!GOLFDOC()->m_pPolySoup->Load(
				MakeStr("%s_%02d.gbin", GetCurMapName(), hole), false))
		{
			g_bQuit = true;
			return;
		}
		GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", GetCurMapName()));
		GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", GetCurMapName()));
		IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(0x28000000 |
			((Doc()->m_pinIndex[17] > 127 && Doc()->m_pinIndex[17] != 0xfd)
					? Doc()->m_pinIndex[17] - 128
					: Doc()->m_pinIndex[17]));
		if (course && course->Data)
		{
			const char* name = course->Data;
			if (GOLFDOC()->m_pSky)
			{
				GOLFDOC()->m_pSky->Clear();
				delete GOLFDOC()->m_pSky;
				GOLFDOC()->m_pSky = NULL;
			}
			GOLFDOC()->m_pSky = new CSky;
			GOLFDOC()->m_pSky->LoadSky(name, 0);
			GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));
		}
		GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
		CSceneManager::Instance()->SetWeather(Doc()->m_golfGame.weather, true);
	}
	else
	{
		if (!GOLFDOC()->m_pPolySoup->Load(
				MakeStr("%s_%02d.gbin", GetCurMapName(), hole), false))
		{
			g_bQuit = true;
			return;
		}
		if (GOLFDOC()->m_pSky)
		{
			GOLFDOC()->m_pSky->Clear();
			delete GOLFDOC()->m_pSky;
			GOLFDOC()->m_pSky = NULL;
		}
		GOLFDOC()->m_pSky = new CSky;
		if (GOLFDOC()->m_tutorialMode < 15)
		{
			GOLFDOC()->m_pSky->LoadSky("blue", 0);
			GOLFDOC()->m_pSky->RegisterCloud("blue");
		}
		else
		{
			GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
			GOLFDOC()->m_pSky->RegisterCloud(
				MakeStr("%s_cloud.dds", GetSkyBoxName()));
		}
		GOLFDOC()->m_pPolySoup->LoadMapCheckData(hole);
	}
	DoInitAfterLoad();
	if (CFx::Instance())
		CFx::Instance()->SetActive(true);
	GetActor("GroundItemMan") << MsgObject(this, 634, 0, 0, 0, 0, 0);
	AfxGetTask()->Init(NULL);
	if (OnlinePlay())
	{
		WSendPacket packet((enumClientPacket)17);
		packet.Send((eSendTo)0);
	}
	else
		SendMsg(this, "GolfRule", 129, 0, 0, 0, 0);
}

void CAutoTarget::SetList(const std::vector<WVector>& list, const WVector& pos)
{
	m_pointList.reserve(list.size() + 1);
	m_pointList.assign(list.begin(), list.end());
	m_pointList.push_back(pos);
	for (unsigned int i = 0; i < m_pointList.size(); i++)
		m_pointList[i].y = 0;
}

void CGolfBg::DoInitAfterLoad()
{
	GOLFDOC()->m_pPolySoup->MakeDataStructure();
	SendMsg(this, "Camera", 239, 0, 0, 0, 0);
	WPolySoup::sPoint* point;
	if (GetDiffFlag() == 1)
	{
		point = GOLFDOC()->m_pPolySoup->FindPoint("\xb1\xd7\xb8\xae\xb5\xe5");
		SetGridCenter(point->pos);
	}
	else
	{
		point = GOLFDOC()->m_pPolySoup->FindPoint("\xb1\xd7\xb8\xae\xb5\xe5"
												  "2");
		if (!point)
			point =
				GOLFDOC()->m_pPolySoup->FindPoint("\xb1\xd7\xb8\xae\xb5\xe5");
		SetGridCenter(point->pos);
	}
	SetStartPoint(GOLFDOC()->GetHoleData().tee);
	SetHolePoint(GOLFDOC()->GetHoleData().pin);
	SendMsg(this, "GolfBg", 322, GetWeather(), 0, 0, 0);
	if (GOLFDOC()->m_pSky)
	{
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}
	CShadowManager::Instance()->UpdateLight("sun", g_lightset);
	if (!OnlinePlay())
	{
		GolfBall().Reset(
			GetTeeShotPos() + WVector(0, GolfBall().GetRadius(), 0));
	}
	CSceneManager::Instance()->Load();
	GOLFDOC()->m_pAutoTarget->Reset();
	std::vector<CNodeContainer>::const_iterator it;
	for (it = GOLFDOC()->m_pPolySoup->m_node.begin();
		it != GOLFDOC()->m_pPolySoup->m_node.end(); it++)
	{
		if (it->GetType() == CNodeContainer::TYPE_AUTO)
			GOLFDOC()->m_pAutoTarget->SetList(it->GetList(), GetHoleCupPos());
	}
	GOLFDOC()->m_pPolySoup->EndLoad(true);
}

void CGolfBg::OnPreLoadInit()
{
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());
}

void CGolfBg::OnLoad()
{
	if (CFx::Instance())
		CFx::Instance()->SetActive(false);
	GOLFDOC()->m_pAutoTarget = new CAutoTarget;
	if (Doc()->m_roomInfo.realGameType == 14)
		SetCurMap(
			Doc()->m_pinIndex[Between<unsigned char>(1,
								  GetHoleIndex(GOLFDOC()->m_currentHole), 18) -
				1]);
	if (!GOLFDOC()->m_pPolySoup->Load(
			MakeStr("%s_%02d.gbin", GetCurMapName(), GOLFDOC()->m_currentHole),
			false))
	{
		g_bQuit = true;
		return;
	}
	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", GetCurMapName()));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", GetCurMapName()));
	GOLFDOC()->m_pSky = new CSky;
	if (GOLFDOC()->m_tutorialMode < 15)
	{
		GOLFDOC()->m_pSky->LoadSky("blue", 0);
		GOLFDOC()->m_pSky->RegisterCloud("blue");
	}
	else
	{
		if (Doc()->m_roomInfo.gameType != 14 &&
			Doc()->m_roomInfo.realGameType != 14)
		{
			GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
			GOLFDOC()->m_pSky->RegisterCloud(
				MakeStr("%s_cloud.dds", GetSkyBoxName()));
		}
		else
		{
			unsigned char map = Doc()->m_pinIndex[17];
			IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
				0x28000000 | ((map > 127 && map != 0xfd) ? map - 128 : map));
			if (course && course->Data)
			{
				const char* name = course->Data;
				GOLFDOC()->m_pSky->LoadSky(name, 0);
				GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", name));
			}
		}
	}
	DoInitAfterLoad();
	if (CFx::Instance())
		CFx::Instance()->SetActive(true);
}

void CGolfBg::OnInit()
{
	CSceneManager::Instance()->UpdateFog(true);
}

void CGolfBg::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}
	switch (msg.message)
	{
	case 317:
	{
		int weather = msg.param1;
		if (weather > 1)
			GOLFDOC()->m_pSky->ClearSky();
		GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());
		GOLFDOC()->m_pSky->ChangeWeatherTex(weather);
		GOLFDOC()->m_pSky->ShowClouds(weather > 0);
		break;
	}
	case 318:
		GOLFDOC()->m_pSky->LoadSky(MakeStr("%s", GetSkyBoxName()), 0);
		GOLFDOC()->m_pSky->RegisterCloud(
			MakeStr("%s_cloud.dds", GetSkyBoxName()));
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateStars(-1);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		break;
	case 319:
		GOLFDOC()->m_pSky->m_color = msg.param1;
		break;
	case 320:
		DoReLoad();
		break;
	case 280:
		GOLFDOC()->m_pPolySoup->SetRenderMode(msg.param1);
		break;
	case 321:
		if (GOLFDOC()->m_pPolySoup)
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		break;
	case 322:
		SetRenderWeather(msg.param1);
		CSceneManager::Instance()->SetWeather(GetRenderWeather(),
			msg.param2 != 0);
		SetWeather(msg.param1);
		break;
	case 485:
	{
		int il_balance0 = msg.param1;
		int il_balance1 = msg.param1;
		int il_balance2 = msg.param1;
		int il_balance3 = msg.param1;

		const std::vector<WVector>& list =
			GOLFDOC()->m_pAutoTarget->GetPointList();
		std::vector<WVector>& camera = *(std::vector<WVector>*)msg.param1;
		unsigned int count = list.size() * 2;
		count += (list.size() - 1) * 2 + 9;
		if (camera.capacity() < count)
			camera.reserve(count);
		else
			camera.clear();
		WVector start;
		start = GetTeeShotPos();
		start.y = GetPVS().GetGroundPoint(start, false, NULL, false).y + 100;
		WVector first;
		first = list.front();
		first.y = GetPVS().GetGroundPoint(first, true, NULL, true).y + 100;
		float dz, dy, dx;
		WVector normal;
		dx = start.x - first.x;
		normal.x = dx;
		dy = start.y - first.y;
		normal.y = dy;
		dz = start.z - first.z;
		normal.z = dz;
		float distance = 250.f;
		WVector end;
		end = start + normal.Normalize() * distance;
		camera.push_back((end + start) * 0.5f);
		camera.push_back(start);
		camera.push_back((start + first) * 0.5f);
		camera.push_back(first);
		WVector tangent;
		int i;
		for (i = 1; i < (int)list.size() - 1; i++)
		{
			tangent = (list[i - 1] + list[i]) * 0.5f;
			tangent.y =
				GetPVS().GetGroundPoint(tangent, true, NULL, true).y + 100;
			camera.push_back(tangent);
			tangent = list[i];
			tangent.y =
				GetPVS().GetGroundPoint(tangent, true, NULL, true).y + 100;
			camera.push_back(tangent);
		}
		float sign = rand() % 2 ? 1.f : -1.f;
		WVector side;
		side = WCrossProduct(WVector(0, sign, 0), list.back() - list[i - 1]);
		side.Normalize();
		side *= 30;
		WVector last;
		last = list.back() + side;
		last.y = GetPVS().GetGroundPoint(last, false, NULL, false).y + 100;
		WVector previous;
		previous = list[i - 1];
		previous.y =
			GetPVS().GetGroundPoint(previous, true, NULL, true).y + 100;
		tangent = (last - previous).Normalize() * 250;
		end = last + tangent;
		side = last + WCrossProduct(WVector(0, -sign, 0), tangent);
		camera.push_back((previous + last) * 0.5f);
		camera.push_back(last);
		camera.push_back((last + end) * 0.5f);
		camera.push_back(end);
		camera.push_back((end + side) * 0.5f);
		camera.push_back(side);
		camera.push_back((side + previous) * 0.5f);
		camera.push_back(previous);
		for (int j = (int)list.size() - 3; j >= 0; j--)
		{
			tangent = (list[j + 1] + list[j]) * 0.5f;
			tangent.y =
				GetPVS().GetGroundPoint(tangent, true, NULL, true).y + 100;
			camera.push_back(tangent);
			tangent = list[j];
			tangent.y =
				GetPVS().GetGroundPoint(tangent, true, NULL, true).y + 100;
			camera.push_back(tangent);
		}
		tangent = WVector(dx, dy, dz).Normalize() * 250;
		end = start + tangent;
		side = start +
			WCrossProduct(WVector(0, rand() % 2 ? 1.f : -1.f, 0), tangent);
		camera.push_back((first + side) * 0.5f);
		camera.push_back(side);
		camera.push_back((side + end) * 0.5f);
		camera.push_back(end);
		camera.push_back((end + start) * 0.5f);
		break;
	}
	case 488:
		if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_%02d.gbin",
											  GetCurMapName(),
											  GOLFDOC()->m_currentHole),
				false))
		{
			g_bQuit = true;
			return;
		}
		DoInitAfterLoad();
		AfxGetTask()->Init(NULL);
		SendMsg(this, "GolfRule", 129, 0, 0, 0, 0);
		break;
	case 487:
		if (!GOLFDOC()->m_pPolySoup->Load(
				MakeStr("%s_rank.gbin", GetCurMapName()), false))
		{
			g_bQuit = true;
			return;
		}
		GOLFDOC()->m_pPolySoup->EndLoad(false);
		GOLFDOC()->m_pPolySoup->MakeDataStructure();
		SetLowestPoint();
		CShadowManager::Instance()->UpdateLight("sun", g_lightset);
		GOLFDOC()->m_pSky->LoadSky(GetSkyBoxName(), 0);
		GOLFDOC()->m_pSky->ClearClouds();
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateStars(-1);
		CSceneManager::Instance()->Load();
		CSceneManager::Instance()->UpdateFog(true);
		SendMsg(this, "Effect", 379, 0, 0, 0, 0);
		SendMsg(this, "Caddie", 356, 0, 0, 0, 0);
		break;
	case 10002:
	{
		char text[256];
		char* src = (char*)msg.param1;
		char* dest = text;
		while ((*dest++ = *src++) != 0)
		{
		}
		break;
	}
	}
}

void CGolfBg::SetStartPoint(const WVector& pos)
{
	GOLFDOC()->GetHoleData().tee =
		GetPVS().GetGroundPoint(pos, false, &GolfBall().m_meshIndex, false);
}

void CGolfBg::SetHolePoint(const WVector& pos)
{
	GOLFDOC()->GetHoleData().pin =
		GetPVS().GetGroundPoint(pos, false, NULL, false);
}

void CGolfBg::SetGridCenter(WVector& pos)
{
	pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	SendMsg(this, "Grid", 116, (int)&pos, 0, 0, 0);
	SendMsg(this, "GolfRule", 116, (int)&pos, 0, 0, 0);
}

void CGolfBg::SetSkyColor(unsigned long color)
{
	if (GOLFDOC()->m_pSky)
		GOLFDOC()->m_pSky->SetColor(color);
}

void CGolfBg::SetLowestPoint()
{
	SendMsg(this, "Camera", 239, 0, 0, 0, 0);
}

const char* CGolfBg::GetSkyBoxName()
{
	if (!GetCurMapName())
		return "blue";
	if (!IsLocalContent((localContentType_t)154))
		return GetCurMapName();
	return "Halloween";
}

void CGolfBg::SetRenderWeather(unsigned char weather)
{
	m_renderWeather = weather;
	if (IsLocalContent((localContentType_t)154) && GetGameType() < 11)
	{
		if (m_renderWeather <= 0)
			m_renderWeather = 1;
		if (GOLFDOC()->m_pPolySoup)
			GOLFDOC()->m_pPolySoup->ChangePointType(2, 3);
	}
}
