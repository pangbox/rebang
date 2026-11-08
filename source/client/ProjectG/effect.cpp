#include "projectg.h"
#include "effect.h"

CRing::CRing()
	: m_pos(NULL), m_pos2(NULL), m_texHandle(0)
{
	m_texHandle = g_resrcmng->LoadTexture("[ring.jpg", 0, 0, 0);
	Init();
}

CRing::CRing(const char* texture)
	: m_pos(NULL), m_pos2(NULL), m_texHandle(0)
{
	m_texHandle = g_resrcmng->LoadTexture(texture, 0, 0, 0);
	Init();
}

CRing::~CRing()
{
	if (m_pos)
	{
		delete[] m_pos;
		m_pos = NULL;
	}
	if (m_pos2)
	{
		delete[] m_pos2;
		m_pos2 = NULL;
	}

	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
}

void CRing::Init()
{
	m_num = 16;
	m_pos = new WVector[16];
	m_pos2 = new WVector[m_num];

	m_bActive = false;

	m_center.Reset();
	m_radius = 3.0f;
	m_height = 0.6f;

	m_angle = 0.0f;
	m_scale = 0.98f;
}

void CRing::SetCenter(const WVector& center)
{
	m_center = center;
}

void CRing::Process(float dt)
{
	if (!m_bActive)
		return;

	CGolfDoc* pGolfDoc = GOLFDOC();
	m_radius = 2.0f;
	m_angle += g_PI * 0.8f * dt;

	for (int i = 0; i < m_num; i++)
	{
		m_pos[i] =
			m_center + m_radius * RotMat(g_PI * 2 * i / m_num + m_angle, 1).xa;
		m_pos2[i] = m_center +
			m_radius * m_scale * RotMat(g_PI * 2 * i / m_num - m_angle, 1).xa;
	}
}

void CRing::Display()
{
	if (!m_bActive)
		return;

	int i, j;
	WTVertex v[4];
	WTVertex* vl[5];

	for (i = 0; i < m_num; i++)
	{
		for (j = 0; j < 4; j++)
		{
			WVector pos = (j & 2) ? m_pos[(i + 1) % m_num] : m_pos[i];
			float side = (j & 1) ? -1.0f : 1.0f;

			v[j].SetPosition(
				WVector(pos.x, pos.y + side * m_height * 0.5f, pos.z));
			v[j].diffuse = 0x32ffffff;
			v[j].tu = (j & 2) ? (i + 1) / (float)m_num : i / (float)m_num;
			v[j].tv = (j & 1) ? 0.0f : 1.0f;
		}

		vl[0] = &v[0];
		vl[1] = &v[1];
		vl[2] = &v[3];
		vl[3] = &v[2];
		vl[4] = NULL;

		g_view->DrawPolygonFan(vl, (m_texHandle & 0x7ff) | 0xc00000, 0x400, 0);
	}

	for (i = 0; i < m_num; i++)
	{
		for (j = 0; j < 4; j++)
		{
			WVector pos = (j & 2) ? m_pos2[(i + 1) % m_num] : m_pos2[i];
			float side = (j & 1) ? -1.0f : 1.0f;

			v[j].SetPosition(WVector(pos.x,
				pos.y + side * m_height * m_scale * 0.5f, pos.z));
			v[j].diffuse = 0x50ffffff;
			v[j].tu = (j & 2) ? (i + 1) / (float)m_num : i / (float)m_num;
			v[j].tv = (j & 1) ? 0.0f : 1.0f;
		}

		vl[0] = &v[0];
		vl[1] = &v[1];
		vl[2] = &v[3];
		vl[3] = &v[2];
		vl[4] = NULL;

		g_view->DrawPolygonFan(vl, (m_texHandle & 0x7ff) | 0xc00000, 0x400, 0);
	}
}
