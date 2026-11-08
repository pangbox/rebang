#include "projectg.h"
#include "w3dspr.h"
#include "lensflare.h"

CLensFlare::w_flare_set CLensFlare::ms_lenzTable[7] = {
	{ 0, 0.5f,   0.03f,  0xff0b0b0f },
	{ 0, 0.4f,   0.05f,  0xff0b0b0f },
	{ 0, 0.2f,   0.032f, 0xff1d1008 },
	{ 0, -0.47f, 0.03f,  0xff110901 },
	{ 0, -0.65f, 0.06f,  0xff050f0c },
	{ 0, -1.0f,  0.2f,   0xff110e01 },
	{ 1, -1.3f,  0.4f,   0xff191919 },
};

static const char* s_overlayFile[] = { "~lensflare01.bmp", "~lensflare02.bmp" };
static const char* s_sprFile[] = { "~glaring_yellowish.bmp", "~sun.bmp",
	"~lensflare03.bmp", "~lensflare04.bmp" };

CLensFlare::CLensFlare()
{
	memset(m_overlay, 0, sizeof(m_overlay));
	memset(m_spr, 0, sizeof(m_spr));
}

CLensFlare::~CLensFlare()
{
	int i;
	for (i = 0; i < 2; i++)
	{
		if (g_resrcmng && m_overlay[i])
		{
			g_resrcmng->Release(m_overlay[i]);
			m_overlay[i] = NULL;
		}
	}

	for (i = 0; i < 4; i++)
	{
		if (g_resrcmng && m_spr[i])
		{
			g_resrcmng->Release(m_spr[i]);
			m_spr[i] = NULL;
		}
	}
}

void CLensFlare::Init(const WVector& sunDir)
{
	float scale[4];

	scale[0] = 8.0f;
	scale[1] = 1.5f;
	scale[2] = 1.5f;
	scale[3] = 1.0f;

	for (unsigned int i = 0;
		i < sizeof(s_overlayFile) / sizeof(s_overlayFile[0]); i++)
	{
		m_overlay[i] = g_resrcmng->GetOverlay(s_overlayFile[i], 0);
		m_overlay[i]->SetCoordMode(0x2200);
	}

	for (unsigned int j = 0; j < sizeof(s_sprFile) / sizeof(s_sprFile[0]); j++)
	{
		m_spr[j] = g_resrcmng->Get3DSpr(s_sprFile[j], 0);
		int tex = g_resrcmng->LoadTexture(s_sprFile[j], 0, 0, 0);
		float width = g_resrcmng->GetTextureWidth(tex) * scale[j];
		float height = g_resrcmng->GetTextureHeight(tex) * scale[j];

		m_size[j].w = width;
		m_size[j].h = height;

		if (g_resrcmng && tex)
		{
			g_resrcmng->Release(tex);
			tex = 0;
		}
	}

	SetSunDirection(sunDir);
	m_angle = 0.0f;
}

void CLensFlare::SetSunDirection(const WVector& sunDir)
{
	m_sunDir = sunDir;
	m_sunDir.Normalize();
	m_alpha = Wabs(m_sunDir.y) * 0.8f + 0.2f;
	m_spr[2]->SetColor(((int)(255.0f * m_alpha) << 24) | 0xffffff);
	m_spr[3]->SetColor(((int)(m_alpha * 255.0f) << 24) | 0xffffff);
}

void CLensFlare::Process(float delta)
{
	m_angle += delta * g_PI;

	if (m_angle > 376.99112f)
		m_angle -= 376.99112f;
}

void CLensFlare::Render(WView* view)
{
	if (m_sunDir * view->camera.za <= 0.0f)
		return;

	WVector pos;
	pos = m_sunDir * 1000.0f + view->camera.pivot;

	WTVertex p;
	view->Projection2(&p, pos);

	if (view->GetWidth() * -1.4f > p.sx || view->GetWidth() * 2.4f < p.sx ||
		view->GetHeight() * -1.4f > p.sy || view->GetHeight() * 2.4f < p.sy)
		return;

	float clipNear = view->GetClipNearValue();
	float clipFar = view->GetClipFarValue();
	view->SetClip(clipNear, 2000.0f, false);
	view->UpdateCamera();

	if (Doc()->m_golfGame.weather)
	{
		if (view->GetWidth() * -0.4f < p.sx && view->GetWidth() * 1.4f > p.sx &&
			view->GetHeight() * -0.4f < p.sy && view->GetHeight() * 1.4f > p.sy)
		{
			m_spr[1]->SetColor(0xff505050);
			m_spr[1]->SetPos(pos);
			m_spr[1]->SetRect(m_size[1].w, m_size[1].h, m_size[1].w * 0.5f,
				m_size[1].h * 0.5f);
			m_spr[1]->Render(view, 0x20800000, W3dSpr::CAMERA_XY_ALIGN);
		}

		view->SetClip(clipNear, clipFar, false);
		view->UpdateCamera();
		return;
	}

	m_spr[0]->SetPos(pos);
	float height = m_size[0].h * 2.0f;
	float width = m_size[0].w * 2.0f;
	m_spr[0]->SetRect(width, height, width * 0.5f, height * 0.5f);
	m_spr[0]->Render(view, 0x20b00000, W3dSpr::CAMERA_XY_ALIGN);

	if (view->GetWidth() * -0.4f > p.sx || view->GetWidth() * 1.4f < p.sx ||
		view->GetHeight() * -0.4f > p.sy || view->GetHeight() * 1.4f < p.sy)
	{
		view->SetClip(clipNear, clipFar, false);
		view->UpdateCamera();
		return;
	}

	m_spr[1]->SetPos(pos);
	m_spr[1]->SetRect(m_size[1].w, m_size[1].h, m_size[1].w * 0.5f,
		m_size[1].h * 0.5f);
	m_spr[1]->Render(view, 0x20800000, W3dSpr::CAMERA_XY_ALIGN);

	m_spr[2]->SetPos(pos);
	{
		float height = m_size[1].h * 2.0f;
		float width = m_size[1].w * 2.0f;
		m_spr[2]->SetRect(width, height, width * 0.5f, height * 0.5f);
	}
	m_spr[2]->Render(view, 0x20800000, W3dSpr::CAMERA_XY_ALIGN);

	if (m_size[1].w * -0.7f > p.sx ||
		m_size[1].w * 0.7f + view->GetWidth() < p.sx ||
		m_size[1].h * -0.7f > p.sy ||
		m_size[1].h * 0.7f + view->GetHeight() < p.sy)
	{
		view->SetClip(clipNear, clipFar, false);
		view->UpdateCamera();
		return;
	}

	float angle[5];
	angle[0] = 1.4137167f + m_angle * 0.05f;
	angle[1] = 0.78539819f - m_angle * 0.05f;
	angle[2] = 3.926991f + m_angle * 0.033333335f;
	angle[3] = 2.984513f - m_angle * 0.033333335f;
	angle[4] = m_angle * 0.016666668f;

	m_spr[3]->SetPos(pos);

	for (int i = 0; i < 5; i++)
	{
		float scale = ((rand() % 800 - 400) * 0.001f + 1.0f) * 2.0f;
		float height = m_size[2].h * scale;
		float width = m_size[2].w * scale;
		m_spr[3]->SetRect(width, height, width * 0.5f, height * 0.5f);
		m_spr[3]->Render(view, angle[i], 0x20800000);
	}

	WRect src;
	WRect dst;
	src.y = 0.0f;
	src.x = 0.0f;
	src.h = 1.0f;
	src.w = 1.0f;
	float dx = p.sx / view->GetWidth() - 0.5f;
	float dy = p.sy / view->GetHeight() - 0.5f;
	float ratio = view->GetWidth() * 0.0015625f;

	for (int j = 0; j < 7; j++)
	{
		WOverlay* overlay = m_overlay[ms_lenzTable[j].overlay];
		int r = rand() % 500;
		double size = ((r * 0.0001f + 1.0f) * ms_lenzTable[j].scale) *
			view->GetWidth() * ratio;
		dst.w = size;
		dst.h = size;
		dst.x =
			(dx * ms_lenzTable[j].pos + 0.5f) * view->GetWidth() - dst.w * 0.5f;
		dst.y = (dy * ms_lenzTable[j].pos + 0.5f) * view->GetHeight() -
			dst.w * 0.5f;
		overlay->Render(view, src, dst, 0xc00000, ms_lenzTable[j].color, 0.0f,
			0);
	}

	view->SetClip(clipNear, clipFar, false);
	view->UpdateCamera();
}
