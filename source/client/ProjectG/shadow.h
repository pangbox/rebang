#pragma once

#include <list>
#include <float.h>
#include <string.h>
#include "singleton.h"
#include "wscene.h"
#include "wmath.h"

class WResourceManager;
class WPuppet;
class WView;
class WShadowView;
class WRecvView;

extern const char* g_theCasterName;

enum eShadowMode
{
	SHADOW_MODE_SW = 0,
	SHADOW_MODE_HW = 1,
	SHADOW_MODE_HW2 = 2
};

struct sShadowRecver
{
	WPuppet* pet;
	char name[0x20];
	float height;

	sShadowRecver()
		: pet(NULL), height(FLT_MAX)
	{
		memset(name, 0, sizeof(name));
	}

	void ClearRecver()
	{
		pet = NULL;
		height = FLT_MAX;
	}
};

class CShadow
{
public:
	enum ShadowType
	{
		SHADOW_NONE = 0,
		SHADOW_PROJECTION = 1
	};

	CShadow(WResourceManager* pResMgr, eShadowMode mode);
	virtual ~CShadow();

	void SetShadowType(ShadowType type);
	void RenderCaster(WView* view, const LightSet& light, const WSphere& sphere,
		std::list<WPuppet*>& pets, sShadowRecver* recver, bool bRenderPets);
	void RenderRecver(sShadowRecver* recver);

protected:
	void RenderProjectionShadow(WView* view, const LightSet& light,
		const WSphere& sphere, std::list<WPuppet*>& pets, sShadowRecver* recver,
		bool bRenderPets);
	bool CheckFrustum(WView* view, const WMatrix& camera, float fov,
		float nearDist, float farDist);
	bool Setup(WView* view, const LightSet& light, const WSphere& sphere,
		sShadowRecver* recver);

	WShadowView* m_pShadowView;
	WRecvView* m_pRecvView;
	ShadowType m_type;
};

class CShadowCaster
{
public:
	CShadowCaster(const char* name, int state, WResourceManager* pResMgr,
		eShadowMode mode);
	~CShadowCaster();

	const char* GetName() const;
	const char* GetLightName() const;
	void Reset();
	int GetState() const;
	void SetState_New(int state);
	void ClearCaster();
	void ClearCaster_New();
	void SetLight(const char* lightName, LightSet& light);
	void UpdateLight(const LightSet& light);
	void AddPet(WPuppet* pPet);
	void AddPet_New(WPuppet* pPet);
	void DeletePet(WPuppet* pPet);
	void ReplacePets(WPuppet* pOld, WPuppet* pNew, bool bShow);
	void UpdateBSphere(WView* view, float minFov, float maxDist);
	void RenderCaster(WView* view, sShadowRecver* recver);
	void RenderRecver(sShadowRecver* recver);
	void ShowCaster(bool bShow);

protected:
	char m_name[0x20];
	char m_lightName[0x20];
	LightSet m_light;
	std::list<WPuppet*> m_pets;
	WSphere m_bsphere;
	CShadow* m_pShadow;
	int m_state;
};

typedef std::list<sShadowRecver*>::iterator ShadowRecverIter;

class CShadowManager : public WSingleton<CShadowManager>
{
public:
	CShadowManager(WResourceManager* pResMgr, bool bEnable);
	virtual ~CShadowManager();

	bool IsEnabled() const;
	void Enable(bool bEnable);
	void SetActive(bool bActive);
	void SetMinViewInfo(float minFov, float maxDist, bool flag);
	CShadowCaster* RegisterCaster_New(const char* name, int state,
		const char* lightName, LightSet& light);
	CShadowCaster* FindCaster(const char* name);
	void AddCastPet_New(const char* name, bool bClear, WPuppet* pPet);
	sShadowRecver* RegisterRecver_New(const char* name);
	void AddRecvPet_New(const char* name, WPuppet* pPet, float height);
	void UpdateLight(const char* lightName, LightSet& light);
	void Reset();
	void Reset_New();
	void Flush(WView* view);

protected:
	void ClearCasters();
	void ClearRecvers();
	void ClearCasters_New();
	void ClearRecvers_New();
	sShadowRecver* FindCommonRecver(const char* name);
	void UpdateBVolume(WView* view);

	bool m_bEnable;
	eShadowMode m_mode;
	bool m_bActive;
	WResourceManager* m_pResMgr;
	std::list<CShadowCaster*> m_casters;
	std::list<sShadowRecver*> m_recvers;
	float m_minFov;
	float m_maxDist;
	bool m_bFlag;
};
