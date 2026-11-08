#include "projectg.h"
#include "golfdoc.h"
#include "quadtree.h"
#include "beach.h"

void CBeach::ReadAttribute(const char* name)
{
	if (!name)
		return;

	int index;
	if (strchr(name, '_') && sscanf(strchr(name, '_') + 1, "%d", &index))
	{
		m_texHandle = g_resrcmng->LoadTexture(
			GOLFDOC()->m_waveInfo.GetTexture(index), 0, 0, 0);
		m_width = GOLFDOC()->m_waveInfo.GetWidth(index);
		m_frequency = GOLFDOC()->m_waveInfo.GetFreq(index);
	}
	else
	{
		m_texHandle = g_resrcmng->LoadTexture("[water.dds", 0, 0, 0);
		m_width = 76.8f;
		m_frequency = 7.5f;
	}

	m_fadeInTime = m_frequency * (4.0f / 15.0f);
	m_fadeOutTime = m_frequency * (8.0f / 15.0f);
	m_delay = m_frequency * 0.2f;
	m_deceleration =
		m_width / (4.0f * m_fadeInTime - m_fadeInTime * m_fadeInTime * 0.5f);
}

void CBeach::Process(float dt)
{
	for (unsigned char i = 0; i < m_layer.size(); i++)
	{
		m_layer[i].time += dt;

		switch (m_layer[i].state)
		{
		case 0:
			m_layer[i].alpha += dt * (255.0f / m_fadeInTime);
			m_layer[i].speed -= dt * m_deceleration;
			m_layer[i].pos += dt * m_layer[i].speed;
			if (m_layer[i].alpha > 255.0f)
			{
				m_layer[i].alpha = 255.0f;
				m_layer[i].state = 1;
				m_layer[(i + 1) % m_layer.size()].delay = m_delay;
			}
			break;

		case 1:
			m_layer[i].alpha -= dt * (255.0f / m_fadeOutTime);
			m_layer[i].speed -= dt * m_deceleration;
			m_layer[i].pos += dt * m_layer[i].speed;
			if (m_layer[i].alpha < 0.0f)
				m_layer[i].state = 2;
			break;

		case 2:
			if (m_layer[i].delay > 0.0f)
			{
				m_layer[i].delay -= dt;
				if (m_layer[i].delay < 0.0f)
				{
					m_layer[i].state = 0;
					m_layer[i].alpha = 0.0f;
					m_layer[i].time = 0.0f;
					m_layer[i].pos = 0.0f;
					m_layer[i].speed = m_deceleration * 4.0f;
				}
			}
			break;
		}
	}
}

void CBeach::Render()
{
	float maxTime = 0.0f;
	unsigned char start = 0;

	for (unsigned char i = 0; i < m_layer.size(); i++)
	{
		if (maxTime < m_layer[i].time)
		{
			start = i;
			maxTime = m_layer[i].time;
		}
	}

	BYTE g = 255 - (BYTE)(((255 - (g_lightset.ambient2 >> 8)) & 0xff) * 0.8f);
	BYTE r = 255 - (BYTE)(((255 - (g_lightset.ambient2 >> 16)) & 0xff) * 0.8f);
	BYTE b = 255 - (BYTE)(((255 - g_lightset.ambient2) & 0xff) * 0.8f);
	unsigned long color = (r << 16) | (g << 8) | b;

	for (unsigned char j = 0; j < m_layer.size(); j++)
	{
		if (m_layer[(j + start) % m_layer.size()].state == 2)
			continue;

		for (unsigned char k = 0; k < m_wavelet.size() - 1; k++)
		{
			unsigned char n;
			int m;
			for (n = 0, m = 0; n < 4; n++, m++)
			{
				unsigned char w = k + ((n & 2) ? 1 : 0);
				bool bEdge = (w == 0 || w == m_wavelet.size() - 1);
				bool bFar = (n & 1) != 0;
				float scale = bFar ? 0.1f : (bEdge ? 0.5f : 1.0f);

				WVector pos = m_wavelet[w].pos[m & 1] -
					scale * m_layer[(j + start) % m_layer.size()].pos *
						m_wavelet[w].dir;
				WVector ground;
				ground = GetPVS().GetGroundPoint(pos, true, NULL, true);
				ground.y += bFar ? 1.5f : 1.0f;

				vtr[n].SetPosition(ground);
				vtr[n].tu = m_wavelet[w].u[m & 1];
				if (bEdge)
					vtr[n].diffuse = color | 0xff000000;
				else
					vtr[n].diffuse = color |
						((int)m_layer[(j + start) % m_layer.size()].alpha
							<< 24);
			}

			g_view->DrawPolygonFan(vtx, (m_texHandle & 0x7ff) | 0x28400000,
				0x400, false);
		}
	}
}
