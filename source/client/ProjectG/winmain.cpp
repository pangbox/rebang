#include "projectg.h"
#include "clientsetting.h"
#include "mousecursor.h"
#include "splash.h"
#include "ppl.h"
#include "browser.h"
#include "ggc_wrapper.h"
#include "../../shared/lfh.h"
#include "../../shared/localize.h"
#include "../../shared/tikimagicboxtable.h"
#include <process.h>
#include "wlocalize.h"

#include "logininfo.h"

ILFILLW1

extern HWND g_hwnd;

static bool IsZero(float f)
{
	return f < g_EPSILON;
}

#define LOAD_DIB_BITMAP(id, hdcDib, hOldDib, pBits, pitch) \
	{ \
		pBits = NULL; \
		bitmaps.hBitmap = LoadBitmap(bitmaps.hInst, MAKEINTRESOURCE(id)); \
		BITMAP bm; \
		GetObject(bitmaps.hBitmap, sizeof(BITMAP), &bm); \
		if (!GetObject(bitmaps.hBitmap, sizeof(BITMAP), &bm)) \
			DeleteObject(bitmaps.hBitmap); \
		else \
		{ \
			BYTE biBuffer[sizeof(BITMAPINFO)]; \
			BITMAPINFO& bi = *(BITMAPINFO*)biBuffer; \
			memset(&bi, 0, sizeof(bi)); \
			bi.bmiHeader.biWidth = (WORD)bm.bmWidth; \
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); \
			bi.bmiHeader.biHeight = (WORD)bm.bmHeight; \
			bi.bmiHeader.biPlanes = 1; \
			bi.bmiHeader.biBitCount = 24; \
			bi.bmiHeader.biCompression = BI_RGB; \
			bi.bmiHeader.biClrUsed = 0; \
			bi.bmiHeader.biClrImportant = 0; \
			bi.bmiHeader.biSizeImage = 0; \
			HDC hdcScreen = GetDC(NULL); \
			bitmaps.hDib = CreateDIBSection(hdcScreen, &bi, DIB_RGB_COLORS, \
				&pBits, NULL, 0); \
			ReleaseDC(NULL, hdcScreen); \
			DIBSECTION ds; \
			GetObject(bitmaps.hDib, sizeof(DIBSECTION), &ds); \
			pitch = ds.dsBm.bmWidthBytes; \
			if (ds.dsBm.bmBitsPixel != 24) \
				MessageBox(NULL, "Use 24bit bitmap!!!", "alert!", MB_OK); \
			HDC hdcBitmap = CreateCompatibleDC(NULL); \
			hdcDib = CreateCompatibleDC(NULL); \
			HGDIOBJ hOldBitmap = SelectObject(hdcBitmap, bitmaps.hBitmap); \
			hOldDib = SelectObject(hdcDib, bitmaps.hDib); \
			BitBlt(hdcDib, 0, 0, bm.bmWidth, bm.bmHeight, hdcBitmap, 0, 0, \
				SRCCOPY); \
			DeleteObject(SelectObject(hdcBitmap, hOldBitmap)); \
			DeleteDC(hdcBitmap); \
			DeleteObject(bitmaps.hBitmap); \
		} \
	}

#define CLEAR_RECT(hdc, x, y, w, h) \
	{ \
		RECT rcClear; \
		rcClear.left = (x); \
		rcClear.right = (x) + (w); \
		rcClear.top = (y); \
		rcClear.bottom = (y) + (h); \
		FillRect(hdc, &rcClear, (HBRUSH)GetStockObject(BLACK_BRUSH)); \
	}

#define LOAD_BITMAP_DC(id, hdcBitmap, hOldBitmap) \
	{ \
		HBITMAP hBitmap = LoadBitmap(bitmaps.hInst, (LPCSTR)(id)); \
		hdcBitmap = CreateCompatibleDC(dc.hdc); \
		hOldBitmap = SelectObject(hdcBitmap, hBitmap); \
	}

WTL::CAppModule _Module;

unsigned long g_iLoadingThreadID;
HANDLE g_hDrawLoading;
char g_executeFilePath[MAX_PATH];
bool g_enable_p4;
HINSTANCE g_instance;
char class_name[128];
char window_name[128];
char computer_name[128];
HHOOK hhook;

bool g_showTime = true;
bool g_processalway = true;
bool g_checkIntegrity = true;

struct sVector
{
	float x, y, z;
};

struct sLoadingBall
{
	sVector vel;
	sVector delta;
	sVector pos;
	int x;
	int y;
	bool bBounce;
	bool bStop;
};

unsigned int __stdcall DrawLoadingByThread(void* pArg);
HANDLE WINAPI GetPangYaMutex();
void ShowSplash(bool bShow);
void HideSplash();
void ShowLoading(bool bShow);
void ShowPPL();
BOOL GetLastWriteTime(HANDLE hFile, char* lpszString);
void SetWindowTitle(HWND hWnd);
void SetDirectory();
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool CheckSSE2_N_OSSupport();
bool CheckPentium4();
bool CheckWndVersion();
bool CheckRelatedDLL(HWND hWnd);

static void ClearRect(HDC hdc, int x, int y, int w, int h)
{
	RECT rc;
	rc.left = x;
	rc.right = x + w;
	rc.top = y;
	rc.bottom = y + h;
	FillRect(hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

static void DrawSprite(BYTE* pDst, int x, int y, int w, int h, int dstPitch,
	int dstHeight, BYTE* pSrc, int sx, int sy, int sw, int sh, int srcPitch)
{
	if (pDst == NULL || pSrc == NULL)
		return;
	if (x < 0 || x + w >= 800)
		return;
	if (y < 0 || y + h >= 600)
		return;

	int dstOffset = (dstHeight - y - sh) * dstPitch + x * 3;
	int srcOffset = sy * srcPitch + sx * 3;
	int d = dstOffset;
	int s = srcOffset;
	int ex = min(sx + sw, dstPitch / 3);
	int ey = min(sy + sh, h * dstPitch / 3);

	for (DWORD j = sy; j < (DWORD)ey; j++)
	{
		for (DWORD i = sx; i < (DWORD)ex; i++)
		{
			if (pSrc[s] || pSrc[s + 1] || pSrc[s + 2])
			{
				memcpy(&pDst[d], &pSrc[s], 3);
			}
			d += 3;
			s += 3;
		}
		d = dstOffset + (j + 1) * dstPitch;
		s = srcOffset + (j + 1) * srcPitch;
	}
}

static void DrawPercent(HDC hdc, int x, int y, HDC hdcNumber, float ratio)
{
	int percent = (int)(100.0f - ratio * 100.0f);
	int hundreds = percent / 100;
	int tens = (percent - hundreds * 100) / 10;
	int ones = percent - hundreds * 100 - tens * 10;

	if (hundreds)
	{
		BitBlt(hdc, x, y, 7, 12, hdcNumber, 7 * hundreds, 0, SRCCOPY);
		BitBlt(hdc, x + 7, y, 7, 12, hdcNumber, 7 * tens, 0, SRCCOPY);
		BitBlt(hdc, x + 14, y, 7, 12, hdcNumber, 7 * ones, 0, SRCCOPY);
	}
	else if (tens)
	{
		ClearRect(hdc, x, y, 7, 12);
		BitBlt(hdc, x + 7, y, 7, 12, hdcNumber, 7 * tens, 0, SRCCOPY);
		BitBlt(hdc, x + 14, y, 7, 12, hdcNumber, 7 * ones, 0, SRCCOPY);
	}
	else
	{
		if (ones)
		{
			ClearRect(hdc, x, y, 14, 12);
			BitBlt(hdc, x + 14, y, 7, 12, hdcNumber, 7 * ones, 0, SRCCOPY);
		}
		else
		{
			ClearRect(hdc, x, y, 21, 12);
			BitBlt(hdc, x + 14, y, 7, 12, hdcNumber, 0, 0, SRCCOPY);
		}
	}

	BitBlt(hdc, x + 21, y, 7, 12, hdcNumber, 70, 0, SRCCOPY);
	BitBlt(hdc, x + 28, y, 14, 12, hdcNumber, 77, 0, SRCCOPY);
}

static void InitBalls(sLoadingBall* balls)
{
	balls[0].vel.x = 70.0f;
	balls[0].vel.y = 200.0f;
	balls[0].vel.z = 30.0f;
	balls[1].vel.x = -50.0f;
	balls[1].vel.y = 190.0f;
	balls[1].vel.z = 40.0f;
	balls[2].vel.x = 10.0f;
	balls[2].vel.y = 260.0f;
	balls[2].vel.z = 70.0f;
	balls[3].vel.x = 90.0f;
	balls[3].vel.y = 130.0f;
	balls[3].vel.z = -60.0f;
	balls[4].vel.x = -80.0f;
	balls[4].vel.y = 280.0f;
	balls[4].vel.z = -80.0f;

	for (int i = 0; i < 5; i++)
	{
		balls[i].pos.x = 0.0f;
		balls[i].pos.y = -0.001f;
		balls[i].pos.z = 0.0f;
		balls[i].x = 0;
		balls[i].y = 0;
		balls[i].bBounce = false;
		balls[i].bStop = false;
	}
}

static void UpdateBalls(sLoadingBall* balls, float dt)
{
	if (0.13f < dt)
		dt = 0.13f;

	for (int i = 0; i < 5; i++)
	{
		balls[i].delta.x = balls[i].vel.x * dt;
		balls[i].delta.y = balls[i].vel.y * dt;
		balls[i].delta.z = balls[i].vel.z * dt;
		balls[i].pos.x += balls[i].delta.x * dt;
		balls[i].pos.y += balls[i].delta.y * dt;
		balls[i].pos.z += balls[i].delta.z * dt;
		if (!balls[i].bStop)
		{
			balls[i].vel.x -= balls[i].delta.x * 0.1f;
			balls[i].vel.y -= 9.8f;
			balls[i].vel.z -= balls[i].delta.y * 0.1f;
		}

		if (balls[i].pos.y <= 0.0f && balls[i].bBounce)
		{
			balls[i].pos.y = 0.001f;
			balls[i].vel.y *= -0.4f;

			if (fabs(balls[i].vel.y) < 30.0f)
			{
				balls[i].vel.x = 0.0f;
				balls[i].vel.y = 0.0f;
				balls[i].vel.z = 0.0f;
				balls[i].bStop = true;
			}
		}
		else
		{
			balls[i].bBounce = true;
		}
	}
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_SNAPSHOT)
		return 0;

	return CallNextHookEx(hhook, nCode, wParam, lParam);
}

static void RunProjectG(HWND hWnd)
{
	new CProjectG(hWnd);
	new CTikiMagicBoxDoc;

	if (CProjectG::Instance()->Init())
	{
		ShowLoading(true);
		HideSplash();

		if (CProjectG::Instance()->Ready())
		{
			COption::Instance()->vApplyLobbyScreenSize();
			CMouseCursor::Instance()->SetMode(0);

			g_input->SetMousePointToLoginBox();

			new CBrowser;
			RECT rc;
			GetClientRect(hWnd, &rc);
			CBrowser::Instance()->Init(g_instance, hWnd, rc);

			WaitForSingleObject(g_hDrawLoading, 90000);

			ShowSplash(false);

			ShowLoading(false);
			_GGC_SetHwnd(g_hwnd);

			MSG msg;
			int ret = -1;

			while (ret <= 0)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
				{
					if (!GetMessage(&msg, NULL, 0, 0))
					{
						_GGC_End();
						break;
					}

					BOOL bRet = FALSE;
					if (CBrowser::IsInstantiated())
						bRet = CBrowser::Instance()->PreTranslateMessage(&msg);

					if (!bRet)
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				else
				{
					ret = CProjectG::Instance()->MainLoop(0);
				}
			}

			if (CBrowser::IsInstantiated())
				delete CBrowser::Instance();
		}
	}

	ShowLoading(false);

	CLoginInfo::Instance()->Destroy();
	if (CTikiMagicBoxDoc::IsInstantiated())
		delete CTikiMagicBoxDoc::Instance();
	if (CProjectG::IsInstantiated())
		delete CProjectG::Instance();
}

HANDLE WINAPI GetPangYaMutex()
{
	HANDLE hMutex =
		CreateMutex(NULL, TRUE, "{071784A2-EE35-4e6a-92D0-6E7A4B985171}");
	if (hMutex == NULL)
	{
		ShowSplash(false);
		return NULL;
	}

	DWORD dwError = GetLastError();
	if (dwError == 182 || dwError == ERROR_ACCESS_DENIED)
	{
		HWND hWnd = FindWindow(class_name, NULL);
		if (hWnd)
		{
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
		}

		ShowSplash(false);
		return NULL;
	}

	return hMutex;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	LFHInstaller::install();

	InitLocalizeSystem();

	CoInitialize(NULL);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

	_Module.Init(NULL, hInstance);

	AtlAxWinInit();

	RECT rc = { 0, 0, 800, 600 };

	strcpy(class_name, "PangYa");

	HANDLE hMutex = GetPangYaMutex();
	if (hMutex)
	{
		int webLogin = CLoginInfo::Instance()->InitWebLogin(GetCommandLine());

		if (webLogin)
		{
			ShellExecute(NULL, NULL, "http://qa.pangya.gametree.co.kr/", NULL,
				NULL, SW_SHOW);
		}
		else
		{
			ShowSplash(true);

			g_instance = hInstance;
			WNDCLASS wc;
			wc.style = 0;
			wc.lpfnWndProc = WinProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = class_name;
			if (RegisterClass(&wc))
			{
				if (!CheckWndVersion())
				{
					MessageBox(NULL,
						K2L_Compatibility(
							"\xc0\xa9\xb5\xb5\xbf\xec\xc1\xee 98 \xc8\xa4\xc0\xba \xc0\xa9\xb5\xb5\xbf\xec\xc1\xee 2000 \xc0\xcc\xbb\xf3\xc0\xc7 \xbf\xee\xbf\xb5\xc3\xbc\xc1\xa6\xb0\xa1 \xc7\xca\xbf\xe4\xc7\xd5\xb4\xcf\xb4\xd9"),
						K2L_Compatibility("\xc6\xce\xbe\xdf"), MB_OK);
				}
				else
				{
					AdjustWindowRectEx(&rc, WS_CAPTION | WS_THICKFRAME, FALSE,
						0);
					int x =
						(GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) /
						2;
					int y =
						(GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) /
						2;
					OffsetRect(&rc, x, y);

					HWND hWnd = CreateWindowEx(0, class_name, window_name,
						WS_CAPTION | WS_THICKFRAME, x, y, rc.right - rc.left,
						rc.bottom - rc.top, NULL, NULL, hInstance, NULL);
					ShowWindow(hWnd, SW_HIDE);

					if (CheckRelatedDLL(hWnd))
					{
						g_hwnd = hWnd;
						if (_GGC_Init())
						{
							SetWindowTitle(hWnd);
							RunProjectG(hWnd);
						}
					}
				}
			}
		}
	}

	ShowSplash(false);

	_Module.Term();
	CoUninitialize();

	if (hMutex)
	{
		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	_GGC_End();

	return 0;
}

unsigned int __stdcall DrawLoadingByThread(void* pArg)
{
	// HACK: I don't think this is actually what the stack looks like, but this
	// is the least bad workaround that seems to match fully.
	struct
	{
		DWORD type;
		void* bits;
	} back;
	struct
	{
		DWORD lastTime;
		int x;
		void* pBallBits;
		void* pGaugeBits;
		DWORD waitTime;
	} anim;
	struct
	{
		union
		{
			HDC hdcLogo;
			HBITMAP hBitmap;
		};
		HINSTANCE hInst;
		union
		{
			HDC hdcNumber;
			HBITMAP hDib;
		};
	} bitmaps;
	struct
	{
		HDC hdcCopyright;
		int height;
	} screen;
	int width;
	struct
	{
		HDC hdcGauge;
		HDC hdcAni;
		HDC hdcBall;
		HDC hdc;
		HDC hdcBar;
		DWORD elapsed;
	} dc;
	struct
	{
		HGDIOBJ hOldBar;
		HGDIOBJ hOldBack;
		HGDIOBJ hOldNumber;
		DWORD fr;
		HGDIOBJ hOldLogo;
		HGDIOBJ hOldBall;
		HGDIOBJ hOldCopyright;
		HWND hWnd;
		int gaugePitch;
		HGDIOBJ hOldAni;
		HGDIOBJ hOldGauge;
		int stride;
		int ballPitch;
	} resources;
	char szPath[MAX_PATH];
	char drive[4], dir[MAX_PATH], fname[128], pathParts[28];
	char* ext = pathParts;
	GetModuleFileName(NULL, szPath, MAX_PATH);
	_splitpath(szPath, drive, dir, fname, ext);

	char szDll[128];
	char* szSuffix = pathParts + 8;
	strcpy(szDll, "LoadingRes");
	char* p = strrchr(fname, '_');
	if (p)
	{
		strcpy(szSuffix, p);
		strcat(szDll, szSuffix);
		LogOut(0, "%s\n", szSuffix);
	}
	strcat(szDll, ".dll");

	bitmaps.hInst = LoadLibrary(szDll);
	if (bitmaps.hInst)
	{
		Sleep(100);
		if (g_iLoadCurNum >= g_iLoadTotalNum)
			g_bExitDrawLoading = true;

		resources.hWnd = g_hwnd;
		dc.hdc = GetDC(resources.hWnd);
		RECT rc;
		GetClientRect(resources.hWnd, &rc);

		DWORD r = timeGetTime();

		r %= 3;
		dc.elapsed = 0;
		back.type = Min(r, (DWORD)2);

		sLoadingBall balls[5];
		InitBalls(balls);

		HDC hdcBack;

		{
			BITMAPINFO bi;
			memset(&bi, 0, sizeof(bi));
			back.bits = NULL;
			bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bi.bmiHeader.biWidth = rc.right - rc.left;
			bi.bmiHeader.biHeight = rc.bottom - rc.top;
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biBitCount = 24;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biSizeImage = 0;
			bi.bmiHeader.biClrUsed = 0;
			bi.bmiHeader.biClrImportant = 0;
			hdcBack = CreateCompatibleDC(dc.hdc);
			HBITMAP hBack = CreateDIBSection(hdcBack, &bi, DIB_RGB_COLORS,
				&back.bits, NULL, 0);
			resources.hOldBack = SelectObject(hdcBack, hBack);
		}

		LOAD_DIB_BITMAP(108, dc.hdcGauge, resources.hOldGauge, anim.pGaugeBits,
			resources.gaugePitch);

		LOAD_DIB_BITMAP(101, dc.hdcBall, resources.hOldBall, anim.pBallBits,
			resources.ballPitch);

		int id;
		if (back.type == 0)
			id = 129;
		else
			id = (back.type == 1) ? 130 : 131;

		LOAD_BITMAP_DC(id, dc.hdcAni, resources.hOldAni);
		LOAD_BITMAP_DC(MAKEINTRESOURCE(103), dc.hdcBar, resources.hOldBar);
		LOAD_BITMAP_DC(MAKEINTRESOURCE(109), bitmaps.hdcNumber,
			resources.hOldNumber);
		LOAD_BITMAP_DC(MAKEINTRESOURCE(122), bitmaps.hdcLogo,
			resources.hOldLogo);
		LOAD_BITMAP_DC(MAKEINTRESOURCE(123), screen.hdcCopyright,
			resources.hOldCopyright);

		FillRect(hdcBack, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

		BitBlt(hdcBack, 685, 3, 118, 134, bitmaps.hdcLogo, 0, 0, SRCCOPY);
		BitBlt(hdcBack, 519, 521, 275, 72, screen.hdcCopyright, 0, 0, SRCCOPY);

		anim.lastTime = timeGetTime();

		anim.waitTime = 0;
		bool bSound = false;
		width = rc.right - rc.left;
		screen.height = rc.bottom - rc.top;
		while (!g_bExitDrawLoading)
		{
			Sleep(20);

			DWORD curTime = timeGetTime();
			DWORD dt = curTime - anim.lastTime;
			anim.lastTime = curTime;
			dc.elapsed += dt;

			if (g_iLoadCurNum < g_iLoadNextNum)
			{
				g_iLoadCurNum++;
			}
			else
			{
				if (g_iLoadCurNum >= g_iLoadTotalNum)
				{
					anim.waitTime += dt;
					if (anim.waitTime > 2000)
						break;
				}
			}

			float ratio = (float)g_iLoadCurNum / g_iLoadTotalNum;

			if (back.type == 0)
			{
				DWORD frame = Min(dc.elapsed % 800 / 50, (DWORD)15);
				BitBlt(hdcBack, 356, 169, 117, 190, dc.hdcAni,
					117 * (frame % 4), 190 * (frame / 4), SRCCOPY);
			}
			else
			{
				DWORD frame = Min(dc.elapsed % 1000 / 50, (DWORD)19);
				BitBlt(hdcBack, 356, 169, 87, 182, dc.hdcAni, 87 * (frame % 5),
					182 * (frame / 5), SRCCOPY);
			}

			if (g_iLoadCurNum < g_iLoadTotalNum)
			{
				CLEAR_RECT(hdcBack, 300, 330, 220, 20);
				BitBlt(hdcBack, 500, 240, 31, 119, dc.hdcBar, 0, 10, SRCCOPY);
				DrawPercent(hdcBack, 490, 240, bitmaps.hdcNumber, ratio);

				DWORD frame = Min((1000 - dc.elapsed % 1000) / 90, (DWORD)10);

				static int s_x;
				static int s_y;
				s_x = 300 - (int)(ratio * -200.0f);
				s_y = 330;
				DrawSprite((BYTE*)back.bits, s_x, s_y, 20, 20, width * 3,
					screen.height, (BYTE*)anim.pGaugeBits, 20 * (frame % 11), 0,
					20, 20, resources.gaugePitch);
			}
			else
			{
				if (!bSound)
				{
					if (COption::Instance()->aIsMssEnabled())
					{
						PlaySound(MAKEINTRESOURCE(121), bitmaps.hInst,
							SND_RESOURCE | SND_ASYNC);
					}
					bSound = true;
				}

				CLEAR_RECT(hdcBack, 300, 330, 220, 20);

				for (int i = 0; i < 5; i++)
					CLEAR_RECT(hdcBack, balls[i].x, balls[i].y, 19, 19);

				BitBlt(hdcBack, 500, 240, 31, 119, dc.hdcBar, 31, 10, SRCCOPY);
				UpdateBalls(balls, (float)dt * 0.001f * 5.0f);
				resources.fr = dc.elapsed % 300 / 42;

				resources.stride = width * 3;
				for (int j = 0; j < 5; j++)
				{
					DWORD f = Min(resources.fr, (DWORD)6);
					anim.x = (int)balls[j].pos.x + 510;
					balls[j].x = anim.x;
					balls[j].y = 320 - (int)(balls[j].pos.z * 0.15f) -
						(int)balls[j].pos.y;
					DrawSprite((BYTE*)back.bits, balls[j].x, balls[j].y, 19, 19,
						resources.stride, screen.height, (BYTE*)anim.pBallBits,
						19 * (f % 7), 0, 19, 19, resources.ballPitch);
				}
			}

			BitBlt(dc.hdc, 0, 0, width, screen.height, hdcBack, 0, 0, SRCCOPY);
		}

		FillRect(dc.hdc, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

		DeleteObject(
			SelectObject(screen.hdcCopyright, resources.hOldCopyright));
		DeleteObject(SelectObject(bitmaps.hdcLogo, resources.hOldLogo));

		DeleteObject(SelectObject(bitmaps.hdcNumber, resources.hOldNumber));
		DeleteObject(SelectObject(dc.hdcBar, resources.hOldBar));
		DeleteObject(SelectObject(dc.hdcBall, resources.hOldBall));
		DeleteObject(SelectObject(dc.hdcGauge, resources.hOldGauge));
		DeleteObject(SelectObject(dc.hdcAni, resources.hOldAni));
		DeleteObject(SelectObject(hdcBack, resources.hOldBack));

		DeleteDC(screen.hdcCopyright);
		DeleteDC(bitmaps.hdcLogo);

		DeleteDC(bitmaps.hdcNumber);
		DeleteDC(dc.hdcBar);
		DeleteDC(dc.hdcBall);
		DeleteDC(dc.hdcGauge);
		DeleteDC(dc.hdcAni);
		DeleteDC(hdcBack);
		ReleaseDC(resources.hWnd, dc.hdc);
	}

	CloseHandle(g_hDrawLoading);
	g_hDrawLoading = NULL;
	g_iLoadingThreadID = 0;
	if (bitmaps.hInst)
		FreeLibrary(bitmaps.hInst);

	_endthreadex(0);

	return 0;
}

void ShowSplash(bool bShow)
{
	if (bShow)
	{
		new CSplash;
		CSplash::Instance()->SetBitmap("PangYaSetup.bmp");
		CSplash::Instance()->SetTransparentColor(RGB(0, 255, 0));
		CSplash::Instance()->ShowSplash();
	}
	else
	{
		if (CSplash::IsInstantiated())
		{
			CSplash::Instance()->CloseSplash();
			delete CSplash::Instance();
		}
	}
}

void HideSplash()
{
	if (CSplash::IsInstantiated())
		CSplash::Instance()->HideSplash();
}

void ShowLoading(bool bShow)
{
	if (bShow)
	{
		if (!g_hDrawLoading && !g_iLoadingThreadID)
		{
			g_hDrawLoading =
				(HANDLE)_beginthreadex(NULL, 0, DrawLoadingByThread, NULL, 0,
					(unsigned int*)&g_iLoadingThreadID);
		}
	}
	else
	{
		g_bExitDrawLoading = true;
	}
}

void ShowPPL()
{
	new CPPL;
	if (COption::IsInstantiated())
	{
		if (CPPL::Instance()->IsPPLEnable())
		{
			WinExec("PangyaPPL.exe", SW_HIDE);
		}
		if (CPPL::IsInstantiated())
			delete CPPL::Instance();
	}
}

BOOL GetLastWriteTime(HANDLE hFile, char* lpszString)
{
	FILETIME ftCreate, ftAccess, ftWrite, ftLocal;
	SYSTEMTIME stCreate;

	if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
		return FALSE;

	if (!FileTimeToLocalFileTime(&ftWrite, &ftLocal))
		return FALSE;

	FileTimeToSystemTime(&ftLocal, &stCreate);

	wsprintf(lpszString,
		"%d\xb3\xe2%02d\xbf\xf9%02d\xc0\xcf %02d\xbd\xc3%02d\xba\xd0",
		stCreate.wYear, stCreate.wMonth, stCreate.wDay, stCreate.wHour,
		stCreate.wMinute);

	return TRUE;
}

void SetWindowTitle(HWND hWnd)
{
	strcpy(window_name, K2L_Compatibility("\xc6\xce\xbe\xdf United(QA)"));
	SetWindowText(hWnd, window_name);
}

void SetDirectory()
{
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;

	case WM_USER:
		DestroyWindow(hWnd);
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SYSCOMMAND:
		if (wParam == SC_RESTORE)
			break;
		if (wParam != SC_CLOSE)
			return 0;
		break;

	case WM_SIZE:
		g_showTime = !(wParam == SIZE_MAXHIDE || wParam == SIZE_MINIMIZED);
		break;
	}

	if (CProjectG::IsInstantiated())
		return CProjectG::Instance()->WinProc(hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool CheckSSE2_N_OSSupport()
{
	return false;
}

bool CheckPentium4()
{
	if (strstr(GetCommandLine(), "-sse2"))
		return true;
	if (strstr(GetCommandLine(), "-nosse2"))
		return false;

	return false;
}

bool CheckWndVersion()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if (osvi.dwPlatformId != VER_PLATFORM_WIN32s &&
		(osvi.dwMajorVersion > 4 ||
			(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion > 0)))
		return true;
	return false;
}

bool CheckRelatedDLL(HWND hWnd)
{
	HMODULE hDll = LoadLibrary("dinput8.dll");
	if (hDll == NULL)
	{
		MessageBox(hWnd,
			K2L_Compatibility(
				"DirectX \xb0\xfc\xb7\xc3 \xc6\xc4\xc0\xcf\xc0\xbb \xc3\xa3\xc0\xbb \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.\nDirectX 9.0c \xc0\xcc\xbb\xf3\xc0\xbb \xbc\xb3\xc4\xa1 \xc8\xc4 \xb4\xd9\xbd\xc3 \xbd\xc7\xc7\xe0\xc7\xd8 \xc1\xd6\xbc\xbc\xbf\xe4."),
			K2L_Compatibility("\xc6\xce\xbe\xdf"), MB_OK);
		return false;
	}

	FreeLibrary(hDll);
	return true;
}
