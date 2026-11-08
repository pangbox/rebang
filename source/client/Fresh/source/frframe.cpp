#include <stdio.h>
#include <string.h>
#include "frframe.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"

IMPLEMENT_OBJECT(FrFrame, FrWnd)

FrFrame::FrFrame()
	: m_pItem(NULL), m_pFrame(NULL)
{
	for (int i = 0; i < 9; ++i)
	{
		m_pBaseBmp[i] = NULL;
		m_pSubBmp[i] = NULL;
	}

	memset(m_pBlinkBmp, 0, sizeof(m_pBlinkBmp));
	m_bBlink = false;
	m_bBlinkShow = false;
	m_captionOffset = false;
	m_fBlinkTime = 0.0f;
	m_emoAtDesc = true;
	m_bCaptionFocus = true;
	m_dwStyle.Disable(FWS_NOWHEELEVENT);
}

FrFrame::~FrFrame()
{
	Release();
}

void FrFrame::Release()
{
}

void FrFrame::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	FrElementDoc* pDoc = pManager->GetDocument();
	FrElementFrame* pFrame = pDoc->GetFrame(item.m_resource);
	m_pFrame = pFrame;
	m_pItem = &item;
	m_caption = item.m_caption;

	std::map<std::string, std::string>& param = item.m_param;
	if (param.find("description") != param.end())
		m_desc = param["description"];

	std::string filename = pFrame->m_bfrmName;
	if (param.find("bgimg") != param.end())
		filename = param["bgimg"];

	if (!filename.empty())
	{
		int start = pFrame->m_aType[0] == 2 ? 6 : 0;
		int end = pFrame->m_aType[0] == 1 ? 3 : 9;
		for (int i = start; i < end; ++i)
			m_pBaseBmp[i] =
				pDoc->GetBitmap(MakeStr("%s%02d", filename.c_str(), i));
	}

	if (pFrame->m_aType[0] == 0)
		if (!pFrame->m_sfrmName.empty())
			for (int i = 0; i < 9; ++i)
				m_pSubBmp[i] = pDoc->GetBitmap(
					MakeStr("%s%02d", pFrame->m_sfrmName.c_str(), i));

	if (pFrame->m_aType[0] == 3)
		if (!pFrame->m_cfrmName.empty())
			for (int i = 0; i < 3; ++i)
				m_pBlinkBmp[i] = pDoc->GetBitmap(
					MakeStr("%s%02d", pFrame->m_cfrmName.c_str(), i));

	m_min = m_a = m_b = m_c = WSize(0, 0);
	m_width = m_height = 0;

	WRect rect((float)item.m_rect.left, (float)item.m_rect.top,
		(short)(item.m_rect.right - item.m_rect.left),
		(short)(item.m_rect.bottom - item.m_rect.top));
	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager, 0x41, rect,
		pParent);

	pFrame->m_aPos[0] = 0;
	pFrame->m_aHeight[0] = -1;
}

void FrFrame::OnDraw()
{
	FrWnd::OnDraw();
	if (!m_pItem)
		return;

	const WPoint& pt = WPoint(m_rect.x, m_rect.y);

	if (m_pFrame->m_layers > 0)
	{
		UpdateRectInfo(0, m_pBaseBmp);
		DrawFrame(0, pt.x, pt.y, m_pBaseBmp);
	}

	unsigned long alpha = (int)(m_wndAlpha2 * m_wndAlpha * 255.0f) << 24;

	GDI()->SetTextColor(alpha | 0xffffff, alpha | 0x808080);
	GDI()->SetTextStyle(2);
	GDI()->Print(WPoint(pt.x + 10.0f, pt.y + 7.0f), 0, "%s", m_caption.c_str());

	GDI()->SetTextColor(alpha | 0x404040, 0xffffffff);
	GDI()->SetTextStyle(0);

	if (!WndManager()->HidePrivacy() || !m_nFlags.GetFlag(FWF_PRIVACY))
	{
		FrEmoticon* pEmo = WndManager()->GetEmoticon();
		float offset = m_captionOffset ? 17.0f : 26.0f;
		if (pEmo && m_emoAtDesc)
			pEmo->PrintText(
				WPoint(pt.x + 10.0f, pt.y + m_a.h + m_b.h + m_c.h - offset), 0,
				m_desc.c_str(), 0xffffffff);
		else
			GDI()->Print(
				WPoint(pt.x + 10.0f, pt.y + m_a.h + m_b.h + m_c.h - offset), 0,
				"%s", m_desc.c_str());
	}

	for (int i = 1; i < m_pFrame->m_layers; ++i)
	{
		UpdateRectInfo(i, m_pSubBmp);
		DrawFrame(i, pt.x, pt.y, m_pSubBmp);
	}
}

void FrFrame::UpdateRectInfo(int layer, const Bitmap** bmp)
{
	if (!bmp[0])
		return;

	m_min.w = bmp[0]->Width() + bmp[2]->Width();

	int type = m_pFrame->m_aType[layer];
	switch (type)
	{
	case 0:
		m_min.h = bmp[0]->Height() + bmp[6]->Height();
		break;

	case 1:
		m_min.h = bmp[0]->Height();
		break;

	case 2:
		m_min.h = bmp[6]->Height();
		break;

	case 3:
		m_min.h = bmp[0]->Height() + bmp[6]->Height();
		break;
	}

	float width = max(m_min.w, m_rect.w);
	float height =
		m_pFrame->m_aHeight[layer] < 0 ? m_rect.h : m_pFrame->m_aHeight[layer];
	height = max(m_min.h, height);

	if (layer == 0)
	{
		m_width = width;
		m_height = height;
	}

	m_a.w = bmp[0]->Width();
	m_b.w = width - m_min.w;
	m_c.w = bmp[2]->Width();
	m_a.h = type == 2 ? 0.0f : bmp[0]->Height();
	m_b.h = type == 0 || type == 3 ? height - m_min.h : 0.0f;
	m_c.h = type == 1 ? 0.0f : bmp[6]->Height();
}

void FrFrame::DrawFrame(int layer, float x, float y, const Bitmap** bmp)
{
	if (m_pFrame->m_aInvisible[layer])
		return;

	if (m_pFrame->m_aPos[layer] < 0)
		y += m_pFrame->m_aPos[layer] + m_rect.h - (m_a.h + m_b.h + m_c.h);
	else
		y += m_pFrame->m_aPos[layer];

	unsigned long color =
		(IsViewFocused() && m_bCaptionFocus) || m_bBlinkShow == true
		? m_pFrame->m_color
		: (m_pFrame->m_color & 0xff000000) |
			((m_pFrame->m_color & 0xffffff) * 4 / 5);
	color = FrALPHA(color, m_wndAlpha2 * m_wndAlpha);

	int type = m_pFrame->m_aType[layer];
	if (type == 0 || type == 1 || type == 3)
	{
		if (m_bBlinkShow)
		{
			if (m_pBlinkBmp[0])
			{
				GDI()->DrawTexture(m_pBlinkBmp[0], WRect(x, y, m_a.w, m_a.h),
					color, 0);
				GDI()->DrawTexture(m_pBlinkBmp[1],
					WRect(x + m_a.w, y, m_b.w, m_a.h), color, 0);
				GDI()->DrawTexture(m_pBlinkBmp[2],
					WRect(x + m_a.w + m_b.w, y, m_c.w, m_a.h), color, 0);
			}
		}
		else
		{
			if (bmp[0])
			{
				GDI()->DrawTexture(bmp[0], WRect(x, y, m_a.w, m_a.h), color, 0);
				GDI()->DrawTexture(bmp[1], WRect(x + m_a.w, y, m_b.w, m_a.h),
					color, 0);
				GDI()->DrawTexture(bmp[2],
					WRect(x + m_a.w + m_b.w, y, m_c.w, m_a.h), color, 0);
			}
		}
	}

	if (type == 0 || type == 3)
	{
		if (bmp[3])
		{
			GDI()->DrawTexture(bmp[3], WRect(x, y + m_a.h, m_a.w, m_b.h), color,
				0);
			GDI()->DrawTexture(bmp[4],
				WRect(x + m_a.w, y + m_a.h, m_b.w, m_b.h), color, 0);
			GDI()->DrawTexture(bmp[5],
				WRect(x + m_a.w + m_b.w, y + m_a.h, m_c.w, m_b.h), color, 0);
		}
	}

	if (type == 0 || type == 2 || type == 3)
	{
		if (bmp[6])
		{
			GDI()->DrawTexture(bmp[6],
				WRect(x, y + m_a.h + m_b.h, m_a.w, m_c.h), color, 0);
			GDI()->DrawTexture(bmp[7],
				WRect(x + m_a.w, y + m_a.h + m_b.h, m_b.w, m_c.h), color, 0);
			GDI()->DrawTexture(bmp[8],
				WRect(x + m_a.w + m_b.w, y + m_a.h + m_b.h, m_c.w, m_c.h),
				color, 0);
		}
	}
}

void FrFrame::SetBlink(bool bEnable)
{
	if (m_pBlinkBmp[0])
	{
		m_bBlink = bEnable;
		m_bBlinkShow = bEnable;
	}
	else
	{
		m_bBlink = false;
		m_bBlinkShow = false;
	}
}

void FrFrame::OnProc(const float deltaTime)
{
	if (m_bBlink)
	{
		if (m_fBlinkTime > 0.8f)
		{
			m_fBlinkTime = 0.0f;
			m_bBlinkShow = !m_bBlinkShow;
		}

		m_fBlinkTime += deltaTime;
	}
}
