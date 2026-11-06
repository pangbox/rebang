#include "stdafx.h"
#include "mathconsts.h"

#include "scenemanager.h"

#include <algorithm>
#include <math.h>

#include "sea.h"
#include "wresrcmng.h"
#include "wview.h"
#include "wtextureview.h"
#include "wutil.h"
#include "wpuppet.h"
#include "wbone.h"
#include "obmarker.h"
#include "golfdoc.h"
#include "shareddoc.h"
#include "taskmanager.h"
#include "pool.h"
#include "river.h"

#include "beach.h"
#include "westpak.h"
#include "../../shared/classdefine.h"
#include "../../shared/localize.h"
#include "shadow.h"
#include "clientsetting.h"
#include "polysoup.h"
#include "sky.h"
#include "autotarget.h"
#include "golfball.h"
#include "quadtree.h"
#include "wfont.h"
#include "chatmsg.h"
#include "snow.h"
#include "icesnow.h"
#include "lobbybg.h"
#include "petal.h"
#include "rain.h"
#include "golftask.h"
#include "objectfactory.h"
#include "frwnd.h"
#include <stdlib.h>

extern int g_nDrawModel;
extern int g_nDrawPoly;
extern int g_nModel;
extern LightSet g_lightset;

WTVertex CSceneManager::vtr[4];
WTVertex* CSceneManager::vtx[5] = { &CSceneManager::vtr[0],
	&CSceneManager::vtr[1], &CSceneManager::vtr[3], &CSceneManager::vtr[2], 0 };

CSceneManager::CSceneManager(void)
	: m_beachList(16, 16),
	  m_waveletList(16, 16),
	  m_poolList(16, 16),
	  m_riverList(16, 16),
	  m_pSea(NULL),
	  m_pMarker(NULL),
	  m_pWeather(NULL),
	  m_pLobbyWeather(NULL),
	  m_useFog(true),
	  m_useImposter(false),
	  m_skipTransparent(false),
	  m_bDrawElementBoxes(false),
	  m_pFont(NULL),
	  m_bOverlapCamera(true),
	  m_bNoOverlapCamera(false),
	  m_debugDraw()
{
	m_pMarker = new CMarker;

	int caps;
	g_resrcmng->VideoReference()->Command(WX_VDEV_GET_CAPS, 1, (intptr_t)&caps);

	if (caps)
	{
		m_skipTransparent = true;
	}

	m_useImposter = COption::Instance()->vIsLodEnabled();
	m_pFont = CChatMsg::Instance()->GetMaskedFont();
}

CSceneManager::~CSceneManager(void)
{
	for (SceneElementIter it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if ((*it).m_pImposter)
		{
			delete (*it).m_pImposter;
			(*it).m_pImposter = 0;
		}
	}
	m_elements.clear();

	m_customElements.clear();
	m_effectElements.clear();
	m_nearElements.clear();
	m_blendElements.clear();
	m_transparentElements.clear();

	Clear();

	if (m_pMarker)
	{
		delete m_pMarker;
		m_pMarker = 0;
	}

	if (m_pWeather)
	{
		delete m_pWeather;
		m_pWeather = 0;
	}

	if (m_pLobbyWeather)
	{
		delete m_pLobbyWeather;
		m_pLobbyWeather = 0;
	}
}

void CSceneManager::RegisterElement(eObjType type, WPuppet* pPuppet,
	CRenderFuncPtr* pRender, const char* name, unsigned long flags, Waabb* pBox)
{
	SceneElement elem;
	elem.m_type = type;
	elem.m_pPuppet = pPuppet;
	elem.m_pRender = pRender;
	elem.m_pBox = pBox;
	if (name)
	{
		strcpy(elem.m_name, name);
	}
	else
	{
		elem.m_name[0] = 0;
	}

	switch (type)
	{
	case 1:
	case 5:
	case 6:
		flags |= 0xB;
		break;
	case 3:
		flags |= 6;
		break;
	case 4:
		flags |= 7;
		break;
	default:
		break;
	}
	elem.m_flags = flags;

	if (m_useImposter && (flags & 2) == 0)
	{
		WTextureImposterView* pView = new WTextureImposterView(pPuppet);
		elem.m_pImposter = pView;
		pView->SetResourceManager(g_resrcmng);

		WTextureView::TextureParam param;
		param.m_numRts = 1;
		param.m_rtTexInfo[0].Set(0x12000, WRenderToTextureSizeInfo::SIZE_ABS,
			true, 0);
		param.m_depthSurfInfo.Set(
			(WRenderToTextureParam::DepthSurfInfo::SurfaceUsage)2,
			WRenderToTextureSizeInfo::SIZE_ABS, true, 1.0f);
		pView->SetTextureParam(param);
	}

	m_elements.push_back(elem);
	m_dirty = true;
}

void CSceneManager::RegisterElement(SceneElemRegister& reg)
{
	RegisterElement(reg.m_type, reg.m_pPuppet, reg.m_pRender, reg.m_name,
		reg.m_flags, reg.m_pBox);
}

void CSceneManager::ReleaseElement(void)
{
	SceneElementIter it = m_elements.begin();
	while (it != m_elements.end())
	{
		if ((*it).m_flags & 8)
		{
			it++;
			continue;
		}

		if ((*it).m_pImposter)
		{
			delete (*it).m_pImposter;
			(*it).m_pImposter = 0;
		}

		it = m_elements.erase(it);
	}
}

void CSceneManager::DeleteElement(const char* name)
{
	if (!IsLocalContent((localContentType_t)0x56))
	{
		return;
	}

	SceneElementIter it = m_elements.begin();
	while (it != m_elements.end())
	{
		if (strcmp((*it).m_name, name) == 0)
		{
			if ((*it).m_pImposter)
			{
				delete (*it).m_pImposter;
				(*it).m_pImposter = 0;
			}
			it = m_elements.erase(it);
			continue;
		}
		++it;
	}
}

void CSceneManager::SetLOD(bool lod, bool apply)
{
	m_useImposter = lod && COption::Instance()->vIsLodEnabled();

	if (!apply)
	{
		return;
	}

	if (m_useImposter)
	{
		for (SceneElementIter it = m_elements.begin(); it != m_elements.end();
			++it)
		{
			if ((*it).m_flags & 2)
			{
				continue;
			}

			if ((*it).m_pImposter)
			{
				continue;
			}

			if (!(*it).m_pPuppet)
			{
				continue;
			}
			(*it).m_pImposter = new WTextureImposterView((*it).m_pPuppet);
			(*it).m_pImposter->SetResourceManager(g_resrcmng);

			WTextureView::TextureParam param;
			param.m_numRts = 1;
			param.m_rtTexInfo[0].Set(0x12000,
				WRenderToTextureSizeInfo::SIZE_ABS, true, 0);
			param.m_depthSurfInfo.Set(
				(WRenderToTextureParam::DepthSurfInfo::SurfaceUsage)2,
				WRenderToTextureSizeInfo::SIZE_ABS, true, 1.0f);

			(*it).m_pImposter->SetTextureParam(param);
		}
	}
	else
	{
		for (SceneElementIter it = m_elements.begin(); it != m_elements.end();
			++it)
		{
			if ((*it).m_pImposter)
			{
				delete (*it).m_pImposter;
				(*it).m_pImposter = 0;
			}
		}
	}
}

void CSceneManager::SetReflection(bool reflection)
{
	for (CPool* pPool = m_poolList.Start(); pPool; pPool = m_poolList.Next())
	{
		pPool->SetReflection(reflection);
	}
}

void CSceneManager::UpdateSea(void)
{
	if (m_pSea)
	{
		m_pSea->LoadTexture();
	}
}

void CSceneManager::UpdateWind(void)
{
	if (m_pWeather)
	{
		m_pWeather->UpdateWind();
	}
}

void CSceneManager::UpdateBright(void)
{
	UpdateFog(false);
}

void CSceneManager::UpdateFog(bool load)
{
	if (load)
	{
		cFile* file = g_resrcmng->GetCFile(
			MakeStr("%s_fog.txt",
				CSharedDoc::Instance()->m_golfGame.pCourse
					? CSharedDoc::Instance()->m_golfGame.pCourse->Data
					: 0),
			-1);

		if (file)
		{
			int rgb[3];

			file->Scan("%d %d %d\n", &rgb[0], &rgb[1], &rgb[2]);
			m_fogColor = ModifyColor(
				((((unsigned char)rgb[0] << 8) | (unsigned char)rgb[1]) << 8) |
					(unsigned char)rgb[2],
				GOLFDOC()->m_bright);

			file->Scan("%f %f\n", &m_fogStart, &m_fogEnd);

			CloseCFile(file);
		}
		else
		{
			m_fogColor = 0xFFFFFF;
			m_fogStart = 100.0f;
			m_fogEnd = 400.0f;
		}
	}

	g_view->SetFogState(m_fogStart * 3.2f, m_fogEnd * 3.2f, m_fogColor);
	m_useFog = !(GOLFDOC()->GameMode() & 0x200);
}

void CSceneManager::UpdateFog(const char* name)
{
	cFile* file = g_resrcmng->GetCFile(MakeStr("%s_fog.txt", name), -1);

	if (file)
	{
		int rgb[3];

		file->Scan("%d %d %d\n", &rgb[0], &rgb[1], &rgb[2]);
		m_fogColor = ModifyColor(
			((((unsigned char)rgb[0] << 8) | (unsigned char)rgb[1]) << 8) |
				(unsigned char)rgb[2],
			GOLFDOC()->m_bright);

		file->Scan("%f %f\n", &m_fogStart, &m_fogEnd);

		CloseCFile(file);
	}
	else
	{
		m_fogColor = 0xFFFFFF;
		m_fogStart = 100.0f;
		m_fogEnd = 400.0f;
	}

	g_view->SetFogState(m_fogStart * 3.2f, m_fogEnd * 3.2f, m_fogColor);
	m_useFog = true;
}

WPuppet* CSceneManager::GetPuppet(const char* name)
{
	if (name == 0)
	{
		return 0;
	}
	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (it != m_elements.end())
	{
		return (*it).m_pPuppet;
	}

	return 0;
}

eObjType CSceneManager::GetType(const char* name)
{
	if (name == 0)
	{
		return (eObjType)7;
	}
	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (it != m_elements.end())
	{
		return (*it).m_type;
	}

	return (eObjType)7;
}

void CSceneManager::SetAlpha(const char* name, unsigned char alpha)
{
	if (name == 0)
	{
		return;
	}

	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (it != m_elements.end())
	{
		(*it).m_alpha = alpha;
	}
}

void CSceneManager::SetVisible(const char* name, bool visible)
{
	if (name == 0)
	{
		return;
	}

	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (it != m_elements.end())
	{
		(*it).m_visible = visible;
	}
}

void CSceneManager::SetPuppet(const char* name, WPuppet* pPuppet)
{
	if (name == 0)
	{
		return;
	}

	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (it != m_elements.end())
	{
		(*it).m_pPuppet = pPuppet;
	}
}

void CSceneManager::SetType(const char* name, eObjType type)
{
	if (name == 0)
	{
		return;
	}

	SceneElementIter it = std::find(m_elements.begin(), m_elements.end(), name);
	if (!(it != m_elements.end()))
	{
		return;
	}

	(*it).m_type = type;
	m_dirty = true;
}

void CSceneManager::Process(float elapsed)
{
	if (GOLFDOC()->m_pSky)
	{
		GOLFDOC()->m_pSky->Process(elapsed);
	}

	if (m_pWeather)
	{
		m_pWeather->Process(elapsed);
	}

	if (m_pLobbyWeather)
	{
		m_pLobbyWeather->Process(elapsed);
	}

	if (m_pSea)
	{
		m_pSea->Process(elapsed);
	}

	CPool* pPool;
	for (pPool = m_poolList.Start(); pPool; pPool = m_poolList.Next())
	{
		pPool->Process(elapsed);
	}

	CBeach* pBeach;
	for (pBeach = m_beachList.Start(); pBeach; pBeach = m_beachList.Next())
	{
		pBeach->Process(elapsed);
	}

	ntRiver* pRiver;
	for (pRiver = m_riverList.Start(); pRiver; pRiver = m_riverList.Next())
	{
		pRiver->Process(elapsed);
	}

	CWavelet* pWavelet;
	for (pWavelet = m_waveletList.Start(); pWavelet;
		pWavelet = m_waveletList.Next())
	{
		pWavelet->Process(elapsed);
	}
}

void CSceneManager::DropTest(void)
{
	WPolySoup::sModel* pModel = GOLFDOC()->m_pPolySoup->FindModel(0);

	Waabb box = pModel->aabb;
	int countX = 1 - (int)((box.max.x - box.min.x) / -16.0f);
	int countZ = 1 - (int)((box.max.z - box.min.z) / -16.0f);
	m_drops.clear();

	WVector pos = box.min;
	for (int z = 0; z < countZ; z++)
	{
		for (int x = 0; x < countX; x++)
		{
			sDrop drop;
			pos.x += 16.0f;

			switch (GOLFDOC()->m_pAutoTarget->CalcDropPos(pos))
			{
			case 0:
			{
				drop.m_pos = GolfBall().m_pos;
				drop.m_color = 0xFFFF0000;
				m_drops.push_back(drop);
				break;
			}
			case 2:
			{
				drop.m_pos = GolfBall().m_pos;
				drop.m_color = 0xFF0000FF;
				m_drops.push_back(drop);
				break;
			}
			}
		}
		pos.z += 16.0f;
		pos.x = box.min.x;
	}

	GolfBall().m_pos = PLAYER(GOLFDOC()->m_currentPlayer)->pos;
}

bool RenderOrder::operator()(const SceneElement& a, const SceneElement& b)
{
	return a.m_type < b.m_type;
}

void CSceneManager::Prerender(WView* pView)
{
	bool useImposter = m_useImposter;
	bool noOverlapCamera = m_bNoOverlapCamera;
	bool overlapCamera = m_bOverlapCamera;

	m_useImposter = false;
	m_bNoOverlapCamera = false;
	m_bOverlapCamera = false;

	for (CPool* pPool = m_poolList.Start(); pPool; pPool = m_poolList.Next())
	{
		pPool->Prerender(pView);
	}

	m_useImposter = useImposter;
	m_bNoOverlapCamera = noOverlapCamera;
	m_bOverlapCamera = overlapCamera;
}

void CSceneManager::Render(WView* pView, unsigned long mask)
{
	pView->SetFogEnable(m_useFog && GOLFDOC()->GameMode() != 0x200000);

	g_nDrawPoly = 0;
	g_nDrawModel = 0;
	g_nModel = GOLFDOC()->m_pPolySoup->m_modelNum;

	m_customElements.clear();
	m_effectElements.clear();
	m_nearElements.clear();
	m_blendElements.clear();
	m_transparentElements.clear();

	if (m_dirty)
	{
		m_elements.sort(RenderOrder());
		m_dirty = false;
	}

	if (mask & 0x80)
	{
		if (GOLFDOC()->m_pSky)
		{
			GOLFDOC()->m_pSky->Render(pView);
		}
	}

	unsigned long imposterBits = mask & 0xC;
	if (imposterBits)
	{
		Render_BuildImposter(pView);
	}

	if (mask & 1)
	{
		Render_BaseAndShadow(pView, mask);
	}

	int numBlend = Render_RenderElement(pView, mask);

	if (mask & 0x40)
	{
		Render_RenderEffect(pView);
	}

	if (mask & 0x100)
	{
		if (m_pMarker)
		{
			m_pMarker->Display();
		}
	}

	if (mask & 0x200)
	{
		if (m_pSea)
		{
			m_pSea->Render();
		}
	}

	if (mask & 0x200)
	{
		Render_RenderWaterStuff(pView);
	}

	pView->Flush(1);

	if (imposterBits)
	{
		Render_RenderBillboardGroup(pView);
		Render_RenderBlendGroup(pView, m_blendElements, 0, numBlend);
	}

	unsigned long weather = mask & 0x400;
	if (weather)
	{
		Render_RenderWeather(pView);
	}

	if (mask & 0x20)
	{
		Render_RenderCustom(pView);
	}

	if (imposterBits)
	{
		Render_RenderNearGroup(pView);
	}

	if (weather)
	{
		Render_RenderWeatherFullScreenOverlay(pView);
	}

	if (mask & 0x800)
	{
		Render_RenderDropPosition(pView);
	}

	pView->Flush(0);

	DrawDebugLine(pView);
}

void CSceneManager::Render_RenderSky(WView* pView)
{
	if (GOLFDOC()->m_pSky)
	{
		GOLFDOC()->m_pSky->Render(pView);
	}
}

void CSceneManager::Render_BuildImposter(WView* pView)
{
	if (!m_useImposter)
	{
		return;
	}

	WxBatchState state;

	for (SceneElementIter it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (&(*it) == 0)
		{
			continue;
		}

		if (!(*it).m_visible)
		{
			continue;
		}

		if ((*it).m_type == 5)
		{
			continue;
		}

		if ((*it).m_type == 6)
		{
			continue;
		}

		if ((*it).m_pRender)
		{
			continue;
		}
		WPuppet* pPuppet = (*it).m_pPuppet;
		if (pPuppet == 0)
		{
			continue;
		}

		if ((*it).m_type != 2)
		{
			continue;
		}

		if (pPuppet->GetRootBone() == 0)
		{
			continue;
		}

		if (!pView->InFrustum(pPuppet->GetBoundSphere()) ||
			!pView->InFrustum(pPuppet->GetBoundBox()))
		{
			continue;
		}
		WTextureImposterView* pImposter = (*it).m_pImposter;
		if (pImposter == 0)
		{
			continue;
		}

		const WMatrix& camera = pView->GetCamera();
		WVector dist;
		dist = camera.pivot - pPuppet->GetBoundSphere().pos;
		dist.y = 0.0f;

		if (dist.Magnitude() <= 800.0f)
		{
			continue;
		}

		if (!pImposter->NeedToUpdate(camera))
		{
			continue;
		}

		(*it).m_pImposter->Begin();
		(*it).m_pPuppet->SetAlpha(0xFF);

		if (!m_skipTransparent)
		{
			(*it).m_pPuppet->Render((*it).m_pImposter, true,
				(*it).m_pPuppet->GetMatrix().xa.Magnitude(), false, false,
				false);
		}
		else
		{
			(*it).m_pImposter->xSetLight(0,
				(*it).m_pPuppet->GetRootBone()->GetLight());
			(*it).m_pPuppet->Transform((*it).m_pImposter,
				WVectorLen((*it).m_pPuppet->GetMatrix().xa), 2);
			(*it).m_pPuppet->GetRootBone()->xRender((*it).m_pImposter, &state,
				false, 0, 0);
		}

		(*it).m_pImposter->Flush(0);
		(*it).m_pImposter->End();
	}
}

void CSceneManager::Render_BaseAndShadow(WView* pView, unsigned long mask)
{
	for (SceneElementIter it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (&(*it) == 0)
		{
			continue;
		}

		if (!(*it).m_visible)
		{
			continue;
		}

		if ((mask & (1 << (*it).m_type)) == 0)
		{
			continue;
		}

		if ((*it).m_type != 0)
		{
			continue;
		}

		WPuppet* pPuppet = (*it).m_pPuppet;
		if (pPuppet == 0)
		{
			return;
		}

		if (pPuppet->GetRootBone() == 0)
		{
			return;
		}

		if (!pView->InFrustum(pPuppet->GetBoundSphere()) ||
			!pView->InFrustum(pPuppet->GetBoundBox()))
		{
			return;
		}

		g_nDrawModel++;
		g_nDrawPoly += (*it).m_pPuppet->GetFaceNum();

		DrawPuppet(pView, (*it).m_pPuppet, (*it).m_pBox);
		pView->Flush(1);

		if (CShadowManager::Instance())
		{
			RenderShadow(pView, mask);
		}

		return;
	}
}

void CSceneManager::Render_RenderOBMarker(WView* pView)
{
	if (m_pMarker)
	{
		m_pMarker->Display();
	}
}

int CSceneManager::Render_RenderElement(WView* pView, unsigned long mask)
{
	int numBlend = 0;
	WVector dist;

	for (SceneElementIter it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (&(*it) == 0)
		{
			continue;
		}

		if (!(*it).m_visible)
		{
			continue;
		}

		if ((mask & (1 << (*it).m_type)) == 0)
		{
			continue;
		}

		if ((*it).m_type == 0)
		{
			continue;
		}

		if ((*it).m_type == 5)
		{
			m_customElements.push_back(
				CustomElement((eObjType)(*it).m_type, (*it).m_pRender));
			continue;
		}

		if ((*it).m_type == 6 && (mask & 0x40))
		{
			m_effectElements.push_back(
				CustomElement((eObjType)(*it).m_type, (*it).m_pRender));
			continue;
		}

		if ((*it).m_pRender)
		{
			(*it).m_pRender->Display();
			continue;
		}

		WPuppet* pPuppet = (*it).m_pPuppet;
		if (pPuppet == 0)
		{
			continue;
		}

		if (pPuppet->GetRootBone() == 0)
		{
			continue;
		}

		if ((*it).m_type == 2)
		{
			if (!pView->InFrustum(pPuppet->GetBoundSphere()) ||
				!pView->InFrustum(pPuppet->GetBoundBox()))
			{
				continue;
			}
		}
		else
		{
			if (!pView->InFrustum(pPuppet->GetBoundSphere()))
			{
				continue;
			}
		}

		if (m_useImposter && (*it).m_pImposter)
		{
			dist = pView->camera.pivot - (*it).m_pPuppet->GetBoundSphere().pos;
			dist.y = 0.0f;
			float distance = dist.Magnitude();

			if (distance < 960.0f)
			{
				g_nDrawModel++;
				g_nDrawPoly += (*it).m_pPuppet->GetFaceNum();
			}

			if (distance > 800.0f)
			{
				float t = Between(0.0f, (960.0f - distance) * 0.00625f, 1.0f);
				(*it).m_alpha = (unsigned char)(t * 255.0f);

				if (distance > 960.0f)
				{
					m_transparentElements.push_back(
						TransparentElement((*it).m_pPuppet, (*it).m_pBox,
							(*it).m_pImposter->GetTexture(0), 0));
				}
				else
				{
					m_blendElements.push_back(
						TransparentElement((*it).m_pPuppet, (*it).m_pBox,
							(*it).m_pImposter->GetTexture(0),
							(*it).m_pPuppet->GetFaceNum()));
					numBlend += (*it).m_pPuppet->GetFaceNum();
				}
			}
			else
			{
				(*it).m_alpha = OverlapCamera(pView, *it);
				if ((*it).m_alpha < 0xFF)
				{
					m_nearElements.push_back(TransparentElement((*it).m_pPuppet,
						(*it).m_pBox, 0, 0));
				}
			}
		}
		else
		{
			if ((*it).m_type == 2 || (*it).m_type == 3)
			{
				g_nDrawModel++;
				g_nDrawPoly += (*it).m_pPuppet->GetFaceNum();
			}

			if ((*it).m_type == 4)
			{
				(*it).m_alpha = (*it).m_pPuppet->GetRootBone()->GetAlpha();
			}
			else if ((*it).m_pPuppet)
			{
				dist =
					pView->camera.pivot - (*it).m_pPuppet->GetBoundSphere().pos;
				dist.y = 0.0f;
				if (dist.Magnitude() > 800.0f)
				{
					(*it).m_alpha = 0xFF;
				}
				else
				{
					(*it).m_alpha = OverlapCamera(pView, *it);
				}
			}
			else
			{
				(*it).m_alpha = 0xFF;
			}

			if ((*it).m_alpha < 0xFF)
			{
				m_nearElements.push_back(
					TransparentElement((*it).m_pPuppet, (*it).m_pBox, 0, 0));
			}
		}

		if ((*it).m_alpha != (*it).m_pPuppet->GetRootBone()->GetAlpha())
		{
			(*it).m_pPuppet->SetAlpha((*it).m_alpha);
		}

		if (!m_skipTransparent)
		{
			if ((*it).m_alpha == 0xFF)
			{
				DrawPuppet(pView, (*it).m_pPuppet, (*it).m_pBox);
			}
		}
		else if ((*it).m_type != 2)
		{
			DrawPuppet(pView, (*it).m_pPuppet, (*it).m_pBox);
		}
	}

	return numBlend;
}

void CSceneManager::Render_RenderEffect(WView* pView)
{
	CustomElementIter it;
	it = m_effectElements.begin();
	for (; it != m_effectElements.end(); ++it)
	{
		if ((*it).m_pRender)
		{
			(*it).m_pRender->DisplayAll();
		}
	}
}

void CSceneManager::Render_RenderSea(WView* pView)
{
	if (m_pSea)
	{
		m_pSea->Render();
	}
}

void CSceneManager::Render_RenderWaterStuff(WView* pView)
{
	CBeach* pBeach;
	for (pBeach = m_beachList.Start(); pBeach; pBeach = m_beachList.Next())
	{
		pBeach->Render();
	}

	ntRiver* pRiver;
	for (pRiver = m_riverList.Start(); pRiver; pRiver = m_riverList.Next())
	{
		pRiver->Render();
	}

	CPool* pPool;
	for (pPool = m_poolList.Start(); pPool; pPool = m_poolList.Next())
	{
		pPool->Render(pView);
	}

	CWavelet* pWavelet;
	for (pWavelet = m_waveletList.Start(); pWavelet;
		pWavelet = m_waveletList.Next())
	{
		pWavelet->Render();
	}
}

struct Greater
{
	WVector m_dir;

	Greater(const WVector& dir);
	bool operator()(const TransparentElement& a, const TransparentElement& b);
};

unsigned char CSceneManager::OverlapCamera(WView* pView,
	const SceneElement& elem)
{
	if (m_bNoOverlapCamera)
	{
		return elem.m_alpha;
	}

	if ((elem.m_flags & 1) || !m_bOverlapCamera)
	{
		return 0xFF;
	}

	const WVector& eye = pView->GetCamera().pivot;
	Waabb box = elem.m_pPuppet->GetBoundBox();
	float dist = 0.0f;

	for (int i = 0; i < 3; i++)
	{
		float d;
		if (eye.p[i] < box.min.p[i])
		{
			d = eye.p[i] - box.min.p[i];
		}
		else if (eye.p[i] > box.max.p[i])
		{
			d = eye.p[i] - box.max.p[i];
		}
		else
		{
			continue;
		}
		dist += d * d;
	}

	float halfDiag = (box.max - box.min).Magnitude() * 0.5f;
	float radius = halfDiag > 96.0f ? 96.0f : halfDiag;

	if (dist > radius * radius)
	{
		return 0xFF;
	}

	float ratio = sqrtf(dist) / radius;
	if (ratio < 0.25f)
	{
		ratio = 0.25f;
	}
	else if (ratio > 1.0f)
	{
		ratio = 1.0f;
	}

	return (unsigned char)(ratio * 255.0f);
}

Greater::Greater(const WVector& dir)
	: m_dir(dir)
{
}

bool Greater::operator()(const TransparentElement& a,
	const TransparentElement& b)
{
	const WVector& pa = a.m_pPuppet->GetBoundSphere().pos;
	const WVector& pb = b.m_pPuppet->GetBoundSphere().pos;
	return pa.x * m_dir.x + pa.y * m_dir.y + pa.z * m_dir.z >
		pb.x * m_dir.x + pb.y * m_dir.y + pb.z * m_dir.z;
}

void CSceneManager::Render_RenderBillboardGroup(WView* pView)
{
	if (!m_transparentElements.size())
	{
		return;
	}

	TransparentElementIter it;
	it = m_transparentElements.begin();
	for (; it != m_transparentElements.end(); it++)
	{
		DrawImposter(pView, *it);
	}

	if (!m_skipTransparent)
	{
		pView->Flush(0);
	}
}

void CSceneManager::Render_RenderBlendGroup(WView* pView,
	std::list<TransparentElement>& elements, int first, int last)
{
	if (elements.size() == 0)
	{
		return;
	}

	if (m_skipTransparent)
	{
		for (TransparentElementIter it = elements.begin(); it != elements.end();
			++it)
		{
			DrawImposter(pView, *it);
		}

		return;
	}

	if (elements.size() == 1 || last < 0x400)
	{
		for (TransparentElementIter it = elements.begin(); it != elements.end();
			++it)
		{
			DrawPuppet(pView, (*it).m_pPuppet, (*it).m_pBox);
			DrawImposter(pView, *it);
		}
		pView->Flush(0);
		return;
	}

	bool split = false;
	std::list<TransparentElement> groups[2];

	WPlane plane;
	if (first % 2)
	{
		plane.normal = pView->camera.za;
	}
	else
	{
		plane.normal = pView->camera.xa;
	}

	elements.sort(Greater(plane.normal));

	float extent[2] = { 0.0f, -1.0f };
	int count[2] = { 0, 0 };
	TransparentElementIter it = elements.begin();
	for (; it != elements.end(); ++it)
	{
		TransparentElementIter next = it;
		++next;
		if (next == elements.end())
		{
			break;
		}

		if (!split)
		{
			groups[0].push_back(*it);
			count[0] += (*it).m_numPolys;

			if (count[0] < last / 3)
			{
				continue;
			}

			if (count[0] > last * 2 / 3)
			{
				split = true;
				continue;
			}

			float proj = (plane.normal *
				((*it).m_pPuppet->GetBoundSphere().pos -
					(*next).m_pPuppet->GetBoundSphere().pos));
			extent[0] = extent[1] > 0.0f
				? extent[1]
				: WVectorLen(WVector((*it).m_pPuppet->GetBoundBox().max.x, 0.0f,
								 (*it).m_pPuppet->GetBoundBox().max.z) -
					  WVector((*it).m_pPuppet->GetBoundBox().min.x, 0.0f,
						  (*it).m_pPuppet->GetBoundBox().min.z));
			extent[1] =
				WVectorLen(WVector((*next).m_pPuppet->GetBoundBox().max.x, 0.0f,
							   (*next).m_pPuppet->GetBoundBox().max.z) -
					WVector((*next).m_pPuppet->GetBoundBox().min.x, 0.0f,
						(*next).m_pPuppet->GetBoundBox().min.z));

			if (Abs(proj) > extent[0] + extent[1])
			{
				split = true;
			}
		}
		else
		{
			groups[1].push_back(*it);
			count[1] += (*it).m_numPolys;
		}
	}

	groups[1].push_back(*it);
	count[1] += (*it).m_numPolys;

	Render_RenderBlendGroup(pView, groups[0], first + 1, count[0]);
	Render_RenderBlendGroup(pView, groups[1], first + 1, count[1]);
}

void CSceneManager::Render_RenderWeather(WView* pView)
{
	if (AfxGetTask()->GetActor("LobbyBg") == 0)
	{
		if (m_pWeather)
		{
			m_pWeather->Display();
		}
	}

	if (m_pLobbyWeather)
	{
		m_pLobbyWeather->Display();
	}
}

void CSceneManager::Render_RenderCustom(WView* pView)
{
	for (CustomElementIter it = m_customElements.begin();
		it != m_customElements.end(); ++it)
	{
		if ((*it).m_pRender)
		{
			(*it).m_pRender->Display();
		}
	}
}

void CSceneManager::Render_RenderNearGroup(WView* pView)
{
	if (!m_nearElements.size() || m_skipTransparent)
	{
		return;
	}

	m_nearElements.sort(Greater(pView->camera.za));

	for (TransparentElementIter it = m_nearElements.begin();
		it != m_nearElements.end(); ++it)
	{
		DrawPuppet(pView, (*it).m_pPuppet, (*it).m_pBox);
		pView->Flush(2);
	}
}

void CSceneManager::Render_RenderWeatherFullScreenOverlay(WView* pView)
{
	if (AfxGetTask()->GetActor("LobbyBg") == 0)
	{
		if (m_pWeather)
		{
			m_pWeather->DisplayFullScreenOverlay();
		}
	}

	if (m_pLobbyWeather)
	{
		m_pLobbyWeather->DisplayFullScreenOverlay();
	}
}

void CSceneManager::Render_RenderDropPosition(WView* pView)
{
	if (!(GOLFDOC()->GameMode() & 0x200))
	{
		return;
	}

	for (std::list<sDrop>::iterator it = m_drops.begin(); it != m_drops.end();
		++it)
	{
		pView->DrawLine((*it).m_pos -
				(pView->camera.ya + pView->camera.xa) * 4.0f,
			(*it).m_color,
			(*it).m_pos + (pView->camera.ya + pView->camera.xa) * 4.0f,
			(*it).m_color, 0x300000);
		pView->DrawLine((*it).m_pos -
				(pView->camera.xa - pView->camera.ya) * 4.0f,
			(*it).m_color,
			(*it).m_pos + (pView->camera.xa - pView->camera.ya) * 4.0f,
			(*it).m_color, 0x300000);
	}

	if (GOLFDOC()->m_bDropRequest && GOLFDOC()->m_bShowDropPos)
	{
		WVector cup = GetHoleCupPos();
		CGolfBall& ball = GolfBall();

		WVector d = ball.m_pos - cup;
		d.y = 0.0f;
		WTVertex fan[3];
		WTVertex* polygon[4];
		for (int n = 0; n < 3; n++)
		{
			polygon[n] = &fan[n];
			fan[n].diffuse = 0x66000000;
		}
		polygon[3] = 0;
		float dist = sqrtf((d * d));
		fan[2].SetPosition(GetHoleCupPos());

		vtr[0].diffuse = 0xFF000000;
		vtr[1].diffuse = 0xFF000000;
		vtr[2].diffuse = 0xFF000000;
		vtr[3].diffuse = 0xFF000000;

		float radius = dist - 25.6f;
		for (int i = 0; i < 128; i++)
		{
			for (int k = 0; k < 2; k++)
			{
				float angle = (((float)(((k & 1) + i) % 128) * (g_PI / 64.0f)));
				fan[k].SetPosition(GetHoleCupPos() +
					WVector(cosf(angle), 0.0f, sinf(angle)) * radius);
			}
			pView->DrawPolygonFan(polygon, 0x300000, 0x400, false);

			for (int j = 0; j < 4; j++)
			{
				float r = (j & 2) ? radius + 1.0f : radius;
				float angle = (((float)(((j & 1) + i) % 128) * (g_PI / 64.0f)));
				vtr[j].SetPosition(WVector(cosf(angle), 0.0f, sinf(angle)) * r +
					GetHoleCupPos());
			}
			pView->DrawPolygonFan(vtx, 0x300000, 0x400, false);
		}
	}
}

void CSceneManager::DrawImposter(WView* pView, const TransparentElement& elem)
{
	for (int i = 0; i < 4; i++)
	{
		float u = (float)(i >> 1);
		float v = (float)(1 - (i & 1));
		WVector pos;
		pos = elem.m_pPuppet->GetBoundSphere().pos;
		pos += elem.m_pPuppet->GetBoundSphere().radius * (u * 2.0f - 1.0f) *
			pView->camera.xa;
		pos.y += elem.m_pPuppet->GetBoundSphere().radius * (1.0f - v * 2.0f);

		vtr[i].SetPosition(pos);

		vtr[i].diffuse =
			((255 - elem.m_pPuppet->GetRootBone()->GetAlpha()) << 24) |
			0xFFFFFF;
		vtr[i].tu = u;
		vtr[i].tv = v;
	}

	pView->DrawPolygonFan(vtx, (elem.m_texture & 0x7FF) | 0x28400000, 0, false);

	if ((unsigned char)(m_debugDraw.bits >> 3) & 1)
	{
		pView->DrawPolygonFan(vtx, 0x4000000, 0, false);
	}
}

void CSceneManager::DrawPuppet(WView* pView, WPuppet* pPuppet, Waabb* pBox)
{
	pPuppet->Render(pView, true, pPuppet->GetMatrix().xa.Magnitude(),
		pView->InFrustumSafe(pPuppet->GetBoundBox()), false, false);

	if (m_debugDraw.bits & 0x1)
	{
		pView->DrawAABB(pPuppet->GetBoundBox(), 0xFFFF0000, false, 0);
	}

	if ((unsigned char)(m_debugDraw.bits >> 1) & 1)
	{
		w_bound_box* box = pPuppet->FindBoundBox("char0");
		if (box)
		{
			int index = 0;
			do
			{
				Wobb obb;
				obb = box->bone->GetMatrix();
				obb *= (box->info->aabb.max - box->info->aabb.min) * 0.5f +
					WVector::ONE * 1.6f;
				const WMatrix& mat = box->bone->GetMatrix();
				const WVector& pivot = box->info->spherePivot;
				obb.center = pivot * mat;
				pView->DrawOBB(obb, 0xFF00FF00);
				++index;
				box = pPuppet->FindBoundBox(MakeStr("char%d", index));
			} while (box);
		}
		else if (pBox)
		{
			pView->DrawAABB(*pBox, 0xFF00FF00, false, 0);
		}
	}

	if ((unsigned char)(m_debugDraw.bits >> 2) & 1)
	{
		pView->DrawSphere(pPuppet->GetBoundSphere(), 0xFFFFFF00, 0x18, 0);
	}
}

void CSceneManager::RenderShadow(WView* pView, unsigned long mask)
{
	int caster = 0;
	SceneElementIter it;

	for (it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		if (&(*it) == 0)
		{
			continue;
		}

		if (!(*it).m_visible)
		{
			continue;
		}

		if ((mask & (1 << (*it).m_type)) == 0)
		{
			continue;
		}

		if ((*it).m_type != 0)
		{
			continue;
		}

		const Waabb& box = (*it).m_pPuppet->GetBoundBox();

		CShadowManager::Instance()->RegisterRecver_New("bg");
		CShadowManager::Instance()->AddRecvPet_New("bg", (*it).m_pPuppet,
			box.min.y);

		for (it = m_elements.begin(); it != m_elements.end(); ++it)
		{
			if (&(*it) == 0)
			{
				continue;
			}

			if (!(*it).m_visible)
			{
				continue;
			}

			if ((mask & (1 << (*it).m_type)) == 0)
			{
				continue;
			}

			if ((*it).m_type == 0)
			{
				continue;
			}

			if (((*it).m_flags & 0x30) == 0)
			{
				continue;
			}

			if ((*it).m_pRender)
			{
				(*it).m_pRender->DisplayShadow();
				continue;
			}

			if ((*it).m_pPuppet == 0 || (*it).m_pPuppet->GetRootBone() == 0)
			{
				continue;
			}

			int state;
			if ((*it).m_flags & 0x20)
			{
				state = 0;
			}
			else
			{
				state = 0x10;
			}

			CShadowManager::Instance()->RegisterCaster_New(
				MakeStr("caster%d", caster), state, "sun", g_lightset);
			CShadowManager::Instance()->AddCastPet_New(
				MakeStr("caster%d", caster), false, (*it).m_pPuppet);
			caster++;
		}

		CShadowManager::Instance()->Flush(pView);

		CShadowManager::Instance()->Reset_New();
		return;
	}
}

void CSceneManager::Clear(void)
{
	if (m_pSea)
	{
		delete m_pSea;
		m_pSea = NULL;
	}

	for (CPool* pool = m_poolList.Start(); pool; pool = m_poolList.Next())
	{
		delete pool;
	}
	m_poolList.Reset();
	for (CBeach* beach = m_beachList.Start(); beach; beach = m_beachList.Next())
	{
		delete beach;
	}
	m_beachList.Reset();
	for (CWavelet* wavelet = m_waveletList.Start(); wavelet;
		wavelet = m_waveletList.Next())
	{
		delete wavelet;
	}
	m_waveletList.Reset();
	for (ntRiver* river = m_riverList.Start(); river;
		river = m_riverList.Next())
	{
		delete river;
	}
	m_riverList.Reset();
	if (m_pMarker)
	{
		m_pMarker->Reset();
	}
}

void CSceneManager::Load(void)
{
	Clear();

	for (std::vector<CNodeContainer>::const_iterator it =
			 GOLFDOC()->m_pPolySoup->m_node.begin();
		it != GOLFDOC()->m_pPolySoup->m_node.end(); ++it)
	{
		if (it->GetType() == 10)
		{
			m_pSea = new CSea;
			m_pSea->SetArea(it->GetList());
		}
		else if (it->GetType() == 5)
		{
			CBeach* pBeach = new CBeach;
			pBeach->SetWavelet(it->GetList(), it->GetName().c_str(), 3);

			m_beachList += pBeach;
		}
		else if (it->GetType() == 7)
		{
			ntRiver* pRiver = new CRiver;
			pRiver->Setup(it->GetList(), it->GetName().c_str());

			m_riverList += pRiver;
		}
		else if (it->GetType() == 8)
		{
			ntRiver* pRiver = new CRiver_ex;
			pRiver->Setup(it->GetList(), it->GetName().c_str());

			m_riverList += pRiver;
		}
		else if (it->GetType() == 13)
		{
			ntRiver* pRiver = new ntRiver_Lava;
			pRiver->Setup(it->GetList(), it->GetName().c_str());

			m_riverList += pRiver;
		}
		else if (it->GetType() == 11)
		{
			CWavelet* pWavelet = new CWavelet;
			pWavelet->SetWavelet(it->GetList(), it->GetName().c_str(), 2);

			m_waveletList += pWavelet;
		}
		else if (it->GetType() == 3 || it->GetType() == 2)
		{
			m_pMarker->AddList(it->GetList());
		}
	}

	for (int i = 0; i < GOLFDOC()->m_pPolySoup->GetAreaNum(); i++)
	{
		WPolySoup::sArea* pArea = GOLFDOC()->m_pPolySoup->GetArea(i);

		if (pArea->type == 2)
		{
			CPool* pPool = CPool::CreatePoolByMapType((eMapType)GetCurMap());
			pPool->SetArea(pArea->aabb, pArea->name);

			m_poolList += pPool;
		}
	}

	m_pMarker->GetReady();
}

static void __stdcall EnableShadows(bool shadow, bool shadowmap)
{
	if (COption::Instance()->vIsShadowEnabled())
	{
		CShadowManager::Instance()->Enable(shadow);
	}

	if (COption::Instance()->vIsShadowmapEnabled())
	{
		GOLFDOC()->m_pPolySoup->EnableShadowmap(shadowmap);
	}
}

void CSceneManager::SetWeather(unsigned char weather, bool lobby)
{
	if (lobby)
	{
		if (m_pWeather)
		{
			delete m_pWeather;
			m_pWeather = 0;
		}

		if (m_pLobbyWeather)
		{
			delete m_pLobbyWeather;
			m_pLobbyWeather = 0;
		}
	}

	if ((GetCurMap() == 11 || GetCurMap() == 13 || GetCurMap() == 19) &&
		weather != 2)
	{
		if (!m_pLobbyWeather)
		{
			int count = 0;
			switch (COption::Instance()->vGetParticleLevel())
			{
			case 0:
				count = 900;
				break;
			case 1:
				count = 500;
				break;
			case 2:
				count = 200;
				break;
			}

			if (GetCurMap() == 19 && count > 200)
			{
				m_pLobbyWeather =
					new CSnow(count / 20, "~bubble.jpg", 0, 128, 128);
			}
			else if (GetCurMap() == 11)
			{
				m_pLobbyWeather =
					new ntImplFlutter<sPetalNameHolder>(count, "", 0);
			}
			else if (GetCurMap() == 13)
			{
				m_pLobbyWeather =
					new ntImplFlutter<sSilicaParticleNameHolder>(count, "", 0);
			}
			m_pLobbyWeather->SetGravity(0.025f);
		}
	}
	else
	{
		if (m_pLobbyWeather)
		{
			delete m_pLobbyWeather;
			m_pLobbyWeather = 0;
		}
	}

	if (GetCurMap() == 9 && (__rtti_obj = AfxGetTask()) != 0 &&
		((IObject*)__rtti_obj)->IsKindOf(&CGolfTask::m_RTTI))
	{
		if (!m_pWeather)
		{
			m_pWeather = new CSnow(0x400, "~GlowGray.jpg", 0, 32, 32);
			m_pWeather->SetGravity(0.25f);
		}
	}
	else if (IsCurMapSnow() && weather != 2 && weather != 3)
	{
		if (!m_pWeather)
		{
			int count = 0x1000;
			if (GetCurMap() == 14 || GetCurMap() == 17)
			{
				count = 0x666;
			}
			m_pWeather = new ntIceSnow(count, "~GlowGraySmall.jpg", 0);
			m_pWeather->SetGravity(0.08f);
		}
	}
	else
	{
		if (GetWeather() == weather && !lobby)
		{
			return;
		}

		if (m_pWeather)
		{
			delete m_pWeather;
			m_pWeather = 0;
		}
	}

	switch (weather)
	{
	case 0:
		if (m_pWeather && GetCurMap() == 9)
		{
			m_pWeather->Reset(rand() % 512 + 512, 0.5f);
		}
		EnableShadows(true, true);
		break;
	case 1:
		if (!m_pWeather)
		{
			m_pWeather = new CCloud;
		}

		if (GetCurMap() == 9)
		{
			m_pWeather->Reset(rand() % 512 + 512, 0.5f);
		}
		EnableShadows(false, false);
		break;
	case 2:
		if (!m_pWeather)
		{
			if (IsCurMapSnow())
			{
				m_pWeather = new CSnow(0x800, "~GlowGray.jpg", 0, 32, 32);
				m_pWeather->SetGravity(0.25f);
			}
			else
			{
				m_pWeather = new CRain;
			}
		}

		if (GetCurMap() == 9 || IsCurMapSnow())
		{
			m_pWeather->Reset(0x800, 1.0f);
		}
		EnableShadows(false, false);
		break;
	case 3:
		if (!m_pWeather)
		{
			m_pWeather = new CSnow(0x800, "~GlowGray.jpg", 0, 32, 32);
			m_pWeather->SetGravity(0.25f);
		}
		else if (GetCurMap() == 9 || IsCurMapSnow())
		{
			m_pWeather->Reset(0x800, 1.0f);
		}
		EnableShadows(false, false);
		break;
	case 4:
		if (!m_pWeather)
		{
			m_pWeather = new CSnow(0x200, "[blue_eft01.jpg", 2, 32, 32);
			m_pWeather->SetGravity(-0.25f);
		}
		break;
	}

	::SetWeather(weather);
}

void CSceneManager::SetLobbyWeather(bool weather)
{
	EnableShadows(true, true);

	if (m_pWeather)
	{
		delete m_pWeather;
		m_pWeather = NULL;
	}

	if (m_pLobbyWeather)
	{
		delete m_pLobbyWeather;
		m_pLobbyWeather = NULL;
	}

	if (m_pLobbyWeather)
	{
		return;
	}

	int count = 0;
	switch (COption::Instance()->vGetParticleLevel())
	{
	case 0:
		count = 900;
		break;
	case 1:
		count = 500;
		break;
	case 2:
		count = 200;
		break;
	}

	float gravity = 0.0f;
	switch (LOBBYBG_MAP)
	{
	case 13:
		m_pLobbyWeather =
			new ntImplFlutter<sSilicaParticleNameHolder>(count, "", 0);
		gravity = 0.025f;
		break;
	case 11:
		m_pLobbyWeather = new ntImplFlutter<sPetalNameHolder>(count, "", 0);
		gravity = 0.025f;
		break;
	case 8:
	case 9:
	case 14:
		m_pLobbyWeather = new ntIceSnow(count * 4, "~GlowGraySmall.jpg", 0);
		gravity = 0.08f;
		break;
	case 19:
		if (count > 200)
		{
			m_pLobbyWeather = new CSnow(count / 20, "~bubble.jpg", 0, 128, 128);
		}
		gravity = 0.025f;
		break;
	}

	if (m_pLobbyWeather)
	{
		m_pLobbyWeather->SetGravity(gravity);
	}
}

void CSceneManager::DrawDebugLine(WView* pView)
{
	if ((unsigned char)(m_debugDraw.bits >> 4) & 1)
	{
		Waabb& box = GOLFDOC()->m_pPolySoup->FindModel(0)->aabb;
		for (int j = 0; j < 4; j++)
		{
			WVector v;
			v.x = (j & 1) ? box.max.x : box.min.x;
			v.z = (j & 2) ? box.max.z : box.min.z;
			v.y = GetPVS().GetLowest();

			vtr[j].SetPosition(v);
			vtr[j].diffuse = 0x808080FF;
		}
		pView->DrawPolygonFan(vtx, 0, 0, false);
	}

	if ((unsigned char)(m_debugDraw.bits >> 5) & 1)
	{
		Waabb& box = GOLFDOC()->m_pPolySoup->FindModel(0)->aabb;
		for (int j = 0; j < 4; j++)
		{
			WVector v;
			v.x = (j & 1) ? box.max.x : box.min.x;
			v.z = (j & 2) ? box.max.z : box.min.z;
			v.y = GetPVS().GetLowest() + 192.0f;

			vtr[j].SetPosition(v);
			vtr[j].diffuse = 0x808080FF;
		}
		pView->DrawPolygonFan(vtx, 0, 0, false);
	}

	if (m_bDrawElementBoxes)
	{
		WVector top;
		for (SceneElementIter it = m_elements.begin(); it != m_elements.end();
			++it)
		{
			if ((*it).m_pPuppet == 0)
			{
				continue;
			}

			top = ((*it).m_pPuppet->GetBoundBox().max +
					  (*it).m_pPuppet->GetBoundBox().min) *
				0.5f;
			top.y = (*it).m_pPuppet->GetBoundBox().max.y;

			if ((top - pView->camera.pivot).Magnitude() > 192.0f &&
				pView->InFrustum((*it).m_pPuppet->GetBoundBox()))
			{
				continue;
			}

			WVector screen;
			screen = pView->Projection(top);
			if (0.0f < screen.z && screen.z < 1.0f)
			{
				m_pFont->Print(pView,
					screen.x -
						m_pFont->GetTextWidth(pView, (*it).m_name) * 0.5f,
					screen.y, (*it).m_name, 0, 0xFFFFFFFF, 0);
			}
		}
		m_pFont->Flush(pView);
	}
}
