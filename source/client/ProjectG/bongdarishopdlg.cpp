#include "exhibition.h"
#include "bongdarishopdlg.h"
#include "frtextbutton.h"
#include "setitemdlg.h"
#include "binstr.h"
#include "simpleoverlay.h"
#include "taskmanager.h"
#include "inputmanager.h"
#include "frwnd.h"
#include "frscrollbar.h"
#include "frdesktop.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

static char* s_bongdariMotions[] = {
	"\xb1\xe2\xba\xbb",
	"\xbb\xcc\xb1\xe2\x31",
	"\xbb\xcc\xb1\xe2\x32",
	"\xbb\xcc\xb1\xe2\x33",
	"\xbb\xcc\xb1\xe2\x34",
	"\xbb\xcc\xb1\xe2\x35",
	"\xbb\xf5\xb7\xce\xbd\xc3\xc0\xdb",
};

IMPLEMENT_OBJECT(FrBongdariShopDlg, FrForm)
BEGIN_FRESH_MSGMAP(FrBongdariShopDlg, FrForm)
ON_FRESH_VI("bongdari_shop", FRCMD_INIT, FrBongdariShopDlg::OnBongdariShopInit)
ON_FRESH_VI("aboutshop", FRCMD_INIT, FrBongdariShopDlg::OnAboutShopBnInit)
ON_FRESH_VV("aboutshop", FRCMD_LBUTTONUP, FrBongdariShopDlg::OnAboutShopBnUp)
ON_FRESH_VI("totalitem", FRCMD_INIT, FrBongdariShopDlg::OnTotalItemBnInit)
ON_FRESH_VV("totalitem", FRCMD_LBUTTONUP, FrBongdariShopDlg::OnTotalItemBnUp)
ON_FRESH_VI("bongdarishopnotice", FRCMD_INIT,
	FrBongdariShopDlg::OnBongdarishopNoticeBnInit)
ON_FRESH_VV("bongdarishopnotice", FRCMD_LBUTTONUP,
	FrBongdariShopDlg::OnBongdarishopNoticeBnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrBongdariShopDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrBongdariShopDlg::OnCloseBnUp)
ON_FRESH_VI("bongdari", FRCMD_INIT, FrBongdariShopDlg::OnBongdariInit)
ON_FRESH_VV("bongdari", FRCMD_OWNERDRAW, FrBongdariShopDlg::OnBongdariOwnerDraw)
ON_FRESH_VI("bonusball01", FRCMD_INIT, FrBongdariShopDlg::OnBonusBall01BnInit)
ON_FRESH_VV("bonusball01", FRCMD_OWNERDRAW,
	FrBongdariShopDlg::OnBonusBall01OwnerDraw)
ON_FRESH_VI("bonusball02", FRCMD_INIT, FrBongdariShopDlg::OnBonusBall02BnInit)
ON_FRESH_VV("bonusball02", FRCMD_OWNERDRAW,
	FrBongdariShopDlg::OnBonusBall02OwnerDraw)
ON_FRESH_VI("bonusball03", FRCMD_INIT, FrBongdariShopDlg::OnBonusBall03BnInit)
ON_FRESH_VV("bonusball03", FRCMD_OWNERDRAW,
	FrBongdariShopDlg::OnBonusBall03OwnerDraw)
ON_FRESH_VI("bonusball04", FRCMD_INIT, FrBongdariShopDlg::OnBonusBall04BnInit)
ON_FRESH_VV("bonusball04", FRCMD_OWNERDRAW,
	FrBongdariShopDlg::OnBonusBall04OwnerDraw)
ON_FRESH_VI("bonusball05", FRCMD_INIT, FrBongdariShopDlg::OnBonusBall05BnInit)
ON_FRESH_VV("bonusball05", FRCMD_OWNERDRAW,
	FrBongdariShopDlg::OnBonusBall05OwnerDraw)
END_FRESH_MSGMAP()

unsigned long g_arrdwPercentNumCookieItem0[5][2] = {
	{ 70, 0 },
	{ 15, 1 },
	{ 8,  2 },
	{ 5,  3 },
	{ 2,  4 },
};

unsigned long g_arrdwPercentNumCookieItem1[4][2] = {
	{ 60, 1 },
	{ 25, 2 },
	{ 10, 3 },
	{ 5,  4 },
};

unsigned long g_arrdwPercentNumCookieItem2[3][2] = {
	{ 70, 2 },
	{ 20, 3 },
	{ 10, 4 },
};

unsigned long g_arrdwPercentSelectCookieItem[19][2] = {
	{ 7, 0x1800000e },
	{ 7, 0x1800000b },
	{ 7, 0x1800000a },
	{ 7, 0x18000009 },
	{ 6, 0x18000006 },
	{ 7, 0x1a00004f },
	{ 5, 0x1a000002 },
	{ 5, 0x1a000011 },
	{ 5, 0x14000005 },
	{ 5, 0x14000003 },
	{ 5, 0x14000002 },
	{ 5, 0x14000001 },
	{ 4, 0x14000020 },
	{ 5, 0x18000010 },
	{ 5, 0x18000011 },
	{ 5, 0x18000012 },
	{ 4, 0x1a000040 },
	{ 3, 0x18000028 },
	{ 3, 0x18000027 },
};

unsigned long g_arrdwPercentSelectPangItem[9][2] = {
	{ 11, 0x18000008 },
	{ 11, 0x18000007 },
	{ 11, 0x18000001 },
	{ 13, 0x18000000 },
	{ 12, 0x18000004 },
	{ 11, 0x18000005 },
	{ 12, 0x1a000028 },
	{ 9,  0x1a00003d },
	{ 10, 0x1a000041 },
};

unsigned long g_arrdwRandSelectPangItem[100];
unsigned long g_arrdwRandSelectCookieItem[100];
unsigned long g_arrdwRandNumCookieItem2[100];
unsigned long g_arrdwRandNumCookieItem1[100];
unsigned long g_arrdwRandNumCookieItem0[100];

WPoint g_arrPtItemSilhouette[9] = {
	WPoint(28.f, 357.f),
	WPoint(93.f, 357.f),
	WPoint(158.f, 357.f),
	WPoint(223.f, 357.f),
	WPoint(288.f, 357.f),
	WPoint(353.f, 357.f),
	WPoint(418.f, 357.f),
	WPoint(483.f, 357.f),
	WPoint(548.f, 357.f),
};

FrBongdariShopDlg::FrBongdariShopDlg()
{
	SendStateBongdariShop(true);
	m_state = 0;
	for (int i = 0; i < 5; ++i)
		m_pPetBonusBall[i] = NULL;
	m_pImgItemBase = g_pFresh->GetManager()->GetBitmap("ITEMS", "item0_13");
	InitBongdariShop();
	InitRandomDataItemSilhouette();
	ShuffleRandomDataItemSilhouette();
	m_pCloseBn = NULL;
	m_pPetBongdari = NULL;
	m_pBongdari = NULL;
	for (int i = 0; i < 5; ++i)
		m_pBonusBallBn[i] = NULL;
	m_pImgShadow = g_pFresh->GetManager()->GetBitmap("BONGDARI_SHOP", "shadow");
	m_pImgTextBongdariClick = g_pFresh->GetManager()->GetBitmap("BONGDARI_SHOP",
		"text_bongdari_click");
	m_pImgTextBonusBallClick = g_pFresh->GetManager()->GetBitmap(
		"BONGDARI_SHOP", "text_bonusball_click");
	m_pImgTextFee[0] =
		g_pFresh->GetManager()->GetBitmap("BONGDARI_SHOP", "text_fee_pang");
	m_pImgTextFee[1] =
		g_pFresh->GetManager()->GetBitmap("BONGDARI_SHOP", "text_fee_coupon");
	m_clickTextPos = 0;
	m_bClickTextUp = false;
	m_bBongdariDown = false;
	m_bUsingBonusBall = false;
	m_rtBongdari = WRect(4.f, 50.f, 607.f, 298.f);
	m_bAnimating = false;
	m_overBonusBall = -1;
	m_cursor = 0;
	static char* filesCount[] = { "[font_wind.jpg" };
	WTITLEFONT fontCount;
	memset(&fontCount, 0, sizeof(fontCount));
	fontCount.filename = filesCount;
	fontCount.fontw = 16;
	fontCount.fonth = 16;
	fontCount.numPages = 1;
	fontCount.texw = 128;
	fontCount.texh = 64;
	fontCount.pCharSet = "1234567890ym%-.?/:,";
	m_pFontCount = g_resrcmng->GetTitleFont();
	m_pFontCount->Create(&fontCount);
	static char* filesPang[] = { "[font_bong.jpg" };
	WTITLEFONT fontPang;
	memset(&fontPang, 0, sizeof(fontPang));
	fontPang.filename = filesPang;
	fontPang.fontw = 9;
	fontPang.fonth = 16;
	fontPang.numPages = 1;
	fontPang.texw = 128;
	fontPang.texh = 32;
	fontPang.pCharSet = "1234567890,-";
	m_pFontPang = g_resrcmng->GetTitleFont();
	m_pFontPang->Create(&fontPang);
	static char* filesCoupon[] = { "[font_bong_red.jpg" };
	WTITLEFONT fontCoupon;
	memset(&fontCoupon, 0, sizeof(fontCoupon));
	fontCoupon.filename = filesCoupon;
	fontCoupon.fontw = 9;
	fontCoupon.fonth = 16;
	fontCoupon.numPages = 1;
	fontCoupon.texw = 128;
	fontCoupon.texh = 32;
	fontCoupon.pCharSet = "1234567890,-";
	m_pFontCoupon = g_resrcmng->GetTitleFont();
	m_pFontCoupon->Create(&fontCoupon);
	m_pNumberUI = new CSimpleUI("[emoti_object.png");
	m_pNumberUI->Add("0", WRect(0.f, 128.f, 64.f, 64.f), false);
	m_pNumberUI->Add("1", WRect(64.f, 128.f, 64.f, 64.f), false);
	m_pNumberUI->Add("2", WRect(128.f, 128.f, 64.f, 64.f), false);
	m_pNumberUI->Add("3", WRect(192.f, 128.f, 64.f, 64.f), false);
	m_pNumberUI->Add("4", WRect(0.f, 192.f, 64.f, 64.f), false);
	m_pNumberUI->Add("5", WRect(64.f, 192.f, 64.f, 64.f), false);
	m_pNumberUI->Add("6", WRect(128.f, 192.f, 64.f, 64.f), false);
	m_pNumberUI->Add("7", WRect(192.f, 192.f, 64.f, 64.f), false);
	m_bOpenSubDlg = false;
	m_couponCount = 0;
	m_usableTimes = -1;
	m_usableBonusTimes = -1;
	m_remainedBonusTimes = 0;

	m_feeAniAlpha = 512;
	m_dataFee = -1;
	m_feeAniPos = 0;
	m_bShowUsableTimes = false;
	m_bShowBonusTimes = false;
	m_timesAniPos = 0;
	m_timesAniAlpha = 512;
	m_processTime = 0;
	m_pTotalItemBn = NULL;
	m_exitTime = 8;
	m_usableTimes = Doc()->GetBsUsableTimes();
	m_usableBonusTimes = Doc()->GetBsUsableBonusTimes();
	m_remainedBonusTimes = Doc()->GetBsRemainedBonusTimes();
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(2);
		packet.Send(TO_GAME);
	}
}

FrBongdariShopDlg::~FrBongdariShopDlg()
{
	Clear();
	SendReBuildMyInventory();
	SendDataClosedBonusBall();
	OpenClosedBonusBallResultDlg();
	if (IsLocalContent(S4_MATCHING_SYSTEM) && AfxGetTask())
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
}

void FrBongdariShopDlg::OnBongdariShopInit(int param)
{
}

void FrBongdariShopDlg::OnAboutShopBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdariShopDlg::OnAboutShopBnUp()
{
	OpenAboutShopDlg();
}

void FrBongdariShopDlg::OnTotalItemBnInit(int param)
{
	m_pTotalItemBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTotalItemBn)
		m_pTotalItemBn->EnableBlink(false);
}

void FrBongdariShopDlg::OnTotalItemBnUp()
{
	OpenTotalItemDlg();
}

void FrBongdariShopDlg::OnBongdarishopNoticeBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdariShopDlg::OnBongdarishopNoticeBnUp()
{
	OpenBongdarishopNoticeDlg(false);
}

void FrBongdariShopDlg::OnCloseBnInit(int param)
{
	m_pCloseBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdariShopDlg::OnCloseBnUp()
{
	OnFreshOkay();
}

void FrBongdariShopDlg::OnBongdariInit(int param)
{
	m_pBongdari = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBongdari)
		m_pBongdari->SetMouseEvent(false);
}

void FrBongdariShopDlg::OnBongdariOwnerDraw()
{
	DrawShadow();
	DisplayItemSilhouette();
	DrawUserInfo();
	switch (m_state)
	{
	case 0:
		DrawTextBongdariClick();
		break;
	case 4:
		DrawTextBonusBallClick();
		break;
	}
	g_view->Flush(0);
	if (m_pPetBongdari)
	{
		WRect rect(GetRect().x + 4, GetRect().y + 49, 607.f, 455.25f);
		m_pPetBongdari->DisplayClip(rect, 55.f, 0.1f, 100.f);
	}
}

void FrBongdariShopDlg::OnBonusBall01BnInit(int param)
{
	m_pBonusBallBn[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBonusBallBn[0])
	{
		m_pBonusBallBn[0]->SetVisible(false);
		m_pBonusBallBn[0]->SetMouseEvent(false);
	}
}

void FrBongdariShopDlg::OnBonusBall01OwnerDraw()
{
	DrawBonusBall(0);
}

void FrBongdariShopDlg::OnBonusBall02BnInit(int param)
{
	m_pBonusBallBn[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBonusBallBn[1])
	{
		m_pBonusBallBn[1]->SetVisible(false);
		m_pBonusBallBn[1]->SetMouseEvent(false);
	}
}

void FrBongdariShopDlg::OnBonusBall02OwnerDraw()
{
	DrawBonusBall(1);
}

void FrBongdariShopDlg::OnBonusBall03BnInit(int param)
{
	m_pBonusBallBn[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBonusBallBn[2])
	{
		m_pBonusBallBn[2]->SetVisible(false);
		m_pBonusBallBn[2]->SetMouseEvent(false);
	}
}

void FrBongdariShopDlg::OnBonusBall03OwnerDraw()
{
	DrawBonusBall(2);
}

void FrBongdariShopDlg::OnBonusBall04BnInit(int param)
{
	m_pBonusBallBn[3] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBonusBallBn[3])
	{
		m_pBonusBallBn[3]->SetVisible(false);
		m_pBonusBallBn[3]->SetMouseEvent(false);
	}
}

void FrBongdariShopDlg::OnBonusBall04OwnerDraw()
{
	DrawBonusBall(3);
}

void FrBongdariShopDlg::OnBonusBall05BnInit(int param)
{
	m_pBonusBallBn[4] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBonusBallBn[4])
	{
		m_pBonusBallBn[4]->SetVisible(false);
		m_pBonusBallBn[4]->SetMouseEvent(false);
	}
}

void FrBongdariShopDlg::OnBonusBall05OwnerDraw()
{
	DrawBonusBall(4);
}

bool FrBongdariShopDlg::OnInit()
{
	EnableDrag(false);
	OpenBongdarishopNoticeDlg(true);
	ShowBongdari();
	InitCouponBongdariShop();
	m_rtBongdari.x += GetRect().x;
	m_rtBongdari.y += GetRect().y;
	return true;
}

void FrBongdariShopDlg::OnProc(const float dt)
{
	if (m_pPetBongdari)
	{
		int result = m_pPetBongdari->ProcessBongdariShop(dt);
		switch (result)
		{
		case 1:
			m_bAnimating = false;
			break;
		case 2:
			m_state = 6;
			break;
		}
	}
	Processing(dt);
	static int i;
	for (i = 0; i < m_bonusBallNum; ++i)
		if (m_pPetBonusBall[i])
			m_pPetBonusBall[i]->ProcessBongdariShop(dt);
	ControlNoticeDlg();
	if (!m_bOpenSubDlg)
	{
		if (m_exitTime < 8)
		{
			m_exitTime += dt;
			if (m_exitTime >= 8 && !m_pCloseBn->IsEnabled())
				m_pCloseBn->Enable(true);
		}
		if (m_rtBongdari.IsInRect(
				WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y)))
		{
			m_cursor = 0;
			switch (m_state)
			{
			case 0:
				if (!m_bUsingBonusBall)
				{
					WRect rect;
					rect = m_pBongdari->GetRect();
					if (rect.IsInRect(g_input->GetMousePoint().x,
							g_input->GetMousePoint().y))
					{
						m_cursor = 1;
						if (m_bBongdariDown)
							m_cursor = 2;
					}
				}
				SetPosMovingClickText(dt);
				break;
			case 4:
				m_overBonusBall = -1;
				for (i = 0; i < m_bonusBallNum; ++i)
				{
					if (m_pBonusBallBn[i] && m_pBonusBallBn[i]->IsVisible() &&
						m_movingData[i].state == 2)
					{
						WRect rect;
						rect = m_pBonusBallBn[i]->GetRect();
						if (rect.IsInRect(g_input->GetMousePoint().x,
								g_input->GetMousePoint().y))
						{
							m_cursor = 1;
							m_overBonusBall = i;
							break;
						}
					}
				}
				SetPosMovingClickText(dt);
				break;
			}
			SetCursor(m_cursor);
		}
	}
	m_feeAniAlpha -= dt * 480;
	m_feeAniPos -= dt * 20;
	m_timesAniAlpha -= dt * 480;
	m_timesAniPos -= dt * 20;
}

bool FrBongdariShopDlg::OnLButtonDown(const WPoint& pt)
{
	FrForm::OnLButtonDown(pt);
	switch (m_state)
	{
	case 0:
	{
		m_bBongdariDown = false;
		WRect rect(m_pBongdari->GetRect());
		if (rect.IsInRect(pt))
			m_bBongdariDown = true;
		break;
	}
	case 4:
		for (int i = 0; i < m_bonusBallNum; ++i)
		{
			if (m_pBonusBallBn[i] && m_pBonusBallBn[i]->IsVisible() &&
				m_movingData[i].state == 2)
			{
				WRect rect;
				rect = m_pBonusBallBn[i]->GetRect();
				if (rect.IsInRect(pt))
				{
					if (m_pTotalItemBn)
						m_pTotalItemBn->EnableBlink(true);
					OpenBonusBall(i);
					g_audio->PlaySfx("\xb9\xf6\xc6\xb0\xc5\xac\xb8\xaf");
					return false;
				}
			}
		}
		break;
	}
	return false;
}

bool FrBongdariShopDlg::OnLButtonUp(const WPoint& pt)
{
	FrForm::OnLButtonUp(pt);
	if (m_pBongdari && m_state == 0 && !m_bUsingBonusBall && m_bBongdariDown)
	{
		WRect rect(m_pBongdari->GetRect());
		if (rect.IsInRect(pt))
		{
			if (Doc()->m_myInfo.stat.Level == 0)
				OpenBongdarishopWarningDlg(0, 0);
			else
			{
				SetUsingBonusBall(true);
				WSendPacket packet((enumClientPacket)0x6d);
				packet.Send(TO_GAME);
				g_audio->PlaySfx("\xb9\xf6\xc6\xb0\xc5\xac\xb8\xaf");
				EnableExitButton(false);
				m_exitTime = 0;
			}
		}
	}
	m_bBongdariDown = false;
	return false;
}

void FrBongdariShopDlg::OnCancel()
{
}

void FrBongdariShopDlg::OpenAboutShopDlg()
{
	FrAboutShopDlg* form =
		CreateForm<FrAboutShopDlg>(g_pFresh->GetManager(), this, "about_shop");
	if (form)
	{
		form->Open((FRESH_PFN_RESULT)&FrBongdariShopDlg::OnAboutShopDlgResult,
			3);

		m_bOpenSubDlg = true;
	}
}

void FrBongdariShopDlg::OpenTotalItemDlg()
{
	FrTotalItemDlg* form =
		CreateForm<FrTotalItemDlg>(g_pFresh->GetManager(), this, "total_item");
	if (form)
	{
		form->SetListItem(&m_totalItem);
		form->Open((FRESH_PFN_RESULT)&FrBongdariShopDlg::OnTotalItemDlgResult,
			3);

		m_bOpenSubDlg = true;
	}
}

void FrBongdariShopDlg::OpenBongdarishopNoticeDlg(bool in)
{
	FrBongdarishopNoticeDlg* form = CreateForm<FrBongdarishopNoticeDlg>(
		g_pFresh->GetManager(), this, "bongdarishop_notice");
	if (form)
	{
		form->Open(
			(FRESH_PFN_RESULT)&FrBongdariShopDlg::OnBongdarishopNoticeDlgResult,
			192);
		form->SetButtonType(in);
		form->SetFlag(in ? 193 : 3);
		m_bOpenSubDlg = true;
	}
}

void FrBongdariShopDlg::OpenBongdarishopRareItemDlg(int index)
{
	FrBongdarishopRareItemDlg* form = CreateForm<FrBongdarishopRareItemDlg>(
		g_pFresh->GetManager(), this, "bongdarishop_rareitem");
	if (form)
	{
		form->Open((FRESH_PFN_RESULT)&FrBongdariShopDlg::
					   OnBongdarishopRareItemDlgResult,
			3);

		if (index >= 0 && index < m_bonusBallNum)
		{
			form->SetRareItem(&m_bonusBall[index]);
			m_bOpenSubDlg = true;
		}
	}
}

void FrBongdariShopDlg::OpenBongdarishopWarningDlg(int type, int price)
{
	FrBongdarishopWarningDlg* form = CreateForm<FrBongdarishopWarningDlg>(
		g_pFresh->GetManager(), this, "bongdarishop_warning");
	if (form)
	{
		form->Open((FRESH_PFN_RESULT)&FrBongdariShopDlg::
					   OnBongdarishopWarningDlgResult,
			64);
		form->SetImgWarningBack(type, price);
		m_bOpenSubDlg = true;
	}
}

void FrBongdariShopDlg::OpenClosedBonusBallResultDlg()
{
	if (AfxGetTask())
	{
		FrClosedBonusBallResultDlg* form =
			CreateForm<FrClosedBonusBallResultDlg>(g_pFresh->GetManager(), NULL,
				"closed_bonusball");
		if (form)
		{
			form->Open(NULL, 3);
			form->SetClosedBonusBall(m_bonusBall);
		}
	}
}

bool FrBongdariShopDlg::OnAboutShopDlgResult(int ret, FrForm* form)
{
	m_bOpenSubDlg = false;
	return true;
}

bool FrBongdariShopDlg::OnTotalItemDlgResult(int ret, FrForm* form)
{
	m_bOpenSubDlg = false;
	return true;
}

bool FrBongdariShopDlg::OnBongdarishopNoticeDlgResult(int ret, FrForm* form)
{
	if (ret == FrOK)
	{
		FrWnd* wnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			"bongdarishop_notice");
		if (!wnd)
		{
			FrBongdarishopNoticeDlg* notice =
				DYNAMIC_CAST(FrBongdarishopNoticeDlg, wnd);
		}
		else if (((FrBongdarishopNoticeDlg*)wnd)->GetMoveLayoutTid())
			OpenAboutShopDlg();
	}
	m_bOpenSubDlg = false;
	return true;
}

bool FrBongdariShopDlg::OnBongdarishopRareItemDlgResult(int ret, FrForm* form)
{
	m_bOpenSubDlg = false;
	return true;
}

bool FrBongdariShopDlg::OnBongdarishopWarningDlgResult(int ret, FrForm* form)
{
	m_bOpenSubDlg = false;
	return true;
}

void FrBongdariShopDlg::ControlNoticeDlg()
{
	FrWnd* wnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
		"bongdarishop_notice");
	if (!wnd)
	{
		FrBongdarishopNoticeDlg* notice =
			DYNAMIC_CAST(FrBongdarishopNoticeDlg, wnd);
	}
	else
	{
		FrBongdarishopNoticeDlg* notice = (FrBongdarishopNoticeDlg*)wnd;
		if (g_input->GetDown("\xc1\xc2", false))
			notice->PrevIndex();
		else if (g_input->GetDown("\xbf\xec", false))
			notice->NextIndex();
	}
}

void FrBongdariShopDlg::InitBongdariShop()
{
	for (int i = 0; i < 9; ++i)
	{
		m_silhouette[i].ballIndex = -1;
		m_silhouette[i].pBitmap = m_pImgItemBase;
		m_silhouette[i].typeId = 0;
		m_silhouette[i].category = 0;
		m_silhouette[i].state = 0;
		m_silhouette[i].time = 10;
	}
	memset(m_bonusBall, 0, sizeof(m_bonusBall));
	m_bonusBallNum = 0;
	for (int i = 0; i < 5; ++i)
	{
		if (m_pPetBonusBall[i])
		{
			delete m_pPetBonusBall[i];
			m_pPetBonusBall[i] = NULL;
		}
	}
	memset(m_movingData, 0, sizeof(m_movingData));
}

void FrBongdariShopDlg::SetBongdariShop()
{
	if (m_bonusBallNum <= 5)
	{
		DeletePetBonusBall();
		SetRandomDataItemSilhouette();
		InitCouponBongdariShop();
		m_state = 1;
	}
}

void FrBongdariShopDlg::SetBonusBall(int num, sBonusBall* balls)
{
	if (balls && num <= 5)
	{
		memcpy(m_bonusBall, balls, sizeof(sBonusBall) * num);
		m_bonusBallNum = num;
	}
}

void FrBongdariShopDlg::OpenBonusBall(int index)
{
	if (index >= 0 && index < m_bonusBallNum)
	{
		for (int i = 0; i < 9; ++i)
		{
			if (m_silhouette[i].ballIndex == index)
			{
				m_movingData[index].state = 3;
				m_movingData[index].bottom =
					WVector(m_silhouette[i].pBitmap->Width() / 2 +
							g_arrPtItemSilhouette[i].x + GetRect().x - 7,
						m_silhouette[i].pBitmap->Height() / 2 +
							g_arrPtItemSilhouette[i].y + GetRect().y - 15,
						0);
				m_movingData[index].velocity =
					WVector((float)(m_movingData[index].bottom.x -
								m_movingData[index].pos.x),
						(float)(m_movingData[index].bottom.y -
							m_movingData[index].pos.y),
						0);
				break;
			}
		}
	}
}

void FrBongdariShopDlg::Clear()
{
	SendStateBongdariShop(false);
	if (m_pNumberUI)
	{
		delete m_pNumberUI;
		m_pNumberUI = NULL;
	}
	if (m_pFontPang)
		m_pFontPang->Erase();
	if (m_pFontCount)
		m_pFontCount->Erase();
	if (m_pFontCoupon)
		m_pFontCoupon->Erase();
	if (g_resrcmng && m_pFontCoupon)
	{
		g_resrcmng->Release(m_pFontCoupon);
		m_pFontCoupon = NULL;
	}
	if (g_resrcmng && m_pFontPang)
	{
		g_resrcmng->Release(m_pFontPang);
		m_pFontPang = NULL;
	}
	if (g_resrcmng && m_pFontCount)
	{
		g_resrcmng->Release(m_pFontCount);
		m_pFontCount = NULL;
	}
	if (m_pPetBongdari)
	{
		delete m_pPetBongdari;
		m_pPetBongdari = NULL;
	}
	DeletePetBonusBall();
	FrWnd* wnd0 =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("about_shop");
	if (!wnd0)
	{
		FrAboutShopDlg* form = DYNAMIC_CAST(FrAboutShopDlg, wnd0);
	}
	else
		((FrAboutShopDlg*)wnd0)->Close(FrCANCEL, true);
	FrWnd* wnd1 =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("total_item");
	if (!wnd1)
	{
		FrTotalItemDlg* form = DYNAMIC_CAST(FrTotalItemDlg, wnd1);
	}
	else
		((FrTotalItemDlg*)wnd1)->Close(FrOK, true);
	FrWnd* wnd2 = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
		"bongdarishop_notice");
	if (!wnd2)
	{
		FrBongdarishopNoticeDlg* form =
			DYNAMIC_CAST(FrBongdarishopNoticeDlg, wnd2);
	}
	else
		((FrBongdarishopNoticeDlg*)wnd2)->Close(FrOK, true);
	FrWnd* wnd3 = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
		"bongdarishop_rareitem");
	if (!wnd3)
	{
		FrBongdarishopRareItemDlg* form =
			DYNAMIC_CAST(FrBongdarishopRareItemDlg, wnd3);
	}
	else
		((FrBongdarishopRareItemDlg*)wnd3)->Close(FrCANCEL, true);
	if (AfxGetTask())
		for (int i = 0; i < m_bonusBallNum; ++i)
			UpdateListMyItem(&m_bonusBall[i]);
}

void FrBongdariShopDlg::CreateBonusBall(WVector pos)
{
	WMatrix camera(g_view->GetCamera());
	WVector2D center(g_view->GetScreenCenter());
	float fov = g_view->GetFOV();
	WMatrix ballCamera;
	ballCamera.Reset();
	ballCamera.zm = -25;
	g_view->SetCamera(ballCamera);
	g_view->SetScreenCenter(g_view->GetWidth() * 0.5f,
		g_view->GetHeight() * 0.5f);
	g_view->SetFOV(0.9773844f);
	g_view->UpdateCamera();
	WTVertex projected;
	g_view->Projection2(&projected, pos);
	g_view->SetCamera(camera);
	g_view->SetScreenCenter(center);
	g_view->SetFOV(fov);
	g_view->UpdateCamera();
	projected.sy -= 80;
	static int index;
	for (index = 0; index < m_bonusBallNum; ++index)
	{
		if (m_movingData[index].pos.x <= 0)
		{
			m_movingData[index].pos = WVector(projected.sx, projected.sy, 0);
			break;
		}
	}
	if (index < m_bonusBallNum)
	{
		float y = (rand() % 10 - 5) * 4.3f + 50;
		float z = (float)(rand() % 8) - 8;
		m_movingData[index].velocity =
			WVector(((float)(rand() % 30) - 20) * 20.6f, -y, z);
		m_movingData[index].bottom =
			WVector(0, (float)(rand() % 70) + GetRect().y + 240, 0);
		m_movingData[index].state = 1;
		m_movingData[index].bounceCount = 0;
		m_movingData[index].scale =
			(m_movingData[index].bottom.y - (GetRect().y + 240) - 35) / 140;
		float direction = (float)(rand() % 2 ? -1 : 1);
		m_movingData[index].rotSpeed =
			((float)(rand() % 30) + 20) * direction * 0.1f;
		ShowBonusBall(index);
	}
}

void FrBongdariShopDlg::InitCouponBongdariShop()
{
	m_couponCount = 0;
	for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); ++it)
	{
		if (ItemManager()->GetCouponKind((*it).tid) == 2)
			m_couponCount += (*it).Common[0];
	}
}

void FrBongdariShopDlg::SetDataFee(int fee)
{
	if (fee > m_dataFee)
	{
		m_dataFee = fee;
		m_feeAniPos = 0;
		m_feeAniAlpha = 512;
	}
}

void FrBongdariShopDlg::UpdateUsableTimes()
{
	m_usableTimes = Doc()->GetBsUsableTimes();
	m_usableBonusTimes = Doc()->GetBsUsableBonusTimes();
	m_remainedBonusTimes = Doc()->GetBsRemainedBonusTimes();
}

void FrBongdariShopDlg::ShowDiscountUsableTimes()
{
	if (m_usableTimes > 1 || m_usableTimes == -1)
	{
		m_bShowUsableTimes = true;
		m_timesAniPos = 0;
		m_timesAniAlpha = 512;
	}
	else if (m_usableBonusTimes > 0)
	{
		m_bShowBonusTimes = true;
		m_timesAniPos = 0;
		m_timesAniAlpha = 512;
	}
	else if (m_usableBonusTimes == 0 && m_remainedBonusTimes == 0)
		OpenBongdarishopWarningDlg(1, 0);
	else
		OpenBongdarishopWarningDlg(2, m_remainedBonusTimes);
	if (m_usableTimes == -1)
		m_bShowUsableTimes = false;
	if (m_usableBonusTimes == -1)
		m_bShowBonusTimes = false;
}

void FrBongdariShopDlg::Processing(float dt)
{
	static int i;
	switch (m_state)
	{
	case 1:
		for (i = 0; i < 9; ++i)
			m_silhouette[i].time = 0;
		g_audio->PlaySfx("play_fail_item");
		m_state = 2;
		break;
	case 2:
	{
		bool finished = true;
		for (i = 0; i < 9; ++i)
		{
			m_silhouette[i].time += dt * 15;
			if ((int)m_silhouette[i].time < 8)
			{
				if ((int)m_silhouette[i].time >= 3)
					m_silhouette[i].state = 1;
				finished = false;
			}
		}
		if (finished)
		{
			SetStartAnimationBongdari();
			m_state = 3;
		}
		break;
	}
	case 3:
		for (i = 0; i < m_bonusBallNum; ++i)
		{
			if (m_pPetBonusBall[i] && m_movingData[i].state == 1)
			{
				m_pPetBonusBall[i]->SetCenter(m_movingData[i].pos.x,
					m_movingData[i].pos.y);
				if (m_movingData[i].bounceCount >= 3)
				{
					m_movingData[i].pos.y = m_movingData[i].bottom.y;
					m_movingData[i].state = 2;
					WRect rect(m_pBonusBallBn[i]->GetRect());
					rect.x = m_movingData[i].pos.x - rect.w * 0.5f;
					rect.y = m_movingData[i].pos.y - rect.h * 0.5f;
					m_pBonusBallBn[i]->SetRect(rect);
				}
				else
				{
					if (!m_pBonusBallBn[i]->IsVisible())
						m_pBonusBallBn[i]->SetVisible(true);
					m_movingData[i].pos.x += dt * m_movingData[i].velocity.x;
					m_movingData[i].pos.y += dt * m_movingData[i].velocity.y;
					if (m_movingData[i].velocity.x > dt * 60)
						m_movingData[i].velocity.x -= dt * 60;
					else if (m_movingData[i].velocity.x < -(dt * 60))
						m_movingData[i].velocity.x += dt * 60;
					m_movingData[i].velocity.y += dt * 480;
					if (m_movingData[i].velocity.y > 0 &&
						m_movingData[i].pos.y >= m_movingData[i].bottom.y)
					{
						m_movingData[i].velocity.y =
							-(m_movingData[i].velocity.y / 3);
						m_movingData[i].velocity.x *= 2.0 / 3.0;
						++m_movingData[i].bounceCount;
						g_audio->PlaySfx("\xb5\xb9\xc7\xc7\xb4\xcf_\xb0\xf8");
					}
					if (m_movingData[i].velocity.x < 0)
					{
						if (m_movingData[i].pos.x <= GetRect().x + 50)
							m_movingData[i].velocity.x =
								-m_movingData[i].velocity.x;
					}
					else if (m_movingData[i].velocity.x > 0)
					{
						if (m_movingData[i].pos.x >=
							GetRect().w + GetRect().x + 50)
							m_movingData[i].velocity.x =
								-m_movingData[i].velocity.x;
					}
				}
			}
		}
		for (i = 0; i < m_bonusBallNum; ++i)
			if (m_movingData[i].state != 2)
				return;
		m_state = 4;
		break;
	case 4:
		for (i = 0; i < m_bonusBallNum; ++i)
		{
			switch (m_movingData[i].state)
			{
			case 2:
				if (m_pPetBonusBall[i])
				{
					WMatrix& matrix = m_pPetBonusBall[i]->GetMatrix();
					matrix *= RotMat(
						(float)(dt * m_movingData[i].rotSpeed * 8) * g_DEGTORAD,
						2);
					matrix *= RotMat(
						(float)(dt * m_movingData[i].rotSpeed * 8) * g_DEGTORAD,
						1);
				}
				break;
			case 3:
				if (m_pPetBonusBall[i])
				{
					m_pPetBonusBall[i]->SetCenter(m_movingData[i].pos.x,
						m_movingData[i].pos.y);
					if (m_movingData[i].pos.y >= m_movingData[i].bottom.y)
					{
						m_movingData[i].pos.y = m_movingData[i].bottom.y;
						m_movingData[i].state = 4;
					}
					else
					{
						m_movingData[i].pos.x +=
							dt * m_movingData[i].velocity.x * 2;
						m_movingData[i].pos.y +=
							dt * m_movingData[i].velocity.y * 2;
					}
				}
				break;
			case 4:
				if (m_pPetBonusBall[i])
				{
					delete m_pPetBonusBall[i];
					m_pPetBonusBall[i] = NULL;
					m_pBonusBallBn[i]->SetVisible(false);
					m_movingData[i].state = 5;
					switch (m_bonusBall[i].category)
					{
					case 0:
						g_audio->PlaySfx(
							"\xbe\xc6\xc0\xcc\xc5\xdb\xc8\xb9\xb5\xe6(\xc6\xce)");
						break;
					case 1:
						g_audio->PlaySfx(
							"\xbe\xc6\xc0\xcc\xc5\xdb\xc8\xb9\xb5\xe6(\xc4\xed\xc5\xb0)");
						break;
					case 2:
						g_audio->PlaySfx(
							"\xbe\xc6\xc0\xcc\xc5\xdb\xc8\xb9\xb5\xe6(\xb7\xb9\xbe\xee)");
						OpenBongdarishopRareItemDlg(i);
						break;
					}
					for (int j = 0; j < 9; ++j)
						if (m_silhouette[j].ballIndex == i)
						{
							m_silhouette[j].time = 0;
							break;
						}
				}
				break;
			case 5:
				for (int j = 0; j < 9; ++j)
				{
					if (m_silhouette[j].ballIndex == i)
					{
						m_silhouette[j].time += dt * 15;
						if ((int)m_silhouette[j].time < 8)
						{
							if ((int)m_silhouette[j].time >= 3)
							{
								m_silhouette[j].state = 2;
								SetOpeningBonusBallInfo(&m_bonusBall[i]);
							}
						}
						else
						{
							m_movingData[i].state = 6;
							AddListTotalItem(&m_bonusBall[i]);
							UpdateListMyItem(&m_bonusBall[i]);
							SendReBuildMyInventory();
						}
						break;
					}
				}
				break;
			case 6:
				m_bonusBall[i].color = -1;
				break;
			}
		}
		for (i = 0; i < m_bonusBallNum; ++i)
			if (m_bonusBall[i].color >= 0)
				return;
		m_pPetBongdari->SetMotion(s_bongdariMotions[6], false, 0);
		m_state = 5;
		break;
	case 6:
		m_pPetBongdari->SetMotion(s_bongdariMotions[0], false, 0);
		m_state = 7;
		m_processTime = 0;
		break;
	case 7:
		m_processTime += dt;
		if (m_processTime > 1)
		{
			m_state = 8;
			m_processTime = 0;
		}
		break;
	case 8:
		for (int j = 0; j < 9; ++j)
			m_silhouette[j].time = 0;
		g_audio->PlaySfx("play_fail_item");
		m_state = 9;
		SetOpeningBonusBallInfo(NULL);
		break;
	case 9:
	{
		bool finished = false;
		for (int j = 0; j < 9; ++j)
		{
			m_silhouette[j].time += dt * 15;
			if ((int)m_silhouette[j].time < 8)
			{
				if ((int)m_silhouette[j].time >= 3)
					m_silhouette[j].state = 0;
			}
			else
				finished = true;
		}
		if (finished)
		{
			m_state = 0;
			m_bUsingBonusBall = false;
		}
	}
	break;
	}
}

void FrBongdariShopDlg::SetPosMovingClickText(float dt)
{
	if (m_bClickTextUp == false)
	{
		m_clickTextPos += dt * 10;
		if (m_clickTextPos >= 5)
			m_bClickTextUp = true;
	}
	else if (m_bClickTextUp == true)
	{
		m_clickTextPos -= dt * 10;
		if (m_clickTextPos <= 0)
			m_bClickTextUp = false;
	}
}

void FrBongdariShopDlg::DisplayItemSilhouette()
{
	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
	if (g && m_bonusBallNum <= 5)
	{
		unsigned long color = 0xffffffff;
		WRect area(GetRect());
		const Bitmap* bitmap = NULL;
		char count[8] = "";
		for (int i = 0; i < 9; ++i)
		{
			const unsigned char state = m_silhouette[i].state;
			switch (state)
			{
			case 0:
				bitmap = m_pImgItemBase;
				color = 0xffffffff;
				break;
			case 1:
				switch (m_silhouette[i].category)
				{
				case 1:
					color = 0xa02020a0;
					break;
				case 2:
					color = 0xa0a02020;
					break;
				default:
					color = 0xa0202020;
					break;
				}
				bitmap = m_silhouette[i].pBitmap;
				break;
			case 2:
				bitmap = m_silhouette[i].pBitmap;
				color = 0xffffffff;
				break;
			}
			if (bitmap)
			{
				float xOffset = 0, yOffset = 0;
				if (state > 0 && state <= 2 &&
					ItemManager()->GetCouponKind(m_silhouette[i].typeId))
				{
					xOffset = -7;
					yOffset = 2;
				}
				WRect rect((float)(area.x + xOffset) +
						g_arrPtItemSilhouette[i].x,
					area.y + g_arrPtItemSilhouette[i].y + yOffset,
					(float)bitmap->Width(), (float)bitmap->Height());
				g->DrawTexture(bitmap, rect, color, 0);
				if (m_silhouette[i].state == 2 &&
					m_silhouette[i].ballIndex >= 0 &&
					m_bonusBall[m_silhouette[i].ballIndex].count > 1 &&
					m_pFontCount)
				{
					sprintf(count, "%d",
						m_bonusBall[m_silhouette[i].ballIndex].count);
					m_pFontCount->Print(g_view, rect.x + 32, rect.y + 30, count,
						0, 0xffffffff, NULL);
				}
				if (m_silhouette[i].time >= 0 &&
					(int)m_silhouette[i].time < 8 && m_pNumberUI)
				{
					m_pNumberUI->Render(
						MakeStr("%d", (int)m_silhouette[i].time),
						area.x + g_arrPtItemSilhouette[i].x - 25,
						area.y + g_arrPtItemSilhouette[i].y - 43, 1.5f, 0);
				}
			}
		}
	}
}

void FrBongdariShopDlg::DrawShadow()
{
	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
	if (g && m_pImgShadow)
	{
		WRect rect;
		if (m_state == 0 && m_pPetBongdari)
		{
			rect = WRect(WPoint(m_pBongdari->GetRect().w * 0.5f +
								 m_pBongdari->GetRect().x -
								 ((float)m_pImgShadow->Width() * 1.5f) * 0.5f,
							 m_pBongdari->GetRect().h +
								 m_pBongdari->GetRect().y - 23 -
								 ((float)m_pImgShadow->Height() * 1.5f) * 0.5f),
				((float)m_pImgShadow->Width() * 1.5f),
				((float)m_pImgShadow->Height() * 1.5f));
			g->DrawTexture(m_pImgShadow, rect, 0x90ffffff, 0);
		}
		for (int i = 0; i < m_bonusBallNum; ++i)
		{
			if (m_pPetBonusBall[i])
			{
				float distance =
					fabs(m_movingData[i].pos.y - m_movingData[i].bottom.y);
				if (distance < 40)
				{
					unsigned char alpha = 144;
					if (distance > 0)
						alpha = (unsigned char)(144 / (distance * 1.5f));
					WVector center(m_movingData[i].pos.x,
						m_movingData[i].bottom.y, 0);
					if (m_overBonusBall == i)
					{
						rect = WRect(center.x -
								0.5f * ((float)m_pImgShadow->Width() * 0.7f),
							center.y -
								0.5f * ((float)m_pImgShadow->Height() * 0.7f) +
								36,
							((float)m_pImgShadow->Width() * 0.7f),
							((float)m_pImgShadow->Height() * 0.7f));
					}
					else
					{
						rect = WRect(center.x -
								0.5f * ((float)m_pImgShadow->Width() * 0.55f),
							center.y -
								0.5f * ((float)m_pImgShadow->Height() * 0.55f) +
								28,
							((float)m_pImgShadow->Width() * 0.55f),
							((float)m_pImgShadow->Height() * 0.55f));
					}
					g->DrawTexture(m_pImgShadow, rect, (alpha << 24) | 0xffffff,
						0);
				}
			}
		}
	}
}

void FrBongdariShopDlg::DrawTextBongdariClick()
{
	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
	if (g && m_pBongdari && m_pImgTextBongdariClick)
	{
		WRect area(m_pBongdari->GetRect());
		WRect rect(area.x - 13, area.y - 100 + m_clickTextPos,
			(float)m_pImgTextBongdariClick->Width(),
			(float)m_pImgTextBongdariClick->Height());
		g->DrawTexture(m_pImgTextBongdariClick, rect, 0xffffffff, 0);
	}
}

void FrBongdariShopDlg::DrawTextBonusBallClick()
{
	FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
	if (g && m_pImgTextBonusBallClick)
	{
		for (int i = 0; i < 5; ++i)
			if (!m_pBonusBallBn[i])
				return;
		if (m_overBonusBall >= 0 && m_overBonusBall < m_bonusBallNum)
		{
			WRect area(m_pBonusBallBn[m_overBonusBall]->GetRect());
			WRect rect(area.x - 10, area.y - 85 + m_clickTextPos,
				(float)m_pImgTextBonusBallClick->Width(),
				(float)m_pImgTextBonusBallClick->Height());
			g->DrawTexture(m_pImgTextBonusBallClick, rect, 0xffffffff, 0);
		}
	}
}

void FrBongdariShopDlg::DrawUserInfo()
{
	if (IsLocalContent(S3_BS_USABLE_TIMES))
	{
		FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
		if (!g || !m_pFontPang || !m_pFontCoupon)
			return;
		char text[32] = "";
		WRect area(GetRect());
		static int pangWidth = m_pFontPang->GetFontInfo()->fontw;
		static int couponWidth = m_pFontCoupon->GetFontInfo()->fontw;
		static float width;
		if (m_usableTimes >= 0)
		{
			short remaining = m_usableTimes - 1;
			if (remaining < 0)
				remaining = 0;
			MakePriceStrA(text, remaining);
			width = (float)((strlen(text) - 1) * (pangWidth - 1.f));
			m_pFontPang->Print(g_view, area.x + 165 - width, area.y + 16, text,
				0, 0xffffffff, NULL);
		}
		else
		{
			sprintf(text, "-");
			m_pFontPang->Print(g_view, area.x + 165, area.y + 16, text, 0,
				0xffffffff, NULL);
		}
		if (m_usableBonusTimes >= 0)
		{
			MakePriceStrA(text, m_usableBonusTimes);
			width = (float)((strlen(text) - 1) * (couponWidth - 1.f));
			m_pFontCoupon->Print(g_view, area.x + 205 - width, area.y + 16,
				text, 0, 0xffffffff, NULL);
		}
		else
		{
			sprintf(text, "-");
			m_pFontCoupon->Print(g_view, area.x + 205, area.y + 16, text, 0,
				0xffffffff, NULL);
		}
		MakePriceStrA(text, m_couponCount);
		width = (float)((strlen(text) - 1) * (pangWidth - 1.f));
		m_pFontPang->Print(g_view, area.x + 327 - width, area.y + 16, text, 0,
			0xffffffff, NULL);
		MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		width = (float)((strlen(text) - 1) * (pangWidth - 1.f));
		m_pFontPang->Print(g_view, area.x + 485 - width, area.y + 16, text, 0,
			0xffffffff, NULL);
		if (m_dataFee >= 0 && m_pImgTextFee[m_dataFee])
		{
			unsigned char alpha;
			if (m_feeAniAlpha < 0)
			{
				alpha = 0;
				m_dataFee = -1;
			}
			else if (m_feeAniAlpha > 255)
				alpha = 255;
			else
				alpha = (unsigned char)m_feeAniAlpha;
			unsigned long color = (alpha << 24) | 0xffffff;
			WRect rect(area.x, area.y + m_feeAniPos,
				(float)m_pImgTextFee[m_dataFee]->Width(),
				(float)m_pImgTextFee[m_dataFee]->Height());
			switch (m_dataFee)
			{
			case 0:
				rect.x += 446;
				rect.y += 10;
				break;
			case 1:
				rect.x += 317;
				rect.y += 10;
				break;
			}
			g->DrawTexture(m_pImgTextFee[m_dataFee], rect, color, 0);
		}
		if (m_bShowUsableTimes && m_pImgTextFee[1])
		{
			unsigned char alpha;
			if (m_timesAniAlpha < 0)
			{
				alpha = 0;
				m_bShowUsableTimes = false;
			}
			else if (m_timesAniAlpha > 255)
				alpha = 255;
			else
				alpha = (unsigned char)m_timesAniAlpha;
			unsigned long color = (alpha << 24) | 0xffffff;
			WRect rect(area.x + 160, area.y + m_timesAniPos + 10,
				(float)m_pImgTextFee[1]->Width(),
				(float)m_pImgTextFee[1]->Height());
			g->DrawTexture(m_pImgTextFee[1], rect, color, 0);
		}
		if (m_bShowBonusTimes && m_pImgTextFee[1])
		{
			unsigned char alpha;
			if (m_timesAniAlpha < 0)
			{
				alpha = 0;
				m_bShowUsableTimes = false;
			}
			else if (m_timesAniAlpha > 255)
				alpha = 255;
			else
				alpha = (unsigned char)m_timesAniAlpha;
			unsigned long color = (alpha << 24) | 0xffffff;
			WRect rect(area.x + 200, area.y + m_timesAniPos + 10,
				(float)m_pImgTextFee[1]->Width(),
				(float)m_pImgTextFee[1]->Height());
			g->DrawTexture(m_pImgTextFee[1], rect, color, 0);
		}
	}
	else
	{
		FrGraphicInterface* g = g_pFresh->GetManager()->GetGDI();
		if (!g || !m_pFontPang)
			return;
		char text[32] = "";
		WRect area(GetRect());
		static int pangWidth = m_pFontPang->GetFontInfo()->fontw;
		static float width;
		MakePriceStrA(text, m_couponCount);
		width = (float)((strlen(text) - 1) * (pangWidth - 1.f));
		m_pFontPang->Print(g_view, area.x + 306 - width, area.y + 16, text, 0,
			0xffffffff, NULL);
		MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		width = (float)((strlen(text) - 1) * (pangWidth - 1.f));
		m_pFontPang->Print(g_view, area.x + 479 - width, area.y + 16, text, 0,
			0xffffffff, NULL);
		if (m_dataFee >= 0 && m_pImgTextFee[m_dataFee])
		{
			unsigned char alpha;
			if (m_feeAniAlpha < 0)
			{
				alpha = 0;
				m_dataFee = -1;
			}
			else if (m_feeAniAlpha > 255)
				alpha = 255;
			else
				alpha = (unsigned char)m_feeAniAlpha;
			unsigned long color = (alpha << 24) | 0xffffff;
			WRect rect(area.x, area.y + m_feeAniPos,
				(float)m_pImgTextFee[m_dataFee]->Width(),
				(float)m_pImgTextFee[m_dataFee]->Height());
			switch (m_dataFee)
			{
			case 0:
				rect.x += 440;
				rect.y += 10;
				break;
			case 1:
				rect.x += 296;
				rect.y += 10;
				break;
			}
			g->DrawTexture(m_pImgTextFee[m_dataFee], rect, color, 0);
		}
	}
}

void FrBongdariShopDlg::SetOpeningBonusBallInfo(sBonusBall* ball)
{
	char text[256] = "";
	if (ball)
	{
		IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(ball->typeId);
		if (item)
			SetMessage(MakeStr("%s %d\xb0\xb3 \xc8\xb9\xb5\xe6", item->Name,
						   ball->count),
				false);
	}
	else
	{
		SetMessage(
			"\xc8\xb9\xb5\xe6 \xbe\xc6\xc0\xcc\xc5\xdb \xc1\xa4\xba\xb8\xb0\xa1 \xc7\xa5\xbd\xc3\xb5\xcb\xb4\xcf\xb4\xd9",
			false);
	}
}

void FrBongdariShopDlg::ShowBongdari()
{
	if (m_pPetBongdari)
	{
		delete m_pPetBongdari;
		m_pPetBongdari = NULL;
	}
	if (m_pBongdari)
	{
		WRect rect = m_pBongdari->GetRect();
		float distance = g_view->GetWidth() * 0.0421875f;
		m_pPetBongdari =
			new CExhibition(rect.w * 0.5f + rect.x, rect.h * 0.5f + rect.y,
				(g_view->GetWidth() * 0.0015625f -
					(1.f / g_view->GetRatio() - 1.3333334f) * 0.7f) /
					g_view->GetHeight() * 69.f,
				true);
		m_pPetBongdari->SetDistanceRange(distance - 5, distance + 5);
		m_pPetBongdari->SetDistance(distance, true, true);
		m_pPetBongdari->SetArea(rect.w, rect.h);
		m_pPetBongdari->SetModel("bon_shop", (eExhibitType)1);
	}
}

void FrBongdariShopDlg::ShowBonusBall(int index)
{
	for (int i = 0; i < 5; ++i)
		if (!m_pBonusBallBn[i])
			return;
	char name[64] = "";
	WRect rect = m_pBonusBallBn[index]->GetRect();
	m_pPetBonusBall[index] = new CExhibition(m_movingData[index].pos.x,
		m_movingData[index].pos.y, 0, true);
	m_pPetBonusBall[index]->SetDistance((3.5f - m_movingData[index].scale) *
			g_view->GetWidth() * 0.0015625f,
		true, true);
	m_pPetBonusBall[index]->SetArea(rect.w, rect.h);
	switch (m_bonusBall[index].color)
	{
	case 0:
		sprintf(name, "B_dol01");
		break;
	case 1:
		sprintf(name, "B_dol02");
		break;
	case 2:
		sprintf(name, "B_dol03");
		break;
	}
	if (strlen(name) > 0)
		m_pPetBonusBall[index]->SetModel(name, (eExhibitType)2);
}

void FrBongdariShopDlg::DeletePetBonusBall()
{
	for (int i = 0; i < 5; ++i)
	{
		if (m_pPetBonusBall[i])
		{
			delete m_pPetBonusBall[i];
			m_pPetBonusBall[i] = NULL;
		}
	}
}

void FrBongdariShopDlg::SetStartAnimationBongdari()
{
	if (m_pPetBongdari)
	{
		m_bAnimating = true;
		m_pPetBongdari->SetMotion(s_bongdariMotions[rand() % 5 + 1], false, 0);
	}
}

void FrBongdariShopDlg::InitRandomDataItemSilhouette()
{
	{
		int index = 0;
		int cumulative = g_arrdwPercentNumCookieItem0[0][0];
		for (int i = 0; i < 100; ++i)
		{
			g_arrdwRandNumCookieItem0[i] =
				g_arrdwPercentNumCookieItem0[index][1];
			if (i == cumulative - 1)
			{
				++index;
				cumulative += g_arrdwPercentNumCookieItem0[index][0];
			}
		}
	}
	{
		int index = 0;
		int cumulative = g_arrdwPercentNumCookieItem1[0][0];
		for (int i = 0; i < 100; ++i)
		{
			g_arrdwRandNumCookieItem1[i] =
				g_arrdwPercentNumCookieItem1[index][1];
			if (i == cumulative - 1)
			{
				++index;
				cumulative += g_arrdwPercentNumCookieItem1[index][0];
			}
		}
	}
	{
		int index = 0;
		int cumulative = g_arrdwPercentNumCookieItem2[0][0];
		for (int i = 0; i < 100; ++i)
		{
			g_arrdwRandNumCookieItem2[i] =
				g_arrdwPercentNumCookieItem2[index][1];
			if (i == cumulative - 1)
			{
				++index;
				cumulative += g_arrdwPercentNumCookieItem2[index][0];
			}
		}
	}
	{
		int index = 0;
		int cumulative = g_arrdwPercentSelectCookieItem[0][0];
		for (int i = 0; i < 100; ++i)
		{
			g_arrdwRandSelectCookieItem[i] =
				g_arrdwPercentSelectCookieItem[index][1];
			if (i == cumulative - 1)
			{
				++index;
				cumulative += g_arrdwPercentSelectCookieItem[index][0];
			}
		}
	}
	{
		int index = 0;
		int cumulative = g_arrdwPercentSelectPangItem[0][0];
		for (int i = 0; i < 100; ++i)
		{
			g_arrdwRandSelectPangItem[i] =
				g_arrdwPercentSelectPangItem[index][1];
			if (i == cumulative - 1)
			{
				++index;
				cumulative += g_arrdwPercentSelectPangItem[index][0];
			}
		}
	}
}

void FrBongdariShopDlg::ShuffleRandomDataItemSilhouette()
{
	for (int i = 0; i < 100; ++i)
	{
		{
			int index = rand() % 100;
			unsigned long temp = g_arrdwRandNumCookieItem0[i];
			g_arrdwRandNumCookieItem0[i] = g_arrdwRandNumCookieItem0[index];
			g_arrdwRandNumCookieItem0[index] = temp;
		}
		{
			int index = rand() % 100;
			unsigned long temp = g_arrdwRandNumCookieItem1[i];
			g_arrdwRandNumCookieItem1[i] = g_arrdwRandNumCookieItem1[index];
			g_arrdwRandNumCookieItem1[index] = temp;
		}
		{
			int index = rand() % 100;
			unsigned long temp = g_arrdwRandNumCookieItem2[i];
			g_arrdwRandNumCookieItem2[i] = g_arrdwRandNumCookieItem2[index];
			g_arrdwRandNumCookieItem2[index] = temp;
		}
	}
	for (int i = 0; i < 100; ++i)
	{
		int index = rand() % 100;
		unsigned long temp = g_arrdwRandSelectCookieItem[i];
		g_arrdwRandSelectCookieItem[i] = g_arrdwRandSelectCookieItem[index];
		g_arrdwRandSelectCookieItem[index] = temp;
	}
	for (int i = 0; i < 100; ++i)
	{
		int index = rand() % 100;
		unsigned long temp = g_arrdwRandSelectPangItem[i];
		g_arrdwRandSelectPangItem[i] = g_arrdwRandSelectPangItem[index];
		g_arrdwRandSelectPangItem[index] = temp;
	}
}

void FrBongdariShopDlg::SetRandomDataItemSilhouette()
{
	int rareCount = 0;
	int cookieCount = 0;
	int slot = 0;
	for (int i = 0; i < 5; ++i)
	{
		switch (m_bonusBall[i].category)
		{
		case 1:
			++cookieCount;
			break;
		case 2:
			++rareCount;
			break;
		}
	}
	if (rareCount > 0)
	{
		for (int i = 0; i < m_bonusBallNum; ++i)
		{
			if (m_bonusBall[i].category == 2)
			{
				m_silhouette[slot].ballIndex = i;
				m_silhouette[slot].pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", "item_rare");
				m_silhouette[slot].typeId = m_bonusBall[i].typeId;
				m_silhouette[slot].category = 2;
				m_silhouette[slot].state = 0;
				m_silhouette[slot].time = 10;
				++slot;
			}
		}
	}
	int randomCount = 0;
	switch (cookieCount)
	{
	case 0:
		randomCount = g_arrdwRandNumCookieItem0[rand() % 100];
		break;
	case 1:
		randomCount = g_arrdwRandNumCookieItem1[rand() % 100];
		break;
	case 2:
		randomCount = g_arrdwRandNumCookieItem2[rand() % 100];
		break;
	}
	if (cookieCount > 0)
	{
		for (int i = 0; i < m_bonusBallNum; ++i)
		{
			if (m_bonusBall[i].category == 1)
			{
				IFF_ITEM_COMMON* item =
					ItemManager()->FindCommonItem(m_bonusBall[i].typeId);
				m_silhouette[slot].ballIndex = i;
				if (item)
					m_silhouette[slot].pBitmap =
						g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
				else
					m_silhouette[slot].pBitmap = NULL;
				m_silhouette[slot].typeId = m_bonusBall[i].typeId;
				m_silhouette[slot].category = 1;
				m_silhouette[slot].state = 0;
				m_silhouette[slot].time = 10;
				++slot;
			}
		}
	}
	if (randomCount > 0)
	{
		for (int i = 0; i < randomCount - cookieCount; ++i)
		{
			unsigned long typeId = SelectDifferentTypeId(1);
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
			m_silhouette[slot].ballIndex = -1;
			if (item)
				m_silhouette[slot].pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
			else
				m_silhouette[slot].pBitmap = NULL;
			m_silhouette[slot].typeId = typeId;
			m_silhouette[slot].category = 1;
			m_silhouette[slot].state = 0;
			m_silhouette[slot].time = 10;
			++slot;
		}
	}
	for (int i = 0; i < m_bonusBallNum; ++i)
	{
		if (m_bonusBall[i].category == 0)
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(m_bonusBall[i].typeId);
			m_silhouette[slot].ballIndex = i;
			if (item)
				m_silhouette[slot].pBitmap =
					g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
			else
				m_silhouette[slot].pBitmap = NULL;
			m_silhouette[slot].typeId = m_bonusBall[i].typeId;
			m_silhouette[slot].category = 0;
			m_silhouette[slot].state = 0;
			m_silhouette[slot].time = 10;
			++slot;
		}
	}
	for (; slot < 9; ++slot)
	{
		unsigned long typeId = g_arrdwRandSelectPangItem[rand() % 100];
		IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
		m_silhouette[slot].ballIndex = -1;
		if (item)
			m_silhouette[slot].pBitmap =
				g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
		else
			m_silhouette[slot].pBitmap = NULL;
		m_silhouette[slot].typeId = typeId;
		m_silhouette[slot].category = 0;
		m_silhouette[slot].state = 0;
		m_silhouette[slot].time = 10;
	}
	for (int i = 0; i < 9; ++i)
	{
		int index = rand() % 9;
		sItemSilhouette temp = m_silhouette[i];
		m_silhouette[i] = m_silhouette[index];
		m_silhouette[index] = temp;
	}
}

unsigned long FrBongdariShopDlg::SelectDifferentTypeId(int category)
{
	unsigned long* table;
	if (category == 0)
		table = g_arrdwRandSelectPangItem;
	else if (category == 1)
		table = g_arrdwRandSelectCookieItem;
	else
		return 0;
	unsigned long typeId = table[rand() % 100];
	for (;;)
	{
		bool duplicate = false;
		for (int i = 0; i < 9; ++i)
		{
			if (m_silhouette[i].typeId == 0)
				break;
			if (m_silhouette[i].typeId == typeId)
			{
				duplicate = true;
				break;
			}
		}
		if (!duplicate)
			return typeId;
		typeId = table[rand() % 100];
	}
}

void FrBongdariShopDlg::AddListTotalItem(sBonusBall* ball)
{
	if (ball)
	{
		std::list<sBonusBall>::iterator it;
		for (it = m_totalItem.begin(); it != m_totalItem.end(); it++)
		{
			if ((*it).typeId == ball->typeId)
			{
				if ((*it).timeType == 0)
					(*it).count += ball->count;
				else
					(*it).time += ball->time;
				return;
			}
		}
		m_totalItem.push_back(*ball);
	}
}

void FrBongdariShopDlg::UpdateListMyItem(sBonusBall* ball)
{
	if (!ball || ball->color < 0)
		return;
	bool add = true;
	std::list<sItemInfo>* list = NULL;
	switch (ball->typeId >> 26)
	{
	case PIG_PART:
		list = &Doc()->m_myItemList;
		break;
	case PIG_ITEM:
		list = &Doc()->m_myItemList;
		break;
	case PIG_BALL:
		list = &Doc()->m_ballList;
		break;
	case PIG_CADITEM:
		list = &Doc()->m_cadItemList;
		break;
	case PIG_SETITEM:
		list = &Doc()->m_setItemList;
		break;
	case PIG_SKIN:
		list = &Doc()->m_cutinList;
		break;
	case PIG_AUXPART:
		list = &Doc()->m_auxPartList;
		break;
	case PIG_QUESTDROP:
		list = &Doc()->m_questDropList;
		break;
	}
	if (list)
		for (std::list<sItemInfo>::iterator it = list->begin();
			it != list->end(); it++)
		{
			if (it->tid == ball->typeId && it->ItemType == 0 &&
				ball->timeType == 0)
			{
				it->Common[0] += (short)ball->count;
				add = false;
			}
		}
	for (std::map<unsigned int, sCaddieInfo>::iterator it =
			 Doc()->m_caddieMap.begin();
		it != Doc()->m_caddieMap.end(); it++)
		if (it->second.tid == ball->typeId)
		{
			add = false;
			break;
		}
	for (std::map<unsigned int, sItemInfo>::iterator it =
			 Doc()->m_clubSetMap.begin();
		it != Doc()->m_clubSetMap.end(); it++)
		if (it->second.guid == ball->id)
		{
			add = false;
			break;
		}
	if (add)
	{
		unsigned long typeId = ball->typeId;
		if ((typeId >> 26) == PIG_CADDIE)
		{
			unsigned long id = ball->id;
			sCaddieInfo info = { 0 };
			info.Remain_Date = 30;
			info.gift_flag = 0;
			info.Rent_flag = 0;
			info.guid = id;
			info.tid = typeId;
			info.Purchase = 0;
			Doc()->m_caddieMap[id] = info;
		}
		else if ((typeId >> 26) == PIG_CLUBSET)
		{
			sItemInfo info;
			unsigned long id = ball->id;
			info.guid = id;
			info.tid = typeId;
			Doc()->m_clubSetMap[id] = info;
		}
		else
		{
			sItemInfo info;
			short common[5] = { 0 };
			common[0] = 1;
			if (ball->timeType == 0)
				common[0] = (short)ball->count;
			else if (ball->timeType == 1 || ball->timeType == 2)
			{
				common[0] = ball->time;
				common[4] = ball->time;
			}
			info.gift_flag = 0;
			unsigned long id = ball->id;
			info.guid = id;
			info.tid = typeId;
			info.Purchase = 0;
			memcpy(info.Common, common, sizeof(common));
			info.gift_flag = 0;
			info.Expired = 0;
			info.ItemType = ball->timeType;
			if (info.ItemType)
			{
				info.Common[0] = 1;
				if (info.ItemType == 2)
					info.HourRemain = ball->time;
				else if (info.ItemType == 3)
					info.HourRemain = ball->time;
				else if (info.ItemType == 4)
					info.HourRemain = ball->time * 24;
				else if (info.ItemType == 5)
					info.HourRemain = ball->time * 720;
			}
			switch (ball->typeId >> 26)
			{
			case PIG_PART:
				Doc()->AddMyPartsList(info);
				break;
			case PIG_BALL:
				Doc()->m_ballList.push_back(info);
				break;
			case PIG_ITEM:
				Doc()->m_myItemList.push_back(info);
				break;
			case PIG_CADITEM:
				Doc()->m_cadItemList.push_back(info);
				break;
			case PIG_SETITEM:
				Doc()->m_setItemList.push_back(info);
				break;
			case PIG_SKIN:
				Doc()->m_cutinList.push_back(info);
				break;
			case PIG_AUXPART:
				Doc()->m_auxPartList.push_back(info);
				break;
			case PIG_QUESTDROP:
				Doc()->m_questDropList.push_back(info);
				break;
			}
		}
	}
	if (ItemManager()->GetCouponKind(ball->typeId) == 2)
		InitCouponBongdariShop();
}

void FrBongdariShopDlg::SendStateBongdariShop(bool enter)
{
	if (IsLocalContent(S3_BONGDARISHOP) && AfxGetTask())
	{
		if (AfxGetTask()->GetActor("Lobby"))
			AfxGetTask()->GetActor("Lobby")
				<< MsgObject(NULL, 175, enter, 0, 0, 0, 0);
		if (AfxGetTask()->GetActor("Shop"))
			AfxGetTask()->GetActor("Shop")
				<< MsgObject(NULL, 175, enter, 0, 0, 0, 0);
		if (AfxGetTask()->GetActor("RealMyRoom"))
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 175, enter, 0, 0, 0, 0);
	}
}

void FrBongdariShopDlg::SendReBuildMyInventory()
{
	if (IsLocalContent(S3_BONGDARISHOP) && AfxGetTask())
	{
		if (AfxGetTask()->GetActor("Shop"))
			AfxGetTask()->GetActor("Shop")
				<< MsgObject(NULL, 176, 0, 0, 0, 0, 0);
		if (AfxGetTask()->GetActor("RealMyRoom"))
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 176, 0, 0, 0, 0, 0);
	}
}

void FrBongdariShopDlg::SendDataClosedBonusBall()
{
	if (AfxGetTask())
	{
		bool found = false;
		for (int i = 0; i < 5; ++i)
		{
			if (m_bonusBall[i].typeId != 0 && m_bonusBall[i].color >= 0)
			{
				found = true;
				break;
			}
		}
		if (found && IsLocalContent(S3_BONGDARISHOP))
		{
			if (AfxGetTask()->GetActor("GolfRule"))
				AfxGetTask()->GetActor("GolfRule")
					<< MsgObject(NULL, 177, (int)m_bonusBall, 0, 0, 0, 0);
		}
	}
}

void FrBongdariShopDlg::EnableExitButton(bool enable)
{
	if (!(m_exitTime < 8) && m_pCloseBn)
		m_pCloseBn->Enable(enable);
}

void FrBongdariShopDlg::DrawBonusBall(unsigned int index)
{
	if (index < 5 && m_state >= 3 && m_state <= 4 && m_pPetBonusBall[index])
	{
		float distance = 3.5f;
		if (m_overBonusBall == index)
			distance = 2.9f;
		m_pPetBonusBall[index]->SetDistance(
			(distance - m_movingData[index].scale) * g_view->GetWidth() *
				0.0015625f,
			true, true);
		m_pPetBonusBall[index]->Display(0.1f, 100.f);
	}
}

IMPLEMENT_OBJECT(FrAboutShopDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrAboutShopDlg, FrForm)
ON_FRESH_VI("about_shop", FRCMD_INIT, FrAboutShopDlg::OnAboutShopInit)
ON_FRESH_VI("aboutviewer", FRCMD_INIT, FrAboutShopDlg::OnAboutViewerInit)
ON_FRESH_VI("index01", FRCMD_INIT, FrAboutShopDlg::OnIndex01Init)
ON_FRESH_VV("index01", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex01BnUp)
ON_FRESH_VI("index02", FRCMD_INIT, FrAboutShopDlg::OnIndex02Init)
ON_FRESH_VV("index02", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex02BnUp)
ON_FRESH_VI("index03", FRCMD_INIT, FrAboutShopDlg::OnIndex03Init)
ON_FRESH_VV("index03", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex03BnUp)
ON_FRESH_VI("index04", FRCMD_INIT, FrAboutShopDlg::OnIndex04Init)
ON_FRESH_VV("index04", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex04BnUp)
ON_FRESH_VI("index05", FRCMD_INIT, FrAboutShopDlg::OnIndex05Init)
ON_FRESH_VV("index05", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex05BnUp)
ON_FRESH_VI("index06", FRCMD_INIT, FrAboutShopDlg::OnIndex06Init)
ON_FRESH_VV("index06", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex06BnUp)
ON_FRESH_VI("index07", FRCMD_INIT, FrAboutShopDlg::OnIndex07Init)
ON_FRESH_VV("index07", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex07BnUp)
ON_FRESH_VI("index08", FRCMD_INIT, FrAboutShopDlg::OnIndex08Init)
ON_FRESH_VV("index08", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex08BnUp)
ON_FRESH_VI("index09", FRCMD_INIT, FrAboutShopDlg::OnIndex09Init)
ON_FRESH_VV("index09", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex09BnUp)
ON_FRESH_VI("index10", FRCMD_INIT, FrAboutShopDlg::OnIndex10Init)
ON_FRESH_VV("index10", FRCMD_LBUTTONUP, FrAboutShopDlg::OnIndex10BnUp)
ON_FRESH_VI("backward", FRCMD_INIT, FrAboutShopDlg::OnBackwardBnInit)
ON_FRESH_VV("backward", FRCMD_LBUTTONUP, FrAboutShopDlg::OnBackwardBnUp)
ON_FRESH_VI("forward", FRCMD_INIT, FrAboutShopDlg::OnForwardBnInit)
ON_FRESH_VV("forward", FRCMD_LBUTTONUP, FrAboutShopDlg::OnForwardBnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrAboutShopDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrAboutShopDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrAboutShopDlg::FrAboutShopDlg()
{
	for (int i = 0; i < 10; ++i)
		m_pIndexBn[i] = NULL;
	m_index = 0;
	m_pBackwardBn = NULL;
	m_pForwardBn = NULL;
}

FrAboutShopDlg::~FrAboutShopDlg()
{
}

void FrAboutShopDlg::OnAboutShopInit(int param)
{
}

void FrAboutShopDlg::OnAboutViewerInit(int param)
{
	m_pAboutViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex01Init(int param)
{
	m_pIndexBn[0] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex01BnUp()
{
	SetImgAboutIndex(0);
}

void FrAboutShopDlg::OnIndex02Init(int param)
{
	m_pIndexBn[1] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex02BnUp()
{
	SetImgAboutIndex(1);
}

void FrAboutShopDlg::OnIndex03Init(int param)
{
	m_pIndexBn[2] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex03BnUp()
{
	SetImgAboutIndex(2);
}

void FrAboutShopDlg::OnIndex04Init(int param)
{
	m_pIndexBn[3] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex04BnUp()
{
	SetImgAboutIndex(3);
}

void FrAboutShopDlg::OnIndex05Init(int param)
{
	m_pIndexBn[4] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex05BnUp()
{
	SetImgAboutIndex(4);
}

void FrAboutShopDlg::OnIndex06Init(int param)
{
	m_pIndexBn[5] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex06BnUp()
{
	SetImgAboutIndex(5);
}

void FrAboutShopDlg::OnIndex07Init(int param)
{
	m_pIndexBn[6] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex07BnUp()
{
	SetImgAboutIndex(6);
}

void FrAboutShopDlg::OnIndex08Init(int param)
{
	m_pIndexBn[7] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex08BnUp()
{
	SetImgAboutIndex(7);
}

void FrAboutShopDlg::OnIndex09Init(int param)
{
	m_pIndexBn[8] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex09BnUp()
{
	SetImgAboutIndex(8);
}

void FrAboutShopDlg::OnIndex10Init(int param)
{
	m_pIndexBn[9] = DYNAMIC_CAST(FrTextButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnIndex10BnUp()
{
	SetImgAboutIndex(9);
}

void FrAboutShopDlg::OnBackwardBnInit(int param)
{
	m_pBackwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnBackwardBnUp()
{
	--m_index;
	SetImgAboutIndex(m_index);
	m_pBackwardBn->SetPushDelay(0);
}

void FrAboutShopDlg::OnForwardBnInit(int param)
{
	m_pForwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnForwardBnUp()
{
	++m_index;
	SetImgAboutIndex(m_index);
	m_pForwardBn->SetPushDelay(0);
}

void FrAboutShopDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrAboutShopDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrAboutShopDlg::OnInit()
{
	SetImgAboutIndex(0);
	return true;
}

void FrAboutShopDlg::SetImgAboutIndex(int index)
{
	for (int i = 0; i < 10; ++i)
		m_pIndexBn[i]->Unselect(false);
	if (m_pBackwardBn)
		m_pBackwardBn->Enable(true);
	if (m_pForwardBn)
		m_pForwardBn->Enable(true);
	if (index <= 0)
	{
		index = 0;
		m_pIndexBn[0]->Select(false);
		if (m_pBackwardBn)
			m_pBackwardBn->Enable(false);
	}
	else if (index >= 9)
	{
		index = 9;
		m_pIndexBn[9]->Select(false);
		if (m_pForwardBn)
			m_pForwardBn->Enable(false);
	}
	m_pIndexBn[index]->Select(false);
	m_index = index;
	if (m_pAboutViewer)
		m_pAboutViewer->Open(MakeStr("help_bs_%02d.jpg", index + 1));
}

IMPLEMENT_OBJECT(FrTotalItemDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrTotalItemDlg, FrForm)
ON_FRESH_VI("total_item", FRCMD_INIT, FrTotalItemDlg::OnTotalItemInit)
ON_FRESH_VI("totalitemlist", FRCMD_INIT, FrTotalItemDlg::OnTotalItemListInit)
ON_FRESH_VI("totalitemlist", FRCMD_OWNERDRAW,
	FrTotalItemDlg::OnTotalItemListOwnerDraw)
ON_FRESH_VV("totalitemlist", FRCMD_RBUTTONDOWN,
	FrTotalItemDlg::OnTotalItemListRBnDown)
ON_FRESH_VI("ok", FRCMD_INIT, FrTotalItemDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrTotalItemDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrTotalItemDlg::FrTotalItemDlg()
{
	m_pTotalItemList = NULL;
	m_pImgIconBase =
		g_pFresh->GetManager()->GetBitmap("ITEMS", "shop_icon_base02");
	static char* files[] = { "[font_wind.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
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

FrTotalItemDlg::~FrTotalItemDlg()
{
	m_itemList.clear();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrTotalItemDlg::OnTotalItemInit(int param)
{
}

void FrTotalItemDlg::OnTotalItemListInit(int param)
{
	m_pTotalItemList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pTotalItemList->UseRightButton(true);
	m_miniBtn[4].m_pBitmap =
		g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_miniBtn[4].m_rect = WRect(50, 35, m_miniBtn[4].m_pBitmap->Width(),
		m_miniBtn[4].m_pBitmap->Height());
	m_miniBtn[3].m_pBitmap =
		g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_miniBtn[3].m_rect = WRect(50, 35, m_miniBtn[3].m_pBitmap->Width(),
		m_miniBtn[3].m_pBitmap->Height());
}

void FrTotalItemDlg::OnTotalItemListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* draw = (FrListItem*)param;
	if (!gdi || !draw)
		return;
	sBonusBall* ball = (sBonusBall*)draw->pData;
	if (!ball->typeId)
		return;
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(ball->typeId);
	if (!item)
		return;
	float dx = 0, dy = 0;
	const Bitmap* image = g_pFresh->GetBitmap(item->Icon);
	if ((ball->typeId >> 26) == 2)
	{
		dx = -12;
		dy = -20;
	}
	if ((ball->typeId >> 26) == 31)
	{
		dx = -2;
		dy = -9;
	}
	if (image)
	{
		if (m_pImgIconBase)
		{
			WRect bgRect(draw->pos.x, draw->pos.y,
				(float)m_pImgIconBase->Width(),
				(float)m_pImgIconBase->Height());
			gdi->DrawTexture(m_pImgIconBase, bgRect, 0xffffffff, 0);
			WRect imageRect(dx + draw->pos.x + 18, dy + draw->pos.y + 11,
				(float)image->Width(), (float)image->Height());
			gdi->DrawTexture(image, imageRect, 0xffffffff, 0);
			gdi->SetTextColor(0xff000000, 0xffffffff);
			gdi->SetTextStyle(4);
			if (ball->timeType != 0)
			{
				if (ball->time > 0)
					gdi->DrawTexture(m_miniBtn[4].m_pBitmap,
						m_miniBtn[4].m_rect + draw->pos, 0xffffffff, 0);
			}
			else
			{
				if (ball->count > 1)
				{
					unsigned int i = 0;
					const char* text = MakeStr("%d", ball->count);
					float x = draw->pos.x + 70;
					for (i = 0; i < strlen(text); ++i)
						x -= m_pFont->GetCharWidth(g_view, text[i] - '0');
					m_pFont->Print(g_view, x, draw->pos.y + 45, text, 0,
						0xffffffff, NULL);
				}
			}
		}
	}
}

void FrTotalItemDlg::OnTotalItemListRBnDown()
{
	if (m_pTotalItemList)
	{
		FrListItem* item = m_pTotalItemList->GetItemUnderCursor();
		if (item)
		{
			sBonusBall* ball = (sBonusBall*)item->pData;
			if (ball)
				OpenInformation(ball->typeId);
		}
	}
}

void FrTotalItemDlg::OpenInformation(unsigned long typeId)
{
	FrWnd* existing =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("information");
	if (!existing)
	{
		FrForm* form = DYNAMIC_CAST(FrForm, existing);
		existing = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
			"information_setitem");
		if (!existing)
		{
			FrSetItemDlg* setForm = DYNAMIC_CAST(FrSetItemDlg, existing);
			IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(typeId);
			if (item)
			{
				if ((item->TypeId & 0xfc000000) == 0x24000000)
				{
					setForm = CreateForm<FrSetItemDlg>(g_pFresh->GetManager(),
						this, "information_setitem");
					if (setForm)
					{
						setForm->SetSetItem(item->TypeId);
						setForm->Open(NULL, 3);
					}
				}
				else
				{
					form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
						"information");
					if (form)
					{
						FrArea* portrait = DYNAMIC_CAST(FrArea,
							form->FindChildByName("portrait"));
						if (portrait)
							portrait->SetBgImg(item->Icon);
						FrEdit* name =
							DYNAMIC_CAST(FrEdit, form->FindChildByName("name"));
						FrArea* level = DYNAMIC_CAST(FrArea,
							form->FindChildByName("level"));
						if (name && level)
						{
							name->AddText(
								MakeStr("\xc0\xcc\xb8\xa7: %s", item->Name),
								false, true);
							name->AddText("\n", false, true);
							if (item->IsUnderLvl)
							{
								level->SetBgImg(
									MakeStr("level_%03d", item->Level + 1));
								name->AddText(
									MakeStr(
										"\xb7\xb9\xba\xa7:                \xc0\xcc\xc7\xcf",
										item->Name),
									false, true);
							}
							else
							{
								level->SetBgImg(
									MakeStr("level_%03d", item->Level + 1));
								name->AddText(
									MakeStr(
										"\xb7\xb9\xba\xa7:                \xc0\xcc\xbb\xf3",
										item->Name),
									false, true);
							}
						}
						IFF_STRUCT::sDesc* desc =
							ItemManager()->FindDesc(item->TypeId);
						if (desc)
							form->SetMessage(desc->Desc, false);
						form->Open(NULL, 3);
					}
				}
			}
		}
	}
}

void FrTotalItemDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrTotalItemDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrTotalItemDlg::OnInit()
{
	return true;
}

void FrTotalItemDlg::SetListItem(std::list<sBonusBall>* items)
{
	if (items)
	{
		m_itemList.assign(items->begin(), items->end());
		if (m_itemList.size() <= 0)
		{
			SetDesc(
				"\xc8\xb9\xb5\xe6\xc7\xcf\xbd\xc5 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			return;
		}
		SetDesc(
			"\xc8\xb9\xb5\xe6\xc7\xcf\xbd\xc5 \xb8\xf0\xb5\xe7 \xbe\xc6\xc0\xcc\xc5\xdb \xb8\xf1\xb7\xcf\xc0\xd4\xb4\xcf\xb4\xd9.");
		for (std::list<sBonusBall>::iterator it = m_itemList.begin();
			it != m_itemList.end(); it++)
			m_pTotalItemList->AddItem(&*it);
	}
}

char* g_strIndex[] = { "1", "2", "3", "4", "5" };

IMPLEMENT_OBJECT(FrBongdarishopNoticeDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrBongdarishopNoticeDlg, FrForm)
ON_FRESH_VI("bongdarishop_notice", FRCMD_INIT,
	FrBongdarishopNoticeDlg::OnBongdarishopNoticeInit)
ON_FRESH_VI("noticeviewer", FRCMD_INIT,
	FrBongdarishopNoticeDlg::OnNoticeViewerInit)
ON_FRESH_VV("noticeviewer", FRCMD_LBUTTONUP,
	FrBongdarishopNoticeDlg::OnNoticeViewerBnUp)
ON_FRESH_VI("listindex", FRCMD_INIT, FrBongdarishopNoticeDlg::OnListIndexInit)
ON_FRESH_VV("listindex", FRCMD_LBUTTONUP,
	FrBongdarishopNoticeDlg::OnListIndexLBnUp)
ON_FRESH_VI("listindex", FRCMD_OWNERDRAW,
	FrBongdarishopNoticeDlg::OnListIndexOwnerDraw)
ON_FRESH_VI("backward", FRCMD_INIT, FrBongdarishopNoticeDlg::OnBackwardBnInit)
ON_FRESH_VV("backward", FRCMD_LBUTTONUP,
	FrBongdarishopNoticeDlg::OnBackwardBnUp)
ON_FRESH_VI("forward", FRCMD_INIT, FrBongdarishopNoticeDlg::OnForwardBnInit)
ON_FRESH_VV("forward", FRCMD_LBUTTONUP, FrBongdarishopNoticeDlg::OnForwardBnUp)
ON_FRESH_VI("in", FRCMD_INIT, FrBongdarishopNoticeDlg::OnInBnInit)
ON_FRESH_VV("in", FRCMD_LBUTTONUP, FrBongdarishopNoticeDlg::OnInBnUp)
ON_FRESH_VI("out", FRCMD_INIT, FrBongdarishopNoticeDlg::OnOutBnInit)
ON_FRESH_VV("out", FRCMD_LBUTTONUP, FrBongdarishopNoticeDlg::OnOutBnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrBongdarishopNoticeDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrBongdarishopNoticeDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrBongdarishopNoticeDlg::FrBongdarishopNoticeDlg()
{
	m_pListIndex = NULL;
	m_index = 0;
	m_moveLayoutTid = 0;
	m_pBackwardBn = NULL;
	m_pForwardBn = NULL;
	m_pNoticeViewer = NULL;
	m_pInBn = NULL;
	m_pOutBn = NULL;
	m_pOkBn = NULL;
}

FrBongdarishopNoticeDlg::~FrBongdarishopNoticeDlg()
{
}

void FrBongdarishopNoticeDlg::OnBongdarishopNoticeInit(int param)
{
}

void FrBongdarishopNoticeDlg::OnNoticeViewerInit(int param)
{
	m_pNoticeViewer = DYNAMIC_CAST(FrViewer, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnNoticeViewerBnUp()
{
	if (m_index >= 0)
	{
		WPoint mouse(g_pFresh->GetManager()->GetMousePos().x,
			g_pFresh->GetManager()->GetMousePos().y);
		std::list<sNoticeImage::sBn>::iterator it;
		for (it = m_notice[m_index].bnList.begin();
			it != m_notice[m_index].bnList.end(); ++it)
		{
			WPoint rect(GetRect().x, GetRect().y);
			WRect buttonRect(rect.x + (*it).rect.x, rect.y + (*it).rect.y,
				(*it).rect.w, (*it).rect.h);
			if (buttonRect.IsInRect(mouse))
			{
				m_moveLayoutTid = (*it).typeId;
				Close(FrOK, true);
				return;
			}
		}
		Close(FrNONE, true);
	}
}

void FrBongdarishopNoticeDlg::OnListIndexInit(int param)
{
	m_pListIndex = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnListIndexLBnUp()
{
	FrListItem* item = m_pListIndex->GetItemUnderCursor();
	if (item && item->pData)
	{
		m_index = atoi((char*)item->pData) - 1;
		SetImgNotice(m_index);
	}
}

void FrBongdarishopNoticeDlg::OnListIndexOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item && item->pData)
	{
		char* text = (char*)item->pData;
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		if (gdi)
		{
			int index = atoi(text);
			if (m_index == index - 1)
			{
				gdi->SetTextColor(0xffff0000, 0xffffffff);
				gdi->SetTextStyle(1);
			}
			else
			{
				gdi->SetTextColor(0xff000000, 0xffffffff);
				gdi->SetTextStyle(0);
			}
			gdi->Print(item->pos, 0, text);
		}
	}
}

void FrBongdarishopNoticeDlg::OnBackwardBnInit(int param)
{
	m_pBackwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnBackwardBnUp()
{
	--m_index;
	SetImgNotice(m_index);
	m_pBackwardBn->SetPushDelay(0);
}

void FrBongdarishopNoticeDlg::OnForwardBnInit(int param)
{
	m_pForwardBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnForwardBnUp()
{
	++m_index;
	SetImgNotice(m_index);
	m_pForwardBn->SetPushDelay(0);
}

void FrBongdarishopNoticeDlg::OnInBnInit(int param)
{
	m_pInBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnInBnUp()
{
	OnFreshOkay();
}

void FrBongdarishopNoticeDlg::OnOutBnInit(int param)
{
	m_pOutBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnOutBnUp()
{
	FrWnd* existing =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	if (!existing)
	{
		FrForm* form = DYNAMIC_CAST(FrForm, existing);
	}
	else
		((FrForm*)existing)->Close(FrCANCEL, true);
	OnFreshCancel();
}

void FrBongdarishopNoticeDlg::OnOkBnInit(int param)
{
	m_pOkBn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopNoticeDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrBongdarishopNoticeDlg::OnInit()
{
	LoadInitFile("bs_ppl.ini");
	SetImgNotice(0);
	return true;
}

void FrBongdarishopNoticeDlg::LoadInitFile(const char* filename)
{
	LoadNoticeImageFile(filename, m_notice, 5, NULL);
	if (m_pBackwardBn)
	{
		WRect rect = m_pBackwardBn->GetRect();
		m_pBackwardBn->MoveWindow(
			WPoint(m_rect.w * 0.5f + m_rect.x - (m_notice.size() * 7.5f + 20),
				rect.y));
	}
	if (m_pForwardBn)
	{
		WRect rect = m_pForwardBn->GetRect();
		m_pForwardBn->MoveWindow(WPoint(
			m_rect.w * 0.5f + m_notice.size() * 7.5f + m_rect.x, rect.y));
	}
	int count = m_notice.size();
	if (count > 0 && count <= 5)
	{
		for (unsigned char i = 0; i < count; ++i)
			m_pListIndex->AddItem(g_strIndex[i]);
		WRect rect = m_pListIndex->GetRect();
		m_pListIndex->MoveWindow(
			WPoint((5 - m_notice.size()) * 7.5f + rect.x, rect.y));
	}
}

void FrBongdarishopNoticeDlg::SetButtonType(bool in)
{
	if (m_pInBn)
		m_pInBn->SetVisible(in);
	if (m_pOutBn)
		m_pOutBn->SetVisible(in);
	if (m_pOkBn)
		m_pOkBn->SetVisible(!in);
}

void FrBongdarishopNoticeDlg::SetImgNotice(int index)
{
	int count = m_notice.size();
	if (count > 0)
	{
		if (m_pBackwardBn)
			m_pBackwardBn->Enable(true);
		if (m_pForwardBn)
			m_pForwardBn->Enable(true);
		if (index <= 0)
		{
			index = 0;
			if (m_pBackwardBn)
				m_pBackwardBn->Enable(false);
		}
		if (index >= count - 1)
		{
			index = count - 1;
			if (m_pForwardBn)
				m_pForwardBn->Enable(false);
		}
		m_index = index;
		if (m_pNoticeViewer)
			m_pNoticeViewer->Open(MakeStr(m_notice[index].name));
	}
}

void FrBongdarishopNoticeDlg::PrevIndex()
{
	OnBackwardBnUp();
}

void FrBongdarishopNoticeDlg::NextIndex()
{
	OnForwardBnUp();
}

IMPLEMENT_OBJECT(FrBongdarishopRareItemDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrBongdarishopRareItemDlg, FrForm)
ON_FRESH_VI("bongdarishop_rareitem", FRCMD_INIT,
	FrBongdarishopRareItemDlg::OnBongdarishopRareItemInit)
ON_FRESH_VI("rareitemicon", FRCMD_INIT,
	FrBongdarishopRareItemDlg::OnRareItemIconInit)
ON_FRESH_VI("rareitemname", FRCMD_INIT,
	FrBongdarishopRareItemDlg::OnRareItemNameInit)
ON_FRESH_VI("rareitemability", FRCMD_INIT,
	FrBongdarishopRareItemDlg::OnRareItemAbilityInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrBongdarishopRareItemDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrBongdarishopRareItemDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrBongdarishopRareItemDlg::FrBongdarishopRareItemDlg()
{
	m_pRareItemIcon = NULL;
	m_pRareItemName = NULL;
	m_pRareItemAbility = NULL;
}

FrBongdarishopRareItemDlg::~FrBongdarishopRareItemDlg()
{
}

void FrBongdarishopRareItemDlg::OnBongdarishopRareItemInit(int param)
{
}

void FrBongdarishopRareItemDlg::OnRareItemIconInit(int param)
{
	m_pRareItemIcon = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrBongdarishopRareItemDlg::OnRareItemNameInit(int param)
{
	m_pRareItemName = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrBongdarishopRareItemDlg::OnRareItemAbilityInit(int param)
{
	m_pRareItemAbility = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrBongdarishopRareItemDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopRareItemDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrBongdarishopRareItemDlg::OnInit()
{
	return true;
}

void FrBongdarishopRareItemDlg::SetRareItem(sBonusBall* ball)
{
	if (!ball)
		return;
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(ball->typeId);
	if (!item)
		return;
	if (m_pRareItemIcon)
	{
		switch (ball->typeId >> 26)
		{
		case 2:
			m_pRareItemIcon->SetBgImg(
				g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", item->Icon));
			break;
		default:
			m_pRareItemIcon->SetBgImg(
				g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon));
			break;
		}
	}
	if (m_pRareItemName)
	{
		m_pRareItemName->AddText(MakeStr("\\cT\\c%s\\cN\\c", item->Name), false,
			true);
		short attr[5];
		short slot[5] = { 0 };
		switch (ball->typeId >> 26)
		{
		case 2:
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(ball->typeId);
			if (!part)
				return;
			memcpy(attr, part->Attr, sizeof(attr));
			memcpy(slot, part->Slot, sizeof(slot));
			break;
		}
		case 8:
		{
			IFF_STRUCT::sCadItem* caddieItem =
				ItemManager()->FindCadItem(ball->typeId);
			if (!caddieItem)
				return;
			memcpy(attr, caddieItem->COM, sizeof(attr));
			break;
		}
		case 7:
		{
			IFF_STRUCT::sCaddie* caddie =
				ItemManager()->FindCaddie(ball->typeId);
			if (!caddie)
				return;
			memcpy(attr, caddie->Attr, sizeof(attr));
			break;
		}
		default:
			return;
		}
		const char* name = "";
		int remaining = 5;
		for (int i = 0; remaining; --remaining, ++i)
		{
			if (attr[i] || slot[i])
			{
				switch (i)
				{
				case 0:
					name = "\xc6\xc4\xbf\xf6";
					break;
				case 1:
					name = "\xc4\xc1\xc6\xae\xb7\xd1";
					break;
				case 2:
					name = "\xc1\xa4\xc8\xae\xb5\xb5";
					break;
				case 3:
					name = "\xbd\xba\xc7\xc9";
					break;
				case 4:
					name = "\xc4\xbf\xba\xea";
					break;
				}
				if (attr[i] > 0 || slot[i] > 0)
				{
					if (attr[i] == 0)
						m_pRareItemAbility->AddText(
							MakeStr(
								"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
								name, slot[i]),
							false, true);
					else if (slot[i] == 0)
						m_pRareItemAbility->AddText(
							MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
								name, attr[i]),
							false, true);
					else
						m_pRareItemAbility->AddText(
							MakeStr(
								"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
								name, attr[i], slot[i]),
							false, true);
				}
			}
		}
	}
}

IMPLEMENT_OBJECT(FrBongdarishopWarningDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrBongdarishopWarningDlg, FrForm)
ON_FRESH_VI("bongdarishop_warning", FRCMD_INIT,
	FrBongdarishopWarningDlg::OnBongdarishopWarningInit)
ON_FRESH_VI("warning_back", FRCMD_INIT,
	FrBongdarishopWarningDlg::OnWarningBackInit)
ON_FRESH_VI("warning_back", FRCMD_OWNERDRAW,
	FrBongdarishopWarningDlg::OnWarningBackOwnerDraw)
ON_FRESH_VI("ok", FRCMD_INIT, FrBongdarishopWarningDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrBongdarishopWarningDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrBongdarishopWarningDlg::FrBongdarishopWarningDlg()
{
	m_pWarningBack = NULL;
	m_price = 0;
	m_type = 0;
	static char* files[] = { "[font_bong.jpg" };
	WTITLEFONT font;
	memset(&font, 0, sizeof(font));
	font.filename = files;
	font.fontw = 9;
	font.fonth = 16;
	font.numPages = 1;
	font.texw = 128;
	font.texh = 32;
	font.pCharSet = "1234567890,-";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&font);
}

FrBongdarishopWarningDlg::~FrBongdarishopWarningDlg()
{
	if (m_pFont)
		m_pFont->Erase();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrBongdarishopWarningDlg::OnBongdarishopWarningInit(int param)
{
}

void FrBongdarishopWarningDlg::OnWarningBackInit(int param)
{
	m_pWarningBack = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrBongdarishopWarningDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrBongdarishopWarningDlg::OnOkBnUp()
{
	FrWnd* existing =
		g_pFresh->GetManager()->GetDesktop()->FindChildByName("bongdari_shop");
	if (!existing)
	{
		FrForm* form = DYNAMIC_CAST(FrForm, existing);
	}
	else
		((FrForm*)existing)->Close(FrCANCEL, true);
	OnFreshOkay();
}

bool FrBongdarishopWarningDlg::OnInit()
{
	return true;
}

void FrBongdarishopWarningDlg::SetImgWarningBack(int type, int price)
{
	if (m_pWarningBack)
	{
		if (type == 0)
			m_pWarningBack->SetBgImg(g_pFresh->GetBitmap("warning_back"));
		else if (type == 1)
			m_pWarningBack->SetBgImg(g_pFresh->GetBitmap("warning_back_2"));
		else if (type == 2)
		{
			m_price = price;
			m_type = 2;
			m_pWarningBack->SetBgImg(g_pFresh->GetBitmap("warning_back_bonus"));
		}
	}
}

void FrBongdarishopWarningDlg::OnWarningBackOwnerDraw(int param)
{
	if (m_type == 2)
	{
		WRect rect;
		rect = GetRect();
		char text[32] = "";
		MakePriceStrA(text, m_price);
		m_pFont->Print(g_view, rect.x + 170, rect.y + 50, text, 0, 0xffffffff,
			NULL);
	}
}

IMPLEMENT_OBJECT(FrClosedBonusBallResultDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrClosedBonusBallResultDlg, FrForm)
ON_FRESH_VI("closed_bonusball", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallResultInit)
ON_FRESH_VI("closedbonusballlist", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallListInit)
ON_FRESH_VI("closedbonusballlist", FRCMD_OWNERDRAW,
	FrClosedBonusBallResultDlg::OnClosedBonusBallListOwnerDraw)
ON_FRESH_VI("closedbonusballname01", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallName01Init)
ON_FRESH_VI("closedbonusballability01", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallAbility01Init)
ON_FRESH_VI("closedbonusballname02", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallName02Init)
ON_FRESH_VI("closedbonusballability02", FRCMD_INIT,
	FrClosedBonusBallResultDlg::OnClosedBonusBallAbility02Init)
ON_FRESH_VI("ok", FRCMD_INIT, FrClosedBonusBallResultDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrClosedBonusBallResultDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrClosedBonusBallResultDlg::FrClosedBonusBallResultDlg()
{
	m_pClosedBonusBallList = NULL;
	m_pName[0] = NULL;
	m_pName[1] = NULL;
	m_pAbility[0] = NULL;
	m_pAbility[1] = NULL;
	m_topRow = -1;
	memset(m_closedBonusBall, 0, sizeof(m_closedBonusBall));
}

FrClosedBonusBallResultDlg::~FrClosedBonusBallResultDlg()
{
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallResultInit(int param)
{
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallListInit(int param)
{
	m_pClosedBonusBallList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_miniBtn[4].m_pBitmap =
		g_pFresh->GetManager()->GetBitmap("ICONS", "clock");
	m_miniBtn[4].m_rect = WRect(50, 35, m_miniBtn[4].m_pBitmap->Width(),
		m_miniBtn[4].m_pBitmap->Height());
	m_miniBtn[3].m_pBitmap =
		g_pFresh->GetManager()->GetBitmap("ICONS", "delete");
	m_miniBtn[3].m_rect = WRect(50, 35, m_miniBtn[3].m_pBitmap->Width(),
		m_miniBtn[3].m_pBitmap->Height());
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallListOwnerDraw(int param)
{
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	FrListItem* draw = (FrListItem*)param;
	if (!gdi || !draw)
		return;
	sBonusBall* ball = (sBonusBall*)draw->pData;
	if (!ball->typeId)
		return;
	IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(ball->typeId);
	if (!item)
		return;
	float dx = 0, dy = 0;
	const Bitmap* image =
		g_pFresh->GetManager()->GetBitmap("ITEMS", item->Icon);
	if (!image)
	{
		image = g_pFresh->GetManager()->GetBitmap("ITEMS_FASHION", item->Icon);
		dx = -12;
		dy = -20;
	}
	if (image)
	{
		WRect rect(dx + draw->pos.x + 18, dy + draw->pos.y + 11,
			(float)image->Width(), (float)image->Height());
		gdi->DrawTexture(image, rect, 0xffffffff, 0);
		gdi->SetTextColor(0xff000000, 0xffffffff);
		gdi->SetTextStyle(4);
		if (ball->timeType && ball->time > 0)
			gdi->DrawTexture(m_miniBtn[4].m_pBitmap,
				m_miniBtn[4].m_rect + draw->pos, 0xffffffff, 0);
	}
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallName01Init(int param)
{
	m_pName[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallAbility01Init(int param)
{
	m_pAbility[0] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallName02Init(int param)
{
	m_pName[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrClosedBonusBallResultDlg::OnClosedBonusBallAbility02Init(int param)
{
	m_pAbility[1] = DYNAMIC_CAST(FrEdit, (FrWnd*)param);
}

void FrClosedBonusBallResultDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrClosedBonusBallResultDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrClosedBonusBallResultDlg::OnInit()
{
	return true;
}

void FrClosedBonusBallResultDlg::OnProc(const float dt)
{
	if (m_pClosedBonusBallList)
	{
		FrScrollBar* bar = m_pClosedBonusBallList->GetScrollBar();
		if (bar)
		{
			if (m_topRow != bar->GetCurTopRow_Int())
			{
				int row = bar->GetCurTopRow_Int();
				m_topRow = row;
				SetEditDataClosedBonusBall(0, row);
				if (m_pClosedBonusBallList->m_itemList.size() > 1)
					SetEditDataClosedBonusBall(1, m_topRow + 1);
			}
		}
	}
}

void FrClosedBonusBallResultDlg::SetClosedBonusBall(sBonusBall* balls)
{
	WRect rect(GetRect());
	if (balls)
	{
		bool found = false;
		int i;
		for (i = 0; i < 5; ++i)
			if (balls[i].typeId && balls[i].color >= 0)
			{
				found = true;
				break;
			}
		if (!found)
		{
			SetDesc(
				"\xc8\xb9\xb5\xe6\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");
			OnFreshOkay();
			return;
		}
		SetDesc(
			"\xc8\xb9\xb5\xe6 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xba \xb8\xb6\xc0\xcc\xb7\xeb\xbf\xa1\xbc\xad \xc8\xae\xc0\xce\xc7\xcf\xbc\xbc\xbf\xe4.");
		memcpy(m_closedBonusBall, balls, sizeof(m_closedBonusBall));
		for (i = 0; i < 5; ++i)
			if (m_closedBonusBall[i].typeId && m_closedBonusBall[i].color >= 0)
				m_pClosedBonusBallList->AddItem(&m_closedBonusBall[i]);
	}
}

void FrClosedBonusBallResultDlg::SetEditDataClosedBonusBall(int line, int index)
{
	if (!m_pClosedBonusBallList)
		return;
	int n = 0;
	FrListBox::ITEM_LIST::iterator it =
		m_pClosedBonusBallList->m_itemList.begin();
	for (; it != m_pClosedBonusBallList->m_itemList.end(); it++, n++)
	{
		if (index != n)
			continue;
		if (!*it)
			return;
		sBonusBall* ball = (sBonusBall*)(*it)->pData;
		if (!ball)
			return;
		IFF_ITEM_COMMON* item = ItemManager()->FindCommonItem(ball->typeId);
		if (!item || !m_pName[line] || !m_pAbility[line])
			return;
		m_pName[line]->ClearLine();
		m_pAbility[line]->ClearLine();
		m_pName[line]->AddText(MakeStr("%s", item->Name), false, true);
		short attr[5];
		bool hasAttr = true;
		short slot[5] = { 0 };
		switch (ball->typeId >> 26)
		{
		case 2:
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(ball->typeId);
			if (!part)
				hasAttr = false;
			else
				memcpy(attr, part->Attr, sizeof(attr));
			if (part)
				memcpy(slot, part->Slot, sizeof(slot));
			break;
		}
		case 8:
		{
			IFF_STRUCT::sCadItem* caddieItem =
				ItemManager()->FindCadItem(ball->typeId);
			if (!caddieItem)
				hasAttr = false;
			else
				memcpy(attr, caddieItem->COM, sizeof(attr));
			break;
		}
		case 7:
		{
			IFF_STRUCT::sCaddie* caddie =
				ItemManager()->FindCaddie(ball->typeId);
			if (!caddie)
				hasAttr = false;
			else
				memcpy(attr, caddie->Attr, sizeof(attr));
			break;
		}
		default:
			hasAttr = false;
		}
		if (hasAttr)
		{
			const char* name = "";
			int remaining = 5;
			for (int i = 0; remaining; --remaining, ++i)
			{
				if (attr[i] || slot[i])
				{
					switch (i)
					{
					case 0:
						name = "\xc6\xc4\xbf\xf6";
						break;
					case 1:
						name = "\xc4\xc1\xc6\xae\xb7\xd1";
						break;
					case 2:
						name = "\xc1\xa4\xc8\xae\xb5\xb5";
						break;
					case 3:
						name = "\xbd\xba\xc7\xc9";
						break;
					case 4:
						name = "\xc4\xbf\xba\xea";
						break;
					}
					if (attr[i] > 0 || slot[i] > 0)
					{
						if (attr[i] == 0)
							m_pAbility[line]->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									name, slot[i]),
								false, true);
						else if (slot[i] == 0)
							m_pAbility[line]->AddText(
								MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
									name, attr[i]),
								false, true);
						else
							m_pAbility[line]->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									name, attr[i], slot[i]),
								false, true);
					}
				}
			}
		}
		else
			m_pAbility[line]->AddText(MakeStr("%d\xb0\xb3", ball->count), false,
				true);
		return;
	}
}
