#pragma once

#include <exdispid.h>

#include "anotherhand.h"

__declspec(selectany) ATL::_ATL_FUNC_INFO BeforeNavigate2Info = {
	CC_STDCALL, VT_EMPTY, 7,
	{ VT_DISPATCH, VT_VARIANT | VT_BYREF, VT_VARIANT | VT_BYREF,
				   VT_VARIANT | VT_BYREF, VT_VARIANT | VT_BYREF, VT_VARIANT | VT_BYREF,
				   VT_BOOL | VT_BYREF }
};
__declspec(selectany) ATL::_ATL_FUNC_INFO NavigateComplete2Info = {
	CC_STDCALL, VT_EMPTY, 2, { VT_DISPATCH, VT_VARIANT | VT_BYREF }
};
__declspec(selectany) ATL::_ATL_FUNC_INFO WindowClosingInfo = {
	CC_STDCALL, VT_EMPTY, 2, { VT_BOOL, VT_BOOL | VT_BYREF }
};
__declspec(selectany) ATL::_ATL_FUNC_INFO DownloadInfo = { CC_STDCALL, VT_EMPTY,
	0, { 0 } };

class CBrowserView : public ATL::CWindowImpl<CBrowserView, ATL::CAxWindow>,
					 public WTL::CMessageFilter,
					 public WTL::CIdleHandler,
					 public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
					 public ATL::IDispEventSimpleImpl<37, CBrowserView,
						 &DIID_DWebBrowserEvents2>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, ATL::CAxWindow::GetWndClassName())

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_MSG_MAP(CBrowserView)
	MESSAGE_HANDLER(WM_DESTROY, OnDestory)
	END_MSG_MAP()

	BEGIN_COM_MAP(CBrowserView)
	END_COM_MAP()

	BEGIN_SINK_MAP(CBrowserView)

	SINK_ENTRY_INFO(37, DIID_DWebBrowserEvents2, DISPID_WINDOWCLOSING,
		OnWindowClosing, &WindowClosingInfo)

	SINK_ENTRY_INFO(37, DIID_DWebBrowserEvents2, DISPID_DOWNLOADBEGIN,
		OnDownloadBegin, &DownloadInfo)
	SINK_ENTRY_INFO(37, DIID_DWebBrowserEvents2, DISPID_DOWNLOADCOMPLETE,
		OnDownloadComplete, &DownloadInfo)
	END_SINK_MAP()

	LRESULT OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void __stdcall OnWindowClosing(VARIANT_BOOL bIsChildWindow,
		VARIANT_BOOL* bCancel);
	void __stdcall OnDownloadBegin();
	void __stdcall OnDownloadComplete();

	int m_downloadCount;
};

template <class Base>
class CComObjectStack2 : public ATL::CComObjectStack<Base>
{
public:
	CComObjectStack2() { }
	virtual ~CComObjectStack2() { }

	virtual ULONG __stdcall AddRef() { return 1; }
	virtual ULONG __stdcall Release() { return 1; }
	virtual HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject)
	{
		return _InternalQueryInterface(iid, ppvObject);
	}
};

class CBrowser : public WSingleton<CBrowser>, public CHandOwner
{
public:
	enum eWebPage
	{
		WEBPAGE_NORMAL,
		WEBPAGE_SELFCERTIFY,
	};

	CBrowser();
	virtual ~CBrowser();

	void Init(HINSTANCE hInstance, HWND hWndParent, const RECT& rect);
	void Open(const char* url, bool bResetScreenSize, bool bWindowed);
	void OpenPost(const char* url, bool bResetScreenSize, bool bWindowed);
	void Close();
	void Process(float elapsed);
	void Display();
	void Refresh();
	BOOL PreTranslateMessage(MSG* pMsg);
	void DrawCloseButton(tagDRAWITEMSTRUCT* pDis);
	void DrawBgButton(tagDRAWITEMSTRUCT* pDis);

	void InitializePostArgument();
	void AddPostArgument(const char* key, const char* value);

	void SetPagetype(eWebPage type) { m_pageType = type; }

protected:
	virtual void PrepareOpen(const char* url, const char* bgImage,
		bool bResetScreenSize, bool bWindowed);
	virtual std::string GetPostArgument() const;
	virtual void OnAnotherHand(int key, void* param);

	eWebPage m_pageType;
	HINSTANCE m_hInstance;
	HWND m_hWndParent;
	RECT m_rect;
	bool m_bOpen;
	DWORD m_dwOpenTime;
	int m_reserved50;
	int m_count;
	std::string m_url;
	std::string m_waitMsg;
	int m_oldWindowMode;
	int m_oldFillMode;
	char m_reserved98[12];
	bool m_bWindowed;
	bool m_bResetScreenSize;

public:
	bool IsOpen() { return m_spBrowser != NULL; }

	void ResetCount() { m_count = 0; }

protected:
	HWND m_hWndBrowser;
	CComObjectStack2<CBrowserView> m_view;

public:
	void SetWaitMsg(const char* msg) { m_waitMsg = msg; }

protected:
	ATL::CComQIPtr<IWebBrowser2> m_spBrowser;
	DWORD m_dwStyle;
	std::list<std::pair<std::string, std::string> > m_postArgument;
};
