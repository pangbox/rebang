#include "projectg.h"
#include "sea.h"
#include "quadtree.h"

CSea::CSea()
	: m_pPoint(NULL), m_pointNum(0), m_layerNum(0), m_scale(1.0f)
{
	m_pLayer = new sLayer[3];
}

CSea::~CSea()
{
	for (int i = 0; i < m_layerNum; i++)
	{
		if (g_resrcmng && m_pLayer[i].texHandle)
		{
			g_resrcmng->Release(m_pLayer[i].texHandle);
			m_pLayer[i].texHandle = 0;
		}
	}

	if (m_pLayer)
	{
		delete[] m_pLayer;
		m_pLayer = NULL;
	}
	if (m_pPoint)
	{
		delete[] m_pPoint;
		m_pPoint = NULL;
	}
}

void CSea::LoadTexture()
{
	for (int i = 0; i < m_layerNum; i++)
	{
		if (g_resrcmng && m_pLayer[i].texHandle)
		{
			g_resrcmng->Release(m_pLayer[i].texHandle);
			m_pLayer[i].texHandle = 0;
		}
		m_pLayer[i].texHandle =
			g_resrcmng->LoadTexture(MakeStr("[sea_0%d.jpg", i + 1), 0, 0, 0);
	}
}

void CSea::SetArea(const std::vector<WVector>& area)
{
	m_pointNum = area.size();
	m_pPoint = new WVector[m_pointNum];

	m_area = Waabb(WVector::ONE * 10000.0f, WVector::ONE * -10000.0f);

	for (int i = 0; i < m_pointNum; i++)
	{
		m_pPoint[i] = area[i];
		m_pPoint[i].y = area[0].y;

		for (int k = 0; k < 3; k++)
		{
			m_area.min.p[k] = Min(m_area.min.p[k], m_pPoint[i].p[k]);
			m_area.max.p[k] = Max(m_area.max.p[k], m_pPoint[i].p[k]);
		}
	}

	m_area.max.y -= 1.0f;
	m_area.min.y = m_area.max.y - 1.0f;

	int last = m_pointNum - 1;
	for (int j = 0; j < m_pointNum / 2; j++)
	{
		WVector a = area[j];
		WVector b = area[last - j];
		WVector c = area[j + 1];
		WVector d = area[last - j - 1];

		Waabb box;
		box.max.x = max(max(a.x, b.x), max(c.x, d.x));
		box.max.y = m_area.max.y;
		box.max.z = max(max(a.z, b.z), max(c.z, d.z));
		box.min.x = min(min(a.x, b.x), min(c.x, d.x));
		box.min.y = m_area.min.y;
		box.min.z = min(min(a.z, b.z), min(c.z, d.z));

		GetPVS().AddWater(box, 1);
	}

	Init();
}

void CSea::Init()
{
	float rad = 30.0f * g_DEGTORAD;
	float speedV = sinf(rad) * -0.02f;
	float speedU = cosf(rad) * 0.02f;

	AddLayer(-speedU, speedV, 200.0f, 255);
	AddLayer(speedU, speedV, 100.0f, 255);
	AddLayer(0.0f, 0.02f, 150.0f, 255);
}

void CSea::AddLayer(float speedU, float speedV, float scale,
	unsigned char alpha)
{
	sLayer& layer = m_pLayer[m_layerNum++];

	layer.speed.x = speedU;
	layer.speed.y = speedV;
	layer.color = g_lightset.ambient2 | (alpha << 24);
	layer.pUV = new WVector2D[m_pointNum];
	layer.ppVertex = new WTVertex*[m_pointNum + 1];
	layer.pVertex = new WTVertex[m_pointNum];
	layer.texHandle = g_resrcmng->LoadTexture(
		MakeStr("[sea_0%d.jpg", Min(m_layerNum, 2)), 0, 0, 0);

	int i;
	for (i = 0; i < m_pointNum; i++)
	{
		layer.pUV[i].x = (m_pPoint[i].x - m_area.min.x) * (1.0f / scale);
		layer.pUV[i].y = (m_pPoint[i].z - m_area.min.z) * (1.0f / scale);
		layer.ppVertex[i] = &layer.pVertex[i];
		layer.pVertex[i].diffuse = layer.color;
	}
	layer.ppVertex[i] = NULL;
}

void CSea::Process(float delta)
{
	for (int i = 0; i < m_layerNum; i++)
	{
		for (int j = 0; j < m_pointNum; j++)
			m_pLayer[i].pUV[j] += WVector2D(delta * m_pLayer[i].speed.x,
				delta * m_pLayer[i].speed.y);
	}
}

void CSea::Render()
{
	if (!g_view->InFrustum(m_area))
		return;

	for (int i = 0; i < m_layerNum; i++)
	{
		sLayer& layer = m_pLayer[i];

		for (int j = 0; j < m_pointNum; j++)
		{
			WVector pos = m_pPoint[j];
			pos.y = m_area.max.y;
			layer.pVertex[j].SetPosition(pos);
			layer.pVertex[j].tu = layer.pUV[j].x;
			layer.pVertex[j].tv = layer.pUV[j].y;
		}

		g_view->DrawPolygonFan(layer.ppVertex,
			(layer.texHandle & 0x7ff) | 0x20800000, 0x400, false);
	}
}
