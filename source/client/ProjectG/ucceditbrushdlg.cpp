#include "ucceditbrushdlg.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frarea.h"
#include "inputmanager.h"

extern Fresh* g_pFresh;

CUccEditBrush::CUccEditBrush()
{
}

CUccEditBrush::~CUccEditBrush()
{
}

void CUccEditBrush::Process()
{
	CUccBase::Process();

	RefreshTexCache(m_pTexture);
}

void CUccEditBrush::DrawLine(IPoint from, IPoint to, Bitmap* brush)
{
	if (!brush)
		return;

	int ady = abs(to.y - from.y);
	int adx = abs(to.x - from.x);
	int steps = Max(adx, ady);

	float sx = (float)(to.x - from.x) / steps;
	float sy = (float)(to.y - from.y) / steps;
	WPoint cur((float)from.x, (float)from.y);

	while (true)
	{
		WPoint pos((float)((int)cur.x / 10 * 10 + 5),
			(float)((int)cur.y / 10 * 10 + 5));
		Draw((int)pos.x, (int)pos.y, brush, false);

		if ((steps == adx && cur.x == to.x) || (steps == ady && cur.y == to.y))
			break;

		cur.x += sx;
		cur.y += sy;
	}

	MustRefresh(4);
}

Bitmap* CUccEditBrush::GetTexture32BPP()
{
	Convert24to32(m_pTexture, m_pTexture32, NULL);
	return m_pTexture32;
}

Bitmap* CUccEditBrush::ProduceOriginalBrush()
{
	m_originalBrush.Create(24, 24, 32);

	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 24; x++)
		{
			BYTE r, g, b;
			m_pTexture->GetPixel(x * 10, y * 10, r, g, b);
			m_originalBrush.SetPixel(x, y, r, g, b, 0xff);
		}
	}

	return &m_originalBrush;
}

IMPLEMENT_OBJECT(FrUccEditBrushDlg, FrForm)

BEGIN_FRESH_MSGMAP(FrUccEditBrushDlg, FrForm)

ON_FRESH_VI("canvas", FRCMD_INIT, FrUccEditBrushDlg::OnCanvasInit)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrUccEditBrushDlg::OnOKBtnUp)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrUccEditBrushDlg::OnCancelBtnUp)

END_FRESH_MSGMAP()

FrUccEditBrushDlg::FrUccEditBrushDlg()

{
	m_index = -2;
	m_defaultBrush[0].Create(10, 10, 32);
	m_defaultBrush[1].Create(10, 10, 32);

	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			m_defaultBrush[0].SetPixel(x, y, 0, 0, 0, 0xff);
			m_defaultBrush[1].SetPixel(x, y, 0xff, 0xff, 0xff, 0xff);
		}
	}
	m_pCanvasImage = NULL;
}

FrUccEditBrushDlg::~FrUccEditBrushDlg()
{
}

void FrUccEditBrushDlg::InitNew()
{
	Bitmap* texture = new Bitmap(240, 240, 32);
	for (int y = 0; y < 240; y++)
	{
		for (int x = 0; x < 240; x++)
		{
			texture->SetPixel(x, y, 0xff, 0xff, 0xff, 0xff);
		}
	}

	m_editBrush.SetTexture(texture);

	if (texture)
		delete texture;

	m_pCanvasImage = m_editBrush.GetTexture();
	m_pCanvas->SetBgImg(m_pCanvasImage);
}

void FrUccEditBrushDlg::InitModify()
{
	Bitmap* texture = new Bitmap(240, 240, 32);
	Bitmap* brush = UccBrushList(m_type)->GetBrush(m_index);
	if (!brush)
	{
		if (texture)
			delete texture;
		return;
	}

	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 24; x++)
		{
			BYTE r, g, b;
			brush->GetPixel(x, y, r, g, b);
			for (int i = y * 10; i < y * 10 + 10; i++)
			{
				for (int j = x * 10; j < x * 10 + 10; j++)
				{
					texture->SetPixel(j, i, r, g, b, 0xff);
				}
			}
		}
	}

	m_editBrush.SetTexture(texture);
	if (texture)
		delete texture;

	m_pCanvasImage = m_editBrush.GetTexture();
	m_pCanvas->SetBgImg(m_pCanvasImage);
}

void FrUccEditBrushDlg::SetIndex(eBrushType type, int index)
{
	if (m_index != -2)
		return;

	m_index = index;
	m_type = type;

	if (m_index == -1)
		InitNew();
	else

		InitModify();
}

void FrUccEditBrushDlg::OnCanvasInit(int param)
{
	m_pCanvas = DYNAMIC_CAST(FrArea, (FrWnd*)param);
	m_editBrush.SetArea(m_pCanvas);
}

void FrUccEditBrushDlg::OnOKBtnUp()
{
	if (m_index != -2)
	{
		Bitmap* brush = m_editBrush.ProduceOriginalBrush();

		GetGDI();

		if (m_index == -1)
		{
			UccBrushList(m_type)->AddBrush(brush);
		}
		else
		{
			UccBrushList(m_type)->SetBrush(m_index, brush);

			RefreshTexCache(UccBrushList(m_type)->GetBrush(m_index));
		}
	}

	OnFreshOkay();
}

void FrUccEditBrushDlg::OnCancelBtnUp()
{
	OnFreshCancel();
}

void FrUccEditBrushDlg::OnProc(const float dt)
{
	CUccEditBrush* pBrush = &m_editBrush;
	const WPoint& mousePos = g_pFresh->GetManager()->GetMousePos();

	if (pBrush->IsMouseOver(mousePos))
	{
		WRect rect = m_pCanvas->GetRect();

		WPoint pos((float)((int)(mousePos.x - rect.x) / 10) * 10.0f + rect.x +
				5.0f,
			(float)((int)(mousePos.y - rect.y) / 10) * 10.0f + rect.y + 5.0f);

		if (g_input->GetButton(LEFT_BUTTON) > 0)
		{
			pBrush->SetDefaultBrush(&m_defaultBrush[0]);

			pBrush->MouseEventHandler(LEFT_BUTTON,
				g_input->GetButton(LEFT_BUTTON), pos);
		}
		else if (g_input->GetButton(RIGHT_BUTTON) > 0)
		{
			pBrush->SetDefaultBrush(&m_defaultBrush[1]);
			pBrush->MouseEventHandler(RIGHT_BUTTON,
				g_input->GetButton(RIGHT_BUTTON), pos);
		}
	}

	pBrush->Process();
}
