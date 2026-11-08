#include "grounditem.h"
#include "grounditemman.h"
#include "scenemanager.h"
#include "contentsdoc.h"
#include "fx.h"
#include "golfball.h"
#include "golfdoc.h"
#include "projectg.h"
#include "wresrcmng.h"
#include "wpuppet.h"
#include "mathconsts.h"

IMPLEMENT_OBJECT(GroundItem, IObject)

unsigned int GroundItem::ms_initCnt = 0;
bool GroundItem::ms_greenItemJackpot = false;
GroundItem::BallInfo GroundItem::ms_ballInfo;

const ThSphere& GroundItem::GetBallBoundingVolume()
{
	return ms_ballInfo.volume;
}

void GroundItem::EnableGreenItemJackpot(bool bEnable)
{
	ms_greenItemJackpot = bEnable;
}

const ThSphere& GroundItem::BallInfo::MakeBallBoundingVolume(int frame)
{
	if (frame == ms_ballInfo.frame)
		return ms_ballInfo.volume;

	WVector p0 = GolfBall().PreData(frame - 1)->pos;
	WVector p1 = GolfBall().PreData(frame)->pos;

	gaV3 sum = (const gaV3&)p0 + (const gaV3&)p1;
	gaV3 diff = (const gaV3&)p1 - (const gaV3&)p0;

	ms_ballInfo.frame = frame;
	ms_ballInfo.volume.Set(sum * 0.5f, diff.Length() + GolfBall().GetRadius());
	LogOut(0, "MakeBallBoundingVolume : %f\n", diff.Length());

	return ms_ballInfo.volume;
}

GroundItem::GroundItem()
{
}

GroundItem::GroundItem(GroundItemMan* pMan, unsigned long id,
	GIMMICK_ITEM_TYPE type, FieldItem::eDisposeTextureType texType,
	const char* modelName, const Ut::Spatial& spatial)
	: m_pMan(pMan),
	  m_id(id),
	  m_type(type),
	  m_state(GROUND_ITEM_READY),
	  m_bEnable(true),
	  m_texType(texType),
	  m_modelName(modelName),
	  m_pPuppet(NULL),
	  m_spatial(spatial)
{
	CContentsDoc::Instance()->GetContainer((localContentType_t)0xa3,
		m_pGimmickContainer);

	m_stateFrame[0] = -1;
	m_stateFrame[1] = -1;
	m_stateFrame[2] = -1;
}

GroundItem::~GroundItem()
{
}

void GroundItem::Initialize()
{
	m_pPuppet = g_resrcmng->GetPuppet(m_modelName.c_str(), false, true, false);

	Ut::MakeObjectName(m_sceneElemName, "SceneElemName", *this, ms_initCnt++);

	CSceneManager::Instance()->RegisterElement(OBJ_ACTOR, m_pPuppet, NULL,
		m_sceneElemName.c_str(), 0x18, NULL);

	m_state = GROUND_ITEM_IDLE;
	m_spatial.ToMatrix(m_mat);
	Transform();
}

void GroundItem::Terminate()
{
	CSceneManager::Instance()->DeleteElement(m_sceneElemName.c_str());
	if (g_resrcmng && m_pPuppet)
	{
		g_resrcmng->Release(m_pPuppet, false);
		m_pPuppet = NULL;
	}
	m_pPuppet = NULL;
}

void GroundItem::Transform()
{
	m_pPuppet->UpdateLightSource(&g_lightset, false, NULL);

	m_pPuppet->ApplyBones(NULL, m_mat);

	m_pPuppet->Transform(NULL, 1.0f, 0);

	m_pPuppet->UpdateBound(true, false);
}

void GroundItem::NSC_OnStep(bool bForce, bool bReplay, float step, float time,
	int ballState)
{
	if (ballState == -1)
		ms_ballInfo.Reset();

	if (NeedToUpdate())
	{
		eGoundItemUpdateType type = UPDATE_NONE;

		switch (m_type)
		{
		case GIMMICK_ITEM_PANG:
			type = m_texType == FieldItem::DISPOSE_TEXTURE_COIN ? UPDATE_MAGNET
																: UPDATE_PANG;
			break;

		case GIMMICK_ITEM_BOX:
			type = m_texType == FieldItem::DISPOSE_TEXTURE_COIN ? UPDATE_MAGNET
																: UPDATE_BOX;
			break;
		}

		Update(type, bForce, bReplay, step, time, ballState);
	}
}

void GroundItem::NSC_OnFirstNewShotFrame(float time, int ballState)
{
	m_stateFrame[1] = m_stateFrame[2] = -1;

	if (m_state == GROUND_ITEM_ATTRACTED)
		ChangeState(GROUND_ITEM_IDLE, ballState);
}

void GroundItem::NSC_OnFirstReplayFrame(int ballState)
{
	if (m_state == GROUND_ITEM_ATTRACTED)
	{
		if (ballState <= m_stateFrame[1])
			ChangeState(GROUND_ITEM_IDLE, ballState);
	}

	if (m_state == GROUND_ITEM_ACQUIRED)
	{
		if (ballState <= m_stateFrame[2])
		{
			ChangeState(GROUND_ITEM_IDLE, ballState);

			GroundItemMan::PlayerStats& stats =
				m_pMan->m_pPlayerStats[Doc()->m_pGolfDoc->m_currentPlayer];
			--stats.numAcquired[m_type];
		}
	}
}

void GroundItem::DrawBoundingVolume()
{
	if (NeedToUpdate())
	{
		WVector pivot;
		float length;
		GetBoundingSphere(pivot, length);
		g_view->DrawSphere(pivot, length, 0xffff0000, 10, 0);

		g_view->DrawAABB(m_pPuppet->GetBoundBox(), 0xff00ff00, false, 0);
	}
}

void GroundItem::Enable(bool bEnable)
{
	m_bEnable = bEnable;

	CSceneManager::Instance()->SetVisible(m_sceneElemName.c_str(),
		NeedToUpdate());
}

void GroundItem::GetBoundingSphere(gaV3& center, float& radius) const
{
	const WSphere& sphere = m_pPuppet->GetBoundSphere();
	(WVector&)center = sphere.pos;
	radius = sphere.radius * 0.7f;
}

void GroundItem::GetBoundingSphere(WVector& center, float& radius) const
{
	const WSphere& sphere = m_pPuppet->GetBoundSphere();
	center = sphere.pos;
	radius = sphere.radius * 0.7f;
}

GroundItem::eGroundItemState GroundItem::ChangeState(eGroundItemState state,
	int frame)
{
	eGroundItemState oldState = m_state;

	m_state = state;

	OnChangeState(frame);
	return oldState;
}

void GroundItem::OnChangeState(int frame)
{
	bool bVisible = false;

	switch (m_state)
	{
	case GROUND_ITEM_IDLE:
		bVisible = true;
		break;

	case GROUND_ITEM_ATTRACTED:
		m_stateFrame[1] = frame;
		bVisible = true;
		break;

	case GROUND_ITEM_ACQUIRED:
		m_stateFrame[2] = frame;
		bVisible = false;
		{
			CFxSequence* pSeq = CFx::Instance()->OpenSequence(
				m_pGimmickContainer->m_crashSeqName[m_type].c_str(), true);

			if (pSeq)
			{
				pSeq->m_pos = m_mat.pivot;

				switch (m_type)
				{
				case GIMMICK_ITEM_PANG:
					pSeq->m_pos.y += 4.0f;
					break;

				case GIMMICK_ITEM_BOX:
					pSeq->m_pos.y += 7.0f;
					break;
				}
			}

			g_audio->PlaySfx(MakeStr("pang_coin_%d", rand() % 3 + 1));
		}
		break;
	}

	CSceneManager::Instance()->SetVisible(m_sceneElemName.c_str(), bVisible);
}

bool GroundItem::TestIntersection(int frame)
{
	const ThSphere& ballVolume = ms_ballInfo.MakeBallBoundingVolume(frame);

	ThSphere itemVolume;
	GetBoundingSphere(itemVolume.pos, itemVolume.r);

	if (ballVolume.TestIntersection(itemVolume))
		return true;

	return false;
}

bool GroundItem::TestIntersection2(int frame, float radius)
{
	ThSphere ballVolume = ms_ballInfo.MakeBallBoundingVolume(frame);
	ballVolume.r = radius;

	ThSphere itemVolume;
	GetBoundingSphere(itemVolume.pos, itemVolume.r);

	if (ballVolume.TestIntersection(itemVolume))
		return true;

	return false;
}

void GroundItem::Update(eGoundItemUpdateType type, bool bForce, bool bReplay,
	float step, float time, int ballState)
{
	Ut::Spatial spatial;

	float phase = m_spatial.pos.x + m_spatial.pos.y + m_spatial.pos.z;

	switch (type)
	{
	case UPDATE_PANG:
	{
		float t = phase + time;
		spatial.scl = m_spatial.scl + (sin(t) + 1.0f) * 7.0f;
		gaQ rot = m_spatial.rot;
		gaQ spin;
		spin.FromAxisAngle(gaV3(0.0f, 1.0f, 0.0f), t * 5.0f);
		rot *= spin;
		spatial.rot = rot;
		spatial.pos = m_spatial.pos;
		spatial.ToMatrix(m_mat);
		Transform();
	}
	break;

	case UPDATE_BOX:
	{
		spatial.scl = m_spatial.scl;
		gaQ rot = m_spatial.rot;
		gaQ spin;
		spin.FromAxisAngle(gaV3(0.0f, -1.0f, 0.0f), (phase + time) * 5.0f);
		rot *= spin;
		spatial.rot = rot;
		float y = (sin((time * 3.0f) + phase) + 1.0f) * 10.0f;
		spatial.pos.x = m_spatial.pos.x;
		spatial.pos.y = m_spatial.pos.y + y;
		spatial.pos.z = m_spatial.pos.z;
		spatial.ToMatrix(m_mat);
		Transform();
	}
	break;

	case UPDATE_MAGNET:
		spatial.scl = m_spatial.scl;
		if (m_state == GROUND_ITEM_ATTRACTED && bForce)
		{
			int totalFrames = GolfBall().m_renderNum - m_stateFrame[1];
			if (totalFrames > 70)
				totalFrames = 70;

			int curFrame = ballState - m_stateFrame[1];
			if (curFrame > totalFrames)
				curFrame = totalFrames;

			int delayFrames = (int)(totalFrames * 0.2f);

			if (ballState < m_stateFrame[1] + delayFrames)
			{
				spatial.rot = m_spatial.rot;
				GolfBall();

				gaQ rot(m_spatial.rot);
				gaQ spin(gaV3(0.0f, -1.0f, 0.0f), time * 20.0f);
				rot *= spin;
				spatial.rot = rot;

				spatial.pos.x = m_spatial.pos.x;
				spatial.pos.y = m_spatial.pos.y;
				spatial.pos.z = m_spatial.pos.z;

				spatial.ToMatrix(m_mat);
				Transform();
			}
			else
			{
				gaQ rot(m_spatial.rot);
				gaQ spin(gaV3(0.0f, -1.0f, 0.0f), time * 30.0f);
				rot *= spin;
				spatial.rot = rot;

				float t = gaMath::Pow(1.0f -
						(float)(curFrame - delayFrames) /
							(totalFrames - delayFrames),
					5.0f);

				float s = 1.0f - t;
				spatial.pos = m_spatial.pos * t +
					(const gaV3&)GolfBall().PreData(ballState)->pos * s;
				spatial.ToMatrix(m_mat);
				Transform();
			}
		}
		else
		{
			gaQ rot = m_spatial.rot;
			gaQ spin;
			spin.FromAxisAngle(gaV3(0.0f, 1.0f, 0.0f), time * 5.0f);
			rot *= spin;
			spatial.rot = rot;
			spatial.pos.x = m_spatial.pos.x;
			spatial.pos.y = m_spatial.pos.y;
			spatial.pos.z = m_spatial.pos.z;
			spatial.ToMatrix(m_mat);
			Transform();
		}
		break;
	}

	if (ballState > 0 && bForce)
	{
		if (type == UPDATE_MAGNET && m_state != GROUND_ITEM_ATTRACTED)
		{
			float radius = 55.0f;
			if (ms_greenItemJackpot)
				radius = 140.0f;

			if (TestIntersection2(ballState, radius))
				ChangeState(GROUND_ITEM_ATTRACTED, ballState);
		}

		if (TestIntersection(ballState))
		{
			GroundItemMan::PlayerStats& stats =
				m_pMan->m_pPlayerStats[Doc()->m_pGolfDoc->m_currentPlayer];
			++stats.numAcquired[m_type];

			if (!bReplay)
			{
				++stats.numAcquiredLive[m_type];

				FieldItem::AcquiredItemInfo info;
				info.type = m_type;
				info.index = m_id;
				info.count = 1;
				info.textureType = m_texType;
				m_pGimmickContainer->AddItemAcquired(info);
			}

			ChangeState(GROUND_ITEM_ACQUIRED, ballState);
		}
	}

	unsigned char alpha = m_pPuppet->GetAlpha(NULL);

	if (!bForce)
	{
		ThSphere ballVolume;
		ballVolume.r = 20.0f;
		const WVector& ballPos = GolfBall().m_pos;
		ballVolume.SetPos(ballPos.x, ballPos.y, ballPos.z);

		ThSphere itemVolume;
		GetBoundingSphere(itemVolume.pos, itemVolume.r);

		if (ballVolume.TestIntersection(itemVolume))
		{
			if (alpha > 100)
				m_pPuppet->SetAlpha(alpha - 1);
			return;
		}
	}

	if (alpha < 255)
		m_pPuppet->SetAlpha(alpha + 1);
}
