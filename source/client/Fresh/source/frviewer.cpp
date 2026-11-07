#include "frviewer.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "frgraphicinterface.h"
#include "frcursor.h"
#include "wresrcmng.h"
#include "westpak.h"
#include <stdio.h>
#include <stdlib.h>

IObject* FrViewerMakeInstance()
{
	return new FrViewer;
}

struct __sFrViewer
{
	__sFrViewer()
	{
		ObjectFactory().AddObjectFunctor(FrViewerMakeInstance, "FrViewer");
	}
};
const WRTTI FrViewer::m_RTTI("FrViewer", &FrWnd::m_RTTI);
static __sFrViewer __implFrViewer;

FrViewer::FrViewer()
{
	m_pView = NULL;
	m_fileName = "";
	m_bgColor = 0xffffffff;
	m_mouseEvent = true;
}

FrViewer::~FrViewer()
{
	if (m_pView)
	{
		delete m_pView;
		m_pView = NULL;
	}
}

void FrViewer::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	pManager->GetDocument();
	std::map<std::string, std::string>& param = item.m_param;
	std::string fileName("");
	bool useWheel = false;
	if (param.find("bgcolor") != param.end())
		sscanf(param["bgcolor"].c_str(), "%x", &m_bgColor);
	if (param.find("image") != param.end())
		fileName = param["image"];
	if (param.find("use_wheel") != param.end())
		useWheel = param["use_wheel"] == "true";
	if (param.find("mouse_event") != param.end())
		m_mouseEvent = atoi(param["mouse_event"].c_str()) != 0;
	m_pItem = &item;
	WRect rect(item.m_rect.left, item.m_rect.top,
		(short)(item.m_rect.right - item.m_rect.left),
		(short)(item.m_rect.bottom - item.m_rect.top));
	unsigned long style = (m_mouseEvent ? 0 : FWS_NOMOUSEEVENT) | FWS_VISIBLE;
	Create(item.m_caption.c_str(), item.m_name.c_str(), pManager, style, rect,
		pParent);
	int rowCapacity = (int)(rect.h * 0.05f);
	if (useWheel && !m_pScrBar)
	{
		m_pScrBar = new FrScrollBar;
		m_pScrBar->Init(pManager, this, 0, rowCapacity, 1, false);
		m_pScrBar->FollowBottom(false);
		m_pScrBar->SetGuideVisible(true);
	}
	if (!fileName.empty() && !Open(fileName.c_str()))
		fileName = "";
}

bool FrViewer::Open(const char* fileName)
{
	if (m_fileName != fileName)
	{
		cFile* file = g_resrcmng->GetCFile(fileName, 0);
		if (!file)
			return false;
		CloseCFile(file);
		m_fileName = fileName;
		if (m_pView)
		{
			delete m_pView;
			m_pView = NULL;
		}
		m_pView = new CBackGround(fileName, false);
		if (m_pScrBar)
		{
			m_pScrBar->ClearItem();
			int h = (int)((float)m_pView->GetHeight() * 0.05f);
			while (h--)
				m_pScrBar->AddItem();
		}
	}
	return true;
}

void FrViewer::OnDraw()
{
	FrWnd::OnDraw();
	if (m_pView)
	{
		WRect dst(m_rect.x, m_rect.y, m_rect.w, m_rect.h);
		WPoint src(0, 0);
		if (m_pScrBar)
			src.y = m_pScrBar->GetCurTopRow() * 20.0f;
		if (IsEnabled())
			m_pView->Draw(&src, &dst, FrALPHA(m_bgColor, m_wndAlpha));
		else
			m_pView->Draw(&src, &dst, FrALPHA(0x50b0b0b0, m_wndAlpha));
	}
	SendCmdToOwnerTarget(FRCMD_OWNERDRAW, 0, 0);
}

void FrViewer::OnResize()
{
	if (m_pView && m_pScrBar)
	{
		m_pScrBar->Resize((int)(m_rect.h * 0.05f), 1, false);
		m_pScrBar->ClearItem();
		int h = (int)((float)m_pView->GetHeight() * 0.05f);
		while (h--)
			m_pScrBar->AddItem();
	}
}

void FrViewer::SetMouseEvent(bool active)
{
	m_mouseEvent = active;
	if (active)
		m_dwStyle.Disable(FWS_NOMOUSEEVENT);
	else
		m_dwStyle.Enable(FWS_NOMOUSEEVENT);
}

void FrViewer::OnSetCursor(bool bInClient, const WPoint& mousePos)
{
	if (m_wndName == "over_char_sel")
	{
		if (!bInClient)
			return;
		if (m_mouseEvent)
			SetCursor(FrCursor::ROLL_OVER);
	}
	if (bInClient && m_mouseEvent)
		SetCursor(FrCursor::ROLL_OVER);
}

bool FrViewer::OnLButtonDown(const WPoint& mousePos)
{
	SendCmdToOwnerTarget(FRCMD_LBUTTONDOWN, 0, 0);
	return false;
}

bool FrViewer::OnLButtonUp(const WPoint& mousePos)
{
	SendCmdToOwnerTarget(FRCMD_LBUTTONUP, 0, 0);
	return false;
}

bool FrViewer::OnRButtonDown(const WPoint& mousePos)
{
	SendCmdToOwnerTarget(FRCMD_RBUTTONDOWN, 0, 0);
	return false;
}

bool FrViewer::OnRButtonUp(const WPoint& mousePos)
{
	SendCmdToOwnerTarget(FRCMD_RBUTTONUP, 0, 0);
	return false;
}

void FrViewer::OnDblClick(const WPoint& mousePos)
{
	SendCmdToOwnerTarget(FRCMD_DBLCLICK, 0, 0);
}
