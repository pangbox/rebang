#include "uccclothes.h"
#include "uccdrawdlg.h"
#include "uccpickcolordlg.h"
#include "uccsetinfodlg.h"
#include "securitykeymanager.h"
#include "notifyform.h"
#include "uccmanager.h"
#include "netresourcemanager.h"
#include "mathconsts.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrUccDrawDlg, FrForm)

FrUccDrawDlg::FrUccDrawDlg()
{
	Init();
}

FrUccDrawDlg::~FrUccDrawDlg()
{
	Clear();
}

void FrUccDrawDlg::Init()
{
	m_unknown384 = 0;
	m_pSetInfoDlg = NULL;
	m_pPickColorDlg = NULL;
	m_pAskResetDlg = NULL;
	m_pAskCompleteDlg = NULL;
	m_pAskCancelDlg = NULL;
	m_pWaitingForm = NULL;
	m_bWaiting = false;
	m_waitingTime = 0;
	m_pClothes[0] = NULL;
	m_pClothes[1] = NULL;
	m_pItemInfo = NULL;
	m_pCanvas = NULL;
	m_pUndoBtn = NULL;
	m_pRedoBtn = NULL;
	m_pResetBtn = NULL;
	m_pDragBtn = NULL;
	m_pZoomInBtn = NULL;
	m_pZoomOutBtn = NULL;
	m_pTabDrawBtn = NULL;
	m_pTabPatternBtn = NULL;
	m_pTabEmoticonBtn = NULL;
	m_pTabTextBtn = NULL;
	m_defaultEraser = 0;
	m_brushType = BRUSH_BRUSH;
	m_side = 0;
	m_drawMode = DRAW_CLOTHES;
	m_bSaved = true;
	m_bCloseAfterSave = false;
	m_name[0] = 0;
	m_patternIndex = -1;
	m_patternRate = 2;
	m_bPatternFolded = true;
	m_bGrid = false;
	m_bInitialized = true;
	m_canvasRect = WRect(0, 0, 0, 0);
	m_pToolBox = g_pFresh->RegisterBitmap("ucc_tool_box");
	m_pSelectBox = g_pFresh->RegisterBitmap("ucc_select_box");
	m_pBgText1[0] = g_pFresh->RegisterBitmap("ucc_bg_text_front1");
	m_pBgText1[1] = g_pFresh->RegisterBitmap("ucc_bg_text_back1");
	m_pBgText2[0] = g_pFresh->RegisterBitmap("ucc_bg_text_front2");
	m_pBgText2[1] = g_pFresh->RegisterBitmap("ucc_bg_text_back2");
	m_pToolChangeIcon = g_pFresh->RegisterBitmap("ucc_tool_change_icon");
	m_pSelectSBox2 = g_pFresh->RegisterBitmap("ucc_select_sbox2");
	m_pSelectBoxYellow = g_pFresh->RegisterBitmap("ucc_select_box_yellow");
	m_pSelectBoxPink = g_pFresh->RegisterBitmap("ucc_select_box_pink");
	m_pSelectColor = g_pFresh->RegisterBitmap("ucc_select_color");
	unsigned char colors[9][3] = {
		{ 255, 28,  28  },
        { 255, 177, 8   },
        { 255, 231, 25  },
        { 122, 179, 55  },
		{ 71,  192, 189 },
        { 14,  57,  135 },
        { 4,   37,  99  },
        { 146, 28,  190 },
		{ 0,   0,   0   }
	};
	for (int i = 0; i < 18; ++i)
	{
		m_color[i].Create(1, 1, 32);
		if (i < 9)
			m_color[i].SetPixel(0, 0, colors[i][0], colors[i][1], colors[i][2],
				255);
		else
			m_color[i].SetPixel(0, 0, 255, 255, 255, 0);
	}
	m_toolTip.Add((FrWnd**)&m_pUndoBtn, UCC_TOOLTIP_UNDO);
	m_toolTip.Add((FrWnd**)&m_pRedoBtn, UCC_TOOLTIP_REDO);
	m_toolTip.Add((FrWnd**)&m_pResetBtn, UCC_TOOLTIP_RESET);
	m_toolTip.Add((FrWnd**)&m_pDragBtn, UCC_TOOLTIP_DRAG);
	m_toolTip.Add((FrWnd**)&m_pZoomInBtn, UCC_TOOLTIP_ZOOMIN);
	m_toolTip.Add((FrWnd**)&m_pZoomOutBtn, UCC_TOOLTIP_ZOOMOUT);
	m_toolTip.Add((FrWnd**)&m_pTempSaveBtn, UCC_TOOLTIP_TEMPSAVE);
	m_tabManager.Add(UCC_TAB_DRAW, &m_pPickColorBtn, &m_pCurrentColorBtn,
		&m_pPaintBtn, &m_pCurrentColorArea, &m_pColorListsArea, NULL);
	m_tabManager.Add(UCC_TAB_DRAW, &m_pColorList, &m_pEraserList, &m_pBrushList,
		&m_pPenList, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pPickColorBtn, &m_pCurrentColorBtn,
		&m_pPaintBtn, &m_pCurrentColorArea, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pColorList, &m_pEraserList, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pArrowDownBtn, &m_pArrowRightBtn,
		&m_pArrowLeftBtn, NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pDeleteBtn, &m_pModifyBtn, &m_pMakeBtn,
		NULL);
	m_tabManager.Add(UCC_TAB_PATTERN, &m_pRateList, &m_pPatternList,
		&m_pBaseColorList, NULL);
	m_emptyPattern.Create(18, 18, 32);
	for (int y = 0; y < 18; ++y)
		for (int x = 0; x < 18; ++x)
			m_emptyPattern.SetPixel(x, y, 244, 244, 244, 255);
	m_blankPattern.Create(88, 88, 32);
	for (int y = 0; y < 88; ++y)
		for (int x = 0; x < 88; ++x)
			m_blankPattern.SetPixel(x, y, 255, 255, 255, 255);
	for (int i = 0; i < 24; ++i)
	{
		Bitmap* brush = UccBrushList(BRUSH_PATTERN)->GetBrush(i);
		if (brush)
			GetResizedImage(brush, NULL, &m_pattern[i], 18, 18);
		else
		{
			m_pattern[i] = m_emptyPattern;
			UccBrushList(BRUSH_PATTERN)->AddBrush(&m_blankPattern);
		}
	}
	m_cursor.SetRect((FrWnd**)&m_pCanvas);
	m_cursor.Add(COverlayCursor::CT_DRAG, "ucc_cursor_drag.tga", 12, 14);
	m_cursor.Add(COverlayCursor::CT_ERASER, "ucc_cursor_eraser.tga", 5, 6);
	m_cursor.Add(COverlayCursor::CT_EYEDROPPER, "ucc_cursor_eyedropper.tga", 6,
		22);
	m_cursor.Add(COverlayCursor::CT_PAINT, "ucc_cursor_paint.tga", 3, 18);
	m_cursor.Add(COverlayCursor::CT_SELECT_COLOR, "ucc_cursor_select_color.tga",
		10, 10);
	m_cursor.Add(COverlayCursor::CT_ZOOM_IN, "ucc_cursor_zoom_in.tga", 11, 10);
	m_cursor.Add(COverlayCursor::CT_ZOOM_OUT, "ucc_cursor_zoom_out.tga", 11,
		10);
}

BEGIN_FRESH_MSGMAP(FrUccDrawDlg, FrForm)
ON_FRESH_VI("ucccanvas", FRCMD_INIT, FrUccDrawDlg::OnUccCanvasInit)
ON_FRESH_VI("undo_btn", FRCMD_INIT, FrUccDrawDlg::OnUndoBtnInit)
ON_FRESH_VV("undo_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnUndoBtnUp)
ON_FRESH_VI("redo_btn", FRCMD_INIT, FrUccDrawDlg::OnRedoBtnInit)
ON_FRESH_VV("redo_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnRedoBtnUp)
ON_FRESH_VI("flip_btn", FRCMD_INIT, FrUccDrawDlg::OnFlipBtnInit)
ON_FRESH_VI("flip_btn", FRCMD_OWNERDRAW, FrUccDrawDlg::OnFlipBtnOwnerDraw)
ON_FRESH_VV("flip_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnFlipBtnUp)
ON_FRESH_VI("reset_btn", FRCMD_INIT, FrUccDrawDlg::OnResetBtnInit)
ON_FRESH_VV("reset_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnResetBtnUp)
ON_FRESH_VI("side1_area", FRCMD_INIT, FrUccDrawDlg::OnSide1AreaInit)
ON_FRESH_VI("side2_area", FRCMD_INIT, FrUccDrawDlg::OnSide2AreaInit)
ON_FRESH_VI("temp_save_btn", FRCMD_INIT, FrUccDrawDlg::OnTempSaveBtnInit)
ON_FRESH_VV("temp_save_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnTempSaveBtnUp)
ON_FRESH_VI("grid_btn", FRCMD_INIT, FrUccDrawDlg::OnGridBtnInit)
ON_FRESH_VV("grid_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnGridBtnUp)
ON_FRESH_VI("drag_btn", FRCMD_INIT, FrUccDrawDlg::OnDragBtnInit)
ON_FRESH_VV("drag_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnDragBtnUp)
ON_FRESH_VI("zoomin_btn", FRCMD_INIT, FrUccDrawDlg::OnZoomInBtnInit)
ON_FRESH_VV("zoomin_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnZoomInBtnUp)
ON_FRESH_VI("zoomout_btn", FRCMD_INIT, FrUccDrawDlg::OnZoomOutBtnInit)
ON_FRESH_VV("zoomout_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnZoomOutBtnUp)
ON_FRESH_VI("draw_tab_btn", FRCMD_INIT, FrUccDrawDlg::OnTabDrawBtnInit)
ON_FRESH_VV("draw_tab_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnTabDrawBtnUp)
ON_FRESH_VI("pattern_tab_btn", FRCMD_INIT, FrUccDrawDlg::OnTabPatternBtnInit)
ON_FRESH_VV("pattern_tab_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnTabPatternBtnUp)
ON_FRESH_VI("emoticon_tab_btn", FRCMD_INIT, FrUccDrawDlg::OnTabEmoticonBtnInit)
ON_FRESH_VV("emoticon_tab_btn", FRCMD_LBUTTONUP,
	FrUccDrawDlg::OnTabEmoticonBtnUp)
ON_FRESH_VI("text_tab_btn", FRCMD_INIT, FrUccDrawDlg::OnTabTextBtnInit)
ON_FRESH_VV("text_tab_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnTabTextBtnUp)
ON_FRESH_VI("finish_btn", FRCMD_INIT, FrUccDrawDlg::OnFinishBtnInit)
ON_FRESH_VV("finish_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnFinishBtnUp)
ON_FRESH_VI("close_btn", FRCMD_INIT, FrUccDrawDlg::OnCloseBtnInit)
ON_FRESH_VV("close_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnCloseBtnUp)
ON_FRESH_VI("paint_btn", FRCMD_INIT, FrUccDrawDlg::OnPaintBtnInit)
ON_FRESH_VV("paint_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnPaintBtnUp)
ON_FRESH_VI("pick_color_btn", FRCMD_INIT, FrUccDrawDlg::OnPickColorBtnInit)
ON_FRESH_VV("pick_color_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnPickColorBtnUp)
ON_FRESH_VI("cur_color_btn", FRCMD_INIT, FrUccDrawDlg::OnCurrentColorBtnInit)
ON_FRESH_VV("cur_color_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnCurrentColorBtnUp)
ON_FRESH_VI("cur_color_area", FRCMD_INIT, FrUccDrawDlg::OnCurrentColorAreaInit)
ON_FRESH_VI("color_list", FRCMD_INIT, FrUccDrawDlg::OnColorListInit)
ON_FRESH_VI("color_list", FRCMD_OWNERDRAW, FrUccDrawDlg::OnColorListOwnerDraw)
ON_FRESH_VV("color_list", FRCMD_LBUTTONDOWN, FrUccDrawDlg::OnColorListBtnDown)
ON_FRESH_VI("eraser_list", FRCMD_INIT, FrUccDrawDlg::OnEraserListInit)
ON_FRESH_VI("eraser_list", FRCMD_OWNERDRAW, FrUccDrawDlg::OnEraserListOwnerDraw)
ON_FRESH_VV("eraser_list", FRCMD_LBUTTONDOWN, FrUccDrawDlg::OnEraserListBtnDown)
ON_FRESH_VI("toolinfo_eraser_area", FRCMD_INIT,
	FrUccDrawDlg::OnToolInfoEraserAreaInit)
ON_FRESH_VI("brush_list", FRCMD_INIT, FrUccDrawDlg::OnBrushListInit)
ON_FRESH_VI("brush_list", FRCMD_OWNERDRAW, FrUccDrawDlg::OnBrushListOwnerDraw)
ON_FRESH_VV("brush_list", FRCMD_LBUTTONDOWN, FrUccDrawDlg::OnBrushListBtnDown)
ON_FRESH_VI("pen_list", FRCMD_INIT, FrUccDrawDlg::OnPenListInit)
ON_FRESH_VI("pen_list", FRCMD_OWNERDRAW, FrUccDrawDlg::OnPenListOwnerDraw)
ON_FRESH_VV("pen_list", FRCMD_LBUTTONDOWN, FrUccDrawDlg::OnPenListBtnDown)
ON_FRESH_VI("toolinfo_pen_area", FRCMD_INIT,
	FrUccDrawDlg::OnToolInfoPenAreaInit)
ON_FRESH_VI("toolinfo_brush_area", FRCMD_INIT,
	FrUccDrawDlg::OnToolInfoBrushAreaInit)
ON_FRESH_VI("color_list_area", FRCMD_INIT, FrUccDrawDlg::OnColorListsAreaInit)
ON_FRESH_VI("arrow_down_btn", FRCMD_INIT, FrUccDrawDlg::OnArrowDownBtnInit)
ON_FRESH_VV("arrow_down_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnArrowDownBtnUp)
ON_FRESH_VI("arrow_right_btn", FRCMD_INIT, FrUccDrawDlg::OnArrowRightBtnInit)
ON_FRESH_VV("arrow_right_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnArrowRightBtnUp)
ON_FRESH_VI("arrow_left_btn", FRCMD_INIT, FrUccDrawDlg::OnArrowLeftBtnInit)
ON_FRESH_VV("arrow_left_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnArrowLeftBtnUp)
ON_FRESH_VI("delete_btn", FRCMD_INIT, FrUccDrawDlg::OnDeleteBtnInit)
ON_FRESH_VV("delete_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnDeleteBtnUp)
ON_FRESH_VI("modify_btn", FRCMD_INIT, FrUccDrawDlg::OnModifyBtnInit)
ON_FRESH_VV("modify_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnModifyBtnUp)
ON_FRESH_VI("make_btn", FRCMD_INIT, FrUccDrawDlg::OnMakeBtnInit)
ON_FRESH_VV("make_btn", FRCMD_LBUTTONUP, FrUccDrawDlg::OnMakeBtnUp)
ON_FRESH_VI("rate_list", FRCMD_INIT, FrUccDrawDlg::OnRateListInit)
ON_FRESH_VI("rate_list", FRCMD_OWNERDRAW, FrUccDrawDlg::OnRateListOwnerDraw)
ON_FRESH_VV("rate_list", FRCMD_LBUTTONDOWN, FrUccDrawDlg::OnRateListBtnDown)
ON_FRESH_VI("pattern_list", FRCMD_INIT, FrUccDrawDlg::OnPatternListInit)
ON_FRESH_VI("pattern_list", FRCMD_OWNERDRAW,
	FrUccDrawDlg::OnPatternListOwnerDraw)
ON_FRESH_VV("pattern_list", FRCMD_LBUTTONDOWN,
	FrUccDrawDlg::OnPatternListBtnDown)
ON_FRESH_VI("base_color_list", FRCMD_INIT, FrUccDrawDlg::OnBaseColorListInit)
ON_FRESH_VI("extended_base_pattern_list", FRCMD_INIT,
	FrUccDrawDlg::OnExtendedBasePatternListInit)
ON_FRESH_VI("etc", FRCMD_INIT, FrUccDrawDlg::OnEtcInit)
ON_FRESH_VI("etc", FRCMD_OWNERDRAW, FrUccDrawDlg::OnEtcOwnerDraw)
ON_FRESH_VI("tooltip", FRCMD_INIT, FrUccDrawDlg::OnToolTipInit)
ON_FRESH_VI("tooltip", FRCMD_OWNERDRAW, FrUccDrawDlg::OnToolTipOnwerDraw)
END_FRESH_MSGMAP()

void FrUccDrawDlg::Clear()
{
	UccCurrentBrush()->SetBrush(BRUSH_BRUSH, 0, 0, 0, 0);
	SetCurrentColor(0, 0, 0);
	if (m_pBrushList->GetSelected())
		m_pBrushList->GetSelected()->selected = false;
	if (m_pPenList->GetSelected())
		m_pPenList->GetSelected()->selected = false;
	if (m_pPatternList->GetSelected())
		m_pPatternList->GetSelected()->selected = false;
	m_pBrushList->SelectItem(m_pBrushList->GetItem(0), true);
	if (m_pClothes[0])
	{
		delete m_pClothes[0];
		m_pClothes[0] = NULL;
	}
	if (m_pClothes[1])
	{
		delete m_pClothes[1];
		m_pClothes[1] = NULL;
	}
	m_bGrid = false;
}

bool FrUccDrawDlg::Open(FRESH_PFN_RESULT callback, const WPoint& pos,
	unsigned long flag)
{
	bool result = FrForm::Open(callback, pos, flag);
	m_canvasRect = m_pCanvas->GetRect() - WPoint(GetRect().x, GetRect().y);
	MoveCanvasToCenter();
	return result;
}

void FrUccDrawDlg::OnCancel()
{
	if (IsSaved())
	{
		m_pAskCancelDlg = NULL;
		Clear();
		FrForm::OnCancel();
		return;
	}
	if (!m_pAskCancelDlg)
	{
		m_pAskCancelDlg = CreateForm<FrNotifyForm>(g_pFresh->GetManager(), this,
			"notify_yesnocancel");
		m_pAskCancelDlg->SetMessage(
			"\xb1\xd7\xb8\xae\xb4\xf8 \xc0\xc7\xbb\xf3\xc0\xbb \xc0\xd3\xbd\xc3\xc0\xfa\xc0\xe5\xc7\xcf\xb0\xed \xb3\xa1\xb3\xbb\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee?",
			false);
		m_pAskCancelDlg->Open(
			(FRESH_PFN_RESULT)&FrUccDrawDlg::OnAskCancelDlgResult, FrESCAPE);
	}
}

bool FrUccDrawDlg::Close(eFormRet ret, bool fade)
{
	CloseWaitingForm();
	return FrForm::Close(ret, fade);
}

void FrUccDrawDlg::SetName(const char* name)
{
	strcpy(m_name, name);
}

void FrUccDrawDlg::Upload()
{
	G_CreateDirectory(MakeStr("%s\\", GetUccTempFoler()));
	if (IsSavingTemporarily())
	{
		std::string filename =
			GenerateTextureFullPath(0, m_pItemInfo->tid, m_pItemInfo->UccIndex);
		if (!ZipUccClothes(m_pClothes[0]->GetTexture32BPP(true),
				m_pClothes[1]->GetTexture32BPP(true), NULL, filename.c_str()))
		{
			Notify(MakeStr(
				"\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)",
				4));
			return;
		}
		NetResManager()->UploadUccTempClothes(m_pItemInfo->tid,
			m_pItemInfo->UccIndex);
	}
	else
	{
		std::string filename =
			GenerateTextureFullPath(0, m_pItemInfo->tid, m_pItemInfo->UccIndex);
		Bitmap* icon = new Bitmap(64, 84, 32);
		CreateItemThumbnail(m_pClothes[0]->GetTexture(),
			m_pClothes[0]->GetMask(), icon);
		if (!ZipUccClothes(m_pClothes[0]->GetTexture(),
				m_pClothes[1]->GetTexture(), icon, filename.c_str()))
		{
			Notify(MakeStr(
				"\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)",
				3));
			return;
		}
		std::string iconName =
			GenerateIconFilename(m_pItemInfo->tid, m_pItemInfo->UccIndex);
		UccManager()->m_icon.insert(
			std::map<std::string, Bitmap*>::value_type(iconName, icon));
		NetResManager()->UploadUccClothes(m_pItemInfo->tid,
			m_pItemInfo->UccIndex, m_name);
	}
}

void FrUccDrawDlg::Notify(const char* message)
{
	if (AfxGetTask()->GetActor("RealMyRoom"))
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 0x23, (int)message, 0, 0, 0, 0);
	else
		AfxGetTask()->GetActor("RealMyRoom")
			<< MsgObject(NULL, 0x23, (int)message, 0, 0, 0, 0);
}

void FrUccDrawDlg::SetCurrentColor(unsigned char r, unsigned char g,
	unsigned char b)
{
	m_currentColor.SetPixel(0, 0, r, g, b, 0xff);
	RefreshTexCache(m_currentColor);
	UccCurrentBrush()->SetBrush(r, g, b);
}

void FrUccDrawDlg::MoveCanvasToCenter()
{
	WPoint offset = WPoint(GetRect().x, GetRect().y);
	WPoint pos = WPoint(m_canvasRect.x, m_canvasRect.y);
	pos += offset;
	Bitmap* texture = m_pClothes[m_side]->GetTexture32BPP(false);
	if (texture)
	{
		int x = (256 - texture->Width()) / 2;
		int y = (256 - texture->Height()) / 2;
		pos.x += x;
		pos.y += y;
	}
	m_pCanvas->MoveWindow(pos);
}

void FrUccDrawDlg::OnProc(const float delta)
{
	if (!m_pClothes[m_side])
		return;
	const WPoint& mouse = g_pFresh->GetManager()->GetMousePos();
	if (m_pClothes[m_side]->GetMode() == MODE_DRAW &&
		(m_brushType == BRUSH_BRUSH || m_brushType == BRUSH_PEN))
		m_cursor.SetCursor(COverlayCursor::CT_NORMAL);
	if (m_pClothes[m_side]->IsMouseOver(mouse))
	{
		if (g_input->GetButton(LEFT_BUTTON) > 0)
			m_pClothes[m_side]->MouseEventHandler(LEFT_BUTTON,
				g_input->GetButton(LEFT_BUTTON), mouse);
	}
	else if (m_pClothes[m_side]->m_bDragging &&
		g_input->GetButton(LEFT_BUTTON) == 2)
		m_pClothes[m_side]->MouseEventHandler(LEFT_BUTTON, 2, WPoint());
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("UNDO", false))
		OnUndoBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("REDO", false))
		OnRedoBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("FLIP", false))
		OnFlipBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("PAINT", false))
		OnPaintBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("DRAG", false))
		OnDragBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("GRID", false))
		OnGridBtnUp();
	if ((g_input->Get("LCONTROL", false) || g_input->Get("RCONTROL", false)) &&
		g_input->GetDown("SAVE", false))
		OnTempSaveBtnUp();
	m_pClothes[m_side]->Process();
	if (m_pUndoBtn && m_pRedoBtn && m_pClothes[m_side])
	{
		m_pUndoBtn->Enable(m_pClothes[m_side]->IsUndoPossible());
		m_pRedoBtn->Enable(m_pClothes[m_side]->IsRedoPossible());
	}
	if (m_bWaiting)
	{
		m_waitingTime += delta;
		if (!m_pWaitingForm)
		{
			m_pWaitingForm =
				CreateForm<FrForm>(g_pFresh->GetManager(), this, "wait");
			if (IsSavingTemporarily())
				m_pWaitingForm->SetMessage(
					"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xbb \xc0\xd3\xbd\xc3 \xc0\xfa\xc0\xe5\xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			else
				m_pWaitingForm->SetMessage(
					"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xbb \xbe\xf7\xb7\xce\xb5\xe5 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9.",
					false);
			m_pWaitingForm->Open(NULL, 0);
			FrButton* cancel = DYNAMIC_CAST(FrButton,
				m_pWaitingForm->FindChildByName("cancel"));
			if (cancel)
				cancel->Enable(false);
		}
		eResourceType resource = m_drawMode == DRAW_CLOTHES
			? RESOURCE_UCC_CLOTHES
			: RESOURCE_UCC_AZTEC;
		if (m_waitingTime > 20.f &&
			NetResManager()->IsFailedToUpload(m_pItemInfo->tid,
				m_pItemInfo->UccIndex, resource))
		{
			AfxGetTask()->GetActor("RealMyRoom") << MsgObject(NULL, 0x23,
				(int)MakeStr(
					"\xbe\xf7\xb7\xce\xb5\xe5\xb0\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9. \xb4\xd9\xbd\xc3 \xbd\xc3\xb5\xb5\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.(\xc4\xda\xb5\xe5:%d)",
					2),
				0, 0, 0, 0);
			m_bWaiting = false;
		}
	}
	else if (m_pWaitingForm)
	{
		m_pWaitingForm->Close(FrOK, true);
		m_pWaitingForm = NULL;
		m_waitingTime = 0;
		m_bWaiting = false;
	}
}

void FrUccDrawDlg::SetUccDrawInfo(eDrawMode mode, sItemInfo* item, bool reset)
{
	m_pItemInfo = item;
	m_drawMode = mode;
	unsigned long typeId = item->tid;
	char uccIndex[9];
	memset(uccIndex, 0, sizeof(uccIndex));
	strncpy(uccIndex, item->UccIndex, 9);
	Bitmap temporary[2];
	bool hasTemporary = false;
	if (!typeId || !strlen(uccIndex) || !IsUccClothes(typeId))
	{
		Notify(MakeStr(
			"\xbc\xad\xb9\xf6\xb7\xce\xba\xce\xc5\xcd \xc7\xca\xbf\xe4\xc7\xd1 \xc1\xa4\xba\xb8\xb8\xa6 \xbe\xf2\xc1\xf6 \xb8\xf8\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9(\xc4\xda\xb5\xe5:%d)",
			1));
		m_bInitialized = false;
		OnCancel();
		return;
	}
	if (!reset)
		hasTemporary = UccManager()->GetUccTempClothes(m_pItemInfo->tid,
			m_pItemInfo->UccIndex, temporary[0], temporary[1]);
	if (mode == DRAW_CLOTHES)
	{
		m_pClothes[0] = new CUccClothes;
		m_pClothes[1] = new CUccClothes;
	}
	else if (mode == DRAW_AZTEC)
	{
	}
	else
	{
		Notify(MakeStr(
			"\xbf\xb9\xbb\xf3\xc4\xa1 \xb8\xf8\xc7\xd1 \xbf\xc0\xb7\xf9\xb0\xa1 \xb9\xdf\xbb\xfd\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9(\xc4\xda\xb5\xe5:%d)",
			1));
		m_bInitialized = false;
		OnCancel();
		return;
	}
	m_side = 0;
	if (mode == DRAW_CLOTHES)
	{
		CItemManager* manager = &Doc()->m_itemManager;
		IFF_STRUCT::sPart* part = manager->FindPart(typeId);
		if (part)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (!m_pClothes[i]->IsInitialized())
				{
					Bitmap* texture =
						g_resrcmng->LoadBitmap(part->Tex[i], 0, false);
					Bitmap* original =
						g_resrcmng->LoadBitmap(part->Tex[i], 0, false);
					Bitmap mask;
					if (original)
						mask = *original;
					else
					{
						mask.Create(texture->Width(), texture->Height(), 24);
						for (unsigned y = 0; y < mask.Height(); ++y)
							for (unsigned x = 0; x < mask.Width(); ++x)
								mask.SetPixel(x, y, 255, 255, 255, 255);
					}
					Bitmap* maskFile = g_resrcmng->LoadBitmap(
						GetMaskFilename(part->Tex[i]).c_str(), 0, false);
					if (maskFile)
					{
						for (unsigned y = 0; y < mask.Height(); ++y)
						{
							for (unsigned x = 0; x < mask.Width(); ++x)
							{
								unsigned char r, g, b;
								maskFile->GetPixel(x, y, r, g, b);
								if (r < 5 && g < 5 && b < 5)
									mask.SetPixel(x, y, 0, 0, 0, 255);
							}
						}
					}
					m_pClothes[i]->InitializeItemDraw(i, m_pCanvas, texture,
						&mask, m_pItemInfo->tid, m_pItemInfo->UccIndex);
					ClearTexCache(texture);
					delete texture;
					ClearTexCache(original);
					delete original;
					ClearTexCache(maskFile);
					delete maskFile;
					if (m_pClothes[i]->GetTexture())
						g_resrcmng->UploadTexture(
							m_pClothes[i]->GetTextureName(),
							m_pClothes[i]->GetTexture(), 0, NULL);
					m_pClothes[i]->SetParent(this);
					if ((m_pItemInfo->status & 2) && hasTemporary)
					{
						Bitmap* texture24 = m_pClothes[i]->GetTexture();
						Bitmap* texture32 =
							m_pClothes[i]->GetTexture32BPP(true);
						*texture32 = temporary[i];
						Convert32to24(texture32, texture24, &mask);
					}
				}
			}
		}
		else
		{
			Notify(
				"\xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xc0\xc7\xbb\xf3\xc0\xd4\xb4\xcf\xb4\xd9.");
			m_bInitialized = false;
			OnCancel();
			return;
		}
	}
	if (!(m_pClothes[0] && m_pClothes[0]->GetTexture() &&
			m_pClothes[0]->GetTexture32BPP(false)))
	{
		Notify(
			"\xc5\xd8\xbd\xba\xc3\xc4\xb8\xa6 \xba\xd2\xb7\xaf\xbf\xc0\xb4\xc2 \xb5\xa5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_bInitialized = false;
		OnCancel();
		return;
	}
	if (!(!m_pClothes[1] ||
			(m_pClothes[1]->GetTexture() &&
				m_pClothes[1]->GetTexture32BPP(false))))
	{
		Notify(
			"\xc5\xd8\xbd\xba\xc3\xc4\xb8\xa6 \xba\xd2\xb7\xaf\xbf\xc0\xb4\xc2 \xb5\xa5\xbf\xa1 \xbd\xc7\xc6\xd0\xc7\xdf\xbd\xc0\xb4\xcf\xb4\xd9.");
		m_bInitialized = false;
		OnCancel();
		return;
	}
	int width = 256 / m_pClothes[m_side]->GetTexture32BPP(false)->Width();
	int height = 256 / m_pClothes[m_side]->GetTexture32BPP(false)->Height();
	if (width > height)
		width = height;
	while (m_pClothes[m_side]->GetZoomRate() < width)
	{
		bool changed = true;
		if (m_pClothes[0])
			changed = changed && m_pClothes[0]->ZoomIn(-1, -1);
		if (m_pClothes[1])
			changed = changed && m_pClothes[1]->ZoomIn(-1, -1);
		if (!changed)
			break;
	}
	m_pZoomInBtn->Enable(m_pClothes[m_side]->GetZoomRate() < 8);
	m_pZoomOutBtn->Enable(m_pClothes[m_side]->GetZoomRate() > 1);
	if (m_canvasRect.w != 0)
		MoveCanvasToCenter();
	for (int i = 1; i >= 0; --i)
	{
		if (m_pClothes[i])
		{
			m_pClothes[i]->MustRefresh(2);
			m_pClothes[i]->Process();
		}
	}
}

void FrUccDrawDlg::OnUccCanvasInit(int param)
{
	m_pCanvas = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnUndoBtnInit(int param)
{
	m_pUndoBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnUndoBtnUp()
{
	if (m_pClothes[m_side])
		m_pClothes[m_side]->Undo();
}

void FrUccDrawDlg::OnRedoBtnInit(int param)
{
	m_pRedoBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnRedoBtnUp()
{
	if (m_pClothes[m_side])
		m_pClothes[m_side]->Redo();
}

void FrUccDrawDlg::OnFlipBtnInit(int param)
{
	m_pFlipBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnFlipBtnOwnerDraw(int param)
{
	WRect rect = m_pFlipBtn->GetRect();
	FrGraphicInterface* gdi = GetGDI();
	if (gdi && m_pToolChangeIcon)
		gdi->DrawTexture(m_pToolChangeIcon,
			WRect(rect.x + 6, rect.y + 6, (float)m_pToolChangeIcon->Width(),
				(float)m_pToolChangeIcon->Height()),
			0xffffffff, 0);
}

void FrUccDrawDlg::OnFlipBtnUp()
{
	eMode mode = m_pClothes[m_side]->GetMode();
	m_pClothes[m_side]->SetMode(MODE_DRAW);
	if (m_side == 0)
		m_side = 1;
	else
		m_side = 0;
	m_pClothes[m_side]->MustRefresh(1);
	if (m_pSide1Area)
		m_pSide1Area->SetBgImg(m_pBgText1[m_side]);
	if (m_pSide2Area)
		m_pSide2Area->SetBgImg(m_pBgText2[1 - m_side]);
	m_pZoomInBtn->Enable(m_pClothes[m_side]->GetZoomRate() < 8);
	m_pZoomOutBtn->Enable(m_pClothes[m_side]->GetZoomRate() > 1);
	MoveCanvasToCenter();
	m_pClothes[m_side]->SetMode(mode);
}

void FrUccDrawDlg::OnResetBtnInit(int param)
{
	m_pResetBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnResetBtnUp()
{
	if (!m_pAskResetDlg)
	{
		m_pAskResetDlg =
			CreateForm<FrForm>(g_pFresh->GetManager(), this, "notify_yesno");
		m_pAskResetDlg->SetMessage(
			"\xc1\xa4\xb8\xbb \xc3\xb3\xc0\xbd\xba\xce\xc5\xcd \xb4\xd9\xbd\xc3 \xb1\xd7\xb8\xae\xbd\xc3\xb0\xda\xbd\xc0\xb4\xcf\xb1\xee? \n\n(\xc7\xf6\xc0\xe7 \xc0\xcc \xb1\xd7\xb8\xae\xb1\xe2\xbf\xeb \xc0\xc7\xbb\xf3\xbf\xa1 \xb1\xd7\xb7\xc1\xc1\xae \xc0\xd6\xb4\xc2 \n   \xb8\xf0\xb5\xe7 \xb1\xd7\xb8\xb2\xc0\xcc \xc1\xf6\xbf\xf6\xc1\xfd\xb4\xcf\xb4\xd9)",
			false);
		m_pAskResetDlg->Open(
			(FRESH_PFN_RESULT)&FrUccDrawDlg::OnAskResetDlgResult, FrESCAPE);
	}
}

void FrUccDrawDlg::OnGridBtnInit(int param)
{
	m_pGridBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnGridBtnUp()
{
	m_bGrid = !m_bGrid;
}

void FrUccDrawDlg::OnDragBtnInit(int param)
{
	m_pDragBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnDragBtnUp()
{
	if (m_pClothes[m_side]->GetMode() == MODE_DRAG)
	{
		m_pClothes[m_side]->SetMode(MODE_DRAW);
		m_cursor.SetCursor(COverlayCursor::CT_NORMAL);
	}
	else
	{
		m_pClothes[m_side]->SetMode(MODE_DRAG);
		m_cursor.SetCursor(COverlayCursor::CT_DRAG);
	}
	SetToolbarStatus(m_pDragBtn);
}

void FrUccDrawDlg::OnZoomInBtnInit(int param)
{
	m_pZoomInBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnZoomInBtnUp()
{
	SetToolbarStatus(m_pZoomInBtn);
	m_cursor.SetCursor(COverlayCursor::CT_ZOOM_IN);
	m_pClothes[m_side]->SetMode(MODE_ZOOMIN);
}

void FrUccDrawDlg::OnZoomOutBtnInit(int param)
{
	m_pZoomOutBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnZoomOutBtnUp()
{
	SetToolbarStatus(m_pZoomOutBtn);
	m_cursor.SetCursor(COverlayCursor::CT_ZOOM_OUT);
	m_pClothes[m_side]->SetMode(MODE_ZOOMOUT);
}

void FrUccDrawDlg::AfterZoom()
{
	m_pZoomInBtn->Enable(m_pClothes[m_side]->GetZoomRate() < 8);
	m_pZoomOutBtn->Enable(m_pClothes[m_side]->GetZoomRate() > 1);
	MoveCanvasToCenter();
	if (m_pClothes[m_side]->GetMode() == MODE_DRAW)
	{
		m_cursor.SetCursor(COverlayCursor::CT_NORMAL);
		SetToolbarStatus(m_pTabDrawBtn);
	}
}

void FrUccDrawDlg::SetSaved(bool saved)
{
	m_bSaved = saved;
	if (saved)
		m_fadingText.Print(
			K2L_Compatibility(
				"\xbc\xbf\xc7\xc1\xb5\xf0\xc0\xda\xc0\xce \xc0\xc7\xbb\xf3\xc0\xcc \xc0\xd3\xbd\xc3 \xc0\xfa\xc0\xe5\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9."),
			3.f, this, 105.f, 20.f, FADE_INOUT, 0xff000000);
	if (m_bCloseAfterSave)
		Close(FrOK, true);
}

void FrUccDrawDlg::OnSide1AreaInit(int param)
{
	m_pSide1Area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnSide2AreaInit(int param)
{
	m_pSide2Area = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnTempSaveBtnInit(int param)
{
	m_pTempSaveBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnTempSaveBtnUp()
{
	SecurityKeyManager()->ReqCreateKey(SK_UCC, m_pItemInfo->guid);
	m_bWaiting = true;
}

void FrUccDrawDlg::OnTabDrawBtnInit(int param)
{
	m_pTabDrawBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTabDrawBtn->SetStatus(FrButton::PRESSED);
}

void FrUccDrawDlg::OnTabDrawBtnUp()
{
	ChangeTab(UCC_TAB_DRAW);
	m_pClothes[m_side]->SetMode(MODE_DRAW);
	m_cursor.SetCursor(COverlayCursor::CT_NORMAL);
	SetToolbarStatus(m_pTabDrawBtn);
}

void FrUccDrawDlg::OnTabPatternBtnInit(int param)
{
	m_pTabPatternBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnTabPatternBtnUp()
{
	m_tabManager.Open(UCC_TAB_PATTERN);
	m_currentTab = UCC_TAB_PATTERN;
	if (!m_bPatternFolded)
		OnArrowDownBtnUp();
	m_pColorList->Enable(false);
	m_pPickColorBtn->Enable(false);
	m_pCurrentColorBtn->Enable(false);
	m_brushType = BRUSH_PATTERN;
	m_pClothes[m_side]->SetMode(MODE_DRAW);
	m_cursor.SetCursor(COverlayCursor::CT_NORMAL);
	SetToolbarStatus(m_pTabPatternBtn);
}

void FrUccDrawDlg::OnTabEmoticonBtnInit(int param)
{
	m_pTabEmoticonBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTabEmoticonBtn->Enable(false);
}

void FrUccDrawDlg::OnTabEmoticonBtnUp()
{
}

void FrUccDrawDlg::OnTabTextBtnInit(int param)
{
	m_pTabTextBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pTabTextBtn->Enable(false);
}

void FrUccDrawDlg::OnTabTextBtnUp()
{
}

void FrUccDrawDlg::OnFinishBtnInit(int param)
{
	m_pFinishBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::Finish()
{
	if (m_pItemInfo && !m_pSetInfoDlg)
	{
		WRect rect = GetRect();
		m_pSetInfoDlg = CreateForm<FrUccSetInfoDlg>(g_pFresh->GetManager(),
			this, "ucc_setinfo");
		m_pSetInfoDlg->SetParent(this);
		m_pSetInfoDlg->Open(
			(FRESH_PFN_RESULT)&FrUccDrawDlg::OnUccSetInfoDlgResult,
			WPoint(rect.x, rect.y) + WPoint(30, 30), 0);
	}
}

void FrUccDrawDlg::OnFinishBtnUp()
{
	Finish();
}

bool FrUccDrawDlg::OnAskCompleteDlgResult(int result, FrForm* form)
{
	m_pAskCompleteDlg = NULL;
	if (result == FrOK)
		Finish();
	return true;
}

bool FrUccDrawDlg::OnAskCancelDlgResult(int result, FrForm* form)
{
	m_pAskCancelDlg = NULL;
	if (result == FrYES)
	{
		m_bCloseAfterSave = true;
		SecurityKeyManager()->ReqCreateKey(SK_UCC, m_pItemInfo->guid);
		m_bWaiting = true;
		return true;
	}
	if (result == FrNO)
	{
		Clear();
		FrForm::OnCancel();
	}
	return true;
}

bool FrUccDrawDlg::OnAskResetDlgResult(int result, FrForm* form)
{
	if (result == FrOK)
		Reset();
	m_pAskResetDlg = NULL;
	return true;
}

bool FrUccDrawDlg::OnUccSetInfoDlgResult(int result, FrForm* form)
{
	m_pSetInfoDlg = NULL;
	if (result == FrOK)
	{
		SecurityKeyManager()->ReqCreateKey(SK_UCC, m_pItemInfo->guid);
		m_bWaiting = true;
	}
	return true;
}

void FrUccDrawDlg::OnCloseBtnInit(int param)
{
	m_pCloseBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnCloseBtnUp()
{
	OnCancel();
}

void FrUccDrawDlg::OnPaintBtnInit(int param)
{
	m_pPaintBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnPaintBtnUp()
{
	m_pClothes[m_side]->SetMode(MODE_PAINT);
	m_cursor.SetCursor(COverlayCursor::CT_PAINT);
	SetToolbarStatus(m_pTabDrawBtn);
}

void FrUccDrawDlg::OnPickColorBtnInit(int param)
{
	m_pPickColorBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnPickColorBtnUp()
{
	m_pClothes[m_side]->SetMode(MODE_SPOID);
	m_cursor.SetCursor(COverlayCursor::CT_EYEDROPPER);
	SetToolbarStatus(m_pTabDrawBtn);
}

void FrUccDrawDlg::OnCurrentColorBtnInit(int param)
{
	m_pCurrentColorBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnCurrentColorBtnUp()
{
	if (!m_pPickColorDlg)
	{
		WRect rect = GetRect();
		m_pPickColorDlg = CreateForm<FrUccPickColorDlg>(g_pFresh->GetManager(),
			this, "ucc_pickcolor");
		m_pPickColorDlg->SetParent(this);
		m_pPickColorDlg->Open(
			(FRESH_PFN_RESULT)&FrUccDrawDlg::OnUccPickColorDlgResult,
			WPoint(rect.x, rect.y) + WPoint(30, 30), 0);
	}
}

bool FrUccDrawDlg::OnUccPickColorDlgResult(int result, FrForm* form)
{
	m_pPickColorDlg = NULL;
	return true;
}

void FrUccDrawDlg::OnCurrentColorAreaInit(int param)
{
	m_pCurrentColorArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_currentColor.Create(1, 1, 32);
	m_currentColor.SetPixel(0, 0, 0, 0, 0, 0xff);
	m_pCurrentColorArea->SetBgImg(&m_currentColor);
}

void FrUccDrawDlg::OnColorListInit(int param)
{
	m_pColorList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildColorList();
}

void FrUccDrawDlg::OnColorListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	WRect base(item->pos.x + 1, item->pos.y + 1, 9.f, 9.f);
	gdi->DrawTexture((Bitmap*)item->pData, base, 0xffffffff, 0);
	if (item->selected)
	{
		WRect rect(item->pos.x - 3, item->pos.y - 3,
			(float)m_pSelectColor->Width(), (float)m_pSelectColor->Height());
		gdi->DrawTexture(m_pSelectColor, rect, 0xffffffff, 0);
	}
}

void FrUccDrawDlg::OnColorListBtnDown()
{
	FrListItem* item = m_pColorList->GetItemUnderCursor();
	if (item && item->pData)
	{
		unsigned char r, g, b, a;
		((Bitmap*)item->pData)->GetPixel(0, 0, r, g, b, a);
		if (a == 255)
			SetCurrentColor(r, g, b);
		if (m_brushType == BRUSH_ERASER)
			UccCurrentBrush()->SetBrush(BRUSH_ERASER,
				m_pEraserList->GetSelected()->idx);
	}
}

void FrUccDrawDlg::OnEraserListInit(int param)
{
	m_pEraserList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildEraserList();
}

void FrUccDrawDlg::OnEraserListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	if (!m_pEraserList->GetSelected() && item->idx == m_defaultEraser)
		m_pEraserList->SelectItem(item, true);
	if (item->selected)
	{
		if (m_brushType != BRUSH_ERASER)
			item->selected = false;
		if (item->selected)
			gdi->DrawTexture(m_pSelectBoxYellow,
				WRect(item->pos.x + 1, item->pos.y + 1,
					(float)m_pSelectBoxYellow->Width(),
					(float)m_pSelectBoxYellow->Height()),
				0xffffffff, 0);
	}
}

void FrUccDrawDlg::OnEraserListBtnDown()
{
	FrListItem* item = m_pEraserList->GetItemUnderCursor();
	if (item && item->pData)
	{
		if (m_pBrushList->GetSelected())
		{
			m_pBrushList->GetSelected()->selected = false;
		}
		if (m_pPenList->GetSelected())
		{
			m_pPenList->GetSelected()->selected = false;
		}
		if (m_pPatternList->GetSelected())
		{
			m_pPatternList->GetSelected()->selected = false;
			m_patternIndex = -1;
		}
		m_brushType = BRUSH_ERASER;
		UccCurrentBrush()->SetBrush(BRUSH_ERASER, item->idx);
		m_cursor.SetCursor(COverlayCursor::CT_ERASER);
		OnTabDrawBtnUp();
	}
}

void FrUccDrawDlg::OnToolInfoEraserAreaInit(int param)
{
	m_pToolInfoEraserArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnBaseColorListInit(int param)
{
	m_pBaseColorList = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnExtendedBasePatternListInit(int param)
{
	m_pExtendedBasePatternList = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnBrushListInit(int param)
{
	m_pBrushList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildBrushList();
}

void FrUccDrawDlg::OnBrushListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	if (!m_pBrushList->GetSelected() && !m_pPenList->GetSelected() &&
		item->idx == UccCurrentBrush()->index &&
		UccCurrentBrush()->type == BRUSH_BRUSH)
		m_pBrushList->SelectItem(item, true);
	if (item->selected)
	{
		if (m_brushType == BRUSH_ERASER)
			item->selected = false;
		if (item->selected)
			gdi->DrawTexture(m_pSelectBoxYellow,
				WRect(item->pos.x + 1, item->pos.y + 1,
					(float)m_pSelectBoxYellow->Width(),
					(float)m_pSelectBoxYellow->Height()),
				0xffffffff, 0);
	}
}

void FrUccDrawDlg::OnBrushListBtnDown()
{
	FrListItem* item = m_pBrushList->GetItemUnderCursor();
	if (item && item->pData)
	{
		if (m_pPenList->GetSelected())
		{
			m_pPenList->GetSelected()->selected = false;
		}
		m_brushType = BRUSH_BRUSH;
		UccCurrentBrush()->SetBrush(BRUSH_BRUSH, item->idx);
		OnTabDrawBtnUp();
	}
}

void FrUccDrawDlg::OnPenListInit(int param)
{
	m_pPenList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildPenList();
}

void FrUccDrawDlg::OnPenListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	if (!m_pBrushList->GetSelected() && !m_pPenList->GetSelected() &&
		item->idx == UccCurrentBrush()->index &&
		UccCurrentBrush()->type == BRUSH_PEN)
		m_pPenList->SelectItem(item, true);
	if (item->selected)
	{
		if (m_brushType == BRUSH_ERASER)
			item->selected = false;
		if (item->selected)
			gdi->DrawTexture(m_pSelectBoxYellow,
				WRect(item->pos.x + 1, item->pos.y + 1,
					(float)m_pSelectBoxYellow->Width(),
					(float)m_pSelectBoxYellow->Height()),
				0xffffffff, 0);
	}
}

void FrUccDrawDlg::OnPenListBtnDown()
{
	FrListItem* item = m_pPenList->GetItemUnderCursor();
	if (item && item->pData)
	{
		if (m_pBrushList->GetSelected())
		{
			m_pBrushList->GetSelected()->selected = false;
		}
		m_brushType = BRUSH_PEN;
		UccCurrentBrush()->SetBrush(BRUSH_PEN, item->idx);
		OnTabDrawBtnUp();
	}
}

void FrUccDrawDlg::OnToolInfoPenAreaInit(int param)
{
	m_pToolInfoPenArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnToolInfoBrushAreaInit(int param)
{
	m_pToolInfoBrushArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnColorListsAreaInit(int param)
{
	m_pColorListsArea = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnArrowDownBtnInit(int param)
{
	m_pArrowDownBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnArrowDownBtnUp()
{
	m_bPatternFolded = !m_bPatternFolded;
	if (m_bPatternFolded)
	{
		m_pArrowLeftBtn->Enable(m_patternIndex > 0);
		m_pArrowRightBtn->Enable(m_patternIndex + 6 < 24);
		m_pExtendedBasePatternList->SetVisible(false);
		WRect rect = m_pPatternList->GetRect();
		rect.h = 27;
		m_pPatternList->SetRect(rect);
		WRect button = m_pArrowDownBtn->GetRect();
		button.y -= 77;
		m_pArrowDownBtn->SetRect(button);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_down_n", FrButton::NORMAL);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_down_o", FrButton::OVER);
	}
	else
	{
		m_pArrowLeftBtn->Enable(false);
		m_pArrowRightBtn->Enable(false);
		m_pExtendedBasePatternList->SetVisible(true);
		WRect rect = m_pPatternList->GetRect();
		rect.h = 101;
		m_pPatternList->SetRect(rect);
		WRect button = m_pArrowDownBtn->GetRect();
		button.y += 77;
		m_pArrowDownBtn->SetRect(button);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_up_n", FrButton::NORMAL);
		m_pArrowDownBtn->SetButtonImg("ucc_arrow_up_o", FrButton::OVER);
	}
	BuildPatternList(m_patternIndex, m_bPatternFolded);
}

void FrUccDrawDlg::OnArrowRightBtnInit(int param)
{
	m_pArrowRightBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccDrawDlg::OnArrowRightBtnUp()
{
	if (m_pPatternList->m_itemList.size() != 0)
	{
		int index = (int)(*m_pPatternList->m_itemList.begin())->pData;
		if (index + 6 < 24)
		{
			m_pArrowLeftBtn->Enable(index + 1 > 0);
			m_pArrowRightBtn->Enable(index + 1 + 6 < 24);
			m_pPatternList->ClearItem();
			for (int i = 0; i < 6; ++i)
				m_pPatternList->AddItem((void*)(index + 1 + i));
		}
	}
}

void FrUccDrawDlg::OnArrowLeftBtnInit(int param)
{
	m_pArrowLeftBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pArrowLeftBtn->Enable(false);
}

void FrUccDrawDlg::OnArrowLeftBtnUp()
{
	if (m_pPatternList->m_itemList.size() != 0)
	{
		int index = (int)(*m_pPatternList->m_itemList.begin())->pData;
		if (index != 0)
		{
			m_pArrowLeftBtn->Enable(index - 1 > 0);
			m_pArrowRightBtn->Enable(index - 1 + 6 < 24);
			m_pPatternList->ClearItem();
			for (int i = 0; i < 6; ++i)
				m_pPatternList->AddItem((void*)(index - 1 + i));
		}
	}
}

void FrUccDrawDlg::OnDeleteBtnInit(int param)
{
	m_pDeleteBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pDeleteBtn->Enable(false);
}

void FrUccDrawDlg::OnDeleteBtnUp()
{
}

void FrUccDrawDlg::OnModifyBtnInit(int param)
{
	m_pModifyBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pModifyBtn->Enable(false);
}

void FrUccDrawDlg::OnModifyBtnUp()
{
}

void FrUccDrawDlg::OnMakeBtnInit(int param)
{
	m_pMakeBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
	m_pMakeBtn->Enable(false);
}

void FrUccDrawDlg::OnMakeBtnUp()
{
}

void FrUccDrawDlg::OnRateListInit(int param)
{
	m_pRateList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
	m_pRateList->AddItem((void*)-1);
}

void FrUccDrawDlg::OnRateListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	int rate = (item->idx + 1) * 2;
	char filename[40];
	sprintf(filename, "ucc_tool_scale_%d.tga", rate);
	Bitmap* bitmap = g_resrcmng->LoadBitmap(filename, 0, false);
	if (bitmap)
	{
		gdi->DrawTexture(bitmap,
			WRect(item->pos, (float)bitmap->Width(), (float)bitmap->Height()),
			0xffffffff, 0);
		if (rate == m_patternRate)
			gdi->DrawTexture(m_pSelectSBox2,
				WRect(item->pos, (float)m_pSelectSBox2->Width(),
					(float)m_pSelectSBox2->Height()),
				0xffffffff, 0);
	}
}

void FrUccDrawDlg::OnRateListBtnDown()
{
	FrListItem* item = m_pRateList->GetItemUnderCursor();
	if (item)
	{
		m_patternRate = (item->idx + 1) * 2;
		BuildCurrentPattern();
	}
}

void FrUccDrawDlg::OnPatternListInit(int param)
{
	m_pPatternList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
	BuildPatternList(0, true);
}

void FrUccDrawDlg::OnPatternListOwnerDraw(int param)
{
	FrListItem* item = (FrListItem*)param;
	if (!item)
		return;
	FrGraphicInterface* gdi = GetGDI();
	if (!gdi)
		return;
	bool selected = (int)item->pData == m_patternIndex;
	if (selected && m_brushType == BRUSH_ERASER)
		item->selected = false;
	gdi->DrawTexture(m_pToolBox,
		WRect(item->pos, (float)m_pToolBox->Width(),
			(float)m_pToolBox->Height()),
		0xffffffff, 0);
	Bitmap* pattern = &m_pattern[(int)item->pData];
	if (pattern)
	{
		if (selected)
			gdi->DrawTexture(&m_emptyPattern,
				WRect(item->pos, (float)m_emptyPattern.Width(),
					(float)m_emptyPattern.Height()),
				0xffffffff, 0);
		gdi->DrawTexture(pattern,
			WRect(item->pos.x + 3, item->pos.y + 3, (float)pattern->Width(),
				(float)pattern->Height()),
			0xffffffff, 0);
	}
	if (selected)
		gdi->DrawTexture(m_pSelectBox,
			WRect(item->pos.x - 2, item->pos.y - 2,
				(float)m_pSelectBox->Width(), (float)m_pSelectBox->Height()),
			0xffffffff, 0);
}

void FrUccDrawDlg::OnPatternListBtnDown()
{
	FrListItem* item = m_pPatternList->GetItemUnderCursor();
	if (item)
	{
		m_patternIndex = (int)item->pData;
		m_brushType = BRUSH_PATTERN;
		BuildCurrentPattern();
		if (!m_bPatternFolded)
			OnArrowDownBtnUp();
	}
}

void FrUccDrawDlg::OnEtcInit(int param)
{
	m_pEtc = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	ChangeTab(UCC_TAB_DRAW);
}

void FrUccDrawDlg::OnEtcOwnerDraw(int param)
{
	WRect rect = m_pCanvas->GetRect();
	if (!m_pClothes[m_side])
		return;
	rect.w = m_pClothes[m_side]->GetTexture32BPP(false)->Width();
	rect.h = m_pClothes[m_side]->GetTexture32BPP(false)->Height();
	rect = WRect(rect.x - 3, rect.y - 3, rect.w + 6, rect.h + 3);
	FrGraphicInterface* gdi = GetGDI();
	gdi->LineBox(rect, 0x33000000, 0, 0.f);
	if (m_bGrid)
	{
		float step = m_pClothes[m_side]->GetZoomRate() * 5.f;
		for (float x = rect.x; x <= (rect.x + rect.w); x += step)
			gdi->Line(WPoint(x, rect.y), WPoint(x, (rect.y + rect.h)),
				0x0f000000, 0x0f000000, 0);
		for (float y = rect.y; y <= (rect.y + rect.h); y += step)
			gdi->Line(WPoint(rect.x, y), WPoint((rect.x + rect.w), y),
				0x0f000000, 0x0f000000, 0);
	}
	WPoint pos = WPoint(GetRect().x, GetRect().y) + WPoint(191, 298);
	gdi->SetTextStyle(1);
	float width = gdi->PrintText11(pos, 0,
		m_side == 0 ? "\xbe\xd5\xb8\xe9" : "\xb5\xde\xb8\xe9", NULL);
	gdi->SetTextStyle(0);
	char text[16];
	sprintf(text, ": %d%%", m_pClothes[m_side]->GetZoomRate() * 100);
	gdi->PrintText11(pos + WPoint(width, 0), 0, text, NULL);
	WPoint flipPos = WPoint(GetRect().x, GetRect().y) + WPoint(26, 297);
	gdi->PrintText11(flipPos, 0, "\xb5\xda\xc1\xfd\xb1\xe2", NULL);
	m_cursor.Render();
	m_fadingText.Process();
}

void FrUccDrawDlg::OnToolTipInit(int param)
{
	m_pToolTip = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccDrawDlg::OnToolTipOnwerDraw(int param)
{
	m_toolTip.Process();
}

void FrUccDrawDlg::BuildBrushList()
{
	if (!m_pBrushList)
		return;
	m_pBrushList->ClearItem();
	for (int i = 0; i < UccBrushList(BRUSH_BRUSH)->GetBrushNum(); ++i)
		m_pBrushList->AddItem(UccBrushList(BRUSH_BRUSH)->GetBrush(i));
}

void FrUccDrawDlg::BuildPenList()
{
	if (!m_pPenList)
		return;
	m_pPenList->ClearItem();
	for (int i = 0; i < UccBrushList(BRUSH_PEN)->GetBrushNum(); ++i)
		m_pPenList->AddItem(UccBrushList(BRUSH_PEN)->GetBrush(i));
}

void FrUccDrawDlg::BuildEraserList()
{
	if (!m_pEraserList)
		return;
	m_pEraserList->ClearItem();
	for (int i = 0; i < UccBrushList(BRUSH_ERASER)->GetBrushNum(); ++i)
		m_pEraserList->AddItem(UccBrushList(BRUSH_ERASER)->GetBrush(i));
}

void FrUccDrawDlg::BuildPatternList(int index, bool folded)
{
	if (!m_pPatternList)
		return;
	int start = (index + 1) / 6 * 6;
	int end;
	if (folded)
	{
		int count = UccBrushList(BRUSH_PATTERN)->GetBrushNum();
		count = count < 6 ? count : 6;
		end = start + count;
		if (end > 24)
		{
			int difference = start - end;
			end = 24;
			start = difference + end;
		}
	}
	else
	{
		start = 0;
		end = 24;
	}
	m_pPatternList->ClearItem();
	for (int i = start; i < end; ++i)
		m_pPatternList->AddItem((void*)i);
}

void FrUccDrawDlg::BuildColorList()
{
	if (!m_pColorList)
		return;
	m_pColorList->ClearItem();
	for (int i = 0; i < 18; ++i)
		m_pColorList->AddItem(&m_color[i]);
}

void FrUccDrawDlg::BuildCurrentPattern()
{
	Bitmap* brush = UccBrushList(BRUSH_PATTERN)->GetBrush(m_patternIndex);
	Bitmap pattern;
	if (!brush)
		return;
	int width = brush->Width() * m_patternRate;
	int height = brush->Height() * m_patternRate;
}

void FrUccDrawDlg::ChangeTab(eUccTab tab)
{
	m_tabManager.Open(tab);
	m_currentTab = tab;
	if (!m_bPatternFolded)
		OnArrowDownBtnUp();
	switch (tab)
	{
	case UCC_TAB_DRAW:
		m_pColorList->Enable(true);
		m_pPickColorBtn->Enable(true);
		m_pCurrentColorBtn->Enable(true);
		if (m_pPenList->GetSelected())
			m_brushType = BRUSH_PEN;
		else if (m_pBrushList->GetSelected())
			m_brushType = BRUSH_BRUSH;
		else
			m_brushType = BRUSH_BRUSH;
		break;
	case UCC_TAB_PATTERN:
		m_pColorList->Enable(false);
		m_pPickColorBtn->Enable(false);
		m_pCurrentColorBtn->Enable(false);
		m_brushType = BRUSH_PATTERN;
		break;
	}
}

void FrUccDrawDlg::Reset()
{
	eDrawMode mode = m_drawMode;
	sItemInfo* item = m_pItemInfo;
	Clear();
	SetUccDrawInfo(mode, item, true);
	ChangeTab(UCC_TAB_DRAW);
	for (int i = 1; i >= 0; --i)
	{
		if (m_pClothes[i])
		{
			m_pClothes[i]->MustRefresh(2);
			m_pClothes[i]->Process();
		}
	}
}

void FrUccDrawDlg::CloseWaitingForm()
{
	if (m_pWaitingForm)
	{
		m_pWaitingForm->Close(FrOK, true);
		m_pWaitingForm = NULL;
		m_waitingTime = 0;
		m_bWaiting = false;
	}
}

void FrUccDrawDlg::SetToolbarStatus(FrButton* button)
{
	m_pGridBtn->SetStatus(FrButton::NORMAL);
	m_pDragBtn->SetStatus(FrButton::NORMAL);
	m_pZoomInBtn->SetStatus(FrButton::NORMAL);
	m_pZoomOutBtn->SetStatus(FrButton::NORMAL);
	if (m_pTabDrawBtn)
		m_pTabDrawBtn->SetStatus(FrButton::NORMAL);
	if (m_pTabPatternBtn)
		m_pTabPatternBtn->SetStatus(FrButton::NORMAL);
	if (m_pTabEmoticonBtn)
		m_pTabEmoticonBtn->SetStatus(FrButton::NORMAL);
	if (m_pTabTextBtn)
		m_pTabTextBtn->SetStatus(FrButton::NORMAL);
	if (button)
		button->SetStatus(FrButton::PRESSED);
}

bool FrUccDrawDlg::IsSavingTemporarily()
{
	return strlen(m_name) == 0;
}
