#include "worldtoureventdlg.h"
#include "contentsdoc.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrWorldTourEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrWorldTourEventDlg, FrForm)

ON_FRESH_VV("map_icon_area", FRCMD_OWNERDRAW,
	FrWorldTourEventDlg::OnMapIconOwnerDraw)
ON_FRESH_VI("show_desc_left_top", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitGiftImageLeftTop)
ON_FRESH_VI("show_desc_right_top", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitGiftImageRightTop)
ON_FRESH_VI("show_desc_left_bottom", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitGiftImageLeftBottom)
ON_FRESH_VI("show_desc_right_bottom", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitGiftImageRightBottom)
ON_FRESH_VI("desc_button", FRCMD_INIT, FrWorldTourEventDlg::OnInitDescButton)
ON_FRESH_VV("desc_button", FRCMD_LBUTTONUP,
	FrWorldTourEventDlg::OnLBDownDescButton)
ON_FRESH_VI("gift_button_cover", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitGiftButtonCover)
ON_FRESH_VI("gift_button", FRCMD_INIT, FrWorldTourEventDlg::OnInitGiftButton)
ON_FRESH_VV("gift_button", FRCMD_LBUTTONUP,
	FrWorldTourEventDlg::OnLBDownGiftButton)
ON_FRESH_VI("course_button", FRCMD_INIT,
	FrWorldTourEventDlg::OnInitCourseButton)
ON_FRESH_VV("course_button", FRCMD_LBUTTONUP,
	FrWorldTourEventDlg::OnLBDownCourseButton)

END_FRESH_MSGMAP()

FrWorldTourEventDlg::FrWorldTourEventDlg()
{
	ClearVariables();
}

FrWorldTourEventDlg::~FrWorldTourEventDlg()
{
	// HACK
	(void)&g_EPSILON;
}

void FrWorldTourEventDlg::ClearVariables()
{
	m_fElapsed = 0.0f;
	m_reserved = 0;
	m_mapFlag = 0;
	memset(m_mapFlagList, 0, sizeof(m_mapFlagList));
	memset(m_mapRect, 0, sizeof(m_mapRect));
	memset(m_mapImage, 0, sizeof(m_mapImage));
	m_pMarkImage = NULL;
	m_pDescButton = NULL;
	m_pCourseButton = NULL;
	m_pGiftButton = NULL;
	m_pGiftImage[0] = NULL;
	m_pGiftImage[1] = NULL;
	m_pGiftImage[2] = NULL;
	m_pGiftImage[3] = NULL;
	m_pGiftDlg = NULL;
	m_pDescDlg = NULL;
}

void FrWorldTourEventDlg::SetMapFlag(unsigned long flag)
{
	m_mapFlag = flag;
}

void FrWorldTourEventDlg::SetMapFlagVector(eMapType type)
{
	unsigned long flag = CMapTypeAdapter::GetMapFlag(type);
	m_mapFlagList[CMapTypeAdapter::GetMapIndex(type)] = flag;
}

void FrWorldTourEventDlg::SetMapImageRect(eMapType type, WRect& rect)
{
	m_mapRect[CMapTypeAdapter::GetMapIndex(type)] = rect;
}

void FrWorldTourEventDlg::SetMapImage(eMapType type, const char* on,
	const char* off)
{
	int index = CMapTypeAdapter::GetMapIndex(type);
	m_mapImage[index][0] = g_pFresh->GetBitmap(on);
	m_mapImage[index][1] = g_pFresh->GetBitmap(off);
}

bool FrWorldTourEventDlg::OnInit()
{
	SetMapFlagVector(MAP_BLUE_LAGOON);
	SetMapFlagVector(MAP_BLUE_WATER);
	SetMapFlagVector(MAP_SEPIA_WIND);
	SetMapFlagVector(MAP_WIND_HILL);
	SetMapFlagVector(MAP_WIZ_WIZ);
	SetMapFlagVector(MAP_WEST_WIZ);
	SetMapFlagVector(MAP_BLUE_MOON);
	SetMapFlagVector(MAP_SILVIA_CANNON);
	SetMapFlagVector(MAP_ICE_CANNON);
	SetMapFlagVector(MAP_WHITE_WIZ);
	SetMapFlagVector(MAP_SHINING_SAND);
	SetMapFlagVector(MAP_PINK_WIND);
	SetMapFlagVector(MAP_DEEP_INFERNO);
	SetMapFlagVector(MAP_ICE_SPA);
	SetMapFlagVector(MAP_LOST_SEAWAY);
	SetMapFlagVector(MAP_EASTERN_VALLEY);
	SetMapFlagVector(MAP_WIZ_CITY);
	SetMapImageRect(MAP_BLUE_LAGOON, WRect(296.0f, 410.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_BLUE_WATER, WRect(218.0f, 424.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_SEPIA_WIND, WRect(139.0f, 310.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_WIND_HILL, WRect(214.0f, 272.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_WIZ_WIZ, WRect(229.0f, 216.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_WEST_WIZ, WRect(173.0f, 222.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_BLUE_MOON, WRect(116.0f, 385.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_SILVIA_CANNON, WRect(223.0f, 470.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_ICE_CANNON, WRect(51.0f, 257.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_WHITE_WIZ, WRect(275.0f, 210.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_SHINING_SAND, WRect(490.0f, 293.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_PINK_WIND, WRect(206.0f, 353.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_DEEP_INFERNO, WRect(354.0f, 263.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_ICE_SPA, WRect(94.0f, 205.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_LOST_SEAWAY, WRect(440.0f, 375.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_EASTERN_VALLEY, WRect(359.0f, 349.0f, 0.0f, 0.0f));
	SetMapImageRect(MAP_WIZ_CITY, WRect(0.0f, 0.0f, 0.0f, 0.0f));
	SetMapImage(MAP_BLUE_LAGOON, "map_bluelagoon_on", "map_bluelagoon_off");
	SetMapImage(MAP_BLUE_WATER, "map_bluewater_on", "map_bluewater_off");
	SetMapImage(MAP_SEPIA_WIND, "map_sepiawind_on", "map_sepiawind_off");
	SetMapImage(MAP_WIND_HILL, "map_windhill_on", "map_windhill_off");
	SetMapImage(MAP_WIZ_WIZ, "map_wizwiz_on", "map_wizwiz_off");
	SetMapImage(MAP_WEST_WIZ, "map_westwiz_on", "map_westwiz_off");
	SetMapImage(MAP_BLUE_MOON, "map_bluemoon_on", "map_bluemoon_off");
	SetMapImage(MAP_SILVIA_CANNON, "map_silviacannon_on",
		"map_silviacannon_off");
	SetMapImage(MAP_ICE_CANNON, "map_icecannon_on", "map_icecannon_off");
	SetMapImage(MAP_WHITE_WIZ, "map_whitewiz_on", "map_whitewiz_off");
	SetMapImage(MAP_SHINING_SAND, "map_shining_on", "map_shining_off");
	SetMapImage(MAP_PINK_WIND, "map_pinkwind_on", "map_pinkwind_off");
	SetMapImage(MAP_DEEP_INFERNO, "map_deepinferno_on", "map_deepinferno_off");
	SetMapImage(MAP_ICE_SPA, "map_icespa_on", "map_icespa_off");
	SetMapImage(MAP_LOST_SEAWAY, "map_lostseaway_on", "map_lostseaway_off");
	SetMapImage(MAP_EASTERN_VALLEY, "map_esternvalley_on",
		"map_esternvalley_off");
	SetMapImage(MAP_WIZ_CITY, "map_lostseaway_on", "map_lostseaway_off");
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_BLUE_LAGOON)] =
		WRect(293.0f, 418.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_BLUE_WATER)] =
		WRect(200.0f, 427.0f, 0.0f, 0.0f);
	eMapType sepia = MAP_SEPIA_WIND;
	m_markRect[CMapTypeAdapter::GetMapIndex(sepia)] =
		WRect(159.0f, 316.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_WIND_HILL)] =
		WRect(217.0f, 283.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_WIZ_WIZ)] =
		WRect(230.0f, 229.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_WEST_WIZ)] =
		WRect(175.0f, 241.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_BLUE_MOON)] =
		WRect(120.0f, 393.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_SILVIA_CANNON)] =
		WRect(242.0f, 488.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_ICE_CANNON)] =
		WRect(65.0f, 266.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_WHITE_WIZ)] =
		WRect(281.0f, 215.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_SHINING_SAND)] =
		WRect(502.0f, 319.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_PINK_WIND)] =
		WRect(212.0f, 366.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_DEEP_INFERNO)] =
		WRect(368.0f, 288.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_ICE_SPA)] =
		WRect(105.0f, 221.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_LOST_SEAWAY)] =
		WRect(450.0f, 385.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_EASTERN_VALLEY)] =
		WRect(359.0f, 349.0f, 0.0f, 0.0f);
	m_markRect[CMapTypeAdapter::GetMapIndex(MAP_WIZ_CITY)] =
		WRect(0.0f, 385.0f, 0.0f, 0.0f);
	m_pMarkImage = g_pFresh->GetBitmap("wte_mark");
	return true;
}

void FrWorldTourEventDlg::OnProc(const float dt)
{
	m_fElapsed += dt;
}

void FrWorldTourEventDlg::DrawMapImage()
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (gdi)
	{
		WRect origin = m_rect;
		for (int i = 0; i < 17; ++i)
		{
			const Bitmap* bitmap;
			if (m_mapFlagList[i] & m_mapFlag)
				bitmap = m_mapImage[i][0];
			else
				bitmap = m_mapImage[i][1];
			if (bitmap)
				gdi->DrawTexture(bitmap,
					WRect(origin.x + m_mapRect[i].x, origin.y + m_mapRect[i].y,
						(float)bitmap->Width(), (float)bitmap->Height()),
					0xffffffff, 0);
			if (m_mapFlagList[i] & m_mapFlag)
			{
				if (m_pMarkImage)
					gdi->DrawTexture(m_pMarkImage,
						WRect(origin.x + m_markRect[i].x,
							origin.y + m_markRect[i].y,
							(float)m_pMarkImage->Width(),
							(float)m_pMarkImage->Height()),
						0xffffffff, 0);
			}
		}
	}
}

void FrWorldTourEventDlg::OnMapIconOwnerDraw()
{
	DrawMapImage();
	if (m_mapFlag == 0xffff)
	{
		const Bitmap* bitmap = g_pFresh->GetBitmap("stamp_good_big");
		if (bitmap)
		{
			WRect origin = m_rect;
			WRect rect(origin.x + 217.0f, origin.y + 122.0f,
				(float)bitmap->Width(), (float)bitmap->Height());
			g_pFresh->GetManager()->GetGDI()->DrawTexture(bitmap, rect,
				0xffffffff, 0);
		}
	}
}

void FrWorldTourEventDlg::OnInitGiftImageLeftTop(int param)
{
	m_pGiftImage[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pGiftImage[0]->SetVisible(false);
}

void FrWorldTourEventDlg::OnInitGiftImageRightTop(int param)
{
	m_pGiftImage[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pGiftImage[1]->SetVisible(false);
}

void FrWorldTourEventDlg::OnInitGiftImageLeftBottom(int param)
{
	m_pGiftImage[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pGiftImage[2]->SetVisible(false);
}

void FrWorldTourEventDlg::OnInitGiftImageRightBottom(int param)
{
	m_pGiftImage[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pGiftImage[3]->SetVisible(false);
}

void FrWorldTourEventDlg::OnInitDescButton(int param)
{
	m_pDescButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pDescButton->SetPushDelay(0.0f);
}

void FrWorldTourEventDlg::OnLBDownDescButton()
{
	if (!m_pDescDlg)
	{
		m_pDescDlg = CreateForm<FrWorldTourEventDescDlg>(g_pFresh->GetManager(),
			this, "world_tour_event_desc_dlg", NULL);
		if (m_pDescDlg)
		{
			WRect rect = m_pDescButton->GetRect();
			m_pDescDlg->SetIconRect(rect);
			m_pDescDlg->Open(
				(FRESH_PFN_RESULT)&FrWorldTourEventDlg::OnDescDlgResult, 17);
		}
	}
}

void FrWorldTourEventDlg::OnInitCourseButton(int param)
{
	m_pCourseButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrWorldTourEventDlg::OnLBDownCourseButton()
{
	if (!m_pDescDlg)
	{
		if (m_pGiftImage[0])
			m_pGiftImage[0]->SetVisible(false);
		if (m_pGiftImage[1])
			m_pGiftImage[1]->SetVisible(false);
		if (m_pGiftImage[2])
			m_pGiftImage[2]->SetVisible(false);
		if (m_pGiftImage[3])
			m_pGiftImage[3]->SetVisible(false);
	}
}

void FrWorldTourEventDlg::OnInitGiftButtonCover(int param)
{
	m_pGiftButtonCover = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pGiftButtonCover)
		m_pGiftButtonCover->SetVisible(false);
}

void FrWorldTourEventDlg::OnInitGiftButton(int param)
{
	m_pGiftButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pGiftButton->SetPushDelay(0.0f);
	CWorldTourEvent* event =
		(CWorldTourEvent*)CContentsDoc::Instance()->GetContainer(
			(localContentType_t)30);
	if (event && !event->GetCanGift())
	{
		m_pGiftButton->Enable(false);
		m_pGiftButton->SetVisible(false);
	}
	if (m_pGiftButton && m_pGiftButtonCover)
	{
		if (!m_pGiftButton->IsEnabled() || !m_pGiftButton->IsVisible())
		{
			m_pGiftButtonCover->Enable(false);
			m_pGiftButtonCover->SetVisible(false);
		}
	}
}

void FrWorldTourEventDlg::OnLBDownGiftButton()
{
	if (!m_pGiftDlg)
	{
		m_pGiftDlg = CreateForm<FrWorldTourEventGiftDlg>(g_pFresh->GetManager(),
			this, "world_tour_event_gift_dlg", NULL);
		if (m_pGiftDlg)
		{
			m_pGiftButtonCover->Enable(true);
			m_pGiftButtonCover->SetVisible(true);
			WRect rect = m_pGiftButton->GetRect();
			m_pGiftDlg->SetIconRect(rect);
			m_pGiftDlg->Open(
				(FRESH_PFN_RESULT)&FrWorldTourEventDlg::OnGiftDlgResult, 17);
		}
	}
}

bool FrWorldTourEventDlg::OnDescDlgResult(int result, FrForm* form)
{
	m_pDescDlg = NULL;
	return true;
}

bool FrWorldTourEventDlg::OnGiftDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		CWorldTourEvent* event =
			(CWorldTourEvent*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)30);
		if (event)
		{
			event->SetCanGift(false);
			event->SetGotGift(true);
		}
		m_pGiftButton->Enable(false);
		m_pGiftButton->SetVisible(false);
		m_pGiftButtonCover->Enable(false);
		m_pGiftButtonCover->SetVisible(false);
		FrForm* notify =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		WRect origin = notify->GetRect();
		notify->SetRect(WRect(origin.x, origin.y, origin.w, origin.h));
		notify->SetTimeLimit(12.0f);
		notify->SetMessage(
			"-\xc6\xce\xbe\xdf\xbc\xb6 \xc5\xf5\xbe\xee\xb8\xa6 \xb8\xb6\xc4\xa1\xb0\xed \xbc\xf6\xbf\xb5\xba\xb9\xc0\xbb \xc1\xf6\xb1\xde\xb9\xde\xbe\xd2\xbd\xc0\xb4\xcf\xb4\xd9.\012\012 -\\c0xffff0000\\c\xb0\xe6\xc7\xb0 \xc0\xcc\xba\xa5\xc6\xae\\c0xff000000\\c\xbf\xa1 \xc0\xda\xb5\xbf\xc0\xb8\xb7\xce \xc0\xc0\xb8\xf0\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9",
			false);
		notify->Open(NULL, 0);
	}
	m_pGiftDlg = NULL;
	return true;
}

IMPLEMENT_OBJECT(FrWorldTourEventDescDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrWorldTourEventDescDlg, FrForm)

ON_FRESH_VI("desc_view", FRCMD_INIT, FrWorldTourEventDescDlg::OnInitDescView)

END_FRESH_MSGMAP()

FrWorldTourEventDescDlg::FrWorldTourEventDescDlg()
	: m_pDescView(NULL)
{
}

FrWorldTourEventDescDlg::~FrWorldTourEventDescDlg()
{
}

void FrWorldTourEventDescDlg::OnInitDescView(int param)
{
	m_pDescView = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pDescView)
	{
		m_pDescView->ShowScrollBar(false);
		m_pDescView->Open("wte_explanation.jpg");
	}
}

IMPLEMENT_OBJECT(FrWorldTourEventGiftDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrWorldTourEventGiftDlg, FrForm)

ON_FRESH_VI("yes_button", FRCMD_INIT, FrWorldTourEventGiftDlg::OnInitYesButton)
ON_FRESH_VV("yes_button", FRCMD_LBUTTONUP,
	FrWorldTourEventGiftDlg::OnLBDownYesButton)
ON_FRESH_VI("no_button", FRCMD_INIT, FrWorldTourEventGiftDlg::OnInitNoButton)
ON_FRESH_VV("no_button", FRCMD_LBUTTONUP,
	FrWorldTourEventGiftDlg::OnLBDownNoButton)

END_FRESH_MSGMAP()

FrWorldTourEventGiftDlg::FrWorldTourEventGiftDlg()
	: m_pYesButton(NULL), m_pNoButton(NULL)
{
}

FrWorldTourEventGiftDlg::~FrWorldTourEventGiftDlg()
{
}

void FrWorldTourEventGiftDlg::OnInitYesButton(int param)
{
	m_pYesButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrWorldTourEventGiftDlg::OnLBDownYesButton()
{
	WSendPacket packet((enumClientPacket)165);
	packet.Encode4(MyGuid(false));
	packet.Send(TO_GAME);
	Close((eFormRet)1, true);
}

void FrWorldTourEventGiftDlg::OnInitNoButton(int param)
{
	m_pNoButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrWorldTourEventGiftDlg::OnLBDownNoButton()
{
	Close((eFormRet)2, true);
}
