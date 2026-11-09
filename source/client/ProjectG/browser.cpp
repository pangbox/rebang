#include "browser.h"
#include "clientsetting.h"
#include "projectg.h"
#include "../../shared/localize.h"
#include "mathconsts.h"

ILFILLB1

extern HWND g_hwnd;
extern WInputDev* g_mouse;

/*
 *
 * It was really, really hard to make the IL balance line up for this file.
 * It turns out that the line numbers have a subtle impact on the IL offsets.
 * So this comment is, uhh, "load-bearing", to borrow a word from our AI overlords.
 *
 */

BOOL CBrowserView::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	return (BOOL)SendMessage(WM_FORWARDMSG, 0, (LPARAM)pMsg);
}

BOOL CBrowserView::OnIdle()
{
	return FALSE;
}

LRESULT CBrowserView::OnDestory(UINT uMsg, WPARAM wParam, LPARAM lParam,
	BOOL& bHandled)
{
	if (CBrowser::Instance()->IsOpen())
	{
		CBrowser::Instance()->Close();
	}

	return 1;
}

void __stdcall CBrowserView::OnWindowClosing(VARIANT_BOOL bIsChildWindow,
	VARIANT_BOOL* bCancel)
{
	USES_CONVERSION;

	*bCancel = VARIANT_TRUE;
	if (CBrowser::Instance()->IsOpen())
	{
		CBrowser::Instance()->Close();
	}
}

void __stdcall CBrowserView::OnDownloadBegin()
{
	m_downloadCount++;
}

void __stdcall CBrowserView::OnDownloadComplete()
{
	if (m_downloadCount > 0)
		m_downloadCount--;
}

CBrowser::CBrowser()
{
	m_bOpen = false;
	m_dwOpenTime = 0xffffffff;
	m_reserved50 = 0;
	m_count = 0;
	m_hInstance = NULL;
	m_hWndParent = NULL;

	m_hWndBrowser = NULL;

	m_dwStyle = 0;

	sOption opt = *COption::Instance()->GetOption();

	m_oldFillMode = opt.vFillMode;
	m_oldWindowMode = opt.vWindowMode;

	m_pageType = WEBPAGE_NORMAL;
}

CBrowser::~CBrowser()
{
	if (m_spBrowser)
	{
		m_view.DispEventUnadvise(m_spBrowser);

		m_spBrowser.Release();
		if (IsOpen() || m_bOpen == true)
		{
			m_view.DestroyWindow();
		}
	}
}

void CBrowser::Init(HINSTANCE hInstance, HWND hWndParent, const RECT& rect)
{
	m_hInstance = hInstance;
	m_hWndParent = hWndParent;
	m_rect = rect;
}

void CBrowser::Close()
{
	m_dwStyle = 0;
	if (m_spBrowser)
	{
		CComVariant vEmpty;
		m_spBrowser->Navigate(
			CComBSTR("http://211.43.194.137:3129/PANGYA/logout/GMlogout.asp"),
			&vEmpty, &vEmpty, &vEmpty, &vEmpty);
		sOption opt = *COption::Instance()->GetOption();

		if (m_bResetScreenSize)
		{
			COption::Instance()->vApplyGameScreenSize();
		}

		if (m_bWindowed &&
			(opt.vWindowMode != m_oldWindowMode ||
				opt.vFillMode != m_oldFillMode))
		{
			opt.vWindowMode = m_oldWindowMode;
			opt.vFillMode = m_oldFillMode;
			COption::Instance()->ApplyChange(opt, m_bResetScreenSize);
		}

		g_input->SetActive(true);

		g_mouse->InitDevice(m_hWndParent, true);

		m_view.DispEventUnadvise(m_spBrowser);

		m_spBrowser.Release();
		if (m_bOpen == true)
		{
			m_view.DestroyWindow();
		}

		m_hWndBrowser = NULL;
		m_bOpen = false;
		m_dwOpenTime = 0xffffffff;

		SetFocus(m_hWndParent);
	}

	g_audio->SetBGMVolumeInBGMArea(1.0f);

	SetCurrentDirectory(g_executeDirectory);

	if (m_pageType == WEBPAGE_NORMAL)
	{
		WSendPacket packet((enumClientPacket)0x3d);
		packet.Send(TO_GAME);

		if (IsLocalContent(S3_SCRATCH))
		{
			WSendPacket packet((enumClientPacket)0x72);
			packet.Send(TO_GAME);
		}

		if (IsLocalContent((localContentType_t)0x15))
		{
			WSendPacket packet((enumClientPacket)0x89);
			packet.Send(TO_GAME);
		}
	}

	m_pageType = WEBPAGE_NORMAL;
}

void CBrowser::Refresh()
{
}

void CBrowser::OnAnotherHand(int key, void* param)
{
	int count = 0;
	while (true)
	{
		WVideoDev* video = g_view->GetVideoDevice();

		if (video && (video->GetRenderCount() & 1) == 0)
		{
			if (COption::Instance()->vIsWindowed() || m_count == 0)
			{
				m_count++;
				InvalidateRect(NULL, NULL, FALSE);
			}
			return;
		}

		Sleep(1);
		if (++count >= 1000)
			return;
	}
}

BOOL CBrowser::PreTranslateMessage(MSG* pMsg)
{
	if (!m_bOpen)
		return FALSE;

	return m_view.PreTranslateMessage(pMsg);
}

void CBrowser::Process(float elapsed)
{
	if (!m_bOpen)
		return;

	if (m_dwOpenTime >= 0xffffffff)
		return;

	WVideoDev* video = g_view->GetVideoDevice();

	if (!video || (video->GetRenderCount() & 1) ||
		GetTickCount() - m_dwOpenTime <= 200)
		return;
	m_dwOpenTime = 0xffffffff;

	if (m_spBrowser == NULL)
	{
		int w = (int)g_view->GetWidth();
		int h = (int)g_view->GetHeight();
		RECT rc = { 0, 0, w, h };

		m_hWndBrowser = m_view.Create(m_hWndParent, rc, "about:blank",
			m_dwStyle | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS |
				WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU);
		m_view.SetWindowText("\xc6\xce\xbe\xdf");

		RECT rcView;
		m_view.GetWindowRect(&rcView);
		RECT rcMain;
		GetWindowRect(g_hwnd, &rcMain);

		HWND hWndInsertAfter = HWND_TOP;
		UINT flags = 0;
		int width = 0;
		int height = 0;

		switch (m_pageType)
		{
		case WEBPAGE_NORMAL:
			width = 500;
			height = 600;
			hWndInsertAfter = HWND_TOP;
			break;
		case WEBPAGE_SELFCERTIFY:
			width = 800;
			height = 600;
			hWndInsertAfter = HWND_NOTOPMOST;
			flags = SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED;
			break;
		}

		rcView.left = rcMain.left + (rcMain.right - rcMain.left) / 2 -
			(width - rcView.left) / 2;
		rcView.top = rcMain.top + (rcMain.bottom - rcMain.top) / 2 -
			(height - rcView.top) / 2;
		rcView.right = rcView.left + width;
		rcView.bottom = rcView.top + height;

		m_view.SetWindowPos(hWndInsertAfter, &rcView, flags);

		m_view.m_downloadCount = 0;
		m_view.QueryControl(&m_spBrowser);
		m_view.DispEventAdvise(m_spBrowser);

		CComVariant vEmpty;

		if (!m_postArgument.empty())
		{
			VARIANT vHeaders = { 0 };
			VARIANT vPostData = { 0 };
			CString strHeaders(
				"Content-Type: application/x-www-form-urlencoded\r\n");
			vHeaders.vt = VT_BSTR;
			vHeaders.bstrVal = strHeaders.AllocSysString();
			std::string postData = GetPostArgument();
			SAFEARRAY* psa =
				SafeArrayCreateVector(VT_UI1, 0, postData.length());
			if (psa)
			{
				void* pData;
				SafeArrayAccessData(psa, &pData);
				memcpy(pData, postData.c_str(), postData.length());
				SafeArrayUnaccessData(psa);

				vPostData.vt = VT_ARRAY | VT_UI1;
				vPostData.parray = psa;
			}

			m_spBrowser->Navigate(CComBSTR(m_url.c_str()), &vEmpty, &vEmpty,
				&vPostData, &vHeaders);
			SafeArrayDestroy(psa);
			SysFreeString(vHeaders.bstrVal);
		}
		else
		{
			m_spBrowser->Navigate(CComBSTR(m_url.c_str()), &vEmpty, &vEmpty,
				&vEmpty, &vEmpty);
		}

		g_input->SetActive(false);
		g_mouse->InitDevice(m_hWndParent, false);
	}
}

void CBrowser::Display()
{
	if (m_bOpen && m_dwOpenTime < 0xffffffff)
	{
		WRect rect(0, 0, g_view->GetWidth(), g_view->GetHeight());

		WOverlay::DrawBox(g_view, rect, 0, 0x88404040, 0.001f);
	}
}

void CBrowser::DrawCloseButton(tagDRAWITEMSTRUCT* pDis)
{
}

void CBrowser::DrawBgButton(tagDRAWITEMSTRUCT* pDis)
{
}

void CBrowser::PrepareOpen(const char* url, const char* bgImage,
	bool bResetScreenSize, bool bWindowed)
{
	m_bWindowed = bWindowed;
	m_bResetScreenSize = bResetScreenSize;
	if (m_bOpen)
		return;

	sOption opt = *COption::Instance()->GetOption();
	m_oldFillMode = opt.vFillMode;
	m_oldWindowMode = opt.vWindowMode;
	if (m_bWindowed)
	{
		if (!opt.vWindowMode || opt.vFillMode)
		{
			opt.vWindowMode = 1;
			opt.vFillMode = 0;
			COption::Instance()->ApplyChange(opt, false);
		}
	}

	m_bOpen = true;
	m_url = url;
	m_dwOpenTime = GetTickCount();
	SetWaitMsg("\xb7\xce\xb5\xf9 \xc1\xdf\xc0\xd4\xb4\xcf\xb4\xd9...");
}

void CBrowser::Open(const char* url, bool bResetScreenSize, bool bWindowed)
{
	InitializePostArgument();
	PrepareOpen(url, "[text_pangya00.jpg", bResetScreenSize, bWindowed);
}

void CBrowser::OpenPost(const char* url, bool bResetScreenSize, bool bWindowed)
{
	PrepareOpen(url, "[text_pangya00.jpg", bResetScreenSize, bWindowed);
}

void CBrowser::InitializePostArgument()
{
	m_postArgument.clear();
}

void CBrowser::AddPostArgument(const char* key, const char* value)
{
	m_postArgument.push_back(std::pair<std::string, std::string>(
		std::string(key), std::string(value)));
}

std::string CBrowser::GetPostArgument() const
{
	std::string result;
	std::list<std::pair<std::string, std::string> >::const_iterator it =
		m_postArgument.begin();
	while (it != m_postArgument.end())
	{
		result += (*it).first;
		result += "=";
		result += (*it).second;

		if (++it != m_postArgument.end())
			result += "&";
	}

	return result;
}
