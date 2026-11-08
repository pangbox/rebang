#include "ucccolorpicker.h"
#include "frarea.h"
#include "wresrcmng.h"

CUccColorPicker::CUccColorPicker()
{
}

CUccColorPicker::~CUccColorPicker()
{
	DestroyVariables();
}

void CUccColorPicker::DestroyVariables()
{
	m_color.r = 0;
	m_color.g = 0;
	m_color.b = 0;
}

void CUccColorPicker::Initialize(FrArea* area)
{
	CUccBase::Initialize(area, NULL, NULL);

	GenerateColorMap();
}

void CUccColorPicker::GetColor(const WPoint& pos, unsigned char& r,
	unsigned char& g, unsigned char& b)
{
	WRect rect = m_pArea->GetRect();

	GetColor((int)(pos.x - rect.x), (int)(pos.y - rect.y), r, g, b);
}

void CUccColorPicker::GetColor(int x, int y, unsigned char& r, unsigned char& g,
	unsigned char& b)
{
	if (x < 0 || y < 0 || x >= m_pTexture32->Width() ||
		y >= m_pTexture32->Height())
		return;
	m_pTexture32->GetPixel(x, y, r, g, b);
	m_color.r = r;
	m_color.g = g;
	m_color.b = b;
}

void CUccColorPicker::GenerateColorMap()
{
	ClearTexCache(m_pTexture32);
	if (m_pTexture32)
		delete m_pTexture32;
	m_pTexture32 = NULL;
	m_pTexture32 = g_resrcmng->LoadBitmap("ucc_color_map.tga", 0, false);
	if (m_pTexture32)

		RefreshTexCache(m_pTexture32);
}

CUccColorBrightness::CUccColorBrightness()
{
}

CUccColorBrightness::~CUccColorBrightness()
{
}

void CUccColorBrightness::Initialize(FrArea* area)
{
	CUccBase::Initialize(area, NULL, NULL);

	WRect rect = m_pArea->GetRect();

	int width = (int)rect.w;
	int height = (int)rect.h;

	ClearTexCache(m_pTexture32);
	if (m_pTexture32)
		delete m_pTexture32;
	m_pTexture32 = NULL;
	m_pTexture32 = new Bitmap(width, height, 32);
}

void CUccColorBrightness::GenerateBrightnessMap(unsigned char r,
	unsigned char g, unsigned char b)
{
	unsigned char h, l, s;

	RGBToHLS(r, g, b, h, l, s);
	l = 128;
	HLSToRGB(h, l, s, r, g, b);

	WRect rect = m_pArea->GetRect();
	int width = (int)rect.w;
	int height = (int)rect.h;

	int y = 0;
	int lastY = 0;

	int half = height / 2;

	double step = Max(half / 11, 1);

	double fr = 255.0;
	double fg = 255.0;
	double fb = 255.0;
	double dr = (255.0 - r) * (-1.0 / 11.0);
	double dg = (255.0 - g) * (-1.0 / 11.0);
	double db = (255.0 - b) * (-1.0 / 11.0);

	do
	{
		for (int i = 0; i < step; i++, y++)
		{
			if (y > half)
				break;

			for (int x = 0; x < width; x++)
			{
				m_pTexture32->SetPixel(x, y, (unsigned char)fr,
					(unsigned char)fg, (unsigned char)fb, 0xff);
				lastY = y;
			}
		}
		fr += dr;
		fg += dg;
		fb += db;
	} while (y <= half);

	fr = r;
	fg = g;
	fb = b;
	dr = r * (-1.0 / 11.0);
	dg = g * (-1.0 / 11.0);
	db = b * (-1.0 / 11.0);

	y = lastY;

	do
	{
		for (int i = 0; i < step; i++, y++)
		{
			if (y >= height)
				break;

			for (int x = 0; x < width; x++)
			{
				m_pTexture32->SetPixel(x, y, (unsigned char)fr,
					(unsigned char)fg, (unsigned char)fb, 0xff);
			}
		}
		fr += dr;
		fg += dg;
		fb += db;
	} while (y < height);

	RefreshTexCache(m_pTexture32);

	m_pArea->SetBgImg(m_pTexture32);
}
