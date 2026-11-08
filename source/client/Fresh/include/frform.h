#pragma once
#include "rtti.h"
#include "frcmdtarget.h"
#include "frelement.h"
#include "frwnd.h"
#include "frframe.h"
#include "../../Wangreal/include/wtypes.h"

enum eFormRet
{
	FrNONE,
	FrOK,
	FrCANCEL,
	FrYES,
	FrNO
};

class FrWndManager;

template <class T>
T* CreateForm(FrWndManager* pManager, FrCmdTarget* pCmdDest,
	const char* lpszTemplateID, FrWnd* pParent = NULL)
{
	T* pForm = new T;

	FrForm* pRet = pForm->_Init(pManager, pCmdDest, lpszTemplateID, pParent);
	if (pRet)
		return DYNAMIC_CAST(T, pRet);

	delete pForm;
	return NULL;
}

class FrEdit;
class FrFrame;
class FrGuiItem;
class FrStatic;

enum eFormFlags
{
	FrESCAPE = 0x1,
	FrENTER = 0x2,
	FrMODALESS = 0x4,
	FrNOTAIL = 0x8,
	FrICON = 0x10,
	FrRET_NONE = 0x20,
	FrRET_OK = 0x40,
	FrRET_CANCEL = 0x80,
	FrNOSOUND = 0x100,
	FrEXCLUSIVE = 0x200,
	FrRESIZABLE = 0x400
};

class FrForm : public FrWnd
{
	DECLARE_OBJECT(FrForm)
	friend class FrWndManager;

public:
	FrForm();
	virtual ~FrForm();
	void Init(FrGuiItem& item, FrWndManager* pManager, FrWnd* pParent);
	FrForm* _Init(FrWndManager* pManager, FrCmdTarget* pCmdDest,
		const char* lpszTemplateID, FrWnd* pParent);
	virtual bool Open(FRESH_PFN_RESULT pFnResult, unsigned long flag);
	virtual bool Open(FRESH_PFN_RESULT pFnResult, const WPoint& pos,
		unsigned long flag);
	virtual bool Close(bool bFade);
	virtual bool Close(eFormRet result, bool bFade);
	void SetResultCallback(FRESH_PFN_RESULT pFnResult);
	virtual void SetCaption(const char* caption);
	virtual void SetDesc(const char* desc);
	virtual void SetMessage(const char* msg, bool spaceAlign);
	virtual void EnableDrag(bool drag);
	virtual void SetTimeLimit(float limit)
	{
		m_timeLimit = limit;
		m_dt = 0.0f;
	}
	void DescHidePrivacy() { m_pBaseFrm->HidePrivacy(true); }
	bool GetFlag(unsigned long flag) { return (m_flag & flag) ? true : false; }
	void SetFlag(unsigned long flag) { m_flag = flag; }
	void EnableFlag(unsigned long flag) { m_flag |= flag; }
	void DisableFlag(unsigned long flag) { m_flag &= ~flag; }
	void SetCloseResult(eFormRet ret) { m_defaultRetCode = ret; }
	void Adjust(WRect& rtRect);
	void SetFrameCaptionFocus(bool bEnable);
	void SetCaptionOffset(bool offset) { m_pBaseFrm->SetCaptionOffset(offset); }
	void SetMessageControlByType(enumGuiType type, FrWnd* pChild);
	virtual bool OnInit();

protected:
	virtual void OnDraw();
	virtual void OnProc(const float deltaTime);
	virtual void OnOK();
	virtual void OnCancel();

public:
	virtual void OnMouseMove(const WPoint& mousePos);
	virtual bool OnLButtonDown(const WPoint& mousePos);
	virtual bool OnLButtonUp(const WPoint& mousePos);
	virtual void OnSetCursor(bool bInClient, const WPoint& mousePos);
	virtual void SetRect(const WRect& rect);
	virtual void SetIconRect(const WRect& rect);
	virtual void SetClientRect(const WRect& rect);

protected:
	bool m_canDrag;
	bool m_bCaptionDown;
	bool m_bResizeBtnDown;
	unsigned long m_flag;
	eFormRet m_retCode;
	FRESH_PFN_RESULT m_pFnResult;
	FrCmdTarget* m_pCmdDest;
	FrFrame* m_pBaseFrm;
	FrStatic* m_pMsgStatic;
	FrEdit* m_pMsgEdit;
	float m_timeLimit;
	float m_dt;
	eFormRet m_defaultRetCode;

private:
	bool IsTitleBarArea(const WPoint& pos);
	virtual bool IsResizeBtnArea(const WPoint& pos);
	static WPoint ms_oldPos;

protected:
	void OnFreshOkay();
	void OnFreshCancel();
	void OnFreshYes();
	void OnFreshNo();

	DECLARE_FRESH_MSGMAP()

public:
	static bool ms_bHasTail;
	static bool ms_bExclusive;
	static void EnableTail(bool enable) { ms_bHasTail = enable; }
};
