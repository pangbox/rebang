#pragma once
#include "wpuppet.h"
#include "wxtnlbuffer.h"

class WxPuppet : public WPuppet
{
public:
	WxPuppet(bool needBuffer);
	virtual ~WxPuppet();
	virtual int LoadPET(char* name, bool flag, int loadFlags);
	virtual void xUpdateTnLBuffers(bool rebuild);
	virtual WPuppet* MakeClone(bool needBuffer);
	void SetAlpha(unsigned char alpha);
	virtual void UpdateLightSource(LightSet* light, bool addLight,
		WScene* scene);
	virtual void UpdateMeshBonePtr(WPuppet* source);
	virtual void Render(WView* view, bool transform, float scale, bool flag0,
		bool flag1, bool software);

private:
	void xReleaseTnLBuffer();
	void xShareTnLResources(WxPuppet* destination);
	void xUpdateTnLResources();

	bool m_xbNeedTnLBuf;
	WxTnLBuffer* m_xpTnLBuf;
};

class WxStaticPuppetGrp : public WResource
{
public:
	WxStaticPuppetGrp();
	virtual ~WxStaticPuppetGrp();

	int xLoad(int count, const char* name, WPuppet** puppets);
	void xUpdateCullFlag(WView* view);
	void xRender(WView* view);

private:
	void xReleaseTnLBuffer();

	int m_xnPets;
	WPuppet** m_xpapPets;
	WxTnLBuffer m_xTnLBuf;
};
