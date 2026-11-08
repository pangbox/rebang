#include "stdafx.h"
#include <math.h>
#include <algorithm>
#include "realmyroom_avatar.h"
#include "ntavatar.h"
#include "realmyroommain.h"
#include "bitmap.h"
#include "texturepart.h"
#include "partsinfo.h"
#include "networksystem.h"
#include "netresourcemanager.h"
#include "clientsetting.h"
#include "golfdoc.h"
#include "projectg.h"
#include "woverlay.h"
#include "fremoticon.h"
#include "packet.h"
#include "shareddoc.h"
#include "quadtree.h"
#include "commonutil.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "wpuppet.h"
#include "wbone.h"
#include "wview.h"
#include "../../shared/localize.h"
#include "mathconsts.h"

#define BONE_HEADNUB "Bip01 HeadNub"
#define BONE_PELVIS "Bip01 Pelvis"
#define OVERLAY_NEAR_Z 0.044f

const char* __fastcall GetStrConst(const char* src, char* dest);
const char* GetCharsTillSpace(const char* str, char* out);

extern Fresh* g_pFresh;
extern WMatrix g_camera;

static const char* g_faceTexture[10][4] = {
	{ "m_skin_01-f03.png", "m_skin_01-f04.png", "m_skin_01-f12.png",
     "m_skin_01-reset.png"															  },
	{ "F_Skin-f05.png",    "F_Skin-f01.png",    "F_Skin-f06.png",
     "F_Skin-reset.png"																 },
	{ "A_Face-f02.png",    "A_Face-f03.png",    "A_Face-f05.png",
     "A_Face-reset.png"																 },
	{ "C_Face-f03.png",    "C_Face-f02.png",    "C_Face-f04.png",
     "C_Face-reset.png"																 },
	{ "D_Face-f07.png",    "D_Face-f04.png",    "D_Face-f05.png",
     "D_Face-reset.png"																 },
	{ "E_def_f03.png",     "E_def_f05.png",     "E_def_f06.png",     "E_def_reset.png"  },
	{ "G_Face-blink1.png", "G_Face-f03.png",    "G_Face-f06.png",
     "G_Face-reset.PNG"																 },
	{ "H_def_f4.PNG",      "H_def_f6.PNG",      "H_def_f7.PNG",      "H_Face-reset.PNG" },
	{ "I_def_f13.PNG",     "I_def_f23.PNG",     "I_def_f21.PNG",     "I_def_reset.PNG"  },
	{ "J_Def_Blink17.PNG", "J_Def_Blink19.PNG", "J_Def_Blink22.PNG",
     "J_def-reset.PNG"																  },
};

#include "balloontext.h"

CRealMyRoom_Avatar::CRealMyRoom_Avatar(const sSlotInfo& slotInfo,
	const sCharacterInfo& charInfo, CRealMyRoomMain* pMain)
	: m_bones(true), m_prevBones(true)
{
	strcpy(m_idName, slotInfo.sNick);
	strcpy(m_nickName, slotInfo.sNick);
	m_targetName[0] = 0;
	m_followName[0] = '\0';

	if (IsLocalContent((localContentType_t)0x10))
	{
		if (slotInfo.nChannelingFlag == 1)
		{
			strcpy(m_providerId, slotInfo.sDisplayID);
			m_bShowProviderId = true;
		}
		else
		{
			memset(m_providerId, 0, sizeof(m_providerId));
			m_bShowProviderId = false;
		}
	}

	m_guid = slotInfo.dwGuid;
	m_bGM = (slotInfo.dwIdentity & 4) ? true : false;
	m_mark = slotInfo.gender;
	m_bGoodPlayer = slotInfo.manner;
	m_bAngelWing = slotInfo.angelicWings;
	m_charTypeId = slotInfo.tidChar;
	m_prevState = m_state = slotInfo.state;
	m_action = slotInfo.action;
	m_pMotionData = NULL;
	m_pPrevMotionData = NULL;
	m_motionTime = 0.0f;
	m_blendTime = 0.0f;
	m_elapsedTime = 0.0f;
	m_facialTime = 0.0f;
	m_up = WVector::UNIT_POS_Y;
	m_bTargetPosChanged = false;
	m_lastMotionSendTime = 0;
	m_startPos = WVector::UNIT_POS_Y;
	m_pMain = pMain;
	m_bPendingTargetAngle = false;
	m_startPos = m_pMain->m_startPos;
	m_bCharInfoChanged = false;
	m_pPetFrame = NULL;
	m_charInfo = charInfo;
	m_headIcon = -1;
	m_pEmblem = (Bitmap*)NetResourceManager::Instance()->GetEmblemByName(
		slotInfo.szEmblemName);
	m_pBalloon = NULL;
	m_random[0] = Random(0.8f, 1.0f);
	m_random[1] = Random(0.8f, 1.0f);
	m_random[2] = Random(0.8f, 1.0f);
	m_balloonTime = 1.0f;
	m_moveDir.x = m_moveDir.y = m_moveDir.z = 0.0f;
	SetAngle(slotInfo.location[2]);
	SetState(m_state);
	SetAction(m_action);
	SetTradeTitle(slotInfo.strTradeTitle);

	m_light.type = 2;
	m_light.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_light.nearone = g_lightset.nearone;
	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);
	m_fxBox.m_frameTime = 100.0f / 3.0f;
}

CRealMyRoom_Avatar::~CRealMyRoom_Avatar()
{
	std::list<sTexturePart*>::iterator it;
	for (it = m_texPartList.begin(); it != m_texPartList.end(); ++it)
		delete *it;
	m_texPartList.clear();

	m_fxBox.ClearAll(false);
}

bool CRealMyRoom_Avatar::IsTargetPositionChanged(bool bChanged)
{
	bool bPrev = m_bTargetPosChanged;
	m_bTargetPosChanged = bChanged;
	return bPrev;
}

void CRealMyRoom_Avatar::SetHide(bool bHide)
{
	m_bHide = bHide;

	if (m_pMain->m_pPointFx)
		CFx::Instance()->SetActive(bHide);
}

bool CRealMyRoom_Avatar::IsClickable(float x, float y) const
{
	if (!g_pFresh->IsVisible())
		return false;

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
		return false;

	if (m_alpha < 0.1f)
		return false;

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (m_state & 0x80)
	{
		WVector head;
		head = m_pPetFrame->GetBonePet()
				   ->FindBone(BONE_HEADNUB)
				   ->GetMatrix()
				   .pivot;

		WVector screen = g_view->Projection(head);

		if (!(screen.z <= 0.0f) && !(1.0f <= screen.z))
		{
			int screenX = (int)floorf(screen.x);
			int screenY = (int)floorf(screen.y) - 48;

			if (m_state & 0x80)
			{
				if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar != NULL)
				{
					if ((CRealMyRoom_AvatarManager::Instance()
								->m_pMyAvatar->m_pos -
							m_pos)
							.Magnitude() > 19.2f)
						return false;
				}

				int width = (int)pGDI->GetTextExtend(m_tradeTitle) + 10;
				if (width < 100)
					width = 100;

				struct
				{
					int left, top, width, height;
				} bounds = { screenX - width / 2, screenY - 8, width, 29 };
				if (bounds.left < x && x < bounds.left + bounds.width &&
					bounds.top < y && y < bounds.top + bounds.height)
					return true;
			}
		}
	}

	return false;
}

CRealMyRoom_Avatar* CRealMyRoom_Avatar::GetFollowingAvatar() const
{
	if (m_followName[0] == '\0')
		return NULL;

	return CRealMyRoom_AvatarManager::Instance()->FindAvatarByNickName(
		m_followName);
}

bool CRealMyRoom_Avatar::GetBoneScreenCoord(const char* boneName, int* pX,
	int* pY) const
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
		return false;

	WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);
	if (pBone == NULL)
		return false;

	WVector screen = g_view->Projection(pBone->GetMatrix().pivot);
	*pX = (int)floor(screen.x);
	*pY = (int)floor(screen.y);

	return true;
}

void CRealMyRoom_Avatar::ChangeCharacterInfo()
{
	m_fxBox.ClearAll(false);

	ParseBoundingBoxScript();
}

void CRealMyRoom_Avatar::SetCharactorInfo(const sCharacterInfo& charInfo)
{
	Stop();

	m_pPetFrame = NULL;
	m_charInfo = charInfo;
	m_charTypeId = charInfo.tid;

	m_fxBox.ClearAll(false);
}

void CRealMyRoom_Avatar::SetPosition(const WVector& position)
{
	m_pos = position;
	SetTargetPosition(position);
}

void CRealMyRoom_Avatar::SetStartPosition(const WVector& position)
{
	m_startPos = position;
}

void CRealMyRoom_Avatar::SetTargetPosition(const WVector& position)
{
	m_targetPos = position;
	m_bTargetPosChanged = true;
}

void CRealMyRoom_Avatar::SetAngle(float angle)
{
	angle = RadianNormalize2PI(angle);
	m_angle = angle;
	m_targetAngle = angle;
}

void CRealMyRoom_Avatar::SetTargetAngle(float angle)
{
	m_targetAngle = RadianNormalize2PI(angle);
}

void CRealMyRoom_Avatar::SetPendingTargetAngle(float angle)
{
	m_pendingTargetAngle = RadianNormalize2PI(angle);
	m_bPendingTargetAngle = true;
}

void CRealMyRoom_Avatar::SetState(unsigned long state)
{
	if (m_state == state)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
		this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		WSendPacket packet((enumClientPacket)0x63);
		packet.Encode1(8);
		packet.Encode4(state);
		packet.Send(TO_GAME);
	}

	m_state = state;
	if (state & 0x7e)
		Stop();
}

void CRealMyRoom_Avatar::SetAction(int action)
{
	if (m_action == action)
		return;

	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
		this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		WSendPacket packet((enumClientPacket)0x63);
		packet.Encode1(5);
		packet.Encode4(action);
		packet.Send(TO_GAME);
	}

	m_action = action;
}

void CRealMyRoom_Avatar::SendMove(const WVector& delta, bool bForce)
{
	if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
		this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		m_moveDir += delta;

		if (m_moveDir.Magnitude() >= 16.5f || bForce)
			m_moveDir.Reset();
	}
}

void CRealMyRoom_Avatar::Stop()
{
	if (this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
	{
		if (!WisEqual(m_targetPos, m_pos, 0.00001f))
		{
			SendMove(WVector(0.0f, 0.0f, 0.0f), true);

			m_targetPos = m_pos;
			m_bTargetPosChanged = true;
		}

		m_targetName[0] = 0;
		m_followName[0] = '\0';
	}
}

void CRealMyRoom_Avatar::Follow(const char* nickName)
{
	if (nickName)
		strcpy(m_followName, nickName);
	else
		m_followName[0] = '\0';
}

void CRealMyRoom_Avatar::ChangeMotion(int motion, const char* name,
	float blendTime)
{
	const char* motionName;

	m_motion = motion;
	if (m_pPetFrame == NULL)
		return;

	switch (motion)
	{
	case 0:
		motionName = "\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc";
		break;
	case 2:
		motionName = "chat_\xbe\xc9\xb1\xe2";
		break;
	case 3:
		motionName = "chat_\xb4\xaf\xb1\xe2";
		break;
	case 1:
		motionName = "chat_\xb4\xde\xb8\xae\xb1\xe2";
		break;
	case 4:
		motionName = name;
		if (WNetworkSystem::Instance()->IsConnected(WNetworkSystem::NET_GAME) &&
			this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			unsigned long now = timeGetTime();

			if (now - m_lastMotionSendTime <= 1000)
				return;

			m_lastMotionSendTime = now;
			WSendPacket packet((enumClientPacket)0x63);
			packet.Encode1(7);
			packet.EncodeStr(std::string(name));
			packet.Send(TO_GAME);
		}
		break;
	default:
		motionName = "\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc";
		break;
	}

	w_motion_data* pMotion =
		m_pPetFrame->FindMotionData(motionName, true, NULL);
	if (pMotion == NULL)
		return;

	if (blendTime > 0.0f && m_pMotionData != NULL)
	{
		m_prevBones.CopyFrom(&m_bones);
		m_pPrevMotionData = m_pMotionData;
		m_prevMotion = m_motion;
		m_pMotionData = pMotion;
		m_motion = motion;
		m_motionTime = 0.0f;
		m_blendTime = blendTime;
	}
	else
	{
		m_pPrevMotionData = NULL;
		m_motionTime = 0.0f;
		m_pMotionData = pMotion;
		m_bones.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL, pMotion->s,
			0, NULL, false));
	}

	if (m_motionTime == 0.0f)
	{
		bool bInvisible = motion != 4;
		WBone* pBone;

		switch (m_charTypeId & 0x3ffffff)
		{
		case 4:
			pBone = m_pPetFrame->GetBonePet()->FindBone("D_Ball");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			pBone = m_pPetFrame->GetBonePet()->FindBone("D_Racket");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			break;
		case 5:
			pBone = m_pPetFrame->GetBonePet()->FindBone("E_Cannon");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			pBone = m_pPetFrame->GetBonePet()->FindBone("E_Telescope");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			break;
		case 6:
			pBone = m_pPetFrame->GetBonePet()->FindBone("G_Bird");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			break;
		case 7:
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword01");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword02");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			pBone = m_pPetFrame->GetBonePet()->FindBone("H_karen");
			if (pBone)
				pBone->SetInvisible(false, bInvisible);
			break;
		}

		if ((m_charTypeId & 0x3ffffff) < 10)
		{
			SetPendingTexPart("face",
				g_faceTexture[m_charTypeId & 0x3ffffff][3]);
		}
	}

	ProcessMotion(0.0f);
}

void CRealMyRoom_Avatar::ProcessMotion(float elapsed)
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
		return;

	if (m_motion != 0 && m_motion != 2 && m_motion != 3 && m_motion != 1)
	{
		if (m_pMotionData->e - m_pMotionData->s < m_motionTime)
			ChangeMotion(0, NULL, 0.2f);
	}

	if (m_pPrevMotionData != NULL)
	{
		float ratio = m_motionTime / m_blendTime;

		if (ratio > 1.0f)
		{
			m_pPrevMotionData = NULL;
		}
		else
		{
			float time = m_motionTime;
			float length = m_pMotionData->e - m_pMotionData->s;
			if (length < m_motionTime)
				time = length;

			m_bones.Blend(&m_prevBones,
				m_pPetFrame->GetCurAniPet()->GetBones(m_pMotionData->name, time,
					0, NULL, false),
				ratio);
		}
	}
	else
	{
		float length = m_pMotionData->e - m_pMotionData->s;

		if (m_motionTime > length)
		{
			if (length > 0.01f)
			{
				float loops = floorf(m_motionTime / length);
				m_motionTime -= loops * length;
			}
			else
			{
				m_motionTime = 0.0f;
			}
		}

		m_bones.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL,
			m_pMotionData->s + m_motionTime, 0, NULL, false));
	}

	if (m_motionTime == 0.0f)
	{
		ScriptProcess(m_pMotionData, m_pMotionData->s, m_pMotionData->s, true);
	}
	else
	{
		ScriptProcess(m_pMotionData, m_pMotionData->s + m_motionTime,
			elapsed + (m_pMotionData->s + m_motionTime), false);
	}
}

void* CRealMyRoom_Avatar::AttachFx(const Attacher& attacher, const char* name,
	bool bSequence, defines_t* defines)
{
	int len = strlen(name);
	if (len < 5)
		return NULL;

	if (strcmpi(name + len - 3, "spr") == 0)
	{
		CFxSpray* pSpray = m_fxBox.LoadSpray(name, defines);
		if (pSpray == NULL)
			return NULL;

		pSpray->m_bMark = bSequence;
		pSpray->m_attacher = (Attacher*)&attacher;
		pSpray->m_centerX = attacher.x;
		pSpray->m_centerY = attacher.y;
		m_fxBox.AddSpray(pSpray);
		return pSpray;
	}
	else
	{
		CFxSequence* pSequence = m_fxBox.LoadSequence(name, true, defines);
		if (pSequence == NULL)
			return NULL;

		pSequence->m_bMark = bSequence;
		pSequence->m_attacher = (Attacher*)&attacher;
		pSequence->m_centerX = attacher.x;
		pSequence->m_centerY = attacher.y;
		m_fxBox.AddSequence(pSequence);
		return pSequence;
	}
}

static bool SlideMove(const WVector& pos, const WVector& delta,
	const Waabb& box, WVector& out)
{
	float lengthSq = delta * delta;
	if (Wabs(lengthSq) < g_EPSILON)
	{
		out = pos;
		return false;
	}

	WPlane planes[4];
	WVector start = pos;
	WVector move = delta;
	int numPlanes = 0;
	int bump;
	for (bump = 0; bump < 1; bump++)
	{
		trace_s trace;
		WVector end = start + move;
		GetPVS().TraceOBB_New(start, end, box, &trace);
		if (trace.startsolid)
		{
			out = start;
			return true;
		}
		if (trace.fraction > 0.0f)
		{
			out = trace.endpos;
			numPlanes = 0;
		}
		if (trace.fraction == 1.0f)
			break;
		if (numPlanes >= 3)
			return true;

		int i;
		for (i = 0; i < numPlanes; i++)
		{
			if (trace.plane.normal * planes[i].normal > 0.99f)
			{
				move += trace.plane.normal;
				break;
			}
		}
		if (i < numPlanes)
			continue;
		planes[numPlanes] = trace.plane;
		numPlanes++;
	}
	return bump > 0;
}

static void MakeUpMatrix(WMatrix& mat, const WVector& up)
{
	if (WisEqual(up, WVector::UNIT_POS_Y, 0.00001f))
	{
		mat.Reset();
		return;
	}
	WVector axis;
	axis = WCrossProduct(up, WVector::UNIT_POS_Y);
	axis.Normalize();
	mat = RotMat(axis,
		(float)acos((double)(WVector::UNIT_POS_Y.x * up.x +
			WVector::UNIT_POS_Y.y * up.y + WVector::UNIT_POS_Y.z * up.z)));
}

void CRealMyRoom_Avatar::ProcessChangeState()
{
	m_prevState = m_state;
}

void CRealMyRoom_Avatar::Process(float elapsed)
{
	if (m_bCharInfoChanged)
	{
		m_partTidList = m_burden.m_partTidList;
		m_bCharInfoChanged = false;
		m_pPetFrame = m_burden.m_pPetFrame;
		m_burden.m_pPetFrame->CalcBVolume();
		m_pPetFrame->UpdateLightSource(&g_lightset, false, NULL);
		ParseBoundingBoxScript();

		switch (m_action)
		{
		case 0:
			ChangeMotion(0, NULL, 0.f);
			break;
		case 1:
			ChangeMotion(2, NULL, 0.f);
			break;
		case 2:
			ChangeMotion(3, NULL, 0.f);
			break;
		default:
			ChangeMotion(0, NULL, 0.f);
			break;
		}
	}

	if (CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar == this &&
		(CSharedDoc::Instance()->m_myInfo.info.dwFlagBlock & 8))
		SetHeadIcon(3);
	else
		SetHeadIcon(-1);

	m_prevState = m_state;
	ProcessMotion(elapsed);

	int ground;
	WVector offset;
	if (m_followName[0])
	{
		CRealMyRoom_Avatar* pTarget =
			CRealMyRoom_AvatarManager::Instance()->FindAvatarByNickName(
				m_followName);
		if (pTarget != NULL)
		{
			offset = pTarget->m_pos - m_pos;
			float distance = WVectorLen(offset);
			if (distance > 64.0f)
			{
				m_followName[0] = 0;
				Stop();
			}
			else if (distance > 12.8f && distance > 0.0f)
			{
				offset.Normalize();
				offset *= distance - 12.8f;
				SetTargetPosition(offset + m_pos);
				SetAction(0);
			}
		}
		else
		{
			Stop();
			m_targetName[0] = 0;
			m_followName[0] = 0;
		}
	}

	WVector moved;
	moved.x = moved.y = moved.z = 0.0f;
	WVector dir;
	dir = m_targetPos - m_pos;
	if (!(Wabs(dir.x) < g_EPSILON) || !(Wabs(dir.z) < g_EPSILON))
	{
		dir.y = 0.0f;
		dir.Normalize();
		WVector delta;
		delta = dir * 16.5f * elapsed;
		offset =
			WVector(m_targetPos.x - m_pos.x, 0.0f, m_targetPos.z - m_pos.z);
		if (delta.Magnitude() > WVectorLen(offset))
			delta = offset;

		WVector newPos;
		if (this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
			m_bHide)
		{
			Waabb box(WVector(-1.f, -40.f, -1.f), WVector(1.f, 8.f, 1.f));
			SlideMove(m_pos, delta, box, newPos);
		}
		else
		{
			newPos = delta + m_pos;
		}

		moved = newPos - m_pos;
		moved.y = 0.0f;
		if (WVectorLen(moved) > 0.01f)
		{
			GetPVS().GetGroundPoint(newPos, true, &ground, true);
			if (ground >= 0)
				m_pos = newPos;
			else if (this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
				moved.x = moved.y = moved.z = 0.0f;
		}
		else
		{
			moved.x = moved.y = moved.z = 0.0f;
		}
	}

	SendMove(moved, false);

	float speed = moved.x * moved.x + moved.z * moved.z;
	if ((float)sqrt((double)speed) > 0.01f)
	{
		if (m_motion != 1)
			ChangeMotion(1, NULL, 0.2f);
	}
	else
	{
		if (this == CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
		{
			if (m_moveDir.x != 0.0f || m_moveDir.z != 0.0f)
				SendMove(WVector::ZERO, true);
			m_targetPos = m_pos;
		}
		if (m_motion != 4)
		{
			switch (m_action)
			{
			case 0:
				if (m_motion != 0)
					ChangeMotion(0, NULL, 0.2f);
				break;
			case 1:
				if (m_motion != 2)
					ChangeMotion(2, NULL, 0.2f);
				break;
			case 2:
				if (m_motion != 3)
					ChangeMotion(3, NULL, 0.3f);
				break;
			}
		}
	}

	if (speed > 0.001f)
	{
		m_targetAngle = (float)atan2((double)dir.z, (double)dir.x) + 1.5707964f;
	}
	else if (this != CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar &&
		m_bPendingTargetAngle)
	{
		float angle = m_pendingTargetAngle;
		m_targetAngle = RadianNormalize2PI(angle);
	}

	float diff = m_targetAngle - m_angle;
	diff = RadianNormalize2PI(diff);
	if (diff > g_PI)
		diff -= g_2_PI;
	if (diff > 0.0f && m_targetAngle < m_angle)
		m_angle -= g_2_PI;
	else if (diff < 0.0f && m_targetAngle > m_angle)
		m_angle += g_2_PI;
	if (fabs(diff) > 0.001f)
	{
		float angle = m_angle + elapsed * diff * 10.0f;
		m_angle = angle;
		if ((diff > 0.0f && angle > m_targetAngle) ||
			(diff < 0.0f && angle < m_targetAngle))
			m_angle = m_targetAngle;
	}

	m_pos = GetPVS().GetGroundPoint(m_pos, false, NULL, false);

	offset = g_view->camera.pivot - m_pos;
	offset.y = 0.0f;
	float cameraDistance = offset.Magnitude();
	if (cameraDistance > 192.0f)
	{
		m_alpha = 0.0f;
	}
	else if (cameraDistance > 160.0f)
	{
		m_alpha = 1.0f - (cameraDistance - 160.0f) * 0.03125f;
	}
	else
	{
		if (IsLocalContent((localContentType_t)10))
		{
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(m_guid);
			if (((*it).second.dwIdentity & 4) && !((*it).second.state & 1))
				m_alpha = .1f;
			else
				m_alpha = 1.f;
		}
		else
			m_alpha = 1.f;
	}

	WVector normal;
	if (m_motion == 3)
		normal = GetPVS().GetMeshNormal(m_pos);
	else
		normal = WVector::UNIT_POS_Y;

	WVector axis = WCrossProduct(m_up, normal);
	axis.Normalize();
	float axisLengthSq = axis * axis;
	if (!(Wabs(axisLengthSq) < g_EPSILON))
	{
		if ((float)acos((double)(m_up * normal)) < elapsed * g_2_PI)
			m_up = normal;
		else
		{
			WMatrix rotation = RotMat(axis, elapsed * -6.2831855f);
			m_up = RotVec(m_up, rotation);
		}
	}

	WMatrix mat;
	MakeUpMatrix(mat, m_up);
	mat = RotMat(m_angle, 1) * mat;
	mat.pivot = m_pos;

	if (m_pPetFrame)
	{
		m_pPetFrame->ApplyBones(&m_bones, mat);
		m_pPetFrame->Transform(g_view, 1.0f, 0);
		m_pPetFrame->UpdateBound(true);
		m_pPetFrame->SetAlpha((unsigned char)(m_alpha * 255.0f));
	}

	if (m_facialTime > 0.0f)
	{
		m_facialTime -= elapsed;
		if (m_facialTime < 0.0f)
		{
			m_facialTime = 0.0f;
			SetPendingTexPart("face",
				g_faceTexture[m_charTypeId & 0x3ffffff][3]);
		}
	}

	float elapsedMs = elapsed * 1000.0f;
	m_fxBox.ProcessAttacher(elapsedMs);
	m_fxBox.CFxBox::Process(elapsedMs);

	m_motionTime += elapsed;
	m_elapsedTime += elapsed;
}

void CRealMyRoom_Avatar::Render()
{
	if (!m_bHide)
		return;

	if (IsLocalContent((localContentType_t)10))
	{
		if (this != CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar && m_bGM)
		{
			int guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::const_iterator it =
				CSharedDoc::Instance()->m_briefUserInfoMap.find(guid);
			if (!(sBriefUserInfo((*it).second).state & 1))
			{
				if (guid != CSharedDoc::Instance()->m_myInfo.info.dwGuid)
					return;
			}
		}
	}

	WVector pos;
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		pos = GetPVS().GetGroundPoint(m_pos, false, NULL, false);
	}
	else
	{
		pos =
			m_pPetFrame->GetBonePet()->FindBone(BONE_PELVIS)->GetMatrix().pivot;
		pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	}

	if (COption::Instance()->vIsShadowEnabled())
	{
		WVector normal(2.9f, 0.0f, 0.0f);
		WVector tangent(0.0f, 0.0f, 2.9f);
		GetPVS().DrawDecal(m_pMain->m_shadowTex, pos, normal, tangent, 1.0f,
			m_alpha, W_POLY_BLEND_NORMAL, false);
	}

	if (m_pPetFrame != NULL && m_pPetFrame->GetCurAniPet() != NULL)
	{
		if (g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox()))
		{
			if (CRealMyRoom_AvatarManager::Instance()->m_pPickedAvatar == this)
			{
				m_light.nearone = g_lightset.nearone;
				m_light.diffuse = m_light.ambient = 0xffffffff;
			}
			else
			{
				m_light.nearone = g_lightset.nearone;
				m_light.ambient = 0xffd2d2d2;
				m_light.diffuse = ModifyColor(g_lightset.diffuse,
					GetPVS().GetShadowIntensity(pos));
			}

			m_pPetFrame->UpdateLightSource(&m_light, false, NULL);
			m_pPetFrame->Render(g_view, true, 1.0f, false, false, true);
		}
	}

	g_view->Flush(1);

	m_fxBox.Display(g_view);
}

void CRealMyRoom_Avatar::RenderOverlay(bool bDetail)
{
	if (m_pBalloon)
		m_pBalloon->SetTransparency(m_alpha);

	if (GOLFDOC()->m_gameMode != 0x200)
	{
		if (Wabs(m_alpha) < g_EPSILON)
			return;
	}

	float alpha = GOLFDOC()->m_gameMode == 0x200 ? 1.0f : m_alpha;

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
		return;

	if (!g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox()))
		return;

	WVector head;
	head = m_pPetFrame->GetBonePet()->FindBone(BONE_HEADNUB)->GetMatrix().pivot;

	WVector screen = g_view->Projection(head);
	if (!(screen.z <= OVERLAY_NEAR_Z) && !(screen.z >= 1.0f))
	{
		screen.x = floorf(screen.x);
		screen.y = floorf(screen.y);

		if (IsLocalContent((localContentType_t)10))
		{
			if (m_guid != CSharedDoc::Instance()->m_myInfo.info.dwGuid)
			{
				std::map<unsigned long, sBriefUserInfo>::iterator it =
					CSharedDoc::Instance()->m_briefUserInfoMap.find(m_guid);
				if ((*it).second.IsIdentity(4) && !((*it).second.state & 1))
					return;
			}
		}

		if (CProjectG::Instance()->HidePrivacy())
			return;

		if (!m_bHide)
			return;

		float y = screen.y - 4.0f;
		FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
		y -= 22.0f;
		FrEmoticon* pEmoticon = g_pFresh->GetManager()->GetEmoticon();

		WOverlay* pIcon;
		if (m_bAngelWing)
			pIcon = static_cast<WOverlay*>(m_pMain->m_pAngelMark[m_mark % 2]);
		else if (m_bGoodPlayer)
			pIcon = static_cast<WOverlay*>(m_pMain->m_pMannerMark[m_mark % 2]);
		else
			pIcon = static_cast<WOverlay*>(m_pMain->m_pSexMark[m_mark]);

		unsigned int emblemWidth = m_pEmblem ? m_pEmblem->Width() : 0;
		int iconWidth = pIcon->GetWidth();
		float x;
		unsigned long alphaColor;
		unsigned long color;
		WRect dst;
		{
			float width =
				(float)emblemWidth + pEmoticon->GetTextWidth(m_nickName);
			WRect src(0.0f, 0.0f, 1.0f, 1.0f);
			x = screen.x - (width + iconWidth) * 0.5f;
			dst = WRect(x, y - 7.0f, pIcon->GetWidth(), pIcon->GetHeight());
			x = (float)pIcon->GetWidth() + x;

			alphaColor = (unsigned char)(int)(alpha * 255.0f) << 24;
			color = alphaColor | 0xffffff;
			pIcon->Render(g_view, src, dst, 0, color, 0.0f, 0);
		}

		if (m_pEmblem)
		{
			dst = WRect(0.5f / (float)m_pEmblem->Width() + x,
				0.5f / (float)m_pEmblem->Height() + y - 5.0f,
				(float)m_pEmblem->Width(), (float)m_pEmblem->Height());
			pGDI->DrawTexture(m_pEmblem, dst, color, 0);
			x = (float)m_pEmblem->Width() + x;
		}

		pGDI->SetScale(1.0f);
		pGDI->SetTextStyle(2);
		if (CRealMyRoom_AvatarManager::Instance()->m_pPickedAvatar == this)
			pGDI->SetTextColor(alphaColor, color);
		else
			pGDI->SetTextColor(color, alphaColor);

		pEmoticon->SetAnim(false);
		pEmoticon->PrintText(WPoint(x, y), 0, m_nickName, color);
	}
}

void CRealMyRoom_Avatar::ScriptProcess(const w_motion_data* motion, float from,
	float to, bool bInit)
{
	if ((!(from <= motion->e) || from == to) && !bInit)
		return;

	int start, end;
	from *= 30.0f;
	if (bInit)
	{
		start = Ceil(from, 0.03f);
		end = start;
	}
	else
	{
		to = Min(motion->e, to) * 30.0f;
		start = Ceil(from, 0.03f);
		end =
			Wabs((float)floor(to) - to) < 1e-5f ? (int)to - 1 : (int)floor(to);

		int startFrame = (int)floor(motion->s * 30.0f + 0.5f);
		if (start == startFrame)
			start++;

		if (start > end)
			return;
	}

	if (m_pPetFrame)
	{
		WPuppet* pPuppet = m_pPetFrame->GetCurAniPet();

		if (pPuppet)
		{
			int first, last;
			if (pPuppet->CheckFrameData(start, end, first, last))
			{
				for (int i = first; i <= last; i++)
					ParsePuppetScript(pPuppet->GetFrameData(i));
			}
		}
	}
}

void CRealMyRoom_Avatar::ParsePuppetScript(const char* script)
{
	static char s_command[1024];
	static char s_name[64];

	unsigned long len;

	if (script == NULL)
		return;

	const char* str = script;
	do
	{
		str = strchr(str, '*');
		if (str == NULL)
			return;

		int cmdLen = 0;
		while (str[cmdLen] != '\0')
		{
			++cmdLen;
			if (str[cmdLen] == ' ')
				break;
			if (str[cmdLen] == '(')
			{
				++cmdLen;
				break;
			}
		}

		if (cmdLen <= 1)
			return;

		strncpy(s_command, str, cmdLen);
		str += cmdLen;
		s_command[cmdLen] = '\0';

		if (strcmp(s_command, "*fx(") == 0)
		{
			char name[64];
			char fxName[64];
			char token[64];
			char boneName[64];

			name[0] = '\0';
			const char* next = GetStrConst(str, name);
			if (!(len = strlen(name)) || strlen(name) >= 64)
				return;

			if (name[0] == '@')
			{
				if (m_pMotionData == NULL)
					return;
				if (strcmp(name + 1, m_pMotionData->name) != 0)
					return;

				next = GetStrConst(next, fxName);
				if (!(len = strlen(fxName)) || strlen(fxName) >= 64)
					return;
			}
			else
			{
				int i = 0;
				char c;
				do
				{
					c = name[i];
					fxName[i] = c;
					++i;
				} while (c != '\0');
			}

			str = GetStrConst(next, token);
			int tokenLen = strlen(token);
			if (tokenLen == 0 || tokenLen >= 64)
				return;

			int j = 0;
			char c;
			do
			{
				c = token[j];
				boneName[j] = c;
				++j;
			} while (c != '\0');

			if (!(len = strlen(boneName)) || strlen(boneName) >= 64)
				return;

			Attacher attacher;
			unsigned int count;
			for (count = 0; count < 4; ++count)
			{
				if (count != 0)
				{
					str = GetStrConst(str, boneName);
					if (!(len = strlen(boneName)) || strlen(boneName) >= 64)
						break;
				}

				WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);
				if (pBone != NULL)
				{
					attacher.mat[count] = &pBone->GetMatrix();
				}
				else
				{
					WPuppet* pPuppet = m_pPetFrame->GetBonePet();
					w_bound_box* bb = pPuppet->m_bbList.Find(boneName);
					if (bb != NULL)
					{
						if (bb->bone != NULL)
							attacher.mat[count] = &bb->bone->GetMatrix();
						attacher.pos[count] = &bb->info->spherePivot;
					}
				}
			}

			if ((int)count > 0)
				AttachFx(attacher, fxName, true, NULL);
		}
		else if (strcmp(s_command, "*hidebone(") == 0 ||
			strcmp(s_command, "*showbone(") == 0)
		{
			char boneName[64] = "";
			int count = 0;

			do
			{
				str = GetStrConst(str, boneName);
				if (!(len = strlen(boneName)) || strlen(boneName) >= 64)
					break;

				WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);
				if (pBone != NULL)
					pBone->SetInvisible(false, s_command[1] == 'h');

				if (str == NULL)
					return;
				if (*str == ')')
					break;
			} while (++count < 64);
		}
		else if (strcmp(s_command, "*snd(") == 0)
		{
			str = GetStrConst(str, s_name);
			if (!(len = strlen(s_name)) || strlen(s_name) >= 64)
				return;

			WVector delta = m_pos - g_camera.pivot;
			if (delta.Magnitude() < 192.0f)
				g_audio->PlaySfx(s_name, &delta, 5, NULL, NULL, 0.5f, 200.0f);
		}
		else if (strcmp(s_command, "*stepsnd(") == 0)
		{
			if (IsLocalContent((localContentType_t)10))
			{
				if (this != CRealMyRoom_AvatarManager::Instance()->m_pMyAvatar)
				{
					len = m_guid;
					std::map<unsigned long, sBriefUserInfo>::const_iterator it =
						CSharedDoc::Instance()->m_briefUserInfoMap.find(len);
					if (m_bGM && !(sBriefUserInfo((*it).second).state & 1))
						continue;
				}

				const char* sound = GetPVS().GetStepSound(
					GetPVS().GetGroundPoint(m_pos, false, NULL, false));
				if (sound != NULL)
				{
					WVector delta = m_pos - g_camera.pivot;
					if (delta.Magnitude() < 192.0f)
						g_audio->PlaySfx(sound, &delta, 5, sound, NULL, 0.5f,
							200.0f);
				}
			}
			else
			{
				const char* sound = GetPVS().GetStepSound(
					GetPVS().GetGroundPoint(m_pos, false, NULL, false));
				if (sound != NULL)
				{
					WVector delta = m_pos - g_camera.pivot;
					if (delta.Magnitude() < 192.0f)
						g_audio->PlaySfx(sound, &delta, 5, sound, NULL, 0.5f,
							200.0f);
				}
			}
		}
		else if (strcmp(s_command, "*ptex(") == 0)
		{
			char part[64];
			char texture[64];

			part[0] = '\0';
			const char* next = GetStrConst(str, part);
			if (!(len = strlen(part)) || strlen(part) >= 64)
				return;

			if (part[0] == '@')
			{
				if (m_pMotionData == NULL)
					return;
				if (strcmpi(part + 1, m_pMotionData->name) != 0)
					return;

				next = GetStrConst(next, s_name);
				if (!(len = strlen(s_name)) || strlen(s_name) >= 64)
					return;
			}
			else
			{
				int i = 0;
				char c;
				do
				{
					c = part[i];
					s_name[i] = c;
					++i;
				} while (c != '\0');
			}

			str = GetStrConst(next, texture);
			if (!(len = strlen(texture)) || strlen(texture) >= 64)
				return;

			if (m_facialTime > 0.0f)
				return;

			SetPendingTexPart(s_name, texture);
		}
		else if (stricmp(s_command, "*showvis(") == 0)
		{
			ParseScript_ShowVis(true, str);
		}
		else if (stricmp(s_command, "*hidevis(") == 0)
		{
			ParseScript_ShowVis(false, str);
		}
	} while (str != NULL);
}

void CRealMyRoom_Avatar::ParseScript_ShowVis(bool bShow, const char* groups)
{
	char token[256] = "";
	int count = 0;

	do
	{
		groups = GetStrConst(groups, token);

		if (strlen(token) == 0 || strlen(token) >= sizeof(token))
			break;

		std::list<std::string>* pGroup =
			CSharedDoc::Instance()->GetVisGroup(token);

		if (pGroup)
		{
			for (std::list<std::string>::iterator it = pGroup->begin();
				it != pGroup->end(); ++it)
			{
				WBone* pBone =
					m_pPetFrame->GetBonePet()->FindBone((*it).c_str());

				if (pBone)
					pBone->SetInvisible(false, !bShow);
			}
		}

		if (groups == NULL || *groups == ')')
			break;
	} while (++count < 64);
}

void CRealMyRoom_Avatar::ParseBoundingBoxScript()
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetBonePet() == NULL)
		return;

	static std::vector<CFxSpray*> s_sprays;
	static std::vector<CFxSequence*> s_sequences;

	char key[] = "*fx";
	WList<w_bound_box*>* pList = &m_pPetFrame->GetBonePet()->m_bbList;

	unsigned int i;
	for (i = 0; i < s_sprays.size(); ++i)
		m_fxBox.DeleteSpray(s_sprays[i]);
	for (i = 0; i < s_sequences.size(); ++i)
		m_fxBox.DeleteSequence(s_sequences[i], true);

	s_sprays.std::vector<CFxSpray*>::~vector();
	s_sequences.std::vector<CFxSequence*>::~vector();

	for (w_bound_box* bb = pList->Start(); bb != NULL; bb = pList->Next())
	{
		const char* option = bb->info->option;
		if (option == NULL || bb->bone == NULL || bb->bone->IsInvisible())
			continue;

		const char* next;
		const char* ext;
		for (const char* str = strstr(option, key); str != NULL;
			str = strstr(next, key))
		{
			char name[64];
			char part[64];

			unsigned int pos = strlen(key);
			if (str[pos] == '(')
				++pos;

			if (pos > strlen(key))
				next = GetStrConst(str + pos, name);
			else
				next = GetCharsTillSpace(str + pos, name);

			int len = strlen(name);
			if (len == 0 || len >= 64)
				continue;

			ext = strrchr(name, '.');
			if (ext == NULL || strlen(ext) < 4)
				continue;

			next = GetStrConst(next, part);
			len = strlen(part);
			if (len == 0 || len >= 64)
				continue;

			bool bCondition = false;
			bool bWearing = false;
			do
			{
				if (part[0] != '&')
					break;

				bCondition = true;
				unsigned long tid = atoi(part + 1);
				if (m_pPetFrame->GetPartList().FindPart(tid) ||
					m_pPetFrame->FindAuxPart(tid))
				{
					bWearing = true;
					break;
				}

				next = GetStrConst(next, part);
				len = strlen(part);
			} while (len != 0 && len < 64);
			if (bCondition && !bWearing)
				continue;

			Attacher attacher;
			attacher.mat[0] = &bb->bone->GetMatrix();
			attacher.pos[0] = &bb->info->spherePivot;

			if (stricmp(ext + 1, "spr") == 0)
			{
				CFxSpray* pSpray = m_fxBox.LoadSpray(name, NULL);
				if (pSpray != NULL)
				{
					pSpray->m_attacher = (Attacher*)&attacher;
					pSpray->m_centerX = attacher.x;
					pSpray->m_centerY = attacher.y;
					memcpy(pSpray->m_attachMat, attacher.mat,
						sizeof(attacher.mat));
					memcpy(pSpray->m_attachPos, attacher.pos,
						sizeof(attacher.pos));
					m_fxBox.AddSpray(pSpray);
					s_sprays.push_back(pSpray);
				}
			}
			else if (stricmp(ext + 1, "seq") == 0)
			{
				CFxSequence* pSequence = m_fxBox.LoadSequence(name, true, NULL);
				if (pSequence != NULL)
				{
					pSequence->m_attacher = (Attacher*)&attacher;
					pSequence->m_centerX = attacher.x;
					pSequence->m_centerY = attacher.y;
					m_fxBox.AddSequence(pSequence);
					s_sequences.push_back(pSequence);
				}
			}
		}
	}
}

sTexturePart* CRealMyRoom_Avatar::FindTexturePart(const char* part,
	const char* texture)
{
	sTexturePart* pTexPart;
	std::list<sTexturePart*>::iterator it;

	it = std::find_if(m_texPartList.begin(), m_texPartList.end(),
		sFindTexPart(texture));

	if (it != m_texPartList.end())
	{
		pTexPart = *it;
	}
	else
	{
		pTexPart = new sTexturePart;

		strcpy(pTexPart->name, part);
		strcpy(pTexPart->part, texture);
		m_texPartList.push_back(pTexPart);
	}

	return pTexPart;
}

void CRealMyRoom_Avatar::SetPendingTexPart(const char* part,
	const char* texture)
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
		return;

	sTexturePart* pTexPart = FindTexturePart(part, texture);

	if (pTexPart->bitmap == NULL)
	{
		m_pPetFrame->GetBonePet()->ChangeTexturePart(m_pPetFrame->GetFaceTex(),
			pTexPart->part, &pTexPart->bitmap, &pTexPart->rect);
	}
	else
	{
		m_pPetFrame->GetBonePet()->ChangeTexturePart(m_pPetFrame->GetFaceTex(),
			pTexPart->bitmap, pTexPart->rect);
	}

	m_pPetFrame->SetMakeup();
}

void CRealMyRoom_Avatar::SetChatFacial(int facial)
{
	SetPendingTexPart("face", g_faceTexture[m_charTypeId & 0x3ffffff][facial]);

	m_facialTime = 5.0f;
}

void CRealMyRoom_Avatar::SetBalloonText(const char* text)
{
	m_balloonTime = m_alpha;
	m_balloonElapsed = 0;
}

void CRealMyRoom_Avatar::SetTradeTitle(const char* title)
{
	if (title)
		strcpy(m_tradeTitle, title);
	else
		m_tradeTitle[0] = '\0';
}

void CRealMyRoom_Avatar::RefreshUccClothes(unsigned long tid, unsigned long uid)
{
	if (m_pPetFrame == NULL)
		return;

	if (IsLocalContent(S4_GUID_BASE))
	{
		m_pPetFrame->SetPart(&m_partTidList, tid, uid);
		m_pPetFrame->SetPart(&m_partTidList, tid, uid);
	}
}

CRealMyRoom_AvatarManager::CRealMyRoom_AvatarManager()
{
	m_avatarCount = 0;
	for (int i = 0; i < 30; i++)
		m_avatar[i] = NULL;

	m_pMyAvatar = NULL;
	m_pPickedAvatar = NULL;
	m_pClickedAvatar = 0;
	m_bActive = true;
	m_bShowName = true;
}

CRealMyRoom_AvatarManager::~CRealMyRoom_AvatarManager()
{
	FreeAllAvatars();
}

void CRealMyRoom_AvatarManager::FreeAllAvatars()
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (m_avatar[i]->m_pPetFrame)
			{
				delete m_avatar[i]->m_pPetFrame;
				m_avatar[i]->m_pPetFrame = NULL;
			}

			if (m_avatar[i])
			{
				delete m_avatar[i];
				m_avatar[i] = NULL;
			}
		}
	}
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::FindAvatarByIDName(
	const char* idName) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
		{
			if (strcmp(m_avatar[i]->m_idName, idName) == 0)
				return m_avatar[i];
		}
	}

	return NULL;
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::FindAvatarByNickName(
	const char* nickName) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
		{
			if (strcmp(m_avatar[i]->m_nickName, nickName) == 0)
				return m_avatar[i];
		}
	}

	return NULL;
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::FindAvatarByGuid(int guid) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
		{
			if (m_avatar[i]->m_guid == guid)
				return m_avatar[i];
		}
	}

	return NULL;
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::CreateAvatar(
	const sSlotInfo& slotInfo, const sCharacterInfo& charInfo,
	CRealMyRoomMain* pMain)
{
	int slot;

	for (slot = 0; slot < 30; slot++)
	{
		if (m_avatar[slot] == NULL)
			break;
	}

	if (slot == 30)
		return NULL;

	m_avatarCount++;

	CRealMyRoom_Avatar* pAvatar =
		new CRealMyRoom_Avatar(slotInfo, charInfo, pMain);
	WVector pos(slotInfo.location[0], 0.f, slotInfo.location[1]);
	pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	pAvatar->SetPosition(pos);
	pAvatar->SetAngle(slotInfo.location[2]);

	m_avatar[slot] = pAvatar;
	return pAvatar;
}

bool CRealMyRoom_AvatarManager::DeleteAvatar(const char* idName)
{
	int i;
	for (i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (strcmp(m_avatar[i]->m_idName, idName) == 0)
				break;
		}
	}

	if (i == 30)
		return false;

	if (m_avatar[i])
	{
		delete m_avatar[i];
		m_avatar[i] = NULL;
	}
	m_avatarCount--;

	return true;
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::AvatarByRayIntersect(
	const WVector& pos, const WVector& dir) const
{
	Waabb aabb;
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (IsLocalContent((localContentType_t)10))
			{
				if (m_avatar[i]->m_guid !=
					CSharedDoc::Instance()->m_myInfo.info.dwGuid)
				{
					CSharedDoc* pDoc = CSharedDoc::Instance();
					std::map<unsigned long, sBriefUserInfo>::iterator it;
					it = pDoc->m_briefUserInfoMap.find(m_avatar[i]->m_guid);

					if (!(pDoc->m_myInfo.info.dwIdentity & 4))
					{
						if ((unsigned char)(sBriefUserInfo((*it).second)
												.dwIdentity >>
								2) &
							1)
						{
							if (!(sBriefUserInfo((*it).second).state & 1))
								break;
						}
					}
				}
			}

			aabb.min = WVector(-1.f, 0.f, -1.f);
			aabb.max = WVector(1.f, 8.f, 1.f);
			aabb.min += m_avatar[i]->m_pos;
			aabb.max += m_avatar[i]->m_pos;

			if (RayIntersectAABB(aabb, pos, dir, NULL, 0.0f) < 1.0f)
				return m_avatar[i];
		}
	}

	return NULL;
}

CRealMyRoom_Avatar* CRealMyRoom_AvatarManager::CheckClick(float x,
	float y) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] && m_avatar[i]->IsClickable(x, y))
			return m_avatar[i];
	}

	return NULL;
}

void CRealMyRoom_AvatarManager::Process(float elapsed)
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
			m_avatar[i]->Process(elapsed);
	}
}

void CRealMyRoom_AvatarManager::Render()
{
	if (!m_bActive)
		return;

	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
			m_avatar[i]->Render();
	}
}

void CRealMyRoom_AvatarManager::ResetAvatarPositionAngle()
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] != NULL)
		{
			m_avatar[i]->m_pos = m_avatar[i]->m_targetPos;
			m_avatar[i]->m_angle = m_avatar[i]->m_targetAngle;
		}
	}
}

static int __cdecl CompareCameraDistance(const void* a, const void* b)
{
	WVector camera = g_view->GetCamera().pivot;

	WVector deltaA;
	deltaA = camera - (*(CRealMyRoom_Avatar* const*)a)->GetPosition();
	WVector deltaB;
	deltaB = camera - (*(CRealMyRoom_Avatar* const*)b)->GetPosition();

	float distA = deltaA.SquareMagnitude();
	float distB = deltaB.SquareMagnitude();

	if (distA > distB)
		return -1;
	if (distA < distB)
		return 1;
	return 0;
}

void CRealMyRoom_AvatarManager::RenderOverlay()
{
	if (!m_bActive)
		return;

	CRealMyRoom_Avatar* avatars[30];
	int count = 0;
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
			avatars[count++] = m_avatar[i];
	}

	qsort(avatars, count, sizeof(CRealMyRoom_Avatar*), CompareCameraDistance);

	for (int j = 0; j < count; j++)
	{
		if (avatars[j])
			avatars[j]->RenderOverlay(m_bShowName);
	}
}

CRealMyRoom_AvatarBurden::CRealMyRoom_AvatarBurden()
	: m_charInfo(), m_pPetFrame(NULL)
{
}

CRealMyRoom_AvatarBurden::CRealMyRoom_AvatarBurden(unsigned long guid,
	const sCharacterInfo& charInfo, CPetFrame* pPetFrame)
	: m_charInfo()
{
	m_guid = guid;

	m_charInfo = charInfo;
	m_pPetFrame = pPetFrame;
}

CRealMyRoom_AvatarBurden::~CRealMyRoom_AvatarBurden()
{
}

bool CRealMyRoom_AvatarBurden::LoadAvatarBurden()
{
	if (!m_partTidList.SetTids(&m_charInfo, 0xff))
		m_partTidList.SetDefaultTids();
	if (!m_partTidList.IsComboValid())
		m_partTidList.SetDefaultTids();

	if (m_pPetFrame)
	{
		delete m_pPetFrame;
		m_pPetFrame = NULL;
	}

	m_pPetFrame = new CPetFrame;

	if (!m_pPetFrame->Build(&m_partTidList, m_charInfo.tidAuxParts, true, false,
			m_guid))
	{
		delete m_pPetFrame;
		m_pPetFrame = NULL;
		return false;
	}

	m_pPetFrame->SetHairClr(&m_partTidList, m_charInfo.hairClr);
	m_pPetFrame->SetShirtsClr(&m_partTidList, m_charInfo.shirtsClr);

	return true;
}
