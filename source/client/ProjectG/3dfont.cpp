#include "projectg.h"
#include "3dfont.h"

C3dFont::C3dFont()
	: m_texHandle(0), m_projectionStyle(ProjectionStyle::Perspective)
{
	m_pos.Reset();

	m_pVtx[0] = &m_vtx[0];
	m_pVtx[1] = &m_vtx[1];
	m_pVtx[2] = &m_vtx[2];
	m_pVtx[3] = &m_vtx[3];

	m_reserved = 0;
}

C3dFont::~C3dFont()
{
	if (m_texHandle)
	{
		g_resrcmng->Release(m_texHandle);
		m_texHandle = 0;
	}
}

void C3dFont::LoadTexture(const char* filename, int width, int height,
	const char* charTable, float offsetX, float offsetY)
{
	m_texHandle = g_resrcmng->LoadTexture(filename, 0, 0, 0);
	int texWidth = g_resrcmng->GetTextureWidth(m_texHandle);
	int texHeight = g_resrcmng->GetTextureHeight(m_texHandle);

	m_uStep = (float)width / texWidth;
	m_cols = texWidth / width;

	m_charTable = charTable;
	m_vStep = (float)height / texHeight;
	m_offsetX = offsetX;
	m_offsetY = offsetY;

	m_width = width;
	m_height = height;
}

void C3dFont::SetPos(const WVector& pos)
{
	m_pos = pos;
}

void C3dFont::SetSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void C3dFont::SetColor(int color)
{
	m_pVtx[0]->diffuse = color;
	m_pVtx[1]->diffuse = color;
	m_pVtx[2]->diffuse = color;
	m_pVtx[3]->diffuse = color;
}

void C3dFont::Printf(WView* view, int space, const char* format, ...)
{
	char buf[1024];

	if (!m_cols)
		return;

	WVector pos = view->Projection(m_pos);

	if (pos.z <= 0.0f || pos.z >= 1.0f)
		return;

	for (int k = 0; k < 4; k++)
	{
		m_vtx[k].sz = pos.z;
		m_vtx[k].rhw = 1.0f;
	}

	va_list args;
	va_start(args, format);
	if (_vsnprintf(buf, sizeof(buf) - 1, format, args) == -1)
		buf[sizeof(buf) - 1] = 0;

	int len = strlen(buf);

	const char* table = m_charTable;

	const char* p = buf;
	if (!table)
		return;

	for (int i = 0; i < len; i++)
	{
		int c = *p++;

		const char* found = strchr(table, c);

		float u = ((found - table) % m_cols) * m_uStep;
		float v = ((found - table) / m_cols) * m_vStep;

		m_vtx[0].tu = u;
		m_vtx[0].tv = v + m_vStep;
		m_vtx[1].tu = u;
		m_vtx[1].tv = v;
		m_vtx[2].tu = u + m_uStep;
		m_vtx[2].tv = v;
		m_vtx[3].tv = v + m_vStep;
		m_vtx[3].tu = u + m_uStep;

		float x0 = m_offsetX - (m_width * len + (len - 1) * space) * 0.5f +
			(m_width + space) * i;
		float x1 = x0 + m_width;
		float y0 = m_offsetY - m_height * 0.5f;
		float y1 = y0 + m_height;

		if (!(pos.x + x0 > g_view->GetWidth() - 0.5f || pos.x + x1 < 0.5f ||
				pos.y + y0 > g_view->GetHeight() - 0.5f || pos.y + y1 < 0.5f))
		{
			m_vtx[0].sx = pos.x + x0;
			m_vtx[0].sy = pos.y + y1;

			m_vtx[1].sx = pos.x + x0;
			m_vtx[1].sy = pos.y + y0;

			m_vtx[2].sx = pos.x + x1;
			m_vtx[2].sy = pos.y + y0;

			m_vtx[3].sx = pos.x + x1;
			m_vtx[3].sy = pos.y + y1;

			for (int j = 0; j < 4; j++)
				ClipChar(j);

			int type = (m_texHandle & 0x7ff) | 0x2080000;
			if (m_projectionStyle == ProjectionStyle::Screen)
				type |= 0x300000;

			view->DrawPolygonFan(m_pVtx, type, 0, 1);
		}
	}
}

void C3dFont::ClipChar(int index)
{
	if (m_vtx[index].sx < 0.5f)
	{
		m_vtx[index].tu += (0.5f - m_vtx[index].sx) * m_uStep / m_width;
		m_vtx[index].sx = 0.5f;
	}
	else if (m_vtx[index].sx > g_view->GetWidth() - 0.5f)
	{
		m_vtx[index].tu -=
			(m_vtx[index].sx - g_view->GetWidth() + 0.5f) * m_uStep / m_width;
		m_vtx[index].sx = g_view->GetWidth() - 0.5f;
	}

	if (m_vtx[index].sy < 0.5f)
	{
		m_vtx[index].tv += (0.5f - m_vtx[index].sy) * m_vStep / m_height;
		m_vtx[index].sy = 0.5f;
	}
	else if (m_vtx[index].sy > g_view->GetHeight() - 0.5f)
	{
		m_vtx[index].tv -=
			(m_vtx[index].sy - g_view->GetHeight() + 0.5f) * m_vStep / m_height;
		m_vtx[index].sy = g_view->GetHeight() - 0.5f;
	}
}
