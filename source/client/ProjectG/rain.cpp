#include "projectg.h"
#include "rain.h"
#include "wind.h"

extern WMatrix g_camera;

#define MAX_RAIN 500

CRain::CRain()
{
	m_frag = new sFrag[MAX_RAIN];
	m_area = WVector(256.0f, 128.0f, 256.0f);
	m_radius = 128.0f;

	for (int i = 0; i < MAX_RAIN; i++)
	{
		m_frag[i].bDraw = true;

		for (int j = 0; j < 3; j++)
			m_frag[i].pos.p[j] =
				((float)(rand() % 1023) / 1024.0f - 0.5f) * m_area.p[j] +
				g_camera.pivot.p[j];
	}

	UpdateWind();
}

void CRain::UpdateWind()
{
	m_dir = Wind().GetGlobalWind() - WVector(0.0f, 20.0f, 0.0f);
	m_dir.Normalize();
	m_dir *= 10.0f;
}

CRain::~CRain()
{
	if (m_frag)
	{
		delete[] m_frag;
		m_frag = NULL;
	}
}

void CRain::Process(float dt)
{
	Waabb box;
	box.min = g_camera.pivot - m_area * 0.5f;
	box.max = g_camera.pivot + m_area * 0.5f;

	for (int i = 0; i < MAX_RAIN; i++)
	{
		m_frag[i].pos += m_dir * 10.0f * dt;
		m_frag[i].bDraw =
			(WVectorLen(m_frag[i].pos - g_camera.pivot) < m_radius ||
				(m_frag[i].pos + m_dir - g_camera.pivot).Magnitude() < m_radius)
			? true
			: false;

		for (int j = 0; j < 3; j++)
		{
			if (m_frag[i].pos.p[j] > box.max.p[j])
				m_frag[i].pos.p[j] -=
					(floorf((m_frag[i].pos.p[j] - box.max.p[j]) / m_area.p[j]) +
						1.0f) *
					m_area.p[j];
			else if (m_frag[i].pos.p[j] < box.min.p[j])
				m_frag[i].pos.p[j] +=
					(floorf((box.min.p[j] - m_frag[i].pos.p[j]) / m_area.p[j]) +
						1.0f) *
					m_area.p[j];
		}
	}
}

void CRain::Display()
{
	WSphere sphere;
	sphere.radius = m_dir.Magnitude() * 0.5f;

	for (int i = 0; i < MAX_RAIN; i++)

		if (m_frag[i].bDraw)
		{
			sphere.pos = m_frag[i].pos + 0.5f * m_dir;

			if (g_view->InFrustum(sphere))
				g_view->DrawLine(m_frag[i].pos, 0xa0e0e0e0,
					m_frag[i].pos + m_dir, 0xa0e0e0e0, 0);
		}
}

void CRain::DisplayFullScreenOverlay()
{
	WOverlay::DrawBox(g_view,
		WRect(0, 0, g_view->GetWidth(), g_view->GetHeight()), 0, 0x60303030,
		0.001f);
}
