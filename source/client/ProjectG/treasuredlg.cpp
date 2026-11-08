#include "treasuredlg.h"
#include "golfdoc.h"
#include "treasure.h"
#include "frdesktop.h"
#include "frviewer.h"
#include "frcombobox.h"
#include "simpleoverlay.h"
#include "inputmanager.h"
#include "taskmanager.h"
#include <time.h>

extern Fresh* g_pFresh;
namespace nsTHunter
{
	bool IsWndSizeMinimum()
	{
		WRect rect(g_pFresh->GetManager()->GetDesktop()->GetRect());
		if (rect.w == 640 && rect.h == 480)
			return true;
		return false;
	}

	unsigned long CalcGauge(unsigned long point)
	{
		if (point < 701)
			return 0;
		return point - 700;
	}
}

bool CalculateUVPos(WRect& rect, int imageWidth, int imageHeight, float width,
	float height, int index)
{
	if ((float)imageWidth < width || (float)imageHeight < height)
		return false;
	int rows = (int)(((float)imageHeight) / height);
	int columns = (int)(((float)imageWidth) / width);
	if (columns * rows < index)
		return false;
	float u = 1.f / ((float)imageWidth);
	rect.x = (float)((index / rows) * width) * u;
	float v = 1.f / ((float)imageHeight);
	rect.y = (float)((index % rows) * height) * v;
	rect.w = u * width;
	rect.h = v * height;
	return true;
}

IMPLEMENT_OBJECT(FrNextHole, FrForm)
BEGIN_FRESH_MSGMAP(FrNextHole, FrForm)
ON_FRESH_VI("next", FRCMD_INIT, FrNextHole::OnAreaNextHole_Init)
ON_FRESH_VI("next", FRCMD_OWNERDRAW, FrNextHole::OnAreaNextHole_OwnerDraw)
ON_FRESH_VI("outback1", FRCMD_INIT, FrNextHole::OnArea_OutFrame_Init)
ON_FRESH_VI("outback2", FRCMD_INIT, FrNextHole::OnArea_OutFrame_Init)
ON_FRESH_VI("outback3", FRCMD_INIT, FrNextHole::OnArea_OutFrame_Init)
END_FRESH_MSGMAP()

FrNextHole::FrNextHole()
{
	m_pHoleOverlay[0] = NULL;
	m_pHoleOverlay[1] = NULL;
	m_pNextHoleArea = NULL;
	m_overlayIndex = 0;
	m_state = (eState)0;
	m_pHoleOverlay[2] = NULL;
}

FrNextHole::~FrNextHole()
{
	if (g_resrcmng && m_pHoleOverlay[1])
	{
		g_resrcmng->Release(m_pHoleOverlay[1]);
		m_pHoleOverlay[1] = NULL;
	}
	if (g_resrcmng && m_pHoleOverlay[0])
	{
		g_resrcmng->Release(m_pHoleOverlay[0]);
		m_pHoleOverlay[0] = NULL;
	}
	if (g_resrcmng && m_pHoleOverlay[2])
	{
		g_resrcmng->Release(m_pHoleOverlay[2]);
		m_pHoleOverlay[2] = NULL;
	}
}

void FrNextHole::OnAreaNextHole_Init(int param)
{
	m_pNextHoleArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

bool FrNextHole::OnInit()
{
	FrForm::OnInit();
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(0x28000000 |
		((GetCurMap() > 127 && GetCurMap() != 253) ? GetCurMap() - 128
												   : GetCurMap()));
	if (!course)
		return false;
	m_pHoleOverlay[0] =
		g_resrcmng->GetOverlay(MakeStr("%s_1.png", course->c.Name), 0);
	m_pHoleOverlay[1] =
		g_resrcmng->GetOverlay(MakeStr("%s_2.png", course->c.Name), 0);
	m_pHoleOverlay[2] = g_resrcmng->GetOverlay("next_chaos.tga", 0);
	if (m_pHoleOverlay[0] || m_pHoleOverlay[1])
	{
		m_texWidth = m_pHoleOverlay[0]->GetWidth();
		m_texHeight = m_pHoleOverlay[0]->GetHeight();
	}
	m_outFrames.reserve(3);
	MoveWindow(GetCalcPoint());
	return true;
}

void FrNextHole::SetNextHole(int hole)
{
	if (m_pHoleOverlay[0] && m_pHoleOverlay[1])
	{
		if (hole > 10 || hole == 19 ||
			Doc()->m_holeOrder[Doc()->m_golfGame.holes - 1] ==
				Doc()->m_pGolfDoc->m_currentHole)
		{
			m_overlayIndex = 1;
			hole -= 10;
		}
		else
			m_overlayIndex = 0;
		if (hole == 19 ||
			Doc()->m_holeOrder[Doc()->m_golfGame.holes - 1] ==
				Doc()->m_pGolfDoc->m_currentHole)
			CalculateUVPos(m_uvRect, m_texWidth, m_texHeight, 140.f, 93.f, 8);
		else
			CalculateUVPos(m_uvRect, m_texWidth, m_texHeight, 140.f, 93.f,
				hole - 1);
	}
}

void FrNextHole::OnAreaNextHole_OwnerDraw(int param)
{
	if (m_state != 1)
	{
		WRect area(m_pNextHoleArea->GetRect());
		if (Doc()->m_roomInfo.realGameType == 14)
		{
			m_overlayIndex = 2;
			m_uvRect.x = 0;
			m_uvRect.y = 0;
			m_uvRect.w = 1;
			m_uvRect.h = 1;
		}
		if (m_pHoleOverlay[m_overlayIndex])
		{
			WRect rect(area.x, area.y, 140.f, 93.f);
			m_pHoleOverlay[m_overlayIndex]->Render(g_view, m_uvRect, rect, 0,
				0xffffffff, 0, 0);
		}
	}
}

void FrNextHole::OnArea_OutFrame_Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
		m_outFrames.push_back(area);
}

void FrNextHole::SetState(eState state)
{
	if (m_state == state)
	{
		if (IsEnabled() && m_state == 1)
		{
			Enable(false);
			SetVisible(false);
		}
	}
	else
	{
		m_state = state;
		if (state == 1)
		{
			Enable(false);
			SetVisible(false);
			return;
		}
		Enable(true);
		MoveWindow(GetCalcPoint());
		SetVisible(true);
		if (nsTHunter::IsWndSizeMinimum() == true)
			SetOutFrameVisible(false);
		else
			SetOutFrameVisible(true);
		SetNextHole(GetNextHole());
	}
}

WPoint FrNextHole::GetCalcPoint()
{
	WRect rect(GetRect());
	float offset = nsTHunter::IsWndSizeMinimum() == true ? 6.f : -38.f;
	WPoint point;
	point.x = (int)((g_view->GetWidth() - rect.w) * .5f) + 210.f;
	point.y = (int)(g_view->GetHeight() - rect.h + offset);
	return point;
}

void FrNextHole::SetOutFrameVisible(bool visible)
{
	int count = m_outFrames.size();
	for (int i = 0; i < count; ++i)
		if (m_outFrames[i])
			m_outFrames[i]->SetVisible(visible);
}

IMPLEMENT_OBJECT(FrTreasureGauge, FrForm)
BEGIN_FRESH_MSGMAP(FrTreasureGauge, FrForm)
ON_FRESH_VI("pointgauge", FRCMD_INIT, FrTreasureGauge::OnImageBarInit)
ON_FRESH_VI("gaugearrow", FRCMD_INIT, FrTreasureGauge::OnArea_Arrow_Init)
ON_FRESH_VI("gaugearrow", FRCMD_OWNERDRAW,
	FrTreasureGauge::OnArea_Arrow_OwnerDraw)
ON_FRESH_VI("outback1", FRCMD_INIT, FrTreasureGauge::OnArea_OutFrame_Init)
ON_FRESH_VI("outback2", FRCMD_INIT, FrTreasureGauge::OnArea_OutFrame_Init)
ON_FRESH_VI("outback3", FRCMD_INIT, FrTreasureGauge::OnArea_OutFrame_Init)
END_FRESH_MSGMAP()

FrTreasureGauge::FrTreasureGauge()
{
	m_pImageBar = NULL;
	m_pArrowArea = NULL;
	m_state = (eState)0;
}

FrTreasureGauge::~FrTreasureGauge()
{
}

void FrTreasureGauge::OnImageBarInit(int param)
{
	m_pImageBar = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pImageBar)
	{
		m_pImageBar->SetRange(0, 1000, 0);
		m_pImageBar->SetBarSpeed(250);
	}
}

bool FrTreasureGauge::OnInit()
{
	m_pArrowBmp = g_pFresh->RegisterBitmap("th_gage_arrow");
	m_outFrames.reserve(3);
	MoveWindow(GetCalcPoint());
	return true;
}

void FrTreasureGauge::SetTPoint()
{
	unsigned long point = CTHunter::Instance()->GetTreasurePoint();
	if (m_pImageBar)
		m_pImageBar->SetDestPos(point, false);
}

void FrTreasureGauge::SetState(eState state)
{
	if (m_state != state)
	{
		m_state = state;
		if (state == 1)
		{
			Enable(false);
			SetVisible(false);
			return;
		}
		Enable(true);
		MoveWindow(GetCalcPoint());
		SetVisible(true);
		if (nsTHunter::IsWndSizeMinimum() == true)
			SetOutFrameVisible(false);
		else
			SetOutFrameVisible(true);
		if (state == 3)
			SetTPoint();
	}
}

WPoint FrTreasureGauge::GetCalcPoint()
{
	WRect rect(GetRect());
	float offset = nsTHunter::IsWndSizeMinimum() == true ? 14.f : -30.f;
	WPoint point;
	point.x = (int)((g_view->GetWidth() - rect.w) * .5f) - 90.f;
	point.y = (int)(g_view->GetHeight() - rect.h + offset);
	return point;
}

void FrTreasureGauge::SetOutFrameVisible(bool visible)
{
	int count = m_outFrames.size();
	for (int i = 0; i < count; ++i)
		if (m_outFrames[i])
			m_outFrames[i]->SetVisible(visible);
}

void FrTreasureGauge::OnArea_Arrow_Init(int param)
{
	m_pArrowArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTreasureGauge::OnArea_Arrow_OwnerDraw(int param)
{
	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
	if (g)
	{
		static float position = (float)m_pImageBar->GetPixelPerHeight();
		if (m_pArrowBmp)
		{
			WRect area(m_pImageBar->GetRect());
			position = (float)m_pImageBar->GetPixelPerHeight();
			const Bitmap* bitmap = m_pArrowBmp;
			WRect rect(position + area.x - 7, area.y - 15,
				(float)bitmap->Width(), (float)bitmap->Height());
			g->DrawTexture(bitmap, rect, 0xffffffff, 0);
		}
	}
}

void FrTreasureGauge::OnArea_OutFrame_Init(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (area)
		m_outFrames.push_back(area);
}

IMPLEMENT_OBJECT(FrTreasureCourse, FrForm)
BEGIN_FRESH_MSGMAP(FrTreasureCourse, FrForm)
ON_FRESH_VI("selectmap", FRCMD_INIT, FrTreasureCourse::OnAreaSelectMapInit)
ON_FRESH_VI("bluelagoon", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("bluelagoon", FRCMD_LBUTTONUP,
	FrTreasureCourse::OnBtnMap_Lagoon_Down)
ON_FRESH_VI("bluelagoon", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Lagoon_OwnerDraw)
ON_FRESH_VI("bluewater", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("bluewater", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Water_Down)
ON_FRESH_VI("bluewater", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Water_OwnerDraw)
ON_FRESH_VI("sepiawind", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("sepiawind", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Sepia_Down)
ON_FRESH_VI("sepiawind", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Sepia_OwnerDraw)
ON_FRESH_VI("windhill", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("windhill", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Hill_Down)
ON_FRESH_VI("windhill", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Hill_OwnerDraw)
ON_FRESH_VI("wizwiz", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("wizwiz", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_WizWiz_Down)
ON_FRESH_VI("wizwiz", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_WizWiz_OwnerDraw)
ON_FRESH_VI("westwiz", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("westwiz", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_West_Down)
ON_FRESH_VI("westwiz", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_West_OwnerDraw)
ON_FRESH_VI("bluemoon", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("bluemoon", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Moon_Down)
ON_FRESH_VI("bluemoon", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Moon_OwnerDraw)
ON_FRESH_VI("silvia", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("silvia", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Silvia_Down)
ON_FRESH_VI("silvia", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Silvia_OwnerDraw)
ON_FRESH_VI("icecannon", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("icecannon", FRCMD_LBUTTONUP,
	FrTreasureCourse::OnBtnMap_Cannon_Down)
ON_FRESH_VI("icecannon", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Cannon_OwnerDraw)
ON_FRESH_VI("whitewiz", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("whitewiz", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_White_Down)
ON_FRESH_VI("whitewiz", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_White_OwnerDraw)
ON_FRESH_VI("shining", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("shining", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Shining_Down)
ON_FRESH_VI("shining", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Shining_OwnerDraw)
ON_FRESH_VI("pinkwind", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("pinkwind", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Pink_Down)
ON_FRESH_VI("pinkwind", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Pink_OwnerDraw)
ON_FRESH_VI("inferno", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("inferno", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Inferno_Down)
ON_FRESH_VI("inferno", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Inferno_OwnerDraw)
ON_FRESH_VI("icespa", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("icespa", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_IceSpa_Down)
ON_FRESH_VI("icespa", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_IceSpa_OwnerDraw)
ON_FRESH_VI("seaway", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("seaway", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_SeaWay_Down)
ON_FRESH_VI("seaway", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_SeaWay_OwnerDraw)
ON_FRESH_VI("panda", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("panda", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Panda_Down)
ON_FRESH_VI("panda", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Panda_OwnerDraw)
ON_FRESH_VI("wizcity", FRCMD_INIT, FrTreasureCourse::OnBtnMap_ALL_Init)
ON_FRESH_VV("wizcity", FRCMD_LBUTTONUP, FrTreasureCourse::OnBtnMap_Wizcity_Down)
ON_FRESH_VI("wizcity", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnBtnMap_Wizcity_OwnerDraw)
ON_FRESH_VI("random", FRCMD_INIT, FrTreasureCourse::OnBtnMap_Random_Init)
ON_FRESH_VV("random", FRCMD_LBUTTONDOWN, FrTreasureCourse::OnBtnMap_Random_Down)
ON_FRESH_VI("f1", FRCMD_INIT, FrTreasureCourse::OnViewerF1)
ON_FRESH_VI("f2", FRCMD_INIT, FrTreasureCourse::OnViewerF2)
ON_FRESH_VI("gauge1", FRCMD_INIT, FrTreasureCourse::OnImageBar)
ON_FRESH_VI("gauge2", FRCMD_INIT, FrTreasureCourse::OnGauge_SmallBar)
ON_FRESH_VI("gauge2", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnGauge_SmallBar_OwnerDraw)
ON_FRESH_VI("currentmap", FRCMD_INIT, FrTreasureCourse::OnViewer_CurrMap_Init)
ON_FRESH_VI("explane", FRCMD_INIT, FrTreasureCourse::OnArea_Explane_Init)
ON_FRESH_VI("explane", FRCMD_OWNERDRAW,
	FrTreasureCourse::OnArea_Explane_OwnerDraw)
ON_FRESH_VI("cloud", FRCMD_INIT, FrTreasureCourse::OnInit_Cloud_Area)
ON_FRESH_VI("cloud", FRCMD_OWNERDRAW, FrTreasureCourse::OnOwnerDraw_Cloud_Area)
END_FRESH_MSGMAP()

FrTreasureCourse::FrTreasureCourse()
{
	FrButton* button = NULL;
	m_mapBtns.reserve(20);
	m_mapBtns.insert(m_mapBtns.begin(), 20, button);
	for (int i = 0; i < 20; ++i)
	{
		m_pMapBmp[i] = NULL;
		m_gauge[i] = 0;
	}
	m_pRandomSmallBmp = NULL;
	m_unknown254 = 0;
	m_pRandomBtn = NULL;
	m_pCloudOverlay = NULL;
	m_pSelectMapArea = NULL;
	m_pViewerF1 = NULL;
	m_pViewerF2 = NULL;
	m_curMapIdx = 0;
	m_unknown1ec = 0;
	m_unknown26c = 0;
	m_unknown270 = 0;
	m_btnIndex = 0;
	m_unknown1e9 = 0;
	m_pImageBar = NULL;
	m_pSmallBar = NULL;
	m_gaugeIndex = -1;
	m_bMapIndexChanged = true;
}

FrTreasureCourse::~FrTreasureCourse()
{
	if (g_resrcmng && m_pCloudOverlay)
	{
		g_resrcmng->Release(m_pCloudOverlay);
		m_pCloudOverlay = NULL;
	}
}

bool FrTreasureCourse::OnInit()
{
	m_gaugePos[0] = WPoint(291.f, 390.f);
	m_gaugePos[1] = WPoint(129.f, 391.f);
	m_gaugePos[2] = WPoint(98.f, 261.f);
	m_gaugePos[3] = WPoint(177.f, 226.f);
	m_gaugePos[4] = WPoint(234.f, 146.f);
	m_gaugePos[5] = WPoint(170.f, 160.f);
	m_gaugePos[6] = WPoint(53.f, 349.f);
	m_gaugePos[7] = WPoint(230.f, 457.f);
	m_gaugePos[8] = WPoint(25.f, 231.f);
	m_gaugePos[9] = WPoint(298.f, 138.f);
	m_gaugePos[10] = WPoint(493.f, 278.f);
	m_gaugePos[11] = WPoint(187.f, 305.f);
	m_gaugePos[12] = WPoint(0.f, 0.f);
	m_gaugePos[13] = WPoint(329.f, 239.f);
	m_gaugePos[14] = WPoint(52.f, 151.f);
	m_gaugePos[15] = WPoint(406.f, 351.f);
	m_gaugePos[16] = WPoint(339.f, 329.f);
	m_gaugePos[19] = WPoint(257.f, 224.f);
	WRect rect(GetRect());
	for (int i = 0; i < 20; ++i)
		if (i != 12 && i != 17)
		{
			m_gaugePos[i].x += rect.x;
			m_gaugePos[i].y += rect.y;
		}
	m_pRandomSmallBmp = g_pFresh->RegisterBitmap("map_random_small");
	m_pCloudBmp = g_pFresh->RegisterBitmap("th_cloud");
	m_pCloudOverlay = g_resrcmng->GetOverlay("th_cloud.tga", 0);
	m_pGaugeTipBmp = g_pFresh->RegisterBitmap("th_gage_tip");
	m_pGaugeHighBmp = g_pFresh->RegisterBitmap("gauge_high");
	m_pGaugeLowBmp = g_pFresh->RegisterBitmap("gauge_low");
	m_pMapLockBmp = g_pFresh->RegisterBitmap("map_lock_small2");
	m_orgRect = GetRect();
	CTHunter::Instance()->DecideTMark();
	SetMapGauge();
	InitCloud();
	if (Doc()->m_curChannel.Type & 0x800)
	{
		unsigned char count = m_mapBtns.size();
		for (unsigned char j = 0; j < count; ++j)
		{
			if (j == 17 || j == 18)
				continue;
			IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
				(j > 127 && j != 253 ? j - 128 : j) | 0x28000000);
			if (course)
			{
				if (course->Difficulty > 3)
					m_mapBtns[j]->Enable(false);
				else
					switch (j)
					{
					case MAP_BLUE_WATER:
					case MAP_BLUE_MOON:
					case MAP_ICE_CANNON:
					case MAP_WHITE_WIZ:
					case MAP_ICE_SPA:
						m_mapBtns[j]->Enable(false);
						break;
					}
			}
		}
		if (m_pRandomBtn)
			m_pRandomBtn->Enable(false);
	}
	return true;
}

void FrTreasureCourse::OnProc(const float dt)
{
	ProcCloud(dt);
}

void FrTreasureCourse::InitCloud()
{
	int counts[] = { 3, 4 };
	srand((unsigned)time(NULL));
	int count = counts[rand() % 2];
	sCloud cloud;
	m_clouds.reserve(count);
	for (int i = 0; i < count; ++i)
		m_clouds.push_back(cloud);
	m_cloudNum = m_clouds.size();
	for (int j = 0; j < m_cloudNum; ++j)
		ReInitCloud(j);
}

void FrTreasureCourse::ReInitCloud(int index)
{
	float scales[] = { .25f, .5f, 1.f };
	float numbers[] = { 0.f, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f };
	WRect form(GetRect());
	WRect area(m_pCloudArea->GetRect());
	float height = form.h / m_cloudNum;
	float start = index * height;
	float minimum = area.y + index * height;
	float maximum = area.y + area.h - (area.y + 20.f);
	float y = Between(minimum, start + height / numbers[rand() % 10], maximum);
	WRect rect;
	rect = WRect(0.f, 0.f, m_pCloudBmp->Width(), m_pCloudBmp->Height());
	float cloudHeight = rect.h;
	sCloud cloud;
	cloud.speed = Between(5.f, Random(1.f, 10.f), 10.f);
	cloud.scale = scales[rand() % 4];
	float random = numbers[rand() % 10];
	rect.x = (float)(m_pCloudArea->GetRect().w * .1f) * random;
	if (1.f - cloud.scale >= .0001f)
	{
		rect.w *= cloud.scale;
		rect.h = cloud.scale * cloudHeight;
		if (cloud.scale != 1.f)
		{
			rect.w += .5f;
			rect.h += .5f;
		}
	}
	if (rect.w + y > maximum)
		y -= rect.w;
	rect.y = y;
	cloud.rect = rect;
	cloud.alpha = 0;
	if (m_pCloudArea->GetRect().x < rect.x)
	{
		cloud.bEntered = true;
		cloud.alpha = 255;
	}
	sCloud& dest = m_clouds[index];
	dest = cloud;
}

void FrTreasureCourse::ProcCloud(float dt)
{
	for (unsigned char i = 0; i < m_cloudNum; ++i)
	{
		CheckArea(i, dt);
		m_clouds[i].rect.x += dt * m_clouds[i].speed;
	}
}

void FrTreasureCourse::CheckArea(int index, float dt)
{
	WRect area(m_pCloudArea->GetRect());
	WRect rect(m_clouds[index].rect);
	float right = area.x + area.w;
	float edge = rect.x + rect.w;
	if (area.x <= rect.x && edge < area.x + rect.w + rect.w)
		ShiftAlpha(index, dt, eALPHA_INC);
	else if (edge > right && right + edge > rect.x)
		ShiftAlpha(index, dt, eALPHA_DEC);
	if (rect.x > right)
		ReInitCloud(index);
}

void FrTreasureCourse::ShiftAlpha(int index, float dt, eALPHA mode)
{
	if (mode == eALPHA_INC)
	{
		if (!m_clouds[index].bEntered)
		{
			if (m_clouds[index].alpha < 255)
				++m_clouds[index].alpha;
			else
				m_clouds[index].alpha = 255;
		}
	}
	else if (mode == eALPHA_DEC)
	{
		if (m_clouds[index].alpha > 0)
			m_clouds[index].alpha -= 5;
		else
			m_clouds[index].alpha = 0;
	}
}

void FrTreasureCourse::OnAreaSelectMapInit(int param)
{
	m_pSelectMapArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTreasureCourse::OnBtnMap_ALL_Init(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (button)
	{
		if (m_btnIndex == 12 || m_btnIndex == 17)
			++m_btnIndex;
		if (m_btnIndex == 18)
			++m_btnIndex;
		m_mapBtns[m_btnIndex++] = button;
		if (m_btnIndex == m_mapBtns.size())
			m_btnIndex = 0;
	}
}

void FrTreasureCourse::OnBtnMap_Lagoon_Down()
{
	SelectMapBtn(MAP_BLUE_LAGOON);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Lagoon_OwnerDraw(int param)
{
	DrawMapBtn(MAP_BLUE_LAGOON);
}

void FrTreasureCourse::OnBtnMap_Water_Down()
{
	SelectMapBtn(MAP_BLUE_WATER);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Water_OwnerDraw(int param)
{
	DrawMapBtn(MAP_BLUE_WATER);
}

void FrTreasureCourse::OnBtnMap_Sepia_Down()
{
	SelectMapBtn(MAP_SEPIA_WIND);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Sepia_OwnerDraw(int param)
{
	DrawMapBtn(MAP_SEPIA_WIND);
}

void FrTreasureCourse::OnBtnMap_Hill_Down()
{
	SelectMapBtn(MAP_WIND_HILL);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Hill_OwnerDraw(int param)
{
	DrawMapBtn(MAP_WIND_HILL);
}

void FrTreasureCourse::OnBtnMap_WizWiz_Down()
{
	SelectMapBtn(MAP_WIZ_WIZ);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_WizWiz_OwnerDraw(int param)
{
	DrawMapBtn(MAP_WIZ_WIZ);
}

void FrTreasureCourse::OnBtnMap_West_Down()
{
	SelectMapBtn(MAP_WEST_WIZ);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_West_OwnerDraw(int param)
{
	DrawMapBtn(MAP_WEST_WIZ);
}

void FrTreasureCourse::OnBtnMap_Moon_Down()
{
	SelectMapBtn(MAP_BLUE_MOON);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Moon_OwnerDraw(int param)
{
	DrawMapBtn(MAP_BLUE_MOON);
}

void FrTreasureCourse::OnBtnMap_Silvia_Down()
{
	SelectMapBtn(MAP_SILVIA_CANNON);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Silvia_OwnerDraw(int param)
{
	DrawMapBtn(MAP_SILVIA_CANNON);
}

void FrTreasureCourse::OnBtnMap_Cannon_Down()
{
	SelectMapBtn(MAP_ICE_CANNON);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Cannon_OwnerDraw(int param)
{
	DrawMapBtn(MAP_ICE_CANNON);
}

void FrTreasureCourse::OnBtnMap_White_Down()
{
	SelectMapBtn(MAP_WHITE_WIZ);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_White_OwnerDraw(int param)
{
	DrawMapBtn(MAP_WHITE_WIZ);
}

void FrTreasureCourse::OnBtnMap_Shining_Down()
{
	SelectMapBtn(MAP_SHINING_SAND);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Shining_OwnerDraw(int param)
{
	DrawMapBtn(MAP_SHINING_SAND);
}

void FrTreasureCourse::OnBtnMap_Pink_Down()
{
	SelectMapBtn(MAP_PINK_WIND);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Pink_OwnerDraw(int param)
{
	DrawMapBtn(MAP_PINK_WIND);
}

void FrTreasureCourse::OnBtnMap_Inferno_Down()
{
	SelectMapBtn(MAP_DEEP_INFERNO);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Inferno_OwnerDraw(int param)
{
	DrawMapBtn(MAP_DEEP_INFERNO);
}

void FrTreasureCourse::OnBtnMap_IceSpa_Down()
{
	SelectMapBtn(MAP_ICE_SPA);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_IceSpa_OwnerDraw(int param)
{
	DrawMapBtn(MAP_ICE_SPA);
}

void FrTreasureCourse::OnBtnMap_SeaWay_Down()
{
	SelectMapBtn(MAP_LOST_SEAWAY);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_SeaWay_OwnerDraw(int param)
{
	DrawMapBtn(MAP_LOST_SEAWAY);
}

void FrTreasureCourse::OnBtnMap_Panda_Down()
{
	SelectMapBtn(MAP_EASTERN_VALLEY);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Panda_OwnerDraw(int param)
{
	DrawMapBtn(MAP_EASTERN_VALLEY);
}

void FrTreasureCourse::OnBtnMap_Wizcity_Down()
{
	SelectMapBtn(MAP_WIZ_CITY);
	Close(true);
}

void FrTreasureCourse::OnBtnMap_Wizcity_OwnerDraw(int param)
{
	DrawMapBtn(MAP_WIZ_CITY);
}

void FrTreasureCourse::OnBtnMap_Random_Init(int param)
{
	m_pRandomBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTreasureCourse::OnBtnMap_Random_Down()
{
	SelectMapBtn(MAP_RANDOM);
	Close(true);
}

void FrTreasureCourse::OnViewerF1(int param)
{
	m_pViewerF1 = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pViewerF1->SetMouseEvent(false);
	m_pViewerF1->Open("th_quick_frm1.tga");
}

void FrTreasureCourse::OnViewerF2(int param)
{
	m_pViewerF2 = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	m_pViewerF2->SetMouseEvent(false);
	m_pViewerF2->Open("th_quick_frm2.tga");
}

void FrTreasureCourse::OnImageBar(int param)
{
	m_pImageBar = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pImageBar)
	{
		m_pImageBar->SetRange(0, 300, 0);
		m_pImageBar->SetBarSpeed(200);
		m_pImageBar->SetVisible(false);
	}
}

void FrTreasureCourse::OnGauge_SmallBar(int param)
{
	m_pSmallBar = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pSmallBar)
	{
		m_pSmallBar->SetRange(0, 300, 0);
		m_pSmallBar->SetBarSpeed(200);
	}
}

void FrTreasureCourse::OnGauge_SmallBar_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
	{
		const Bitmap* back = m_pSmallBar->GetGaugeSkin(0);
		const Bitmap* bar = m_pSmallBar->GetGaugeSkin(1);
		if (back && bar)
			for (int i = 0; i < 20; ++i)
				if (i != 12 && i != 17 && m_gaugeIndex != i)
				{
					m_pSmallBar->SetPos(nsTHunter::CalcGauge(m_gauge[i]));
					gi->DrawTexture(back,
						WRect(m_gaugePos[i].x, m_gaugePos[i].y,
							(float)back->Width(), (float)back->Height()),
						0xffffffff, 0);
					gi->DrawTexture(bar,
						WRect(m_gaugePos[i].x, m_gaugePos[i].y,
							(float)bar->Width(), (float)bar->Height()),
						0xffffffff, 0);
					m_pSmallBar->ResetGauge();
				}
	}
}

void FrTreasureCourse::OnViewer_CurrMap_Init(int param)
{
	m_pCurrMapViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void FrTreasureCourse::OnArea_Explane_Init(int param)
{
	m_pExplaneArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTreasureCourse::OnArea_Explane_OwnerDraw(int param)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi && m_pExplaneArea && m_pGaugeTipBmp)
	{
		bool inside;
		{
			WRect rect(m_pExplaneArea->GetRect());
			inside = rect.IsInRect(g_input->GetMousePoint().x,
				g_input->GetMousePoint().y);
		}
		if (inside)
			gi->DrawTexture(m_pGaugeTipBmp,
				WRect(610.f, 100.f, (float)m_pGaugeTipBmp->Width(),
					(float)m_pGaugeTipBmp->Height()),
				0xffffffff, 0);
	}
}

void FrTreasureCourse::OnInit_Cloud_Area(int param)
{
	m_pCloudArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTreasureCourse::OnOwnerDraw_Cloud_Area(int param)
{
	if (m_pCloudOverlay)
		for (int i = 0; i < m_cloudNum; ++i)
			m_pCloudOverlay->Render(g_view, WRect(0.f, 0.f, 1.f, 1.f),
				m_clouds[i].rect, 0, (m_clouds[i].alpha << 24) | 0xffffff, 0,
				0);
}

void FrTreasureCourse::OpenSelectedMap(unsigned char map)
{
}

void FrTreasureCourse::DrawMapBtn(eMapType map)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (gi)
	{
		WRect rect(m_mapBtns[map]->GetRect());
		float x = rect.w * .5f - (m_pMapLockBmp->Width() >> 1);
		float y = rect.h * .5f - (m_pMapLockBmp->Height() >> 1);
		if (Doc()->m_curChannel.Type & 0x800)
		{
			x = floor(x);
			IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
				(map > 127 && map != 253 ? map - 128 : map) | 0x28000000);
			if (!course)
				return;
			if (course->Difficulty > 3)
				gi->DrawTexture(m_pMapLockBmp,
					WRect(rect.x + x, rect.y + y, (float)m_pMapLockBmp->Width(),
						(float)m_pMapLockBmp->Height()),
					0xffffffff, 0);
			else
				switch (map)
				{
				case MAP_BLUE_WATER:
				case MAP_BLUE_MOON:
				case MAP_ICE_CANNON:
				case MAP_WHITE_WIZ:
				case MAP_ICE_SPA:
					gi->DrawTexture(m_pMapLockBmp,
						WRect(rect.x + x, rect.y + y,
							(float)m_pMapLockBmp->Width(),
							(float)m_pMapLockBmp->Height()),
						0xffffffff, 0);
					break;
				}
		}
		if (m_mapBtns[map]->GetStatus() == FrButton::OVER)
		{
			WPoint pos(m_gaugePos[map].x - 15.f, m_gaugePos[map].y);
			OpenGauge(pos, CMapTypeAdapter::GetMapIndex(map));
		}
	}
}

void FrTreasureCourse::DrawCurrentMap(FrGraphicInterface* gi, int map)
{
	WRect area(GetRect());
	if (map != MAP_RANDOM)
		gi->DrawTexture(m_pMapBmp[map],
			WRect(area.w - 5.f, area.y + 47.f, (float)m_pMapBmp[map]->Width(),
				(float)m_pMapBmp[map]->Height()),
			0xffffffff, 0);
	else
		gi->DrawTexture(m_pRandomSmallBmp,
			WRect(area.w - 5.f, area.y + 47.f,
				(float)m_pRandomSmallBmp->Width(),
				(float)m_pRandomSmallBmp->Height()),
			0xffffffff, 0);
}

void FrTreasureCourse::SetMapGauge()
{
	for (int i = 0; i < 20; ++i)
	{
		unsigned long gauge =
			CTHunter::Instance()->GetTHunterGauge((eMapType)i);
		if (gauge != 9999)
			m_gauge[i] = gauge;
	}
}

bool FrTreasureCourse::OpenGauge(WPoint& pos, int index)
{
	if (index == -1)
		return false;
	if (m_gaugeIndex == index)
		return false;
	m_gaugeIndex = index;
	m_pImageBar->MoveWindow(pos);
	m_pImageBar->ResetGauge();
	m_pImageBar->SetRange(0, 300, 0);
	m_pImageBar->SetDestPos(nsTHunter::CalcGauge(m_gauge[index]), false);
	m_pImageBar->SetVisible(true);
	return true;
}

void FrTreasureCourse::OpenCurrentMap(int map)
{
	if (m_gaugeIndex != map)
	{
		IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
			(map > 127 && map != 253 ? map - 128 : map) | 0x28000000);
		if (course)
		{
			if (map < 127)
				m_pCurrMapViewer->Open(MakeStr("mid_%s.tga", course->c.Icon));
			else
				m_pCurrMapViewer->Open("map_random_small.tga");
		}
	}
}

void FrTreasureCourse::SelectMapBtn(unsigned char map)
{
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
		(map > 127 && map != 253 ? map - 128 : map) | 0x28000000);
	if (course)
	{
		if (m_curMapIdx == map)
			SetMapIndexChanged(false);
		else
			SetMapIndexChanged(true);
		if (GetMapIndexChanged())
		{
			m_pCurrMapViewer->Open(MakeStr("mid_%s", course->c.Icon));
		}
		if (GetMapIndexChanged() &&
			(!stricmp(g_pFresh->GetManager()->GetLayoutID(), "GAMEROOM") ||
				!stricmp(g_pFresh->GetManager()->GetLayoutID(),
					"GAMEROOM_EXT") ||
				!stricmp(g_pFresh->GetManager()->GetLayoutID(),
					"AVATARCHAT_MAIN")))
		{
			WSendPacket packet((enumClientPacket)10);
			packet.Encode2(0xffff);
			packet.Encode1(1);
			packet.Encode1(3);
			packet.Encode1((unsigned char)course->c.TypeId);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			m_curMapIdx = CMapTypeAdapter::GetMapIndex(
				(eMapType)(course->c.TypeId & 0x3ffffff));
			return;
		}
		m_curMapIdx = CMapTypeAdapter::GetMapIndex(
			(eMapType)(course->c.TypeId & 0x3ffffff));
	}
}

void FrTreasureCourse::SetSelectMap(unsigned char map)
{
	m_curMapIdx = CMapTypeAdapter::GetMapIndex((eMapType)map);
	OpenCurrentMap(map);
}

void FrTreasureCourse::SetCurMapIdx(unsigned char index)
{
	m_curMapIdx = index;
}

unsigned char FrTreasureCourse::GetCurMapIdx()
{
	return m_curMapIdx;
}

bool FrTreasureCourse::GetMapIndexChanged() const
{
	return m_bMapIndexChanged;
}

void FrTreasureCourse::SetMapIndexChanged(bool changed)
{
	m_bMapIndexChanged = changed;
}

IMPLEMENT_OBJECT(FrTreasureScore, FrForm)
BEGIN_FRESH_MSGMAP(FrTreasureScore, FrForm)
ON_FRESH_VI("imagebar", FRCMD_INIT, FrTreasureScore::OnGBarImage_ImageBar_Init)
ON_FRESH_VI("back", FRCMD_INIT, FrTreasureScore::OnView_Backgound_Init)
ON_FRESH_VI("team", FRCMD_INIT, FrTreasureScore::OnArea_Team_Init)
END_FRESH_MSGMAP()

FrTreasureScore::FrTreasureScore()
	: m_pImageBar(NULL),
	  m_pBackground(NULL),
	  m_pTeamArea(NULL),
	  m_pTeamBlueBmp(NULL),
	  m_pTeamRedBmp(NULL)
{
}

FrTreasureScore::~FrTreasureScore()
{
}

bool FrTreasureScore::OnInit()
{
	MoveWindow(GetCalcPoint());
	char filename[128];
	sprintf(filename, "th_result_%s.tga",
		Doc()->m_golfGame.pCourse ? Doc()->m_golfGame.pCourse->Data : NULL);
	if (m_pBackground)
		m_pBackground->Open(filename);
	m_pTeamBlueBmp = g_pFresh->RegisterBitmap("th_team_blue");
	m_pTeamRedBmp = g_pFresh->RegisterBitmap("th_team_red");
	IsTeamMode();
	SetTPoint();
	return true;
}

void FrTreasureScore::SetTPoint()
{
	unsigned long point = CTHunter::Instance()->GetTreasurePoint();
	if (m_pImageBar)
		m_pImageBar->SetDestPos(point, false);
}

void FrTreasureScore::SetState(bool show)
{
	if (m_bShow != show)
	{
		m_bShow = show;
		if (!show)
		{
			SetVisible(false);
			return;
		}
		MoveWindow(GetCalcPoint());
		SetVisible(true);
	}
}

WPoint FrTreasureScore::GetCalcPoint()
{
	WRect rect(GetRect());
	WPoint point;
	point.x = (int)((g_view->GetWidth() - rect.w) * .5f);
	point.y = (int)((g_view->GetHeight() - rect.h) * .5f) - 20.f;
	return point;
}

void FrTreasureScore::IsTeamMode()
{
	if (Doc()->m_golfGame.gameType != 1)
	{
		m_pTeamArea->SetVisible(false);
		return;
	}
	for (int i = 0; i < GetPlayerNum(); ++i)
	{
		unsigned long id = Doc()->m_myInfo.info.dwGuid;
		if (id == PLAYER(i)->oid)
		{
			const Bitmap* bitmap =
				PLAYER(i)->team == 0 ? m_pTeamBlueBmp : m_pTeamRedBmp;
			if (bitmap)
				m_pTeamArea->SetBgImg(bitmap);
			break;
		}
	}
}

void FrTreasureScore::OnGBarImage_ImageBar_Init(int param)
{
	m_pImageBar = DYNAMIC_CAST(FrGaugeBarImage, (FrWnd*)param);
	if (m_pImageBar)
	{
		m_pImageBar->SetRange(0, 1000, 0);
		m_pImageBar->SetBarSpeed(600);
	}
}

void FrTreasureScore::OnView_Backgound_Init(int param)
{
	m_pBackground = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pBackground)
		m_pBackground->SetMouseEvent(false);
}

void FrTreasureScore::OnArea_Team_Init(int param)
{
	m_pTeamArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

CTFrame::CTFrame()
	: m_frameCount(3)
{
	m_frames.reserve(m_frameCount);
}

CTFrame::CTFrame(unsigned char count)
	: m_frameCount(count)
{
	m_frames.reserve(count);
}

CTFrame::~CTFrame()
{
	Reset();
}

void CTFrame::AddFrame(FrArea* frame)
{
	if (frame)
		m_frames.push_back(frame);
}

bool CTFrame::SetFrameWidth(float width)
{
	if (0 == width)
		return false;
	unsigned char first, last;
	unsigned char count = (unsigned char)m_frames.size();
	if (count == 3)
	{
		first = 3;
		last = 6;
	}
	else if (count == 9)
	{
		first = 0;
		last = 3;
	}
	for (unsigned char i = first; i < last; ++i)
	{
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.w += width;
		m_frames[i]->SetRect(rect);
	}
	return true;
}

bool CTFrame::SetFrameHeight(float height)
{
	if (0 == height)
		return false;
	unsigned char first, middle, last;
	if ((unsigned char)m_frames.size() == 6)
	{
		first = 0;
		middle = 3;
		last = 6;
	}
	for (unsigned char i = first; i < middle; ++i)
	{
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.h += height;
		m_frames[i]->SetRect(rect);
	}
	for (unsigned char i = middle; i < last; ++i)
	{
		WRect rect;
		rect = m_frames[i]->GetRect();
		rect.y += height;
		m_frames[i]->SetRect(rect);
	}
	return true;
}

IMPLEMENT_OBJECT(FrTreasureGift, FrForm)
BEGIN_FRESH_MSGMAP(FrTreasureGift, FrForm)
ON_FRESH_VI("outback3", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("outback4", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("outback5", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("outback6", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("outback7", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("outback8", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_Out)
ON_FRESH_VI("inback3", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("inback4", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("inback5", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("inback6", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("inback7", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("inback8", FRCMD_INIT, FrTreasureGift::OnInit_RangeArea_In)
ON_FRESH_VI("grid", FRCMD_INIT, FrTreasureGift::OnInit_GridArea)
ON_FRESH_VI("grid", FRCMD_OWNERDRAW, FrTreasureGift::OnOwnerDraw_GridArea)
ON_FRESH_VI("giftlist", FRCMD_INIT, FrTreasureGift::OnInit_Gift_List)
ON_FRESH_VI("giftlist", FRCMD_OWNERDRAW, FrTreasureGift::OnOwnerDraw_Gift_List)
ON_FRESH_VI("box", FRCMD_INIT, FrTreasureGift::OnInit_Box_Area)
ON_FRESH_VI("box", FRCMD_OWNERDRAW, FrTreasureGift::OnOwnerDraw_Box_Area)
END_FRESH_MSGMAP()

FrTreasureGift::FrTreasureGift()
	: m_colNum(8)
{
	m_rowNum = 1;
	m_bShow = true;
}

FrTreasureGift::~FrTreasureGift()
{
}

bool FrTreasureGift::OnInit()
{
	m_boxOpener.OnInit();
	MoveWindow(GetCalcPoint());
	CTHunter::Instance()->GetTreasurePoint();
	SetPointByFrameHeight();
	m_rowNum = m_boxOpener.m_slotNum / 8;
	return true;
}

void FrTreasureGift::SetPointByFrameHeight()
{
	float outer, inner;
	if (m_boxOpener.m_giftNum > 8 && m_boxOpener.m_giftNum < 17)
	{
		outer = 65.f;
		inner = 64.f;
	}
	else if (m_boxOpener.m_giftNum > 16 && m_boxOpener.m_giftNum < 25)
	{
		outer = 129.f;
		inner = 128.f;
	}
	else
		return;
	m_outRange.SetFrameHeight(outer);
	m_inRange.SetFrameHeight(inner);
}

WPoint FrTreasureGift::GetCalcPoint()
{
	WRect rect(GetRect());
	WPoint point;
	point.x = (int)((g_view->GetWidth() - rect.w) * .5f);
	point.y = (int)((g_view->GetHeight() - rect.h) * .5f) + 50.f;
	return point;
}

void FrTreasureGift::SetState(bool show)
{
	if (m_bShow != show)
	{
		m_bShow = show;
		if (!show)
		{
			SetVisible(false);
			return;
		}
		MoveWindow(GetCalcPoint());
		SetVisible(true);
	}
}

void FrTreasureGift::OnOwnerDraw_Gift_List(int param)
{
	if (param)
		m_boxOpener.DrawGiftList((FrListItem*)param);
}

void FrTreasureGift::OnProc(const float dt)
{
	m_boxOpener.OnProc(dt);
}

void FrTreasureGift::OnInit_RangeArea_Out(int param)
{
	m_outRange.AddFrame(DYNAMIC_CAST(FrArea, (FrWnd*)param));
}

void FrTreasureGift::OnInit_RangeArea_In(int param)
{
	m_inRange.AddFrame(DYNAMIC_CAST(FrArea, (FrWnd*)param));
}

void FrTreasureGift::OnInit_Gift_List(int param)
{
	m_boxOpener.m_pGiftList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrTreasureGift::OnInit_GridArea(int param)
{
	m_pGridArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrTreasureGift::OnOwnerDraw_GridArea(int param)
{
	static unsigned long color = 0xffa2d7fd;
	WRect area(m_boxOpener.m_pGiftList->GetRect());
	{
		WRect line(area.x + 25.f, area.y + 30.f, area.w - 65.f, 1.f);
		WOverlay::DrawBox(g_view, WRect(line), 0, color, 0);
	}
	if (m_rowNum == 2)
	{
		WRect line(area.x + 25.f, area.y + 98.f, area.w - 65.f, 1.f);
		WOverlay::DrawBox(g_view, WRect(line), 0, color, 0);
	}
	else if (m_rowNum == 3)
	{
		WRect line(area.x + 25.f, area.y + 166.f, area.w - 65.f, 1.f);
		WOverlay::DrawBox(g_view, WRect(line), 0, color, 0);
	}
	for (unsigned char i = 0; i < 8; ++i)
	{
		WRect line((float)(i * 65.f) + area.x + 33.f, area.y + 35.f, 1.f,
			area.h - 60.f);
		WOverlay::DrawBox(g_view, WRect(line), 0, color, 0);
	}
}

void FrTreasureGift::OnInit_Box_Area(int param)
{
	m_boxOpener.m_pBoxArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_boxOpener.m_pBoxArea)
	{
		WRect rect(m_boxOpener.m_pBoxArea->GetRect());
		rect.y = GetRect().y - 10.f;
		m_boxOpener.m_pBoxArea->SetRect(rect);
	}
}

void FrTreasureGift::OnOwnerDraw_Box_Area(int param)
{
	m_boxOpener.DrawBoxArea();
}

IMPLEMENT_OBJECT(FrTestDlg_combo, FrForm)
BEGIN_FRESH_MSGMAP(FrTestDlg_combo, FrForm)
ON_FRESH_VI("mapcomboex", FRCMD_INIT, FrTestDlg_combo::OnInit_combo)
ON_FRESH_VV("mapcomboex", FRCMD_LBUTTONDOWN, FrTestDlg_combo::ONLBTN_DOWN)
END_FRESH_MSGMAP()

FrTestDlg_combo::FrTestDlg_combo()
{
	m_pCombo = NULL;
}

FrTestDlg_combo::~FrTestDlg_combo()
{
}

void FrTestDlg_combo::OnInit_combo(int param)
{
	m_pCombo = DYNAMIC_CAST(FrComboCtlEx, (FrWnd*)param);
	if (m_pCombo)
	{
		m_pCombo->AddTexItem("hole_front");
		m_pCombo->AddTexItem("hole_back");
		m_pCombo->AddTexItem("hole_random");
		m_pCombo->AddTexItem("hole_shuffle");
		m_pCombo->SelectItem(0);
	}
}

bool FrTestDlg_combo::OnInit()
{
	return true;
}

void FrTestDlg_combo::ONLBTN_DOWN()
{
	m_pCombo->GetCurrentIdx();
}

CBoxOpener::CBoxOpener()
{
	m_boxType = 0;
	m_progress = 0;
	m_giftNum = 0;
	m_slotNum = 0;
	m_pGiftList = NULL;
	m_nextSeq = eSEQ_NONE;
	m_reserveSeq = eSEQ_NONE;
	m_curSeq = eSEQ_NONE;
	m_bApproach = false;
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	m_delay = 2000;
	char* files[] = { "[font_wind.jpg" };
	font.filename = files;
	font.fontw = 16;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 64;
	font.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&font);
}

CBoxOpener::~CBoxOpener()
{
	if (m_pOpenAni)
	{
		delete m_pOpenAni;
		m_pOpenAni = NULL;
	}
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
	m_bApproach = false;
}

bool CBoxOpener::OnInit()
{
	if (m_bApproach == true)
		CTHunter::Instance()->ReAdjustRepository();
	m_giftNum = CTHunter::Instance()->GetRepositorySize();
	if (m_giftNum < 9)
		m_slotNum = 8;
	else if (m_giftNum > 8 && m_giftNum < 17)
		m_slotNum = 16;
	else if (m_giftNum > 16 && m_giftNum < 25)
		m_slotNum = 24;
	else
		m_slotNum = 8;
	m_gifts.reserve(m_slotNum);
	sTGift gift;
	for (unsigned char i = 0; i < m_slotNum; ++i)
	{
		if (i < m_giftNum)
		{
			sTreasureHunt info;
			info = CTHunter::Instance()->GetGiftInfo(i);
			gift.index = i;
			gift.typeId = info.dwTid;
			gift.quantity = info.wCount;
			gift.bMine = info.dwUID == Doc()->m_myInfo.info.dwUID;
		}
		else
			gift.Reset();
		m_gifts.push_back(gift);
		m_pGiftList->AddItem(&m_gifts[i]);
	}
	WRect rect(m_pGiftList->GetRect());
	rect.h = (m_slotNum / 8) * 68.f;
	m_pGiftList->SetRect(rect);
	m_pBoxCloseBmp = g_pFresh->RegisterBitmap("th_item_box_n");
	m_pBoxOpenBmp = g_pFresh->RegisterBitmap("th_item_box_o");
	m_pMark1Bmp = g_pFresh->RegisterBitmap("th_item_mark1");
	m_pMark2Bmp = g_pFresh->RegisterBitmap("th_item_mark2");
	m_pMsgBmp = g_pFresh->RegisterBitmap("th_msg1");
	m_pBoxFrameBmp = g_pFresh->RegisterBitmap("th_box_frm");
	for (int j = 1; j < 5; ++j)
		m_pBoxBmp[j - 1] = g_pFresh->RegisterBitmap(MakeStr("th_box_%d", j));
	m_pOpenAni = new CSimpleUI("[emoti_object.png");
	m_pOpenAni->Add("0", WRect(0, 128, 64, 64), false);
	m_pOpenAni->Add("1", WRect(64, 128, 64, 64), false);
	m_pOpenAni->Add("2", WRect(128, 128, 64, 64), false);
	m_pOpenAni->Add("3", WRect(192, 128, 64, 64), false);
	m_pOpenAni->Add("4", WRect(0, 192, 64, 64), false);
	m_pOpenAni->Add("5", WRect(64, 192, 64, 64), false);
	m_pOpenAni->Add("6", WRect(128, 192, 64, 64), false);
	m_pOpenAni->Add("7", WRect(192, 192, 64, 64), false);
	m_boxType = DecideTRBox();
	if (m_boxType > 5)
		m_boxType = 1;
	ChangeSequence(eSEQ_INIT);
	return true;
}

void CBoxOpener::OnProc(const float dt)
{
	m_dt = dt;
	if (m_reserveSeq != eSEQ_NONE)
		ChangeSequence(m_reserveSeq);
	if (m_pfnProc)
		(this->*m_pfnProc)();
}

void CBoxOpener::ChangeSequence(eSeqProc seq)
{
	switch (seq)
	{
	case eSEQ_INIT:
		m_lastTime = timeGetTime();
		m_pfnProc = &CBoxOpener::ProcInit;
		CTHunter::Instance()->SetReturnLobby(false);
		break;
	case eSEQ_CREATE:
		m_lastTime = timeGetTime();
		m_delay = 250;
		m_pfnProc = &CBoxOpener::ProcCreate;
		m_nextSeq = eSEQ_MYITEM_OPEN;
		break;
	case eSEQ_MYITEM_OPEN:
		m_lastTime = timeGetTime();
		m_pfnProc = &CBoxOpener::ProcMyItemOpen;
		m_nextSeq = eSEQ_COMPLETE;
		for (unsigned char i = 0; i < m_giftNum; ++i)
			m_pGiftList->GetItem(i);
		m_progress = 0;
		m_delay = 200;
		break;
	case eSEQ_COMPLETE:
		m_lastTime = timeGetTime();
		m_pfnProc = &CBoxOpener::ProcComplete;
		m_delay = 1500;
		break;
	case eSEQ_END:
		m_lastTime = timeGetTime();
		m_pfnProc = &CBoxOpener::ProcEnd;
		m_delay = 3000;
		break;
	case eSEQ_FINISH:
		CTHunter::Instance()->SetReturnLobby(true);
		break;
	}
	m_curSeq = seq;
	m_reserveSeq = eSEQ_NONE;
}

void CBoxOpener::ProcInit()
{
	if (timeGetTime() - m_lastTime > 1000)
	{
		for (unsigned char i = 0; i < m_pGiftList->GetCurrentItemSize(true);
			++i)
			GetGiftItem(i)->state = 1;
		m_reserveSeq = eSEQ_CREATE;
	}
}

void CBoxOpener::ProcCreate()
{
	if (timeGetTime() - m_lastTime > m_delay)
	{
		if (m_progress < m_giftNum)
		{
			sTGift* gift = GetGiftItem(m_progress++);
			if (gift && gift->typeId)
			{
				gift->bItem = true;
				gift->state = 3;
				g_audio->PlaySfx("play_fail_item");
			}
		}
		else if (m_progress >= m_giftNum && m_progress < m_slotNum)
			GetGiftItem(m_progress++)->state = 2;
		else
			++m_progress;
		m_lastTime = timeGetTime();
		if (m_progress == m_giftNum)
			m_delay = 50;
		if (m_progress == m_slotNum)
			m_reserveSeq = eSEQ_MYITEM_OPEN;
	}
}

void CBoxOpener::ProcMyItemOpen()
{
	if (m_progress < m_giftNum)
	{
		if (timeGetTime() - m_lastTime > 250 && m_progress < m_giftNum)
		{
			sTGift* gift = GetGiftItem(m_progress++);
			if (gift && gift->typeId)
			{
				gift->state = 4;
				gift->frame = 0;
				g_audio->PlaySfx("play_fail_item");
			}
			else
				++m_progress;
			m_lastTime = timeGetTime();
		}
	}
	if (m_progress == m_giftNum && timeGetTime() - m_lastTime > m_delay)
		m_reserveSeq = eSEQ_COMPLETE;
}

void CBoxOpener::ProcComplete()
{
	if (timeGetTime() - m_lastTime > m_delay && m_curSeq == eSEQ_COMPLETE)
		m_reserveSeq = eSEQ_FINISH;
}

void CBoxOpener::ProcBoxNone()
{
}

void CBoxOpener::ProcEnd()
{
	if (timeGetTime() - m_lastTime > m_delay && m_curSeq == eSEQ_END)
		m_reserveSeq = eSEQ_FINISH;
}

CBoxOpener::sTGift* CBoxOpener::GetGiftItem(int index)
{
	FrListItem* item = m_pGiftList->GetItem(index);
	if (!item)
		return NULL;
	return (sTGift*)item->pData;
}

void CBoxOpener::DrawGiftList(FrListItem* item)
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	sTGift* gift = (sTGift*)item->pData;
	if (!gi)
		return;
	WPoint offset(m_pGiftList->GetRect().TopLeft());
	const Bitmap* bitmap;
	if (m_curSeq >= eSEQ_COMPLETE && gift->bMine)
		bitmap = m_pBoxOpenBmp;
	else
		bitmap = m_pBoxCloseBmp;
	gi->DrawTexture(bitmap,
		WRect(item->pos.x, item->pos.y - 5.f, bitmap->Width(),
			bitmap->Height()),
		0xffffffff, 0);
	if (m_curSeq >= eSEQ_MYITEM_OPEN && m_giftNum == 0)
	{
		static float x = g_view->GetWidth() * .5f - (m_pMsgBmp->Width() >> 1);
		static float y = g_view->GetHeight() * .5f + 30.f;
		gi->DrawTexture(m_pMsgBmp,
			WRect(x, y, m_pMsgBmp->Width(), m_pMsgBmp->Height()), 0xffffffff,
			0);
	}
	unsigned long color = 0xffffffff;
	bitmap = NULL;
	GetItemOffset(offset, gift->typeId, true);
	switch (gift->state)
	{
	case 0:
		return;
	case 1:
		bitmap = m_pMark1Bmp;
		offset.x = 27.f;
		offset.y = 18.f;
		break;
	case 2:
		bitmap = m_pMark2Bmp;
		offset.x = 27.f;
		offset.y = 28.f;
		break;
	case 3:
		if (gift->bItem == true)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(gift->typeId);
			if (common)
			{
				if (gift->typeId != 0x1a000010)
					bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS",
						common->Icon);
				else
					bitmap =
						g_pFresh->GetManager()->GetBitmap("ITEMS", "Item_Pang");
				color = 0xa0202020;
				offset.x += 13.f;
				offset.y += 7.f;
			}
		}
		break;
	case 4:
		color = gift->bMine == true ? 0xffffffff : 0x20202020;
		if (gift->bItem == true)
		{
			IFF_ITEM_COMMON* common =
				ItemManager()->FindCommonItem(gift->typeId);
			if (common)
			{
				if (gift->typeId != 0x1a000010)
					bitmap = g_pFresh->GetManager()->GetBitmap("ITEMS",
						common->Icon);
				else
					bitmap =
						g_pFresh->GetManager()->GetBitmap("ITEMS", "Item_Pang");
				offset.x += 13.f;
				offset.y += 7.f;
			}
		}
		break;
	}
	if (bitmap)
	{
		gi->DrawTexture(bitmap,
			WRect(item->pos.x + offset.x, item->pos.y + offset.y,
				bitmap->Width(), bitmap->Height()),
			color, 0);
		if (m_pFont && gift->state == 4 && gift->quantity)
		{
			WPoint pos(item->pos);
			GetItemOffset(pos, gift->typeId, false);
			char text[16] = { 0 };
			itoa(gift->quantity, text, 10);
			if (gift->typeId == 0x1a000010)
				pos.x -= 15.f;
			m_pFont->Print(g_view, pos.x + item->pos.x + offset.x,
				pos.y + offset.y + item->pos.y, text, 0, 0xffffffff, NULL);
		}
	}
	if (gift->state >= 3 && gift->frame >= 0.f && (int)gift->frame < 8 &&
		m_pOpenAni)
	{
		m_pOpenAni->Render(MakeStr("%d", (int)gift->frame), item->pos.x - 10.f,
			item->pos.y - 25.f, 1.5f, 0);
		gift->frame += m_dt * 15.f;
	}
}

void CBoxOpener::DrawBoxArea()
{
	FrGraphicInterface* gi = g_pFresh->GetManager()->GetGDI();
	if (!gi)
		return;
	WRect pos(m_pBoxArea->GetRect());
	if (m_pBoxFrameBmp)
		gi->DrawTexture(m_pBoxFrameBmp,
			WRect(pos.x, pos.y, m_pBoxFrameBmp->Width(),
				m_pBoxFrameBmp->Height()),
			0xffffffff, 0);
	if (m_pBoxBmp[m_boxType - 1])
		gi->DrawTexture(m_pBoxBmp[m_boxType - 1],
			WRect(pos.x + 20.f, pos.y + 22.f, m_pBoxBmp[m_boxType - 1]->Width(),
				m_pBoxBmp[m_boxType - 1]->Height()),
			0xffffffff, 0);
	if (m_bApproach == true && CTHunter::Instance()->GetGiftSize() > 0)
	{
		gi->SetTextColor(0xffff0000, 0xffffffff);
		gi->Print(WPoint(pos.x + 230.f, pos.y + 134.f), 0,
			"\xb0\xb0\xc0\xba\xc1\xbe\xb7\xf9\xc0\xc7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xc7\xd1 \xbd\xbd\xb7\xd4\xbf\xa1 \xc7\xd5\xbb\xea\xc7\xa5\xb1\xe2 \xb5\xcb\xb4\xcf\xb4\xd9.");
		gi->SetTextColor(0xffffffff, 0xffffffff);
	}
}

void CBoxOpener::GetItemOffset(WPoint& pos, unsigned long typeId, bool icon)
{
	unsigned long category = typeId >> 26;
	if (icon == true)
	{
		switch (category)
		{
		case 6:
			if (ItemManager()->GetCouponKind(typeId))
				pos = WPoint(0, 0);
			else
				pos = WPoint(0, 0);
			break;
		case 28:
			pos = WPoint(-10, -20);
			break;
		case 31:
			pos = WPoint(0, -10);
			break;
		}
	}
	else
	{
		switch (category)
		{
		case 6:
			if (ItemManager()->GetCouponKind(typeId))
				pos = WPoint(40, 33);
			else
				pos = WPoint(40, 33);
			break;
		case 28:
			pos = WPoint(48, 50);
			break;
		case 31:
			pos = WPoint(45, 43);
			break;
		}
	}
}

unsigned char CBoxOpener::DecideTRBox()
{
	unsigned char counts[] = { 3, 4, 3, 5 };
	unsigned char maps[] = { 2, 3, 11, 0, 1, 6, 7, 10, 13, 15, 4, 5, 8, 9, 14 };
	unsigned char current = Doc()->m_roomInfo.mapType;
	unsigned char map =
		current > 127 && current != 253 ? current - 128 : current;
	unsigned char end = 0;
	unsigned char index = 0;
	for (unsigned char group = 0; group < 4; ++group)
	{
		end += counts[group];
		for (; index < end; ++index)
			if (maps[index] == map)
				return group + 1;
	}
	return 1;
}
