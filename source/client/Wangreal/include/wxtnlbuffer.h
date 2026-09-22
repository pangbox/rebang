#pragma once
#include "wtypes.h"

class WBone;
class WPuppet;
class WVideoDev;
class WView;
struct WxBatch;
struct WxBatchState;
struct w_mesh;

struct WxVb
{
	int xhVb;
	int xnVtxs;
	ulong xdwFVF;
	ulong xdwUsage;
	WxVb* xpNext;

	WxVb(ulong dwFVF, ulong dwUsage);
	~WxVb();
	void xReset(ulong dwFVF, ulong dwUsage);
	void xClear(WVideoDev* pVideo);
};

struct WxIb
{
	int xhIb;
	int xnIdxs;
	ulong xdwUsage;

	WxIb();
};

struct WxBatch
{
	WBone* xpBone;
	w_mesh* xpMesh;
	int xiBaseVtxIdx;
	int xnVtxs;
	int xiBaseIdxIdx;
	int xnIdxs;
	unsigned long xdwFVF;
	WxBatch* xpNext;

	WxBatch(WBone* pBone, w_mesh* pMesh, unsigned long dwFVF, int iBaseVtxIdx,
		int nVtxs, int iBaseIdxIdx, int nIdxs);
	void xClear();
	void xCopy(WxBatch** ppOut);
	void xProcessShader(WView* pView);
};

struct WxBatchGrp
{
	WxVb* xpVb;
	WxIb* xpIb;
	int xiFlag0;
	int xiFlag1;
	int xiBaseVtxIdx;
	int xnVtxs;
	int xiBaseIdxIdx;
	int xnIdxs;
	WxBatch* xpaBatches;
	WxBatchGrp* xpNext;

	WxBatchGrp(WxVb* pVb, WxIb* pIb, int iFlag0, int iFlag1, int iBaseVtxIdx,
		int nVtxs, int iBaseIdxIdx, int nIdxs);
	void xClear();
	void xCopy(WxBatchGrp** ppOut);
	void xRenderSubset(WView* pView, WxBatchState* pState, WxBatch* pBatch,
		int nBatches);
	void xRenderByBatch(WView* pView, WxBatchState* pState);
	void xRenderAtOnce(WView* pView, WxBatchState* pState, bool bEffect,
		int* pOrder, int nOrder);
};

void __fastcall xRender(WView* pView, WxBatchState* pState, WBone* pBone,
	w_mesh* pMesh);

class WxTnLBuffer
{
public:
	WxTnLBuffer();
	~WxTnLBuffer();

	void xCopy(WxTnLBuffer** ppOut, WVideoDev* pVideo);
	WxBatchGrp* xGetBatchGrps();
	void xUpdateStaticBufferInfo(WBone* pSrc, WBone* pDst, int iIdx);
	void xUpdateStaticBufferInfo(WPuppet** ppPets, int nPets);
	void xUpdateMatrixIdx(unsigned char* pBuf, WxBatchGrp* pGrp,
		WxBatch* pBatch, WBone* pBone, WPuppet* pPet);
	void xUpdateMatrixIdx(WPuppet* pPet);
	void xSetRootBone(WPuppet* pPet);
	void xClear();
	int xCreateBuffers(WVideoDev* pVideo, WPuppet* pPet);
	int xCreateStaticBuffers(WVideoDev* pVideo, WPuppet** ppPets, int nPets);
	void xRender(WView* pView);
	int xFillBuffers();

private:
	void xUpdateBones(WBone* pBone);
	void xGetMatchingBatches(WxBatchGrp** ppGrp, WxBatch** ppBatch, WxVb* pVb,
		WxIb* pIb, WBone* pBone, w_mesh* pMesh, int nPets);
	void xSortBatches();
	void xCount(bool bVtxs, WxBatchGrp* pGrp, WxBatch* pBatch, WBone* pBone);
	void xFillIndexBufferByBone(unsigned char* pBuf, WxBatchGrp* pGrp,
		WxBatch* pBatch, WBone* pBone);
	void xGetMatchingBuffers(WxVb** ppVb, WxIb** ppIb, w_mesh* pMesh);
	int xAllocBuffers(int nCopies);
	void xFillIndexBuffer(const WxIb& ib);
	int xDetermineProperty(WBone* pBone, int nCopies, int iLightMode);
	void xFillVertexBufferByMesh(unsigned char* pBuf, WxBatchGrp* pGrp,
		WxBatch* pBatch, w_mesh* pMesh);
	void xFillVertexBufferByBone(unsigned char* pBuf, WxBatchGrp* pGrp,
		WxBatch* pBatch, WBone* pBone);
	void xFillVertexBuffer(const WxVb& vb);

	int m_xiFlag;
	WxVb* m_xpaVbs;
	WxIb m_xaIbs[32];
	WxBatchGrp* m_xpaBatchGrps;
	WPuppet* m_xpPet;
	WVideoDev* m_xpVideo;
};
