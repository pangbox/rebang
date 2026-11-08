#include "scoredlg.h"
#include "projectg.h"
#include "golfdoc.h"
#include "netresourcemanager.h"

extern Fresh* g_pFresh;

void ResizeScoreBar23(FrArea* bar2, FrArea* bar3, float x, float w)
{
	if (bar2 && bar3)
	{
		WRect rect;
		rect = bar3->GetRect();
		w = w - 7.0f - rect.w;
		rect.x = x + w;
		bar3->SetRect(rect);
		rect = bar2->GetRect();
		rect.w = w - (rect.x - x);
		bar2->SetRect(rect);
	}
}

IMPLEMENT_OBJECT(FrScoreDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrScoreDlg, FrForm)

ON_FRESH_VI("front9btn", FRCMD_INIT, FrScoreDlg::OnFront9BtnInit)
ON_FRESH_VV("front9btn", FRCMD_LBUTTONUP, FrScoreDlg::OnFront9BtnUp)
ON_FRESH_VI("back9btn", FRCMD_INIT, FrScoreDlg::OnBack9BtnInit)
ON_FRESH_VV("back9btn", FRCMD_LBUTTONUP, FrScoreDlg::OnBack9BtnUp)
ON_FRESH_VI("shotbtn", FRCMD_INIT, FrScoreDlg::OnShotBtnInit)
ON_FRESH_VV("shotbtn", FRCMD_LBUTTONUP, FrScoreDlg::OnShotBtnUp)
ON_FRESH_VI("graphbtn", FRCMD_INIT, FrScoreDlg::OnGraphBtnInit)
ON_FRESH_VV("graphbtn", FRCMD_LBUTTONUP, FrScoreDlg::OnGraphBtnUp)
ON_FRESH_VI("pangbtn", FRCMD_INIT, FrScoreDlg::OnPangBtnInit)
ON_FRESH_VV("pangbtn", FRCMD_LBUTTONUP, FrScoreDlg::OnPangBtnUp)
ON_FRESH_VI("versusbtn", FRCMD_INIT, FrScoreDlg::OnVersusBtnInit)
ON_FRESH_VV("versusbtn", FRCMD_LBUTTONUP, FrScoreDlg::OnVersusBtnUp)
ON_FRESH_VI("puttbtn", FRCMD_INIT, FrScoreDlg::OnPuttBtnInit)
ON_FRESH_VI("hole_list", FRCMD_INIT, FrScoreDlg::OnHoleListInit)
ON_FRESH_VI("hole_list", FRCMD_OWNERDRAW, FrScoreDlg::OnHoleListOwnerDraw)
ON_FRESH_VI("par_list", FRCMD_INIT, FrScoreDlg::OnParListInit)
ON_FRESH_VI("par_list", FRCMD_OWNERDRAW, FrScoreDlg::OnParListOwnerDraw)
ON_FRESH_VI("golfer_list", FRCMD_INIT, FrScoreDlg::OnGolferListInit)
ON_FRESH_VI("golfer_list", FRCMD_OWNERDRAW, FrScoreDlg::OnGolferListOwnerDraw)
ON_FRESH_VI("record_list", FRCMD_INIT, FrScoreDlg::OnRecordListInit)
ON_FRESH_VI("record_list", FRCMD_OWNERDRAW, FrScoreDlg::OnRecordListOwnerDraw)
ON_FRESH_VI("total_list", FRCMD_INIT, FrScoreDlg::OnTotalListInit)
ON_FRESH_VI("total_list", FRCMD_OWNERDRAW, FrScoreDlg::OnTotalListOwnerDraw)
ON_FRESH_VI("cancel", FRCMD_INIT, FrScoreDlg::OnCancelBtnInit)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrScoreDlg::OnCancelBtnUp)
ON_FRESH_VI("close", FRCMD_INIT, FrScoreDlg::OnCloseBtnInit)
ON_FRESH_VV("close", FRCMD_LBUTTONUP, FrScoreDlg::OnCloseBtnUp)
ON_FRESH_VI("blankpanel1", FRCMD_INIT, FrScoreDlg::OnBlankPanel01Init)
ON_FRESH_VI("blankpanel1", FRCMD_OWNERDRAW, FrScoreDlg::OnBlankPanel01OwnerDraw)
ON_FRESH_VI("blankpanel2", FRCMD_INIT, FrScoreDlg::OnBlankPanel02Init)
ON_FRESH_VI("blankpanel2", FRCMD_OWNERDRAW, FrScoreDlg::OnBlankPanel02OwnerDraw)
ON_FRESH_VI("blankpanel3", FRCMD_INIT, FrScoreDlg::OnBlankPanel03Init)
ON_FRESH_VI("blankpanel3", FRCMD_OWNERDRAW, FrScoreDlg::OnBlankPanel03OwnerDraw)

END_FRESH_MSGMAP()

FrScoreDlg::FrScoreDlg()
{
	m_bFront9 = true;
	m_pBar2 = NULL;
	m_pBar3 = NULL;
	m_pFront9Btn = NULL;
	m_pBack9Btn = NULL;
	m_pUnknown120 = NULL;
	m_pUnknown124 = NULL;
	m_pUnknown128 = NULL;
	m_pUnknown12c = NULL;
	m_pCancelBtn = NULL;
	m_pCloseBtn = NULL;
	m_type = Doc()->m_golfGame.gameType == 6 ? 3 : 0;
	m_aniTime = 0;
	m_pPoint = NULL;
	m_pMyData = NULL;
	memset(m_pTypeBtn, 0, sizeof(m_pTypeBtn));
	memset(m_pList, 0, sizeof(m_pList));
	memset(m_pDigit, 0, sizeof(m_pDigit));
	memset(m_versusOID, 0, sizeof(m_versusOID));
	LoadBitmaps();
	m_bShowBlankPanel = Doc()->m_roomInfo.realGameType == 14;
	m_pBlankPanel[0] = NULL;
	m_pBlankPanel[1] = NULL;
	m_pBlankPanel[2] = NULL;
}

void FrScoreDlg::SetPlayer(unsigned long uid)
{
	if (Doc()->GetIndex(uid) == 0xff)
		return;
	sRivalData* rival = &Doc()->m_rivalList[Doc()->GetIndex(uid)];
	for (int i = 0; i < 5; ++i)
		if (m_pList[i])
		{
			m_pList[i]->ClearItem();
			m_pList[i]->AddItem(rival);
		}
	m_bFront9 = Doc()->m_golfGame.holes <= 9;
	SetFrontBackBtn();
	Resize();
}

void FrScoreDlg::SetVisibleCloseBtn(bool bVisible)
{
	if (m_pCloseBtn)
		m_pCloseBtn->SetVisible(bVisible);
}

void FrScoreDlg::SetVisibleCancelBtn(bool bVisible)
{
	if (m_pCancelBtn)
		m_pCancelBtn->SetVisible(bVisible);
}

void FrScoreDlg::SetGuildPlayer(unsigned long uid)
{
	if (0)
		DrawVersus(NULL);
	sRivalData* left = NULL;
	sRivalData* right = NULL;
	if (uid == 0xffffff)
	{
		left = &Doc()->m_rivalList[Doc()->GetIndex(Doc()->m_teamPlayerGuid[0])];
		right =
			&Doc()->m_rivalList[Doc()->GetIndex(Doc()->m_teamPlayerGuid[1])];
	}
	else
	{
		for (std::vector<sGuildMatchup>::iterator it =
				 Doc()->m_guildMatchupList.begin();
			it != Doc()->m_guildMatchupList.end(); ++it)
		{
			if (uid == _ITER_BASE(it)->uid[0] || uid == _ITER_BASE(it)->uid[1])
			{
				left =
					&Doc()
						 ->m_rivalList[Doc()->GetIndex(_ITER_BASE(it)->uid[0])];
				right =
					&Doc()
						 ->m_rivalList[Doc()->GetIndex(_ITER_BASE(it)->uid[1])];
				break;
			}
		}
		if (!left && !right)
		{
			left =
				&Doc()
					 ->m_rivalList[Doc()->GetIndex(Doc()->m_teamPlayerGuid[0])];
			right =
				&Doc()
					 ->m_rivalList[Doc()->GetIndex(Doc()->m_teamPlayerGuid[1])];
		}
	}
	if (!left && !right)
		return;
	m_pMyData = NULL;
	if (left->oid == Doc()->m_myInfo.info.dwGuid)
		m_pMyData = left;
	else if (right->oid == Doc()->m_myInfo.info.dwGuid)
		m_pMyData = right;
	m_versusOID[0] = left->oid;
	m_versusOID[1] = right->oid;
	for (int i = 0; i < 5; ++i)
		if (m_pList[i])
		{
			m_pList[i]->ClearItem();
			m_pList[i]->AddItem(left);
			m_pList[i]->AddItem(right);
		}
	if (Doc()->m_golfGame.holes == 18)
	{
		bool front = (GetHoleIndex(Doc()->m_pGolfDoc->m_currentHole) < 10 ||
			Doc()->m_pGolfDoc->m_bNoStat);
		m_bFront9 = front;
	}
	else
		m_bFront9 = true;
	SetFrontBackBtn();
	Resize();
	m_pCancelBtn->SetVisible(true);
}

void FrScoreDlg::Resize()
{
	WRect rect = GetRect();
	rect.h = Doc()->m_golfGame.gameType == 6 ? 255.0f : 197.0f;
	ResizeScoreBar23(m_pBar2, m_pBar3, rect.x, rect.w);
	SetRect(rect);
}

void FrScoreDlg::LoadBitmaps()
{
	for (int i = 0; i < 10; ++i)
		m_pDigit[i] =
			g_pFresh->GetManager()->GetBitmap("SCORECARD", MakeStr("%d", i));
	m_pPlus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "+");
	m_pMinus = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
	m_pPoint = g_pFresh->GetManager()->GetBitmap("SCORECARD", "point");
	m_pWin = g_pFresh->GetManager()->GetBitmap("SCORECARD", "win");
	m_pLose = g_pFresh->GetManager()->GetBitmap("SCORECARD", "lose");
	m_pDraw = g_pFresh->GetManager()->GetBitmap("SCORECARD", "draw");
	m_pNoResult = g_pFresh->GetManager()->GetBitmap("SCORECARD", "-");
	m_pPCIcon = g_pFresh->GetManager()->GetBitmap("golf", "pc_icon_mass");
}

void FrScoreDlg::OnBar2Init(int param)
{
	m_pBar2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrScoreDlg::OnBar3Init(int param)
{
	m_pBar3 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrScoreDlg::OnFront9BtnInit(int param)
{
	m_pFront9Btn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	SetFrontBackBtn();
}

void FrScoreDlg::OnFront9BtnUp()
{
	m_bFront9 = true;
	SetFrontBackBtn();
}

void FrScoreDlg::OnBack9BtnUp()
{
	if (Doc()->m_golfGame.holes > 9)
	{
		m_bFront9 = false;
		SetFrontBackBtn();
	}
}

void FrScoreDlg::OnShotBtnUp()
{
	if (m_type != 0)
	{
		m_type = 0;
		SetTypeBtn();
	}
}

void FrScoreDlg::OnGraphBtnUp()
{
	if (m_type != 1)
	{
		m_type = 1;
		SetTypeBtn();
	}
}

void FrScoreDlg::OnPangBtnUp()
{
	if (m_type != 2)
	{
		m_type = 2;
		SetTypeBtn();
	}
}

void FrScoreDlg::OnVersusBtnUp()
{
	if (m_type != 3)
	{
		m_type = 3;
		SetTypeBtn();
	}
}

void FrScoreDlg::OnPuttBtnInit(int param)
{
	FrButton* button = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	button->SetVisible(false);
}

void FrScoreDlg::OnBack9BtnInit(int param)
{
	m_pBack9Btn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (Doc()->m_golfGame.holes <= 9)
		m_pBack9Btn->SetVisible(false);
	SetFrontBackBtn();
}

void FrScoreDlg::OnShotBtnInit(int param)
{
	m_pTypeBtn[0] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (Doc()->m_golfGame.gameType == 6)
		m_pTypeBtn[0]->SetVisible(false);
}

void FrScoreDlg::OnGraphBtnInit(int param)
{
	m_pTypeBtn[1] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (Doc()->m_golfGame.gameType == 6 && m_pTypeBtn[1])
		m_pTypeBtn[1]->SetVisible(false);
}

void FrScoreDlg::OnPangBtnInit(int param)
{
	m_pTypeBtn[2] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrScoreDlg::OnVersusBtnInit(int param)
{
	m_pTypeBtn[3] = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pTypeBtn[3] && Doc()->m_golfGame.gameType != 6)
		m_pTypeBtn[3]->SetVisible(false);
}

void FrScoreDlg::OnHoleListInit(int param)
{
	m_pList[4] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrScoreDlg::OnHoleListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	float width = (float)m_pList[4]->GetItemWidth();
	float height = (float)m_pList[4]->GetItemHeight();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	WPoint pos;
	pos.x = width / 18.0f + item->pos.x;
	pos.y = height * 0.5f + item->pos.y - 6.0f;
	if (!item->pData)
		return;
	for (unsigned char i = m_bFront9 ? 0 : 9; i < (m_bFront9 ? 9 : 18); ++i)
	{
		if (i <= GetHoles() && Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			DrawDigit(gdi, MakeStr("%d", Doc()->m_holeOrder[i]), pos,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
			pos.x += width / 9.0f;
		}
	}
}

void FrScoreDlg::OnParListInit(int param)
{
	m_pList[3] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrScoreDlg::OnParListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	float width = (float)m_pList[3]->GetItemWidth();
	float height = (float)m_pList[3]->GetItemHeight();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	float offset = width / 18.0f;
	WPoint pos;
	pos.x = offset + item->pos.x;
	pos.y = height * 0.5f + item->pos.y - 6.0f;
	if (!item->pData)
		return;
	int total = 0;
	for (int i = m_bFront9 ? 0 : 9; i < (m_bFront9 ? 9 : 18); ++i)
	{
		if (i <= Doc()->m_golfGame.holes &&
			Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			total += Doc()->m_holePar[Doc()->m_holeOrder[i] - 1];
			DrawDigit(gdi,
				MakeStr("%d", Doc()->m_holePar[Doc()->m_holeOrder[i] - 1]), pos,
				0xff9ff5b8, FrGDI::ALIGN_CENTER, false);
		}
		pos.x += 38.0f;
	}
	DrawDigit(gdi, MakeStr("%d", total),
		WPoint(offset + item->pos.x + 355.0f, pos.y), 0xff9ff5b8,
		FrGDI::ALIGN_CENTER, false);
}

void FrScoreDlg::OnGolferListInit(int param)
{
	m_pList[0] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrScoreDlg::OnGolferListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (item)
	{
		FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
		if (gdi)
		{
			sRivalData* rival = (sRivalData*)item->pData;
			if (rival)
			{
				WPoint pos;
				WRect rect;
				rect.x = item->pos.x;
				rect.y = item->pos.y;
				rect.w = (float)m_pList[0]->GetItemWidth();
				rect.h = (float)m_pList[0]->GetItemHeight();
				if (rival->guildUID)
				{
					const Bitmap* emblem =
						NetResourceManager::Instance()->GetEmblemByName(
							rival->guildMark);
					if (emblem)
						gdi->DrawTexture(emblem,
							WRect(item->pos.x, item->pos.y + 20.0f,
								(float)emblem->Width(),
								(float)emblem->Height()),
							0xffffffff, 0);
				}
				if (!CProjectG::Instance()->HidePrivacy())
				{
					gdi->Box(rect, 0x809b9dff);
					pos.x = item->pos.x + 26.0f;
					pos.y = item->pos.y + 26.0f;
					gdi->SetTextColor(0xff000000, 0xffffffff);
					gdi->SetTextStyle(0);
					g_pFresh->GetManager()->PrintText(pos, 0, rival->nickname,
						100.0f, 0xffffffff);
					sRivalData* user = (sRivalData*)item->pData;
					if (m_pPCIcon && user && (user->capability & 0x80))
					{
						WRect icon;
						icon.x = item->pos.x + 26.0f;
						icon.y = item->pos.y + 13.0f;
						icon.w = (float)m_pPCIcon->Width();
						icon.h = (float)m_pPCIcon->Height();
						gdi->DrawTexture(m_pPCIcon, icon, 0xffffffff, 0);
					}
				}
			}
		}
	}
}

void FrScoreDlg::OnRecordListInit(int param)
{
	m_pList[1] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrScoreDlg::OnRecordListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi || !item->pData)
		return;
	gdi->Box(WRect(item->pos.x, item->pos.y, m_pList[1]->GetItemWidth(),
				 m_pList[1]->GetItemHeight()),
		0x809b9dff);
	switch (m_type)
	{
	case 0:
		DrawShot(item);
		break;
	case 1:
		DrawGraph(item);
		break;
	case 2:
		DrawPang(item);
		break;
	case 3:
		DrawVersus(item);
		break;
	}
}

void FrScoreDlg::OnTotalListInit(int param)
{
	m_pList[2] = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrScoreDlg::OnTotalListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	float width = (float)m_pList[2]->GetItemWidth();
	float height = (float)m_pList[2]->GetItemHeight();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	sRivalData* rival = (sRivalData*)item->pData;
	if (!rival)
		return;
	gdi->Box(WRect(item->pos.x, item->pos.y, m_pList[2]->GetItemWidth(),
				 m_pList[2]->GetItemHeight()),
		0x809b9dff);
	WPoint pos;
	pos.y = height * 0.5f + item->pos.y - 6.0f;
	pos.x = width * 0.25f + item->pos.x;
	switch (m_type)
	{
	case 2:
	{
		DrawDigit(gdi, MakeStr("%I64d", rival->totalPang), pos, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		WPoint right;
		right.x = width * 0.75f + item->pos.x;
		right.y = pos.y;
		char* format = "%d";
		if (rival->totalScore)
			format = "%+I64d";
		DrawDigit(gdi, MakeStr(format, rival->totalPang), right, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		break;
	}
	case 0:
	case 1:
	{
		int current = GetMyCurHoleInGuildMatch();
		if (Doc()->m_golfGame.gameType == 6 && rival != m_pMyData)
		{
			int strokes = 0;
			int score = 0;
			for (int i = 0; i < current; ++i)
			{
				strokes += rival->holeStroke[Doc()->m_holeOrder[i] - 1];
				score += rival->holeScore[Doc()->m_holeOrder[i] - 1];
			}
			DrawDigit(gdi, MakeStr("%d", strokes), pos, 0xffffffff,
				FrGDI::ALIGN_CENTER, false);
			WPoint right;
			right.x = width * 0.75f + item->pos.x;
			right.y = pos.y;
			char* format = "%d";
			if (rival->totalScore)
				format = "%+d";
			DrawDigit(gdi, MakeStr(format, score), right, 0xffffffff,
				FrGDI::ALIGN_CENTER, false);
		}
		else
		{
			DrawDigit(gdi, MakeStr("%d", rival->totalStroke), pos, 0xffffffff,
				FrGDI::ALIGN_CENTER, false);
			WPoint right;
			right.x = width * 0.75f + item->pos.x;
			right.y = pos.y;
			char* format = "%d";
			if (rival->totalScore)
				format = "%+d";
			DrawDigit(gdi, MakeStr(format, rival->totalScore), right,
				0xffffffff, FrGDI::ALIGN_CENTER, false);
		}
		break;
	}
	case 3:
		DrawDigit(gdi, MakeStr("%d", rival->guildPoint), pos, 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		DrawDigit(gdi, MakeStr("%d", rival->guildPoint),
			WPoint(width * 0.75f + item->pos.x, pos.y), 0xffffffff,
			FrGDI::ALIGN_CENTER, false);
		break;
	}
}

void FrScoreDlg::OnCancelBtnInit(int param)
{
	m_pCancelBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pCancelBtn->SetVisible(false);
}

void FrScoreDlg::OnCancelBtnUp()
{
	Close(FrOK, true);
}

void FrScoreDlg::OnCloseBtnInit(int param)
{
	if (!m_pCloseBtn)
		m_pCloseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	if (m_pCloseBtn)
		m_pCloseBtn->SetVisible(false);
}

void FrScoreDlg::OnCloseBtnUp()
{
	Close(FrOK, true);
}

void FrScoreDlg::OnBlankPanel01Init(int param)
{
	m_pBlankPanel[0] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[0])
		m_pBlankPanel[0]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnBlankPanel01OwnerDraw(int param)
{
	if (m_pBlankPanel[0])
		m_pBlankPanel[0]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnBlankPanel02Init(int param)
{
	m_pBlankPanel[1] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[1])
		m_pBlankPanel[1]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnBlankPanel02OwnerDraw(int param)
{
	if (m_pBlankPanel[1])
		m_pBlankPanel[1]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnBlankPanel03Init(int param)
{
	m_pBlankPanel[2] = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	if (m_pBlankPanel[2])
		m_pBlankPanel[2]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnBlankPanel03OwnerDraw(int param)
{
	if (m_pBlankPanel[2])
		m_pBlankPanel[2]->SetVisible(m_bShowBlankPanel);
}

void FrScoreDlg::OnProc(const float delta)
{
	if (m_aniTime < 5.0f)
		m_aniTime += delta;
}

void FrScoreDlg::OnDraw()
{
	float height = Doc()->m_golfGame.gameType == 6 ? 116.0f : 58.0f;
	WPoint points[2];
	WPoint& p1 = points[0];
	WPoint& p2 = points[1];
	p2.x = GetRect().x + 129.0f;
	p1.x = p2.x;
	p1.y = GetRect().y + 126.0f;
	p2.y = p1.y + height;
	for (int i = 0; i < 10; ++i)
	{
		g_view->DrawLine2D(p1, p2, 0xff000000, 0xff000000, 0);
		p1.x += 38.0f;
		p2.x += 38.0f;
	}
	p1.x += 27.0f;
	p2.x += 27.0f;
	g_view->DrawLine2D(p1, p2, 0xff000000, 0xff000000, 0);
}

void FrScoreDlg::SetFrontBackBtn()
{
	m_aniTime = 0;
	if (m_pFront9Btn)
		m_pFront9Btn->SetStatus(
			m_bFront9 ? FrButton::PRESSED : FrButton::NORMAL);
	if (m_pBack9Btn)
		m_pBack9Btn->SetStatus(
			m_bFront9 ? FrButton::NORMAL : FrButton::PRESSED);
}

void FrScoreDlg::SetTypeBtn()
{
	m_aniTime = 0;
	for (int i = 0; i < 4; ++i)
		if (m_pTypeBtn[i])
			m_pTypeBtn[i]->SetStatus(
				m_type == i ? FrButton::PRESSED : FrButton::NORMAL);
}

void FrScoreDlg::DrawShot(FrListItem* item)
{
	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int count = 0;
	int current = GetMyCurHoleInGuildMatch();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	sRivalData* rival = (sRivalData*)item->pData;
	if (!rival)
		return;
	int start = m_bFront9 ? 0 : 9;
	int end = start - (int)(m_aniTime * -20.0f) + 1;
	if (end > start + 9)
		end = start + 9;
	for (int i = start; i < end; ++i)
	{
		if (i <= Doc()->m_golfGame.holes &&
			Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			if (Doc()->m_golfGame.gameType == 6 && i >= current)
				break;
			if (rival->holeStroke[Doc()->m_holeOrder[i] - 1] > 0)
			{
				int column = count < 9 ? count : count - 9;
				WPoint pos;
				pos.x = (column + 0.5f) * width / 9.0f + item->pos.x;
				pos.y = height * 0.5f + item->pos.y - 6.0f;
				unsigned long color;
				switch (rival->holeScore[Doc()->m_holeOrder[i] - 1])
				{
				case -4:
				case -3:
				case -2:
					color = 0xffff4f4f;
					break;
				case -1:
					color = 0xffffbd42;
					break;
				case 0:
					color = Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] <
							rival->holeStroke[Doc()->m_holeOrder[i] - 1]
						? 0xff6f6f71
						: 0xffffffff;
					break;
				case 1:
					color = 0xffb9b9b9;
					break;
				default:
					color = 0xff6f6f71;
					break;
				}
				DrawDigit(gdi,
					MakeStr("%d", rival->holeStroke[Doc()->m_holeOrder[i] - 1]),
					pos, color, FrGDI::ALIGN_CENTER, false);
			}
			++count;
		}
	}
}

void FrScoreDlg::DrawGraph(FrListItem* item)
{
	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	float middle = height * 0.5f + item->pos.y;
	float scale = (height - 5.0f) / 6.0f;
	int current = GetMyCurHoleInGuildMatch();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	_WPOINT points[2];
	_WPOINT& p1 = points[0];
	_WPOINT& p2 = points[1];
	p1.x = item->pos.x;
	p2.x = item->pos.x + width;
	p2.y = middle;
	p1.y = middle;
	g_view->DrawLine2D(p1, p2, 0xff95ffb3, 0xff95ffb3, 0);
	sRivalData* rival = (sRivalData*)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = start - (int)(m_aniTime * -20.0f) + 1;
	if (end > start + 9)
		end = start + 9;
	int value = rival->holeScore[Doc()->m_holeOrder[start] - 1];
	int score;
	if (value < -3)
		score = -3;
	else if (value > 3)
		score = 3;
	else
		score = value;
	unsigned long color = GetScoreColor(score);
	p1.x = width / 18.0f + item->pos.x;
	p1.y = score * scale + middle;
	for (int i = start + 1; i < end; ++i)
	{
		if (i <= Doc()->m_golfGame.holes &&
			Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			if (Doc()->m_golfGame.gameType == 6 && i >= current - 1)
				break;
			if (rival->holeStroke[Doc()->m_holeOrder[i] - 1] > 0)
			{
				int value = rival->holeScore[Doc()->m_holeOrder[i] - 1];
				if (value < -3)
					score = -3;
				else if (value > 3)
					score = 3;
				else
					score = value;
				unsigned long nextColor = GetScoreColor(score);
				p2.x = p1.x + width / 9.0f;
				p2.y = score * scale + middle;
				g_view->DrawLine2D(p1, p2, color, nextColor, 0);
				p1 = p2;
				color = nextColor;
			}
		}
	}
	if (m_pPoint)
	{
		WRect rect(0, 0, 4, 4);
		int end = start - (int)(m_aniTime * -20.0f) + 1;
		if (end > start + 9)
			end = start + 9;
		int count = 0;
		for (int i = start; i < end; ++i)
		{
			if (i <= Doc()->m_golfGame.holes &&
				Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
			{
				if (Doc()->m_golfGame.gameType == 6 && i >= current)
					break;
				if (rival->holeStroke[Doc()->m_holeOrder[i] - 1] > 0)
				{
					int value = rival->holeScore[Doc()->m_holeOrder[i] - 1];
					if (value < -3)
						score = -3;
					else
					{
						score = 3;
						if (value <= 3)
							score = value;
					}
					int column = count < 9 ? count : count - 9;
					rect.x =
						(column + 0.5f) * width / 9.0f + item->pos.x - 2.0f;
					rect.y = score * scale + middle - 2.0f;
					gdi->DrawTexture(m_pPoint, rect, 0xff696969, 0);
				}
				++count;
			}
		}
	}
}

void FrScoreDlg::DrawPang(FrListItem* item)
{
	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int count = 0;
	int current = GetMyCurHoleInGuildMatch();
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	sRivalData* rival = (sRivalData*)item->pData;
	int start = m_bFront9 ? 0 : 9;
	int end = start - (int)(m_aniTime * -20.0f) + 1;
	if (end > start + 9)
		end = start + 9;
	for (int i = start; i < end; ++i)
	{
		if (i <= Doc()->m_golfGame.holes &&
			Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			if (Doc()->m_golfGame.gameType == 6 && i >= current)
				break;
			if (rival->holeStroke[Doc()->m_holeOrder[i] - 1] > 0)
			{
				int column = count < 9 ? count : count - 9;
				WPoint pos;
				pos.x = (float)(int)((column + 0.5f) * width / 9.0f +
					item->pos.x + 2.0f);
				pos.y = (float)(int)(height * 0.5f + item->pos.y - 6.0f);
				DrawDigit(gdi,
					MakeStr("%d",
						(int)rival->holePang[Doc()->m_holeOrder[i] - 1]),
					pos, 0xffe98919, FrGDI::ALIGN_CENTER, true);
			}
			++count;
		}
	}
}

void FrScoreDlg::DrawVersus(FrListItem* item)
{
	float width = (float)m_pList[1]->GetItemWidth();
	float height = (float)m_pList[1]->GetItemHeight();
	int count = 0;
	FrGraphicInterface* gdi = g_pFresh->GetManager()->GetGDI();
	if (!gdi)
		return;
	sRivalData* rival = (sRivalData*)item->pData;
	sRivalData* other = rival->oid == m_versusOID[0]
		? &Doc()->m_rivalList[Doc()->GetIndex(m_versusOID[1])]
		: &Doc()->m_rivalList[Doc()->GetIndex(m_versusOID[0])];
	if (!other)
		return;
	int start = m_bFront9 ? 0 : 9;
	int end = start - (int)(m_aniTime * -20.0f) + 1;
	if (end > start + 9)
		end = start + 9;
	for (int i = start; i < end; ++i)
	{
		if (Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
		{
			int column = count < 9 ? count : count - 9;
			WRect dest;
			WPoint pos;
			pos.x = (column + 0.5f) * width / 9.0f + item->pos.x;
			pos.y = height * 0.5f + item->pos.y - 6.0f;
			const Bitmap* bitmap;
			switch (GetGuildVersus(rival, other, Doc()->m_holeOrder[i] - 1))
			{
			case 0:
				bitmap = m_pWin;
				break;
			case 1:
				bitmap = m_pLose;
				break;
			case 2:
				bitmap = m_pDraw;
				break;
			case 3:
				bitmap = m_pNoResult;
				break;
			default:
				++count;
				continue;
			}
			if (bitmap)
			{
				dest.x = (float)(int)(pos.x - bitmap->Width() * 0.5f);
				dest.y = pos.y;
				dest.w = (float)bitmap->Width();
				dest.h = (float)bitmap->Height();
				gdi->DrawTexture(bitmap, dest, 0xffffffff, 0);
			}
			++count;
		}
	}
}

void FrScoreDlg::DrawDigit(FrGraphicInterface* gdi, char* text, WPoint pos,
	unsigned long color, FrGDI::eTextAlign align, bool narrow)
{
	int len = strlen(text);
	const Bitmap* digits[16];
	int width = 0;
	if (align)
	{
		width = 0;
		for (int i = 0; i < len; ++i)
		{
			switch (text[i])
			{
			case '+':
				digits[i] = m_pPlus;
				break;
			case '-':
				digits[i] = m_pMinus;
				break;
			default:
				digits[i] = m_pDigit[text[i] - '0'];
				break;
			}
			width += digits[i]->Width() - (narrow ? (len < 4 ? 0 : 2) : 0);
		}
	}
	WRect rect;
	switch (align)
	{
	case FrGDI::ALIGN_LEFT:
		rect.x = (float)(int)pos.x;
		break;
	case FrGDI::ALIGN_CENTER:
		rect.x = (float)(int)(pos.x - width * 0.5f);
		break;
	case FrGDI::ALIGN_RIGHT:
		rect.x = (float)(int)(pos.x - width);
		break;
	}
	rect.y = pos.y;
	for (int i = 0; i < len; ++i)
	{
		rect.w = (float)digits[i]->Width();
		rect.h = (float)digits[i]->Height();
		gdi->DrawTexture(digits[i], rect, color, 0);
		rect.x += rect.w - (narrow ? (len < 4 ? 0 : 2) : 0);
	}
}

int FrScoreDlg::GetGuildVersus(sRivalData* left, sRivalData* right, int hole)
{
	if (left->state && right->state)
	{
		if (!left->holeStroke[hole] && !right->holeStroke[hole])
			return 3;
		if (!left->holeStroke[hole] && right->holeStroke[hole])
			return 1;
		if (left->holeStroke[hole] && !right->holeStroke[hole])
			return 0;
	}
	if (!left->holeStroke[hole] || !right->holeStroke[hole])
		return 3;
	if (left->holeStroke[hole] < right->holeStroke[hole])
		return 0;
	if (left->holeStroke[hole] > right->holeStroke[hole])
		return 1;
	if (left->holeStroke[hole] == right->holeStroke[hole])
		return 2;
	return 3;
}

unsigned long FrScoreDlg::GetScoreColor(int score)
{
	switch (score)
	{
	case -4:
	case -3:
	case -2:
		return 0xffff0000;
	case -1:
		return 0xffffbb4f;
	case 0:
		return 0xffffffff;
	case 1:
		return 0xff82c3ff;
	default:
		return 0xff0036ff;
	}
}

int FrScoreDlg::GetMyCurHoleInGuildMatch()
{
	if (Doc()->m_golfGame.gameType != 6)
		return 0;
	if (!m_pMyData)
		return 0;
	int start = m_bFront9 ? 0 : 9;
	int end = start - (int)(m_aniTime * -20.0f) + 1;
	if (end > start + 9)
		end = start + 9;
	int i;
	for (i = start; i < end; ++i)
	{
		if (i <= Doc()->m_golfGame.holes &&
			Doc()->m_holePar[Doc()->m_holeOrder[i] - 1] > 0)
			if (m_pMyData->holeStroke[Doc()->m_holeOrder[i] - 1] <= 0)
				break;
	}
	return i;
}
