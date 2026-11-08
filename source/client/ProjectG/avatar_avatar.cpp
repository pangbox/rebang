#include "stdafx.h"
#include "mathconsts.h"
#include "commonutil.h"
#include "avatar_avatar.h"
#include "ntavatar.h"
#include "quadtree.h"
#include "partsinfo.h"
#include "balloontext.h"
#include "texturepart.h"
#include "avatar_main.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frgraphicinterface.h"
#include "fremoticon.h"
#include "clientsetting.h"
#include "netresourcemanager.h"
#include "woverlay.h"
#include "gmtoolkit.h"

extern Fresh* g_pFresh;

// clang-format off
static const char* s_faceTextures[10][4] = {
	{ "m_skin_01-f03.png", "m_skin_01-f04.png", "m_skin_01-f12.png", "m_skin_01-reset.png" },
	{ "F_Skin-f05.png", "F_Skin-f01.png", "F_Skin-f06.png", "F_Skin-reset.png" },
	{ "A_Face-f02.png", "A_Face-f03.png", "A_Face-f05.png", "A_Face-reset.png" },
	{ "C_Face-f03.png", "C_Face-f02.png", "C_Face-f04.png", "C_Face-reset.png" },
	{ "D_Face-f07.png", "D_Face-f04.png", "D_Face-f05.png", "D_Face-reset.png" },
	{ "E_def_f03.png", "E_def_f05.png", "E_def_f06.png", "E_def_reset.png" },
	{ "G_Face-blink1.png", "G_Face-f03.png", "G_Face-f06.png", "G_Face-reset.PNG" },
	{ "H_def_f4.PNG", "H_def_f6.PNG", "H_def_f7.PNG", "H_Face-reset.PNG" },
	{ "I_def_f13.PNG", "I_def_f23.PNG", "I_def_f21.PNG", "I_def_reset.PNG" },
	{ "J_Def_Blink17.PNG", "J_Def_Blink19.PNG", "J_Def_Blink22.PNG", "J_def-reset.PNG" },
};
// clang-format on

extern WMatrix g_camera;

extern LightSet g_lightset;

ntAvatar::ntAvatar(CQuadTree& pvs, const sSlotInfo& slotInfo,
	const sCharacterInfo& charInfo, ntAvatarChatMain* pChatMain)
	: m_bones(true), m_prevBones(true)
{
	m_pPVS = &pvs;
	strcpy(m_idName, slotInfo.sNick);
	strcpy(m_nickName, slotInfo.sNick);
	m_targetName[0] = false;
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

	m_uid = slotInfo.dwUserUID;
	m_guid = slotInfo.dwGuid;
	m_bGM = (slotInfo.dwIdentity >> 2) & 1;
	m_mark = slotInfo.gender;
	m_bGoodPlayer = slotInfo.manner;
	m_bAngelWing = slotInfo.angelicWings;
	m_charTypeId = slotInfo.tidChar;
	m_state = slotInfo.state;
	m_prevState = m_state;
	m_action = slotInfo.action;
	m_pMotionData = NULL;
	m_pPrevMotionData = NULL;
	m_blendElapsed = 0.0f;
	m_blendTime = 0.0f;
	m_motionTime = 0.0f;
	m_facialTime = 0.0f;
	m_up = WVector::UNIT_POS_Y;
	m_bTargetPosChanged = false;
	m_lastMotionSendTime = 0;
	m_bPendingTargetAngle = false;
	m_pMain = pChatMain;
	m_bBurdenLoaded = false;
	m_pPetFrame = NULL;
	m_charInfo = charInfo;
	m_headIcon = -1;

	m_pEmblem = NULL;
	if (slotInfo.GuildId != 0)
	{
		m_pEmblem = WSingleton<NetResourceManager>::Instance()->GetEmblemByName(
			slotInfo.szEmblemName);
	}

	m_pBalloon = NULL;
	m_balloonColor[0] = Random(0.8f, 1.0f, -1);
	m_balloonColor[1] = Random(0.8f, 1.0f, -1);
	m_balloonColor[2] = Random(0.8f, 1.0f, -1);
	m_balloonColor[3] = 1.0f;

	m_moveDir.Reset();
	float angle = slotInfo.location[2];
	m_targetAngle = m_angle = RadianNormalize2PI(angle);
	SetState(m_state);
	SetAction(m_action);
	if (slotInfo.strTradeTitle)
	{
		strcpy(m_tradeTitle, slotInfo.strTradeTitle);
	}
	else
	{
		m_tradeTitle[0] = '\0';
	}

	m_light.type = 2;
	m_light.ambient = ModifyColor(g_lightset.ambient, 1.0f);
	m_light.nearone = g_lightset.nearone;

	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);

	m_fxBox.m_frameTime = 33.333332f;
}

ntAvatar::~ntAvatar()
{
	if (m_pBalloon)
	{
		WSingleton<ntBalloonTextManager>::Instance()->DeleteBalloonText(
			m_pBalloon);
	}

	for (std::list<sTexturePart*>::iterator it = m_texPartList.begin();
		it != m_texPartList.end(); ++it)
	{
		delete *it;
	}
	m_texPartList.clear();

	m_fxBox.ClearAll(false);
}

bool ntAvatar::IsTargetPositionChanged(bool changed)
{
	bool wasChanged = m_bTargetPosChanged;
	m_bTargetPosChanged = changed;
	return wasChanged;
}

bool ntAvatar::IsClickable(float x, float y) const
{
	if (!g_pFresh->IsVisible())
	{
		return false;
	}

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		return false;
	}

	if (m_height < 0.1f)
	{
		return false;
	}

	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();
	if (m_state & 0x80)
	{
		WVector head;
		head = m_pPetFrame->GetBonePet()
				   ->FindBone("Bip01 HeadNub")
				   ->GetMatrix()
				   .pivot;

		WVector screen = g_view->Projection(head);

		if (!(screen.z <= 0.0f) && !(1.0f <= screen.z))
		{
			int screenX = (int)floorf(screen.x);
			int screenY = (int)floorf(screen.y) - 48;

			if (m_state & 0x80)
			{
				if ((WSingleton<ntAvatarManager>::Instance()
							->m_pMyAvatar->GetPosition() -
						GetPosition())
						.Magnitude() > 19.2f)
				{
					return false;
				}

				int width = (int)pGDI->GetTextExtend(m_tradeTitle) + 10;
				if (width < 100)
				{
					width = 100;
				}

				RECT rc;
				rc.left = screenX - width / 2;

				rc.top = screenY - 8;
				if (rc.left < x && x < rc.left + width && rc.top < y &&
					y < rc.top + 29)
				{
					return true;
				}

				WOverlay* pIcon = m_pMain->m_pStateIconN1;

				rc.left = screenX - pIcon->GetWidth() / 2;
				rc.top = screenY - pIcon->GetHeight() / 2 - 36;

				if (rc.left < x && x < rc.left + pIcon->GetWidth() &&
					rc.top < y && y < rc.top + pIcon->GetHeight())
				{
					return true;
				}
			}
		}
	}

	return false;
}

ntAvatar* ntAvatar::GetFollowingAvatar() const
{
	if (m_followName[0] != '\0')
	{
		return NULL;
	}

	return WSingleton<ntAvatarManager>::Instance()->FindAvatarByNickName(
		m_followName);
}

bool ntAvatar::GetBoneScreenCoord(const char* boneName, int* pX, int* pY) const
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		return false;
	}

	WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);
	if (pBone == NULL)
	{
		return false;
	}

	WVector screen = g_view->Projection(pBone->GetMatrix().pivot);
	*pX = (int)floor(screen.x);
	*pY = (int)floor(screen.y);

	return true;
}

void ntAvatar::SetCharactorInfo(const sCharacterInfo& charInfo)
{
	CPartTidList partTidList;

	Stop();

	m_pPetFrame = NULL;
	m_charInfo = charInfo;
	m_charTypeId = charInfo.tid;

	m_fxBox.ClearAll(false);
}

void ntAvatar::SetPosition(const WVector& position)
{
	m_pos = position;
	SetTargetPosition(position);
}

void ntAvatar::SetTargetPosition(const WVector& position)
{
	m_targetPos = position;
	m_bTargetPosChanged = true;
}

void ntAvatar::SetAngle(float angle)
{
	float normalized = RadianNormalize2PI(angle);
	m_angle = normalized;
	m_targetAngle = normalized;
}

void ntAvatar::SetTargetAngle(float angle)
{
	m_targetAngle = RadianNormalize2PI(angle);
}

void ntAvatar::SetPendingTargetAngle(float angle)
{
	m_pendingTargetAngle = RadianNormalize2PI(angle);
	m_bPendingTargetAngle = true;
}

void ntAvatar::SetState(unsigned long state)
{
	if (m_state == state)
	{
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME) &&
		this == WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
	{
		WSendPacket packet((enumClientPacket)0x63);
		packet.Encode1(8);
		packet.Encode4(state);
		packet.Send(TO_GAME);
	}

	m_state = state;
	if (state & 0x7E)
	{
		Stop();
	}
}

void ntAvatar::SetAction(int action)
{
	if (m_action == action)
	{
		return;
	}

	if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME) &&
		this == WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
	{
		WSendPacket packet((enumClientPacket)0x63);
		packet.Encode1(5);
		packet.Encode4(action);
		packet.Send(TO_GAME);
	}

	m_action = action;
}

void ntAvatar::SendMove(const WVector& position, bool force)
{
	if (!WSingleton<WNetworkSystem>::Instance()->IsConnected(
			WNetworkSystem::NET_GAME) ||
		this != WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
	{
		return;
	}

	m_moveDir += position;
	float speed = m_spItem[2].getvalue();
	if (!(m_moveDir.Magnitude() >= speed * 16.5f) && !force)
	{
		return;
	}

	WSendPacket packet((enumClientPacket)0x63);
	packet.Encode1(6);
	packet.EncodeBuffer(&m_moveDir.x, 4);
	packet.EncodeBuffer(&m_moveDir.z, 4);
	packet.EncodeBuffer(&m_angle, 4);
	packet.Send(TO_GAME);

	m_moveDir.Reset();
}

void ntAvatar::Stop()
{
	if (this != WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
	{
		return;
	}

	if (m_targetPos != m_pos)
	{
		SendMove(WVector(0.0f, 0.0f, 0.0f), true);

		m_targetPos = m_pos;
		m_bTargetPosChanged = true;
	}

	m_targetName[0] = false;
	m_followName[0] = '\0';
}

void ntAvatar::Follow(const char* nickName)
{
	if (nickName)
	{
		strcpy(m_followName, nickName);
	}
	else
	{
		m_followName[0] = '\0';
	}
}

void ntAvatar::ChangeMotion(int motion, const char* name, float blendTime)
{
	const char* motionName;

	m_motion = motion;

	if (m_pPetFrame == NULL)
	{
		return;
	}

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
		if (WSingleton<WNetworkSystem>::Instance()->IsConnected(
				WNetworkSystem::NET_GAME) &&
			this == WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
		{
			unsigned long now = timeGetTime();

			if (now - m_lastMotionSendTime <= 1000)
			{
				return;
			}

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
	{
		return;
	}

	if (blendTime > 0.0f && m_pMotionData != NULL)
	{
		m_prevBones.CopyFrom(&m_bones);
		m_pPrevMotionData = m_pMotionData;
		m_prevMotion = m_motion;
		m_pMotionData = pMotion;
		m_motion = motion;
		m_blendElapsed = 0.0f;
		m_blendTime = blendTime;
	}
	else
	{
		m_pPrevMotionData = NULL;
		m_blendElapsed = 0.0f;
		m_pMotionData = pMotion;
		m_bones.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL, pMotion->s,
			0, NULL, false));
	}

	if (m_blendElapsed == 0.0f)
	{
		bool invisible = motion != 4;

		switch ((m_charTypeId & 0x3FFFFFF) - 4)
		{
		case 0:
		{
			WBone* pBone = m_pPetFrame->GetBonePet()->FindBone("D_Ball");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}

			pBone = m_pPetFrame->GetBonePet()->FindBone("D_Racket");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}
			break;
		}
		case 1:
		{
			WBone* pBone = m_pPetFrame->GetBonePet()->FindBone("E_Cannon");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}

			pBone = m_pPetFrame->GetBonePet()->FindBone("E_Telescope");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}
			break;
		}
		case 2:
		{
			WBone* pBone = m_pPetFrame->GetBonePet()->FindBone("G_Bird");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}
			break;
		}
		case 3:
		{
			WBone* pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword01");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}

			pBone = m_pPetFrame->GetBonePet()->FindBone("H_Sword02");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}

			pBone = m_pPetFrame->GetBonePet()->FindBone("H_karen");
			if (pBone)
			{
				pBone->SetInvisible(false, invisible);
			}
			break;
		}
		}

		SetPendingTexPart("face", s_faceTextures[m_charTypeId & 0x3FFFFFF][3]);
	}

	ProcessMotion(0.0f);
}

void ntAvatar::ProcessMotion(float elapsed)
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		return;
	}

	if (m_motion != 0 && m_motion != 2 && m_motion != 3 && m_motion != 1)
	{
		if (m_pMotionData->e - m_pMotionData->s < m_blendElapsed)
		{
			ChangeMotion(0, NULL, 0.2f);
		}
	}

	if (m_pPrevMotionData != NULL)
	{
		float t = m_blendElapsed / m_blendTime;

		if (t > 1.0f)
		{
			m_pPrevMotionData = NULL;
		}
		else
		{
			float time = m_blendElapsed;
			float length = m_pMotionData->e - m_pMotionData->s;
			if (length < m_blendElapsed)
			{
				time = length;
			}

			m_bones.Blend(&m_prevBones,
				m_pPetFrame->GetCurAniPet()->GetBones(m_pMotionData->name, time,
					0, NULL, false),
				t);
		}
	}
	else
	{
		float length = m_pMotionData->e - m_pMotionData->s;

		if (m_blendElapsed > length)
		{
			if (length > 0.01f)
			{
				float loops = floorf(m_blendElapsed / length);
				m_blendElapsed -= loops * length;
			}
			else
			{
				m_blendElapsed = 0.0f;
			}
		}

		m_bones.CopyFrom(m_pPetFrame->GetCurAniPet()->GetBones(NULL,
			m_pMotionData->s + m_blendElapsed, 0, NULL, false));
	}

	if (m_blendElapsed == 0.0f)
	{
		ScriptProcess(m_pMotionData, m_pMotionData->s, m_pMotionData->s, true);
	}
	else
	{
		ScriptProcess(m_pMotionData, m_pMotionData->s + m_blendElapsed,
			elapsed + (m_pMotionData->s + m_blendElapsed), false);
	}
}

void* ntAvatar::AttachFx(const Attacher& attacher, const char* name,
	bool bAlive, defines_t* pDefines)
{
	int len = strlen(name);

	if (len < 5)
	{
		return NULL;
	}

	if (strcmpi(name + len - 3, "spr") == 0)
	{
		CFxSpray* pSpray = m_fxBox.LoadSpray(name, pDefines);

		if (pSpray == NULL)
		{
			return NULL;
		}

		pSpray->m_bMark = bAlive;
		pSpray->m_attacher = (Attacher*)&attacher;
		pSpray->m_centerX = attacher.x;
		pSpray->m_centerY = attacher.y;
		m_fxBox.AddSpray(pSpray);
		return pSpray;
	}
	else
	{
		CFxSequence* pSequence = m_fxBox.LoadSequence(name, true, pDefines);

		if (pSequence == NULL)
		{
			return NULL;
		}

		pSequence->m_bMark = bAlive;
		pSequence->m_attacher = (Attacher*)&attacher;
		pSequence->m_centerX = attacher.x;
		pSequence->m_centerY = attacher.y;
		m_fxBox.AddSequence(pSequence);
		return pSequence;
	}
}

CQuadTree& ntAvatar::GetPVS()
{
	return *m_pPVS;
}

void ntAvatar::SetLocalPVS(CQuadTree& pvs)
{
	m_pPVS = &pvs;
}

static void ClipVelocity(const WVector& in, const WVector& normal, WVector& out,
	float overbounce)
{
	float backoff = (WDotProduct(in, normal) * overbounce);
	out = in - normal * backoff;
}

static bool SlideMove(const WVector& pos, const WVector& velocity,
	CQuadTree* pPVS, const Waabb& box, WVector& out)
{
	WVector start;
	WVector dir;
	WVector clipDir;
	WVector crease;
	WPlane planes[4];
	trace_s trace;
	int numPlanes;
	int bump, numBumps = 4;
	int i, j, k;

	if (Wabs(velocity.SquareMagnitude()) < g_EPSILON)
	{
		out = pos;
		return false;
	}

	start = pos;
	dir = velocity;
	numPlanes = 0;

	for (bump = 0; bump < numBumps; bump++)
	{
		pPVS->TraceAABB(start, start + dir, box, &trace);

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
		{
			break;
		}

		if (numPlanes >= 3)
		{
			return true;
		}

		for (i = 0; i < numPlanes; i++)
		{
			if (WDotProduct(trace.plane.normal, planes[i].normal) > 0.99f)
			{
				dir += trace.plane.normal;
				break;
			}
		}

		if (i < numPlanes)
		{
			continue;
		}

		planes[numPlanes] = trace.plane;
		numPlanes++;

		for (i = 0; i < numPlanes; i++)
		{
			if (WDotProduct(dir, planes[i].normal) >= 0.0f)
			{
				continue;
			}

			ClipVelocity(dir, planes[i].normal, clipDir, 1.001f);

			for (j = 0; j < numPlanes; j++)
			{
				if (j == i)
				{
					continue;
				}

				if (WDotProduct(planes[j].normal, clipDir) >= 0.0f)
				{
					continue;
				}

				ClipVelocity(clipDir, planes[j].normal, clipDir, 1.001f);

				if (WDotProduct(clipDir, planes[i].normal) >= 0.0f)
				{
					continue;
				}
				const WVector& normal = planes[j].normal;
				crease = WCrossProduct(planes[i].normal, normal);
				crease.Normalize();
				clipDir = crease * WDotProduct(crease, dir);

				for (k = 0; k < numPlanes; k++)
				{
					if (k == i || k == j)
					{
						continue;
					}

					if (WDotProduct(clipDir, planes[k].normal) >= 0.0f)
					{
						continue;
					}

					return true;
				}
			}

			dir = clipDir;
			break;
		}
	}

	return bump > 0;
}

static void MakeUpRotation(WMatrix& mat, const WVector& up)
{
	if (WisEqual(up, WVector::UNIT_POS_Y, 1e-5f))
	{
		mat.Reset();
	}
	else
	{
		WVector axis;
		axis = WCrossProduct(up, WVector::UNIT_POS_Y);
		axis.Normalize();

		float angle = acosf(WDotProduct(up, WVector::UNIT_POS_Y));
		mat = RotMat(axis, angle);
	}
}

void ntAvatar::ProcessChangeState()
{
	m_prevState = m_state;
}

void ntAvatar::Process(float elapsed)
{
	if (m_bBurdenLoaded)
	{
		m_partTidList = m_burden.m_partTidList;

		m_bBurdenLoaded = false;
		m_pPetFrame = m_burden.m_pPetFrame;
		m_pPetFrame->CalcBVolume();
		m_pPetFrame->UpdateLightSource(&g_lightset, false, NULL);

		switch (m_action)
		{
		case 0:
			ChangeMotion(0, NULL, 0.0f);
			break;
		case 1:
			ChangeMotion(2, NULL, 0.0f);
			break;
		case 2:
			ChangeMotion(3, NULL, 0.0f);
			break;
		default:
			ChangeMotion(0, NULL, 0.0f);
			break;
		}
	}

	if (WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar == this &&
		(Doc()->GetFlagBlock() & 8))
	{
		SetHeadIcon(3);
	}
	else
	{
		SetHeadIcon(-1);
	}

	m_prevState = m_state;
	ProcessMotion(elapsed);

	WVector delta;
	int hit;
	if (m_followName[0] != '\0')
	{
		ntAvatar* pTarget =
			WSingleton<ntAvatarManager>::Instance()->FindAvatarByNickName(
				m_followName);
		if (pTarget)
		{
			delta = pTarget->m_pos - m_pos;
			float dist = sqrtf(
				delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
			if (dist > 64.0f)
			{
				m_followName[0] = '\0';
				Stop();
			}
			else if (dist > 12.8f && dist > 0.0f)
			{
				delta.Normalize();
				delta *= dist - 12.8f;
				SetTargetPosition(m_pos + delta);
				SetAction(0);
			}
		}
		else
		{
			Stop();
			m_targetName[0] = false;
			m_followName[0] = '\0';
		}
	}

	WVector move;
	move.Reset();
	WVector dir;
	dir = m_targetPos - m_pos;
	if (!(Wabs(dir.x) < g_EPSILON) || !(Wabs(dir.z) < g_EPSILON))
	{
		dir.y = 0.0f;
		dir.Normalize();

		float speed = m_spItem[2].getvalue();
		WVector velocity;
		velocity = dir * 16.5f * speed * elapsed;

		delta = WVector(m_targetPos.x - m_pos.x, 0.0f, m_targetPos.z - m_pos.z);
		if (velocity.Magnitude() >
			sqrtf(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z))
		{
			velocity = delta;
		}

		WVector newPos;
		if (this == WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
		{
			Waabb box(WVector(-1.0f, -40.0f, -1.0f), WVector(1.0f, 8.0f, 1.0f));
			SlideMove(m_pos, velocity, m_pPVS, box, newPos);
		}
		else
		{
			newPos = m_pos + velocity;
		}

		move = newPos - m_pos;
		move.y = 0.0f;

		if (move.Magnitude() > 0.01f)
		{
			GetPVS().GetGroundPoint(newPos, true, &hit, true);
			if (hit >= 0)
			{
				m_pos = newPos;
			}
			else if (this ==
				WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
			{
				move.Reset();
			}
		}
		else
		{
			move.Reset();
		}
	}

	SendMove(move, false);

	float moveSq = move.x * move.x + move.z * move.z;
	if (sqrtf(moveSq) > 0.01f)
	{
		if (m_motion != 1)
		{
			ChangeMotion(1, NULL, 0.2f);
		}
	}
	else
	{
		if (this == WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
		{
			if (m_moveDir.x != 0.0f || m_moveDir.z != 0.0f)
			{
				SendMove(WVector::ZERO, true);
			}
			m_targetPos = m_pos;
		}

		if (m_motion != 4)
		{
			switch (m_action)
			{
			case 0:
				if (m_motion != 0)
				{
					ChangeMotion(0, NULL, 0.2f);
				}
				break;
			case 1:
				if (m_motion != 2)
				{
					ChangeMotion(2, NULL, 0.2f);
				}
				break;
			case 2:
				if (m_motion != 3)
				{
					ChangeMotion(3, NULL, 0.3f);
				}
				break;
			}
		}
	}

	if (moveSq > 0.001f)
	{
		m_targetAngle = atan2f(dir.z, dir.x) + 1.5707964f;
	}
	else if (this != WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
	{
		if (m_bPendingTargetAngle)
		{
			float angle = m_pendingTargetAngle;
			m_targetAngle = RadianNormalize2PI(angle);
		}
	}

	float turn = RadianDelta(m_targetAngle, m_angle);
	if (turn > 0.0f && m_targetAngle < m_angle)
	{
		m_angle -= g_2_PI;
	}
	else if (turn < 0.0f && m_targetAngle > m_angle)
	{
		m_angle += g_2_PI;
	}

	if (fabs(turn) > 0.001f)
	{
		m_angle += elapsed * turn * 10.0f;
		if ((turn > 0.0f && m_angle > m_targetAngle) ||
			(turn < 0.0f && m_angle < m_targetAngle))
		{
			m_angle = m_targetAngle;
		}
	}

	m_pos = GetPVS().GetGroundPoint(m_pos, false, NULL, false);

	delta = g_view->camera.pivot - m_pos;

	float cameraDist = sqrtf(delta.x * delta.x + delta.z * delta.z);
	if (cameraDist > 192.0f)
	{
		m_height = 0.0f;
	}
	else if (cameraDist > 160.0f)
	{
		m_height = 1.0f - (cameraDist - 160.0f) * 0.03125f;
	}
	else
	{
		if (IsLocalContent((localContentType_t)10))
		{
			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			int guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				pDoc->m_briefUserInfoMap.find(guid);

			if ((*it).second.dwIdentity & 4)
			{
				if (!((*it).second.state & 1))
				{
					m_height = 0.1f;
				}
				else
				{
					m_height = 1.0f;
				}
			}
			else
			{
				m_height = 1.0f;
			}
		}
		else
		{
			m_height = 1.0f;
		}
	}

	WVector up;
	if (m_motion == 3)
	{
		up = GetPVS().GetMeshNormal(m_pos);
	}
	else
	{
		up = WVector::UNIT_POS_Y;
	}

	WVector axis = WCrossProduct(m_up, up);
	axis.Normalize();

	if (!(Wabs(axis.SquareMagnitude()) < g_EPSILON))
	{
		if (acosf(WDotProduct(up, m_up)) < elapsed * g_2_PI)
		{
			m_up = up;
		}
		else
		{
			WMatrix rot = RotMat(axis, elapsed * -6.2831855f);
			m_up = RotVec(m_up, rot);
		}
	}

	WMatrix mat;
	MakeUpRotation(mat, m_up);

	mat = RotMat(m_angle, 1) * mat;

	float scale = m_spItem[0].getvalue();
	mat.pivot = m_pos;
	{
		WVector axisScale(scale, scale, scale);
		mat.AxisScale(axisScale);
	}

	if (m_pPetFrame)
	{
		m_pPetFrame->ApplyBones(&m_bones, mat);
		m_pPetFrame->Transform(g_view, 1.0f, 0);
		m_pPetFrame->UpdateBound(true);
		m_pPetFrame->SetAlpha((unsigned char)(m_height * 255.0f));
		m_pPetFrame->GetBonePet()->ScaleBone("Bip01 Head",
			m_spItem[1].getvalue());
	}

	if (m_facialTime > 0.0f)
	{
		m_facialTime -= elapsed;
		if (m_facialTime < 0.0f)
		{
			m_facialTime = 0.0f;
			SetPendingTexPart("face",
				s_faceTextures[m_charTypeId & 0x3FFFFFF][3]);
		}
	}

	m_balloonTime += elapsed;
	if (m_balloonTime > 5.0f)
	{
		WSingleton<ntBalloonTextManager>::Instance()->DeleteBalloonText(
			m_pBalloon);
		m_pBalloon = NULL;
	}

	m_fxBox.ProcessAttacher(elapsed * 1000.0f);
	m_fxBox.Process(elapsed * 1000.0f);

	m_blendElapsed += elapsed;
	m_motionTime += elapsed;
}

void ntAvatar::Render()
{
	if (IsLocalContent((localContentType_t)10))
	{
		if (this != WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar &&
			m_bGM)
		{
			CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
			int guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::iterator it;
			it = pDoc->m_briefUserInfoMap.find(guid);

			if (!(sBriefUserInfo((*it).second).state & 1))
			{
				if (guid != MyGuid(false))
				{
					return;
				}
			}
		}
	}

	WVector pos;
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		pos = GetPVS().GetGroundPoint(GetPosition(), false, NULL, false);
	}
	else
	{
		pos = m_pPetFrame->GetBonePet()
				  ->FindBone("Bip01 Pelvis")
				  ->GetMatrix()
				  .pivot;
		pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	}

	if (COption::Instance()->vIsShadowEnabled())
	{
		WVector xAxis = WVector(2.9f, 0.0f, 0.0f) * m_spItem[0].getvalue();
		WVector zAxis = WVector(0.0f, 0.0f, 2.9f) * m_spItem[0].getvalue();

		GetPVS().DrawDecal(m_pMain->m_shadowTex, pos, xAxis, zAxis, 1.0f,
			m_height, W_POLY_BLEND_NORMAL, false);
	}

	if (m_pPetFrame != NULL && m_pPetFrame->GetCurAniPet() != NULL)
	{
		if (g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox()))
		{
			if (WSingleton<ntAvatarManager>::Instance()->m_pPickedAvatar ==
				this)
			{
				m_light.nearone = g_lightset.nearone;
				m_light.diffuse = m_light.ambient = 0xFFFFFFFF;
			}
			else
			{
				m_light.nearone = g_lightset.nearone;
				m_light.ambient = ModifyColor(g_lightset.ambient, 1.0f);
				m_light.diffuse = ModifyColor(g_lightset.diffuse,
					GetPVS().GetShadowIntensity(pos));
			}

			m_pPetFrame->UpdateLightSource(&m_light, false, NULL);

			m_pPetFrame->Render(g_view, true, 1.0f, false, false, false);
		}
	}

	g_view->Flush(1);

	m_fxBox.Display(g_view);
}

void ntAvatar::RenderOverlay(bool detail)
{
	if (m_pBalloon != NULL)
	{
		m_pBalloon->SetTransparency(m_height);
	}

	if (GOLFDOC()->GameMode() != 0x200 && Wabs(m_height) < g_EPSILON)
	{
		return;
	}

	float alpha;
	if (GOLFDOC()->GameMode() == 0x200)
	{
		alpha = 1.0f;
	}
	else
	{
		alpha = m_height;
	}

	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		return;
	}

	if (!g_view->InFrustum(m_pPetFrame->GetBonePet()->GetBoundBox()))
	{
		return;
	}

	WVector head =
		m_pPetFrame->GetBonePet()->FindBone("Bip01 HeadNub")->GetMatrix().pivot;

	WVector screen = g_view->Projection(head);

	if (screen.z <= 0.044f || screen.z >= 1.0f)
	{
		return;
	}

	screen.x = floorf(screen.x);
	screen.y = floorf(screen.y);

	if (IsLocalContent((localContentType_t)10))
	{
		if (m_guid != MyGuid(false))
		{
			unsigned long guid = m_guid;
			std::map<unsigned long, sBriefUserInfo>::iterator it =
				WSingleton<CSharedDoc>::Instance()->m_briefUserInfoMap.find(
					guid);
			sBriefUserInfo& info = (*it).second;

			if (info.IsIdentity(4))
			{
				if (!(info.state & 1))
				{
					return;
				}
			}
		}
	}

	if (WSingleton<CProjectG>::Instance()->HidePrivacy())
	{
		return;
	}

	float y = screen.y - 4.0f;
	FrEmoticon* pEmoticon = g_pFresh->GetManager()->GetEmoticon();
	y -= 22.0f;
	FrGraphicInterface* pGDI = g_pFresh->GetManager()->GetGDI();

	WOverlay* pIcon;
	if (m_bAngelWing)
	{
		pIcon = m_pMain->m_pAngelIcon[m_mark % 2];
	}
	else if (m_bGoodPlayer)
	{
		pIcon = m_pMain->m_pMannerIcon[m_mark % 2];
	}
	else
	{
		pIcon = m_pMain->m_pIcon[m_mark];
	}

	float x = screen.x -
		(pIcon->GetWidth() + pEmoticon->GetTextWidth(m_nickName) +
			(float)(m_pEmblem != NULL ? m_pEmblem->Width() : 0)) *
			0.5f;

	WRect iconRect;
	unsigned long alphaBits;
	unsigned long color;
	{
		_WRECT iconSrc = { 0.0f, 0.0f, 1.0f, 1.0f };
		iconRect = WRect(x, y - 7.0f, (float)pIcon->GetWidth(),
			(float)pIcon->GetHeight());
		x += pIcon->GetWidth();
		alphaBits = (unsigned char)(int)(alpha * 255.0f) << 24;
		color = alphaBits | 0xFFFFFF;
		pIcon->Render(g_view, iconSrc, iconRect, 0, color, 0.0f, 0);
	}

	if (m_pEmblem != NULL)
	{
		iconRect = WRect(x + 0.5f / m_pEmblem->Width(),
			y + 0.5f / m_pEmblem->Height() - 5.0f, (float)m_pEmblem->Width(),
			(float)m_pEmblem->Height());
		pGDI->DrawTexture(m_pEmblem, iconRect, color, 0);
		x += m_pEmblem->Width();
	}

	pGDI->SetScale(1.0f);
	pGDI->SetTextStyle(2);
	if (WSingleton<ntAvatarManager>::Instance()->m_pPickedAvatar == this)
	{
		pGDI->SetTextColor(alphaBits, color);
	}
	else
	{
		pGDI->SetTextColor(color, alphaBits);
	}

	pEmoticon->SetAnim(false);
	pEmoticon->PrintText(WPoint(x, y), 0, m_nickName, color);

	if (m_headIcon != -1)
	{
		int column = m_headIcon % 4;
		int row = m_headIcon % 16 / 4;

		_WRECT iconUV;
		iconUV.x = column * 0.25f;
		iconUV.y = row * 0.25f;
		iconUV.w = 0.25f;
		iconUV.h = 0.25f;
		_WRECT headRect = { screen.x - 64.0f - 15.0f, screen.y - 64.0f + 20.0f,
			64.0f, 64.0f };
		m_pMain->m_pEmoticon->Render(g_view, iconUV, headRect, 0,
			(unsigned char)(int)(m_height * 255.0f) << 24 | 0xFFFFFF, 0.0f, 0);
	}

	if (!detail)
	{
		return;
	}

	if ((m_state & 0xFE) == 0)
	{
		return;
	}

	WOverlay* pStateIcon = NULL;
	if (m_state & 0x80)
	{
		y -= 22.0f;

		int width = (int)pGDI->GetTextExtend(m_tradeTitle) + 10;
		if (width < 100)
		{
			width = 100;
		}

		tagRECT bar;
		bar.left = (int)(screen.x - width / 2);
		bar.right = width;
		bar.bottom = 29;
		bar.top = (int)(y - 8.0f);
		WOverlay::DrawFrameOverlay9(g_view, m_pMain->m_pStateBar, bar,
			WSingleton<ntAvatarManager>::Instance()->m_pClickedAvatar == this
				? 0xFF808080
				: 0xFFFFFFFF);

		unsigned long oldColor = pGDI->GetTextColor();
		pGDI->SetScale(1.0f);
		pGDI->SetTextStyle(
			WSingleton<ntAvatarManager>::Instance()->m_pClickedAvatar == this
				? 2
				: 0);
		unsigned long titleAlpha = (unsigned char)(int)(m_height * 255.0f)
			<< 24;
		pGDI->SetTextColor(titleAlpha | 0xFFFFFF, titleAlpha);

		pEmoticon->SetAnim(true);
		pEmoticon->PrintText(WPoint(screen.x, y), 1, m_tradeTitle,
			(unsigned char)(int)(m_height * 255.0f) << 24 | 0xFFFFFF);

		pGDI->SetTextColor(oldColor, 0xFFFFFFFF);

		pStateIcon = m_pMain->m_pStateIconN1;
	}
	else if (!(m_state & 0x8000))
	{
		if (m_state & 0x7E)
		{
			if (m_state & 0x02)
			{
				pStateIcon = m_pMain->m_pStateIcon1;
			}
			else if (m_state & 0x04)
			{
				pStateIcon = m_pMain->m_pStateIcon2;
			}
			else if (m_state & 0x08)
			{
				pStateIcon = m_pMain->m_pStateIcon3;
			}
			else if (m_state & 0x10)
			{
				pStateIcon = m_pMain->m_pStateIcon4;
			}
			else if (m_state & 0x20)
			{
				pStateIcon = m_pMain->m_pStateIcon5;
			}
			else if (m_state & 0x40)
			{
				pStateIcon = m_pMain->m_pStateIcon6;
			}
		}
	}

	if (pStateIcon != NULL)
	{
		WRect stateRect(screen.x - pStateIcon->GetWidth() / 2,
			y - pStateIcon->GetHeight() / 2 - 36.0f, pStateIcon->GetWidth(),
			pStateIcon->GetHeight());
		_WRECT stateUV = { 0.5f / pStateIcon->GetWidth(),
			0.5f / pStateIcon->GetHeight(), 1.0f, 1.0f };
		pStateIcon->Render(g_view, stateUV, stateRect, 0,
			(unsigned char)(int)(m_height * 255.0f) << 24 | 0xFFFFFF, 0.0f, 0);
	}
}

void ntAvatar::RefreshUccClothes(unsigned long tid, unsigned long id)
{
	if (m_pPetFrame == NULL)
	{
		return;
	}

	if (!IsLocalContent((localContentType_t)0x5F))
	{
		return;
	}

	m_pPetFrame->SetPart(&m_partTidList, tid, id);
	m_pPetFrame->SetPart(&m_partTidList, tid, id);

	m_pPetFrame->GetBonePet()->xUpdateTnLBuffers(true);
}

void ntAvatar::ScriptProcess(const w_motion_data* data, float from, float to,
	bool all)
{
	if ((!(from <= data->e) || from == to) && !all)
	{
		return;
	}

	int start, end;
	from *= 30.0f;
	if (all)
	{
		start = Ceil(from, 0.03f);
		end = start;
	}
	else
	{
		to = Min(data->e, to) * 30.0f;
		start = Ceil(from, 0.03f);
		end = Wabs(floorf(to) - to) < g_EPSILON ? (int)to - 1 : (int)floor(to);

		if (start == (int)floor(data->s * 30.0f + 0.5f))
		{
			start++;
		}

		if (start > end)
		{
			return;
		}
	}

	if (!m_pPetFrame)
	{
		return;
	}

	WPuppet* pPuppet = m_pPetFrame->GetCurAniPet();

	if (pPuppet)
	{
		int first, last;
		if (pPuppet->CheckFrameData(start, end, first, last))
		{
			for (int i = first; i <= last; i++)
			{
				ParsePuppetScript(pPuppet->GetFrameData(i));
			}
		}
	}
}

void ntAvatar::ParsePuppetScript(const char* script)
{
	static char s_name[64];
	static char s_command[1024];

	if (script == NULL)
	{
		return;
	}

	const char* p = script;

	while (p)
	{
		p = strchr(p, '*');

		if (p == NULL)
		{
			return;
		}

		int len = 0;

		while (p[len] != '\0')
		{
			if (p[++len] == ' ')
			{
				break;
			}

			if (p[len] == '(')
			{
				len++;
				break;
			}
		}

		if (len <= 1)
		{
			return;
		}

		strncpy(s_command, p, len);
		p += len;
		s_command[len] = '\0';

		if (strcmp(s_command, "*fx(") == 0)
		{
			char token[64];
			char fxName[64];
			char argument[64];
			char boneName[64];

			token[0] = '\0';
			p = GetStrConst(p, token);

			if (strlen(token) == 0 || strlen(token) >= sizeof(token))
			{
				return;
			}

			if (token[0] == '@')
			{
				if (m_pMotionData == NULL ||
					strcmp(token + 1, m_pMotionData->name) != 0)
				{
					return;
				}

				p = GetStrConst(p, fxName);

				if (strlen(fxName) == 0 || strlen(fxName) >= sizeof(fxName))
				{
					return;
				}
			}
			else
			{
				strcpy(fxName, token);
			}

			p = GetStrConst(p, argument);

			int argumentLen = strlen(argument);
			if (argumentLen == 0 || argumentLen >= 64)
			{
				return;
			}

			strcpy(boneName, argument);

			if (strlen(boneName) == 0 || strlen(boneName) >= sizeof(boneName))
			{
				return;
			}

			Attacher attacher;

			unsigned int i;
			for (i = 0; i < 4; i++)
			{
				if (i != 0)
				{
					p = GetStrConst(p, boneName);

					if (strlen(boneName) == 0 ||
						strlen(boneName) >= sizeof(boneName))
					{
						break;
					}
				}

				WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);

				if (pBone)
				{
					attacher.mat[i] = &pBone->GetMatrix();
				}
				else
				{
					w_bound_box* pBox =
						m_pPetFrame->GetBonePet()->FindBoundBox(boneName);

					if (pBox)
					{
						if (pBox->bone)
						{
							attacher.mat[i] = &pBox->bone->GetMatrix();
						}
						attacher.pos[i] = &pBox->info->spherePivot;
					}
				}
			}

			if ((int)i > 0)
			{
				AttachFx(attacher, fxName, true, NULL);
			}
		}
		else if (strcmp(s_command, "*hidebone(") == 0 ||
			strcmp(s_command, "*showbone(") == 0)
		{
			char boneName[64] = "";

			for (int i = 0; i < 64; i++)
			{
				p = GetStrConst(p, boneName);

				if (strlen(boneName) == 0 ||
					strlen(boneName) >= sizeof(boneName))
				{
					break;
				}

				WBone* pBone = m_pPetFrame->GetBonePet()->FindBone(boneName);

				if (pBone)
				{
					pBone->SetInvisible(false, s_command[1] == 'h');
				}

				if (p == NULL || *p == ')')
				{
					break;
				}
			}
		}
		else if (strcmp(s_command, "*snd(") == 0)
		{
			p = GetStrConst(p, s_name);

			if (strlen(s_name) == 0 || strlen(s_name) >= sizeof(s_name))
			{
				return;
			}

			WVector toCamera = m_pos - g_camera.pivot;
			if (toCamera.Magnitude() < 192.0f)
			{
				g_audio->PlaySfx(s_name, &toCamera, 5, NULL, NULL, 0.5f,
					200.0f);
			}
		}
		else if (strcmp(s_command, "*stepsnd(") == 0)
		{
			if (IsLocalContent((localContentType_t)10))
			{
				if (this !=
					WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar)
				{
					CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
					int guid = m_guid;
					std::map<unsigned long, sBriefUserInfo>::iterator it;
					it = pDoc->m_briefUserInfoMap.find(guid);

					if (m_bGM)
					{
						if (!(sBriefUserInfo((*it).second).state & 1))
						{
							continue;
						}
					}
				}

				const char* sound = GetPVS().GetStepSound(
					GetPVS().GetGroundPoint(m_pos, false, NULL, false));

				if (sound)
				{
					WVector toCamera = m_pos - g_camera.pivot;
					if (toCamera.Magnitude() < 192.0f)
					{
						g_audio->PlaySfx(sound, &toCamera, 5, sound, NULL, 0.5f,
							200.0f);
					}
				}
			}
			else
			{
				const char* sound = GetPVS().GetStepSound(
					GetPVS().GetGroundPoint(m_pos, false, NULL, false));

				if (sound)
				{
					WVector toCamera = m_pos - g_camera.pivot;
					if (toCamera.Magnitude() < 192.0f)
					{
						g_audio->PlaySfx(sound, &toCamera, 5, sound, NULL, 0.5f,
							200.0f);
					}
				}
			}
		}
		else if (strcmp(s_command, "*ptex(") == 0)
		{
			char token[64];
			char texture[64];

			token[0] = '\0';
			p = GetStrConst(p, token);

			if (strlen(token) == 0 || strlen(token) >= sizeof(token))
			{
				return;
			}

			if (token[0] == '@')
			{
				if (m_pMotionData == NULL ||
					strcmpi(token + 1, m_pMotionData->name) != 0)
				{
					return;
				}

				p = GetStrConst(p, s_name);

				if (strlen(s_name) == 0 || strlen(s_name) >= sizeof(s_name))
				{
					return;
				}
			}
			else
			{
				strcpy(s_name, token);
			}

			p = GetStrConst(p, texture);

			if (strlen(texture) == 0 || strlen(texture) >= sizeof(texture))
			{
				return;
			}

			if (m_facialTime > 0.0f)
			{
				return;
			}

			SetPendingTexPart(s_name, texture);
		}
		else if (stricmp(s_command, "*showvis(") == 0)
		{
			ParseScript_ShowVis(true, p);
		}
		else if (stricmp(s_command, "*hidevis(") == 0)
		{
			ParseScript_ShowVis(false, p);
		}
	}
}

void ntAvatar::ParseScript_ShowVis(bool show, const char* script)
{
	char token[0x100] = "";
	int count = 0;

	do
	{
		script = GetStrConst(script, token);

		if (strlen(token) == 0 || strlen(token) >= sizeof(token))
		{
			break;
		}

		std::list<std::string>* pGroup =
			WSingleton<CSharedDoc>::Instance()->GetVisGroup(token);

		if (pGroup)
		{
			for (std::list<std::string>::iterator it = pGroup->begin();
				it != pGroup->end(); ++it)
			{
				WBone* pBone =
					m_pPetFrame->GetBonePet()->FindBone((*it).c_str());

				if (pBone)
				{
					pBone->SetInvisible(false, !show);
				}
			}
		}

		if (script == NULL || *script == ')')
		{
			break;
		}
	} while (++count < 64);
}

sTexturePart* ntAvatar::FindTexturePart(const char* part, const char* texture)
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

void ntAvatar::SetPendingTexPart(const char* part, const char* texture)
{
	if (m_pPetFrame == NULL || m_pPetFrame->GetCurAniPet() == NULL)
	{
		return;
	}

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

void ntAvatar::SetChatFacial(int facial)
{
	SetPendingTexPart("face", s_faceTextures[m_charTypeId & 0x3FFFFFF][facial]);

	m_facialTime = 5.0f;
}

void ntAvatar::SetBalloonText(const char* text)
{
	if (m_pBalloon == NULL)
	{
		m_pBalloon =
			WSingleton<ntBalloonTextManager>::Instance()->CreateBalloonText(
				m_guid, text, m_balloonColor, 5.0f);
	}
	else
	{
		m_pBalloon->SetText(text);
	}

	m_balloonColor[3] = m_height;
	m_balloonTime = 0;
}

void ntAvatar::SetTradeTitle(const char* title)
{
	if (title)
	{
		strcpy(m_tradeTitle, title);
	}
	else
	{
		m_tradeTitle[0] = '\0';
	}
}

SpecialPrizeItem& ntAvatar::GetMySpItem(eSPAVILITYTYPE type)
{
	return m_spItem[type];
}

void ntAvatar::UseSPItem(eSPAVILITYTYPE type)
{
	WSendPacket packet(0x0c);
	packet.Encode1(6);
	packet.Encode4(
		WSingleton<ntAvatarManager>::Instance()->m_pMyAvatar->GetGuid());
	packet.Encode4(type);
	packet.Send(TO_GAME);
}

ntAvatarManager::ntAvatarManager()
{
	m_pPVS = NULL;
	m_pPVS = &::GetPVS();

	m_avatarCount = 0;
	for (int i = 0; i < 30; i++)
	{
		m_avatar[i] = NULL;
	}

	m_pMyAvatar = NULL;
	m_pPickedAvatar = NULL;
	m_pClickedAvatar = NULL;
	m_bActive = true;
	m_bShowName = true;
}

ntAvatarManager::~ntAvatarManager()
{
	FreeAllAvatars();
}

void ntAvatarManager::FreeAllAvatars()
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

ntAvatar* ntAvatarManager::FindAvatarByIDName(const char* idName) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (strcmp(m_avatar[i]->m_idName, idName) == 0)
			{
				return m_avatar[i];
			}
		}
	}

	return NULL;
}

ntAvatar* ntAvatarManager::FindAvatarByNickName(const char* nickName) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (strcmp(m_avatar[i]->m_nickName, nickName) == 0)
			{
				return m_avatar[i];
			}
		}
	}

	return NULL;
}

ntAvatar* ntAvatarManager::FindAvatarByGuid(int guid) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (m_avatar[i]->m_guid == guid)
			{
				return m_avatar[i];
			}
		}
	}

	return NULL;
}

ntAvatar* ntAvatarManager::CreateAvatar(const sSlotInfo& slotInfo,
	const sCharacterInfo& charInfo, ntAvatarChatMain* pChatMain)
{
	int i;
	for (i = 0; i < 30; i++)
	{
		if (!m_avatar[i])
		{
			break;
		}
	}

	if (i == 30)
	{
		return NULL;
	}

	m_avatarCount++;

	ntAvatar* pAvatar = new ntAvatar(GetPVS(), slotInfo, charInfo, pChatMain);

	WVector pos(slotInfo.location[0], 0.0f, slotInfo.location[1]);
	pos = GetPVS().GetGroundPoint(pos, false, NULL, false);
	pAvatar->SetPosition(pos);

	float angle = slotInfo.location[2];
	if (angle >= g_2_PI || angle < 0.0f)
	{
		angle -= (float)floor((double)(angle * 0.15915494f)) * g_2_PI;
	}
	pAvatar->m_angle = angle;
	pAvatar->m_targetAngle = angle;

	m_avatar[i] = pAvatar;
	return pAvatar;
}

bool ntAvatarManager::DeleteAvatar(const char* idName)
{
	int i;
	for (i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (strcmp(m_avatar[i]->m_idName, idName) == 0)
			{
				break;
			}
		}
	}

	if (i == 30)
	{
		return false;
	}

	if (m_avatar[i])
	{
		delete m_avatar[i];
		m_avatar[i] = NULL;
	}
	m_avatarCount--;

	return true;
}

ntAvatar* ntAvatarManager::AvatarByRayIntersect(const WVector& pos,
	const WVector& dir) const
{
	Waabb aabb;
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			if (IsLocalContent((localContentType_t)10))
			{
				if (m_avatar[i]->m_guid != MyGuid(false))
				{
					CSharedDoc* pDoc = WSingleton<CSharedDoc>::Instance();
					std::map<unsigned long, sBriefUserInfo>::const_iterator it =
						pDoc->m_briefUserInfoMap.find(m_avatar[i]->GetGuid());

					if (!IsAdministrator(0xFFFFFFFF))
					{
						if (sBriefUserInfo((*it).second).IsIdentity(4))
						{
							if (!(sBriefUserInfo((*it).second).state & 1))
							{
								break;
							}
						}
					}
				}
			}

			aabb.min = WVector(-1.0f, 0.0f, -1.0f);
			aabb.max = WVector(1.0f, 8.0f, 1.0f);
			aabb.min += m_avatar[i]->m_pos;
			aabb.max += m_avatar[i]->m_pos;

			if (RayIntersectAABB(aabb, pos, dir, NULL, 0.0f) < 1.0f)
			{
				return m_avatar[i];
			}
		}
	}

	return NULL;
}

ntAvatar* ntAvatarManager::CheckClick(float x, float y) const
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i] && m_avatar[i]->IsClickable(x, y))
		{
			return m_avatar[i];
		}
	}

	return NULL;
}

void ntAvatarManager::Process(float elapsed)
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			m_avatar[i]->Process(elapsed);
		}
	}
}

void ntAvatarManager::Render()
{
	if (!m_bActive)
	{
		return;
	}

	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			m_avatar[i]->Render();
		}
	}
}

void ntAvatarManager::ResetAvatarPositionAngle()
{
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			m_avatar[i]->m_pos = m_avatar[i]->m_targetPos;
			m_avatar[i]->m_angle = m_avatar[i]->m_targetAngle;
		}
	}
}

CQuadTree& ntAvatarManager::GetPVS()
{
	return *m_pPVS;
}

void ntAvatarManager::SetLocalPVS(CQuadTree& pvs)
{
	m_pPVS = &pvs;
}

static int __cdecl CompareCameraDistance(const void* a, const void* b)
{
	WVector delta = g_view->GetCamera().pivot;
	WVector deltaA;
	deltaA = delta - (*(ntAvatar* const*)a)->m_pos;
	delta = delta - (*(ntAvatar* const*)b)->m_pos;

	float distA =
		deltaA.x * deltaA.x + deltaA.y * deltaA.y + deltaA.z * deltaA.z;
	float distB = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;

	if (distA > distB)
	{
		return -1;
	}

	if (distA < distB)
	{
		return 1;
	}

	return 0;
}

void ntAvatarManager::RenderOverlay()
{
	if (!m_bActive)
	{
		return;
	}

	ntAvatar* avatars[30];
	int count = 0;
	for (int i = 0; i < 30; i++)
	{
		if (m_avatar[i])
		{
			avatars[count++] = m_avatar[i];
		}
	}

	qsort(avatars, count, sizeof(ntAvatar*), CompareCameraDistance);

	for (int j = 0; j < count; j++)
	{
		if (avatars[j])
		{
			avatars[j]->RenderOverlay(m_bShowName);
		}
	}
}

ntAvatarBurden::ntAvatarBurden(unsigned long first, unsigned long key,
	const sCharacterInfo& charInfo, CPetFrame* pPetFrame)
	: m_charInfo()
{
	m_uid = first;
	m_guid = key;

	m_charInfo = charInfo;
	m_pPetFrame = pPetFrame;
}

ntAvatarBurden::~ntAvatarBurden()
{
}

bool ntAvatarBurden::Load()
{
	if (!m_partTidList.SetTids(&m_charInfo, 0xFF))
	{
		m_partTidList.SetDefaultTids();
	}

	if (!m_partTidList.IsComboValid())
	{
		m_partTidList.SetDefaultTids();
	}

	if (m_pPetFrame)
	{
		delete m_pPetFrame;
		m_pPetFrame = NULL;
	}

	m_pPetFrame = new CPetFrame;
	if (!m_pPetFrame->Build(&m_partTidList, m_charInfo.tidAuxParts, true, true,
			m_guid))
	{
		delete m_pPetFrame;
		m_pPetFrame = NULL;
		return false;
	}

	m_pPetFrame->SetHairClr(&m_partTidList, m_charInfo.hairClr);
	m_pPetFrame->SetShirtsClr(&m_partTidList, m_charInfo.shirtsClr & 0xF);

	return true;
}
