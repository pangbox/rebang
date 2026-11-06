#include "stdafx.h"
#include "exhibition.h"
#include "partsinfo.h"
#include "texturepart.h"
#include "fxsequence.h"
#include "fxspray.h"
#include "inputmanager.h"
#include "mousecursor.h"
#include "bongdarishopdlg.h"
#include "soundmanager.h"
#include "wbone.h"
#include "wresrcmng.h"
#include "taskmanager.h"
#include "realmyroomtask.h"
#include "mascot.h"
#include "fresh.h"
#include "frwndmanager.h"
#include "frdesktop.h"
#include "commonutil.h"
#include "../../shared/localize.h"
#include "mathconsts.h"
#include <algorithm>

extern Fresh* g_pFresh;
extern WMatrix g_camera;

const char* __fastcall GetStrConst(const char* str, char* out);
const char* GetCharsTillSpace(const char* str, char* out);

static const int s_charMotionTable[10][12] = {
	{ 0, 1, 3, 5, 6, -1 },
	{ 0, 1, 3, 5, 6, -1 },
	{ 0, 1, 3, 5, 6, -1 },
	{ 0, 1, 3, 5, 6, -1 },
	{ 0, 3, 1, 5, 6, -1 },
	{ 0, 3, 5, 6, -1 },
	{ 0, 6, 3, -1 },
	{ 0, 4, 4, 0 },
	{ 0, 3, 5, 7, 10, -1 },
	{ 0, 3, 5, 7, 10, -1 },
};

static const int s_caddieMotionTable[28][9] = {
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
	{ 0, 3, 4, 6, 7, -1 },
};

static char* s_charMotions[12] = {
	"\xb1\xe2\xba\xbb\xc0\xda\xbc\xbc",
	"\xbc\xbc\xc0\xcc\xba\xea\xc6\xc4\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xba\xb8\xb1\xe2\xbd\xc7\xb0\xdd\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"1\xb5\xee\xb8\xf0\xbc\xc7",
	"2\xb5\xee\xb8\xf0\xbc\xc7",
	"3\xb5\xee\xb8\xf0\xbc\xc7",
	"\xc5\xb8\xc0\xd3\xbf\xc0\xb9\xf6\xba\xa1\xc4\xbfOB\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xc6\xdb\xc6\xc3\xbc\xba\xb0\xf8",
	"\xc6\xdb\xc6\xc3\xbd\xc7\xc6\xd0",
	"\xc6\xdb\xc6\xc3\xc8\xc4\xbd\xc7\xb8\xc1\xc6\xf7\xc1\xee",
	"\xc0\xcc\xb1\xdb\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
	"\xbe\xcb\xb9\xd9\xc8\xa6\xc0\xce\xbd\xc2\xb8\xae\xc6\xf7\xc1\xee",
};

static char* s_caddieMotions[10] = {
	"\xb1\xe2\xba\xbb",
	"\xb1\xe2\xb4\xd9\xb8\xae\xb1\xe2",
	"\xb3\xf4\xc0\xba\xb0\xf8\xc7\xc7\xc7\xcf\xb1\xe2",
	"\xba\xa1\xc4\xbf",
	"\xc0\xe7\xc3\xcb\xc7\xcf\xb1\xe2",
	"\xbd\xc7\xb0\xdd",
	"\xc2\xf9\xbd\xba",
	"\xc8\xa6\xc0\xce",
	"\xc8\xa6\xc0\xce\xbf\xf8",
};

static char* s_chatFacials[10][4] = {
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

CExhibition::CExhibition(float x, float y, float anchor, bool autoRecover)
	: m_delay(0.f),
	  m_motionLength(0.f),
	  m_bLButtonDown(false),
	  m_bRButtonDown(false),
	  m_curDist(20.f),
	  m_pitchBias(0.f),
	  m_motionTime(0.f),
	  m_pMotion(NULL),
	  m_pPetFrame(NULL),
	  m_pModel(NULL),
	  m_motionIndex(-1),
	  m_charIndex(-1),
	  m_caddieIndex(-1),
	  m_bChatFacial(false),
	  m_bUnused(true)
{
	m_center.x = x;
	m_center.y = y;
	m_bEnableZoom = true;
	m_bEnableRotate = true;
	m_bAutoRecover = autoRecover;
	m_anchor = anchor;
	Init();
	m_fxBox.Init(g_resrcmng);
	m_fxBox.SetCamera(&g_camera);
	m_pWingR = NULL;
	m_pWingL = NULL;
	m_fxBox.m_frameTime = 33.333332f;
}

CExhibition::~CExhibition()
{
	ReleasePet();
	m_fxBox.ClearAll(false);
	for (std::list<sTexturePart*>::iterator it = m_texParts.begin();
		it != m_texParts.end(); ++it)
		delete *it;
	m_texParts.clear();
}

void CExhibition::ReleasePet()
{
	if (m_pModel)
		m_pModel->LockBoneSetPos(0xff);
	if (m_type == EXHIBIT_CHARACTER)
	{
		if (m_pPetFrame)
		{
			delete m_pPetFrame;
			m_pPetFrame = NULL;
		}
		m_pModel = NULL;
	}
	else
	{
		if (g_resrcmng && m_pModel)
		{
			g_resrcmng->Release(m_pModel, false);
			m_pModel = NULL;
		}
	}
	if (g_resrcmng && m_msgTexture)
	{
		g_resrcmng->Release(m_msgTexture);
		m_msgTexture = 0;
	}
}

void CExhibition::Init()
{
	m_angleVel.Reset();
	m_defaultAngle.Reset();
	m_angle.Reset();
	m_prevYaw = 0.0f;
	m_dirTime = 0.0f;
	m_facialTime = 0.0f;
	m_dist = m_curDist;
	m_defDist = m_curDist;
	m_reserved = 0;
	m_minDist = 15.0f;
	m_maxDist = 25.0f;
	m_typeId = 0;
	m_pAuxParts = NULL;
	m_controlFlag.Set(6);
	m_bNextMotion = false;
	UpdateCamera(0.0f);
	m_recoverState = 2;
	m_modelMat.Reset();
	m_msgTexture = 0;
	m_light.diffuse = 0x969696;
	m_light.type = 2;
}

void CExhibition::SetDistance(float dist, bool force, bool setView)
{
	m_dist = Between(m_minDist, dist, m_maxDist);
	if (force)
	{
		m_dist = dist;
		m_curDist = dist;
	}
	UpdateCamera(0.f);
	if (g_view && (!IS_EXACTKINDOF(CRealMyRoomTask, AfxGetTask()) || setView))
	{
		g_view->SetCamera(m_camMat);
		g_view->UpdateCamera();
	}
}

float CExhibition::GetDistance()
{
	return m_dist;
}

void CExhibition::Zoom(float delta)
{
	SetDistance(delta + m_dist, false, false);
	m_recoverState = 1;
	m_recoverTime = 0.0f;
}

void CExhibition::UpdateCamera(float dt)
{
	float diff = m_curDist - m_dist;
	float step = dt * 15.f;
	if (diff > step)
		m_curDist -= step;
	else if (diff < dt * -15.f)
		m_curDist += step;
	else
		m_curDist = m_dist;
	m_camMat = RotMat(m_angle.x, 0) * RotMat(m_angle.y, 1);
	float depth = -m_curDist;
	m_camMat.pivot = m_camMat.za * depth;
	if (m_controlFlag.GetFlag(0x10) && m_pPetFrame)
	{
		WBone* bone = m_pPetFrame->GetBonePet()->FindBone("Bip01");
		if (bone)
		{
			m_camMat.pivot.x += bone->GetMatrix().pivot.x;
			m_camMat.pivot.z += bone->GetMatrix().pivot.z;
		}
	}
}

const char* CExhibition::GetMotionName()
{
	if (m_pMotion)
		return m_pMotion->name;
	return NULL;
}

void CExhibition::Process(float deltaTime, bool bPaused)
{
	if (!m_pModel)
		return;
	if (!m_pModel->GetRootBone())
		return;
	if (m_alpha < 255.0f)
	{
		m_alpha = 255.0f;
		SetAlpha(0xff);
	}
	int state = g_input->GetButton(RIGHT_BUTTON);
	if (state == 1)
	{
		if (InValidArea())
			m_bRButtonDown = true;
	}
	else if (state == 2)
		m_bRButtonDown = false;
	state = g_input->GetButton(LEFT_BUTTON);
	if (state == 1)
	{
		if (InValidArea())
			m_bLButtonDown = true;
	}
	else if (state == 2)
		m_bLButtonDown = false;
	m_bNextMotion = false;
	if (!m_controlFlag.GetFlag(8))
	{
rotate_check:
		if (!m_bLButtonDown || !m_bEnableRotate)
		{
zoom_check:
			if (!m_bRButtonDown || !m_bEnableZoom)
			{
				if (m_bAutoRecover)
				{
					float old = m_recoverTime;
					m_recoverTime = deltaTime + old;
					if (m_recoverTime > 10.0f && m_recoverState == 1)
					{
						float dp = m_defaultAngle.x -
							GetBaseAngle(m_angle.x, m_defaultAngle.x);
						float dy = m_defaultAngle.y -
							GetBaseAngle(m_angle.y, m_defaultAngle.y);
						if (WisZero(dp) && WisZero(dy))
						{
							m_angleVel.Reset();
							m_recoverState = 2;
						}
						else
						{
							float sum = Abs<float>(dy) + Abs<float>(dp);
							m_recoverState = 0;
							m_angleVel.x = dp * (1.f / sum) * g_PI;
							m_angleVel.y = dy * (1.f / sum) * g_PI;
						}
						m_dist = m_defDist;
					}
				}
			}
			else if (bPaused && m_controlFlag.GetFlag(1))
			{
				Zoom(CInputManager::Instance()->GetMouseDelta().y * .1f);
				CMouseCursor::Instance()->SetCursor(6);
			}
		}
		else
		{
			float dp = m_angle.x -
				CInputManager::Instance()->GetMouseDelta().y * .01f -
				m_defaultAngle.x;
			if (m_controlFlag.GetFlag(4) &&
				dp < m_pitchBias + g_DEGTORAD * 15.f && dp > g_DEGTORAD * -60.f)
				m_angle.x -=
					CInputManager::Instance()->GetMouseDelta().y * .01f;
			if (m_controlFlag.GetFlag(2))
				m_angle.y -=
					CInputManager::Instance()->GetMouseDelta().x * .01f;
			if (m_controlFlag.GetFlag(6))
			{
				m_recoverState = 1;
				m_recoverTime = 0.0f;
			}
		}
	}
	else
	{
		if (!m_bLButtonDown)
			goto zoom_check;
		if (!m_bRButtonDown || g_input->GetButton(MIDDLE_BUTTON) != 2)
			goto rotate_check;
		m_bNextMotion = NextMotion();
	}
	switch (m_recoverState)
	{
	case 0:
	{
		m_angle +=
			WVector2D(deltaTime * m_angleVel.x, deltaTime * m_angleVel.y);
		if (((m_angleVel.x < 0.0f && m_angle.x < m_defaultAngle.x) ||
				(m_angleVel.x > 0.0f && m_angle.x > m_defaultAngle.x)) ||
			((m_angleVel.y < 0.0f && m_angle.y < m_defaultAngle.y) ||
				(m_angleVel.y > 0.0f && m_angle.y > m_defaultAngle.y)))
		{
			m_angle = m_defaultAngle;
			m_angleVel.Reset();
			m_recoverState = 2;
		}
	}
	case 2:
	{
		if (m_type == EXHIBIT_BALL || m_type == EXHIBIT_CLUB ||
			m_type == EXHIBIT_MATCH || m_type == EXHIBIT_ITEM)
		{
			m_modelMat *= RotMat(deltaTime * (-60.f * g_DEGTORAD), 1);
		}
	}

	break;
	case 1:
	{
		m_angleVel.Reset();
		break;
	}
	}
	UpdateCamera(deltaTime);
	m_angle.x = GetBaseAngle(m_angle.x, m_defaultAngle.x);
	m_angle.y = GetBaseAngle(m_angle.y, m_defaultAngle.y);
	float t = deltaTime + m_dirTime;
	m_dirTime = t;
	if (t > 1.0f)
	{
		m_dirTime = t - 1.0f;
		if (!m_bDirChanged)
		{
			if (Abs<float>(GetBaseAngle(m_prevYaw, m_angle.y) - m_angle.y) >
				g_DEGTORAD)
			{
				m_prevYaw = m_angle.y;
				m_bDirChanged = true;
			}
		}
	}
	if (m_bChatFacial)
	{
		float t2 = deltaTime + m_facialTime;
		m_facialTime = t2;
		if (t2 > 2.0f)
		{
			m_facialTime = t2 - 2.0f;
			SetPendingTexPart("face", s_chatFacials[m_charIndex][3]);
			m_bChatFacial = false;
		}
	}
	m_delay -= deltaTime;
	if (m_delay > 0.0f)
		return;
	if (m_pMotion)
	{
		ScriptProcess(m_pMotion, m_motionTime, deltaTime + m_motionTime, false);
		m_motionTime += deltaTime;
		if (m_motionTime > m_pMotion->e)
		{
			SetMotion(m_pMotion->nextmotion, true, m_motionTime - m_pMotion->e);
			goto done;
		}
	}
	Animate();
done:
	float elapsed = deltaTime * 1000.0f;
	CFxBox* pBox = &m_fxBox;
	pBox->ProcessAttacher(elapsed);
	m_fxBox.Process(elapsed);
}

int CExhibition::ProcessBongdariShop(float deltaTime)
{
	if (!m_pModel)
		return 0;
	if (m_alpha < 255.0f)
	{
		m_alpha = 255.0f;
		SetAlpha(0xff);
	}
	int state = g_input->GetButton(LEFT_BUTTON);
	if (state == 1)
	{
		if (InValidArea())
			m_bLButtonDown = true;
	}
	else if (state == 2)
		m_bLButtonDown = false;
	if (m_bAutoRecover)
	{
		float old = m_recoverTime;
		m_recoverTime = deltaTime + old;
		if (m_recoverTime > 10.0f && m_recoverState == 1)
		{
			float dp =
				m_defaultAngle.x - GetBaseAngle(m_angle.x, m_defaultAngle.x);
			float dy =
				m_defaultAngle.y - GetBaseAngle(m_angle.y, m_defaultAngle.y);
			if (!WisZero(dp) || !WisZero(dy))
			{
				float a = Abs<float>(dy);
				float b = Abs<float>(dp);
				m_recoverState = 0;

				m_angleVel.x = dp * (1.f / (b + a)) * g_PI;
				m_angleVel.y = dy * (1.f / (b + a)) * g_PI;
			}
			else
			{
				m_angleVel.Reset();
				m_recoverState = 2;
			}
			m_dist = m_defDist;
		}
	}
	switch (m_recoverState)
	{
	case 0:
	{
		m_angle += deltaTime * m_angleVel;
		if (((m_angleVel.x < 0.0f && m_angle.x < m_defaultAngle.x) ||
				(m_angleVel.x > 0.0f && m_angle.x > m_defaultAngle.x)) ||
			((m_angleVel.y < 0.0f && m_angle.y < m_defaultAngle.y) ||
				(m_angleVel.y > 0.0f && m_angle.y > m_defaultAngle.y)))
		{
			m_angle = m_defaultAngle;
			m_angleVel.Reset();
			m_recoverState = 2;
		}
		break;
	}
	case 1:
	{
		m_angleVel.Reset();
	}
	}
	UpdateCamera(deltaTime);
	m_angle.x = GetBaseAngle(m_angle.x, m_defaultAngle.x);
	m_angle.y = GetBaseAngle(m_angle.y, m_defaultAngle.y);
	float t = deltaTime + m_dirTime;
	m_dirTime = t;
	if (t > 1.0f)
	{
		m_dirTime = t - 1.0f;
		if (!m_bDirChanged)
		{
			float da =
				Abs<float>(GetBaseAngle(m_prevYaw, m_angle.y) - m_angle.y);
			if (da > g_DEGTORAD)
			{
				m_prevYaw = m_angle.y;
				m_bDirChanged = true;
			}
		}
	}
	float idle = m_delay;
	m_delay = idle - deltaTime;
	if (m_delay > 0.0f)
		return 0;
	int result = 0;
	if (m_pMotion)
	{
		ScriptProcess(m_pMotion, m_motionTime, deltaTime + m_motionTime, false);
		float t3 = deltaTime + m_motionTime;
		m_motionTime = t3;
		if (t3 > m_pMotion->e)
		{
			const char* next = m_pMotion->nextmotion;
			if (strcmp(m_pMotion->name, next) != 0)
				result = (strcmp(m_pMotion->name,
							  "\xbb\xf5\xb7\xce\xbd\xc3\xc0\xdb") == 0) +
					1;
			SetMotion(next, true, t3 - m_pMotion->e);
			goto done;
		}
	}
	Animate();
done:
	float elapsed = deltaTime * 1000.0f;
	CFxBox* pBox = &m_fxBox;
	pBox->ProcessAttacher(elapsed);
	m_fxBox.Process(elapsed);
	return result;
}

void CExhibition::SetDirection(float direction)
{
	m_angle.y = direction;
}

void CExhibition::SetDefaultDirection(float direction)
{
	m_recoverState = 1;
	m_recoverTime = 10.0f;
	m_defaultAngle.y = direction;
}

bool CExhibition::IsDirectionChanged()
{
	if (!m_bLButtonDown && m_bDirChanged)
	{
		m_bDirChanged = false;
		return true;
	}
	return false;
}

float CExhibition::GetBaseAngle(float angle, float base)
{
	while (angle > base + g_PI)
		angle -= (2.f * g_PI);
	while (angle < base - g_PI)
		angle += (2.f * g_PI);
	return angle;
}

void CExhibition::Animate()
{
	WVector lightDirection(0.f, -1.f, 1.f);
	if (m_type == EXHIBIT_CHARACTER || m_type == EXHIBIT_CADDIE)
		m_light.nearOne = RotVec(lightDirection, m_camMat);
	else
		m_light.nearOne = RotVec(WVector(1.f, -1.f, 1.f), m_camMat);
	m_light.nearOne.Normalize();
	switch (m_type)
	{
	case EXHIBIT_CHARACTER:
	{
		m_pPetFrame->ApplyBones(m_pPetFrame->GetCurAniPet()->GetBones(NULL,
									m_motionTime, 0, NULL, true),
			m_modelMat);
		break;
	}
	case EXHIBIT_CADDIE:
	case EXHIBIT_MASCOT:
	case EXHIBIT_CARD:
		m_pModel->ApplyBones(
			m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false), m_modelMat);
		break;
	case EXHIBIT_BALL:
	case EXHIBIT_MATCH:
		m_pModel->ApplyBones(NULL, m_modelMat);
		break;
	case EXHIBIT_CLUB:
		m_pModel->ApplyBones(
			m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
			m_modelMat * RotMat(-15.f * g_DEGTORAD, 2));
		break;
	case EXHIBIT_ITEM:
		m_pModel->ApplyBones(
			m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
			m_modelMat * RotMat(-15.f * g_DEGTORAD, 2));
		break;
	case EXHIBIT_FURNITURE:
	{
		if (IsLocalContent((localContentType_t)0x56))
		{
			Waabb box;
			box = m_pModel->GetBoundBox();
			m_modelMat.ym = (box.max.y - box.min.y) * -.5f;
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat);
		}
		break;
	}
	case EXHIBIT_FURNITURE_BASE:
		if (IsLocalContent((localContentType_t)0x56))
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat);
		break;
	case EXHIBIT_FURNITURE_WALL:
		if (IsLocalContent((localContentType_t)0x56))
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat * RotMat(-15.f * g_DEGTORAD, 2));
		break;
	case EXHIBIT_FURNITURE_FLOOR:
	{
		if (IsLocalContent((localContentType_t)0x56))
		{
			WMatrix scale;
			scale.Reset();
			scale *= .7f;
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat * RotMat(90.f * g_DEGTORAD, 0) *
					RotMat(-15.f * g_DEGTORAD, 2) * scale);
		}
		break;
	}
	case EXHIBIT_FURNITURE_ROOM:
	{
		if (IsLocalContent((localContentType_t)0x56))
		{
			WMatrix scale;
			scale.Reset();
			scale *= .85f;
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat * RotMat(90.f * g_DEGTORAD, 0) *
					RotMat(-15.f * g_DEGTORAD, 2) * scale);
		}
		break;
	}
	case EXHIBIT_FURNITURE_TEXTURE:
		if (IsLocalContent((localContentType_t)0x56))
		{
			m_modelMat.xx = m_modelMat.yy = 2.f;
			m_pModel->ApplyBones(
				m_pModel->GetBones(NULL, m_motionTime, 0, NULL, false),
				m_modelMat);
		}
		break;
	}
	if (m_pModel)
	{
		m_pModel->Transform(NULL, 1.f, 0);
		m_pModel->LockBoneSetPos(255);
	}
}

inline void CExhibition::ScriptProcess(const w_motion_data* pMotion, float from,
	float to, bool bForce)
{
	if ((from <= pMotion->e && from != to) || bForce)
	{
		from *= 30.f;
		int first, last;
		if (bForce)
		{
			last = first = Ceil(from, 0.03f);
		}
		else
		{
			to = Min(pMotion->e, to) * 30.f;
			first = Ceil(from, .03f);
			float diff = (float)floor(to) - to;
			if (Wabs(diff) < g_EPSILON)
				last = (int)to - 1;
			else
				last = (int)floor(to);
			int start = (int)floor(pMotion->s * 30.0f + 0.5f);
			if (first == start)
				first++;
		}
		if (first <= last)
		{
			int from2, to2;
			WPuppet* pPuppet;
			if (m_type == EXHIBIT_CHARACTER)
				pPuppet = m_pPetFrame->GetCurAniPet();
			else
				pPuppet = m_pModel;
			if (pPuppet)
			{
				if (pPuppet->CheckFrameData(first, last, from2, to2))
				{
					for (int i = from2; i <= to2; ++i)
						ParsePuppetScript(pPuppet->m_frame[i].data);
				}
			}
		}
	}
}

void CExhibition::ParsePuppetScript(const char* script)
{
	static char s_scriptName[64];
	static char s_scriptCmd[1024];
	char cond[0x400];
	char motion[0x100];
	char tex2[0x40];
	char seg[0x28];
	char name1[0x40];
	char name2[0x40];
	char fxName[0x40];
	char part[0x40];
	char name3[0x40];
	const char* rest;
	for (; script; script = rest)
	{
		rest = strchr(script, '*');
		if (!rest)
			return;
		int n = 0;
		while (rest[n])
		{
			if (rest[++n] == ' ')
				break;
			if (rest[n] == '(')
			{
				++n;
				break;
			}
		}
		if (n <= 1)
			return;
		strncpy(s_scriptCmd, rest, n);
		rest += n;
		s_scriptCmd[n] = 0;
		if (strcmp(s_scriptCmd, "*fx(") == 0)
		{
			fxName[0] = 0;
			rest = GetStrConst(rest, fxName);
			if (strlen(fxName) == 0 || strlen(fxName) >= 64)
				return;
			if (fxName[0] == '@')
			{
				if (!m_pMotion || strcmp(fxName + 1, m_pMotion->name) != 0)
					return;
				rest = GetStrConst(rest, name2);
				if (strlen(name2) == 0 || strlen(name2) >= 64)
					return;
			}
			else
				strcpy(name2, fxName);
			rest = GetStrConst(rest, part);
			int len = strlen(part);
			if (len == 0 || len >= 64)
				return;
			bool found = false;
			if (part[0] == '&')
			{
				do
				{
					unsigned long id = atoi(part + 1);
					if (!found && m_pPetFrame->GetPartList().FindPart(id))
						found = true;
					rest = GetStrConst(rest, part);
					len = strlen(part);
				} while (len && len < 64 && part[0] == '&');
				if (!found)
					return;
			}
			strcpy(name3, part);
			if (strlen(name3) == 0 || strlen(name3) >= 64)
				return;
			Attacher attacher;
			attacher.x = m_center.x;
			attacher.y = m_center.y;
			int i;
			for (i = 0; i < sizeof(attacher.mat) / sizeof(attacher.mat[0]); ++i)
			{
				if (i)
				{
					rest = GetStrConst(rest, name3);
					if (strlen(name3) == 0 || strlen(name3) >= 64)
						break;
				}
				WBone* bone = m_pModel->FindBone(name3);
				if (bone)
					attacher.mat[i] = &bone->GetMatrix();
				else
				{
					w_bound_box* box = m_pModel->FindBoundBox(name3);
					if (box)
					{
						if (box->bone)
							attacher.mat[i] = &box->bone->GetMatrix();
						attacher.pos[i] = &box->info->spherePivot;
					}
				}
			}
			if (i > 0)
			{
				char* ext = strrchr(name2, '.');
				if (ext && strlen(ext) >= 4)
				{
					if (stricmp(ext + 1, "spr") == 0)
					{
						CFxSpray* spray = m_fxBox.LoadSpray(name2, NULL);
						if (spray)
						{
							spray->m_attacher = &attacher;
							spray->m_centerX = attacher.x;
							spray->m_centerY = attacher.y;
							memcpy(spray->m_attachMat, attacher.mat,
								sizeof(attacher.mat));
							memcpy(spray->m_attachPos, attacher.pos,
								sizeof(attacher.pos));
							m_fxBox.AddSpray(spray);
						}
					}
					else
					{
						CFxSequence* seq =
							m_fxBox.LoadSequence(name2, true, NULL);
						if (seq)
						{
							seq->m_attacher = &attacher;
							seq->m_centerX = attacher.x;
							seq->m_centerY = attacher.y;
							m_fxBox.AddSequence(seq);
						}
					}
				}
			}
		}
		else if (strcmp(s_scriptCmd, "*hidebone(") == 0)
		{
			char bone[0x40] = { 0 };
			int i = 0;
			do
			{
				rest = GetStrConst(rest, bone);
				if (strlen(bone) == 0 || strlen(bone) >= 0x40)
					break;
				WBone* pBone;
				if (m_type == EXHIBIT_CHARACTER)
					pBone = m_pPetFrame->GetBonePet()->FindBone(bone);
				else
					pBone = m_pModel->FindBone(bone);
				if (pBone)
					pBone->SetInvisible(false, true);
				if (!rest)
					return;
				if (*rest == ')')
					break;
			} while (++i < 0x40);
		}
		else if (strcmp(s_scriptCmd, "*snd(") == 0)
		{
			rest = GetStrConst(rest, s_scriptName);
			if (strlen(s_scriptName) == 0 || strlen(s_scriptName) >= 0x40)
				return;
			g_audio->PlaySfx(s_scriptName);
		}
		else if (strcmp(s_scriptCmd, "*ptex(") == 0)
		{
			name1[0] = '\0';
			rest = GetStrConst(rest, name1);
			if (strlen(name1) == 0 || strlen(name1) >= 0x40)
				return;
			if (name1[0] == '@')
			{
				if (!m_pMotion || strcmpi(name1 + 1, m_pMotion->name) != 0)
					return;
				rest = GetStrConst(rest, s_scriptName);
				if (strlen(s_scriptName) == 0 || strlen(s_scriptName) >= 0x40)
					return;
			}
			else
			{
				strcpy(s_scriptName, name1);
			}
			rest = GetStrConst(rest, tex2);
			if (strlen(tex2) == 0 || strlen(tex2) >= 0x40)
				return;
			if (m_bChatFacial)
				return;
			SetPendingTexPart(s_scriptName, tex2);
		}
		else if (strcmp(s_scriptCmd, "*playif(") == 0)
		{
			int off = 0;
			motion[0] = '\0';
			cond[0] = '\0';
			rest = GetStrConst(rest, cond);
			rest = GetStrConst(rest, motion);
			if (cond[0] == '\0' || motion[0] == '\0')
				return;
			for (;;)
			{
				char* segStart = cond + off;
				char* semi = strchr(segStart, ';');
				int segLen;
				if (semi)
					segLen = semi - cond - off;
				else
				{
					segLen = (int)strlen(segStart);
					if (segLen == 0)
						goto next;
				}
				strncpy(seg, segStart, segLen);
				seg[segLen] = '\0';
				strcat(seg, ".mpet");

				if (m_pPetFrame && m_pPetFrame->HasMPet(seg))
				{
					SetMotion(motion, false, 0.0f);
					break;
				}
				if (!semi)
					goto next;
				off += segLen + 1;
			}
		}
		else if (stricmp(s_scriptCmd, "*showvis(") == 0)
		{
			ParseScript_ShowVis(true, rest);
		}
		else if (stricmp(s_scriptCmd, "*hidevis(") == 0)
		{
			ParseScript_ShowVis(false, rest);
		}
		else if (strcmp(s_scriptCmd, "*give") == 0)
		{
			FrWnd* pWnd = g_pFresh->GetManager()->GetDesktop()->FindChildByName(
				"bongdari_shop");
			FrBongdariShopDlg* pShop = NULL;
			if (pWnd)
				pShop = (FrBongdariShopDlg*)pWnd;
			else
				__rtti_obj = NULL;
			if (pShop)
			{
				w_bound_box* bb = m_pModel->FindBoundBox("out");
				if (bb)
					pShop->CreateBonusBall(bb->info->spherePivot);
			}
		}

next:;
	}
}

void CExhibition::ParseScript_ShowVis(bool bShow, const char* script)
{
	char name[256] = { 0 };
	for (int i = 0; i < 0x40; ++i)
	{
		script = GetStrConst(script, name);
		if (strlen(name) == 0)
			return;
		if (strlen(name) >= 0x100)
			return;
		std::list<std::string>* pGroup = Doc()->GetVisGroup(name);
		if (pGroup)
		{
			std::list<std::string>::iterator it = pGroup->begin();
			if (it != pGroup->end())
			{
				do
				{
					WBone* bone =
						m_pPetFrame->GetBonePet()->FindBone((*it).c_str());
					if (bone)
						bone->SetInvisible(false, !bShow);
					++it;
				} while (it != pGroup->end());
			}
		}
		if (!script)
			return;
		if (*script == ')')
			return;
	}
}

bool CExhibition::InValidArea()
{
	WVector mouse = CInputManager::Instance()->GetMousePoint();
	if (m_width > 0.0f && m_height > 0.0f)
	{
		float dx = Abs<float>(mouse.x - m_center.x);
		if (dx < m_width * 0.5f)
		{
			float dy = Abs<float>(mouse.y - m_center.y);
			if (dy < m_height * 0.5f)
				return true;
		}
	}
	else if (m_radius > 0.0f)
	{
		float dy = mouse.y - m_center.y;
		float dx = mouse.x - m_center.x;
		if (dy * dy + dx * dx < m_radius * m_radius)
			return true;
	}
	return false;
}

void CExhibition::SetCenter(float x, float y)
{
	m_center.x = x;
	m_center.y = y;
	m_fxBox.SetScreenCenter(x, y);
	float oldFov = g_view->GetFOV();
	g_view->SetFOV(g_view->GetRatio() * 2.0943952f);
	float t = (float)tanf(g_view->GetFOV() * .5f);
	float height = t * m_curDist + t * m_curDist;
	float scale = m_height / g_view->GetHeight() * m_anchor;
	m_modelMat.pivot = WVector(0.f, -height * scale, 0.f);
	g_view->SetFOV(oldFov);
	ParseBoundingBoxScript();
	if (m_pWingR)
	{
		m_fxBox.DeleteSequence(m_pWingR, true);
		m_pWingR = NULL;
	}
	if (m_pWingL)
	{
		m_fxBox.DeleteSequence(m_pWingL, true);
		m_pWingL = NULL;
	}
}

const WVector2D& CExhibition::GetCenter()
{
	return m_center;
}

void CExhibition::SetArea(float radius)
{
	m_width = 0.0f;
	m_height = 0.0f;
	m_radius = radius;
}

void CExhibition::SetArea(float w, float h)
{
	m_radius = 0.0f;
	m_width = w;
	m_height = h;
}

bool CExhibition::SetMotion(const char* name, bool bContinue, float time)
{
	if (!bContinue)
	{
		m_fxBox.ClearAll(false);
		ParseBoundingBoxScript();
	}
	if (m_type == EXHIBIT_CHARACTER)
	{
		if (!m_pPetFrame)
			return false;
		m_pMotion = m_pPetFrame->FindMotionData(name, true, NULL);
		m_pPetFrame->SetInvisible(false);
	}
	else
	{
		if (!m_pModel)
			return false;
		if (!m_pModel->GetRootBone())
			return false;
		m_pMotion = m_pModel->FindMotionData(name);
		m_pModel->GetRootBone()->SetInvisible(true, false);
	}
	if (m_pMotion)
	{
		ScriptProcess(m_pMotion, m_pMotion->s, m_pMotion->s, true);
		m_motionTime = Min(time + m_pMotion->s, m_pMotion->e);
		UpdateCamera(0.0f);
		Animate();
		return true;
	}
	return false;
}

const char* CExhibition::GetNextCharMotion(int index)
{
	m_motionIndex++;
	m_motionIndex %= 12;
	int motion = s_charMotionTable[index][m_motionIndex];
	if (motion == -1)
	{
		m_motionIndex = 0;
		motion = s_charMotionTable[index][0];
	}
	return s_charMotions[motion];
}

const char* CExhibition::GetNextCaddieMotion(int index)
{
	if (index >= 28)
		index = 27;
	m_motionIndex = (m_motionIndex + 1) % 9;
	if (s_caddieMotionTable[index][m_motionIndex] == -1)
	{
		m_motionIndex = 0;
		return s_caddieMotions[s_caddieMotionTable[index][0]];
	}
	return s_caddieMotions[s_caddieMotionTable[index][m_motionIndex]];
}

w_motion_data* CExhibition::GetMotionData()
{
	return m_pMotion;
}

bool CExhibition::NextMotion()
{
	for (;;)
	{
		bool bResult;
		switch (m_type)
		{
		case EXHIBIT_CHARACTER:
			bResult = SetMotion(GetNextCharMotion(m_charIndex), false, 0.0f);
			break;
		case EXHIBIT_CADDIE:
			bResult =
				SetMotion(GetNextCaddieMotion(m_caddieIndex), false, 0.0f);
			break;
		case EXHIBIT_MASCOT:
			bResult = SetMotion("Default", false, 0.0f);
			break;
		case EXHIBIT_CLUB:
			bResult = SetMotion("default", false, 0.0f);
			break;
		case EXHIBIT_ITEM:
		case EXHIBIT_CARD:
			bResult = SetMotion("default", false, 0.0f);
			break;
		case EXHIBIT_BALL:
		case EXHIBIT_MATCH:
		case EXHIBIT_FURNITURE:
		case EXHIBIT_FURNITURE_BASE:
		case EXHIBIT_FURNITURE_WALL:
		case EXHIBIT_FURNITURE_FLOOR:
		case EXHIBIT_FURNITURE_ROOM:
		case EXHIBIT_FURNITURE_TEXTURE:
			return true;
		default:
			continue;
		}
		if (bResult)
			return bResult;
	}
}

bool CExhibition::SetModel(CPartTidList& parts, unsigned long* auxParts,
	const char* motion, float delay, int option)
{
	m_motionIndex = -1;
	m_typeId = parts.m_charTid;
	m_pAuxParts = auxParts;
	ReleasePet();
	m_type = EXHIBIT_CHARACTER;
	m_light.ambient = 0x969696;
	m_pPetFrame = new CPetFrame;
	if (!m_pPetFrame->Build(&parts, auxParts, true, false, option))
	{
		if (m_pPetFrame)
		{
			delete m_pPetFrame;
			m_pPetFrame = NULL;
		}
		return false;
	}

	m_pModel = m_pPetFrame->GetBonePet();
	m_parts = parts;
	WMatrix matrix;
	matrix.Reset();
	m_pModel->ApplyBones(NULL, matrix);
	m_pModel->Transform(NULL, 1.f, 0);
	m_pModel->UpdateBound(false, false);
	SetScreenPos(m_pModel);
	m_charIndex = m_pPetFrame->GetTidChar() & 0x3ffffff;
	if (motion)
		SetMotion(motion, false, 0.f);
	else
		SetMotion(GetNextCharMotion(m_charIndex), false, 0.f);
	m_delay = Random(0.f, delay);
	if (m_pMotion)
		m_motionLength = (m_pMotion->e - m_pMotion->s) + m_delay;
	else
		m_motionLength = 0.f;
	m_alpha = 0.f;
	m_pPetFrame->SetAlpha(0);
	if (parts.m_charTid == 0x4000005 || parts.m_charTid == 0x4000006)
		m_pitchBias = -15.f * g_DEGTORAD;
	else
		m_pitchBias = 0.f;
	ParseBoundingBoxScript();
	m_pPetFrame->Refresh(&parts);
	return true;
}

bool CExhibition::SetModel(unsigned long tid)
{
	m_motionIndex = -1;
	m_typeId = tid;
	m_pAuxParts = NULL;
	ReleasePet();
	int n;
	switch (m_typeId >> 26)
	{
	case 8:
	{
		m_type = EXHIBIT_CADDIE;
		m_light.ambient = 0x969696;
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(m_typeId);
		if (!pCadItem)
			return false;
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(
			((pCadItem->c.TypeId >> 21) & 31) | 0x1c000000);
		if (!pCaddie)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s_ex.pet", pCaddie->Data),
			false, true, false);
		if (!m_pModel)
			return false;
		m_caddieIndex = pCaddie->c.TypeId & 0x3ffffff;
		if (strlen(pCadItem->FaceTex))
			SetPendingTexPart("face", pCadItem->FaceTex);
		if (strlen(pCadItem->BodyTex))
			SetPendingTexPart("body", pCadItem->BodyTex);
		SetMotion(GetNextCaddieMotion(m_caddieIndex), false, 0.f);
		break;
	}
	case 7:
	{
		m_type = EXHIBIT_CADDIE;
		m_light.ambient = 0x969696;
		IFF_STRUCT::sCaddie* pCaddie = ItemManager()->FindCaddie(m_typeId);
		if (!pCaddie)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pCaddie->Data),
			false, true, false);
		if (!m_pModel)
			return false;
		m_caddieIndex = pCaddie->c.TypeId & 0x3ffffff;
		ParseBoundingBoxScript();
		SetMotion(GetNextCaddieMotion(m_caddieIndex), false, 0.0f);
		goto applyModel;
	}
	case 5:
	{
		m_type = EXHIBIT_BALL;
		m_light.ambient = 0x646464;
		IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(m_typeId);
		if (!pBall)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pBall->Data), false,
			true, false);
		if (!m_pModel)
			return false;
		switch (m_typeId)
		{
		case 0x14000005:
		case 0x1400001a:
		case 0x14000080:
			AttachFx();
		}
		break;
	}
	case 4:
	{
		m_type = EXHIBIT_CLUB;
		m_light.ambient = 0x646464;
		IFF_STRUCT::sClubSet* pClub = ItemManager()->FindClubSet(m_typeId);
		if (!pClub)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s_ui.pet", pClub->c.Icon),
			false, true, false);
		if (m_pModel)
		{
			ParseBoundingBoxScript();
			SetMotion("default", true, 0.0f);
		}
		else
			return false;
		break;
	}
	case 0xb:
	{
		unsigned long matchId = tid & 0xffffff00;
		unsigned long icon = tid & 0xff;
		m_type = EXHIBIT_MATCH;
		m_light.ambient = 0x646464;
		IFF_STRUCT::sMatch* pMatch = ItemManager()->FindMatch(matchId);
		if (!pMatch)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pMatch->Icon[icon]),
			false, true, false);
		if (!m_pModel)
			goto modelFailed;
		break;
	}
	case 0x10:
	{
		m_type = EXHIBIT_MASCOT;
		m_light.ambient = 0x969696;
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(m_typeId);
		if (!pMascot)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pMascot->Data),
			false, true, false);
		if (!m_pModel)
			return false;
		ParseBoundingBoxScript();
		SetMotion("Default", false, 0.f);
		n = 0;
		std::map<unsigned int, sMascotInfo>::const_iterator it =
			Doc()->m_mascotMap.begin();
		for (; it != Doc()->m_mascotMap.end(); ++n, ++it)
		{
			if ((*it).second.tid == m_typeId)
			{
				pMascot = ItemManager()->FindMascot(m_typeId);
				if (pMascot && pMascot->ShowMsg)
					m_msgTexture =
						CMascot::ChangeMascotMsgInTex((*it).second.szMsg,
							pMascot, m_pModel, MakeStr("ui%02d", n));
				break;
			}
		}
		break;
	}

checkSetModel:
{
	if (!m_pModel)
		return false;
	ParseBoundingBoxScript();
	SetMotion("default", true, 0.0f);
	break;
}

	case 6:
	{
		m_type = EXHIBIT_ITEM;
		m_light.ambient = 0x646464;
		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(m_typeId);
		if (!pItem)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pItem->Data), false,
			true, false);
		if (!m_pModel)
			return false;
		ParseBoundingBoxScript();
		SetMotion("default", true, 0.0f);
		break;
	}
	case 0x1f:
	{
		m_type = EXHIBIT_CARD;
		m_light.ambient = 0xc8c8c8;
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(m_typeId);
		if (!pCard)
			return false;
		unsigned long kind = m_typeId >> 0x16 & 0xf;
		if (kind == 3 || kind == 4)
		{
			m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pCard->Image),
				false, true, false);
		}
		if (!m_pModel)
			return false;
		ParseBoundingBoxScript();
		SetMotion("default", true, 0.0f);
		break;
	}
	case 9:
	{
		unsigned long kind = m_typeId >> 0x15 & 0x1f;
		if (kind != 6 && kind != 5 && kind != 4 && kind != 8)
			break;
		m_type = EXHIBIT_ITEM;
		m_light.ambient = 0x646464;
		IFF_STRUCT::sSetItem* pSet = ItemManager()->FindSetItem(m_typeId);
		if (!pSet)
			return false;
		m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pSet->c.Icon), false,
			true, false);
		goto checkSetModel;
	}
	case 0x12:
	{
		if (!IsLocalContent((localContentType_t)0x56))
			break;
		m_light.ambient = 0x989898;
		IFF_STRUCT::sFurniture* pFurn = ItemManager()->FindFurniture(m_typeId);
		if (!pFurn)
			return false;
		unsigned long kind = pFurn->c.TypeId >> 0xb & 0x7ff;
		if (kind == 8)
			m_type = EXHIBIT_FURNITURE_ROOM;
		else if (kind == 9)
			m_type = EXHIBIT_FURNITURE_FLOOR;
		else if (kind == 0xe)
			m_type = EXHIBIT_FURNITURE_TEXTURE;
		else if (pFurn->IsMove == 5)
			m_type = EXHIBIT_FURNITURE_BASE;
		else if (pFurn->IsMove == 6)
			m_type = EXHIBIT_FURNITURE_WALL;
		else
			m_type = EXHIBIT_FURNITURE;
		if (m_type == EXHIBIT_FURNITURE_WALL ||
			m_type == EXHIBIT_FURNITURE_FLOOR ||
			m_type == EXHIBIT_FURNITURE_ROOM)
		{
			m_pModel = g_resrcmng->GetPuppet(MakeStr("%s_ui.pet", pFurn->Data),
				false, true, false);
		}
		else
		{
			if (m_type == EXHIBIT_FURNITURE_TEXTURE)
			{
				pFurn = ItemManager()->FindFurniture(m_typeId);
				if (!pFurn)
					return false;
				m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pFurn->Data),
					false, true, false);
				if (!m_pModel)
					return false;
				for (int i = 0; i < 3; ++i)
				{
					if (strlen(pFurn->OrgTex[i]) != 0 &&
						strlen(pFurn->Tex[i]) != 0)
						SetPendingTexPart(MakeStr("%s.jpg", pFurn->OrgTex[i]),
							MakeStr("%s.jpg", pFurn->Tex[i]));
				}
			}
			else
			{
				m_pModel = g_resrcmng->GetPuppet(MakeStr("%s.pet", pFurn->Data),
					false, true, false);
			}
		}
		if (!m_pModel)
			return false;
		ParseBoundingBoxScript();
		break;
	}
	}

applyModel:
{
	WMatrix matrix;
	matrix.Reset();
	m_pModel->ApplyBones(NULL, matrix);
	m_pModel->Transform(NULL, 1.0f, 0);
	m_pModel->UpdateBound(false, false);
	SetScreenPos(m_pModel);
	m_delay = 0.0f;
	if (m_pMotion)
		m_motionLength = m_pMotion->e - m_pMotion->s;
	else
		m_motionLength = 0.0f;
	m_alpha = 0.0f;
	m_pModel->SetAlpha(0);
	return true;
}

modelFailed:
	return false;
}

bool CExhibition::SetModel(const char* name, eExhibitType type)
{
	m_motionIndex = -1;
	m_typeId = 0;
	ReleasePet();
	switch (type)
	{
	case EXHIBIT_CADDIE:
		m_type = EXHIBIT_CADDIE;
		m_light.ambient = 0x969696;
		m_pModel =
			g_resrcmng->GetPuppet(MakeStr("%s.pet", name), false, true, false);
		ParseBoundingBoxScript();
		SetMotion("\xb1\xe2\xba\xbb", false, 0.f);
		break;
	case EXHIBIT_BALL:
		m_type = EXHIBIT_BALL;
		m_light.ambient = 0xc8c8c8;
		m_pModel =
			g_resrcmng->GetPuppet(MakeStr("%s.pet", name), false, true, false);
		break;
	case EXHIBIT_CARD:
		m_type = EXHIBIT_CARD;
		m_light.ambient = 0xffffff;
		ParseBoundingBoxScript();
		m_pModel =
			g_resrcmng->GetPuppet(MakeStr("%s.pet", name), false, true, false);
		break;
	}
	WMatrix matrix;
	matrix.Reset();
	m_pModel->ApplyBones(NULL, matrix);
	m_pModel->Transform(NULL, 1.f, 0);
	m_pModel->UpdateBound(false, false);
	SetScreenPos(m_pModel);
	m_delay = 0.f;
	if (m_pMotion)
		m_motionLength = m_pMotion->e - m_pMotion->s;
	else
		m_motionLength = 0.f;
	m_alpha = 0.f;
	m_pModel->SetAlpha(0);
	return true;
}

void CExhibition::AttachFx()
{
	IFF_STRUCT::sBall* ball = ItemManager()->FindBall(m_typeId);
	if (!ball)
		return;
	char* filename = ball->Fx[0];
	int len = strlen(filename);
	if (len < 5)
		return;
	Attacher attacher;
	WBone* bone = NULL;
	if (ball->FxBone[0][0])
		bone = m_pModel->FindBone(ball->FxBone[0]);
	if (bone)
		attacher.mat[0] = &bone->GetMatrix();
	else
	{
		w_bound_box* box;
		if (ball->FxBone[0][0])
			box = m_pModel->FindBoundBox(ball->FxBone[0]);
		else
			box = m_pModel->FindBoundBox("center");
		if (box)
		{
			if (box->bone)
				attacher.mat[0] = &box->bone->GetMatrix();
			attacher.pos[0] = &box->info->spherePivot;
		}
	}
	attacher.x = m_center.x;
	attacher.y = m_center.y;
	if (strcmpi(ball->Fx[0] + len - 3, "spr") == 0)
	{
		CFxSpray* spray = m_fxBox.LoadSpray(filename, NULL);
		if (spray)
		{
			spray->m_attacher = &attacher;
			spray->m_centerX = attacher.x;
			spray->m_centerY = attacher.y;
			memcpy(spray->m_attachMat, attacher.mat, sizeof(attacher.mat));
			memcpy(spray->m_attachPos, attacher.pos, sizeof(attacher.pos));
			m_fxBox.AddSpray(spray);
		}
	}
	else
	{
		CFxSequence* seq = m_fxBox.LoadSequence(filename, true, NULL);
		if (seq)
		{
			seq->m_attacher = &attacher;
			seq->m_centerX = attacher.x;
			seq->m_centerY = attacher.y;
			m_fxBox.AddSequence(seq);
		}
	}
}

void CExhibition::SetScreenPos(WPuppet* puppet)
{
	if (!puppet)
		return;
	WMatrix camera;
	camera.Reset();
	camera.zm = -m_curDist;
	float aspect = g_view->GetRatio();
	float oldFov = g_view->GetFOV();
	if (m_type == EXHIBIT_CHARACTER || m_type == EXHIBIT_CADDIE)
	{
		WBone* head = puppet->FindBone("Bip01 Head");
		WBone* foot = puppet->FindBone("Bip01 Footsteps");
		WVector headPos, footPos;
		if (head && foot)
		{
			headPos = head->GetMatrix().pivot;
			footPos = foot->GetMatrix().pivot;
		}
		else
		{
			w_bound_box* box = puppet->FindBoundBox("center");
			if (!box)
				return;
			headPos = WVector(0.f,
				(box->info->aabb.max.y - box->info->aabb.min.y) * .75f, 0.f);
			footPos = WVector::ZERO;
		}
		g_view->SetFOV(2.0943952f * (g_view->GetHeight() / g_view->GetWidth()));
		g_view->SetCamera(camera);
		g_view->UpdateCamera();
		WVector projHead = g_view->Projection(headPos);
		WVector projFoot = g_view->Projection(footPos);
		float dy = Abs(projHead.y - projFoot.y);
		float scale;
		if (dy < m_height * .75f)
			scale = ((m_height * .75f) / dy - 1.f) * .5f + 1.f;
		else
			scale = 1.f - (1.f - (m_height * .75f) / dy) * .5f;
		if (scale < .5f)
			scale = .5f;
		else if (scale > 1.2f)
			scale = 1.2f;
		m_modelMat.Reset();
		m_modelMat *= scale;
	}
	else if (m_type == EXHIBIT_FURNITURE || m_type == EXHIBIT_FURNITURE_BASE ||
		m_type == EXHIBIT_FURNITURE_WALL)
	{
		g_view->SetFOV(aspect * 2.0943952f);
		m_modelMat.Reset();
	}
	else
	{
		g_view->SetFOV(aspect * 2.0943952f);
		m_modelMat.Reset();
	}
	m_defDist = m_dist = m_curDist;
	float scale = m_height / g_view->GetHeight() * m_anchor;
	float t = (float)tanf(g_view->GetFOV() * .5f);
	float height = t * m_curDist + t * m_curDist;
	m_modelMat.pivot = WVector(0.f, -height * scale, 0.f);
	g_view->SetFOV(oldFov);
	g_view->UpdateCamera();
	if (m_type == EXHIBIT_CHARACTER || m_type == EXHIBIT_CADDIE)
	{
		m_defaultAngle.x = 6.25319f;
		m_defaultAngle.y = .33f;
	}
	else if (m_type == EXHIBIT_FURNITURE || m_type == EXHIBIT_FURNITURE_BASE ||
		m_type == EXHIBIT_FURNITURE_WALL)
	{
		m_defaultAngle.x = 5.85319f;
		m_defaultAngle.y = .33f;
	}
	m_angle = m_defaultAngle;
	m_prevYaw = m_defaultAngle.y;
}

void CExhibition::SetAlpha(unsigned char alpha)
{
	if (m_type == EXHIBIT_CHARACTER)
	{
		if (m_pPetFrame)
			m_pPetFrame->SetAlpha(alpha);
	}
	else if (m_pModel)
		m_pModel->SetAlpha(alpha);
}

void CExhibition::ParseBoundingBoxScript()
{
	WPuppet* puppet = m_pModel;
	if (!puppet)
		return;
	char command[] = "*fx";
	for (unsigned i = 0; i < m_bbSpray.size(); ++i)
		m_fxBox.DeleteSpray(m_bbSpray[i]);
	for (unsigned i = 0; i < m_bbSeq.size(); ++i)
		m_fxBox.DeleteSequence(m_bbSeq[i], true);
	m_bbSpray.clear();
	m_bbSeq.clear();
	for (w_bound_box* box = puppet->m_bbList.Start(); box;
		box = puppet->m_bbList.Next())
	{
		if (!box->info->option || !box->bone || (box->bone->IsInvisible()))
			continue;
		const char* script = strstr(box->info->option, command);
		while (script)
		{
			char filename[64], part[64];
			unsigned len = strlen(command);
			if (script[len] == '(')
				++len;
			const char* rest = len > strlen(command)
				? GetStrConst(script + len, filename)
				: GetCharsTillSpace(script + len, filename);
			int filenameLen = strlen(filename);
			if (filenameLen && filenameLen < 64)
			{
				char* ext = strrchr(filename, '.');
				if (ext && strlen(ext) >= 4)
				{
					bool valid = true;
					if (m_type == EXHIBIT_CHARACTER)
					{
						rest = GetStrConst(rest, part);
						int partLen = strlen(part);
						if (partLen && partLen < 64)
						{
							bool tagged = false;
							do
							{
								if (part[0] != '&')
									break;
								tagged = true;
								unsigned long id = atoi(part + 1);
								if (m_pPetFrame->GetPartList().FindPart(id) ||
									m_pPetFrame->FindAuxPart(id))
								{
									tagged = false;
									break;
								}
								rest = GetStrConst(rest, part);
								partLen = strlen(part);
							} while (partLen && partLen < 64);
							if (tagged)
								valid = false;
						}
						else
							valid = false;
					}
					if (valid)
					{
						Attacher attacher;
						attacher.x = m_center.x;
						attacher.y = m_center.y;
						attacher.mat[0] = &box->bone->GetMatrix();
						attacher.pos[0] = &box->info->spherePivot;
						if (stricmp(ext + 1, "spr") == 0)
						{
							CFxSpray* spray = m_fxBox.LoadSpray(filename, NULL);
							if (spray)
							{
								spray->m_attacher = &attacher;
								spray->m_centerX = attacher.x;
								spray->m_centerY = attacher.y;
								memcpy(spray->m_attachMat, attacher.mat,
									sizeof(attacher.mat));
								memcpy(spray->m_attachPos, attacher.pos,
									sizeof(attacher.pos));
								m_fxBox.AddSpray(spray);
								m_bbSpray.push_back(spray);
							}
						}
						else if (stricmp(ext + 1, "seq") == 0)
						{
							CFxSequence* seq =
								m_fxBox.LoadSequence(filename, true, NULL);
							if (seq)
							{
								seq->m_attacher = &attacher;
								seq->m_centerX = attacher.x;
								seq->m_centerY = attacher.y;
								m_fxBox.AddSequence(seq);
								m_bbSeq.push_back(seq);
							}
						}
					}
				}
			}
			script = strstr(rest, command);
		}
	}
}

sTexturePart* CExhibition::FindTexturePart(char* target, char* texture)
{
	sTexturePart* part;
	std::list<sTexturePart*>::iterator it;
	it = std::find_if(m_texParts.begin(), m_texParts.end(),
		sFindTexPart(texture));
	if (it != m_texParts.end())
		part = *it;
	else
	{
		part = new sTexturePart;
		strcpy(part->name, target);
		strcpy(part->part, texture);
		m_texParts.push_back(part);
	}
	return part;
}

void CExhibition::SetPendingTexPart(char* primary, const char* secondary)
{
	sTexturePart* pPart = FindTexturePart(primary, (char*)secondary);
	if (!pPart)
		return;
	if (m_type == EXHIBIT_CHARACTER)
	{
		if (!m_pPetFrame)
			return;
		if (pPart->bitmap == NULL)
		{
			m_pPetFrame->GetBonePet()->ChangeTexturePart(
				m_pPetFrame->GetFaceTex(), pPart->part, &pPart->bitmap,
				&pPart->rect);
		}
		else
		{
			m_pPetFrame->GetBonePet()->ChangeTexturePart(
				m_pPetFrame->GetFaceTex(), pPart->bitmap, pPart->rect);
		}
		m_pPetFrame->SetMakeup();
	}
	else if (m_type == EXHIBIT_FURNITURE_TEXTURE)
	{
		if (!m_pModel)
			return;
		int partHandle = g_resrcmng->LoadTexture(primary, 0, 0, NULL);
		if (!partHandle)
			return;
		int texHandle = g_resrcmng->LoadTexture(secondary, 0, 0, NULL);
		if (!texHandle)
			return;
		m_pModel->ChangeTextureByHandle(texHandle, partHandle, m_pModel);
	}
	else
	{
		if (!m_pModel)
			return;
		if (pPart->bitmap == NULL)
			m_pModel->ChangeTexturePart(pPart->name, pPart->part,
				&pPart->bitmap, &pPart->rect);
		else
			m_pModel->ChangeTexturePart(pPart->name, pPart->bitmap,
				pPart->rect);
	}
}

void CExhibition::SetChatFacial(int facial, unsigned long typeId)
{
	unsigned long index = typeId & 0x3ffffff;
	if (facial >= 0 && facial < 4 && index < 10)
	{
		m_bChatFacial = true;
		SetPendingTexPart("face", s_chatFacials[index][facial]);
	}
}

void CExhibition::AttachAngelWingFx()
{
	Attacher attacher;
	if (m_pPetFrame)
	{
		WBone* bone = m_pPetFrame->GetBonePet()->FindBone("Bip01 R Finger1");
		m_pWingR = m_fxBox.LoadSequence("High-Rank.seq", true, NULL);
		m_pWingL = m_fxBox.LoadSequence("High-Rank.seq", true, NULL);
		if (m_pWingR && bone)
		{
			attacher.x = m_center.x;
			attacher.y = m_center.y;
			attacher.mat[0] = &bone->GetMatrix();
			m_pWingR->m_attacher = &attacher;
			m_pWingR->m_centerX = attacher.x;
			m_pWingR->m_centerY = attacher.y;
			m_fxBox.AddSequence(m_pWingR);
		}
		bone = m_pPetFrame->GetBonePet()->FindBone("Bip01 L Finger1");
		if (m_pWingL && bone)
		{
			attacher.mat[0] = &bone->GetMatrix();
			attacher.x = m_center.x;
			attacher.y = m_center.y;
			m_pWingL->m_attacher = &attacher;
			m_pWingL->m_centerX = attacher.x;
			m_pWingL->m_centerY = attacher.y;
			m_fxBox.AddSequence(m_pWingL);
		}
	}
}

bool CExhibition::ChangeBoneTexture(const char* bone, const char* texPart,
	const char* texName)
{
	w_texlist* pTex = g_resrcmng->FindTexture(texPart);
	WBone* pBone;
	if (pTex && m_pModel && (pBone = m_pModel->GetRootBone()))
	{
		int handle = g_resrcmng->LoadTexture(texName, 0x80000000, 0, NULL);
		return pBone->ChangeTexture(bone, pTex->texhandle, handle);
	}
	return false;
}

bool CExhibition::ChangeBoneTexture(const char* bone, const char* texName)
{
	WBone* pBone;
	if (m_pModel && (pBone = m_pModel->GetRootBone()))
	{
		int handle = g_resrcmng->LoadTexture(texName, 0x80000000, 0, NULL);
		return pBone->ChangeTexture(bone, handle);
	}
	return false;
}

void CExhibition::DisplayClip(const WRect& rect, float fov, float nearClip,
	float farClip)
{
	bool fog = g_view->SetFogEnable(false);
	float oldNear = g_view->GetClipNearValue(),
		  oldFar = g_view->GetClipFarValue(), oldFov = g_view->GetFOV();
	m_pModel->UpdateBSphere();
	const WSphere& sphere = m_pModel->GetBoundSphere();
	float dx = m_camMat.pivot.x - sphere.pos.x,
		  dy = m_camMat.pivot.y - sphere.pos.y,
		  dz = m_camMat.pivot.z - sphere.pos.z;
	float dist = (float)sqrt(dx * dx + dy * dy + dz * dz);
	float radius =
		(float)sqrt(m_modelMat.xx * m_modelMat.xx +
			m_modelMat.yx * m_modelMat.yx + m_modelMat.zx * m_modelMat.zx) *
		sphere.radius;
	float nearPlane = Max(.5f, dist - radius),
		  farPlane = Min(100.f, dist + radius);
	if (nearClip > 0.f)
		nearPlane = nearClip;
	if (farClip > nearClip)
		farPlane = Max(farClip, nearPlane);
	g_view->SetClip(nearPlane, farPlane, false);
	g_view->SetCamera(m_camMat);
	g_view->SetClippingArea(rect);
	fov *= g_DEGTORAD;
	g_view->SetFOV(
		fov / g_view->GetWidth() * g_view->GetHeight() * (4.f / 3.f));
	g_view->SetScreenCenter(m_center.x, m_center.y);
	g_view->UpdateCamera();
	m_pModel->UpdateLightSource(&m_light, false, NULL);
	m_pModel->Render(g_view, true, 1.f, false, false, false);
	g_view->Flush(1);
	g_view->SetCamera(g_camera);
	g_view->SetClip(oldNear, oldFar, false);
	g_view->SetFOV(oldFov);
	g_view->ResetClippingArea();
	g_view->SetScreenCenter(g_view->GetWidth() * .5f,
		g_view->GetHeight() * .5f);
	g_view->UpdateCamera();
	g_view->SetFogEnable(fog);
}

void CExhibition::MoveModel(const WPoint& point, const WRect* rect, float fov)
{
	float width = rect ? rect->w : g_view->GetWidth();
	float height = rect ? rect->h : g_view->GetHeight();
	float distance = m_curDist;
	fov *= g_DEGTORAD;
	float t = tanf(fov * .5f);
	float scale = t * distance / (width * .5f);
	WVector from((m_center.x - width * .5f) * scale,
		(m_center.y - height * .5f) * scale * -1.f, distance);
	WVector to;
	to.z = m_curDist;

	to.x = (point.x - width * .5f) * (tanf(fov * .5f) * to.z / (width * .5f));
	to.y = (point.y - height * .5f) * (tanf(fov * .5f) * to.z / (width * .5f)) *
		-1.f;
	WVector delta;
	delta = from - to;
	WPuppet* puppet = m_pModel;
	if (puppet)
		puppet->MoveBone(puppet->GetRootBone()->GetBoneName(), delta);
}

void CExhibition::Display(float nearClip, float farClip)
{
	bool fog = g_view->SetFogEnable(false);
	g_view->SetCamera(m_camMat);
	float oldNear = g_view->GetClipNearValue(),
		  oldFar = g_view->GetClipFarValue(), oldFov = g_view->GetFOV();
	float nearPlane, farPlane;
	if (m_type == EXHIBIT_BALL || m_type == EXHIBIT_CLUB ||
		m_type == EXHIBIT_ITEM)
	{
		nearPlane = .5f;
		farPlane = 10.f;
	}
	else
	{
		WSphere sphere;
		if (m_pModel)
		{
			m_pModel->UpdateBSphere();
			sphere = m_pModel->GetBoundSphere();
		}
		else
		{
			sphere.pos = WVector(0.f, 0.f, 0.f);
			sphere.radius = 0.f;
		}
		float dx = m_camMat.pivot.x - sphere.pos.x,
			  dy = m_camMat.pivot.y - sphere.pos.y,
			  dz = m_camMat.pivot.z - sphere.pos.z;
		float dist = (float)sqrt(dx * dx + dy * dy + dz * dz);
		float radius =
			(float)sqrt(m_modelMat.xx * m_modelMat.xx +
				m_modelMat.yx * m_modelMat.yx + m_modelMat.zx * m_modelMat.zx) *
			sphere.radius;
		nearPlane = Max(.5f, dist - radius);
		farPlane = Min(100.f, dist + radius);
	}
	if (nearClip > 0.f)
		nearPlane = nearClip;
	if (farClip > nearClip)
		farPlane = Max(farClip, nearPlane);
	g_view->SetFOV(g_view->GetHeight() / g_view->GetWidth() * 2.0943952f);
	g_view->SetClip(nearPlane, farPlane, false);
	g_view->SetScreenCenter(m_center.x, m_center.y);
	g_view->UpdateCamera();
	if (m_type == EXHIBIT_CHARACTER)
	{
		if (m_pPetFrame)
		{
			m_pPetFrame->UpdateLightSource(&m_light, false, NULL);
			m_pPetFrame->Render(g_view, true, 1.f, false, false, true);
		}
	}
	else if (m_pModel)
	{
		m_pModel->UpdateLightSource(&m_light, false, NULL);
		m_pModel->Render(g_view, true, 1.f, false, false, false);
	}
	m_fxBox.Display(g_view);
	g_view->Flush(1);
	g_view->SetClip(oldNear, oldFar, false);
	g_view->SetFOV(oldFov);
	g_view->SetScreenCenter(g_view->GetWidth() * .5f,
		g_view->GetHeight() * .5f);
	g_view->UpdateCamera();
	g_view->SetFogEnable(fog);
}
