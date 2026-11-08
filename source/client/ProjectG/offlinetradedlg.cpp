#include "offlinetradedlg.h"
#include "standinginline.h"
#include "binstr.h"
#include "wlocalize.h"
#include "frcombobox.h"
#include "../../shared/localize.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrOfflineTradeFinishEditDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrOfflineTradeFinishEditDlg, FrForm)
ON_FRESH_VI("tradefinisheditdlg", FRCMD_INIT,
	FrOfflineTradeFinishEditDlg::OnTradeFinishEditDlgInit)
ON_FRESH_VI("trade_item_list", FRCMD_INIT,
	FrOfflineTradeFinishEditDlg::OnTradeItemListInit)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	FrOfflineTradeFinishEditDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONUP,
	FrOfflineTradeFinishEditDlg::OnTradeItemListLBtnUp)
ON_FRESH_VI("userid", FRCMD_INIT, FrOfflineTradeFinishEditDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, FrOfflineTradeFinishEditDlg::OnTitleInit)
ON_FRESH_VI("ok", FRCMD_INIT, FrOfflineTradeFinishEditDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrOfflineTradeFinishEditDlg::OnOkBnUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, FrOfflineTradeFinishEditDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW,
	FrOfflineTradeFinishEditDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

FrOfflineTradeFinishEditDlg::FrOfflineTradeFinishEditDlg()
{
	m_pSelectItem = 0;
	m_extendBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_n");
	m_extendBtn[0].rect =
		WRect(96.0f, 66.0f, (float)m_extendBtn[0].pBitmap->Width(),
			(float)m_extendBtn[0].pBitmap->Height());
	m_extendBtn[0].index = -1;
	m_extendBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_o");
	m_extendBtn[1].rect =
		WRect(96.0f, 66.0f, (float)m_extendBtn[1].pBitmap->Width(),
			(float)m_extendBtn[1].pBitmap->Height());
	m_extendBtn[1].index = -1;
	m_returnBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_return_n");
	m_returnBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_returnBtn[0].pBitmap->Width(),
			(float)m_returnBtn[0].pBitmap->Height());
	m_returnBtn[0].index = -1;
	m_returnBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_return_o");
	m_returnBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_returnBtn[1].pBitmap->Width(),
			(float)m_returnBtn[1].pBitmap->Height());
	m_returnBtn[1].index = -1;
	m_deleteBtn[0].pBitmap = g_pFresh->GetBitmap("delete_btn_n");
	m_deleteBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_deleteBtn[0].pBitmap->Width(),
			(float)m_deleteBtn[0].pBitmap->Height());
	m_deleteBtn[0].index = -1;
	m_deleteBtn[1].pBitmap = g_pFresh->GetBitmap("delete_btn_o");
	m_deleteBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_deleteBtn[1].pBitmap->Width(),
			(float)m_deleteBtn[1].pBitmap->Height());
	m_deleteBtn[1].index = -1;
	m_buyBtn[0].pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtn[0].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[0].pBitmap->Width(),
		(float)m_buyBtn[0].pBitmap->Height());
	m_buyBtn[0].index = -1;
	m_buyBtn[1].pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtn[1].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[1].pBitmap->Width(),
		(float)m_buyBtn[1].pBitmap->Height());
	m_buyBtn[1].index = -1;
	m_deleteMark.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteMark.rect =
		WRect(144.0f, 35.0f, (float)m_deleteMark.pBitmap->Width(),
			(float)m_deleteMark.pBitmap->Height());
	m_tradeState[0].pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState[0].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[0].pBitmap->Width(),
			(float)m_tradeState[0].pBitmap->Height());
	m_tradeState[1].pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState[1].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[1].pBitmap->Width(),
			(float)m_tradeState[1].pBitmap->Height());
	m_tradeState[2].pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState[2].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[2].pBitmap->Width(),
			(float)m_tradeState[2].pBitmap->Height());
	m_uploadBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_item_upload_n");
	m_uploadBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_uploadBtn[0].pBitmap->Width(),
			(float)m_uploadBtn[0].pBitmap->Height());
	m_uploadBtn[0].index = -1;
	m_uploadBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_item_upload_o");
	m_uploadBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_uploadBtn[1].pBitmap->Width(),
			(float)m_uploadBtn[1].pBitmap->Height());
	m_uploadBtn[1].index = -1;
}

FrOfflineTradeFinishEditDlg::~FrOfflineTradeFinishEditDlg()
{
}

void FrOfflineTradeFinishEditDlg::OnTradeItemListOwnerDraw(int param)
{
	FrTradeFinishEditDlg::OnTradeItemListOwnerDraw(param);
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	FrListItem* draw = (FrListItem*)param;
	if (!draw || !draw->pData)
		return;
	const Bitmap* bitmap = g_pFresh->GetBitmap("rmr_item_slot");
	sStallSlot* slot = (sStallSlot*)draw->pData;
	graphic->SetTextStyle(0);
	graphic->SetTextColor(0xff000000, 0xffffffff);
	if (slot->IsValid == 0)
	{
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(draw->pos.x, draw->pos.y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);

		{
			WPoint pos = WPoint(182, 7) + draw->pos;
			graphic->Print(WPoint(pos), 2,
				"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5");
		}
		if (draw->underCursor && (m_buyBtn[0].rect + draw->pos).IsInRect(mouse))
		{
			graphic->DrawTexture(m_buyBtn[1].pBitmap,
				m_buyBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)22);
		}
		else
		{
			graphic->DrawTexture(m_buyBtn[0].pBitmap,
				m_buyBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
		}
	}
	else
	{
		__int64 hours = 0;
		FILETIME end, now;
		if (!slot->IsPermanence)
		{
			SystemTimeToFileTime(&slot->time, &end);
			SystemTimeToFileTime(&Doc()->GetServerTime(), &now);
			__int64 remaining =
				((__int64)end.dwHighDateTime - now.dwHighDateTime) *
					0x100000000i64 -
				now.dwLowDateTime + end.dwLowDateTime;
			hours = remaining / 60 / 60 / 10000000;
			graphic->DrawTexture(m_tradeState[2].pBitmap,
				m_tradeState[2].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			if (hours >= 0 && remaining >= 0)
			{
				graphic->SetTextColor(0xffffffff, 0xffffffff);
				if (hours / 24 >= 1)
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2, "%d\xc0\xcf",
						hours / 24 + 1);
				}
				else
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2, "%d\xbd\xc3", hours);
				}
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}
			else
			{
				graphic->SetTextColor(0xffffffff, 0xffffffff);
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2,
						"\xb1\xe2\xb0\xa3\xb8\xb8\xb7\xe1");
				}
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}

			if (draw->underCursor &&
				(m_extendBtn[0].rect + draw->pos).IsInRect(mouse))
			{
				graphic->DrawTexture(m_extendBtn[1].pBitmap,
					m_extendBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)67);
			}
			else
			{
				graphic->DrawTexture(m_extendBtn[0].pBitmap,
					m_extendBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
			}
		}
		if (!slot->itemData.dwTid)
		{
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(draw->pos.x, draw->pos.y, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);

			{
				WPoint pos = WPoint(182, 7) + draw->pos;
				graphic->Print(WPoint(pos), 2,
					"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
			}
			if (draw->underCursor &&
				(m_uploadBtn[0].rect + draw->pos).IsInRect(mouse))
			{
				graphic->DrawTexture(m_uploadBtn[1].pBitmap,
					m_uploadBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)68);
			}
			else
			{
				graphic->DrawTexture(m_uploadBtn[0].pBitmap,
					m_uploadBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
			}
		}
		else
		{
			if (!slot->IsPermanence)
			{
				if (hours >= 0)
				{
					if (draw->underCursor &&
						(m_deleteBtn[0].rect + draw->pos).IsInRect(mouse))
					{
						graphic->DrawTexture(m_deleteBtn[1].pBitmap,
							m_deleteBtn[1].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip((eBarButton)66);
					}
					else
					{
						graphic->DrawTexture(m_deleteBtn[0].pBitmap,
							m_deleteBtn[0].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
					}
				}
				else
				{
					if (draw->underCursor &&
						(m_returnBtn[0].rect + draw->pos).IsInRect(mouse))
					{
						graphic->DrawTexture(m_returnBtn[1].pBitmap,
							m_returnBtn[1].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip((eBarButton)69);
					}
					else
					{
						graphic->DrawTexture(m_returnBtn[0].pBitmap,
							m_returnBtn[0].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
					}
				}
			}
			else
			{
				if (draw->underCursor &&
					(m_deleteBtn[0].rect + draw->pos).IsInRect(mouse))
				{
					graphic->DrawTexture(m_deleteBtn[1].pBitmap,
						m_deleteBtn[1].rect + draw->pos + WPoint(0, 0),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip((eBarButton)66);
				}
				else
				{
					graphic->DrawTexture(m_deleteBtn[0].pBitmap,
						m_deleteBtn[0].rect + draw->pos + WPoint(0, 0),
						0xffffffff, 0);
				}
			}
		}
	}
}

void FrOfflineTradeFinishEditDlg::OnTradeItemListLBtnUp()
{
	FrListItem* item = m_pTradeItemList->GetItemUnderCursor();
	if (!item)
		return;
	sStallSlot* slot = (sStallSlot*)item->pData;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	if (slot->IsValid == 0 &&
		((m_buyBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_buyBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		CBuyingSlotDlg* dlg = CreateForm<CBuyingSlotDlg>(g_pFresh->GetManager(),
			this, "buyslotdlg");
		dlg->SetSlotNum(item->idx + 1);
		dlg->Open(NULL, FrESCAPE);
		return;
	}
	if (slot->IsValid == 1 && slot->itemData.dwTid == 0 &&
		((m_uploadBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_uploadBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		m_pSelectItem = item;
		FrForm::OnOK();
		return;
	}
	if (slot->IsValid == 1 &&
		((m_extendBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_extendBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		CBuyingSlotDlg* dlg = CreateForm<CBuyingSlotDlg>(g_pFresh->GetManager(),
			this, "buyslotdlg");
		dlg->SetSlotNum(item->idx + 1);
		dlg->Open(NULL, FrESCAPE);
		return;
	}
}

void FrOfflineTradeFinishEditDlg::SetListTradeItem(std::list<sTradeItem>* list)
{
	sStall* stall = Doc()->GetStallItem();
	m_pTradeItemList->ClearItem();
	for (int i = 0; i < stall->maxSlotNum; ++i)
		m_pTradeItemList->AddItem(&stall->slotList[i]);
}

void FrOfflineTradeFinishEditDlg::GetSelectSlotInfo(int& uid, int& slot)
{
	if (!m_pSelectItem)
	{
		slot = 0;
		uid = 0;
		return;
	}
	sStallSlot* data = (sStallSlot*)m_pSelectItem->pData;
	uid = data->stallKey;
	slot = data->Idx;
}

void FrOfflineTradeFinishEditDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void FrOfflineTradeFinishEditDlg::OnToolTipOnwerDraw(int param)
{
	CIconToolTip::GetInstance()->Render();
}

IMPLEMENT_OBJECT(COfflineTradeDlg, FrForm)

BEGIN_FRESH_MSGMAP(COfflineTradeDlg, FrForm)
ON_FRESH_VI("AreaSub_Nick", FRCMD_INIT, COfflineTradeDlg::OnAreaSubNickInit)
ON_FRESH_VI("AreaSub_Nick", FRCMD_OWNERDRAW,
	COfflineTradeDlg::OnAreaSubNickOwnerDraw)
ON_FRESH_VV("AreaSub_Nick", FRCMD_LBUTTONUP,
	COfflineTradeDlg::OnAreaSubNickLBtnUP)
ON_FRESH_VI("AreaSub_Price", FRCMD_INIT, COfflineTradeDlg::OnAreaSubPriceInit)
ON_FRESH_VI("AreaSub_Price", FRCMD_OWNERDRAW,
	COfflineTradeDlg::OnAreaSubPriceOwnerDraw)
ON_FRESH_VV("AreaSub_Price", FRCMD_LBUTTONUP,
	COfflineTradeDlg::OnAreaSubPriceLBtnUP)
ON_FRESH_VI("AreaSub_ItemName", FRCMD_INIT,
	COfflineTradeDlg::OnAreaSubItemNameInit)
ON_FRESH_VI("AreaSub_ItemName", FRCMD_OWNERDRAW,
	COfflineTradeDlg::OnAreaSubItemNameOwnerDraw)
ON_FRESH_VV("AreaSub_ItemName", FRCMD_LBUTTONUP,
	COfflineTradeDlg::OnAreaSubItemNameLBtnUP)
ON_FRESH_VI("Edit_ItemSearch", FRCMD_INIT, COfflineTradeDlg::OnItemSearchInit)
ON_FRESH_BI("Edit_ItemSearch", FRCMD_ENTERKEY,
	COfflineTradeDlg::OnItemSearchEnterKey)
ON_FRESH_VV("Edit_ItemSearch", FRCMD_LBUTTONUP,
	COfflineTradeDlg::OnItemSearchLBtnUp)
ON_FRESH_VI("BtnMain_search", FRCMD_INIT, COfflineTradeDlg::OnBtnMainSearchInit)
ON_FRESH_VV("BtnMain_search", FRCMD_LBUTTONUP,
	COfflineTradeDlg::OnBtnMainSearchUp)
ON_FRESH_VI("num_maxpage", FRCMD_INIT, COfflineTradeDlg::OnEditMaxPageInit)
ON_FRESH_VI("num_curpage", FRCMD_INIT, COfflineTradeDlg::OnEditCurrentPageInit)
ON_FRESH_BI("num_curpage", FRCMD_ENTERKEY,
	COfflineTradeDlg::OnEditCurrentPageEnterKey)
ON_FRESH_VI("BtnPage_Prev", FRCMD_INIT, COfflineTradeDlg::OnBtnPrevInit)
ON_FRESH_VV("BtnPage_Prev", FRCMD_LBUTTONUP, COfflineTradeDlg::OnBtnPrevBtnUp)
ON_FRESH_VI("BtnPage_Next", FRCMD_INIT, COfflineTradeDlg::OnBtnNextInit)
ON_FRESH_VV("BtnPage_Next", FRCMD_LBUTTONUP, COfflineTradeDlg::OnBtnNextBtnUp)
ON_FRESH_VI("BtnPage_go", FRCMD_INIT, COfflineTradeDlg::OnBtnGoPageInit)
ON_FRESH_VV("BtnPage_go", FRCMD_LBUTTONUP, COfflineTradeDlg::OnBtnGoPageBtnUp)
ON_FRESH_VI("BtnRefresh", FRCMD_INIT, COfflineTradeDlg::OnBtnRefreshInit)
ON_FRESH_VV("BtnRefresh", FRCMD_LBUTTONUP, COfflineTradeDlg::OnBtnRefreshBtnUp)
ON_FRESH_VI("BtnSub_Quick", FRCMD_INIT, COfflineTradeDlg::OnBtnDetailInit)
ON_FRESH_VV("BtnSub_Quick", FRCMD_LBUTTONUP, COfflineTradeDlg::OnBtnDetailBtnUp)
ON_FRESH_VI("offlinetrade_list", FRCMD_INIT, COfflineTradeDlg::OnMaketListInit)
ON_FRESH_VI("offlinetrade_list", FRCMD_OWNERDRAW,
	COfflineTradeDlg::OnMaketListOwnerDraw)
ON_FRESH_VV("offlinetrade_list", FRCMD_LBUTTONDOWN,
	COfflineTradeDlg::OnMaketListBtnDown)
ON_FRESH_VI("Edit_Msg", FRCMD_INIT, COfflineTradeDlg::OnEditMsgInit)
ON_FRESH_VV("Edit_Msg", FRCMD_LBUTTONUP, COfflineTradeDlg::OnEditMsgLBtnUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, COfflineTradeDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, COfflineTradeDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

COfflineTradeDlg::COfflineTradeDlg()
{
	m_pAreaSubNick = NULL;
	m_pAreaSubPrice = NULL;
	m_pAreaSubItemName = NULL;
	m_pItemSearch = NULL;
	m_pMarketList = NULL;
	m_pCurPage = NULL;
	m_pMaxPage = NULL;
	m_pBtnPrev = NULL;
	m_pBtnNext = NULL;
	m_pBtnGoPage = NULL;
	m_pBtnMainSearch = NULL;
	m_pBtnRefresh = NULL;
	m_pStandingInLineDlg = NULL;
	m_pBtnDetail = NULL;
	m_pEditMsg = NULL;
	m_maxPage = 0;
	m_curPage = 0;
	m_searchType = 1;
	m_detailType = 0;
	m_searchString.clear();
	SetSearchString(
		"\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5");
	m_bReserved = false;
	m_bCanVisit = 0;
	m_moveBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_btn_move_n");
	m_moveBtn[0].rect = WRect(490.f, 1.f, (float)m_moveBtn[0].pBitmap->Width(),
		(float)m_moveBtn[0].pBitmap->Height());
	m_moveBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_btn_move_o");
	m_moveBtn[1].rect = WRect(490.f, 1.f, (float)m_moveBtn[1].pBitmap->Width(),
		(float)m_moveBtn[1].pBitmap->Height());
}

COfflineTradeDlg::~COfflineTradeDlg()
{
}

bool COfflineTradeDlg::OnInit()
{
	if (m_pItemSearch)
	{
		m_pItemSearch->SetFontColor(0xffd1d1d0);
		m_pItemSearch->ClearLine();
		m_pItemSearch->AddLine(
			"\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0,
			false);
		m_pItemSearch->SetKeyFocus(true);
	}
	int canVisit = 0;
	AfxGetTask()->SendMsgToMainActor(
		MsgObject(NULL, 0x231, (int)&canVisit, 2, 3, 0, 0));
	m_bCanVisit = canVisit;
	if (!m_bCanVisit)
	{
		m_pEditMsg->SetVisible(true);
		m_pEditMsg->AddText(
			" * \xc7\xf6\xc0\xe7 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xbb \xb9\xe6\xb9\xae \xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false, true);
	}
	else
		m_pEditMsg->SetVisible(false);
	return true;
}

void COfflineTradeDlg::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	CIconToolTip::GetInstance()->ClearToolTip();
}

void COfflineTradeDlg::OnAreaSubNickInit(int param)
{
	m_pAreaSubNick = DYNAMIC_CAST(FrViewer, param);
	if (m_pAreaSubNick)
	{
		WRect rect = m_pAreaSubNick->GetRect();
		m_pAreaSubNick->SetRect(WRect(rect.x, rect.y, rect.w + 55.0f, rect.h));
	}
}

void COfflineTradeDlg::OnAreaSubNickOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->SetTextColor(0xffffffff, 0xffffffff);
	WRect rect = m_pAreaSubNick->GetRect();
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		graphic->SetTextColor(0xffffffff, 0xff000000);
		graphic->SetTextStyle(2);
	}
	else
		graphic->SetTextStyle(0);
	graphic->Print(WPoint(rect.x + 10.0f, rect.y), 0,
		"\xb4\xd0\xb3\xd7\xc0\xd3");
}

void COfflineTradeDlg::OnAreaSubNickLBtnUP()
{
	m_searchType = 4;
	m_detailType = 0;
	m_curPage = 1;
	if (SetSearchString(""))
		RequestMaketList();
}

void COfflineTradeDlg::OnAreaSubPriceInit(int param)
{
	m_pAreaSubPrice = DYNAMIC_CAST(FrViewer, param);
	if (m_pAreaSubPrice)
	{
		WRect rect = m_pAreaSubPrice->GetRect();
		m_pAreaSubPrice->SetRect(WRect(rect.x, rect.y, rect.w + 35.0f, rect.h));
	}
}

void COfflineTradeDlg::OnAreaSubPriceOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->SetTextColor(0xffffffff, 0xffffffff);
	WRect rect = m_pAreaSubPrice->GetRect();
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		graphic->SetTextColor(0xffffffff, 0xff000000);
		graphic->SetTextStyle(2);
	}
	else
		graphic->SetTextStyle(0);
	graphic->Print(WPoint(rect.x + 10.0f, rect.y), 0, "\xb0\xa1\xb0\xdd");
}

void COfflineTradeDlg::OnAreaSubPriceLBtnUP()
{
	m_searchType = 3;
	m_detailType = 0;
	m_curPage = 1;
	if (SetSearchString(""))
		RequestMaketList();
}

void COfflineTradeDlg::OnAreaSubItemNameInit(int param)
{
	m_pAreaSubItemName = DYNAMIC_CAST(FrViewer, param);
	if (m_pAreaSubItemName)
	{
		WRect rect = m_pAreaSubItemName->GetRect();
		m_pAreaSubItemName->SetRect(
			WRect(rect.x, rect.y, rect.w + 75.0f, rect.h));
	}
}

void COfflineTradeDlg::OnAreaSubItemNameOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	graphic->SetTextColor(0xffffffff, 0xffffffff);
	WRect rect = m_pAreaSubItemName->GetRect();
	if (rect.IsInRect(g_pFresh->GetManager()->GetMousePos()))
	{
		graphic->SetTextColor(0xffffffff, 0xff000000);
		graphic->SetTextStyle(2);
	}
	else
		graphic->SetTextStyle(0);
	graphic->Print(WPoint(rect.x + 10.0f, rect.y), 0,
		"\xbe\xc6\xc0\xcc\xc5\xdb\xc0\xcc\xb8\xa7");
}

void COfflineTradeDlg::OnAreaSubItemNameLBtnUP()
{
	m_searchType = 1;
	m_detailType = 0;
	m_curPage = 1;
	if (SetSearchString(""))
		RequestMaketList();
}

void COfflineTradeDlg::OnMaketListInit(int param)
{
	m_pMarketList = DYNAMIC_CAST(FrListBox, param);
}

void COfflineTradeDlg::SetListItem()
{
	char text[32] = { 0 };
	std::list<sSaleItem>* list = Doc()->GetMaketList();
	if (m_pMarketList)
	{
		m_pMarketList->ClearItem();
		for (std::list<sSaleItem>::iterator it = list->begin();
			it != list->end(); it++)
			m_pMarketList->AddItem(&*it);
	}
}

void COfflineTradeDlg::OnMaketListOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	FrListItem* draw = (FrListItem*)param;
	if (!draw)
		return;
	graphic->SetTextColor(0xff000000, 0xffffffff);
	graphic->SetTextStyle(0);
	sSaleItem* item = (sSaleItem*)draw->pData;
	const Bitmap* bitmap = g_pFresh->GetBitmap("maketlist_select");
	if ((draw->selected || draw->underCursor) && bitmap)
		graphic->DrawTexture(bitmap,
			WRect(draw->pos.x, draw->pos.y - 5.f, 520.f, 23.f), 0xffffffff, 0);
	if (item->bOpen == 1)
		graphic->Print(WPoint(470.f, 1.f) + draw->pos, 2, "\xb0\xf8  \xb0\xb3");
	else
	{
		graphic->SetTextColor(0xffd1d1d0, 0xffffffff);
		graphic->Print(WPoint(470.f, 1.f) + draw->pos, 2,
			"\xba\xf1\xb0\xf8\xb0\xb3");
	}
	IFF_ITEM_COMMON* common = ItemManager()->FindCommonItem(item->typeId);
	if (common)
		graphic->Print(WPoint(160.f, 1.f) + draw->pos, 2, "%s", common->Name);
	graphic->Print(WPoint(250.f, 1.f) + draw->pos, 2, "%d", item->price);
	graphic->Print(WPoint(280.f, 1.f) + draw->pos, 2, "%d", item->count);
	graphic->Print(WPoint(400.f, 1.f) + draw->pos, 2, "%s", item->nickname);
	if (m_bCanVisit)
	{
		if (draw->underCursor &&
			(m_moveBtn[0].rect + draw->pos)
				.IsInRect(g_pFresh->GetManager()->GetMousePos()))
		{
			graphic->DrawTexture(m_moveBtn[1].pBitmap,
				m_moveBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)22);
		}
		else
		{
			WRect rect = m_moveBtn[0].rect + draw->pos;
			graphic->DrawTexture(m_moveBtn[0].pBitmap,
				WRect(rect.x, rect.y, rect.w, rect.h), 0xffffffff, 0);
		}
	}
	graphic->SetTextColor(0xff000000, 0xffffffff);
}

void COfflineTradeDlg::OnMaketListBtnDown()
{
	FrListItem* draw = m_pMarketList->GetItemUnderCursor();
	if (!draw)
		return;
	if (g_pFresh->IsCurrentLayout("REALMYROOM") ||
		g_pFresh->IsCurrentLayout("REALMYROOMVISITOR"))
		return;
	sSaleItem* item = (sSaleItem*)draw->pData;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if ((m_moveBtn[0].rect + draw->pos).IsInRect(mouse) ||
		(m_moveBtn[1].rect + draw->pos).IsInRect(mouse))
	{
		Doc()->m_replayTargetUID = item->ownerUID;
		if (Doc()->m_replayTargetUID == 0xffffffff)
		{
			FrForm* dlg =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			dlg->SetMessage(
				"\xb9\xe6\xb9\xae\xc7\xd2 \xc0\xaf\xc0\xfa\xc0\xc7 \xc1\xa4\xba\xb8\xb8\xa6 \xbe\xf2\xbe\xee\xbf\xc0\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.",
				false);
			dlg->Open(NULL, FrESCAPE | FrENTER);
			return;
		}
		if (MyUID() == Doc()->m_replayTargetUID)
		{
			Doc()->m_rmrMode = 1;
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x222, 0, 0, 0, 0, 0));
		}
		else
		{
			Doc()->m_rmrMode = 5;
			AfxGetTask()->SendMsgToMainActor(
				MsgObject(NULL, 0x222, 0, 0, 0, 0, 0));
		}
	}
}

void COfflineTradeDlg::OnItemSearchInit(int param)
{
	m_pItemSearch = DYNAMIC_CAST(FrEdit, param);
	if (m_pItemSearch)
	{
		m_pItemSearch->SetFontColor(0xffd1d1d0);
		m_pItemSearch->ClearLine();
		m_pItemSearch->AddLine(
			"\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0,
			false);
		m_pItemSearch->SetKeyFocus(true);
	}
}

bool COfflineTradeDlg::OnItemSearchEnterKey(int param)
{
	OnAreaSubItemNameLBtnUP();
	return true;
}

void COfflineTradeDlg::OnItemSearchLBtnUp()
{
	std::string text = m_pItemSearch->GetLine(1, false);
	std::string empty;
	empty.clear();
	if (!stricmp("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5",
			text.c_str()))
	{
		m_pItemSearch->SetFontColor(0xff000000);
		m_pItemSearch->ClearLine();
		m_pItemSearch->AddLine("", 0, false);
		m_pItemSearch->SetKeyFocus(true);
	}
}

void COfflineTradeDlg::OnBtnMainSearchInit(int param)
{
	m_pBtnMainSearch = DYNAMIC_CAST(FrButton, param);
}

void COfflineTradeDlg::OnBtnMainSearchUp()
{
	m_searchType = 1;
	m_detailType = 0;
	m_curPage = 1;
	if (SetSearchString(""))
		RequestMaketList();
}

void COfflineTradeDlg::OnEditMaxPageInit(int param)
{
	m_pMaxPage = DYNAMIC_CAST(FrEdit, param);
	if (m_pMaxPage)
		m_pMaxPage->AddLine(MakeStr("%d", m_maxPage), 0, false);
}

void COfflineTradeDlg::OnEditCurrentPageInit(int param)
{
	m_pCurPage = DYNAMIC_CAST(FrEdit, param);
	if (m_pCurPage)
		m_pCurPage->AddLine(MakeStr("%d", m_curPage), 0, false);
}

bool COfflineTradeDlg::OnEditCurrentPageEnterKey(int param)
{
	return true;
}

void COfflineTradeDlg::SetPageData(int curPage, int maxPage)
{
	if (curPage != -1)
	{
		m_curPage = curPage;
		m_pBtnDetail->SetVisible(false);
	}
	m_maxPage = maxPage;
	if (m_curPage < 1)
		m_curPage = Max(m_curPage, 1);
	if (m_curPage > maxPage)
		m_curPage = maxPage;
	if (m_pCurPage)
	{
		m_pCurPage->ClearLine();
		m_pCurPage->AddLine(MakeStr("%d", m_curPage), 0, false);
	}
	if (m_pMaxPage)
	{
		m_pMaxPage->ClearLine();
		m_pMaxPage->AddLine(MakeStr("%d", m_maxPage), 0, false);
	}
}

void COfflineTradeDlg::RequestMaketList()
{
	if (!stricmp("\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5",
			m_searchString.c_str()))
		m_searchString.clear();
	if (m_searchString.size())
	{
		if (IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet((enumClientPacket)0xc9);
			packet.Encode1((unsigned char)m_searchType);
			packet.Encode1((unsigned char)m_detailType);
			packet.EncodeStr(m_searchString);
			packet.Encode2((unsigned short)m_curPage);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			m_pBtnDetail->SetVisible(true);
		}
	}
	else
	{
		if (IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet((enumClientPacket)0xca);
			packet.Encode2((unsigned short)m_curPage);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
	}
}

void COfflineTradeDlg::OnBtnPrevInit(int param)
{
	m_pBtnPrev = DYNAMIC_CAST(FrButton, param);
}

void COfflineTradeDlg::OnBtnPrevBtnUp()
{
	--m_curPage;
	SetPageData(m_curPage, m_maxPage);
	RequestMaketList();
}

void COfflineTradeDlg::OnBtnNextInit(int param)
{
	m_pBtnNext = DYNAMIC_CAST(FrButton, param);
}

void COfflineTradeDlg::OnBtnNextBtnUp()
{
	++m_curPage;
	SetPageData(m_curPage, m_maxPage);
	RequestMaketList();
}

void COfflineTradeDlg::OnBtnGoPageInit(int param)
{
	m_pBtnGoPage = DYNAMIC_CAST(FrButton, param);
}

void COfflineTradeDlg::OnBtnGoPageBtnUp()
{
	m_curPage = atoi(m_pCurPage->GetLine(1, false));
	SetPageData(m_curPage, m_maxPage);
	RequestMaketList();
}

void COfflineTradeDlg::OnBtnRefreshInit(int param)
{
	m_pBtnRefresh = DYNAMIC_CAST(FrButton, param);
}

void COfflineTradeDlg::OnBtnRefreshBtnUp()
{
	m_curPage = 1;
	SetPageData(m_curPage, m_maxPage);
	m_searchType = 1;
	m_detailType = 0;
	m_searchString.clear();
	SetSearchString(
		"\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5");
	m_bReserved = false;
	m_bCanVisit = 0;
	m_pEditMsg->SetVisible(true);
	m_pEditMsg->AddText(
		" * \xc7\xf6\xc0\xe7 \xbb\xf3\xc5\xc2\xbf\xa1\xbc\xad\xb4\xc2 \xb4\xd9\xb8\xa5 \xbb\xe7\xb6\xf7\xc0\xc7 \xb8\xb6\xc0\xcc\xb7\xeb\xc0\xbb \xb9\xe6\xb9\xae \xc7\xcf\xbd\xc7\xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
		false, true);
	if (IsLocalContent((localContentType_t)102))
	{
		WSendPacket packet((enumClientPacket)0xca);
		packet.Encode2((unsigned short)m_curPage);
		packet.Send(TO_GAME);
		m_pItemSearch->SetFontColor(0xffd1d1d0);
		m_pItemSearch->ClearLine();
		m_pItemSearch->AddLine(
			"\xb9\xb0\xb0\xc7\xc0\xbb \xb0\xf1\xb6\xf3\xba\xb8\xb7\xc5", 0,
			false);
		m_pItemSearch->SetKeyFocus(true);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
	m_pBtnDetail->SetVisible(false);
}

void COfflineTradeDlg::OnBtnDetailInit(int param)
{
	m_pBtnDetail = DYNAMIC_CAST(FrButton, param);
	if (m_pBtnDetail)
		m_pBtnDetail->SetVisible(false);
}

void COfflineTradeDlg::OnBtnDetailBtnUp()
{
	WRect pos;
	pos = m_pBtnDetail->GetRect();
	if (!m_pStandingInLineDlg)
	{
		m_pStandingInLineDlg = CreateForm<CStandingInLine>(
			g_pFresh->GetManager(), this, "arraydlg");
		if (m_pStandingInLineDlg)
			m_pStandingInLineDlg->Open(
				(FRESH_PFN_RESULT)&COfflineTradeDlg::OnStandingInLineDlgResult,
				WPoint(pos.x + 10.0f, pos.y), FrESCAPE);
	}
}

void COfflineTradeDlg::OnEditMsgInit(int param)
{
	m_pEditMsg = DYNAMIC_CAST(FrEdit, param);
	if (m_pEditMsg)
		m_pEditMsg->SetVisible(false);
}

void COfflineTradeDlg::OnEditMsgLBtnUp()
{
}

void COfflineTradeDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void COfflineTradeDlg::OnToolTipOnwerDraw(int param)
{
	CIconToolTip::GetInstance()->Render();
}

bool COfflineTradeDlg::SetSearchString(std::string str)
{
	if (str.size() == 0)
	{
		std::string text;
		if (m_pItemSearch)
			text = m_pItemSearch->GetLine(1, false);
		if (text.size() == 0)
		{
			FrForm* dlg =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			dlg->SetMessage(
				"\xb0\xcb\xbb\xf6\xc7\xcf\xb7\xc1\xb4\xc2 \xb4\xdc\xbe\xee\xb0\xa1 \xbf\xc3\xb9\xd9\xb8\xa3\xc1\xf6 \xbe\xca\xb0\xc5\xb3\xaa \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xc0\xd4\xb7\xc2\xc7\xcf\xbc\xbc\xbf\xe4",
				false);
			dlg->Open(NULL, FrESCAPE | FrENTER);
			m_searchString.clear();
			return false;
		}
		SetSearchString(text);
	}
	else
		m_searchString = str;
	return true;
}

bool COfflineTradeDlg::OnStandingInLineDlgResult(int result, FrForm* form)
{
	unsigned int selected = 0;
	if (result == 1 && m_pStandingInLineDlg)
		selected = m_pStandingInLineDlg->GetSelectNum();
	switch (selected)
	{
	case 0:
		m_detailType = 0;
		break;
	case 1:
		m_detailType = 10;
		break;
	case 2:
		m_detailType = 11;
		break;
	case 3:
		m_detailType = 12;
		break;
	case 4:
		m_detailType = 13;
		break;
	case 5:
		m_detailType = 14;
		break;
	case 6:
		m_detailType = 15;
		break;
	case 7:
		m_detailType = 16;
		break;
	case 8:
		m_detailType = 17;
		break;
	case 9:
		m_detailType = 18;
		break;
	case 11:
		m_detailType = 21;
		break;
	case 12:
		m_detailType = 23;
		break;
	case 13:
		m_detailType = 22;
		break;
	case 14:
		m_detailType = 24;
		break;
	case 15:
		m_detailType = 19;
		break;
	}
	m_curPage = 1;
	RequestMaketList();
	m_pStandingInLineDlg = NULL;
	return true;
}

IMPLEMENT_OBJECT(CBuyingSlotDlg, FrForm)

BEGIN_FRESH_MSGMAP(CBuyingSlotDlg, FrForm)
ON_FRESH_VI("days", FRCMD_INIT, CBuyingSlotDlg::OnDaysInit)
ON_FRESH_VV("days", FRCMD_LBUTTONDOWN, CBuyingSlotDlg::OnDaysBtnDown)
ON_FRESH_VI("my_money", FRCMD_INIT, CBuyingSlotDlg::OnMyMoneyInit)
ON_FRESH_VI("money_left", FRCMD_INIT, CBuyingSlotDlg::OnMoneyLeftInit)
ON_FRESH_VI("price", FRCMD_INIT, CBuyingSlotDlg::OnPriceInit)
ON_FRESH_VI("checkout", FRCMD_INIT, CBuyingSlotDlg::OnCheckOutInit)
ON_FRESH_VV("checkout", FRCMD_LBUTTONDOWN, CBuyingSlotDlg::OnCheckOutBtnDown)
ON_FRESH_VI("slot_price", FRCMD_INIT, CBuyingSlotDlg::OnSlotPriceInit)
END_FRESH_MSGMAP()

sdays_t sDay[] = {
	{ K2L_Compatibility("1\xc0\xcf"),                1   },
	{ K2L_Compatibility("7\xc0\xcf"),                7   },
	{ K2L_Compatibility("15\xc0\xcf"),               15  },
	{ K2L_Compatibility("30\xc0\xcf"),               30  },
	{ K2L_Compatibility("365\xc0\xcf"),              365 },
	{ K2L_Compatibility("\xb9\xab\xc1\xa6\xc7\xd1"), 0   }
};

CBuyingSlotDlg::CBuyingSlotDlg()
{
	m_pDays = NULL;
	m_pMyMoney = NULL;
	m_pMoneyLeft = NULL;
	m_pPrice = NULL;
	m_pCheckOut = NULL;
	m_pSlotPrice = NULL;
	m_price = 0;
}

CBuyingSlotDlg::~CBuyingSlotDlg()
{
}

void CBuyingSlotDlg::OnDaysInit(int param)
{
	m_pDays = DYNAMIC_CAST(FrComboBox, param);
	if (m_pDays)
	{
		m_pDays->ClearList();
		IFF_STRUCT::sFurniture* furniture =
			ItemManager()->FindFurniture(0x4800601e);
		for (int i = 0; i < 4; ++i)
			if (furniture->COM[i])
				m_pDays->AddString(sDay[i].str);
		m_pDays->SetLine(1, sDay[0].str, 0, false, 0);
		m_price = furniture->COM[0];
		m_pSlotPrice->ClearLine();
		m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
	}
}

void CBuyingSlotDlg::OnDaysBtnDown()
{
	IFF_STRUCT::sFurniture* furniture =
		ItemManager()->FindFurniture(0x4800601e);
	const char* text = m_pDays->GetLine(1, false);
	for (int i = 0; i < 5; ++i)
	{
		if (!stricmp(sDay[i].str, text))
		{
			m_price = furniture->COM[i];
			m_pSlotPrice->ClearLine();
			m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
		}
	}
	RecalculatePang();
}

void CBuyingSlotDlg::OnPriceInit(int param)
{
	m_pPrice = DYNAMIC_CAST(FrEdit, param);
	if (m_pPrice)
	{
		char text[32];
		MakePriceStrA(text, m_price);
		strcat(text, "\xc6\xce");
		m_pPrice->SetLine(1, text, 0, false, 0);
	}
}

void CBuyingSlotDlg::OnMyMoneyInit(int param)
{
	m_pMyMoney = DYNAMIC_CAST(FrEdit, param);
	if (m_pMyMoney)
	{
		char text[32];
		MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		strcat(text, "\xc6\xce");
		m_pMyMoney->SetLine(1, text, 0, false, 0);
	}
}

void CBuyingSlotDlg::OnMoneyLeftInit(int param)
{
	m_pMoneyLeft = DYNAMIC_CAST(FrEdit, param);
	char text[32];
	if (m_pMoneyLeft)
	{
		if (Doc()->m_myInfo.stat.i64Pang - m_price < 0)
		{
			MakePriceStrA(text, m_price - Doc()->m_myInfo.stat.i64Pang);
			strcat(text, "\xc6\xce \xba\xce\xc1\xb7");
			m_pMoneyLeft->SetLine(1, text, 0xffff0000, false, 0);
		}
		else
		{
			MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang - m_price);
			strcat(text, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, text, 0, false, 0);
		}
	}
}

void CBuyingSlotDlg::RecalculatePang()
{
	char text[32];
	if (m_pPrice)
	{
		MakePriceStrA(text, m_price);
		strcat(text, "\xc6\xce");
		m_pPrice->SetLine(1, text, 0, false, 0);
	}
	if (m_pMyMoney)
	{
		MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang);
		strcat(text, "\xc6\xce");
		m_pMyMoney->SetLine(1, text, 0, false, 0);
	}

	if (m_pMoneyLeft)
	{
		if (Doc()->m_myInfo.stat.i64Pang - m_price < 0)
		{
			MakePriceStrA(text, m_price - Doc()->m_myInfo.stat.i64Pang);
			strcat(text, "\xc6\xce \xba\xce\xc1\xb7");
			m_pMoneyLeft->SetLine(1, text, 0xffff0000, false, 0);
		}
		else
		{
			MakePriceStrA(text, Doc()->m_myInfo.stat.i64Pang - m_price);
			strcat(text, "\xc6\xce");
			m_pMoneyLeft->SetLine(1, text, 0, false, 0);
		}
	}
}

void CBuyingSlotDlg::OnCheckOutInit(int param)
{
	m_pCheckOut = DYNAMIC_CAST(FrButton, param);
}

void CBuyingSlotDlg::OnCheckOutBtnDown()
{
	sBuyItem buyItem;
	IFF_STRUCT::sFurniture* furniture =
		ItemManager()->FindFurniture(0x4800601e);
	const char* text = m_pDays->GetLine(1, false);
	int day = 0;
	int i;
	for (i = 0; i < 5; ++i)
		if (!stricmp(sDay[i].str, text))
		{
			day = sDay[i].day;
			break;
		}
	sStall* stall = Doc()->GetStallItem();
	if (furniture->COM[i] > Doc()->m_myInfo.stat.i64Pang)
	{
		FrForm* dlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		dlg->SetMessage(
			"\xbc\xd2\xc1\xf6\xc7\xcf\xbd\xc5 \xb1\xdd\xbe\xd7\xc0\xcc \xba\xce\xc1\xb7\xc7\xd5\xb4\xcf\xb4\xd9. \xbd\xbd\xb7\xd4\xc0\xbb \xb1\xb8\xb8\xc5\xc7\xcf\xbd\xc7 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
			false);
		dlg->Open(NULL, FrESCAPE | FrENTER);
	}
	else if (IsLocalContent((localContentType_t)102))
	{
		WSendPacket packet(0xc5);
		packet.Encode4(stall->stallKey);
		packet.Encode4(0x4800601e);
		packet.Encode1((unsigned char)m_slotNum);
		packet.Encode4(day * 24 * 60);
		packet.Send(TO_GAME);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
	FrForm::OnOK();
}

void CBuyingSlotDlg::OnSlotPriceInit(int param)
{
	m_pSlotPrice = DYNAMIC_CAST(FrEdit, param);
	if (m_pSlotPrice)
	{
		m_pSlotPrice->ClearLine();
		m_pSlotPrice->AddText(MakeStr("%d \xc6\xce", m_price), false, true);
	}
}

IMPLEMENT_OBJECT(COfflineShopEditDlg, FrForm)

BEGIN_FRESH_MSGMAP(COfflineShopEditDlg, FrForm)
ON_FRESH_VI("areaBg01", FRCMD_INIT, COfflineShopEditDlg::OnAreaBg01Init)
ON_FRESH_VI("areaBg02", FRCMD_INIT, COfflineShopEditDlg::OnAreaBg02Init)
ON_FRESH_VI("areaBg03", FRCMD_INIT, COfflineShopEditDlg::OnAreaBg03Init)
ON_FRESH_VI("areaBg04", FRCMD_INIT, COfflineShopEditDlg::OnAreaBg04Init)
ON_FRESH_VI("areaBg05", FRCMD_INIT, COfflineShopEditDlg::OnAreaBg05Init)
ON_FRESH_VI("edit_slot", FRCMD_INIT, COfflineShopEditDlg::OnStallSlotNumInit)
ON_FRESH_VI("edit_slotstate", FRCMD_INIT, COfflineShopEditDlg::OnSlotNumInit)
ON_FRESH_VI("tradeeditdlg", FRCMD_INIT, COfflineShopEditDlg::OnTradeEditDlgInit)
ON_FRESH_VI("my_item_list", FRCMD_INIT, COfflineShopEditDlg::OnMyItemListInit)
ON_FRESH_VV("my_item_list", FRCMD_LBUTTONDOWN,
	COfflineShopEditDlg::OnMyItemListLClick)
ON_FRESH_VV("my_item_list", FRCMD_RBUTTONDOWN,
	COfflineShopEditDlg::OnMyItemListRClick)
ON_FRESH_VI("my_item_list", FRCMD_OWNERDRAW,
	COfflineShopEditDlg::OnMyItemListOwnerDraw)
ON_FRESH_VI("trade_item_list", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeItemListInit)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONDOWN,
	COfflineShopEditDlg::OnTradeItemListLClick)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	COfflineShopEditDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("visitor", FRCMD_INIT, COfflineShopEditDlg::OnVisitorInit)
ON_FRESH_VI("userid", FRCMD_INIT, COfflineShopEditDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, COfflineShopEditDlg::OnTitleInit)
ON_FRESH_VI("trade_title_edit", FRCMD_INIT,
	COfflineShopEditDlg::OnTitleEditInit)
ON_FRESH_BI("trade_title_edit", FRCMD_ENTERKEY,
	COfflineShopEditDlg::OnTitleEditEnterKey)
ON_FRESH_VI("trade_title_edit_bn", FRCMD_INIT,
	COfflineShopEditDlg::OnTitleEditBnInit)
ON_FRESH_VV("trade_title_edit_bn", FRCMD_LBUTTONUP,
	COfflineShopEditDlg::OnTitleEditBnUp)
ON_FRESH_VI("trade_myitem_icon", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemIconInit)
ON_FRESH_VV("trade_myitem_icon", FRCMD_OWNERDRAW,
	COfflineShopEditDlg::OnTradeMyItemIconOwnerDraw)
ON_FRESH_VI("trade_myitem_name", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemNameInit)
ON_FRESH_VI("trade_myitem_ability", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemAbilityInit)
ON_FRESH_VI("trade_myitem_number", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemNumberInit)
ON_FRESH_VI("trade_myitem_price", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemPriceInit)
ON_FRESH_VV("trade_myitem_price", FRCMD_LBUTTONUP,
	COfflineShopEditDlg::OnTradeMyItemPriceClick)
ON_FRESH_VI("trade_myitem_ok_bn", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemOkBnInit)
ON_FRESH_VV("trade_myitem_ok_bn", FRCMD_LBUTTONUP,
	COfflineShopEditDlg::OnTradeMyItemOkBnUp)
ON_FRESH_VI("trade_myitem_cancel_bn", FRCMD_INIT,
	COfflineShopEditDlg::OnTradeMyItemCancelBnInit)
ON_FRESH_VV("trade_myitem_cancel_bn", FRCMD_LBUTTONUP,
	COfflineShopEditDlg::OnTradeMyItemCancelBnUp)
ON_FRESH_VI("open", FRCMD_INIT, COfflineShopEditDlg::OnOpenBnInit)
ON_FRESH_VV("open", FRCMD_LBUTTONUP, COfflineShopEditDlg::OnOpenBnUp)
ON_FRESH_VI("close", FRCMD_INIT, COfflineShopEditDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, COfflineShopEditDlg::OnCloseBnUp)
ON_FRESH_VI("exit", FRCMD_INIT, COfflineShopEditDlg::OnExitBnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, COfflineShopEditDlg::OnExitBnUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, COfflineShopEditDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, COfflineShopEditDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

COfflineShopEditDlg::COfflineShopEditDlg()
{
	m_pAreaBg01 = 0;
	m_pAreaBg02 = 0;
	m_pAreaBg03 = 0;
	m_pAreaBg04 = 0;
	m_pAreaBg05 = 0;
	m_pCloseBn = 0;
	m_pStallSlotNum = 0;
	m_pSlotNum = 0;
	m_usedSlotNum = 0;
	m_selectUID = 0;
	m_selectSlot = 0;
	m_pFinishEditDlg = 0;
	m_bExit = 0;
	m_buyBtn[0].pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtn[0].rect = WRect(120.0f, 30.0f, (float)m_buyBtn[0].pBitmap->Width(),
		(float)m_buyBtn[0].pBitmap->Height());
	m_buyBtn[1].pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtn[1].rect = WRect(120.0f, 30.0f, (float)m_buyBtn[1].pBitmap->Width(),
		(float)m_buyBtn[1].pBitmap->Height());
	m_deleteBtn[0].pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_normal");
	m_deleteBtn[0].rect =
		WRect(120.0f, 30.0f, (float)m_deleteBtn[0].pBitmap->Width(),
			(float)m_deleteBtn[0].pBitmap->Height());
	m_deleteBtn[1].pBitmap = g_pFresh->GetBitmap("trade_item_del_bn_over");
	m_deleteBtn[1].rect =
		WRect(120.0f, 30.0f, (float)m_deleteBtn[1].pBitmap->Width(),
			(float)m_deleteBtn[1].pBitmap->Height());
}

COfflineShopEditDlg::~COfflineShopEditDlg()
{
	if (!m_bExit)
		IsLocalContent((localContentType_t)102);
}

void COfflineShopEditDlg::OnAreaBg01Init(int param)
{
	m_pAreaBg01 = DYNAMIC_CAST(FrArea, param);
	if (m_pAreaBg01)
		m_pAreaBg01->SetBgImg("rmr_trade_edit_back_01");
}

void COfflineShopEditDlg::OnAreaBg02Init(int param)
{
	m_pAreaBg02 = DYNAMIC_CAST(FrArea, param);
	if (m_pAreaBg02)
		m_pAreaBg02->SetBgImg("rmr_trade_edit_back_02");
}

void COfflineShopEditDlg::OnAreaBg03Init(int param)
{
	m_pAreaBg03 = DYNAMIC_CAST(FrArea, param);
	if (m_pAreaBg03)
		m_pAreaBg03->SetBgImg("rmr_trade_edit_back_03");
}

void COfflineShopEditDlg::OnAreaBg04Init(int param)
{
	m_pAreaBg04 = DYNAMIC_CAST(FrArea, param);
	if (m_pAreaBg04)
		m_pAreaBg04->SetBgImg("rmr_trade_edit_back_04");
}

void COfflineShopEditDlg::OnAreaBg05Init(int param)
{
	m_pAreaBg05 = DYNAMIC_CAST(FrArea, param);
	if (m_pAreaBg05)
		m_pAreaBg05->SetVisible(false);
}

void COfflineShopEditDlg::OnStallSlotNumInit(int param)
{
	m_pStallSlotNum = DYNAMIC_CAST(FrEdit, param);
}

void COfflineShopEditDlg::OnSlotNumInit(int param)
{
	m_pSlotNum = DYNAMIC_CAST(FrEdit, param);
}

void COfflineShopEditDlg::OnTradeItemListInit(int param)
{
	m_pTradeItemList = DYNAMIC_CAST(FrListBox, param);
	if (IsLocalContent((localContentType_t)102) && Doc()->GetTradeMode() == 1)
	{
		sStall* stall = Doc()->GetStallItem();
		for (int i = 0; i < stall->maxSlotNum; ++i)
			m_pTradeItemList->AddItem(&stall->slotList[i]);
	}
}

void COfflineShopEditDlg::OnTradeItemListLClick()
{
	FrListItem* item = m_pTradeItemList->GetItemUnderCursor();
	if (item && item->pData)
		item->selected = !item->selected;
}

void COfflineShopEditDlg::OnTradeItemListOwnerDraw(int param)
{
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic || !param)
		return;
	FrListItem* draw = (FrListItem*)param;
	sStallSlot* slot = (sStallSlot*)draw->pData;
	{
		const Bitmap* bitmap = m_pTradeItemBaseNormal;
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(draw->pos.x + 1, draw->pos.y + 1,
					(float)m_pTradeItemBaseNormal->Width(),
					(float)m_pTradeItemBaseNormal->Height()),
				0xffffffff, 0);
	}
	if (slot->IsValid == 1 && slot->itemData.dwTid == 0)
		graphic->Print(WPoint(draw->pos.x + 10, draw->pos.y + 13), 0,
			"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
	if (slot->IsValid == 0 && slot->itemData.dwTid == 0)
		graphic->Print(WPoint(draw->pos.x + 10, draw->pos.y + 13), 0,
			"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5  ");
	else if (slot->itemData.dwTid == 0x4800581f ||
		slot->itemData.dwTid == 0x4800601e)
		graphic->Print(WPoint(draw->pos.x + 10, draw->pos.y + 13), 0,
			"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
	else
	{
		if (!slot->itemData.dwTid)
			return;
		IFF_ITEM_COMMON* common =
			ItemManager()->FindCommonItem(slot->itemData.dwTid);
		{
			const Bitmap* bitmap = m_pTradeItemBaseNormal;
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(draw->pos.x + 1, draw->pos.y + 1,
						(float)m_pTradeItemBaseNormal->Width(),
						(float)m_pTradeItemBaseNormal->Height()),
					0xffffffff, 0);
		}
		char price[32] = { 0 };
		MakePriceStrA(price, slot->itemData.i64Price);
		if (slot->itemData.iNum == 1)
			MakeStr("%s\xc6\xce", price);
		else
			MakeStr("\xb0\xb3\xb4\xe7%s\xc6\xce", price);
		graphic->Print(WPoint(draw->pos.x + 10, draw->pos.y + 13), 0, "%s",
			common->Name);
	}
}

void COfflineShopEditDlg::OnVisitorInit(int param)
{
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);
	if (IsLocalContent((localContentType_t)102) && Doc()->GetTradeMode() == 1)
		m_pVisitor->SetVisible(false);
}

void COfflineShopEditDlg::OnOpenBnInit(int param)
{
	m_pOpenBn = DYNAMIC_CAST(FrButton, param);
	if (m_pOpenBn)
		m_pOpenBn->SetVisible(false);
}

void COfflineShopEditDlg::OnTradeMyItemOkBnUp()
{
	if (!m_pSelectItem)
		return;
	sTradeItem item = *m_pSelectItem;
	int number = atoi(m_pTradeMyItemNumber->GetLine(1, false));
	char price[10] = { 0 };
	const char* text = m_pTradeMyItemPrice->GetLine(1, false);
	int i = 0;
	char* out = price;
	for (; i < 9; ++i)
	{
		char ch = text[i];
		if (ch == 0)
			break;
		if (ch >= '0' && ch <= '9')
			*out++ = ch;
		else if (ch != ',')
		{
			price[0] = 0;
			break;
		}
	}
	__int64 value = atoi(price);
	int available = item.iNum;
	for (std::list<sTradeItem>::iterator it = m_tradeItemList.begin();
		it != m_tradeItemList.end(); ++it)
	{
		if (&*it == m_pSelectItem)
		{
			available = (*it).iNum;
			for (std::list<sTradeItem>::iterator mine = m_myItemList.begin();
				mine != m_myItemList.end(); ++mine)
			{
				if ((*mine).dwGuid == m_pSelectItem->dwGuid)
				{
					available += (*mine).iNum;
					break;
				}
			}
			break;
		}
	}

	if (number < 1 || number > available)
	{
		SetTradeResultCode(10, 0xe1, NULL);
		return;
	}
	if (value <= 0)
	{
		SetTradeResultCode(11, 0xe1, NULL);
		return;
	}
	if (Doc()->GetStallItem()->stallKey != m_selectUID)
	{
		FrForm* notify =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
		notify->SetMessage(
			"\xc7\xf6\xc0\xe7 \xc1\xf8\xbf\xad\xb4\xeb\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xc0\xcf\xc4\xa1\xc7\xcf\xc1\xf6 \xbe\xca\xbd\xc0\xb4\xcf\xb4\xd9. \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\xc0\xd4\xb4\xcf\xb4\xd9.",
			false);
		notify->Open(NULL, FrESCAPE | FrENTER);
		m_selectUID = 0;
		m_selectSlot = 0;
		OnExitBnUp();
		return;
	}
	item.i64Price = value;
	item.iNum = number;
	if (IsLocalContent((localContentType_t)102))
	{
		WSendPacket packet(0xc7);
		packet.Encode4(m_selectUID);
		packet.Encode2(1);
		packet.Encode1((unsigned char)m_selectSlot);
		packet.EncodeBuffer(&item, sizeof(item));
		packet.Send(TO_GAME);
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
	}
}

void COfflineShopEditDlg::SetSelectData(unsigned long uid, unsigned long slot)
{
	m_selectUID = uid;
	m_selectSlot = slot;
}

void COfflineShopEditDlg::OnCloseBnInit(int param)
{
	m_pCloseBn = DYNAMIC_CAST(FrButton, param);
	if (m_pCloseBn)
		m_pCloseBn->SetVisible(false);
}

void COfflineShopEditDlg::OnExitBnUp()
{
	m_bExit = 1;
	COfflineShopDlg* dlg = CreateForm<COfflineShopDlg>(g_pFresh->GetManager(),
		this, "offlinetradedlg");
	if (dlg)
	{
		dlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
		dlg->OpenTradeShop(Doc()->m_replayTargetUID);
		dlg->SetListTradeItem(false);
		dlg->SetFadeout(false);
	}
	FrForm::OnFreshCancel();
}

bool COfflineShopEditDlg::OnInit()
{
	FrForm::EnableDrag(false);
	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
	return true;
}

void COfflineShopEditDlg::InitAllUI()
{
	FrForm::EnableDrag(false);
	SetListEnableTradingMyItem();
	SetListTradingItem();
	SetListMyItem();
	SetListTradeItem();
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
}

void COfflineShopEditDlg::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	CIconToolTip::GetInstance()->ClearToolTip();
}

void COfflineShopEditDlg::SetListTradeItem()
{
	FrTradeEditDlg::SetListTradeItem();
	if (m_pTradeItemList)
	{
		m_pTradeItemList->ClearItem();
		m_usedSlotNum = 0;
		sStall* stall = Doc()->GetStallItem();
		if (!stall)
			stall = &*Doc()->GetListStallItem()->begin();
		if (stall)
		{
			for (int i = 0; i < stall->maxSlotNum; ++i)
			{
				if (stall->slotList[i].Idx)
					m_pTradeItemList->AddItem(&stall->slotList[i]);
				if (stall->slotList[i].itemData.dwTid)
					++m_usedSlotNum;
			}
			if (m_pStallSlotNum)
			{
				m_pStallSlotNum->ClearLine();
				m_pStallSlotNum->AddLine(MakeStr("%d", stall->maxSlotNum), 0,
					false);
			}
			if (m_pSlotNum)
			{
				m_pSlotNum->ClearLine();
				m_pSlotNum->AddLine(
					MakeStr("%2d/%2d", m_usedSlotNum, stall->useSlotNum), 0,
					false);
			}
		}
	}
}

void COfflineShopEditDlg::SetSlotData()
{
	m_usedSlotNum = 0;
	sStall* stall = Doc()->GetStallItem();
	if (!stall)
		stall = &*Doc()->GetListStallItem()->begin();
	if (stall)
	{
		for (int i = 0; i < stall->maxSlotNum; ++i)
		{
			if (stall->slotList[i].itemData.dwTid)
				++m_usedSlotNum;
		}
		if (m_pStallSlotNum)
		{
			m_pStallSlotNum->ClearLine();
			m_pStallSlotNum->AddLine(MakeStr("%d", stall->maxSlotNum), 0,
				false);
		}
		if (m_pSlotNum)
		{
			m_pSlotNum->ClearLine();
			m_pSlotNum->AddLine(
				MakeStr("%2d/%2d", m_usedSlotNum, stall->useSlotNum), 0, false);
		}
	}
}

bool COfflineShopEditDlg::IsCheckCardPats(unsigned long typeId,
	unsigned long id)
{
	std::list<sSCardAvilityPeriodInfo>& list = Doc()->m_cardAbilityList[0];
	std::list<sSCardAvilityPeriodInfo>::iterator it = list.begin();
	for (; it != list.end(); ++it)
		if ((*it).partsTid == id && (*it).partsUid == typeId)
			break;
	if (it != list.end())
		return false;
	return true;
}

bool COfflineShopEditDlg::OnOfflineEquitCancelDlgResult(int result,
	FrForm* form)
{
	if (result == 1)
	{
		FrListItem* item = m_pTradeItemList->GetItemUnderCursor();
		if (item && IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet((enumClientPacket)0xc8);
			packet.Encode4(Doc()->GetStallItem()->stallKey);
			packet.Encode2(1);
			packet.Encode1((unsigned char)(item->idx + 1));
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
	}
	return true;
}

bool COfflineShopEditDlg::OnOfflineTradeItemOnDlgResult(int result,
	FrForm* form)
{
	if (result == 1 && m_pFinishEditDlg)
	{
		int uid, slot;
		m_pFinishEditDlg->GetSelectSlotInfo(uid, slot);
		sTradeItem item = *m_pSelectItem;
		int number = atoi(m_pTradeMyItemNumber->GetLine(1, false));
		char price[10] = { 0 };
		const char* text = m_pTradeMyItemPrice->GetLine(1, false);
		int i = 0;
		char* out = price;
		for (; i < 9; ++i)
		{
			char ch = text[i];
			if (ch == 0)
				break;
			if (ch >= '0' && ch <= '9')
				*out++ = ch;
			else if (ch != ',')
			{
				price[0] = 0;
				break;
			}
		}
		item.i64Price = atoi(price);
		item.iNum = number;
		if (IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet(0xc7);
			packet.Encode4(uid);
			packet.Encode2(1);
			packet.Encode1((unsigned char)slot);
			packet.EncodeBuffer(&item, sizeof(item));
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
	}
	m_pFinishEditDlg = NULL;
	return true;
}

void COfflineShopEditDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void COfflineShopEditDlg::OnToolTipOnwerDraw(int param)
{
	CIconToolTip::GetInstance()->Render();
}

void COfflineShopEditDlg::DelListMyItem(sTradeItem* item, unsigned long count,
	unsigned char slot)
{
	if (!item->dwTid)
		SetListMyItem();
	else
		FrTradeEditDlg::DelListMyItem(item);
	SetTradeMyItem(NULL);
	SetControlsTradeMyItem(NULL, false);
	SetListTradeItem();
	if (m_pMyItemList)
		m_pMyItemList->UnselectAllItem();
}

bool COfflineShopEditDlg::OnOfflineTradeBuyDlgResult(int result, FrForm* form)
{
	if (result == 1)
	{
		FrOfflineTradeFinishEditDlg* dlg =
			CreateForm<FrOfflineTradeFinishEditDlg>(g_pFresh->GetManager(),
				this, "offlinetradefinisheditdlg");
		if (dlg)
		{
			dlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
			dlg->SetFadeout(false);
			dlg->SetListTradeItem(&m_tradeItemList);
			dlg->SetTradeUserId();
			dlg->SetTradeTitle(m_title);
		}
	}
	return true;
}

IMPLEMENT_OBJECT(COfflineShopDlg, FrForm)

BEGIN_FRESH_MSGMAP(COfflineShopDlg, FrForm)
ON_FRESH_VI("tradedlg", FRCMD_INIT, COfflineShopDlg::OnTradeDlgInit)
ON_FRESH_VI("visitor_back", FRCMD_INIT, COfflineShopDlg::OnVisitorBackInit)
ON_FRESH_VV("visitor_back", FRCMD_OWNERDRAW,
	COfflineShopDlg::OnVisitorBackOwnerDraw)
ON_FRESH_VI("mypang", FRCMD_INIT, COfflineShopDlg::OnMyPangInit)
ON_FRESH_VV("mypang", FRCMD_OWNERDRAW, COfflineShopDlg::OnMyPangOwnerDraw)
ON_FRESH_VI("trade_item_list", FRCMD_INIT, COfflineShopDlg::OnTradeItemListInit)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONDOWN,
	COfflineShopDlg::OnTradeItemListLClick)
ON_FRESH_VV("trade_item_list", FRCMD_RBUTTONDOWN,
	COfflineShopDlg::OnTradeItemListRClick)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	COfflineShopDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("noitem01", FRCMD_INIT, COfflineShopDlg::OnNoItem01Init)
ON_FRESH_VI("noitem02", FRCMD_INIT, COfflineShopDlg::OnNoItem02Init)
ON_FRESH_VI("visitor", FRCMD_INIT, COfflineShopDlg::OnVisitorInit)
ON_FRESH_VI("userid", FRCMD_INIT, COfflineShopDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, COfflineShopDlg::OnTitleInit)
ON_FRESH_VI("edit", FRCMD_INIT, COfflineShopDlg::OnEditBnInit)
ON_FRESH_VV("edit", FRCMD_LBUTTONUP, COfflineShopDlg::OnEditBnUp)
ON_FRESH_VI("close", FRCMD_INIT, COfflineShopDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, COfflineShopDlg::OnCloseBnUp)
ON_FRESH_VI("hide", FRCMD_INIT, COfflineShopDlg::OnHideBnInit)
ON_FRESH_VV("hide", FRCMD_LBUTTONUP, COfflineShopDlg::OnHideBnUp)
ON_FRESH_VI("exit", FRCMD_INIT, COfflineShopDlg::OnExitBnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, COfflineShopDlg::OnExitBnUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, COfflineShopDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, COfflineShopDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

COfflineShopDlg::COfflineShopDlg()
{
	m_pVisitor = 0;
	m_bExit = 0;
	m_pSelectItem = 0;
	m_extendBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_n");
	m_extendBtn[0].rect =
		WRect(96.0f, 66.0f, (float)m_extendBtn[0].pBitmap->Width(),
			(float)m_extendBtn[0].pBitmap->Height());
	m_extendBtn[0].index = -1;
	m_extendBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_extend_btn_o");
	m_extendBtn[1].rect =
		WRect(96.0f, 66.0f, (float)m_extendBtn[1].pBitmap->Width(),
			(float)m_extendBtn[1].pBitmap->Height());
	m_extendBtn[1].index = -1;
	m_returnBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_return_n");
	m_returnBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_returnBtn[0].pBitmap->Width(),
			(float)m_returnBtn[0].pBitmap->Height());
	m_returnBtn[0].index = -1;
	m_returnBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_return_o");
	m_returnBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_returnBtn[1].pBitmap->Width(),
			(float)m_returnBtn[1].pBitmap->Height());
	m_returnBtn[1].index = -1;
	m_deleteBtn[0].pBitmap = g_pFresh->GetBitmap("delete_btn_n");
	m_deleteBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_deleteBtn[0].pBitmap->Width(),
			(float)m_deleteBtn[0].pBitmap->Height());
	m_deleteBtn[0].index = -1;
	m_deleteBtn[1].pBitmap = g_pFresh->GetBitmap("delete_btn_o");
	m_deleteBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_deleteBtn[1].pBitmap->Width(),
			(float)m_deleteBtn[1].pBitmap->Height());
	m_deleteBtn[1].index = -1;
	m_buyBtn[0].pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtn[0].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[0].pBitmap->Width(),
		(float)m_buyBtn[0].pBitmap->Height());
	m_buyBtn[0].index = -1;
	m_buyBtn[1].pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtn[1].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[1].pBitmap->Width(),
		(float)m_buyBtn[1].pBitmap->Height());
	m_buyBtn[1].index = -1;
	m_deleteMark.pBitmap = g_pFresh->GetBitmap("delete");
	m_deleteMark.rect =
		WRect(144.0f, 35.0f, (float)m_deleteMark.pBitmap->Width(),
			(float)m_deleteMark.pBitmap->Height());
	m_tradeState[0].pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState[0].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[0].pBitmap->Width(),
			(float)m_tradeState[0].pBitmap->Height());
	m_tradeState[1].pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState[1].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[1].pBitmap->Width(),
			(float)m_tradeState[1].pBitmap->Height());
	m_tradeState[2].pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState[2].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[2].pBitmap->Width(),
			(float)m_tradeState[2].pBitmap->Height());
	m_uploadBtn[0].pBitmap = g_pFresh->GetBitmap("rmr_item_upload_n");
	m_uploadBtn[0].rect =
		WRect(144.0f, 66.0f, (float)m_uploadBtn[0].pBitmap->Width(),
			(float)m_uploadBtn[0].pBitmap->Height());
	m_uploadBtn[0].index = -1;
	m_uploadBtn[1].pBitmap = g_pFresh->GetBitmap("rmr_item_upload_o");
	m_uploadBtn[1].rect =
		WRect(144.0f, 66.0f, (float)m_uploadBtn[1].pBitmap->Width(),
			(float)m_uploadBtn[1].pBitmap->Height());
	m_uploadBtn[1].index = -1;
}

COfflineShopDlg::~COfflineShopDlg()
{
	if (!m_bExit)
		IsLocalContent((localContentType_t)102);
}

void COfflineShopDlg::OnVisitorBackOwnerDraw()
{
}

void COfflineShopDlg::OnVisitorInit(int param)
{
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);
	if (IsLocalContent((localContentType_t)102) && Doc()->GetTradeMode() == 1)
		m_pVisitor->SetVisible(false);
}

void COfflineShopDlg::OnCloseBnUp()
{
	m_bExit = 1;
	if (IsLocalContent((localContentType_t)102))
		AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 0x22f, 0, 0, 0, 0, 0));
	FrForm::OnOK();
}

void COfflineShopDlg::OnVisitorBackInit(int param)
{
	m_pVisitorBack = DYNAMIC_CAST(FrArea, param);
	if (m_pVisitorBack)
		m_pVisitorBack->SetVisible(false);
}

void COfflineShopDlg::OnTradeItemListOwnerDraw(int param)
{
	FrTradeDlg::OnTradeItemListOwnerDraw(param);
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	FrListItem* draw = (FrListItem*)param;
	if (!draw || !draw->pData)
		return;
	const Bitmap* bitmap = g_pFresh->GetBitmap("rmr_item_slot");
	sStallSlot* slot = (sStallSlot*)draw->pData;
	graphic->SetTextStyle(0);
	graphic->SetTextColor(0xff000000, 0xffffffff);
	if (slot->IsValid == 0)
	{
		if (bitmap)
			graphic->DrawTexture(bitmap,
				WRect(draw->pos.x, draw->pos.y, (float)bitmap->Width(),
					(float)bitmap->Height()),
				0xffffffff, 0);

		{
			WPoint pos = WPoint(182, 7) + draw->pos;
			graphic->Print(WPoint(pos), 2,
				"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4 \xb1\xb8\xb8\xc5");
		}
		if (draw->underCursor && (m_buyBtn[0].rect + draw->pos).IsInRect(mouse))
		{
			graphic->DrawTexture(m_buyBtn[1].pBitmap,
				m_buyBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			CIconToolTip::GetInstance()->SetToolTip((eBarButton)22);
		}
		else
		{
			graphic->DrawTexture(m_buyBtn[0].pBitmap,
				m_buyBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
		}
	}
	else
	{
		__int64 hours = 0;
		FILETIME end, now;
		if (!slot->IsPermanence)
		{
			SystemTimeToFileTime(&slot->time, &end);
			SystemTimeToFileTime(&Doc()->GetServerTime(), &now);
			__int64 remaining =
				((__int64)end.dwHighDateTime - now.dwHighDateTime) *
					0x100000000i64 -
				now.dwLowDateTime + end.dwLowDateTime;
			hours = remaining / 60 / 60 / 10000000;
			graphic->DrawTexture(m_tradeState[2].pBitmap,
				m_tradeState[2].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			if (hours >= 0 && remaining >= 0)
			{
				graphic->SetTextColor(0xffffffff, 0xffffffff);
				if (hours / 24 >= 1)
				{
					if (hours % 24)
					{
						WPoint pos = WPoint(m_tradeState[2].rect.x,
										 m_tradeState[2].rect.y) +
							draw->pos + WPoint(55, 6);
						graphic->Print(WPoint(pos), 2, "%d\xc0\xcf",
							hours / 24 + 1);
					}
					else
					{
						WPoint pos = WPoint(m_tradeState[2].rect.x,
										 m_tradeState[2].rect.y) +
							draw->pos + WPoint(55, 6);
						graphic->Print(WPoint(pos), 2, "%d\xc0\xcf",
							hours / 24);
					}
				}
				else
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2, "%d\xbd\xc3", hours);
				}
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}
			else
			{
				graphic->SetTextColor(0xffffffff, 0xffffffff);
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2,
						"\xb1\xe2\xb0\xa3\xb8\xb8\xb7\xe1");
				}
				graphic->SetTextColor(0xff000000, 0xffffffff);
			}

			if (draw->underCursor &&
				(m_extendBtn[0].rect + draw->pos).IsInRect(mouse))
			{
				graphic->DrawTexture(m_extendBtn[1].pBitmap,
					m_extendBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)67);
			}
			else
			{
				graphic->DrawTexture(m_extendBtn[0].pBitmap,
					m_extendBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
			}
		}
		if (!slot->itemData.dwTid)
		{
			if (bitmap)
				graphic->DrawTexture(bitmap,
					WRect(draw->pos.x, draw->pos.y, (float)bitmap->Width(),
						(float)bitmap->Height()),
					0xffffffff, 0);

			{
				WPoint pos = WPoint(182, 7) + draw->pos;
				graphic->Print(WPoint(pos), 2,
					"\xbb\xf3\xc1\xa1 \xbe\xc6\xc0\xcc\xc5\xdb \xbd\xbd\xb7\xd4");
			}
			if (draw->underCursor &&
				(m_uploadBtn[0].rect + draw->pos).IsInRect(mouse))
			{
				graphic->DrawTexture(m_uploadBtn[1].pBitmap,
					m_uploadBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)68);
			}
			else
			{
				graphic->DrawTexture(m_uploadBtn[0].pBitmap,
					m_uploadBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
			}
		}
		else
		{
			if (!slot->IsPermanence)
			{
				if (hours >= 0)
				{
					if (draw->underCursor &&
						(m_deleteBtn[0].rect + draw->pos).IsInRect(mouse))
					{
						graphic->DrawTexture(m_deleteBtn[1].pBitmap,
							m_deleteBtn[1].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip((eBarButton)66);
					}
					else
					{
						graphic->DrawTexture(m_deleteBtn[0].pBitmap,
							m_deleteBtn[0].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
					}
				}
				else
				{
					if (draw->underCursor &&
						(m_returnBtn[0].rect + draw->pos).IsInRect(mouse))
					{
						graphic->DrawTexture(m_returnBtn[1].pBitmap,
							m_returnBtn[1].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
						CIconToolTip::GetInstance()->SetToolTip((eBarButton)69);
					}
					else
					{
						graphic->DrawTexture(m_returnBtn[0].pBitmap,
							m_returnBtn[0].rect + draw->pos + WPoint(0, 0),
							0xffffffff, 0);
					}
				}
			}
			else
			{
				if (draw->underCursor &&
					(m_deleteBtn[0].rect + draw->pos).IsInRect(mouse))
				{
					graphic->DrawTexture(m_deleteBtn[1].pBitmap,
						m_deleteBtn[1].rect + draw->pos + WPoint(0, 0),
						0xffffffff, 0);
					CIconToolTip::GetInstance()->SetToolTip((eBarButton)66);
				}
				else
				{
					graphic->DrawTexture(m_deleteBtn[0].pBitmap,
						m_deleteBtn[0].rect + draw->pos + WPoint(0, 0),
						0xffffffff, 0);
				}
			}
		}
	}
}

void COfflineShopDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void COfflineShopDlg::OnToolTipOnwerDraw(int param)
{
	CIconToolTip::GetInstance()->Render();
}

void COfflineShopDlg::OnTradeItemListLClick()
{
	FrListItem* item = m_pTradeItemList->GetItemUnderCursor();
	if (!item || !item->pData)
		return;
	sStallSlot* slot = (sStallSlot*)item->pData;
	if (!slot)
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();

	if (slot->IsValid == 0 &&
		((m_buyBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_buyBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		CBuyingSlotDlg* dlg = CreateForm<CBuyingSlotDlg>(g_pFresh->GetManager(),
			this, "buyslotdlg");
		dlg->SetSlotNum(item->idx + 1);
		dlg->Open(NULL, FrESCAPE);
		return;
	}
	if (slot->IsValid == 1 && !slot->itemData.dwTid &&
		((m_uploadBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_uploadBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		m_pSelectItem = item;
		FrForm* dlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		if (dlg)
		{
			dlg->Open((FRESH_PFN_RESULT)&COfflineShopDlg::
						  OnOfflineShopEditDlgCreateResult,
				FrRET_OK | FrRET_CANCEL);
			dlg->SetFadeout(false);
			dlg->SetMessage(
				"\xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb7\xcf\xc0\xbb \xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
				false);
		}
		return;
	}
	if (slot->IsValid == 1 &&
		((m_extendBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_extendBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		CBuyingSlotDlg* dlg = CreateForm<CBuyingSlotDlg>(g_pFresh->GetManager(),
			this, "buyslotdlg");
		dlg->SetSlotNum(item->idx + 1);
		dlg->Open(NULL, FrESCAPE);
		return;
	}
	if (slot->IsValid == 1 &&
		((m_returnBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_returnBtn[1].rect + item->pos).IsInRect(mouse) ||
			(m_deleteBtn[0].rect + item->pos).IsInRect(mouse) ||
			(m_deleteBtn[1].rect + item->pos).IsInRect(mouse)))
	{
		if (IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet((enumClientPacket)0xc8);
			packet.Encode4(Doc()->GetStallItem()->stallKey);
			packet.Encode2(1);
			packet.Encode1((unsigned char)(item->idx + 1));
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
		}
		return;
	}
	item->selected = !item->selected;
}

bool COfflineShopDlg::OnOfflineShopEditDlgCreateResult(int result, FrForm* form)
{
	if (result == 1)
	{
		sStallSlot* slot;
		if (!m_pSelectItem || !(slot = (sStallSlot*)m_pSelectItem->pData))
		{
			FrForm* notify =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
			notify->SetMessage(
				"\xbc\xb1\xc5\xc3\xc7\xd1 \xbd\xbd\xb7\xd4\xc0\xc7 \xc1\xa4\xba\xb8\xb0\xa1 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9. \xc6\xed\xc1\xfd \xbd\xc7\xc6\xd0\xc0\xd4\xb4\xcf\xb4\xd9.",
				false);
			notify->Open(NULL, FrESCAPE | FrENTER);
			return true;
		}
		m_bExit = 1;
		COfflineShopEditDlg* dlg = CreateForm<COfflineShopEditDlg>(
			g_pFresh->GetManager(), this, "tradeeditdlg");
		if (dlg)
		{
			dlg->Open(NULL, FrRET_OK | FrRET_CANCEL);
			dlg->SetSelectData(slot->stallKey, slot->Idx);
			dlg->SetFadeout(false);
			m_pSelectItem = NULL;
			FrForm::OnFreshOkay();
		}
	}
	else
		m_pSelectItem = NULL;
	return true;
}

void COfflineShopDlg::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	CIconToolTip::GetInstance()->ClearToolTip();
}

void COfflineShopDlg::SetListTradeItem(bool refresh)
{
	FrTradeDlg::SetListTradeItem(false);
	m_pTradeItemList->UseRightButton(false);
	sStall* stall = Doc()->GetStallItem();
	m_pTradeItemList->ClearItem();
	for (int i = 0; i < stall->maxSlotNum; ++i)
		m_pTradeItemList->AddItem(&stall->slotList[i]);
}

IMPLEMENT_OBJECT(COfflineShopVisitorDlg, FrForm)

BEGIN_FRESH_MSGMAP(COfflineShopVisitorDlg, FrForm)
ON_FRESH_VI("tradedlg", FRCMD_INIT, COfflineShopVisitorDlg::OnTradeDlgInit)
ON_FRESH_VI("visitor_back", FRCMD_INIT,
	COfflineShopVisitorDlg::OnVisitorBackInit)
ON_FRESH_VV("visitor_back", FRCMD_OWNERDRAW,
	COfflineShopVisitorDlg::OnVisitorBackOwnerDraw)
ON_FRESH_VI("mypang", FRCMD_INIT, COfflineShopVisitorDlg::OnMyPangInit)
ON_FRESH_VV("mypang", FRCMD_OWNERDRAW,
	COfflineShopVisitorDlg::OnMyPangOwnerDraw)
ON_FRESH_VI("trade_item_list", FRCMD_INIT,
	COfflineShopVisitorDlg::OnTradeItemListInit)
ON_FRESH_VV("trade_item_list", FRCMD_LBUTTONDOWN,
	COfflineShopVisitorDlg::OnTradeItemListLClick)
ON_FRESH_VV("trade_item_list", FRCMD_RBUTTONDOWN,
	COfflineShopVisitorDlg::OnTradeItemListRClick)
ON_FRESH_VI("trade_item_list", FRCMD_OWNERDRAW,
	COfflineShopVisitorDlg::OnTradeItemListOwnerDraw)
ON_FRESH_VI("noitem01", FRCMD_INIT, COfflineShopVisitorDlg::OnNoItem01Init)
ON_FRESH_VI("noitem02", FRCMD_INIT, COfflineShopVisitorDlg::OnNoItem02Init)
ON_FRESH_VI("visitor", FRCMD_INIT, COfflineShopVisitorDlg::OnVisitorInit)
ON_FRESH_VI("userid", FRCMD_INIT, COfflineShopVisitorDlg::OnUserIdInit)
ON_FRESH_VI("trade_title", FRCMD_INIT, COfflineShopVisitorDlg::OnTitleInit)
ON_FRESH_VI("edit", FRCMD_INIT, COfflineShopVisitorDlg::OnEditBnInit)
ON_FRESH_VV("edit", FRCMD_LBUTTONUP, COfflineShopVisitorDlg::OnEditBnUp)
ON_FRESH_VI("close", FRCMD_INIT, COfflineShopVisitorDlg::OnCloseBnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, COfflineShopVisitorDlg::OnCloseBnUp)
ON_FRESH_VI("hide", FRCMD_INIT, COfflineShopVisitorDlg::OnHideBnInit)
ON_FRESH_VV("hide", FRCMD_LBUTTONUP, COfflineShopVisitorDlg::OnHideBnUp)
ON_FRESH_VI("exit", FRCMD_INIT, COfflineShopVisitorDlg::OnExitBnInit)
ON_FRESH_VV("exit", FRCMD_LBUTTONUP, COfflineShopVisitorDlg::OnExitBnUp)
ON_FRESH_VI("tooltip", FRCMD_INIT, COfflineShopVisitorDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW,
	COfflineShopVisitorDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

COfflineShopVisitorDlg::COfflineShopVisitorDlg()
{
	m_pVisitor = 0;
	m_buyBtn[0].pBitmap = g_pFresh->GetBitmap("buy_btn_n");
	m_buyBtn[0].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[0].pBitmap->Width(),
		(float)m_buyBtn[0].pBitmap->Height());
	m_buyBtn[0].index = -1;
	m_buyBtn[1].pBitmap = g_pFresh->GetBitmap("buy_btn_o");
	m_buyBtn[1].rect = WRect(144.0f, 66.0f, (float)m_buyBtn[1].pBitmap->Width(),
		(float)m_buyBtn[1].pBitmap->Height());
	m_buyBtn[1].index = -1;
	m_tradeState[0].pBitmap = g_pFresh->GetBitmap("rmr_trade_state1");
	m_tradeState[0].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[0].pBitmap->Width(),
			(float)m_tradeState[0].pBitmap->Height());
	m_tradeState[1].pBitmap = g_pFresh->GetBitmap("rmr_trade_state2");
	m_tradeState[1].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[1].pBitmap->Width(),
			(float)m_tradeState[1].pBitmap->Height());
	m_tradeState[2].pBitmap = g_pFresh->GetBitmap("rmr_trade_state3");
	m_tradeState[2].rect =
		WRect(70.0f, 35.0f, (float)m_tradeState[2].pBitmap->Width(),
			(float)m_tradeState[2].pBitmap->Height());
}

COfflineShopVisitorDlg::~COfflineShopVisitorDlg()
{
}

void COfflineShopVisitorDlg::OnEditBnUp()
{
}

void COfflineShopVisitorDlg::OnVisitorBackOwnerDraw()
{
}

void COfflineShopVisitorDlg::OnCloseBnUp()
{
	FrForm::OnOK();
}

void COfflineShopVisitorDlg::OnVisitorInit(int param)
{
	m_pVisitor = DYNAMIC_CAST(FrEdit, param);
	if (IsLocalContent((localContentType_t)102) && Doc()->GetTradeMode() == 1)
		m_pVisitor->SetVisible(false);
}

void COfflineShopVisitorDlg::OnVisitorBackInit(int param)
{
	m_pVisitorBack = DYNAMIC_CAST(FrArea, param);
	if (m_pVisitorBack)
		m_pVisitorBack->SetVisible(false);
}

void COfflineShopVisitorDlg::OnTradeItemListOwnerDraw(int param)
{
	FrTradeDlg::OnTradeItemListOwnerDraw(param);
	FrGraphicInterface* graphic = g_pFresh->GetManager()->GetGDI();
	if (!graphic)
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	FrListItem* draw = (FrListItem*)param;
	if (!draw)
		return;
	sStallSlot* slot = (sStallSlot*)draw->pData;
	if (!slot)
		return;
	const Bitmap* bitmap = g_pFresh->GetBitmap("rmr_item_slot");
	if (slot->IsValid == 1 && slot->itemData.dwTid)
	{
		FILETIME end, now;
		SystemTimeToFileTime(&slot->time, &end);
		SystemTimeToFileTime(&Doc()->GetServerTime(), &now);
		__int64 hours = (((__int64)end.dwHighDateTime - now.dwHighDateTime) *
								0x100000000i64 -
							now.dwLowDateTime + end.dwLowDateTime) /
			60 / 60 / 10000000;
		if (hours >= 0)
		{
			if (!slot->IsPermanence)
			{
				graphic->DrawTexture(m_tradeState[2].pBitmap,
					m_tradeState[2].rect + draw->pos + WPoint(0, 0), 0xffffffff,
					0);
				graphic->SetTextColor(0xffffffff, 0xffffffff);
				if (hours / 24 > 1)
				{
					if (hours % 24)
					{
						WPoint pos = WPoint(m_tradeState[2].rect.x,
										 m_tradeState[2].rect.y) +
							draw->pos + WPoint(55, 6);
						graphic->Print(WPoint(pos), 2, "%d\xc0\xcf",
							hours / 24 + 1);
					}
					else
					{
						WPoint pos = WPoint(m_tradeState[2].rect.x,
										 m_tradeState[2].rect.y) +
							draw->pos + WPoint(55, 6);
						graphic->Print(WPoint(pos), 2, "%d\xc0\xcf",
							hours / 24);
					}
				}
				else
				{
					WPoint pos =
						WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
						draw->pos + WPoint(55, 6);
					graphic->Print(WPoint(pos), 2, "%d\xbd\xc3", hours);
				}
			}
			if (draw->underCursor &&
				(m_buyBtn[0].rect + draw->pos).IsInRect(mouse))
			{
				graphic->DrawTexture(m_buyBtn[1].pBitmap,
					m_buyBtn[1].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
				CIconToolTip::GetInstance()->SetToolTip((eBarButton)22);
			}
			else
				graphic->DrawTexture(m_buyBtn[0].pBitmap,
					m_buyBtn[0].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
		}
		else
		{
			graphic->DrawTexture(m_tradeState[2].pBitmap,
				m_tradeState[2].rect + draw->pos + WPoint(0, 0), 0xffffffff, 0);
			graphic->SetTextColor(0xffffffff, 0xffffffff);
			{
				WPoint pos =
					WPoint(m_tradeState[2].rect.x, m_tradeState[2].rect.y) +
					draw->pos + WPoint(55, 6);
				graphic->Print(WPoint(pos), 2,
					"\xb1\xb8\xb8\xc5 \xba\xd2\xb0\xa1");
			}
		}
	}
}

void COfflineShopVisitorDlg::OnTradeItemListLClick()
{
	FrListItem* item = m_pTradeItemList->GetItemUnderCursor();
	if (!item)
		return;
	sStallSlot* slot = (sStallSlot*)item->pData;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	FILETIME end, now;
	SystemTimeToFileTime(&slot->time, &end);
	SystemTimeToFileTime(&Doc()->GetServerTime(), &now);
	if (slot->IsValid == 1 &&
		((((__int64)end.dwHighDateTime - now.dwHighDateTime) * 0x100000000i64 -
			 now.dwLowDateTime + end.dwLowDateTime) /
					60 / 60 / 10000000 >=
				0 ||
			slot->IsPermanence))
	{
		WRect hit = m_buyBtn[0].rect + item->pos;
		if ((hit).IsInRect(mouse) ||
			(m_buyBtn[1].rect + item->pos).IsInRect(mouse))
		{
			COfflineBuyDlg* dlg = CreateForm<COfflineBuyDlg>(
				g_pFresh->GetManager(), this, "tradebuydlg");
			if (dlg)
			{
				dlg->Open((FRESH_PFN_RESULT)&COfflineShopVisitorDlg::
							  OnTradeBuyDlgResult,
					FrRET_OK);
				dlg->SetFadeout(false);
				sTradeItem* buyItem = &slot->itemData;
				dlg->SetBuyItem(buyItem);
				dlg->SetBuyitemData(*slot);
				dlg->SetControlsBuyItem(buyItem);
			}
			return;
		}
	}
	item->selected = !item->selected;
}

void COfflineShopVisitorDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, param);
}

void COfflineShopVisitorDlg::OnToolTipOnwerDraw(int param)
{
	CIconToolTip::GetInstance()->Render();
}

bool COfflineShopVisitorDlg::OnOfflineShopEditDlgCreateResult(int result,
	FrForm* form)
{
	return true;
}

void COfflineShopVisitorDlg::OnProc(const float delta)
{
	FrForm::OnProc(delta);
	CIconToolTip::GetInstance()->ClearToolTip();
}

void COfflineShopVisitorDlg::SetListTradeItem(bool refresh)
{
	FrTradeDlg::SetListTradeItem(false);
	sStall* stall = Doc()->GetStallItem();
	m_pTradeItemList->UseRightButton(false);
	m_pTradeItemList->ClearItem();
	for (int i = 0; i < stall->maxSlotNum; ++i)
		if (stall->slotList[i].IsValid == 1 &&
			stall->slotList[i].itemData.dwTid)
			m_pTradeItemList->AddItem(&stall->slotList[i]);
}

IMPLEMENT_OBJECT(COfflineBuyDlg, FrForm)

BEGIN_FRESH_MSGMAP(COfflineBuyDlg, FrForm)
ON_FRESH_VI("tradebuydlg", FRCMD_INIT, COfflineBuyDlg::OnTradeBuyDlgInit)
ON_FRESH_VI("ok", FRCMD_INIT, COfflineBuyDlg::OnOkBnInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, COfflineBuyDlg::OnOkBnUp)
ON_FRESH_VI("buyitem_icon", FRCMD_INIT, COfflineBuyDlg::OnBuyItemIconInit)
ON_FRESH_VV("buyitem_icon", FRCMD_OWNERDRAW,
	COfflineBuyDlg::OnBuyItemIconOwnerDraw)
ON_FRESH_VI("buyitem_name", FRCMD_INIT, COfflineBuyDlg::OnBuyItemNameInit)
ON_FRESH_VI("buyitem_ability", FRCMD_INIT, COfflineBuyDlg::OnBuyItemAbilityInit)
ON_FRESH_VI("my_pang", FRCMD_INIT, COfflineBuyDlg::OnMyPangInit)
ON_FRESH_VI("price", FRCMD_INIT, COfflineBuyDlg::OnPriceInit)
ON_FRESH_VI("pang_left", FRCMD_INIT, COfflineBuyDlg::OnPangLeftInit)
ON_FRESH_VI("num", FRCMD_INIT, COfflineBuyDlg::OnNumberInit)
ON_FRESH_VI("num_next", FRCMD_INIT, COfflineBuyDlg::OnNumberNextBnInit)
ON_FRESH_VV("num_next", FRCMD_LBUTTONUP, COfflineBuyDlg::OnNumberNextBnUp)
ON_FRESH_VI("num_prev", FRCMD_INIT, COfflineBuyDlg::OnNumberPrevBnInit)
ON_FRESH_VV("num_prev", FRCMD_LBUTTONUP, COfflineBuyDlg::OnNumberPrevBnUp)
END_FRESH_MSGMAP()

COfflineBuyDlg::COfflineBuyDlg()
{
}

COfflineBuyDlg::~COfflineBuyDlg()
{
}

void COfflineBuyDlg::OnOkBnUp()
{
	bool canBuy = true;
	m_count = 0;
	if (IsLocalContent((localContentType_t)102) && Doc()->GetTradeMode() == 1)
	{
		std::list<sStall>* list = Doc()->GetListStallItem();
		std::list<sStall>::iterator it;
		for (it = list->begin(); it != list->end(); it++)
			if ((*it).stallKey == m_buyItemData.stallKey)
				break;
		if (it != list->end())
		{
			m_count = atoi(m_pNumber->GetLine(1, false));
			if (m_count >= m_pBuyItem->iNum)
				m_count = m_pBuyItem->iNum;
			if (m_count <= 1)
				m_count = 1;
			if (m_buyItem.i64Price * m_buyItem.iNum >
				Doc()->m_myInfo.stat.i64Pang)
				canBuy = false;
			if (!Doc()->ConfirmInsertItem(m_buyItem.dwTid, m_buyItem.iNum))
			{
				FrForm* form =
					CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify");
				WRect rect = form->GetRect();
				form->SetRect(WRect(rect.x, rect.y, rect.w, rect.h));
				form->SetTimeLimit(12.f);
				form->SetMessage(
					"\xc7\xf6\xc0\xe7 \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xc7 \xba\xb8\xc0\xaf\xb0\xb3\xbc\xf6\xb0\xa1 20,000\xb0\xb3\xc0\xcc\xb9\xc7\xb7\xce \xbe\xc6\xc0\xcc\xc5\xdb\xc0\xbb \xb1\xb8\xc0\xd4 \xb6\xc7\xb4\xc2 \xbf\xc5\xb1\xe6 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.",
					false);
				form->Open(NULL, 0);
			}
			else if (canBuy)
			{
				FrForm* form = CreateForm<FrForm>(g_pFresh->GetManager(), this,
					"notify_okcancel");
				form->SetMessage(
					"\\c0xffff0000\\c\xc1\xa4\xb8\xbb \xb1\xb8\xc0\xd4\xc7\xcf\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
					false);
				form->Open((FRESH_PFN_RESULT)&COfflineBuyDlg::OnConfirmResult,
					FrESCAPE | FrENTER);
				return;
			}
		}
		FrForm::OnFreshCancel();
	}
	else
		FrTradeBuyDlg::OnOkBnUp();
}

bool COfflineBuyDlg::OnConfirmResult(int result, FrForm* form)
{
	if (result == 1 && form)
	{
		if (IsLocalContent((localContentType_t)102))
		{
			WSendPacket packet((enumClientPacket)0xc6);
			packet.Encode4(Doc()->m_replayTargetUID);
			packet.Encode4(m_buyItemData.stallKey);
			packet.Encode1(m_buyItemData.Idx);
			packet.Encode2((unsigned short)m_count);
			packet.Encode8(m_buyItemData.itemData.i64Price);
			packet.Send(TO_GAME);
			AfxGetTask()->SendMsgToMainActor(MsgObject(NULL, 1, 0, 0, 0, 0, 0));
			m_count = 0;
		}
		FrForm::OnFreshOkay();
	}
	return true;
}
