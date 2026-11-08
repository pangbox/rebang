#include "projectg.h"
#include "woverlay.h"
#include "clientsetting.h"
#include "ani_overlay.h"

CAniOverlay::CAniOverlay(const char* filename, float width, float height)
	: m_pOverlay(NULL)
{
	m_pOverlay = g_resrcmng->GetOverlay(filename, 0);
	m_width = width;
	m_height = height;

	m_cols = (int)(m_pOverlay->GetWidth() / m_width);
	m_rows = (int)(m_pOverlay->GetHeight() / m_height);
	m_frameNum = m_cols * m_rows;
}

CAniOverlay::~CAniOverlay()
{
	if (m_pOverlay)
	{
		g_resrcmng->Release(m_pOverlay);
		m_pOverlay = NULL;
	}
}

void CAniOverlay::Render(float x, float y, int frame, int mode, int color,
	float scale)
{
	m_clipRect.x = x - m_width * scale * 0.5f;
	m_clipRect.y = y - m_height * scale * 0.5f;
	m_clipRect.w = m_width * scale;
	m_clipRect.h = m_height * scale;
	if (m_pOverlay)
		m_pOverlay->SetClippingArea(&m_clipRect);

	m_rect.x = m_clipRect.x - (frame % m_cols) * scale * m_width;
	m_rect.y = m_clipRect.y - (frame / m_cols) * scale * m_height;
	if (!m_pOverlay)
		return;
	m_rect.w = m_pOverlay->GetWidth() * scale;
	m_rect.h = m_pOverlay->GetHeight() * scale;

	m_pOverlay->Render(g_view, WRect(0, 0, 1.0f, 1.0f), m_rect, mode, color, 0,
		0);
}

CEffectOverlay::CEffectOverlay()
	: m_pAniOverlay(NULL), m_bActive(false)
{
	m_pAniOverlay = new CAniOverlay("bar-impact.jpg", 32.0f, 32.0f);
}

CEffectOverlay::~CEffectOverlay()
{
	if (m_pAniOverlay)
	{
		delete (m_pAniOverlay);
		(m_pAniOverlay) = 0;
	}
}

void CEffectOverlay::Process(float delta)
{
	if (!m_bActive)
		return;

	static float s_time = 0;

	s_time += delta;

	if (s_time > m_frameTime)
	{
		s_time -= m_frameTime;
		m_frame++;

		if (m_frame == 8)
		{
			s_time = 0;
			m_bActive = false;
		}
	}
}

void CEffectOverlay::SetActive()
{
	m_bActive = true;
	m_frame = 0;
	m_frameTime = 1.0f / 16;
}

void CEffectOverlay::Display()
{
	if (!m_bActive)
		return;
	m_pAniOverlay->Render(s_barStart, COption::Instance()->gGetBar_Y() - 8.0f,
		m_frame, 0x800000, 0xffffffff, 1.0f);
}
