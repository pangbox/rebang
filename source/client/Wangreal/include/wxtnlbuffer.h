#pragma once
#include "wtypes.h"

class WPuppet;
class WVideoDev;
class WView;

struct WxVb
{
	int xhVb;
	int xnVtxs;
	ulong xdwFVF;
	ulong xdwUsage;
	WxVb* xpNext;
};

struct WxIb
{
	int xhIb;
	int xnIdxs;
	ulong xdwUsage;
};

struct WxBatchGrp;

class WxTnLBuffer
{
public:
	WxTnLBuffer();
	~WxTnLBuffer();

	void xClear();
	void xCopy(WxTnLBuffer** destination, WVideoDev* video);
	void xSetRootBone(WPuppet* puppet);
	WxBatchGrp* xGetBatchGrps();
	int xCreateBuffers(WVideoDev* video, WPuppet* puppet);
	int xCreateStaticBuffers(WVideoDev* video, WPuppet** puppets, int count);
	int xFillBuffers();
	void xUpdateStaticBufferInfo(WPuppet** puppets, int count);
	void xUpdateStaticBufferInfo(WPuppet* puppet, int count);
	void xUpdateMatrixIdx(WPuppet* puppet);
	void xRender(WView* view);

private:
	int m_xiFlag;
	WxVb* m_xpaVbs;
	WxIb m_xaIbs[32];
	WxBatchGrp* m_xpaBatchGrps;
	WPuppet* m_xpPet;
	WVideoDev* m_xpVideo;
};
