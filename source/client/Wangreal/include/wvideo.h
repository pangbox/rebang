#pragma once
#include <windows.h>
#include <math.h>
#include "wmath.h"
#include "wscene.h"

enum WVDTRANSFORMSTATETYPE
{
	WVDTS_VIEW = 0x2,
	WVDTS_PROJECTION = 0x3,
	WVDTS_TEXTURE0 = 0x10,
	WVDTS_TEXTURE1 = 0x11,
	WVDTS_TEXTURE2 = 0x12,
	WVDTS_TEXTURE3 = 0x13,
	WVDTS_TEXTURE4 = 0x14,
	WVDTS_TEXTURE5 = 0x15,
	WVDTS_TEXTURE6 = 0x16,
	WVDTS_TEXTURE7 = 0x17,
	WVDTS_FORCE_DWORD = 0x7FFFFFFF,
};

enum wVDevMessage
{
	W_VDEV_SETCLIPPER,
	W_VDEV_ACTIVATEAPP,
	W_VDEV_MOVE,
	W_VDEV_SIZE,
	W_VDEV_SETSIZE,
	W_VDEV_CAPTURE_SCREEN,
	W_VDEV_GET_FRONTSURFACE,
	W_VDEV_GET_BACKSURFACE,
	W_VDEV_DRAWBOX,
	W_VDEV_SETHWND,
	W_VDEV_SETRECT,
	W_VDEV_FLUSH,
	W_VDEV_FLIPFRONTSURFACE,
	W_VDEV_GET_CAPTURE_MODE,
	W_VDEV_SET_CAPTURE_MODE,
	W_VDEV_RELEASE_CAPTURERESOURCE,
	W_VDEV_CAPTURED_BG,
	W_VDEV_OVERDRAW_ANALYZE,
	W_VDEV_GET_ZBUFF_HISTOGRAM,
	W_VDEV_SCREEN_SHOT,
	W_VDEV_GET_SPLASH,
	W_VDEV_GET_COPIEDSCREENSPLASH,
	W_VDEV_UPDATE_LOD_TEXTURE,
	W_VDEV_SET_DDS_RES,
	W_VDEV_USE_MIPMAP,
	W_VDEV_MAX_MIPLVL,
	W_VDEV_MIP_CREATE_FILTER,
	W_VDEV_MIP_TEXSTAGE_FILTER,
	W_VDEV_GETAVAIL_VRAM,
	W_VDEV_NUM_TEXTURES,
	W_VDEV_NUM_TNL_BUFFERS,
	WX_VDEV_GET_STATISTICS,
	WX_VDEV_GET_CAPS,
	WX_VDEV_FILLMODE,
	WX_VDEV_DRIVER_INFO,
	WX_VDEV_GET_SORTBUFFER_SIZE,
	WX_VDEV_GET_MERGEBUFFER_SIZE,
};

enum wWxVertexElem
{
	WX_VELEM_POSITION,
	WX_VELEM_NORMAL,
	WX_VELEM_DIFFUSE,
	WX_VELEM_TEX1,
	WX_VELEM_TEX2,
};

enum WVDTEXTURESTAGESTATETYPE
{
	WVDTSS_COLOROP = 0x1,
	WVDTSS_COLORARG1 = 0x2,
	WVDTSS_COLORARG2 = 0x3,
	WVDTSS_ALPHAOP = 0x4,
	WVDTSS_ALPHAARG1 = 0x5,
	WVDTSS_ALPHAARG2 = 0x6,
	WVDTSS_BUMPENVMAT00 = 0x7,
	WVDTSS_BUMPENVMAT01 = 0x8,
	WVDTSS_BUMPENVMAT10 = 0x9,
	WVDTSS_BUMPENVMAT11 = 0xA,
	WVDTSS_TEXCOORDINDEX = 0xB,
	WVDTSS_BUMPENVLSCALE = 0x16,
	WVDTSS_BUMPENVLOFFSET = 0x17,
	WVDTSS_TEXTURETRANSFORMFLAGS = 0x18,
	WVDTSS_COLORARG0 = 0x1A,
	WVDTSS_ALPHAARG0 = 0x1B,
	WVDTSS_RESULTARG = 0x1C,
	WVDTSS_CONSTANT = 0x20,
	WVDTSS_FORCE_DWORD = 0x7FFFFFFF,
};

enum WVDRENDERSTATETYPE
{
	WVDRS_ZENABLE = 0x7,
	WVDRS_FILLMODE = 0x8,
	WVDRS_SHADEMODE = 0x9,
	WVDRS_ZWRITEENABLE = 0xE,
	WVDRS_ALPHATESTENABLE = 0xF,
	WVDRS_LASTPIXEL = 0x10,
	WVDRS_SRCBLEND = 0x13,
	WVDRS_DESTBLEND = 0x14,
	WVDRS_CULLMODE = 0x16,
	WVDRS_ZFUNC = 0x17,
	WVDRS_ALPHAREF = 0x18,
	WVDRS_ALPHAFUNC = 0x19,
	WVDRS_DITHERENABLE = 0x1A,
	WVDRS_ALPHABLENDENABLE = 0x1B,
	WVDRS_FOGENABLE = 0x1C,
	WVDRS_SPECULARENABLE = 0x1D,
	WVDRS_FOGCOLOR = 0x22,
	WVDRS_FOGTABLEMODE = 0x23,
	WVDRS_FOGSTART = 0x24,
	WVDRS_FOGEND = 0x25,
	WVDRS_FOGDENSITY = 0x26,
	WVDRS_RANGEFOGENABLE = 0x30,
	WVDRS_STENCILENABLE = 0x34,
	WVDRS_STENCILFAIL = 0x35,
	WVDRS_STENCILZFAIL = 0x36,
	WVDRS_STENCILPASS = 0x37,
	WVDRS_STENCILFUNC = 0x38,
	WVDRS_STENCILREF = 0x39,
	WVDRS_STENCILMASK = 0x3A,
	WVDRS_STENCILWRITEMASK = 0x3B,
	WVDRS_TEXTUREFACTOR = 0x3C,
	WVDRS_WRAP0 = 0x80,
	WVDRS_WRAP1 = 0x81,
	WVDRS_WRAP2 = 0x82,
	WVDRS_WRAP3 = 0x83,
	WVDRS_WRAP4 = 0x84,
	WVDRS_WRAP5 = 0x85,
	WVDRS_WRAP6 = 0x86,
	WVDRS_WRAP7 = 0x87,
	WVDRS_CLIPPING = 0x88,
	WVDRS_LIGHTING = 0x89,
	WVDRS_AMBIENT = 0x8B,
	WVDRS_FOGVERTEXMODE = 0x8C,
	WVDRS_COLORVERTEX = 0x8D,
	WVDRS_LOCALVIEWER = 0x8E,
	WVDRS_NORMALIZENORMALS = 0x8F,
	WVDRS_DIFFUSEMATERIALSOURCE = 0x91,
	WVDRS_SPECULARMATERIALSOURCE = 0x92,
	WVDRS_AMBIENTMATERIALSOURCE = 0x93,
	WVDRS_EMISSIVEMATERIALSOURCE = 0x94,
	WVDRS_VERTEXBLEND = 0x97,
	WVDRS_CLIPPLANEENABLE = 0x98,
	WVDRS_POINTSIZE = 0x9A,
	WVDRS_POINTSIZE_MIN = 0x9B,
	WVDRS_POINTSPRITEENABLE = 0x9C,
	WVDRS_POINTSCALEENABLE = 0x9D,
	WVDRS_POINTSCALE_A = 0x9E,
	WVDRS_POINTSCALE_B = 0x9F,
	WVDRS_POINTSCALE_C = 0xA0,
	WVDRS_MULTISAMPLEANTIALIAS = 0xA1,
	WVDRS_MULTISAMPLEMASK = 0xA2,
	WVDRS_PATCHEDGESTYLE = 0xA3,
	WVDRS_DEBUGMONITORTOKEN = 0xA5,
	WVDRS_POINTSIZE_MAX = 0xA6,
	WVDRS_INDEXEDVERTEXBLENDENABLE = 0xA7,
	WVDRS_COLORWRITEENABLE = 0xA8,
	WVDRS_TWEENFACTOR = 0xAA,
	WVDRS_BLENDOP = 0xAB,
	WVDRS_POSITIONDEGREE = 0xAC,
	WVDRS_NORMALDEGREE = 0xAD,
	WVDRS_SCISSORTESTENABLE = 0xAE,
	WVDRS_SLOPESCALEDEPTHBIAS = 0xAF,
	WVDRS_ANTIALIASEDLINEENABLE = 0xB0,
	WVDRS_MINTESSELLATIONLEVEL = 0xB2,
	WVDRS_MAXTESSELLATIONLEVEL = 0xB3,
	WVDRS_ADAPTIVETESS_X = 0xB4,
	WVDRS_ADAPTIVETESS_Y = 0xB5,
	WVDRS_ADAPTIVETESS_Z = 0xB6,
	WVDRS_ADAPTIVETESS_W = 0xB7,
	WVDRS_ENABLEADAPTIVETESSELLATION = 0xB8,
	WVDRS_TWOSIDEDSTENCILMODE = 0xB9,
	WVDRS_CCW_STENCILFAIL = 0xBA,
	WVDRS_CCW_STENCILZFAIL = 0xBB,
	WVDRS_CCW_STENCILPASS = 0xBC,
	WVDRS_CCW_STENCILFUNC = 0xBD,
	WVDRS_COLORWRITEENABLE1 = 0xBE,
	WVDRS_COLORWRITEENABLE2 = 0xBF,
	WVDRS_COLORWRITEENABLE3 = 0xC0,
	WVDRS_BLENDFACTOR = 0xC1,
	WVDRS_SRGBWRITEENABLE = 0xC2,
	WVDRS_DEPTHBIAS = 0xC3,
	WVDRS_WRAP8 = 0xC6,
	WVDRS_WRAP9 = 0xC7,
	WVDRS_WRAP10 = 0xC8,
	WVDRS_WRAP11 = 0xC9,
	WVDRS_WRAP12 = 0xCA,
	WVDRS_WRAP13 = 0xCB,
	WVDRS_WRAP14 = 0xCC,
	WVDRS_WRAP15 = 0xCD,
	WVDRS_SEPARATEALPHABLENDENABLE = 0xCE,
	WVDRS_SRCBLENDALPHA = 0xCF,
	WVDRS_DESTBLENDALPHA = 0xD0,
	WVDRS_BLENDOPALPHA = 0xD1,
	WVDRS_FORCE_DWORD = 0x7FFFFFFF,
};

enum WVDSAMPLERSTATETYPE
{
	WVDSAMP_ADDRESSU = 0x1,
	WVDSAMP_ADDRESSV = 0x2,
	WVDSAMP_ADDRESSW = 0x3,
	WVDSAMP_BORDERCOLOR = 0x4,
	WVDSAMP_MAGFILTER = 0x5,
	WVDSAMP_MINFILTER = 0x6,
	WVDSAMP_MIPFILTER = 0x7,
	WVDSAMP_MIPMAPLODBIAS = 0x8,
	WVDSAMP_MAXMIPLEVEL = 0x9,
	WVDSAMP_MAXANISOTROPY = 0xA,
	WVDSAMP_SRGBTEXTURE = 0xB,
	WVDSAMP_ELEMENTINDEX = 0xC,
	WVDSAMP_DMAPOFFSET = 0xD,
	WVDSAMP_FORCE_DWORD = 0x7FFFFFFF,
};

enum wVDevState
{
	W_VDEVSTATE_NORMAL,
	W_VDEVSTATE_LOST,
	NUM_W_VDEVSTATES
};

enum WFxParameterType
{
	WFxParamWorldViewProjection = 0x0,
	WFxParamWorld = 0x1,
	WFxParamWorldView = 0x2,
	WFxParamPrevView = 0x3,
	WFxParamView = 0x4,
	WFxParamProjection = 0x5,
	WFxParamViewProjection = 0x6,
	WFxParamConstColor = 0x7,
	WFxParamLightDiffuse = 0x8,
	WFxParamLightAmbient = 0x9,
	WFxParamLightDirection = 0xA,
	WFxParamFogRange = 0xB,
	WFxParamFogColor = 0xC,
	WFxParamCameraPosition = 0xD,
	WFxParamSphericalHarmonicsCoefficients = 0xE,
	WFxParamExtraTexture = 0xF,
	WFxParamRefTexture = 0x10,
	WFxParamMaterialColor = 0x11,
	WFxParamTextureTransform = 0x12,
	WFxParamBlurUvOffset = 0x13,
	WFxParamShadowColor = 0x14,
	WFxParamNum = 0x15,
};

struct WRenderToTextureSizeInfo
{
	bool m_isAbsolute;
	float m_width;
	float m_height;

	static const WRenderToTextureSizeInfo SIZE_FULL;
	static const WRenderToTextureSizeInfo SIZE_HALF;
	static const WRenderToTextureSizeInfo SIZE_ABS;

	WRenderToTextureSizeInfo();
	WRenderToTextureSizeInfo(bool isAbsolute, float width, float height)
		: m_isAbsolute(isAbsolute), m_width(width), m_height(height)
	{
	}
	void Reset();
};

struct WRenderToTextureParam
{
	enum
	{
		NUM_SIMULTANEOUSRTS = 2
	};

	struct RtTexInfo
	{
		int m_hTex;
		bool m_needToClear;
		ulong m_clearClr;

		RtTexInfo()
			: m_hTex(0), m_needToClear(false), m_clearClr(0)
		{
		}
	};

	struct DepthSurfInfo
	{
		enum SurfaceUsage
		{
			USE_NOT_NEEDED,
			USE_MAIN_SURFACE,
			USE_SHARED_SURFACE,
			USE_EXCLUSIVE_SURFACE
		};

		SurfaceUsage m_surfUsage;
		int m_hTex;
		bool m_needToClear;
		float m_clearZ;

		DepthSurfInfo()
			: m_surfUsage(USE_NOT_NEEDED),
			  m_hTex(0),
			  m_needToClear(false),
			  m_clearZ(1.0f)
		{
		}
	};

	RtTexInfo m_rtTexInfo0;
	RtTexInfo m_rtTexInfo1;
	DepthSurfInfo m_depthSurfInfo;

	int GetTexture(unsigned index) const;
	bool CanClearAtOnce() const;

	WRenderToTextureParam();
};

class WDevice
{
public:
	virtual ~WDevice();
	virtual const char* GetDeviceName();
	virtual const char* EnumModeName();
	virtual void* ExternProc();
};

struct WTVertex
{
	float x;
	float y;
	float z;
	float rhw;
	unsigned int diffuse;
	float tu;
	float tv;
	float lu;
	float lv;
	float vz;
};

class WVideoDev : public WDevice
{
public:
	virtual ~WVideoDev();
	virtual wVDevState GetDeviceState() = 0;
	virtual void SetMainThreadId(unsigned int threadId);
	virtual void DrawPolygonFan(WTVertex** p, int iType, int iNum, int iType2,
		unsigned int dwVertexTypeDesc) = 0;
	virtual void DrawIndexedTriangles(WTVertex* p, int pNum,
		unsigned short* fList, int fNum, unsigned int iType,
		unsigned int iType2);
	virtual void DrawLine(WTVertex** p, int type) = 0;
	virtual int Command(wVDevMessage message, int param1, int param2) = 0;
	virtual int CreateTexture(LPBITMAPINFO src, int type) = 0;
	virtual void UpdateTexture(int texHandle, LPBITMAPINFO src, LPVOID data,
		unsigned int type) = 0;
	virtual void DestroyTexture(int texHandle) = 0;
	virtual int UploadCompressedTexture(void* pSrc, size_t srcSize,
		int type) = 0;
	virtual void UpdateCompressedTexture(int texHandle, void* pSrcData,
		size_t srcDataSize, int type) = 0;
	virtual void FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src,
		void* data, int type) = 0;
	virtual bool IsTextureFilled(int handle) = 0;
	virtual int GetTextureWidth(int hTex) = 0;
	virtual int GetTextureHeight(int hTex) = 0;
	virtual void SetRenderTargetSizeInfo(int handle,
		const WRenderToTextureSizeInfo& sizeInfo) = 0;
	virtual bool BeginScene() = 0;
	virtual void EndScene() = 0;
	virtual void Paint() = 0;
	virtual void SetGlobalRenderState(int rs, int rsEx) = 0;
	virtual bool IsSupportVs() = 0;
	virtual bool IsSupportPs() = 0;
	virtual bool IsSupportMrt() = 0;
	virtual bool IsSupportClipPlane() = 0;
	virtual void Clear(unsigned int color, int flags, float z) = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual int GetBackBufferBpp() const = 0;
	virtual bool IsWindowed() = 0;
	virtual bool IsFillScreenMode() = 0;
	virtual float GetMonitorSupportFps() const = 0;
	virtual bool SetFogEnable(bool enable) = 0;
	virtual void SetFogState(float fogStart, float fogEnd,
		unsigned int fogColor) = 0;
	virtual WVideoDev* MakeClone(char* modeName, HWND hWnd, int iTnL) = 0;
	virtual bool Reset(bool bWindowed, int iWidth, int iHeight, int iColor,
		int lWndStyle, int fillMode) = 0;

protected:
	virtual unsigned int VertexSize(unsigned int dwVertexTypeDesc) = 0;

public:
	virtual BYTE xGetStride(int hVb) = 0;
	virtual bool xHasVertexElem(unsigned int fvf, wWxVertexElem elem) const = 0;
	virtual int xGetVertexElemOffset(unsigned int fvf,
		wWxVertexElem elem) const = 0;
	virtual int xGetBlendWeightSize(unsigned int fvf) const = 0;
	virtual unsigned int xDetermineFVF(int iDrawFlag, int iDrawFlag2,
		int iMaxBoneNum) = 0;
	virtual unsigned int xDetermineBufferUsage(unsigned int fvf) = 0;
	virtual int xCreateVertexBuffer(int numVertices, unsigned int fvf,
		unsigned int dwUsage) = 0;
	virtual int xCreateIndexBuffer(int numIndices, unsigned int dwUsage) = 0;
	virtual void xReleaseVertexBuffer(int hVb) = 0;
	virtual void xReleaseIndexBuffer(int hIb) = 0;
	virtual char* xLockVertexBuffer(int hVb, unsigned int uiOffset,
		unsigned int uiSize) = 0;
	virtual char* xLockIndexBuffer(int hIb, unsigned int uiOffset,
		unsigned int uiSize) = 0;
	virtual void xUnlockVertexBuffer(int hVb) = 0;
	virtual void xUnlockIndexBuffer(int hIb) = 0;
	virtual void xDrawIndexedTriangles(const WxViewState& viewState,
		const WxBatchState& batchState) = 0;
	virtual void xSetTransform(WVDTRANSFORMSTATETYPE state,
		const WMatrix4& matrix) = 0;
	virtual void xSetPrevViewTransform(const WMatrix4& matrix) = 0;
	virtual void SetShaderSource(const char* shaderSrc) = 0;
	virtual void BeginUsingCustomRenderState() = 0;
	virtual void SetCustomRenderState(WVDRENDERSTATETYPE state,
		unsigned int value) = 0;
	virtual void SetCustomTextureStageState(unsigned int stage,
		WVDTEXTURESTAGESTATETYPE type, unsigned int value) = 0;
	virtual void SetCustomTransform(WVDTRANSFORMSTATETYPE state,
		const WMatrix4& matrix) = 0;
	virtual void SetCustomTexture(unsigned int stage, int hTex) = 0;
	virtual void SetCustomClipPlane(unsigned int index,
		const WPlane& value) = 0;
	virtual void SetCustomSamplerState(unsigned int sampler,
		WVDSAMPLERSTATETYPE type, unsigned int value) = 0;
	virtual void SetCustomFxMacro(unsigned int fxMacro) = 0;
	virtual void SetCustomFxParamInt(WFxParameterType paramType, int value) = 0;
	virtual void SetCustomFxParamVector2(WFxParameterType paramType,
		const WVector2D& value) = 0;
	virtual void SetCustomFxParamVector3(WFxParameterType paramType,
		const WVector& value) = 0;
	virtual void SetCustomFxParamVector4(WFxParameterType paramType,
		const WVector4& value) = 0;
	virtual void SetCustomFxParamMatrix(WFxParameterType paramType,
		const WMatrix4& value) = 0;
	virtual void SetCustomFxParamTexture(WFxParameterType paramType,
		int hTex) = 0;
	virtual void EndUsingCustomRenderState() = 0;
	virtual bool BeginRenderToTexture(WRenderToTextureParam* param) = 0;
	virtual void EndRenderToTexture(WRenderToTextureParam* param) = 0;
	virtual bool SupportRenderTargetFormat() = 0;
	virtual bool IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight,
		int iColor) = 0;
	virtual bool GetWindowDisplayMode(int& iWidth, int& iHeight,
		int& iColor) = 0;
	virtual int GetBufferingMeshNum() const = 0;
	virtual void SetViewPort(unsigned int x, unsigned int y, unsigned int w,
		unsigned int h) = 0;

protected:
	ulong m_mainThreadId;
	ulong m_renderCount;
	float m_clip_scale_z;
	float m_clip_near_scale;
};
