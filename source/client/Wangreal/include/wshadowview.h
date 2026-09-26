#pragma once
#include "wview.h"
#include "wtextureview.h"

class WPuppet;

class WShadowView : public WView
{
public:
	WShadowView(unsigned char shadowDense);
	virtual ~WShadowView();

	virtual void SetTextureSize(int w, int h) = 0;
	void SetShadowDense(unsigned char shadowDense);
	virtual int GetTexture() = 0;

	virtual void BeginScene()
	{
		update |= 3;
		UpdateCamera();
	}
	virtual void EndScene() { }
	virtual bool IsShadowView() { return true; }

	virtual void Render() = 0;
	virtual void Render(WPuppet* pet, bool bTransform, float matscale,
		bool noclipping, bool updatemat) = 0;

protected:
	unsigned char m_fgColor;
};

class WShadowViewSw : public WShadowView
{
public:
	WShadowViewSw(unsigned char shadowDense);
	virtual ~WShadowViewSw();

	virtual void SetTextureSize(int w, int h);
	virtual int GetTexture() { return m_texture; }
	virtual void Clear(ulong clearColor, int mode);
	virtual void Render();
	virtual void Render(WPuppet* pet, bool bTransform, float matscale,
		bool noclipping, bool updatemat);

protected:
	virtual void DrawIndexedTrianglesDirect(WTVertex* plist, int pn,
		unsigned short* flist, int fn, int type, int type2);
	void DrawPolygonFanDirect(WTVertex** vl, int drawoption, int n,
		int drawoption2);
	void Filtering();

private:
	void PutPolygonColor(WTVertex** p);

	int m_texture;
	unsigned char* m_paTargetBuff;
	Bitmap* m_pUpdate;
};

class WShadowViewHw : public WShadowView
{
public:
	WShadowViewHw(unsigned char shadowDense);
	virtual ~WShadowViewHw();

	virtual void SetTextureSize(int w, int h);
	virtual int GetTexture()
	{
		return m_filteredTexView ? m_filteredTexView->GetTexture(0) : 0;
	}
	virtual void Clear(ulong clearColor, int mode);
	virtual void Render();
	virtual void Render(WPuppet* pet, bool bTransform, float matscale,
		bool noclipping, bool updatemat);

private:
	WRenderToTextureParam m_orgTexParam;
	WTextureView* m_filteredTexView;
	bool m_r2tBegun;

	static WTVertex m_vtx[4];
	static WTVertex* m_vl[5];
};

class WRecvView : public WView
{
public:
	WRecvView();
	virtual ~WRecvView();

	void SetRenderView(WView* view);
	WView* GetRenderView();

	virtual void BeginScene()
	{
		update |= 3;
		UpdateCamera();
	}
	virtual void Clear(ulong clearColor, int mode) = 0;
	virtual void EndScene() { }
	virtual bool IsShadowView() { return true; }

	virtual void CollectRecvTris(WPuppet* pPet, bool bTransform, float matscale,
		bool noclipping, bool updatemat) = 0;
	virtual bool HasRecver() = 0;
	virtual void Render(WShadowView* shadowView, WPuppet* pPet) = 0;

protected:
	WView* m_renderview;
};

class WRecvViewSw : public WRecvView
{
public:
	WRecvViewSw();
	virtual ~WRecvViewSw();

	virtual void Clear(ulong clearColor, int mode);
	void DrawPolygonFan(WTVertex** vl, int clipoption, int drawoption);
	virtual void DrawIndexedTriangles(WTVertex* p, int pnum, unsigned short* f,
		int fnum, int drawoption, int drawoption2);
	virtual void CollectRecvTris(WPuppet* pPet, bool bTransform, float matscale,
		bool noclipping, bool updatemat);
	virtual bool HasRecver() { return m_cur_face > 0 ? true : false; }
	virtual void Render(WShadowView* shadowView, WPuppet* pPet);

private:
	void AllocVtxBuffMore();
	void AddPolygonList(WTVertex** vl);
	void AddPolygonIdxList(WTVertex** p, int pnum, unsigned short* f, int fnum);
	static int __cdecl CompareDepthPoly(const void* elem1, const void* elem2);
	void SortFace();

	static WTVertex** g_vtxIdxList;

	int m_cur_vtx;
	int m_cur_face;
	WTVertex** m_vtxIdxList;
	WTVertex* m_vtxList;
	unsigned short* m_faceList;
	int m_allocLen;

	int GetClipCode(float x, float y, float z);
	void ClipEdge(WTVertex* out, const WTVertex* a, const WTVertex* b, float t);

	static WTVertex** plist;
	static WTVertex* pl;
	static unsigned short* flist;
	static unsigned short* fl;
	static int m_count;
};

class WRecvViewHw : public WRecvView
{
public:
	WRecvViewHw();
	virtual ~WRecvViewHw();

	virtual void Clear(ulong clearColor, int mode);
	virtual void CollectRecvTris(WPuppet* pPet, bool bTransform, float matscale,
		bool noclipping, bool updatemat);
	virtual bool HasRecver() { return m_hasRecver; }
	virtual void Render(WShadowView* shadowView, WPuppet* pPet);

private:
	void MakeEqualToRenderView();

	bool m_hasRecver;
};
