#pragma once

#include "wmath.h"
#include "wvideo.h"
#include "w3danispr.h"
#include "w3dspr.h"

class IActor;
class CPetBody;

class CParticle
{
public:
	CParticle(void);
	virtual ~CParticle(void);
	virtual void Process(float fElapsed) = 0;
	virtual void Display(void) = 0;
	virtual void DisplayShadow(void) { }
	virtual void MakeParticle(void) = 0;
	int GetTexHandle(void) { return m_texHandle; }
	void LoadTexture(int handle) { m_texHandle = handle; }
	virtual void SetActive(bool bActive) { m_bActive = bActive; }
	void SetCenter(WVector const& vCenter) { m_vCenter = vCenter; }
	bool IsActive(void) { return m_bActive; }
	virtual void SetNormal(WVector const& vNormal) { }

protected:
	virtual void Generate(void) { }
	virtual void BlowAway(float fPower, int nIndex) { }

	int m_nCount;
	WVector m_vCenter;
	int m_texHandle;
	bool m_bActive;
	W3dAniSpr* m_pAniSpr;
	W3dSpr* m_pSpr;
	float m_fGenTime;
	float m_fRate;
	int m_unknown28;
	WTVertex* m_pBuffer0;
	WTVertex* m_pBuffer1;
	WTVertex* m_pBuffer2;
	WTVertex* m_pBuffer3;
	WTVertex* m_pBufferEnd;
	WTVertex m_buffers[4];
};

class CShotParticle : public CParticle
{
public:
	struct sPiece
	{
		bool m_bAlive;
		WVector m_vPos;
		WVector m_vVelocity;
		float m_fLife;
		float m_fLength;
		float m_fSlowTime;
	};

	CShotParticle(void);
	virtual ~CShotParticle(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	sPiece* m_pPieces;
	int m_nPieces;
};

class CDust : public CParticle
{
public:
	struct sParticle
	{
		WVector m_vPos;
		float m_fAngle;
		float m_fStartSize;
		float m_fEndSize;
	};

	CDust(void) { }
	CDust(int nCount);
	virtual ~CDust(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	sParticle* m_pParticles;
	int m_nParticles;
	float m_fDuration;
	float m_fRemain;
};

class CGrassParticle : public CParticle
{
public:
	struct sGrass
	{
		int m_nFrame;
		float m_fAlpha;
		float m_fAngle;
		float m_fAngularSpeed;
		WVector m_vPos;
		WVector m_vVelocity;
		bool m_bAlive;
		float m_fFadeTime;
	};

	CGrassParticle(int nCount, WVector const& vCenter);
	virtual ~CGrassParticle(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	sGrass* m_pGrass;
	int m_nGrass;
	WVector m_vOrigin;
};

class CBunkerShotParticle : public CParticle
{
public:
	struct sSmoke
	{
		WVector m_vPos;
		WVector m_vVelocity;
	};

	struct sParticle
	{
		bool m_bAlive;
		WVector m_vPos;
		WVector m_vVelocity;
		float m_fLength;
		float m_fLife;
		float m_fMaxLife;
	};

	CBunkerShotParticle(int nSmoke, int nParticle);
	virtual ~CBunkerShotParticle(void);
	virtual void MakeParticle(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);

protected:
	virtual void Generate(void);

private:
	sSmoke* m_pSmokes;
	sParticle* m_pParticles;
	int m_nSmokes;
	int m_nParticles;
	float m_fDuration;
	float m_fRemain;
	unsigned long m_dwColor;
};

class CPowerShot : public CParticle
{
public:
	CPowerShot(void);
	virtual ~CPowerShot(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	float m_fSize;
	float m_fWidth;
	int m_nSegments;
	int m_nAlpha;
	float m_fLife;
	float m_fTime;
	float m_fSpeed;
	float m_fDistance;
};

class CLightColumn : public CParticle
{
public:
	CLightColumn(void);
	virtual ~CLightColumn(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);
	void FadeIn(void);
	void FadeOut(void);

private:
	int m_nFade;
	float m_fHeight;
	float m_fDistance;
	unsigned char m_byAlpha;
	unsigned char m_byTargetAlpha;
	int m_hEffectTex[5];
	int m_hScreenTex;
	float m_fFrame;
};

class CBunkerParticle : public CParticle
{
public:
	CBunkerParticle(void);
	virtual ~CBunkerParticle(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	int m_nAlpha;
	float m_fRemain;
	int m_unknown;
	float m_fSize;
	float m_fDuration;
};

class CFragment : public CParticle
{
public:
	struct sFrag
	{
		WVector m_vPos;
		WVector m_vVelocity;
	};

	CFragment(void);
	virtual ~CFragment(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);
	virtual void SetNormal(WVector const& vNormal) { m_vNormal = vNormal; }

private:
	sFrag* m_pFrags;
	int m_nFrags;
	int m_unknownEC;
	int m_unknownF0;
	float m_fAlpha;
	WVector m_vNormal;
};

class CRipple : public CParticle
{
public:
	struct sRipple
	{
		float m_fRadius;
		float m_fPrevX;
		float m_fX;
		float m_fZ;
		float m_fSize;
		bool m_bAlive;
		float m_fDelay;
		float m_fWait;
	};

	CRipple(void);
	virtual ~CRipple(void);
	void Init(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

protected:
	void AddRipple(float fSize, float fDelay, float fX, float fZ);

private:
	sRipple* m_pRipples;
	int m_nRipples;
	CParticle* m_pSubParticle;
};

class CStarDust : public CParticle
{
public:
	struct sParticle
	{
		bool m_bAlive;
		float m_fRadius;
		WVector m_vPos;
		WVector m_vAxisX;
		WVector m_vAxisY;
		int m_unknown2C;
		int m_unknown30;
		int m_unknown34;
		WVector m_vOrigin;
		float m_fAngle;
		float m_fAngularSpeed;
		float m_fLife;
	};

	CStarDust(void);
	virtual ~CStarDust(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

protected:
	virtual void Generate(void);
	virtual void BlowAway(float fPower, int nIndex);

private:
	sParticle* m_pParticles;
	int m_nParticles;
	int m_nGenerated;
	float m_fFade;
	float m_fAlpha;
};

class CEnergyCenter : public CParticle
{
public:
	struct sSpark
	{
		bool m_bAlive;
		WVector m_vPos;
		WVector m_vDir;
		float m_fAlpha;
	};

	CEnergyCenter(int nCount, WVector const& vCenter, float fRadius);
	virtual ~CEnergyCenter(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

protected:
	void Generate(float fElapsed);

private:
	sSpark* m_pSparks;
	float m_fRadius;
	int m_nGenerated;
	int m_nSparks;
	WVector m_vOrigin;
	float m_fLife;
	float m_fTime;
	float m_fAlpha;
	float m_fMaxAlpha;
	IActor* m_pGolfBg;
};

class CStarDust2 : public CParticle
{
public:
	struct sParticle
	{
		bool m_bAlive;
		float m_fRadius;
		WVector m_vPos;
		WVector m_vAxisX;
		WVector m_vAxisY;
		int m_unknown2C;
		int m_unknown30;
		int m_unknown34;
		WVector m_vOrigin;
		float m_fAngle;
		float m_fAngularSpeed;
		float m_fLife;
	};

	CStarDust2(void);
	virtual ~CStarDust2(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

protected:
	virtual void Generate(void);

private:
	sParticle* m_pParticles;
	int m_nParticles;
	int m_nGenerated;
	float m_fFade;
	float m_fAlpha;
};

class CDroplet : public CParticle
{
public:
	struct sDroplet
	{
		WVector m_vPos;
		WVector m_vVelocity;
		int m_unknown18;
	};

	CDroplet(void);
	virtual ~CDroplet(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void MakeParticle(void);

private:
	sDroplet* m_pDroplets;
	int m_nDroplets;
	float m_fDuration;
	float m_fAlpha;
};

class CCoin : public CParticle
{
public:
	struct sCoin
	{
		sCoin(void)
			: m_pBody(NULL)
		{
		}

		CPetBody* m_pBody;
		float m_fSpin;
		WVector m_vVelocity;
		float m_fBounceSpeed;
	};

	struct sSpark
	{
		bool m_bAlive;
		WVector m_vPos;
		WVector m_vVelocity;
		float m_fAlpha;
		float m_fLife;
		float m_fSlowTime;
	};

	CCoin(int nCount, WVector const& vCenter);
	virtual ~CCoin(void);
	virtual void MakeParticle(void);
	virtual void Process(float fElapsed);
	virtual void Display(void);
	virtual void DisplayShadow(void);
	virtual void SetActive(bool bActive);

protected:
	bool ShouldDisplay(void);
	virtual void Generate(void);

private:
	sCoin* m_pCoins;
	sSpark* m_pSparks;
	int m_nCoins;
	int m_nSparks;
	WVector m_vOrigin;
	float m_fDuration;
	float m_fRemain;
	int m_nDropSounds;
};

inline void CLightColumn::FadeIn(void)
{
	m_nFade = 1;
}

inline void CLightColumn::FadeOut(void)
{
	m_nFade = 2;
}
