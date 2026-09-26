#include "wfont.h"
#include "wresrcmng.h"
#include "wminmax.h"
#include <stdio.h>
#include <string.h>

struct w_fnthead
{
	long id;
	int fontsize;
	int bAntialiased;
	int dummy;
};

unsigned char WFixedFont::m_mid[65536];

WFont::WFont(void)
	: m_eType(NORMAL), m_overlayList(4, 4)
{
	SetCoordMode(0x2600);

	m_scale = 1.0f;
	m_useOverlay = false;

	SetColorSet(1, 0xff0000);
	SetColorSet(2, 0x00ff00);
	SetColorSet(3, 0x0000ff);

	m_space = 1;
}

WFont::~WFont(void)
{
	ResetOverlay();
}

float WFont::GetFontScale(WView* view)
{
	return (view && (GetCoordMode() & 0x100))
		? view->GetWidth() * m_scale / 640.0f
		: m_scale;
}

float WFont::PrintOut(WView* view, float x, float y, const char* text, int type,
	unsigned long diffuse, bool draw, Bitmap* bmp)
{
	char* next;
	float len = 0;
	char temp[1024];
	char* ptr;
	unsigned long color;

	if (draw)
	{
		if (!bmp)
		{
			x = GetUnit(view, x, W_UNIT_XPOS);
			y = GetUnit(view, y, W_UNIT_YPOS);
		}

		color = diffuse;
	}

	ptr = (char*)text;
	while (ptr && *ptr)
	{
		if (*ptr == '\\')
		{
			if (ptr[1] >= '0' && ptr[1] <= '9')
			{
				if (ptr[1] == '0')
					color = diffuse;
				else
				{
					unsigned long c = m_colorset[ptr[1] - '0'];
					color = (diffuse & 0xff000000) |
						((((c & 0xff0000) >> 16) *
								 ((diffuse & 0xff0000) >> 16) >>
							 8)
							<< 16) |
						((((c & 0xff00) >> 8) * ((diffuse & 0xff00) >> 8) >> 8)
							<< 8) |
						(((c & 0xff) * (diffuse & 0xff)) >> 8);
				}
				ptr++;
			}
			else if (ptr[1] == '<')
			{
				char filename[32];
				float xs = 256.0f, ys = 256.0f;
				float scale = 1.0f;
				float ox = 0, oy = 0;
				WRect rect;
				WOverlay* overlay;

				int n = strchr(ptr, '>') - ptr - 2;
				memcpy(temp, ptr + 2, n);
				temp[n] = 0;
				ptr += n + 2;

				char* p = strstr(temp, "src=");
				if (p)
					sscanf(p + 4, "%s", filename);

				p = strstr(temp, "size=");
				if (p)
					sscanf(p + 5, "%f %f", &xs, &ys);

				p = strstr(temp, "rect=");
				if (p)
					sscanf(p + 5, "%f %f %f %f", &rect.x, &rect.y, &rect.w,
						&rect.h);
				else
				{
					rect.x = rect.y = 0;
					rect.w = xs;
					rect.h = ys;
				}

				p = strstr(temp, "scale=");
				if (p)
					sscanf(p + 6, "%f", &scale);

				p = strstr(temp, "offset=");
				if (p)
				{
					sscanf(p + 7, "%f %f", &ox, &oy);
					if ((GetCoordMode() & 0x100) && view)
					{
						ox = view->GetWidth() / 640.0f * ox;
						oy = view->GetHeight() / 480.0f * oy;
					}
				}

				m_useOverlay = true;
				overlay = m_overlayList.Find(filename);
				if (!overlay)
				{
					overlay = GetResrcManager()->GetOverlay(filename, 0);
					m_overlayList.AddItem(overlay, filename, true);
				}

				if (draw && overlay)
				{
					overlay->SetCoordMode(0x1200);
					overlay->Render(view,
						WRect(rect.x / xs, rect.y / ys, rect.w / xs,
							rect.h / ys),
						WRect(x + len + ox, y + oy, rect.w * scale,
							rect.h * scale),
						type, color, 0.0f, 0);
				}
			}
			ptr++;
		}
		else
		{
			int n = strlen(ptr);
			next = 0;
			for (int i = 0; i < n; i++)
			{
				if (ptr[i] == '\\')
				{
					next = ptr + i;
					break;
				}
				if (ptr[i] & 0x80)
					i++;
			}

			if (next)
			{
				memcpy(temp, ptr, next - ptr);
				temp[next - ptr] = 0;
				len += draw
					? PrintInside(view, x + len, y, temp, type, color, bmp)
					: GetTextWidthInside(view, temp);
			}
			else
				len += draw
					? PrintInside(view, x + len, y, ptr, type, color, bmp)
					: GetTextWidthInside(view, ptr);

			ptr = next;
		}
	}

	return (((GetCoordMode() & 0x1000) && view) ? 640.0f / view->GetWidth()
												: 1.0f) *
		len;
}

float WFont::Print(WView* view, float x, float y, const char* text, int type,
	unsigned long diffuse, Bitmap* bmp)
{
	return PrintOut(view, x, y, text, type, diffuse, true, bmp);
}

float WFont::GetTextWidth(WView* view, const char* text)
{
	return PrintOut(view, 0, 0, text, 0, 0, false, 0);
}

void WFont::ResetOverlay(void)
{
	for (WOverlay* overlay = m_overlayList.Start(); overlay;
		overlay = m_overlayList.Next())
		GetResrcManager()->Release(overlay);

	m_overlayList.Reset();
}

void WFont::Flush(WView* view)
{
	if (!m_useOverlay && m_overlayList.Start())
		ResetOverlay();

	m_useOverlay = false;
}

WTexFont::WTexFont(const char* filename)
{
	if (filename)
		WOverlay::Load(filename, 0x800);
}

WTexFont::~WTexFont(void)
{
}

int WTexFont::Load(const char* filename, int flag)
{
	return WOverlay::Load(filename, flag | 0x800);
}

float WTexFont::GetTextWidthInside(WView* view, const char* pText)
{
	return GetFontScale(view) * strlen(pText) * 8.0f;
}

float WTexFont::PrintInside(WView* view, float x, float y, const char* text,
	int type, unsigned long diffuse, Bitmap* bmp)
{
	float len = 0;

	int i = 0;
	int c;
	while ((c = text[i]) != 0 && i < 1024)
	{
		float sx = (c % 16) * 0.0625f;
		float sy = (c / 16) * 0.125f;

		DrawTexture(view, GetTexhandle(), WRect(sx, sy, 0.0625f, 0.125f),
			WRect(x, y, GetFontScale(view) * 8.0f, GetFontScale(view) * 16.0f),
			type, diffuse, 0.0f, 0, false);

		x += GetFontScale(view) * 8.0f;
		len += GetFontScale(view) * 8.0f;
		i++;
	}

	return len;
}

WTitleFont::WTitleFont(void)
{
	m_iTexIndex = 0;
	m_fWidthIndex = 0;
	m_pCharSet = 0;
	m_numTex = 0;
}

WTitleFont::~WTitleFont(void)
{
	Erase();
}

WTitleFont::WTitleFont(tagWTITLEFONT* info)
{
	Create(info);
}

void WTitleFont::Create(tagWTITLEFONT* info)
{
	int i, n, x, y;
	Bitmap* bitmap;
	int maxw;

	Erase();

	m_info = *info;

	int numWidth = m_info.texw / m_info.fontw;
	int numHeight = m_info.texh / m_info.fonth;

	m_numCharSet = m_info.numPages * numHeight * numWidth;
	m_numWidth = numWidth;
	m_numPerPage = numHeight * numWidth;
	m_numTex = m_info.numPages;
	m_pCharSet = info->pCharSet;

	m_iTexIndex = new int[m_info.numPages];
	m_fWidthIndex = new float[m_numCharSet];

	for (i = 0, n = 0; i < info->numPages; i++)
	{
		m_iTexIndex[i] = 0;
		bitmap = GetResrcManager()->LoadBitmap(info->filename[i], 0, false);
		if (bitmap)
		{
			char* ext = strrchr(info->filename[i], '.');
			if (!strcmpi(ext, ".tga"))
				m_iTexIndex[i] = GetResrcManager()->LoadTexture(
					info->filename[i], 0x80080000, 0, 0);
			else
				m_iTexIndex[i] = GetResrcManager()->LoadTexture(
					info->filename[i], 0x80000, 0, 0);

			for (y = 0; y <= m_info.texh - m_info.fonth; y += m_info.fonth)
			{
				for (x = 0; x <= m_info.texw - m_info.fontw; x += m_info.fontw)
				{
					maxw = 0;
					for (int j = y; j < y + m_info.fonth; j++)
					{
						unsigned char* line = bitmap->vram + bitmap->pitch * j;
						for (int k = x + maxw; k < x + m_info.fontw; k++)
						{
							int v;
							if (bitmap->bi->bmiHeader.biBitCount == 8)
								v = line[k];
							else if (bitmap->bi->bmiHeader.biBitCount == 32)
								v = line[k * 4 + 3];
							else
								v = (line[k * 3] + line[k * 3 + 1] +
										line[k * 3 + 2]) /
									3;

							if (v > 50 && k - x > maxw)
								maxw = k - x;
						}
					}
					m_fWidthIndex[n++] = (float)maxw;
				}
			}

			m_info.texw = GetResrcManager()->VideoReference()->GetTextureWidth(
				m_iTexIndex[i]);
			m_info.texh = GetResrcManager()->VideoReference()->GetTextureHeight(
				m_iTexIndex[i]);

			delete bitmap;
		}
	}
}

void WTitleFont::Erase(void)
{
	if (m_iTexIndex)
	{
		for (int i = 0; i < m_numTex; i++)
		{
			if (m_iTexIndex[i])
			{
				GetResrcManager()->Release(m_iTexIndex[i]);
				m_iTexIndex[i] = 0;
			}
		}
	}

	if (m_iTexIndex)
	{
		delete[] m_iTexIndex;
		m_iTexIndex = 0;
	}
	if (m_fWidthIndex)
	{
		delete[] m_fWidthIndex;
		m_fWidthIndex = 0;
	}
	m_pCharSet = 0;
}

float WTitleFont::PutChar(WView* view, float x, float y, const int nChar,
	int type, unsigned long diffuse)
{
	_WRECT s;

	int page = nChar / m_numPerPage;
	int idx = nChar % m_numPerPage;
	int col = idx % m_numWidth;
	int row = idx / m_numWidth;

	s.x = (m_info.fontw * col) / (float)m_info.texw;
	s.y = (m_info.fonth * row) / (float)m_info.texh;
	s.w = m_info.fontw / (float)m_info.texw;
	s.h = m_info.fonth / (float)m_info.texh;

	DrawTexture(view, m_iTexIndex[page], s,
		WRect(x, y, m_info.fontw * GetFontScale(view),
			m_info.fonth * GetFontScale(view)),
		type, diffuse, 0.0f, 0, true);

	return GetFontScale(view) * m_fWidthIndex[nChar];
}

float WTitleFont::GetCharWidth(WView* view, const int nChar)
{
	return GetFontScale(view) * m_fWidthIndex[nChar];
}

float WTitleFont::PrintInside(WView* view, float x, float y, const char* pText,
	int type, unsigned long diffuse, Bitmap* bmp)
{
	float xpos = x;
	float w = 0;
	const char* ptr = pText;
	char c;

	while ((c = *ptr++) != 0)
	{
		char* p = strchr(m_pCharSet, c);
		if (p)
		{
			int idx = p - m_pCharSet;
			w = PutChar(view, xpos, y, idx, type, diffuse);
		}
		else if (c == ' ')
			w = m_info.fontw * 0.5f;

		xpos += w;
	}

	return xpos - x;
}

float WTitleFont::GetTextWidthInside(WView* view, const char* pText)
{
	float width = 0;
	const char* ptr = pText;
	char c;

	while ((c = *ptr++) != 0)
	{
		char* p = strchr(m_pCharSet, c);
		if (p)
			width += m_fWidthIndex[p - m_pCharSet];
		else if (c == ' ')
			width += m_info.fontw * 0.5f;
	}

	return GetFontScale(view) * width;
}

WFntFont::WFntFont(unsigned char* ptr)
{
	m_bFullEnglish = false;
	m_bFixedWidth = true;
	m_cacheLen = 0;

	for (int i = 0; i < 8; i++)
	{
		m_bitmap[i] = 0;
		m_cachedHandle[i] = 0;
		m_updateFrame[i] = 0;
	}

	memset(m_cached, 0, sizeof(m_cached));
	memset(m_cachedFontWidth, 0, sizeof(m_cachedFontWidth));

	m_font = ptr;
	m_clone = ptr != 0;
}

WFntFont::~WFntFont(void)
{
	Release();

	if (m_font && !m_clone)
	{
		delete[] m_font;
		m_font = 0;
	}
}

WFont* WFntFont::MakeClone(void)
{
	return new WFntFont(m_font);
}

void WFntFont::Release(void)
{
	int i;

	for (i = 0; i < 8; i++)
	{
		if (m_cachedHandle[i] > 0)
			GetResrcManager()->Release(m_cachedHandle[i]);

		if (m_bitmap[i])
			delete m_bitmap[i];

		m_cachedHandle[i] = 0;
		m_bitmap[i] = 0;
		m_updateFrame[i] = 0;
	}

	m_cacheLen = 0;
}

int WFntFont::Load(const char* fntName)
{
	cFile* file = GetResrcManager()->GetCFile(fntName, 0xffff);

	if (file)
	{
		int len = file->m_nLen;
		if (m_font)
		{
			delete[] m_font;
			m_font = 0;
		}
		m_font = new unsigned char[len];
		file->Read(m_font, len);
		CloseCFile(file);

		return 0;
	}

	return 1;
}

void WFntFont::Flush(WView* view)
{
	if (m_cacheLen / 8 >= 15)
		Release();

	WFont::Flush(view);
}

int WFntFont::GetFont(unsigned short code, unsigned char* ptr, int width)
{
	int maxWidth = 1;
	unsigned char* font;

	if (m_bFullEnglish && code < 0x7f)
		code += 0xa380;

	if (code >= 0x20 && code < 0x7f)
		font = m_font + (code - 0x20) * 32;
	else if (code >= 0xa1a0 && code <= 0xacff)
		font = m_font +
			(((unsigned char)(code >> 8) - 0xa0) * 96 + (unsigned char)code -
				0xa0) *
				32;
	else if (code >= 0xb0a0 && code <= 0xc8ff)
		font = m_font +
			(((unsigned char)(code >> 8) - 0xa3) * 96 + (unsigned char)code -
				0xa0) *
				32;
	else
		font = m_font;

	for (int y = 0; y < 16; y++)
	{
		unsigned short bits = (font[0] << 8) | font[1];

		for (int x = 0; x < 16; x++)
		{
			if (bits & (1 << (16 - x)))
			{
				ptr[x] = 0xff;
				if (maxWidth < x)
					maxWidth = x;
			}
			else
				ptr[x] = 0;
		}

		ptr += width;
		font += 2;
	}

	return maxWidth + 4;
}

void WFntFont::FlushFont(void)
{
	for (int i = 0; i < 8; i++)
	{
		if (m_updateFrame[i])
		{
			if (m_cachedHandle[i] > 0)
				GetResrcManager()->FixTexture(m_cachedHandle[i], m_bitmap[i], 0,
					0);
			else
				m_cachedHandle[i] =
					GetResrcManager()->UploadTexture(0, m_bitmap[i], 0, 0);

			m_updateFrame[i] = 0;
		}
	}
}

int WFntFont::AddCache(unsigned short code)
{
	int idx = m_cacheLen;
	m_cached[idx] = code;

	int page = GetCacheNum(idx);
	if (!m_bitmap[page])
	{
		m_bitmap[page] = new Bitmap(64, 64, 8);
		m_bitmap[page]->SetGrayPalette(0);
		memset(m_bitmap[page]->vram, 0,
			m_bitmap[page]->bi->bmiHeader.biHeight * m_bitmap[page]->pitch);
	}

	int n = idx - page * 16;
	int y = n / 4;
	m_cachedFontWidth[idx] =
		GetFont(code, m_bitmap[page]->vram + (y * 64 + n % 4) * 16, 64);
	m_updateFrame[page] = 1;

	m_cacheLen = (m_cacheLen + 1) % 128;

	return idx;
}

int WFntFont::GetFontIdx(unsigned short code)
{
	for (int i = 0; i <= m_cacheLen; i++)
		if (code == m_cached[i])
			return i;

	return AddCache(code);
}

int WFntFont::PutChar(WView* view, unsigned short code, float x, float y,
	unsigned long type, unsigned long diffuse, int bGetWidth)
{
	if (code == ' ')
		return 10;

	int idx = GetFontIdx(code);
	int width = m_cachedFontWidth[idx];

	if (bGetWidth)
		return width;

	int page = GetCacheNum(idx);
	int n = idx - page * 16;
	float dx = n % 4 * 16;
	float dy = n / 4 * 16;
	WRect s(dx / 64.0f, dy / 64.0f, 0.25f, 0.25f);

	WRect d(x, y, GetFontScale(view) * 16.0f, GetFontScale(view) * 16.0f);

	DrawTexture(view, m_cachedHandle[page], s, d, type, diffuse, 0.0f, 0,
		false);

	return width;
}

float WFntFont::PrintInside(WView* view, float x, float y, const char* text,
	int type, unsigned long diffuse, Bitmap* bmp)
{
	int width;
	float fixedw;
	float oldx = x;

	for (int i = 0; text[i];)
	{
		if (text[i] & 0x80)
		{
			GetFontIdx(
				((unsigned char)text[i] << 8) | (unsigned char)text[i + 1]);
			i += 2;
		}
		else
		{
			if (text[i] != ' ')
				GetFontIdx(text[i]);
			i++;
		}
	}

	FlushFont();

	char c;
	while ((c = *text++) != 0)
	{
		unsigned short code;
		if (c & 0x80)
		{
			code = ((unsigned char)c << 8) | (unsigned char)*text++;
			fixedw = 12.0f;
		}
		else
		{
			code = (unsigned char)c;
			fixedw = 6.0f;
		}

		width = PutChar(view, code, x, y, type, diffuse, 0);
		x += m_bFixedWidth ? GetFontScale(view) * fixedw
						   : width * GetFontScale(view) - 3.0f;
	}

	return x - oldx;
}

float WFntFont::GetTextWidthInside(WView* view, const char* text)
{
	int width;
	float fixedw;
	float x = 0;
	char c;

	while ((c = *text++) != 0)
	{
		unsigned short code;
		if (c & 0x80)
		{
			code = ((unsigned char)c << 8) | (unsigned char)*text++;
			fixedw = GetFontScale(view) * 12.0f;
		}
		else
		{
			code = (unsigned char)c;
			fixedw = GetFontScale(view) * 6.0f;
		}

		width = code == ' ' ? 10 : m_cachedFontWidth[GetFontIdx(code)];
		x += m_bFixedWidth ? fixedw : width * GetFontScale(view) - 3.0f;
	}

	return x;
}

WFixedFont::WFixedFont(int maxTexture)
	: m_textList(8, 8)
{
	Init(maxTexture);
}

WFixedFont::WFixedFont(int w, int h, int alias, unsigned char* ptr,
	int maxTexture)
	: m_textList(8, 8)
{
	Init(maxTexture);

	m_iFontWidth = w;
	m_iFontHeight = h;

	int bpp = alias ? 4 : 1;
	m_bytePerChar = (bpp * w + 7) / 8 * h + 2;
	m_bAntialiased = alias;

	m_font = ptr;
	m_clone = true;
}

WFixedFont::~WFixedFont(void)
{
	Clear(true);

	if (!m_clone && m_font)
	{
		delete[] m_font;
		m_font = 0;
	}

	for (int i = 0; i < m_nMaxTexture; i++)
	{
		if (m_bitmap[i])
			delete m_bitmap[i];
	}

	if (m_update)
	{
		delete[] m_update;
		m_update = 0;
	}
	if (m_texIndex)
	{
		delete[] m_texIndex;
		m_texIndex = 0;
	}
	if (m_bitmap)
	{
		delete[] m_bitmap;
		m_bitmap = 0;
	}
}

WFont* WFixedFont::MakeClone(void)
{
	return new WFixedFont(m_iFontWidth, m_iFontHeight, m_bAntialiased, m_font,
		m_nMaxTexture);
}

void WFixedFont::Init(int maxTexture)
{
	m_font = 0;
	m_clone = false;
	m_eType = NORMAL;
	m_bFullEnglish = false;
	m_bFixedWidth = false;
	m_bAntialiased = 0;
	m_update_flag = false;
	m_nMaxTexture = maxTexture;
	m_texIndex = new int[maxTexture];
	m_bitmap = new Bitmap*[maxTexture];
	m_update = new bool[maxTexture];

	m_flush_num = 0;

	for (int i = 0; i < maxTexture; i++)
	{
		m_bitmap[i] = 0;
		m_texIndex[i] = 0;
		m_update[i] = false;
	}

	m_iFontWidth = m_iFontHeight = m_nMaxWidth = m_bytePerChar = 0;
	m_nowX = m_nowY = 0;
}

int WFixedFont::Load(const char* fntName)
{
	w_fnthead hd;

	cFile* file = GetResrcManager()->GetCFile(fntName, 0xffff);
	if (!file)
		return 1;

	int len = file->m_nLen - sizeof(w_fnthead);
	if (m_font)
	{
		delete[] m_font;
		m_font = 0;
	}
	m_font = new unsigned char[len];
	file->Read(&hd, sizeof(w_fnthead));
	file->Read(m_font, len);

	CloseCFile(file);

	m_iFontWidth = hd.fontsize;
	m_iFontHeight = hd.fontsize;

	int bpp = hd.bAntialiased ? 4 : 1;

	m_bytePerChar = (bpp * hd.fontsize + 7) / 8 * hd.fontsize + 2;

	m_bAntialiased = hd.bAntialiased;

	return 0;
}

unsigned char* WFixedFont::GetFontAddr(unsigned short code, int* width)
{
	if (code < 0x20)
		code = '$';

	unsigned char* p = m_font + (code - 0x20) * m_bytePerChar;

	*width = *(unsigned short*)(p + m_bytePerChar - 2);

	return p;
}

void WFixedFont::Write2Buffer(unsigned char* font, unsigned char* buff, int len)
{
	unsigned char* ptr = buff;
	int x, y;

	if (m_bAntialiased)
	{
		for (y = 0; y < m_iFontHeight; y++, buff += len - x)
		{
			for (x = 0; x < m_iFontWidth; x += 2, buff++)
			{
				unsigned char c = *font++;
				if (c)
				{
					*buff = c & 0xf0;
					buff++;
					*buff = c << 4;
				}
				else
				{
					*buff++ = 0;
					*buff = 0;
				}
			}
		}
	}
	else
	{
		for (y = 0; y < m_iFontHeight; y++, buff += len - x)
		{
			for (x = 0; x < m_iFontWidth; x += 8)
			{
				unsigned char c = *font++;
				if (c)
				{
					for (int b = 7; b >= 0; b--)
						*buff++ = (c & (1 << b)) ? 0xff : 0;
				}
				else
				{
					memset(buff, 0, 8);
					buff += 8;
				}
			}
		}
	}

	if (m_eType == BOLD)
	{
		static int offset[9][2] = {
			{ 0,  0  },
			{ 0,  -1 },
			{ -1, 0  },
			{ 1,  0  },
			{ 0,  1  },
			{ -1, -1 },
			{ 1,  -1 },
			{ -1, 1  },
			{ 1,  1  },
		};
		unsigned char temp[64 * 64];

		for (y = 0; y < m_iFontHeight; y++)
		{
			for (x = 0; x < m_iFontWidth; x++)
			{
				int sum = 0;
				for (int i = 0; i < sizeof(offset) / sizeof(offset[0]); i++)
					if (offset[i][0] + x >= 0 &&
						offset[i][0] + x < m_iFontWidth &&
						offset[i][1] + y >= 0 &&
						offset[i][1] + y < m_iFontHeight)
						sum += ptr[(offset[i][1] + y) * len + offset[i][0] + x];
				temp[y * 64 + x] = Min(255, sum);
			}
		}

		for (y = 0; y < m_iFontHeight; y++)
			for (x = 0; x < m_iFontWidth; x++)
				ptr[y * len + x] = temp[y * 64 + x];
	}
}

int WFixedFont::GetTexture(const char* text)
{
	int width = 0;
	int x = 0;
	int i;
	const char* p = text;
	char c;

	while ((c = *p++) != 0)
	{
		unsigned short code;
		if (c & 0x80)
			code = ((unsigned char)c << 8) | (unsigned char)*p++;
		else
			code = (unsigned char)c;

		if (code == ' ')
			width += m_iFontWidth * 14 / 36;
		else
		{
			int w;
			unsigned char* font = GetFontAddr(code, &w);
			Write2Buffer(font, m_mid + x, 1024);
			x += m_iFontWidth;

			if (c & 0x80)
				width += m_bFixedWidth ? m_iFontWidth : m_space + w - 1;
			else
				width += m_bFixedWidth ? m_iFontWidth : m_space + w;
		}

		if (x < width)
		{
			for (i = 0; i < m_iFontHeight; i++)
				memset(m_mid + x + i * 1024, 0, width - x);
		}
		x = width;
	}

	return width;
}

void WFixedFont::CreateBitmap(int i)
{
	m_bitmap[i] = new Bitmap;

	m_bitmap[i]->Create(256, 256, 8);
	m_bitmap[i]->SetGrayPalette(0);
	memset(m_bitmap[i]->vram, 0, 256 * 256);
}

int WFixedFont::WriteToBitmap(int width, unsigned char* mid)
{
	int srcx = 0;

	m_update_flag = true;

	do
	{
		int nPage = m_nowY / 256;
		int y = m_nowY % 256;

		int len = 255 - m_nowX;
		if (len > width)
			len = width;

		if (!m_bitmap[nPage])
			CreateBitmap(nPage);
		m_update[nPage] = true;

		unsigned char* src = mid + srcx;
		unsigned char* dst = m_bitmap[nPage]->vram + y * 256 + m_nowX;

		for (int ny = 0; ny < m_iFontHeight; ny++)
		{
			memcpy(dst, src, len);

			src += 1024;
			dst += 256;
		}

		m_nowX += len;
		if (m_nowX >= 255)
		{
			m_nowY += m_iFontHeight;
			m_nowX = 0;

			if (m_iFontHeight + m_nowY % 256 > 256)
			{
				m_nowY = m_nowY - m_nowY % 256 + 256;

				if (m_nowY / 256 >= m_nMaxTexture)
				{
					ReArrange();

					return -1;
				}
			}
		}

		width -= len;
		srcx += len;
	} while (width > 0);

	return m_nowY;
}

WFixedFont::w_fixedtext* WFixedFont::GetNewPart(const char* text)
{
	if (text)
	{
		w_fixedtext* f;
		int px, py, width;

		do
		{
			f = m_textList.Find(text);
			if (f)
				break;

			px = m_nowX;
			py = m_nowY;
			width = GetTexture(text);
		} while (WriteToBitmap(width, m_mid) < 0);

		if (!f)
		{
			f = (w_fixedtext*)new char[sizeof(w_fixedtext) + strlen(text)];
			strcpy(f->msg, text);
			f->width = width;
			f->px = px;
			f->py = py;
			m_textList.AddItem(f, f->msg, false);
		}

		return f;
	}

	return 0;
}

void WFixedFont::AddOutArea(w_fixedtext* tex, float x, float y, int type,
	unsigned long diffuse)
{
	w_flush_area* area = &m_flush_list[m_flush_num++];
	area->x = x;
	area->y = y;
	area->tex = tex;
	area->type = type;
	area->diffuse = diffuse;
}

void WFixedFont::Flush(WView* view)
{
	Update();

	for (int i = 0; i < m_flush_num; i++)
	{
		w_flush_area* area = &m_flush_list[i];
		int px = area->tex->px;
		int py = area->tex->py % 256;
		int left = area->tex->width;
		float x = area->x;
		float y = area->y;
		int nPage = area->tex->py / 256;

		do
		{
			int width;
			if (left + px > 255)
				width = 255 - px;
			else
				width = left;

			WRect s(px / 256.0f, py / 256.0f, width / 256.0f,
				m_iFontHeight / 256.0f);
			WRect d(x, y, width * GetFontScale(view),
				m_iFontHeight * GetFontScale(view));

			DrawTexture(view, m_texIndex[nPage], s, d, area->type,
				area->diffuse, 0.0f, 0, true);

			px += width;
			x += width * GetFontScale(view);

			if (px >= 255)
			{
				px = 0;
				py += m_iFontHeight;
				if (m_iFontHeight + py > 256)
				{
					nPage++;
					py = 0;
				}
			}

			left -= width;
		} while (left > 0);
	}

	m_flush_num = 0;

	WFont::Flush(view);
}

float WFixedFont::PrintInside(WView* view, float x, float y, const char* text,
	int type, unsigned long diffuse, Bitmap* bmp)
{
	w_fixedtext* f = GetNewPart(text);

	if (bmp)
		PrintInTex(f, x, y, bmp, type, diffuse);
	else
	{
		if (m_flush_num + 1 >= sizeof(m_flush_list) / sizeof(m_flush_list[0]))
			Flush(view);

		AddOutArea(f, x, y, type, diffuse);
	}

	return f->width * GetScale();
}

void WFixedFont::PrintInTex(w_fixedtext* f, float x, float y, Bitmap* bmp,
	int type, unsigned long diffuse)
{
	unsigned char* src;
	int srcBypp;
	tagRGBQUAD rgb;
	int width;
	int height;
	int iy;
	int len;
	int nPage;
	int nowY;
	int ix;
	unsigned short rgbIdx;
	unsigned char alpha;
	int nowX;

	width = f->width;
	nowX = f->px;
	nowY = f->py;
	srcBypp = bmp->BitsPerPixel() >> 3;
	rgbIdx = 0x100;
	rgb.rgbBlue = 0;
	rgb.rgbGreen = 0;
	rgb.rgbRed = 0;
	alpha = (unsigned char)(diffuse >> 24);
	ix = (int)x;
	iy = (int)y;

	do
	{
		nPage = nowY / 256;
		int py = nowY % 256;

		len = 255 - nowX;
		height = m_iFontHeight;

		if (len > width)
			len = width;
		width -= len;

		if (ix < 0)
		{
			nowX -= ix;
			len += ix;
			ix = 0;
		}
		if (ix + len > bmp->bi->bmiHeader.biWidth)
			len = bmp->bi->bmiHeader.biWidth - ix;

		if (iy < 0)
		{
			py -= iy;
			height += iy;
			iy = 0;
		}
		if (iy + height > bmp->bi->bmiHeader.biHeight)
			height = bmp->bi->bmiHeader.biHeight - iy;

		src = m_bitmap[nPage]->vram + py * 256 + nowX;
		unsigned char* dst =
			bmp->vram + bmp->BitsPerPixel() * ix / 8 + bmp->pitch * iy;

		for (int j = 0; j < height; j++)
		{
			for (int i = 0; i < len; i++)
			{
				if (*src)
				{
					if (rgbIdx != *src)
					{
						rgbIdx = *src;
						rgb.rgbBlue =
							m_bitmap[nPage]->bi->bmiColors[*src].rgbBlue *
							(unsigned char)diffuse;
						rgb.rgbGreen =
							m_bitmap[nPage]->bi->bmiColors[*src].rgbGreen *
							(unsigned char)(diffuse >> 8);
						rgb.rgbRed =
							m_bitmap[nPage]->bi->bmiColors[*src].rgbRed *
							(unsigned char)(diffuse >> 16);
					}
					dst[0] = rgb.rgbBlue * alpha + dst[0] * (255 - alpha);
					dst[1] = rgb.rgbGreen * alpha + dst[1] * (255 - alpha);
					dst[2] = rgb.rgbRed * alpha + dst[2] * (255 - alpha);
				}
				src++;
				dst += srcBypp;
			}
			src += 256 - len;
			dst += bmp->pitch - len * srcBypp;
		}

		nowX += len;
		ix += len;
		if (nowX >= 255)
		{
			nowX = 0;
			nowY += m_iFontHeight;
			if (m_iFontHeight + nowY % 256 > 256)
				nowY += 256 - nowY % 256;
		}
	} while (width > 0);
}

float WFixedFont::GetTextWidthInside(WView* view, const char* text)
{
	const char* ptr = text;
	float width = 0;
	char c;

	while ((c = *ptr++) != 0)
	{
		char wide = c & 0x80;
		unsigned short code;
		if (wide)
			code = ((unsigned char)c << 8) | (unsigned char)*ptr++;
		else
			code = (unsigned char)c;

		if (m_bFixedWidth)
			width += m_iFontWidth;
		else if (code == ' ')
			width += m_iFontWidth * 14 / 36;
		else
		{
			int w;
			GetFontAddr(code, &w);
			if (wide)
				width += m_space + w - 1;
			else
				width += m_space + w;
		}
	}

	return ((GetCoordMode() & 0x1000) && view)
		? GetFontScale(view) * (view->GetWidth() * width / 640.0f)
		: GetFontScale(view) * width;
}

void WFixedFont::Update(void)
{
	unsigned long flag = 0;

	if (!m_update_flag)
		return;

	m_update_flag = false;

	for (int i = 0; i <= m_nowY / 256; i++)
	{
		if (m_update[i] == true)
		{
			m_update[i] = false;

			if (m_eType == MASKED)
			{
				flag = 0x800;
				m_bitmap[i]->bi->bmiColors[0].rgbBlue = 255;
				m_bitmap[i]->bi->bmiColors[0].rgbGreen = 0;
				m_bitmap[i]->bi->bmiColors[0].rgbRed = 0;
			}

			if (m_texIndex[i] > 0)
				GetResrcManager()->FixTexture(m_texIndex[i], m_bitmap[i], flag,
					0);
			else
				m_texIndex[i] =
					GetResrcManager()->UploadTexture(0, m_bitmap[i], flag, 0);
		}
	}
}

void WFixedFont::Reset(void)
{
	Clear(true);
}

void WFixedFont::ReArrange(void)
{
	WList<w_temp_pair*> list;
	int i;

	for (i = 0; i < m_flush_num; i++)
	{
		w_temp_pair* pair;
		for (pair = list.Start(); pair; pair = list.Next())
			if (pair->tex == m_flush_list[i].tex)
				break;

		if (!pair)
		{
			w_temp_pair* np =
				(w_temp_pair*)new char[strlen(m_flush_list[i].tex->msg) + 12];
			np->tex = m_flush_list[i].tex;
			strcpy(np->msg, m_flush_list[i].tex->msg);
			list += np;
		}
	}

	Clear(true);

	w_temp_pair* p;
	for (p = list.Start(); p; p = list.Next())
		p->newone = GetNewPart(p->msg);

	for (i = 0; i < m_flush_num; i++)
	{
		w_temp_pair* q;
		for (q = list.Start(); q->tex != m_flush_list[i].tex; q = list.Next())
			;
		m_flush_list[i].tex = q->newone;
	}

	for (p = list.Start(); p; p = list.Next())
		delete[] p;
}

void WFixedFont::Clear(bool flag)
{
	for (w_fixedtext* f = m_textList.Start(); f; f = m_textList.Next())
		delete[] f;
	m_textList.Reset();

	for (int i = 0; i < m_nMaxTexture; i++)
	{
		if (m_bitmap[i])
		{
			delete m_bitmap[i];
			m_bitmap[i] = 0;
		}
		if (m_texIndex[i] > 0)
		{
			GetResrcManager()->Release(m_texIndex[i]);
			m_texIndex[i] = 0;
		}
		m_update[i] = false;
	}

	m_nowX = m_nowY = 0;
}

#include "wmemblock.inl"
