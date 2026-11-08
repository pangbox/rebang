#include "wd3d8.h"
#include <cmath>
#include <algorithm>

#include <wdxtc.h>
#include <windows.h>
#include <wlist.h>
#include <wvideo.h>
#include <xmmintrin.h>
#include "wlocalize.h"

extern void AddVideoDevice(WVideoDev* video);

unsigned long WDirect3D8::ms_ZbuffHistogram[256] = { 0 };
eWindowsVersion g_winVer = WinVerNone;

static D3DTEXTURESTAGESTATETYPE g_TexStageStateList[6] = {
	D3DTSS_COLORARG1,
	D3DTSS_COLORARG2,
	D3DTSS_COLOROP,
	D3DTSS_ALPHAARG1,
	D3DTSS_ALPHAARG2,
	D3DTSS_ALPHAOP,
};

static D3DSAMPLERSTATETYPE g_TexSamplerStateList[] = { D3DSAMP_MIPFILTER };

WDirect3D8::FMTLIST WDirect3D8::ms_fmtList[5] = {
	{ D3DFMT_R5G6B5,   "R5G6B5",   0x10 },
	{ D3DFMT_X1R5G5B5, "X1R5G5B5", 0x10 },
	{ D3DFMT_A1R5G5B5, "A1R5G5B5", 0x10 },
	{ D3DFMT_X8R8G8B8, "X8R8G8B8", 0x20 },
	{ D3DFMT_A8R8G8B8, "A8R8G8B8", 0x20 },
};

static const char* DriverPath[4] = {
	"C:/WINNT/system/",
	"C:/WINNT/system32/",
	"C:/WINDOWS/system/",
	"C:/WINDOWS/system32/",
};

D3DFORMAT WDirect3D8::ms_fmtRecomList[6][4] = {
	{ D3DFMT_R5G6B5,   D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8   },
	{ D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8, D3DFMT_A8R3G3B2 },
	{ D3DFMT_A4R4G4B4, D3DFMT_R8G8B8,   D3DFMT_A8R3G3B2, D3DFMT_A1R5G5B5 },
	{ D3DFMT_R8G8B8,   D3DFMT_X8R8G8B8, D3DFMT_R5G6B5,   D3DFMT_A1R5G5B5 },
	{ D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_UNKNOWN  },
	{ D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4, D3DFMT_A1R5G5B5, D3DFMT_UNKNOWN  },
};

WDirect3D8::pix_info WDirect3D8::ms_fmtTypeList[9] = {
	{ D3DFMT_R5G6B5,   2, 3, 0x0B, 2, 5, 3, 0, 8, 0    },
	{ D3DFMT_R8G8B8,   3, 0, 0x10, 0, 8, 0, 0, 8, 0    },
	{ D3DFMT_A1R5G5B5, 2, 3, 0x0A, 3, 5, 3, 0, 7, 0x0F },
	{ D3DFMT_X1R5G5B5, 2, 3, 0x0A, 3, 5, 3, 0, 8, 0x10 },
	{ D3DFMT_R3G3B2,   1, 5, 5,    5, 2, 6, 0, 8, 0    },
	{ D3DFMT_A4R4G4B4, 2, 4, 8,    4, 4, 4, 0, 4, 0x0C },
	{ D3DFMT_A8R8G8B8, 4, 0, 0x10, 0, 8, 0, 0, 0, 0x18 },
	{ D3DFMT_A8R3G3B2, 2, 5, 5,    5, 2, 6, 0, 0, 8    },
	{ D3DFMT_X8R8G8B8, 4, 0, 0x10, 0, 8, 0, 0, 8, 0x18 },
};

WDirect3D8::sRtFormat WDirect3D8::ms_RtFmt[4] = {
	{ D3DFMT_A8R8G8B8, 4 },
	{ D3DFMT_A4R4G4B4, 2 },
	{ D3DFMT_A1R5G5B5, 2 },
	{ D3DFMT_A8R3G3B2, 2 },
};

WDirect3D8::sRtFormat WDirect3D8::ms_RtNoAlphaFmt[3] = {
	{ D3DFMT_X8R8G8B8, 4 },
	{ D3DFMT_R5G6B5,   2 },
	{ D3DFMT_X1R5G5B5, 2 },
};

WDirect3D8::sRtFormat WDirect3D8::ms_RtDepthFmt[5] = {
	{ D3DFMT_D24X8,   4 },
	{ D3DFMT_D16,     2 },
	{ D3DFMT_D24S8,   4 },
	{ D3DFMT_D24X4S4, 4 },
	{ D3DFMT_D15S1,   2 },
};

extern const char* const g_msgD3DInitFailed = K2L_Compatibility(
	"Direct3D 9\xb8\xa6 \xc3\xca\xb1\xe2\xc8\xad \xc7\xd2 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9.");

static sCpatureOption g_captureOption;

static D3DXMATRIX g_mId(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

static bool g_formatChanged = false;
static const char* g_error = 0;

eWindowsVersion GetWindowsVersion()
{
	OSVERSIONINFOEX osver;
	memset(&osver, 0, sizeof(osver));
	eWindowsVersion version = WinVerNone;
	osver.dwOSVersionInfoSize = sizeof(osver);
	if (!GetVersionEx((OSVERSIONINFO*)&osver))
	{
		osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO*)&osver))
			return WinVerNone;
	}
	switch (osver.dwPlatformId)
	{
	case VER_PLATFORM_WIN32s:
	case VER_PLATFORM_WIN32_WINDOWS:
		version = WinVerOld;
		break;
	case VER_PLATFORM_WIN32_NT:
		if (osver.dwMajorVersion <= 4)
			version = WinVerWindowsNt;
		if (osver.dwMajorVersion == 5)
		{
			if (osver.dwMinorVersion == 0)
				version = WinVerWindows2000;
			else if (osver.dwMinorVersion == 1)
				version = WinVerWindowsXp;
		}
		else if (osver.dwMajorVersion == 6)
		{
			if (osver.dwMinorVersion == 0)
				version = WinVerWindowsVista;
			else if (osver.dwMinorVersion == 1)
				version = WinVerWindows7;
		}
		break;
	}
	return version;
}

IDirect3D9* fnWD3D_Create()
{
	return Direct3DCreate9(D3D_SDK_VERSION);
}

HRESULT fnWD3DDevice_GetRenderTarget(IDirect3DDevice9* dev, DWORD rtIdx,
	IDirect3DSurface9** surf)
{
	return dev->GetRenderTarget(rtIdx, surf);
}

HRESULT fnWD3DDevice_CreateSurface(IDirect3DDevice9* dev, UINT w, UINT h,
	D3DFORMAT fmt, D3DPOOL pool, IDirect3DSurface9** surf, HANDLE* sharedHandle)
{
	return dev->CreateOffscreenPlainSurface(w, h, fmt, pool, surf,
		sharedHandle);
}

HRESULT fnWD3DDevice_CreateTexture(IDirect3DDevice9* dev, UINT width,
	UINT height, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool,
	IDirect3DTexture9** texture, HANDLE* shared)
{
	return dev->CreateTexture(width, height, levels, usage, format, pool,
		texture, shared);
}

HRESULT fnWD3DDevice_CreateVertexBuffer(IDirect3DDevice9* dev, UINT size,
	DWORD usage, DWORD fvf, D3DPOOL pool, IDirect3DVertexBuffer9** buffer,
	HANDLE* shared)
{
	return dev->CreateVertexBuffer(size, usage, fvf, pool, buffer, shared);
}

HRESULT fnWD3DDevice_CreateIndexBuffer(IDirect3DDevice9* dev, UINT size,
	DWORD usage, D3DFORMAT format, D3DPOOL pool, IDirect3DIndexBuffer9** buffer,
	HANDLE* shared)
{
	return dev->CreateIndexBuffer(size, usage, format, pool, buffer, shared);
}

HRESULT fnWD3DDevice_SetStreamSource(IDirect3DDevice9* dev, UINT strmNo,
	IDirect3DVertexBuffer9* vb, UINT offset, UINT stride)
{
	return dev->SetStreamSource(strmNo, vb, offset, stride);
}

HRESULT fnWD3DDevice_SetIndices(IDirect3DDevice9* dev,
	IDirect3DIndexBuffer9* buffer, UINT baseVertexIndex)
{
	return dev->SetIndices(buffer);
}

HRESULT fnWD3DDevice_DrawIndexedPrimitive(IDirect3DDevice9* dev,
	D3DPRIMITIVETYPE primType, INT baseVtxIdx, UINT minIdx, UINT numVtxs,
	UINT startIdx, UINT primCount)
{
	return dev->DrawIndexedPrimitive(primType, baseVtxIdx, minIdx, numVtxs,
		startIdx, primCount);
}

HRESULT fnWD3DDevice_CopyRect(IDirect3DDevice9* dev, IDirect3DSurface9* srcSurf,
	const RECT* srcRc, IDirect3DSurface9* dstSurf)
{
	return dev->StretchRect(srcSurf, srcRc, dstSurf, srcRc, D3DTEXF_NONE);
}

HRESULT fnWD3DVertexBuffer_Lock(IDirect3DVertexBuffer9* buffer,
	unsigned int offset, unsigned int size, unsigned char** data,
	unsigned long flags)
{
	return buffer->Lock(offset, size, (void**)data, flags);
}

HRESULT fnWD3DIndexBuffer_Lock(IDirect3DIndexBuffer9* buffer,
	unsigned int offset, unsigned int size, unsigned char** data,
	unsigned long flags)
{
	return buffer->Lock(offset, size, (void**)data, flags);
}

HRESULT fnWD3DX_D3DXCreateTexture(IDirect3DDevice9* dev, UINT width,
	UINT height, UINT levels, DWORD usage, D3DFORMAT format, D3DPOOL pool,
	IDirect3DTexture9** texture)
{
	return D3DXCreateTexture(dev, width, height, levels, usage, format, pool,
		texture);
}

HRESULT fnWD3DX_D3DXCreateTextureFromFileInMemoryEx(IDirect3DDevice9* dev,
	const void* data, UINT size, UINT width, UINT height, UINT levels,
	DWORD usage, D3DFORMAT format, D3DPOOL pool, DWORD filter, DWORD mipFilter,
	D3DCOLOR colorKey, D3DXIMAGE_INFO* info, PALETTEENTRY* palette,
	IDirect3DTexture9** texture)
{
	return D3DXCreateTextureFromFileInMemoryEx(dev, data, size, width, height,
		levels, usage, format, pool, filter, mipFilter, colorKey, info, palette,
		texture);
}

#include "screencape.h"

namespace nsWindowUtility
{
	class CDWMApiDll
	{
	public:
		CDWMApiDll()
		{
			this->m_hDWMApi = 0;
			this->m_fIsEnabled = 0;
			this->m_fEnable = 0;
		}
		virtual ~CDWMApiDll()
		{
			if (this->m_hDWMApi)
			{
				FreeLibrary(this->m_hDWMApi);
				this->m_hDWMApi = 0;
			}
		}

		BOOL Load_DWMAPIDll()
		{
			this->m_hDWMApi = LoadLibrary(TEXT("dwmapi.dll"));
			if (!this->m_hDWMApi)
			{
				return 0;
			}
			this->m_fIsEnabled = reinterpret_cast<IsCompositionEnabledPtr>(
				GetProcAddress(this->m_hDWMApi, "DwmIsCompositionEnabled"));
			this->m_fEnable = reinterpret_cast<EnableCompositionPtr>(
				GetProcAddress(this->m_hDWMApi, "DwmEnableComposition"));

			return TRUE;
		}
		BOOL IsCompositionEnabled()
		{
			BOOL bEnabled = 0;

			if (!this->m_fIsEnabled || FAILED(this->m_fIsEnabled(&bEnabled)) ||
				bEnabled != 1)
			{
				return FALSE;
			}

			return TRUE;
		}
		HRESULT EnableComposition(BOOL bEnable)
		{
			if (this->m_fEnable && this->m_fIsEnabled)
			{
				return this->m_fEnable(bEnable);
			}
			return E_FAIL;
		}

	private:
		HMODULE m_hDWMApi;
		IsCompositionEnabledPtr m_fIsEnabled;
		EnableCompositionPtr m_fEnable;
	};
}

inline sCpatureOption::sCpatureOption()
{
	dxCopyRectsOK = true;
	useTexture = false;
}

inline void sCpatureOption::SetMode(eCaptureMode mode, bool windowed)
{
	this->currentMode = mode;
	switch (mode)
	{
	case CM_B:
		this->FullScreen_PresentationInterval = 0x80000000;
		this->SwapEffect = D3DSWAPEFFECT_COPY;
		this->useMemCopy = false;
		this->updateWholeScreen = false;
		this->useTexture = g_winVer >= WinVerWindowsVista;
		break;
	case CM_A:
		this->FullScreen_PresentationInterval = 0;
		this->SwapEffect = D3DSWAPEFFECT_COPY;
		this->useMemCopy = true;
		this->updateWholeScreen = true;
		this->useTexture = false;
		break;
	}
}

static void SetD3DMATRIXFromWMatrix(D3DMATRIX& d3dm, const WMatrix& wm)
{
	*(WVector*)&d3dm.m[0][0] = *(const WVector*)&wm.p[0];
	*(WVector*)&d3dm.m[1][0] = *(const WVector*)&wm.p[3];
	*(WVector*)&d3dm.m[2][0] = *(const WVector*)&wm.p[6];
	*(WVector*)&d3dm.m[3][0] = *(const WVector*)&wm.p[9];
	d3dm._14 = d3dm._24 = d3dm._34 = 0.0f;
	d3dm._44 = 1.0f;
}

inline WSplashD3D::_MYVERTEX::_MYVERTEX()
{
}

inline WSplashD3D::_MYVERTEX::_MYVERTEX(const D3DVECTOR& v, float _rhw,
	unsigned long _color, float _tu, float _tv)
	: sx(v.x), sy(v.y), sz(v.z), rhw(_rhw), color(_color), tu(_tu), tv(_tv)
{
}

WSplashD3D::WSplashD3D(WDirect3D8* pDriver)
	: m_pDriver(pDriver), m_nSurfCount(0)
{
	memset(m_hTexs, 0, sizeof(m_hTexs));
}

WSplashD3D::~WSplashD3D()
{
	for (int i = 0; i < m_nSurfCount; ++i)
	{
		if (m_hTexs[i])
		{
			m_pDriver->DestroyTexture(m_hTexs[i]);
			m_hTexs[i] = 0;
		}
	}
}

int WSplashD3D::Init(tagBITMAPINFO& bi, void* data, bool fitToScreen)
{
	m_offset.x = 0;
	m_offset.y = 0;
	m_nSurfCount = 0;
	m_iSrcWidth = bi.bmiHeader.biWidth;
	m_iSrcHeight = bi.bmiHeader.biHeight;
	if (fitToScreen)
	{
		m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
		m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;
	}
	else
	{
		m_iBufWidth = m_iSrcWidth;
		m_iBufHeight = m_iSrcHeight;
	}
	int maxWidth = m_pDriver->m_d3dcaps.MaxTextureWidth;
	int maxHeight = m_pDriver->m_d3dcaps.MaxTextureHeight;
	if (m_iSrcWidth <= maxWidth && m_iSrcHeight <= maxHeight)
	{
		if (!m_hTexs[0])
		{
			m_hTexs[0] = m_pDriver->CreateTexture(&bi, 0x80000);
			if (!m_hTexs[0])
				return 0;
			m_pDriver->UpdateTexture(m_hTexs[0], &bi, data, 0x80000);
		}
		m_rects[0].left = 0;
		m_rects[0].top = 0;
		m_rects[0].right = m_iBufWidth;
		m_rects[0].bottom = m_iBufHeight;
		m_nSurfCount = 1;
		m_uvs[0].x = (float)bi.bmiHeader.biWidth /
			m_pDriver->GetTextureWidth(m_hTexs[0]);
		m_uvs[0].y = (float)bi.bmiHeader.biHeight /
			m_pDriver->GetTextureHeight(m_hTexs[0]);
	}
	else
	{
		Bitmap bitmap(maxWidth, maxHeight, bi.bmiHeader.biBitCount);
		int numX = (m_iSrcWidth + maxWidth - 1) / maxWidth;
		int numY = (m_iSrcHeight + maxHeight - 1) / maxHeight;
		int k;
		int cps = bi.bmiHeader.biBitCount >> 3;
		int srcBpl = (bi.bmiHeader.biWidth * cps + 3) & ~3;
		for (int i = 0; i < numY; ++i)
		{
			for (int j = 0; j < numX; ++j)
			{
				m_hTexs[m_nSurfCount] =
					m_pDriver->CreateTexture(bitmap.bi, 0x80000);
				if (!m_hTexs[m_nSurfCount])
					return 0;
				int widthToFill = (j + 1) * maxWidth > bi.bmiHeader.biWidth
					? bi.bmiHeader.biWidth - j * maxWidth
					: maxWidth;
				int heightToFill = (i + 1) * maxHeight > bi.bmiHeader.biHeight
					? bi.bmiHeader.biHeight - i * maxHeight
					: maxHeight;
				for (k = 0; k < heightToFill; ++k)
					memcpy(bitmap.vram + k * bitmap.pitch,
						(BYTE*)data + (i * maxHeight + k) * srcBpl +
							j * maxWidth * cps,
						widthToFill * cps);
				if (widthToFill < maxWidth)
					for (k = 0; k < heightToFill; ++k)
						memset(bitmap.vram + k * bitmap.pitch +
								widthToFill * cps,
							0, (maxWidth - widthToFill) * cps);
				if (heightToFill < maxHeight)
					for (k = heightToFill; k < maxHeight; ++k)
						memset(bitmap.vram + k * bitmap.pitch, 0,
							maxWidth * cps);
				m_pDriver->UpdateTexture(m_hTexs[m_nSurfCount], bitmap.bi,
					bitmap.vram, 0x80000);
				m_rects[m_nSurfCount].left =
					j * maxWidth * m_iBufWidth / m_iSrcWidth;
				m_rects[m_nSurfCount].right =
					(j + 1) * maxWidth * m_iBufWidth / m_iSrcWidth;
				m_rects[m_nSurfCount].top =
					i * maxHeight * m_iBufHeight / m_iSrcHeight;
				m_rects[m_nSurfCount].bottom =
					(i + 1) * maxHeight * m_iBufHeight / m_iSrcHeight;
				if (m_rects[m_nSurfCount].right > m_iBufWidth)
					m_rects[m_nSurfCount].right = m_iBufWidth;
				if (m_rects[m_nSurfCount].bottom > m_iBufHeight)
					m_rects[m_nSurfCount].bottom = m_iBufHeight;
				RECT dest = { j * maxWidth, i * maxHeight, (j + 1) * maxWidth,
					(i + 1) * maxHeight };
				if (dest.right > m_iSrcWidth)
					dest.right = m_iSrcWidth;
				if (dest.bottom > m_iSrcHeight)
					dest.bottom = m_iSrcHeight;
				RECT clip = { 0, 0, dest.right - dest.left,
					dest.bottom - dest.top };
				m_uvs[m_nSurfCount].x = (float)clip.right / maxWidth;
				m_uvs[m_nSurfCount].y = (float)clip.bottom / maxHeight;
				++m_nSurfCount;
			}
		}
	}
	for (int i = m_nSurfCount; i < 64; ++i)
	{
		if (m_hTexs[i] > 0)
		{
			m_pDriver->DestroyTexture(m_hTexs[i]);
			m_hTexs[i] = 0;
		}
	}
	return 1;
}

int WSplashD3D::InitFromScreen(bool bCopy)
{
	m_nSurfCount = 0;
	m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
	m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;
	IDirect3DSurface9* pRenderTarget;
	D3DSURFACE_DESC sd;
	if (FAILED(m_pDriver->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget)))
		return 0;
	if (FAILED(pRenderTarget->GetDesc(&sd)))
	{
		pRenderTarget->Release();
		return 0;
	}
	WDirect3D8::pix_info* fmt = 0;
	for (unsigned int i = 0; i < 9; ++i)
	{
		if (WDirect3D8::ms_fmtTypeList[i].pixFmt == sd.Format)
		{
			fmt = &WDirect3D8::ms_fmtTypeList[i];
			break;
		}
	}
	if (!fmt)
	{
		pRenderTarget->Release();
		return 0;
	}
	unsigned int maxWidth = m_pDriver->m_d3dcaps.MaxTextureWidth;
	unsigned int maxHeight = m_pDriver->m_d3dcaps.MaxTextureHeight;
	maxWidth = Min<unsigned int>(maxWidth, m_iBufWidth);
	maxHeight = Min<unsigned int>(maxHeight, m_iBufHeight);
	int numX = (m_iBufWidth + maxWidth - 1) / maxWidth;
	int numY = (m_iBufHeight + maxHeight - 1) / maxHeight;
	BITMAPINFO bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biWidth = maxWidth;
	bi.bmiHeader.biHeight = maxHeight;
	bi.bmiHeader.biBitCount = fmt->cpp * 8;
	for (int i = 0; i < numY; ++i)
	{
		for (int j = 0; j < numX; ++j)
		{
			if (!m_hTexs[m_nSurfCount])
			{
				m_hTexs[m_nSurfCount] = m_pDriver->CreateTexture(&bi, 0x80000);
				if (!m_hTexs[m_nSurfCount])
				{
					pRenderTarget->Release();
					return 0;
				}
				m_pDriver->xInstantiateTexture(m_hTexs[m_nSurfCount]);
				if (!m_pDriver->m_texList[m_hTexs[m_nSurfCount]].pTex)
				{
					m_pDriver->DestroyTexture(m_hTexs[m_nSurfCount]);
					pRenderTarget->Release();
					return 0;
				}
			}
			float u = (float)bi.bmiHeader.biWidth /
				m_pDriver->GetTextureWidth(m_hTexs[m_nSurfCount]);
			float v = (float)bi.bmiHeader.biHeight /
				m_pDriver->GetTextureHeight(m_hTexs[m_nSurfCount]);
			m_rects[m_nSurfCount].left = j * maxWidth;
			m_rects[m_nSurfCount].right = (j + 1) * maxWidth;
			m_rects[m_nSurfCount].top = i * maxHeight;
			m_rects[m_nSurfCount].bottom = (i + 1) * maxHeight;
			if (m_rects[m_nSurfCount].right > m_iBufWidth)
			{
				m_uvs[m_nSurfCount].x = u -
					(float)(m_rects[m_nSurfCount].right - m_iBufWidth) /
						maxWidth;
				m_rects[m_nSurfCount].right = m_iBufWidth;
			}
			else
				m_uvs[m_nSurfCount].x = u;
			if (m_rects[m_nSurfCount].bottom > m_iBufHeight)
			{
				m_uvs[m_nSurfCount].y = v -
					(float)(m_rects[m_nSurfCount].bottom - m_iBufHeight) /
						maxHeight;
				m_rects[m_nSurfCount].bottom = m_iBufHeight;
			}
			else
				m_uvs[m_nSurfCount].y = v;
			if (bCopy)
			{
				RECT src;
				src.left = j * maxWidth;
				src.right = (j + 1) * maxWidth;
				src.top = i * maxHeight;
				src.bottom = (i + 1) * maxHeight;
				if (src.right > m_iBufWidth)
					src.right = m_iBufWidth;
				if (src.bottom > m_iBufHeight)
					src.bottom = m_iBufHeight;
				RECT dest;
				dest.left = dest.top = 0;
				dest.right = src.right - src.left;
				dest.bottom = src.bottom - src.top;
				IDirect3DSurface9* targSurf;
				if (SUCCEEDED(m_pDriver->m_texList[m_hTexs[m_nSurfCount]]
							.pTex->GetSurfaceLevel(0, &targSurf)))
				{
					D3DXLoadSurfaceFromSurface(targSurf, 0, &dest,
						pRenderTarget, 0, &src, 1, 0);
					targSurf->Release();
				}
			}
			++m_nSurfCount;
		}
	}
	for (int i = m_nSurfCount; i < 64; ++i)
		if (m_hTexs[i] > 0)
			m_pDriver->DestroyTexture(m_hTexs[i]);
	pRenderTarget->Release();
	return 1;
}

void WSplashD3D::Reset()
{
	D3DSURFACE_DESC sdSplashTex, sdRenderTarget;
	IDirect3DSurface9* pRenderTarget;
	if (!m_nSurfCount)
		return;
	if (FAILED(m_pDriver->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget)))
		return;
	if (FAILED(pRenderTarget->GetDesc(&sdRenderTarget)))
	{
		pRenderTarget->Release();
		return;
	}
	pRenderTarget->Release();
	m_pDriver->m_texList[m_hTexs[0]].pTex->GetLevelDesc(0, &sdSplashTex);
	if (sdRenderTarget.Format != sdSplashTex.Format)
	{
		for (int i = 0; i < m_nSurfCount; ++i)
		{
			if (m_hTexs[i])
			{
				m_pDriver->DestroyTexture(m_hTexs[i]);
				m_hTexs[i] = 0;
			}
		}
	}
	InitFromScreen(false);
}

void WSplashD3D::SetTexCoordOffset(float u, float v)
{
	m_offset.x = u;
	m_offset.y = v;
}

void WSplashD3D::Draw(const WPoint* pSrc, const WRect* pDest,
	unsigned long color)
{
	if (m_pDriver->GetDeviceState() == W_VDEVSTATE_LOST)
		return;
	_MYVERTEX rect[4];
	for (int i = 0; i < m_nSurfCount; ++i)
	{
		float ol = (float)m_rects[i].left;
		float or = (float)m_rects[i].right;
		float ot = (float)m_rects[i].top;
		float ob = (float)m_rects[i].bottom;
		float l = (float)m_rects[i].left;
		float r = (float)m_rects[i].right;
		float t = (float)m_rects[i].top;
		float b = (float)m_rects[i].bottom;
		float su = 0, sv = 0, u = m_uvs[i].x, v = m_uvs[i].y;
		if (pSrc)
		{
			if (pSrc->x > r || pSrc->y > b)
				continue;
			if (pSrc->x > l)
			{
				su = (pSrc->x - l) / (r - l) * m_uvs[i].x;
				l = pSrc->x;
			}
			if (pSrc->y > t)
			{
				sv = (pSrc->y - t) / (b - t) * m_uvs[i].y;
				t = pSrc->y;
			}
			l -= pSrc->x;
			r -= pSrc->x;
			t -= pSrc->y;
			b -= pSrc->y;
			if (pDest)
			{
				if (l > pDest->w || pDest->h < t)
					continue;
				if (pDest->w < r)
				{
					u = (pSrc->x + pDest->w - l) / (or -ol) * m_uvs[i].x;
					r = pDest->w;
				}
				if (pDest->h < b)
				{
					v = (pSrc->y + pDest->h - t) / (ob - ot) * m_uvs[i].y;
					b = pDest->h;
				}
				l += pDest->x;
				r += pDest->x;
				b += pDest->y;
				t += pDest->y;
			}
		}
		l -= 0.5f;
		r -= 0.5f;
		t -= 0.5f;
		b -= 0.5f;
		rect[0] = _MYVERTEX(D3DXVECTOR3(l, b, 0.001f), 1, color, su, v);
		rect[1] = _MYVERTEX(D3DXVECTOR3(l, t, 0.001f), 1, color, su, sv);
		rect[2] = _MYVERTEX(D3DXVECTOR3(r, b, 0.001f), 1, color, u, v);
		rect[3] = _MYVERTEX(D3DXVECTOR3(r, t, 0.001f), 1, color, u, sv);
		m_pDriver->DrawPrimitive((m_hTexs[i] & 0x7ff) | 0x22300000, 4,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, rect,
			D3DPT_TRIANGLESTRIP, 1028);
	}
}

void WSplashD3D::Draw(unsigned long color)
{
	Draw(0, 0, color);
}

void WSplashD3D::ResetScreenSize()
{
	float fSclX = (float)m_pDriver->m_d3dpp.BackBufferWidth / m_iBufWidth;
	float fSclY = (float)m_pDriver->m_d3dpp.BackBufferHeight / m_iBufHeight;
	m_iBufWidth = m_pDriver->m_d3dpp.BackBufferWidth;
	m_iBufHeight = m_pDriver->m_d3dpp.BackBufferHeight;
	for (int i = 0; i < m_nSurfCount; ++i)
	{
		m_rects[i].left = (int)(m_rects[i].left * fSclX);
		m_rects[i].top = (int)(m_rects[i].top * fSclY);
		m_rects[i].right = (int)(m_rects[i].right * fSclX);
		m_rects[i].bottom = (int)(m_rects[i].bottom * fSclY);
	}
}

int WSplashD3D::GetWidth()
{
	return m_iSrcWidth;
}

int WSplashD3D::GetHeight()
{
	return m_iSrcHeight;
}

void WDirect3D8::sMergeBuffer::CheckAndIncreaseVertexStreamBuffer(
	unsigned long size)
{
	if (vertexStreamBufferSize < size)
	{
		vertexStreamBufferSize = size;
		vertexStreamBuffer = (unsigned char*)realloc(vertexStreamBuffer, size);
	}
}

void WDirect3D8::sMergeBuffer::CheckAndIncreaseIndexBuffer(unsigned long size)
{
	if (indexBufferSize < size)
	{
		indexBufferSize = size;
		indexBuffer = (unsigned short*)realloc(indexBuffer, size);
	}
}

WDirect3D8::sRtBackup::sRtBackup()
{
	Init();
}

void WDirect3D8::sRtBackup::Init()
{
	rt.Init();
	depthSurf = 0;
	memset(&viewport, 0, sizeof(viewport));
}

void WDirect3D8::sRtBackup::Restore(IDirect3DDevice9* dev,
	const sRtBackup& curRt)
{
	for (int i = 0; i < 2; ++i)
		if (rt.surf[i] != curRt.rt.surf[i])
			dev->SetRenderTarget(i, rt.surf[i]);
	if (depthSurf != curRt.depthSurf)
		dev->SetDepthStencilSurface(depthSurf);
	if (memcmp(&viewport, &curRt.viewport, sizeof(viewport)))
		dev->SetViewport(&viewport);
}

void WDirect3D8::WFxParamPool::Create(IDirect3DDevice9* dev)
{
}

void WDirect3D8::WFxParamPool::Release()
{
	ClearParamCache();
	if (m_pool)
	{
		m_pool->Release();
		m_pool = 0;
	}
}

void WDirect3D8::WFxParamPool::OnLostDevice(IDirect3DDevice9* dev)
{
}

void WDirect3D8::WFxParamPool::OnResetDevice(IDirect3DDevice9* dev)
{
	ClearParamCache();
}

void WDirect3D8::WFxParamPool::GetInt(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	int& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, int>::const_iterator it =
			m_paramCacheListPair_Int.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Int.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, int>::const_iterator it =
			m_paramCacheListPair_Int.second.find(hParam);
		if (it != m_paramCacheListPair_Int.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetInt(hParam, &Value);
}

void WDirect3D8::WFxParamPool::SetInt(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	int Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, int>::iterator it =
			m_paramCacheListPair_Int.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Int.first.end())
		{
			m_paramCacheListPair_Int.first.insert(
				std::map<WVDFXPARAMETERTYPE, int>::value_type(wvdFxParamType,
					Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, int>::iterator it =
			m_paramCacheListPair_Int.second.find(hParam);
		if (it == m_paramCacheListPair_Int.second.end())
		{
			m_paramCacheListPair_Int.second.insert(
				std::map<const char*, int>::value_type(hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetInt(hParam, Value);
}

void WDirect3D8::WFxParamPool::GetVector2(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	D3DXVECTOR2& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR2>::const_iterator it =
			m_paramCacheListPair_Vec2.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Vec2.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR2>::const_iterator it =
			m_paramCacheListPair_Vec2.second.find(hParam);
		if (it != m_paramCacheListPair_Vec2.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetFloatArray(hParam, (FLOAT*)&Value, 2);
}

void WDirect3D8::WFxParamPool::SetVector2(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	const D3DXVECTOR2& Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR2>::iterator it =
			m_paramCacheListPair_Vec2.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Vec2.first.end())
		{
			m_paramCacheListPair_Vec2.first.insert(
				std::map<WVDFXPARAMETERTYPE, D3DXVECTOR2>::value_type(
					wvdFxParamType, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR2>::iterator it =
			m_paramCacheListPair_Vec2.second.find(hParam);
		if (it == m_paramCacheListPair_Vec2.second.end())
		{
			m_paramCacheListPair_Vec2.second.insert(
				std::map<const char*, D3DXVECTOR2>::value_type(hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetFloatArray(hParam, (const FLOAT*)&Value, 2);
}

void WDirect3D8::WFxParamPool::GetVector3(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	D3DXVECTOR3& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR3>::const_iterator it =
			m_paramCacheListPair_Vec3.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Vec3.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR3>::const_iterator it =
			m_paramCacheListPair_Vec3.second.find(hParam);
		if (it != m_paramCacheListPair_Vec3.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetFloatArray(hParam, (FLOAT*)&Value, 3);
}

void WDirect3D8::WFxParamPool::SetVector3(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	const D3DXVECTOR3& Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR3>::iterator it =
			m_paramCacheListPair_Vec3.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Vec3.first.end())
		{
			m_paramCacheListPair_Vec3.first.insert(
				std::map<WVDFXPARAMETERTYPE, D3DXVECTOR3>::value_type(
					wvdFxParamType, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR3>::iterator it =
			m_paramCacheListPair_Vec3.second.find(hParam);
		if (it == m_paramCacheListPair_Vec3.second.end())
		{
			m_paramCacheListPair_Vec3.second.insert(
				std::map<const char*, D3DXVECTOR3>::value_type(hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetFloatArray(hParam, (const FLOAT*)&Value, 3);
}

void WDirect3D8::WFxParamPool::GetVector4(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	D3DXVECTOR4& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR4>::const_iterator it =
			m_paramCacheListPair_Vec4.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Vec4.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR4>::const_iterator it =
			m_paramCacheListPair_Vec4.second.find(hParam);
		if (it != m_paramCacheListPair_Vec4.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetVector(hParam, &Value);
}

void WDirect3D8::WFxParamPool::SetVector4(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	const D3DXVECTOR4& Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXVECTOR4>::iterator it =
			m_paramCacheListPair_Vec4.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Vec4.first.end())
		{
			m_paramCacheListPair_Vec4.first.insert(
				std::map<WVDFXPARAMETERTYPE, D3DXVECTOR4>::value_type(
					wvdFxParamType, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, D3DXVECTOR4>::iterator it =
			m_paramCacheListPair_Vec4.second.find(hParam);
		if (it == m_paramCacheListPair_Vec4.second.end())
		{
			m_paramCacheListPair_Vec4.second.insert(
				std::map<const char*, D3DXVECTOR4>::value_type(hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetVector(hParam, &Value);
}

void WDirect3D8::WFxParamPool::GetMatrix(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	D3DXMATRIX& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXMATRIX>::const_iterator it =
			m_paramCacheListPair_Mat.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Mat.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, D3DXMATRIX>::const_iterator it =
			m_paramCacheListPair_Mat.second.find(hParam);
		if (it != m_paramCacheListPair_Mat.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetMatrix(hParam, &Value);
}

void WDirect3D8::WFxParamPool::SetMatrix(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	const D3DXMATRIX& Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, D3DXMATRIX>::iterator it =
			m_paramCacheListPair_Mat.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Mat.first.end())
		{
			m_paramCacheListPair_Mat.first.insert(
				std::map<WVDFXPARAMETERTYPE, D3DXMATRIX>::value_type(
					wvdFxParamType, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, D3DXMATRIX>::iterator it =
			m_paramCacheListPair_Mat.second.find(hParam);
		if (it == m_paramCacheListPair_Mat.second.end())
		{
			m_paramCacheListPair_Mat.second.insert(
				std::map<const char*, D3DXMATRIX>::value_type(hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetMatrix(hParam, &Value);
}

void WDirect3D8::WFxParamPool::GetTexture(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	IDirect3DBaseTexture9*& Value) const
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, IDirect3DBaseTexture9*>::const_iterator
			it = m_paramCacheListPair_Tex.first.find(wvdFxParamType);
		if (it != m_paramCacheListPair_Tex.first.end())
		{
			Value = (*it).second;
			return;
		}
	}
	else
	{
		std::map<const char*, IDirect3DBaseTexture9*>::const_iterator it =
			m_paramCacheListPair_Tex.second.find(hParam);
		if (it != m_paramCacheListPair_Tex.second.end())
		{
			Value = (*it).second;
			return;
		}
	}
	ef->GetTexture(hParam, &Value);
}

void WDirect3D8::WFxParamPool::SetTexture(ID3DXEffect* ef,
	WVDFXPARAMETERTYPE wvdFxParamType, const char* hParam, bool isShared,
	IDirect3DBaseTexture9* Value)
{
	if (!ef || !hParam)
		return;
	if (isShared)
	{
		std::map<WVDFXPARAMETERTYPE, IDirect3DBaseTexture9*>::iterator it =
			m_paramCacheListPair_Tex.first.find(wvdFxParamType);
		if (it == m_paramCacheListPair_Tex.first.end())
		{
			m_paramCacheListPair_Tex.first.insert(std::map<WVDFXPARAMETERTYPE,
				IDirect3DBaseTexture9*>::value_type(wvdFxParamType, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	else
	{
		std::map<const char*, IDirect3DBaseTexture9*>::iterator it =
			m_paramCacheListPair_Tex.second.find(hParam);
		if (it == m_paramCacheListPair_Tex.second.end())
		{
			m_paramCacheListPair_Tex.second.insert(
				std::map<const char*, IDirect3DBaseTexture9*>::value_type(
					hParam, Value));
		}
		else
		{
			if ((*it).second == Value)
				return;
			(*it).second = Value;
		}
	}
	ef->SetTexture(hParam, Value);
}

void WDirect3D8::WFxParamPool::ClearParamCache()
{
	m_paramCacheListPair_Int.first.clear();
	m_paramCacheListPair_Int.second.clear();
	m_paramCacheListPair_Vec2.first.clear();
	m_paramCacheListPair_Vec2.second.clear();
	m_paramCacheListPair_Vec3.first.clear();
	m_paramCacheListPair_Vec3.second.clear();
	m_paramCacheListPair_Vec4.first.clear();
	m_paramCacheListPair_Vec4.second.clear();
	m_paramCacheListPair_Mat.first.clear();
	m_paramCacheListPair_Mat.second.clear();
	m_paramCacheListPair_Tex.first.clear();
	m_paramCacheListPair_Tex.second.clear();
}

bool WDirect3D8::WRenderState::operator==(const WRenderState& rhs) const
{
	if (m_fxMacro != rhs.m_fxMacro)
		return false;
	if (m_rsList.size() != rhs.m_rsList.size())
		return false;
	if (m_tssList.size() != rhs.m_tssList.size())
		return false;
	if (m_transfList.size() != rhs.m_transfList.size())
		return false;
	if (m_texList.size() != rhs.m_texList.size())
		return false;
	if (m_clipPlaneList.size() != rhs.m_clipPlaneList.size())
		return false;
	if (m_ssList.size() != rhs.m_ssList.size())
		return false;
	if (m_fxParamIntList.size() != rhs.m_fxParamIntList.size())
		return false;
	if (m_fxParamVec2List.size() != rhs.m_fxParamVec2List.size())
		return false;
	if (m_fxParamVec3List.size() != rhs.m_fxParamVec3List.size())
		return false;
	if (m_fxParamVec4List.size() != rhs.m_fxParamVec4List.size())
		return false;
	if (m_fxParamMatList.size() != rhs.m_fxParamMatList.size())
		return false;
	if (m_fxParamTexList.size() != rhs.m_fxParamTexList.size())
		return false;
	for (RsList::const_iterator itr1 = m_rsList.begin(); itr1 != m_rsList.end();
		itr1++)
	{
		RsList::const_iterator itr2;
		for (itr2 = rhs.m_rsList.begin(); itr2 != rhs.m_rsList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_rsList.end() || (*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (TssList::const_iterator itr1 = m_tssList.begin();
		itr1 != m_tssList.end(); itr1++)
	{
		TssList::const_iterator itr2;
		for (itr2 = rhs.m_tssList.begin(); itr2 != rhs.m_tssList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type &&
				(*itr1).m_Type2 == (*itr2).m_Type2)
				break;
		}
		if (itr2 == rhs.m_tssList.end() || (*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (TransfList::const_iterator itr1 = m_transfList.begin();
		itr1 != m_transfList.end(); itr1++)
	{
		TransfList::const_iterator itr2;
		for (itr2 = rhs.m_transfList.begin(); itr2 != rhs.m_transfList.end();
			itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_transfList.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (TexList::const_iterator itr1 = m_texList.begin();
		itr1 != m_texList.end(); itr1++)
	{
		TexList::const_iterator itr2;
		for (itr2 = rhs.m_texList.begin(); itr2 != rhs.m_texList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_texList.end() || (*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (ClipPlaneList::const_iterator itr1 = m_clipPlaneList.begin();
		itr1 != m_clipPlaneList.end(); itr1++)
	{
		ClipPlaneList::const_iterator itr2;
		for (itr2 = rhs.m_clipPlaneList.begin();
			itr2 != rhs.m_clipPlaneList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_clipPlaneList.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (SsList::const_iterator itr1 = m_ssList.begin(); itr1 != m_ssList.end();
		itr1++)
	{
		SsList::const_iterator itr2;
		for (itr2 = rhs.m_ssList.begin(); itr2 != rhs.m_ssList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type &&
				(*itr1).m_Type2 == (*itr2).m_Type2)
				break;
		}
		if (itr2 == rhs.m_ssList.end() || (*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamIntList::const_iterator itr1 = m_fxParamIntList.begin();
		itr1 != m_fxParamIntList.end(); itr1++)
	{
		FxParamIntList::const_iterator itr2;
		for (itr2 = rhs.m_fxParamIntList.begin();
			itr2 != rhs.m_fxParamIntList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamIntList.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamVec2List::const_iterator itr1 = m_fxParamVec2List.begin();
		itr1 != m_fxParamVec2List.end(); itr1++)
	{
		FxParamVec2List::const_iterator itr2;
		for (itr2 = rhs.m_fxParamVec2List.begin();
			itr2 != rhs.m_fxParamVec2List.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamVec2List.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamVec3List::const_iterator itr1 = m_fxParamVec3List.begin();
		itr1 != m_fxParamVec3List.end(); itr1++)
	{
		FxParamVec3List::const_iterator itr2;
		for (itr2 = rhs.m_fxParamVec3List.begin();
			itr2 != rhs.m_fxParamVec3List.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamVec3List.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamVec4List::const_iterator itr1 = m_fxParamVec4List.begin();
		itr1 != m_fxParamVec4List.end(); itr1++)
	{
		FxParamVec4List::const_iterator itr2;
		for (itr2 = rhs.m_fxParamVec4List.begin();
			itr2 != rhs.m_fxParamVec4List.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamVec4List.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamMatList::const_iterator itr1 = m_fxParamMatList.begin();
		itr1 != m_fxParamMatList.end(); itr1++)
	{
		FxParamMatList::const_iterator itr2;
		for (itr2 = rhs.m_fxParamMatList.begin();
			itr2 != rhs.m_fxParamMatList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamMatList.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	for (FxParamTexList::const_iterator itr1 = m_fxParamTexList.begin();
		itr1 != m_fxParamTexList.end(); itr1++)
	{
		FxParamTexList::const_iterator itr2;
		for (itr2 = rhs.m_fxParamTexList.begin();
			itr2 != rhs.m_fxParamTexList.end(); itr2++)
		{
			if ((*itr1).m_Type == (*itr2).m_Type)
				break;
		}
		if (itr2 == rhs.m_fxParamTexList.end() ||
			(*itr1).m_Value != (*itr2).m_Value)
			return false;
	}
	return true;
}

void WDirect3D8::WRenderState::SetRenderState(D3DRENDERSTATETYPE state,
	unsigned long value)
{
	RsList::iterator it;
	for (it = this->m_rsList.begin(); it != this->m_rsList.end(); it++)
	{
		if ((*it).m_Type == state)
		{
			(*it).m_Value = value;
			break;
		}
	}
	if (it == this->m_rsList.end())
		this->m_rsList.push_back(
			sTwoVarRs<D3DRENDERSTATETYPE, DWORD>(state, value));
}

void WDirect3D8::WRenderState::SetTextureStageState(unsigned long stage,
	D3DTEXTURESTAGESTATETYPE type, unsigned long value)
{
	TssList::iterator it;
	for (it = this->m_tssList.begin(); it != this->m_tssList.end(); it++)
	{
		if ((*it).m_Type == stage && (*it).m_Type2 == type)
		{
			(*it).m_Value = value;
			break;
		}
	}

	if (it == this->m_tssList.end())
		this->m_tssList.push_back(
			sThreeVarRs<DWORD, D3DTEXTURESTAGESTATETYPE, DWORD>(stage, type,
				value));
}

void WDirect3D8::WRenderState::SetTransform(D3DTRANSFORMSTATETYPE state,
	const D3DXMATRIX& matrix)
{
	std::vector<sTwoVarRs<D3DTRANSFORMSTATETYPE, D3DXMATRIX> >::iterator it;
	for (it = this->m_transfList.begin(); it != this->m_transfList.end(); it++)
	{
		if ((*it).m_Type == state)
		{
			(*it).m_Value = matrix;
			break;
		}
	}
	if (it == this->m_transfList.end())
		this->m_transfList.push_back(
			sTwoVarRs<D3DTRANSFORMSTATETYPE, D3DXMATRIX>(state, matrix));
}

void WDirect3D8::WRenderState::SetTexture(unsigned long stage,
	IDirect3DBaseTexture9* pTex)
{
	TexList::iterator it;
	for (it = this->m_texList.begin(); it != this->m_texList.end(); it++)
	{
		if ((*it).m_Type == stage)
		{
			(*it).m_Value = pTex;
			break;
		}
	}
	if (it == this->m_texList.end())
		this->m_texList.push_back(
			sTwoVarRs<DWORD, IDirect3DBaseTexture9*>(stage, pTex));
}

void WDirect3D8::WRenderState::SetClipPlane(unsigned long Index,
	const D3DXPLANE& Value)
{
	ClipPlaneList::iterator it;
	for (it = this->m_clipPlaneList.begin(); it != this->m_clipPlaneList.end();
		it++)
	{
		if ((*it).m_Type == Index)
		{
			(*it).m_Value = Value;
			break;
		}
	}
	if (it == this->m_clipPlaneList.end())
		this->m_clipPlaneList.push_back(sClipPlane(Index, Value));
}

void WDirect3D8::WRenderState::SetSamplerState(unsigned long sampler,
	D3DSAMPLERSTATETYPE type, unsigned long value)
{
	SsList::iterator it;
	for (it = this->m_ssList.begin(); it != this->m_ssList.end(); it++)
	{
		if ((*it).m_Type == sampler && (*it).m_Type2 == type)
		{
			(*it).m_Value = value;
			break;
		}
	}
	if (it == this->m_ssList.end())
		this->m_ssList.push_back(sThreeVarRs<DWORD, D3DSAMPLERSTATETYPE, DWORD>(
			sampler, type, value));
}

void WDirect3D8::WRenderState::SetFxMacro(unsigned long macro)
{
	m_fxMacro = macro;
}

void WDirect3D8::WRenderState::SetFxParamInt(WVDFXPARAMETERTYPE paramType,
	int value)
{
	FxParamIntList::iterator it;
	for (it = this->m_fxParamIntList.begin();
		it != this->m_fxParamIntList.end(); it++)
	{
		if ((*it).m_Type == paramType)
		{
			(*it).m_Value = value;
			break;
		}
	}
	if (it == this->m_fxParamIntList.end())
		this->m_fxParamIntList.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, int>(paramType, value));
}

void WDirect3D8::WRenderState::SetFxParamVector2(WVDFXPARAMETERTYPE ParamType,
	const D3DXVECTOR2& Value)
{
	FxParamVec2List::iterator it;
	for (it = this->m_fxParamVec2List.begin();
		it != this->m_fxParamVec2List.end(); it++)
	{
		if ((*it).m_Type == ParamType)
		{
			(*it).m_Value = Value;
			break;
		}
	}
	if (it == this->m_fxParamVec2List.end())
		this->m_fxParamVec2List.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR2>(ParamType, Value));
}

void WDirect3D8::WRenderState::SetFxParamVector3(WVDFXPARAMETERTYPE ParamType,
	const D3DXVECTOR3& Value)
{
	FxParamVec3List::iterator it;
	for (it = this->m_fxParamVec3List.begin();
		it != this->m_fxParamVec3List.end(); it++)
	{
		if ((*it).m_Type == ParamType)
		{
			(*it).m_Value = Value;
			break;
		}
	}
	if (it == this->m_fxParamVec3List.end())
		this->m_fxParamVec3List.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR3>(ParamType, Value));
}

void WDirect3D8::WRenderState::SetFxParamVector4(WVDFXPARAMETERTYPE ParamType,
	const D3DXVECTOR4& Value)
{
	FxParamVec4List::iterator it;
	for (it = this->m_fxParamVec4List.begin();
		it != this->m_fxParamVec4List.end(); it++)
	{
		if ((*it).m_Type == ParamType)
		{
			(*it).m_Value = Value;
			break;
		}
	}
	if (it == this->m_fxParamVec4List.end())
		this->m_fxParamVec4List.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, D3DXVECTOR4>(ParamType, Value));
}

void WDirect3D8::WRenderState::SetFxParamMatrix(WVDFXPARAMETERTYPE ParamType,
	const D3DXMATRIX& Value)
{
	FxParamMatList::iterator it;
	for (it = this->m_fxParamMatList.begin();
		it != this->m_fxParamMatList.end(); it++)
	{
		if ((*it).m_Type == ParamType)
		{
			(*it).m_Value = Value;
			break;
		}
	}
	if (it == this->m_fxParamMatList.end())
		this->m_fxParamMatList.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, D3DXMATRIX>(ParamType, Value));
}

void WDirect3D8::WRenderState::SetFxParamTexture(WVDFXPARAMETERTYPE ParamType,
	IDirect3DBaseTexture9* pTex)
{
	FxParamTexList::iterator it;
	for (it = this->m_fxParamTexList.begin();
		it != this->m_fxParamTexList.end(); it++)
	{
		if ((*it).m_Type == ParamType)
		{
			(*it).m_Value = pTex;
			break;
		}
	}
	if (it == this->m_fxParamTexList.end())
		this->m_fxParamTexList.push_back(
			sTwoVarRs<WVDFXPARAMETERTYPE, IDirect3DBaseTexture9*>(ParamType,
				pTex));
}

void WDirect3D8::WRenderState::Begin(WDirect3D8& wd3d)
{
	D3DXMATRIX proj, view;
	sEffect* effect;

	if (!this->HasAnyState())
	{
		return;
	}

	for (RsList::iterator it = this->m_rsList.begin();
		it != this->m_rsList.end(); it++)
	{
		wd3d.m_pd3dDevice->GetRenderState((*it).m_Type, &(*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.m_pd3dDevice->SetRenderState((*it).m_Type, (*it).m_Value);
		}
	}

	for (TssList::iterator it = this->m_tssList.begin();
		it != this->m_tssList.end(); it++)
	{
		wd3d.m_pd3dDevice->GetTextureStageState((*it).m_Type, (*it).m_Type2,
			&(*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.m_pd3dDevice->SetTextureStageState((*it).m_Type, (*it).m_Type2,
				(*it).m_Value);
		}
	}

	for (TransfList::iterator it = this->m_transfList.begin();
		it != this->m_transfList.end(); it++)
	{
		wd3d.m_pd3dDevice->GetTransform((*it).m_Type, &(*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.m_pd3dDevice->SetTransform((*it).m_Type, &(*it).m_Value);
		}
	}

	for (TexList::iterator it = this->m_texList.begin();
		it != this->m_texList.end(); it++)
	{
		TexList::reference state = *it;
		wd3d.m_pd3dDevice->GetTexture(state.m_Type, &state.m_OldValue);
		if (state.m_Value != state.m_OldValue)
		{
			wd3d.m_pd3dDevice->SetTexture(state.m_Type, state.m_Value);
		}
		if (state.m_OldValue != NULL)
		{
			state.m_OldValue->Release();
		}
	}

	for (ClipPlaneList::iterator it = this->m_clipPlaneList.begin();
		it != this->m_clipPlaneList.end(); it++)
	{
		wd3d.m_pd3dDevice->GetClipPlane((*it).m_Type,
			(float*)&(*it).m_OldValue);
		if (wd3d.m_lastEffect)
		{
			D3DXMatrixInverse(&view, 0, &wd3d.m_xLastViewMatrix);
			D3DXMatrixTranspose(&view, &view);
			D3DXPlaneTransform(&(*it).m_SetValue, &(*it).m_Value, &view);
			D3DXMatrixInverse(&proj, 0, &wd3d.m_xLastProjMatrix);
			D3DXMatrixTranspose(&proj, &proj);
			D3DXPlaneTransform(&(*it).m_SetValue, &(*it).m_SetValue, &proj);
		}
		else
		{
			(*it).m_SetValue = (*it).m_Value;
		}
		if (!((*it).m_SetValue == (*it).m_OldValue))
		{
			wd3d.m_pd3dDevice->SetClipPlane((*it).m_Type,
				(float*)&(*it).m_SetValue);
		}
	}

	for (SsList::iterator it = this->m_ssList.begin();
		it != this->m_ssList.end(); it++)
	{
		wd3d.m_pd3dDevice->GetSamplerState((*it).m_Type, (*it).m_Type2,
			&(*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.m_pd3dDevice->SetSamplerState((*it).m_Type, (*it).m_Type2,
				(*it).m_Value);
		}
	}

	if (!wd3d.m_lastEffect)
	{
		return;
	}

	effect = &wd3d.m_EffectTable[wd3d.m_lastEffect];
	if (!effect->pEffect)
	{
		return;
	}

	for (FxParamIntList::iterator it = this->m_fxParamIntList.begin();
		it != this->m_fxParamIntList.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		wd3d.GetFxParamPool().GetInt(effect->pEffect, (*it).m_Type,
			effect->param[(*it).m_Type].handle,
			effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.GetFxParamPool().SetInt(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_Value);
		}
	}

	for (FxParamVec2List::iterator it = this->m_fxParamVec2List.begin();
		it != this->m_fxParamVec2List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		wd3d.GetFxParamPool().GetVector2(effect->pEffect, (*it).m_Type,
			effect->param[(*it).m_Type].handle,
			effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.GetFxParamPool().SetVector2(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_Value);
		}
	}

	for (FxParamVec3List::iterator it = this->m_fxParamVec3List.begin();
		it != this->m_fxParamVec3List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		wd3d.GetFxParamPool().GetVector3(effect->pEffect, (*it).m_Type,
			effect->param[(*it).m_Type].handle,
			effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.GetFxParamPool().SetVector3(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_Value);
		}
	}

	for (FxParamVec4List::iterator it = this->m_fxParamVec4List.begin();
		it != this->m_fxParamVec4List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		wd3d.GetFxParamPool().GetVector4(effect->pEffect, (*it).m_Type,
			effect->param[(*it).m_Type].handle,
			effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.GetFxParamPool().SetVector4(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_Value);
		}
	}

	for (FxParamMatList::iterator it = this->m_fxParamMatList.begin();
		it != this->m_fxParamMatList.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}

		wd3d.GetFxParamPool().GetMatrix(effect->pEffect, (*it).m_Type,
			effect->param[(*it).m_Type].handle,
			effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
		if ((*it).m_Value != (*it).m_OldValue)
		{
			wd3d.GetFxParamPool().SetMatrix(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_Value);
		}
	}

	for (FxParamTexList::iterator it = this->m_fxParamTexList.begin();
		it != this->m_fxParamTexList.end(); it++)
	{
		FxParamTexList::reference state = *it;
		if (!effect->param[state.m_Type].handle)
		{
			continue;
		}
		wd3d.GetFxParamPool().GetTexture(effect->pEffect, state.m_Type,
			effect->param[state.m_Type].handle,
			effect->param[state.m_Type].isShared, state.m_OldValue);
		if (state.m_Value != state.m_OldValue)
		{
			wd3d.GetFxParamPool().SetTexture(effect->pEffect, state.m_Type,
				effect->param[state.m_Type].handle,
				effect->param[state.m_Type].isShared, state.m_Value);
		}
		if (state.m_OldValue != NULL)
		{
			state.m_OldValue->Release();
		}
	}

	effect->pEffect->CommitChanges();
}

void WDirect3D8::WRenderState::End(WDirect3D8& wd3d)
{
	sEffect* effect;

	if (!this->HasAnyState())
	{
		return;
	}

	for (RsList::iterator it = this->m_rsList.begin();
		it != this->m_rsList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.m_pd3dDevice->SetRenderState((*it).m_Type, (*it).m_OldValue);
	}

	for (TssList::iterator it = this->m_tssList.begin();
		it != this->m_tssList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.m_pd3dDevice->SetTextureStageState((*it).m_Type, (*it).m_Type2,
				(*it).m_OldValue);
	}

	for (TransfList::iterator it = this->m_transfList.begin();
		it != this->m_transfList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.m_pd3dDevice->SetTransform((*it).m_Type, &(*it).m_OldValue);
	}

	for (TexList::iterator it = this->m_texList.begin();
		it != this->m_texList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.m_pd3dDevice->SetTexture((*it).m_Type, (*it).m_OldValue);
	}

	for (ClipPlaneList::iterator it = this->m_clipPlaneList.begin();
		it != this->m_clipPlaneList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_SetValue)
			wd3d.m_pd3dDevice->SetClipPlane((*it).m_Type,
				(float*)&(*it).m_OldValue);
	}

	for (SsList::iterator it = this->m_ssList.begin();
		it != this->m_ssList.end(); it++)
	{
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.m_pd3dDevice->SetSamplerState((*it).m_Type, (*it).m_Type2,
				(*it).m_OldValue);
	}

	if (!wd3d.m_lastEffect)
	{
		return;
	}

	effect = &wd3d.m_EffectTable[wd3d.m_lastEffect];
	if (!effect->pEffect)
	{
		return;
	}

	for (FxParamIntList::iterator it = this->m_fxParamIntList.begin();
		it != this->m_fxParamIntList.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetInt(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	for (FxParamVec2List::iterator it = this->m_fxParamVec2List.begin();
		it != this->m_fxParamVec2List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetVector2(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	for (FxParamVec3List::iterator it = this->m_fxParamVec3List.begin();
		it != this->m_fxParamVec3List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetVector3(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	for (FxParamVec4List::iterator it = this->m_fxParamVec4List.begin();
		it != this->m_fxParamVec4List.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetVector4(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	for (FxParamMatList::iterator it = this->m_fxParamMatList.begin();
		it != this->m_fxParamMatList.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetMatrix(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	for (FxParamTexList::iterator it = this->m_fxParamTexList.begin();
		it != this->m_fxParamTexList.end(); it++)
	{
		if (!effect->param[(*it).m_Type].handle)
		{
			continue;
		}
		if ((*it).m_OldValue != (*it).m_Value)
			wd3d.GetFxParamPool().SetTexture(effect->pEffect, (*it).m_Type,
				effect->param[(*it).m_Type].handle,
				effect->param[(*it).m_Type].isShared, (*it).m_OldValue);
	}

	wd3d.m_EffectTable[wd3d.m_lastEffect].pEffect->CommitChanges();
}

WDirect3D8::WDirect3D8(char* devName, int id)
{
	if (g_winVer == WinVerNone)
		g_winVer = GetWindowsVersion();
	if (devName)
	{
		m_devName = new char[strlen(devName) + 6];
		strcpy(m_devName, "[DX8]");
		strcat(m_devName, devName);
	}
	m_devId = id;
	m_modList = 0;
	m_pd3dDevice = 0;
	m_hwnd = 0;
	m_BackBufBpp = 0;
	m_fps = 60.0f;
	m_d3d8 = 0;
	m_fmtWindowed = D3DFMT_X8R8G8B8;
	m_clientRcCheckedAfterReset = true;
	m_pEventQuery = 0;
	m_pCopiedScreenSurface = 0;
	m_hCopiedScreenTexture = 0;
	m_pCopiedScreenSplash = 0;
	m_useCopiedScreen = false;
	memset(m_texList, 0, sizeof(m_texList));
	memset(m_depthSurfList, 0, sizeof(m_depthSurfList));
	m_commonDepthSurf = 0;
	memset(m_xaVbList, 0, sizeof(m_xaVbList));
	memset(m_xaIbList, 0, sizeof(m_xaIbList));
	m_bUseMipmap = false;
	m_iMaxMipLvl = 1;
	m_dwMipTexStateFilter = 1;
	m_xnDIPs = 0;
	m_xnDPs = 0;
	m_xnDIPUPs = 0;
	m_xnDPUPs = 0;
	m_xnTotalTris = 0;
	m_iDDSRes = 0;
	m_dwMipCreateFilter = 2;
	m_RtFmtIdx = -1;
	m_RtNoAlphaFmtIdx = -1;
	m_RtLowMemFmtIdx = -1;
	m_RtNoAlphaLowMemFmtIdx = -1;
	m_RtDepthFmtIdx = -1;
	m_xiVbSize = 0;
	m_pLockableVB = 0;
	m_pLockableIB = 0;
	memset(&m_MergeBuffer, 0, sizeof(m_MergeBuffer));
	m_Effect = -1;
	static float _shcoeff[28] = { -0.33614999f, 0.94016099f, 0.127409f,
		1.14203f, -0.33537f, 1.16755f, 0.100581f, 1.3035001f, -0.28731999f,
		1.35096f, 0.036699001f, 1.39474f, -0.16829801f, 0.0227974f,
		-0.22076701f, -0.159484f, -0.149726f, -0.038500499f, -0.29665101f,
		-0.140673f, -0.106568f, -0.126265f, -0.36153501f, -0.102911f,
		0.0286815f, -0.0049609598f, -0.060617f, 1.0f };
	for (int i = 0; i < 28; ++i)
		m_SHCoeff[i / 4][i % 4] = _shcoeff[i];
	m_GlobalRS[0] = 0;
	m_GlobalRS[1] = 0;
	m_pScreenCape = 0;
	m_pScreenCapeFactory = 0;
	m_pkDWMApiDll = 0;
	m_pkDWMApiDll = new nsWindowUtility::CDWMApiDll;
	if (m_pkDWMApiDll)
	{
		if (g_winVer < WinVerWindowsVista || !m_pkDWMApiDll->Load_DWMAPIDll())
		{
			delete m_pkDWMApiDll;
			m_pkDWMApiDll = 0;
		}
	}
	m_fillScrMode = false;
}

WDirect3D8::~WDirect3D8()
{
	WDirect3D::Release();
	Release();
	if (g_error)
	{
		MessageBoxA(0, g_error,
			"\xba\xf1\xb5\xf0\xbf\xc0 \xc4\xab\xb5\xe5 \xbf\xa1\xb7\xaf", 0);
		g_error = 0;
	}
	if (m_pkDWMApiDll)
	{
		delete m_pkDWMApiDll;
		m_pkDWMApiDll = 0;
	}
}

void WDirect3D8::Release()
{
	if (m_pLockableVB)
	{
		m_pLockableVB->Release();
		m_pLockableVB = 0;
	}
	if (m_pLockableIB)
	{
		m_pLockableIB->Release();
		m_pLockableIB = 0;
	}
	if (m_MergeBuffer.vertexStreamBuffer)
		free(m_MergeBuffer.vertexStreamBuffer);
	if (m_MergeBuffer.indexBuffer)
		free(m_MergeBuffer.indexBuffer);
	if (m_pScreenCape)
	{
		m_pScreenCape->Release();
		m_pScreenCape = 0;
	}
	if (m_pScreenCapeFactory)
	{
		delete m_pScreenCapeFactory;
		m_pScreenCapeFactory = 0;
	}
	if (m_commonDepthSurf)
	{
		m_commonDepthSurf->Release();
		m_commonDepthSurf = 0;
	}
	for (int i = 0; i < 32; ++i)
	{
		if (m_depthSurfList[i].pDepth)
		{
			m_depthSurfList[i].pDepth->Release();
			m_depthSurfList[i].pDepth = 0;
		}
	}
	if (m_pCopiedScreenSurface)
	{
		m_pCopiedScreenSurface->Release();
		m_pCopiedScreenSurface = 0;
	}
	if (m_hCopiedScreenTexture > 0)
	{
		DestroyTexture(m_hCopiedScreenTexture);
		m_hCopiedScreenTexture = 0;
	}
	m_useCopiedScreen = false;
	if (m_pd3dDevice)
		ReleaseAllRendertargetBackupResource();
	ReleaseShaderResource();
	if (m_devName)
	{
		delete[] m_devName;
		m_devName = 0;
	}
	if (m_modList)
	{
		delete[] m_modList;
		m_modList = 0;
	}
	if (m_pEventQuery)
	{
		m_pEventQuery->Release();
		m_pEventQuery = 0;
	}
	if (m_pd3dDevice)
	{
		m_pd3dDevice->Release();
		m_pd3dDevice = 0;
	}
	if (m_d3d8)
	{
		m_d3d8->Release();
		m_d3d8 = 0;
	}
	m_devId = -1;
}

int WDirect3D8::WinProc(unsigned int message, unsigned long wParam,
	unsigned long lParam)
{
	switch (message)
	{
	case WM_PAINT:
		if (this->m_pd3dDevice && this->m_bWindow &&
			this->m_pd3dDevice->TestCooperativeLevel() >= 0)
		{
			this->m_pd3dDevice->Present(0, 0, this->m_hwnd, 0);
		}
		return -1;

	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SIZE:
		case SC_MOVE:
		case SC_MAXIMIZE:
		case SC_KEYMENU:
		case SC_MONITORPOWER:
			if (!m_bWindow)
				return 1;
			break;
		}
		break;
	case WM_ACTIVATEAPP:
		return 1;
	default:
		break;
	}

	return 0;
}

inline void WDirect3D8::SetRenderState(unsigned long dwType,
	unsigned long dwTypeEx)
{
	unsigned long state = (dwType & 0xfffc0000) | (dwTypeEx & 0x3fff);
	if (m_lastRenderState == state)
	{
		return;
	}
	FlushRenderPrimitive();
	SetBlendMode(state);
	unsigned long dwXor = state ^ m_lastRenderState;
	if (dwXor & 0x300000)
	{
		if ((m_lastRenderState & 0x300000) == 0x200000)
		{
			_SetRenderState(D3DRS_DEPTHBIAS, 0);
		}
		switch (state & 0x300000)
		{
		case 0:
			if ((m_lastRenderState & 0x300000) != 0x200000)
				_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			break;
		case 0x100000:
			_SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
			break;
		case 0x200000:
		{
			if (m_lastRenderState & 0x300000)
				_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			static float fDepthBias = -0.0005f;
			_SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&fDepthBias);
			break;
		}
		case 0x300000:
			_SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			break;
		}
	}
	if (dwXor & 0x2000000)
	{
		if (state & 0x2000000)
		{
			_SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			_SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
		else
		{
			_SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			_SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}
	}
	if (dwXor & 0x8000000)
		_SetRenderState(D3DRS_FOGENABLE, state & 0x8000000 ? TRUE : FALSE);
	if (dwXor & 0x80000)
	{
		if (state & 0x80000)
		{
			_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		}
		else
		{
			_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		}
	}
	if (dwXor & 0x20000000)
		_SetRenderState(D3DRS_ZWRITEENABLE, state & 0x20000000 ? FALSE : TRUE);
	if (dwXor & 0x80000000)
		_SetRenderState(D3DRS_ALPHAREF, state & 0x80000000 ? 128 : 0);
	if (dwXor & 0xc00)
	{
		switch (state & 0xc00)
		{
		case 0:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			break;
		case 0x400:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			break;
		case 0x800:
			_SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			break;
		}
	}
	if (dwXor & 4)
	{
		if (state & 4)
			_SetRenderState(D3DRS_LIGHTING, FALSE);
		else
			_SetRenderState(D3DRS_LIGHTING, TRUE);
	}
	else if (dwXor & 0x10)
	{
		if (state & 0x10)
			_SetRenderState(D3DRS_LIGHTING, FALSE);
		else
			_SetRenderState(D3DRS_LIGHTING, TRUE);
	}
	if (dwXor & 0x2000)
		_SetRenderState(D3DRS_COLORVERTEX, state & 0x2000 ? FALSE : TRUE);
	if (dwXor & 0x2014)
	{
		m_xbUseTFactor = (state & 0x14) && (state & 0x2000);
		SetTextureStageState(0, 1,
			m_xbUseTFactor ? D3DTA_TFACTOR : D3DTA_DIFFUSE);
		SetTextureStageState(0, 4, D3DTA_DIFFUSE);
	}
	m_lastRenderState = state;
}

inline void WDirect3D8::SetBlendState(unsigned long srcBlend,
	unsigned long dstBlend)
{
	if (this->m_blendEnable && srcBlend == D3DBLEND_ONE &&
		dstBlend == D3DBLEND_ZERO)
	{
		this->_SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		this->m_blendEnable = false;
		return;
	}
	if (!this->m_blendEnable)
	{
		this->_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		this->m_blendEnable = true;
	}
	if (this->m_lastBlendState[0] != srcBlend)
	{
		this->_SetRenderState(D3DRS_SRCBLEND, srcBlend);
		this->m_lastBlendState[0] = srcBlend;
	}
	if (this->m_lastBlendState[1] != dstBlend)
	{
		this->_SetRenderState(D3DRS_DESTBLEND, dstBlend);
		this->m_lastBlendState[1] = dstBlend;
	}
}

inline void WDirect3D8::SetBlendMode(unsigned long dwState)
{
	dwState &= 0x41800000;

	if (this->m_lastBlendMode == dwState)
	{
		return;
	}

	this->m_lastBlendMode = dwState;

	switch (dwState)
	{
	case 0x00000000:
		this->SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
		break;
	case 0x40800000:
		this->SetBlendState(D3DBLEND_ONE, D3DBLEND_ONE);
		break;
	case 0x01000000:
	case 0x41000000:
		this->SetBlendState(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
		break;
	case 0x01800000:
	case 0x41800000:
		this->SetBlendState(D3DBLEND_ZERO, D3DBLEND_INVSRCCOLOR);
		break;
	case 0x40000000:
		this->SetBlendState(D3DBLEND_ONE, D3DBLEND_ZERO);
		break;
	case 0x0800000:
		this->SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_ONE);
		break;
	default:
		break;
	}
}

inline void WDirect3D8::SetTextureStageState(int stage, int type,
	unsigned long value)
{
	if (type >= 6)
	{
		if (stage < m_MaxTextureBlendStages &&
			m_texStageState[stage][type - 6] != value)
		{
			_SetSamplerState(stage, g_TexSamplerStateList[type - 6], value);
			m_texStageState[stage][type] = value;
			return;
		}
	}
	if ((unsigned int)type < 6 && stage < m_MaxTextureBlendStages &&
		m_texStageState[stage][type] != value)
	{
		_SetTextureStageState(stage, g_TexStageStateList[type], value);
		m_texStageState[stage][type] = value;
	}
}

HRESULT WDirect3D8::SetVertexShader(unsigned long dwVertexTypeDesc)
{
	if (this->m_xLastVertexDecl == dwVertexTypeDesc)
	{
		return 0;
	}

	this->FlushRenderPrimitive();

	this->m_xLastVertexDecl = dwVertexTypeDesc;
	if ((dwVertexTypeDesc & D3DFVF_POSITION_MASK) == 2 ||
		(dwVertexTypeDesc & D3DFVF_POSITION_MASK) == 4)
	{
		return this->m_pd3dDevice->SetFVF(dwVertexTypeDesc);
	}
	return this->m_pd3dDevice->SetFVF((dwVertexTypeDesc & 0xFFFFAFF3) | 2);
}

inline void WDirect3D8::SetVtxMode(unsigned long dwVertexTypeDesc)
{
	m_lastVtxType = dwVertexTypeDesc & 0x7fffffff;
	SetVertexShader(dwVertexTypeDesc & 0x3fffffff);
	m_vtxSize = VertexSize(m_lastVtxType & 0xbfffffff);
	switch (m_lastVtxType & 0x7fffff7f)
	{
	case 0x12:
	case 0x42:
	case 0x44:
	case 0x52:
		SetTextureStageState(0, 2, 3);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 3);
		break;
	case 0x102:
		if (m_xbUseTFactor)
			SetTextureStageState(0, 2, 4);
		else
			SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x104:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 2);
		break;
	case 0x112:
	case 0x142:
	case 0x144:
	case 0x152:
	case 0x1106:
	case 0x1108:
	case 0x110a:
	case 0x110c:
	case 0x1116:
	case 0x1118:
	case 0x111a:
	case 0x111c:
	case 0x1156:
	case 0x1158:
	case 0x115a:
	case 0x115c:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 1);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x202:
	case 0x212:
	case 0x242:
	case 0x244:
	case 0x252:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 4);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x204:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 4);
		SetTextureStageState(0, 5, 2);
		break;
	case 0x40000202:
	case 0x40000212:
	case 0x40000242:
	case 0x40000244:
	case 0x40000252:
		SetTextureStageState(0, 2, 4);
		SetTextureStageState(1, 2, 7);
		SetTextureStageState(0, 5, 4);
		break;
	case 0x40000204:
		SetTextureStageState(0, 2, 2);
		SetTextureStageState(1, 2, 7);
		SetTextureStageState(0, 5, 2);
		break;
	}
}

inline void WDirect3D8::SetTexture(unsigned long iType)
{
	if (this->m_lastTexState != iType)
	{
		if ((iType & 0x7FF) != (this->m_lastTexState & 0x7FF))
		{
			this->xInstantiateAndFillTexture(static_cast<int>(iType & 0x7FF));
			IDirect3DTexture9* tex = this->m_texList[iType & 0x7FF].pTex;
			if (this->m_pTexture[0] != tex)
			{
				this->FlushRenderPrimitive();
				this->m_pTexture[0] = tex;
			}
			this->m_pd3dDevice->SetTexture(0, tex);
		}
		if ((iType >> 11 & 0x7F) != (this->m_lastTexState >> 11 & 0x7F))
		{
			this->xInstantiateAndFillTexture(
				static_cast<int>(iType >> 11 & 0x7F));
			IDirect3DTexture9* tex = this->m_texList[iType >> 11 & 0x7F].pTex;
			if (this->m_pTexture[1] != tex)
			{
				this->FlushRenderPrimitive();
				this->m_pTexture[1] = tex;
			}
			this->m_pd3dDevice->SetTexture(1, tex);
		}
		this->m_lastTexState = iType;
	}
}

inline void WDirect3D8::SetVtxType(unsigned long type, unsigned long type2,
	unsigned long vertexType, unsigned long diffuse)
{
	type |= this->m_GlobalRS[0];
	type2 |= this->m_GlobalRS[1];
	this->SetRenderState(type, type2);
	this->SetTexture(type & 0x3FFFF);
	this->m_xbCurHwTnL = type2 >> 22 != 0;
	this->xSetRenderState(type2, diffuse);
	this->xSetTnLBuffer(static_cast<int>(type2 >> 22),
		static_cast<int>(type2 >> 14 & 0xFF));
	if (this->m_xbCurHwTnL != this->m_xbLastHwTnL)
	{
		if (this->m_fog && type & 0x8000000)
		{
			this->_SetRenderState(D3DRS_SPECULARMATERIALSOURCE,
				this->m_xbCurHwTnL ? 0 : 2);
		}
		this->m_xbLastHwTnL = this->m_xbCurHwTnL;
	}
	this->SetVtxMode(vertexType | (type & 0x40000) << 12);

	unsigned long flags = 0;
	if (this->IsSupportVS() || this->IsSupportPS())
	{
		flags = this->m_CustomRenderState.GetFxMacro();
		if (type & 0x07FF)
		{
			flags |= 0x01;
		}
		if (!(type & 0x40000000) && vertexType & 0x40)
		{
			flags |= 0x04;
		}
		if ((vertexType & D3DFVF_POSITION_MASK) == 4)
		{
			flags |= 0x2000;
			switch (type2 & 0x60)
			{
			case 0x40:
				flags |= 0x1001;
				break;
			case 0x20:
				flags |= 0x0801;
				break;
			}
		}
		else
		{
			if (type & 0x3F800)
			{
				flags |= 0x0010;
			}
			if (type & 0x40000)
			{
				flags |= 0x0020;
			}
			if (type2 & 0x100 && !(type & 0x1800000))
			{
				flags |= 0x4000;
			}
			if ((type2 & 0x80u) != 0 && !(type & 0x1800000))
			{
				flags |= 0x8000;
			}
			if (this->m_LightEnable == 1 && this->m_xdwDiffuse != -1)
			{
				flags |= 0x0008;
			}
			if (type & 0x8000000)
			{
				flags |= 0x0080;
			}
			if (vertexType & 0x10)
			{
				flags |= 0x0002;
			}
			if (this->m_xbUseTFactor == 1)
			{
				flags |= 0x0040;
			}
			if (type2 & 0x04)
			{
				flags |= 0x0100;
			}
			if (type2 & 0x0200 && !(type2 & 0x04))
			{
				flags |= 0x0400;
			}
		}
		if (!this->IsSupportVS())
		{
			flags |= 0x20000000;
		}
		if (!this->IsSupportPS())
		{
			flags |= 0x40000000;
		}
	}
	this->_SetShader(flags);
}

bool WDirect3D8::SetFogEnable(bool enable)
{
	return WDirect3D::SetFogEnable(enable);
}

void WDirect3D8::SetFogState(float fogStart, float fogEnd,
	unsigned long fogColor)
{
	if (fogStart != m_fogStart)
		_SetRenderState(D3DRS_FOGSTART, *(DWORD*)&fogStart);
	if (fogEnd != m_fogEnd)
		_SetRenderState(D3DRS_FOGEND, *(DWORD*)&fogEnd);
	if (fogColor != m_fogColor)
		_SetRenderState(D3DRS_FOGCOLOR, fogColor);
	WDirect3D::SetFogState(fogStart, fogEnd, fogColor);
}

void WDirect3D8::ResetFogState()
{
	_SetRenderState(D3DRS_FOGSTART, *(DWORD*)&m_fogStart);
	_SetRenderState(D3DRS_FOGEND, *(DWORD*)&m_fogEnd);
	_SetRenderState(D3DRS_FOGCOLOR, m_fogColor);
}

_D3DFORMAT WDirect3D8::FindDepthBufferFormat(_D3DFORMAT format)
{
	for (int i = 0; i < sizeof(ms_RtDepthFmt) / sizeof(ms_RtDepthFmt[0]); ++i)
	{
		if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, format,
				D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
				ms_RtDepthFmt[i].fmt) == S_OK &&
			m_d3d8->CheckDepthStencilMatch(m_devId, D3DDEVTYPE_HAL, format,
				format, ms_RtDepthFmt[i].fmt) == S_OK)
			return ms_RtDepthFmt[i].fmt;
	}
	return D3DFMT_D16;
}

int WDirect3D8::Init(char* modName, HWND__* hWnd, int iTnL)
{
	Nv::Factory::ScreenCapeFactory* screenCapeFactory;
	Nv::IScreenCape* screenCape;
	D3DDISPLAYMODE d3ddm;
	D3DDISPLAYMODE t;
	char temp[64];

	D3DPERF_SetOptions(1);
	this->m_renderCount = 0;
	this->m_d3d8 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!this->m_d3d8)
	{
		g_error = g_msgD3DInitFailed;
		return 1;
	}
	this->m_d3d8->GetDeviceCaps(this->m_devId, D3DDEVTYPE_HAL,
		&this->m_d3dcaps);
	this->m_MaxTextureBlendStages = this->m_d3dcaps.MaxSimultaneousTextures;
	memset(&this->m_d3dpp, 0, sizeof this->m_d3dpp);
	if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) == 0)
	{
		this->m_fmtWindowed = d3ddm.Format;
	}
	if (strstr(modName, "Window"))
	{
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc\x20\xb8\xf0\xb5\xe5\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return 4;
		}
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		m_fps = !d3ddm.RefreshRate ? 120.0f : (float)d3ddm.RefreshRate;
		this->m_bWindow = true;
		this->m_BackBufBpp = this->GetBackBufferBpp(d3ddm.Format);
		if (!this->m_BackBufBpp)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc\x20\xb8\xf0\xb5\xe5\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return 4;
		}
		this->m_d3dpp.Windowed = 1;
		this->m_d3dpp.BackBufferWidth = clientRect.right - clientRect.left;
		this->m_d3dpp.BackBufferHeight = clientRect.bottom - clientRect.top;
		this->m_d3dpp.BackBufferFormat = d3ddm.Format;
		this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		this->m_d3dpp.FullScreen_RefreshRateInHz = 0;
		this->m_d3dpp.PresentationInterval = 0;
	}
	else
	{
		D3DFORMAT _list[3] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5,
			D3DFMT_X8R8G8B8 };
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		for (unsigned j = 0; j < 3; ++j)
		{
			if (m_d3d8->GetAdapterModeCount(0, _list[j]))
			{
				fmt = _list[j];
				break;
			}
		}
		unsigned i = 0;
		unsigned m = -1;
		for (; i < m_d3d8->GetAdapterModeCount(m_devId, fmt); ++i)
		{
			m_d3d8->EnumAdapterModes(m_devId, fmt, i, &d3ddm);
			int j;
			for (j = 4; j >= 0; --j)
				if (ms_fmtList[j].format == d3ddm.Format)
					break;
			if (j >= 0)
			{
				sprintf(temp, "w%d h%d b%d", d3ddm.Width, d3ddm.Height,
					ms_fmtList[j].bitNum);
				if (!strcmp(modName, temp) &&
					(m == -1 ||
						(d3ddm.RefreshRate != 60 && !d3ddm.RefreshRate)))
				{
					t = d3ddm;
					m = i;
				}
			}
		}
		if (m == -1)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb8\xae\xc7\xc1\xb7\xb9\xbd\xc3\x20\xb7\xb9\xc0\xcc\xc6\xae\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return 2;
		}
		m_fps = !t.RefreshRate ? 120.0f : (float)t.RefreshRate;
		this->m_bWindow = false;
		this->m_BackBufBpp = GetBackBufferBpp(t.Format);
		if (!this->m_BackBufBpp)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc\x20\xb8\xf0\xb5\xe5\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return 4;
		}
		this->m_d3dpp.Windowed = 0;
		this->m_d3dpp.BackBufferWidth = t.Width;
		this->m_d3dpp.BackBufferHeight = t.Height;
		this->m_d3dpp.BackBufferFormat = t.Format;
		this->m_d3dpp.FullScreen_RefreshRateInHz = t.RefreshRate;
		this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		this->m_d3dpp.PresentationInterval = 0;
	}

	this->m_d3dpp.hDeviceWindow = hWnd;
	this->m_d3dpp.BackBufferCount = 1;
	this->m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	this->m_d3dpp.EnableAutoDepthStencil = TRUE;
	this->m_d3dpp.AutoDepthStencilFormat =
		this->FindDepthBufferFormat(this->m_d3dpp.BackBufferFormat);
	this->m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	this->m_hwnd = hWnd;
	this->m_lWndStyle = ~0;

	bool isSupportHwVs = IsSupportVS();
	bool isSupportHwPs = IsSupportPS();

	if (this->m_d3dcaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION &&
		this->m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		this->m_xdwDevBehavior =
			D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	}
	else
	{
		this->m_xdwDevBehavior =
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	}

	if (this->m_d3d8->CreateDevice(this->m_devId, D3DDEVTYPE_HAL, hWnd,
			this->m_xdwDevBehavior, &this->m_d3dpp,
			&this->m_pd3dDevice) != S_OK)
	{
		g_error =
			"\xb5\xf0\xb9\xd9\xc0\xcc\xbd\xba\xb8\xa6\x20\xbb\xfd\xbc\xba\xc7\xd2\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
		return 5;
	}

	if (this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, 0) >= 0)
	{
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT,
			&this->m_pEventQuery);
	}

	this->m_pd3dDevice->GetDeviceCaps(&this->m_d3dcaps);
	if (!isSupportHwVs || !isSupportHwPs)
	{
		this->m_d3dcaps.VertexShaderVersion = 0xFFFE0000;
		this->m_d3dcaps.PixelShaderVersion = 0xFFFF0000;
	}

	if (!IsSupportVS())
		IsSupportPS();
	m_pScreenCapeFactory = new Nv::Factory::ScreenCapeFactory;
	m_pScreenCape = m_pScreenCapeFactory->CreateScreenCapeLayer();
	if (m_pScreenCape)
		m_pScreenCape->Initialize(hWnd, (void**)&m_pd3dDevice,
			Nv::GraphicDeviceType::Direct3D9);
	this->m_pd3dDevice->ShowCursor(TRUE);
	this->SetTextureFormat(this->m_d3dpp.BackBufferFormat);
	this->SetRenderTargetFormat();
	this->SetDefaultState();
	this->m_pd3dDevice->BeginScene();
	this->m_pd3dDevice->Clear(0, 0, 3u, 0, 0.0, 0);
	this->m_pd3dDevice->EndScene();
	this->Present();
	WDirect3D::Init();

	if (this->m_pCopiedScreenSplash)
	{
		delete this->m_pCopiedScreenSplash;
		this->m_pCopiedScreenSplash = 0;
	}

	this->m_pd3dDevice->SetVertexShader(0);
	this->m_pd3dDevice->SetPixelShader(0);
	this->BackupMainRenderTarget();

	m_curRt = m_mainRt;

	m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(144000);
	m_MergeBuffer.CheckAndIncreaseIndexBuffer(18000);

	return 0;
}

int WDirect3D8::GetBackBufferBpp(_D3DFORMAT format)
{
	int i;
	for (i = sizeof(ms_fmtList) / sizeof(ms_fmtList[0]) - 1; i >= 0; --i)
	{
		if (ms_fmtList[i].format == format)
			break;
	}
	if (i >= 0)
		return ms_fmtList[i].bitNum;
	return 0;
}

void WDirect3D8::SetDefaultState()
{
	static struct
	{
		D3DRENDERSTATETYPE state;
		unsigned long value;
	} def_render_state[] = {
		{ D3DRS_ZENABLE,                  1  },
		{ D3DRS_FILLMODE,                 3  },
		{ D3DRS_SHADEMODE,                2  },
		{ D3DRS_ZWRITEENABLE,             1  },
		{ D3DRS_ALPHATESTENABLE,          1  },
		{ D3DRS_LASTPIXEL,                1  },
		{ D3DRS_ZFUNC,                    4  },
		{ D3DRS_ALPHAREF,                 0  },
		{ D3DRS_ALPHAFUNC,                5  },
		{ D3DRS_DITHERENABLE,             1  },
		{ D3DRS_ALPHABLENDENABLE,         0  },
		{ D3DRS_FOGENABLE,                0  },
		{ D3DRS_SPECULARENABLE,           0  },
		{ D3DRS_FOGTABLEMODE,             0  },
		{ D3DRS_DEPTHBIAS,                0  },
		{ D3DRS_RANGEFOGENABLE,           0  },
		{ D3DRS_STENCILENABLE,            0  },
		{ D3DRS_TEXTUREFACTOR,            0  },
		{ D3DRS_CULLMODE,                 1  },
		{ D3DRS_CLIPPING,                 1  },
		{ D3DRS_LIGHTING,                 0  },
		{ D3DRS_AMBIENT,                  0  },
		{ D3DRS_FOGVERTEXMODE,            3  },
		{ D3DRS_BLENDOP,                  1  },
		{ D3DRS_INDEXEDVERTEXBLENDENABLE, 0  },
		{ D3DRS_VERTEXBLEND,              0  },
		{ D3DRS_NORMALIZENORMALS,         0  },
		{ D3DRS_COLORVERTEX,              1  },
		{ D3DRS_SPECULARMATERIALSOURCE,   2  },
		{ D3DRS_COLORWRITEENABLE,         15 },
	};

	static struct
	{
		unsigned long stage;
		D3DTEXTURESTAGESTATETYPE type;
		unsigned long value;
	} def_texture_state[12] = {
		{ 0, D3DTSS_TEXCOORDINDEX, 0 },
		{ 1, D3DTSS_TEXCOORDINDEX, 1 },
		{ 0, D3DTSS_COLORARG1,     2 },
		{ 0, D3DTSS_COLORARG2,     0 },
		{ 1, D3DTSS_COLORARG1,     2 },
		{ 1, D3DTSS_COLORARG2,     1 },
		{ 0, D3DTSS_ALPHAARG1,     2 },
		{ 0, D3DTSS_ALPHAARG2,     0 },
		{ 0, D3DTSS_ALPHAOP,       4 },
		{ 1, D3DTSS_ALPHAARG1,     1 },
		{ 1, D3DTSS_ALPHAARG2,     2 },
		{ 1, D3DTSS_ALPHAOP,       2 },
	};

	static struct
	{
		unsigned long stage;
		D3DSAMPLERSTATETYPE type;
		unsigned long value;
	} def_sampler_state[10] = {
		{ 0, D3DSAMP_ADDRESSU,  1 },
		{ 0, D3DSAMP_ADDRESSV,  1 },
		{ 1, D3DSAMP_ADDRESSU,  3 },
		{ 1, D3DSAMP_ADDRESSV,  3 },
		{ 0, D3DSAMP_MINFILTER, 2 },
		{ 0, D3DSAMP_MAGFILTER, 2 },
		{ 0, D3DSAMP_MIPFILTER, 1 },
		{ 1, D3DSAMP_MINFILTER, 2 },
		{ 1, D3DSAMP_MAGFILTER, 2 },
		{ 1, D3DSAMP_MIPFILTER, 1 },
	};

	IDirect3DBaseTexture9* surf;
	DWORD value;
	int n;

	for (unsigned int i = 0;
		i < sizeof(def_render_state) / sizeof(def_render_state[0]); ++i)
	{
		this->m_pd3dDevice->GetRenderState(def_render_state[i].state, &value);
		if (value == def_render_state[i].value)
		{
			continue;
		}
		this->m_pd3dDevice->SetRenderState(def_render_state[i].state,
			def_render_state[i].value);
	}

	for (unsigned int i = 0;
		i < sizeof(def_texture_state) / sizeof(def_texture_state[0]); ++i)
	{
		this->m_pd3dDevice->GetTextureStageState(def_texture_state[i].stage,
			def_texture_state[i].type, &value);
		if (value == def_texture_state[i].value)
		{
			continue;
		}
		_SetTextureStageState(def_texture_state[i].stage,
			static_cast<D3DTEXTURESTAGESTATETYPE>(def_texture_state[i].type),
			def_texture_state[i].value);
	}

	for (unsigned int i = 0;
		i < sizeof(def_sampler_state) / sizeof(def_sampler_state[0]); ++i)
	{
		this->m_pd3dDevice->GetSamplerState(def_sampler_state[i].stage,
			def_sampler_state[i].type, &value);
		if (value == def_sampler_state[i].value)
		{
			continue;
		}
		_SetSamplerState(def_sampler_state[i].stage, def_sampler_state[i].type,
			def_sampler_state[i].value);
	}

	for (int i = 0; i < 2; i++)
	{
		m_pTexture[i] = 0;
		this->m_pd3dDevice->GetTexture(i, &surf);
		if (!surf)
		{
			continue;
		}
		_SetTexture(i, 0);
		surf->Release();
	}

	this->m_pd3dDevice->GetRenderState(D3DRS_SRCBLEND,
		&this->m_lastBlendState[0]);
	this->m_pd3dDevice->GetRenderState(D3DRS_DESTBLEND,
		&this->m_lastBlendState[1]);
	this->m_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,
		&this->m_blendEnable);

	for (n = 0; n < 2; ++n)
	{
		for (unsigned int j = 0;
			j < sizeof(g_TexStageStateList) / sizeof(g_TexStageStateList[0]);
			++j)
			m_pd3dDevice->GetTextureStageState(n, g_TexStageStateList[j],
				&m_texStageState[n][j]);
	}

	this->m_lastBlendMode = -1;
	this->m_lastVtxType = -1;
	this->m_xLastDrawType = 1028;
	this->m_xLastRenderState = 1028;
	this->m_xLastVertexDecl = -1;
	this->m_lastTexState = 0;
	this->m_lastRenderState = 0;
	this->m_fog = false;
	this->m_xhLastVb = 0;
	this->m_xhLastIb = 0;
	this->m_xdwLastUsage = 0;
	this->m_xdwLastFVF = 0;
	this->m_xbLastHwTnL = false;
	this->m_xbUseTFactor = false;
	this->m_xbLastVertexBlend = false;
	this->m_lastEffect = 0;
	memset(this->m_xaLights, 0, sizeof this->m_xaLights);
	memset(this->m_xaWLights, 0, sizeof this->m_xaWLights);
	memset(&this->m_xMaterial, 0, sizeof this->m_xMaterial);
	this->m_xMaterial.Diffuse.r = 1.0;
	this->m_xMaterial.Diffuse.b = 1.0;
	this->m_xMaterial.Diffuse.g = 1.0;
	this->m_xMaterial.Diffuse.a = 1.0;
	this->m_xMaterial.Ambient.b = 1.0;
	this->m_xMaterial.Ambient.g = 1.0;
	this->m_xMaterial.Ambient.r = 1.0;
	this->m_xMaterial.Ambient.a = 1.0;
	this->m_xdwDiffuse = -1;
	this->m_xdwTFactor = -1;
	this->m_LightEnable = false;
	for (int iIndex = 0; iIndex < 256; ++iIndex)
	{
		D3DXMATRIX& i = this->m_xLastWorldMatrix[iIndex];
		D3DXMatrixIdentity(&i);
	}
	D3DXMatrixIdentity(&this->m_xLastViewMatrix);
	D3DXMatrixIdentity(&this->m_xLastProjMatrix);
	D3DXMatrixIdentity(&this->m_xPrevViewMatrix);
	this->m_LastTFactor = -1;
	this->ResetFogState();
	this->m_pd3dDevice->SetMaterial(&this->m_xMaterial);
	this->m_pd3dDevice->LightEnable(0, 1);
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);
	this->FlushRenderPrimitive();
	this->m_LastTFactor = -1;
	this->m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, -1);
}

WDirect3D8::pix_info* WDirect3D8::FindPixInfoByFormat(D3DFORMAT fmt)
{
	for (int i = 0; i < sizeof(ms_fmtTypeList) / sizeof(ms_fmtTypeList[0]); ++i)
	{
		if (ms_fmtTypeList[i].pixFmt == fmt)
			return &ms_fmtTypeList[i];
	}
	return 0;
}

int WDirect3D8::SetTextureFormat(D3DFORMAT format)
{
	int result = 0;
	for (int i = 0; i < 6; ++i)
	{
		int j;
		for (j = 0; j < 4; ++j)
		{
			D3DFORMAT fmt = ms_fmtRecomList[i][j];
			if (m_d3d8->CheckDeviceFormat(m_devId, D3DDEVTYPE_HAL, format, 0,
					D3DRTYPE_TEXTURE, fmt) == S_OK)
			{
				pix_info* pix = FindPixInfoByFormat(fmt);
				if (pix)
					m_fmt[i] = pix;
				break;
			}
		}
		if (j == 4)
			result |= 1;
	}
	return result;
}

HRESULT WDirect3D8::Present()
{
	if (m_pScreenCape)
		m_pScreenCape->Render();
	++m_renderCount;
	HRESULT result = m_pd3dDevice->Present(0, 0, m_hwnd, 0);
	if (FAILED(result))
		return result;
	if (m_pEventQuery)
	{
		m_pEventQuery->Issue(D3DISSUE_END);
		while (m_pEventQuery->GetData(0, 0, D3DGETDATA_FLUSH) == S_FALSE)
		{
		}
	}
	else
	{
		IDirect3DSurface9* pRenderTarget;
		if (SUCCEEDED(m_pd3dDevice->GetRenderTarget(0, &pRenderTarget)))
		{
			RECT rc;
			D3DLOCKED_RECT lr;
			SetRect(&rc, 0, 0, 1, 1);
			pRenderTarget->LockRect(&lr, &rc, D3DLOCK_READONLY);
			pRenderTarget->UnlockRect();
			pRenderTarget->Release();
		}
	}
	return result;
}

WDirect3D8* WDirect3D8::CreateClone(char* devName, int id)
{
	return new WDirect3D8(devName, id);
}

WVideoDev* WDirect3D8::MakeClone(char* modeName, HWND hWnd, int iTnL)
{
	if (m_devId >= 0)
	{
		WDirect3D8* clone = CreateClone(m_devName, m_devId);
		if (!clone->Init(modeName, hWnd, iTnL))
			return clone;
		clone->Release();
		delete clone;
	}
	return 0;
}

void WDirect3D8::Clear(unsigned long color, int flags, float z)
{
	static int count = 0;

	if (this->m_devState == W_VDEVSTATE_LOST)
		return;

	if (this->m_useCopiedScreen)
	{
		if (g_captureOption.useMemCopy || !g_captureOption.dxCopyRectsOK)
		{
			IDirect3DSurface9* surface = 0;
			if (g_captureOption.useTexture)
			{
				IDirect3DTexture9* texture =
					this->m_texList[this->m_hCopiedScreenTexture].pTex;
				texture->GetSurfaceLevel(0, &surface);
			}
			else
			{
				surface = this->m_pCopiedScreenSurface;
			}
			IDirect3DSurface9* rt;
			if (this->m_pd3dDevice->GetRenderTarget(0, &rt) == S_OK)
			{
				RECT rc;
				if (g_captureOption.updateWholeScreen || !count)
				{
					rc.left = 0;
					rc.top = 0;
					rc.right = this->m_capturedDdsd.Width;
					rc.bottom = this->m_capturedDdsd.Height;
				}
				else
				{
					rc.left = 118;
					rc.top = this->m_capturedDdsd.Height - 128;
					rc.right = 523;
					rc.bottom = this->m_capturedDdsd.Height - 68;
				}
				D3DXLoadSurfaceFromSurface(rt, 0, &rc, surface, 0, &rc, 1, 0);
				rt->Release();
			}
			if (g_captureOption.useTexture)
				surface->Release();
		}
		else if (g_captureOption.useTexture)
		{
			float l, t, r, b;
			struct VERTEX
			{
				VERTEX() { }
				VERTEX(float _x, float _y, float _z, float _w, float _u,
					float _v)
					: x(_x), y(_y), z(_z), w(_w), u(_u), v(_v)
				{
				}
				float x, y, z, w, u, v;
			};
			float uvl, uvt, uvr, uvb;
			VERTEX rect[4];

			if (g_captureOption.updateWholeScreen || !count)
			{
				l = -0.5f;
				r = this->m_capturedDdsd.Width - 0.5f;
				t = -0.5f;
				b = this->m_capturedDdsd.Height - 0.5f;
				uvr = 1.0f;
				uvl = 0.0f;
				uvt = 0.0f;
				uvb = 1.0f;
			}
			else
			{
				l = 117.5f;
				r = 522.5f;
				t = static_cast<float>(this->m_capturedDdsd.Height - 128) -
					0.5f;
				b = static_cast<float>(this->m_capturedDdsd.Height - 68) - 0.5f;
				float invWidth = 1.0f / this->m_capturedDdsd.Width;
				uvl = invWidth * 118.0f;
				uvr = invWidth * 523.0f;
				uvt = static_cast<float>(this->m_capturedDdsd.Height - 128) /
					this->m_capturedDdsd.Height;
				uvb = static_cast<float>(this->m_capturedDdsd.Height - 68) /
					this->m_capturedDdsd.Height;
			}
			rect[0] = VERTEX(l, b, 0.001f, 1.0f, uvl, uvb);
			rect[1] = VERTEX(l, t, 0.001f, 1.0f, uvl, uvt);
			rect[2] = VERTEX(r, b, 0.001f, 1.0f, uvr, uvb);
			rect[3] = VERTEX(r, t, 0.001f, 1.0f, uvr, uvt);
			this->DrawPrimitive(0x60300000 |
					(this->m_hCopiedScreenTexture & 0x7FF),
				4, 0x104, rect, D3DPT_TRIANGLESTRIP, 1028);
		}
		else
		{
			IDirect3DSurface9* rt;
			if (this->m_pd3dDevice->GetRenderTarget(0, &rt) == S_OK)
			{
				if (g_captureOption.updateWholeScreen || !count)
				{
					fnWD3DDevice_CopyRect(this->m_pd3dDevice,
						this->m_pCopiedScreenSurface, 0, rt);
				}
				else
				{
					RECT rc;
					rc.left = 118;
					rc.right = 523;
					rc.top = this->m_capturedDdsd.Height - 128;
					rc.bottom = this->m_capturedDdsd.Height - 68;
					fnWD3DDevice_CopyRect(this->m_pd3dDevice,
						this->m_pCopiedScreenSurface, &rc, rt);
				}
				rt->Release();
			}
		}
		if (!g_captureOption.updateWholeScreen)
		{
			++count;
		}
	}
	else
	{
		unsigned long clearFlags = 0;
		count = 0;
		if (flags & 1)
		{
			clearFlags |= 1;
		}
		if (flags & 2)
		{
			clearFlags |= 2;
		}
		this->m_pd3dDevice->Clear(0, 0, clearFlags, color, z, 0);
	}
}

bool WDirect3D8::BeginScene()
{
	HRESULT hResult = this->m_pd3dDevice->TestCooperativeLevel();
	if (FAILED(hResult))
	{
		this->m_devState = W_VDEVSTATE_LOST;

		if (hResult == D3DERR_DEVICELOST || hResult != D3DERR_DEVICENOTRESET)
		{
			return false;
		}

		if (!this->xReset(false))
		{
			return false;
		}
	}
	if (this->m_d3dpp.Windowed && !this->m_fillScrMode &&
		!this->m_clientRcCheckedAfterReset)
	{
		this->m_clientRcCheckedAfterReset = true;
		RECT rcClient;
		GetClientRect(this->m_hwnd, &rcClient);
		if (rcClient.right - rcClient.left != this->m_d3dpp.BackBufferWidth ||
			rcClient.bottom - rcClient.top != this->m_d3dpp.BackBufferHeight)
		{
			SetRect(&rcClient, 0, 0, this->m_d3dpp.BackBufferWidth,
				this->m_d3dpp.BackBufferHeight);
			AdjustWindowRectEx(&rcClient,
				GetWindowLongA(this->m_hwnd, GWL_STYLE), 0, 0);
			if (rcClient.right - rcClient.left <=
					GetSystemMetrics(SM_CXFULLSCREEN) &&
				rcClient.bottom - rcClient.top <=
					GetSystemMetrics(SM_CYFULLSCREEN))
			{
				this->m_devState = W_VDEVSTATE_LOST;
				if (!this->xReset(false))
				{
					return false;
				}
			}
		}
	}

	this->m_devState = W_VDEVSTATE_NORMAL;
	this->m_xhLastIb = 0;
	this->m_xhLastVb = 0;
	this->m_xnDIPs = 0;
	this->m_xnDPs = 0;
	this->m_xnDIPUPs = 0;
	this->m_xnDPUPs = 0;
	this->m_xnTotalTris = 0;
	this->m_pd3dDevice->BeginScene();
	this->m_MergeBuffer.primitiveCount = 0;

	return true;
}

void WDirect3D8::EndScene()
{
	this->Flush(0);
	this->FlushRenderPrimitive();
	if (this->m_devState != W_VDEVSTATE_LOST)
	{
		this->m_Effect = NULL;
		this->ApplyShader();
		this->m_pd3dDevice->EndScene();
	}
}

bool WDirect3D8::xReset(bool switchWinMode)
{
	this->m_renderCount = 0;
	this->xReset_ReleaseResource();

	std::map<unsigned long, sEffect>::iterator it;
	for (it = this->m_EffectTable.begin(); it != this->m_EffectTable.end();
		it++)
	{
		if ((*it).second.pEffect)
		{
			(*it).second.pEffect->OnLostDevice();
		}
	}

	if (this->m_pEventQuery)
	{
		this->m_pEventQuery->Release();
		this->m_pEventQuery = 0;
	}

	if (FAILED(this->m_pd3dDevice->Reset(&this->m_d3dpp)))
	{
		D3DDISPLAYMODE d3ddm;
		if (!this->m_d3dpp.Windowed)
		{
			return false;
		}
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1 \xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc \xb8\xf0\xb5\xe5\xb8\xa6 \xbe\xf2\xbe\xee \xbf\xc3 \xbc\xf6 \xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return false;
		}
		if (d3ddm.Format == this->m_d3dpp.BackBufferFormat)
		{
			return false;
		}
		this->m_d3dpp.BackBufferFormat = d3ddm.Format;
		this->m_d3dpp.AutoDepthStencilFormat =
			this->FindDepthBufferFormat(d3ddm.Format);
		if (FAILED(this->m_pd3dDevice->Reset(&this->m_d3dpp)))
		{
			return false;
		}
	}
	this->m_devState = W_VDEVSTATE_NORMAL;
	this->m_clientRcCheckedAfterReset = false;
	if (SUCCEEDED(this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, 0)))
	{
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT,
			&this->m_pEventQuery);
	}
	this->SetTextureFormat(this->m_d3dpp.BackBufferFormat);
	this->SetRenderTargetFormat();
	this->SetDefaultState();
	if (this->m_d3dpp.Windowed)
	{
		if (this->m_lWndStyle != -1)
		{
			SetWindowLongA(this->m_hwnd, GWL_STYLE,
				this->m_lWndStyle | WS_VISIBLE);
		}
		RECT rc, old_rc;
		int windowWidth = this->m_d3dpp.BackBufferWidth;
		int windowHeight = this->m_d3dpp.BackBufferHeight;
		if (this->m_fillScrMode)
		{
			windowWidth = GetSystemMetrics(SM_CXSCREEN);
			windowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		SetRect(&rc, 0, 0, windowWidth, windowHeight);
		GetClientRect(this->m_hwnd, &old_rc);
		if (switchWinMode || old_rc.right - old_rc.left != windowWidth ||
			old_rc.bottom - old_rc.top != windowHeight)
		{
			AdjustWindowRectEx(&rc, GetWindowLongA(this->m_hwnd, GWL_STYLE), 0,
				0);
			int dx = (rc.left - rc.right + GetSystemMetrics(SM_CXSCREEN)) / 2;
			int dy = (rc.top - rc.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2;
			OffsetRect(&rc, dx, dy);
			SetWindowPos(this->m_hwnd, HWND_NOTOPMOST, dx, dy,
				rc.right - rc.left, rc.bottom - rc.top, 0x40u);
			UpdateWindow(this->m_hwnd);
			g_formatChanged = false;
		}
		if (this->m_pkDWMApiDll && this->m_fillScrMode)
		{
			if (static_cast<float>(m_d3dpp.BackBufferWidth) /
						m_d3dpp.BackBufferHeight <
					1.4f &&
				static_cast<float>(windowWidth) / windowHeight > 1.4f)
			{
				if (!m_pkDWMApiDll->IsCompositionEnabled())
					m_pkDWMApiDll->EnableComposition(true);
			}
			else if (m_pkDWMApiDll->IsCompositionEnabled())
				m_pkDWMApiDll->EnableComposition(false);
		}
	}
	this->xReset_CreateResource();
	if (this->m_pCopiedScreenSplash)
	{
		this->m_pCopiedScreenSplash->Reset();
	}
	this->m_pd3dDevice->GetRenderTarget(0, &this->m_mainRt.rt.surf[0]);
	this->m_mainRt.rt.surf[1] = 0;
	this->m_pd3dDevice->GetDepthStencilSurface(&this->m_mainRt.depthSurf);
	this->m_pd3dDevice->GetViewport(&this->m_mainRt.viewport);
	memcpy(&this->m_curRt, &this->m_mainRt, sizeof this->m_curRt);

	for (it = this->m_EffectTable.begin(); it != this->m_EffectTable.end();
		it++)
	{
		if ((*it).second.pEffect)
		{
			(*it).second.pEffect->OnResetDevice();
		}
	}

	return true;
}

void WDirect3D8::xReset_ReleaseResource()
{
	for (int vbIndex = 0; vbIndex < 1024; ++vbIndex)
	{
		sVb8& vb = this->m_xaVbList[vbIndex];
		if ((vb.xdwUsage & 0x200) == 0)
		{
			continue;
		}

		vb.xbNeedToBeFilled = true;

		if (!vb.xpVb)
		{
			continue;
		}

		vb.xpVb->Release();
		vb.xpVb = 0;
	}

	for (int ibIndex = 0; ibIndex < 256; ++ibIndex)
	{
		if ((m_xaIbList[ibIndex].xdwUsage & 0x200) == 0)
		{
			continue;
		}

		m_xaVbList[ibIndex].xbNeedToBeFilled = true;

		if (!m_xaIbList[ibIndex].xpIb)
		{
			continue;
		}

		m_xaIbList[ibIndex].xpIb->Release();
		m_xaIbList[ibIndex].xpIb = 0;
	}

	for (int iIndex = 0; iIndex < 2048; ++iIndex)
	{
		d3d8_texture& i = this->m_texList[iIndex];
		if (!i.pSurf)
		{
			continue;
		}

		i.isFilled = false;
		i.pSurf->Release();
		i.pSurf = 0;

		if (!i.pTex)
		{
			continue;
		}

		i.pTex->Release();
		i.pTex = 0;
	}

	if (this->m_commonDepthSurf)
	{
		this->m_commonDepthSurf->Release();
		this->m_commonDepthSurf = 0;
	}

	for (int iIndex = 0; iIndex < 32; ++iIndex)
	{
		sDepthSurf& i = this->m_depthSurfList[iIndex];
		if (!i.pDepth)
		{
			continue;
		}

		i.pDepth->Release();
		i.pDepth = 0;
	}
	memset(this->m_depthSurfList, 0, sizeof this->m_depthSurfList);

	if (this->m_pCopiedScreenSurface)
	{
		this->m_pCopiedScreenSurface->Release();
		this->m_pCopiedScreenSurface = 0;
	}

	if (this->m_hCopiedScreenTexture > 0)
	{
		this->DestroyTexture(this->m_hCopiedScreenTexture);
		this->m_hCopiedScreenTexture = 0;
	}

	this->m_useCopiedScreen = false;
	if (this->m_pLockableVB)
	{
		this->m_pLockableVB->Release();
		this->m_pLockableVB = 0;
	}

	if (this->m_pLockableIB)
	{
		this->m_pLockableIB->Release();
		this->m_pLockableIB = 0;
	}

	this->ReleaseAllRendertargetBackupResource();
}

void WDirect3D8::xReset_CreateResource()
{
	unsigned long width, height;

	for (int iIndex = 0; iIndex < 1024; ++iIndex)
	{
		sVb8& i = this->m_xaVbList[iIndex];
		if (i.xdwUsage & 0x200)
		{
			this->m_pd3dDevice->CreateVertexBuffer(i.xnVtxs * i.xbStride,
				i.xdwUsage, i.xdwFVF, D3DPOOL_DEFAULT, &i.xpVb, 0);
		}
	}

	for (int iIndex = 0; iIndex < 256; ++iIndex)
	{
		sIb8& i = this->m_xaIbList[iIndex];
		if (i.xdwUsage & 0x200)
		{
			this->m_pd3dDevice->CreateIndexBuffer(i.xnIdxs * 2, i.xdwUsage,
				D3DFMT_INDEX16, D3DPOOL_DEFAULT, &i.xpIb, 0);
		}
	}

	for (unsigned int i = 0; i < 2048; i++)
	{
		if (!this->m_texList[i].renderTargetType)
		{
			continue;
		}

		WRenderToTextureSizeInfo sizeInfo = m_texList[i].renderTargetSizeInfo;

		if (sizeInfo.m_isAbsolute)
		{
			width = this->m_texList[i].width;
			height = this->m_texList[i].height;
		}
		else
		{
			width = static_cast<int>(
				floor(this->GetWidth() * sizeInfo.m_width + 0.5f));
			height = static_cast<int>(
				floor(this->GetHeight() * sizeInfo.m_height + 0.5f));
		}

		this->CreateTextureSurface(width, height,
			this->m_texList[i].mipmaplevel, this->m_texList[i].renderTargetType,
			i, 0, 32);
		this->SetRenderTargetSizeInfo(i, sizeInfo);
	}
}

void WDirect3D8::CreateEventQuery()
{
	if (this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, 0) >= 0)
	{
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT,
			&this->m_pEventQuery);
	}
}

void WDirect3D8::ReleaseEventQuery()
{
	if (this->m_pEventQuery)
	{
		this->m_pEventQuery->Release();
		this->m_pEventQuery = 0;
	}
}

void WDirect3D8::xInstantiateAndFillTexture(int m_hTex)
{
	if (m_hTex)
	{
		if (!m_texList[m_hTex].pTex)
			xInstantiateTexture(m_hTex);
		if (m_texList[m_hTex].needToBeFilled)
			xFillTexture(m_hTex);
	}
}

void WDirect3D8::xInstantiateTexture(int m_hTex)
{
	d3d8_texture& tex = m_texList[m_hTex];
	DWORD mipLevels;
	D3DSURFACE_DESC topLvlD3dsd;
	if (tex.pixFmtInfo)
	{
		m_pd3dDevice->CreateTexture(tex.width, tex.height, tex.mipmaplevel, 0,
			tex.pixFmtInfo->pixFmt, D3DPOOL_MANAGED, &tex.pTex, 0);
		return;
	}
	if ((unsigned int)m_iDDSRes <= 0)
	{
		mipLevels = m_bUseMipmap ? m_iMaxMipLvl : 1;
		D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, tex.dxtcData,
			tex.dxtcDataSize, -1, -1, mipLevels, 0, D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED, 1, m_dwMipCreateFilter, 0, 0, 0, &tex.pTex);
		if (!tex.pTex)
			return;
		tex.pTex->GetLevelDesc(0, &topLvlD3dsd);
		mipLevels = tex.pTex->GetLevelCount();
	}
	else
	{
		IDirect3DTexture9* pTex = 0;
		mipLevels = m_bUseMipmap ? m_iDDSRes + m_iMaxMipLvl : m_iDDSRes + 1;
		D3DXCreateTextureFromFileInMemoryEx(m_pd3dDevice, tex.dxtcData,
			tex.dxtcDataSize, -1, -1, mipLevels, 0, D3DFMT_UNKNOWN,
			D3DPOOL_MANAGED, 1, m_dwMipCreateFilter, 0, 0, 0, &pTex);
		if (!pTex)
			return;
		UINT srcMipLevels = pTex->GetLevelCount();
		UINT iTopSrcLvlToLoad =
			m_iDDSRes + 1 > srcMipLevels ? srcMipLevels - 1 : m_iDDSRes;
		mipLevels = m_bUseMipmap ? srcMipLevels - iTopSrcLvlToLoad : 1;
		pTex->GetLevelDesc(iTopSrcLvlToLoad, &topLvlD3dsd);
		D3DXCreateTexture(m_pd3dDevice, topLvlD3dsd.Width, topLvlD3dsd.Height,
			mipLevels, 0, topLvlD3dsd.Format, D3DPOOL_MANAGED, &tex.pTex);
		if (!tex.pTex)
		{
			pTex->Release();
			return;
		}
		for (UINT i = 0; i < mipLevels; ++i)
		{
			IDirect3DSurface9* srcSurf;
			IDirect3DSurface9* dstSurf;
			if (pTex->GetSurfaceLevel(i + iTopSrcLvlToLoad, &srcSurf) < 0)
				continue;
			if (tex.pTex->GetSurfaceLevel(i, &dstSurf) >= 0)
			{
				D3DXLoadSurfaceFromSurface(dstSurf, 0, 0, srcSurf, 0, 0, 1, 0);
				dstSurf->Release();
			}
			srcSurf->Release();
		}
		pTex->Release();
	}
	tex.width = topLvlD3dsd.Width;
	tex.height = topLvlD3dsd.Height;
	tex.mipmaplevel = mipLevels;
	tex.dxtcDataSize = 0;
	if (tex.dxtcData)
	{
		delete[] tex.dxtcData;
		tex.dxtcData = 0;
	}
}

void WDirect3D8::xFillTexture(int m_hTex)
{
	D3DLOCKED_RECT rect;

	d3d8_texture& texture = this->m_texList[m_hTex];
	IDirect3DTexture9* pTex = texture.pTex;
	if (pTex)
	{
		if (texture.bitmap)
		{
			for (int mip = 0; mip < texture.mipmaplevel; ++mip)
			{
				if (pTex->LockRect(mip, &rect, 0, 0) == S_OK)
				{
					this->UpdateTextureSurface(rect.pBits, texture.width >> mip,
						texture.height >> mip, rect.Pitch, texture.pixFmtInfo,
						texture.bitmap->bi, texture.bitmap->vram,
						texture.updateType);
					pTex->UnlockRect(mip);
				}
			}
		}
		texture.needToBeFilled = false;
		texture.isFilled = true;
	}
}

void WDirect3D8::xInstantiateVertexBuffer(int hVb)
{
	D3DPOOL m_pool;
	if (m_xaVbList[hVb].xdwUsage & 0x200)
		m_pool = D3DPOOL_DEFAULT;
	else
		m_pool = D3DPOOL_MANAGED;
	fnWD3DDevice_CreateVertexBuffer(m_pd3dDevice,
		m_xaVbList[hVb].xnVtxs * m_xaVbList[hVb].xbStride,
		m_xaVbList[hVb].xdwUsage, m_xaVbList[hVb].xdwFVF, m_pool,
		&m_xaVbList[hVb].xpVb, 0);
	this->xFillVertexBuffer(hVb);
	this->m_xiVbSize += m_xaVbList[hVb].xnVtxs * m_xaVbList[hVb].xbStride;
}

void WDirect3D8::xInstantiateIndexBuffer(int hIb)
{
	D3DPOOL m_pool;
	if (m_xaIbList[hIb].xdwUsage & 0x200)
		m_pool = D3DPOOL_DEFAULT;
	else
		m_pool = D3DPOOL_MANAGED;
	fnWD3DDevice_CreateIndexBuffer(m_pd3dDevice, 2 * m_xaIbList[hIb].xnIdxs,
		m_xaIbList[hIb].xdwUsage, D3DFMT_INDEX16, m_pool, &m_xaIbList[hIb].xpIb,
		0);
	this->xFillIndexBuffer(hIb);
}

void WDirect3D8::xFillVertexBuffer(int hVb)
{
	void* pb;

	if (!m_xaVbList[hVb].xpVb)
	{
		return;
	}

	unsigned long flags = 0;
	if (m_xaVbList[hVb].xdwUsage & 0x200)
	{
		flags = 0x2000;
	}

	m_xaVbList[hVb].xpVb->Lock(0, 0, &pb, flags);
	memcpy(pb, m_xaVbList[hVb].xpVertexData,
		m_xaVbList[hVb].xnVtxs * m_xaVbList[hVb].xbStride);
	m_xaVbList[hVb].xpVb->Unlock();

	m_xaVbList[hVb].xbNeedToBeFilled = false;
}

void WDirect3D8::xFillIndexBuffer(int hIb)
{
	void* pb;

	if (!m_xaIbList[hIb].xpIb)
	{
		return;
	}

	unsigned long flags = 0;
	if (m_xaIbList[hIb].xdwUsage & 0x200)
	{
		flags = 0x2000;
	}

	m_xaIbList[hIb].xpIb->Lock(0, 0, &pb, flags);
	memcpy(pb, m_xaIbList[hIb].xpIndexData, m_xaIbList[hIb].xnIdxs * 2);
	m_xaIbList[hIb].xpIb->Unlock();

	m_xaIbList[hIb].xbNeedToBeFilled = false;
}

void WDirect3D8::Paint()
{
	if (this->m_devState != W_VDEVSTATE_LOST)
	{
		this->Present();
	}
}

bool WDirect3D8::IsTextureFilled(int texHandle) const
{
	return this->m_texList[texHandle].isFilled;
}

int WDirect3D8::GetTextureWidth(int m_hTex) const
{
	return this->m_texList[m_hTex].width;
}

int WDirect3D8::GetTextureHeight(int m_hTex) const
{
	return this->m_texList[m_hTex].height;
}

void WDirect3D8::SetRenderTargetSizeInfo(int m_hTex,
	const WRenderToTextureSizeInfo& sizeInfo)
{
	this->m_texList[m_hTex].renderTargetSizeInfo = sizeInfo;
}

int WDirect3D8::LockVB(const void* vPtr, int vNum, int vPitch, int offset)
{
	WDirect3D8& self = *this;

	unsigned char* pBuffer;

	if (!self.m_pLockableVB)
	{
		unsigned long dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		self.m_pd3dDevice->CreateVertexBuffer(0x100000, dwUsage, 0,
			D3DPOOL_DEFAULT, &self.m_pLockableVB, 0);
		self.m_pd3dDevice->CreateIndexBuffer(0x80000, dwUsage, D3DFMT_INDEX16,
			D3DPOOL_DEFAULT, &self.m_pLockableIB, 0);
		self.m_iVBOffset = 0x100000;
		self.m_iIBOffset = 0x80000;
	}

	if (!self.m_pLockableVB || !self.m_pLockableIB)
	{
		return -1;
	}

	int dwPitchOff = (vPitch - self.m_iVBOffset % vPitch) % vPitch;
	int len = (offset + vNum) * vPitch;
	unsigned long dwLockFlags = 0x1000;
	if ((self.m_iVBOffset + len + dwPitchOff) > 0x100000 ||
		(self.m_iVBOffset + len + dwPitchOff) / vPitch >=
			(int)self.m_d3dcaps.MaxVertexIndex ||
		(self.m_iVBOffset + len + dwPitchOff) / vPitch >= 0x10000)
	{
		dwLockFlags = 0x2000;
		self.m_iVBOffset = 0;
		dwPitchOff = 0;
	}

	if (fnWD3DVertexBuffer_Lock(self.m_pLockableVB,
			self.m_iVBOffset + offset * vPitch + dwPitchOff, vPitch * vNum,
			&pBuffer, dwLockFlags) != S_OK)
	{
		return -1;
	}

	memcpy(pBuffer, vPtr, vPitch * vNum);
	self.m_pLockableVB->Unlock();
	int result = (self.m_iVBOffset + dwPitchOff) / vPitch;
	self.m_iVBOffset += len + dwPitchOff;

	return result;
}

int WDirect3D8::LockIB(const unsigned short* pvIb, int iNum)
{
	unsigned char* pBuffer;
	int len = 2 * iNum;
	unsigned long lockFlags = 0x1000;
	if (len + this->m_iIBOffset > 0x80000)
	{
		lockFlags = 0x2000;
		this->m_iIBOffset = 0;
	}
	if (fnWD3DIndexBuffer_Lock(m_pLockableIB, m_iIBOffset, len, &pBuffer,
			lockFlags) != S_OK)
	{
		return -1;
	}
	memcpy(pBuffer, pvIb, len);
	this->m_pLockableIB->Unlock();
	int result = this->m_iIBOffset / sizeof(unsigned short);
	this->m_iIBOffset += len;
	return result;
}

bool WDirect3D8::DrawIndexedPrimitiveLockable(_D3DPRIMITIVETYPE primitiveType,
	const void* vPtr, int vNum, int vPitch, unsigned short* iPtr, int primNum)
{
	int off[2];
	int hVB;
	int hIb;

	off[0] = this->LockVB(vPtr, vNum, vPitch, 0);
	if (off[0] == -1)
	{
		return false;
	}
	if (primitiveType == 2)
	{
		off[1] = this->LockIB(iPtr, 2 * primNum);
	}
	else
	{
		off[1] = this->LockIB(iPtr, 3 * primNum);
	}
	if (off[1] == -1)
	{
		return false;
	}
	hVB = this->m_xhLastVb;
	hIb = this->m_xhLastIb;
	this->m_xhLastVb = NULL;
	this->m_xhLastIb = NULL;
	fnWD3DDevice_SetStreamSource(m_pd3dDevice, 0, this->m_pLockableVB, 0,
		vPitch);
	this->m_pd3dDevice->SetIndices(this->m_pLockableIB);
	if (this->ApplyShader())
	{
		this->m_CustomRenderState.Begin(*this);
		fnWD3DDevice_DrawIndexedPrimitive(m_pd3dDevice, primitiveType, off[0],
			0, vNum, off[1], primNum);
		++this->m_xnDIPs;
		this->m_xnTotalTris += primNum;
		this->m_CustomRenderState.End(*this);
	}
	this->xSetTnLBuffer(hVB, hIb);
	return true;
}

bool WDirect3D8::DrawPrimitiveLockable(_D3DPRIMITIVETYPE primitiveType,
	unsigned int primitiveCount, const void* pVertexStreamZeroData,
	unsigned int vertexStreamZeroStride)
{
	int vNum = primitiveCount;
	switch (primitiveType)
	{
	case D3DPT_TRIANGLEFAN:
	case D3DPT_TRIANGLESTRIP:
		vNum = primitiveCount + 2;
		break;
	case D3DPT_TRIANGLELIST:
		vNum = primitiveCount * 3;
		break;
	case D3DPT_LINESTRIP:
		vNum = primitiveCount + 1;
		break;
	case D3DPT_LINELIST:
		vNum = primitiveCount * 2;
		break;
	}
	int off =
		this->LockVB(pVertexStreamZeroData, vNum, vertexStreamZeroStride, 0);
	if (off == -1)
	{
		return false;
	}
	int hVb = this->m_xhLastVb;
	int hIb = this->m_xhLastIb;
	this->m_xhLastVb = NULL;
	fnWD3DDevice_SetStreamSource(m_pd3dDevice, 0, this->m_pLockableVB, 0,
		vertexStreamZeroStride);
	if (this->ApplyShader())
	{
		this->m_CustomRenderState.Begin(*this);
		this->m_pd3dDevice->DrawPrimitive(primitiveType, off, primitiveCount);
		++this->m_xnDPs;
		this->m_xnTotalTris += primitiveCount;
		this->m_CustomRenderState.End(*this);
	}
	this->xSetTnLBuffer(hVb, hIb);
	return true;
}

void WDirect3D8::FlushRenderPrimitive()
{
	D3DXMATRIX bak;
	int restoreTm;

	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		this->m_MergeBuffer.primitiveCount = 0;
		return;
	}

	if (!this->m_MergeBuffer.primitiveCount ||
		GetCurrentThreadId() != this->m_mainThreadId)
	{
		return;
	}

	this->xInstantiateAndFillTexture(
		static_cast<int>(this->m_lastTexState & 0x7FF));
	this->xInstantiateAndFillTexture(
		static_cast<int>(this->m_lastTexState >> 11 & 0x7F));

	if (this->m_lastRenderState & 0x1000)
	{
		this->m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(
			this->m_MergeBuffer.numVertices *
			this->m_MergeBuffer.vertexStreamZeroStride);
		unsigned int srcIndex =
			this->m_MergeBuffer.vertexStreamZeroStride * m_MergeBuffer.minIndex;
		unsigned char* src =
			&this->m_xaVbList[this->m_xhLastVb].xpVertexData[srcIndex];
		FillVertex(m_MergeBuffer.vertexStreamBuffer, src, m_xLastVertexDecl,
			m_MergeBuffer.numVertices, m_MergeBuffer.vertexStreamZeroStride,
			m_xLastWorldMatrix);
		unsigned int vPitch = this->m_MergeBuffer.vertexStreamZeroStride;
		unsigned int position = m_xLastVertexDecl & D3DFVF_POSITION_MASK;
		if (position == 12)
			vPitch -= 16;
		else if (position == 10)
			vPitch -= 12;
		else if (position == 8)
			vPitch -= 8;
		else if (position == 6)
			vPitch -= 4;
		int off = this->LockVB(this->m_MergeBuffer.vertexStreamBuffer,
			this->m_MergeBuffer.numVertices, vPitch,
			this->m_MergeBuffer.minIndex);
		if (off == -1)
		{
			this->m_MergeBuffer.primitiveCount = 0;
			return;
		}
		int hVb = this->m_xhLastVb;
		int hIb = this->m_xhLastIb;
		fnWD3DDevice_SetStreamSource(this->m_pd3dDevice, 0, this->m_pLockableVB,
			0, vPitch);
		restoreTm = this->m_xLastWorldMatrix[0] != g_mId;
		if (restoreTm)
		{
			memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
			memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
			this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0),
				this->m_xLastWorldMatrix);
		}
		if (this->ApplyShader())
		{
			this->m_CustomRenderState.Begin(*this);
			fnWD3DDevice_DrawIndexedPrimitive(this->m_pd3dDevice,
				this->m_MergeBuffer.type, off, 0,
				this->m_MergeBuffer.numVertices + this->m_MergeBuffer.minIndex,
				this->m_MergeBuffer.startIndex,
				this->m_MergeBuffer.primitiveCount);
			this->m_xnTotalTris += this->m_MergeBuffer.primitiveCount;
			++this->m_xnDIPs;
			this->m_CustomRenderState.End(*this);
		}
		this->m_xhLastVb = 0;
		this->m_xhLastIb = hIb;
		this->xSetTnLBuffer(hVb, hIb);
	}
	else
	{
		if (!this->m_MergeBuffer.lockVertex)
		{
			restoreTm =
				this->m_xLastWorldMatrix[0] != this->m_MergeBuffer.xLastMatrix;
			if (restoreTm)
			{
				memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
				memcpy(this->m_xLastWorldMatrix,
					&this->m_MergeBuffer.xLastMatrix, sizeof(D3DXMATRIX));
				this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0),
					this->m_xLastWorldMatrix);
			}
			int hVb = this->m_xhLastVb;
			int hIb = this->m_xhLastIb;
			this->xSetTnLBuffer(this->m_MergeBuffer.xhVb,
				this->m_MergeBuffer.xhIb);
			if (this->ApplyShader())
			{
				this->m_CustomRenderState.Begin(*this);
				fnWD3DDevice_DrawIndexedPrimitive(this->m_pd3dDevice,
					this->m_MergeBuffer.type, 0, this->m_MergeBuffer.minIndex,
					this->m_MergeBuffer.numVertices,
					this->m_MergeBuffer.startIndex,
					this->m_MergeBuffer.primitiveCount);
				this->m_xnTotalTris += this->m_MergeBuffer.primitiveCount;
				++this->m_xnDIPs;
				this->m_CustomRenderState.End(*this);
			}
			this->xSetTnLBuffer(hVb, hIb);
		}
		else
		{
			restoreTm = this->m_xLastWorldMatrix[0] != g_mId;
			if (restoreTm)
			{
				memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
				memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
				this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0),
					this->m_xLastWorldMatrix);
			}
			if (this->m_MergeBuffer.lockIndex == 1)
			{
				this->DrawIndexedPrimitiveLockable(this->m_MergeBuffer.type,
					this->m_MergeBuffer.vertexStreamBuffer,
					this->m_MergeBuffer.numVertices,
					this->m_MergeBuffer.vertexStreamZeroStride,
					this->m_MergeBuffer.indexBuffer,
					this->m_MergeBuffer.primitiveCount);
			}
			else
			{
				this->DrawPrimitiveLockable(this->m_MergeBuffer.type,
					this->m_MergeBuffer.primitiveCount,
					this->m_MergeBuffer.vertexStreamBuffer,
					this->m_MergeBuffer.vertexStreamZeroStride);
			}
		}
	}
	if (restoreTm)
	{
		memcpy(this->m_xLastWorldMatrix, &bak, sizeof(D3DXMATRIX));
		this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0),
			this->m_xLastWorldMatrix);
	}
	this->m_MergeBuffer.primitiveCount = 0;
}

void WDirect3D8::DrawPrimitive(int iType, int iNum,
	unsigned long dwVertexTypeDesc, void* lpvVertices,
	_D3DPRIMITIVETYPE dptPrimitiveType, int iType2)
{
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}
	this->_SetTransform(D3DTS_WORLD, g_mId);
	this->SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xFFFFFFFF);
	switch (dptPrimitiveType)
	{
	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		iNum -= 2;
		break;
	case D3DPT_TRIANGLELIST:
		iNum /= 3;
		break;
	case D3DPT_LINESTRIP:
		iNum -= 1;
		break;
	case D3DPT_LINELIST:
		iNum /= 2;
		break;
	default:
		break;
	}
	this->_DrawPrimitiveUP(dptPrimitiveType, iNum, lpvVertices,
		this->m_vtxSize);
}

static int FillIndices(D3DPRIMITIVETYPE primitiveType,
	unsigned short* pIndexData, int startIndex, int numVertices)
{
	int i;
	switch (primitiveType)
	{
	case D3DPT_LINELIST:
		for (i = 0; i < numVertices - 1; i++)
		{
			pIndexData[i * 2 + 0] = startIndex + 2 * i + 0;
			pIndexData[i * 2 + 1] = startIndex + 2 * i + 1;
		}
		return i;
	case D3DPT_LINESTRIP:
		for (i = 0; i < numVertices - 1; i++)
		{
			pIndexData[i * 2 + 0] = i + startIndex + 0;
			pIndexData[i * 2 + 1] = i + startIndex + 1;
		}
		return i;
	case D3DPT_TRIANGLEFAN:
		for (i = 0; i <= numVertices - 3; i++)
		{
			pIndexData[i * 3 + 0] = startIndex;
			pIndexData[i * 3 + 1] = i + startIndex + 1;
			pIndexData[i * 3 + 2] = i + startIndex + 2;
		}
		return i;
	case D3DPT_TRIANGLESTRIP:
		for (i = 0; i <= numVertices - 3; i++)
		{
			if (!(i & 1))
			{
				pIndexData[i * 3 + 1] = i + startIndex + 1;
				pIndexData[i * 3 + 2] = i + startIndex + 2;
			}
			else
			{
				pIndexData[i * 3 + 0] = i + startIndex + 0;
				pIndexData[i * 3 + 1] = i + startIndex + 2;
				pIndexData[i * 3 + 2] = i + startIndex + 1;
			}
		}
		return i;
	case D3DPT_TRIANGLELIST:
		for (i = 0; i < numVertices / 3; i++)
		{
			pIndexData[i * 3 + 0] = i + startIndex + 2 * i + 0;
			pIndexData[i * 3 + 1] = i + startIndex + 2 * i + 1;
			pIndexData[i * 3 + 2] = i + startIndex + 2 * i + 2;
		}
		return i;
	default:
		return 0;
	}
	return i;
}

void WDirect3D8::FillVertex(unsigned char* dest, const unsigned char* src,
	int xLastVertexDecl, int NumVertices, int VertexStreamZeroStride,
	const D3DXMATRIX* tm)
{
	int off = 0;
	const int& positionType = xLastVertexDecl & D3DFVF_POSITION_MASK;
	if (positionType == D3DFVF_XYZ)
	{
		__m128 m11;
		{
			const float (&p)[4] = tm->m[0];
			m11 = _mm_set_ps(p[3], p[2], p[1], p[0]);
		}
		__m128 m21;
		{
			const float (&p)[4] = tm->m[1];
			m21 = _mm_set_ps(p[3], p[2], p[1], p[0]);
		}
		__m128 m31;
		{
			const float (&p)[4] = tm->m[2];
			m31 = _mm_set_ps(p[3], p[2], p[1], p[0]);
		}
		__m128 m41;
		{
			const float (&p)[4] = tm->m[3];
			m41 = _mm_set_ps(p[3], p[2], p[1], p[0]);
		}
		if (xLastVertexDecl & D3DFVF_NORMAL)
		{
			for (int i = 0; i < NumVertices; i++, off += VertexStreamZeroStride)
			{
				float* positionOut = (float*)(dest + off);
				const float* p =
					(const float*)(src + i * VertexStreamZeroStride);
				__m128 _d1_prod0 = _mm_mul_ps(_mm_set_ps1(p[0]), m11);
				__m128 _d1_prod1 = _mm_mul_ps(_mm_set_ps1(p[1]), m21);
				__m128 _d1_prod2 = _mm_mul_ps(_mm_set_ps1(p[2]), m31);
				__m128 _d1_sum9 = _mm_add_ps(m41, _d1_prod0);
				__m128 _d1_sum10 = _mm_add_ps(_d1_sum9, _d1_prod1);
				__m128 _d1 = _mm_add_ps(_d1_sum10, _d1_prod2);
				__m128 _d2_prod0 = _mm_mul_ps(_mm_set_ps1(p[3]), m11);
				__m128 _d2_prod1 = _mm_mul_ps(_mm_set_ps1(p[4]), m21);
				__m128 _d2_prod2 = _mm_mul_ps(_mm_set_ps1(p[5]), m31);
				__m128 _d2_sum9 = _mm_add_ps(_d2_prod0, _d2_prod1);
				__m128 _d2 = _mm_add_ps(_d2_sum9, _d2_prod2);
				DWORD x = *(const volatile DWORD*)&_d1.m128_f32[0];
				((float*)(dest + i * VertexStreamZeroStride))[1] =
					_d1.m128_f32[1];
				*(DWORD*)positionOut = x;
				positionOut[2] = _d1.m128_f32[2];
				positionOut[3] = _d2.m128_f32[0];
				positionOut[4] = _d2.m128_f32[1];
				positionOut[5] = _d2.m128_f32[2];
				memcpy(dest + off + 24, src + i * VertexStreamZeroStride + 24,
					VertexStreamZeroStride - 24);
			}
		}
		else
		{
			for (int i = 0; i < NumVertices; i++, off += VertexStreamZeroStride)
			{
				float* positionOut = (float*)(dest + off);
				const float* p =
					(const float*)(src + i * VertexStreamZeroStride);
				__m128 _d_prod0 = _mm_mul_ps(_mm_set_ps1(p[0]), m11);
				__m128 _d_prod1 = _mm_mul_ps(_mm_set_ps1(p[1]), m21);
				__m128 _d_prod2 = _mm_mul_ps(_mm_set_ps1(p[2]), m31);
				__m128 _d_sum9 = _mm_add_ps(m41, _d_prod0);
				__m128 _d_sum10 = _mm_add_ps(_d_sum9, _d_prod1);
				__m128 _d = _mm_add_ps(_d_sum10, _d_prod2);
				DWORD x = *(const volatile DWORD*)&_d.m128_f32[0];
				((float*)(dest + i * VertexStreamZeroStride))[1] =
					_d.m128_f32[1];
				*(DWORD*)positionOut = x;
				positionOut[2] = _d.m128_f32[2];
				memcpy(dest + off + 12, src + i * VertexStreamZeroStride + 12,
					VertexStreamZeroStride - 12);
			}
		}
	}
	else if (positionType == D3DFVF_XYZRHW)
	{
		memcpy(dest, src, VertexStreamZeroStride * NumVertices);
	}
	else if (xLastVertexDecl & D3DFVF_NORMAL)
	{
		if (positionType == 12)
		{
			{
				int srcOff = 0;
				const BYTE* walk = src;
				for (int i = 0; i < NumVertices; i++,
						 walk += VertexStreamZeroStride,
						 off += VertexStreamZeroStride - 16)
				{
					do
					{
						srcOff = int(walk - src);
					} while (srcOff != int(walk - src));
					const D3DXVECTOR3* normal = (const D3DXVECTOR3*)(walk + 28);
					const D3DXVECTOR3* position =
						(const D3DXVECTOR3*)((const BYTE*)normal - 28);
					D3DXVECTOR3 n[4] = { D3DXVECTOR3(), D3DXVECTOR3(),
						D3DXVECTOR3(), D3DXVECTOR3() };
					D3DXVECTOR3 v[4] = { D3DXVECTOR3(), D3DXVECTOR3(),
						D3DXVECTOR3(), D3DXVECTOR3() };
					D3DXVec3TransformCoord(&v[0], position,
						&tm[src[srcOff + 24]]);
					D3DXVec3TransformNormal(&n[0], normal,
						&tm[src[srcOff + 24]]);
					D3DXVec3TransformCoord(&v[1], position,
						&tm[src[srcOff + 25]]);
					D3DXVec3TransformNormal(&n[1], normal,
						&tm[src[srcOff + 25]]);
					D3DXVec3TransformCoord(&v[2], position,
						&tm[src[srcOff + 26]]);
					D3DXVec3TransformNormal(&n[2], normal,
						&tm[src[srcOff + 26]]);
					D3DXVec3TransformCoord(&v[3], position,
						&tm[src[srcOff + 27]]);
					D3DXVec3TransformNormal(&n[3], normal,
						&tm[src[srcOff + 27]]);
					*(D3DXVECTOR3*)(dest + off) =
						v[0] * *(const float*)(src + srcOff + 12) +
						v[1] * *(const float*)(src + srcOff + 16) +
						v[2] * *(const float*)(src + srcOff + 20) +
						v[3] *
							(1.0f - *(const float*)(src + srcOff + 12) -
								*(const float*)(src + srcOff + 16) -
								*(const float*)(src + srcOff + 20));
					*(D3DXVECTOR3*)(dest + off + 12) =
						n[0] * *(const float*)(src + srcOff + 12) +
						n[1] * *(const float*)(src + srcOff + 16) +
						n[2] * *(const float*)(src + srcOff + 20) +
						n[3] *
							(1.0f - *(const float*)(src + srcOff + 12) -
								*(const float*)(src + srcOff + 16) -
								*(const float*)(src + srcOff + 20));
					memcpy(dest + off + 24, walk + 40,
						VertexStreamZeroStride - 40);
				}
			}
		}
		else if (positionType == 10)
		{
			for (int i = 0; i < NumVertices;
				i++, off += VertexStreamZeroStride - 12)
			{
				const D3DXVECTOR3* position =
					(const D3DXVECTOR3*)(src + i * VertexStreamZeroStride);
				const D3DXVECTOR3* normal =
					(const D3DXVECTOR3*)(src + i * VertexStreamZeroStride + 24);
				const float* weights =
					(const float*)(src + i * VertexStreamZeroStride + 12);
				D3DXVECTOR3 v[3];
				D3DXVECTOR3 n[3];
				D3DXVec3TransformCoord(&v[0], position,
					&tm[src[i * VertexStreamZeroStride + 20]]);
				D3DXVec3TransformNormal(&n[0], normal,
					&tm[src[i * VertexStreamZeroStride + 20]]);
				D3DXVec3TransformCoord(&v[1], position,
					&tm[src[i * VertexStreamZeroStride + 21]]);
				D3DXVec3TransformNormal(&n[1], normal,
					&tm[src[i * VertexStreamZeroStride + 21]]);
				D3DXVec3TransformCoord(&v[2], position,
					&tm[src[i * VertexStreamZeroStride + 22]]);
				D3DXVec3TransformNormal(&n[2], normal,
					&tm[src[i * VertexStreamZeroStride + 22]]);
				*(D3DXVECTOR3*)(dest + off) = v[0] * weights[0] +
					v[1] * weights[1] + v[2] * (1.0f - weights[0] - weights[1]);
				*(D3DXVECTOR3*)(dest + off + 12) = n[0] * weights[0] +
					n[1] * weights[1] + n[2] * (1.0f - weights[0] - weights[1]);
				memcpy(dest + off + 24, src + i * VertexStreamZeroStride + 36,
					VertexStreamZeroStride - 36);
			}
		}
		else if (positionType == 8)
		{
			const BYTE* walk = src;
			for (int i = 0; i < NumVertices; i++,
					 walk += VertexStreamZeroStride,
					 off += VertexStreamZeroStride - 8)
			{
				const D3DXVECTOR3* position = (const D3DXVECTOR3*)(walk);
				const D3DXVECTOR3* normal = (const D3DXVECTOR3*)(walk + 20);
				const float* weights = (const float*)(walk + 12);
				D3DXVECTOR3 v[2];
				D3DXVECTOR3 n[2];
				D3DXVec3TransformCoord(&v[0], position, &tm[walk[16]]);
				D3DXVec3TransformNormal(&n[0], normal, &tm[walk[16]]);
				D3DXVec3TransformCoord(&v[1], position, &tm[walk[17]]);
				D3DXVec3TransformNormal(&n[1], normal, &tm[walk[17]]);
				*(D3DXVECTOR3*)(dest + off) =
					v[0] * weights[0] + v[1] * (1.0f - weights[0]);
				*(D3DXVECTOR3*)(dest + off + 12) =
					n[0] * weights[0] + n[1] * (1.0f - weights[0]);
				memcpy(dest + off + 24, walk + 32, VertexStreamZeroStride - 32);
			}
		}
		else if (positionType == 6)
		{
			const BYTE* walk = src;
			for (int i = 0; i < NumVertices; i++,
					 walk += VertexStreamZeroStride,
					 off += VertexStreamZeroStride - 4)
			{
				const D3DXVECTOR3* position = (const D3DXVECTOR3*)(walk);
				const D3DXVECTOR3* normal = (const D3DXVECTOR3*)(walk + 16);
				const float* weights = (const float*)(walk + 12);
				D3DXVec3TransformCoord((D3DXVECTOR3*)(dest + off), position,
					&tm[walk[12]]);
				D3DXVec3TransformNormal((D3DXVECTOR3*)(dest + off + 12), normal,
					&tm[walk[12]]);
				memcpy(dest + off + 24, walk + 28, VertexStreamZeroStride - 28);
			}
		}
	}
	else
	{
		if (positionType == 12)
		{
			{
				int srcOff = 0;
				if (*(volatile int*)&NumVertices > srcOff)
				{
					const BYTE* position = src;
					for (int i = 0; i < NumVertices;
						srcOff += VertexStreamZeroStride,
							 position += VertexStreamZeroStride, i++,
							 off += VertexStreamZeroStride - 16)
					{
						D3DXVECTOR3 v[4] = { D3DXVECTOR3(), D3DXVECTOR3(),
							D3DXVECTOR3(), D3DXVECTOR3() };
						D3DXVec3TransformCoord(&v[0],
							(const D3DXVECTOR3*)(*(
								const BYTE* volatile*)&position),
							&tm[src[srcOff + 24]]);
						D3DXVec3TransformCoord(&v[1],
							(const D3DXVECTOR3*)(*(
								const BYTE* volatile*)&position),
							&tm[src[srcOff + 25]]);
						D3DXVec3TransformCoord(&v[2],
							(const D3DXVECTOR3*)(*(
								const BYTE* volatile*)&position),
							&tm[src[srcOff + 26]]);
						D3DXVec3TransformCoord(&v[3],
							(const D3DXVECTOR3*)(*(
								const BYTE* volatile*)&position),
							&tm[src[srcOff + 27]]);
						*(D3DXVECTOR3*)(dest + off) =
							v[0] * *(const float*)(src + srcOff + 12) +
							v[1] * *(const float*)(src + srcOff + 16) +
							v[2] * *(const float*)(src + srcOff + 20) +
							v[3] *
								(1.0f - *(const float*)(src + srcOff + 12) -
									*(const float*)(src + srcOff + 16) -
									*(const float*)(src + srcOff + 20));
						memcpy(dest + off + 12,
							(*(const BYTE* volatile*)&position) + 28,
							VertexStreamZeroStride - 28);
					}
				}
				if (srcOff == NumVertices * VertexStreamZeroStride)
					return;
			}
		}
		else if (positionType == 10)
		{
			for (int i = 0; i < NumVertices;
				i++, off += VertexStreamZeroStride - 12)
			{
				const D3DXVECTOR3* position =
					(const D3DXVECTOR3*)(src + i * VertexStreamZeroStride);
				const float* weights =
					(const float*)(src + i * VertexStreamZeroStride + 16);
				D3DXVECTOR3 v[3];
				D3DXVec3TransformCoord(&v[0], position,
					&tm[src[i * VertexStreamZeroStride + 20]]);
				D3DXVec3TransformCoord(&v[1], position,
					&tm[src[i * VertexStreamZeroStride + 21]]);
				D3DXVec3TransformCoord(&v[2], position,
					&tm[src[i * VertexStreamZeroStride + 22]]);
				*(D3DXVECTOR3*)(dest + off) = v[0] * weights[-1] +
					v[1] * weights[0] +
					v[2] * (1.0f - weights[-1] - weights[0]);
				memcpy(dest + off + 12, src + i * VertexStreamZeroStride + 24,
					VertexStreamZeroStride - 24);
			}
		}
		else if (positionType == 8)
		{
			const BYTE* walk = src;
			for (int i = 0; i < NumVertices; i++,
					 walk += VertexStreamZeroStride,
					 off += VertexStreamZeroStride - 8)
			{
				const D3DXVECTOR3* position = (const D3DXVECTOR3*)(walk);
				const float* weights = (const float*)(walk + 12);
				D3DXVECTOR3 v[2];
				D3DXVec3TransformCoord(&v[0], position, &tm[walk[16]]);
				D3DXVec3TransformCoord(&v[1], position, &tm[walk[17]]);
				*(D3DXVECTOR3*)(dest + off) =
					v[0] * weights[0] + v[1] * (1.0f - weights[0]);
				memcpy(dest + off + 12, walk + 20, VertexStreamZeroStride - 20);
			}
		}
		else if (positionType == 6)
		{
			const BYTE* walk = src + 4;
			for (int i = 0; i < NumVertices; walk += VertexStreamZeroStride,
					 off += VertexStreamZeroStride - 4, i++)
			{
				const D3DXVECTOR3* position = (const D3DXVECTOR3*)(walk - (4));
				const float* weights = (const float*)(walk + 8);
				D3DXVec3TransformCoord((D3DXVECTOR3*)(dest + off), position,
					&tm[walk[8]]);
				memcpy(dest + off + 12, walk + 12, VertexStreamZeroStride - 16);
			}
		}
	}
}

void WDirect3D8::_DrawPrimitiveUP(_D3DPRIMITIVETYPE type,
	unsigned int primitiveCount, const void* pVertexStreamZeroData,
	unsigned int vertexStreamZeroStride)
{
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}

	int numVertices = primitiveCount;
	switch (type)
	{
	case D3DPT_TRIANGLEFAN:
		numVertices = primitiveCount + 2;
		break;
	case D3DPT_TRIANGLESTRIP:
		numVertices = primitiveCount + 2;
		break;
	case D3DPT_TRIANGLELIST:
		numVertices = primitiveCount * 3;
		break;
	case D3DPT_LINESTRIP:
		numVertices = primitiveCount + 1;
		break;
	case D3DPT_LINELIST:
		numVertices = primitiveCount * 2;
		break;
	}

	if (this->m_MergeBuffer.primitiveCount > 0)
	{
		bool canMerge = false;
		if (m_MergeBuffer.type == D3DPT_LINELIST ||
			m_MergeBuffer.type == D3DPT_LINESTRIP)
		{
			if (type == D3DPT_LINELIST || type == D3DPT_LINESTRIP)
				canMerge = true;
		}
		else if (m_MergeBuffer.type == D3DPT_TRIANGLELIST ||
			m_MergeBuffer.type == D3DPT_TRIANGLESTRIP ||
			m_MergeBuffer.type == D3DPT_TRIANGLEFAN)
		{
			if (type == D3DPT_TRIANGLELIST || type == D3DPT_TRIANGLESTRIP ||
				type == D3DPT_TRIANGLEFAN)
				canMerge = true;
		}

		if (!this->m_MergeBuffer.xhVb && !this->m_MergeBuffer.xhIb &&
			this->m_MergeBuffer.vertexStreamZeroStride ==
				vertexStreamZeroStride)
		{
			if (vertexStreamZeroStride *
						(numVertices + this->m_MergeBuffer.numVertices) <
					this->m_MergeBuffer.vertexStreamBufferSize &&
				canMerge)
			{
				int vtxPerPrimitive = 3;
				if (type == D3DPT_LINELIST || type == D3DPT_LINESTRIP)
				{
					vtxPerPrimitive = 2;
				}
				else if (type == D3DPT_TRIANGLELIST ||
					type == D3DPT_TRIANGLESTRIP || type == D3DPT_TRIANGLEFAN)
				{
					vtxPerPrimitive = 3;
				}
				this->m_MergeBuffer.CheckAndIncreaseIndexBuffer(2 *
					vtxPerPrimitive *
					(primitiveCount + this->m_MergeBuffer.primitiveCount));
				if (!this->m_MergeBuffer.lockIndex)
				{
					this->m_MergeBuffer.primitiveCount =
						FillIndices(this->m_MergeBuffer.type,
							this->m_MergeBuffer.indexBuffer, 0,
							this->m_MergeBuffer.numVertices);
					this->m_MergeBuffer.lockIndex = 1;
					if (this->m_MergeBuffer.type == D3DPT_LINESTRIP)
					{
						this->m_MergeBuffer.type = D3DPT_LINELIST;
					}
					else
					{
						this->m_MergeBuffer.type = D3DPT_TRIANGLELIST;
					}
				}
				int mergeIndex;

				if (this->m_MergeBuffer.type == D3DPT_LINELIST)
				{
					mergeIndex = 2 * m_MergeBuffer.primitiveCount;
				}
				else
				{
					mergeIndex = 3 * m_MergeBuffer.primitiveCount;
				}
				FillVertex(&this->m_MergeBuffer
							   .vertexStreamBuffer[vertexStreamZeroStride *
								   this->m_MergeBuffer.numVertices],
					static_cast<const unsigned char*>(pVertexStreamZeroData),
					this->m_xLastVertexDecl, numVertices,
					vertexStreamZeroStride, this->m_xLastWorldMatrix);
				this->m_MergeBuffer.primitiveCount += FillIndices(type,
					&this->m_MergeBuffer.indexBuffer[mergeIndex],
					this->m_MergeBuffer.numVertices, numVertices);
				this->m_MergeBuffer.numVertices += numVertices;
				return;
			}
		}
		this->FlushRenderPrimitive();
	}

	this->m_MergeBuffer.type = type;
	this->m_MergeBuffer.primitiveCount = primitiveCount;
	this->m_MergeBuffer.vertexStreamZeroStride = vertexStreamZeroStride;
	this->m_MergeBuffer.numVertices = numVertices;
	this->m_MergeBuffer.xhVb = 0;
	this->m_MergeBuffer.xhIb = 0;
	this->m_MergeBuffer.lockVertex = 1;
	this->m_MergeBuffer.lockIndex = 0;
	memcpy(&this->m_MergeBuffer.xLastMatrix, this->m_xLastWorldMatrix,
		sizeof(D3DXMATRIX));
	this->m_MergeBuffer.CheckAndIncreaseVertexStreamBuffer(
		vertexStreamZeroStride * numVertices);

	FillVertex(this->m_MergeBuffer.vertexStreamBuffer,
		static_cast<const unsigned char*>(pVertexStreamZeroData),
		this->m_xLastVertexDecl, numVertices, vertexStreamZeroStride,
		this->m_xLastWorldMatrix);
}

void WDirect3D8::_DrawIndexedPrimitive(_D3DPRIMITIVETYPE type,
	unsigned int minIndex, unsigned int numVertices, unsigned int startIndex,
	unsigned int primitiveCount)
{
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}

	if (this->m_MergeBuffer.primitiveCount > 0)
	{
		if (this->m_MergeBuffer.xhVb)
		{
			if (this->m_MergeBuffer.type == D3DPT_TRIANGLELIST)
			{
				if (m_xaVbList[m_xhLastVb].xbStride ==
						this->m_MergeBuffer.vertexStreamZeroStride &&
					m_xaVbList[m_xhLastVb].xpVertexData &&
					this->m_xaIbList[this->m_xhLastIb].xpIndexData &&
					m_xaVbList[m_xhLastVb].xbStride *
							(numVertices + this->m_MergeBuffer.numVertices) <=
						this->m_MergeBuffer.vertexStreamBufferSize)
				{
					this->m_MergeBuffer.CheckAndIncreaseIndexBuffer(6 *
						(primitiveCount + this->m_MergeBuffer.primitiveCount));
					if (!this->m_MergeBuffer.lockVertex)
					{
						this->m_MergeBuffer.lockVertex = 1;
						this->m_MergeBuffer.lockIndex = 1;
						unsigned int srcIndex =
							m_MergeBuffer.vertexStreamZeroStride *
							m_MergeBuffer.minIndex;
						FillVertex(this->m_MergeBuffer.vertexStreamBuffer,
							reinterpret_cast<const unsigned char*>(
								&m_xaVbList[m_MergeBuffer.xhVb]
									.xpVertexData[srcIndex]),
							this->m_xLastVertexDecl,
							this->m_MergeBuffer.numVertices,
							this->m_MergeBuffer.vertexStreamZeroStride,
							&this->m_MergeBuffer.xLastMatrix);
						if (this->m_MergeBuffer.minIndex)
						{
							for (UINT i = 0;
								i < 3 * this->m_MergeBuffer.primitiveCount; i++)
							{
								this->m_MergeBuffer.indexBuffer[i] =
									this->m_xaIbList[this->m_MergeBuffer.xhIb]
										.xpIndexData[i +
											this->m_MergeBuffer.startIndex] -
									this->m_MergeBuffer.minIndex;
							}
						}
						else
						{
							memcpy(this->m_MergeBuffer.indexBuffer,
								&this->m_xaIbList[this->m_MergeBuffer.xhIb]
									.xpIndexData[this->m_MergeBuffer
											.startIndex],
								6 * this->m_MergeBuffer.primitiveCount);
						}
					}
					FillVertex(&this->m_MergeBuffer.vertexStreamBuffer
								   [this->m_MergeBuffer.vertexStreamZeroStride *
									   this->m_MergeBuffer.numVertices],
						reinterpret_cast<const unsigned char*>(
							&this->m_xaVbList[this->m_xhLastVb]
								.xpVertexData[minIndex *
									this->m_MergeBuffer
										.vertexStreamZeroStride]),
						this->m_xLastVertexDecl, numVertices,
						this->m_MergeBuffer.vertexStreamZeroStride,
						this->m_xLastWorldMatrix);
					for (UINT i = 0, index = startIndex; i < 3 * primitiveCount;
						i++, index++)
					{
						this->m_MergeBuffer
							.indexBuffer[this->m_MergeBuffer.primitiveCount +
								i + 2 * this->m_MergeBuffer.primitiveCount] =
							this->m_MergeBuffer.numVertices +
							this->m_xaIbList[this->m_xhLastIb]
								.xpIndexData[index] -
							minIndex;
					}
					this->m_MergeBuffer.numVertices += numVertices;
					this->m_MergeBuffer.primitiveCount += primitiveCount;
					return;
				}
			}
		}
		this->FlushRenderPrimitive();
	}
	this->m_MergeBuffer.type = type;
	this->m_MergeBuffer.minIndex = minIndex;
	this->m_MergeBuffer.numVertices = numVertices;
	this->m_MergeBuffer.startIndex = startIndex;
	this->m_MergeBuffer.lockVertex = 0;
	this->m_MergeBuffer.lockIndex = 0;
	this->m_MergeBuffer.primitiveCount = primitiveCount;
	this->m_MergeBuffer.xhVb = this->m_xhLastVb;
	this->m_MergeBuffer.xhIb = this->m_xhLastIb;
	this->m_MergeBuffer.vertexStreamZeroStride =
		this->m_xaVbList[this->m_xhLastVb].xbStride;
	memcpy(&this->m_MergeBuffer.xLastMatrix, this->m_xLastWorldMatrix,
		sizeof(D3DXMATRIX));
	if (!this->m_xaVbList[this->m_xhLastVb].xpVertexData ||
		!this->m_xaIbList[this->m_xhLastIb].xpIndexData ||
		this->m_lastRenderState & 0x1000)
	{
		this->FlushRenderPrimitive();
	}
}

void WDirect3D8::_DrawIndexedPrimitiveUP(_D3DPRIMITIVETYPE type,
	unsigned int numVertices, unsigned int primitiveCount,
	const void* pIndexData, const void* pVertexStreamZeroData,
	unsigned int vertexStreamZeroStride)
{
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}
	if (type == 4)
	{
		this->DrawIndexedPrimitiveLockable(D3DPT_TRIANGLELIST,
			pVertexStreamZeroData, numVertices, vertexStreamZeroStride,
			(unsigned short*)pIndexData, primitiveCount);
	}
	else
	{
		this->m_CustomRenderState.Begin(*this);
		this->m_pd3dDevice->DrawIndexedPrimitiveUP(type, 0, numVertices,
			primitiveCount, pIndexData, D3DFMT_INDEX16, pVertexStreamZeroData,
			vertexStreamZeroStride);
		++this->m_xnDIPUPs;
		this->m_xnTotalTris += primitiveCount;
		this->m_CustomRenderState.End(*this);
	}
}

void WDirect3D8::_SetTexture(unsigned long stage, IDirect3DTexture9* pTexture)
{
	if (stage >= 2)
		FlushRenderPrimitive();
	else if (m_pTexture[stage] != pTexture)
	{
		FlushRenderPrimitive();
		m_pTexture[stage] = pTexture;
	}
	m_pd3dDevice->SetTexture(stage, pTexture);
}

void WDirect3D8::_SetLight(unsigned long index, const _D3DLIGHT9* pLight)
{
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetLight(index, pLight);
}

void WDirect3D8::_SetShader(unsigned long flags)
{
	m_Effect = flags;
	if (flags)
	{
		std::map<unsigned long, sEffect>::const_iterator it =
			m_EffectTable.find(flags);
		if (it == m_EffectTable.end())
		{
			if (!CreateEffect(flags))
				m_Effect = 0;
		}
		else if (!(*it).second.pEffect)
			m_Effect = 0;
	}
}

bool WDirect3D8::ApplyShader()
{
	bool ret = true;
	if (m_lastEffect && m_lastEffect != m_Effect)
	{
		sEffect& effect = m_EffectTable[m_lastEffect];
		if (effect.pEffect)
		{
			effect.pEffect->EndPass();
			effect.pEffect->End();
		}
	}
	if (!m_Effect)
	{
		if (m_lastEffect)
		{
			m_pd3dDevice->SetVertexShader(0);
			m_pd3dDevice->SetPixelShader(0);
		}
	}
	else
	{
		UpdateShaderValue(m_Effect);
		sEffect& effect = m_EffectTable[m_Effect];
		if (effect.pEffect)
		{
			if (m_lastEffect == m_Effect)
				effect.pEffect->CommitChanges();
			else
			{
				UINT passcnt;
				effect.pEffect->Begin(&passcnt, 7);
				effect.pEffect->BeginPass(0);
			}
		}
		else
			ret = false;
	}
	m_lastEffect = m_Effect;
	return ret;
}

bool WDirect3D8::LoadSHCoeff(const char* filename)
{
	IDirect3DCubeTexture9* pScratchEnvironmentMap = 0;
	if (D3DXCreateCubeTextureFromFileA(this->m_pd3dDevice, filename,
			&pScratchEnvironmentMap) >= 0)
	{
		float fLight[3][9];
		memset(fLight, 0, sizeof(fLight));
		D3DXSHProjectCubeMap(3, pScratchEnvironmentMap, fLight[0], fLight[1],
			fLight[2]);
		static const float s_fSqrtPI = sqrt(3.14159265358979323846f);
		const float fC0 = 1.0f / (s_fSqrtPI + s_fSqrtPI);
		const float fC1 = 1.0f / (sqrt(3.0f) * s_fSqrtPI);
		const float fC2 = sqrt(15.0f) / (s_fSqrtPI * 8.0f);
		const float fC3 = sqrt(5.0f) / (s_fSqrtPI * 16.0f);
		const float fC4 = fC2 * 0.5f;
		for (int i = 0; i < 3; i++)
		{
			this->m_SHCoeff[i][0] = -fC1 * fLight[i][3];
			this->m_SHCoeff[i][1] = -fC1 * fLight[i][1];
			this->m_SHCoeff[i][2] = fC1 * fLight[i][2];
			this->m_SHCoeff[i][3] = fC0 * fLight[i][0] - fC3 * fLight[i][6];
		}
		for (int i = 0; i < 3; i++)
		{
			this->m_SHCoeff[i + 3][0] = fC2 * fLight[i][4];
			this->m_SHCoeff[i + 3][1] = -fC2 * fLight[i][5];
			this->m_SHCoeff[i + 3][2] = fC3 * fLight[i][6] * 3.0f;
			this->m_SHCoeff[i + 3][3] = -fC2 * fLight[i][7];
		}
		this->m_SHCoeff[6][3] = 1.0f;
		this->m_SHCoeff[6][0] = fC4 * fLight[0][8];
		this->m_SHCoeff[6][1] = fC4 * fLight[1][8];
		this->m_SHCoeff[6][2] = fC4 * fLight[2][8];
		pScratchEnvironmentMap->Release();
		return true;
	}
	return false;
}

void WDirect3D8::UpdateShaderValue(int h)
{
	sEffect& fx = this->m_EffectTable[h];

	if (!fx.pEffect)
	{
		return;
	}

	if (fx.param[0].handle)
	{
		D3DXMATRIX mat, temp;
		D3DXMatrixMultiply(&temp, this->m_xLastWorldMatrix,
			&this->m_xLastViewMatrix);
		D3DXMatrixMultiply(&mat, &temp, &this->m_xLastProjMatrix);
		this->m_fxParamPool.SetMatrix(fx.pEffect,
			WVDFXPARAM_WORLDVIEWPROJECTION, fx.param[0].handle,
			fx.param[0].isShared, mat);
	}
	if (fx.param[1].handle)
	{
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_WORLD,
			fx.param[1].handle, fx.param[1].isShared,
			*(this->m_xLastWorldMatrix));
	}
	if (fx.param[2].handle)
	{
		D3DXMATRIX mat;
		D3DXMatrixMultiply(&mat, this->m_xLastWorldMatrix,
			&this->m_xLastViewMatrix);
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_WORLDVIEW,
			fx.param[2].handle, fx.param[2].isShared, mat);
	}
	if (fx.param[3].handle)
	{
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_PREV_VIEW,
			fx.param[3].handle, fx.param[3].isShared, this->m_xPrevViewMatrix);
	}
	if (fx.param[4].handle)
	{
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_VIEW,
			fx.param[4].handle, fx.param[4].isShared, this->m_xPrevViewMatrix);
	}
	if (fx.param[5].handle)
	{
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_PROJECTION,
			fx.param[5].handle, fx.param[5].isShared, this->m_xLastProjMatrix);
	}
	if (fx.param[6].handle)
	{
		D3DXMATRIX mat;
		D3DXMatrixMultiply(&mat, &this->m_xLastViewMatrix,
			&this->m_xLastProjMatrix);
		this->m_fxParamPool.SetMatrix(fx.pEffect, WVDFXPARAM_VIEWPROJECTION,
			fx.param[6].handle, fx.param[6].isShared, mat);
	}
	if (fx.param[7].handle)
	{
		D3DXVECTOR4 c;
		c.w = static_cast<float>(this->m_LastTFactor >> 24 & 0xFF) / 255.f;
		c.x = static_cast<float>(this->m_LastTFactor >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_LastTFactor >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_LastTFactor >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_CONSTCOLOR,
			fx.param[7].handle, fx.param[7].isShared, c);
	}
	if (fx.param[8].handle)
	{
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_LIGHT_DIFFUSE,
			fx.param[8].handle, fx.param[8].isShared,
			*((const D3DXVECTOR4*)&this->m_xaLights[0].Diffuse));
	}
	if (fx.param[9].handle)
	{
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_LIGHT_AMBIENT,
			fx.param[9].handle, fx.param[9].isShared,
			*((const D3DXVECTOR4*)&this->m_xaLights[0].Ambient));
	}
	if (fx.param[10].handle)
	{
		D3DXVECTOR4 c(this->m_xaLights[0].Direction.x,
			this->m_xaLights[0].Direction.y, this->m_xaLights[0].Direction.z,
			1.0f);
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_LIGHT_DIRECTION,
			fx.param[10].handle, fx.param[10].isShared, c);
	}
	if (fx.param[11].handle)
	{
		D3DXVECTOR4 c;
		c.x = this->m_LastFogStart;
		c.y = this->m_LastFogEnd;
		c.z = 0.0;
		c.w = 1.0;
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_FOG_RANGE,
			fx.param[11].handle, fx.param[11].isShared, c);
	}
	if (fx.param[12].handle)
	{
		D3DXVECTOR3 c;
		c.x = static_cast<float>(this->m_fogColor >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_fogColor >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_fogColor >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector3(fx.pEffect, WVDFXPARAM_FOG_COLOR,
			fx.param[12].handle, fx.param[12].isShared, c);
	}
	if (fx.param[13].handle)
	{
		D3DXVECTOR4 c(0, 0, 0, 1);
		D3DXMATRIX mat;
		D3DXMatrixInverse(&mat, 0, &this->m_xLastViewMatrix);
		c.x = mat.m[3][0];
		c.y = mat.m[3][1];
		c.z = mat.m[3][2];
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_CAMERAPOSITION,
			fx.param[13].handle, fx.param[13].isShared, c);
	}
	if (fx.param[14].handle)
	{
		fx.pEffect->SetVectorArray(fx.param[14].handle,
			reinterpret_cast<D3DXVECTOR4*>(this->m_SHCoeff), 7);
	}
	if (fx.param[17].handle)
	{
		D3DXVECTOR4 c;
		c.w = static_cast<float>(this->m_xdwDiffuse >> 24 & 0xFF) / 255.f;
		c.x = static_cast<float>(this->m_xdwDiffuse >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_xdwDiffuse >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_xdwDiffuse >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector4(fx.pEffect, WVDFXPARAM_MATERIALCOLOR,
			fx.param[17].handle, fx.param[17].isShared, c);
	}
}

void WDirect3D8::ReloadShader()
{
	this->m_Effect = NULL;

	this->ApplyShader();
	this->ReleaseShaderResource();
	if (!IsSupportVS())
		IsSupportPS();
}

void WDirect3D8::ReleaseShaderResource()
{
	m_shadersrc = "";

	for (std::map<unsigned long, sEffect>::iterator it =
			 this->m_EffectTable.begin();
		it != this->m_EffectTable.end(); it++)
	{
		if ((*it).second.pEffect)
		{
			(*it).second.pEffect->Release();
		}
	}

	this->m_EffectTable.clear();

	this->m_fxParamPool.Release();
}

bool WDirect3D8::CreateEffect(unsigned long flags)
{
	static struct
	{
		WVDFXMACROFLAG wvdFxMacroFlag;
		const char* txt;
	} macroList[24] = {
		{ WVDFXMACRO_TEXTURE,                   "WVDFXMACRO_TEXTURE"            },
		{ WVDFXMACRO_NORMAL,                    "WVDFXMACRO_NORMAL"             },
		{ WVDFXMACRO_VERTEX_COLOR,              "WVDFXMACRO_VERTEX_COLOR"       },
		{ WVDFXMACRO_MATERIAL_COLOR,            "WVDFXMACRO_MATERIAL_COLOR"     },
		{ WVDFXMACRO_TEXTURE_STAGE2,            "WVDFXMACRO_TEXTURE_STAGE2"     },
		{ WVDFXMACRO_TEXTURE_STAGE2_ADD,        "WVDFXMACRO_TEXTURE_STAGE2_ADD" },
		{ WVDFXMACRO_USE_CONSTCOLOR,            "WVDFXMACRO_USE_CONSTCOLOR"     },
		{ WVDFXMACRO_FOG,                       "WVDFXMACRO_FOG"                },
		{ WVDFXMACRO_NO_LIGHTING,               "WVDFXMACRO_NO_LIGHTING"        },
		{ WVDFXMACRO_RIMLIGHTING,               "WVDFXMACRO_RIMLIGHTING"        },
		{ WVDFXMACRO_APPLY_SPHERICAL_HARMONICS,
         "WVDFXMACRO_APPLY_SPHERICAL_HARMONICS"                                 },
		{ WVDFXMACRO_POSTPROCESSING_DOF,        "WVDFXMACRO_POSTPROCESSING_DOF" },
		{ WVDFXMACRO_POSTPROCESSING_MOTIONBLUR,
         "WVDFXMACRO_POSTPROCESSING_MOTIONBLUR"                                 },
		{ WVDFXMACRO_OVERLAY,                   "WVDFXMACRO_OVERLAY"            },
		{ WVDFXMACRO_MRT_VELOCITY,              "WVDFXMACRO_MRT_VELOCITY"       },
		{ WVDFXMACRO_MRT_DOF_FACTOR,            "WVDFXMACRO_MRT_DOF_FACTOR"     },
		{ WVDFXMACRO_PROJ_TEXCOORD,             "WVDFXMACRO_PROJ_TEXCOORD"      },
		{ WVDFXMACRO_SHADOW_CASTER,             "WVDFXMACRO_SHADOW_CASTER"      },
		{ WVDFXMACRO_SHADOW_RECVER,             "WVDFXMACRO_SHADOW_RECVER"      },
		{ WVDFXMACRO_BLURRING,                  "WVDFXMACRO_BLURRING"           },
		{ WVDFXMACRO_PERPIXEL_LIGHTING,         "WVDFXMACRO_PERPIXEL_LIGHTING"  },
		{ WVDFXMACRO_NO_VERTEXSHADER,           "WVDFXMACRO_NO_VERTEXSHADER"    },
		{ WVDFXMACRO_NO_PIXELSHADER,            "WVDFXMACRO_NO_PIXELSHADER"     },
		{ WVDFXMACRO_PARAM_SHARING,             "WVDFXMACRO_PARAM_SHARING"      },
	};

	static struct
	{
		WVDFXPARAMETERTYPE wvdFxParamType;
		const char* semantic;
	} paramList[21] = {
		{ WVDFXPARAM_WORLDVIEWPROJECTION, "WVDFXPARAM_WORLDVIEWPROJECTION" },
		{ WVDFXPARAM_WORLD,               "WVDFXPARAM_WORLD"               },
		{ WVDFXPARAM_WORLDVIEW,           "WVDFXPARAM_WORLDVIEW"           },
		{ WVDFXPARAM_PREV_VIEW,           "WVDFXPARAM_PREV_VIEW"           },
		{ WVDFXPARAM_VIEW,                "WVDFXPARAM_VIEW"                },
		{ WVDFXPARAM_PROJECTION,          "WVDFXPARAM_PROJECTION"          },
		{ WVDFXPARAM_VIEWPROJECTION,      "WVDFXPARAM_VIEWPROJECTION"      },
		{ WVDFXPARAM_CONSTCOLOR,          "WVDFXPARAM_CONSTCOLOR"          },
		{ WVDFXPARAM_LIGHT_DIFFUSE,       "WVDFXPARAM_LIGHT_DIFFUSE"       },
		{ WVDFXPARAM_LIGHT_AMBIENT,       "WVDFXPARAM_LIGHT_AMBIENT"       },
		{ WVDFXPARAM_LIGHT_DIRECTION,     "WVDFXPARAM_LIGHT_DIRECTION"     },
		{ WVDFXPARAM_FOG_RANGE,           "WVDFXPARAM_FOG_RANGE"           },
		{ WVDFXPARAM_FOG_COLOR,           "WVDFXPARAM_FOG_COLOR"           },
		{ WVDFXPARAM_CAMERAPOSITION,      "WVDFXPARAM_CAMERAPOSITION"      },
		{ WVDFXPARAM_SHCOEFF,             "WVDFXPARAM_SHCOEFF"             },
		{ WVDFXPARAM_EXTRATEXTURE,        "WVDFXPARAM_EXTRATEXTURE"        },
		{ WVDFXPARAM_REFTEXTURE,          "WVDFXPARAM_REFTEXTURE"          },
		{ WVDFXPARAM_MATERIALCOLOR,       "WVDFXPARAM_MATERIALCOLOR"       },
		{ WVDFXPARAM_TEXTURE_TRANSFORM,   "WVDFXPARAM_TEXTURE_TRANSFORM"   },
		{ WVDFXPARAM_BLUR_UVOFFSET,       "WVDFXPARAM_BLUR_UVOFFSET"       },
		{ WVDFXPARAM_SHADOW_COLOR,        "WVDFXPARAM_SHADOW_COLOR"        },
	};

	D3DXMACRO macro[24];
	memset(macro, 0, sizeof(macro));
	D3DXPARAMETER_DESC paramDesc;
	ID3DXBuffer* err;

	int macroCount = 0;
	for (unsigned int i = 0; i < sizeof(macroList) / sizeof(macroList[0]); ++i)
	{
		if (flags & macroList[i].wvdFxMacroFlag)
		{
			macro[macroCount].Name = macroList[i].txt;
			macro[macroCount++].Definition = "";
		}
	}

	err = 0;
	sEffect* effect = &this->m_EffectTable[flags];
	memset(effect, 0, sizeof(sEffect));
	if (D3DXCreateEffect(this->m_pd3dDevice, this->m_shadersrc.c_str(),
			this->m_shadersrc.size(), macro, 0, 0,
			this->m_fxParamPool.GetPool(), &effect->pEffect, &err) != S_OK)
	{
		return false;
	}

	for (unsigned int i = 0; i < sizeof(paramList) / sizeof(paramList[0]); ++i)
	{
		D3DXHANDLE handle =
			effect->pEffect->GetParameterBySemantic(0, paramList[i].semantic);
		if (handle && effect->pEffect->IsParameterUsed(handle, 0) == TRUE)
		{
			effect->param[paramList[i].wvdFxParamType].handle = handle;
			effect->pEffect->GetParameterDesc(handle, &paramDesc);
			if (paramDesc.Flags & 1)
			{
				effect->param[paramList[i].wvdFxParamType].isShared = true;
			}
		}
	}

	return true;
}

void WDirect3D8::_SetTransform(D3DTRANSFORMSTATETYPE state,
	const D3DXMATRIX& matrix)
{
	if (m_devState == W_VDEVSTATE_LOST)
		return;
	if (state == D3DTS_VIEW)
	{
		if (m_xLastViewMatrix == matrix)
			return;
		FlushRenderPrimitive();
		m_xLastViewMatrix = matrix;
	}
	else if (state == D3DTS_PROJECTION)
	{
		if (m_xLastProjMatrix == matrix)
			return;
		FlushRenderPrimitive();
		m_xLastProjMatrix = matrix;
	}
	else if (state >= D3DTS_WORLD && (unsigned)(state - D3DTS_WORLD) < 256)
	{
		if (m_xLastWorldMatrix[state - D3DTS_WORLD] == matrix)
			return;
		m_xLastWorldMatrix[state - D3DTS_WORLD] = matrix;
		if (IsSupportVS() || state != D3DTS_WORLD)
			return;
	}
	m_pd3dDevice->SetTransform(state, &matrix);
}

void WDirect3D8::_SetRenderState(_D3DRENDERSTATETYPE state, unsigned long value)
{
	this->FlushRenderPrimitive();
	switch (state)
	{
	case D3DRS_TEXTUREFACTOR:
		this->m_LastTFactor = value;
		break;

	case D3DRS_FOGSTART:
		this->m_LastFogStart = reinterpret_cast<float&>(value);
		break;
	case D3DRS_FOGEND:
		this->m_LastFogEnd = reinterpret_cast<float&>(value);
		break;
	case D3DRS_LIGHTING:
		this->m_LightEnable = value != 0;
		break;
	default:
		break;
	}
	this->m_pd3dDevice->SetRenderState(state, value);
}

void WDirect3D8::_SetTextureStageState(unsigned long stage,
	_D3DTEXTURESTAGESTATETYPE type, unsigned long value)
{
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetTextureStageState(stage, type, value);
}

void WDirect3D8::_SetSamplerState(unsigned long sampler,
	_D3DSAMPLERSTATETYPE type, unsigned long value)
{
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetSamplerState(sampler, type, value);
}

void WDirect3D8::DrawPrimitiveIndexed(int iType, unsigned long dwVertexTypeDesc,
	void* lpvVertices, int pNum, unsigned short* fList, int fNum, int iType2)
{
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}
	this->_SetTransform(D3DTS_WORLD, g_mId);
	this->SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xFFFFFFFF);
	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}
	this->DrawIndexedPrimitiveLockable(D3DPT_TRIANGLELIST, lpvVertices, pNum,
		this->m_vtxSize, fList, fNum / 3);
}

bool WDirect3D8::SetRenderState4Flushing(int pass)
{
	static DWORD dwAlphaRef = 0;
	if (pass == 0)
	{
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);
		_SetRenderState(D3DRS_ALPHAREF, 255);
		if (m_lastRenderState & 0x80000000)
		{
			dwAlphaRef = 128;
			return true;
		}
		dwAlphaRef = 0;
		return false;
	}
	else if (pass == 1)
	{
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
	}
	else if (pass == 2)
	{
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
	}
	else
	{
		_SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		_SetRenderState(D3DRS_ALPHAREF, dwAlphaRef);
	}
	return true;
}

int WDirect3D8::UploadCompressedTextureSurface(void* pSrcData,
	unsigned int srcDataSize, int type)
{
	int m_hTex;
	if (*(static_cast<unsigned long*>(pSrcData) + 4) >
			this->m_d3dcaps.MaxTextureWidth ||
		*(static_cast<unsigned long*>(pSrcData) + 3) >
			this->m_d3dcaps.MaxTextureHeight)
	{
		BITMAPINFO bmpInfo;
		WDXTC dxtc;
		dxtc.Load(static_cast<unsigned char*>(pSrcData), srcDataSize);
		unsigned char* pDecompData = dxtc.GetDecompData();
		unsigned long texWidth = *(static_cast<unsigned long*>(pSrcData) + 4);
		bmpInfo.bmiHeader.biWidth = this->m_d3dcaps.MaxTextureWidth;
		if (texWidth <= static_cast<unsigned long>(bmpInfo.bmiHeader.biWidth))
		{
			bmpInfo.bmiHeader.biWidth = texWidth;
		}
		unsigned long texHeight = *(static_cast<unsigned long*>(pSrcData) + 3);
		bmpInfo.bmiHeader.biHeight = this->m_d3dcaps.MaxTextureHeight;
		if (texHeight <= static_cast<unsigned long>(bmpInfo.bmiHeader.biHeight))
		{
			bmpInfo.bmiHeader.biHeight = texHeight;
		}
		bmpInfo.bmiHeader.biBitCount = 32;
		m_hTex = this->CreateTexture(&bmpInfo, type);
		bmpInfo.bmiHeader.biWidth =
			*(static_cast<unsigned long*>(pSrcData) + 4);
		bmpInfo.bmiHeader.biHeight =
			*(static_cast<unsigned long*>(pSrcData) + 3);
		this->UpdateTextureSurface(m_hTex, &bmpInfo, pDecompData, type);
	}
	else
	{
		m_hTex = this->GetTextureNum(static_cast<unsigned>(type) >> 29 & 1);
		this->m_texList[m_hTex].width =
			*(static_cast<unsigned long*>(pSrcData) + 4) >> this->m_iDDSRes;
		this->m_texList[m_hTex].height =
			*(static_cast<unsigned long*>(pSrcData) + 3) >> this->m_iDDSRes;
		this->m_texList[m_hTex].pixFmtInfo = 0;
		this->m_texList[m_hTex].dxtcDataSize = srcDataSize;
		this->m_texList[m_hTex].dxtcData = new char[srcDataSize];
		memcpy(this->m_texList[m_hTex].dxtcData, pSrcData, srcDataSize);
		this->m_texList[m_hTex].updateType = 0;
		this->m_texList[m_hTex].needToBeFilled = false;
	}
	return m_hTex;
}

void WDirect3D8::UpdateCompressedTexture(int texHandle, void* pSrcData,
	unsigned int srcDataSize, int type)
{
}

void WDirect3D8::SetRenderTargetFormat()
{
	this->m_RtFmtIdx = -1;
	this->m_RtNoAlphaFmtIdx = -1;
	this->m_RtLowMemFmtIdx = -1;
	this->m_RtNoAlphaLowMemFmtIdx = -1;
	this->m_RtDepthFmtIdx = -1;

	for (unsigned int i = 0; i < 4; i++)
	{
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, 1, D3DRTYPE_SURFACE,
				ms_RtFmt[i].fmt) != S_OK)
		{
			continue;
		}

		this->m_RtFmtIdx = i;
		break;
	}

	for (unsigned int i = 0; i < 3; i++)
	{
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, 1, D3DRTYPE_SURFACE,
				ms_RtNoAlphaFmt[i].fmt) != S_OK)
		{
			continue;
		}

		this->m_RtNoAlphaFmtIdx = i;
		break;
	}

	for (unsigned int i = 0; i < 5; i++)
	{
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, 2, D3DRTYPE_SURFACE,
				ms_RtDepthFmt[i].fmt) != S_OK)
		{
			continue;
		}

		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, ms_RtFmt[this->m_RtFmtIdx].fmt,
				ms_RtDepthFmt[i].fmt) != S_OK)
		{
			continue;
		}

		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat,
				ms_RtNoAlphaFmt[this->m_RtNoAlphaFmtIdx].fmt,
				ms_RtDepthFmt[i].fmt) != S_OK)
		{
			continue;
		}

		this->m_RtDepthFmtIdx = i;
		break;
	}

	for (unsigned int i = 0; i < 4; i++)
	{
		if (ms_RtFmt[i].bpp > 2)
		{
			continue;
		}

		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, 1, D3DRTYPE_SURFACE,
				ms_RtFmt[i].fmt) != S_OK)
		{
			continue;
		}

		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, ms_RtFmt[i].fmt,
				ms_RtDepthFmt[this->m_RtDepthFmtIdx].fmt) != S_OK)
		{
			continue;
		}

		this->m_RtLowMemFmtIdx = i;
		break;
	}

	for (unsigned int i = 0; i < 3; i++)
	{
		if (ms_RtNoAlphaFmt[i].bpp > 2)
		{
			continue;
		}

		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, 1, D3DRTYPE_SURFACE,
				ms_RtNoAlphaFmt[i].fmt) != S_OK)
		{
			continue;
		}

		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL,
				this->m_d3dpp.BackBufferFormat, ms_RtNoAlphaFmt[i].fmt,
				ms_RtDepthFmt[this->m_RtDepthFmtIdx].fmt) != S_OK)
		{
			continue;
		}

		this->m_RtNoAlphaLowMemFmtIdx = i;
		break;
	}

	if (this->m_RtLowMemFmtIdx == -1)
	{
		this->m_RtLowMemFmtIdx = this->m_RtFmtIdx;
	}

	if (this->m_RtNoAlphaLowMemFmtIdx == -1)
	{
		this->m_RtNoAlphaLowMemFmtIdx = this->m_RtNoAlphaFmtIdx;
	}
}

void WDirect3D8::CreateTextureSurface(unsigned long dwWidth,
	unsigned long dwHeight, int iMipmapCount, int renderTargetType, int iIndex,
	int bChromaKey, int bitNum)
{
	unsigned long i;
	unsigned long j;
	int k;
	D3DFORMAT format;
	unsigned long dwUsage;

	if (renderTargetType != 0 || !(this->m_d3dcaps.TextureCaps & 2))
	{
		i = dwWidth;
		j = dwHeight;
	}
	else
	{
		for (i = 1; i < dwWidth; i *= 2)
		{
		}
		for (j = 1; j < dwHeight; j *= 2)
		{
		}
	}

	if (i >= this->m_d3dcaps.MaxTextureWidth)
	{
		i = this->m_d3dcaps.MaxTextureWidth;
	}
	if (j >= this->m_d3dcaps.MaxTextureHeight)
	{
		j = this->m_d3dcaps.MaxTextureHeight;
	}

	for (k = iMipmapCount; (dwWidth >> k <= 1 || dwHeight >> k <= 1) && k >= 0;
		--k)
	{
	}

	this->m_texList[iIndex].width = i;
	this->m_texList[iIndex].height = j;
	this->m_texList[iIndex].pixFmtInfo =
		this->m_fmt[bChromaKey + (bitNum <= 16 ? 0 : 3)];
	this->m_texList[iIndex].mipmaplevel = k;
	this->m_texList[iIndex].renderTargetType = renderTargetType;
	this->m_texList[iIndex].renderTargetSizeInfo.Reset();
	this->m_texList[iIndex].isFilled = false;

	if (!renderTargetType)
	{
		return;
	}

	switch (renderTargetType & 0xE000)
	{
	case 0x2000:
		if (renderTargetType & 0x10000)
			format = ms_RtFmt[this->m_RtLowMemFmtIdx].fmt;
		else
			format = ms_RtFmt[this->m_RtFmtIdx].fmt;
		dwUsage = 1;
		break;
	case 0x4000:
		if (renderTargetType & 0x10000)
			format = ms_RtNoAlphaFmt[this->m_RtNoAlphaLowMemFmtIdx].fmt;
		else
			format = ms_RtNoAlphaFmt[this->m_RtNoAlphaFmtIdx].fmt;
		dwUsage = 1;
		break;
	case 0x8000:
		format = ms_RtDepthFmt[this->m_RtDepthFmtIdx].fmt;
		dwUsage = 2;
		break;
	default:
		format = ms_RtFmt[this->m_RtFmtIdx].fmt;
		dwUsage = 1;
		break;
	}
	this->m_pd3dDevice->CreateTexture(this->m_texList[iIndex].width,
		this->m_texList[iIndex].height, 1, dwUsage, format, D3DPOOL_DEFAULT,
		&this->m_texList[iIndex].pTex, 0);
	if (this->m_texList[iIndex].pTex)
	{
		this->m_texList[iIndex].pTex->GetSurfaceLevel(0,
			&this->m_texList[iIndex].pSurf);
	}
}

unsigned char* WDirect3D8::GetPixelPtr(BITMAPINFO* bi, unsigned char* data,
	int x, int y)
{
	if (bi->bmiHeader.biBitCount == 8)
	{
		return reinterpret_cast<unsigned char*>(
			&bi->bmiColors[data[y * (bi->bmiHeader.biWidth + 3 & ~3) + x]]);
	}
	return reinterpret_cast<unsigned char*>(
		&data[3 * x + y * (3 * (bi->bmiHeader.biWidth + 1) & ~3)]);
}

void WDirect3D8::UpdateTextureSurfaceNormal(void* buff, int pitch,
	pix_info* pix, BITMAPINFO* bi, void* data)
{
	int w = bi->bmiHeader.biWidth;
	int h = bi->bmiHeader.biHeight;
	for (int y = 0; y < h; ++y)
	{
		unsigned char* ptr = static_cast<unsigned char*>(buff) + y * pitch;
		for (int i = 0; i < w; ++i, ptr += pix->cpp)
		{
			unsigned long code;
			if (i % 3 < 1)
				code = (255 >> pix->b_r_shift << pix->b_l_shift) |
					(255 >> pix->g_r_shift << pix->g_l_shift) |
					(255 >> pix->r_r_shift << pix->r_l_shift) |
					(128 >> pix->a_r_shift << pix->a_l_shift);
			else
				code = (255 >> pix->b_r_shift << pix->b_l_shift) |
					(255 >> pix->g_r_shift << pix->g_l_shift) |
					(255 >> pix->r_r_shift << pix->r_l_shift) |
					(0 >> pix->a_r_shift << pix->a_l_shift);
			memcpy(ptr, &code, pix->cpp);
		}
	}
}

inline void WDirect3D8::UpdateTextureSurfaceDirect(void* buff, int pitch,
	pix_info* pix, BITMAPINFO* bi, void* data)
{
	int cps = bi->bmiHeader.biBitCount >> 3;
	int w = bi->bmiHeader.biWidth;
	int h = bi->bmiHeader.biHeight;
	int nCps = ((cps * w + 3) & ~3) - cps * w;
	int nPitch = pitch - w * pix->cpp;
	unsigned char* src = static_cast<unsigned char*>(data);
	unsigned char* ptr = static_cast<unsigned char*>(buff);
	unsigned long code;
	if (bi->bmiHeader.biBitCount == 8)
	{
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				unsigned char* p =
					reinterpret_cast<unsigned char*>(&bi->bmiColors[*src]);
				if (pix->a_r_shift == 8)
					code = (p[2] >> pix->r_r_shift << pix->r_l_shift) |
						(p[1] >> pix->g_r_shift << pix->g_l_shift) |
						(p[0] >> pix->b_r_shift << pix->b_l_shift);
				else
					code = (p[3] >> pix->a_r_shift << pix->a_l_shift) |
						(p[2] >> pix->r_r_shift << pix->r_l_shift) |
						(p[1] >> pix->g_r_shift << pix->g_l_shift) |
						(p[0] >> pix->b_r_shift << pix->b_l_shift);
				src += cps;
				memcpy(ptr, &code, pix->cpp);
				ptr += pix->cpp;
			}
			ptr += nPitch;
			src += nCps;
		}
	}
	else
	{
		for (int y = 0; y < h; ++y)
		{
			for (int x = 0; x < w; ++x)
			{
				if (pix->a_r_shift == 8)
					code = (src[2] >> pix->r_r_shift << pix->r_l_shift) |
						(src[1] >> pix->g_r_shift << pix->g_l_shift) |
						(src[0] >> pix->b_r_shift << pix->b_l_shift);
				else
					code = (src[3] >> pix->a_r_shift << pix->a_l_shift) |
						(src[2] >> pix->r_r_shift << pix->r_l_shift) |
						(src[1] >> pix->g_r_shift << pix->g_l_shift) |
						(src[0] >> pix->b_r_shift << pix->b_l_shift);
				src += cps;
				memcpy(ptr, &code, pix->cpp);
				ptr += pix->cpp;
			}
			ptr += nPitch;
			src += nCps;
		}
	}
}

inline void WDirect3D8::UpdateTextureSurfaceSampling(void* buff, int width,
	int height, int pitch, pix_info* pix, BITMAPINFO* bi, void* data, int type)
{
	int useChromaKey = type & 0x800;
	unsigned long chromaKey;
	if (useChromaKey)
		chromaKey = (255 >> pix->a_r_shift << pix->a_l_shift) |
			(255 >> pix->b_r_shift << pix->b_l_shift) |
			(0 >> pix->g_r_shift << pix->g_l_shift) |
			(0 >> pix->r_r_shift << pix->r_l_shift);
	int cps = bi->bmiHeader.biBitCount >> 3;
	int bpl = (cps * bi->bmiHeader.biWidth + 3) & ~3;
	for (int j = 0; j < height; ++j)
	{
		unsigned char* ptr = static_cast<unsigned char*>(buff) + j * pitch;
		int offset = (bi->bmiHeader.biHeight != height
				? bpl * (j * bi->bmiHeader.biHeight / height)
				: bpl * j);
		for (int i = 0; i < width; ++i, ptr += pix->cpp)
		{
			unsigned char* p = static_cast<unsigned char*>(data) + offset +
				(bi->bmiHeader.biWidth != width
						? cps * (i * bi->bmiHeader.biWidth / width)
						: cps * i);
			if (bi->bmiHeader.biBitCount == 8)
				p = reinterpret_cast<unsigned char*>(&bi->bmiColors[*p]);
			unsigned long code;
			if (pix->a_r_shift == 8)
				code = (p[2] >> pix->r_r_shift << pix->r_l_shift) |
					(p[1] >> pix->g_r_shift << pix->g_l_shift) |
					(p[0] >> pix->b_r_shift << pix->b_l_shift);
			else if (useChromaKey)
			{
				code = (255 >> pix->a_r_shift << pix->a_l_shift) |
					(p[2] >> pix->r_r_shift << pix->r_l_shift) |
					(p[1] >> pix->g_r_shift << pix->g_l_shift) |
					(p[0] >> pix->b_r_shift << pix->b_l_shift);
				if (code == chromaKey)
					code = 0;
			}
			else
				code = (p[3] >> pix->a_r_shift << pix->a_l_shift) |
					(p[2] >> pix->r_r_shift << pix->r_l_shift) |
					(p[1] >> pix->g_r_shift << pix->g_l_shift) |
					(p[0] >> pix->b_r_shift << pix->b_l_shift);
			memcpy(ptr, &code, pix->cpp);
		}
	}
}

void WDirect3D8::UpdateTextureSurfaceFiltering(void* buff, int w, int h,
	int pitch, WDirect3D8::pix_info* pix, tagBITMAPINFO* bi,
	unsigned char* data)
{
	int spitch =
		(bi->bmiHeader.biWidth * (bi->bmiHeader.biBitCount >> 3) + 3) & ~3;
	float scale_w = (float)bi->bmiHeader.biWidth / w;
	float scale_h = (float)bi->bmiHeader.biHeight / h;
	float extent = scale_w * scale_h;
	for (int y = 0; y < h; ++y)
	{
		int x;
		unsigned char* ptr = (unsigned char*)buff + pitch * y;
		for (x = 0; x < w; ++x, ptr += pix->cpp)
		{
			float u = x * scale_w, v = y * scale_h;
			float fr, fg, fb;
			fr = fg = fb = 0;
			float u1 = u + scale_w, v1 = v + scale_h;
			for (float va = v; va < v1;)
			{
				float dv = min(floor(va) + 1 - va, v1 - va);
				for (float ua = u; ua < u1;)
				{
					float du = min(floor(ua) + 1 - ua, u1 - ua);
					RGBQUAD* rgb;
					if (bi->bmiHeader.biBitCount > 8)
						rgb = (RGBQUAD*)(data + spitch * (int)va + (int)ua * 3);
					else
						rgb = &bi->bmiColors[data[spitch * (int)va + (int)ua]];
					fr += rgb->rgbRed * du * dv;
					fg += rgb->rgbGreen * du * dv;
					fb += rgb->rgbBlue * du * dv;
					ua += du;
				}
				va += dv;
			}
			int code = pix->Pack((BYTE)(fr / extent), (BYTE)(fg / extent),
				(BYTE)(fb / extent));
			memcpy(ptr, &code, pix->cpp);
		}
	}
}

inline void WDirect3D8::UpdateTextureSurfaceAlpha(void* buff, int pitch,
	WDirect3D8::pix_info* pix, const tagRECT& rc, tagBITMAPINFO* bi, void* data)
{
	int nSrcBpp = bi->bmiHeader.biBitCount >> 3;
	int nSrcPitch =
		bi->bmiHeader.biBitCount * bi->bmiHeader.biWidth / 8 + 3 & ~3;
	int nDstBpp = pix->cpp;
	unsigned long code;

	for (int y = 0; y < rc.bottom - rc.top; y++)
	{
		for (int x = 0; x < rc.right - rc.left; x++)
		{
			unsigned char* dataLine =
				static_cast<unsigned char*>(data) + y * nSrcPitch + x * nSrcBpp;
			unsigned char* buffLine =
				static_cast<unsigned char*>(buff) + y * pitch + x * nDstBpp;
			BYTE pixel[3];
			BYTE alpha = dataLine[3];

			if (pix->cpp == 2)
			{
				unsigned int buffword = buffLine[0] | (buffLine[1] << 8);
				pixel[2] = buffword >> pix->r_l_shift << pix->r_r_shift;
				pixel[1] = buffword >> pix->g_l_shift << pix->g_r_shift;
				pixel[0] = buffword >> pix->b_l_shift << pix->b_r_shift;
			}
			else
			{
				memcpy(pixel, buffLine, 3);
			}

			BYTE mult[3];
			for (code = 0; code < 3; code++)
			{
				mult[code] =
					(alpha * dataLine[code] + (255 - alpha) * pixel[code]) /
					255;
			}

			code = pix->a_r_shift == 8
				? pix->Pack(mult[2], mult[1], mult[0])
				: pix->Pack(dataLine[3], mult[2], mult[1], mult[0]);

			memcpy(buffLine, &code, pix->cpp);
		}
	}
}

void WDirect3D8::UpdateTextureSurface(void* buff, int width, int height,
	int pitch, pix_info* pix, BITMAPINFO* bi, void* data, int type)
{
	if (type & 0x20000)
	{
		UpdateTextureSurfaceNormal(buff, pitch, pix, bi, data);
	}
	else if (!(type & 0x800) &&
		((type & 0x80000) ? width >= bi->bmiHeader.biWidth &&
					height >= bi->bmiHeader.biHeight
						  : width == bi->bmiHeader.biWidth &&
					height == bi->bmiHeader.biHeight))
	{
		UpdateTextureSurfaceDirect(buff, pitch, pix, bi, data);
	}
	else if (!(type & 0x800) &&
		(bi->bmiHeader.biBitCount == 8 || bi->bmiHeader.biBitCount == 24))
	{
		UpdateTextureSurfaceFiltering(buff, width, height, pitch, pix, bi,
			static_cast<unsigned char*>(data));
	}
	else
	{
		UpdateTextureSurfaceSampling(buff, width, height, pitch, pix, bi, data,
			type);
	}
}

void WDirect3D8::UpdateTextureSurface(int texHandle, tagBITMAPINFO* bi,
	void* data, int type)
{
	d3d8_texture* tex = &m_texList[texHandle];
	if (tex)
	{
		if (!tex->bitmap)
		{
			Bitmap bitmap;
			bitmap.SetBITMAPINFO(bi, static_cast<unsigned char*>(data));
			tex->bitmap = new Bitmap();
			*tex->bitmap = bitmap;
		}
		else if (bi->bmiHeader.biWidth == tex->bitmap->Width() &&
			bi->bmiHeader.biHeight == tex->bitmap->Height() &&
			bi->bmiHeader.biBitCount == tex->bitmap->BitsPerPixel())
		{
			memcpy(tex->bitmap->GetVram(0), data,
				tex->bitmap->Height() * tex->bitmap->pitch);
		}
		tex->updateType = type;
		tex->needToBeFilled = true;
	}
}

void WDirect3D8::DestroyTexture(int texHandle)
{
	if (texHandle == (this->m_lastTexState & 0x7FF) ||
		texHandle == (this->m_lastTexState >> 11 & 0x7F))
	{
		this->FlushRenderPrimitive();
	}

	if (this->m_texList[texHandle].pSurf)
	{
		this->m_texList[texHandle].pSurf->Release();
		this->m_texList[texHandle].pSurf = 0;
	}

	if (this->m_texList[texHandle].pTex)
	{
		this->m_texList[texHandle].pTex->Release();
		this->m_texList[texHandle].pTex = 0;
	}

	if (this->m_texList[texHandle].bitmap)
	{
		delete this->m_texList[texHandle].bitmap;
		this->m_texList[texHandle].bitmap = 0;
	}

	if (this->m_texList[texHandle].dxtcData)
	{
		delete[] this->m_texList[texHandle].dxtcData;
		this->m_texList[texHandle].dxtcData = 0;
	}

	memset(&this->m_texList[texHandle], 0, sizeof(d3d8_texture));
	WDirect3D::DestroyTexture(texHandle);
}

void WDirect3D8::FixTexturePart(int texHandle, const tagRECT& rc,
	tagBITMAPINFO* src, void* data, int type)
{
	d3d8_texture* tex = &m_texList[texHandle];

	if (!tex || !tex->bitmap)
	{
		return;
	}

	int cps = tex->bitmap->bi->bmiHeader.biBitCount >> 3;
	signed char* vramptr =
		(signed char*)&tex->bitmap
			->vram[tex->bitmap->pitch * rc.top + cps * rc.left];
	if (type & 0x40000)
	{
		if (src->bmiHeader.biBitCount == 32 &&
			(tex->bitmap->bi->bmiHeader.biBitCount == 24 ||
				tex->bitmap->bi->bmiHeader.biBitCount == 32))
		{
			this->UpdateTextureSurfaceAlpha(vramptr, tex->bitmap->pitch,
				FindPixInfoByFormat(tex->bitmap->bi->bmiHeader.biBitCount == 32
						? D3DFMT_A8R8G8B8
						: D3DFMT_R8G8B8),
				rc, src, data);
		}
	}
	else if (tex->bitmap->bi->bmiHeader.biBitCount == src->bmiHeader.biBitCount)
	{
		int srcPitch = cps * src->bmiHeader.biWidth + 3 & ~3;
		for (int i = 0; i < rc.bottom - rc.top; i++)
		{
			memcpy(&vramptr[i * tex->bitmap->pitch],
				static_cast<unsigned char*>(data) + i * srcPitch,
				cps * src->bmiHeader.biWidth);
		}
	}

	tex->updateType = type | (tex->updateType & 0x80000);
	tex->needToBeFilled = true;
}

int WDirect3D8::Command(wVDevMessage message, int param1, int param2)
{
	int numTextures;
	int numVb;
	int numIb;
	int devId;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	FILETIME ftAccess;
	FILETIME ftWrite;
	FILETIME ftCreate;
	SYSTEMTIME stCreate;
	FILETIME ftLocal;
	D3DADAPTER_IDENTIFIER9 identifier;
	char filename[MAX_PATH];

	switch (message)
	{
	case W_VDEV_SETHWND:

		this->m_hwnd = (HWND)param1;
		break;

	case W_VDEV_CAPTURE_SCREEN:

		return this->CaptureScreen((Bitmap*)param2) != 0;
	case W_VDEV_GET_FRONTSURFACE:

		break;
	case W_VDEV_GET_BACKSURFACE:

		break;
	case W_VDEV_DRAWBOX:

		break;
	case W_VDEV_SETRECT:

		break;
	case W_VDEV_FLIPFRONTSURFACE:

		break;
	case W_VDEV_OVERDRAW_ANALYZE:

		break;
	case W_VDEV_GET_ZBUFF_HISTOGRAM:

		break;
	case W_VDEV_UPDATE_LOD_TEXTURE:

		break;
	case W_VDEV_GETAVAIL_VRAM:

		break;
	case W_VDEV_FLUSH:

		this->Flush(param1);
		break;
	case W_VDEV_GET_CAPTURE_MODE:

		*(unsigned long*)param1 = g_captureOption.currentMode;
		break;
	case W_VDEV_SET_CAPTURE_MODE:

		g_captureOption.SetMode(
			static_cast<sCpatureOption::eCaptureMode>(param1), param2 == 1);

	case W_VDEV_RELEASE_CAPTURERESOURCE:

		this->ReleaseCopiedScreenResource();
		break;
	case W_VDEV_CAPTURED_BG:
		if (param1 == 1)
		{
			if (!m_useCopiedScreen)
				return BeginCapturedBackground();
		}
		else
			m_useCopiedScreen = false;
		break;
	case W_VDEV_SCREEN_SHOT:

		return this->ScreenShot((const char*)param1,
			static_cast<D3DXIMAGE_FILEFORMAT>(param2));
	case W_VDEV_GET_SPLASH:

		return reinterpret_cast<int>(new WSplashD3D(this));
	case W_VDEV_GET_COPIEDSCREENSPLASH:
		return (int)m_pCopiedScreenSplash;
	case W_VDEV_SET_DDS_RES:

		this->m_iDDSRes = param1 <= 0 ? 0 : (param1 >= 5 ? 5 : param1);
		this->m_useHiQualityTex = this->m_iDDSRes == 0;
		break;
	case W_VDEV_USE_MIPMAP:

		this->m_bUseMipmap = param1 != 0;
		break;
	case W_VDEV_MAX_MIPLVL:

		this->m_iMaxMipLvl = param1 <= 0 ? 0 : param1;
		break;
	case W_VDEV_MIP_CREATE_FILTER:

		if (param1 >= 1 && param1 <= 5)
			this->m_dwMipCreateFilter = param1;
		break;
	case W_VDEV_MIP_TEXSTAGE_FILTER:

		if (param1 >= 0 && param1 <= 3)
		{
			this->m_dwMipTexStateFilter = param1;
			this->_SetSamplerState(0, D3DSAMP_MIPFILTER, param1);
		}
		break;
	case W_VDEV_NUM_TEXTURES:

		numTextures = 0;
		for (int i = 0; i < 2048; i++)
		{
			if (this->m_texList[i].pTex)
			{
				numTextures++;
			}
		}
		*reinterpret_cast<unsigned long*>(param1) = numTextures;
		break;
	case W_VDEV_NUM_TNL_BUFFERS:

		numVb = 0;
		for (int i = 0; i < 1024; i++)
		{
			if (this->m_xaVbList[i].xpVb)
			{
				numVb++;
			}
		}
		numIb = 0;
		for (int i = 0; i < 256; i++)
		{
			if (this->m_xaIbList[i].xpIb)
			{
				numIb++;
			}
		}
		*reinterpret_cast<unsigned long*>(param1) = numVb;
		*reinterpret_cast<unsigned long*>(param2) = numIb;
		break;
	case WX_VDEV_GET_STATISTICS:

		*reinterpret_cast<unsigned long*>(param1 + 0) = this->m_xnTotalTris;
		*reinterpret_cast<unsigned long*>(param1 + 4) = this->m_xnDPUPs;
		*reinterpret_cast<unsigned long*>(param1 + 8) = this->m_xnDIPUPs;
		*reinterpret_cast<unsigned long*>(param1 + 12) = this->m_xnDPs;
		*reinterpret_cast<unsigned long*>(param1 + 16) = this->m_xnDIPs;
		break;
	case WX_VDEV_GET_CAPS:

		this->xGetCaps(param1, reinterpret_cast<unsigned long*&>(param2));
		break;
	case WX_VDEV_FILLMODE:

		if (this->m_pd3dDevice)
		{
			switch (param1)
			{
			case 0:
				this->_SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				break;
			case 1:
				this->_SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
				break;
			case 2:
				this->_SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
				break;
			default:

				break;
			}
		}
		break;
	case WX_VDEV_DRIVER_INFO:
	{
		memset(&identifier, 0, sizeof identifier);
		if (this->m_d3d8->GetAdapterIdentifier(m_devId, 0, &identifier) != S_OK)
		{
			return 0;
		}
		if (!*identifier.Description)
		{
			return 0;
		}
		strcpy(reinterpret_cast<char*>(param1), identifier.Description);
		for (unsigned char i = 0; i < 4; ++i)
		{
			sprintf(filename, "%s%s", DriverPath[i], identifier.Driver);
			hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, 0,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
				CloseHandle(hFile);
				FileTimeToLocalFileTime(&ftCreate, &ftLocal);
				FileTimeToSystemTime(&ftLocal, &stCreate);
				*reinterpret_cast<unsigned long*>(param2 + 0) =
					identifier.WHQLLevel;
				*reinterpret_cast<unsigned long*>(param2 + 4) = stCreate.wYear;
				*reinterpret_cast<unsigned long*>(param2 + 8) = stCreate.wMonth;
				*reinterpret_cast<unsigned long*>(param2 + 12) = stCreate.wDay;
				return 0;
			}
		}
		return 0;
	}
	case WX_VDEV_GET_SORTBUFFER_SIZE:

		*reinterpret_cast<unsigned long*>(param1 + 0) =
			this->GetSortBufferSize();
		*reinterpret_cast<unsigned long*>(param1 + 4) =
			this->GetSortBufferSwSize();
		*reinterpret_cast<unsigned long*>(param1 + 8) =
			this->GetSortBufferHwSize();
		break;
	case WX_VDEV_GET_MERGEBUFFER_SIZE:

		*reinterpret_cast<unsigned long*>(param1) =
			this->m_MergeBuffer.GetVertexStreamBufferSize();
		*reinterpret_cast<unsigned long*>(param1 + 4) =
			this->m_MergeBuffer.GetIndexBufferSize();
		break;
	default:

		break;
	}
	return 0;
}

char* WDirect3D8::GetDeviceName()
{
	if (this->m_devName)
	{
		return this->m_devName;
	}
	return "";
}

char* WDirect3D8::DuplicateString(const char* str)
{
	const size_t bufLen = strlen(str) + 1;
	char* result = new char[bufLen];
	strcpy(result, str);
	return result;
}

bool WDirect3D8::GetDevName(const D3DDISPLAYMODE& mode, char* bufptr,
	int buflen)
{
	int i;
	for (i = 4; i >= 0; --i)
		if (ms_fmtList[i].format == mode.Format)
			break;
	if (i >= 0)
	{
		sprintf(bufptr, "w%d h%d b%d", mode.Width, mode.Height,
			ms_fmtList[i].bitNum);
		return false;
	}
	return true;
}

char* WDirect3D8::EnumModeName()
{
	char *srcStr, *dstStr;
	D3DCAPS9 caps;
	D3DDISPLAYMODE mode;
	IDirect3D9* d3d8;
	WList<char*> list(16, 16);
	size_t len = 0;
	D3DFORMAT fmt;
	char* modeName;
	char temp[128];

	if (m_devId >= 0)
	{
		if (!m_modList)
		{
			d3d8 = Direct3DCreate9(D3D_SDK_VERSION);
			if (!d3d8)
			{
				g_error = g_msgD3DInitFailed;
				return 0;
			}

			if (d3d8->GetDeviceCaps(this->m_devId, D3DDEVTYPE_HAL, &caps))
			{
				d3d8->Release();
				return 0;
			}

			D3DFORMAT formatList[3] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5,
				D3DFMT_X8R8G8B8 };
			fmt = D3DFMT_X8R8G8B8;
			for (unsigned iIndex = 0; iIndex < 3; ++iIndex)
			{
				if (m_d3d8->GetAdapterModeCount(0, formatList[iIndex]))
				{
					fmt = formatList[iIndex];
					break;
				}
			}

			for (UINT i = 0; i < d3d8->GetAdapterModeCount(this->m_devId, fmt);
				i++)
			{
				d3d8->EnumAdapterModes(this->m_devId, fmt, i, &mode);
				if (!this->GetDevName(mode, temp, 128) && !list.Find(temp))
				{
					modeName = this->DuplicateString(temp);
					list.AddItem(modeName, modeName, false);
					len += strlen(modeName) + 1;
				}
			}

			this->m_modList = new char[len + 1];
			modeName = list.Start();
			size_t i = 0;
			for (; modeName; modeName = list.Next())
			{
				strcpy(m_modList + i, modeName);
				i += strlen(m_modList + i) + 1;
				delete[] modeName;
			}
			this->m_modList[i] = 0;
			d3d8->Release();
		}
		return this->m_modList;
	}
	return 0;
}

bool WDirect3D8::GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor)
{
	D3DDISPLAYMODE d3ddm;

	if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK)
	{
		return false;
	}

	iWidth = d3ddm.Width;
	iHeight = d3ddm.Height;

	switch (d3ddm.Format)
	{
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		iColor = 32;
		break;
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		iColor = 16;
	default:

		break;
	}
	return true;
}

bool WDirect3D8::IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight,
	int iColor)
{
	D3DDISPLAYMODE d3ddm;
	char modeName[64];
	char temp[64];

	sprintf(modeName, "w%d h%d b%d", iWidth, iHeight, iColor);
	if (bWindowed)
	{
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK)
		{
			return false;
		}
		switch (d3ddm.Format)
		{
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			if (iColor == 16)
			{
				return false;
			}
			break;
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			if (iColor == 32)
			{
				return false;
			}
		default:
			break;
		}
		return true;
	}
	D3DFORMAT fmt = D3DFMT_X8R8G8B8;
	if (iColor == 16)
	{
		if (this->m_d3d8->GetAdapterModeCount(this->m_devId, D3DFMT_R5G6B5) !=
			0)
		{
			fmt = D3DFMT_R5G6B5;
		}
		else
		{
			fmt = D3DFMT_X1R5G5B5;
		}
	}
	for (UINT i = 0; i < this->m_d3d8->GetAdapterModeCount(this->m_devId, fmt);
		i++)
	{
		this->m_d3d8->EnumAdapterModes(this->m_devId, fmt, i, &d3ddm);
		if (this->GetDevName(d3ddm, temp, 64))
		{
			continue;
		}
		if (strcmp(modeName, temp) != 0)
		{
			continue;
		}
		if (d3ddm.RefreshRate < 0x3C && d3ddm.RefreshRate)
		{
			continue;
		}
		return true;
	}
	return false;
}

bool WDirect3D8::Reset(bool bWindowed, int iWidth, int iHeight, int iColor,
	long lWndStyle, int fillMode)
{
	D3DDISPLAYMODE d3ddm;
	char modeName[64];
	char temp[64];

	if (!bWindowed)
	{
		fillMode = 0;
	}

	bool switchWinMode = false;
	if (m_d3dpp.Windowed != bWindowed)
		switchWinMode = true;

	this->m_fillScrMode = fillMode > 0;
	m_lWndStyle = lWndStyle;
	SetWindowLongA(this->m_hwnd, GWL_STYLE, lWndStyle);
	SetWindowLongA(this->m_hwnd, GWL_STYLE,
		GetWindowLongA(this->m_hwnd, GWL_STYLE) | WS_VISIBLE);
	if (bWindowed)
	{
		int newBpp = this->m_BackBufBpp;
		if (this->m_d3dpp.Windowed)
		{
			if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != 0)
			{
				g_error =
					"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc\x20\xb8\xf0\xb5\xe5\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
				return false;
			}
			this->m_fmtWindowed = d3ddm.Format;
			newBpp = GetBackBufferBpp(d3ddm.Format);
		}
		g_formatChanged = (iWidth != this->m_d3dpp.BackBufferWidth ||
			iHeight != this->m_d3dpp.BackBufferHeight ||
			this->m_BackBufBpp != newBpp || this->m_d3dpp.Windowed == 0);
		this->m_BackBufBpp = newBpp;
		this->m_d3dpp.Windowed = 1;
		this->m_bWindow = true;
		this->m_d3dpp.BackBufferWidth = iWidth;
		this->m_d3dpp.BackBufferHeight = iHeight;
		this->m_d3dpp.BackBufferFormat = this->m_fmtWindowed;
		this->m_d3dpp.SwapEffect = g_captureOption.SwapEffect;
		this->m_d3dpp.FullScreen_RefreshRateInHz = 0;
		this->m_d3dpp.PresentationInterval =
			g_captureOption.FullScreen_PresentationInterval;
	}
	else
	{
		D3DDISPLAYMODE t;

		sprintf(modeName, "w%d h%d b%d", iWidth, iHeight, iColor);
		DWORD m;
		const D3DFORMAT _list[3] = { D3DFMT_X8R8G8B8, D3DFMT_R5G6B5,
			D3DFMT_X1R5G5B5 };
		int j = 0;
		m = -1;
		for (; m == -1 && j < 3U; ++j)
		{
			D3DFORMAT fmt = _list[j];
			if (m_d3d8->GetAdapterModeCount(0, fmt))
			{
				for (UINT i = 0;
					i < this->m_d3d8->GetAdapterModeCount(this->m_devId, fmt);
					i++)
				{
					this->m_d3d8->EnumAdapterModes(this->m_devId, fmt, i,
						&d3ddm);
					if (!GetDevName(d3ddm, temp, 64))
					{
						if (!strcmp(modeName, temp) &&
							(d3ddm.RefreshRate >= 60 || !d3ddm.RefreshRate))
						{
							memcpy(&t, &d3ddm, sizeof(t));
							m = i;
							break;
						}
					}
				}
			}
		}

		if (m == -1)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb8\xae\xc7\xc1\xb7\xb9\xbd\xc3\x20\xb7\xb9\xc0\xcc\xc6\xae\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return false;
		}

		this->m_d3dpp.Windowed = 0;
		this->m_bWindow = false;
		this->m_BackBufBpp = iColor;

		float fps;
		if (t.RefreshRate == 0)
		{
			fps = 120.0f;
		}
		else
		{
			fps = static_cast<float>(t.RefreshRate);
		}

		this->m_fps = fps;
		this->m_d3dpp.BackBufferWidth = t.Width;
		this->m_d3dpp.BackBufferHeight = t.Height;
		this->m_d3dpp.BackBufferFormat = t.Format;
		this->m_d3dpp.SwapEffect = g_captureOption.SwapEffect;
		this->m_d3dpp.FullScreen_RefreshRateInHz = t.RefreshRate;
		this->m_d3dpp.PresentationInterval =
			g_captureOption.FullScreen_PresentationInterval;
	}
	D3DFORMAT backBufferFormat = this->m_d3dpp.BackBufferFormat;

	D3DFORMAT stencilFormat = this->FindDepthBufferFormat(backBufferFormat);
	this->m_d3dpp.AutoDepthStencilFormat = stencilFormat;

	if (!this->xReset(switchWinMode))
	{
		Sleep(100);
		if (!this->xReset(switchWinMode))
		{
			return false;
		}
	}
	if (bWindowed)
	{
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != 0)
		{
			g_error =
				"\xc0\xfb\xc0\xfd\xc7\xd1\x20\xb5\xf0\xbd\xba\xc7\xc3\xb7\xb9\xc0\xcc\x20\xb8\xf0\xb5\xe5\xb8\xa6\x20\xbe\xf2\xbe\xee\x20\xbf\xc3\x20\xbc\xf6\x20\xbe\xf8\xbd\xc0\xb4\xcf\xb4\xd9";
			return false;
		}
		m_fps = !d3ddm.RefreshRate ? 120.0f : (float)d3ddm.RefreshRate;
		this->m_fmtWindowed = d3ddm.Format;
	}

	if (g_captureOption.updateWholeScreen)
	{
		this->m_pd3dDevice->BeginScene();
		this->m_pd3dDevice->Clear(0, 0, 3, 0, 0.0, 0);
		this->m_pd3dDevice->EndScene();
		this->Present();
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			this->m_pd3dDevice->BeginScene();
			this->m_pd3dDevice->Clear(0, 0, 3, 0, 0.0, 0);
			this->m_pd3dDevice->EndScene();
			this->Present();
		}
	}
	return true;
}

void WDirect3D8::ReleaseCopiedScreenResource()
{
	if (this->m_pCopiedScreenSurface)
	{
		this->m_pCopiedScreenSurface->Release();
		this->m_pCopiedScreenSurface = 0;
	}
	if (this->m_hCopiedScreenTexture > 0)
	{
		this->DestroyTexture(this->m_hCopiedScreenTexture);
		this->m_hCopiedScreenTexture = 0;
	}
	this->m_useCopiedScreen = false;
}

bool WDirect3D8::BeginRenderToTexture(const WRenderToTextureParam& param)
{
	IDirect3DSurface9* rt;
	D3DVIEWPORT9 viewport;

	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return false;
	}

	if (param.m_rtTexInfo[0].m_hTex <= 0)
	{
		return false;
	}

	const d3d8_texture& tex0 = this->m_texList[param.m_rtTexInfo[0].m_hTex];
	if (!tex0.pSurf)
	{
		return false;
	}

	if (!param.CanClearAtOnce())
	{
		for (int ii = 0; ii < 2; ++ii)
		{
			const WRenderToTextureParam::RtTexInfo& i = param.m_rtTexInfo[ii];
			if (i.m_hTex > 0 && i.m_needToClear)
			{
				this->m_pd3dDevice->SetRenderTarget(0,
					this->m_texList[i.m_hTex].pSurf);
				this->m_pd3dDevice->Clear(0, 0, 1u, i.m_clearClr, 1.0, 0);
			}
		}
	}

	IDirect3DSurface9* pDepth = 0;
	switch (param.m_depthSurfInfo.m_surfUsage)
	{
	case WRenderToTextureParam::DepthSurfInfo::USE_MAIN_SURFACE:
		pDepth = this->m_mainRt.depthSurf;
		break;
	case WRenderToTextureParam::DepthSurfInfo::USE_SHARED_SURFACE:
		pDepth = this->FindDepthSurf(tex0.width, tex0.height);
		break;
	case WRenderToTextureParam::DepthSurfInfo::USE_EXCLUSIVE_SURFACE:
		pDepth = this->m_texList[param.m_depthSurfInfo.m_hTex].pSurf;
		break;
	default:
		break;
	}

	for (int i = 0; i < 2; i++)
	{
		rt = param.m_rtTexInfo[i].m_hTex > 0
			? this->m_texList[param.m_rtTexInfo[i].m_hTex].pSurf
			: 0;

		if (rt != this->m_curRt.rt.surf[i])
		{
			this->m_pd3dDevice->SetRenderTarget(i, rt);
		}
	}

	if (pDepth != this->m_curRt.depthSurf)
	{
		this->m_pd3dDevice->SetDepthStencilSurface(pDepth);
	}

	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = tex0.width;
	viewport.Height = tex0.height;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;

	if (memcmp(&viewport, &this->m_curRt.viewport, 0x18) != 0)
	{
		this->m_pd3dDevice->SetViewport(&viewport);
	}

	unsigned long clearFlags = 0;
	if (param.CanClearAtOnce() && param.m_rtTexInfo[0].m_needToClear)
	{
		clearFlags = D3DCLEAR_TARGET;
	}

	if (pDepth && param.m_depthSurfInfo.m_needToClear)
	{
		clearFlags |= D3DCLEAR_ZBUFFER;
	}

	if (clearFlags)
	{
		this->m_pd3dDevice->Clear(0, 0, clearFlags,
			param.m_rtTexInfo[0].m_clearClr, param.m_depthSurfInfo.m_clearZ, 0);
	}

	this->BackupRenderTarget(param, pDepth, viewport);

	return true;
}

void WDirect3D8::EndRenderToTexture(const WRenderToTextureParam& param)
{
	if (this->m_devState != 1 && param.m_rtTexInfo[0].m_hTex > 0 &&
		this->m_texList[param.m_rtTexInfo[0].m_hTex].pSurf)
	{
		this->RestoreBackedupRenderTarget();
		if (param.m_rtTexInfo[0].m_hTex > 0)
		{
			this->m_texList[param.m_rtTexInfo[0].m_hTex].isFilled = true;
		}
		if (param.m_rtTexInfo[1].m_hTex > 0)
		{
			this->m_texList[param.m_rtTexInfo[1].m_hTex].isFilled = true;
		}
	}
}

IDirect3DSurface9* WDirect3D8::FindDepthSurf(WORD w, WORD h)
{
	int i;
	for (i = 0; i < 32; i++)
	{
		if (!this->m_depthSurfList[i].pDepth)
		{
			break;
		}
		if (w == this->m_depthSurfList[i].wWidth &&
			h == this->m_depthSurfList[i].wHeight)
		{
			return this->m_depthSurfList[i].pDepth;
		}
	}
	for (; i < 32; ++i)
	{
		if (m_depthSurfList[i].pDepth)
			continue;
		this->m_depthSurfList[i].wWidth = w;
		this->m_depthSurfList[i].wHeight = h;
		this->m_pd3dDevice->CreateDepthStencilSurface(w, h,
			ms_RtDepthFmt[this->m_RtDepthFmtIdx].fmt, D3DMULTISAMPLE_NONE, 0, 1,
			&this->m_depthSurfList[i].pDepth, 0);
		return this->m_depthSurfList[i].pDepth;
	}
	if (this->m_commonDepthSurf)
	{
		this->m_commonDepthSurf->Release();
	}
	this->m_pd3dDevice->CreateDepthStencilSurface(w, h,
		ms_RtDepthFmt[this->m_RtDepthFmtIdx].fmt, D3DMULTISAMPLE_NONE, 0, 1,
		&this->m_commonDepthSurf, 0);
	return this->m_commonDepthSurf;
}

bool WDirect3D8::BeginCapturedBackground()
{
	pix_info* pixInfo;
	int m_hTex;
	HRESULT hRes;
	bool created;
	D3DLOCKED_RECT lrDst;
	D3DLOCKED_RECT lrSrc;
	IDirect3DSurface9* pRenderTarget;

	this->FlushRenderPrimitive();
	if (fnWD3DDevice_GetRenderTarget(m_pd3dDevice, 0, &pRenderTarget) == S_OK)
	{
		if (pRenderTarget->GetDesc(&this->m_capturedDdsd) == S_OK)
		{
			if (g_captureOption.useTexture)
			{
				if (!this->m_hCopiedScreenTexture)
				{
					pixInfo = FindPixInfoByFormat(this->m_capturedDdsd.Format);
					if (!pixInfo)
						goto failed;

					{
						BITMAPINFO bi;
						memset(&bi, 0, sizeof bi);
						bi.bmiHeader.biWidth = this->m_capturedDdsd.Width;
						bi.bmiHeader.biHeight = this->m_capturedDdsd.Height;
						bi.bmiHeader.biBitCount = WORD(pixInfo->cpp * 8);
						m_hTex = this->CreateTexture(&bi, 0x4000);
					}
					this->m_hCopiedScreenTexture = m_hTex;
					if (m_hTex <= 0)
						goto failed;
					if (!this->m_texList[m_hTex].pTex)
					{
						this->DestroyTexture(m_hTex);
						goto failed;
					}
				}
			}
			else if (!this->m_pCopiedScreenSurface)
			{
				if (g_captureOption.useMemCopy ||
					!g_captureOption.dxCopyRectsOK)
				{
					created = SUCCEEDED(
						hRes = fnWD3DDevice_CreateSurface(this->m_pd3dDevice,
							this->m_capturedDdsd.Width,
							this->m_capturedDdsd.Height,
							this->m_capturedDdsd.Format, D3DPOOL_DEFAULT,
							&this->m_pCopiedScreenSurface, 0));
				}
				else
				{
					created = SUCCEEDED(
						hRes = this->m_pd3dDevice->CreateRenderTarget(
							this->m_capturedDdsd.Width,
							this->m_capturedDdsd.Height,
							this->m_capturedDdsd.Format, D3DMULTISAMPLE_NONE, 0,
							0, &this->m_pCopiedScreenSurface, 0));
				}
				if (!created)
					goto failed;
				__assume(created);
			}
			this->m_useCopiedScreen = true;
			IDirect3DSurface9* pSurf = 0;
			if (g_captureOption.useTexture)
			{
				IDirect3DTexture9* tex =
					this->m_texList[this->m_hCopiedScreenTexture].pTex;
				tex->GetSurfaceLevel(0, &pSurf);
			}
			else
			{
				pSurf = this->m_pCopiedScreenSurface;
			}
			if (!g_captureOption.useMemCopy && g_captureOption.dxCopyRectsOK)
			{
				static RECT rc;
				rc.left = 0;
				rc.top = 0;
				rc.right = this->m_capturedDdsd.Width;
				rc.bottom = this->m_capturedDdsd.Height;
				hRes = fnWD3DDevice_CopyRect(this->m_pd3dDevice, pRenderTarget,
					&rc, pSurf);
				if (hRes == S_OK)
					goto done;
				g_captureOption.dxCopyRectsOK = false;
			}
			pRenderTarget->LockRect(&lrSrc, 0, 0x8810);
			pSurf->LockRect(&lrDst, 0, 0);
			int smallerPitch = Min<int>(lrSrc.Pitch, lrDst.Pitch);
			for (int i = 0; i < (int)this->m_capturedDdsd.Height; i++)
			{
				memcpy(static_cast<char*>(lrDst.pBits) + lrDst.Pitch * i,
					static_cast<char*>(lrSrc.pBits) + lrSrc.Pitch * i,
					smallerPitch);
			}
			pRenderTarget->UnlockRect();
			pSurf->UnlockRect();
done:
			pRenderTarget->Release();
			if (g_captureOption.useTexture)
			{
				pSurf->Release();
			}
			return true;
		}
failed:
		pRenderTarget->Release();
	}
	return false;
}

unsigned long* WDirect3D8::GetZBufferHistogram()
{
	bool ok = false;
	IDirect3DSurface9* pDepthTarget;
	D3DSURFACE_DESC ddsd;
	D3DLOCKED_RECT LockedRect;
	if (m_pd3dDevice->GetDepthStencilSurface(&pDepthTarget) == S_OK)
	{
		if (pDepthTarget->GetDesc(&ddsd) == S_OK &&
			ddsd.Format == D3DFMT_D16_LOCKABLE &&
			pDepthTarget->LockRect(&LockedRect, 0, D3DLOCK_READONLY) == S_OK)
		{
			memset(ms_ZbuffHistogram, 0, sizeof(ms_ZbuffHistogram));
			for (unsigned int y = 0; y < m_d3dpp.BackBufferHeight; ++y)
			{
				unsigned char* row =
					(unsigned char*)LockedRect.pBits + y * LockedRect.Pitch;
				WORD* src = (WORD*)row;
				for (unsigned int x = 0; x < m_d3dpp.BackBufferWidth; ++x)
				{
					WORD z = *src++;
					z >>= 8;
					++ms_ZbuffHistogram[z];
				}
			}
			ok = true;
			pDepthTarget->UnlockRect();
		}
		pDepthTarget->Release();
	}
	return ok ? ms_ZbuffHistogram : 0;
}

HRESULT WDirect3D8::ScreenShot(const char* pDestFilename, int format)
{
	D3DDISPLAYMODE mode;
	IDirect3DSurface9* surf;

	HRESULT result = this->m_pd3dDevice->GetDisplayMode(0, &mode);
	if (FAILED(result))
	{
		return result;
	}

	result = this->m_pd3dDevice->CreateOffscreenPlainSurface(mode.Width,
		mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &surf, 0);
	if (FAILED(result))
	{
		return result;
	}

	result = this->m_pd3dDevice->GetFrontBufferData(0, surf);
	if (FAILED(result))
	{
		surf->Release();
		return result;
	}
	result = D3DXSaveSurfaceToFileA(pDestFilename,
		static_cast<D3DXIMAGE_FILEFORMAT>(format), surf, 0, 0);

	surf->Release();
	return result;
}

bool WDirect3D8::CaptureScreen(Bitmap* bitmap)
{
	bool ok = false;
	IDirect3DSurface9* pRenderTarget;
	D3DSURFACE_DESC ddsd;
	D3DLOCKED_RECT LockedRect;
	if (m_pd3dDevice->GetRenderTarget(0, &pRenderTarget) == S_OK)
	{
		if (pRenderTarget->GetDesc(&ddsd) == S_OK)
		{
			pix_info* pix = FindPixInfoByFormat(ddsd.Format);
			if (pix &&
				pRenderTarget->LockRect(&LockedRect, 0, D3DLOCK_READONLY) ==
					S_OK)
			{
				if (pix->cpp == 2)
				{
					for (int y = 0; y < bitmap->bi->bmiHeader.biHeight; ++y)
					{
						for (int x = 0; x < bitmap->bi->bmiHeader.biWidth; ++x)
						{
							DWORD code = *(WORD*)((char*)LockedRect.pBits +
								LockedRect.Pitch * y + pix->cpp * x);
							BYTE g = (code >> pix->g_l_shift) << pix->g_r_shift;
							BYTE r = (code >> pix->r_l_shift) << pix->r_r_shift;
							BYTE b = (code >> pix->b_l_shift) << pix->b_r_shift;
							BYTE* dst =
								bitmap->vram + bitmap->pitch * y + x * 3;
							dst[0] = b;
							dst[1] = g;
							dst[2] = r;
						}
					}
				}
				else if (pix->cpp == 4)
				{
					for (int y = 0; y < bitmap->bi->bmiHeader.biHeight; ++y)
					{
						for (int x = 0; x < bitmap->bi->bmiHeader.biWidth; ++x)
						{
							DWORD code = *(DWORD*)((char*)LockedRect.pBits +
								LockedRect.Pitch * y + pix->cpp * x);
							BYTE g = code >> pix->g_l_shift;
							BYTE r = code >> pix->r_l_shift;
							BYTE b = code >> pix->b_l_shift;
							BYTE* dst =
								bitmap->vram + bitmap->pitch * y + x * 3;
							dst[0] = b;
							dst[1] = g;
							dst[2] = r;
						}
					}
				}
				pRenderTarget->UnlockRect();
				ok = true;
			}
		}
		pRenderTarget->Release();
	}
	return ok;
}

void EnumDirect3D8()
{
	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d)
	{
		D3DCAPS9 d3dcaps;
		D3DADAPTER_IDENTIFIER9 identifier;
		for (UINT i = 0; i < d3d->GetAdapterCount(); ++i)
		{
			memset(&identifier, 0, sizeof(identifier));
			if (d3d->GetAdapterIdentifier(i, 0, &identifier) == S_OK &&
				d3d->GetDeviceCaps(i, D3DDEVTYPE_HAL, &d3dcaps) == S_OK)
				AddVideoDevice(new WDirect3D8(identifier.Description, i));
		}
		d3d->Release();
	}
	else
		g_error = g_msgD3DInitFailed;
}

void WDirect3D8::xGetCaps(unsigned long iItem, unsigned long* pdwCaps)
{
	switch (iItem)
	{
	case 0:
		if ((m_d3dcaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) &&
			(m_d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
			*pdwCaps = 1;
		else
			*pdwCaps = 0;
		break;
	case 1:
		*pdwCaps = 1;
		break;
	case 2:
		*pdwCaps = (this->m_d3dcaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
		break;
	case 3:
		pdwCaps[0] = this->m_d3dcaps.MaxTextureWidth;
		pdwCaps[1] = this->m_d3dcaps.MaxTextureHeight;
		break;
	case 4:
		*pdwCaps = this->m_d3dcaps.MaxTextureBlendStages;
		break;
	case 5:
		*pdwCaps = this->m_d3dcaps.MaxSimultaneousTextures;
		break;
	case 6:
		*pdwCaps = this->m_d3dcaps.MaxVertexBlendMatrices;
		break;
	case 7:
		*pdwCaps = this->m_d3dcaps.MaxVertexBlendMatrixIndex;
		break;
	case 8:
		*pdwCaps =
			reinterpret_cast<unsigned long&>(this->m_d3dcaps.MaxPointSize);
		break;
	case 9:
		*pdwCaps = this->m_d3dcaps.VertexShaderVersion;
		break;
	case 10:
		*pdwCaps = this->m_d3dcaps.PixelShaderVersion;
		break;
	default:

		break;
	}
}

unsigned long WDirect3D8::xDetermineFVF(int iDrawFlag, int iDrawFlag2,
	int iMaxBoneNum)
{
	unsigned long result;
	if (iDrawFlag2 & 0x1000)
	{
		switch (iMaxBoneNum)
		{
		case 1:
			result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB1;
			break;
		case 2:
			result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB2;
			break;
		case 3:
			result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB3;
			break;
		case 4:
			result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB4;
			break;
		default:
			result = D3DFVF_XYZ;
		}
	}
	else
	{
		result = D3DFVF_XYZ;
	}
	if (!(iDrawFlag2 & 0x14))
	{
		result |= D3DFVF_NORMAL;
	}
	if (!(iDrawFlag & 0x40000000) && !(iDrawFlag2 & 0x2000))
	{
		result |= D3DFVF_DIFFUSE;
	}

	if (iDrawFlag & 0x3F800)
	{
		result |= D3DFVF_TEX2;
	}
	else
	{
		result |= D3DFVF_TEX1;
	}

	return result;
}

unsigned long WDirect3D8::xGetFVF(int hVb)
{
	return this->m_xaVbList[hVb].xdwFVF;
}

unsigned char WDirect3D8::xGetStride(int hVb)
{
	return this->m_xaVbList[hVb].xbStride;
}

bool WDirect3D8::xHasVertexElem(unsigned long dwFVF, wWxVertexElem elem) const
{
	switch (elem)
	{
	case WX_VELEM_POSITION:
		if ((dwFVF & D3DFVF_POSITION_MASK) != 0)
			return true;
		return false;
	case WX_VELEM_NORMAL:
		return (dwFVF & D3DFVF_NORMAL) ? true : false;
	case WX_VELEM_DIFFUSE:
		return (dwFVF & D3DFVF_DIFFUSE) ? true : false;
	case WX_VELEM_TEX1:
		if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1)
			return true;
		return false;
	case WX_VELEM_TEX2:
		if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX2)
			return true;
		return false;
	default:
		return false;
	}
}

int WDirect3D8::xGetVertexElemOffset(unsigned long dwFVF,
	wWxVertexElem elem) const
{
	if (elem == WX_VELEM_POSITION)
		return 0;
	else
	{
		BYTE off = 0;
		switch (dwFVF & D3DFVF_POSITION_MASK)
		{
		case D3DFVF_XYZ:
			off = 12;
			break;
		case D3DFVF_XYZRHW:
			off = 16;
			break;
		case D3DFVF_XYZB1:
			off = 16;
			break;
		case D3DFVF_XYZB2:
			off = 20;
			break;
		case D3DFVF_XYZB3:
			off = 24;
			break;
		case D3DFVF_XYZB4:
			off = 28;
			break;
		default:
			break;
		}

		if (elem == WX_VELEM_NORMAL)
		{
			if (dwFVF & D3DFVF_NORMAL)
				return off;
		}
		else
		{
			if (dwFVF & D3DFVF_NORMAL)
				off += 12;
			if (elem == WX_VELEM_DIFFUSE)
			{
				if (dwFVF & D3DFVF_DIFFUSE)
					return off;
			}
			else
			{
				if (dwFVF & D3DFVF_DIFFUSE)
					off += 4;
				if (elem == WX_VELEM_TEX1)
				{
					if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1)
						return off;
				}
				else
				{
					if (((dwFVF & D3DFVF_TEXCOUNT_MASK) >>
							D3DFVF_TEXCOUNT_SHIFT) >= 1)
						off += 8;
					if (elem == WX_VELEM_TEX2 &&
						((dwFVF & D3DFVF_TEXCOUNT_MASK) >>
							D3DFVF_TEXCOUNT_SHIFT) >= 2)
						return off;
				}
			}
		}
		return -1;
	}
}

int WDirect3D8::xGetBlendWeightSize(unsigned long dwFVF) const
{
	int result = 0;
	switch (dwFVF & D3DFVF_POSITION_MASK)
	{
	case D3DFVF_XYZ:
	case D3DFVF_XYZRHW:
		result = 0;
		break;
	case D3DFVF_XYZB1:
		result = 4;
		break;
	case D3DFVF_XYZB2:
		result = 8;
		break;
	case D3DFVF_XYZB3:
		result = 12;
		break;
	case D3DFVF_XYZB4:
		result = 16;
		break;
	default:
		break;
	}
	if (dwFVF & D3DFVF_LASTBETA_UBYTE4)
	{
		result -= 4;
	}
	return result;
}

void WDirect3D8::xCreateVertexBuffer(int hVb, int numVertices,
	unsigned long dwFVF, unsigned long dwUsage)
{
	unsigned int vertexSize = this->VertexSize(dwFVF);
	this->m_xaVbList[hVb].xdwUsage = dwUsage;
	this->m_xaVbList[hVb].xbStride = vertexSize;
	this->m_xaVbList[hVb].xdwFVF = dwFVF;
	this->m_xaVbList[hVb].xnVtxs = numVertices;
	this->m_xaVbList[hVb].xbNeedToBeFilled = true;
	this->m_xaVbList[hVb].xpVertexData =
		new unsigned char[numVertices * vertexSize];
	this->m_xaVbList[hVb].xpVb = 0;
}

void WDirect3D8::xCreateIndexBuffer(int hIb, int numIndices,
	unsigned long dwUsage)
{
	this->m_xaIbList[hIb].xdwUsage = dwUsage;
	this->m_xaIbList[hIb].xnIdxs = numIndices;
	this->m_xaIbList[hIb].xbNeedToBeFilled = true;
	this->m_xaIbList[hIb].xpIndexData = new WORD[numIndices];
	this->m_xaIbList[hIb].xpIb = 0;
}

unsigned long WDirect3D8::xDetermineBufferUsage(unsigned long dwFvf)
{
	const DWORD maxVtxBlendMtx = this->m_d3dcaps.MaxVertexBlendMatrices;
	const DWORD vtxBlendMtxIndex = this->m_d3dcaps.MaxVertexBlendMatrixIndex;
	if (this->m_xdwDevBehavior & D3DCREATE_MIXED_VERTEXPROCESSING &&
		dwFvf & 0x1000 && (maxVtxBlendMtx < 4 || vtxBlendMtxIndex < 0x80))
	{
		return 16;
	}
	return 0;
}

void WDirect3D8::xReleaseVertexBuffer(int hVb)
{
	if (hVb <= 0)
	{
		return;
	}
	if (hVb == this->m_xhLastVb)
	{
		this->FlushRenderPrimitive();
	}
	this->m_xiVbSize -=
		this->m_xaVbList[hVb].xnVtxs * this->m_xaVbList[hVb].xbStride;
	if (this->m_xaVbList[hVb].xpVertexData)
	{
		delete[] this->m_xaVbList[hVb].xpVertexData;
		this->m_xaVbList[hVb].xpVertexData = 0;
	}
	if (this->m_xaVbList[hVb].xpVb)
	{
		this->m_xaVbList[hVb].xpVb->Release();
		this->m_xaVbList[hVb].xpVb = 0;
	}
	memset(&this->m_xaVbList[hVb], 0, sizeof(sVb8));
	WDirect3D::xReleaseVertexBuffer(hVb);
}

void WDirect3D8::xReleaseIndexBuffer(int hIb)
{
	if (hIb <= 0)
	{
		return;
	}

	if (hIb == this->m_xhLastIb)
	{
		this->FlushRenderPrimitive();
	}

	if (this->m_xaIbList[hIb].xpIndexData)
	{
		delete[] this->m_xaIbList[hIb].xpIndexData;
		this->m_xaIbList[hIb].xpIndexData = 0;
	}

	IDirect3DIndexBuffer9*& buffer = m_xaIbList[hIb].xpIb;
	if (buffer)
	{
		buffer->Release();
		buffer = 0;
	}

	memset(&buffer, 0, sizeof(buffer));
	WDirect3D::xReleaseIndexBuffer(hIb);
}

unsigned char* WDirect3D8::xLockVertexBuffer(int hVb, UINT uiOffset,
	UINT uiSize)
{
	this->m_xaVbList[hVb].xbNeedToBeFilled = true;
	return &reinterpret_cast<unsigned char*>(
		this->m_xaVbList[hVb].xpVertexData)[uiOffset];
}

void WDirect3D8::xUnlockVertexBuffer(int hVb)
{
}

unsigned char* WDirect3D8::xLockIndexBuffer(int hIb, UINT uiOffset, UINT uiSize)
{
	this->m_xaIbList[hIb].xbNeedToBeFilled = true;
	return &reinterpret_cast<unsigned char*>(
		this->m_xaIbList[hIb].xpIndexData)[uiOffset];
}

void WDirect3D8::xUnlockIndexBuffer(int hIb)
{
}

void WDirect3D8::xSetTransform(WVDTRANSFORMSTATETYPE state,
	const WMatrix4& matrix)
{
	this->_SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(state),
		(const D3DXMATRIX&)matrix);
}

void WDirect3D8::xSetPrevViewTransform(const WMatrix4& matrix)
{
	memcpy(&this->m_xPrevViewMatrix, &matrix, sizeof(WMatrix4));
}

void WDirect3D8::xSetLight(unsigned long index, const LightSet& wLight)
{
	if (!wLight.type)
	{
		return;
	}

	if (wLight.type == this->m_xaWLights[index].type &&
		wLight.nearone.x == this->m_xaWLights[index].nearone.x &&
		wLight.nearone.y == this->m_xaWLights[index].nearone.y &&
		wLight.nearone.z == this->m_xaWLights[index].nearone.z &&
		wLight.diffuse == this->m_xaWLights[index].diffuse &&
		wLight.ambient == this->m_xaWLights[index].ambient)
	{
		return;
	}

	this->FlushRenderPrimitive();

	this->m_xaWLights[index].type = wLight.type;
	this->m_xaWLights[index].nearone.x = wLight.nearone.x;
	this->m_xaWLights[index].nearone.y = wLight.nearone.y;
	this->m_xaWLights[index].nearone.z = wLight.nearone.z;
	this->m_xaWLights[index].diffuse = wLight.diffuse;
	this->m_xaWLights[index].ambient = wLight.ambient;

	this->m_xaLights[index].Type =
		wLight.type == 2 ? D3DLIGHT_DIRECTIONAL : D3DLIGHT_POINT;
	this->m_xaLights[index].Ambient.r =
		static_cast<float>(wLight.ambient >> 16 & 0xFF) / 255.f;
	this->m_xaLights[index].Ambient.g =
		static_cast<float>(wLight.ambient >> 8 & 0xFF) / 255.f;
	this->m_xaLights[index].Ambient.b =
		static_cast<float>(wLight.ambient >> 0 & 0xFF) / 255.f;
	this->m_xaLights[index].Diffuse.r =
		static_cast<float>(wLight.diffuse >> 16 & 0xFF) / 255.f;
	this->m_xaLights[index].Diffuse.g =
		static_cast<float>(wLight.diffuse >> 8 & 0xFF) / 255.f;
	this->m_xaLights[index].Diffuse.b =
		static_cast<float>(wLight.diffuse >> 0 & 0xFF) / 255.f;

	if (wLight.type == 2)
	{
		this->m_xaLights[index].Direction.x = wLight.nearone.x;
		this->m_xaLights[index].Direction.y = wLight.nearone.y;
		this->m_xaLights[index].Direction.z = wLight.nearone.z;
	}
	else
	{
		this->m_xaLights[index].Position.x = wLight.nearone.x;
		this->m_xaLights[index].Position.y = wLight.nearone.y;
		this->m_xaLights[index].Position.z = wLight.nearone.z;
	}

	if (!index)
	{
		this->m_LightDirect = this->m_xaLights[index].Direction;
	}

	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetLight(index, &this->m_xaLights[index]);
}

void WDirect3D8::xDrawIndexedPrimitive(const WxViewState& viewState,
	const WxBatchState& batchState)
{
	static int nTris;
	static D3DPRIMITIVETYPE pt;

	D3DXMATRIX m;

	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}
	if (!(batchState.xiFlag1 & 0x14))
	{
		this->xSetLight(0, viewState.xLight);
	}
	this->_SetTransform(D3DTS_VIEW, *(const D3DXMATRIX*)&viewState.xmView);
	this->_SetTransform(D3DTS_PROJECTION,
		*(const D3DXMATRIX*)&viewState.xmProj);
	if (batchState.xnmTransfs)
	{
		for (int i = 0; i < batchState.xnmTransfs; i++)
		{
			SetD3DMATRIXFromWMatrix(m, *batchState.xpapmW[i]);
			if (batchState.xpapmO)
			{
				D3DXMATRIX mInv;
				SetD3DMATRIXFromWMatrix(mInv, *batchState.xpapmO[i]);
				D3DXMatrixInverse(&mInv, 0, &mInv);
				m = mInv * m;
			}
			this->_SetTransform(D3DTS_WORLDMATRIX(i), m);
		}
	}
	else
	{
		SetD3DMATRIXFromWMatrix(m, batchState.xmW);
		this->_SetTransform(D3DTS_WORLD, m);
	}

	int vtxType = batchState.xiFlag0;
	if (!this->m_fog)
	{
		vtxType &= ~0x8000000u;
	}
	this->SetVtxType(vtxType, batchState.xiFlag1,
		this->m_xaVbList[(unsigned long)batchState.xiFlag1 >> 22].xdwFVF,
		batchState.xdwDiffuse);
	switch (batchState.xiFlag1 & 3)
	{
	case 0:
		pt = D3DPT_TRIANGLELIST;
		nTris = batchState.xnIdxs / 3;
		break;
	case 1:
		pt = D3DPT_TRIANGLESTRIP;
		nTris = batchState.xnIdxs - 2;
		break;
	case 2:
		pt = D3DPT_POINTLIST;
		nTris = batchState.xnIdxs;
		break;
	}
	this->_DrawIndexedPrimitive(pt, batchState.xiBaseVtxIdx, batchState.xnVtxs,
		batchState.xiBaseIdxIdx, nTris);
}

void WDirect3D8::SetShaderSource(const char* shaderSrc)
{
	this->m_Effect = NULL;
	this->ApplyShader();
	this->ReleaseShaderResource();
	if (!IsSupportVS())
		IsSupportPS();
	if (shaderSrc)
	{
		this->m_shadersrc = shaderSrc;
	}
}

inline void WDirect3D8::xSetTnLBuffer(int hVb, int hIb)
{
	if (hVb && hIb)
	{
		bool mustCreateBuf = false;
		if (this->m_xhLastVb != hVb)
		{
			if (this->m_xdwDevBehavior & 0x80)
			{
				unsigned long changed =
					m_xaVbList[hVb].xdwUsage ^ m_xdwLastUsage;
				if (changed)
				{
					this->m_xdwLastUsage = this->m_xaVbList[hVb].xdwUsage;
					if (changed & 0x10)
					{
						this->m_pd3dDevice->SetSoftwareVertexProcessing(
							this->m_xaVbList[hVb].xdwUsage >> 4 & 1);
						mustCreateBuf = true;
					}
				}
			}
			if (!this->m_xaVbList[hVb].xpVb)
			{
				this->xInstantiateVertexBuffer(hVb);
			}
			if (this->m_xaVbList[hVb].xbNeedToBeFilled)
			{
				this->xFillVertexBuffer(hVb);
			}
			this->m_pd3dDevice->SetStreamSource(0, this->m_xaVbList[hVb].xpVb,
				0, this->m_xaVbList[hVb].xbStride);
			this->m_xhLastVb = hVb;
		}
		if (this->m_xhLastIb != hIb || mustCreateBuf)
		{
			if (!this->m_xaIbList[hIb].xpIb)
			{
				this->xInstantiateIndexBuffer(hIb);
			}
			if (this->m_xaIbList[hIb].xbNeedToBeFilled)
			{
				this->xFillIndexBuffer(hIb);
			}
			this->m_pd3dDevice->SetIndices(this->m_xaIbList[hIb].xpIb);
			this->m_xhLastIb = hIb;
		}
	}
	else
	{
		this->m_xhLastVb = hVb;
		this->m_xhLastIb = hIb;
	}
}

inline void WDirect3D8::xSetRenderState(unsigned long dwFlag,
	unsigned long dwDiffuse)
{
	if (!this->m_xbCurHwTnL)
	{
		return;
	}
	if (this->m_xbUseTFactor && dwDiffuse != this->m_xdwTFactor)
	{
		this->m_xdwTFactor = dwDiffuse;
		this->FlushRenderPrimitive();
		this->m_LastTFactor = dwDiffuse;
		this->m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwDiffuse);
	}
	else if (dwDiffuse != this->m_xdwDiffuse)
	{
		this->FlushRenderPrimitive();
		this->m_xdwDiffuse = dwDiffuse;
		this->m_xMaterial.Diffuse.a =
			static_cast<float>(dwDiffuse >> 24 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.r =
			static_cast<float>(dwDiffuse >> 16 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.g =
			static_cast<float>(dwDiffuse >> 8 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.b =
			static_cast<float>(dwDiffuse >> 0 & 0xFF) / 255.f;
		this->m_pd3dDevice->SetMaterial(&this->m_xMaterial);
	}
}

void WDirect3D8::BeginUsingCustomRenderState()
{
	this->FlushRenderPrimitive();
	if (!this->m_CustomRenderState.HasAnyState())
	{
		return;
	}
	this->m_CustomRenderStateBackupList.push(this->m_CustomRenderState);
	this->m_CustomRenderState.Clear();
}

void WDirect3D8::SetCustomRenderState(WVDRENDERSTATETYPE state,
	unsigned long value)
{
	this->m_CustomRenderState.SetRenderState(
		static_cast<D3DRENDERSTATETYPE>(state), value);
}

void WDirect3D8::SetCustomTextureStageState(unsigned long stage,
	WVDTEXTURESTAGESTATETYPE type, unsigned long value)
{
	this->m_CustomRenderState.SetTextureStageState(stage,
		static_cast<D3DTEXTURESTAGESTATETYPE>(type), value);
}

void WDirect3D8::SetCustomTransform(WVDTRANSFORMSTATETYPE state,
	const WMatrix4& matrix)
{
	this->m_CustomRenderState.SetTransform(
		static_cast<D3DTRANSFORMSTATETYPE>(state),
		reinterpret_cast<const D3DXMATRIX&>(matrix));
}

void WDirect3D8::SetCustomTexture(unsigned long stage, int m_hTex)
{
	if (m_hTex < 0)
	{
		return;
	}
	if (m_hTex)
	{
		if (!this->m_texList[m_hTex].pTex)
		{
			this->xInstantiateTexture(m_hTex);
		}

		if (this->m_texList[m_hTex].needToBeFilled)
		{
			this->xFillTexture(m_hTex);
		}
	}
	this->m_CustomRenderState.SetTexture(stage,
		m_hTex > 0 ? this->m_texList[m_hTex].pTex : 0);
}

void WDirect3D8::SetCustomClipPlane(unsigned long index, const WPlane& value)
{
	this->m_CustomRenderState.SetClipPlane(index,
		reinterpret_cast<const D3DXPLANE&>(value));
}

void WDirect3D8::SetCustomSamplerState(unsigned long sampler,
	WVDSAMPLERSTATETYPE type, unsigned long value)
{
	this->m_CustomRenderState.SetSamplerState(sampler,
		static_cast<D3DSAMPLERSTATETYPE>(type), value);
}

void WDirect3D8::SetCustomFxMacro(unsigned long fxMacro)
{
	this->m_CustomRenderState.SetFxMacro(fxMacro);
}

void WDirect3D8::SetCustomFxParamInt(WVDFXPARAMETERTYPE paramType, int value)
{
	this->m_CustomRenderState.SetFxParamInt(paramType, value);
}

void WDirect3D8::SetCustomFxParamVector2(WVDFXPARAMETERTYPE paramType,
	const WVector2D& value)
{
	this->m_CustomRenderState.SetFxParamVector2(paramType,
		reinterpret_cast<const D3DXVECTOR2&>(value));
}

void WDirect3D8::SetCustomFxParamVector3(WVDFXPARAMETERTYPE paramType,
	const WVector& value)
{
	this->m_CustomRenderState.SetFxParamVector3(paramType,
		reinterpret_cast<const D3DXVECTOR3&>(value));
}

void WDirect3D8::SetCustomFxParamVector4(WVDFXPARAMETERTYPE paramType,
	const WVector4& value)
{
	this->m_CustomRenderState.SetFxParamVector4(paramType,
		reinterpret_cast<const D3DXVECTOR4&>(value));
}

void WDirect3D8::SetCustomFxParamMatrix(WVDFXPARAMETERTYPE paramType,
	const WMatrix4& value)
{
	this->m_CustomRenderState.SetFxParamMatrix(paramType,
		reinterpret_cast<const D3DXMATRIX&>(value));
}

void WDirect3D8::SetCustomFxParamTexture(WVDFXPARAMETERTYPE paramType,
	int m_hTex)
{
	if (m_hTex < 0)
	{
		return;
	}
	if (m_hTex)
	{
		if (!this->m_texList[m_hTex].pTex)
		{
			this->xInstantiateTexture(m_hTex);
		}

		if (this->m_texList[m_hTex].needToBeFilled)
		{
			this->xFillTexture(m_hTex);
		}
	}
	this->m_CustomRenderState.SetFxParamTexture(paramType,
		m_hTex > 0 ? this->m_texList[m_hTex].pTex : 0);
}

void WDirect3D8::EndUsingCustomRenderState()
{
	if (this->m_CustomRenderState.HasAnyState())
	{
		this->FlushRenderPrimitive();
		this->m_CustomRenderState.Clear();
	}

	if (this->m_CustomRenderStateBackupList.size() <= 0)
	{
		return;
	}

	this->m_CustomRenderState = this->m_CustomRenderStateBackupList.top();
	this->m_CustomRenderStateBackupList.pop();
}

void* WDirect3D8::SnapShotCustomRenderState()
{
	if (!m_CustomRenderState.HasAnyState())
		return 0;
	if (m_CustomRenderStateSnapShotList.size() == 0)
	{
		m_CustomRenderStateSnapShotList.push_back(m_CustomRenderState);
	}
	else if (!(m_CustomRenderState ==
				 *m_CustomRenderStateSnapShotList.rbegin()))
	{
		m_CustomRenderStateSnapShotList.push_back(m_CustomRenderState);
	}
	return &*m_CustomRenderStateSnapShotList.rbegin();
}

void WDirect3D8::ClearCustomRenderStateSnapShotList()
{
	this->m_CustomRenderStateSnapShotList.clear();
}

void WDirect3D8::ApplyCustomRenderState(const void* customRenderState)
{
	if (customRenderState)
	{
		const WRenderState& crs =
			*static_cast<const WRenderState*>(customRenderState);
		if (crs != this->m_CustomRenderState)
		{
			this->FlushRenderPrimitive();
		}
		this->m_CustomRenderState = crs;
	}
	else if (this->m_CustomRenderState.HasAnyState())
	{
		this->FlushRenderPrimitive();
		this->m_CustomRenderState.Clear();
	}
}

void WDirect3D8::SetViewPort(unsigned long x, unsigned long y, unsigned long w,
	unsigned long h)
{
	D3DVIEWPORT9 viewport;

	if (this->m_devState == W_VDEVSTATE_LOST)
	{
		return;
	}

	viewport.X = x;
	viewport.Y = y;
	viewport.Width = w;
	viewport.Height = h;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;

	if (!memcmp(&viewport, &this->m_curRt.viewport, sizeof(D3DVIEWPORT9)))
	{
		return;
	}

	this->FlushRenderPrimitive();
	m_curRt.viewport = viewport;
	this->m_pd3dDevice->SetViewport(&viewport);
}

void WDirect3D8::BackupMainRenderTarget()
{
	this->m_pd3dDevice->GetRenderTarget(0, &this->m_mainRt.rt.surf[0]);
	this->m_mainRt.rt.surf[1] = 0;
	this->m_pd3dDevice->GetDepthStencilSurface(&this->m_mainRt.depthSurf);
	this->m_pd3dDevice->GetViewport(&this->m_mainRt.viewport);
}

void WDirect3D8::ReleaseMainRenderTarget()
{
	for (int iIndex = 0; iIndex < 2; ++iIndex)
	{
		if (m_mainRt.rt.surf[iIndex])
		{
			m_mainRt.rt.surf[iIndex]->Release();
		}
	}
	if (this->m_mainRt.depthSurf)
	{
		this->m_mainRt.depthSurf->Release();
	}
	m_mainRt.Init();
}

void WDirect3D8::BackupRenderTarget(const WRenderToTextureParam& param,
	IDirect3DSurface9* depthSurf, const _D3DVIEWPORT9& viewport)
{
	this->m_rtBackupList.push(this->m_curRt);
	sRtBackup newRt;
	newRt.rt.surf[0] = param.m_rtTexInfo[0].m_hTex > 0
		? this->m_texList[param.m_rtTexInfo[0].m_hTex].pSurf
		: 0;
	newRt.rt.surf[1] = param.m_rtTexInfo[1].m_hTex > 0
		? this->m_texList[param.m_rtTexInfo[1].m_hTex].pSurf
		: 0;
	newRt.depthSurf = depthSurf;
	newRt.viewport = viewport;
	memcpy(&this->m_curRt, &newRt, sizeof(sRtBackup));
}

void WDirect3D8::RestoreBackedupRenderTarget()
{
	if (m_rtBackupList.size() != 0)
	{
		sRtBackup backedupRt = m_rtBackupList.top();
		m_rtBackupList.pop();
		backedupRt.Restore(m_pd3dDevice, m_curRt);
		m_curRt = backedupRt;
	}
}

void WDirect3D8::ReleaseAllRendertargetBackupResource()
{
	while (this->m_rtBackupList.size() > 0)
	{
		this->m_rtBackupList.pop();
	}
	this->m_curRt.Init();
	this->ReleaseMainRenderTarget();
}
