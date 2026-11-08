#include "stdafx.h"
#include "sky.h"
#include "lensflare.h"
#include "wind.h"
#include "fx.h"
#include "fxbox.inl"
#include "mathconsts.h"

CSky::CSky()
{
	m_pLensFlare = NULL;
	m_pStar = NULL;
	m_pMoon = NULL;
	m_cloudTex = 0;
	m_cloudNum = 0;
	m_activeCloudNum = 0;
	m_cloudInterval = 0.0f;
	m_cloudTime = 0.0f;
	m_height = 0.0f;
	m_turn = 0.0f;
	m_cloudRange = 1120.0f;
	m_cloudFadeRange = 1600.0f;
	m_cloudHeight = 416.0f;
	m_color = 0xffffffff;
	m_farDiv = 2;
	m_nearDiv = 2;
	memset(m_farTex, 0, sizeof(m_farTex));
	memset(m_nearTex, 0, sizeof(m_nearTex));
	memset(m_upTex, 0, sizeof(m_upTex));
	memset(m_dnTex, 0, sizeof(m_dnTex));
	m_bLoaded = 0;
	m_upAngle = 0.0f;
	m_nearAngle = 0.0f;
}

CSky::~CSky()
{
	Clear();
}

void CSky::Clear()
{
	if (!m_bLoaded)
		return;
	for (int i = 0; i < 4; ++i)
	{
		if (m_batch[i].xiFlag0 & 0x7ff)
		{
			g_resrcmng->VideoReference()->xReleaseVertexBuffer(
				(unsigned int)m_batch[i].xiFlag1 >> 22);
			g_resrcmng->VideoReference()->xReleaseIndexBuffer(
				(m_batch[i].xiFlag1 >> 14) & 0xff);
			m_batch[i].xiFlag1 = 0;
			m_batch[i].xiFlag0 = 0;
		}
	}
	for (unsigned int i = 0; i < sizeof(m_farTex) / sizeof(m_farTex[0]); ++i)
	{
		if (g_resrcmng && m_farTex[i])
		{
			g_resrcmng->Release(m_farTex[i]);
			m_farTex[i] = 0;
		}
	}
	for (int i = 0; i < 5; ++i)
	{
		if (g_resrcmng && m_nearTex[i])
		{
			g_resrcmng->Release(m_nearTex[i]);
			m_nearTex[i] = 0;
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		if (g_resrcmng && m_upTex[i])
		{
			g_resrcmng->Release(m_upTex[i]);
			m_upTex[i] = 0;
		}
	}
	for (int i = 0; i < 2; ++i)
	{
		if (g_resrcmng && m_dnTex[i])
		{
			g_resrcmng->Release(m_dnTex[i]);
			m_dnTex[i] = 0;
		}
	}
	ClearClouds();
	if (m_pLensFlare)
	{
		delete m_pLensFlare;
		m_pLensFlare = NULL;
	}
	if (m_pStar)
	{
		delete m_pStar;
		m_pStar = NULL;
	}
	if (m_pMoon)
	{
		delete m_pMoon;
		m_pMoon = NULL;
	}
	m_bLoaded = 0;
}

void CSky::LoadTexByWeather(const char* name, const char* part)
{
	char filename[128] = { 0 };
	sprintf(filename, "%s_%s.jpg", name, part);
	int tex = g_resrcmng->LoadTexture(filename, 0x80000000, 0, NULL);
	if (!tex)
	{
		char fallback[256] = { 0 };
		sprintf(fallback, "[%s", filename);
		tex = g_resrcmng->LoadTexture(fallback, 0x80000000, 0, NULL);
	}
	if (tex)
	{
		if (!strcmp("far", part))
			m_farTex[0] = tex;
		else if (!strcmp("near", part))
			m_nearTex[0] = tex;
		else if (!strcmp("up", part))
			m_upTex[0] = tex;
		else if (!strcmp("dn", part))
			m_dnTex[0] = tex;
	}
	for (int i = tex ? 1 : 0; i < 5; ++i)
	{
		sprintf(filename, "%s_%s_%d.jpg", name, part, i);
		tex = g_resrcmng->LoadTexture(filename, 0x80000000, 0, NULL);
		if (!strcmp("far", part))
			m_farTex[i] = tex;
		else if (!strcmp("near", part))
			m_nearTex[i] = tex;
		else if (!strcmp("up", part))
		{
			m_upTex[i] = tex;
			break;
		}
		else if (!strcmp("dn", part))
		{
			m_dnTex[i] = tex;
			break;
		}
	}
}

int CSky::IsThereTexHandle(const char* part, int weather)
{
	int* tex;
	if (!strcmp(part, "far"))
		tex = &m_farTex[weather];
	else if (!strcmp(part, "near"))
		tex = &m_nearTex[weather];
	else if (!strcmp(part, "up"))
		tex = weather == 0 ? &m_upTex[0] : &m_upTex[1];
	else if (!strcmp(part, "dn"))
		tex = weather == 0 ? &m_dnTex[0] : &m_dnTex[1];
	else
		return -1;
	return *tex;
}

void CSky::LoadSky(const char* name, float turn)
{
	if (m_bLoaded)
		Clear();
	m_bLoaded = 1;
	int divisions[4] = { 2, 2, 1, 1 };
	LoadTexByWeather(name, "far");
	int tex = IsThereTexHandle("far", 0);
	if (tex)
		divisions[0] = 1;
	SetSkyTex(tex, 0, 0);
	LoadTexByWeather(name, "near");
	tex = IsThereTexHandle("near", 0);
	if (tex)
		divisions[1] = 1;
	SetSkyTex(tex, 0, 1);
	LoadTexByWeather(name, "up");
	SetSkyTex(IsThereTexHandle("up", 0), 0, 2);
	LoadTexByWeather(name, "dn");
	SetSkyTex(IsThereTexHandle("dn", 0), 0, 3);
	m_turn = turn;
	m_farDiv = Max(divisions[0], 1);
	m_nearDiv = Max(divisions[1], 1);
	WVideoDev* video = g_resrcmng->VideoReference();
	float turns[2] = { turn / divisions[0], turn / divisions[1] };
	for (int part = 0; part < 4; ++part)
	{
		if (!(m_batch[part].xiFlag0 & 0x7ff))
			continue;
		m_batch[part].xiFlag1 = 5;
		unsigned long fvf = video->xDetermineFVF(m_batch[part].xiFlag0,
			m_batch[part].xiFlag1, 0);
		unsigned long usage = video->xDetermineBufferUsage(fvf) | 8;
		int stride = video->VertexSize(fvf);
		int numVertices, numIndices, rowBytes, columns;
		float radius;
		float heights[3], vs[3];
		if (part <= 1)
		{
			numVertices = (32 + divisions[part]) * 3;
			numIndices = divisions[part] * 4 + 130;
			rowBytes = stride * (32 + divisions[part]);
			radius = part == 0 ? 500.0f : 400.0f;
			heights[0] = radius * (g_PI / 4.0f);
			heights[1] = radius * (g_PI / 5.0f);
			heights[2] = radius * (-g_PI / 5.0f);
			vs[0] = 0.0f;
			vs[1] = 0.1f;
			vs[2] = 1.0f;
			columns = 32 / divisions[part] + 1;
		}
		else
		{
			m_batch[part].xiFlag1 |= 0x400;
			numVertices = 4;
			numIndices = 4;
			rowBytes = stride * 2;
			heights[0] = (part == 2 ? 1.0f : -1.0f) * (250.0f * g_PI / 4.0f);
			vs[0] = 0.0f;
			vs[1] = 1.0f;
			columns = 2 / divisions[part];
		}
		int vb = video->xCreateVertexBuffer(numVertices, fvf, usage);
		int ib = video->xCreateIndexBuffer(numIndices, usage);
		unsigned long colors[3];
		if (!part)
			colors[0] = ModifyColor(m_color, GOLFDOC()->m_bright);
		else
			colors[0] = ModifyColor(m_color, GOLFDOC()->m_bright) | 0xff000000;
		colors[1] = colors[0] | 0xff000000;
		colors[2] = colors[0] | 0xff000000;
		unsigned char* vertices = video->xLockVertexBuffer(vb, 0, 0);
		int band;
		for (band = 0; band < divisions[part]; ++band)
		{
			for (int i = 0; i < columns; ++i)
			{
				float x, z, u;
				if (part <= 1)
				{
					float angle =
						((i + band * (columns - 1)) / 32.0f - turns[part]) *
						(2.0f * g_PI);
					x = cosf(angle) * radius;
					z = sinf(angle) * radius;
					u = 1.0f - divisions[part] * i / 32.0f;
					for (int row = 0; row < 3; ++row)
					{
						int offset =
							stride * (i + band * columns) + rowBytes * row;
						memcpy(vertices + offset, &x, 4);
						offset += 4;
						memcpy(vertices + offset, &heights[row], 4);
						offset += 4;
						memcpy(vertices + offset, &z, 4);
						offset += 4;
						memcpy(vertices + offset, &colors[row], 4);
						offset += 4;
						memcpy(vertices + offset, &u, 4);
						offset += 4;
						memcpy(vertices + offset, &vs[row], 4);
						offset += 4;
					}
				}
				else
				{
					u = i % 2 ? 0.0f : 1.0f;

					for (int row = 0; row < 2; ++row)
					{
						int offset =
							stride * (i + band * columns) + rowBytes * row;
						x = (i ? 1.0f : -1.0f) * 500.0f;
						z = row ? -500.0f : 500.0f;
						memcpy(vertices + offset, &x, 4);
						offset += 4;
						memcpy(vertices + offset, &heights[0], 4);
						offset += 4;
						memcpy(vertices + offset, &z, 4);
						offset += 4;
						memcpy(vertices + offset, &colors[0], 4);
						offset += 4;
						memcpy(vertices + offset, &u, 4);
						offset += 4;
						memcpy(vertices + offset, &vs[row], 4);
						offset += 4;
					}
				}
			}
		}
		video->xUnlockVertexBuffer(vb);
		unsigned short* indices =
			(unsigned short*)video->xLockIndexBuffer(ib, 0, 0);
		int count;
		if (part <= 1)
		{
			count = 0;
			for (band = 0; band < 2; ++band)
			{
				for (int j = 0; j < divisions[part]; ++j)
				{
					for (int i = 0; i < columns; ++i)
					{
						unsigned short index =
							i + j * columns + band * (32 + divisions[part]);
						indices[count] = index;
						indices[count + 1] = index + 32 + divisions[part];
						count += 2;
					}
				}
				if (band < 1)
				{
					indices[count] = indices[count - 1];
					indices[count + 1] = 32 + divisions[part];
					count += 2;
				}
			}
		}
		else
		{
			indices[0] = 0;
			indices[1] = 2;
			indices[2] = 1;
			indices[3] = 3;
			count = 4;
		}
		video->xUnlockIndexBuffer(ib);
		m_batch[part].xnmTransfs = 0;
		m_batch[part].xnVtxs = numVertices;
		m_batch[part].xiBaseVtxIdx = 0;
		m_batch[part].xiBaseIdxIdx = 0;
		m_batch[part].xiFlag1 |= ((vb << 8) | (ib & 0xff)) << 14;
		m_batch[part].xnIdxs = count;
	}
}

void CSky::SetSkyTex(int tex, unsigned char unused, unsigned char index)
{
	if (tex)
		m_batch[index].xiFlag0 = (tex & 0x7ff) | 0x22300000;
	else
		m_batch[index].xiFlag0 = 0;
}

void CSky::ChangeWeatherTex(unsigned char weather)
{
	int farTex = IsThereTexHandle("far", weather);
	int nearTex = IsThereTexHandle("near", weather);
	if (!farTex && !nearTex)
		return;
	SetSkyTex(farTex, weather, 0);
	SetSkyTex(nearTex, weather, 1);
	SetSkyTex(IsThereTexHandle("up", weather), weather, 2);
	SetSkyTex(IsThereTexHandle("dn", weather), weather, 3);
}

void CSky::ClearClouds()
{
	if (g_resrcmng && m_cloudTex)
	{
		g_resrcmng->Release(m_cloudTex);
		m_cloudTex = 0;
	}
	m_cloudNum = 0;
	m_activeCloudNum = 0;
}

void CSky::RegisterCloud(char* name)
{
	ClearClouds();
	m_cloudTex = g_resrcmng->LoadTexture(name, 0, 0, NULL);
}

void CSky::ShowClouds(bool bShow)
{
	if (bShow == true)
	{
		for (int i = 0; i < m_cloudNum; ++i)
			m_cloud[i].bActive = rand() % 2 == 0;
	}
	else
	{
		for (int i = 0; i < m_cloudNum; ++i)
			m_cloud[i].bActive = true;
	}
}

void CSky::GenerateClouds(int num, float rate)
{
	num = Min(num, 50);
	m_cloudNum = num;
	if (rate < 1.0f)
		m_cloudInterval = 10.0f;
	else
		m_cloudInterval = 9.0f / rate + 1.0f;
	for (int i = 0; i < num; ++i)
	{
		sCloud& cloud = m_cloud[i];
		cloud.size = Random(1.0f, 2.0f) * 256.0f;
		cloud.tile = rand() % 4;
		cloud.windRand = Random(-0.1f, 0.1f);
		cloud.bFlip = rand() % 2 != 0;
		if (rand() % 2 == 0)
		{
			cloud.pos =
				WVector(Random(-1.0f, 1.0f), 0.0f, Random(-1.0f, 1.0f)) *
				m_cloudRange;
			cloud.pos.y = m_cloudHeight;
			cloud.bActive = true;
			++m_activeCloudNum;
		}
		else
		{
			cloud.pos.y = m_cloudHeight;
			cloud.bActive = false;
		}
	}
}

void CSky::GenerateStars(int map)
{
	if (m_pStar)
	{
		delete m_pStar;
		m_pStar = NULL;
	}
	if (map == -1)
		map = GetCurMap();
	IFF_STRUCT::sCourse* course = ItemManager()->FindCourse(
		(map > 127 && map != 253 ? map - 128 : map) | 0x28000000);
	if (!course)
		return;
	const char* name = course->SkyFx;
	if (!*name || (int)strlen(name) < 5)
		return;
	const char* ext = strrchr(name, '.');
	if (!ext || strlen(ext) < 4)
		return;
	if (!m_pStar)
	{
		m_pStar = new CPvsFxBox;
		m_pStar->Init(g_resrcmng);
		m_pStar->SetCamera(&g_camera);
		m_pStar->m_frameTime = 1000.0f / 30.0f;
	}
	if (!stricmp(ext + 1, "spr"))
	{
		CFxSpray* spray = m_pStar->LoadSpray(name, NULL);
		if (spray)
		{
			Attacher attacher;
			attacher.mat[0] = NULL;
			attacher.pos[0] = &g_view->camera.pivot;
			spray->m_attacher = &attacher;
			spray->m_centerX = attacher.x;
			spray->m_centerY = attacher.y;
			memcpy(spray->m_attachMat, attacher.mat, sizeof(attacher.mat));
			memcpy(spray->m_attachPos, attacher.pos, sizeof(attacher.pos));
			m_pStar->AddSpray(spray);
		}
	}
	else if (!stricmp(ext + 1, "seq"))
	{
		CFxSequence* seq = m_pStar->LoadSequence(name, true, NULL);
		if (seq)
		{
			Attacher attacher;
			attacher.mat[0] = NULL;
			attacher.pos[0] = &g_view->camera.pivot;
			seq->m_attacher = &attacher;
			seq->m_centerX = attacher.x;
			seq->m_centerY = attacher.y;
			m_pStar->AddSequence(seq);
			m_pStar->Process(m_pStar->m_frameTime);
			for (std::map<CFxSpray*, CFxSpray*>::iterator it =
					 seq->GetSprayList().begin();
				it != seq->GetSprayList().end(); ++it)
			{
				CFxSpray* spray = (*it).second;
				spray->m_attacher = &attacher;
				spray->m_centerX = attacher.x;
				spray->m_centerY = attacher.y;
				memcpy(spray->m_attachMat, attacher.mat, sizeof(attacher.mat));
				memcpy(spray->m_attachPos, attacher.pos, sizeof(attacher.pos));
				m_pStar->AddSpray(spray);
			}
			m_pStar->DeleteSequence(seq, false);
		}
	}
}

void CSky::GenerateMoons()
{
	if (m_pMoon)
	{
		delete m_pMoon;
		m_pMoon = NULL;
	}
	int num = GOLFDOC()->m_pPolySoup->m_header.pointNum;
	WPolySoup::sPoint* points = GOLFDOC()->m_pPolySoup->m_point;
	for (int i = 0; i < num; ++i)
	{
		WPolySoup::sPoint* point = &points[i];
		if (point->type != 3)
			continue;
		const char* ext = strrchr(point->option.c_str(), '.');
		if (!ext || strlen(ext) < 4)
			continue;
		if (!m_pMoon)
		{
			m_pMoon = new CPvsFxBox;
			m_pMoon->Init(g_resrcmng);
			m_pMoon->SetCamera(&g_camera);
			m_pMoon->m_frameTime = 1000.0f / 30.0f;
		}
		if (!stricmp(ext + 1, "spr"))
		{
			CFxSpray* spray = m_pMoon->LoadSpray(point->option.c_str(), NULL);
			if (spray)
			{
				Attacher attacher;
				attacher.mat[0] = NULL;
				attacher.pos[0] = &g_view->camera.pivot;
				spray->m_attacher = &attacher;
				spray->m_centerX = attacher.x;
				spray->m_centerY = attacher.y;
				spray->Vel() = point->pos.Normalize() * 50.0f;
				memcpy(spray->m_attachMat, attacher.mat, sizeof(attacher.mat));
				memcpy(spray->m_attachPos, attacher.pos, sizeof(attacher.pos));
				m_pMoon->AddSpray(spray);
			}
		}
		else if (!stricmp(ext + 1, "seq"))
		{
			CFxSequence* seq =
				m_pMoon->LoadSequence(point->option.c_str(), true, NULL);
			if (seq)
			{
				Attacher attacher;
				attacher.mat[0] = NULL;
				attacher.pos[0] = &g_view->camera.pivot;
				seq->m_attacher = &attacher;
				seq->m_centerX = attacher.x;
				seq->m_centerY = attacher.y;
				m_pMoon->AddSequence(seq);
				m_pMoon->Process(m_pMoon->m_frameTime);
				for (std::map<CFxSpray*, CFxSpray*>::iterator it =
						 seq->GetSprayList().begin();
					it != seq->GetSprayList().end(); ++it)
				{
					CFxSpray* spray = (*it).second;
					spray->m_attacher = &attacher;
					spray->m_centerX = attacher.x;
					spray->m_centerY = attacher.y;
					spray->Vel() =
						point->pos.Normalize() * 50.0f + spray->Pos();
					memcpy(spray->m_attachMat, attacher.mat,
						sizeof(attacher.mat));
					memcpy(spray->m_attachPos, attacher.pos,
						sizeof(attacher.pos));
					m_pMoon->AddSpray(spray);
				}
				m_pMoon->DeleteSequence(seq, false);
			}
		}
	}
}

void CSky::GenerateLensFlare()
{
	if (m_pLensFlare)
	{
		delete m_pLensFlare;
		m_pLensFlare = NULL;
	}
	int num = GOLFDOC()->m_pPolySoup->m_header.pointNum;
	WPolySoup::sPoint* points = GOLFDOC()->m_pPolySoup->m_point;
	for (int i = 0; i < num; ++i)
	{
		WPolySoup::sPoint* point = &points[i];
		if (point && point->type == 2)
		{
			if (!m_pLensFlare)
				m_pLensFlare = new CLensFlare;
			m_pLensFlare->Init(point->pos);
			break;
		}
	}
}

void CSky::ClearSky()
{
	switch (GetCurMap())
	{
	case 5:
	case 6:
		m_pStar->Clear();
		m_pMoon->Clear();
		if (m_pStar)
		{
			delete m_pStar;
			m_pStar = NULL;
		}
		if (m_pMoon)
		{
			delete m_pMoon;
			m_pMoon = NULL;
		}
		break;
	case 7:
		m_pMoon->Clear();
		if (m_pMoon)
		{
			delete m_pMoon;
			m_pMoon = NULL;
		}
		break;
	}
}

void CSky::Process(float delta)
{
	m_cloudTime += delta;
	if (m_cloudTime > m_cloudInterval)
	{
		m_cloudTime -= m_cloudInterval;
		if (m_activeCloudNum < m_cloudNum)
		{
			for (int i = 0; i < m_cloudNum; ++i)
			{
				if (!m_cloud[i].bActive)
				{
					m_cloud[i].pos = WVector(Random(-1.0f, 1.0f), 0.0f, -1.0f) *
						m_cloudRange * RotMat(Wind().GetGlobalDirection(), 1);
					m_cloud[i].pos.y = m_cloudHeight;
					m_cloud[i].bActive = true;
					m_cloud[i].bFlip = rand() % 2 != 0;
					++m_activeCloudNum;
					break;
				}
			}
		}
	}
	for (int i = 0; i < m_cloudNum; ++i)
	{
		sCloud& cloud = m_cloud[i];
		if (!cloud.bActive)
			continue;
		cloud.pos +=
			((cloud.windRand + 1.0f) * 3.0f) * Wind().GetGlobalWind() * delta;
		float dx = cloud.pos.x - g_camera.pivot.x;
		float dz = cloud.pos.z - g_camera.pivot.z;
		float alpha =
			Max(0.0f, 1.0f - sqrtf(dx * dx + dz * dz) / m_cloudFadeRange);
		if (alpha > 0.5f)
			cloud.alpha = 255.0f;
		else
			cloud.alpha = (alpha + alpha) * 255.0f;
		WVector pos;
		pos = cloud.pos - GOLFDOC()->m_pPolySoup->GetBaseModel()->pos;
		float distance = sqrtf(pos.x * pos.x + pos.z * pos.z);
		alpha = Max(0.0f, 1.0f - distance / m_cloudRange);
		if (alpha < 0.1f)
			cloud.alpha = alpha * cloud.alpha * 10.0f;
		if (sqrt(2.0) * m_cloudRange < distance)
		{
			cloud.bActive = false;
			--m_activeCloudNum;
		}
	}
	if (m_pStar)
		m_pStar->Process(delta * 1000.0f);
	if (m_pMoon)
		m_pMoon->Process(delta * 1000.0f);
	if (m_pLensFlare)
		m_pLensFlare->Process(delta);
	WVector wind = Wind().GetGlobalWind();
	WMatrix rot;
	g_camera.GetRotMatrix(&rot);
	WVector left;
	left = WVector(-1.0f, 0.0f, 0.0f) * rot;
	left.Normalize();
	wind.Normalize();
	float dot = left * wind;
	m_nearAngle += Wind().GetGlobalWind().Magnitude() * dot * delta * 0.005f;
	if (fabsf(m_nearAngle) >= 360.0f)
		m_nearAngle = 0.0f;
	m_upAngle += delta * 0.01f;
	if (m_upAngle >= 360.0f)
		m_upAngle = 0.0f;
}

void CSky::Render(WView* view)
{
	float nearClip = view->GetClipNearValue();
	float farClip = view->GetClipFarValue();
	view->SetClip(1.0f, CalcClipFarByView(view), false);
	view->UpdateCamera();
	m_pos = view->camera.pivot;
	m_pos.y += m_height;
	RenderUpAndBottom(view);
	if (GOLFDOC()->m_gameMode != 0x200 || view->GetReflective())
	{
		RenderFarCylinder(view);
		if (m_pStar)
		{
			for (std::map<CFxSpray*, CFxSpray*>::iterator it =
					 m_pStar->GetSprayList().begin();
				it != m_pStar->GetSprayList().end(); ++it)
				(*it).second->Pos() = g_view->camera.pivot;
			m_pStar->Display(g_view);
			g_view->Flush(0);
		}
		if (m_pMoon)
		{
			for (std::map<CFxSpray*, CFxSpray*>::iterator it =
					 m_pMoon->GetSprayList().begin();
				it != m_pMoon->GetSprayList().end(); ++it)
			{
				CFxSpray* spray = (*it).second;
				spray->Pos() = g_view->camera.pivot + spray->Vel();
			}
			m_pMoon->Display(g_view);
			g_view->Flush(0);
		}
		if (m_pLensFlare && !Doc()->m_golfGame.weather)
			m_pLensFlare->Render(view);
		RenderNearCylinder(view);
		if (GOLFDOC()->m_gameMode != 0x200000)
		{
			if (GOLFDOC()->m_gameMode & 0x200)
				RenderFlatCloud(view);
			else
				RenderBillboardCloud(view);
		}
	}
	view->SetClip(nearClip, farClip, false);
	view->UpdateCamera();
}

void CSky::RenderUpAndBottom(WView* view)
{
	WMatrix mat;
	if (m_batch[2].xiFlag0 & 0x7ff)
	{
		mat.Reset();
		mat.Rotate(m_upAngle, 1);
		mat *= CalcScaleByView(view);
		mat.pivot = m_pos;
		m_batch[2].xmW = mat;
		view->xDrawIndexedTriangles(m_batch[2]);
	}
	if (m_batch[3].xiFlag0 & 0x7ff)
	{
		mat.Reset();
		mat *= CalcScaleByView(view);
		mat.pivot = m_pos;
		m_batch[3].xmW = mat;
		view->xDrawIndexedTriangles(m_batch[3]);
	}
}

void CSky::RenderFarCylinder(WView* view)
{
	if (m_batch[0].xiFlag0 & 0x7ff)
	{
		WMatrix mat;
		mat.Reset();
		mat *= CalcScaleByView(view);
		mat.pivot = m_pos;
		m_batch[0].xmW = mat;
		view->xDrawIndexedTriangles(m_batch[0]);
	}
}

void CSky::RenderBillboardCloud(WView* view)
{
	WTVertex vtx[4];
	WTVertex* quad[5] = { &vtx[0], &vtx[1], &vtx[3], &vtx[2], NULL };
	for (int i = 0; i < m_cloudNum; ++i)
	{
		if (!m_cloud[i].bActive)
			continue;
		for (int j = 0; j < 4; ++j)
		{
			WVector pos;
			pos = RotVec(WVector(j & 2 ? 0.5f : -0.5f, j & 1 ? 0.25f : -0.25f,
							 0.0f),
					  view->camera) *
					m_cloud[i].size +
				m_cloud[i].pos;
			vtx[j].SetPosition(pos);
			vtx[j].tu = m_cloud[i].bFlip ? (j & 2 ? 0.0f : 1.0f)
										 : (j & 2 ? 1.0f : 0.0f);
			vtx[j].tv = (m_cloud[i].tile + (j & 1 ? 0 : 1)) * 0.25f;
			vtx[j].diffuse = ((unsigned char)m_cloud[i].alpha << 24) |
				ModifyColor(m_color, GOLFDOC()->m_bright);
		}
		view->DrawPolygonFan(quad, (m_cloudTex & 0x7ff) | 0x20300000, 0x400, 0);
	}
}

void CSky::RenderFlatCloud(WView* view)
{
	WTVertex vtx[4];
	WTVertex* quad[5] = { &vtx[0], &vtx[1], &vtx[3], &vtx[2], NULL };
	for (int i = 0; i < m_cloudNum; ++i)
	{
		sCloud& cloud = m_cloud[i];
		if (!cloud.bActive)
			continue;
		for (int j = 0; j < 4; ++j)
		{
			WVector pos;
			pos = WVector(j & 2 ? 0.5f : -0.5f, 0.0f, j & 1 ? 0.25f : -0.25f) *
					cloud.size +
				cloud.pos;
			vtx[j].SetPosition(pos);
			vtx[j].tu =
				cloud.bFlip ? (j & 2 ? 0.0f : 1.0f) : (j & 2 ? 1.0f : 0.0f);
			vtx[j].tv = (cloud.tile + (j & 1 ? 0 : 1)) * 0.25f;
			vtx[j].diffuse =
				ModifyColor(m_color, GOLFDOC()->m_bright) | 0xff000000;
		}
		view->DrawPolygonFan(quad, (m_cloudTex & 0x7ff) | 0x20300000, 0x400, 0);
	}
}

void CSky::RenderNearCylinder(WView* view)
{
	if (m_batch[1].xiFlag0 & 0x7ff)
	{
		WMatrix mat;
		mat.Reset();
		mat.Rotate(m_nearAngle, 1);
		mat *= CalcScaleByView(view);
		mat.pivot = m_pos;
		m_batch[1].xmW = mat;
		view->xDrawIndexedTriangles(m_batch[1]);
	}
}

void CSky::UpdateBright()
{
}

float CSky::CalcScaleByView(WView* view) const
{
	if (view->GetProjectionMode() == WView::PARALLEL)
		return (view->GetScale() + view->GetScale()) * 0.002f;
	return 1.0f;
}

float CSky::CalcClipFarByView(WView* view) const
{
	if (view->GetProjectionMode() == WView::PARALLEL)
		return CalcScaleByView(view) * 1000.0f;
	return view->GetClipFarValue();
}
