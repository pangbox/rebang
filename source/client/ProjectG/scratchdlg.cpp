#include "scratchdlg.h"
#include "frviewer.h"
#include "frdesktop.h"
#include "frtextbutton.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;
WPoint g_ptScratchItem[] = { WPoint(-17.0f, -38.0f), WPoint(23.0f, -38.0f),
	WPoint(3.0f, -38.0f) };

IMPLEMENT_OBJECT(FrScratchDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrScratchDlg, FrForm)
ON_FRESH_VI("scratchdlg", FRCMD_INIT, FrScratchDlg::OnScratchDlgInit)
ON_FRESH_VI("scratch_bottom_02", FRCMD_INIT, FrScratchDlg::OnScratchBottomInit)
ON_FRESH_VV("scratch_bottom_02", FRCMD_OWNERDRAW,
	FrScratchDlg::OnScratchBottomOwnerDraw)
ON_FRESH_VI("scratch_cover_01", FRCMD_INIT, FrScratchDlg::OnScratchCover01Init)
ON_FRESH_VV("scratch_cover_01", FRCMD_OWNERDRAW,
	FrScratchDlg::OnScratchCover01OwnerDraw)
ON_FRESH_VI("scratch_cover_02", FRCMD_INIT, FrScratchDlg::OnScratchCover02Init)
ON_FRESH_VV("scratch_cover_02", FRCMD_OWNERDRAW,
	FrScratchDlg::OnScratchCover02OwnerDraw)
ON_FRESH_VI("scratch_card", FRCMD_INIT, FrScratchDlg::OnScratchCardInit)
ON_FRESH_BI("scratch_card", FRCMD_ENTERKEY, FrScratchDlg::OnScratchCardEnterKey)
ON_FRESH_VI("scratch_nextpage", FRCMD_INIT,
	FrScratchDlg::OnScratchNextPageBnInit)
ON_FRESH_VV("scratch_nextpage", FRCMD_LBUTTONUP,
	FrScratchDlg::OnScratchNextPageBnUp)
ON_FRESH_VI("serial_number", FRCMD_INIT, FrScratchDlg::OnSerialNumberInit)
ON_FRESH_BI("serial_number", FRCMD_ENTERKEY,
	FrScratchDlg::OnSerialNumberEnterKey)
ON_FRESH_VI("serial_number_bn", FRCMD_INIT, FrScratchDlg::OnSerialNumberBnInit)
ON_FRESH_VV("serial_number_bn", FRCMD_LBUTTONUP,
	FrScratchDlg::OnSerialNumberBnUp)
ON_FRESH_VI("about", FRCMD_INIT, FrScratchDlg::OnAboutBnInit)
ON_FRESH_VV("about", FRCMD_LBUTTONUP, FrScratchDlg::OnAboutBnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrScratchDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrScratchDlg::OnCloseBnUp)
END_FRESH_MSGMAP()

FrScratchDlg::FrScratchDlg()
{
	m_pScratchCover[0] = NULL;
	m_pScratchCover[1] = NULL;
	m_pScratchCard = NULL;
	m_pNextPageBn = NULL;
	m_pSerialNumber = NULL;
	m_pSerialNumberBn = NULL;
	static char* fontName = "[font_wind.jpg";
	tagWTITLEFONT fontInfo;
	memset(&fontInfo, 0, sizeof(fontInfo));
	fontInfo.filename = &fontName;
	fontInfo.fontw = 16;
	fontInfo.fonth = 16;
	fontInfo.numPages = 1;
	fontInfo.texw = 128;
	fontInfo.texh = 64;
	fontInfo.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&fontInfo);
	m_pImgCover[0] = NULL;
	m_pImgCover[1] = NULL;
	m_pImgClosedCover =
		g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_cover_close");
	m_pImgEraser =
		g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_eraser");
	m_pImgChangeNumber[0] =
		g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_text_minus");
	m_pImgChangeNumber[1] =
		g_pFresh->GetManager()->GetBitmap("SCRATCHDLG", "scratch_text_plus");
	m_pImgItem[0] = NULL;
	m_pImgItem[1] = NULL;
	memset(m_rtItem, 0, sizeof(m_rtItem));
	m_rtScratchArea = WRect(0.f, 0.f, 0.f, 0.f);
	m_ptEraserOffset = WPoint(-10.f, -10.f);
	m_bScratching = false;
	m_scratchCardCount = 0;
	memset(m_scratchItem, 0, sizeof(m_scratchItem));
	m_scratchItemCount = 0;
	m_result = -1;
	m_bEnableRequestScratchItem = true;
	m_bScratchable = false;
	m_bEnableSerialNumber = true;
	m_bShowResult = false;
	m_changeNumberType[0] = -1;
	m_changeNumberType[1] = -1;
	m_changeNumberPos[0] = 0.0f;
	m_changeNumberPos[1] = 0.0f;
	m_changeNumberAlpha[0] = 0;
	m_changeNumberAlpha[1] = 0;
	m_resultTime = 0.0f;
	memset(m_bCheckedRect, 0, sizeof(m_bCheckedRect));
	memset(m_rtCheck, 0, sizeof(m_rtCheck));
	if (IsLocalContent(S4_MATCHING_SYSTEM))
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(2);
		packet.Send(TO_GAME);
	}
}

FrScratchDlg::~FrScratchDlg()
{
	if (IsLocalContent(S4_MATCHING_SYSTEM) && WNetworkSystem::Instance())
	{
		WSendPacket packet((enumClientPacket)0xb9);
		packet.Encode1(1);
		packet.Send(TO_GAME);
	}
	if (m_pFont)
		m_pFont->Erase();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
	OpenScratchResultDlg();
}

void FrScratchDlg::OnScratchDlgInit(int param)
{
}

void FrScratchDlg::OnScratchBottomInit(int param)
{
	FrArea* area = DYNAMIC_CAST(FrArea, param);
}

void FrScratchDlg::OnScratchBottomOwnerDraw()
{
	DrawScratchItem();
	DrawChangeScratchCardNumber();
}

void FrScratchDlg::OnScratchCover01Init(int param)
{
	m_pScratchCover[0] = DYNAMIC_CAST(FrArea, param);
	if (m_pScratchCover[0])
	{
		m_pImgCover[0] = m_pScratchCover[0]->GetBgImg();
		ResetBmpScratchCover(0);
	}
}

void FrScratchDlg::OnScratchCover01OwnerDraw()
{
}

void FrScratchDlg::OnScratchCover02Init(int param)
{
	m_pScratchCover[1] = DYNAMIC_CAST(FrArea, param);
	if (m_pScratchCover[1])
	{
		m_pImgCover[1] = m_pScratchCover[1]->GetBgImg();
		ResetBmpScratchCover(1);
	}
}

void FrScratchDlg::OnScratchCover02OwnerDraw()
{
}

void FrScratchDlg::OnScratchCardInit(int param)
{
	m_pScratchCard = DYNAMIC_CAST(FrEdit, param);
}

bool FrScratchDlg::OnScratchCardEnterKey(int param)
{
	return false;
}

void FrScratchDlg::OnScratchNextPageBnInit(int param)
{
	m_pNextPageBn = DYNAMIC_CAST(FrButton, param);
	if (m_pNextPageBn)
		m_pNextPageBn->SetVisible(false);
}

void FrScratchDlg::OnScratchNextPageBnUp()
{
	if (m_pNextPageBn)
	{
		m_pNextPageBn->Enable(false);
		m_bScratchable = false;
		if (!m_scratchItem[0].typeId)
			SetBmpResultText(5);
		else
		{
			SetBmpResultText(4);
			OnLButtonUp(WPoint(0.0f, 0.0f));
		}
	}
}

void FrScratchDlg::OnSerialNumberInit(int param)
{
	m_pSerialNumber = DYNAMIC_CAST(FrEdit, param);
}

bool FrScratchDlg::OnSerialNumberEnterKey(int param)
{
	return false;
}

void FrScratchDlg::OnSerialNumberBnInit(int param)
{
	m_pSerialNumberBn = DYNAMIC_CAST(FrButton, param);
}

void FrScratchDlg::OnSerialNumberBnUp()
{
	if (m_pSerialNumberBn)
	{
		SendScratchSerialNumber();
		EnableScratchSerialNumber(false);
	}
}

void FrScratchDlg::OnAboutBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
}

void FrScratchDlg::OnAboutBnUp()
{
	FrScratchAboutDlg* dlg = CreateForm<FrScratchAboutDlg>(
		g_pFresh->GetManager(), this, "scratch_about", NULL);
	if (dlg)
		dlg->Open(NULL, 3);
}

void FrScratchDlg::OnCloseBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
}

void FrScratchDlg::OnCloseBnUp()
{
	OnFreshOkay();
}

bool FrScratchDlg::OnInit()
{
	if (m_pImgEraser && m_pScratchCover[0] && m_pScratchCover[1])
	{
		m_rtScratchArea.x = (int)(m_pScratchCover[0]->GetRect().x -
			(float)(m_pImgEraser->Width() / 2));
		m_rtScratchArea.y = (int)(m_pScratchCover[0]->GetRect().y -
			(float)(m_pImgEraser->Height() / 2));
		m_rtScratchArea.w = (int)(m_pImgCover[0]->Width() +
			m_pImgCover[1]->Width() + m_pImgEraser->Width());
		m_rtScratchArea.h =
			(int)(m_pImgCover[0]->Height() + m_pImgEraser->Height());
	}
	InitScratchCard();
	SetBmpColsedScratchCover();
	return true;
}

void FrScratchDlg::OnProc(const float delta)
{
	ProcessChangeScratchCardNumber(delta);
}

void FrScratchDlg::OnMouseMove(const WPoint& pt)
{
	if (m_bScratching)
	{
		RequestScratchItem();
		OpenScratchCover(pt);
	}
}

bool FrScratchDlg::OnLButtonDown(const WPoint& pt)
{
	FrForm::OnLButtonDown(pt);
	if (m_pScratchCover[0] && m_pScratchCover[1])
	{
		WRect rect(m_pScratchCover[0]->GetRect().x + 10.0f,
			m_pScratchCover[0]->GetRect().y + 10.0f,
			(float)(m_bmpCover[0].Width() + m_bmpCover[1].Width()) - 20.0f,
			(float)m_bmpCover[0].Height() - 20.0f);
		if (rect.IsInRect(pt))
			m_bScratching = true;
	}
	return false;
}

bool FrScratchDlg::OnLButtonUp(const WPoint& pt)
{
	FrForm::OnLButtonUp(pt);
	m_bScratching = false;
	return false;
}

void FrScratchDlg::OnCancel()
{
}

void FrScratchDlg::InitScratchCard()
{
	m_scratchCardCount = 0;
	for (std::list<sItemInfo>::iterator it = Doc()->m_myItemList.begin();
		it != Doc()->m_myItemList.end(); it++)
		if (ItemManager()->GetCouponKind((*it).tid) == 3)
			m_scratchCardCount += (*it).Common[0];
	if (m_pScratchCard)
		m_pScratchCard->SetLine(1, MakeStr("%d", m_scratchCardCount), 0, false,
			0);
}

void FrScratchDlg::SetBmpColsedScratchCover()
{
	if (!m_bScratchable)
	{
		if (!m_scratchCardCount)
		{
			memcpy(m_bmpCover[0].vram, m_pImgClosedCover->vram,
				m_pImgClosedCover->pitch *
					m_pImgClosedCover->bi->bmiHeader.biHeight);
			m_pScratchCover[0]->SetBgImg(&m_bmpCover[0]);
			GDI()->UpdateTextureCacheInfo(&m_bmpCover[0]);
		}
		else
			ResetBmpScratchCover(0);
		ResetBmpScratchCover(1);
	}
}

void FrScratchDlg::ResetBmpScratchCover(int index)
{
	if (m_pImgCover[index])
	{
		if (!m_bmpCover[index].bi)
			m_bmpCover[index].Create(m_pImgCover[index]->Width(),
				m_pImgCover[index]->Height(),
				m_pImgCover[index]->BitsPerPixel());
		memcpy(m_bmpCover[index].vram, m_pImgCover[index]->vram,
			m_pImgCover[index]->pitch *
				m_pImgCover[index]->bi->bmiHeader.biHeight);
		m_pScratchCover[index]->SetBgImg(&m_bmpCover[index]);
		GDI()->UpdateTextureCacheInfo(&m_bmpCover[index]);
	}
}

void FrScratchDlg::OpenScratchCover(const WPoint& pt)
{
	if (m_bScratchable && m_pScratchCover && m_pImgEraser &&
		m_rtScratchArea.IsInRect(pt))
	{
		bool updated[2] = { false, false };
		for (unsigned int y = 0; y < m_pImgEraser->Height(); ++y)
		{
			for (unsigned int x = 0; x < m_pImgEraser->Width(); ++x)
			{
				if (((Bitmap*)m_pImgEraser)->GetAlpha(x, y))
				{
					bool erased = false;
					for (int i = 0; i < 2; ++i)
						if (!erased)
						{
							int px = (int)((float)(int)x + pt.x -
								m_pScratchCover[i]->GetRect().x +
								m_ptEraserOffset.x);
							int py = (int)((float)(int)y + pt.y -
								m_pScratchCover[i]->GetRect().y +
								m_ptEraserOffset.y);
							if (px >= 0 && px < m_bmpCover[i].Width() &&
								py >= 0 && py < m_bmpCover[i].Height())
							{
								m_bmpCover[i].SetAlpha(px, py, 0);
								updated[i] = true;
								erased = true;
							}
						}
				}
			}
		}
		for (int i = 0; i < 2; ++i)
			if (updated[i])
				GDI()->UpdateTextureCacheInfo(&m_bmpCover[i]);
	}
}

void FrScratchDlg::EnableNextPageButton(bool enable)
{
	if (m_pNextPageBn)
	{
		m_pNextPageBn->SetVisible(enable);
		m_pNextPageBn->Enable(true);
	}
}

void FrScratchDlg::EnableScratchSerialNumber(bool enable)
{
	if (m_pSerialNumberBn)
		m_pSerialNumberBn->Enable(enable);
	if (m_pSerialNumber)
	{
		m_pSerialNumber->ClearLine();
		m_pSerialNumber->Enable(enable);
		m_pSerialNumber->SetKeyFocus(enable);
		m_pSerialNumber->SetViewFocus(enable);
	}
	m_bEnableSerialNumber = enable;
}

void FrScratchDlg::SendScratchSerialNumber()
{
	if (m_pSerialNumber)
	{
		const char* number = m_pSerialNumber->GetLine(1, false);
		if (strlen(number) == 13)
		{
			WSendPacket packet((enumClientPacket)0x71);
			packet.Encode4(13);
			packet.EncodeBuffer(number, 13);
			packet.Send(TO_GAME);
			m_bEnableSerialNumber = false;
		}
		else
			SetBmpResultText(0);
	}
}

void FrScratchDlg::ResponseScratchSerialNumber(int result,
	unsigned long itemTypeId)
{
	m_result = result;
	switch (result)
	{
	case 0:
	{
		sBonusBall ball = { 0 };
		ball.typeId = 0x1a000030;
		ball.id = itemTypeId;
		ball.count = 1;
		ball.color = 0;
		ball.category = 0;
		ball.timeType = 0;
		ball.time = 0;
		UpdateListMyItem(&ball);
		InitScratchCard();
		SetBmpColsedScratchCover();
	}
	break;
	case 1:
		SetBmpResultText(1);
		break;
	case 3:
		SetBmpResultText(2);
		break;
	case 4:
		SetBmpResultText(3);
		break;
	default:
		SetBmpResultText(0);
		break;
	}
}

void FrScratchDlg::RequestScratchItem()
{
	if (m_scratchCardCount && m_bEnableRequestScratchItem)
	{
		WSendPacket packet((enumClientPacket)0x70);
		packet.Send(TO_GAME);
		m_bEnableRequestScratchItem = false;
		EnableScratchSerialNumber(false);
	}
}

void FrScratchDlg::SetScratchItem(int count, sBonusBall* balls)
{
	if (balls)
	{
		memcpy(m_scratchItem, balls, count * sizeof(sBonusBall));
		m_scratchItemCount = count;
		m_bScratchable = true;
		for (int i = 0; i < count; ++i)
			UpdateListMyItem(&balls[i]);
		InitScratchCard();
		SetBmpScratchItem();
		SetRectScratchItem();
		SetRectCheckScratchItemRect();
	}
}

void FrScratchDlg::ClearScratchItem()
{
	memset(m_scratchItem, 0, sizeof(m_scratchItem));
	m_scratchItemCount = 0;
}

void FrScratchDlg::SetBmpResultText(int result)
{
	if (result < 0 || result >= 6)
	{
		m_bShowResult = false;
		return;
	}
	if (result == 4)
	{
		OpenScratchResultDlg();
	}
	else
	{
		FrForm* dlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify", NULL);
		if (dlg)
		{
			dlg->Open((FRESH_PFN_RESULT)&FrScratchDlg::OnConfirmResult, 0xc0);
			dlg->SetFadeout(false);
			switch (result)
			{
			case 0:
				dlg->SetMessage(
					"\\c0xffff0000\\c\xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\\c0xff000000\\c\xb0\xa1 \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 1:
				dlg->SetMessage(
					"\xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xc7\xd1 \\c0xffff0000\\c\xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\\c0xff000000\\c\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 2:
				dlg->SetMessage(
					"\\c0xffff0000\\c\xc0\xaf\xc8\xbf \xb1\xe2\xb0\xa3\\c0xff000000\\c\xc0\xcc \xc1\xf6\xb3\xad \xc4\xab\xb5\xe5\xc0\xc7 \xbd\xc3\xb8\xae\xbe\xf3 \xb9\xf8\xc8\xa3\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 3:
				dlg->SetMessage(
					"\xc0\xcc\xba\xa5\xc6\xae\xb7\xce \xb9\xde\xc0\xb8\xbd\xc5 \xc4\xed\xc6\xf9\xc0\xba \xc0\xaf\xc8\xbf\xb1\xe2\xb0\xa3 \xb3\xbb\xbf\xa1 \xc7\xd1 \xb0\xe8\xc1\xa4\xb4\xe7 \xc3\xd6\xb4\xeb 5\xb0\xb3\xb1\xee\xc1\xf6\xb8\xb8 \xbb\xe7\xbf\xeb\xc0\xcc \xb0\xa1\xb4\xc9\xc7\xd5\xb4\xcf\xb4\xd9. \xc0\xcc\xb9\xcc \xc4\xed\xc6\xf9\xc0\xbb \xb8\xf0\xb5\xce \xbb\xe7\xbf\xeb\xc7\xcf\xbc\xcc\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				break;
			case 5:
				dlg->SetMessage("\xb4\xd9\xc0\xbd \xb1\xe2\xc8\xb8\xbf\xa1...",
					false);
				break;
			default:
				dlg->SetMessage("\xbf\xc0\xb7\xf9", false);
				break;
			}
		}
	}
	m_resultTime = 0.0f;
	m_bShowResult = true;
}

void FrScratchDlg::ProcessResultText(float delta)
{
	if (m_result != 0 && m_bShowResult)
	{
		m_resultTime += delta;
		if (m_resultTime > 2.0f)
		{
			m_resultTime = 0.0f;
			m_bShowResult = false;
			EnableScratchSerialNumber(true);
			EnableNextPageButton(false);
			if (!m_bEnableRequestScratchItem)
			{
				ClearDataCheckScratchItem();
				ClearScratchItem();
				SetBmpColsedScratchCover();
				m_bEnableRequestScratchItem = true;
			}
		}
	}
}

void FrScratchDlg::ClearDataCheckScratchItem()
{
	memset(m_bCheckedRect, 0, sizeof(m_bCheckedRect));
	memset(m_rtCheck, 0, sizeof(m_rtCheck));
}

void FrScratchDlg::SetBmpScratchItem()
{
	if (m_scratchItemCount <= 0)
	{
		m_pImgItem[0] = g_pFresh->GetBitmap("scratch_text_nothing");
		return;
	}
	for (int i = 0; i < m_scratchItemCount; ++i)
	{
		if (m_scratchItem[i].typeId)
		{
			IFF_ITEM_COMMON* item =
				ItemManager()->FindCommonItem(m_scratchItem[i].typeId);
			if (item)
				m_pImgItem[i] = g_pFresh->GetBitmap(item->Icon);
			if ((m_scratchItem[i].typeId & 0xfc000000) == 0x24000000)
			{
				m_scratchItemCount = 1;
				break;
			}
		}
		else
		{
			m_pImgItem[i] = g_pFresh->GetBitmap("scratch_text_nothing");
			break;
		}
	}
}

void FrScratchDlg::SetRectScratchItem()
{
	if (m_pImgItem[0])
	{
		WPoint margin(12.0f, 12.0f);
		if (m_scratchItemCount < 2)
		{
			if ((m_scratchItem[0].typeId & 0xfc000000) != 0x18000000)
				margin = WPoint(0.0f, 0.0f);
			m_rtItem[2].x =
				(int)(m_rect.x + m_rect.w * 0.5f + g_ptScratchItem[2].x -
					(float)(m_pImgItem[0]->Width() / 2) + margin.x);
			m_rtItem[2].y =
				(int)(m_rect.y + m_rect.h * 0.5f + g_ptScratchItem[2].y -
					(float)(m_pImgItem[0]->Height() / 2) + margin.y);
			m_rtItem[2].w = (int)m_pImgItem[0]->Width();
			m_rtItem[2].h = (int)m_pImgItem[0]->Height();
		}
		else
		{
			m_rtItem[0].x =
				(int)(m_rect.x + m_rect.w * 0.5f + g_ptScratchItem[0].x -
					(float)(m_pImgItem[0]->Width() / 2) + margin.x);
			m_rtItem[0].y =
				(int)(m_rect.y + m_rect.h * 0.5f + g_ptScratchItem[0].y -
					(float)(m_pImgItem[0]->Height() / 2) + margin.y);
			m_rtItem[0].w = (int)m_pImgItem[0]->Width();
			m_rtItem[0].h = (int)m_pImgItem[0]->Height();
			m_rtItem[1].x =
				(int)(m_rect.x + m_rect.w * 0.5f + g_ptScratchItem[1].x -
					(float)(m_pImgItem[1]->Width() / 2) + margin.x);
			m_rtItem[1].y =
				(int)(m_rect.y + m_rect.h * 0.5f + g_ptScratchItem[1].y -
					(float)(m_pImgItem[1]->Height() / 2) + margin.y);
			m_rtItem[1].w = (int)m_pImgItem[1]->Width();
			m_rtItem[1].h = (int)m_pImgItem[1]->Height();
		}
	}
}

void FrScratchDlg::SetRectCheckScratchItemRect()
{
	if (m_pImgItem[0])
	{
		WPoint margin(24.0f, 24.0f);
		if (m_scratchItemCount < 2)
		{
			if ((m_scratchItem[0].typeId & 0xfc000000) != 0x18000000)
				margin = WPoint(0.0f, 0.0f);
			SetRectCheckScratchItemRectByIndex(2, margin);
		}
		else
		{
			SetRectCheckScratchItemRectByIndex(0, margin);
			SetRectCheckScratchItemRectByIndex(1, margin);
		}
	}
}

void FrScratchDlg::SetRectCheckScratchItemRectByIndex(int index, WPoint margin)
{
	m_rtCheck[index][0].w = (int)((m_rtItem[index].w - margin.x) * 0.25f);
	m_rtCheck[index][0].h = (int)((m_rtItem[index].h - margin.y) * 0.25f);
	for (int i = 1; i < 4; ++i)
	{
		m_rtCheck[index][i].w = m_rtCheck[index][0].w;
		m_rtCheck[index][i].h = m_rtCheck[index][0].h;
	}
	m_rtCheck[index][0].x = (int)((float)(GetRect().x + GetRect().w * 0.5f) +
		g_ptScratchItem[index].x - m_rtCheck[index][0].w);
	m_rtCheck[index][0].y = (int)((float)(GetRect().y + GetRect().h * 0.5f) +
		g_ptScratchItem[index].y - m_rtCheck[index][0].h);
	m_rtCheck[index][1].x = (int)((float)(GetRect().x + GetRect().w * 0.5f) +
		g_ptScratchItem[index].x);
	m_rtCheck[index][1].y = (int)((float)(GetRect().y + GetRect().h * 0.5f) +
		g_ptScratchItem[index].y - m_rtCheck[index][1].h);
	m_rtCheck[index][2].x = (int)((float)(GetRect().x + GetRect().w * 0.5f) +
		g_ptScratchItem[index].x - m_rtCheck[index][2].w);
	m_rtCheck[index][2].y = (int)((float)(GetRect().y + GetRect().h * 0.5f) +
		g_ptScratchItem[index].y);
	m_rtCheck[index][3].x = (int)((float)(GetRect().x + GetRect().w * 0.5f) +
		g_ptScratchItem[index].x);
	m_rtCheck[index][3].y = (int)((float)(GetRect().y + GetRect().h * 0.5f) +
		g_ptScratchItem[index].y);
}

void FrScratchDlg::CheckScratchItemRect(const WPoint& pt)
{
	const WPoint& point = pt;
	if (m_bScratchable)
	{
		if (m_scratchItemCount < 2)
		{
			for (int i = 0; i < 4; ++i)
				if (m_rtCheck[2][i].IsInRect(point))
				{
					m_bCheckedRect[0][i] = true;
					break;
				}
		}
		else
		{
			for (int i = 0; i < m_scratchItemCount; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (m_rtCheck[i][j].IsInRect(point))
					{
						m_bCheckedRect[i][j] = true;
						break;
					}
				}
			}
		}
	}
}

void FrScratchDlg::ProcessCheckScratchItemRect(float delta)
{
	if (m_bScratchable && !m_bShowResult)
	{
		int count = 0;
		if (m_scratchItemCount < 2)
		{
			for (int i = 0; i < 4; ++i)
				if (m_bCheckedRect[0][i])
					++count;
			if (count < 3)
				return;
		}
		else
		{
			for (int i = 0; i < m_scratchItemCount; ++i)
				for (int j = 0; j < 4; ++j)
					if (m_bCheckedRect[i][j])
						++count;
			if (count < m_scratchItemCount * 3)
				return;
		}
		{
			m_bScratchable = false;
			if (!m_scratchItem[0].typeId)
			{
				SetBmpResultText(5);
			}
			else
			{
				SetBmpResultText(4);
				OnLButtonUp(WPoint(0.0f, 0.0f));
			}
		}
	}
}

void FrScratchDlg::ProcessChangeScratchCardNumber(float delta)
{
	float alpha = delta * 480.0f;
	for (int i = 0; i < 2; ++i)
	{
		m_changeNumberAlpha[i] -= (int)alpha;
		m_changeNumberPos[i] -= delta * 20.0f;
	}
}

void FrScratchDlg::InitChangeScratchCardNumber(int type)
{
	if (type >= 0 && type < 2)
	{
		int index = 0;
		if (m_changeNumberPos[0] > m_changeNumberPos[1])
			index = 1;
		m_changeNumberType[index] = type;
		m_changeNumberPos[index] = 0.0f;
		m_changeNumberAlpha[index] = 512;
	}
}

void FrScratchDlg::DrawScratchItem()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (graphic && m_pImgItem[0])
	{
		if (m_scratchItemCount < 2)
		{
			graphic->DrawTexture(m_pImgItem[0], m_rtItem[2], -1, 0);
			if (m_scratchItem[0].count > 1)
			{
				unsigned int n = 0;
				const char* text = MakeStr("%d", m_scratchItem[0].count);
				float x = m_rtItem[2].x + 40.0f;
				for (n = 0; n < strlen(text); ++n)
					x -= m_pFont->GetCharWidth(g_view, text[n] - '0');
				m_pFont->Print(g_view, x, m_rtItem[2].y + 30.0f, text, 0, -1,
					NULL);
			}
		}
		else
		{
			for (int i = 0; i < 2; ++i)
			{
				if (!m_pImgItem[i])
					continue;
				graphic->DrawTexture(m_pImgItem[i], m_rtItem[i], -1, 0);
				if (m_scratchItem[i].count > 1)
				{
					unsigned int n = 0;
					const char* text = MakeStr("%d", m_scratchItem[i].count);
					float x = m_rtItem[i].x + 40.0f;
					for (n = 0; n < strlen(text); ++n)
						x -= m_pFont->GetCharWidth(g_view, text[n] - '0');
					m_pFont->Print(g_view, x, m_rtItem[i].y + 30.0f, text, 0,
						-1, NULL);
				}
			}
		}
	}
}

void FrScratchDlg::DrawChangeScratchCardNumber()
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	for (int i = 0; i < 2; ++i)
	{
		if (m_changeNumberType[i] < 0 ||
			!m_pImgChangeNumber[m_changeNumberType[i]])
			continue;
		unsigned char alpha;
		if (m_changeNumberAlpha[i] < 0)
		{
			alpha = 0;
			m_changeNumberType[i] = -1;
		}
		else if (m_changeNumberAlpha[i] > 255)
		{
			alpha = 255;
		}
		else
		{
			alpha = (unsigned char)m_changeNumberAlpha[i];
		}
		WRect rect(m_rect.x + 348.0f, m_rect.y + m_changeNumberPos[i] + 113.0f,
			m_pImgChangeNumber[m_changeNumberType[i]]->Width(),
			m_pImgChangeNumber[m_changeNumberType[i]]->Height());
		graphic->DrawTexture(m_pImgChangeNumber[m_changeNumberType[i]], rect,
			(alpha << 24) | 0xffffff, 0);
	}
}

void FrScratchDlg::OpenScratchResultDlg()
{
	if (!m_scratchItem[0].typeId)
		return;
	FrScratchResultDlg* dlg = CreateForm<FrScratchResultDlg>(
		g_pFresh->GetManager(), this, "scratchresultdlg", NULL);
	if (!dlg)
		return;
	dlg->Open((FRESH_PFN_RESULT)&FrScratchDlg::OnConfirmResult, 0xc0);
	dlg->SetFadeout(false);
	dlg->SetResultItem(m_scratchItem, m_scratchItemCount);
}

bool FrScratchDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == FrOK || result == FrCANCEL)
	{
		FrScratchDlg* dlg =
			(FrScratchDlg*)g_pFresh->GetDesktop()->FindChildByName(
				"scratchdlg");
		// HACK!
		dlg = dlg ? dlg : DYNAMIC_CAST(FrScratchDlg, dlg);
		if (dlg)
		{
			dlg->EnableScratchSerialNumber(true);
			dlg->EnableNextPageButton(false);
			dlg->ClearDataCheckScratchItem();
			dlg->ClearScratchItem();
			dlg->SetBmpColsedScratchCover();
			dlg->SetEnableRequestScratchItem(true);
		}
	}
	return true;
}

void FrScratchDlg::UpdateListMyItem(sBonusBall* ball)
{
	if (!ball)
		return;
	bool add = true;
	std::list<sItemInfo>* list = NULL;
	switch (ball->typeId >> 26)
	{
	case PIG_PART:
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
		m_scratchItemCount = 1;
		return;
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
	{
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
	}
	for (std::map<unsigned int, sCaddieInfo>::iterator it =
			 Doc()->m_caddieMap.begin();
		it != Doc()->m_caddieMap.end(); it++)
	{
		if (it->second.tid == ball->typeId)
		{
			add = false;
			break;
		}
	}
	for (std::map<unsigned int, sItemInfo>::iterator it =
			 Doc()->m_clubSetMap.begin();
		it != Doc()->m_clubSetMap.end(); it++)
		if (it->second.guid == ball->id)
			return;
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
		else if ((typeId >> 26) == PIG_FURNITURE)
		{
			unsigned long id = ball->id;
			IFF_STRUCT::sFurniture* item = ItemManager()->FindFurniture(typeId);
			sFurniture_List furniture;
			if (item)
			{
				memset(&furniture, 0, sizeof(furniture));
				furniture.id = id;
				furniture.typeId = typeId;
				furniture.x = item->Pos[0];
				furniture.y = item->Pos[1];
				furniture.z = item->Pos[2];
				furniture.r = item->Pos[3];
				furniture.bArrange = 0;
				Doc()->AddFurniture(furniture);
			}
		}
		else
		{
			sItemInfo info;
			short common[5] = { 0 };
			common[0] = 1;
			if (ball->timeType == 0)
			{
				common[0] = (short)ball->count;
			}
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
				if (info.ItemType == 3)
					info.HourRemain = ball->time;
				else if (info.ItemType == 4)
					info.HourRemain = ball->time * 24;
				else if (info.ItemType == 5)
					info.HourRemain = ball->time * 720;
			}
			unsigned long group = ball->typeId >> 26;
			// HACK: most likely should not be necessary.
			__assume(group < 64);
			switch (group)
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
}

IMPLEMENT_OBJECT(FrScratchResultDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrScratchResultDlg, FrForm)
ON_FRESH_VI("scratchresultdlg", FRCMD_INIT,
	FrScratchResultDlg::OnScratchResultDlgInit)
ON_FRESH_VI("resultitem_icon", FRCMD_INIT,
	FrScratchResultDlg::OnResultItemIconInit)
ON_FRESH_VV("resultitem_icon", FRCMD_OWNERDRAW,
	FrScratchResultDlg::OnResultItemIconOwnerDraw)
ON_FRESH_VI("resultitem_name", FRCMD_INIT,
	FrScratchResultDlg::OnResultItemNameInit)
ON_FRESH_VI("resultitem_ability", FRCMD_INIT,
	FrScratchResultDlg::OnResultItemAbilityInit)
END_FRESH_MSGMAP()

FrScratchResultDlg::FrScratchResultDlg()
{
	m_pResultItemIcon = NULL;
	m_pResultItemName = NULL;
	m_pResultItemAbility = NULL;
	static char* fontName = "[font_wind.jpg";
	tagWTITLEFONT fontInfo;
	memset(&fontInfo, 0, sizeof(fontInfo));
	fontInfo.filename = &fontName;
	fontInfo.fontw = 16;
	fontInfo.fonth = 16;
	fontInfo.numPages = 1;
	fontInfo.texw = 128;
	fontInfo.texh = 64;
	fontInfo.pCharSet = "1234567890ym%-.?/:";
	m_pFont = g_resrcmng->GetTitleFont();
	m_pFont->Create(&fontInfo);
	memset(m_resultItem, 0, sizeof(m_resultItem));
	m_resultItemCount = 0;
	m_bShowAbility = false;
	m_pImgItem[0] = NULL;
	m_pImgItem[1] = NULL;
}

FrScratchResultDlg::~FrScratchResultDlg()
{
	if (m_pFont)
		m_pFont->Erase();
	if (g_resrcmng && m_pFont)
	{
		g_resrcmng->Release(m_pFont);
		m_pFont = NULL;
	}
}

void FrScratchResultDlg::OnScratchResultDlgInit(int param)
{
}

void FrScratchResultDlg::OnResultItemIconInit(int param)
{
	m_pResultItemIcon = DYNAMIC_CAST(FrArea, param);
}

void FrScratchResultDlg::OnResultItemIconOwnerDraw()
{
	if (!m_pImgItem[0])
		return;
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	if (m_resultItemCount < 2)
	{
		WRect rect(m_pResultItemIcon->GetRect().x,
			m_pResultItemIcon->GetRect().y, m_pImgItem[0]->Width(),
			m_pImgItem[0]->Height());
		if (m_bShowAbility)
		{
			unsigned long group = m_resultItem[0].typeId >> 26;
			if (group == 2 || (group > 6 && group <= 8))
			{
				rect.x += 30.0f;
				rect.y += 24.0f;
			}
			else
			{
				rect.x = m_rect.x + (m_rect.w - m_pImgItem[0]->Width()) * 0.5f;
				rect.y = m_rect.y + 60.0f;
			}
		}
		else
		{
			rect.x =
				m_rect.x + (m_rect.w - m_pImgItem[0]->Width()) * 0.5f + 9.0f;
			rect.y = m_rect.y + 60.0f;
		}
		graphic->DrawTexture(m_pImgItem[0], rect, -1, 0);
		if (m_resultItem[0].count > 1)
		{
			unsigned int n = 0;
			const char* text = MakeStr("%d", m_resultItem[0].count);
			float x = rect.x + 40.0f;
			for (n = 0; n < strlen(text); ++n)
				x -= m_pFont->GetCharWidth(g_view, text[n] - '0');
			m_pFont->Print(g_view, x, rect.y + 30.0f, text, 0, -1, NULL);
		}
	}
	else
	{
		WRect rect(m_rect.x + (m_rect.w - m_pImgItem[0]->Width()) * 0.5f,
			m_rect.y + 60.0f, m_pImgItem[0]->Width(), m_pImgItem[0]->Height());
		for (int i = 0; i < 2; ++i)
		{
			if (!m_pImgItem[i])
				continue;
			WRect itemRect = rect;
			itemRect.x = rect.x - 26.0f + i * 70.0f;
			graphic->DrawTexture(m_pImgItem[i], itemRect, -1, 0);
			if (m_resultItem[i].count > 1)
			{
				unsigned int n = 0;
				const char* text = MakeStr("%d", m_resultItem[i].count);
				float x = rect.x + 40.0f - 26.0f + i * 70.0f;
				for (n = 0; n < strlen(text); ++n)
					x -= m_pFont->GetCharWidth(g_view, text[n] - '0');
				m_pFont->Print(g_view, x, rect.y + 30.0f, text, 0, -1, NULL);
			}
		}
	}
}

void FrScratchResultDlg::OnResultItemNameInit(int param)
{
	m_pResultItemName = DYNAMIC_CAST(FrEdit, param);
}

void FrScratchResultDlg::OnResultItemAbilityInit(int param)
{
	m_pResultItemAbility = DYNAMIC_CAST(FrEdit, param);
}

bool FrScratchResultDlg::OnInit()
{
	return true;
}

void FrScratchResultDlg::OnProc(const float delta)
{
}

void FrScratchResultDlg::SetResultItem(sBonusBall* balls, int count)
{
	m_pResultItemName->ClearLine();
	m_pResultItemAbility->ClearLine();
	m_pImgItem[0] = NULL;
	m_pImgItem[1] = NULL;
	if (!balls || count <= 0)
	{
		memset(m_resultItem, 0, sizeof(m_resultItem));
		m_resultItemCount = 0;
		return;
	}
	memcpy(m_resultItem, balls, sizeof(m_resultItem));
	m_resultItemCount = count;
	IFF_ITEM_COMMON* common = NULL;
	for (int i = 0; i < m_resultItemCount; ++i)
	{
		if (m_resultItem[i].typeId)
		{
			common = ItemManager()->FindCommonItem(m_resultItem[i].typeId);
			if (common)
				m_pImgItem[i] =
					g_pFresh->GetManager()->GetBitmap("ITEMS", common->Icon);
		}
	}
	unsigned long group = m_resultItem[0].typeId >> 26;
	if (group < PIG_BALL || group > PIG_ITEM)
	{
		m_bShowAbility = true;
		if (m_pResultItemName && common)
		{
			WRect rect;
			rect = WRect(m_rect.x + 10.0f, m_rect.y + 35.0f, 240.0f, 80.0f);
			m_pResultItemName->SetRect(rect);
			m_pResultItemName->AddText(
				MakeStr("\\cT\\c%s\\cN\\c", common->Name), false, true);
			short attributes[5];
			short slots[5] = { 0 };
			switch (m_resultItem[0].typeId >> 26)
			{
			case PIG_PART:
			{
				IFF_STRUCT::sPart* item =
					ItemManager()->FindPart(m_resultItem[0].typeId);
				if (!item)
					return;
				memcpy(attributes, item->Attr, sizeof(attributes));
				memcpy(slots, item->Slot, sizeof(slots));
			}
			break;
			case PIG_CADITEM:
			{
				IFF_STRUCT::sCadItem* item =
					ItemManager()->FindCadItem(m_resultItem[0].typeId);
				if (!item)
					return;
				memcpy(attributes, item->COM, sizeof(attributes));
			}
			break;
			case PIG_CADDIE:
			{
				IFF_STRUCT::sCaddie* item =
					ItemManager()->FindCaddie(m_resultItem[0].typeId);
				if (!item)
					return;
				memcpy(attributes, item->Attr, sizeof(attributes));
			}
			break;
			default:
				return;
			}
			const char* name = "";
			for (int i = 0, remaining = 5; remaining; ++i, --remaining)
			{
				if (attributes[i] || slots[i])
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
					if (attributes[i] > 0 || slots[i] > 0)
					{
						if (attributes[i] == 0)
							m_pResultItemAbility->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									name, slots[i]),
								false, true);
						else if (slots[i] == 0)
							m_pResultItemAbility->AddText(
								MakeStr("\\c0xff0000ff\\c%s%+d\\c0xff000000\\c",
									name, attributes[i]),
								false, true);
						else
							m_pResultItemAbility->AddText(
								MakeStr(
									"\\c0xff0000ff\\c%s%+d/\xbd\xbd\xb7\xd4%+d\\c0xff000000\\c",
									name, attributes[i], slots[i]),
								false, true);
					}
				}
			}
		}
	}
}

IMPLEMENT_OBJECT(FrScratchAboutDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrScratchAboutDlg, FrForm)
ON_FRESH_VI("aboutviewer", FRCMD_INIT, FrScratchAboutDlg::OnAboutViewerInit)
ON_FRESH_VI("index01", FRCMD_INIT, FrScratchAboutDlg::OnIndex01Init)
ON_FRESH_VV("index01", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnIndex01BnUp)
ON_FRESH_VI("index02", FRCMD_INIT, FrScratchAboutDlg::OnIndex02Init)
ON_FRESH_VV("index02", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnIndex02BnUp)
ON_FRESH_VI("index03", FRCMD_INIT, FrScratchAboutDlg::OnIndex03Init)
ON_FRESH_VV("index03", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnIndex03BnUp)
ON_FRESH_VI("index04", FRCMD_INIT, FrScratchAboutDlg::OnIndex04Init)
ON_FRESH_VV("index04", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnIndex04BnUp)
ON_FRESH_VI("index05", FRCMD_INIT, FrScratchAboutDlg::OnIndex05Init)
ON_FRESH_VV("index05", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnIndex05BnUp)
ON_FRESH_VI("backward", FRCMD_INIT, FrScratchAboutDlg::OnBackwardBnInit)
ON_FRESH_VV("backward", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnBackwardBnUp)
ON_FRESH_VI("forward", FRCMD_INIT, FrScratchAboutDlg::OnForwardBnInit)
ON_FRESH_VV("forward", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnForwardBnUp)
ON_FRESH_VI("ok", FRCMD_INIT, FrScratchAboutDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrScratchAboutDlg::OnOkBnUp)
END_FRESH_MSGMAP()

FrScratchAboutDlg::FrScratchAboutDlg()
{
	for (int i = 0; i < 5; ++i)
		m_pIndexBn[i] = NULL;
	m_aboutIndex = 0;
	m_pBackwardBn = NULL;
	m_pForwardBn = NULL;
}

FrScratchAboutDlg::~FrScratchAboutDlg()
{
}

void FrScratchAboutDlg::OnAboutViewerInit(int param)
{
	m_pAboutViewer = DYNAMIC_CAST(FrViewer, param);
}

void FrScratchAboutDlg::OnIndex01Init(int param)
{
	m_pIndexBn[0] = DYNAMIC_CAST(FrTextButton, param);
}

void FrScratchAboutDlg::OnIndex01BnUp()
{
	SetImgAboutIndex(0);
}

void FrScratchAboutDlg::OnIndex02Init(int param)
{
	m_pIndexBn[1] = DYNAMIC_CAST(FrTextButton, param);
}

void FrScratchAboutDlg::OnIndex02BnUp()
{
	SetImgAboutIndex(1);
}

void FrScratchAboutDlg::OnIndex03Init(int param)
{
	m_pIndexBn[2] = DYNAMIC_CAST(FrTextButton, param);
}

void FrScratchAboutDlg::OnIndex03BnUp()
{
	SetImgAboutIndex(2);
}

void FrScratchAboutDlg::OnIndex04Init(int param)
{
	m_pIndexBn[3] = DYNAMIC_CAST(FrTextButton, param);
}

void FrScratchAboutDlg::OnIndex04BnUp()
{
	SetImgAboutIndex(3);
}

void FrScratchAboutDlg::OnIndex05Init(int param)
{
	m_pIndexBn[4] = DYNAMIC_CAST(FrTextButton, param);
}

void FrScratchAboutDlg::OnIndex05BnUp()
{
	SetImgAboutIndex(4);
}

void FrScratchAboutDlg::OnBackwardBnInit(int param)
{
	m_pBackwardBn = DYNAMIC_CAST(FrButton, param);
}

void FrScratchAboutDlg::OnBackwardBnUp()
{
	--m_aboutIndex;
	SetImgAboutIndex(m_aboutIndex);
	m_pBackwardBn->SetPushDelay(0.0f);
}

void FrScratchAboutDlg::OnForwardBnInit(int param)
{
	m_pForwardBn = DYNAMIC_CAST(FrButton, param);
}

void FrScratchAboutDlg::OnForwardBnUp()
{
	++m_aboutIndex;
	SetImgAboutIndex(m_aboutIndex);
	m_pForwardBn->SetPushDelay(0.0f);
}

void FrScratchAboutDlg::OnOkBnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, param);
}

void FrScratchAboutDlg::OnOkBnUp()
{
	OnFreshOkay();
}

bool FrScratchAboutDlg::OnInit()
{
	SetImgAboutIndex(0);
	return true;
}

void FrScratchAboutDlg::SetImgAboutIndex(int index)
{
	for (int i = 0; i < 5; ++i)
		m_pIndexBn[i]->Unselect(0);
	if (m_pBackwardBn)
		m_pBackwardBn->Enable(true);
	if (m_pForwardBn)
		m_pForwardBn->Enable(true);
	if (index <= 0)
	{
		index = 0;
		m_pIndexBn[0]->Select(0);
		if (m_pBackwardBn)
			m_pBackwardBn->Enable(false);
	}
	else if (index >= 4)
	{
		index = 4;
		m_pIndexBn[4]->Select(0);
		if (m_pForwardBn)
			m_pForwardBn->Enable(false);
	}
	m_pIndexBn[index]->Select(0);
	m_aboutIndex = index;
	if (m_pAboutViewer)
		m_pAboutViewer->Open(MakeStr("help_scratch_%02d.jpg", index + 1));
}
