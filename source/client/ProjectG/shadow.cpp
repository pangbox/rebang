#include "stdafx.h"
#include "mathconsts.h"
#include "shadow.h"
#include <set>
#include "wshadowview.h"

namespace
{
	class DisableShadowPet
	{
	public:
		static DisableShadowPet* __fastcall Instance()
		{
			static DisableShadowPet _Singleton;
			return &_Singleton;
		}

		bool Exist(const char* name) const
		{
			if (!name)
			{
				return false;
			}
			std::string str(name);
			std::string::size_type pos = str.rfind('.');
			if (pos != std::string::npos)
			{
				str.erase(pos, str.length() - pos);
			}

			return m_names.end() != m_names.find(str);
		}

	private:
		DisableShadowPet()
		{
			cFile* pFile = g_resrcmng->GetCFile("club_noshadow.txt", 0xffff);
			if (!pFile)
			{
				return;
			}
			char* buf = new char[pFile->Length()];
			memset(buf, 0, pFile->Length());
			pFile->Read(buf, pFile->Length());
			std::string line;
			for (int i = 0; i < pFile->Length(); ++i)
			{
				if (buf[i] == '\n' || buf[i] == '\r')
				{
					if (!line.empty())
					{
						m_names.insert(line);
						line.clear();
					}
				}
				else
				{
					line += buf[i];
				}
			}

			if (!line.empty())
			{
				m_names.insert(line);
			}

			delete[] buf;
			CloseCFile(pFile);
		}

		std::set<std::string> m_names;
	};
}

CShadow::CShadow(WResourceManager* pResMgr, eShadowMode mode)
	: m_pShadowView(NULL), m_pRecvView(NULL), m_type(SHADOW_PROJECTION)
{
	switch (mode)
	{
	case SHADOW_MODE_SW:
		m_pShadowView = new WShadowViewSw(0x60);
		m_pRecvView = new WRecvViewSw;
		break;
	case SHADOW_MODE_HW:
		m_pShadowView = new WShadowViewHw(0x60);
		m_pRecvView = new WRecvViewHw;
		break;
	default:
		m_pShadowView = new WShadowViewHw(0x60);
		m_pRecvView = new WRecvViewHw;
		break;
	}
	m_pShadowView->SetResourceManager(pResMgr);
	if (mode == SHADOW_MODE_SW)
	{
		m_pShadowView->SetTextureSize(0x80, 0x80);
	}
	else
	{
		int size[2];
		size[0] = 0x100;
		size[1] = 0x100;
		pResMgr->VideoReference()->Command(WX_VDEV_GET_CAPS, 3, (int)size);
		m_pShadowView->SetTextureSize(size[0] > 0x800 ? 0x800 : size[0],
			size[1] > 0x800 ? 0x800 : size[1]);
	}
	m_pRecvView->SetResourceManager(pResMgr);
}

CShadow::~CShadow()
{
	if (m_pShadowView)
	{
		delete m_pShadowView;
		m_pShadowView = NULL;
	}

	if (m_pRecvView)
	{
		delete m_pRecvView;
		m_pRecvView = NULL;
	}
}

void CShadow::SetShadowType(ShadowType type)
{
	m_type = type;
}

void CShadow::RenderCaster(WView* view, const LightSet& light,
	const WSphere& sphere, std::list<WPuppet*>& pets, sShadowRecver* recver,
	bool bRenderPets)
{
	if (m_type == SHADOW_NONE)
	{
		return;
	}

	switch (m_type)
	{
	case SHADOW_PROJECTION:
		RenderProjectionShadow(view, light, sphere, pets, recver, bRenderPets);
		break;
	}
	view->SetCamera(view->camera);
	view->SetClip(view->GetClipNearValue(), view->GetClipFarValue(), false);
	view->UpdateCamera();
}

void CShadow::RenderProjectionShadow(WView* view, const LightSet& light,
	const WSphere& sphere, std::list<WPuppet*>& pets, sShadowRecver* recver,
	bool bRenderPets)
{
	if (!Setup(view, light, sphere, recver) || !bRenderPets || !m_pShadowView)
	{
		return;
	}

	for (std::list<WPuppet*>::iterator it = pets.begin(); it != pets.end();
		it++)
	{
		WPuppet* pPet = *it;
		if (pPet && !DisableShadowPet::Instance()->Exist(pPet->GetPuppetName()))
		{
			m_pShadowView->Render(pPet, true, 1.0f, false, false);
		}
	}
	m_pShadowView->Render();
}

bool CShadow::CheckFrustum(WView* view, const WMatrix& cam, float fov,
	float nearDist, float farDist)
{
	WVector v[8];
	int i;
	float h;

	float halfFov = fov * 0.5f;
	float t = tanf(halfFov);
	h = nearDist * t;
	for (i = 0; i < 4; i++)
	{
		v[i] =
			WVector((i & 1) ? h : -h, ((i >> 1) & 1) ? h : -h, nearDist) * cam;
	}
	h = farDist * t;
	for (i = 4; i < 8; i++)
	{
		v[i] =
			WVector((i & 1) ? h : -h, ((i >> 1) & 1) ? h : -h, farDist) * cam;
	}

	Waabb aabb;
	aabb.min = aabb.max = v[0];
	for (i = 1; i < 8; i++)
	{
		aabb.AddPoint(v[i]);
	}

	return view->InFrustum(aabb);
}

bool CShadow::Setup(WView* view, const LightSet& light, const WSphere& bsphere,
	sShadowRecver* recver)
{
	WMatrix cam;
	float dist;

	if (light.type == 2)
	{
		dist = bsphere.radius + 1000000.0f;
		WVector eye = bsphere.pos - light.nearone * dist;
		cam = MakeCameraMatrix(eye, bsphere.pos);
	}
	else
	{
		cam = MakeCameraMatrix(light.nearone, bsphere.pos);
		dist = WVector::UNIT_POS_Z * (bsphere.pos * ~cam);
	}

	float nearDist = dist * 0.5f;
	float farDist = dist + bsphere.radius;
	float fov = asinf(bsphere.radius / dist) * 2.0f;

	if (nearDist > farDist)
	{
		return false;
	}

	if (!CheckFrustum(view, cam, fov, nearDist, farDist))
	{
		return false;
	}

	m_pRecvView->SetRenderView(view);
	fov = fov * (4.0f / 3.0f);
	m_pRecvView->SetFOV(fov);
	m_pRecvView->SetCamera(cam);
	m_pRecvView->SetClip(nearDist, farDist, false);
	m_pRecvView->BeginScene();
	m_pRecvView->Clear(0, 6);
	m_pRecvView->CollectRecvTris(recver->pet, true, 1.0f, false, false);
	m_pRecvView->EndScene();
	if (!m_pRecvView->HasRecver())
	{
		return false;
	}

	m_pShadowView->SetFOV(fov);
	m_pShadowView->SetCamera(cam);
	m_pShadowView->SetClip(nearDist, farDist, false);
	m_pShadowView->BeginScene();
	m_pShadowView->Clear(0, 6);
	return true;
}

void CShadow::RenderRecver(sShadowRecver* recver)
{
	m_pRecvView->Render(m_pShadowView, recver->pet);
}

CShadowCaster::CShadowCaster(const char* name, int state,
	WResourceManager* pResMgr, eShadowMode mode)
{
	strcpy(m_name, name);
	m_state = state | 2;
	m_pShadow = new CShadow(pResMgr, mode);
}

CShadowCaster::~CShadowCaster()
{
	if (m_pShadow)
	{
		delete m_pShadow;
		m_pShadow = NULL;
	}
}

const char* CShadowCaster::GetName() const
{
	return m_name;
}

const char* CShadowCaster::GetLightName() const
{
	return m_lightName;
}

void CShadowCaster::Reset()
{
	m_state &= ~0xc;
}

int CShadowCaster::GetState() const
{
	return m_state;
}

void CShadowCaster::SetState_New(int state)
{
	m_state = state;
}

void CShadowCaster::ClearCaster()
{
	m_pets.clear();
	m_state |= 2;
}

void CShadowCaster::ClearCaster_New()
{
	m_pets.clear();
	m_state = 2;
}

void CShadowCaster::SetLight(const char* lightName, LightSet& light)
{
	if (lightName)
	{
		strcpy(m_lightName, lightName);
	}
	m_light = light;
	m_state |= 1;
}

void CShadowCaster::AddPet(WPuppet* pPet)
{
	return;
	m_pets.push_back(pPet);
	m_state &= ~2;
}

void CShadowCaster::AddPet_New(WPuppet* pPet)
{
	m_pets.push_back(pPet);
	m_state &= ~2;
}

void CShadowCaster::DeletePet(WPuppet* pPet)
{
	for (std::list<WPuppet*>::iterator it = m_pets.begin(); it != m_pets.end();
		it++)
	{
		if (*it == pPet &&
			_stricmp((*it)->GetPuppetName(), pPet->GetPuppetName()) == 0)
		{
			m_pets.erase(it);
			if (m_pets.size() == 0)
			{
				m_state |= 2;
			}

			return;
		}
	}
}

void CShadowCaster::ReplacePets(WPuppet* pOld, WPuppet* pNew, bool bShow)
{
	if (pOld == NULL)
	{
		if (m_pets.size() == 0)
		{
			if (pNew != NULL)
			{
				m_pets.push_back(pNew);
				m_state &= ~2;
			}
		}
		else if (m_pets.size() == 1)
		{
			std::list<WPuppet*>::iterator first = m_pets.begin();
			if (pNew != NULL)
			{
				*first = pNew;
			}
			else
			{
				m_pets.erase(first);
				m_state |= 2;
			}
		}
	}
	else
	{
		std::list<WPuppet*>::iterator it;
		for (it = m_pets.begin(); it != m_pets.end(); it++)
		{
			if (*it == pOld &&
				_stricmp((*it)->GetPuppetName(), pOld->GetPuppetName()) == 0)
			{
				if (pNew != NULL)
				{
					*it = pNew;
				}
				else
				{
					m_pets.erase(it);
					if (m_pets.size() == 0)
					{
						m_state |= 2;
					}
				}
				break;
			}
		}
	}
	m_state &= ~0x80;
	ShowCaster(bShow);
}

void CShadowCaster::UpdateBSphere(WView* view, float minAngle, float minDist)
{
	if (m_state & 0x22)
	{
		return;
	}

	float fov = view->GetFOV();
	if (fov < minAngle)
	{
		fov = minAngle;
	}
	float nearDist = view->GetClipNearValue();
	float farDist = nearDist + minDist;

	float tanFov = tanf(fov * 0.5f);
	float tanRatioFov = tanf(view->GetRatio() * fov * 0.5f);

	WVector nearV(tanFov * nearDist, tanRatioFov * nearDist, nearDist);
	WVector farV(tanFov * farDist, tanRatioFov * farDist, farDist);
	float c =
		(farV.SquareMagnitude() - nearV.SquareMagnitude()) / (minDist * 2.0f);

	m_bsphere.pos = c * view->camera.za + view->camera.pivot;

	m_bsphere.radius = (farV - WVector(0.0f, 0.0f, c)).Magnitude();
}

void CShadowCaster::RenderCaster(WView* view, sShadowRecver* recver)
{
	int state = m_state;
	if ((state & 0x40) == 0 && m_bsphere.radius < 8.0f)
	{
		m_bsphere.radius = 8.0f;
	}

	if ((state & 0x20) && (state & 0x80))
	{
		m_pShadow->RenderCaster(view, m_light, m_bsphere, m_pets, recver,
			false);
	}
	else
	{
		m_state = state | 0x80;
		m_pShadow->RenderCaster(view, m_light, m_bsphere, m_pets, recver, true);
	}
}

void CShadowCaster::RenderRecver(sShadowRecver* recver)
{
	m_pShadow->RenderRecver(recver);
}

void CShadowCaster::ShowCaster(bool bShow)
{
	if (bShow)
	{
		m_state &= ~0x20;
	}
	else
	{
		m_state |= 0x20;
	}
}

const char* g_theCasterName = "theCaster";

CShadowManager::CShadowManager(WResourceManager* pResMgr, bool bEnable)
	: m_bEnable(bEnable), m_bActive(bEnable), m_pResMgr(pResMgr)
{
	int caps;
	pResMgr->VideoReference()->Command(WX_VDEV_GET_CAPS, 1, (int)&caps);
	m_minFov = 90.0f * g_DEGTORAD;
	m_maxDist = 200.0f;
	m_bFlag = true;
	m_mode = (eShadowMode)(caps != 0);
}

CShadowManager::~CShadowManager()
{
	ClearCasters();
	ClearRecvers();
}

void CShadowManager::ClearCasters()
{
	std::list<CShadowCaster*>::iterator it;
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		CShadowCaster* pCaster = *it;
		if (pCaster)
		{
			delete pCaster;
		}
	}
	m_casters.clear();
}

void CShadowManager::ClearRecvers()
{
	ShadowRecverIter it;
	for (it = m_recvers.begin(); it != m_recvers.end(); it++)
	{
		delete *it;
		*it = NULL;
	}
	m_recvers.clear();
}

void CShadowManager::ClearCasters_New()
{
	std::list<CShadowCaster*>::iterator it;
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		(*it)->Reset();
		(*it)->ClearCaster_New();
	}
}

void CShadowManager::ClearRecvers_New()
{
	ShadowRecverIter it;
	for (it = m_recvers.begin(); it != m_recvers.end(); it++)
	{
		(*it)->ClearRecver();
	}
}

bool CShadowManager::IsEnabled() const
{
	return m_bEnable;
}

void CShadowManager::Enable(bool bEnable)
{
	m_bEnable = bEnable;
}

CShadowCaster* CShadowManager::RegisterCaster_New(const char* name, int state,
	const char* lightName, LightSet& light)
{
	const char* casterName = g_theCasterName;
	CShadowCaster* pCaster = FindCaster(casterName);
	if (pCaster == NULL)
	{
		pCaster = new CShadowCaster(casterName, 0, m_pResMgr, m_mode);
		pCaster->SetLight(lightName, light);
		m_casters.push_back(pCaster);
	}
	else
	{
		pCaster->SetLight(lightName, light);
	}

	return pCaster;
}

void CShadowManager::AddCastPet_New(const char* name, bool bClear,
	WPuppet* pPet)
{
	CShadowCaster* pCaster = FindCaster(name);
	if (!pCaster)
	{
		return;
	}

	if (bClear)
	{
		pCaster->ClearCaster();
	}
	pCaster->AddPet_New(pPet);
}

CShadowCaster* CShadowManager::FindCaster(const char* name)
{
	std::list<CShadowCaster*>::iterator it;
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		if (strcmp((*it)->GetName(), g_theCasterName) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

sShadowRecver* CShadowManager::RegisterRecver_New(const char* name)
{
	sShadowRecver* pRecver = FindCommonRecver(name);
	if (pRecver == NULL)
	{
		pRecver = new sShadowRecver;
		strcpy(pRecver->name, name);
		m_recvers.push_back(pRecver);
	}

	return pRecver;
}

void CShadowManager::AddRecvPet_New(const char* name, WPuppet* pPet,
	float height)
{
	sShadowRecver* pRecver = FindCommonRecver(name);
	if (!pRecver)
	{
		return;
	}

	pRecver->pet = pPet;
	pRecver->height = height;
}

sShadowRecver* CShadowManager::FindCommonRecver(const char* name)
{
	ShadowRecverIter it;
	for (it = m_recvers.begin(); it != m_recvers.end(); it++)
	{
		if (strcmp((*it)->name, name) == 0)
		{
			return *it;
		}
	}

	return NULL;
}

void CShadowManager::Flush(WView* view)
{
	if (!m_bEnable || !m_bActive)
	{
		return;
	}

	std::list<CShadowCaster*>::iterator it;
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		(*it)->Reset();
	}
	UpdateBVolume(view);
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		if (((*it)->GetState() & 0x2e) == 0)
		{
			ShadowRecverIter rit;
			for (rit = m_recvers.begin(); rit != m_recvers.end(); rit++)
			{
				(*it)->RenderCaster(view, *rit);
				(*it)->RenderRecver(*rit);
			}
		}
	}
	view->UpdateCamera();
}

void CShadowManager::SetActive(bool bActive)
{
	m_bActive = bActive;
}

void CShadowManager::UpdateBVolume(WView* view)
{
	float dist;
	if (m_bFlag && WisEqual(view->camera.za, WVector::UNIT_NEG_Y, 0.00001f))
	{
		if (m_recvers.size() > 0)
		{
			sShadowRecver* recver = *m_recvers.begin();
			dist = Max(view->camera.pivot.y - recver->height, m_maxDist);
		}
		else
		{
			dist = m_maxDist;
		}
	}
	else
	{
		dist = m_maxDist;
	}

	for (std::list<CShadowCaster*>::iterator it = m_casters.begin();
		it != m_casters.end(); it++)
	{
		(*it)->UpdateBSphere(view, m_minFov, dist);
	}
}

void CShadowManager::UpdateLight(const char* lightName, LightSet& light)
{
	std::list<CShadowCaster*>::iterator it;
	for (it = m_casters.begin(); it != m_casters.end(); it++)
	{
		if (strcmp((*it)->GetLightName(), lightName) == 0)
		{
			(*it)->SetLight(NULL, light);
		}
	}
}

void CShadowManager::Reset()
{
	ClearCasters();
	ClearRecvers();
}

void CShadowManager::Reset_New()
{
	ClearCasters_New();
	ClearRecvers_New();
}

void CShadowManager::SetMinViewInfo(float minFov, float maxDist, bool flag)
{
	m_minFov = Max(minFov, 0.001f);
	m_maxDist = Max(maxDist, 0.001f);
	m_bFlag = flag;
}
