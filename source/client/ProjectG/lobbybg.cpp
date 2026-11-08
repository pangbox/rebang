#include "projectg.h"
#include "golfdoc.h"
#include "sky.h"
#include "shadow.h"
#include "wind.h"
#include "fx.h"
#include "quadtree.h"
#include "polysoup.h"
#include "scenemanager.h"
#include "clientsetting.h"
#include "lobbybg.h"
#include "mathconsts.h"

extern bool g_bQuit;

int LOBBYBG_MAP = 19;
char* LOBBYBG_MAPNAME = "wizcity";

IMPLEMENT_ACTOR(ntLobbyBg, IActor)

void ntLobbyBg::OnPreLoadInit()
{
	new CShadowManager(g_resrcmng, COption::Instance()->vIsShadowEnabled());

	if (LOBBYBG_MAP == 13)
		g_view->SetClip(0.1f, 26214.4f, true);
}

void ntLobbyBg::OnLoad()
{
	if (CFx::Instance())
		CFx::Instance()->SetActive(false);

	if (!GOLFDOC()->m_pPolySoup->Load(MakeStr("%s_rank.gbin", LOBBYBG_MAPNAME),
			false))
	{
		g_bQuit = true;
		return;
	}

	GOLFDOC()->m_waveInfo.Load(MakeStr("%s_wave.txt", LOBBYBG_MAPNAME));
	GOLFDOC()->m_riverInfo.Load(MakeStr("%s_river.txt", LOBBYBG_MAPNAME));

	GOLFDOC()->m_pSky = new CSky;
	GOLFDOC()->m_pSky->LoadSky(LOBBYBG_MAPNAME, 0.0f);
	GOLFDOC()->m_pSky->RegisterCloud(MakeStr("%s_cloud.dds", LOBBYBG_MAPNAME));

	GOLFDOC()->m_pPolySoup->MakeDataStructure();

	if (GOLFDOC()->m_pSky)
	{
		GOLFDOC()->m_pSky->GenerateStars(LOBBYBG_MAP);
		GOLFDOC()->m_pSky->GenerateLensFlare();
		GOLFDOC()->m_pSky->GenerateMoons();
	}

	CShadowManager::Instance()->UpdateLight("sun", g_lightset);

	CSceneManager::Instance()->Load();

	GOLFDOC()->m_pPolySoup->EndLoad(false);

	if (CFx::Instance())
		CFx::Instance()->SetActive(true);
}

void ntLobbyBg::OnInit()
{
	CSceneManager::Instance()->UpdateFog(LOBBYBG_MAPNAME);

	Wind().CheckSpecialWind();
}

void ntLobbyBg::OnDestroy()
{
	if (CShadowManager::Instance())
		delete CShadowManager::Instance();
}

void ntLobbyBg::HandleMsg(const MsgObject& msg)
{
	if (msg.time > g_CurrentTime)
	{
		StoreDelayedMsg(msg);
		return;
	}

	switch (msg.message)
	{
	case 0x13d:
		GOLFDOC()->m_pSky->GenerateClouds(50, Wind().GetGlobalIntensity());
		break;

	case 0x13e:
	{
		GOLFDOC()->m_pSky->LoadSky(LOBBYBG_MAPNAME, 0.0f);
		GOLFDOC()->m_pSky->RegisterCloud(
			MakeStr("%s_cloud.dds", LOBBYBG_MAPNAME));
		GOLFDOC()->m_pSky->GenerateStars(LOBBYBG_MAP);
		GOLFDOC()->m_pSky->GenerateMoons();
		GOLFDOC()->m_pSky->GenerateLensFlare();
	}
	break;

	case 0x13f:
		GOLFDOC()->m_pSky->m_color = msg.param1;
		break;

	case 0x140:
		DoReLoad();
		break;

	case 0x118:
		GOLFDOC()->m_pPolySoup->SetRenderMode(msg.param1);
		break;

	case 0x141:
		if (GOLFDOC()->m_pPolySoup)
			GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		break;
	}
}

ntLobbyBg::ntLobbyBg()
{
	m_dispPriority = DISP_PRIORITY_1;
}

void ntLobbyBg::DoReLoad()
{
	SendMsg(this, "Ambient", 500, 0, 0, 0, 0);

	OnLoad();

	AfxGetTask()->Init(NULL);
}

void ntLobbyBg::SetSkyColor(unsigned long color)
{
	if (GOLFDOC()->m_pSky)
		GOLFDOC()->m_pSky->m_color = color;
}

void ntLobbyBg::OnProcess(float delta)
{
	GetPVS().PreProcess(delta, false);
}
