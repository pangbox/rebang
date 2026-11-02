#include "stdafx.h"
#include "mathconsts.h"
#include <math.h>
#include "particle.h"
#include "w3danispr.h"
#include "wresrcmng.h"
#include "objectfactory.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "particleparty.h"
#include "golfball.h"
#include "quadtree.h"
#include "shareddoc.h"
#include "fx.h"
#include "fxspray.h"
#include "soundmanager.h"
#include "wview.h"
#include "golfdoc.h"
#include "polysoup.h"
#include "bitmap.h"
#include "westpak.h"
#include "wind.h"
#include "taskmanager.h"
#include "wtemplate.h"
#include "commonutil.h"
#include "clientsetting.h"
#include "wpuppet.h"

extern Fresh* g_pFresh;

CGolfDoc* CParticleRecv::m_pDoc;

IMPLEMENT_ACTOR(CParticleEffect, IActor)

CParticleEffect::CParticleEffect(void)
	: m_recvList(16, 16),
	  m_pManager(NULL),
	  m_bgEffectList(16, 16),
	  m_pParticle(NULL)
{
}

void CParticleEffect::OnDestroy(void)
{
	(void)g_PI;

	if (m_pManager)
	{
		delete m_pManager;
		m_pManager = NULL;
	}

	Clear();
}

void CParticleEffect::Clear(void)
{
	CParticleRecv::m_pDoc = NULL;

	for (CParticleRecv* recv = m_recvList.Start(); recv != NULL;
		recv = m_recvList.Next())
	{
		delete recv;
	}
	m_recvList.Reset();
}

void CParticleEffect::OnPreLoadInit(void)
{
	m_pManager = new CParticleManager;

	m_pParticle = new CLightColumn;
	m_pManager->MakeParticle(m_pParticle, NULL, true);

	CSceneManager::Instance()->RegisterElement((eObjType)6, 0, this,
		"\xc0\xcc\xc6\xe5\xc6\xae", 0x10, 0);
}

void CParticleEffect::OnInit(void)
{
	Clear();
}

void CParticleEffect::OnProcess(float elapsed)
{
	for (CParticleRecv* recv = m_recvList.Start(); recv != NULL;
		recv = m_recvList.Next())
	{
		if (recv->IsActive())
		{
			recv->Process(elapsed);
		}
	}

	m_pManager->Process(elapsed);
}

void CParticleEffect::Display(void)
{
	for (CParticleRecv* recv = m_recvList.Start(); recv != NULL;
		recv = m_recvList.Next())
	{
		if (recv->IsActive())
		{
			recv->Display();
		}
	}

	m_pManager->Display();
}

void CParticleEffect::DisplayShadow(void)
{
	if (m_pManager)
	{
		m_pManager->DisplayShadow();
	}
}

void CParticleEffect::HandleMsg(const MsgObject& msg)
{
	if (WSingleton<COption>::Instance()->vGetParticleLevel() == 2)
	{
		return;
	}

	switch (msg.message)
	{
	case 0x16C:
		m_pManager->MakeParticle(new CPowerShot, "\xc6\xc4\xbf\xf6\xbc\xa6",
			false);
		break;

	case 0x16E:
		if (CheckExtShotEffect("Bunker_coin.spr", "wiz_Moneybunkershot"))
		{
			break;
		}
		m_pManager->MakeParticle(new CShotParticle, NULL, false);
		break;

	case 0x16F:
		m_pManager->MakeParticle(
			new CGrassParticle(msg.param1, *(const WVector*)msg.param2),
			"\xc0\xdc\xb5\xf0", false);
		break;

	case 0x170:
		if (CheckExtShotEffect("Bunker_coin.spr", "wiz_Moneybunkershot"))
		{
			break;
		}
		m_pManager->MakeParticle(new CBunkerShotParticle(4, 100),
			"\xba\xa1\xc4\xbf", false);
		break;

	case 0x171:
		if (CheckExtShotEffect("Bunker_coinFall.spr", "wiz_Moneybunkerin"))
		{
			break;
		}
		m_pManager->MakeParticle(new CBunkerParticle, "\xba\xa1\xc4\xbf",
			false);
		break;

	case 0x173:
	{
		if (CheckExtShotEffect("Bunker_coinFall.spr", "wiz_Moneybunkerin"))
		{
			break;
		}
		CParticle* pDust = new CDust(1);
		pDust->SetCenter(
			GolfBall().m_pos - WVector(0.0f, GolfBall().m_radius, 0.0f));
		m_pManager->MakeParticle(pDust, "\xb8\xd5\xc1\xf6", false);
		break;
	}

	case 0x174:
	{
		WVector normal;
		normal = GolfBall().PreData(msg.param1)->normal;
		WVector offset = normal * GolfBall().m_radius;
		WVector center = GolfBall().PreData(msg.param1)->pos - offset;
		CParticle* pFragment = new CFragment;
		pFragment->SetNormal(normal);
		pFragment->SetCenter(center);
		m_pManager->MakeParticle(pFragment, "\xc0\xd4\xc0\xda", false);
		break;
	}

	case 0x176:
	{
		WVector ground =
			GetPVS().GetGroundPoint(GolfBall().m_pos, true, NULL, true);
		ground.y += GolfBall().GetRadius();
		CParticle* pRipple = new CRipple;
		pRipple->SetCenter(ground);
		m_pManager->MakeParticle(pRipple, "\xc6\xc4\xb9\xae", false);
		break;
	}

	case 0x172:
	{
		CParticleRecv* pRecv = m_recvList.Find((const char*)msg.param1);
		if (!pRecv)
		{
			pRecv = new CParticleRecv(MakeStr("%s.eft", msg.param1));
			m_recvList.AddItem(pRecv, (const char*)msg.param1, false);
		}

		m_recv = *(const sRecv*)msg.param2;

		pRecv->SetActive(true);
		pRecv->SetCenter(m_recv.center);
		pRecv->SetGround(m_recv.ground);
		pRecv->SetDuration(m_recv.duration);
		break;
	}

	case 0x175:
	{
		const char* name = (const char*)msg.param1;
		if (name)
		{
			CParticleRecv* pRecv = m_recvList.Find(name);
			pRecv->SetActive(false);
		}
		else
		{
			for (CParticleRecv* pRecv = m_recvList.Start(); pRecv != NULL;
				pRecv = m_recvList.Next())
			{
				pRecv->SetActive(false);
			}
		}
		break;
	}

	case 0x177:
		m_pManager->RemoveAll();
		break;

	case 0x178:
		if (m_pParticle)
		{
			m_pParticle->FadeIn();
		}
		break;

	case 0x179:
		if (m_pParticle)
		{
			m_pParticle->FadeOut();
		}
		break;

	case 0x17A:
		if (m_pParticle)
		{
			m_pParticle->SetActive(true);
		}
		break;

	case 0x17B:
		if (m_pParticle)
		{
			m_pParticle->SetActive(false);
		}
		break;

	case 0x17D:
	{
		WVector pos = !msg.param2 ? GetHoleCupPos() : GolfBall().m_pos;
		CParticle* pCoin = new CCoin(msg.param1, pos);
		g_audio->PlaySfx("pang_coin_emit", NULL, 0, NULL, NULL, 0.5f, 200.0f);
		m_pManager->MakeParticle(pCoin, "\xc8\xa6\xc4\xc5", false);
		break;
	}

	case 0x17E:
	{
		if (WSingleton<COption>::Instance()->vGetParticleLevel() < 1)
		{
			CSceneManager::Instance()->SetFog(false);
		}
		WVector pos = !msg.param1 ? GetHoleCupPos() : GolfBall().m_pos;
		m_pManager->MakeParticle(new CEnergyCenter(30, pos, msg.param2 * 0.1f),
			"\xc8\xa6\xc4\xc5", false);
		break;
	}

	case 0x17F:
		if (GOLFDOC()->GetDay() &&
			WSingleton<COption>::Instance()->vGetParticleLevel() < 1)
		{
			CSceneManager::Instance()->SetFog(true);

			IActor* pActor = AfxGetTask()->GetActor("GolfBg");
			if (pActor)
			{
				MsgObject msgObj;
				msgObj.sender = NULL;
				msgObj.time = 0;
				msgObj.message = 0x13F;
				msgObj.param1 = -1;
				pActor << msgObj;
			}

			GetPVS().SetBaseVtxColor(0xffffffff);
			GetPVS().SetObjVtxColor(0xffffffff);
		}
		break;

	case 0x180:
	{
		WPuppet* pPuppet = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (pPuppet && pPuppet->GetRootBone())
		{
			pPuppet->SetAlpha(0);
		}
		m_pManager->MakeParticle(new CStarDust, "\xc8\xa6\xc4\xc5", false);
		break;
	}

	case 0x181:
	{
		WPuppet* pPuppet = CSceneManager::Instance()->GetPuppet("\xb0\xf8");
		if (pPuppet && pPuppet->GetRootBone())
		{
			pPuppet->SetAlpha(0);
		}
		m_pManager->MakeParticle(new CStarDust2, "\xc8\xa6\xc4\xc5", false);
		break;
	}
	}
}

bool CParticleEffect::CheckExtShotEffect(const char* sprayName,
	const char* sfxName)
{
	bool result = false;
	WVector pos = GolfBall().m_pos;
	unsigned char groundType = GetPVS().GetGroundType(pos, true, NULL);
	if (groundType == 3)
	{
		if (GetCurMap() == 0x13)
		{
			CallSprayParticle(sprayName, sfxName, &pos);
			result = true;
		}
	}

	return result;
}

void CParticleEffect::CallSprayParticle(const char* sprayName,
	const char* sfxName, const WVector* pPos)
{
	if (sprayName == NULL)
	{
		return;
	}

	CFxSpray* pSpray = CFx::Instance()->OpenSpray(sprayName);
	if (pSpray && pPos)
	{
		pSpray->m_pos = *pPos;
	}

	if (sfxName)
	{
		g_audio->PlaySfx(sfxName, NULL, 0, NULL, NULL, 0.5f, 200.0f);
	}
}

CParticleManager::CParticleManager(void)
	: m_textures(16, 8)
{
	m_count = 0;
	m_permanentCount = 0;
	m_lastIndex = -1;

	RegisterTexture("\xc6\xc4\xbf\xf6\xbc\xa6", "shockwave.jpg");
	RegisterTexture("\xc8\xa6\xc4\xc5", "~particle_all03.jpg");
	RegisterTexture("\xb8\xd5\xc1\xf6", "[dust.jpg");
	RegisterTexture("\xc0\xdc\xb5\xf0", "[eft01.jpg");
	RegisterTexture("\xba\xa1\xc4\xbf", "[dust01.jpg");
	RegisterTexture("\xc0\xd4\xc0\xda", "~particle_all02.jpg");
	RegisterTexture("\xbf\xac\xb1\xe2", "[dust.jpg");
	RegisterTexture("\xc6\xc4\xb9\xae", "sea-eft.jpg");

	for (int i = 0; i < 20; i++)
	{
		m_particles[i] = NULL;
	}
}

void CParticleManager::RegisterTexture(const char* name, const char* fileName)
{
	sTexture* pTexture = new sTexture;
	pTexture->m_handle = g_resrcmng->LoadTexture(fileName, 0, 0, NULL);

	strcpy(pTexture->m_fileName, fileName);
	m_textures.AddItem(pTexture, name, false);
}

CParticleManager::~CParticleManager(void)
{
	for (int i = 0; i < 20; i++)
	{
		if (m_particles[i])
		{
			delete m_particles[i];
			m_particles[i] = NULL;
		}
	}

	for (sTexture* pTexture = m_textures.Start(); pTexture != NULL;
		pTexture = m_textures.Next())
	{
		if (pTexture->m_handle)
		{
			g_resrcmng->Release(pTexture->m_handle);
			pTexture->m_handle = 0;
		}

		delete pTexture;
	}
}

void CParticleManager::Process(float elapsed)
{
	for (int i = 0; i <= m_lastIndex; i++)
	{
		if (m_particles[i])
		{
			m_particles[i]->Process(elapsed);

			if (i >= m_permanentCount)
			{
				if (!m_particles[i]->IsActive())
				{
					m_count--;
					if (m_particles[i])
					{
						delete m_particles[i];
						m_particles[i] = NULL;
					}

					if (i == m_lastIndex)
					{
						while (m_lastIndex > 0)
						{
							if (m_particles[m_lastIndex] != NULL)
							{
								break;
							}

							m_lastIndex--;
						}
					}
				}
			}
		}
	}
}

void CParticleManager::Display(void)
{
	for (int i = 0; i <= m_lastIndex; i++)
	{
		if (m_particles[i])
		{
			m_particles[i]->Display();
		}
	}
}

void CParticleManager::DisplayShadow(void)
{
	for (int i = 0; i <= m_lastIndex; i++)
	{
		if (m_particles[i])
		{
			m_particles[i]->DisplayShadow();
		}
	}
}

void CParticleManager::MakeParticle(CParticle* pParticle, const char* texName,
	bool bPermanent)
{
	if (m_count == 20)
	{
		delete pParticle;
		return;
	}

	int i = 0;
	while (m_particles[i] != NULL)
	{
		i++;
	}

	m_lastIndex = i < m_lastIndex ? m_lastIndex : i;
	m_count++;
	if (bPermanent)
	{
		m_permanentCount++;
	}

	m_particles[i] = pParticle;
	if (texName)
	{
		sTexture* pTexture = m_textures.Find(texName);
		pParticle->LoadTexture(pTexture->m_handle);
	}

	m_particles[i]->MakeParticle();
}

void CParticleManager::RemoveAll(void)
{
	for (int i = m_permanentCount; i <= m_lastIndex; i++)
	{
		if (m_particles[i])
		{
			m_particles[i]->SetActive(false);
			delete m_particles[i];
			m_particles[i] = NULL;
		}
	}

	m_count = m_permanentCount;
}

void CParticleManager::Remove(const char* texName)
{
	w_texlist* pTexture = g_resrcmng->FindTexture(texName);
	for (int i = m_permanentCount; i <= m_lastIndex; i++)
	{
		if (m_particles[i]->GetTexHandle() == pTexture->texhandle)
		{
			m_particles[i]->SetActive(false);
			if (m_particles[i])
			{
				delete m_particles[i];
				m_particles[i] = NULL;
			}
			m_count--;
		}
	}
}

CParticleRecv::CParticleRecv(const char* fileName)
	: m_aniSpr(NULL)
{
	cFile* file = g_resrcmng->GetCFile(fileName, -1);

	if (file == NULL)
	{
		return;
	}

	file->Read(&m_alphaStart, 4);
	file->Read(&m_alphaEnd, 4);
	file->Read(&m_sizeStart, 4);
	file->Read(&m_sizeEnd, 4);
	file->Read(&m_speedMin, 4);
	file->Read(&m_speedMax, 4);
	file->Read(&m_spinMin, 4);
	file->Read(&m_spinMax, 4);
	file->Read(&m_lifeMin, 4);
	file->Read(&m_lifeMax, 4);
	file->Read(&m_gravity, 4);
	file->Read(&m_interval, 4);
	file->Read(&m_frameInterval, 4);
	file->Read(&m_emitTime, 4);
	file->Read(&m_bounce, 4);
	file->Read(&m_accel, 0xc);
	file->Read(&m_renderType, 4);
	file->Read(&m_emitCount, 4);

	file->Read(&m_emitShape, 4);

	file->Read(&m_distMin, 4);
	file->Read(&m_distMax, 4);
	file->Read(&m_box, 0x18);
	file->Read(&m_spread, 4);

	file->Read(&m_spriteSize, 4);

	file->Read(&m_startFrame, 4);
	file->Read(&m_randomFrame, 1);

	file->Read(m_texName, 0x20);

	CloseCFile(file);

	m_fragCount = (int)(m_lifeMax / m_interval) * m_emitCount;
	m_frags = new sFrag[m_fragCount];
	for (int i = 0; i < m_fragCount; i++)
	{
		m_frags[i].alive = false;
	}

	m_dirMatrix.Reset();
	m_dirAngles.Reset();

	m_aniSpr =
		g_resrcmng->Get3DAniSpr(m_texName, 0, m_spriteSize, m_spriteSize);

	m_timer = 0;
	m_nextFrag = 0;
	m_liveCount = 0;
	m_active = true;
	m_visible = true;

	m_objIndex = 0xffff;
	m_alpha = 1.0f;

	if (m_pDoc == NULL)
	{
		m_pDoc = (CGolfDoc*)AfxGetDocument();
	}

	m_pTarget = &m_center;
}

CParticleRecv::~CParticleRecv(void)
{
	Destroy();
}

void CParticleRecv::Destroy(void)
{
	if (m_aniSpr != NULL)
	{
		g_resrcmng->Release(m_aniSpr);
		m_aniSpr = NULL;
	}

	if (m_frags != NULL)
	{
		delete[] m_frags;
		m_frags = NULL;
	}
}

void CParticleRecv::SetTarget(WVector* pTarget)
{
	if (pTarget)
	{
		m_pTarget = pTarget;
	}
	else
	{
		m_pTarget = &m_center;
	}
}

void CParticleRecv::Process(float elapsed)
{
	if (m_active)
	{
		float interval = m_interval;

		for (int i = 0; i < m_fragCount; i++)
		{
			if (m_frags[i].alive)
			{
				BlowAway(elapsed, i);
			}
		}

		m_timer += elapsed;
		if (m_emitTime > 0.0f)
		{
			m_emitTime -= elapsed;
		}

		if (m_timer > interval && m_emitTime >= 0.0f)
		{
			int count;
			if (elapsed > interval)
			{
				count = (int)(m_timer / interval);
			}
			else
			{
				count = 1;
			}

			m_timer -= count * interval;
			while (count > 0)
			{
				for (int j = 0; j < m_emitCount; j++)
				{
					Generate();
					if (elapsed > interval)
					{
						BlowAway(count * interval + m_timer, m_nextFrag);
					}

					m_nextFrag = (m_nextFrag + 1) % m_fragCount;
					m_liveCount++;
				}

				count--;
			}
		}

		m_alpha =
			m_pDoc->m_pPolySoup->GetObjAlpha(m_objIndex) * (1.0f / 255.0f);
	}
}

void CParticleRecv::Display(void)
{
	if (!m_active || !m_visible)
	{
		return;
	}

	for (int i = 0; i < m_fragCount; i++)
	{
		if (m_frags[i].alive)
		{
			float size = m_frags[i].size;
			int color = ((int)(m_frags[i].alpha * m_alpha) << 24) + 0xffffff;

			m_aniSpr->SetRect(size, size, size * 0.5f, size * 0.5f);
			m_aniSpr->SetPos(m_frags[i].pos);
			m_aniSpr->SetColor(color);
			m_aniSpr->Render(g_view, m_frags[i].angle, m_renderType | 0x400000,
				m_frags[i].sprNum);
		}
	}
}

void CParticleRecv::Generate(void)
{
	m_frags[m_nextFrag].alive = true;
	m_frags[m_nextFrag].alpha = m_alphaStart;
	m_frags[m_nextFrag].size = m_sizeStart;
	m_frags[m_nextFrag].life =
		m_lifeMin + (m_lifeMax - m_lifeMin) * (rand() % 1023) / 1024.0f;
	m_frags[m_nextFrag].lifeLeft = m_frags[m_nextFrag].life;
	m_frags[m_nextFrag].frameTime = m_frameInterval;
	m_frags[m_nextFrag].sprNum =
		m_randomFrame ? rand() % m_aniSpr->GetSpriteNum() : m_startFrame;

	float sign = rand() % 2 ? 1.0f : -1.0f;
	m_frags[m_nextFrag].spin =
		(m_spinMin + (m_spinMax - m_spinMin) * (rand() % 1023) / 1024.0f) *
		sign;
	m_frags[m_nextFrag].angle = 0;
	m_frags[m_nextFrag].pos = m_center;

	switch (m_emitShape)
	{
	case 0:

		m_frags[m_nextFrag].pos.x +=
			(m_box.max.x - m_box.min.x) * (rand() % 1023) / 1024.0f +
			m_box.min.x;
		m_frags[m_nextFrag].pos.y +=
			(m_box.max.y - m_box.min.y) * (rand() % 1023) / 1024.0f +
			m_box.min.y;
		m_frags[m_nextFrag].pos.z +=
			(m_box.max.z - m_box.min.z) * (rand() % 1023) / 1024.0f +
			m_box.min.z;

		m_frags[m_nextFrag].vel =
			(WVector::UNIT_POS_Z *
				(m_distMin +
					(m_distMax - m_distMin) * (rand() % 1023) / 1024.0f)) *
			RotMat(6.2831855f * ((rand() % 1023) / 1024.0f), 1);
		m_frags[m_nextFrag].vel.y =
			m_speedMin + (m_speedMax - m_speedMin) * (rand() % 1023) / 1024.0f;
		m_frags[m_nextFrag].vel = m_frags[m_nextFrag].vel * m_dirMatrix;
		break;

	case 1:

		if (Wabs(m_spread) < g_EPSILON)
		{
			m_frags[m_nextFrag].vel =
				(WVector::UNIT_POS_Z *
					RotMat(6.2831855f * ((rand() % 1023) / 1024.0f), 0)) *
				RotMat(6.2831855f * ((rand() % 1023) / 1024.0f), 1);
		}
		else
		{
			m_frags[m_nextFrag].pos +=
				(WVector::UNIT_POS_Z * m_spread *
					RotMat(6.2831855f * ((rand() % 1023) / 1024.0f), 0)) *
				RotMat(6.2831855f * ((rand() % 1023) / 1024.0f), 1);

			WVector dir = m_frags[m_nextFrag].pos - m_center;
			m_frags[m_nextFrag].vel = (1.0f / dir.Magnitude()) * dir;
		}

		m_frags[m_nextFrag].vel *=
			m_speedMin + (m_speedMax - m_speedMin) * (rand() % 1023) / 1024.0f;
		break;
	}
}

void CParticleRecv::BlowAway(float elapsed, int index)
{
	m_frags[index].lifeLeft -= elapsed;
	if (m_frameInterval > 0.0f)
	{
		m_frags[index].frameTime -= elapsed;
	}

	if (m_frags[index].lifeLeft < 0.0f || m_frags[index].alpha < 0.0f ||
		m_frags[index].size < 0.0f)
	{
		m_frags[index].alive = false;
		m_liveCount--;
	}
	else
	{
		m_frags[index].size +=
			(m_sizeEnd - m_sizeStart) / m_frags[index].life * elapsed;
		m_frags[index].angle += elapsed * m_frags[index].spin;
		m_frags[index].alpha +=
			(m_alphaEnd - m_alphaStart) / m_frags[index].life * elapsed;

		m_frags[index].vel +=
			m_pDoc->m_pPolySoup->GetObjWindFactor(m_objIndex) *
			Wind().GetWind(m_frags[index].pos) * elapsed;
		m_frags[index].vel += elapsed * m_accel;
		m_frags[index].vel.y -= elapsed * m_gravity * 34.295296f;

		float height;
		float fall;
		if (m_bounce > 0.0f &&
			(height = m_frags[index].pos.y - m_frags[index].size) > m_ground &&
			(fall = elapsed * m_frags[index].vel.y) + m_frags[index].pos.y -
					m_frags[index].size <
				m_ground)
		{
			float ratio = height / fall;
			m_frags[index].pos += ratio * m_frags[index].vel * elapsed;
			m_frags[index].vel.y = m_bounce * m_frags[index].vel.y * -1.0f;
		}
		else
		{
			m_frags[index].pos += elapsed * m_frags[index].vel;
		}

		if (m_frags[index].frameTime < 0.0f)
		{
			m_frags[index].frameTime = m_frameInterval;
			m_frags[index].sprNum =
				(m_frags[index].sprNum + 1) % m_aniSpr->GetSpriteNum();
		}
	}
}

void CParticleRecv::SetBox(const Waabb& box)
{
	m_scale = Max(Max(box.max.x / Max(m_box.max.x, 1.0f),
					  box.max.y / Max(m_box.max.y, 1.0f)),
		box.max.z / Max(m_box.max.z, 1.0f));
	m_box = box;

	ApplyScale();
}

void CParticleRecv::ApplyScale(void)
{
	m_sizeStart *= m_scale;
	m_sizeEnd *= m_scale;
	m_speedMin *= m_scale;
	m_speedMax *= m_scale;
	m_distMin *= m_scale;
	m_distMax *= m_scale;
}

void CParticleRecv::Disconnect(void)
{
	m_pDoc = NULL;
}

CParticle2D::~CParticle2D(void)
{
	Release();
}

int CParticle2D::Initialize(const std::list<std::string>& bitmapNames,
	const WRect& rect, unsigned int maxCount, eSTYLE style, float speedMin,
	float speedMax, const WVector2D& velMin, const WVector2D& velMax)
{
	m_rect = rect;
	m_maxCount = maxCount;
	m_style = style;
	m_speedMin = speedMin;
	m_speedMax = speedMax;
	m_velMin = velMin;
	m_velMax = velMax;

	m_bitmaps.reserve(bitmapNames.size());

	std::list<std::string>::const_iterator it = bitmapNames.begin();
	while (it != bitmapNames.end())
	{
		const Bitmap* pBitmap = g_pFresh->GetBitmap((*it).c_str());
		if (pBitmap)
		{
			m_bitmaps.push_back(pBitmap);
		}

		++it;
	}

	if (m_bitmaps.empty())
	{
		return FALSE;
	}

	for (unsigned int i = 0; i < maxCount; i++)
	{
		sParticle* pParticle = new sParticle;
		memset(pParticle, 0, sizeof(sParticle));

		pParticle->m_alive = 1;
		pParticle->m_pBitmap = m_bitmaps[rand() % m_bitmaps.size()];

		pParticle->m_pos.x = m_rect.x + rand() % (int)m_rect.w;
		pParticle->m_pos.y = m_rect.y + rand() % (int)m_rect.h;

		pParticle->m_speed = speedMin + rand() % (int)(speedMax - speedMin);

		pParticle->m_vel = velMin;

		if (!(Wabs(velMax.x - velMin.x) < g_EPSILON))
		{
			float range = velMax.x - velMin.x;
			pParticle->m_vel.x =
				velMin.x + rand() % (int)(float)fabs((double)range);
		}

		if (!(Wabs(velMax.y - velMin.y) < g_EPSILON))
		{
			float range = velMax.y - velMin.y;
			pParticle->m_vel.y =
				velMin.y + rand() % (int)(float)fabs((double)range);
		}

		if (m_style == STYLE_TWINKLE)
		{
			pParticle->m_fadeIn = rand() % 2;
			pParticle->m_alpha = rand() % 125 + 125.0f;
		}
		else
		{
			pParticle->m_fadeIn = 0;
			pParticle->m_alpha = 255.0f;
		}

		MakeDisplayRect(pParticle);
		m_particles.push_back(pParticle);
	}

	return !m_particles.empty();
}

void CParticle2D::MakeDisplayRect(sParticle* pParticle)
{
	pParticle->m_src.x = 0;
	pParticle->m_src.y = 0;
	pParticle->m_src.w = (float)pParticle->m_pBitmap->Width();
	pParticle->m_src.h = (float)pParticle->m_pBitmap->Height();

	pParticle->m_dest.x =
		pParticle->m_pos.x - pParticle->m_pBitmap->Width() * 0.5f;
	pParticle->m_dest.y =
		pParticle->m_pos.y - pParticle->m_pBitmap->Height() * 0.5f;
	pParticle->m_dest.w = (float)pParticle->m_pBitmap->Width();
	pParticle->m_dest.h = (float)pParticle->m_pBitmap->Height();

	if (m_rect.x > pParticle->m_dest.x &&
		pParticle->m_dest.x + pParticle->m_dest.w > m_rect.x)
	{
		pParticle->m_src.x = m_rect.x - pParticle->m_dest.x;
		pParticle->m_src.w -= pParticle->m_src.x;
		pParticle->m_dest.x = m_rect.x;
		pParticle->m_dest.w = pParticle->m_src.w;
	}

	if (m_rect.y > pParticle->m_dest.y &&
		pParticle->m_dest.y + pParticle->m_dest.h > m_rect.y)
	{
		pParticle->m_src.y = m_rect.y - pParticle->m_dest.y;
		pParticle->m_src.h -= pParticle->m_src.y;
		pParticle->m_dest.y = m_rect.y;
		pParticle->m_dest.h = pParticle->m_src.h;
	}

	if (m_rect.x + m_rect.w > pParticle->m_dest.x &&
		m_rect.x + m_rect.w < pParticle->m_dest.x + pParticle->m_dest.w)
	{
		pParticle->m_src.w -=
			pParticle->m_dest.x + pParticle->m_dest.w - (m_rect.x + m_rect.w);
		pParticle->m_dest.w = pParticle->m_src.w;
	}

	if (m_rect.y + m_rect.h > pParticle->m_dest.y &&
		m_rect.y + m_rect.h < pParticle->m_dest.y + pParticle->m_dest.h)
	{
		pParticle->m_src.h -=
			pParticle->m_dest.y + pParticle->m_dest.h - (m_rect.y + m_rect.h);
		pParticle->m_dest.h = pParticle->m_src.h;
	}

	pParticle->m_src.x = (float)floor(pParticle->m_src.x);
	pParticle->m_src.y = (float)floor(pParticle->m_src.y);
	pParticle->m_src.w = (float)floor(pParticle->m_src.w);
	pParticle->m_src.h = (float)floor(pParticle->m_src.h);

	pParticle->m_dest.x = (float)floor(pParticle->m_dest.x);
	pParticle->m_dest.y = (float)floor(pParticle->m_dest.y);
	pParticle->m_dest.w = (float)floor(pParticle->m_dest.w);
	pParticle->m_dest.h = (float)floor(pParticle->m_dest.h);
}

void CParticle2D::Process(float elapsed)
{
	std::list<sParticle*>::iterator it = m_particles.begin();
	while (it != m_particles.end())
	{
		sParticle* pParticle = *it;
		float speed = elapsed * pParticle->m_speed;

		pParticle->m_pos += pParticle->m_vel * speed;
		if (m_style == STYLE_TWINKLE)
		{
			if (pParticle->m_fadeIn)
			{
				if (SAFE_ADDVALUE(pParticle->m_alpha, 255.0f,
						elapsed * 125.0f) >= 255.0f)
				{
					pParticle->m_fadeIn = 0;
				}
			}
			else
			{
				if (SAFE_SUBVALUE(pParticle->m_alpha, 125.0f,
						elapsed * 125.0f) <= 125.0f)
				{
					pParticle->m_fadeIn = 1;
				}
			}
		}

		MakeDisplayRect(pParticle);

		if (pParticle->m_dest.x >= m_rect.x + m_rect.w)
		{
			pParticle->m_alive = 0;
		}
		if (m_rect.y + m_rect.h <= pParticle->m_dest.y)
		{
			pParticle->m_alive = 0;
		}

		if (pParticle->m_alive == 0)
		{
			delete pParticle;
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}

	unsigned int count = m_maxCount - m_particles.size();
	for (; count != 0; count--)
	{
		sParticle* pParticle = new sParticle;
		memset(pParticle, 0, sizeof(sParticle));

		pParticle->m_alive = 1;
		pParticle->m_pBitmap = m_bitmaps[rand() % m_bitmaps.size()];

		pParticle->m_pos.x = m_rect.x + rand() % (int)m_rect.w;
		pParticle->m_pos.y = m_rect.y;

		pParticle->m_speed =
			m_speedMin + rand() % (int)(m_speedMax - m_speedMin);

		pParticle->m_vel = m_velMin;

		if (!(Wabs(m_velMax.x - m_velMin.x) < g_EPSILON))
		{
			float range = m_velMax.x - m_velMin.x;
			pParticle->m_vel.x =
				m_velMin.x + rand() % (int)(float)fabs((double)range);
		}

		if (!(Wabs(m_velMax.y - m_velMin.y) < g_EPSILON))
		{
			float range = m_velMax.y - m_velMin.y;
			pParticle->m_vel.y =
				m_velMin.y + rand() % (int)(float)fabs((double)range);
		}

		if (m_style == STYLE_TWINKLE)
		{
			pParticle->m_fadeIn = rand() % 2;
			pParticle->m_alpha = rand() % 125 + 125.0f;
		}
		else
		{
			pParticle->m_fadeIn = 0;
			pParticle->m_alpha = 255.0f;
		}

		m_particles.push_back(pParticle);
	}
}

void CParticle2D::Display(void)
{
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	std::list<sParticle*>::iterator it = m_particles.begin();
	for (; it != m_particles.end(); ++it)
	{
		sParticle* pParticle = *it;
		pGDI->DrawTexture(pParticle->m_pBitmap, pParticle->m_src,
			pParticle->m_dest,
			((unsigned char)pParticle->m_alpha << 24) | 0xffffff, 0);
	}
}

void CParticle2D::Release(void)
{
	std::list<sParticle*>::iterator it = m_particles.begin();
	while (it != m_particles.end())
	{
		if (*it)
		{
			delete *it;
			it = m_particles.erase(it);
		}
		else
		{
			++it;
		}
	}

	m_particles.clear();
}
