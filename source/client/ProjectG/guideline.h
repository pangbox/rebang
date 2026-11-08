#pragma once

#include "actor.h"
#include "scenemanager.h"
class CFxSequence;
class CGuideLine : public IActor, public CRenderFuncPtr
{
protected:
	CGuideLine();

public:
	virtual ~CGuideLine() { }
	DECLARE_OBJECT(CGuideLine)
	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnInit();
	virtual void OnProcess(float delta);
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	virtual void Display();

protected:
	void SetActive(bool bActive);
	void CalcSegment();
	void SetPoint(const WVector& step);

private:
	struct sRoad
	{
		WVector left;
		WVector right;
		WVector pos;
		CFxSequence* seq;
	};

	bool m_bRemake;
	bool m_bChanged;
	int m_roadNum;
	float m_alpha;
	bool m_bActive;
	float m_lifeTime;
	bool m_bDelayFx;
	sRoad m_road[80];
	int m_texHandle;
	WTVertex m_v[4];
	WTVertex* m_vl[5];
};
