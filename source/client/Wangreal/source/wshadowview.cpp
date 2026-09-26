#include "wshadowview.h"
#include "wtextureview.h"
#include "wpuppet.h"
#include "bitmap.h"
#include <stdlib.h>
#include <string.h>

struct Edge
{
	int y;
	int n;
	long x;
	long dx;
};

__forceinline WView::~WView()
{
}

WShadowView::WShadowView(unsigned char shadowDense)
{
	SetShadowDense(shadowDense);
	m_bProcessEffect = false;
	m_bDisableFog = true;
}

WShadowView::~WShadowView()
{
}

void WShadowView::SetShadowDense(unsigned char shadowDense)
{
	m_fgColor = 0xff - shadowDense;
}

#include "fixed.h"

WShadowViewSw::WShadowViewSw(unsigned char shadowDense)
	: WShadowView(shadowDense), m_texture(0), m_paTargetBuff(0), m_pUpdate(0)
{
}

WShadowViewSw::~WShadowViewSw()
{
	if (GetResrcManager() && m_texture)
	{
		GetResrcManager()->Release(m_texture);
		m_texture = 0;
	}
	if (m_paTargetBuff)
	{
		delete[] m_paTargetBuff;
		m_paTargetBuff = 0;
	}
	if (m_pUpdate)
	{
		delete m_pUpdate;
		m_pUpdate = 0;
	}
}

void WShadowViewSw::SetTextureSize(int w, int h)
{
	SetViewport((float)w, (float)h);
	m_paTargetBuff = new unsigned char[w * h];
	m_pUpdate = new Bitmap(w / 2, h / 2, 8);
	m_pUpdate->SetGrayPalette();
	m_texture = GetResrcManager()
		? GetResrcManager()->UploadTexture(0, m_pUpdate, 0, 0)
		: 0;
}

void WShadowViewSw::Clear(ulong clearColor, int mode)
{
	UpdateCamera();
	memset(m_paTargetBuff, 0xff, (int)GetWidth() * (int)GetHeight());
}

void WShadowViewSw::DrawPolygonFanDirect(WTVertex** vl, int drawoption, int n,
	int drawoption2)
{
	PutPolygonColor(vl);
}

void WShadowViewSw::DrawIndexedTrianglesDirect(WTVertex* plist, int pn,
	unsigned short* flist, int fn, int type, int type2)
{
	int width = (int)GetWidth();
	int height = (int)GetHeight();
	WTVertex* vl[4];
	WTVertex p[3];

	vl[0] = &p[0];
	vl[1] = &p[1];
	vl[2] = &p[2], vl[3] = 0;
	WTVertex* t = NULL;

	for (int i = 0; i < fn; i += 3)
	{
		for (int j = 0; j < 3; j++)
		{
			t = &plist[flist[i + j]];
			WVector v(t->sx * matrix.xx + t->sy * matrix.xy +
					t->sz * matrix.xz + matrix.xm,
				t->sx * matrix.yx + t->sy * matrix.yy + t->sz * matrix.yz +
					matrix.ym,
				t->sx * matrix.zx + t->sy * matrix.zy + t->sz * matrix.zz +
					matrix.zm);
			float w = 1.0f / v.z;
			p[j].sx = v.x * w + m_center_x;
			p[j].sy = v.y * w + m_center_y;
		}

		if ((p[0].sx < 0.0f && p[1].sx < 0.0f && p[2].sx < 0.0f) ||
			(p[0].sy < 0.0f && p[1].sy < 0.0f && p[2].sy < 0.0f) ||
			(p[0].sx >= width && p[1].sx >= width && p[2].sx >= width) ||
			(p[0].sy >= height && p[1].sy >= height && p[2].sy >= height))
			continue;

		PutPolygonColor(vl);
	}
}

void WShadowViewSw::PutPolygonColor(WTVertex** p)
{
	static Edge edge[2];

	int width = (int)GetWidth();
	int height = (int)GetHeight();
	unsigned char* vram;
	int num, i, y, top = 0;

	for (num = 1; p[num]; num++)
		if (p[top]->sy > p[num]->sy)
			top = num;

	WTVertex* v = p[top];
	edge[0].y = edge[1].y = y = (int)v->sy;
	edge[0].x = edge[1].x = FloatToFixed16_16(v->sx);
	edge[0].n = edge[1].n = top;

	for (i = 0, vram = m_paTargetBuff + y * width; y < height;)
	{
		for (Edge* e = edge; e <= &edge[1]; e++)
		{
			while (y >= e->y)
			{
				if (i++ >= num)
					return;
				if (e == edge)
				{
					if (e->n <= 0)
						e->n = num - 1;
					else
						e->n--;
				}
				else
				{
					if (++e->n >= num)
						e->n = 0;
				}
				WTVertex* v = p[e->n];
				e->y = (int)v->sy;
				int dy = e->y - y + 1;
				if (dy > 0)
				{
					e->dx = (FloatToFixed16_16(v->sx) - e->x) / dy;
					e->x += e->dx;
				}
			}
		}

		int count = edge[0].y > edge[1].y ? edge[1].y - y : edge[0].y - y;

		for (; count > 0; count--, y++, vram += width)
		{
			Edge* l = edge[0].x < edge[1].x ? &edge[0] : &edge[1];
			short x = Fixed16_16ToShort(l->x);
			Edge* r = l == edge ? &edge[1] : &edge[0];
			short len = Fixed16_16ToShort(r->x) - x + 1;
			if (y >= 0 && y < height)
			{
				if (x < 0)
				{
					len += x;
					x = 0;
				}
				if (x + len > width)
					len = width - x;
				if (len > 0)
					memset(vram + x, m_fgColor, len);
			}
			for (unsigned char k = 0; k < 2; k++)
				edge[k].x += edge[k].dx;
		}
	}
}

void WShadowViewSw::Filtering()
{
	int width = (int)GetWidth();
	int height = (int)GetHeight();

	int offset[4];
	offset[0] = 0;
	offset[1] = 1;
	offset[2] = width;
	offset[3] = width + 1;

	int w = width / 2;
	int h = height / 2;
	int y;

	for (y = 1; y < h - 1; y++)
	{
		unsigned char* src = m_paTargetBuff + y * 2 * width;
		unsigned char* dst = m_pUpdate->GetVram(y);
		for (int x = 0; x < w; x++)
		{
			int sum = 0, k = 0;
			do
				sum += src[offset[k]];
			while (++k < 4);
			*dst++ = sum >> 2;
			src += 2;
		}
	}
	for (y = 0; y < w; y++)
	{
		m_pUpdate->GetVram(h)[-y - 1] = 0xff;
		m_pUpdate->vram[y] = 0xff;
	}
	for (y = 0; y < h; y++)
	{
		m_pUpdate->GetVram(y)[w - 1] = 0xff;
		m_pUpdate->GetVram(y)[0] = 0xff;
	}
}

void WShadowViewSw::Render(WPuppet* pet, bool bTransform, float matscale,
	bool noclipping, bool updatemat)
{
	pet->Render(this, bTransform, matscale, noclipping, updatemat, true);
}

void WShadowViewSw::Render()
{
	Filtering();
	if (m_texture)
		GetResrcManager()->FixTexture(m_texture, m_pUpdate, 0, 0);
}

WTVertex WShadowViewHw::m_vtx[4];
WTVertex* WShadowViewHw::m_vl[5] = { &m_vtx[0], &m_vtx[1], &m_vtx[3], &m_vtx[2],
	0 };

WShadowViewHw::WShadowViewHw(unsigned char shadowDense)
	: WShadowView(shadowDense), m_filteredTexView(0), m_r2tBegun(false)
{
}

WShadowViewHw::~WShadowViewHw()
{
	if (GetResrcManager() && m_orgTexParam.m_rtTexInfo0.m_hTex)
	{
		GetResrcManager()->Release(m_orgTexParam.m_rtTexInfo0.m_hTex);
		m_orgTexParam.m_rtTexInfo0.m_hTex = 0;
	}
	if (m_filteredTexView)
	{
		delete m_filteredTexView;
		m_filteredTexView = 0;
	}
}

void WShadowViewHw::SetTextureSize(int w, int h)
{
	SetViewport((float)w, (float)h);
	m_orgTexParam.m_rtTexInfo0.m_needToClear = true;
	m_orgTexParam.m_rtTexInfo0.m_clearClr = 0xffffffff;
	m_orgTexParam.m_depthSurfInfo.m_surfUsage =
		WRenderToTextureParam::DepthSurfInfo::USE_SHARED_SURFACE;
	m_orgTexParam.m_depthSurfInfo.m_needToClear = true;
	m_orgTexParam.m_depthSurfInfo.m_clearZ = 1.0f;

	m_filteredTexView = new WTextureView;
	m_filteredTexView->SetResourceManager(GetResrcManager());
	m_filteredTexView->SetViewport(GetWidth() * 0.5f, GetHeight() * 0.5f);

	WTextureView::TextureParam param;
	param.m_numRts = 1;
	param.m_rtTexInfo[0].m_texStyle = 0x14000;
	param.m_rtTexInfo[0].m_sizeInfo = WRenderToTextureSizeInfo::SIZE_ABS;
	param.m_rtTexInfo[0].m_needToClear = false;
	param.m_rtTexInfo[0].m_clearClr = 0;
	m_filteredTexView->SetTextureParam(param);
}

void WShadowViewHw::Clear(ulong clearColor, int mode)
{
	GetVideoDevice()->EndScene();
	UpdateCamera();

	if (GetVideoDevice()->GetDeviceState() == W_VDEVSTATE_NORMAL &&
		!m_orgTexParam.m_rtTexInfo0.m_hTex)
	{
		Bitmap bitmap;
		BITMAPINFO bi;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biWidth = (int)GetWidth();
		bi.bmiHeader.biHeight = (int)GetHeight();
		bitmap.SetBITMAPINFO(&bi, 0);
		m_orgTexParam.m_rtTexInfo0.m_hTex =
			GetResrcManager()->UploadTexture(0, &bitmap, 0x14000, 0);
	}

	m_r2tBegun = false;
	if (m_orgTexParam.m_rtTexInfo0.m_hTex > 0)
	{
		m_r2tBegun = GetVideoDevice()->BeginRenderToTexture(&m_orgTexParam);
		if (m_r2tBegun)
			m_r2tBegun = GetVideoDevice()->BeginScene();
	}
}

void WShadowViewHw::Render(WPuppet* pet, bool bTransform, float matscale,
	bool noclipping, bool updatemat)
{
	if (m_r2tBegun)
	{
		WVideoDev* video = GetVideoDevice();
		video->BeginUsingCustomRenderState();
		if (video->IsSupportVs() && video->IsSupportPs())
		{
			video->SetCustomFxMacro(0x20000);
			video->SetCustomFxParamVector4(WFxParamShadowColor,
				WVector4(m_fgColor / 255.0f, m_fgColor / 255.0f,
					m_fgColor / 255.0f, 1.0f));
		}
		else
		{
			video->SetCustomRenderState(WVDRS_LIGHTING, 0);
			video->SetCustomRenderState(WVDRS_TEXTUREFACTOR,
				0xff000000 | (m_fgColor << 16) | (m_fgColor << 8) | m_fgColor);
			video->SetCustomRenderState(WVDRS_CLIPPING, 1);
			video->SetCustomTextureStageState(0, WVDTSS_COLORARG2, 3);
			video->SetCustomTextureStageState(0, WVDTSS_COLOROP, 3);
			video->SetCustomTextureStageState(1, WVDTSS_COLOROP, 1);
			video->SetCustomTextureStageState(0, WVDTSS_ALPHAOP, 2);
		}
		video->SetCustomRenderState(WVDRS_SRCBLEND, 2);
		video->SetCustomRenderState(WVDRS_DESTBLEND, 1);
		pet->Render(this, bTransform, matscale, noclipping, updatemat, false);
		video->EndUsingCustomRenderState();
	}
}

void WShadowViewHw::Render()
{
	if (m_r2tBegun)
	{
		Flush(0);
		GetVideoDevice()->EndScene();
		GetVideoDevice()->EndRenderToTexture(&m_orgTexParam);
		GetVideoDevice()->BeginScene();

		float x = 0.0f, y = 0.0f;
		m_vtx[0].sx = x - 0.5f;
		m_vtx[0].sy = y - 0.5f;
		m_vtx[0].sz = 0.001f;
		m_vtx[0].rhw = 1.0f;
		m_vtx[0].tu = 0.0f;
		m_vtx[0].tv = 0.0f;

		m_vtx[1].sx = m_filteredTexView->SCREEN_XS - 0.5f;
		m_vtx[1].sy = y - 0.5f;
		m_vtx[1].sz = 0.001f;
		m_vtx[1].rhw = 1.0f;
		m_vtx[1].tu = 1.0f;
		m_vtx[1].tv = 0.0f;

		m_vtx[2].sx = x - 0.5f;
		m_vtx[2].sy = m_filteredTexView->SCREEN_YS - 0.5f;
		m_vtx[2].sz = 0.001f;
		m_vtx[2].rhw = 1.0f;
		m_vtx[2].tu = 0.0f;
		m_vtx[2].tv = 1.0f;

		m_vtx[3].sx = m_filteredTexView->SCREEN_XS - 0.5f;
		m_vtx[3].sy = m_filteredTexView->SCREEN_YS - 0.5f;
		m_vtx[3].sz = 0.001f;
		m_vtx[3].rhw = 1.0f;
		m_vtx[3].tu = 1.0f;
		m_vtx[3].tv = 1.0f;

		m_filteredTexView->Begin();
		m_filteredTexView->DrawPolygonFan(m_vl,
			(m_orgTexParam.m_rtTexInfo0.m_hTex & 0x7ff) | 0x60300000, 0, true);

		_WPOINT p[5];
		p[0].x = 0.0f;
		p[0].y = 0.0f;
		p[1].x = m_filteredTexView->GetWidth() - 1.0f;
		p[1].y = 0.0f;
		p[2].x = m_filteredTexView->GetWidth() - 1.0f;
		p[2].y = m_filteredTexView->GetHeight() - 1.0f;
		p[3].x = 0.0f;
		p[3].y = m_filteredTexView->GetHeight() - 1.0f;
		p[4].x = 0.0f;
		p[4].y = 0.0f;
		for (int i = 0; i < 4; i++)
			m_filteredTexView->DrawLine2D(p[i], p[i + 1], 0xffffffff,
				0x20300000);

		m_filteredTexView->End();
		m_r2tBegun = false;
	}
}

WRecvView::WRecvView()
{
	m_bProcessEffect = false;
	m_bDisableFog = true;
	m_renderview = 0;
	SetViewport(1.0f, 1.0f);
}

WRecvView::~WRecvView()
{
}

void WRecvView::SetRenderView(WView* view)
{
	m_renderview = view;
}

WView* WRecvView::GetRenderView()
{
	return m_renderview;
}

WTVertex** WRecvViewSw::plist;
WTVertex* WRecvViewSw::pl;
unsigned short* WRecvViewSw::flist;
unsigned short* WRecvViewSw::fl;
int WRecvViewSw::m_count = 0;
WTVertex** WRecvViewSw::g_vtxIdxList = 0;

WRecvViewSw::WRecvViewSw()
{
	m_vtxList = 0;
	m_faceList = 0;
	m_vtxIdxList = 0;
	m_allocLen = 0;
	m_cur_vtx = 0;
	m_cur_face = 0;
	if (m_count++ == 0)
	{
		plist = new WTVertex*[8000];
		pl = new WTVertex[8000];
		flist = new unsigned short[24000];
		fl = new unsigned short[24000];
	}
}

WRecvViewSw::~WRecvViewSw()
{
	if (m_vtxList)
		delete[] m_vtxList;
	if (m_vtxIdxList)
		delete[] m_vtxIdxList;
	if (m_faceList)
		delete[] m_faceList;
	if (--m_count == 0)
	{
		delete[] plist;
		delete[] pl;
		delete[] flist;
		delete[] fl;
	}
}

void WRecvViewSw::AddPolygonList(WTVertex** vl)
{
	int n = 0;
	if (vl[0])
		while (vl[++n])
			;

	if (m_cur_vtx + n >= m_allocLen || m_cur_face + n * 3 - 6 >= m_allocLen)
		AllocVtxBuffMore();

	for (int i = 0; i < n; i++)
		memcpy(&m_vtxList[m_cur_vtx + i], vl[i], sizeof(WTVertex));

	for (int j = 2; j < n; j++)
	{
		m_faceList[m_cur_face++] = m_cur_vtx + j - 1;
		m_faceList[m_cur_face++] = m_cur_vtx + j;
		m_faceList[m_cur_face++] = m_cur_vtx;
	}
	m_cur_vtx += n;
}

void WRecvViewSw::Clear(ulong clearColor, int mode)
{
	UpdateCamera();
}

void WRecvViewSw::DrawPolygonFan(WTVertex** vl, int clipoption, int drawoption)
{
	if (drawoption & 0xa5800000)
		return;
	if (!vl)
		return;
	for (int i = 0; vl[i]; i++)
	{
		vl[i]->tu = vl[i]->sx;
		vl[i]->tv = vl[i]->sy;
		vl[i]->SetPosition(WVector(vl[i]->sx, vl[i]->sy, vl[i]->sz));
	}
	AddPolygonList(vl);
}

void WRecvViewSw::AddPolygonIdxList(WTVertex** p, int pnum, unsigned short* f,
	int fnum)
{
	while (m_cur_vtx + pnum >= m_allocLen || m_cur_face + fnum >= m_allocLen)
		AllocVtxBuffMore();

	for (int i = 0; i < pnum; i++)
		memcpy(&m_vtxList[m_cur_vtx + i], p[i], sizeof(WTVertex));
	for (int j = 0; j < fnum; j++)
		m_faceList[m_cur_face++] = f[j] + m_cur_vtx;
	m_cur_vtx += pnum;
}

int WRecvViewSw::GetClipCode(float x, float y, float z)
{
	int code = x < 0.0f ? 4 : (x > 1.0f ? 8 : 0);
	code |= y < 0.0f ? 0x10 : (y > 1.0f ? 0x20 : 0);
	code |= z < clip_scaled_near ? 1 : 0;
	return code | (z > clip_scaled_far ? 2 : 0);
}

void WRecvViewSw::ClipEdge(WTVertex* out, const WTVertex* a, const WTVertex* b,
	float t)
{
	out->sx = (b->sx - a->sx) * t + a->sx;
	out->sy = (b->sy - a->sy) * t + a->sy;
	out->sz = (b->sz - a->sz) * t + a->sz;
	out->tu = (b->tu - a->tu) * t + a->tu;
	out->tv = (b->tv - a->tv) * t + a->tv;
}

void WRecvViewSw::DrawIndexedTriangles(WTVertex* p, int pnum, unsigned short* f,
	int fnum, int drawoption, int drawoption2)
{
	static unsigned char ccodes[8000];
	int fl, i;

	if (drawoption & 0x80000000 || drawoption & 0x21800000)
		return;

	for (i = 0; i < pnum; i++)
	{
		WVector v(p[i].sx, p[i].sy, p[i].sz);
		WVector t = v * matrix;
		float w = 1.0f / t.z;

		pl[i].tu = t.x * w + m_center_x;
		pl[i].tv = t.y * w + m_center_y;
		pl[i].SetPosition(v);

		ccodes[i] = GetClipCode(pl[i].tu, pl[i].tv, t.z);
	}

	fl = 0;

	for (i = 0; i < fnum / 3; i++)
	{
		int codes_and = 0xff, codes_or = 0;
		for (int k = 0; k < 3; k++)
		{
			codes_or |= ccodes[f[i * 3 + k]];
			codes_and &= ccodes[f[i * 3 + k]];
		}
		if (codes_and)
			continue;
		unsigned short* p = &f[i * 3];
		int fn = 3;
		if (codes_or)
		{
			for (unsigned char mask = 1; codes_or; mask <<= 1)
			{
				if (!(codes_or & mask))
					continue;
				codes_or &= ~mask;

				if (mask != 0x10 && mask != 0x20)
					continue;

				static unsigned short flist1[16];
				static unsigned short flist2[2];
				int p2n = 0;
				unsigned short* p2 = p == flist1 ? flist2 : flist1;
				int i1, i0;
				for (i1 = 0, i0 = fn - 1; i1 < fn; i0 = i1, i1++)
				{
					WTVertex* a = &pl[p[i1]];
					const WTVertex* b = &pl[p[i0]];
					if (!(ccodes[p[i0]] & mask))
						p2[p2n++] = p[i0];
					if ((ccodes[p[i1]] ^ ccodes[p[i0]]) & mask)
					{
						float t = 0.0f;
						switch (mask)
						{
						case 1:
							t = (clip_scaled_near - a->sz) / (b->sz - a->sz);
							break;
						case 2:
							t = (clip_scaled_far - a->sz) / (b->sz - a->sz);
							break;
						case 4:
							t = (0.0f - a->tu) / (b->tu - a->tu);
							break;
						case 8:
							t = (1.0f - a->tu) / (b->tu - a->tu);
							break;
						case 0x10:
							t = (0.0f - a->tv) / (b->tv - a->tv);
							break;
						case 0x20:
							t = (1.0f - a->tv) / (b->tv - a->tv);
							break;
						}

						ClipEdge(&pl[pnum], a, b, t);

						WVector tt =
							WVector(pl[pnum].sx, pl[pnum].sy, pl[pnum].sz) *
							matrix;
						float w = 1.0f / tt.z;

						pl[pnum].tu = tt.x * w + m_center_x;
						pl[pnum].tv = tt.y * w + m_center_y;

						ccodes[pnum] =
							GetClipCode(pl[pnum].tu, pl[pnum].tv, clip_near);
						p2[p2n++] = pnum++;
					}
				}
				p = p2;
				fn = p2n;
			}
		}

		for (int k = 0; k + 2 < fn; k++)
		{
			flist[fl * 3] = p[0];
			flist[fl * 3 + 1] = p[k + 1];
			flist[fl * 3 + 2] = p[k + 2];
			fl++;
		}
	}

	if (fl > 0)
	{
		static unsigned short idxtable[8000];
		unsigned char dirty[8000];

		memset(dirty, 0, (pnum + 7) / 8);

		int pn;
		for (i = pn = 0; i < fl * 3; i++)
		{
			int idx = flist[i];
			if (!(dirty[idx >> 3] & (1 << (idx & 7))))
			{
				dirty[idx >> 3] |= 1 << (idx & 7);
				plist[pn] = &pl[idx];
				idxtable[idx] = pn;
				flist[i] = idxtable[idx];
				pn++;
			}
			else
				flist[i] = idxtable[idx];
		}

		AddPolygonIdxList(plist, pn, flist, fl * 3);
	}
}

void WRecvViewSw::AllocVtxBuffMore()
{
	unsigned short* oldFace = m_faceList;
	m_allocLen += 0x400;
	WTVertex* oldVtx = m_vtxList;
	WTVertex** oldIdxVtx = m_vtxIdxList;

	m_vtxList = new WTVertex[m_allocLen];
	m_vtxIdxList = new WTVertex*[m_allocLen];
	m_faceList = new unsigned short[m_allocLen * 3];

	for (int i = 0; i < m_allocLen; i++)
		m_vtxIdxList[i] = &m_vtxList[i];

	if (m_cur_vtx > 0)
		memcpy(m_vtxList, oldVtx, m_cur_vtx * sizeof(WTVertex));
	if (m_cur_face > 0)
		memcpy(m_faceList, oldFace, m_cur_face * sizeof(unsigned short));

	if (oldVtx)
		delete[] oldVtx;
	if (oldIdxVtx)
		delete[] oldIdxVtx;
	if (oldFace)
		delete[] oldFace;
}

int __cdecl WRecvViewSw::CompareDepthPoly(const void* elem1, const void* elem2)
{
	const unsigned short* f1 = (const unsigned short*)elem1;
	const unsigned short* f2 = (const unsigned short*)elem2;
	float z1 = g_vtxIdxList[f1[2]]->sz + g_vtxIdxList[f1[1]]->sz +
		g_vtxIdxList[f1[0]]->sz;
	float z2 = g_vtxIdxList[f2[2]]->sz + g_vtxIdxList[f2[1]]->sz +
		g_vtxIdxList[f2[0]]->sz;
	if (z1 == z2)
		return 0;
	return z1 > z2 ? 1 : -1;
}

void WRecvViewSw::SortFace()
{
	g_vtxIdxList = m_vtxIdxList;
	qsort(m_faceList, m_cur_face / 3, 6, CompareDepthPoly);
}

void WRecvViewSw::CollectRecvTris(WPuppet* pPet, bool bTransform,
	float matscale, bool noclipping, bool updatemat)
{
	pPet->Render(this, bTransform, matscale, noclipping, updatemat, true);
}

void WRecvViewSw::Render(WShadowView* shadowView, WPuppet* pPet)
{
	int flag = (shadowView->GetTexture() & 0x7ff) | 0x61200000;
	if (m_cur_vtx > 0)
		m_renderview->DrawIndexedTrianglesDirect(m_vtxList, m_cur_vtx,
			m_faceList, m_cur_face, flag | 0x2000000, 0);
	m_cur_vtx = 0;
	m_cur_face = 0;
}

WRecvViewHw::WRecvViewHw()
{
	m_hasRecver = false;
}

WRecvViewHw::~WRecvViewHw()
{
}

void WRecvViewHw::Clear(ulong clearColor, int mode)
{
	UpdateCamera();
}

void WRecvViewHw::CollectRecvTris(WPuppet* pPet, bool bTransform,
	float matscale, bool noclipping, bool updatemat)
{
	MakeEqualToRenderView();
	m_hasRecver = InFrustum(pPet->m_bound);
}

void WRecvViewHw::Render(WShadowView* shadowView, WPuppet* pPet)
{
	if (m_hasRecver)
	{
		if (shadowView->GetTexture() > 0)
		{
			WVideoDev* video = GetVideoDevice();
			WMatrix4 mView;
			WMatrix4 mTexTransf = shadowView->xGetViewState().xmView *
				shadowView->xGetViewState().xmProj * ms_uvConvMat;
			if (!video->IsSupportVs() || !video->IsSupportPs())
			{
				SetWMatrix4FromWMatrix(mView, camera);
				mTexTransf = mView * mTexTransf;
			}

			video->BeginUsingCustomRenderState();
			video->SetCustomTexture(0, shadowView->GetTexture());
			video->SetCustomRenderState(WVDRS_LIGHTING, 0);
			video->SetCustomRenderState(WVDRS_SRCBLEND, 1);
			video->SetCustomRenderState(WVDRS_DESTBLEND, 3);
			video->SetCustomRenderState(WVDRS_ZWRITEENABLE, 0);
			video->SetCustomRenderState(WVDRS_ZFUNC, 3);
			if (video->IsSupportVs() && video->IsSupportPs())
			{
				video->SetCustomFxMacro(0x40000);
				video->SetCustomFxParamMatrix(WFxParamTextureTransform,
					mTexTransf);
			}
			else
			{
				video->SetCustomTextureStageState(0, WVDTSS_COLORARG1, 2);
				video->SetCustomTextureStageState(0, WVDTSS_COLOROP, 2);
				video->SetCustomTextureStageState(1, WVDTSS_COLOROP, 1);
				video->SetCustomTextureStageState(0, WVDTSS_TEXCOORDINDEX,
					0x20000);
				video->SetCustomTextureStageState(0,
					WVDTSS_TEXTURETRANSFORMFLAGS, 0x104);
				video->SetCustomTransform(WVDTS_TEXTURE0, mTexTransf);
			}
			video->SetCustomSamplerState(0, WVDSAMP_ADDRESSU, 3);
			video->SetCustomSamplerState(0, WVDSAMP_ADDRESSV, 3);
			pPet->Render(this, true, 1.0f, false, false, false);
			Flush(0);
			video->EndUsingCustomRenderState();
		}
		m_hasRecver = false;
	}
}

void WRecvViewHw::MakeEqualToRenderView()
{
	if (m_renderview)
	{
		SetViewport(m_renderview->GetWidth(), m_renderview->GetHeight());
		SetFOV_Unmodified(m_renderview->GetFOV_Unmodified());
		SetClip(m_renderview->GetClipNearValue(),
			m_renderview->GetClipFarValue(), false);
		SetCamera(m_renderview->GetCamera());
		UpdateCamera();
	}
}
