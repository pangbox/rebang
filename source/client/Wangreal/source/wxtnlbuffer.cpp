#include "wxtnlbuffer.h"
#include "wmath.h"
#include "wview.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wmesh.h"
#include <new>

WxVb::WxVb(unsigned long dwFVF, unsigned long dwUsage)
{
	xReset(dwFVF, dwUsage);
}

WxVb::~WxVb()
{
	xReset(0, 0);
}

void WxVb::xReset(unsigned long dwFVF, unsigned long dwUsage)
{
	xhVb = 0;
	xnVtxs = 0;
	xdwFVF = dwFVF;
	xdwUsage = dwUsage;
	xpNext = 0;
}

void WxVb::xClear(WVideoDev* pVideo)
{
	if (xpNext)
	{
		xpNext->xClear(pVideo);
		delete xpNext;
		xpNext = 0;
	}
	pVideo->xReleaseVertexBuffer(xhVb);
}

WxIb::WxIb()
{
	xhIb = 0;
	xnIdxs = 0;
	xdwUsage = 0;
}

WxBatch::WxBatch(WBone* pBone, w_mesh* pMesh, unsigned long dwFVF,
	int iBaseVtxIdx, int nVtxs, int iBaseIdxIdx, int nIdxs)
	: xpBone(pBone),
	  xpMesh(pMesh),
	  xiBaseVtxIdx(iBaseVtxIdx),
	  xnVtxs(nVtxs),
	  xiBaseIdxIdx(iBaseIdxIdx),
	  xnIdxs(nIdxs),
	  xdwFVF(dwFVF),
	  xpNext(0)
{
}

void WxBatch::xClear()
{
	if (xpNext)
	{
		xpNext->xClear();
		delete xpNext;
		xpNext = 0;
	}
}

void WxBatch::xCopy(WxBatch** ppOut)
{
	*ppOut = new WxBatch(xpBone, xpMesh, xdwFVF, xiBaseVtxIdx, xnVtxs,
		xiBaseIdxIdx, xnIdxs);
}

void WxBatch::xProcessShader(WView* pView)
{
	if (!xpMesh->bSpec)
		return;

	WVideoDev* pVideo = pView->GetVideoDevice();
	if (!pVideo)
		return;

	int i;
	int iOffset = pVideo->xGetVertexElemOffset(xdwFVF, WX_VELEM_TEX2);
	if (iOffset < 0)
		return;

	BYTE nStride =
		pVideo->xGetStride((int)((unsigned int)xpMesh->xiDrawFlag2 >> 22));
	unsigned char* pData = pVideo->xLockVertexBuffer(
		(int)((unsigned int)xpMesh->xiDrawFlag2 >> 22), xiBaseVtxIdx * nStride,
		xnVtxs * nStride);

	WBone* pBone = xpBone;
	WMatrix mInv = ~pBone->m_matrix;
	if (pBone->m_light.type == 1)
	{
	}
	else if (pBone->m_light.type == 2)
	{
		WVector vLocalE;
		WVector vLocalL;
		WVector vXA;
		WVector vYA;
		WVector vZA;
		WVector* normList =
			xpMesh->myNormalList ? xpMesh->myNormalList : xpMesh->normList;

		vLocalE = pView->GetCamera().pivot * mInv;
		vLocalL = RotVec(pBone->m_light.nearOne, mInv).Normalize();

		for (i = 0; i < xnVtxs; i++, pData += nStride)
		{
			if (vLocalL * normList[i] >= 0.0f)
			{
				*(float*)(pData + iOffset) = *(float*)(pData + iOffset + 4) =
					0.75f;
			}
			else
			{
				vZA = ((xpMesh->vecList[i] - vLocalE).Normalize() + vLocalL)
						  .Normalize();

				if (vZA * normList[i] >= 0.0f)
				{
					*(float*)(pData + iOffset) =
						*(float*)(pData + iOffset + 4) = 0.75f;
				}
				else
				{
					if (WisEqual(vZA.y, -1.0f, g_EPSILON) ||
						WisEqual(vZA.y, 1.0f, g_EPSILON))
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_X, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}
					else
					{
						vXA =
							WCrossProduct(WVector::UNIT_POS_Y, vZA).Normalize();
						vYA = WCrossProduct(vZA, vXA).Normalize();
					}

					*(float*)(pData + iOffset) = Wabs(vXA * normList[i]);
					*(float*)(pData + iOffset + 4) = Wabs(vYA * normList[i]);
				}
			}
		}
	}

	pVideo->xUnlockVertexBuffer((int)((unsigned int)xpMesh->xiDrawFlag2 >> 22));
}

WxBatchGrp::WxBatchGrp(WxVb* pVb, WxIb* pIb, int iFlag0, int iFlag1,
	int iBaseVtxIdx, int nVtxs, int iBaseIdxIdx, int nIdxs)
	: xpVb(pVb),
	  xpIb(pIb),
	  xiFlag0(iFlag0),
	  xiFlag1(iFlag1),
	  xiBaseVtxIdx(iBaseVtxIdx),
	  xnVtxs(nVtxs),
	  xiBaseIdxIdx(iBaseIdxIdx),
	  xnIdxs(nIdxs),
	  xpaBatches(0),
	  xpNext(0)
{
}

__forceinline void WxBatchGrp::xClear()
{
	if (xpNext)
	{
		xpNext->xClear();
		delete xpNext;
		xpNext = 0;
	}
	if (xpaBatches)
	{
		xpaBatches->xClear();
		delete xpaBatches;
		xpaBatches = 0;
	}
}

void WxBatchGrp::xCopy(WxBatchGrp** ppOut)
{
	*ppOut = new WxBatchGrp(xpVb, xpIb, xiFlag0, xiFlag1, xiBaseVtxIdx, xnVtxs,
		xiBaseIdxIdx, xnIdxs);
}

void WxBatchGrp::xRenderByBatch(WView* pView, WxBatchState* pState)
{
	for (WxBatch* pBatch = xpaBatches; pBatch; pBatch = pBatch->xpNext)
	{
		if (WBone::IsMeshVisible(pBatch->xpBone, pBatch->xpMesh) &&
			(!pView->IsShadowView() ||
				(pBatch->xpMesh->drawFlag & 0x300000) != 0x100000))
		{
			WBone* pBone = pBatch->xpBone;
			pState->xmW = pBone->m_matrix;
			pState->xnmTransfs = 0;
			xRenderSubset(pView, pState, pBatch, 1);
		}
	}
}

void WxBatchGrp::xRenderAtOnce(WView* pView, WxBatchState* pState, bool bEffect,
	int* pOrder, int nOrder)
{
	if (pOrder == 0)
	{
		WxBatch* pBatch;
		WxBatch* pStart = 0;
		int nRun = 0;
		{
			for (pBatch = xpaBatches; pBatch; pBatch = pBatch->xpNext)
			{
				if (!pBatch->xpBone || !pBatch->xpMesh ||
					!WBone::IsMeshVisible(pBatch->xpBone, pBatch->xpMesh) ||
					(pView->IsShadowView() &&
						(pBatch->xpMesh->drawFlag & 0x300000) == 0x100000))
				{
					if (pStart)
					{
						xRenderSubset(pView, pState, pStart, nRun);
						pStart = 0;
						nRun = 0;
					}
				}
				else if (bEffect && (pBatch->xpMesh->drawFlag & 0x400000))
				{
					if (pStart)
						xRenderSubset(pView, pState, pStart, nRun);
					pStart = pBatch;
					nRun = 1;
				}
				else if (pStart)
				{
					if (pBatch->xpBone->m_alpha == pStart->xpBone->m_alpha &&
						pBatch->xpMesh->drawFlag == pStart->xpMesh->drawFlag &&
						pBatch->xpMesh->xiDrawFlag2 ==
							pStart->xpMesh->xiDrawFlag2 &&
						stricmp(pBatch->xpMesh->mpetName,
							pStart->xpMesh->mpetName) == 0)
					{
						++nRun;
					}
					else
					{
						xRenderSubset(pView, pState, pStart, nRun);
						pStart = pBatch;
						nRun = 1;
					}
				}
				else
				{
					pStart = pBatch;
					nRun = 1;
				}
			}
			if (pStart)
				xRenderSubset(pView, pState, pStart, nRun);
		}
	}
}

void WxBatchGrp::xRenderSubset(WView* pView, WxBatchState* pState,
	WxBatch* pBatch, int nBatches)
{
	int nVtxs = 0;
	int nIdxs = 0;
	WxBatch* p = pBatch;
	if (nBatches > 0)
	{
		int k = nBatches;
		do
		{
			nVtxs += p->xnVtxs;
			nIdxs += p->xnIdxs;
			--k;
			p = p->xpNext;
		} while (k);
	}
	if (pView->ProcessEffect())
	{
		p = pBatch;
		if (nBatches > 0)
		{
			int k = nBatches;
			do
			{
				p->xProcessShader(pView);
				--k;
				p = p->xpNext;
			} while (k);
		}
	}
	pState->xiFlag0 = pBatch->xpMesh->drawFlag;
	pState->xiFlag1 = pBatch->xpMesh->xiDrawFlag2;
	pState->xiBaseVtxIdx = pBatch->xiBaseVtxIdx;
	pState->xiBaseIdxIdx = pBatch->xiBaseIdxIdx;
	pState->xnIdxs = nIdxs;
	pState->xnVtxs = nVtxs;
	::xRender(pView, pState, pBatch->xpBone, pBatch->xpMesh);
}

WxTnLBuffer::WxTnLBuffer()
	: m_xiFlag(0), m_xpaVbs(0)
{
	m_xpaBatchGrps = 0;
	m_xpPet = 0;
	m_xpVideo = 0;
	memset(m_xaIbs, 0, sizeof(m_xaIbs));
}

WxTnLBuffer::~WxTnLBuffer()
{
	xClear();
}

void WxTnLBuffer::xClear()
{
	if ((m_xiFlag & 1) == 0 && m_xpaVbs)
	{
		m_xpaVbs->xClear(m_xpVideo);
		if (m_xpaVbs)
		{
			delete m_xpaVbs;
			m_xpaVbs = 0;
		}
	}
	WxIb* pIb = m_xaIbs;
	int n = 32;
	do
	{
		if (pIb->xnIdxs != 0 && (m_xiFlag & 1) == 0)
			m_xpVideo->xReleaseIndexBuffer(pIb->xhIb);
		pIb->xnIdxs = 0;
		pIb->xhIb = 0;
		pIb->xdwUsage = 0;
		++pIb;
		--n;
	} while (n);
	if (m_xpaBatchGrps)
	{
		m_xpaBatchGrps->xClear();
		if (m_xpaBatchGrps)
		{
			delete m_xpaBatchGrps;
			m_xpaBatchGrps = 0;
		}
	}
}

void WxTnLBuffer::xCopy(WxTnLBuffer** ppOut, WVideoDev* pVideo)
{
	*ppOut = new WxTnLBuffer;
	(*ppOut)->m_xiFlag |= m_xiFlag | 1;
	(*ppOut)->m_xpVideo = pVideo;
	(*ppOut)->m_xpaVbs = m_xpaVbs;
	for (unsigned int i = 0; i < 32; ++i)
		(*ppOut)->m_xaIbs[i] = m_xaIbs[i];
	WxBatchGrp** ppGrp = &(*ppOut)->m_xpaBatchGrps;
	for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
	{
		pGrp->xCopy(ppGrp);
		WxBatch** ppBatch = &(*ppGrp)->xpaBatches;
		for (WxBatch* pBatch = pGrp->xpaBatches; pBatch;
			pBatch = pBatch->xpNext)
		{
			pBatch->xCopy(ppBatch);
			ppBatch = &(*ppBatch)->xpNext;
		}
		ppGrp = &(*ppGrp)->xpNext;
	}
}

void WxTnLBuffer::xSetRootBone(WPuppet* pPet)
{
	m_xpPet = pPet;
	xUpdateBones(pPet->m_rootbone);
}

void WxTnLBuffer::xUpdateBones(WBone* pBone)
{
	do
	{
		for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
		{
			for (WxBatch* pBatch = pGrp->xpaBatches; pBatch;
				pBatch = pBatch->xpNext)
			{
				if (pBatch->xpBone->m_id == pBone->m_id)
				{
					w_mesh* pOld = pBatch->xpBone->m_mesh;
					w_mesh* pNew = pBone->m_mesh;
					if (pOld)
					{
						do
						{
							if (pOld == pBatch->xpMesh)
							{
								pBatch->xpMesh = pNew;
								break;
							}
							pOld = pOld->next;
							pNew = pNew->next;
						} while (pOld);
					}
					pBatch->xpBone = pBone;
				}
			}
		}
		if (pBone->m_next)
			xUpdateBones(pBone->m_next);
		pBone = pBone->m_child;
	} while (pBone);
}

WxBatchGrp* WxTnLBuffer::xGetBatchGrps()
{
	return m_xpaBatchGrps;
}

int WxTnLBuffer::xCreateBuffers(WVideoDev* pVideo, WPuppet* pPet)
{
	m_xpPet = pPet;
	m_xpVideo = pVideo;
	return xDetermineProperty(m_xpPet->GetRootBone(), 1, pPet->GetLightMode()) |
		xAllocBuffers(1);
}

int WxTnLBuffer::xCreateStaticBuffers(WVideoDev* pVideo, WPuppet** ppPets,
	int nPets)
{
	m_xiFlag |= 2;
	m_xpPet = ppPets[0];
	m_xpVideo = pVideo;
	return xDetermineProperty(m_xpPet->GetRootBone(), nPets,
			   ppPets[0]->GetLightMode()) |
		xAllocBuffers(nPets);
}

int WxTnLBuffer::xDetermineProperty(WBone* pBone, int nCopies, int iLightMode)
{
	w_mesh* pMesh = pBone->m_mesh;
	if (pMesh)
	{
		unsigned int uShadow = ~((unsigned int)iLightMode >> 11) & 0x2000;
		do
		{
			pMesh->xiDrawFlag2 = (pMesh->maxBoneNum > 0 ? 0x1000 : 0) |
				(pMesh->xiDrawFlag2 & 0xc14) | uShadow;
			if ((pMesh->xiDrawFlag2 & 0x2000) == 0)
				pMesh->xiDrawFlag2 = pMesh->xiDrawFlag2 | 4;
			WxVb* pVb = 0;
			WxIb* pIb = 0;
			WxBatchGrp* pGrp = 0;
			WxBatch* pBatch = 0;
			xGetMatchingBuffers(&pVb, &pIb, pMesh);
			xGetMatchingBatches(&pGrp, &pBatch, pVb, pIb, pBone, pMesh,
				nCopies);
			pVb->xnVtxs += pMesh->vtxNum * nCopies;
			pIb->xnIdxs += pMesh->indexNum * nCopies;
			pMesh->xpBatchGrp = pGrp;
			pMesh->xpBatch = pBatch;
			pMesh = pMesh->next;
		} while (pMesh);
	}
	if (pBone->m_next)
		xDetermineProperty(pBone->m_next, nCopies, iLightMode);
	if (pBone->m_child)
		xDetermineProperty(pBone->m_child, nCopies, iLightMode);
	return 0;
}

void WxTnLBuffer::xGetMatchingBuffers(WxVb** ppVb, WxIb** ppIb, w_mesh* pMesh)
{
	WxVb* pPrevVb = 0;
	WxIb* pIb = 0;
	unsigned long dwFVF = m_xpVideo->xDetermineFVF(pMesh->drawFlag,
		pMesh->xiDrawFlag2, pMesh->maxBoneNum);
	unsigned long dwUsage = m_xpVideo->xDetermineBufferUsage(dwFVF) | 8;
	unsigned long dwVbUsage = dwUsage;
	if (pMesh->bSpec)
		dwVbUsage |= 0x200;
	WxVb* pVb = 0;
	if (m_xpaVbs)
	{
		pVb = m_xpaVbs;
		do
		{
			if (dwFVF == pVb->xdwFVF && dwVbUsage == pVb->xdwUsage)
				break;
			pPrevVb = pVb;
			pVb = pVb->xpNext;
		} while (pVb);
	}
	if (!pVb)
	{
		pVb = new WxVb(dwFVF, dwVbUsage);
		if (pPrevVb)
			pPrevVb->xpNext = pVb;
		else
			m_xpaVbs = pVb;
	}

	for (unsigned int i = 0; i < 32; ++i)
	{
		if (m_xaIbs[i].xnIdxs == 0)
		{
			pIb = &m_xaIbs[i];
			pIb->xdwUsage = dwUsage;
			break;
		}
		if (dwUsage == m_xaIbs[i].xdwUsage &&
			m_xaIbs[i].xnIdxs + pMesh->indexNum < 9000)
		{
			pIb = &m_xaIbs[i];
			break;
		}
	}
	*ppVb = pVb;
	*ppIb = pIb;
}

void WxTnLBuffer::xGetMatchingBatches(WxBatchGrp** ppGrp, WxBatch** ppBatch,
	WxVb* pVb, WxIb* pIb, WBone* pBone, w_mesh* pMesh, int nPets)
{
	WxBatch* pBatch = 0;
	WxBatchGrp* pPrev = 0;
	WxBatchGrp* pGrp = 0;
	if (m_xpaBatchGrps)
	{
		pGrp = m_xpaBatchGrps;
		do
		{
			if (pGrp->xpVb == pVb && pGrp->xpIb == pIb &&
				pGrp->xiFlag0 == pMesh->drawFlag &&
				pGrp->xiFlag1 == pMesh->xiDrawFlag2)
				break;
			pPrev = pGrp;
			pGrp = pGrp->xpNext;
		} while (pGrp);
	}
	if (!pGrp)
	{
		pGrp = new WxBatchGrp(pVb, pIb, pMesh->drawFlag, pMesh->xiDrawFlag2, -1,
			0, -1, 0);
		if (pPrev)
			pPrev->xpNext = pGrp;
		else
			m_xpaBatchGrps = pGrp;
	}
	WxBatch* pTail = pGrp->xpaBatches;
	if (!pTail)
		pTail = 0;
	else
		while (pTail->xpNext)
			pTail = pTail->xpNext;
	for (int i = 0; i < nPets; i++)
	{
		WxBatch* pNextBatch = pBatch;
		pBatch = new WxBatch(pBone, pMesh, pVb->xdwFVF, -1, 0, -1, 0);
		pBatch->xpNext = pNextBatch;
		if (!pTail)
			pGrp->xpaBatches = pBatch;
		else
			pTail->xpNext = pBatch;
	}
	*ppGrp = pGrp;
	*ppBatch = pBatch;
}

void WxTnLBuffer::xSortBatches()
{
}

int WxTnLBuffer::xAllocBuffers(int nCopies)
{
	int iBase = 0;
	for (WxVb* pVb = m_xpaVbs; pVb; pVb = pVb->xpNext)
	{
		pVb->xhVb = m_xpVideo->xCreateVertexBuffer(pVb->xnVtxs, pVb->xdwFVF,
			pVb->xdwUsage);
		for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
		{
			if (pGrp->xpVb == pVb)
			{
				pGrp->xiFlag1 |= pVb->xhVb << 22;
				WxBatch* pBatch = pGrp->xpaBatches;
				pGrp->xiBaseVtxIdx = iBase;
				for (; pBatch; pBatch = pBatch->xpNext)
				{
					pBatch->xiBaseVtxIdx = iBase;
					xCount(true, pGrp, pBatch, m_xpPet->m_rootbone);
					int nVtxs = pBatch->xnVtxs;
					if (nCopies > 1)
					{
						int iOff = iBase + nVtxs;
						int k = nCopies - 1;
						do
						{
							pBatch = pBatch->xpNext;
							pBatch->xiBaseVtxIdx = iOff;
							iOff += nVtxs;
							--k;
							pBatch->xnVtxs = nVtxs;

						} while (k);
					}
					pGrp->xnVtxs += nVtxs * nCopies;
					iBase += nVtxs * nCopies;
				}
			}
		}
		iBase = 0;
	}
	int iBaseIdx = 0;
	for (int i = 0; i < 32; ++i)
	{
		WxIb* pIb = &m_xaIbs[i];
		if (pIb->xnIdxs > 0)
		{
			pIb->xhIb =
				m_xpVideo->xCreateIndexBuffer(pIb->xnIdxs, pIb->xdwUsage);
			for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
			{
				if (pGrp->xpIb == pIb)
				{
					pGrp->xiFlag1 |= (pIb->xhIb & 0xff) << 14;
					WxBatch* pBatch = pGrp->xpaBatches;
					pGrp->xiBaseIdxIdx = iBaseIdx;
					for (; pBatch; pBatch = pBatch->xpNext)
					{
						pBatch->xiBaseIdxIdx = iBaseIdx;
						xCount(false, pGrp, pBatch, m_xpPet->m_rootbone);
						int nIdxs = pBatch->xnIdxs;
						if (nCopies > 1)
						{
							int iOff = iBaseIdx + nIdxs;
							int k = nCopies - 1;
							do
							{
								pBatch = pBatch->xpNext;
								pBatch->xiBaseIdxIdx = iOff;
								iOff += nIdxs;
								--k;
								pBatch->xnIdxs = nIdxs;

							} while (k);
						}
						pGrp->xnIdxs += nIdxs * nCopies;
						iBaseIdx += nIdxs * nCopies;
					}
				}
			}
			iBaseIdx = 0;
		}
	}
	return 0;
}

void WxTnLBuffer::xCount(bool bVtxs, WxBatchGrp* pGrp, WxBatch* pBatch,
	WBone* pBone)
{
	{
		for (w_mesh* pMesh = pBone->m_mesh; pMesh; pMesh = pMesh->next)
		{
			if (pMesh->xpBatch == pBatch)
			{
				if (bVtxs)
					pBatch->xnVtxs += pMesh->vtxNum;
				else
					pBatch->xnIdxs += pMesh->indexNum;
				break;
			}
		}
		if (pBone->m_next)
			xCount(bVtxs, pGrp, pBatch, pBone->m_next);
		if (pBone->m_child)
			xCount(bVtxs, pGrp, pBatch, pBone->m_child);
	}
}

int WxTnLBuffer::xFillBuffers()
{
	for (WxVb* pVb = m_xpaVbs; pVb; pVb = pVb->xpNext)
		xFillVertexBuffer(*pVb);
	WxIb* pIb = m_xaIbs;
	int n = 32;
	do
	{
		if (pIb->xnIdxs != 0)
			xFillIndexBuffer(*pIb);
		++pIb;
		--n;
	} while (n);
	return 0;
}

void WxTnLBuffer::xFillVertexBuffer(const WxVb& vb)
{
	unsigned char* pBuf = m_xpVideo->xLockVertexBuffer(vb.xhVb, 0, 0);
	for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
	{
		if (pGrp->xpVb == &vb)
		{
			int nStride = m_xpVideo->VertexSize(pGrp->xpVb->xdwFVF);
			for (WxBatch* pBatch = pGrp->xpaBatches; pBatch;
				pBatch = pBatch->xpNext)
				xFillVertexBufferByBone(pBuf + pBatch->xiBaseVtxIdx * nStride,
					pGrp, pBatch, pBatch->xpBone);
		}
	}
	m_xpVideo->xUnlockVertexBuffer(vb.xhVb);
}

void WxTnLBuffer::xFillVertexBufferByBone(unsigned char* pBuf, WxBatchGrp* pGrp,
	WxBatch* pBatch, WBone* pBone)
{
	{
		for (w_mesh* pMesh = pBone->m_mesh; pMesh; pMesh = pMesh->next)
		{
			if (pMesh->xpBatch == pBatch)
			{
				pMesh->xiDrawFlag2 |= pGrp->xpVb->xhVb << 22;
				pMesh->xiBaseVtxIdx = pBatch->xiBaseVtxIdx;
				xFillVertexBufferByMesh(pBuf, pGrp, pBatch, pMesh);
				break;
			}
		}
		if (pBone->m_next)
			xFillVertexBufferByBone(pBuf, pGrp, pBatch, pBone->m_next);
		if (pBone->m_child)
			xFillVertexBufferByBone(pBuf, pGrp, pBatch, pBone->m_child);
	}
}

void WxTnLBuffer::xFillVertexBufferByMesh(unsigned char* pBuf, WxBatchGrp* pGrp,
	WxBatch* pBatch, w_mesh* pMesh)
{
	unsigned long dwFVF = pGrp->xpVb->xdwFVF;
	int iOff = 0;
	const WMatrixPtrList* pList = 0;
	int nStride = m_xpVideo->VertexSize(dwFVF);
	int nBlendSize = m_xpVideo->xGetBlendWeightSize(dwFVF);
	bool bNormal = m_xpVideo->xHasVertexElem(dwFVF, WX_VELEM_NORMAL);
	bool bColor = m_xpVideo->xHasVertexElem(dwFVF, WX_VELEM_DIFFUSE);
	bool bTex0 = m_xpVideo->xHasVertexElem(dwFVF, WX_VELEM_TEX1);
	bool bTex1 = m_xpVideo->xHasVertexElem(dwFVF, WX_VELEM_TEX2);
	const WMatrix* pMat;
	if ((m_xiFlag & 2) == 0)
	{
		if (strlen(pMesh->mpetName) != 0)
			pList = &WPuppet::GetOrginalMatrixPtrList(pMesh->mpetName);
		else
			pList = &m_xpPet->xGetTransfMatPtrList();
		int iId = pBatch->xpBone->m_id;
		if (iId < (int)pList->size())
			pMat = (*pList)[iId];
		else
			pMat = &WMatrix::IDENTITY;
	}
	else
	{
		pMat = &pBatch->xpBone->m_matrix;
	}
	WMatrix mRot;
	pMat->GetRotMatrix(&mRot);
	WVector* pNorm =
		pMesh->myNormalList ? pMesh->myNormalList : pMesh->normList;
	int i = 0;
	for (; i < pMesh->vtxNum; ++i)
	{
		if (!(pMesh->xiDrawFlag2 & 0x1000))
		{
			if (!(m_xiFlag & 2))
				memcpy(pBuf + iOff, &pMesh->vecList[i], sizeof(WVector));
			else
				*(WVector*)(pBuf + iOff) = pMesh->vecList[i] * *pMat;
			iOff += 12;
		}
		else
		{
			if (i < pMesh->rigidNum)
				*(WVector*)(pBuf + iOff) =
					pMesh->vecList[i] * *(*pList)[pMesh->boneList[i]->m_id];
			else
				*(WVector*)(pBuf + iOff) = pMesh->vecList[i] * *pMat;
			iOff += 0x10 + nBlendSize;
		}
		if (bNormal)
		{
			if (!(pMesh->xiDrawFlag2 & 0x1000))
			{
				if (!(m_xiFlag & 2))
					memcpy(pBuf + iOff, &pNorm[i], sizeof(WVector));
				else
					*(WVector*)(pBuf + iOff) = pNorm[i] * mRot;
			}
			else if (i < pMesh->rigidNum)
			{
				WMatrix mBone;
				(*pList)[pMesh->boneList[i]->m_id]->GetRotMatrix(&mBone);
				*(WVector*)(pBuf + iOff) = pNorm[i] * mBone;
			}
			else
				*(WVector*)(pBuf + iOff) = pNorm[i] * mRot;
			iOff += 12;
		}
		if (bColor)
		{
			if (pMesh->vtxColorPtrList)
				*(unsigned long*)(pBuf + iOff) = *pMesh->vtxColorPtrList[i];
			else
				*(unsigned long*)(pBuf + iOff) = pMesh->vtxColorList[i];
			iOff += 4;
		}
		if (bTex0)
		{
			memcpy(pBuf + iOff, pMesh->uvData[i], sizeof(float) * 2);
			iOff += 8;
		}
		if (bTex1)
		{
			memcpy(pBuf + iOff, pMesh->uvBackup[i], sizeof(float) * 2);
			iOff += 8;
		}
	}
	if (pMesh->xiDrawFlag2 & 0x1000)
	{
		int iVtx = 0;
		int iDst = 12;
		int iBone = 0;
		float afWeights[3];
		if (pMesh->blendedRigidNum > 0)
		{
			do
			{
				unsigned long uIdx = 0;
				memset(afWeights, 0, sizeof(afWeights));
				float fTotal = 0.0f;
				int j = 0;
				for (; fTotal < 0.999f && j < 3; ++j, ++iBone)
				{
					if (pMesh->blendedBoneList[iBone])
					{
						uIdx |= pMesh->blendedBoneList[iBone]->m_id << (j * 8);
						afWeights[j] = pMesh->blendWeightList[iBone];
						fTotal += pMesh->blendWeightList[iBone];
					}
				}
				if (j == 3 && fTotal < 0.999f && pMesh->blendedBoneList[iBone])
				{
					uIdx |= pMesh->blendedBoneList[iBone]->m_id << 24;
					++iBone;
				}

				memcpy(&pBuf[iDst], afWeights, nBlendSize);
				*(unsigned int*)(&pBuf[iDst] + nBlendSize) = uIdx;
				iDst += nStride;
				++iVtx;
			} while (iVtx < pMesh->blendedRigidNum);
		}
		afWeights[0] = 1.0f;
		afWeights[2] = 0.0f;
		afWeights[1] = 0.0f;
		if ((int)nBlendSize > 0)
		{
			if (iVtx < pMesh->rigidNum)
			{
				do
				{
					int iId = pMesh->boneList[iVtx]->m_id;
					memcpy(&pBuf[iDst], afWeights, nBlendSize);
					*(int*)(pBuf + nBlendSize + iDst) = iId;
					++iVtx;
					iDst += nStride;
				} while (iVtx < pMesh->rigidNum);
			}
			int iId = pBatch->xpBone->m_id;
			if (iVtx < pMesh->vtxNum)
			{
				do
				{
					memcpy(&pBuf[iDst], afWeights, nBlendSize);
					*(int*)(pBuf + nBlendSize + iDst) = iId;
					++iVtx;
					iDst += nStride;
				} while (iVtx < pMesh->vtxNum);
			}
		}
		else
		{
			if (iVtx < pMesh->rigidNum)
			{
				do
				{
					*(int*)(pBuf + nBlendSize + iDst) =
						pMesh->boneList[iVtx]->m_id;
					++iVtx;
					iDst += nStride;
				} while (iVtx < pMesh->rigidNum);
			}
			int iId = pBatch->xpBone->m_id;
			if (iVtx < pMesh->vtxNum)
			{
				do
				{
					*(int*)(pBuf + iDst + nBlendSize) = iId;
					++iVtx;
					iDst += nStride;
				} while (iVtx < pMesh->vtxNum);
			}
		}
	}
}

void WxTnLBuffer::xFillIndexBuffer(const WxIb& ib)
{
	unsigned char* pBuf = m_xpVideo->xLockIndexBuffer(ib.xhIb, 0, 0);
	for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
	{
		if (pGrp->xpIb == &ib)
		{
			for (WxBatch* pBatch = pGrp->xpaBatches; pBatch;
				pBatch = pBatch->xpNext)
				xFillIndexBufferByBone(pBuf + pBatch->xiBaseIdxIdx * 2, pGrp,
					pBatch, pBatch->xpBone);
		}
	}
	m_xpVideo->xUnlockIndexBuffer(ib.xhIb);
}

void WxTnLBuffer::xFillIndexBufferByBone(unsigned char* pBuf, WxBatchGrp* pGrp,
	WxBatch* pBatch, WBone* pBone)
{
	{
		unsigned char* pDst = pBuf;
		for (w_mesh* pMesh = pBone->m_mesh; pMesh; pMesh = pMesh->next)
		{
			if (pMesh->xpBatch == pBatch)
			{
				pMesh->xiDrawFlag2 |= (pGrp->xpIb->xhIb & 0xff) << 14;
				pMesh->xiBaseIdxIdx = pBatch->xiBaseIdxIdx;
				for (int i = 0; i < pMesh->indexNum; ++i, pDst += 2)
					*(unsigned short*)pDst =
						(unsigned short)(pMesh->indexList[i] +
							pMesh->xiBaseVtxIdx);
				break;
			}
		}
		if (pBone->m_next)
			xFillIndexBufferByBone(pBuf, pGrp, pBatch, pBone->m_next);
		if (pBone->m_child)
			xFillIndexBufferByBone(pBuf, pGrp, pBatch, pBone->m_child);
	}
}

void WxTnLBuffer::xUpdateStaticBufferInfo(WPuppet** ppPets, int nPets)
{
	WBone* pRoot = ppPets[0]->m_rootbone;
	for (int i = 1; i < nPets; ++i)
		xUpdateStaticBufferInfo(pRoot, ppPets[i]->m_rootbone, i);
}

void WxTnLBuffer::xUpdateStaticBufferInfo(WBone* pSrc, WBone* pDst, int iIdx)
{
	{
		WBone* pFound = pDst->FindBone(pSrc->m_name, 0);
		if (pFound)
		{
			w_mesh* pMeshSrc = pSrc->m_mesh;
			for (w_mesh* pMeshDst = pFound->m_mesh; pMeshSrc && pMeshDst;
				pMeshDst = pMeshDst->next)
			{
				WxBatchGrp* pGrp = pMeshSrc->xpBatchGrp;
				WxBatch* pBatch = pMeshSrc->xpBatch;
				for (int i = 0; i < iIdx; ++i)
					pBatch = pBatch->xpNext;
				pMeshDst->xpBatchGrp = pGrp;
				pMeshDst->xiDrawFlag2 = pGrp->xiFlag1;
				pMeshDst->xpBatch = pBatch;
				pMeshDst->xiBaseVtxIdx = pBatch->xiBaseVtxIdx;
				pMeshDst->xiBaseIdxIdx = pBatch->xiBaseIdxIdx;
				pBatch->xpBone = pFound;
				pBatch->xpMesh = pMeshDst;
				pMeshSrc = pMeshSrc->next;
			}
		}
		if (pSrc->m_next)
			xUpdateStaticBufferInfo(pSrc->m_next, pDst, iIdx);
		if (pSrc->m_child)
			xUpdateStaticBufferInfo(pSrc->m_child, pDst, iIdx);
	}
}

void WxTnLBuffer::xUpdateMatrixIdx(WPuppet* pPet)
{
	for (WxBatchGrp* pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
	{
		if (pGrp->xiFlag1 & 0x1000)
		{
			int nStride = m_xpVideo->VertexSize(pGrp->xpVb->xdwFVF);
			unsigned char* pBuf =
				m_xpVideo->xLockVertexBuffer(pGrp->xpVb->xhVb, 0, 0);
			for (WxBatch* pBatch = pGrp->xpaBatches; pBatch;
				pBatch = pBatch->xpNext)
				xUpdateMatrixIdx(pBuf + pBatch->xiBaseVtxIdx * nStride, pGrp,
					pBatch, m_xpPet->m_rootbone, pPet);
			m_xpVideo->xUnlockVertexBuffer(pGrp->xpVb->xhVb);
		}
	}
}

void WxTnLBuffer::xUpdateMatrixIdx(unsigned char* pBuf, WxBatchGrp* pGrp,
	WxBatch* pBatch, WBone* pBone, WPuppet* pPet)
{
	{
		w_mesh* pMesh = pBone->m_mesh;
		while (pMesh && pMesh->xpBatch != pBatch)
			pMesh = pMesh->next;
		if (pMesh)
		{
			unsigned long fvf = pGrp->xpVb->xdwFVF;
			int nStride = m_xpVideo->VertexSize(fvf);
			int iVtx;
			int iBone = 0;
			int iOff = 0x18;
			for (iVtx = 0; iVtx < pMesh->blendedRigidNum; ++iVtx)
			{
				{
					unsigned int uIdx = 0;
					float fTotal = 0.0f;
					int j = 0;
					do
					{
						if (j >= 3)
							break;
						uIdx |= pMesh->blendedBoneList[iBone]->m_id << (j * 8);
						fTotal += pMesh->blendWeightList[iBone];
						++j;
						++iBone;
					} while (fTotal < 0.999f);
					if (j == 3 && fTotal < 0.999f)
					{
						uIdx |= pMesh->blendedBoneList[iBone++]->m_id << 24;
					}
					*(unsigned int*)(pBuf + iOff) = uIdx;
					iOff += nStride;
				}
			}
			if (iVtx < pMesh->rigidNum)
			{
				int* pIdx = (int*)(pBuf + iOff);
				do
				{
					*pIdx = pMesh->boneList[iVtx]->m_id;
					++iVtx;
					pIdx = (int*)((char*)pIdx + nStride);
				} while (iVtx < pMesh->rigidNum);
			}
		}
		if (pBone->m_next)
			xUpdateMatrixIdx(pBuf, pGrp, pBatch, pBone->m_next, pPet);
		if (pBone->m_child)
			xUpdateMatrixIdx(pBuf, pGrp, pBatch, pBone->m_child, pPet);
	}
}

void WxTnLBuffer::xRender(WView* pView)
{
	WxBatchGrp* pGrp = m_xpaBatchGrps;
	WxBatchState st;
	if ((m_xiFlag & 2) != 0)
	{
		st.xmW.Reset();
		st.xnmTransfs = 0;
		if (pGrp)
		{
			do
			{
				pGrp->xRenderAtOnce(pView, &st, true, 0, 0);
				pGrp = pGrp->xpNext;
			} while (pGrp);
		}
	}
	else
	{
		bool bAtOnce = false;
		if (pGrp)
		{
			do
			{
				if ((pGrp->xiFlag1 & 0x1000) == 0)
					pGrp->xRenderByBatch(pView, &st);
				else
					bAtOnce = true;
				pGrp = pGrp->xpNext;
			} while (pGrp);
			if (bAtOnce)
			{
				st.xnmTransfs = (int)m_xpPet->xGetTransfMatPtrList().size();
				if (st.xnmTransfs > 0)
				{
					st.xpapmW = &m_xpPet->xGetTransfMatPtrList()[0];
					st.xpapmO = &WPuppet::GetOrginalMatrixPtrList(
						m_xpPet->GetPuppetName())[0];
				}
				for (pGrp = m_xpaBatchGrps; pGrp; pGrp = pGrp->xpNext)
				{
					if (pGrp->xiFlag1 & 0x1000)
						pGrp->xRenderAtOnce(pView, &st, false, 0, 0);
				}
			}
		}
	}
}

void __fastcall xRender(WView* pView, WxBatchState* pState, WBone* pBone,
	w_mesh* pMesh)
{
	const WMatrix* const* ppmOld = pState->xpapmO;
	if (pBone && pMesh)
	{
		if (pState->xiFlag1 & 0x1000)
		{
			if (strlen(pMesh->mpetName) != 0)
				pState->xpapmO =
					&WPuppet::GetOrginalMatrixPtrList(pMesh->mpetName)[0];
		}
		if (pState->xiFlag1 & 4)
		{
			LightSet* pLight = pView->xGetLight();
			pState->xdwDiffuse =
				((unsigned long)pBone->m_alpha << 24) | pLight->ambient2;
		}
		else
		{
			pState->xdwDiffuse =
				((unsigned long)pBone->m_alpha << 24) | 0xffffff;
		}
		if (pState->xiFlag0 & 0x400000)
		{
			WVector v[8] = {
				WVector(pMesh->localAabb.min.x, pMesh->localAabb.min.y,
					pMesh->localAabb.min.z),
				WVector(pMesh->localAabb.max.x, pMesh->localAabb.min.y,
					pMesh->localAabb.min.z),
				WVector(pMesh->localAabb.max.x, pMesh->localAabb.min.y,
					pMesh->localAabb.max.z),
				WVector(pMesh->localAabb.min.x, pMesh->localAabb.min.y,
					pMesh->localAabb.max.z),
				WVector(pMesh->localAabb.min.x, pMesh->localAabb.max.y,
					pMesh->localAabb.min.z),
				WVector(pMesh->localAabb.max.x, pMesh->localAabb.max.y,
					pMesh->localAabb.min.z),
				WVector(pMesh->localAabb.max.x, pMesh->localAabb.max.y,
					pMesh->localAabb.max.z),
				WVector(pMesh->localAabb.min.x, pMesh->localAabb.max.y,
					pMesh->localAabb.max.z),
			};
			float fMin = g_HUGE;
			float fMax = -g_HUGE;
			WMatrix mWorld;
			if (pMesh->aabbBone)
				mWorld = pMesh->aabbBone->m_matrix * pView->GetInvCamera();
			else
				mWorld = pBone->m_matrix * pView->GetInvCamera();
			for (int i = 0; i < 8; ++i)
			{
				v[i].z = TransformZ(v[i], mWorld);
				if (v[i].z < fMin)
					fMin = v[i].z;
				if (v[i].z > fMax)
					fMax = v[i].z;
			}
			pState->xfDepth = fMax * 0.3f + fMin * 0.7f;
		}
	}
	pView->xDrawIndexedTriangles(*pState);
	pState->xpapmO = ppmOld;
}
