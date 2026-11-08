#include <stdio.h>
#include <string.h>
#include "frform.h"
#include "frframe.h"
#include "frstatic.h"
#include "frtext.h"
#include "fredit.h"
#include "frwndmanager.h"
#include "frelement.h"
#include "soundmanager.h"

FRESH_TU_ILFILL

extern WView* g_view;

bool FrForm::ms_bHasTail = true;
bool FrForm::ms_bExclusive = false;
WPoint FrForm::ms_oldPos;

IMPLEMENT_OBJECT(FrForm, FrWnd)

BEGIN_FRESH_MSGMAP(FrForm, FrCmdTarget)
ON_FRESH_VV("ok", FRCMD_LBUTTONUP, FrForm::OnFreshOkay)
ON_FRESH_VV("cancel", FRCMD_LBUTTONUP, FrForm::OnFreshCancel)
ON_FRESH_VV("yes", FRCMD_LBUTTONUP, FrForm::OnFreshOkay)
ON_FRESH_VV("no", FRCMD_LBUTTONUP, FrForm::OnFreshCancel)
END_FRESH_MSGMAP()

void FrForm::OnFreshCancel()
{
	OnCancel();
}

void FrForm::OnFreshOkay()
{
	OnOK();
}

FrForm::FrForm()
	: m_canDrag(true),
	  m_bCaptionDown(false),
	  m_bResizeBtnDown(false),
	  m_pBaseFrm(NULL),
	  m_pMsgStatic(NULL),
	  m_pMsgEdit(NULL),
	  m_timeLimit(-1.0f)
{
	m_flag = 0;
	m_defaultRetCode = FrNONE;
}

FrForm::~FrForm()
{
}

void FrForm::Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent)
{
	m_pFnResult = NULL;

	FrElementDoc* pDoc = pManager->GetDocument();
	FrElementForm* pElement = pDoc->GetForm(item.m_resource);
	if (pElement == NULL)
		return;

	std::map<std::string, std::string>& param = item.m_param;

	if (param.find("drag") != param.end())
	{
		int tf;
		sscanf(param["drag"].c_str(), "%d", &tf);
		EnableDrag(tf != 0);
	}

	if (param.find("modal") != param.end() ||
		(param.find("modal") != param.end() && param["modal"] == "false"))
		m_flag |= FrMODALESS;

	WRect rect((float)item.m_rect.left, (float)item.m_rect.top,
		(float)pElement->m_bgSize.w, (float)pElement->m_bgSize.h);
	m_pCmdDest = m_pOwner;
	Create("Form", item.m_name.c_str(), pManager, 0x21, rect, pParent);

	FrWnd* pWnd =
		pManager->DoCreate(pElement->m_bgFrame, pManager, this, m_pOwner);
	m_pBaseFrm = DYNAMIC_CAST(FrFrame, pWnd);
	SetCaption(pElement->m_caption.c_str());
	SetDesc(pElement->m_desc.c_str());

	std::list<FrGuiItem*>::iterator it;
	for (it = pElement->m_guiList.begin(); it != pElement->m_guiList.end();
		++it)
	{
		FrWnd* pChild = pManager->DoCreate(**it, pManager, this, m_pOwner);
		switch ((*it)->m_type)
		{
		case GI_STATIC:
		case GI_EDIT:
			if ((*it)->m_name == "message")
				SetMessageControlByType((*it)->m_type, pChild);
			break;
		}
	}

	SetViewFocus(false);
}

void FrForm::SetMessageControlByType(enumGuiType type, FrWnd* pChild)
{
	if (type == GI_STATIC)
	{
		m_pMsgStatic = DYNAMIC_CAST(FrStatic, pChild);
	}
	else if (type == GI_EDIT)
	{
		m_pMsgEdit = DYNAMIC_CAST(FrEdit, pChild);
	}
}

FrForm* FrForm::_Init(FrWndManager* pManager, FrCmdTarget* pCmdDest,
	const char* lpszTemplateID, FrWnd* pParent)
{
	m_pFnResult = NULL;

	FrElementDoc* pDoc = pManager->GetDocument();
	FrElementForm* pForm = pDoc->GetForm(lpszTemplateID);
	if (pForm == NULL)
		return NULL;

	pManager->SetKeyFocus(NULL, true);

	WSize s((float)pForm->m_bgSize.w, (float)pForm->m_bgSize.h);
	WPoint pos = pManager->GetCreatePosition(s);
	WRect rect(pos, s);

	SetOwner(this);
	m_pCmdDest = pCmdDest;

	if (pParent)
		Create("Form", lpszTemplateID, pManager, 0x21, rect, pParent);
	else
		Create("Form", lpszTemplateID, pManager, 0x21, rect,
			(FrWnd*)pManager->GetDesktop());

	FrWnd* pWnd = pManager->DoCreate(pForm->m_bgFrame, pManager, this, this);
	m_pBaseFrm = DYNAMIC_CAST(FrFrame, pWnd);
	const char* caption = pForm->m_caption.c_str();
	SetCaption(caption);
	SetDesc(pForm->m_desc.c_str());

	std::list<FrGuiItem*>::iterator it;
	for (it = pForm->m_guiList.begin(); it != pForm->m_guiList.end(); ++it)
	{
		FrWnd* pChild = pManager->DoCreate(**it, pManager, this, this);
		if ((*it)->m_name == "message")
			SetMessageControlByType((*it)->m_type, pChild);
	}

	return this;
}

bool FrForm::Open(FRESH_PFN_RESULT pFnResult, unsigned long flag)
{
	if (!WndManager()->IsValidWindow(this))
		return false;

	m_pFnResult = pFnResult;
	m_flag = flag;

	if (ms_bExclusive)
		SetVisible(false);

	SetViewFocus(!IsFixed() && !ms_bExclusive);
	OnInit();

	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_dialog_open");

	if (m_flag & FrEXCLUSIVE)
		ms_bExclusive = true;

	if (!(flag & FrMODALESS))
		SetFadeout(false);

	return true;
}

bool FrForm::Open(FRESH_PFN_RESULT pFnResult, const WPoint& pos,
	unsigned long flag)
{
	MoveWindow(pos);
	Open(pFnResult, flag);

	SetFadeout(false);

	return true;
}

bool FrForm::Close(bool bFade)
{
	return Close(m_defaultRetCode, bFade);
}

bool FrForm::Close(eFormRet result, bool bFade)
{
	WndManager()->ResetKey();

	m_retCode = result;
	if (m_pFnResult)
	{
		if (!(m_pCmdDest->*m_pFnResult)(result, this))
			return false;
	}

	if (!(m_flag & FrICON))
		SetIconRect(m_rect);

	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_dialog_close");

	if (m_flag & FrEXCLUSIVE)
		ms_bExclusive = false;

	return FrWnd::Close(ms_bHasTail ? bFade : false);
}

void FrForm::SetResultCallback(FRESH_PFN_RESULT pFnResult)
{
	m_pFnResult = pFnResult;
}

void FrForm::SetCaption(const char* caption)
{
	if (m_pBaseFrm)
		m_pBaseFrm->SetCaption(caption);
}

void FrForm::SetDesc(const char* desc)
{
	if (m_pBaseFrm)
		m_pBaseFrm->SetDesc(desc);
}

void FrForm::SetMessage(const char* msg, bool spaceAlign)
{
	if (m_pMsgStatic)
		m_pMsgStatic->SetCaption(msg);

	if (m_pMsgEdit)
	{
		m_pMsgEdit->ClearLine();
		m_pMsgEdit->AddText(msg, spaceAlign, true);
	}
}

void FrForm::EnableDrag(bool drag)
{
	m_canDrag = drag;
}

bool FrForm::OnInit()
{
	if (ms_bHasTail && !(m_flag & FrNOTAIL))
	{
		if (!(m_flag & FrICON))
			SetIconRect(m_rect);
		m_nFlags.Enable(FWF_FADING);
	}

	if (m_flag & FrRET_NONE)
		m_defaultRetCode = FrNONE;
	else if (m_flag & FrRET_OK)
		m_defaultRetCode = FrOK;
	else if (m_flag & FrRET_CANCEL)
		m_defaultRetCode = FrCANCEL;

	return true;
}

void FrForm::SetIconRect(const WRect& rect)
{
	m_iconRect.x = rect.x + rect.w * 0.5f;
	m_iconRect.y = rect.y + rect.h * 0.5f;
	m_iconRect.w = rect.w * 0.0f;
	m_iconRect.h = rect.h * 0.0f;
}

void FrForm::OnOK()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_ok_click");

	Close(FrOK, true);
}

void FrForm::OnCancel()
{
	if (!(m_flag & FrNOSOUND))
		g_audio->PlaySfx("ui_button_cancel_click");

	Close(FrCANCEL, true);
}

void FrForm::OnDraw()
{
	FrWnd::OnDraw();
}

void FrForm::OnProc(const float deltaTime)
{
	FrWnd::OnProc(deltaTime);

	if (m_timeLimit > 0.0f)
	{
		m_dt += deltaTime;
		if (m_dt > m_timeLimit)
			OnCancel();
	}
}

bool FrForm::IsTitleBarArea(const WPoint& pos)
{
	WRect topArea(m_rect.x, m_rect.y, m_rect.w, 20.0f);

	return topArea.IsInRect(pos);
}

bool FrForm::IsResizeBtnArea(const WPoint& pos)
{
	WRect btnArea(m_rect.x + m_rect.w - 16.0f, m_rect.y + m_rect.h - 16.0f,
		16.0f, 16.0f);

	return false;
}

void FrForm::OnSetCursor(bool bInClient, const WPoint& mousePos)
{
	if (!IsViewFocused())
		return;

	if (IsTitleBarArea(mousePos) && m_canDrag)
		SetCursor(1);
}

void FrForm::OnMouseMove(const WPoint& mousePos)
{
	if (GetCapture() == this)
	{
		WPoint delta = mousePos - ms_oldPos;
		delta.x = (float)(int)delta.x;
		delta.y = (float)(int)delta.y;

		if (m_canDrag && m_bCaptionDown)
		{
			WRect dr(m_rect.TopLeft() + delta, m_rect.Size());
			WndManager()->ConfineRect(dr);
			FrWnd::MoveWindow(dr.TopLeft());
		}

		if (m_canDrag && m_bResizeBtnDown)
		{
			WRect dr(m_rect.TopLeft(), m_rect.Size() + WSize(delta.x, delta.y));
			WndManager()->ConfineRect(dr);
			SetRect(dr);
		}

		ms_oldPos = mousePos;
	}
}

bool FrForm::OnLButtonDown(const WPoint& mousePos)
{
	if (IsViewFocused())
	{
		if (IsTitleBarArea(mousePos))
		{
			m_bCaptionDown = true;
			ms_oldPos = mousePos;
			SetCapture();
		}

		if (IsResizeBtnArea(mousePos))
		{
			m_bResizeBtnDown = true;
			ms_oldPos = mousePos;
			SetCapture();
		}
	}

	FindNextTopFocus(false);

	return false;
}

bool FrForm::OnLButtonUp(const WPoint& mousePos)
{
	ReleaseCapture();

	m_bCaptionDown = false;
	m_bResizeBtnDown = false;

	return false;
}

void FrForm::SetRect(const WRect& rect)
{
	FrWnd::SetRect(rect);

	if (m_pBaseFrm)
		m_pBaseFrm->SetRect(rect);
}

void FrForm::SetClientRect(const WRect& rect)
{
	FrWnd::SetClientRect(rect);

	if (m_pBaseFrm)
		m_pBaseFrm->SetClientRect(rect);
}

void FrForm::Adjust(WRect& rtRect)
{
	float fWidth = g_view->GetWidth();
	float fHeight = g_view->GetHeight();

	if ((rtRect.x + rtRect.w) > fWidth)
		rtRect.x = fWidth - rtRect.w;

	if ((rtRect.y + rtRect.h) > fHeight)
		rtRect.y = fHeight - rtRect.h;
}

void FrForm::SetFrameCaptionFocus(bool bEnable)
{
	if (m_pBaseFrm)
		m_pBaseFrm->SetCaptionFocus(bEnable);
}
