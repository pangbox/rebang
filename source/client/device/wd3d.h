#pragma once
#include <vector>
#include <wdevice.h>
#include "wvideo.h"
#include <d3d9.h>
#include <string>

class WDirect3D : public WVideoDev
{
public:
	WDirect3D(const WDirect3D&);
	WDirect3D();
	virtual ~WDirect3D();
	virtual wVDevState GetDeviceState() const { return this->m_devState; }
	virtual int GetBackBufferBpp() const { return this->m_BackBufBpp; }
	virtual void DrawPolygonFan(WTVertex**, int, int, int, unsigned long);
	virtual void DrawIndexedTriangles(WTVertex*, int, unsigned short*, int, int,
		int);
	virtual int CreateTexture(tagBITMAPINFO*, int);
	virtual void UpdateTexture(int, tagBITMAPINFO*, void*, unsigned long);
	virtual int UploadCompressedTexture(void*, unsigned int, int);
	virtual bool SetFogEnable(bool);
	virtual void SetFogState(float, float, unsigned long);
	virtual bool Reset(bool, int, int, int, long, int) = 0;
	virtual int GetBufferingMeshNum() const { return this->m_bufPolySwNum; }

protected:
	void Buffering(const WxViewState&, const WxBatchState&);
	void Buffering(int, int, int, unsigned long, void*, _D3DPRIMITIVETYPE,
		float);
	void Release();
	void Init();
	int GetTextureNum(int);
	virtual void DrawPrimitive(int, int, unsigned long, void*,
		_D3DPRIMITIVETYPE, int) = 0;
	virtual void DrawPrimitiveIndexed(int, unsigned long, void*, int,
		unsigned short*, int, int) = 0;
	virtual bool SetRenderState4Flushing(int) = 0;
	virtual void Flush(unsigned long);
	virtual void FlushEqual();
	virtual void FlushMultiPass(unsigned long);
	virtual void FlushOnePass(unsigned long);
	virtual void FlushAlways();
	virtual int UploadCompressedTextureSurface(void*, unsigned int, int) = 0;
	virtual void CreateTextureSurface(unsigned long, unsigned long, int, int,
		int, int, int) = 0;
	virtual void UpdateTextureSurface(int, tagBITMAPINFO*, void*, int) = 0;
	virtual void DestroyTexture(int);
	virtual bool SupportRenderTargetFormat() const = 0;
	virtual bool IsSupportedDisplayMode(bool, int, int, int) = 0;
	virtual void SetViewPort(unsigned long, unsigned long, unsigned long,
		unsigned long) = 0;
	void* ModifyVertices(WTVertex*, int, unsigned long, bool);
	void* ModifyVertices(WTVertex**, int, unsigned long, bool);
	virtual unsigned long VertexSize(unsigned long);

	void ApplyGamma(float);
	unsigned int GetSortBufferSize() const { return this->m_buf4sort.size(); }
	unsigned int GetSortBufferSwSize() const
	{
		return this->m_bufPolySw.size();
	}
	unsigned int GetSortBufferHwSize() const
	{
		return this->m_bufPolyHw.size();
	}
	int m_iRefTable[256];
	int m_MaxTextureBlendStages;
	int m_BackBufBpp;

private:
	char* m_chVertex;
	unsigned long m_dwOffset;
	void ClearVertices() { m_dwOffset = 0; }
	virtual void FlushRenderPrimitive() { }
	unsigned char CalcFog(float);
	struct w_poly_buffer
	{
		bool bHw;
		float depth;
	};
	struct w_poly_buffer_sw : public w_poly_buffer
	{
		int type;
		int type2;
		int num;
		unsigned long dwVertexTypeDesc;
		void* lpvVertices;
		_D3DPRIMITIVETYPE dptPrimitiveType;
		void* customRenderState;
	};
	struct w_poly_buffer_hw : public w_poly_buffer
	{
		WxViewState VState;
		WxBatchState BState;
		void* customRenderState;
	};
	std::vector<w_poly_buffer*> m_buf4sort;
	std::vector<w_poly_buffer_sw> m_bufPolySw;
	std::vector<w_poly_buffer_hw> m_bufPolyHw;
	void DrawBuffered(const w_poly_buffer_hw&);
	void DrawBuffered(const w_poly_buffer_sw&);
	virtual void* SnapShotCustomRenderState() = 0;
	virtual void ClearCustomRenderStateSnapShotList() = 0;
	virtual void ApplyCustomRenderState(const void*) = 0;
	static int __cdecl ComparePolyDepth(const void*, const void*);
	enum enumTexState
	{
		TS_VOID = 0,
		TS_CREATED = 1,
		TS_UPDATED = 2,
	};
	int m_texCount1;
	int m_texCount2;
	enumTexState m_texList[2048];
	float CalcDepth(WTVertex**, int);

protected:
	int m_bufPolySwNum;
	int m_bufPolyHwNum;
	int m_bufSortNum;
	wVDevState m_devState;
	bool m_fog;
	float m_fogStart;
	float m_fogEnd;
	unsigned long m_fogColor;
	bool m_useHiQualityTex;

public:
	virtual void xCreateVertexBuffer(int, int, unsigned long,
		unsigned long) = 0;
	virtual int xCreateVertexBuffer(int, unsigned long, unsigned long);
	virtual void xCreateIndexBuffer(int, int, unsigned long) = 0;
	virtual int xCreateIndexBuffer(int, unsigned long);
	virtual void xReleaseVertexBuffer(int);
	virtual void xReleaseIndexBuffer(int);
	virtual void xDrawIndexedTriangles(const WxViewState&, const WxBatchState&);

private:
	virtual void xDrawIndexedPrimitive(const WxViewState&,
		const WxBatchState&) = 0;
	int xGetVbHandle();
	int xGetIbHandle();
	int m_xiVbCount;
	int m_xiIbCount;
	int m_xahVbs[1024];
	int m_xahIbs[256];
};

inline unsigned long WDirect3D::VertexSize(unsigned long dwVertexTypeDesc)
{
	switch (dwVertexTypeDesc & 0x7FFFFFFF)
	{
	case 0x0042:
		return 16;
	case 0x0282:
		return 32;
	case 0x1106:
		return 24;
	case 0x0204:
		return 32;
	case 0x1118:
		return 40;
	case 0x111A:
		return 44;
	case 0x0142:
		return 24;
	case 0x02C4:
		return 40;
	case 0x0184:
		return 28;
	case 0x0102:
		return 20;
	case 0x1116:
		return 36;
	case 0x0244:
		return 36;
	case 0x0104:
		return 24;
	case 0x0152:
		return 36;
	case 0x0052:
		return 28;
	case 0x1108:
		return 28;
	case 0x01C2:
		return 28;
	case 0x0080:
		return 28;
	case 0x00C4:
		return 24;
	case 0x0252:
		return 44;
	case 0x00C2:
		return 20;
	case 0x0242:
		return 32;
	case 0x0212:
		return 40;
	case 0x0144:
		return 28;
	case 0x0284:
		return 36;
	case 0x01C4:
		return 32;
	case 0x110C:
		return 36;
	case 0x0000:
		return 24;
	case 0x111C:
		return 48;
	case 0x110A:
		return 32;
	case 0x02C2:
		return 36;
	case 0x0202:
		return 28;
	case 0x0112:
		return 32;
	case 0x0044:
		return 20;
	case 0x0182:
		return 24;
	default:
		return 0;
	}
}
