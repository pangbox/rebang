#include "stdafx.h"
#include "pool.h"
#include "quadtree.h"
#include "slicestr.h"
#include "clientsetting.h"
#include "mathconsts.h"

float causticSpeed = 1;
int causticAlpha = 255;

inline CPoolCaustic::sLayer::~sLayer()
{
	if (g_resrcmng && texHandle)
	{
		g_resrcmng->Release(texHandle);
		texHandle = 0;
	}
}

CPool* CPool::CreatePoolByMapType(eMapType type)
{
	switch (type)
	{
	case 8:
	case 11:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 19:
		return new CPoolCaustic;
	default:
		return new CPoolReflective;
	}
}

CPool::CPool()
	: m_type(0), m_flag()
{
	m_area.min.Reset();
	m_area.max.Reset();
}

CPool::~CPool()
{
}

void CPool::SetArea(const Waabb& area, const char* option)
{
	m_area = area;
	m_type = 0;
	WSliceStr slice(option);
	slice.Get("*type", "%d", &m_type);
	if (strstr(option, "*noripple"))
		m_type = 1;
	switch (m_type)
	{
	case 1:
		m_flag.Enable(3);
		break;
	case 2:
		m_flag.Enable(13);
		break;
	}
	if (!m_flag.GetFlag(4))
		GetPVS().AddWater(m_area, 3);
	BuildMesh();
}

CPoolCaustic::CPoolCaustic()
	: m_layerNum(0)
{
}

CPoolCaustic::~CPoolCaustic()
{
}

inline void CPoolCaustic::SetReflection(bool)
{
}

inline void CPoolCaustic::Prerender(WView*)
{
}

void CPoolCaustic::Process(float dt)
{
	WView* view = g_view;
	if (!view->InFrustum(m_area) || !view->InFrustum(m_causticArea))
		return;

	float speed = 1.0f;
	if (GetCurMap() == 13)
		speed = .7f;
	else if (GetCurMap() == 14 && m_type == 2)
		speed = .5f;

	for (int i = 0; i < m_layerNum; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_layer[i].vtx[j].tu += (float)(speed * m_layer[i].speedU) * dt;
			m_layer[i].vtx[j].tv += speed * m_layer[i].speedV * dt;
		}
	}
}

void CPoolCaustic::Render(WView* view)
{
	if (!view->InFrustum(m_area) || !view->InFrustum(m_causticArea))
		return;

	for (int i = 0; i < m_layerNum; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_layer[i].vtx[j].SetPosition(m_corner[j]);
			m_layer[i].vtx[j].diffuse =
				(causticAlpha << 24) | (m_layer[i].vtx[j].diffuse & 0xffffff);
		}
		unsigned long flag = 0x800000;
		if (GetCurMap() == 14 && i < 2)
			flag = 0;
		view->DrawPolygonFan(m_layer[i].vl,
			flag | (m_layer[i].texHandle & 0x7ff) | 0x20400000, 0x400, 0);
	}
}

void CPoolCaustic::BuildMesh()
{
	m_causticArea = m_area;
	m_causticArea.max.y -= 1;
	m_causticArea.min.y = m_causticArea.max.y - 1;
	m_corner[0] =
		WVector(m_causticArea.max.x, m_causticArea.max.y, m_causticArea.max.z);
	m_corner[1] =
		WVector(m_causticArea.max.x, m_causticArea.max.y, m_causticArea.min.z);
	m_corner[2] =
		WVector(m_causticArea.min.x, m_causticArea.max.y, m_causticArea.min.z);
	m_corner[3] =
		WVector(m_causticArea.min.x, m_causticArea.max.y, m_causticArea.max.z);
	float scale = 1.3f;
	if (GetCurMap() == 14 && m_type == 2)
		scale *= .25f;
	float v = sinf(30 * g_DEGTORAD) * -.02f, u = cosf(30 * g_DEGTORAD) * .02f;
	AddLayer(-u, v, scale * 200, 255);
	AddLayer(u, v, scale * 100, 255);
	AddLayer(0, .02f, scale * 150, 255);
}

void CPoolCaustic::AddLayer(float speedU, float speedV, float scale,
	unsigned char alpha)
{
	if (m_layerNum >= 3)
		return;
	sLayer& layer = m_layer[m_layerNum];
	layer.speedU = speedU;
	layer.speedV = speedV;
	int index = Min(m_layerNum + 1, 2);
	const char* name = GetCurMapName();
	if (!stricmp(name, "pola") && m_type == 0)
		name = "icee";
	layer.texHandle = g_resrcmng->LoadTexture(
		MakeStr("[caustic_%s%02d.jpg", name, index), 0, 0, 0);
	if (!layer.texHandle)
		layer.texHandle = g_resrcmng->LoadTexture(
			MakeStr("[caustic_%02d.jpg", index), 0, 0, 0);
	float inverse = 1.0f / scale;
	int i;
	for (i = 0; i < 4; i++)
	{
		layer.vtx[i].tu = (m_corner[i].x - m_causticArea.min.x) * inverse;
		layer.vtx[i].tv = (m_corner[i].z - m_causticArea.min.z) * inverse;
		layer.vtx[i].diffuse = g_lightset.ambient2 | (alpha << 24);
		layer.vl[i] = &layer.vtx[i];
	}
	layer.vl[i] = NULL;
	++m_layerNum;
}

CPoolReflective::CPoolReflective()
	: m_grid(NULL), m_pReflectView(NULL)
{
	m_vl[0] = &m_vtx[0];
	m_vl[1] = &m_vtx[1];
	m_vl[2] = &m_vtx[3];
	m_vl[3] = &m_vtx[2];
	m_vl[4] = NULL;
}

CPoolReflective::~CPoolReflective()
{
	if (m_grid)
	{
		for (int i = 0; i < m_gridZ; i++)
		{
			if (m_grid[i])
			{
				delete[] m_grid[i];
				m_grid[i] = NULL;
			}
		}
		if (m_grid)
		{
			delete[] m_grid;
			m_grid = NULL;
		}
	}
	if (m_pReflectView)
	{
		delete m_pReflectView;
		m_pReflectView = NULL;
	}
}

void CPoolReflective::SetReflection(bool enable)
{
	if (enable)
	{
		if (!m_pReflectView)
		{
			m_pReflectView = new WTextureView;
			m_pReflectView->SetResourceManager(g_resrcmng);
			m_pReflectView->SetViewport(g_view->GetWidth() * .5f,
				g_view->GetHeight() * .5f);
			m_pReflectView->SetReflective(true);
			WTextureView::TextureParam param;
			param.m_numRts = 1;
			param.m_rtTexInfo[0].Set(0x14000,
				WRenderToTextureSizeInfo::SIZE_HALF, false, 0);
			param.m_depthSurfInfo.Set(
				WRenderToTextureParam::DepthSurfInfo::USE_SHARED_SURFACE,
				WRenderToTextureSizeInfo::SIZE_HALF, true, 1.0f);
			m_pReflectView->SetTextureParam(param);
		}
	}
	else if (m_pReflectView)
	{
		delete m_pReflectView;
		m_pReflectView = NULL;
	}
}

void CPoolReflective::Process(float dt)
{
	if (!g_view->InFrustum(m_area))
		return;
	if (!m_flag.GetFlag(1) && g_CurrentTime - m_dropTime > 1000)
	{
		m_dropTime = g_CurrentTime;
		Droplet();
	}
	if (!m_flag.GetFlag(2))
	{
		MakeNorm();
		Wave(dt);
	}
}

void CPoolReflective::Prerender(WView* view)
{
	if (view->GetCamera().pivot.y < m_area.max.y || !view->InFrustum(m_area) ||
		!m_pReflectView || !CSceneManager::Instance())
		return;
	WVideoDev* video = m_pReflectView->GetVideoDevice();
	m_pReflectView->SetFOV(view->GetFOV());
	WMatrix mat = view->GetCamera() *
		~WMatrix(WPlane(WVector::UNIT_POS_Y, -m_area.max.y));
	m_pReflectView->SetClip(.1f, view->GetClipFarValue(), false);
	m_pReflectView->SetCamera(mat);
	m_pReflectView->UpdateCamera();
	WView* oldView = g_view;
	WMatrix oldCamera = g_camera;
	g_view = m_pReflectView;
	g_camera = m_pReflectView->GetCamera();
	m_pReflectView->Begin();
	CSceneManager::Instance()->Render(m_pReflectView, 128);
	if (!m_flag.GetFlag(8))
	{
		WPlane plane(WVector::UNIT_POS_Y, 1 - m_area.max.y);
		video->BeginUsingCustomRenderState();
		video->SetCustomRenderState(WVDRS_CLIPPLANEENABLE, 1);
		video->SetCustomClipPlane(0, plane);
		CSceneManager::Instance()->Render(m_pReflectView, 13);
		video->EndUsingCustomRenderState();
	}
	m_pReflectView->Flush(0);
	m_pReflectView->End();
	g_view = oldView;
	g_camera = oldCamera;
}

// HACK!
__declspec(dllimport) __forceinline float PoolSpecularDot(const WVector& normal,
	const WVector& normalX, const WVector& half)
{
	return normalX.x * half.x + normal.y * half.y + normal.z * half.z;
}

void CPoolReflective::Render(WView* view)
{
	if (view->GetCamera().pivot.y < m_area.max.y || !view->InFrustum(m_area))
		return;

	int tex = m_pReflectView ? m_pReflectView->GetTexture(0) : 0;
	WVideoDev* dev = view->GetVideoDevice();
	if (tex)
	{
		dev->BeginUsingCustomRenderState();
		WMatrix4 m = view->xGetViewState().xmView *
			view->xGetViewState().xmProj * WView::ms_uvConvMat;
		dev->SetCustomFxMacro(0x10000);
		dev->SetCustomFxParamMatrix(WVDFXPARAM_TEXTURE_TRANSFORM, m);
	}

	unsigned char alpha;
	if (IsCurMapSnow() || GetCurMap() == 10)
		alpha = 40;
	else if (m_type == 2)
		alpha = 80;
	else
		alpha = 160;

	float d = max(-view->GetCamera().za * WVector::UNIT_POS_Y, 0.0f);
	float fresnel = 1.0f - d;
	float sun;

	if (m_flag.GetFlag(2))
	{
		WVector pos;
		pos.y = m_area.max.y;
		sun = max(-g_lightset.nearone * WVector::UNIT_POS_Y, 0.0f);
		float spec =
			max(((view->GetCamera().pivot - m_area.GetCenter()).Normalize() -
					g_lightset.nearone)
						.Normalize() *
					WVector::UNIT_POS_Y,
				0.0f);
		spec *= spec;

		unsigned long color =
			AddDiffuse(AddDiffuse(AddDiffuse(0, g_lightset.ambient, 0x66),
						   g_lightset.diffuse, (unsigned char)(sun * 102.0f)),
				0x808080, (unsigned char)(spec * 255.0f));
		color |= (unsigned char)(((1.0f - fresnel) * 0.5f + fresnel) * alpha)
			<< 24;

		for (int k = 0; k < 4; k++)
		{
			pos.x = ((k >> 1) & 1) ? m_area.max.x : m_area.min.x;
			pos.z = (k & 1) ? m_area.max.z : m_area.min.z;
			m_vtx[k].SetPosition(pos);
			m_vtx[k].diffuse = color;
		}

		view->DrawPolygonFan(m_vl, (tex & 0x7ff) | 0xa400000, 0, false);
	}
	else
	{
		for (int i = 0; i < m_gridZ - 1; i++)
		{
			for (int j = 0; j < m_gridX - 1; j++)
			{
				int count = 0;
				for (int k = 0; k < 4; k++)
				{
					sWaveVertex* v =
						&m_grid[i + (k & 1)][j + ((unsigned)k >> 1 & 1)];
					if (!v->bWater)
						count++;

					m_vtx[k].SetPosition(v->pos);

					sun = max(-g_lightset.nearone * v->normal, 0.0f);
					float spec =
						(((view->GetCamera().pivot - v->pos).Normalize() -
							 g_lightset.nearone)
									.Normalize() *
								v->normal >
							0.0f)
						? PoolSpecularDot(v->normal, v->normal,
							  ((view->GetCamera().pivot - v->pos).Normalize() -
								  g_lightset.nearone)
								  .Normalize())
						: 0.0f;
					float squaredSpec = spec * spec;

					unsigned long color = AddDiffuse(
						AddDiffuse(AddDiffuse(0, g_lightset.ambient, 0x66),
							g_lightset.diffuse, (unsigned char)(sun * 102.0f)),
						0x808080, (unsigned char)(squaredSpec * 255.0f));
					m_vtx[k].diffuse = color |
						((unsigned char)(((1.0f - fresnel) * 0.5f + fresnel) *
							 alpha)
							<< 24);
				}

				if (count != 4)
					view->DrawPolygonFan(m_vl, (tex & 0x7ff) | 0xa400000, 0,
						false);
			}
		}
	}

	if (tex)
		dev->EndUsingCustomRenderState();
}

void CPoolReflective::BuildMesh()
{
	if (!m_flag.GetFlag(2))
	{
		m_gridX = (int)ceilf((m_area.max.x - m_area.min.x) / 20.f) + 1;
		m_gridZ = (int)ceilf((m_area.max.z - m_area.min.z) / 20.f) + 1;
		float stepX = (m_area.max.x - m_area.min.x) / (m_gridX - 1),
			  stepZ = (m_area.max.z - m_area.min.z) / (m_gridZ - 1);
		m_grid = new sWaveVertex*[m_gridZ];
		for (int z = 0; z < m_gridZ; z++)
			m_grid[z] = new sWaveVertex[m_gridX];
		for (int z = 0; z < m_gridZ; z++)
		{
			for (int x = 0; x < m_gridX; x++)
			{
				m_grid[z][x].pos.x = x * stepX + m_area.min.x;
				m_grid[z][x].pos.y = m_area.max.y;
				m_grid[z][x].pos.z = z * stepZ + m_area.min.z;
				m_grid[z][x].force = 0;
				m_grid[z][x].speed = 0;
				m_grid[z][x].tu = .5f;
				m_grid[z][x].tv = .5f;
				m_grid[z][x].normal = WVector::UNIT_POS_Y;
				m_grid[z][x].bWater = false;
			}
		}
		m_dropTime = g_CurrentTime;
		ExcludePoint();
	}
	SetReflection(COption::Instance()->vIsReflectionEnabled());
}

void CPoolReflective::Droplet()
{
	int x = rand() % (m_gridX - 1), z = rand() % (m_gridZ - 1);
	m_grid[z][x].speed += 2;
	m_grid[z][x + 1].speed += 2;
	m_grid[z + 1][x].speed += 2;
	m_grid[z + 1][x + 1].speed += 2;
}

void CPoolReflective::MakeNorm()
{
	for (int z = 0; z < m_gridZ; z++)
	{
		for (int x = 0; x < m_gridX; x++)
		{
			if (!m_grid[z][x].bWater)
				continue;
			sWaveVertex& vertex = m_grid[z][x];
			WVector a, b;
			if (z == m_gridZ - 1)
			{
				a = m_grid[z][x == m_gridX - 1 ? x - 1 : x + 1].pos -
					m_grid[z][x].pos;
				b = m_grid[z - 1][x].pos - m_grid[z][x].pos;
			}
			else if (x == m_gridX - 1)
			{
				a = m_grid[z][x - 1].pos - m_grid[z][x].pos;
				b = m_grid[z + 1 == m_gridZ - 1 ? z - 1 : z + 1][x].pos -
					m_grid[z][x].pos;
			}
			else
			{
				a = m_grid[z][x + 1].pos - m_grid[z][x].pos;
				b = m_grid[z + 1][x].pos - m_grid[z][x].pos;
			}
			WVector normal;
			normal = WCrossProduct(b, a);
			normal.Normalize();
			vertex.normal = normal;
			m_grid[z][x].tu = (normal.x + 1) * .5f;
			m_grid[z][x].tv = (normal.z + 1) * .5f;
		}
	}
}

void CPoolReflective::Wave(float dt)
{
	for (int z = 0; z < m_gridZ; z++)
	{
		for (int x = 0; x < m_gridX; x++)
		{
			m_grid[z][x].force = 0;
		}
	}
	for (int z = 1; z < m_gridZ - 1; z++)
	{
		for (int x = 1; x < m_gridX - 1; x++)
		{
			if (m_grid[z][x].bWater)
			{
				float force = m_grid[z][x].pos.y - m_grid[z][x - 1].pos.y;
				m_grid[z][x].force -= force;
				m_grid[z][x - 1].force += force;
				force = m_grid[z][x].pos.y - m_grid[z][x].pos.y;
				m_grid[z][x].force -= force;
				m_grid[z - 1][x].force += force;
				force = m_grid[z][x].pos.y - m_grid[z][x + 1].pos.y;
				m_grid[z][x].force -= force;
				m_grid[z][x + 1].force += force;
				force = m_grid[z][x].pos.y - m_grid[z + 1][x].pos.y;
				m_grid[z][x].force -= force;
				m_grid[z + 1][x].force += force;
				force = (m_grid[z][x].pos.y - m_grid[z + 1][x + 1].pos.y) *
					.70710677f;
				m_grid[z][x].force -= force;
				m_grid[z + 1][x + 1].force += force;
				force = (m_grid[z][x].pos.y - m_grid[z - 1][x - 1].pos.y) *
					.70710677f;
				m_grid[z][x].force -= force;
				m_grid[z - 1][x - 1].force += force;
				force = (m_grid[z][x].pos.y - m_grid[z + 1][x - 1].pos.y) *
					.70710677f;
				m_grid[z][x].force -= force;
				m_grid[z + 1][x - 1].force += force;
				force = (m_grid[z][x].pos.y - m_grid[z + 1][x - 1].pos.y) *
					.70710677f;
				m_grid[z][x].force -= force;
				m_grid[z + 1][x - 1].force += force;
			}
		}
	}
	for (int z = 0; z < m_gridZ; z++)
	{
		for (int x = 0; x < m_gridX; x++)
		{
			if (m_grid[z][x].bWater)
			{
				m_grid[z][x].speed += dt * m_grid[z][x].force;
				m_grid[z][x].pos.y = Max(m_area.max.y - 2,
					m_grid[z][x].pos.y + dt * m_grid[z][x].speed);
			}
		}
	}
}

void CPoolReflective::ExcludePoint()
{
	for (int z = 0; z < m_gridZ; z++)
	{
		for (int x = 0; x < m_gridX; x++)
		{
			int index;
			WVector ground;
			ground =
				GetPVS().GetGroundPoint(m_grid[z][x].pos, true, &index, false);
			if (index >= 0 && ground.y < m_area.max.y &&
				GetPVS().GetMeshNormal(index).y > 0)
				m_grid[z][x].bWater = true;
		}
	}
}

unsigned long __fastcall AddDiffuse(unsigned long a, unsigned long b,
	unsigned char alpha)
{
	unsigned long blue =
		Min<unsigned long>(255, (a & 255) + (((b & 255) * alpha) >> 8));
	unsigned long green = Min<unsigned long>(255,
		((a >> 8) & 255) + ((((b >> 8) & 255) * alpha) >> 8));
	unsigned long red = Min<unsigned long>(255,
		((a >> 16) & 255) + ((((b >> 16) & 255) * alpha) >> 8));
	return (red << 16) | (green << 8) | blue;
}
