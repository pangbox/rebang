#include <math.h>
#include <new>
#include <stdio.h>
#include <string.h>
#include "wxpuppet.h"
#include "wbone.h"
#include "wview.h"

WxPuppet::WxPuppet(bool needBuffer)
	: m_xbNeedTnLBuf(needBuffer)
{
	m_xpTnLBuf = 0;
}

WxPuppet::~WxPuppet()
{
	xReleaseTnLBuffer();
}

void WxPuppet::xReleaseTnLBuffer()
{
	if (m_xpTnLBuf)
	{
		delete m_xpTnLBuf;
		m_xpTnLBuf = 0;
	}
}

int WxPuppet::LoadPET(char* name, bool flag, int loadFlags)
{
	int result = WPuppet::LoadPET(name, flag, loadFlags);
	char hint[260];
	sprintf(hint, "WxPuppet:%s", name);
	SetLeakHint(hint);
	if (m_xbNeedTnLBuf && result == 0)
	{
		xReleaseTnLBuffer();
		m_xpTnLBuf = new WxTnLBuffer;
		WVideoDev* video = GetResrcManager()->video;
		int createResult = m_xpTnLBuf->xCreateBuffers(video, this);
		return m_xpTnLBuf->xFillBuffers() | createResult;
	}
	return result;
}

void WxPuppet::xUpdateTnLBuffers(bool rebuild)
{
	if (rebuild)
	{
		xReleaseTnLBuffer();
		m_xpTnLBuf = new WxTnLBuffer;
		m_xpTnLBuf->xCreateBuffers(GetResrcManager()->video, this);
	}
	m_xpTnLBuf->xFillBuffers();
}

void WxPuppet::xShareTnLResources(WxPuppet* destination)
{
	// HACK: this is a workaround for an ordering issue for exactness
	(void)&WList<int>::Next;
	if (m_xpTnLBuf)
	{
		m_xpTnLBuf->xCopy(&destination->m_xpTnLBuf, GetResrcManager()->video);
		destination->m_xpTnLBuf->xSetRootBone(destination);
	}
}

void WxPuppet::xUpdateTnLResources()
{
	m_xpTnLBuf->xSetRootBone(this);
}

void WxPuppet::SetAlpha(unsigned char alpha)
{
	if (m_xpTnLBuf == 0)
		m_rootbone->SetAlpha(alpha, true);
	else
		m_rootbone->xSetAlpha(alpha, true);
}

void WxPuppet::UpdateLightSource(LightSet* light, bool addLight, WScene* scene)
{
	m_rootbone->SetLight(light);
	if (m_xpTnLBuf == 0)
	{
		int lightMode = m_lightmode;
		if (addLight)
			lightMode |= 0x08000000;
		m_rootbone->CalcLight(light, true, lightMode, scene);
	}
}

void WxPuppet::UpdateMeshBonePtr(WPuppet* source)
{
	WPuppet::UpdateMeshBonePtr(source);
	if (m_xpTnLBuf)
		m_xpTnLBuf->xUpdateMatrixIdx(source);
}

void WxPuppet::Render(WView* view, bool transform, float scale, bool flag0,
	bool flag1, bool software)
{
	if (software)
	{
		WPuppet::Render(view, transform, scale, flag0, flag1, false);
		return;
	}

	if (m_xpTnLBuf)
	{
		if (transform)
			Transform(view, scale, (flag0 ? 1 : 0) | (flag1 ? 0 : 2));
		view->xSetLight(0, m_rootbone->m_light);
		m_xpTnLBuf->xRender(view);
	}
}

WxStaticPuppetGrp::WxStaticPuppetGrp()
	: m_xnPets(0), m_xpapPets(0)
{
}

WxStaticPuppetGrp::~WxStaticPuppetGrp()
{
	xReleaseTnLBuffer();
	if (m_xpapPets)
	{
		delete[] m_xpapPets;
		m_xpapPets = 0;
	}
	m_xnPets = 0;
}

void WxStaticPuppetGrp::xReleaseTnLBuffer()
{
	m_xTnLBuf.xClear();
}

int WxStaticPuppetGrp::xLoad(int count, const char*, WPuppet** puppets)
{
	m_xnPets = 0;
	m_xpapPets = new WPuppet*[count];
	memcpy(m_xpapPets, puppets, count * sizeof(WPuppet*));
	m_xTnLBuf.xCreateStaticBuffers(GetResrcManager()->video, puppets, count);
	m_xTnLBuf.xUpdateStaticBufferInfo(puppets, count);
	m_xTnLBuf.xFillBuffers();
	m_xnPets = count;
	return 0;
}

void WxStaticPuppetGrp::xUpdateCullFlag(WView* view)
{
	for (int i = 0; i < m_xnPets; ++i)
	{
		WPuppet* puppet = m_xpapPets[i];
		if (puppet->m_rootbone)
		{
			float scale = (float)sqrt(puppet->m_mat.xx * puppet->m_mat.xx +
				puppet->m_mat.yx * puppet->m_mat.yx +
				puppet->m_mat.zx * puppet->m_mat.zx);
			m_xpapPets[i]->Transform(view, scale, 2);
		}
	}
}

void WxStaticPuppetGrp::xRender(WView* view)
{
	m_xTnLBuf.xRender(view);
}

WPuppet* WxPuppet::MakeClone(bool needBuffer)
{
	// HACK: Force MakeClone to move below dtors
	(void)&WList<int>::operator-=;
	w_share_pet_data shareData;
	shareData.petName = m_petName;
	shareData.rootbone = m_rootbone;
	shareData.length = m_aniLen;
	shareData.faceNum = m_faceNum;
	shareData.numFramedata = m_framenum;
	shareData.frame = m_frame;
	shareData.bound = m_bound;
	shareData.bbList = &m_bbList;
	shareData.mdList = m_mdList;
	shareData.fanimList = &m_fanimList;

	WxPuppet* clone = new WxPuppet(needBuffer);
	clone->m_iPetType = m_iPetType;
	clone->m_rendMode = m_rendMode;
	clone->Share(&shareData);
	if (needBuffer && m_xpTnLBuf)
		xShareTnLResources(clone);
	return clone;
}
