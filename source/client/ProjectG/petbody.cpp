#include "texturepart.h"
#include "petbody.h"
#include "slicestr.h"
#include "wbone.h"
#include "rtti.h"
#include "taskmanager.h"
#include "quadtree.h"
#include "golfball.h"
#include "golftask.h"
#include "shadow.h"
#include "fx.h"
#include "soundmanager.h"
#include "commonutil.h"
#include "projectg.h"
#include "mathconsts.h"

extern WView* g_view;
extern WResourceManager* g_resrcmng;

static WMatrix s_headMat(0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
CPetBody::CPetBody(const char* script, bool bOption)
	: m_scriptHandle(8, 0)
{
	Init();
	InitScript(script, bOption);
}

void CPetBody::Init()
{
	m_pMotion = NULL;
	m_pPet = NULL;
	m_pLookAt = NULL;
	m_time = 0.0f;
	m_scale = 1.0f;
	m_moveScale = 1.0f;
	m_aniSpeed = 1.0f;
	m_flightState = 2;
	m_bFly = false;
	m_nextMotion[0] = '\0';
	m_customGround = "";

	m_rot.Reset();
	m_pos.Reset();
	m_vel.Reset();
	m_mat.Reset();
	m_leanMat.Reset();

	m_light.type = 2;
	m_bGlobalLight = m_bLocalLight = true;

	m_lookAngle.Reset();
	m_lookDest.Reset();
}

CPetBody::~CPetBody()
{
	for (ScriptHandle* handle = m_scriptHandle.Start(); handle;
		handle = m_scriptHandle.Next())
		delete handle;
	m_scriptHandle.Reset();

	for (TexPartIter it = m_texPart.begin(); it != m_texPart.end(); it++)
		delete *it;
	m_texPart.clear();

	if (m_pPet)
	{
		g_resrcmng->Release(m_pPet, false);
		m_pPet = NULL;
	}

	m_localLight.clear();
}

void CPetBody::InitScript(const char* script, bool bOption)
{
	WSliceStr slice(script);
	char name[256];
	int shadow = 0;

	if (slice.Get("*pet", "%s", name))
		LoadPuppet(name, false, bOption);

	if (slice.Get("*shadow", "%d", &shadow))
		SetShadowType(shadow);
}

void CPetBody::Reset()
{
	m_flags.Reset();
	m_pLookAt = NULL;
	m_lookDest.Reset();
	m_lookAngle.Reset();

	m_moveScale = 1.0f;
	m_aniSpeed = 1.0f;
	m_nextMotion[0] = '\0';
}

void CPetBody::SetCustomGround(char* name)
{
	m_customGround = name;
}

void CPetBody::Display()
{
	if (!ShouldDisplay())
		return;

	if (m_flags.GetFlag(FLAG_PLAY | FLAG_UPDATE))
	{
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time,
							   m_flags.GetFlag(FLAG_MOVE) ? 1 : 0, NULL, false),
			m_leanMat);
		m_pPet->Transform(NULL, 1.0f, 0);
		m_pPet->UpdateBSphere();
	}

	const WSphere& sphere = m_pPet->GetBoundSphere();
	bool bVisible = g_view->InFrustum(sphere);
	if (bVisible)
	{
		if (m_flags.GetFlag(FLAG_PLAY | FLAG_UPDATE | FLAG_LOOKUP))
		{
			if (m_pPet->FindBoneByTail("Head"))
				m_pPet->FindBoneByTail("Head")->SetMatrix(
					~s_headMat * GetMatrix(BODY_HEAD), NULL);
		}

		if (m_flags.GetFlag(FLAG_PLAY | FLAG_UPDATE))
		{
			if (m_bGlobalLight)
				m_pPet->UpdateLightSource(&m_light, false, NULL);

			if (m_bLocalLight)
			{
				std::vector<sLightSet>::iterator it;
				for (it = m_localLight.begin(); it != m_localLight.end(); it++)
				{
					it->light.nearone = RotVec(it->pos, m_leanMat);
					m_pPet->UpdateLightSource(&it->light, true, NULL);
				}
			}
		}

		m_pPet->Render(g_view, true, m_scale, g_view->InFrustumSafe(sphere),
			false, false);
	}

	m_flags.Disable(FLAG_UPDATE | FLAG_LOOKUP);
}

void CPetBody::DisplayShadow(char* recvName)
{
	if (!ShouldDisplay())
		return;

	if (m_flags.GetFlag(FLAG_PLAY | FLAG_UPDATE))
	{
		m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time,
							   m_flags.GetFlag(FLAG_MOVE) ? 1 : 0, NULL, false),
			m_leanMat);
		m_pPet->Transform(NULL, 1.0f, 0);
		m_pPet->UpdateBSphere();
	}

	if (m_flags.GetFlag(FLAG_PLAY | FLAG_UPDATE | FLAG_LOOKUP))
	{
		if (m_pPet->FindBoneByTail("Head"))
			m_pPet->FindBoneByTail("Head")->SetMatrix(
				~s_headMat * GetMatrix(BODY_HEAD), NULL);
	}

	CShadowManager::Instance()->AddCastPet_New(recvName, false, m_pPet);
}

void CPetBody::SetLight()
{
	m_pPet->UpdateBound(false, false);
	m_bLocalLight = true;
	if (IS_KINDOF(CGolfTask, AfxGetTask()))
	{
		m_light.diffuse = ModifyColor(g_lightset.diffuse,
			GetPVS().GetShadowIntensity(
				WVector(m_pos.x, m_pPet->GetBoundBox().max.y, m_pos.z)));
		m_light.ambient = g_lightset.ambient;
		m_light.nearone = g_lightset.nearone;
	}
	else
	{
		m_light.diffuse = 0xffffff;
		m_light.ambient = 0xffffff;
		m_light.type = 2;
		m_light.nearone = WVector(0.0f, -1.0f, 1.0f);
		m_light.nearone.Normalize();
	}
}

void CPetBody::Process(float dt)
{
	if (m_flags.GetFlag(FLAG_PLAY))
	{
		Animation(dt * m_aniSpeed);

		if (m_flags.GetFlag(FLAG_MOVE))
		{
			if (m_flags.GetFlag(FLAG_DELTA_VALID))
			{
				m_flags.Disable(FLAG_DELTA_VALID);
				m_vel = m_moveScale * RotVec(m_delta, m_mat);
				m_pos += m_vel;
			}

			FlightProcess(dt);

			m_mat.pivot = m_pos;
			m_leanMat.pivot = m_pos;
		}
	}

	if (m_pPet->FindBoneByTail("Head"))
		LookUpProcess(dt);
}

void CPetBody::Animation(float dt)
{
	if (!m_pMotion || m_pMotion->s == m_pMotion->e)
	{
		m_flags.Disable(FLAG_MOVE | FLAG_PLAY);
		return;
	}

	float moved = Move(dt);

	if (moved > 0.0f)
	{
		ScriptProcess(m_pMotion, m_time, m_time + moved);
		m_time += moved;

		if (m_time >= m_pMotion->e)
		{
			if (m_nextMotion[0])
			{
				SetMotion(m_nextMotion, m_bNextMotionMove, dt - moved, false);
				m_nextMotion[0] = 0;
			}
			else
			{
				SetMotion(m_pMotion->nextmotion, m_flags.GetFlag(FLAG_MOVE),
					dt - moved, true);
			}

			Animation(dt - moved);
		}
	}
}

float CPetBody::Move(float dt)
{
	float end = Min(m_pMotion->end, m_time + dt);

	if (m_flags.GetFlag(FLAG_MOVE))
	{
		if (!m_flags.GetFlag(FLAG_DELTA_VALID))
		{
			m_delta = m_pPet->GetDeltaVec(m_pMotion->name,
				m_time - m_pMotion->s, end - m_pMotion->s);
			m_flags.Enable(FLAG_DELTA_VALID);
		}
		else
			m_delta += m_pPet->GetDeltaVec(m_pMotion->name,
				m_time - m_pMotion->s, end - m_pMotion->s);
	}

	return end - m_time;
}

void CPetBody::SetPos(const WVector& pos)
{
	m_pos = pos;
	m_mat.pivot = m_pos;
	m_leanMat.pivot = m_pos;
	m_flags.Enable(FLAG_UPDATE);
	m_flags.Disable(FLAG_MOVE);
}

void CPetBody::SetRotate(const WVector& rot)
{
	m_rot = rot;
	m_mat = RotMat(m_rot.x, 0) * RotMat(m_rot.y, 1) * RotMat(m_rot.z, 2);
	m_mat.pivot = m_pos;
	m_leanMat = m_mat;

	m_flags.Enable(FLAG_UPDATE);
}

void CPetBody::SetRotate_Lean(const WVector& rot)
{
	m_rot = rot;
	m_leanMat = RotMat(m_rot.x, 0) * RotMat(m_rot.y, 1) * RotMat(m_rot.z, 2);
	m_leanMat.pivot = m_pos;

	m_flags.Enable(FLAG_UPDATE);
}

void CPetBody::SetRotate(float angle)
{
	WVector pos = m_mat.pivot;
	m_rot = WVector(0.0f, angle, 0.0f);
	m_mat = RotMat(angle, 1);
	m_mat.pivot = pos;
	m_leanMat = m_mat;

	m_flags.Enable(FLAG_UPDATE);
}

void CPetBody::SetRotateSlope(float angle, WVector normal)
{
	WMatrix mat;
	WVector pos = m_mat.pivot;
	WVector dir = normal;
	dir.y = 0.0f;
	normal = WCrossProduct(normal, dir);
	normal.y *= -1.0f;

	mat = m_mat;
	m_mat = RotMat(normal, angle * 0.0034377468f);
	m_mat = mat * m_mat;

	m_mat.pivot = pos;
	m_leanMat = m_mat;

	m_flags.Enable(FLAG_UPDATE);
}

void CPetBody::MultRotate(WMatrix* mat)
{
	if (mat)
	{
		WVector pos = m_leanMat.pivot;
		WMatrix rot;

		m_leanMat.GetRotMatrix(&rot);
		m_leanMat = rot * *mat;
		m_leanMat.pivot = pos;
		m_flags.Enable(FLAG_UPDATE);
	}
}

void CPetBody::Rotate(float angle)
{
	m_rot.y += angle;
	m_mat *= RotMat(angle, 1);
	m_mat.pivot = m_pos;
	m_leanMat = m_mat;

	m_flags.Enable(FLAG_UPDATE);
}

void CPetBody::AddScriptHandle(const char* name, IActor* actor, int param)
{
	ScriptHandle* handle = new ScriptHandle;

	handle->name = name;
	handle->actor = actor;
	handle->param = param;

	m_scriptHandle += handle;
}

void CPetBody::AddLocalLight(const LightSet& light, WVector pos)
{
	sLightSet localLight;

	localLight.light = light;
	localLight.pos = pos;
	m_localLight.push_back(localLight);
}

void CPetBody::TakeOff(WVector target)
{
	m_flightState = 0;
	m_flightStart = m_pos;
	m_flightTarget = target;
	m_leanAngle = -0.7853982f;
	m_flightHeight = 0.0f;
	m_flightSpeed = 50.0f;
}

void CPetBody::LoadPuppet(const char* name, bool bOptimizeBone, bool bOption)
{
	m_pPet = g_resrcmng->GetPuppet(name, false, true, bOption);
	if (bOptimizeBone && m_pPet)
		m_pPet->OptimizeBone();

	SetScale(1.0f);
	SetPivot(NULL, WVector(0.0f, 0.0f, 0.0f));
}

void CPetBody::ScriptProcess(const w_motion_data* motion, float from, float to)
{
	if (from <= motion->e && from != to)
	{
		from *= 30.0f;
		to = Min(motion->e, to) * 30.0f;
		int first = Ceil(from, 0.03f);
		int last = to == floorf(to) ? (int)to - 1 : (int)floorf(to);

		if (first <= last)
		{
			int start, end;
			if (m_pPet->CheckFrameData(first, last, start, end))
			{
				for (int i = start; i <= end; i++)
					ParsePuppetScript(m_pPet->GetFrameData(i));
			}
		}
	}
}

void CPetBody::ParsePuppetScript(const char* script)
{
	static char* s_scriptKeyword[] = { "*ptex(", "*snd(", "*fly", "*hide",
		"*show", "*fx(", "*lookup", "*dontlookup" };
	static char s_name[64];
	static char s_token[1024];

	if (!script)
		return;

	const char* p = script;
	int length;
	while (p && (p = strchr(p, '*')) != NULL)
	{
		int len = 0;
		while (p[len])
		{
			if (p[++len] == ' ')
				break;
			if (p[len] == '(')
			{
				++len;
				break;
			}
		}
		if (len <= 1)
			break;

		strncpy(s_token, p, len);
		s_token[len] = '\0';
		p += len;

		for (unsigned int i = 0; i < 8; i++)
		{
			if (strcmpi(s_token, s_scriptKeyword[i]) != 0)
				continue;

			switch (i)
			{
			case 0:
			{
				char name[64];
				char tex[64];
				name[0] = '\0';
				p = GetStrConst(p, name);
				if (!(length = strlen(name)) || strlen(name) >= 64)
					break;

				if (name[0] == '@')
				{
					if (!m_pMotion || strcmpi(name + 1, m_pMotion->name))
						break;

					p = GetStrConst(p, s_name);
					if (!(length = strlen(s_name)) || strlen(s_name) >= 64)
						break;
				}
				else
				{
					strcpy(s_name, name);
				}

				p = GetStrConst(p, tex);
				if (!(length = strlen(tex)) || strlen(tex) >= 64)
					break;

				SetPendingTexPart(s_name, tex);
				break;
			}
			case 1:
				p = GetStrConst(p, s_name);
				if (!(length = strlen(s_name)) || strlen(s_name) >= 64)
					break;

				PlaySfx(s_name);
				break;
			case 2:
				m_bFly = true;
				break;
			case 3:
				m_flags.Enable(FLAG_HIDE);
				break;
			case 4:
				m_flags.Disable(FLAG_HIDE);
				break;
			case 5:
			{
				char name[64];
				char file[64];
				char bone[64];
				name[0] = '\0';
				p = GetStrConst(p, name);
				if (!(length = strlen(name)) || strlen(name) >= 64)
					break;

				if (name[0] == '@')
				{
					if (!m_pMotion || strcmpi(name + 1, m_pMotion->name))
						break;

					p = GetStrConst(p, file);
					if (!(length = strlen(file)) || strlen(file) >= 64)
						break;
				}
				else
				{
					strcpy(file, name);
				}

				Attacher attacher;
				int j;
				for (j = 0; j < sizeof(attacher.mat) / sizeof(attacher.mat[0]);
					j++)
				{
					p = GetStrConst(p, bone);
					if (!(length = strlen(bone)) || strlen(bone) >= 64)
						break;

					WBone* pBone = m_pPet->FindBone(bone);
					if (pBone)
					{
						attacher.mat[j] = &pBone->GetMatrix();
					}
					else
					{
						w_bound_box* box = m_pPet->m_bbList.Find(bone);
						if (box)
						{
							if (box->bone)
								attacher.mat[j] = &box->bone->GetMatrix();
							attacher.pos[j] = &box->info->spherePivot;
						}
					}
				}
				if (j > 0)
					CFx::Instance()->Attach(attacher, file, true, NULL);
				break;
			}
			}
			break;
		}
	}

	ScriptHandle* h = m_scriptHandle.Start();
	while (h)
	{
		if (const char* found = strstr(script, h->name))
		{
			MsgObject msg;
			msg.message = h->param;
			msg.sender = NULL;
			msg.time = 0;
			msg.param1 = (int)found;
			h->actor << msg;
		}
		h = m_scriptHandle.Next();
	}
}

sTexturePart* CPetBody::FindTexturePart(char* primary, char* secondary)
{
	sTexturePart* part;
	TexPartIter it;
	it = std::find_if(m_texPart.begin(), m_texPart.end(),
		sFindTexPart(secondary));
	if (it != m_texPart.end())
	{
		part = *it;
	}
	else
	{
		part = new sTexturePart;
		strcpy(part->name, primary);
		strcpy(part->part, secondary);
		m_texPart.push_back(part);
	}
	return part;
}

bool CPetBody::ShouldDisplay()
{
	return m_flags.GetFlag(FLAG_HIDE) ? false : true;
}

void CPetBody::SetPendingTexPart(char* primary, char* secondary)
{
	sTexturePart* part = FindTexturePart(primary, secondary);

	if (part->bitmap == NULL)
	{
		m_pPet->ChangeTexturePart(part->name, part->part, &part->bitmap,
			&part->rect);
	}
	else
	{
		m_pPet->ChangeTexturePart(part->name, part->bitmap, part->rect);
	}
}

void CPetBody::PlaySfx(char* name)
{
	char buf[64];
	WVector pos;
	int score;

	if (name)
	{
		pos = m_pos - g_camera.pivot;
		char* p = strchr(name, '-');

		if (p)
		{
			p++;
			int len = p - name;

			if (strncmp(p, "\xc2\xf9\xbd\xba", 4) == 0)
			{
				if (Doc()->m_golfGame.gameType == GAME_TYPE_TEAM)
				{
					score = (unsigned char)TEAM(
								PLAYER(GOLFDOC()->m_currentPlayer)->team)
								->stroke[GOLFDOC()->m_currentHole - 1] -
						GOLFDOC()->GetHoleData().par;
				}
				else
				{
					score = (unsigned char)PLAYER(GOLFDOC()->m_currentPlayer)
								->stroke[GOLFDOC()->m_currentHole - 1] -
						GOLFDOC()->GetHoleData().par;
				}

				if (GolfBall().m_groundType == 2 &&
					(score == -2 || score == -1))
				{
					strncpy(buf, name, len);

					switch (score)
					{
					case -2:
						strcpy(buf + len, "\xc0\xcc\xb1\xdb");
						break;

					case -1:
						strcpy(buf + len, "\xb9\xf6\xb5\xf0");
						break;
					}

					strcat(buf, name + len);
					strcpy(name, buf);
				}
			}
		}

		if (IS_KINDOF(CGolfTask, AfxGetTask()))
			g_audio->PlaySfx(name, &pos, 5);
		else
			g_audio->PlaySfx(name);
	}
}

void CPetBody::SetPivot(const char* name, WVector offset)
{
	if (!m_pPet)
		return;
	const WVector& off = offset;
	WVector pivot = m_pPet->GetPivot(name);
	pivot += off;
	pivot *= m_scale;

	m_pos -= pivot;
}

void CPetBody::SetShadowType(int type)
{
	m_shadowType = type;
}

void CPetBody::SetMotion(const char* name, bool bMove, float time,
	bool bApplyDelta)
{
	WVector delta;

	const w_motion_data* prev = m_pMotion;
	m_pMotion = m_pPet->FindMotionData(name);

	if (m_pMotion)
	{
		m_bFly = false;
		m_time = Min(time + m_pMotion->s, m_pMotion->e);
		m_time = Max(0.0f, m_time);

		if (m_time > m_pMotion->s)
			ScriptProcess(m_pMotion, m_pMotion->s, m_time);

		m_flags.Enable(FLAG_PLAY);

		if (bMove)
		{
			m_flags.Enable(FLAG_MOVE);
		}
		else
		{
			if (prev && bApplyDelta && !WisZero(m_pMotion->e - m_pMotion->s))
			{
				delta = m_pPet->GetDeltaVec(prev->name, prev->e - prev->s);
				delta = m_moveScale * RotVec(delta, m_mat);
				SetPos(delta + m_pos);
			}

			m_flags.Disable(FLAG_MOVE);
		}
	}
}

void CPetBody::SetScale(float scale)
{
	m_scale = scale;
	m_leanMat.AxisScale(WVector(scale, scale, scale));

	if (m_pPet)
	{
		m_pPet->GetMatrix().AxisScale(WVector(scale, scale, scale));
	}
}

void CPetBody::ChangeTexture(char* name, int index)
{
	m_pPet->ChangeTexture(name, index);
}

WMatrix CPetBody::GetMatrix(eBody body)
{
	WMatrix mat;

	switch (body)
	{
	case BODY_HEAD:
		mat = RotMat(m_lookAngle.x, 0) * RotMat(m_lookAngle.y, 1) * s_headMat *
			m_pPet->FindBoneByTail("Head")->GetMatrix();
		break;
	case BODY_ROOT:
		mat = m_pPet->GetMatrix();
		break;
	case BODY_ROOT_POS:
		mat = m_pPet->GetMatrix();
		mat.pivot = m_pos;
		break;
	}

	return mat;
}

void CPetBody::LookUpProcess(float dt)
{
	if (m_pLookAt)
	{
		WVector dir =
			*m_pLookAt - m_pPet->FindBoneByTail("Head")->GetMatrix().pivot;
		m_lookDest.x = Between(-0.34906584f,
			atanf(dir.y / sqrtf(dir.x * dir.x + dir.z * dir.z)), g_PI / 4);

		m_lookDest.y = CalcDeltaAngle(*m_pLookAt - m_mat.pivot, GetDirection());
		if (m_bLookTurn && Abs(m_lookDest.y) > g_PI / 4)
		{
			float over = Abs(m_lookDest.y) - g_PI / 4;
			SetRotate(WVector(m_rot.x,
				(m_lookDest.y > 0 ? over : -over) + m_rot.y, m_rot.z));
		}

		m_lookDest.y = Between(-g_PI / 4, m_lookDest.y, g_PI / 4);
	}
	else
	{
		m_lookDest.x = m_lookDest.y = 0.0f;
	}

	WVector2D diff = m_lookDest - m_lookAngle;

	if (Abs(diff.x) > g_DEGTORAD)
	{
		m_flags.Enable(FLAG_LOOKUP);
		m_lookAngle.x += diff.x * Min(dt / diff.Magnitude() * (g_PI / 2), 1.0f);
	}

	if (Abs(diff.y) > g_DEGTORAD)
	{
		m_flags.Enable(FLAG_LOOKUP);
		m_lookAngle.y += diff.y * Min(dt / diff.Magnitude() * (g_PI / 2), 1.0f);
	}
}

void CPetBody::FlightProcess(float dt)
{
	CGolfBall& ball = GolfBall();
	WVector& pos = m_pos;
	WVector cur = pos;

	float total = (m_flightTarget - m_flightStart).Magnitude();
	float dist = WVector(m_flightTarget.x - cur.x, m_flightTarget.y,
		m_flightTarget.z - cur.z)
					 .Magnitude();

	switch (m_flightState)
	{
	case 0:
		if (m_bFly)
		{
			if (!(Wabs(m_leanAngle) < g_EPSILON))
			{
				if (m_leanAngle + dt * 0.4f < 0.0f)
				{
					m_leanAngle += dt * 0.4f;
				}
				else
				{
					m_leanAngle = 0.0f;
				}

				float yaw = CalcDeltaAngle(GetDirection(), WVector::UNIT_NEG_Z);
				SetRotate_Lean(WVector(m_leanAngle, yaw, 0.0f));
			}

			if (ball.m_pos.y > m_flightHeight)
			{
				m_flightHeight = ball.m_pos.y;
			}

			if (m_pos.y < m_flightHeight)
			{
				m_pos.y += dt * m_flightSpeed;

				if (m_flightSpeed > 0.0f)
				{
					m_flightSpeed -= dt * 25.0f;
				}
			}

			if (m_flightHeight + 10.0f < m_pos.y)
			{
				m_pos.y -= dt * 10.0f;
			}

			if (dist * 1.5f <= total)
			{
				m_flightState = 1;
				m_leanAngle = 0.0f;
				RestoreLeanMat();
			}
		}
		else
		{
			m_flightState = 1;
			m_leanAngle = 0.0f;
			RestoreLeanMat();
		}
		break;

	case 1:
		if (!m_bFly || dist * 3.0f <= total)
		{
			m_flightState = 2;
		}
		break;

	case 2:
	{
		float drop = 0.0f;
		if (!m_customGround.empty())
		{
			w_bound_box* box = m_pPet->FindBoundBox(m_customGround.c_str());
			if (box)
			{
				Waabb aabb = box->info->aabb;
				aabb.min = aabb.min * box->bone->GetMatrix();
				aabb.max = aabb.max * box->bone->GetMatrix();
				aabb.min.x = (aabb.min.x + aabb.max.x) * 0.5f;
				aabb.min.z = (aabb.min.z + aabb.max.z) * 0.5f;

				drop = aabb.min.y -
					GetPVS().GetGroundPoint(aabb.min, true, NULL, true).y;
			}
		}

		if (m_bFly)
		{
			if (!WisEqual(m_leanAngle, -g_PI / 8, g_EPSILON))
			{
				if (m_leanAngle - dt * 0.3f > -g_PI / 8)
				{
					m_leanAngle -= dt * 0.3f;
				}
				else
				{
					m_leanAngle = -g_PI / 8;
				}

				float yaw = CalcDeltaAngle(GetDirection(), WVector::UNIT_NEG_Z);
				SetRotate_Lean(WVector(m_leanAngle, yaw, 0.0f));
			}
		}
		else
		{
			m_leanAngle = 0.0f;
			RestoreLeanMat();
		}

		drop = Min(dt * 36.0f, drop);

		m_pos.y -= drop;
		break;
	}
	}

	if (m_bFly && !(Wabs(ball.m_initCurve) < g_EPSILON))
	{
		WVector side = WCrossProduct(WVector::UNIT_POS_Y, m_vel);
		side.Normalize();

		float d = (ball.m_pos - pos) * side;
		pos += side * d * 1.2f * dt;
	}
}

void CPetBody::LookUp(WVector* target, bool bTurn)
{
	if (m_pPet->FindBoneByTail("Head"))
	{
		m_pLookAt = target;
		m_bLookTurn = bTurn;
	}
}

float CPetBody::GetAnimationLength(const char* name)
{
	const w_motion_data* motion = m_pPet->FindMotionData(name);

	return motion->e - motion->s;
}

void CPetBody::SetNextMotion(const char* name, bool bMove)
{
	m_bNextMotionMove = bMove;
	strcpy(m_nextMotion, name);
}

void CPetBody::Update()
{
	m_pPet->ApplyBones(m_pPet->GetBones(NULL, m_time,
						   m_flags.GetFlag(FLAG_MOVE) ? 1 : 0, NULL, false),
		m_leanMat);
	m_pPet->Transform(0, 1.0f, 0);
	GetPet()->UpdateBSphere();
}

WPuppet* CPetBody::GetPet()
{
	return m_pPet;
}
