#include "uccpickcolordlg.h"
#include "uccdrawdlg.h"
#include "inputmanager.h"
#include "projectg.h"

extern Fresh* g_pFresh;

IMPLEMENT_OBJECT(FrUccPickColorDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrUccPickColorDlg, FrForm)

ON_FRESH_VI("cancel_btn", FRCMD_INIT, FrUccPickColorDlg::OnCancelBtnInit)
ON_FRESH_VV("cancel_btn", FRCMD_LBUTTONUP, FrUccPickColorDlg::OnCancelBtnUp)
ON_FRESH_VI("ok_btn", FRCMD_INIT, FrUccPickColorDlg::OnOkBtnInit)
ON_FRESH_VV("ok_btn", FRCMD_LBUTTONUP, FrUccPickColorDlg::OnOkBtnUp)
ON_FRESH_VI("usecolor_btn", FRCMD_INIT, FrUccPickColorDlg::OnUseColorBtnInit)
ON_FRESH_VV("usecolor_btn", FRCMD_LBUTTONUP, FrUccPickColorDlg::OnUseColorBtnUp)
ON_FRESH_VI("color_list", FRCMD_INIT, FrUccPickColorDlg::OnColorListInit)
ON_FRESH_VI("color_list", FRCMD_OWNERDRAW,
	FrUccPickColorDlg::OnColorListOwnerDraw)
ON_FRESH_VV("color_list", FRCMD_LBUTTONDOWN,
	FrUccPickColorDlg::OnColorListBtnDown)
ON_FRESH_VI("canvas", FRCMD_INIT, FrUccPickColorDlg::OnCanvasInit)
ON_FRESH_VI("canvas_cursor", FRCMD_INIT, FrUccPickColorDlg::OnCanvasCursorInit)
ON_FRESH_VI("brightness_canvas", FRCMD_INIT,
	FrUccPickColorDlg::OnBrightnessCanvasInit)
ON_FRESH_VI("brightness_cursor1", FRCMD_INIT,
	FrUccPickColorDlg::OnBrightnessCursor1Init)
ON_FRESH_VI("brightness_cursor2", FRCMD_INIT,
	FrUccPickColorDlg::OnBrightnessCursor2Init)
ON_FRESH_VI("new_color", FRCMD_INIT, FrUccPickColorDlg::OnNewColorInit)
ON_FRESH_VI("prev_color", FRCMD_INIT, FrUccPickColorDlg::OnPrevColorInit)

END_FRESH_MSGMAP()

FrUccPickColorDlg::FrUccPickColorDlg()
{
	Init();
}

void FrUccPickColorDlg::SetParent(FrUccDrawDlg* pParent)
{
	m_pParent = pParent;
	BuildColorList();
}

void FrUccPickColorDlg::Init()
{
	m_pParent = NULL;
	m_prevColor.Create(1, 1, 32);
	m_newColor.Create(1, 1, 32);
	m_pSelectColor = g_pFresh->RegisterBitmap("ucc_select_color");
}

void FrUccPickColorDlg::BuildColorList()
{
	if (m_pColorList && m_pParent)
	{
		m_pColorList->ClearItem();
		for (int i = 0; i < 18; ++i)
			m_pColorList->AddItem(&m_pParent->m_color[i]);
	}
}

void FrUccPickColorDlg::MoveCanvasCursor(unsigned char r, unsigned char g,
	unsigned char b)
{
	int x, y;
	unsigned char h, l, s;
	RGBToHLS(r, g, b, h, l, s);
	Bitmap* pBitmap = m_colorPicker.GetTexture32BPP(false);
	if (pBitmap)
	{
		x = pBitmap->Width() * h / 240;
		y = pBitmap->Height() - pBitmap->Height() * s / 240;
	}
	MoveCanvasCursor(x, y);
}

void FrUccPickColorDlg::MoveCanvasCursor(const WPoint& pos)
{
	WRect canvas = m_pCanvas->GetRect();
	unsigned char x = (unsigned char)(pos.x - canvas.x);
	unsigned char y = (unsigned char)(pos.y - canvas.y);
	MoveCanvasCursor(x, y);
}

void FrUccPickColorDlg::MoveCanvasCursor(unsigned char x, unsigned char y)
{
	if (m_pCanvasCursor)
	{
		WRect cursor = m_pCanvasCursor->GetRect();
		WRect canvas = m_pCanvas->GetRect();
		WPoint pos;
		pos.x = x - cursor.w * 0.5f;
		pos.y = y - cursor.h * 0.5f;
		pos.x += canvas.x;
		pos.y += canvas.y;
		if (pos.x < 0)
			pos.x = 0;
		if (pos.y < 0)
			pos.y = 0;
		m_pCanvasCursor->MoveWindow(pos);
		m_pCanvasCursor->SetVisible(true);
	}
}

void FrUccPickColorDlg::MoveBrightnessCursor(unsigned char r, unsigned char g,
	unsigned char b)
{
	unsigned char h, l, s;
	RGBToHLS(r, g, b, h, l, s);
	MoveBrightnessCursor(l);
}

void FrUccPickColorDlg::MoveBrightnessCursor(unsigned char l)
{
	if (m_pBrightnessCanvas && m_pBrightnessCursor1 && m_pBrightnessCursor2)
	{
		WRect canvas = m_pBrightnessCanvas->GetRect();
		WRect cursor1;
		cursor1 = m_pBrightnessCursor1->GetRect();
		WRect cursor2;
		cursor2 = m_pBrightnessCursor2->GetRect();
		WPoint pos1(cursor1.x, cursor1.y);
		int height = (int)canvas.h;
		pos1.y = canvas.y + (height - height * l / 255) - cursor1.h * 0.5f;
		WPoint pos2(cursor2.x, pos1.y);
		m_pBrightnessCursor1->MoveWindow(pos1);
		m_pBrightnessCursor2->MoveWindow(pos2);
	}
}

void FrUccPickColorDlg::MoveBrightnessCursor(const WPoint& pos)
{
	if (m_pBrightnessCanvas && m_pBrightnessCursor1 && m_pBrightnessCursor2)
	{
		WRect canvas = m_pBrightnessCanvas->GetRect();
		WRect cursor1;
		cursor1 = m_pBrightnessCursor1->GetRect();
		WRect cursor2;
		cursor2 = m_pBrightnessCursor2->GetRect();
		WPoint pos1(cursor1.x, cursor1.y);
		WPoint pos2(cursor2.x, cursor2.y);
		if (canvas.y <= pos.y && canvas.y + canvas.h >= pos.y)
		{
			pos1.y = pos.y;
			pos2.y = pos.y;
		}
		pos1.y -= cursor1.h * 0.5f;
		pos2.y -= cursor2.h * 0.5f;
		m_pBrightnessCursor1->MoveWindow(pos1);
		m_pBrightnessCursor2->MoveWindow(pos2);
	}
}

void FrUccPickColorDlg::OnColorListInit(int param)
{
	m_pColorList = DYNAMIC_CAST(FrListBox, (FrWnd*)param);
}

void FrUccPickColorDlg::OnColorListOwnerDraw(int param)
{
	FrListItem* pItem = (FrListItem*)param;
	if (!pItem)
		return;
	FrGraphicInterface* pGDI = GetGDI();
	if (!pGDI)
		return;
	Bitmap* pBitmap = (Bitmap*)pItem->pData;
	WRect rect(pItem->pos.x + 1.0f, pItem->pos.y + 1.0f, 9.0f, 9.0f);
	pGDI->DrawTexture(pBitmap, rect, 0xffffffff, 0);
	if (pItem->selected == true)
	{
		pGDI->DrawTexture(m_pSelectColor,
			WRect(pItem->pos.x - 3.0f, pItem->pos.y - 3.0f,
				(float)m_pSelectColor->Width(),
				(float)m_pSelectColor->Height()),
			0xffffffff, 0);
	}
}

void FrUccPickColorDlg::OnColorListBtnDown()
{
	FrListItem* pItem = m_pColorList->GetItemUnderCursor();
	if (pItem)
	{
		Bitmap* pBitmap = (Bitmap*)pItem->pData;
		if (pBitmap)
		{
			unsigned char r, g, b, a;
			pBitmap->GetPixel(0, 0, r, g, b, a);
			if (a == 255)
			{
				m_colorBrightness.GenerateBrightnessMap(r, g, b);
				SetCurrentColor(r, g, b);
				FindColor(r, g, b);
			}
		}
	}
}

void FrUccPickColorDlg::OnCancelBtnInit(int param)
{
	m_pCancelBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccPickColorDlg::OnOkBtnInit(int param)
{
	m_pOkBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccPickColorDlg::OnUseColorBtnInit(int param)
{
	m_pUseColorBtn = DYNAMIC_CAST(FrButton, (FrWnd*)param);
}

void FrUccPickColorDlg::OnCanvasInit(int param)
{
	m_pCanvas = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_colorPicker.Initialize(m_pCanvas);
	m_pCanvas->SetBgImg(m_colorPicker.GetTexture32BPP(false));
}

void FrUccPickColorDlg::OnCanvasCursorInit(int param)
{
	m_pCanvasCursor = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_pCanvasCursor->SetVisible(false);
}

void FrUccPickColorDlg::OnBrightnessCanvasInit(int param)
{
	m_pBrightnessCanvas = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_colorBrightness.Initialize(m_pBrightnessCanvas);
	unsigned char r, g, b;
	UccCurrentBrush()->GetBrushColor(r, g, b);
	m_colorBrightness.GenerateBrightnessMap(r, g, b);
	m_pBrightnessCanvas->SetBgImg(m_colorBrightness.GetTexture32BPP(false));
}

void FrUccPickColorDlg::OnBrightnessCursor1Init(int param)
{
	m_pBrightnessCursor1 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccPickColorDlg::OnBrightnessCursor2Init(int param)
{
	m_pBrightnessCursor2 = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	unsigned char r, g, b;
	UccCurrentBrush()->GetBrushColor(r, g, b);
}

void FrUccPickColorDlg::OnNewColorInit(int param)
{
	m_pNewColor = DYNAMIC_CAST(FrArea, (FrWnd*)param);
}

void FrUccPickColorDlg::OnPrevColorInit(int param)
{
	m_pPrevColor = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	unsigned char r, g, b;
	UccCurrentBrush()->GetBrushColor(r, g, b);
	m_prevColor.SetPixel(0, 0, r, g, b, 255);
	m_pPrevColor->SetBgImg(&m_prevColor);
}

void FrUccPickColorDlg::OnCancelBtnUp()
{
	OnCancel();
}

void FrUccPickColorDlg::OnOkBtnUp()
{
	if (m_pParent)
	{
		m_pParent->SetCurrentColor(m_color.r, m_color.g, m_color.b);
		FindColor(m_color.r, m_color.g, m_color.b);
	}
	OnOK();
}

void FrUccPickColorDlg::OnUseColorBtnUp()
{
	int index = -1;
	FrListItem* pItem = m_pColorList->GetSelected();
	if (pItem)
	{
		Bitmap* selected = (Bitmap*)pItem->pData;
		for (int i = 0; i < 18; ++i)
		{
			if (&m_pParent->m_color[i] == selected)
			{
				if (i < 9)
				{
					pItem = NULL;
					index = -1;
				}
				else
					index = i;
				break;
			}
		}
	}
	if (!pItem)
	{
		index = -1;
		for (int i = 0; i < 18; ++i)
		{
			unsigned char r, g, b, a;
			m_pParent->m_color[i].GetPixel(0, 0, r, g, b, a);
			if (!a)
			{
				index = i;
				break;
			}
		}
	}
	if (index == -1)
	{
		static int last = 8;
		if (++last >= 18)
			last = 9;
		index = last;
	}
	m_pParent->m_color[index].SetPixel(0, 0, m_color.r, m_color.g, m_color.b,
		255);
	RefreshTexCache(m_pParent->m_color[index]);
}

void FrUccPickColorDlg::FindColor(unsigned char r, unsigned char g,
	unsigned char b)
{
	int x, y;
	unsigned char h, l, s;
	RGBToHLS(r, g, b, h, l, s);
	Bitmap* pBitmap = m_colorPicker.GetTexture32BPP(false);
	if (pBitmap)
	{
		x = pBitmap->Width() * h / 240;
		y = pBitmap->Height() - pBitmap->Height() * s / 240;
	}
	MoveCanvasCursor(x, y);
	MoveBrightnessCursor(l);
}

void FrUccPickColorDlg::SetCurrentColor(unsigned char r, unsigned char g,
	unsigned char b)
{
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
	m_newColor.SetPixel(0, 0, r, g, b, 255);
	RefreshTexCache(m_newColor);
	m_pNewColor->SetBgImg(&m_newColor);
}

void FrUccPickColorDlg::OnProc(const float delta)
{
	const WPoint& pos = g_pFresh->GetManager()->GetMousePos();
	if (m_colorPicker.IsMouseOver(pos) && g_input->GetButton(LEFT_BUTTON) > 0)
	{
		unsigned char r, g, b;
		m_colorPicker.GetColor(pos, r, g, b);
		m_colorBrightness.GenerateBrightnessMap(r, g, b);
		SetCurrentColor(r, g, b);
		MoveCanvasCursor(pos);
		unsigned char h, l, s;
		RGBToHLS(r, g, b, h, l, s);
		MoveBrightnessCursor(l);
	}
	if (m_colorBrightness.IsMouseOver(pos) &&
		g_input->GetButton(LEFT_BUTTON) > 0)
	{
		unsigned char r, g, b;
		m_colorBrightness.GetColor(pos, r, g, b);
		SetCurrentColor(r, g, b);
		MoveBrightnessCursor(pos);
	}
}
