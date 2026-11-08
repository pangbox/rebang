#include "stdafx.h"
#include "golfdoc.h"
#include "quadtree.h"
#include "scenemanager.h"
#include "slicestr.h"
#include "river.h"

WTVertex CRiver::vtr[4];
WTVertex* CRiver::vtx[5] = { &vtr[0], &vtr[1], &vtr[3], &vtr[2], NULL };
WTVertex ntRiver_Lava::vtr[4];
WTVertex* ntRiver_Lava::vtx[5] = { &vtr[0], &vtr[1], &vtr[3], &vtr[2], NULL };
WTVertex CRiver_ex::vtr[4];
WTVertex* CRiver_ex::vtx[5] = { &vtr[0], &vtr[1], &vtr[3], &vtr[2], NULL };

#define RIVERINFO() (GOLFDOC()->m_riverInfo)
#define PVS() (GetPVS())

CRiver::CRiver()
{
	for (int i = 0; i < 2; i++)
	{
		m_side[i].scroll = 0.0f;
		m_side[i].scrollVel = 0.0f;
		m_side[i].texHandle = 0;
		m_side[i].tileHead = 0.0f;
		m_side[i].tileSide = 0.0f;
		m_side[i].depth = 0.0f;
		m_side[i].bActive = false;
	}
}

CRiver::~CRiver()
{
	for (int i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_side[i].texHandle)
		{
			g_resrcmng->Release(m_side[i].texHandle);
			m_side[i].texHandle = 0;
		}
	}
}

void CRiver::ReadAttribute(const char* name)
{
	int index[2] = { -1, -1 };

	if (strstr(name, "_"))
	{
		m_side[0].bActive = sscanf(strchr(name, '_') + 1, "%d", &index[0]) != 0;

		if (sscanf(strrchr(name, '_') + 1, "%d", &index[1]) &&
			index[1] != index[0])
		{
			m_side[1].bActive = true;
		}
		else
		{
			m_side[1].bActive = false;
		}

		for (int i = 0; i < 2; i++)
		{
			if (m_side[i].bActive)
			{
				m_side[i].scroll = Random(0.0f, 1.0f);
				m_side[i].scrollVel = RIVERINFO().GetScrollVelocity(index[i]);
				m_side[i].tileHead = RIVERINFO().GetHeadTile(index[i]);
				m_side[i].tileSide = RIVERINFO().GetSideTile(index[i]);
				m_side[i].depth = RIVERINFO().GetDepth(index[i]) * 3.2f;
				m_side[i].texHandle = g_resrcmng->LoadTexture(
					MakeStr("[%s_river_%s.jpg", GetCurMapName(),
						i == 0 ? "up" : "down"),
					0, 0, 0);
			}
		}
	}
	else if (strstr(name, "*d"))
	{
		float depth = 0.0f;
		int count = sscanf(strchr(name, 'd') + 1, "%f", &depth);
		if (count + 1 == 0)
		{
			depth = 1.0f;
		}

		for (int i = 0; i < 2; i++)
		{
			m_side[i].scroll = Random(0.0f, 1.0f);
			m_side[i].scrollVel = Random(1.0f, 3.0f);
			m_side[i].tileHead = 1.0f;
			m_side[i].tileSide = 1.0f;
			m_side[i].depth = depth;
			m_side[i].texHandle = g_resrcmng->LoadTexture(
				MakeStr("[%s_river_%s.jpg", GetCurMapName(),
					i == 0 ? "up" : "down"),
				0, 0, 0);
			m_side[i].bActive = true;
		}
	}
	else
	{
		for (int i = 0; i < 2; i++)
		{
			m_side[i].scroll = Random(0.0f, 1.0f);
			m_side[i].scrollVel = Random(1.0f, 3.0f);
			m_side[i].tileHead = 1.0f;
			m_side[i].tileSide = 1.0f;

			char* file;
			if (GetCurMap() == 11)
			{
				m_side[i].depth = 10.56f;
				file = MakeStr("[pink_river_%s.jpg", i == 0 ? "up" : "down");
			}
			else if (GetCurMap() == 15)
			{
				m_side[i].depth = 3.2f;
				file = MakeStr("[gs_river_%s.jpg", i == 0 ? "up" : "down");
			}
			else
			{
				m_side[i].depth = 3.2f;
				file = MakeStr("[wind_river_%s.jpg", i == 0 ? "up" : "down");
			}
			m_side[i].texHandle = g_resrcmng->LoadTexture(file, 0, 0, 0);
			m_side[i].bActive = true;
		}
	}
}

void CRiver::Setup(const std::vector<WVector>& points, const char* name)
{
	ReadAttribute(name);

	Waabb box;
	WVector dir;
	dir = points[1] - points[0];
	dir.y = 0.0f;
	dir = WCrossProduct(dir, WVector::UNIT_POS_Y).Normalize();

	m_step = 0.0f;
	float height = PVS().GetLowestGround(points[1]) +
		Max(m_side[0].depth, m_side[1].depth);
	do
	{
		m_step += 3.2f;
	} while (!(PVS().GetLowestGround(points[1] + m_step * dir) > height));
	m_step += 3.2f;

	m_river.reserve(50);

	unsigned int i, count;
	for (i = 0, count = 0; i < points.size() - 1; i++)
	{
		dir = points[i + 1] - points[i];
		dir.y = 0.0f;
		dir = WCrossProduct(dir, WVector::UNIT_POS_Y).Normalize();
		dir *= m_step;

		float steps = Max(1.0f,
			(float)ceil((double)(Abs(PVS().GetLowestGround(points[i + 1]) -
									 PVS().GetLowestGround(points[i])) *
				0.625f)));

		height = 0.0f;
		for (float t = 0.0f; t < 1.0f; t += 1.0f / steps)
		{
			float prev = height;
			WVector p;
			p = points[i] * (1.0f - t) + points[i + 1] * t;
			height = PVS().GetLowestGround(p);
			if (height > prev + 1.6f)
				continue;
			sRiver r;
			for (unsigned int j = 0; j < 2; j++)
			{
				float sign = (j & 1) ? 1.0f : -1.0f;
				r.pos[j] = p + sign * dir;
				r.pos[j].y = height + m_side[0].depth;
				if (count == 0)
				{
					r.u[j] = 0.0f;
				}
				else
				{
					r.u[j] =
						(r.pos[j] - m_river[count - 1].pos[j]).Magnitude() /
							m_step * m_side[j].tileHead +
						m_river[count - 1].u[j];
				}
			}

			if (count > 0)
			{
				box.max.x = Max(Max(r.pos[0].x, r.pos[1].x),
					Max(m_river[count - 1].pos[0].x,
						m_river[count - 1].pos[1].x));
				box.max.y = Min(r.pos[0].y, m_river[count - 1].pos[0].y);
				box.max.z = Max(Max(r.pos[0].z, r.pos[1].z),
					Max(m_river[count - 1].pos[0].z,
						m_river[count - 1].pos[1].z));
				box.min.x = Min(Min(r.pos[0].x, r.pos[1].x),
					Min(m_river[count - 1].pos[0].x,
						m_river[count - 1].pos[1].x));
				box.min.y = box.max.y - Max(m_side[0].depth, m_side[1].depth);
				box.min.z = Min(Min(r.pos[0].z, r.pos[1].z),
					Min(m_river[count - 1].pos[0].z,
						m_river[count - 1].pos[1].z));
				GetPVS().AddWater(box, 2);
			}

			m_river.push_back(r);
			count++;
		}
	}
}

void CRiver::Process(float dt)
{
	if (m_side[0].bActive)
		m_side[0].scroll -= dt * m_side[0].scrollVel;
	if (m_side[1].bActive)
		m_side[1].scroll -= dt * m_side[1].scrollVel;
}

void CRiver::Render()
{
	for (unsigned char i = 0; i < 2; i++)
	{
		if (m_side[i].bActive)
		{
			for (unsigned char j = 0; j < 4; j++)
			{
				vtr[j].tu = (j & 1) ? 0.0f : m_side[i].tileSide;
			}
			for (unsigned char k = 0; k < m_river.size() - 1; k++)
			{
				for (unsigned char j = 0; j < 4; j++)
				{
					unsigned char n = k + ((j >> 1) & 1);

					vtr[j].SetPosition(m_river[n].pos[j & 1]);

					vtr[j].tv = m_river[n].u[i] + m_side[i].scroll;

					if (n == 0 || n == m_river.size() - 1)
					{
						vtr[j].diffuse = 0xffffff;
					}
					else
					{
						vtr[j].diffuse = 0xffffffff;
					}
				}
				g_view->DrawPolygonFan(vtx,
					(m_side[i].texHandle & 0x7ff) | 0x28c00000, 0x400, false);
			}
		}
	}
}

ntRiver_Lava::ntRiver_Lava()
{
	for (int i = 0; i < 2; i++)
	{
		m_side[i].scroll = 0.0f;
		m_side[i].scrollVel = 0.0f;
		m_side[i].texHandle = 0;
		m_side[i].tileHead = 0.0f;
		m_side[i].tileSide = 0.0f;
		m_side[i].depth = 0.0f;
		m_side[i].bActive = false;
	}
}

ntRiver_Lava::~ntRiver_Lava()
{
	for (int i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_side[i].texHandle)
		{
			g_resrcmng->Release(m_side[i].texHandle);
			m_side[i].texHandle = 0;
		}
	}
}

void ntRiver_Lava::Setup(const std::vector<WVector>& points, const char* name)
{
	ReadAttribute(name);

	Waabb aabb;
	WVector dir;
	float height;
	float widths[256] = { 0.0f };
	unsigned int i, n;
	for (i = 0; i < points.size() - 1; i++)
	{
		dir = points[i + 1] - points[i];
		dir.y = 0;
		dir = WCrossProduct(dir, WVector::UNIT_POS_Y).Normalize();

		height = GetPVS().GetLowestGround(points[i]) +
			Max(m_side[0].depth, m_side[1].depth);

		for (;;)
		{
			if (GetPVS().GetLowestGround(
					dir * (widths[i] += 6.4f) + points[i]) > height)
			{
				widths[i] += 3.2f;
				break;
			}

			if (widths[i] > 160.0f)
				break;
		}
	}

	m_river.reserve(50);

	for (i = 0, n = 0; i < points.size() - 1; i++)
	{
		dir = points[i + 1] - points[i];
		dir.y = 0;
		dir = WCrossProduct(dir, WVector::UNIT_POS_Y).Normalize();
		dir *= widths[i];

		sRiver river;
		WVector pos = points[i];
		float ground = GetPVS().GetLowestGround(points[i]);

		for (unsigned int j = 0; j < 2; j++)
		{
			river.pos[j] = dir * ((j & 1) ? 1.0f : -1.0f) + pos;
			river.pos[j].y = ground + m_side[0].depth;
			if (n == 0)
			{
				river.u[j] = 0.0f;
			}
			else
			{
				WVector d = river.pos[j] - m_river[n - 1].pos[j];
				river.u[j] = d.Magnitude() / widths[i] * m_side[j].tileHead +
					m_river[n - 1].u[j];
			}
		}

		if (n > 0)
		{
			aabb.max.x = Max(Max(river.pos[0].x, river.pos[1].x),
				Max(m_river[n - 1].pos[0].x, m_river[n - 1].pos[1].x));
			aabb.max.y = Min(river.pos[0].y, m_river[n - 1].pos[0].y);
			aabb.max.z = Max(Max(river.pos[0].z, river.pos[1].z),
				Max(m_river[n - 1].pos[0].z, m_river[n - 1].pos[1].z));

			aabb.min.x = Min(Min(river.pos[0].x, river.pos[1].x),
				Min(m_river[n - 1].pos[0].x, m_river[n - 1].pos[1].x));
			aabb.min.y = aabb.max.y - Max(m_side[0].depth, m_side[1].depth);
			aabb.min.z = Min(Min(river.pos[0].z, river.pos[1].z),
				Min(m_river[n - 1].pos[0].z, m_river[n - 1].pos[1].z));

			GetPVS().AddWater(aabb, 2);
		}

		m_river.push_back(river);

		n++;
	}
}

void ntRiver_Lava::Process(float dt)
{
	if (m_side[0].bActive)
		m_side[0].scroll -= dt * m_side[0].scrollVel;
	if (m_side[1].bActive)
		m_side[1].scroll -= dt * m_side[1].scrollVel;
}

void ntRiver_Lava::Render()
{
	for (unsigned char i = 0; i < 2; i++)
	{
		if (m_side[i].bActive)
		{
			for (unsigned char j = 0; j < 4; j++)
			{
				vtr[j].tu = (j & 1) ? 0.0f : 1.0f;
			}
			for (unsigned char k = 0; k < m_river.size() - 1; k++)
			{
				for (unsigned char j = 0; j < 4; j++)
				{
					unsigned char n = k + ((j >> 1) & 1);

					vtr[j].SetPosition(m_river[n].pos[j & 1]);

					vtr[j].tv = m_river[n].u[i] + m_side[i].scroll;

					if (n == 0 || n == m_river.size() - 1)
					{
						vtr[j].diffuse = 0xffffff;
					}
					else
					{
						vtr[j].diffuse = 0xffffffff;
					}
				}
				g_view->DrawPolygonFan(vtx,
					(m_side[i].texHandle & 0x7ff) | 0x28c00000, 0x400, false);
			}
		}
	}
}

void ntRiver_Lava::ReadAttribute(const char* name)
{
	int index[2] = { 0, 1 };

	m_side[0].bActive = true;
	m_side[1].bActive = true;

	for (int i = 0; i < 2; i++)
	{
		m_side[i].scroll = Random(0.0f, 1.0f);
		m_side[i].scrollVel = RIVERINFO().GetScrollVelocity(index[i]);
		m_side[i].tileHead = RIVERINFO().GetHeadTile(index[i]);
		m_side[i].tileSide = RIVERINFO().GetSideTile(index[i]);
		m_side[i].depth = RIVERINFO().GetDepth(index[i]) * 3.2f;
		m_side[i].texHandle = g_resrcmng->LoadTexture(
			MakeStr("[hell_river_%s.jpg", i == 0 ? "up" : "down"), 0, 0, 0);
	}

	if (strstr(name, "*d"))
	{
		float depth = 0.0f;
		if (sscanf(strchr(name, 'd') + 1, "%f", &depth) + 1 == 0)
			depth = 1.0f;

		m_side[0].depth = depth;
		m_side[1].depth = depth;
	}
}

CRiver_ex::CRiver_ex()
{
	for (int i = 0; i < 2; i++)
	{
		m_side[i].scroll = 0.0f;
		m_side[i].scrollVel = 0.0f;
		m_side[i].texHandle = 0;
		m_side[i].tileHead = 0.0f;
		m_side[i].tileSide = 0.0f;
		m_side[i].depth = 0.0f;
		m_side[i].bActive = false;
	}
}

CRiver_ex::~CRiver_ex()
{
	for (int i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_side[i].texHandle)
		{
			g_resrcmng->Release(m_side[i].texHandle);
			m_side[i].texHandle = 0;
		}
	}
}

void CRiver_ex::ReadAttribute(const char* name)
{
	int index[2] = { -1, -1 };

	if (strstr(name, "_"))
	{
		m_side[0].bActive = sscanf(strchr(name, '_') + 1, "%d", &index[0]) != 0;

		if (sscanf(strrchr(name, '_') + 1, "%d", &index[1]) &&
			index[1] != index[0])
		{
			m_side[1].bActive = true;
		}
		else
		{
			m_side[1].bActive = false;
		}

		for (int i = 0; i < 2; i++)
		{
			if (m_side[i].bActive)
			{
				m_side[i].scroll = Random(0.0f, 1.0f);
				m_side[i].scrollVel = RIVERINFO().GetScrollVelocity(index[i]);
				m_side[i].tileHead = RIVERINFO().GetHeadTile(index[i]);
				m_side[i].tileSide = RIVERINFO().GetSideTile(index[i]);
				m_side[i].depth = RIVERINFO().GetDepth(index[i]) * 3.2f;
				m_side[i].texHandle = g_resrcmng->LoadTexture(
					MakeStr("[%s_river_%s.jpg", GetCurMapName(),
						i == 0 ? "up" : "down"),
					0, 0, 0);
			}
		}
	}
	else if (strstr(name, "*d"))
	{
		float depth = 0.0f;
		int count = sscanf(strchr(name, 'd') + 1, "%f", &depth);
		if (count + 1 == 0)
		{
			depth = 1.0f;
		}

		float speed = 3.0f;
		if (strstr(name, "*v"))
		{
			WSliceStr slice(name);
			if (!slice.Get("*v", "%f", &speed))
			{
				speed = 1.0f;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			m_side[i].scroll = Random(0.0f, 1.0f);
			m_side[i].scrollVel = speed;
			m_side[i].tileHead = 1.0f;
			m_side[i].tileSide = 1.0f;
			m_side[i].depth = depth;
			m_side[i].texHandle = g_resrcmng->LoadTexture(
				MakeStr("[%s_river_%s.jpg", GetCurMapName(),
					i == 0 ? "up" : "down"),
				0, 0, 0);
			m_side[i].bActive = true;
		}
	}
	else
	{
		float speed = 3.0f;
		if (strstr(name, "*v"))
		{
			WSliceStr slice(name);
			if (!slice.Get("*v", "%f", &speed))
			{
				speed = 1.0f;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			m_side[i].scroll = Random(0.0f, 1.0f);
			m_side[i].scrollVel = speed;
			m_side[i].tileHead = 1.0f;
			m_side[i].tileSide = 1.0f;

			char* file;
			if (GetCurMap() == 11)
			{
				m_side[i].depth = 10.56f;
				file = MakeStr("[pink_river_%s.jpg", i == 0 ? "up" : "down");
			}
			else if (GetCurMap() == 15)
			{
				m_side[i].depth = 3.2f;
				file = MakeStr("[gs_river_%s.jpg", i == 0 ? "up" : "down");
			}
			else if (GetCurMap() == 16)
			{
				m_side[i].depth = 3.2f;
				file = MakeStr("[panda_river_%s.jpg", i == 0 ? "up" : "down");
			}
			else
			{
				m_side[i].depth = 3.2f;
				file = MakeStr("[wind_river_%s.jpg", i == 0 ? "up" : "down");
			}
			m_side[i].texHandle = g_resrcmng->LoadTexture(file, 0, 0, 0);
			m_side[i].bActive = true;
		}
	}
}

void CRiver_ex::Setup(const std::vector<WVector>& points, const char* name)
{
	if (points.size() < 3)
		return;

	ReadAttribute(name);
	Waabb box;
	m_river.reserve(50);

	unsigned int i, count;
	for (i = 0, count = 0; i < points.size() - 1; i += 2)
	{
		sRiver r;
		memset(&r, 0, sizeof(r));
		for (unsigned int j = 0; j < 2; j++)
		{
			r.pos[j] = points[i + j];
			r.u[j] = 0.0f;
		}

		if (count > 0)
		{
			float len = (r.pos[1] - r.pos[0]).Magnitude();
			if (len != 0.0f)
			{
				WVector a = r.pos[0] - m_river[count - 1].pos[0];
				WVector b = r.pos[1] - m_river[count - 1].pos[1];
				float inv = 1.0f / len;
				r.u[0] = m_side[0].tileHead * (a.Magnitude() * inv) +
					m_river[count - 1].u[0];
				r.u[1] = m_side[1].tileHead * (b.Magnitude() * inv) +
					m_river[count - 1].u[1];
			}

			box.max.x = Max(Max(r.pos[0].x, r.pos[1].x),
				Max(m_river[count - 1].pos[0].x, m_river[count - 1].pos[1].x));
			box.max.y = Max(Max(r.pos[0].y, r.pos[1].y),
				Max(m_river[count - 1].pos[0].y, m_river[count - 1].pos[1].y));
			box.max.z = Max(Max(r.pos[0].z, r.pos[1].z),
				Max(m_river[count - 1].pos[0].z, m_river[count - 1].pos[1].z));
			box.min.x = Min(Min(r.pos[0].x, r.pos[1].x),
				Min(m_river[count - 1].pos[0].x, m_river[count - 1].pos[1].x));
			box.min.y = Min(Min(r.pos[0].y, r.pos[1].y),
				Min(m_river[count - 1].pos[0].y, m_river[count - 1].pos[1].y));
			box.min.z = Min(Min(r.pos[0].z, r.pos[1].z),
				Min(m_river[count - 1].pos[0].z, m_river[count - 1].pos[1].z));

			if (abs(box.max.y - box.min.y) > 8.0f)
			{
				WVector cur = (r.pos[1] - r.pos[0]) * 0.25f;
				WVector prv =
					(m_river[count - 1].pos[1] - m_river[count - 1].pos[0]) *
					0.25f;
				WVector a = r.pos[0];
				WVector b = m_river[count - 1].pos[0];
				for (int s = 4; s != 0; s--)
				{
					WVector a2 = a;
					a = a2 + cur;
					WVector b2 = b;
					b = b2 + prv;
					Waabb sub;
					sub.max.x = Max(Max(a2.x, a.x), Max(b2.x, b.x));
					sub.max.y = Max(Max(a2.y, a.y), Max(b2.y, b.y));
					sub.max.z = Max(Max(a2.z, a.z), Max(b2.z, b.z));
					sub.min.x = Min(Min(a2.x, a.x), Min(b2.x, b.x));
					sub.min.y = Min(Min(a2.y, a.y), Min(b2.y, b.y));
					sub.min.z = Min(Min(a2.z, a.z), Min(b2.z, b.z));
					GetPVS().AddWater(sub, 2);
				}
			}
			else
			{
				GetPVS().AddWater(box, 2);
			}
		}

		m_river.push_back(r);
		count++;
	}
}

void CRiver_ex::Process(float dt)
{
	if (m_side[0].bActive)
		m_side[0].scroll -= dt * m_side[0].scrollVel;
	if (m_side[1].bActive)
		m_side[1].scroll -= dt * m_side[1].scrollVel;
}

void CRiver_ex::Render()
{
	unsigned int count = m_river.size();
	if (count < 3)
		return;

	for (unsigned char i = 0; i < 2; i++)
	{
		if (m_side[i].bActive)
		{
			for (unsigned char j = 0; j < 4; j++)
			{
				vtr[j].tu = (j & 2) ? 1.0f : 0.0f;
			}
			for (unsigned char k = 0; k < count - 1; k++)
			{
				for (unsigned int j = 0; j < 4; j++)
				{
					unsigned char n = k + (j & 1);

					vtr[j].SetPosition(m_river[n].pos[(j >> 1) & 1]);
					vtr[j].tv = m_river[n].u[i] + m_side[i].scroll;

					if ((k < 1 && !(j & 1)) || (k > count - 3 && (j & 1)))
					{
						vtr[j].diffuse = 0xffffff;
					}
					else
					{
						vtr[j].diffuse = 0xffffffff;
					}
				}
				g_view->DrawPolygonFan(vtx,
					(m_side[i].texHandle & 0x7ff) | 0x28c00000, 0x400, false);
			}
		}
	}
}
