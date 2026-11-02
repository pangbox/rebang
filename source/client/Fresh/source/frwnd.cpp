#include <string.h>
#include <algorithm>
#include "frwnd.h"
#include "frscrollbar.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "frform.h"
#include "frtooltip.h"
#include "soundmanager.h"

FRESH_TU_ILFILL_FRWND

extern WView* g_view;

typedef std::list<FrWnd*> FRWNDLIST;

FrToolTip* FrWnd::m_pToolTip = NULL;

IObject* FrWndMakeInstance()
{
	return new FrWnd;
}

struct __sFrWnd
{
	__sFrWnd() { ObjectFactory().AddObjectFunctor(FrWndMakeInstance, "FrWnd"); }
};

const WRTTI FrWnd::m_RTTI("FrWnd", &IObject::m_RTTI);
static __sFrWnd __implFrWnd;

FrWnd::FrWnd()
	: m_nFlags(0),
	  m_pParentWnd(NULL),
	  m_pOwner(NULL),
	  m_dwStyle(0),
	  m_wndAlpha(1.0f),
	  m_wndAlpha2(1.0f),
	  m_pWndManager(NULL),
	  m_pScrBar(NULL),
	  m_fadeTime(0.0f),
	  m_szPushSound(NULL),
	  m_dblClickTimeout(-100.0f),
	  m_dblClicked(false),
	  m_hoverOn(false),
	  m_hoverTime(0.0f),
	  m_accHoverTime(0.0f),
	  m_pToolTipData(NULL)
{
}

FrWnd::~FrWnd()
{
	FrWndManager* pManager = m_pWndManager;
	if (pManager)
	{
		pManager->DeleteTopmostWindow(this);
		pManager->ReleaseCapture(this);
	}
	DestroyChild();
	if (m_pToolTipData)
	{
		delete m_pToolTipData;
		m_pToolTipData = NULL;
	}
}

void FrWnd::CreateToolTip(FrWndManager* pManager)
{
	if (m_pToolTip == NULL)
		m_pToolTip = new FrToolTip(pManager);
}

FrToolTip* FrWnd::ToolTip()
{
	return m_pToolTip;
}

void FrWnd::DestroyToolTip()
{
	if (m_pToolTip)
		delete m_pToolTip;
	m_pToolTip = NULL;
}

void FrWnd::RemoveWindow(FrWnd* pWnd)
{
	FRWNDLIST::iterator it =
		std::find(m_childList.begin(), m_childList.end(), pWnd);
	if (it != m_childList.end())
		m_childList.erase(it);
}

bool FrWnd::Close(bool bFade)
{
	if (!m_nFlags.GetFlag(FWF_DESTROY))
	{
		if (bFade)
			m_nFlags.Enable(FWF_FADEOUT | FWF_FADING);
		else
		{
			m_nFlags.Disable(FWF_FADING);
			m_nFlags.Enable(FWF_DESTROY);
		}

		if (m_pWndManager)
			m_pWndManager->ResetKeyFocus(this);
	}

	if (m_dwStyle.GetFlag(FWS_TOPMOST))
		SetTopmost(false);

	return true;
}

void FrWnd::DestroyChild()
{
	FRWNDLIST::iterator it = m_childList.begin();
	while (it != m_childList.end())
	{
		delete *it;
		it = m_childList.erase(it);
	}
}

bool FrWnd::Create(const char* lpszWindowText, const char* lpszWindowName,
	FrWndManager* pManager, unsigned long dwStyle, const WRect& rect,
	FrWnd* pParentWnd)
{
	if (lpszWindowText)
		m_wndText = lpszWindowText;
	if (lpszWindowName)
		m_wndName = lpszWindowName;

	PreCreateWindow(pManager, dwStyle, rect, pParentWnd);
	m_dwStyle.Enable(FWS_NOWHEELEVENT);
	m_nRefID = pManager->RefIndex()++;

	if (m_dwStyle.GetFlag(FWS_TOPMOST))
		pManager->AddTopmostWindow(this);

	if (pParentWnd)
	{
		pParentWnd->AddChild(this);
		m_rect.x += pParentWnd->m_rect.x;
		m_rect.y += pParentWnd->m_rect.y;
	}

	m_dblClickTimeout = -100.0f;
	m_dblClicked = false;
	return true;
}

void FrWnd::PreCreateWindow(FrWndManager* pManager, unsigned long dwStyle,
	const WRect& rect, FrWnd* pParentWnd)
{
	m_pWndManager = pManager;
	m_dwStyle = dwStyle;
	m_rect = rect;
	m_pParentWnd = pParentWnd;
}

bool FrWnd::SendCmdToOwnerTarget(eFrCmd cmd, int var1, sFRESH_HANDLER* pHandler)
{
	if (m_nFlags.GetFlag(FWF_DESTROY))
		return false;
	return SendCmdToOwnerTarget(m_pOwner, cmd, var1, pHandler);
}

bool FrWnd::SendCmdToOwnerTarget(FrCmdTarget* pCmdTarget, eFrCmd cmd, int var1,
	sFRESH_HANDLER* pHandler)
{
	if (pCmdTarget)
	{
		std::string fullname("");
		if (this == m_pWndManager->GetDesktop())
			fullname = m_pWndManager->GetLayoutID();
		else if (this == pCmdTarget)
			fullname = ".";
		else
		{
			std::string temp;
			fullname = m_wndName;

			for (FrWnd* pParent = m_pParentWnd;;
				pParent = pParent->m_pParentWnd)
			{
				if (pParent == NULL || pParent == WndManager()->GetDesktop())
				{
					temp = m_pWndManager->GetLayoutID();
					temp += ".";
					temp += fullname;
					fullname = temp;
					break;
				}

				if (pParent == pCmdTarget)
					break;

				temp = pParent->m_wndName;
				temp += ".";
				temp += fullname;
				fullname = temp;
			}
		}

		return pCmdTarget->OnFreshMsg(fullname.c_str(), cmd, var1, pHandler);
	}
	return true;
}

FrGraphicInterface* FrWnd::GDI() const
{
	return m_pWndManager->GetGDI();
}

FrEmoticon* FrWnd::Emo() const
{
	return m_pWndManager->GetEmoticon();
}

FrWnd* FrWnd::GetParent() const
{
	return m_pParentWnd;
}

FrWnd* FrWnd::FindChild(const FrWnd* pWnd)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if (*it == pWnd)
			return *it;

		if (*it)
		{
			FrWnd* pFound = (*it)->FindChild(pWnd);
			if (pFound)
				return pFound;
		}
	}
	return NULL;
}

FrWnd* FrWnd::FindChild(const char* lpszWindowText)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if ((*it)->m_wndText == lpszWindowText)
			return *it;
	}
	return NULL;
}

FrWnd* FrWnd::FindChildByName(const char* lpszWindowName)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if ((*it)->m_wndName == lpszWindowName)
			return *it;
	}
	return NULL;
}

FrWnd* FrWnd::FindChildByStyle(unsigned long style)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if ((*it)->m_dwStyle.GetFlag(style))
			return *it;
	}
	return NULL;
}

FrWnd* FrWnd::FindChildForm(const char* lpszWindowName)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		FrWnd* pWnd = *it;
		if (IS_KINDOF(FrForm, pWnd) && (*it)->m_wndName == lpszWindowName)
			return *it;
	}
	return NULL;
}

void FrWnd::EnumerateChildWindow(bool (*callback)(FrWnd*, void*), void* parm)
{
	for (FRWNDLIST::iterator it = m_childList.begin(); it != m_childList.end();
		++it)
	{
		FrWnd* pWnd = *it;
		if (!callback(pWnd, parm))
			break;
	}
}

void FrWnd::SetOwner(FrCmdTarget* pOwner)
{
	m_pOwner = pOwner;
}

bool FrWnd::IsChild(const FrWnd* pWnd) const
{
	FrWnd* pParent = m_pParentWnd;
	while (pParent)
	{
		if (pParent == pWnd)
			return true;
		pParent = pParent->m_pParentWnd;
	}
	return false;
}

void FrWnd::CloseChild(FrWnd* pChildWnd, bool bFade, bool force)
{
	if (pChildWnd)
	{
		FRWNDLIST::iterator it =
			std::find(m_childList.begin(), m_childList.end(), pChildWnd);
		if (it != m_childList.end())
		{
			if (force || !pChildWnd->IsFixed())
				(*it)->Close(bFade);
		}
	}
	else
	{
		FRWNDLIST::iterator it = m_childList.begin();
		while (it != m_childList.end())
		{
			if ((!force && (*it)->IsFixed()) ||
				(*it)->m_nFlags.GetFlag(FWF_DESTROY))
			{
				it++;
				continue;
			}
			(*it)->Close(bFade);
			it++;
		}
	}
}

void FrWnd::CloseChildForm(bool bFade, bool force)
{
	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if (!force && (*it)->IsFixed())
			continue;
		if ((*it)->m_nFlags.GetFlag(FWF_DESTROY))
			continue;
		FrWnd* pWnd = *it;
		if (IS_KINDOF(FrForm, pWnd))
			(*it)->Close(bFade);
	}
}

void FrWnd::GetClientRect(WRect& rect) const
{
	rect = m_rect;
	ScreenToClient(rect);
}

void FrWnd::SetClientRect(const WRect& rect)
{
	WRect r = rect;
	ClientToScreen(r);
	SetRect(r);
}

void FrWnd::ClientToScreen(WRect& rect) const
{
	FrWnd* pParent = m_pParentWnd;
	while (pParent)
	{
		rect.x += pParent->m_rect.x;
		rect.y += pParent->m_rect.y;
		pParent = pParent->m_pParentWnd;
	}
}

void FrWnd::ClientToScreen(WPoint& point) const
{
	FrWnd* pParent = m_pParentWnd;
	while (pParent)
	{
		point.x += pParent->m_rect.x;
		point.y += pParent->m_rect.y;
		pParent = pParent->m_pParentWnd;
	}
}

void FrWnd::ScreenToClient(WRect& rect) const
{
	FrWnd* pParent = m_pParentWnd;
	while (pParent)
	{
		rect.x -= pParent->m_rect.x;
		rect.y -= pParent->m_rect.y;
		pParent = pParent->m_pParentWnd;
	}
}

void FrWnd::ScreenToClient(WPoint& point) const
{
	FrWnd* pParent = m_pParentWnd;
	while (pParent)
	{
		point.x -= pParent->m_rect.x;
		point.y -= pParent->m_rect.y;
		pParent = pParent->m_pParentWnd;
	}
}

void FrWnd::SetWindowTextA(const char* lpszText)
{
	if (lpszText)
		m_wndText = lpszText;
}

void FrWnd::GetWindowTextA(char* lpszTextBuf, unsigned int nBuffMax) const
{
	strcpy(lpszTextBuf, m_wndText.c_str());
}

void FrWnd::GetWindowTextA(std::string& outText) const
{
	outText = m_wndText;
}

unsigned int FrWnd::GetWindowTextLengthA() const
{
	return m_wndText.length();
}

void FrWnd::SetWindowName(const char* lpszName)
{
	if (lpszName)
		m_wndName = lpszName;
}

void FrWnd::GetWindowName(char* lpszNameBuf, unsigned int nBuffMax) const
{
	strcpy(lpszNameBuf, m_wndName.c_str());
}

void FrWnd::GetWindowName(std::string& outName) const
{
	outName = m_wndName;
}

unsigned int FrWnd::GetWindowNameLength() const
{
	return m_wndName.length();
}

void FrWnd::SetToolTipText(const std::string& text)
{
	m_wndToolTip = text;
	m_nFlags.Enable(FWF_TOOLTIPS);
	if (m_pToolTipData == NULL)
	{
		m_pToolTipData = new sToolTipData;
		m_pToolTipData->style = 0;
		m_pToolTipData->bFixWnd = FALSE;
		m_pToolTipData->posFixWnd = WPoint(0, 0);
	}
}

void FrWnd::SetFixTooltipWnd(const WPoint& pos)
{
	if (m_pToolTipData)
	{
		m_pToolTipData->bFixWnd = TRUE;
		m_pToolTipData->posFixWnd = pos;
	}
}

void FrWnd::SetUnFixToolTipWnd()
{
	if (m_pToolTipData)
		m_pToolTipData->bFixWnd = FALSE;
}

int FrWnd::IsFixedToolTip() const
{
	if (m_pToolTipData)
		return m_pToolTipData->bFixWnd;
	return FALSE;
}

const WPoint& FrWnd::GetToolTipWndPos() const
{
	if (m_pToolTipData && m_pToolTipData->bFixWnd)
		return m_pToolTipData->posFixWnd;
	return m_pWndManager->GetMousePos();
}

void FrWnd::SetToolTipFrameStyle(unsigned long style)
{
	if (m_pToolTipData)
		m_pToolTipData->style = style;
}

unsigned long FrWnd::GetToolTipFrameStyle() const
{
	if (m_pToolTipData)
		return m_pToolTipData->style;
	return 0;
}

void FrWnd::OnDisplay(bool checkTopmost, bool drawChild, bool drawOneself)
{
	if (checkTopmost && m_dwStyle.GetFlag(FWS_TOPMOST))
		return;
	if (!m_nFlags.GetFlag(FWF_INITED))
		return;
	if (!m_dwStyle.GetFlag(FWS_VISIBLE) && !m_nFlags.GetFlag(FWF_FADING_EX))
		return;
	if (m_nFlags.GetFlag(FWF_DESTROY))
		return;

	if (m_nFlags.GetFlag(FWF_FADING))
	{
		DoFadeDisplay();
		return;
	}

	if (drawOneself)
		OnDraw();

	if (drawChild)
	{
		FRWNDLIST::iterator it;
		for (it = m_childList.begin(); it != m_childList.end(); ++it)
		{
			FrWnd* pWnd = *it;
			if (m_nFlags.GetFlag(FWF_FADING))
				pWnd->m_wndAlpha = m_wndAlpha;
			pWnd->OnDisplay(true, true, true);
		}
	}
}

void FrWnd::OnDraw()
{
}

void FrWnd::DoFadeProcess(const float deltaTime, bool bExtend)
{
	if (bExtend)
	{
		if (m_nFlags.GetFlag(FWF_FADEOUT_EX))
		{
			m_fadeTime -= deltaTime * 5.0f;
			if (m_fadeTime < 0.0f)
			{
				m_fadeTime = 0.0f;
				m_nFlags.Disable(FWF_FADING_EX);
				if (m_nFlags.GetFlag(FWF_FADEELEMENT))
				{
					m_nFlags.Disable(FWF_FADEELEMENT);
					m_dwStyle.Disable(FWS_VISIBLE);
				}
				else
					Close(false);
			}
		}
		else
		{
			if (m_nFlags.GetFlag(FWF_FADEELEMENT))
				m_dwStyle.Enable(FWS_VISIBLE);

			m_fadeTime += deltaTime * 5.0f;
			if (m_fadeTime > 0.9f)
			{
				m_fadeTime = 1.0f;
				m_nFlags.Disable(FWF_FADING_EX);
				if (m_nFlags.GetFlag(FWF_FADEELEMENT))
					m_nFlags.Disable(FWF_FADEELEMENT);
			}
		}
		SetAlpha2ToChild(m_fadeTime);
	}
	else
	{
		if (m_nFlags.GetFlag(FWF_FADEOUT))
		{
			m_fadeTime -= deltaTime * 5.0f;
			if (m_fadeTime < 0.0f)
			{
				m_fadeTime = 0.0f;
				if (m_nFlags.GetFlag(FWF_FADEELEMENT))
				{
					m_nFlags.Disable(FWF_FADEELEMENT);
					m_dwStyle.Disable(FWS_VISIBLE);
				}
				else
					m_nFlags.Enable(FWF_DESTROY);
			}
		}
		else
		{
			if (m_nFlags.GetFlag(FWF_FADEELEMENT))
				m_dwStyle.Enable(FWS_VISIBLE);

			m_fadeTime += deltaTime * 5.0f;
			if (m_fadeTime > 0.9f)
			{
				m_fadeTime = 1.0f;
				m_nFlags.Disable(FWF_FADING);
				if (m_nFlags.GetFlag(FWF_FADEELEMENT))
					m_nFlags.Disable(FWF_FADEELEMENT);
			}
		}
	}
}

void FrWnd::DoFadeDisplay()
{
	float t = 1.0f - m_fadeTime;
	WRect box;
	box.x = t * m_iconRect.x + m_rect.x * m_fadeTime;
	box.y = t * m_iconRect.y + m_rect.y * m_fadeTime;
	box.w = t * m_iconRect.w + m_rect.w * m_fadeTime;
	box.h = t * m_iconRect.h + m_rect.h * m_fadeTime;
	WOverlay::DrawLineBox(g_view, box, 0, 0xffffffff);
}

void FrWnd::CheckHover(float deltaTime, bool bInClient, bool& hoverChecked)
{
	if (bInClient)
	{
		if (hoverChecked)
			return;
		hoverChecked = true;

		if (m_hoverOn)
			return;

		m_accHoverTime += deltaTime;
		if (m_accHoverTime > m_hoverTime)
		{
			m_accHoverTime = 0.0f;
			m_hoverOn = true;
			SendCmdToOwnerTarget(FRCMD_HOVERON, (int)this, NULL);
		}
	}
	else
	{
		m_accHoverTime = 0.0f;
		if (m_hoverOn)
		{
			m_hoverOn = false;
			SendCmdToOwnerTarget(FRCMD_HOVEROFF, (int)this, NULL);
		}
	}
}

void FrWnd::OnProcess(const float deltaTime, FrInputState& istate,
	bool checkTopmost, bool bUnableMode)
{
	if (checkTopmost && m_dwStyle.GetFlag(FWS_TOPMOST))
		return;

	if (!m_nFlags.GetFlag(FWF_INITED) && !m_nFlags.GetFlag(FWF_DESTROY))
	{
		m_nFlags.Enable(FWF_INITED);
		return;
	}

	if (!m_dwStyle.GetFlag(FWS_VISIBLE) && !m_nFlags.GetFlag(FWF_FADING_EX))
		return;

	if (m_nFlags.GetFlag(FWF_FADING))
	{
		DoFadeProcess(deltaTime, false);
		return;
	}

	FRWNDLIST::iterator it = --m_childList.end();
	while (it != m_childList.end())
	{
		FrWnd* pWnd = *it;
		if (pWnd->m_nFlags.GetFlag(FWF_DESTROY))
		{
			if (pWnd->m_dwStyle.GetFlag(FWS_TOPMOST))
				m_pWndManager->DeleteTopmostWindow(pWnd);
			pWnd->ResetKeyFocus();
			bool viewFocused = pWnd->m_nFlags.GetFlag(FWF_VIEWFOCUS);
			delete pWnd;
			it = m_childList.erase(it);
			if (viewFocused)
				m_pWndManager->HandOverViewFocus();
		}
		else
			pWnd->OnProcess(deltaTime, istate, true, bUnableMode);
		--it;
	}

	if (m_nFlags.GetFlag(FWF_FADING_EX))
	{
		DoFadeProcess(deltaTime, true);
		return;
	}

	if (!m_nFlags.GetFlag(FWF_INITED))
		return;

	OnProc(deltaTime);

	if (m_pWndManager->GetWheelFocus() == NULL)
	{
		FrScrollBar* pScrBar = DYNAMIC_CAST(FrScrollBar, this);
		if (pScrBar == NULL)
			pScrBar = m_pScrBar;

		if (pScrBar && !pScrBar->m_dwStyle.GetFlag(FWS_NOWHEELEVENT) &&
			pScrBar->IsBarVisible())
		{
			bool bInClient = pScrBar->GetParent()
				? pScrBar->GetParent()->m_rect.IsInRect(istate.mousePos)
				: m_rect.IsInRect(istate.mousePos);
			if (bInClient)
			{
				m_pWndManager->SetWheelFocus(pScrBar);
				if (!WisZero(istate.wheelDelta, g_EPSILON))
					OnWheel(istate);
			}
		}
		else if (IsVisible())
		{
			if (IS_KINDOF(FrForm, this) && m_rect.IsInRect(istate.mousePos))
				m_pWndManager->SetWheelFocus(
					m_pWndManager->GetDesktop()->m_pScrBar);
		}
	}

	if (!m_dwStyle.GetFlag(FWS_TOPMOST) && bUnableMode == true &&
		m_pWndManager && m_pWndManager->GetDesktop())
		return;

	if (m_nFlags.GetFlag(FWF_TOOLTIPS) && m_rect.IsInRect(istate.mousePos))
	{
		m_pToolTip->SetToolTipText(m_wndToolTip);
		m_pToolTip->SetFrameStyle(m_pToolTipData ? m_pToolTipData->style : 0);
		if (IsFixedToolTip())
			m_pToolTip->Move(m_pToolTipData->posFixWnd);
	}

	if (m_dwStyle.GetFlag(FWS_DISABLED))
		return;
	if (m_pWndManager && m_pWndManager->GetDesktop() &&
		m_pWndManager->GetDesktop()->m_dwStyle.GetFlag(FWS_DISABLED))
		return;

	if (m_dblClickTimeout > -0.1f)
	{
		m_dblClickTimeout -= deltaTime;
		if (m_dblClickTimeout < 0.0f)
			m_dblClickTimeout = -100.0f;
	}

	if (istate.keyFocused == this && istate.im)
		OnKeyFocus(istate.im);

	if (!IsViewFocused())
		return;
	if (m_dwStyle.GetFlag(FWS_NOMOUSEEVENT))
		return;

	FrWnd* pCaptured = m_pWndManager->GetCapture();
	if (pCaptured && pCaptured != this)
	{
		if (!m_pWndManager->IsValidWindow(pCaptured))
			m_pWndManager->ReleaseCapture(pCaptured);
		return;
	}

	bool bInClient = m_rect.IsInRect(istate.mousePos);
	OnSetCursor(bInClient, istate.mousePos);

	if (m_dwStyle.GetFlag(FWS_HOVER))
		CheckHover(deltaTime, bInClient, istate.hoverChecked);

	if (pCaptured == NULL && !bInClient)
		return;

	if (istate.mouse & 0x2)
		OnMouseMove(istate.mousePos);

	if (istate.mouse & 0x4)
	{
		if (m_dblClickTimeout > -0.1f && m_dblClickPos == istate.mousePos)
		{
			m_dblClicked = true;
			istate.mouse &= ~0x4;
		}
		else if (!OnLButtonDown(istate.mousePos))
		{
			istate.mouse &= ~0x4;
			if (!m_dwStyle.GetFlag(FWS_NODBLCLICK))
			{
				m_dblClickTimeout = 0.5f;
				m_dblClickPos = istate.mousePos;
			}

			if (istate.keyFocused != this)
			{
				m_pWndManager->GetDesktop()->ResetKeyFocus();
				SetKeyFocus(true);
			}
			else if (m_dwStyle.GetFlag(FWS_KEYEVENT))
				OnSelectText(&istate);
		}
	}

	if (istate.mouse & 0x8)
	{
		if (m_dblClicked)
		{
			OnDblClick(istate.mousePos);
			istate.mouse &= ~0x8;
			m_dblClickTimeout = -100.0f;
			m_dblClicked = false;
		}
		else if (!OnLButtonUp(istate.mousePos))
			istate.mouse &= ~0x8;
	}

	if (istate.mouse & 0x10)
	{
		if (!OnRButtonDown(istate.mousePos))
			istate.mouse &= ~0x10;
	}

	if (istate.mouse & 0x20)
	{
		if (!OnRButtonUp(istate.mousePos))
			istate.mouse &= ~0x20;
	}
}

void FrWnd::SetVisible(bool visible)
{
	if (!visible && m_pWndManager)
		m_pWndManager->ReleaseCapture(this);
	m_dwStyle.Turn(FWS_VISIBLE, visible);
}

void FrWnd::SetTopmost(bool topmost)
{
	if (topmost)
	{
		m_dwStyle.Enable(FWS_TOPMOST);
		m_pWndManager->AddTopmostWindow(this);
	}
	else
	{
		m_dwStyle.Disable(FWS_TOPMOST);
		m_pWndManager->DeleteTopmostWindow(this);

		FRWNDLIST::iterator it;
		for (it = m_childList.begin(); it != m_childList.end(); ++it)
			(*it)->SetTopmost(false);
	}
}

void FrWnd::EnableHover(bool enable, float time)
{
	m_dwStyle.Turn(FWS_HOVER, enable);
	m_hoverTime = time;
	m_accHoverTime = 0.0f;
}

void FrWnd::SetWheelEvent(bool enable)
{
	m_dwStyle.Turn(FWS_NOWHEELEVENT, !enable);
	if (m_pScrBar)
		m_pScrBar->SetWheelEvent(enable);
}

void FrWnd::SetViewFocus(bool takeFromOthers)
{
	FrWnd* pModal = m_pWndManager->GetModalForm();
	if (pModal && pModal != this)
		return;

	if (takeFromOthers)
		m_pWndManager->GetDesktop()->ResetViewFocus(NULL);

	FrWnd* pWnd = this;
	while (pWnd)
	{
		if (pWnd->m_pParentWnd == NULL || IS_KINDOF(FrForm, pWnd))
		{
			pWnd->m_nFlags.Enable(FWF_VIEWFOCUS);
			return;
		}
		pWnd = pWnd->m_pParentWnd;
	}
}

void FrWnd::ResetViewFocus(FrWnd* pWndStop)
{
	if (pWndStop == this)
		return;

	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		(*it)->ResetViewFocus(pWndStop);
		(*it)->ReleaseCapture();
	}
	m_nFlags.Disable(FWF_VIEWFOCUS);
}

FrWnd* FrWnd::FindViewFocused(bool enabled_visible)
{
	for (FRWNDLIST::reverse_iterator rit = m_childList.rbegin();
		rit != m_childList.rend(); rit++)
	{
		FrWnd* pWnd = *rit;
		if (pWnd->m_dwStyle.GetFlag(FWS_TOPMOST))
		{
			if (pWnd->m_nFlags.GetFlag(FWF_TOPFOCUS))
				return *rit;
		}
		else if (pWnd->m_nFlags.GetFlag(FWF_VIEWFOCUS))
		{
			if (!enabled_visible || (pWnd->IsEnabled() && pWnd->IsVisible()))
				return *rit;
		}
	}
	return NULL;
}

bool FrWnd::IsViewFocused() const
{
	FrWnd* pWnd = (FrWnd*)this;
	while (pWnd)
	{
		if (pWnd->m_nFlags.GetFlag(FWF_VIEWFOCUS) ||
			pWnd->m_dwStyle.GetFlag(FWS_FIXED))
			return true;
		if (IS_KINDOF(FrForm, pWnd))
			return false;
		pWnd = pWnd->m_pParentWnd;
	}
	return false;
}

bool FrWnd::IsTopFocus() const
{
	if (m_nFlags.GetFlag(FWF_TOPFOCUS) && m_dwStyle.GetFlag(FWS_TOPMOST))
		return true;
	return false;
}

bool FrWnd::SetKeyFocus(bool resetPrevImeData)
{
	return WndManager()->SetKeyFocus(this, resetPrevImeData);
}

bool FrWnd::ResetKeyFocus()
{
	return WndManager()->ResetKeyFocus(this);
}

void FrWnd::MoveWindow(const WPoint& pos)
{
	WPoint step = pos - m_rect.TopLeft();
	m_rect.x = pos.x;
	m_rect.y = pos.y;

	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
	{
		if (IS_KINDOF(FrForm, *it))
			continue;
		WPoint dest = (*it)->m_rect.TopLeft() + step;
		(*it)->MoveWindow(dest);
	}
}

void FrWnd::SetRect(const WRect& rect)
{
	MoveWindow(WPoint(rect.x, rect.y));
	m_rect.w = rect.w;
	m_rect.h = rect.h;
	OnResize();

	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
		(*it)->OnResize();
}

bool FrWnd::SetCapture()
{
	return WndManager()->SetCapture(this);
}

FrWnd* FrWnd::GetCapture()
{
	return m_pWndManager->GetCapture();
}

bool FrWnd::ReleaseCapture()
{
	return WndManager()->ReleaseCapture(this);
}

void FrWnd::SetCursor(int hCursor)
{
	m_pWndManager->SetCursor(hCursor);
}

int FrWnd::GetCursor() const
{
	return m_pWndManager->GetCursor();
}

void FrWnd::MoveCursor(const char* name)
{
	FrWnd* pModal = m_pWndManager->GetModalForm();
	if (pModal && this != pModal)
		return;
	m_pWndManager->MoveCursor(this, name);
}

void FrWnd::SetPushSound(const char* name_const)
{
	m_szPushSound = name_const;
}

bool FrWnd::PlayPushSound()
{
	if (m_szPushSound)
	{
		g_audio->PlaySfx(m_szPushSound);
		return true;
	}
	return false;
}

void FrWnd::SetWheelFocus()
{
	FrScrollBar* pScrBar = DYNAMIC_CAST(FrScrollBar, this);
	if (pScrBar)
		m_pWndManager->SetWheelFocus(pScrBar);
	else
		m_pWndManager->SetWheelFocus(m_pScrBar);
}

void FrWnd::AddChild(FrWnd* pChild)
{
	m_childList.push_back(pChild);
}

void FrWnd::SetElementFadeOut(bool bOut)
{
	if (m_nFlags.GetFlag(FWF_FADING_EX))
		return;

	m_nFlags.Enable(FWF_FADING_EX | FWF_FADEELEMENT);
	if (bOut)
	{
		m_nFlags.Enable(FWF_FADEOUT_EX);
		m_fadeTime = 1.0f;
	}
	else
	{
		m_nFlags.Disable(FWF_FADEOUT_EX);
		m_fadeTime = 0.0f;
	}
}

void FrWnd::SetFadeout(bool bOut)
{
	if (m_nFlags.GetFlag(FWF_FADING_EX))
		return;

	m_nFlags.Enable(FWF_FADING_EX);
	if (bOut)
	{
		m_nFlags.Enable(FWF_FADEOUT_EX);
		m_fadeTime = 1.0f;
	}
	else
	{
		m_nFlags.Disable(FWF_FADEOUT_EX);
		m_fadeTime = 0.0f;
	}
}

void FrWnd::SetAlpha2ToChild(float a)
{
	m_wndAlpha2 = a;

	FRWNDLIST::iterator it;
	for (it = m_childList.begin(); it != m_childList.end(); ++it)
		(*it)->SetAlpha2ToChild(m_wndAlpha2);
}

void FrWnd::FindNextTopFocus(bool bForce)
{
	if (!m_dwStyle.GetFlag(FWS_TOPMOST))
		return;

	m_pWndManager->ResetTopFocus();
	m_nFlags.Enable(FWF_TOPFOCUS);

	FrForm* pForm = DYNAMIC_CAST(FrForm, this);
	if (m_nFlags.GetFlag(FWF_FADING_EX) && !bForce)
		return;

	if (pForm)
		pForm->SetFrameCaptionFocus(true);
	SetAlpha2ToChild(1.0f);
}
