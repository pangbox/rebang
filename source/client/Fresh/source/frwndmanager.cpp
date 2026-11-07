#include <string.h>
#include "frwndmanager.h"
#include "frwnd.h"
#include "frdesktop.h"
#include "frcursor.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "frform.h"
#include "frframe.h"
#include "frbutton.h"
#include "frstatic.h"
#include "frtextbutton.h"
#include "frarea.h"
#include "frlistbox.h"
#include "fredit.h"
#include "frcombobox.h"
#include "frgaugebar.h"
#include "frviewer.h"
#include "frcontextmenuctrl.h"
#include "frtabbutton.h"
#include "frgroupbox.h"
#include "frmacroitem.h"
#include "frtooltip.h"
#include "../texturecache/tcachemanager.h"
#include "inputmanager.h"
#include "chatmsg.h"

typedef std::list<FrWnd*> FRWNDLIST;

bool FocusWndCompare(const void* d1, const void* d2)
{
	if (d1 == NULL)
		return false;

	if (d2 == NULL)
		return true;

	if (((const FrWnd*)d1)->GetParent() == d2)
		return false;

	if (((const FrWnd*)d2)->GetParent() == d1)
		return true;

	return ((const FrWnd*)d1)->IsTopFocus() < ((const FrWnd*)d2)->IsTopFocus();
}

FrWndManager::FrWndManager(FrElementDoc* pDoc)
	: m_pDoc(pDoc),
	  m_pDesktop(NULL),
	  m_pCursor(NULL),
	  m_pCaptured(NULL),
	  m_pDevice(NULL),
	  m_pEmoticon(NULL)
{
	m_istate.im = NULL;
	m_istate.keyFocused = NULL;
	m_istate.wheelFocused = NULL;
	m_pFocusedEdit = NULL;
	m_caretPos = WPoint(0, 0);
	m_hidePrivacy = false;
}

FrWndManager::~FrWndManager()
{
	if (m_pDesktop)
	{
		delete m_pDesktop;
		m_pDesktop = NULL;
	}
	if (m_pDevice)
	{
		delete m_pDevice;
		m_pDevice = NULL;
	}
	if (m_pCursor)
	{
		delete m_pCursor;
		m_pCursor = NULL;
	}
	if (m_pEmoticon)
	{
		delete m_pEmoticon;
		m_pEmoticon = NULL;
	}

	if (m_exclusiveKey)
	{
		if (CChatMsg::IsInstantiated())
			CChatMsg::Instance()->SetActive(false, true, true);
	}
}

void FrWndManager::CreateToolTip()
{
	FrWnd::CreateToolTip(this);
}

void FrWndManager::DestroyToolTip()
{
	FrWnd::DestroyToolTip();
}

bool FrWndManager::Init(const char* wallPaper, bool exclusiveKey)
{
	m_exclusiveKey = exclusiveKey;
	m_cursorIndex = 0;
	memset(&m_istate, 0, sizeof(m_istate));

	CheckSystemStatus();

	if (m_pDevice == NULL)
		m_pDevice = new FrGraphicInterface(g_view);

	m_pDevice->Reset();

	if (m_pDesktop)
	{
		delete m_pDesktop;
		m_pDesktop = NULL;
	}
	m_pDesktop = new FrDesktop;
	m_pDesktop->m_pWndManager = this;
	m_pDesktop->Init(g_view->GetWidth(), g_view->GetHeight());
	m_pDesktop->SetWallPaper(wallPaper, true);

	if (m_pCursor == NULL)
		m_pCursor = new FrCursor;

	m_pCursor->Init();

	if (m_pEmoticon == NULL)
		m_pEmoticon = new FrEmoticon(this);

	m_pEmoticon->Init();

	if (m_exclusiveKey)
		CChatMsg::Instance()->SetActive(true, true, true);

	return true;
}

void FrWndManager::RemoveFade()
{
	FRWNDLIST::reverse_iterator rit = m_pDesktop->m_childList.rbegin();
	for (; rit != m_pDesktop->m_childList.rend(); ++rit)
		(*rit)->m_fadeTime = -1.0f;
}

void FrWndManager::Display(bool drawDesktop)
{
	m_pFocusedEdit = NULL;

	m_pDesktop->OnDisplay(true, true, drawDesktop);

	for (FRWNDLIST::iterator it = m_topmostList.begin();
		it != m_topmostList.end(); ++it)
	{
		FrWnd* pWnd = *it;

		if (pWnd->m_pParentWnd)
		{
			if (!pWnd->m_pParentWnd->IsVisible())
				continue;
		}

		pWnd->OnDisplay(false, true, true);
	}

	if (FrWnd::IsToolTipInstantiated())
		FrWnd::ToolTip()->OnDisplay();
}

void FrWndManager::ResetTopFocus()
{
	if (m_topmostList.size() == 0)
		return;

	FRWNDLIST::iterator i;
	for (i = m_topmostList.begin(); i != m_topmostList.end(); ++i)
	{
		FrWnd* pWnd = *i;

		if (pWnd)
		{
			pWnd->m_nFlags.Disable(FWF_TOPFOCUS);

			if (!strcmp(pWnd->m_wndName.c_str(), "messenger_chat") &&
				!pWnd->m_nFlags.GetFlag(FWF_DESTROY))
			{
				FrForm* pForm = DYNAMIC_CAST(FrForm, pWnd);
				if (pForm)
					pForm->SetFrameCaptionFocus(false);

				if (!pWnd->m_nFlags.GetFlag(FWF_FADING_EX))
					pWnd->SetAlpha2ToChild(0.7f);
			}
		}
	}
}

void FrWndManager::CheckSystemStatus()
{
	m_istate.wheelDelta = g_input->GetMouseDelta().z;
	m_istate.mouse = 0;

	m_istate.mousePos =
		WPoint(g_input->GetMousePoint().x, g_input->GetMousePoint().y);

	if (m_istate.mousePos != m_istate.oldMousePos)
	{
		m_istate.oldMousePos = m_istate.mousePos;
		m_istate.mouse |= FW_MOUSE_MOVE;
	}

	switch (g_input->GetButton(LEFT_BUTTON))
	{
	case 1:
		m_istate.mouse |= FW_LBUT_DOWN;
		break;
	case 2:
		m_istate.mouse |= FW_LBUT_UP;
		break;
	}

	switch (g_input->GetButton(RIGHT_BUTTON))
	{
	case 1:
		m_istate.mouse |= FW_RBUT_DOWN;
		break;
	case 2:
		m_istate.mouse |= FW_RBUT_UP;
		break;
	}

	m_istate.hoverChecked = false;
	m_istate.im = CChatMsg::Instance();

	if (m_exclusiveKey && !m_istate.im->IsActive())
		m_istate.im->SetActive(true, false, false);
}

void FrWndManager::Process(const float deltaTime)
{
	if (m_state == OPENING && m_pDevice)
	{
		m_fadeoutAlpha += 0.02f;
		if (m_fadeoutAlpha > 1.0f)
		{
			m_fadeoutAlpha = 1.0f;
			m_state = OPENED;
		}
		m_pDevice->SetAlpha(m_fadeoutAlpha);
	}

	int cursor = m_pCursor->GetCursor();

	if (m_exclusiveKey)
		m_cursorIndex = 0;
	else
		m_cursorIndex = cursor;

	CheckSystemStatus();
	FrWnd* pOldKeyFocused = m_istate.keyFocused;
	ProcessHotKey();

	m_istate.wheelFocused = NULL;
	bool bFocus = false;

	if (FrWnd::IsToolTipInstantiated())
		FrWnd::ToolTip()->OnProcess(deltaTime);

	FrWnd* pTopWnd = NULL;
	for (FRWNDLIST::reverse_iterator it = m_topmostList.rbegin();
		it != m_topmostList.rend(); ++it)
	{
		FrWnd* pWnd = *it;
		pWnd->OnProcess(deltaTime, m_istate, false, false);

		if (pWnd->IsVisible())
		{
			WRect rc;
			pWnd->GetClientRect(rc);
			if (rc.IsInRect(m_istate.mousePos))
			{
				bFocus = true;
				pTopWnd = pWnd;
			}
		}
	}

	if (pTopWnd && !pTopWnd->m_nFlags.GetFlag(FWF_DESTROY) &&
		pTopWnd->m_nFlags.GetFlag(FWF_TOOLTIPS) &&
		FrWnd::IsToolTipInstantiated())
	{
		FrWnd::ToolTip()->SetToolTipText(pTopWnd->GetToolTipText());
		FrWnd::ToolTip()->SetFrameStyle(pTopWnd->GetToolTipFrameStyle());
		if (pTopWnd->IsFixedToolTip())
			FrWnd::ToolTip()->Move(pTopWnd->GetToolTipWndPos());
	}

	ProcessKey(pOldKeyFocused);
	m_pDesktop->OnProcess(deltaTime, m_istate, true, bFocus);

	if (m_cursorIndex != cursor)
		m_pCursor->SetCursor((FrCursor::eCursor)m_cursorIndex);
}

void FrWndManager::CloseWindow(FrWnd* pWnd, bool bFade)
{
	m_pDesktop->CloseChild(pWnd, bFade, false);
}

void FrWndManager::CloseForm(bool bFade)
{
	m_pDesktop->CloseChildForm(bFade, false);
}

void FrWndManager::ProcessHotKey()
{
	if (g_input->GetDown("TAB", true) && !g_input->Get("LALT", true) &&
		!g_input->Get("RALT", true))
		MoveKeyFocusToNext(true);

	if (g_input->GetDown("ESCAPE", true) == 1)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, m_pDesktop->FindViewFocused(true));
		if (pForm && pForm->GetFlag(FrESCAPE) &&
			pForm->m_nFlags.GetFlag(FWF_INITED) &&
			!pForm->m_nFlags.GetFlag(FWF_FADEOUT))
		{
			pForm->OnCancel();
			g_input->ExclusiveGetDownUseDone("ESCAPE");
		}
	}

	if (g_input->GetDown("ENTER", true) == 1 ||
		g_input->GetDown("PADENTER", true) == 1)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, m_pDesktop->FindViewFocused(true));
		if (pForm && pForm->GetFlag(FrENTER) &&
			pForm->m_nFlags.GetFlag(FWF_INITED))
		{
			pForm->OnOK();
			g_input->ExclusiveGetDownUseDone("ENTER");
			g_input->ExclusiveGetDownUseDone("PADENTER");
		}
	}
}

void FrWndManager::ProcessKey(FrWnd* pOldKeyFocused)
{
	m_istate.im->ProcessMacro(0.01f);

	if (g_input->GetButton(LEFT_BUTTON) == 1)
		m_topmostList.sort(FocusWndCompare);

	if ((m_istate.keyFocused == NULL ||
			(pOldKeyFocused && pOldKeyFocused == m_istate.keyFocused)) &&
		(m_istate.keyFocused || !MoveKeyFocusToNext(false)))
		return;

	const char* text = m_istate.keyFocused->OnSelectText(NULL);
	if (text)
	{
		m_istate.keyFocused->SetWindowTextA(text);
		m_istate.im->Reset();
		m_istate.im->SetChatText(text, false);
	}
	else
	{
		std::string text;
		m_istate.keyFocused->GetWindowTextA(text);
		m_istate.im->Reset();
		m_istate.im->SetChatText(text.c_str(), false);
	}
}

void FrWndManager::CloseLayout()
{
	m_pDesktop->SendCmdToOwnerTarget(FRCMD_DESTROY, 0, NULL);

	m_pDesktop->CloseChild(NULL, false, true);
	m_pDesktop->SetOwner(NULL);
	m_layoutID = "";
}

bool FrWndManager::CreateLayout(const char* layout, FrCmdTarget* owner,
	bool firstTime)
{
	FrElementLayout* pElement = m_pDoc->GetLayout(layout);
	if (pElement == NULL)
		return false;

	m_layoutID = layout;
	m_pDesktop->m_pOwner = owner;
	m_pDesktop->ReleaseCapture();
	m_pDesktop->SetViewFocus(true);

	m_istate.keyFocused = NULL;
	m_pDesktop->SendCmdToOwnerTarget(FRCMD_INIT, 0, NULL);

	std::list<FrGuiItem*>::iterator it = pElement->m_guiList.begin();
	std::list<FrGuiItem*>::iterator end = pElement->m_guiList.end();
	for (; it != end; ++it)
		DoCreate(**it, this, m_pDesktop, owner);

	m_pDesktop->SetAniBg(pElement->m_aniBg);

	m_pDesktop->SendCmdToOwnerTarget(FRCMD_FINISH, 0, NULL);

	if (!firstTime)
	{
		m_state = OPENING;
		m_fadeoutAlpha = 0.0f;
	}

	return true;
}

struct sFrCreateSub
{
	FrGuiItem* pItem;
	FrWndManager* pManager;
	FrWnd* pParent;
	FrCmdTarget* pOwner;
};

template <class T>
T* DoCreateItem(const sFrCreateSub& m)
{
	T* pItem = new T;
	pItem->SetOwner(m.pOwner);
	pItem->Init(*m.pItem, m.pManager, m.pParent);
	return pItem;
}

void DoCreateMaroItem(const sFrCreateSub& m)
{
	FrMacroItem mcr;
	mcr.SetOwner(m.pOwner);
	mcr.Init(*m.pItem, m.pManager, m.pParent);
}

FrWnd* FrWndManager::DoCreate(FrGuiItem& item, FrWndManager* pManager,
	FrWnd* pParent, FrCmdTarget* pOwner)
{
	FrWnd* pWnd = NULL;
	sFrCreateSub m;
	m.pItem = &item;
	m.pManager = this;
	m.pOwner = pOwner;
	m.pParent = pParent;

	switch (item.m_type)
	{
	case GI_FORM:
		pWnd = DoCreateItem<FrForm>(m);
		break;
	case GI_FRAME:
		pWnd = DoCreateItem<FrFrame>(m);
		break;
	case GI_BUTTON:
		pWnd = DoCreateItem<FrButton>(m);
		break;
	case GI_STATIC:
		pWnd = DoCreateItem<FrStatic>(m);
		break;
	case GI_TEXTBUTTON:
		pWnd = DoCreateItem<FrTextButton>(m);
		break;
	case GI_AREA:
		pWnd = DoCreateItem<FrArea>(m);
		break;
	case GI_LISTBOX:
		pWnd = DoCreateItem<FrListBox>(m);
		break;
	case GI_EDIT:
		pWnd = DoCreateItem<FrEdit>(m);
		break;
	case GI_COMBOBOX:
		pWnd = DoCreateItem<FrComboBox>(m);
		break;
	case GI_GAUGEBAR:
		pWnd = DoCreateItem<FrGaugeBar>(m);
		break;
	case GI_GAUGEBAREX:
		pWnd = DoCreateItem<FrGaugeBarEx>(m);
		break;
	case GI_GAUGEBARIMAGE:
		pWnd = DoCreateItem<FrGaugeBarImage>(m);
		break;
	case GI_VIEWER:
		pWnd = DoCreateItem<FrViewer>(m);
		break;
	case GI_CONTEXTMENU:
		pWnd = DoCreateItem<FrContextMenuCtrl>(m);
		break;
	case GI_TABBUTTON:
		pWnd = DoCreateItem<FrTabButton>(m);
		break;
	case GI_GROUPBOX:
		pWnd = DoCreateItem<FrGroupBox>(m);
		break;
	case GI_COMBOCTLEX:
		pWnd = DoCreateItem<FrComboCtlEx>(m);
		break;
	case GI_MACROITEM:
		pWnd = NULL;
		DoCreateMaroItem(m);
		break;
	}

	if (pWnd)
		pWnd->SendCmdToOwnerTarget(FRCMD_INIT, (int)pWnd, NULL);

	return pWnd;
}

FrElementDoc* FrWndManager::GetDocument() const
{
	return m_pDoc;
}

const Bitmap* FrWndManager::GetBitmap(const char* resource,
	const char* id) const
{
	if (m_pDoc)
		return m_pDoc->GetBitmap(id ? std::string(id) : std::string(""));
	return NULL;
}

bool FrWndManager::IsValidWindow(FrWnd* pWnd)
{
	if (m_pDesktop == pWnd)
		return true;

	return m_pDesktop->FindChild(pWnd) != NULL;
}

bool FrWndManager::AddTopmostWindow(FrWnd* pWnd)
{
	for (FRWNDLIST::iterator it = m_topmostList.begin();
		it != m_topmostList.end(); ++it)
	{
		if (*it == pWnd)
			return false;
	}

	m_topmostList.push_back(pWnd);
	return true;
}

bool FrWndManager::DeleteTopmostWindow(FrWnd* pWnd)
{
	for (FRWNDLIST::iterator it = m_topmostList.begin();
		it != m_topmostList.end(); ++it)
	{
		if (*it == pWnd)
		{
			m_topmostList.erase(it);
			return true;
		}
	}

	return false;
}

FrWnd* FrWndManager::GetModalForm()
{
	FRWNDLIST::reverse_iterator rit = m_pDesktop->m_childList.rbegin();
	for (; rit != m_pDesktop->m_childList.rend(); ++rit)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, *rit);

		if (pForm && !pForm->GetFlag(FrMODALESS) &&
			!(pForm->m_nFlags.GetFlag(FWF_FADING) &&
				pForm->m_nFlags.GetFlag(FWF_FADEOUT)) &&
			!pForm->m_nFlags.GetFlag(FWF_DESTROY) && *rit != m_pDesktop)
			return *rit;
	}

	return NULL;
}

FrWnd* FrWndManager::HandOverViewFocus()
{
	FrWnd* pModal = GetModalForm();
	if (pModal)
	{
		pModal->m_nFlags.Enable(FWF_VIEWFOCUS);
		return NULL;
	}

	FRWNDLIST::reverse_iterator rit = m_pDesktop->m_childList.rbegin();
	for (; rit != m_pDesktop->m_childList.rend(); ++rit)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, *rit);

		if (pForm && pForm->IsVisible() &&
			pForm->m_dwStyle.GetFlag(FWS_KEYEVENT))
		{
			pForm->m_nFlags.Enable(FWF_VIEWFOCUS);

			if (pForm->m_dwStyle.GetFlag(FWS_FIXED))
				break;

			return pForm;
		}
	}

	m_pDesktop->m_nFlags.Enable(FWF_VIEWFOCUS);
	return m_pDesktop;
}

bool FrWndManager::SetKeyFocus(FrWnd* pWnd, bool resetPrevImeData)
{
	if (pWnd == NULL ||
		(pWnd->m_dwStyle.GetFlag(FWS_KEYEVENT) && pWnd->IsVisible() &&
			!pWnd->m_dwStyle.GetFlag(FWS_DISABLED)))
	{
		if (m_istate.keyFocused)
		{
			m_istate.keyFocused->m_nFlags.Disable(FWF_KEYFOCUS);
			m_istate.keyFocused->SendCmdToOwnerTarget(FRCMD_LOSTKEYFOCUS,
				(int)&m_istate, NULL);
		}

		if (pWnd)
		{
			pWnd->EnableKeyFocus(m_istate);

			if (resetPrevImeData)
			{
				if (m_exclusiveKey)
				{
					CChatMsg::Instance()->Reset();
					std::string text;
					pWnd->GetWindowTextA(text);
					CChatMsg::Instance()->SetChatText(text.c_str(), false);
				}
			}
		}

		m_istate.keyFocused = pWnd;
		return true;
	}

	return false;
}

bool FrWndManager::CanGetKeyFocus(FrWnd* pWnd)
{
	if (pWnd == NULL ||
		(pWnd->m_dwStyle.GetFlag(FWS_KEYEVENT) && pWnd->IsVisible() &&
			!pWnd->m_dwStyle.GetFlag(FWS_DISABLED)))
		return true;

	return false;
}

bool FrWndManager::ResetKeyFocus(FrWnd* pWnd)
{
	if (m_istate.keyFocused == pWnd)
	{
		m_istate.keyFocused = NULL;
		pWnd->m_nFlags.Disable(FWF_KEYFOCUS);
		pWnd->SendCmdToOwnerTarget(FRCMD_LOSTKEYFOCUS, (int)&m_istate, NULL);
		return true;
	}

	for (FRWNDLIST::iterator it = pWnd->m_childList.begin();
		it != pWnd->m_childList.end(); ++it)
	{
		if (ResetKeyFocus(*it) == true)
			return true;
	}

	return false;
}

bool FrWndManager::MoveKeyFocusToNext(bool resetPrevImeData)
{
	FrWnd* pParent;

	if (m_istate.keyFocused)
	{
		pParent = m_istate.keyFocused->GetParent();
		if (pParent == NULL)
			return false;
	}
	else
	{
		pParent = m_pDesktop->FindViewFocused(true);
		if (pParent == NULL)
			pParent = m_pDesktop;
	}

	m_topmostList.sort(FocusWndCompare);

	FRWNDLIST::iterator it;
	for (it = pParent->m_childList.begin(); it != pParent->m_childList.end();
		++it)
	{
		if (m_istate.keyFocused == *it)
		{
			++it;
			break;
		}
	}

	FRWNDLIST::iterator itNext;
	for (itNext = it; itNext != pParent->m_childList.end(); ++itNext)
	{
		if (SetKeyFocus(*itNext, resetPrevImeData))
			return true;
	}

	for (itNext = pParent->m_childList.begin(); itNext != it; ++itNext)
	{
		if (SetKeyFocus(*itNext, resetPrevImeData))
			return true;
	}

	return false;
}

void FrWndManager::SetWheelFocus(FrScrollBar* pScrBar)
{
	m_istate.wheelFocused = pScrBar;
}

void FrWndManager::MoveCursor(FrWnd* pParent, const char* name)
{
	FrWnd* pWnd = pParent->FindChildByName(name);
	if (pWnd)
	{
		WRect rect = pWnd->GetRect();
		m_pCursor->MoveCursor(
			WPoint(rect.x + rect.w * 0.5f, rect.y + rect.h * 0.5f));
	}
}

bool FrWndManager::SetCapture(FrWnd* pWnd)
{
	if (m_pCaptured == NULL)
	{
		m_pCaptured = pWnd;
		return true;
	}

	return false;
}

bool FrWndManager::ReleaseCapture(FrWnd* pWnd)
{
	if (m_pCaptured == pWnd)
	{
		m_pCaptured = NULL;
		return true;
	}

	return false;
}

bool FrWndManager::HasEscKeyWindow()
{
	FRWNDLIST::reverse_iterator rit = m_pDesktop->m_childList.rbegin();
	for (; rit != m_pDesktop->m_childList.rend(); ++rit)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, *rit);
		if (pForm && pForm->GetFlag(FrESCAPE))
			return true;
	}

	return false;
}

bool FrWndManager::HasValidWindow()
{
	FRWNDLIST::reverse_iterator rit = m_pDesktop->m_childList.rbegin();
	for (; rit != m_pDesktop->m_childList.rend(); ++rit)
	{
		FrForm* pForm = DYNAMIC_CAST(FrForm, *rit);
		if (pForm && !pForm->m_dwStyle.GetFlag(FWS_FIXED))
			return true;
	}

	return false;
}

WPoint FrWndManager::GetCreatePosition(const WSize& rectSize)
{
	const WRect& rc = m_pDesktop->GetRect();

	WPoint pos(rc.x + (int)((rc.w - rectSize.w + 1.0f) * 0.5f),
		rc.y + (int)((rc.h - rectSize.h + 1.0f) * 0.5f));

	WRect dr(pos.x, pos.y, rectSize.w, rectSize.h);
	ConfineRect(dr);

	return WPoint(dr.x, dr.y);
}

void FrWndManager::ConfineRect(WRect& dr)
{
	const WRect& rc = m_pDesktop->GetRect();
	if (dr.x < rc.x)
		dr.x = rc.x;
	if (dr.y < rc.y)
		dr.y = rc.y;
	if (dr.x > rc.Right() - 100.0f)
		dr.x = rc.Right() - 100.0f;
	if (dr.y > rc.Bottom() - 30.0f)
		dr.y = rc.Bottom() - 30.0f;
}

void FrWndManager::SetExclusiveKey(bool set)
{
	m_exclusiveKey = set;
	if (CChatMsg::IsInstantiated())
		CChatMsg::Instance()->SetActive(set, true, false);
}

float FrWndManager::PrintText(const WPoint& pos, unsigned long align,
	const char* text, float limit, unsigned long emoDiffuse)
{
	if (m_pDevice == NULL)
		return 0.0f;

	if (m_pEmoticon)
	{
		if (limit > 0.0f && m_pEmoticon->GetTextWidth(text) > limit)
			return m_pEmoticon->PrintText(pos, align,
				m_istate.im->MakeShortID(text, limit), emoDiffuse);
		return m_pEmoticon->PrintText(pos, align, text, emoDiffuse);
	}

	if (limit > 0.0f && m_pDevice->GetTextExtend(text) > limit)
		return m_pDevice->PrintText(pos, align,
			m_istate.im->MakeShortID(text, limit), NULL);
	return m_pDevice->PrintText(pos, align, text, NULL);
}

float FrWndManager::PrintText11(const WPoint& pos, unsigned long align,
	const char* text, float limit, unsigned long emoDiffuse)
{
	if (m_pDevice == NULL)
		return 0.0f;

	if (m_pEmoticon)
	{
		if (limit > 0.0f && m_pEmoticon->GetTextWidth11(text) > limit)
			return m_pEmoticon->PrintText11(pos, align,
				m_istate.im->MakeShortID(text, limit), emoDiffuse);
		return m_pEmoticon->PrintText11(pos, align, text, emoDiffuse);
	}

	if (limit > 0.0f && m_pDevice->GetTextExtend11(text) > limit)
		return m_pDevice->PrintText11(pos, align,
			m_istate.im->MakeShortID(text, limit), NULL);
	return m_pDevice->PrintText11(pos, align, text, NULL);
}

float FrWndManager::GetTextWidth(const char* text)
{
	if (m_pDevice == NULL)
		return 0.0f;

	if (m_pEmoticon)
		return m_pEmoticon->GetTextWidth(text);

	return m_pDevice->GetTextExtend(text);
}

void FrWndManager::IME_ShowCandWindow(FrEdit* pFocusedEdit, WPoint& caretPos)
{
	m_pFocusedEdit = pFocusedEdit;
	m_caretPos = caretPos;
}
