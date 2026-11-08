#include "stdafx.h"
#include "mathconsts.h"
#include "wind.h"
#include "fx.h"
#include "fxsequence.h"
#include "golfball.h"
#include "quadtree.h"
#include "cardsystem.h"
#include "weather.h"
#include "slicestr.h"
#include "s5/utilities.h"
#include "s5/classicserver/classicserver.h"

extern WMatrix g_camera;
extern LightSet g_lightset;

CWind& Wind()
{
	static CWind wind;
	return wind;
}

CWind::CWind()
{
	m_bActive = true;
	m_minIntensity = 1;
	m_maxIntensity = 9;
	m_intensity = 0;
	m_direction = 0;
	m_spIntensity = -1;
	m_spDirection = 0;
	m_bSpecialWind = false;
	m_camera = RotMat(-30 * g_DEGTORAD, 0);
	m_camera.pivot =
		(-g_view->GetWidth() * 0.265625f) * RotMat(-30 * g_DEGTORAD, 0).za;
	m_light.type = 2;
	m_light.ambient = 0xffffff;
	m_light.diffuse = 0xffffff;
	m_light.nearone = WVector(0, -1, 1).Normalize();
	m_pFx = NULL;
}

void CWind::SetSpecialWind(bool special)
{
	if (m_bSpecialWind != special)
	{
		m_bSpecialWind = special;
		if (special)
		{
			CFx::Instance()->UpdateExtWind(m_specialWind);
			g_audio->PlaySfx("\xb7\xb9\xc0\xcc\xc1\xae\xb9\xdf\xbb\xe7", NULL,
				0, "\xb7\xb9\xc0\xcc\xc1\xae\xb9\xdf\xbb\xe7");
		}
		else
			CFx::Instance()->UpdateExtWind(m_globalWind);
	}
}

void CWind::GenerateFx(bool generate)
{
	if (!generate)
	{
		if (m_pFx)
		{
			CFx::Instance()->CloseSequence(m_pFx, false);
			m_pFx = NULL;
		}
	}
	else if (!m_pFx)
	{
		Attacher attacher;
		attacher.mat[0] = &m_fxMatrix;
		attacher.pos[0] = &m_fxPos;
		m_pFx = (CFxSequence*)CFx::Instance()->Attach(attacher,
			"SilviaCannon.seq", false, NULL);
		if (m_pFx)
			m_pFx->SetLight(g_lightset);
		g_audio->PlaySfx("\xbf\xa1\xb3\xca\xc1\xf6\xc1\xfd\xbe\xe0", NULL, 0,
			"\xbf\xa1\xb3\xca\xc1\xf6\xc1\xfd\xbe\xe0");
	}
}

float UnusedFunction()
{
	return 0.0f;
}

void CWind::CheckSpecialWind()
{
	WPolySoup::sArea* area = GOLFDOC()->m_pPolySoup->FindArea("silviacannon");
	if (area)
	{
		char name[64];
		WVector rot;
		WSliceStr slice(area->name);
		slice.Get("*fx", "%s %f %f %f", name, &rot.x, &rot.y, &rot.z);
		m_spDirection = rot.y;
		unsigned long random =
			Doc()->m_holeRandom[GOLFDOC()->m_currentHole - 1];
		m_spIntensity = (random & 3) + 6;
		m_fxMatrix.Reset();
		m_fxMatrix.Rotate(rot);
		m_fxMatrix.pivot = (area->aabb.min + area->aabb.max) * 0.5f;
		m_fxPos.Reset();
		m_specialWind = (float)m_spIntensity * RotMat(m_spDirection, 1).xa;
	}
	else
		m_spIntensity = -1;
	m_bSpecialWind = false;
	if (m_pFx)
	{
		if (CFx::Instance()->FindSequence(m_pFx))
		{
			if (m_pFx->m_filename == "SilviaCannon.seq")
				CFx::Instance()->CloseSequence(m_pFx, false);
		}
		m_pFx = NULL;
	}
}

void CWind::Init(unsigned char intensity, unsigned char direction,
	unsigned char hole, unsigned char absolute)
{
	if (Doc()->m_gameMode != 5)
	{
		if (intensity == 255)
			intensity = rand() % 9;
		if (direction == 255)
			direction = rand() % 255;
		if (GOLFDOC()->m_tutorialMode < 15 && GOLFDOC()->m_tutorialMode != 8)
			intensity = 0;
		if (absolute)
		{
			GetPVS().SetRandSeed(direction);
			if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
				m_intensity = hole * 0.1f + intensity + 1;
			else
				m_intensity = intensity + 1.0f;
			m_intensity = Between(1.0f, (float)m_intensity, 9.0f);
			m_direction = direction * (g_2_PI / 255);
			m_roll = 0;
			m_heading = 0;
		}
		else
		{
			m_intensity += (float)((intensity >> 1) - 2);
			m_intensity = Between(1.0f, (float)m_intensity, 9.0f);
			m_direction += (direction - 128) * (20 * g_DEGTORAD / 128);
			if (m_direction > g_2_PI)
				m_direction -= g_2_PI;
			else if (m_direction < 0.0f)
				m_direction += g_2_PI;
			if (GOLFDOC()->m_pPolySoup)
				GOLFDOC()->m_pPolySoup->UpdateBgFxWind();
		}
	}
	else
	{
		if (intensity != 255 && direction != 255)
		{
			GetPVS().SetRandSeed(direction);
			m_intensity = intensity + 1.0f;
			m_intensity = Between(1.0f, (float)m_intensity, 9.0f);
			m_direction = direction * (g_2_PI / 255);
			m_roll = 0;
			m_heading = 0;
		}
	}
	m_globalWind = (float)m_intensity * RotMat(m_direction, 1).za;
	if (IsLocalContent((localContentType_t)135))
		m_globalWindRef = m_globalWind;
}

void CWind::Process(float delta)
{
	if (HasSpecialWind())
	{
		WVector distance = m_fxMatrix.pivot - g_camera.pivot;
		if (g_audio->IsPlaying("\xb7\xb9\xc0\xcc\xc1\xae\xb9\xdf\xbb\xe7"))
			g_audio->SetSFXVolumeWhilePlaying(
				"\xb7\xb9\xc0\xcc\xc1\xae\xb9\xdf\xbb\xe7",
				Max(0.0f, 1.0f - distance.Magnitude() / 960));
		if (g_audio->IsPlaying("\xbf\xa1\xb3\xca\xc1\xf6\xc1\xfd\xbe\xe0"))
			g_audio->SetSFXVolumeWhilePlaying(
				"\xbf\xa1\xb3\xca\xc1\xf6\xc1\xfd\xbe\xe0",
				Max(0.0f, 1.0f - distance.Magnitude() / 960));
	}
	if (GolfBall().m_playFrame < 0)
		m_wind = GetWind(g_camera.pivot);
	else
		m_wind = GetWind(GolfBall().m_pos);
	if (WVectorLen(m_wind) < 0.1f)
		return;
	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
	{
		m_roll += S5::CLASSICSRV::WindRollSpeed(m_wind.Magnitude()) * delta;
		S5::CLASSICSRV::WindAdditionalPitchAngleProcess(delta);
	}
	else
	{
		m_roll += (WVectorLen(m_wind) * 2 - 1) * delta * (g_PI * 0.15f);
	}
	if (m_roll > g_2_PI)
		m_roll -= g_2_PI;
	float angle;
	if (GOLFDOC()->m_gameMode & 0x200)
		angle = CalcDeltaAngle(m_wind, GOLFDOC()->m_cameraData.mat.za);
	else
		angle = CalcDeltaAngle(m_wind, g_camera.za);
	float diff = GetBaseAngle(angle - m_heading);
	if (Abs(diff) > g_DEGTORAD)
		m_heading = GetBaseAngle(
			m_heading + Between(-delta * g_PI, diff, delta * g_PI));
}

void CWind::SetWind(const WVector& wind)
{
	m_direction = CalcDeltaAngle(wind, WVector::UNIT_POS_Z);
	SetIntensity(wind.Magnitude());
	if (CSceneManager::Instance()->m_pWeather)
		CSceneManager::Instance()->m_pWeather->UpdateWind();
}

void CWind::PlusWind(const WVector& wind)
{
	SetWind(m_globalWind + wind);
}

WVector CWind::GetGlobalWind()
{
	if (!m_bActive)
		return WVector::ZERO;

	WVector wind;

	if (IsLocalContent((localContentType_t)135))
	{
		if (m_globalWindRef == m_globalWind)
		{
			if (m_bSpecialWind)
				wind = m_specialWind;
			else
				wind = m_globalWind;
		}
		else
		{
			m_globalWind = m_globalWindRef;
			if (m_bSpecialWind)
				wind = m_specialWind;
			else
				wind = m_globalWind;
		}
	}
	else if (m_bSpecialWind)
	{
		wind = m_specialWind;
	}
	else
	{
		wind = m_globalWind;
	}

	if (IsLocalContent((localContentType_t)81) && Doc()->m_gameMode == 1)
	{
		float intensity =
			wind.Magnitude() - CCardManager::Instance()->GetWindPowerDown();
		if (intensity < 1)
			intensity = 1;
		wind.Normalize();
		wind *= intensity;
	}

	if (GOLFDOC()->m_playerNum)
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000006 ||
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000020)
		{
			wind.Normalize();
			if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
			{
				float factor = m_intensity - floorf(m_intensity) + 1;
				wind *= factor;
			}
		}
		else if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000016)
		{
			wind.x = -wind.x;
			wind.y = -wind.y;
			wind.z = -wind.z;
		}
	}

	return wind;
}

float CWind::GetGlobalIntensity()
{
	if (!m_bActive)
		return 0;
	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000006 ||
		PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000020)
	{
		if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
			return m_intensity - floorf(m_intensity) + 1;
		else
			return 1;
	}
	else if (IsLocalContent((localContentType_t)81) && Doc()->m_gameMode == 1)
	{
		float intensity = m_intensity;
		if (m_bSpecialWind)
			intensity = m_spIntensity;
		intensity -= CCardManager::Instance()->GetWindPowerDown();
		if (intensity < 1)
			intensity = 1;
		return intensity;
	}
	else if (m_bSpecialWind)
	{
		return m_spIntensity;
	}
	else
	{
		return m_intensity;
	}
}

float CWind::GetGlobalDirection()
{
	if (m_bSpecialWind)
		return m_spDirection;
	if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000016)
		return m_direction * -1.0f;
	return m_direction;
}

void CWind::SetIntensity(float intensity)
{
	if (Doc()->m_gameMode == 5 && m_bSpecialWind)
	{
		WPolySoup::sArea* area =
			GOLFDOC()->m_pPolySoup->FindArea("silviacannon");
		if (area)
		{
			char name[64];
			WVector rot;
			WSliceStr slice(area->name);
			slice.Get("*fx", "%s %f %f %f", name, &rot.x, &rot.y, &rot.z);
			m_spDirection = rot.y;
			m_spIntensity = intensity;
			m_fxMatrix.Reset();
			m_fxMatrix.Rotate(rot);
			m_fxMatrix.pivot = (area->aabb.min + area->aabb.max) * 0.5f;
			m_fxPos.Reset();
			m_specialWind = (float)m_spIntensity * RotMat(m_spDirection, 1).xa;
		}
	}
	else
	{
		m_intensity = intensity;
		m_globalWind = (float)m_intensity * RotMat(m_direction, 1).za;
		if (IsLocalContent((localContentType_t)135))
			m_globalWindRef = m_globalWind;
		std::vector<sWind>::iterator it;
		for (it = m_windList.begin(); it != m_windList.end(); ++it)
		{
			(*it).wind.Normalize();
			if ((*it).type == 1)
				(*it).wind *= m_intensity * (*it).intensity;
			else
				(*it).wind *= m_intensity + (*it).intensity;
		}
	}
}

void CWind::SetDirection(float direction)
{
	if (Doc()->m_gameMode == 5 && m_bSpecialWind)
	{
		WPolySoup::sArea* area =
			GOLFDOC()->m_pPolySoup->FindArea("silviacannon");
		if (area)
		{
			char name[64];
			WVector rot;
			WSliceStr slice(area->name);
			slice.Get("*fx", "%s %f %f %f", name, &rot.x, &rot.y, &rot.z);
			m_spDirection = direction;
			m_fxMatrix.Reset();
			m_fxMatrix.Rotate(rot);
			m_fxMatrix.pivot = (area->aabb.min + area->aabb.max) * 0.5f;
			m_fxPos.Reset();
			m_specialWind = (float)m_spIntensity * RotMat(rot.y, 1).xa;
		}
	}
	else
	{
		m_direction = direction;
		m_globalWind = (float)m_intensity * RotMat(m_direction, 1).za;
		if (IsLocalContent((localContentType_t)135))
			m_globalWindRef = m_globalWind;
	}
}

void CWind::MakeWindList()
{
	if (!OnlinePlay() && Doc()->GetPlayingMode() != 2)
		Wind().Init(255, 255, 0, 1);
	m_windList.clear();
	for (int i = 0; i < GOLFDOC()->m_pPolySoup->GetAreaNum(); ++i)
	{
		WPolySoup::sArea* area = GOLFDOC()->m_pPolySoup->GetArea(i);
		if (area->type == 4)
			AddWindArea(area->aabb, area->name);
	}
}

void CWind::AddWindArea(const Waabb& area, char* param)
{
	sWind wind;
	wind.index = m_windList.size();
	wind.area = area;
	wind.link = -1;
	wind.pitch = 0;
	wind.yaw = 0;
	char difficulty[8] = "hard";
	switch (*param)
	{
	case '*':
		sscanf(param + 1, "%f %f %f %s", &wind.intensity, &wind.pitch,
			&wind.yaw, difficulty);
		wind.type = 1;
		wind.wind = (m_intensity * wind.intensity) *
			(RotMat(wind.yaw * g_DEGTORAD, 0) *
				RotMat(wind.pitch * g_DEGTORAD, 1))
				.za;
		break;
	case '+':
		sscanf(param + 1, "%f %f %f %s", &wind.intensity, &wind.pitch,
			&wind.yaw, difficulty);
		wind.type = 0;
		wind.wind = (m_intensity + wind.intensity) *
			(RotMat(wind.yaw * g_DEGTORAD, 0) *
				RotMat(wind.pitch * g_DEGTORAD, 1))
				.za;
		break;
	case '-':
	{
		sscanf(param + 1, "%f %f %f %s", &wind.intensity, &wind.pitch,
			&wind.yaw, difficulty);
		wind.type = 2;
		float intensity = Max(0.0f, m_intensity - wind.intensity);
		wind.wind = intensity *
			(RotMat(wind.yaw * g_DEGTORAD, 0) *
				RotMat(wind.pitch * g_DEGTORAD, 1))
				.za;
		break;
	}
	case '=':
		sscanf(param + 1, "%f %f %f %s", &wind.intensity, &wind.pitch,
			&wind.yaw, difficulty);
		wind.type = 3;
		wind.wind = wind.intensity *
			(RotMat(wind.yaw * g_DEGTORAD, 0) *
				RotMat(wind.pitch * g_DEGTORAD, 1))
				.za;
		break;
	default:
		sscanf(param, "%f %f %f %s", &wind.intensity, &wind.pitch, &wind.yaw,
			difficulty);
		wind.type = 0;
		wind.wind = (m_intensity + wind.intensity) *
			(RotMat(wind.yaw * g_DEGTORAD, 0) *
				RotMat(wind.pitch * g_DEGTORAD, 1))
				.za;
		break;
	}
	switch (GetDiffFlag())
	{
	case 0:
		if (strcmpi(difficulty, "normal"))
			return;
		break;
	case 1:
		if (!strcmpi(difficulty, "hard"))
			return;
		break;
	case 2:
		if (!strcmpi(difficulty, "easy"))
			return;
		break;
	}
	std::vector<sWind>::iterator it;
	for (it = m_windList.begin(); it != m_windList.end(); ++it)
	{
		if (wind.area & (*it).area)
		{
			wind.link = (*it).index;
			(*it).link = wind.index;
		}
	}
	m_windList.push_back(wind);
}

bool CWind::IsInclude(const Waabb& area, const WVector& pos)
{
	return area.IsInclude(pos);
}

WVector CWind::GetWind(const WVector& pos)
{
	if (!m_bActive)
		return WVector::ZERO;
	WVector wind;
	if (IsLocalContent((localContentType_t)135))
	{
		if (m_globalWindRef == m_globalWind)
		{
			if (m_bSpecialWind)
				wind = m_specialWind;
			else
				wind = m_globalWind;
		}
		else
		{
			m_globalWind = m_globalWindRef;
			if (m_bSpecialWind)
				wind = m_specialWind;
			else
				wind = m_globalWind;
		}
	}
	else if (m_bSpecialWind)
	{
		wind = m_specialWind;
	}
	else
	{
		wind = m_globalWind;
	}
	std::vector<sWind>::iterator it;
	for (it = m_windList.begin(); it != m_windList.end(); ++it)
	{
		if (IsInclude((*it).area, pos))
		{
			if ((*it).link >= 0)
			{
				const sWind& current = *it;
				const Waabb& otherArea = m_windList[current.link].area;
				sWind& other = m_windList[current.link];
				if (IsInclude(otherArea, pos))
				{
					WVector distance1 =
						(current.area.min + current.area.max) * 0.5f - pos;
					WVector distance2 =
						(other.area.min + other.area.max) * 0.5f - pos;
					WVector extent1 = current.area.max - current.area.min;
					WVector extent2 = other.area.max - other.area.min;
					float a =
						1 - 2 * WVectorLen(distance1) / WVectorLen(extent1);
					float b =
						1 - 2 * WVectorLen(distance2) / WVectorLen(extent2);
					float scale = 1 / (a + b);
					wind = (*it).wind * (scale * a) + other.wind * (scale * b);
					break;
				}
			}
			wind = (*it).wind;
			break;
		}
	}
	if (IsLocalContent((localContentType_t)81) && Doc()->m_gameMode == 1)
	{
		float intensity =
			wind.Magnitude() - CCardManager::Instance()->GetWindPowerDown();
		if (intensity < 1)
			intensity = 1;
		wind.Normalize();
		wind *= intensity;
	}
	if (GOLFDOC()->m_playerNum)
	{
		if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000006 ||
			PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000020)
		{
			wind.Normalize();
			if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
			{
				float factor = m_intensity - floorf(m_intensity) + 1;
				wind *= factor;
			}
		}
		else if (PLAYER(GOLFDOC()->m_currentPlayer)->item == 0x18000016)
		{
			wind.x = -wind.x;
			wind.y = -wind.y;
			wind.z = -wind.z;
		}
	}
	return wind;
}

bool CWind::IsInTurbulence(const WVector& pos)
{
	std::vector<sWind>::iterator it;
	for (it = m_windList.begin(); it != m_windList.end(); ++it)
		if (IsInclude((*it).area, pos))
			return true;
	return false;
}

bool CWind::IsInTurbulence(const WVector& from, const WVector& to)
{
	std::vector<sWind>::iterator it;
	for (it = m_windList.begin(); it != m_windList.end(); ++it)
	{
		if (!(Min(from.x, from.x + to.x) > (*it).area.max.x) &&
			!(Max(from.x, from.x + to.x) < (*it).area.min.x) &&
			!(Min(from.z, from.z + to.z) > (*it).area.max.z) &&
			!(Max(from.z, from.z + to.z) < (*it).area.min.z))
			return true;
	}
	return false;
}

void CWind::Display(WPuppet* puppet, float x, float y)
{
	if (!puppet)
		return;
	if (WVectorLen(m_wind) < 0.1f)
		return;
	float ratio = g_view->GetRatio();
	float fov = g_view->GetFOV();
	g_view->SetFOV(ratio * (120 * g_DEGTORAD));
	if (GOLFDOC()->m_gameMode & 0x200)
		m_camera = RotMat(-90 * g_DEGTORAD, 0);
	else
		m_camera = RotMat(-30 * g_DEGTORAD, 0);
	m_camera.pivot = m_camera.za * (-g_view->GetWidth() * 0.265625f);
	g_view->SetCamera(m_camera);
	g_view->SetScreenCenter(x, y);
	g_view->UpdateCamera();
	float angle = 0;
	if (S5::CLASSICSRV::IsClassicServer(S5::UTIL::GetServerProperty()))
		angle = S5::CLASSICSRV::GetWindAdditionalPitchAngle() * g_DEGTORAD;
	if (Wabs(m_wind.y) < g_EPSILON)
	{
		puppet->ApplyBones(0, RotMat(m_roll, 2) * RotMat(m_heading + angle, 1));
	}
	else
	{
		float pitch = m_wind.y > 0 ? (g_PI / 2) : -(g_PI / 2);
		puppet->ApplyBones(0,
			RotMat(m_roll, 2) * RotMat(pitch, 0) *
				RotMat(m_heading + angle, 1));
	}
	puppet->Transform(NULL, 1.0f, false);
	puppet->UpdateLightSource(&m_light, false, false);
	puppet->Render(g_view, true, 1.0f, true, false, false);
	g_view->SetCamera(g_camera);
	g_view->SetFOV(fov);
	g_view->ResetScreenCenter();
	g_view->UpdateCamera();
}

void CWind::DrawWindArea(WView* view)
{
	std::vector<sWind>::iterator it;
	for (it = m_windList.begin(); it != m_windList.end(); ++it)
		view->DrawAABB((*it).area, 0xffff0000, 0, 0);
}

bool CWind::IsGlobalWind_Compare()
{
	return m_globalWind == m_globalWindRef;
}
