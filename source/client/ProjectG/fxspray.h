#pragma once

#include <list>
#include "ringq.h"

class CFxBox;
class CFxSequence;
class CPetBody;
struct Attacher;

struct defines_t
{
	char name[16][260];
	float value[16];
	int num;
};

struct CFxParticle
{
	struct sTail
	{
		WVector pos;
		int frame;
	};

	struct sBandana
	{
		WVector pos[2];
		float length;
	};

	CFxParticle();
	virtual ~CFxParticle();

	bool bStop;
	bool bGround;
	WVector pos;
	WVector reserved;
	WVector bgPos;
	WMatrix bgRot;
	WVector bubble;
	std::list<WVector> bubblePhase;
	WVector vel;
	float wind;
	WVector rotVel;
	WVector rot;
	int frame;
	float frameTime;
	float lifeTime;
	float life;
	float initSize;
	float size;
	float ratio;
	union
	{
		unsigned long color;
		unsigned char bgra[4];
	};
	W3dAniSpr* sprite;
	CPetBody* pet;
	TRingQ<sTail> tail;
	TRingQ<sBandana> bandana;
	float tailTime;
	float volOpacity;
	int spawnNum;
	float spawnTime;
};

class CFxSpray
{
	friend class CFxBox;

public:
	bool m_bMark;
	Attacher* m_attacher;
	float m_centerX;
	float m_centerY;
	WMatrix* m_attachMat[4];
	WVector* m_attachPos[4];
	defines_t m_defines;
	char m_filename[64];
	CFxBox& m_fxBox;
	std::list<CFxParticle*> m_particle;
	CFxSequence* m_parent;
	bool m_bActive;
	float m_time;
	WVector m_quake;
	WVector m_quakePos;
	unsigned char m_flash;
	int m_particleNum;
	int m_tailNum;
	WVector m_pos;
	WVector m_bgPos;
	WVector m_vel;
	CFxSpray* m_spawnParent;
	WMatrix m_rot;
	WMatrix m_nozzleRot;

	struct sInit_WVector
	{
		WVector min;
		WVector max;
	};

	struct sVel
	{
		int start;
		float startRate;
		int end;
		float endRate;
	};

	struct sRot
	{
		float start;
		float startRate;
		float end;
		float endRate;
	};

	struct sNozzleRot
	{
		WVector axis;
		int start;
		float startSpeed;
		int end;
		float endSpeed;
	};

	struct sRevol
	{
		WVector center;
		WVector axis;
		int start;
		float startSpeed;
		int end;
		float endSpeed;
	};

	struct sGen
	{
		int start;
		int end;
		int num;
		float spread;
		int count;
		float time;
	};

	struct sGravity
	{
		WVector pos;
		float power;
		float radius;
		float outerPower;
	};

	struct sFade
	{
		float start;
		unsigned char startColor[4];
		float end;
		unsigned char endColor[4];
	};

	struct sSize
	{
		float start;
		float startSize;
		float end;
		float endSize;
	};

	struct sFrame
	{
		float start;
		float end;
		int startFrame;
		int endFrame;
		int interval;
	};

	struct sQuake
	{
		WVector pos;
		int start;
		WVector startPower;
		int end;
		WVector endPower;
		int interval;
		float time;
	};

	struct sFlash
	{
		int start;
		int peak;
		int end;
	};

	struct sBubble
	{
		WVector amplitude;
		WVector frequency;
	};

	struct sSpawn
	{
		int type;
		char name[260];
		bool attached;
		float time;
		int count;
	};

	struct sSound
	{
		char name[260];
		float range;
	};

	struct Param
	{
		bool bStrongBond;
		bool bSpawnAttached;
		bool bBillboard;
		bool bAttachedToParent;
		int attachedAxis;
		W3dAniSpr* sprite;
		char petName[260];
		bool bPetLight;
		bool bSphere;
		Waabb genPos;
		bool bRegenAtSamePos;
		float camOffset;
		int lifeTime[2];
		float volMaxOpacity;
		float volCore[2];
		float friction;
		float groundFriction;
		WVector gravity;
		float groundHeight;
		float groundBounce;
		float groundRadius;
		unsigned long blend[2];
		int lighting;
		unsigned long lightColor;
		int initFrame[2];
		int tailNum;
		float tailInterval;
		unsigned char tailColor[4];
		char tailTexture[260];
		int tailTexHandle;
		float tailTexScale;
		bool bMonolith;
		bool bCameraSide;
		sInit_WVector initVelocity;
		float addWind[2];
		WVector extWind;
		int extOrientation;
		std::list<sVel> velocity;
		sInit_WVector initAngle;
		sInit_WVector initRotation;
		std::list<sRot> rotation;
		std::list<sNozzleRot> nozzleRotation;
		std::list<sNozzleRot> sprayRotation;
		std::list<sRevol> revolution;
		std::list<sRevol> preRevolution;
		std::list<sGen> generation;
		std::list<sGravity> gravityPoint;
		std::list<sFade> fade;
		float initSize[2];
		float initRatio;
		std::list<sSize> size;
		std::list<sFrame> frame;
		std::list<sQuake> quake;
		std::list<sFlash> flash;
		float distLimit[2];
		std::list<sBubble> bubble;
		std::list<sSpawn> spawn;
		std::list<sSound> sound;
		bool bEvenSphere;
	};

	Param m_param;
	unsigned char m_unused[4];
	float m_sphereAngle;

	CFxSpray(CFxBox& fxBox);
	virtual ~CFxSpray();

	void Reset();
	bool ParseFile(const char* filename, bool bUseOriginal);
	bool ParseLine(const char* line);

	void SetLight(const LightSet& light);
	void SetWind(const WVector& wind);
	void SetDistLimit(float nearDist, float farDist);
	void SetActive(bool bActive);
	WVector& Pos();
	WVector& Vel();
	WVector& InitialMinVel();
	WVector& InitialMaxVel();

protected:
	bool Process(float elapsed);
	bool Generate(float elapsed);
	bool InitParticle(bool bRegen, CFxParticle* particle, float velRate,
		float spread, Waabb& aabb, bool bFirst);
	void Spawn(const sSpawn& spawn, CFxParticle* particle);
	void Display(WView* view);
	WMatrix Rotate(float time, float elapsed);
	WMatrix RotateNozzle(float time, float elapsed);
	void Revolute(float time, float elapsed);
	void RevoluteGrand(float time, float elapsed);
	WVector GetBgPos();
	WVector GetBgPos(const CFxParticle* particle);
	WMatrix GetBgRot(const CFxParticle* particle);
	unsigned long GetParticleColor(const WView* view, const WVector& pos,
		const CFxParticle* particle, unsigned long color);
	void MakeTail(float elapsed, CFxParticle* particle, const WVector& pos);
	void DrawSprite(WView* view, CFxParticle* particle, const WVector& pos);
	void DrawTail(WView* view, CFxParticle* particle, const WVector& pos);
	void DrawBandana(WView* view, CFxParticle* particle, const WVector& pos);
	void GetBoneMatrix(WMatrix* mat);
};

typedef std::list<CFxParticle*>::iterator::value_type FxParticlePtr;

#include "fxspray.inl"
