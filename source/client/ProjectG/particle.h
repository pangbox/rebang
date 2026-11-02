#pragma once

#include <list>
#include <vector>
#include <string>
#include "wtypes.h"
#include "wmath.h"
#include "wlist.h"
#include "actor.h"
#include "scenemanager.h"

class W3dAniSpr;
class CGolfDoc;
class CBgEffect;
class CParticle;
class CParticleManager;
class CParticleRecv;
class IObject;
class Bitmap;
class CLightColumn;

struct sRecv
{
	WVector center;
	float ground;
	float duration;
};

IObject* CParticleEffectMakeInstance(void);

class CParticleEffect : public IActor, public CRenderFuncPtr
{
	DECLARE_OBJECT(CParticleEffect)

public:
	virtual ~CParticleEffect(void) { }
	virtual void OnPreLoadInit(void);
	virtual void OnInit(void);
	virtual void OnProcess(float elapsed);
	virtual void OnDestroy(void);
	virtual void HandleMsg(const MsgObject& msg);
	virtual void DisplayShadow(void);
	virtual void Display(void);

	bool CheckExtShotEffect(const char* sprayName, const char* sfxName);
	void CallSprayParticle(const char* sprayName, const char* sfxName,
		const WVector* pPos);

protected:
	CParticleEffect(void);

	void Clear(void);

	WList<CParticleRecv*> m_recvList;
	CParticleManager* m_pManager;
	sRecv m_recv;
	WList<CBgEffect*> m_bgEffectList;
	CLightColumn* m_pParticle;
};

class CParticleManager
{
public:
	struct sTexture
	{
		sTexture(void)
			: m_handle(0)
		{
		}

		char m_fileName[0x20];
		int m_handle;
	};

	CParticleManager(void);
	~CParticleManager(void);

	void Process(float elapsed);
	void Display(void);
	void DisplayShadow(void);
	void MakeParticle(CParticle* pParticle, const char* texName,
		bool bPermanent);
	void RemoveAll(void);
	void Remove(const char* texName);

protected:
	void RegisterTexture(const char* name, const char* fileName);

	CParticle* m_particles[20];
	int m_lastIndex;
	int m_count;
	int m_permanentCount;
	WList<sTexture*> m_textures;
};

class CParticleRecv
{
	friend class CParticleEffect;

public:
	struct sFrag
	{
		WVector pos;
		WVector vel;
		float alpha;
		bool alive;
		float angle;
		float spin;
		float size;
		int sprNum;
		float frameTime;
		float life;
		float lifeLeft;
	};

	CParticleRecv(const char* fileName);
	~CParticleRecv(void);

	void SetTarget(WVector* pTarget);
	void Process(float elapsed);
	void Display(void);
	void SetBox(const Waabb& box);
	static void Disconnect(void);

	void SetActive(bool active) { m_active = active; }

	bool IsActive(void) { return m_active; }

	void SetCenter(const WVector& center) { m_center = center; }

	void SetGround(float ground) { m_ground = ground; }

	void SetDuration(float duration)
	{
		if (duration > 0.0f)
			m_emitTime = duration;
	}

protected:
	void Destroy(void);
	void ApplyScale(void);
	void Generate(void);
	void BlowAway(float elapsed, int index);

	int m_emitShape;
	float m_alphaStart;
	float m_alphaEnd;
	float m_sizeStart;
	float m_sizeEnd;
	float m_speedMin;
	float m_speedMax;
	float m_distMin;
	float m_distMax;
	float m_spinMin;
	float m_spinMax;
	float m_gravity;
	float m_lifeMin;
	float m_lifeMax;
	float m_frameInterval;
	float m_interval;
	float m_bounce;
	float m_emitTime;
	float m_ground;
	int m_renderType;
	bool m_active;
	sFrag* m_frags;
	int m_liveCount;
	W3dAniSpr* m_aniSpr;
	WMatrix m_dirMatrix;
	WVector m_dirAngles;
	WVector m_accel;
	Waabb m_box;
	float m_spread;
	bool m_randomFrame;
	float m_spriteSize;
	int m_startFrame;
	char m_texName[0x20];
	int m_fragCount;
	int m_nextFrag;
	WVector* m_pTarget;
	WVector m_center;
	float m_timer;
	int m_emitCount;
	bool m_visible;
	int m_objIndex;
	float m_alpha;
	float m_scale;

private:
	static CGolfDoc* m_pDoc;
};

class CParticle2D
{
public:
	enum eSTYLE
	{
		STYLE_NORMAL = 0,
		STYLE_TWINKLE = 1,
	};

	struct sParticle
	{
		WVector2D m_pos;
		WVector2D m_vel;
		const Bitmap* m_pBitmap;
		float m_alpha;
		int m_fadeIn;
		float m_speed;
		int m_alive;
		WRect m_src;
		WRect m_dest;
	};

	CParticle2D() { }
	~CParticle2D();

	int Initialize(const std::list<std::string>& bitmapNames, const WRect& rect,
		unsigned int maxCount, eSTYLE style, float speedMin, float speedMax,
		const WVector2D& velMin, const WVector2D& velMax);
	void Process(float elapsed);
	void Display(void);
	void Release(void);

private:
	void MakeDisplayRect(sParticle* pParticle);

protected:
	WRect m_rect;
	eSTYLE m_style;
	unsigned int m_maxCount;
	float m_speedMin;
	float m_speedMax;
	WVector2D m_velMin;
	WVector2D m_velMax;
	std::list<sParticle*> m_particles;
	std::vector<const Bitmap*> m_bitmaps;
};
