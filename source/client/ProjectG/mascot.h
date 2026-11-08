#pragma once

#include "actor.h"
#include "scenemanager.h"
#include "../../shared/classdefine.h"
class CCameraBird;
struct sMascotInfo;
class CMascot : public IActor, public CRenderFuncPtr
{
public:
	virtual ~CMascot() { }

	DECLARE_OBJECT(CMascot)

	virtual void OnPreLoadInit();
	virtual void OnLoad();
	virtual void OnReload();
	virtual void OnInit();
	virtual void OnProcess(float delta);
	virtual void OnDestroy();
	virtual void HandleMsg(const MsgObject& msg);

	virtual void Display();

	static int ChangeMascotMsgInTex(const char* msg,
		const IFF_STRUCT::sMascot* pMascot, WPuppet* pPuppet,
		const char* texName);

	struct sMascot
	{
		sMascot()
			: pBird(NULL), pIff(NULL), pInfo(NULL)
		{
		}
		CCameraBird* pBird;
		WVector pos;
		IFF_STRUCT::sMascot* pIff;
		sMascotInfo* pInfo;
	};

protected:
	CMascot() { }

	sMascot m_mascot[4];
	int m_fxHandle;
};
