#include "projectg.h"
#include "taileffect.h"

int index = 0;

CTailEffect::CTailEffect(const std::string& texture)
{
	m_texHandle = 0;
	m_head = 0;
	m_tail = 0;
	m_bReserved = false;
	m_reserved = 0;

	Init(texture);
}

CTailEffect::~CTailEffect()
{
	for (std::map<std::string, int>::iterator it = m_texMap.begin();
		it != m_texMap.end(); ++it)
	{
		if ((*it).second)
			g_resrcmng->Release((*it).second);
	}

	m_texMap.clear();
}

void CTailEffect::Init(const std::string& texture)
{
	m_maxEdge = 30;
	m_life = 0.3f;

	m_edge.reserve(30);
	m_spot.reserve(m_maxEdge);

	ChangeTexture(texture.c_str());
}

void CTailEffect::Reset()
{
	for (int i = 0; i < m_maxEdge; i++)
		m_edge[i].bActive = false;

	m_edge.clear();
	m_edge.reserve(m_maxEdge);

	m_tail = 0;
	m_head = 0;
	index = 0;
}

void CTailEffect::PutSpot(const WVector& nearPos, const WVector& farPos)
{
	static int s_skipCount = 0;

	if (WisEqual(m_lastNear, nearPos, g_EPSILON) &&
		WisEqual(m_lastFar, farPos, g_EPSILON))
	{
		s_skipCount++;
		return;
	}

	if (index >= m_maxEdge)
		return;

	TAILEDGE spot;
	spot.bActive = true;
	spot.life = m_life;
	spot.nearPos = nearPos;
	spot.farPos = farPos;
	m_spot[index++] = spot;

	m_lastNear = nearPos;
	m_lastFar = farPos;

	int num = index;
	m_head = m_tail = 0;

	if (num <= 3)
		return;

	int count = num - 3;
	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			float t = j * 0.2f;
			float t2 = t * t;
			float t3 = t2 * t;
			float b0 = -t3 + 3.0f * t2 - 3.0f * t + 1.0f;
			float b1 = 3.0f * t3 - 6.0f * t2 + 0.0f * t + 4.0f;
			float b2 = -3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f;
			float b3 = t3 + 0.0f * t2 + 0.0f * t;

			const TAILEDGE& p0 = m_spot[i];
			const TAILEDGE& p1 = m_spot[i + 1];
			const TAILEDGE& p2 = m_spot[i + 2];
			const TAILEDGE& p3 = m_spot[i + 3];
			WVector nearPos;
			nearPos.x = (b0 * p0.nearPos.x + b1 * p1.nearPos.x +
							b2 * p2.nearPos.x + b3 * p3.nearPos.x) /
				6.0f;
			nearPos.y = (b0 * p0.nearPos.y + b1 * p1.nearPos.y +
							b2 * p2.nearPos.y + b3 * p3.nearPos.y) /
				6.0f;
			nearPos.z = (b0 * p0.nearPos.z + b1 * p1.nearPos.z +
							b2 * p2.nearPos.z + b3 * p3.nearPos.z) /
				6.0f;

			WVector farPos;
			farPos.x = (b0 * p0.farPos.x + b1 * p1.farPos.x + b2 * p2.farPos.x +
						   b3 * p3.farPos.x) /
				6.0f;
			farPos.y = (b0 * p0.farPos.y + b1 * p1.farPos.y + b2 * p2.farPos.y +
						   b3 * p3.farPos.y) /
				6.0f;
			farPos.z = (b0 * p0.farPos.z + b1 * p1.farPos.z + b2 * p2.farPos.z +
						   b3 * p3.farPos.z) /
				6.0f;

			TAILEDGE edge;
			edge.bActive = true;
			edge.life = m_life;
			edge.nearPos = nearPos;
			edge.farPos = farPos;
			m_edge[m_head] = edge;

			m_head = (m_head + 1) % m_maxEdge;
			if (m_head == m_tail)
				m_tail = (m_tail + 1) % m_maxEdge;
		}
	}
}

void CTailEffect::Process(float elapsed)
{
	int length = GetLength();
	if (!length)
		return;

	int tail = m_tail;
	for (int i = 0; i < length; i++)
	{
		int idx = (tail + i) % m_maxEdge;
		if (m_edge[idx].bActive)
		{
			m_edge[idx].life -= elapsed;
			if (m_edge[idx].life <= 0.0f)
				m_tail = (m_tail + 1) % m_maxEdge;
		}
	}

	if (!GetLength())
		Reset();
}

void CTailEffect::Display()
{
	int length = GetLength();
	if (!length)
		return;

	WTVertex vtx[4];
	WTVertex* pVtx[5] = { &vtx[0], &vtx[1], &vtx[3], &vtx[2], NULL };

	for (int i = 0; i < length - 1; i++)
	{
		float alpha = i / (float)length;
		int idx = (m_tail + i) % m_maxEdge;

		for (int j = 0; j < 4; j++)
		{
			int k = (j & 2) ? (idx + 1) % m_maxEdge : idx;

			WVector pos = (j & 1) ? m_edge[k].farPos : m_edge[k].nearPos;
			vtx[j].SetPosition(pos);
			vtx[j].tu = (j & 2) ? (i + 1) / (float)length : alpha;
			vtx[j].tv = (j & 1) ? 0.0f : 1.0f;
			vtx[j].diffuse = ((int)(255.0f * alpha) << 24) | 0xffffff;
		}

		g_view->DrawPolygonFan(pVtx, (m_texHandle & 0x7ff) | 0x20c00000, 0x400,
			false);
	}
}

void CTailEffect::ChangeTexture(const std::string& texture)
{
	std::map<std::string, int>::iterator it;
	it = m_texMap.find(texture);

	if (it != m_texMap.end())
	{
		m_texHandle = (*it).second;
	}
	else
	{
		m_texHandle = g_resrcmng->LoadTexture(texture.c_str(), 0, 0, 0);
		m_texMap[texture] = m_texHandle;
	}
}
