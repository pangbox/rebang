#include "mathconsts.h"
#include "minatl.h"
#include "projectg.h"
#include "mascot.h"
#include "npc.h"
#include "golfdoc.h"
#include "golfball.h"
#include "fxinterpreter.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"

class CCameraBird : public CCreature
{
public:
	CCameraBird(const char* name, unsigned char playerIndex);

	virtual void Process(float dt);
	virtual void Display();
	virtual void CheckTarget(float dt);

	void ChangePet(const char* name);

protected:
	unsigned char m_playerIndex;
	WMatrix m_headMat;
};

extern Fresh* g_pFresh;

IMPLEMENT_ACTOR(CMascot, IActor)

void CMascot::OnDestroy()
{
	GetFxInterpreter()->DetachFxAll();
	m_fxHandle = -1;
	for (int i = 0; i < 4; ++i)
	{
		if (m_mascot[i].pBird)
		{
			delete m_mascot[i].pBird;
			m_mascot[i].pBird = NULL;
		}
	}
}

void CMascot::OnPreLoadInit()
{
	for (int i = 0; i < 4; ++i)
		CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, NULL, this,
			MakeStr("mascot%02d", i), 8, NULL);
}

void CMascot::OnLoad()
{
	OnDestroy();
	for (unsigned char i = 0; i < GetPlayerNum(); ++i)
	{
		m_mascot[i].pInfo = &Doc()->m_userInfo[i].mascotInfo;
		if (GOLFDOC()->GetPlayer(i)->state == 3)
			continue;
		if (!m_mascot[i].pInfo->tid)
			continue;
		IFF_STRUCT::sMascot* pMascot =
			ItemManager()->FindMascot(m_mascot[i].pInfo->tid);
		if (pMascot)
		{
			m_mascot[i].pIff = pMascot;
			m_mascot[i].pBird =
				new CCameraBird(MakeStr("%s.pet", pMascot->Data), i);
			m_mascot[i].pBird->SetMotion("Default", -1.0f);
			m_mascot[i].pBird->SetAddTex(
				ChangeMascotMsgInTex(m_mascot[i].pInfo->szMsg, pMascot,
					m_mascot[i].pBird->GetPet(), MakeStr("_golf%02d", i)));
		}
	}
}

#pragma optimize("a", on)
int CMascot::ChangeMascotMsgInTex(const char* msg,
	const IFF_STRUCT::sMascot* pMascot, WPuppet* pPuppet, const char* texName)
{
	if (!msg || !pMascot || !pPuppet)
		return 0;
	if (pMascot->ShowMsg)
	{
		if (!strrchr(pMascot->MsgTex, '.'))
			return 0;
		char name[256];
		strcpy(name, pMascot->MsgTex);
		sprintf(strrchr(name, '.'), "_%s%s", texName,
			strrchr(pMascot->MsgTex, '.'));
		int texture = g_resrcmng->LoadTexture(pMascot->MsgTex, 0, 0, name);
		w_texlist* original = g_resrcmng->FindTexture(pMascot->MsgTex);
		if (texture && original)
		{
			Bitmap* bitmap = g_resrcmng->LoadBitmap(pMascot->MsgTex, 0, false);
			RECT rect = { 0, 0, bitmap->Width(), bitmap->Height() };
			if (g_pFresh)
			{
				FrGraphicInterface* device = g_pFresh->GetManager()->GetGDI();
				if (device && strlen(msg))
				{
					device->SetTextStyle(1);
					device->SetAlpha(1.0f);
					device->SetTextColor(0xff000000, 0xff808080);
					device->PrintText(
						WPoint(float2int(bitmap->Width() * 0.5f),
							float2int(bitmap->Height() * 0.5f) - 6.0f),
						1, msg, bitmap);
				}
			}
			g_resrcmng->ChangeTexturePart(texture, bitmap, rect);
			pPuppet->ChangeTextureByHandle(texture, original->texhandle, NULL);
			delete bitmap;
			return texture;
		}
		if (g_resrcmng && texture)
			g_resrcmng->Release(texture);
	}
	return 0;
}
#pragma optimize("", on)

void CMascot::OnReload()
{
	OnLoad();
}

void CMascot::OnInit()
{
}

void CMascot::OnProcess(float delta)
{
	for (int i = 0; i < GetPlayerNum(); ++i)
		if (m_mascot[i].pBird)
			m_mascot[i].pBird->Process(delta);
}

void CMascot::Display()
{
}

void CMascot::HandleMsg(const MsgObject& msg)
{
	switch (msg.message)
	{
	case 78:
	{
		unsigned long uid = msg.param1;
		if (uid == MyGuid(true))
		{
			Doc()->m_userInfo[0].mascotInfo = *(sMascotInfo*)msg.param2;
			Doc()->m_myInfo.userEquip.guidMascot =
				Doc()->m_userInfo[0].mascotInfo.guid;
		}
		m_mascot[GOLFDOC()->m_currentPlayer].pInfo =
			&Doc()->m_userInfo[0].mascotInfo;
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(
			m_mascot[GOLFDOC()->m_currentPlayer].pInfo->tid);
		if (pMascot)
		{
			CCameraBird* bird = m_mascot[GOLFDOC()->m_currentPlayer].pBird;
			if (bird)
			{
				bird->ChangePet(MakeStr("%s.pet", pMascot->Data));
				int texture = ChangeMascotMsgInTex(
					m_mascot[GOLFDOC()->m_currentPlayer].pInfo->szMsg, pMascot,
					m_mascot[GOLFDOC()->m_currentPlayer].pBird->GetPet(),
					MakeStr("_golf%02d", GOLFDOC()->m_currentPlayer));
				m_mascot[GOLFDOC()->m_currentPlayer].pBird->SetAddTex(texture);
			}
		}
	}
	break;
	case 355:
		if (m_mascot[GOLFDOC()->m_currentPlayer].pBird)
			m_mascot[GOLFDOC()->m_currentPlayer].pBird->SetActive(true);
		break;
	case 356:
		if (m_mascot[GOLFDOC()->m_currentPlayer].pBird)
			m_mascot[GOLFDOC()->m_currentPlayer].pBird->SetActive(false);
		break;
	}
}

CCameraBird::CCameraBird(const char* name, unsigned char playerIndex)
{
	Init(name, 8);
	m_playerIndex = playerIndex;
	m_speed = 0.5f;
}

void CCameraBird::Process(float dt)
{
	if (PLAYER(m_playerIndex)->state != 3 &&
		GOLFDOC()->m_currentPlayer == m_playerIndex &&
		!(GOLFDOC()->GameMode() & 0x22001))
		CCreature::Process(dt);
}

void CCameraBird::Display()
{
	if (m_pPet && m_bActive && !(m_time < 0.0f) &&
		PLAYER(m_playerIndex)->state != 3 &&
		GOLFDOC()->m_currentPlayer == m_playerIndex &&
		!(GOLFDOC()->GameMode() & 0x22001))
	{
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time, 0, NULL, false),
			Abs(m_roll) > 0.0f ? RotMat(m_roll, 2) * m_mat : m_mat);
		m_pPet->Transform(g_view, 1.0f, 0);
		m_pPet->UpdateBSphere();
		if (g_view->InFrustum(m_pPet->GetBoundSphere()))
		{
			m_pPet->UpdateLightSource(&g_lightset, false, NULL);
			m_pPet->Render(g_view, true, 1.0f,
				g_view->InFrustumSafe(m_pPet->GetBoundBox()), false, false);
		}
	}
}

void CCameraBird::CheckTarget(float dt)
{
	if (GolfBall().m_playFrame >= 0 && GOLFDOC()->GameMode() != 0x1000)
		return;
	AfxGetTask()->GetActor("Player")
		<< MsgObject(NULL, 292, m_playerIndex, (int)&m_headMat, 0, 0, 0);
	WVector dir = m_headMat.za;
	dir.y = 0.0f;
	dir.Normalize();
	m_target = m_headMat.pivot - dir * (1.2f * 3.2f);
	m_target.y = GolfBall().m_pos.y + 1.2f * 3.2f;
	dir = m_target - m_mat.pivot;
	float distance = dir.Magnitude();
	float ratio = Min(distance / (2.0f * 3.2f), 1.0f);
	m_mat.za = (GolfBall().m_pos - m_mat.pivot) * (1.0f - ratio) + dir * ratio;
	m_mat.za.y = 0.0f;
	m_mat.za.Normalize();
	m_mat.za *= -1.0f;
	WVector& up = m_mat.ya;
	WVector& right = m_mat.xa;
	up = WVector::UNIT_POS_Y;
	right = WCrossProduct(up, m_mat.za);
	WVector pos = m_mat.pivot;
	m_mat.pivot = WVector::ZERO;
	m_mat = RotMat(-55.0f * g_DEGTORAD, 1) * m_mat;
	m_mat.pivot = pos;
	if (distance < 10.0 * 3.2f)
	{
		if (distance > 0.5)
			m_mat.pivot += dir * (distance * m_speed) * dt;
	}
	else
	{
		m_mat.za = g_camera.za * -1.0f;
		m_mat.xa = WCrossProduct(m_mat.ya, m_mat.za);
		m_mat.pivot = m_target;
	}
}

void CCameraBird::ChangePet(const char* name)
{
	if (g_resrcmng && m_pPet)
	{
		g_resrcmng->Release(m_pPet, false);
		m_pPet = NULL;
	}
	m_pPet = g_resrcmng->GetPuppet(name, false, true, false);
	CSceneManager::Instance()->SetPuppet(name, m_pPet);
}
