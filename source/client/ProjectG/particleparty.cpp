#include "stdafx.h"
#include "mathconsts.h"
#include "commonutil.h"
#include "particleparty.h"
#include "clientsetting.h"
#include "golfball.h"
#include "shadow.h"
#include "petbody.h"
#include "quadtree.h"

extern LightSet g_lightset;
extern WMatrix g_camera;

#define SAFE_DELETE(p) \
	{ \
		if (p) \
		{ \
			delete (p); \
			(p) = NULL; \
		} \
	}

#define SAFE_DELETE_ARRAY(p) \
	{ \
		if (p) \
		{ \
			delete[] (p); \
			(p) = NULL; \
		} \
	}

CParticle::CParticle(void)
	: m_texHandle(0), m_bActive(false), m_pAniSpr(NULL), m_pSpr(NULL)
{
	m_pBuffer0 = &m_buffers[0];
	m_pBuffer1 = &m_buffers[1];
	m_pBuffer2 = &m_buffers[3];
	m_pBuffer3 = &m_buffers[2];
	m_pBufferEnd = NULL;
}

CParticle::~CParticle(void)
{
	SAFE_DELETE(m_pSpr);
	SAFE_DELETE(m_pAniSpr);
}

CShotParticle::CShotParticle(void)
{
	m_nPieces = 100;
	m_pPieces = new sPiece[m_nPieces];
}

CShotParticle::~CShotParticle(void)
{
	SAFE_DELETE_ARRAY(m_pPieces);
}

void CShotParticle::MakeParticle(void)
{
	CGolfBall& ball = GolfBall();
	if (ball.m_renderNum < 3)
	{
		return;
	}

	const CGolfBall::sExpected* pExpected = &ball.m_expected[3];
	if (!pExpected)
	{
		return;
	}

	for (int i = 0; i < m_nPieces; i++)
	{
		m_pPieces[i].m_bAlive = true;
		m_pPieces[i].m_vPos = ball.m_pos;
		m_pPieces[i].m_vVelocity = pExpected->vel * 0.1f +
			WVector((float)(rand() % 41) - 20.0f, (float)(rand() % 41) - 20.0f,
				(float)(rand() % 41) - 20.0f);

		m_pPieces[i].m_fLength = 0.7f;
		m_pPieces[i].m_fLife = (float)(rand() % 10) * 0.05f + 0.5f;
		m_pPieces[i].m_fSlowTime = m_pPieces[i].m_fLife * 0.6f;
	}

	m_nCount = m_nPieces;
	m_bActive = true;
}

void CShotParticle::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nPieces; i++)
	{
		sPiece* pPiece = &m_pPieces[i];

		if (pPiece->m_bAlive)
		{
			if (pPiece->m_fLife < pPiece->m_fSlowTime)
			{
				pPiece->m_vVelocity *= pPiece->m_fLife / pPiece->m_fSlowTime;
				pPiece->m_fLength *= 0.9f;
			}

			pPiece->m_vPos += pPiece->m_vVelocity * fElapsed;

			pPiece->m_fLife -= fElapsed;
			if (pPiece->m_fLife < 0.0f)
			{
				pPiece->m_bAlive = false;
				m_nCount--;
			}
		}
	}

	if (m_nCount == 0)
	{
		m_bActive = false;
	}
}

void CShotParticle::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nPieces; i++)
	{
		if (m_pPieces[i].m_bAlive)
		{
			WVector vDir = m_pPieces[i].m_vVelocity * -1.0f;
			vDir.Normalize();
			vDir *= m_pPieces[i].m_fLength;

			g_view->DrawLine(m_pPieces[i].m_vPos, 0xFFFFFFFF,
				m_pPieces[i].m_vPos + vDir, 0xFFFFFF, 0);
		}
	}
}

CPowerShot::CPowerShot(void)
{
	m_nSegments = 16;

	m_fSize = 2.0f;
	m_fWidth = 0.8f;

	for (int i = 0; i < 4; i++)
	{
		m_buffers[i].tu = (i & 2) ? 1.0f : 0.0f;
		m_buffers[i].tv = (i & 1) ? 1.0f : 0.0f;
	}
}

CPowerShot::~CPowerShot(void)
{
}

void CPowerShot::MakeParticle(void)
{
	m_fDistance = 0.0f;
	m_fTime = 0.0f;
	m_fLife = 0.8f;
	m_fSpeed = 20.0f;
	m_vCenter = GolfBall().m_pos;
	m_vCenter.y += 1.6f;
	m_bActive = true;
}

void CPowerShot::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fDistance += fElapsed * m_fSpeed;

	m_fTime += fElapsed;
	m_nAlpha = float2int((1.0f - m_fTime / m_fLife) * 255.0f);

	if (m_fTime > m_fLife)
	{
		m_bActive = false;
	}
}

void CPowerShot::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	float fStep = 6.2831855f / (float)m_nSegments;

	for (int k = 0; k < 4; k++)
	{
		m_buffers[k].diffuse = (m_nAlpha << 24) | 0xFFFFFF;
	}

	for (int i = 0; i < m_nSegments; i++)
	{
		float fAngle = (float)i * fStep;

		for (int j = 0; j < 4; j++)
		{
			float fSinSide = (j & 2) ? 1.0f : 0.0f;
			float fCosSide = (j & 2) ? 1.0f : 0.0f;
			WVector vPos;
			vPos = WVector(cosf(fAngle + fCosSide * fStep), 0.0f,
					   sinf(fAngle + fSinSide * fStep)) *
					((m_fSize - ((j & 1) ? 0.0f : 1.0f) * m_fWidth) *
						m_fDistance) +
				m_vCenter;
			m_buffers[j].SetPosition(vPos);
		}

		g_view->DrawPolygonFan(&m_pBuffer0, (m_texHandle & 0x7FF) | 0xC00000,
			0x400, false);
	}
}

CLightColumn::CLightColumn(void)
	: m_hScreenTex(0)
{
	for (int i = 0; i < 5; i++)
	{
		m_hEffectTex[i] = 0;
	}
}

CLightColumn::~CLightColumn(void)
{
	for (int i = 0; i < 5; i++)
	{
		if (g_resrcmng && m_hEffectTex[i])
		{
			g_resrcmng->Release(m_hEffectTex[i]);
			m_hEffectTex[i] = 0;
		}
	}

	if (g_resrcmng && m_hScreenTex)
	{
		g_resrcmng->Release(m_hScreenTex);
		m_hScreenTex = 0;
	}
}

void CLightColumn::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	WVector vHole = GetHoleCupPos();
	WVector vToCamera(g_camera.pivot.x - vHole.x, 0.0f,
		g_camera.pivot.z - vHole.z);

	m_fDistance = Max(GolfBall().GetRadius() * 10.0f,
		sqrtf(vToCamera.x * vToCamera.x + vToCamera.z * vToCamera.z) * 0.01f);

	if (IsLocalContent((localContentType_t)0x3A) &&
		(WSingleton<CSharedDoc>::Instance()->m_curChannel.Type & 0x800) &&
		WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType != 0xB &&
		WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType != 0xC)
	{
		m_fDistance =
			Max(GolfBall().GetRadius() * 20.0f, vToCamera.Magnitude() * 0.01f);
	}

	m_fFrame -= fElapsed * 2.0f;

	if (m_nFade == 1)
	{
		m_byAlpha =
			(unsigned char)(m_byAlpha + 20 < 255 ? m_byAlpha + 20 : 255);
		m_byTargetAlpha = (unsigned char)Min(255, m_byTargetAlpha + 20);

		if (m_byAlpha == 0xFF && m_byTargetAlpha == 0x50)
		{
			m_nFade = 0;
		}
	}
	else if (m_nFade == 2)
	{
		m_byAlpha = (unsigned char)(m_byAlpha - 20 > 0 ? m_byAlpha - 20 : 0);
		m_byTargetAlpha =
			(unsigned char)(m_byTargetAlpha - 20 > 0 ? m_byTargetAlpha - 20
													 : 0);

		if (m_byAlpha == 0 && m_byTargetAlpha == 0)
		{
			m_nFade = 0;
		}
	}
}

void CLightColumn::Display(void)
{
	if (!m_bActive || (GOLFDOC()->GameMode() & 0x200))
	{
		return;
	}

	const WVector& vHole = GetHoleCupPos();
	WVector vToCamera(g_camera.pivot.x - vHole.x, 0.0f,
		g_camera.pivot.z - vHole.z);

	WVector vSide = WCrossProduct(vToCamera, WVector::UNIT_POS_Y);
	vSide.Normalize();

	int nIndex;
	if (WSingleton<CSharedDoc>::Instance()->m_golfGame.gameType == 1)
	{
		nIndex = (unsigned char)TEAM(PLAYER(GOLFDOC()->m_currentPlayer)->team)
					 ->stroke[GOLFDOC()->m_currentHole - 1] -
			GOLFDOC()->GetHoleData().par + 1;
		if (nIndex < 0)
		{
			nIndex = 0;
		}
		else if (nIndex > 3)
		{
			nIndex = 3;
		}
	}
	else if (GOLFDOC()->m_bNoStat)
	{
		nIndex = 4;
	}
	else
	{
		nIndex = (unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
					 ->stroke[GOLFDOC()->m_currentHole - 1] -
			GOLFDOC()->GetHoleData().par + 1;
		if (nIndex < 0)
		{
			nIndex = 0;
		}
		else if (nIndex > 3)
		{
			nIndex = 3;
		}
	}

	WVector vBack = vToCamera;
	vBack = vBack.Normalize() * 0.5f;

	WVector vBase[2] = { GetHoleCupPos(), GetHoleCupPos() - vBack };
	WVector vWidth[2] = { m_fDistance * 0.5f * vSide,
		m_fDistance * 0.25f * vSide };
	float fTexV[2] = { 0.0f, m_fFrame };
	float fTexScale[2] = { 2.0f, 400.0f };
	unsigned char byAlpha[2] = { m_byTargetAlpha, m_byAlpha };
	unsigned long nTex[2] = { (m_hEffectTex[nIndex] & 0x7FF) | 0x20400000,
		(m_hScreenTex & 0x7FF) | 0x20C00000 };
	int nSegments[3] = { 10, 1, 1 };
	float fStart[3] = { 0.0f, 50.0f, m_fHeight * 0.6f };
	float fLength[3] = { 50.0f, m_fHeight * 0.6f - 50.0f, m_fHeight * 0.4f };

	m_buffers[0].tu = 0.0f;
	m_buffers[1].tu = 0.0f;
	m_buffers[2].tu = 1.0f;
	m_buffers[3].tu = 1.0f;

	for (int k = 0; k < 2; k++)
	{
		if (byAlpha[k])
		{
			for (int j = 0; j < 3; j++)
			{
				for (int s = 0; s < nSegments[j]; s++)
				{
					for (int i = 0; i < 4; i++)
					{
						float fY = (float)((i & 1) + s) / (float)nSegments[j] *
								fLength[j] +
							fStart[j];
						WVector vPoint = vBase[k];
						vPoint.y += fY;
						float fSide = (i & 2) ? 1.0f : -1.0f;

						vPoint += fSide * vWidth[k];
						m_buffers[i].SetPosition(vPoint);
						m_buffers[i].tv =
							(m_fHeight - fY) / m_fHeight * fTexScale[k] +
							fTexV[k];
						if (j <= 1)
						{
							m_buffers[i].diffuse =
								((int)byAlpha[k] << 24) | 0xFFFFFF;
						}
						else
						{
							float fRemain = m_fHeight - fY;
							m_buffers[i].diffuse =
								((unsigned char)(fRemain * (int)byAlpha[k] /
									 (m_fHeight * 0.4f))
									<< 24) |
								0xFFFFFF;
						}
					}

					g_view->DrawPolygonFan(&m_pBuffer0, nTex[k], 0, false);
				}
			}

			if (sqrtf(vToCamera.x * vToCamera.x + vToCamera.z * vToCamera.z) >
				320.0f)
			{
				break;
			}
		}
	}
}

void CLightColumn::MakeParticle(void)
{
	for (int i = 0; i < 5; i++)
	{
		m_hEffectTex[i] = g_resrcmng->LoadTexture(
			MakeStr("[hole_ef_nomal_%d.jpg", i + 10), 0, 0, NULL);
	}
	m_hScreenTex = g_resrcmng->LoadTexture("hole_scr01.jpg", 0, 0, NULL);

	m_fFrame = 0.0f;

	m_nFade = 0;
	m_fHeight = 1600.0f;
	m_byTargetAlpha = 0xFF;
	m_byAlpha = 0xFF;
	m_bActive = true;
}

CDust::CDust(int nCount)
	: m_pParticles(NULL)
{
	m_nParticles = nCount;
	m_pParticles = new sParticle[m_nParticles];
}

CDust::~CDust(void)
{
	SAFE_DELETE_ARRAY(m_pParticles);
}

void CDust::MakeParticle(void)
{
	SAFE_DELETE(m_pSpr);
	m_pSpr = new W3dSpr;
	m_pSpr->SetResourceManager(g_resrcmng);
	m_pSpr->AttachTexture(m_texHandle);

	float fSpeed = GolfBall().m_vel.Magnitude() * 0.5f;

	for (int i = 0; i < m_nParticles; i++)
	{
		m_pParticles[i].m_vPos = m_vCenter;
		m_pParticles[i].m_fAngle =
			((float)(rand() % 128) / 128.0f - 0.5f) * 1.5707964f;
		m_pParticles[i].m_fStartSize = 1.0f > fSpeed ? fSpeed : 1.0f;
		m_pParticles[i].m_fEndSize = 1.0f < fSpeed ? fSpeed : 1.0f;
	}

	m_fDuration = m_fRemain = 1.0f;
	m_bActive = true;
}

void CDust::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fRemain -= fElapsed / m_fDuration;
	if (!(m_fRemain < 0.0f))
	{
		return;
	}

	m_fRemain = 1.0f;
	m_bActive = false;
}

void CDust::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nParticles; i++)
	{
		int nAlpha = float2int(m_fRemain * 128.0f);
		float fSize =
			(m_pParticles[i].m_fEndSize - m_pParticles[i].m_fStartSize) *
				(1.0f - m_fRemain) +
			m_pParticles[i].m_fStartSize;

		m_pSpr->SetPos(m_pParticles[i].m_vPos);
		m_pSpr->SetRect(fSize, fSize, fSize * 0.5f, fSize * 0.5f);
		m_pSpr->SetColor((nAlpha << 24) + 0xFFFFFF);
		m_pSpr->Render(g_view, m_pParticles[i].m_fAngle * m_fRemain,
			0x20400000);
	}
}

CGrassParticle::CGrassParticle(int nCount, WVector const& vCenter)
{
	m_nGrass = nCount;
	m_vOrigin = vCenter;
	m_pGrass = new sGrass[m_nGrass];
}

CGrassParticle::~CGrassParticle(void)
{
	SAFE_DELETE_ARRAY(m_pGrass);
}

void CGrassParticle::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	bool bActive = false;
	for (int i = 0; i < m_nGrass; i++)
	{
		sGrass* pGrass = &m_pGrass[i];

		if (pGrass->m_bAlive)
		{
			pGrass->m_fAlpha -= fElapsed / pGrass->m_fFadeTime * 255.0f;
			pGrass->m_vVelocity.y -= fElapsed * 34.295296f;
			pGrass->m_vPos += pGrass->m_vVelocity * fElapsed;
			pGrass->m_fAngle += fElapsed * pGrass->m_fAngularSpeed;

			if (pGrass->m_fAlpha < 0.0f ||
				pGrass->m_vPos.y <
					GetPVS()
						.GetGroundPoint(pGrass->m_vPos, false, NULL, false)
						.y)
			{
				pGrass->m_bAlive = false;
			}
			else
			{
				bActive = true;
			}
		}
	}

	m_bActive = bActive;
}

void CGrassParticle::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nGrass; i++)
	{
		sGrass* pGrass = &m_pGrass[i];

		if (pGrass->m_bAlive)
		{
			int nColor = (float2int(pGrass->m_fAlpha) << 24) | 0xFFFFFF;

			m_pAniSpr->SetPos(pGrass->m_vPos);
			m_pAniSpr->SetColor(nColor);
			m_pAniSpr->Render(g_view, pGrass->m_fAngle, 0x20400000,
				pGrass->m_nFrame);
		}
	}
}

void CGrassParticle::MakeParticle(void)
{
	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 64.0f, 64.0f);

	m_pAniSpr->SetRect(0.5f, 0.5f, 0.25f, 0.25f);

	for (int i = 0; i < m_nGrass; i++)
	{
		sGrass* pGrass = &m_pGrass[i];

		pGrass->m_vPos = GolfBall().m_pos;
		pGrass->m_vPos.y -= GolfBall().m_radius;
		pGrass->m_bAlive = true;
		pGrass->m_fAngle = (float)(rand() % 1023) * 0.0061359233f;
		pGrass->m_fAngularSpeed =
			((float)(rand() % 1023) * 0.0009765625f - 0.5f) * 12.566371f;

		pGrass->m_vVelocity.x = (float)(rand() % 1025 - 512) *
				(m_vOrigin.x * 0.2f + 3.0f) * 0.001953125f +
			m_vOrigin.x;
		pGrass->m_vVelocity.y = (float)(rand() % 1025 - 512) *
				(m_vOrigin.y * 0.2f + 3.0f) * 0.001953125f +
			m_vOrigin.y;
		pGrass->m_vVelocity.z = (float)(rand() % 1025 - 512) *
				(m_vOrigin.z * 0.2f + 3.0f) * 0.001953125f +
			m_vOrigin.z;

		pGrass->m_nFrame = rand() % 4;
		pGrass->m_fFadeTime = 1.5f;
		pGrass->m_fAlpha = 255.0f;
	}

	m_bActive = true;
}

CBunkerShotParticle::CBunkerShotParticle(int nSmoke, int nParticle)
	: m_pSmokes(NULL),
	  m_pParticles(NULL),
	  m_nSmokes(nSmoke),
	  m_nParticles(nParticle),
	  m_dwColor(0xF8E9AB)
{
	if (nSmoke > 0)
	{
		m_pSmokes = new sSmoke[nSmoke];
	}

	if (nParticle > 0)
	{
		m_pParticles = new sParticle[nParticle];
	}
}

CBunkerShotParticle::~CBunkerShotParticle(void)
{
	SAFE_DELETE_ARRAY(m_pSmokes);
	SAFE_DELETE_ARRAY(m_pParticles);
	m_nSmokes = 0;
	m_nParticles = 0;
}

void CBunkerShotParticle::MakeParticle(void)
{
	SAFE_DELETE(m_pSpr);
	m_pSpr = new W3dSpr;
	m_pSpr->SetResourceManager(g_resrcmng);
	m_pSpr->AttachTexture(m_texHandle);

	Generate();

	m_fDuration = m_fRemain = 1.0f;
	g_audio->PlaySfx("\xba\xa1\xc4\xbf\xb8\xf0\xb7\xa1\xb3\xaf\xb8\xb2", NULL,
		0, NULL, NULL, 0.5f, 200.0f);

	m_bActive = true;
}

void CBunkerShotParticle::Generate(void)
{
	CGolfBall& ball = GolfBall();
	if (ball.m_renderNum < 3)
	{
		return;
	}

	const CGolfBall::sExpected* pExpected = &ball.m_expected[3];
	if (!pExpected)
	{
		return;
	}

	for (int i = 0; i < m_nSmokes; i++)
	{
		sSmoke* pSmoke = &m_pSmokes[i];

		pSmoke->m_vPos = ball.m_pos;
		pSmoke->m_vVelocity = pExpected->vel * 0.5f +
			WVector((float)(rand() % 11) - 5.0f, (float)(rand() % 11) - 5.0f,
				(float)(rand() % 11) - 5.0f) *
				2.5f;
	}

	for (int j = 0; j < m_nParticles; j++)
	{
		sParticle* pParticle = &m_pParticles[j];

		pParticle->m_bAlive = true;
		pParticle->m_vPos = ball.m_pos;
		pParticle->m_vVelocity = pExpected->vel * 0.6f +
			WVector((float)(rand() % 41) - 20.0f, (float)(rand() % 41) - 20.0f,
				(float)(rand() % 41) - 20.0f);
		pParticle->m_fLength = 0.3f;
		pParticle->m_fLife = (float)(rand() % 10) * 0.05f + 1.0f;
		pParticle->m_fMaxLife = pParticle->m_fLife * 0.5f;
	}
}

void CBunkerShotParticle::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fRemain -= fElapsed / m_fDuration;

	if (m_fRemain < 0.0f)
	{
		m_bActive = false;
		return;
	}

	for (int i = 0; i < m_nSmokes; i++)
	{
		sSmoke* pSmoke = &m_pSmokes[i];

		pSmoke->m_vVelocity.y -= fElapsed * 120.033536f;
		pSmoke->m_vPos += m_pSmokes[i].m_vVelocity * fElapsed;
	}

	for (int j = 0; j < m_nParticles; j++)
	{
		sParticle* pParticle = &m_pParticles[j];

		if (pParticle->m_bAlive)
		{
			pParticle->m_vVelocity.x *= 0.99f;
			pParticle->m_vVelocity.y -= fElapsed * 102.885888f;
			pParticle->m_vVelocity.z *= 0.99f;
			pParticle->m_vPos += pParticle->m_vVelocity * fElapsed;
			pParticle->m_fLife -= fElapsed;

			if (pParticle->m_fLife < 0.0f)
			{
				pParticle->m_bAlive = false;
				m_nParticles--;
			}
		}
	}
}

void CBunkerShotParticle::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nSmokes; i++)
	{
		sSmoke* pSmoke = &m_pSmokes[i];
		float fSize = (1.0f - m_fRemain) * 40.0f;

		m_pSpr->SetRect(fSize, fSize, fSize * 0.5f, 0.0f);
		m_pSpr->SetColor(((int)(m_fRemain * 255.0f) << 24) | m_dwColor);
		m_pSpr->SetPos(pSmoke->m_vPos);
		m_pSpr->Render(g_view, 0x20400000, W3dSpr::CAMERA_X_ALIGN);
	}

	for (int j = 0; j < m_nParticles; j++)
	{
		sParticle* pParticle = &m_pParticles[j];

		if (pParticle->m_bAlive)
		{
			WVector vDir = pParticle->m_vVelocity * -1.0f;
			vDir = pParticle->m_fLength * vDir.Normalize();

			if (pParticle->m_fLength > 0.1f)
			{
				pParticle->m_fLength *= 0.95f;
			}

			if (pParticle->m_fLife < pParticle->m_fMaxLife)
			{
				vDir *= pParticle->m_fLife / pParticle->m_fMaxLife;
			}

			g_view->DrawLine(pParticle->m_vPos, 0xFFA08000,
				pParticle->m_vPos + vDir, 0xA08000, 0);
		}
	}
}

CBunkerParticle::CBunkerParticle(void)
{
}

CBunkerParticle::~CBunkerParticle(void)
{
}

void CBunkerParticle::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fRemain -= fElapsed / m_fDuration;

	if (m_fRemain < 0.0f)
	{
		m_bActive = false;
	}
}

void CBunkerParticle::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	m_fSize = (1.0f - m_fRemain) * 20.0f;
	m_pSpr->SetRect(m_fSize, m_fSize, m_fSize * 0.5f, 0.0f);

	m_nAlpha = float2int(m_fRemain * 255.0f);
	m_pSpr->SetColor((m_nAlpha << 24) | 0xFFFFFF);

	m_pSpr->Render(g_view, 0x20400000, W3dSpr::CAMERA_X_ALIGN);
}

void CBunkerParticle::MakeParticle(void)
{
	SAFE_DELETE(m_pSpr);
	m_pSpr = new W3dSpr;
	m_pSpr->SetResourceManager(g_resrcmng);
	m_pSpr->AttachTexture(m_texHandle);

	m_fSize = 0.0f;
	m_fRemain = 1.0f;
	m_fDuration = 0.7f;
	m_pSpr->SetPos(GolfBall().m_pos);

	g_audio->PlaySfx("\xba\xa1\xc4\xbf\xb8\xf0\xb7\xa1\xb3\xaf\xb8\xb2", NULL,
		0, NULL, NULL, 0.5f, 200.0f);

	m_bActive = true;
}

CFragment::CFragment(void)
	: m_pFrags(NULL), m_nFrags(30)
{
	m_pFrags = new sFrag[m_nFrags];
}

CFragment::~CFragment(void)
{
	SAFE_DELETE_ARRAY(m_pFrags);
}

void CFragment::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fAlpha -= fElapsed * 96.0f;

	for (int i = 0; i < m_nFrags; i++)
	{
		m_pFrags[i].m_vVelocity.y -= fElapsed * 17.147648f;
		m_pFrags[i].m_vPos += m_pFrags[i].m_vVelocity * fElapsed;
	}

	if (m_fAlpha < 0.0f)
	{
		m_bActive = false;
	}
}

void CFragment::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	m_pAniSpr->SetColor((float2int(m_fAlpha) << 24) | 0xFFFFFF);

	for (int i = 0; i < m_nFrags; i++)
	{
		m_pAniSpr->SetPos(m_pFrags[i].m_vPos);
		m_pAniSpr->Render(g_view, 0.0f, 0xC00000, 7);
	}
}

void CFragment::MakeParticle(void)
{
	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 32.0f, 32.0f);

	WMatrix matAxis;
	matAxis.Reset();

	if (!WisZero(m_vNormal.y - 1.0f))
	{
		matAxis.ya = m_vNormal;
		matAxis.xa = WCrossProduct(matAxis.ya, WVector::UNIT_POS_Y);
		matAxis.xa.Normalize();

		matAxis.za = WCrossProduct(matAxis.xa, matAxis.ya);
		matAxis.za.Normalize();

		matAxis.pivot.Reset();
	}

	for (int i = 0; i < m_nFrags; i++)
	{
		float fRatio = (float)(rand() % 1023) * 0.0009765625f;
		m_pFrags[i].m_vPos = m_vCenter;
		m_pFrags[i].m_vVelocity = (float)(rand() % 1023) * 0.0009765625f *
			WVector::UNIT_POS_Z *
			RotMat((float)(rand() % 1023) * 0.0061359233f, 1);
		m_pFrags[i].m_vVelocity.y =
			(((1.0f - fRatio) * 2.0f) + (fRatio * 5.0f));
		m_pFrags[i].m_vVelocity = m_pFrags[i].m_vVelocity *
			Max(2.0f, GolfBall().m_vel.Magnitude() * 0.1f) * matAxis;
	}

	m_fAlpha = 255.0f;
	m_bActive = true;

	float fSize = Between(0.2f, GolfBall().m_vel.Magnitude() * 0.01f, 0.5f);
	m_pAniSpr->SetRect(fSize, fSize, fSize * 0.5f, fSize * 0.5f);
}

CDroplet::CDroplet(void)
{
	m_nDroplets = 100;
	m_pDroplets = new sDroplet[m_nDroplets];
	m_pSpr = g_resrcmng->Get3DSpr("w-drop.dds", 0);

	m_pSpr->SetRect(0.2f, 0.2f, 0.1f, 0.1f);
}

CDroplet::~CDroplet(void)
{
	if (m_pSpr)
	{
		g_resrcmng->Release(m_pSpr);
		m_pSpr = NULL;
	}

	SAFE_DELETE_ARRAY(m_pDroplets);
}

void CDroplet::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fAlpha -= fElapsed / m_fDuration * 255.0f;
	if (m_fAlpha < 0.0f)
	{
		m_bActive = false;
		return;
	}

	if (m_fDuration < 0.0f)
	{
		m_bActive = false;
		return;
	}

	for (int i = 0; i < m_nDroplets; i++)
	{
		m_pDroplets[i].m_vVelocity.y -= fElapsed * 34.295296f;
		m_pDroplets[i].m_vPos += m_pDroplets[i].m_vVelocity * fElapsed;
	}
}

void CDroplet::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	int nColor = (float2int(m_fAlpha) << 24) | 0xFFFFFF;

	for (int i = 0; i < m_nDroplets; i++)
	{
		m_pSpr->SetPos(m_pDroplets[i].m_vPos);
		m_pSpr->SetColor(nColor);
		m_pSpr->Render(g_view, 0.0f, 0x20400000);
	}
}

void CDroplet::MakeParticle(void)
{
	for (int i = 0; i < m_nDroplets; i++)
	{
		m_pDroplets[i].m_vPos = GolfBall().m_pos;
		m_pDroplets[i].m_vVelocity = (float)(rand() % 1023) * 0.005859375f *
			WVector::UNIT_POS_Z *
			RotMat((float)(rand() % 1023) * 0.0061359233f, 1);
		float t = (float)(rand() % 1023) * 0.0009765625f;
		m_pDroplets[i].m_vVelocity.y = (1.0f - t) * 10.0f + t * 40.0f;
	}

	m_fAlpha = 255.0f;
	m_fDuration = 1.5f;
	m_bActive = true;
}

CRipple::CRipple(void)
	: m_pRipples(NULL), m_nRipples(0)
{
	m_pRipples = new sRipple[5];

	AddRipple(15.0f, 0.01f, 255.0f, 255.0f);
	AddRipple(17.0f, 0.2f, 255.0f, 255.0f);
	AddRipple(19.0f, 0.5f, 128.0f, 128.0f);

	m_pSubParticle = new CDroplet;

	Init();
}

CRipple::~CRipple(void)
{
	SAFE_DELETE_ARRAY(m_pRipples);
	SAFE_DELETE(m_pSubParticle);
}

void CRipple::Init(void)
{
	for (int i = 0; i < m_nRipples; i++)
	{
		m_pRipples[i].m_bAlive = false;
		m_pRipples[i].m_fRadius = 0.0f;
		m_pRipples[i].m_fPrevX = m_pRipples[i].m_fX;
		m_pRipples[i].m_fWait = 0.0f;
	}

	m_nCount = m_nRipples;
}

void CRipple::MakeParticle(void)
{
	Init();

	for (int i = 0; i < m_nRipples; i++)
	{
		m_pRipples[i].m_fWait = m_pRipples[i].m_fDelay;
	}

	m_bActive = true;
	m_pSubParticle->SetCenter(m_vCenter);
	m_pSubParticle->MakeParticle();
}

void CRipple::AddRipple(float fSize, float fDelay, float fX, float fZ)
{
	m_pRipples[m_nRipples].m_fSize = fSize;
	m_pRipples[m_nRipples].m_fDelay = fDelay;
	m_pRipples[m_nRipples].m_fX = fX;
	m_pRipples[m_nRipples].m_fZ = fZ;
	m_nRipples++;
}

void CRipple::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nRipples; i++)
	{
		if (m_pRipples[i].m_fWait > 0.0f)
		{
			m_pRipples[i].m_fWait -= fElapsed;
			if (m_pRipples[i].m_fWait < 0.0f)
			{
				m_pRipples[i].m_bAlive = true;
			}
		}

		if (m_pRipples[i].m_bAlive)
		{
			m_pRipples[i].m_fRadius += fElapsed * m_pRipples[i].m_fSize;
			m_pRipples[i].m_fPrevX -= fElapsed * m_pRipples[i].m_fZ;

			if (m_pRipples[i].m_fPrevX < 0.0f)
			{
				m_pRipples[i].m_bAlive = false;
				m_nCount--;

				if (m_nCount == 0)
				{
					m_bActive = false;
				}
			}
		}
	}

	m_pSubParticle->Process(fElapsed);
}

void CRipple::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nRipples; i++)
	{
		if (m_pRipples[i].m_bAlive)
		{
			for (int j = 0; j < 4; j++)
			{
				WVector vPos = m_vCenter;
				float fX = vPos.x +
					((j & 2) ? 1.0f : -1.0f) * m_pRipples[i].m_fRadius * 0.5f;
				float fZ = vPos.z +
					((j & 1) ? 1.0f : -1.0f) * m_pRipples[i].m_fRadius * 0.5f;
				m_buffers[j].x = fX;
				m_buffers[j].z = fZ;
				m_buffers[j].y = vPos.y;
				m_buffers[j].tu = (j & 2) ? 1.0f : 0.0f;
				m_buffers[j].tv = (j & 1) ? 0.0f : 1.0f;
				m_buffers[j].diffuse =
					(float2int(m_pRipples[i].m_fPrevX) << 24) | 0xFFFFFF;
			}

			g_view->DrawPolygonFan(&m_pBuffer0,
				(m_texHandle & 0x7FF) | 0x20800000, 0x400, false);
		}
	}

	m_pSubParticle->Display();
}

CCoin::CCoin(int nCount, WVector const& vCenter)
{
	m_vOrigin = vCenter;
	m_nCoins = nCount;
	m_pCoins = new sCoin[nCount];
	m_nSparks = 50;
	m_pSparks = new sSpark[50];
}

CCoin::~CCoin(void)
{
	for (int i = 0; i < m_nCoins; i++)
	{
		if (m_pCoins[i].m_pBody)
		{
			delete m_pCoins[i].m_pBody;
			m_pCoins[i].m_pBody = NULL;
		}
	}
	SAFE_DELETE_ARRAY(m_pCoins);
	SAFE_DELETE_ARRAY(m_pSparks);
	m_nSparks = m_nCoins = 0;
}

void CCoin::MakeParticle(void)
{
	m_fRemain = 1.0f;
	m_fDuration = 3.0f;

	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 32.0f, 32.0f);
	m_pAniSpr->SetRect(0.3f, 0.3f, 0.15f, 0.15f);

	m_nDropSounds = 0;
	Generate();

	SetActive(true);
}

void CCoin::Generate(void)
{
	if (m_nCoins > 0)
	{
		sCoin* pCoin = &m_pCoins[0];

		if (!pCoin->m_pBody)
		{
			if (IsLocalContent((localContentType_t)0x17))
			{
				pCoin->m_pBody =
					new CPetBody("*pet coin2.pet *shadow 0", false);
			}
			else
			{
				pCoin->m_pBody = new CPetBody("*pet coin.pet *shadow 0", false);
			}
			pCoin->m_pBody->GetPet()->CalcBound(NULL, true);
		}

		pCoin->m_pBody->SetPos(m_vOrigin);

		pCoin->m_fSpin = (float)(rand() % 32) * 0.19634955f;
		pCoin->m_fBounceSpeed = (float)(rand() % 5 - 2) * 0.1f + 1.5f;
		pCoin->m_vVelocity = WVector(0.3f, pCoin->m_fBounceSpeed, 0.0f) *
			RotMat(6.2831855f / (float)(rand() % 32 + 1), 1);
	}

	if (m_nCoins > 1)
	{
		for (int i = 1; i < m_nCoins; i++)
		{
			sCoin* pCoin = &m_pCoins[i];

			if (!pCoin->m_pBody)
			{
				if (IsLocalContent((localContentType_t)0x17))
				{
					pCoin->m_pBody =
						new CPetBody("*pet coin2.pet *shadow 0", false);
				}
				else
				{
					pCoin->m_pBody =
						new CPetBody("*pet coin.pet *shadow 0", false);
				}
				pCoin->m_pBody->GetPet()->CalcBound(NULL, true);
			}

			pCoin->m_pBody->SetPos(m_vOrigin);

			pCoin->m_fSpin = (float)(rand() % 32) * 0.19634955f;
			pCoin->m_fBounceSpeed = (float)(rand() % 5 - 2) * 0.1f + 1.3f;
			pCoin->m_vVelocity =
				WVector((float)(rand() % 51 - 25) * 0.005f + 0.25f,
					pCoin->m_fBounceSpeed, 0.0f) *
				RotMat((float)i * 6.2831855f / (m_nCoins - 1), 1);
		}
	}

	for (int j = 0; j < m_nSparks; j++)
	{
		sSpark* pSpark = &m_pSparks[j];

		pSpark->m_vPos = m_vOrigin;
		pSpark->m_vVelocity =
			WVector((float)(rand() % 5 - 2) * 0.02f + 1.3f,
				(float)(rand() % 21 - 10) * 0.05f + 2.0f, 0.0f) *
			RotMat((float)j * 6.2831855f / (m_nSparks - 1), 1);
		pSpark->m_fAlpha = 255.0f;
		pSpark->m_fLife = (float)(rand() % 10) * 0.05f + 0.5f;
		pSpark->m_fSlowTime = pSpark->m_fLife * 0.8f;
		pSpark->m_bAlive = true;
	}
}

bool CCoin::ShouldDisplay(void)
{
	return m_bActive;
}

void CCoin::SetActive(bool bActive)
{
	m_bActive = bActive;
}

void CCoin::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	m_fRemain -= fElapsed / m_fDuration;

	if (m_fRemain < 0.0f)
	{
		m_fRemain = 1.0f;
		SetActive(false);
	}

	for (int i = 0; i < m_nCoins; i++)
	{
		sCoin* pCoin = &m_pCoins[i];
		int nAlpha = (int)(m_fRemain * 1275.0f);
		unsigned char alpha = (unsigned char)nAlpha;
		if (nAlpha >= 255)
		{
			alpha = 255;
		}
		pCoin->m_pBody->SetAlpha(alpha);
		pCoin->m_pBody->SetLight();
		pCoin->m_pBody->SetPos(
			pCoin->m_pBody->GetPos() + pCoin->m_vVelocity * 15.0f * fElapsed);
		pCoin->m_pBody->SetRotate(WVector(0.0f, pCoin->m_fSpin, 0.0f));

		pCoin->m_fSpin += fElapsed * 15.0f;

		float fFriction = fElapsed * 0.1f;

		if (pCoin->m_vVelocity.x > fFriction)
		{
			pCoin->m_vVelocity.x -= fFriction;
		}
		else if (pCoin->m_vVelocity.x < -fFriction)
		{
			pCoin->m_vVelocity.x += fFriction;
		}

		if (pCoin->m_vVelocity.z > fFriction)
		{
			pCoin->m_vVelocity.z -= fFriction;
		}
		else if (pCoin->m_vVelocity.z < -fFriction)
		{
			pCoin->m_vVelocity.z += fFriction;
		}

		pCoin->m_vVelocity.y -= fElapsed * 3.0f;

		WVector vPos = pCoin->m_pBody->GetPos();
		float fGroundY = GetPVS().GetGroundPoint(vPos, true, NULL, true).y;
		if (vPos.y < fGroundY)
		{
			if (m_nDropSounds < 4)
			{
				g_audio->PlaySfx("pang_coin_drop", NULL, 0, NULL, NULL, 0.5f,
					200.0f);
				m_nDropSounds++;
			}
			pCoin->m_fBounceSpeed *= 0.5f;
			vPos.y = fGroundY;
			pCoin->m_vVelocity.y = pCoin->m_fBounceSpeed;

			pCoin->m_pBody->SetPos(vPos);
		}

		pCoin->m_pBody->Process(fElapsed);
	}

	for (int j = 0; j < m_nSparks; j++)
	{
		sSpark* pSpark = &m_pSparks[j];

		if (pSpark->m_bAlive)
		{
			if (pSpark->m_fLife < pSpark->m_fSlowTime)
			{
				pSpark->m_vVelocity *=
					pSpark->m_fLife / pSpark->m_fSlowTime * 0.8f;
				pSpark->m_fAlpha =
					pSpark->m_fAlpha * (pSpark->m_fLife / pSpark->m_fSlowTime);
			}

			pSpark->m_vPos += pSpark->m_vVelocity * 7.0f * fElapsed;

			pSpark->m_fLife -= fElapsed;
			if (pSpark->m_fLife < 0.0f)
			{
				pSpark->m_bAlive = false;
			}
		}
	}
}

void CCoin::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < m_nCoins; i++)
	{
		m_pCoins[i].m_pBody->Display();
	}

	for (int j = 0; j < m_nSparks; j++)
	{
		sSpark* pSpark = &m_pSparks[j];

		m_pAniSpr->SetColor((float2int(pSpark->m_fAlpha) << 24) | 0xFFFFFF);
		m_pAniSpr->SetPos(pSpark->m_vPos);
		m_pAniSpr->Render(g_view, 0.0f, 0x20C00000, 5);
	}
}

void CCoin::DisplayShadow(void)
{
	if (!m_bActive)
	{
		return;
	}

	char szName[32];

	sprintf(szName, "coin%x", this);

	CShadowManager::Instance()->RegisterCaster_New(szName, 16, "sun",
		g_lightset);

	for (int i = 0; i < m_nCoins; i++)
	{
		m_pCoins[i].m_pBody->DisplayShadow(szName);
	}
}

CEnergyCenter::CEnergyCenter(int nCount, WVector const& vCenter, float fRadius)
{
	m_fRadius = fRadius;
	m_nSparks = nCount;
	m_vOrigin = vCenter;

	m_fLife = 1.0f;
	m_fTime = 0.0f;
	m_fAlpha = 255.0f;
	m_fMaxAlpha = 255.0f;
	m_pGolfBg = AfxGetTask()->GetActor("GolfBg");
	m_pSparks = new sSpark[m_nSparks];
}

CEnergyCenter::~CEnergyCenter(void)
{
	SAFE_DELETE_ARRAY(m_pSparks);
	m_nSparks = 0;
}

void CEnergyCenter::MakeParticle(void)
{
	m_nGenerated = 0;
	m_bActive = true;

	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 32.0f, 32.0f);
}

void CEnergyCenter::Generate(float fElapsed)
{
	for (int i = 0; m_nGenerated < m_nSparks && i < fElapsed * 35.0f; i++)
	{
		sSpark* pSpark = &m_pSparks[m_nGenerated];

		pSpark->m_vPos = WVector((float)(rand() % 31 - 15) * 0.05f + 8.0f,
							 (float)(rand() % 31 - 15) * 0.05f + 8.0f, 0.0f) *
				RotMat((float)m_nSparks * 6.2831855f /
						(float)(rand() % m_nSparks + 1),
					1) +
			m_vOrigin;
		pSpark->m_vDir = m_vOrigin - pSpark->m_vPos;

		const WVector& vDir = pSpark->m_vDir.Normalize();
		pSpark->m_vDir = WVector(vDir.x, vDir.y, vDir.z);

		pSpark->m_fAlpha = 0.0f;
		pSpark->m_bAlive = true;
		m_nGenerated++;
	}
}

void CEnergyCenter::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	Generate(fElapsed);

	bool bAllArrived = true;

	for (int i = 0; i < m_nGenerated; i++)
	{
		sSpark* pSpark = &m_pSparks[i];

		if (pSpark->m_bAlive)
		{
			float fAlpha = fElapsed * 1000.0f + pSpark->m_fAlpha;
			if (fAlpha < 255.0f)
			{
				pSpark->m_fAlpha = fAlpha;
			}
			else
			{
				pSpark->m_fAlpha = 255.0f;
			}

			pSpark->m_vPos += pSpark->m_vDir * 20.0f * fElapsed;

			WVector vRel = pSpark->m_vPos - m_vOrigin;
			pSpark->m_vPos = vRel * RotMat(m_fRadius, 1) + m_vOrigin;

			if (pSpark->m_vPos.y * pSpark->m_vDir.y >=
				m_vOrigin.y * pSpark->m_vDir.y)
			{
				m_fLife += 255.0f / (float)m_nSparks;
				pSpark->m_bAlive = false;
			}
			else
			{
				bAllArrived = false;
			}

			pSpark->m_vDir = m_vOrigin - pSpark->m_vPos;
			pSpark->m_vDir = pSpark->m_vDir.Normalize();
		}
	}

	m_fTime -= fElapsed * 1.5707964f;

	if (bAllArrived)
	{
		m_fLife -= fElapsed * 400.0f;
		if (m_fLife < 0.0f)
		{
			m_bActive = false;
		}
	}
	else
	{
		float fAlpha = m_fAlpha - fElapsed * 250.0f;
		if (fAlpha > 160.0f)
		{
			m_fAlpha = fAlpha;
		}
		else
		{
			m_fAlpha = 160.0f;
		}
	}
}

void CEnergyCenter::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	if (GOLFDOC()->GetDay() && COption::Instance()->vGetParticleLevel() < 1)
	{
		if (!WisEqual(m_fAlpha, m_fMaxAlpha, 0.00001f))
		{
			m_fMaxAlpha = m_fAlpha;

			unsigned char byAlpha = (unsigned char)float2int(m_fAlpha);
			unsigned long dwColor =
				(((0xFFFFFF00 | byAlpha) << 8 | byAlpha) << 8) | byAlpha;

			GetPVS().SetBaseVtxColor(dwColor);
			GetPVS().SetObjVtxColor(dwColor);

			if (m_pGolfBg)
			{
				MsgObject msg;
				msg.sender = NULL;
				msg.time = 0;
				msg.message = 0x13F;
				msg.param1 = dwColor;
				m_pGolfBg << msg;
			}
		}
	}

	m_pAniSpr->SetColor(((int)m_fLife << 24) | 0xFFFFFF);
	m_pAniSpr->SetPos(m_vOrigin);
	m_pAniSpr->SetRect(1.0f, 1.0f, 0.5f, 0.5f);
	m_pAniSpr->Render(g_view, m_fTime, 0xF00000, 4);

	for (int i = 0; i < m_nGenerated; i++)
	{
		sSpark* pSpark = &m_pSparks[i];

		if (pSpark->m_bAlive)
		{
			m_pAniSpr->SetColor(((int)pSpark->m_fAlpha << 24) | 0xFFFFFF);
			m_pAniSpr->SetPos(pSpark->m_vPos);
			m_pAniSpr->SetRect(0.5f, 0.5f, 0.25f, 0.25f);
			m_pAniSpr->Render(g_view, 0.0f, 0x20C00000, 6);
		}
	}
}

CStarDust::CStarDust(void)
{
	m_fRate = 0.01f;
	m_nParticles = 400;
	m_pParticles = new sParticle[m_nParticles];
}

CStarDust::~CStarDust(void)
{
	SAFE_DELETE_ARRAY(m_pParticles);
	m_nParticles = 0;
}

void CStarDust::MakeParticle(void)
{
	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 32.0f, 32.0f);

	m_nGenerated = 0;
	m_fAlpha = 255.0f;
	m_fGenTime = 0.0f;
	m_fFade = 0.0f;
	m_bActive = true;
	for (int i = 0; i < m_nParticles; i++)
	{
		m_pParticles[i].m_bAlive = false;
	}
}

void CStarDust::Generate(void)
{
	WVector vSide = WCrossProduct(GolfBall().m_vel, g_camera.za);
	vSide = vSide.Normalize() * 2.0f * GolfBall().GetRadius();

	sParticle* p = &m_pParticles[m_nGenerated];

	p->m_bAlive = true;
	p->m_fRadius = GolfBall().m_radius;
	p->m_fAngularSpeed = (float)(rand() % 1023) * 0.0015339808f + 1.5707964f;
	p->m_vAxisX = WVector((float)(rand() % 1024 - 512) * 0.001953125f,
					  (float)(rand() % 1024 - 512) * 0.001953125f,
					  (float)(rand() % 1024 - 512) * 0.001953125f) *
			2.0f +
		g_camera.za;
	p->m_vAxisY = vSide / vSide.Magnitude() +
		WVector((float)(rand() % 1024 - 512) * 0.001953125f,
			(float)(rand() % 1024 - 512) * 0.001953125f,
			(float)(rand() % 1024 - 512) * 0.001953125f) *
			2.0f;
	p->m_vOrigin = GolfBall().m_pos;
	p->m_fAngle = (float)(rand() % 1023) * 0.0061359233f;
	p->m_fLife = 1.0f;
}

void CStarDust::BlowAway(float fPower, int nIndex)
{
	sParticle* p = &m_pParticles[nIndex];

	if (!p->m_bAlive)
	{
		return;
	}

	float fSpeed = fPower * 2.0f;

	p->m_fAngularSpeed -= fSpeed;
	p->m_fAngle -= p->m_fAngularSpeed * fPower;
	p->m_vPos =
		(p->m_vAxisX * cosf(p->m_fAngle) + p->m_vAxisY * sinf(p->m_fAngle)) *
			p->m_fRadius +
		p->m_vOrigin;
	p->m_fRadius += fSpeed;
	p->m_fLife -= fPower * 0.3f;

	if (p->m_fLife < 0.0f)
	{
		p->m_bAlive = false;
	}
}

void CStarDust::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	WPuppet* pPuppet;
	int nCount;

	if (GolfBall().m_vel.y < -1.0f)
	{
		m_fGenTime += Min(fElapsed, fElapsed * m_fAlpha * 0.005f);
		m_fAlpha = Max(0.0f, m_fAlpha - fElapsed * 127.0f);

		pPuppet = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (pPuppet && pPuppet->GetRootBone())
		{
			pPuppet->SetAlpha(255 - (unsigned char)(int)m_fAlpha);
		}

		if (Wabs(m_fAlpha) < g_EPSILON)
		{
			m_bActive = false;
			m_fAlpha = 0.0f;
			if (pPuppet && pPuppet->GetRootBone())
			{
				pPuppet->SetAlpha(255);
			}
		}
	}
	else
	{
		m_fGenTime += fElapsed;
	}

	for (int i = 0; i < m_nParticles; i++)
	{
		BlowAway(fElapsed, i);
	}

	if (m_fGenTime > m_fRate)
	{
		if (fElapsed > m_fRate)
		{
			nCount = (int)(m_fGenTime / m_fRate);
		}
		else
		{
			nCount = 1;
		}

		m_fGenTime -= nCount * m_fRate;

		while (nCount > 0)
		{
			for (int k = 2; k != 0; k--)
			{
				Generate();

				if (fElapsed > m_fRate)
				{
					BlowAway(nCount * m_fRate + m_fGenTime, m_nGenerated);
				}

				m_nGenerated = (m_nGenerated + 1) % m_nParticles;
			}

			nCount--;
		}
	}

	m_fFade -= fElapsed * (g_PI * 6.0f);
}

void CStarDust::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	int nColor = ((int)m_fAlpha << 24) | 0xFFFFFF;

	m_pAniSpr->SetRect(1.0f, 1.0f, 0.5f, 0.5f);
	m_pAniSpr->SetColor(nColor);
	m_pAniSpr->SetPos(GolfBall().m_pos);
	m_pAniSpr->Render(g_view, m_fFade, 0x20C00000, 4);

	for (int i = 0; i < m_nParticles; i++)
	{
		sParticle* p = &m_pParticles[i];

		if (p->m_bAlive)
		{
			float fSize = p->m_fLife * 0.6f;

			m_pAniSpr->SetPos(p->m_vPos);
			m_pAniSpr->SetRect(fSize, fSize, fSize * 0.5f, fSize * 0.5f);
			m_pAniSpr->SetColor(nColor);
			m_pAniSpr->Render(g_view, 0.0f, 0x20C00000, 5);
		}
	}
}

CStarDust2::CStarDust2(void)
{
	m_fRate = 0.1f;
	m_nParticles = 60;
	m_pParticles = new sParticle[m_nParticles];
}

CStarDust2::~CStarDust2(void)
{
	SAFE_DELETE_ARRAY(m_pParticles);
	m_nParticles = 0;
}

void CStarDust2::MakeParticle(void)
{
	SAFE_DELETE(m_pAniSpr);
	m_pAniSpr = new W3dAniSpr;
	m_pAniSpr->SetResourceManager(g_resrcmng);
	m_pAniSpr->LoadTexture(m_texHandle, 32.0f, 32.0f);

	m_nGenerated = 0;
	m_fAlpha = 255.0f;
	m_fGenTime = 0.0f;
	m_fFade = 0.0f;
	m_bActive = true;
	for (int i = 0; i < m_nParticles; i++)
	{
		m_pParticles[i].m_bAlive = false;
	}
}

void CStarDust2::Generate(void)
{
	WVector vSide = WCrossProduct(GolfBall().m_vel, g_camera.za);
	vSide = vSide.Normalize() * 2.0f * GolfBall().GetRadius();

	for (int i = 0; i < 4; i++)
	{
		sParticle* p = &m_pParticles[m_nGenerated];
		m_nGenerated = (m_nGenerated + 1) % m_nParticles;

		p->m_bAlive = true;
		p->m_fRadius = GolfBall().m_radius * 4.0f;
		p->m_fAngularSpeed =
			(float)(rand() % 1023) * 0.0015339808f + 1.5707964f;
		p->m_vAxisX = WVector((float)(rand() % 1024 - 512) * 0.001953125f,
						  (float)(rand() % 1024 - 512) * 0.001953125f,
						  (float)(rand() % 1024 - 512) * 0.001953125f) *
				2.0f +
			g_camera.za;
		p->m_vAxisY = vSide / vSide.Magnitude() +
			WVector((float)(rand() % 1024 - 512) * 0.001953125f,
				(float)(rand() % 1024 - 512) * 0.001953125f,
				(float)(rand() % 1024 - 512) * 0.001953125f) *
				2.0f;
		p->m_vOrigin = GolfBall().m_pos;
		p->m_fAngle = (float)(rand() % 1023) * 0.0061359233f;
		p->m_fLife = 255.0f;
	}
}

void CStarDust2::Process(float fElapsed)
{
	if (!m_bActive)
	{
		return;
	}

	if (GolfBall().m_vel.y < 0.0f)
	{
		m_fGenTime += Min(fElapsed, fElapsed * m_fAlpha * 0.005f);
		m_fAlpha = Max(0.0f, m_fAlpha - fElapsed * 150.0f);

		CSceneManager::Instance()
			->GetPuppet("\xb0\xf8")
			->SetAlpha(255 - (unsigned char)float2int(m_fAlpha));

		if (Wabs(m_fAlpha) < g_EPSILON)
		{
			m_bActive = false;
			m_fAlpha = 0.0f;
			CSceneManager::Instance()->GetPuppet("\xb0\xf8")->SetAlpha(255);
		}
	}
	else
	{
		m_fGenTime += fElapsed;
	}

	if (m_fGenTime > m_fRate)
	{
		m_fGenTime -= m_fRate;
		Generate();
	}

	m_fFade -= fElapsed * 18.849556f;

	for (int i = 0; i < m_nParticles; i++)
	{
		sParticle* p = &m_pParticles[i];

		if (p->m_bAlive)
		{
			float fSpeed = fElapsed * 2.0f;

			p->m_fAngularSpeed -= fSpeed;
			p->m_fAngle -= p->m_fAngularSpeed * fElapsed;
			p->m_vPos = (p->m_vAxisX * cosf(p->m_fAngle) +
							p->m_vAxisY * sinf(p->m_fAngle)) *
					p->m_fRadius +
				p->m_vOrigin;
			p->m_fRadius += fSpeed;
			float fLife = p->m_fLife - fElapsed * 100.0f;
			if (fLife > 0.0f)
			{
				p->m_fLife = fLife;
			}
			else
			{
				p->m_fLife = 0.0f;
			}

			if (GolfBall().m_radius * 0.5f > p->m_fRadius)
			{
				p->m_bAlive = false;
			}
		}
	}
}

void CStarDust2::Display(void)
{
	if (!m_bActive)
	{
		return;
	}

	int nColor = ((int)m_fAlpha << 24) | 0xFFFFFF;

	m_pAniSpr->SetColor(nColor);
	m_pAniSpr->SetPos(GolfBall().m_pos);
	m_pAniSpr->SetRect(2.0f, 2.0f, 1.0f, 1.0f);
	m_pAniSpr->Render(g_view, m_fFade, 0x20F00000, 5);

	for (int i = 0; i < m_nParticles; i++)
	{
		sParticle* p = &m_pParticles[i];

		if (p->m_bAlive)
		{
			m_pAniSpr->SetPos(p->m_vPos);
			m_pAniSpr->SetRect(0.5f, 0.5f, 0.25f, 0.25f);
			m_pAniSpr->SetColor((((int)p->m_fLife << 24) | 0xFFFFFF) & nColor);
			m_pAniSpr->Render(g_view, 0.0f, 0xF00000, 6);
		}
	}
}
