#include "projectg.h"
#include "snow.h"
#include "w3danispr.h"
#include "wind.h"

extern WMatrix g_camera;

CSnow::CSnow(int num, const char* sprName, int sprIndex, int sprWidth,
	int sprHeight)
	: m_frag(NULL), m_spr(NULL)
{
	m_spr =
		g_resrcmng->Get3DAniSpr(sprName, 0, (float)sprWidth, (float)sprHeight);

	m_bNoAdditiveBlend = false;
	m_sprIndex = sprIndex;

	m_area.min = WVector(-160.0f, -112.0f, -160.0f);
	m_area.max = WVector(160.0f, 48.0f, 160.0f);

	Reset(num, 1.0f);
}

CSnow::~CSnow()
{
	if (m_frag)
	{
		delete[] m_frag;
		m_frag = NULL;
	}
	if (g_resrcmng && m_spr)
	{
		g_resrcmng->Release(m_spr);
		m_spr = NULL;
	}
}

void CSnow::Reset(int num, float size)
{
	m_num = num;

	if (m_frag)
	{
		delete[] m_frag;
		m_frag = NULL;
	}
	m_frag = new sFrag[m_num];
	m_size = size;

	m_spr->SetRect(size, size, size * 0.5f, size * 0.5f);

	for (int i = 0; i < m_num; i++)
	{
		m_frag[i].sprIndex =
			(m_sprIndex < 0) ? rand() % m_spr->GetSpriteNum() : m_sprIndex;
		m_frag[i].swayAngle = Random(0.0f, g_PI * 2.0f);
		m_frag[i].swayPhase = Random(0.0f, g_PI);

		for (int j = 0; j < 3; j++)
			m_frag[i].basePos.p[j] =
				Random(-1.0f, 1.0f) * 160.0f + g_camera.pivot.p[j];

		m_frag[i].pos = m_frag[i].basePos + m_frag[i].sway;
	}
}

void CSnow::Process(float dt)
{
	Waabb box = m_area;
	box.min += g_camera.pivot;
	box.max += g_camera.pivot;

	WVector size = m_area.max - m_area.min;

	for (int i = 0; i < m_num; i++)
	{
		m_frag[i].basePos += Wind().GetWind(m_frag[i].basePos) * dt;
		m_frag[i].basePos.y -= dt * m_gravity * 34.295296f;

		m_frag[i].swayPhase += dt * (g_PI / 2.0f);
		m_frag[i].sway = WVector(cosf(m_frag[i].swayAngle), 0.0f,
							 sinf(m_frag[i].swayAngle)) *
			4.0f * sinf(m_frag[i].swayPhase);

		for (int j = 0; j < 3; j++)
		{
			if (m_frag[i].basePos.p[j] > box.max.p[j])
				m_frag[i].basePos.p[j] -=
					ceilf((m_frag[i].basePos.p[j] - box.max.p[j]) / size.p[j]) *
					size.p[j];
			else if (m_frag[i].basePos.p[j] < box.min.p[j])
				m_frag[i].basePos.p[j] +=
					ceilf((box.min.p[j] - m_frag[i].basePos.p[j]) / size.p[j]) *
					size.p[j];
		}

		m_frag[i].pos = m_frag[i].basePos + m_frag[i].sway;
		m_frag[i].bDraw = (m_frag[i].pos - g_camera.pivot) * g_camera.za > 0.0f;
	}
}

void CSnow::Display()
{
	WSphere sphere;

	int type = (m_bNoAdditiveBlend == true) ? 0x20000000 : 0x20800000;

	for (int i = 0; i < m_num; i++)
	{
		if (m_frag[i].bDraw)
		{
			m_spr->SetPos(m_frag[i].pos);
			m_spr->Render(g_view, 0.0f, type, m_frag[i].sprIndex);
		}
	}
}

void CSnow::DisplayFullScreenOverlay()
{
	if (Doc()->m_golfGame.weather)
		WOverlay::DrawBox(g_view,
			WRect(0, 0, g_view->GetWidth(), g_view->GetHeight()), 0, 0x4c3c3c3c,
			0.001f);
}

void CSnow::TurnOffAdditiveBlend()
{
	m_bNoAdditiveBlend = true;
}
