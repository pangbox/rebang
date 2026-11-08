#include "frhalloweendlg.h"
#include "contentsdoc.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrHalloweenEventGiftDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrHalloweenEventGiftDlg, FrForm)

ON_FRESH_VI("giftframe_01", FRCMD_INIT, FrHalloweenEventGiftDlg::OnInitFrame01)
ON_FRESH_VI("giftframe_01_cover", FRCMD_INIT,
	FrHalloweenEventGiftDlg::OnInitFrameCover)

END_FRESH_MSGMAP()

FrHalloweenEventGiftDlg::FrHalloweenEventGiftDlg()
	: m_pFrame01(NULL), m_pFrameCover(NULL)
{
}

FrHalloweenEventGiftDlg::~FrHalloweenEventGiftDlg()
{
}

bool FrHalloweenEventGiftDlg::OnInit()
{
	return FrForm::OnInit();
}

void FrHalloweenEventGiftDlg::OnInitFrame01(int param)
{
	m_pFrame01 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFrame01)
	{
		CHalloweenEvent* event =
			(CHalloweenEvent*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)33);
		if (event)
		{
			if (event->GetGiftCnt() > 1)
			{
				m_pFrame01->Enable(false);
				m_pFrame01->SetVisible(false);
			}
			else
			{
				m_pFrame01->Enable(true);
				m_pFrame01->SetVisible(true);
			}
		}
		else
		{
			m_pFrame01->Enable(true);
			m_pFrame01->SetVisible(true);
		}
	}
}

void FrHalloweenEventGiftDlg::OnInitFrameCover(int param)
{
	m_pFrameCover = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pFrameCover)
	{
		CHalloweenEvent* event =
			(CHalloweenEvent*)CContentsDoc::Instance()->GetContainer(
				(localContentType_t)33);
		if (event)
		{
			if (event->GetGiftCnt() > 1)
			{
				m_pFrameCover->Enable(true);
				m_pFrameCover->SetVisible(true);
			}
			else
			{
				m_pFrameCover->Enable(false);
				m_pFrameCover->SetVisible(false);
			}
		}
		else
		{
			m_pFrameCover->Enable(false);
			m_pFrameCover->SetVisible(false);
		}
	}
}

IMPLEMENT_OBJECT(FrHalloweenEventDescDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrHalloweenEventDescDlg, FrForm)

ON_FRESH_VI("desc_view", FRCMD_INIT, FrHalloweenEventDescDlg::OnInitDescView)

END_FRESH_MSGMAP()

FrHalloweenEventDescDlg::FrHalloweenEventDescDlg()
{
	m_pDescView = NULL;
	m_bScroll = false;
}

FrHalloweenEventDescDlg::~FrHalloweenEventDescDlg()
{
}

bool FrHalloweenEventDescDlg::OnInit()
{
	if (m_pDescView)
	{
		bool scroll = false;
		float offset = m_pDescView->GetScrollBarOffset() * 0.5f;
		int width = m_pDescView->GetSrcWidth();
		width -= (int)m_pDescView->GetViewWidth();
		int height = m_pDescView->GetSrcHeight();
		if (height > 460)
		{
			height = 460;
			scroll = true;
		}
		if (scroll)
			m_bScroll = true;
		else
			m_bScroll = false;
		if (scroll)
			m_pDescView->ShowScrollBar(true);
		else
			m_pDescView->ShowScrollBar(false);
		int viewHeight = (int)m_pDescView->GetViewHeight();
		WRect rect = m_pDescView->GetRect();
		rect.w += width;
		rect.h += height - viewHeight;
		if (scroll)
			rect.w -= offset * 2;
		m_pDescView->SetRect(rect);
	}
	return FrForm::OnInit();
}

void FrHalloweenEventDescDlg::OnInitDescView(int param)
{
	m_pDescView = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
	if (m_pDescView)
	{
		m_pDescView->ShowScrollBar(false);
		m_pDescView->Open("hw_explanation.jpg");
	}
}

IMPLEMENT_OBJECT(FrHalloweenEventDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrHalloweenEventDlg, FrForm)

ON_FRESH_VI("desc_button", FRCMD_INIT, FrHalloweenEventDlg::OnInitDescButton)
ON_FRESH_VV("desc_button", FRCMD_LBUTTONUP,
	FrHalloweenEventDlg::OnLBDownDescButton)
ON_FRESH_VV("map_icon_area", FRCMD_OWNERDRAW,
	FrHalloweenEventDlg::OnMapIconOwnerDraw)
ON_FRESH_VI("current_candy", FRCMD_OWNERDRAW,
	FrHalloweenEventDlg::OnCurrentItemOwnerDraw)

END_FRESH_MSGMAP()

FrHalloweenEventDlg::FrHalloweenEventDlg()
{
	// HACK
	(void)g_EPSILON;
	m_pTicketFrame = NULL;
	m_pDescDlg = NULL;
	m_mapFlag = 0;
	// HACK: ????????????
	memset(m_mapFlagVector, 0, sizeof(m_mapFlagVector) + 3);
	for (int i = 0; i < 4; ++i)
	{
		m_itemCount[i] = 0;
		m_itemCountPos[i] = WPoint(0, 0);
	}
	memset(m_pMapImage, 0, sizeof(m_pMapImage));
}

FrHalloweenEventDlg::~FrHalloweenEventDlg()
{
}

bool FrHalloweenEventDlg::OnInit()
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
	SetMapImageRect(MAP_WHITE_WIZ, WRect(282.0f, 199.0f, 328.0f, 108.0f));
	SetMapImageRect(MAP_SHINING_SAND, WRect(463.0f, 302.0f, 516.0f, 218.0f));
	SetMapImageRect(MAP_WIZ_WIZ, WRect(214.0f, 199.0f, 245.0f, 208.0f));
	SetMapImageRect(MAP_DEEP_INFERNO, WRect(332.0f, 260.0f, 393.0f, 178.0f));
	SetMapImageRect(MAP_BLUE_LAGOON, WRect(296.0f, 399.0f, 336.0f, 420.0f));
	SetMapImageRect(MAP_SILVIA_CANNON, WRect(236.0f, 467.0f, 293.0f, 382.0f));
	SetMapImageRect(MAP_BLUE_WATER, WRect(207.0f, 430.0f, 242.0f, 334.0f));
	SetMapImageRect(MAP_BLUE_MOON, WRect(79.0f, 377.0f, 178.0f, 323.0f));
	SetMapImageRect(MAP_PINK_WIND, WRect(209.0f, 336.0f, 257.0f, 259.0f));
	SetMapImageRect(MAP_WIND_HILL, WRect(196.0f, 271.0f, 240.0f, 190.0f));
	SetMapImageRect(MAP_SEPIA_WIND, WRect(120.0f, 309.0f, 177.0f, 236.0f));
	SetMapImageRect(MAP_ICE_CANNON, WRect(45.0f, 258.0f, 103.0f, 189.0f));
	SetMapImageRect(MAP_WEST_WIZ, WRect(141.0f, 225.0f, 184.0f, 150.0f));
	SetMapImageRect(MAP_ICE_SPA, WRect(65.0f, 199.0f, 120.0f, 208.0f));
	m_mapMarkRect[9] = WRect(286.0f, 198.0f, 322.0f, 212.0f);
	m_mapMarkRect[10] = WRect(467.0f, 307.0f, 490.0f, 321.0f);
	m_mapMarkRect[4] = WRect(209.0f, 198.0f, 245.0f, 207.0f);
	m_mapMarkRect[13] = WRect(334.0f, 265.0f, 370.0f, 279.0f);
	m_mapMarkRect[0] = WRect(292.0f, 399.0f, 328.0f, 416.0f);
	m_mapMarkRect[7] = WRect(249.0f, 472.0f, 285.0f, 486.0f);
	m_mapMarkRect[1] = WRect(197.0f, 422.0f, 233.0f, 436.0f);
	m_mapMarkRect[6] = WRect(104.0f, 386.0f, 115.0f, 387.0f);
	m_mapMarkRect[11] = WRect(209.0f, 348.0f, 245.0f, 362.0f);
	m_mapMarkRect[3] = WRect(205.0f, 275.0f, 241.0f, 289.0f);
	m_mapMarkRect[2] = WRect(128.0f, 320.0f, 164.0f, 334.0f);
	m_mapMarkRect[8] = WRect(54.0f, 258.0f, 90.0f, 272.0f);
	m_mapMarkRect[5] = WRect(142.0f, 234.0f, 178.0f, 248.0f);
	m_mapMarkRect[14] = WRect(74.0f, 198.0f, 110.0f, 207.0f);
	m_pMark = g_pFresh->GetBitmap("hw_mark");
	m_pTicketFrame = g_pFresh->GetBitmap("hw_ticket_frame");
	m_itemCountPos[0] = WPoint(115.0f, 60.0f);
	m_itemCountPos[1] = WPoint(115.0f, 77.0f);
	m_itemCountPos[2] = WPoint(115.0f, 94.0f);
	m_itemCountPos[3] = WPoint(115.0f, 111.0f);
	m_reservedPos = WPoint(659.0f, 496.0f);
	int itemIds[4] = { 73, 75, 74, 76 };
	for (int i = 0; i < 4; ++i)
	{
		std::list<sItemInfo>::iterator it =
			std::find(Doc()->m_myItemList.begin(), Doc()->m_myItemList.end(),
				(unsigned long)(itemIds[i] | 0x1a000000));
		if (it != Doc()->m_myItemList.end())
			m_itemCount[i] = it->Common[0];
		else
			m_itemCount[i] = 0;
	}
	return true;
}

void FrHalloweenEventDlg::OnInitDescButton(int param)
{
	m_pDescButton = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pDescButton->SetPushDelay(0);
	WPoint origin(m_rect.x, m_rect.y);
	WRect rect = m_pDescButton->GetRect();
	rect.x = origin.x + 352;
	rect.y = origin.y + 151;
	m_pDescButton->SetRect(rect);
}

void FrHalloweenEventDlg::OnLBDownDescButton()
{
	if (!m_pDescDlg)
	{
		m_pDescDlg = CreateForm<FrHalloweenEventDescDlg>(g_pFresh->GetManager(),
			this, "halloween2007_event_desc_dlg", NULL);
		if (m_pDescDlg)
		{
			WRect rect = m_pDescButton->GetRect();
			m_pDescDlg->SetIconRect(rect);
			m_pDescDlg->Open(
				(FRESH_PFN_RESULT)&FrHalloweenEventDlg::OnDescDlgResult, 17);
		}
	}
}

bool FrHalloweenEventDlg::OnDescDlgResult(int result, FrForm* form)
{
	m_pDescDlg = NULL;
	return true;
}

void FrHalloweenEventDlg::SetMapImage(eMapType type, const char* on,
	const char* off)
{
	int index = CMapTypeAdapter::GetMapIndex(type);
	m_pMapImage[index][0] = g_pFresh->GetBitmap(on);
	m_pMapImage[index][1] = g_pFresh->GetBitmap(off);
}

void FrHalloweenEventDlg::SetMapImageRect(eMapType type, WRect& rect)
{
	m_mapImageRect[CMapTypeAdapter::GetMapIndex(type)] = rect;
}

void FrHalloweenEventDlg::SetMapFlagVector(eMapType type)
{
	unsigned long flag = CMapTypeAdapter::GetMapFlag(type);
	m_mapFlagVector[CMapTypeAdapter::GetMapIndex(type)] = flag;
}

void FrHalloweenEventDlg::DrawMapImage()
{
	// HACK
	if (0)
		SetMapImageRect(MAP_BLUE_LAGOON, m_rect);
}

void FrHalloweenEventDlg::OnMapIconOwnerDraw()
{
	DrawMapImage();
}

void FrHalloweenEventDlg::OnCurrentItemOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	int space = gdi->SetSpace(0);
	WRect rect = m_rect;
	WPoint pos;
	float width = m_pTicketFrame->Width();
	float height = m_pTicketFrame->Height();
	WRect dest(rect.x - width + 40, (rect.h - height) * 0.5f + rect.y, width,
		height);
	gdi->DrawTexture(m_pTicketFrame, dest, 0xffffffff, 0);
	gdi->SetTextColor(0xffffff00, 0xffffffff);
	for (int i = 0; i < 4; ++i)
	{
		pos = m_itemCountPos[i];
		pos.x += dest.x;
		pos.y += dest.y;
		gdi->PrintText(pos, 2, MakeStr("%d", m_itemCount[i]), 0);
	}
	gdi->SetTextColor(0xff000000, 0xffffffff);
	gdi->SetSpace(space);
}
