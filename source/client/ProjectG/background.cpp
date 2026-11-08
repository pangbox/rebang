#include "background.h"
#include "wsplash.h"

#define SAFE_DELETE(p) \
	{ \
		if (p) \
		{ \
			delete (p); \
			(p) = 0; \
		} \
	}

CBackGround::CBackGround(const char* name, bool screenSized)
	: m_pSplash(0), m_du(0), m_dv(0), m_u(0), m_v(0)
{
	Load(name, screenSized);
}

CBackGround::CBackGround(bool screenSized, const Bitmap* bitmap)
{
	if (bitmap)
	{
		WSplash* pSplash = (WSplash*)g_resrcmng->VideoReference()->Command(
			W_VDEV_GET_SPLASH, 0, 0);
		if (pSplash)
		{
			if (!pSplash->Init(*bitmap->bi, bitmap->vram, screenSized))
				SAFE_DELETE(pSplash);
			m_pSplash = pSplash;
		}
	}
}

CBackGround::~CBackGround()
{
	SAFE_DELETE(m_pSplash);
}

void CBackGround::Load(const char* name, bool screenSized)
{
	if (name)
	{
		Bitmap* bitmap = g_resrcmng->LoadBitmap(name, 0, false);
		WSplash* pSplash = (WSplash*)g_resrcmng->VideoReference()->Command(
			W_VDEV_GET_SPLASH, 0, 0);
		if (pSplash)
		{
			if (!pSplash->Init(*bitmap->bi, bitmap->GetVram(0), screenSized))
				SAFE_DELETE(pSplash);

			m_pSplash = pSplash;
		}

		SAFE_DELETE(bitmap);
	}
	else
	{
		WSplash* pSplash = (WSplash*)g_resrcmng->VideoReference()->Command(
			W_VDEV_GET_SPLASH, 0, 0);
		if (pSplash)
		{
			if (!pSplash->InitFromScreen(true))
				SAFE_DELETE(pSplash);
			m_pSplash = pSplash;
		}
	}
}

void CBackGround::Process(const float deltaTime)
{
	if (m_pSplash)
	{
		m_u += deltaTime * m_du;
		m_v += deltaTime * m_dv;

		if (Abs(m_u) > 1.0f)
		{
			if (m_u > 0)
				m_u -= 1.0f;
			else
				m_u += 1.0f;
		}

		if (Abs(m_v) > 1.0f)
		{
			if (m_v > 0)
				m_v -= 1.0f;
			else
				m_v += 1.0f;
		}

		m_pSplash->SetTexCoordOffset(m_u, m_v);
	}
}

void CBackGround::Draw(unsigned long color)
{
	if (m_pSplash)
		m_pSplash->Draw(color);
}

void CBackGround::Draw(const WPoint* pos, const WRect* rect,
	unsigned long color)
{
	if (m_pSplash)
		m_pSplash->Draw(pos, rect, color);
}

void CBackGround::ResetScreenSize()
{
	if (m_pSplash)
		m_pSplash->ResetScreenSize();
}

int CBackGround::GetWidth()
{
	if (m_pSplash)
		return m_pSplash->GetWidth();

	return 0;
}

int CBackGround::GetHeight()
{
	if (m_pSplash)
		return m_pSplash->GetHeight();

	return 0;
}
