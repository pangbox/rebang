#include "uccbrush.h"
#include "wresrcmng.h"

sUccCurrentBrush* UccCurrentBrush()

{
	static sUccCurrentBrush s_currentBrush;

	return &s_currentBrush;
}

CUccBrush* UccBrushList(eBrushType type)
{
	static CUccBrush s_brushList[4];
	static bool s_bInit = false;
	if (!s_bInit)
	{
		s_bInit = true;
		for (int i = 0; i < 4; i++)
		{
			s_brushList[i].Initialize((eBrushType)i);
		}
	}

	return &s_brushList[type];
}

CUccBrush::CUccBrush()
{
}

CUccBrush::~CUccBrush()
{
	Clear();
}

void CUccBrush::Initialize(eBrushType type)
{
	Clear();
	m_type = type;

	char filename[256];
	int i = 0;
	while (true)
	{
		switch (m_type)
		{
		case BRUSH_BRUSH:

			sprintf(filename, "ucc_brush%02d.tga", i++);
			break;

		case BRUSH_PEN:

			sprintf(filename, "ucc_pen%02d.tga", i++);
			break;

		case BRUSH_ERASER:

			sprintf(filename, "ucc_eraser%02d.tga", i++);
			break;

		case BRUSH_PATTERN:
			sprintf(filename, "ucc_pattern%02d.tga", i++);
			break;
		}

		Bitmap* brush = LoadBrushFromFile(filename);

		if (!brush)
			break;

		AddBrush(brush);

		ClearTexCache(brush);
		if (brush)
			delete brush;
		brush = NULL;
	}
}

void CUccBrush::Clear()
{
	for (unsigned int i = 0; i < m_brush.size(); i++)
	{
		ClearTexCache(m_brush[i]);
		if (m_brush[i])
			delete m_brush[i];
		m_brush[i] = NULL;
	}
	m_brush.clear();
}

Bitmap* CUccBrush::GetBrush(int index)
{
	if (index < m_brush.size())
	{
		return m_brush[index];
	}

	return NULL;
}

void CUccBrush::SetBrush(int index, Bitmap* brush)
{
	if (index >= m_brush.size())
		return;

	ClearTexCache(m_brush[index]);
	if (m_brush[index])
		delete m_brush[index];
	m_brush[index] = NULL;

	Bitmap* newBrush = new Bitmap;
	*newBrush = *brush;
	m_brush[index] = newBrush;
}

Bitmap* CUccBrush::LoadBrushFromFile(const char* filename)
{
	Bitmap* bitmap = g_resrcmng->LoadBitmap(filename, 0, false);
	if (bitmap && bitmap->BitsPerPixel() == 32)
		return bitmap;

	return NULL;
}

void CUccBrush::AddBrush(Bitmap* brush)
{
	if (!brush)
		return;

	Bitmap* newBrush = new Bitmap;

	if (brush->BitsPerPixel() == 32)
	{
		*newBrush = *brush;
		m_brush.push_back(newBrush);
	}
	else if (brush->BitsPerPixel() == 8)
	{
		newBrush->Create(brush->Width(), brush->Height(), 32);
		for (unsigned int y = 0; y < brush->Height(); y++)
		{
			for (unsigned int x = 0; x < brush->Width(); x++)
			{
				BYTE r, g, b;
				brush->GetPixel(x, y, r, g, b);
				if (r || g || b)
					newBrush->SetPixel(x, y, 0xff, 0xff, 0xff, 0);
				else
					newBrush->SetPixel(x, y, 0, 0, 0, 0xff);
			}
		}

		m_brush.push_back(newBrush);
	}
}

void CUccBrush::RemoveBrush(int index)
{
	if (index >= m_brush.size())
		return;

	ClearTexCache(m_brush[index]);
	if (m_brush[index])
		delete m_brush[index];
	m_brush[index] = NULL;

	for (unsigned int i = index; i < m_brush.size() - 1; i++)
		m_brush[i] = m_brush[i + 1];

	m_brush[m_brush.size() - 1] = NULL;
	m_brush.resize(m_brush.size() - 1);
}

sUccCurrentBrush::sUccCurrentBrush()

{
	type = BRUSH_BRUSH;
	index = 0;
	r = 0;
	g = 0;
	b = 0;
	SetBrush(BRUSH_BRUSH, 0, 0, 0, 0);
}

void sUccCurrentBrush::SetBrush(eBrushType type, int index)
{
	SetBrush(type, index, r, g, b);
}

void sUccCurrentBrush::SetBrush(unsigned char r, unsigned char g,
	unsigned char b)
{
	SetBrush(type, index, r, g, b);
}

void sUccCurrentBrush::SetBrush(eBrushType type, int index, unsigned char r,
	unsigned char g, unsigned char b)
{
	Bitmap* src = UccBrushList(type)->GetBrush(index);
	if (!src)
		return;

	if (type == BRUSH_ERASER)
	{
		r = 0xff;
		g = 0xff;
		b = 0xff;
	}

	brush.Create(24, 24, 32);
	for (int y = 0; y < 24; y++)
	{
		for (int x = 0; x < 24; x++)
		{
			BYTE sr, sg, sb, sa;
			src->GetPixel(x, y, sr, sg, sb, sa);

			if (sr || sg || sb)
				brush.SetPixel(x, y, 0, 0, 0, 0);
			else

				brush.SetPixel(x, y, r, g, b, sa);
		}
	}

	if (type != BRUSH_ERASER)
	{
		this->type = type;
		this->index = index;
		this->r = r;
		this->g = g;
		this->b = b;
	}
}

void sUccCurrentBrush::SetBrush(eBrushType type, Bitmap* brush)
{
	if (!brush)
		return;

	this->type = type;
	this->brush = *brush;
}

void sUccCurrentBrush::GetBrushColor(unsigned char& r, unsigned char& g,
	unsigned char& b)
{
	r = this->r;
	g = this->g;
	b = this->b;
}
