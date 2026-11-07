#include "wmemblock.inl"
#include "textout.h"
#include "freetype2.h"
#include "wresrcmng.h"
#include <mmsystem.h>
#include <string.h>

CTextOut::CTextOut(CRenderer* renderer)
{
	m_pFontSet = 0;
	m_pRenderer = renderer;
	m_iFontSize = 0;
	m_WorkTextureNum = 2;
	m_WorkTextureCnt = 0;
	m_WorkX = 0;
	m_WorkY[0] = 0;
	m_WorkY[1] = 0;
	m_BeginWorkTextureCnt = 0;
	m_bResetTexture = false;
	m_RecentCheckTime = timeGetTime();
	m_WorkImage.Create(0x100, 0x100, 0x20);
	memset(m_WorkImage.vram, 0xff, m_WorkImage.Size());
	for (int y = 0; y < 256; ++y)
		for (int x = 0; x < 256; ++x)
			m_WorkImage.vram[m_WorkImage.pitch * y + x * 4 + 3] = 0;
}

CTextOut::~CTextOut(void)
{
	if (m_pFontSet != 0)
		delete m_pFontSet;
}

bool CTextOut::Load(const char* path, int size, int fontIndex, bool preload)
{
	CFreeType2* font = new CFreeType2;
	if (!font->Load(path, size, fontIndex, preload))
	{
		delete font;
		return false;
	}
	m_pFontSet = font;
	m_iFontSize = size;
	return true;
}

void CTextOut::SetFontSize(int size)
{
	m_iFontSize = size;
}

int CTextOut::GetWidth(const char* text)
{
	return GetTextSet(text, m_iFontSize)->width;
}

int CTextOut::Print(float x, float y, const char* text,
	unsigned long renderState, unsigned long diffuse)
{
	if (text == 0 || text[0] == '\0')
		return 0;
	_TEXTSET* set = GetTextSet(text, m_iFontSize);
	set->lastrendertick = timeGetTime();
	_DISPLAYLIST entry;
	entry.set = set;
	entry.x = x;
	entry.y = y;
	entry.rs = renderState;
	entry.diffuse = diffuse;
	m_DisplayList.push_back(entry);
	return set->width;
}

float CTextOut::PrintInside(WView*, float x, float y, const char* text,
	int renderState, unsigned long diffuse, Bitmap*)
{
	return (float)Print(x, y, text, renderState, diffuse);
}

float CTextOut::GetTextWidthInside(WView*, const char* text)
{
	return (float)GetTextSet(text, m_iFontSize)->width;
}

void CTextOut::Flush(WView* view)
{
	if (m_DisplayList.size() == 0)
		return;
	do
	{
		if (m_bResetTexture == true)
		{
			Reset();
			++m_WorkTextureNum;
			m_bResetTexture = false;
		}
		for (unsigned int index = 0; index < m_DisplayList.size(); ++index)
		{
			if (!m_DisplayList[index].set->valid)
				UpdateText(m_DisplayList[index].set);
		}
	} while (m_bResetTexture == true);

	if (m_bUpdateTexture)
		UpdateTexture();

	for (unsigned int index = 0; index < m_DisplayList.size(); ++index)
	{
		_DISPLAYLIST& item = m_DisplayList[index];
		WRect source;
		WRect target;
		target.x = item.x;
		target.y = item.y;
		for (_IMAGESET* image = item.set->ptr; image != 0; image = image->next)
		{
			source.x = (float)image->x / 256.0f;
			source.y = (float)image->y / 256.0f;
			source.w = (float)image->w / 256.0f;
			source.h = (float)image->h / 256.0f;
			target.w = (float)image->w;
			target.h = (float)image->h;
			DrawTexture(view, m_TextList[image->idx], source, target,
				(int)item.rs, item.diffuse, 0.0f, 0, true);
			target.x += (float)image->w;
		}
	}
	m_DisplayList.clear();
	m_BeginWorkTextureCnt = m_WorkTextureCnt;

	unsigned long now = timeGetTime();
	if (now - m_RecentCheckTime > 5000)
	{
		list::multimap_str<_TEXTSET*>::iterator current = m_TextSet.begin();
		while (current != m_TextSet.end())
		{
			if (now - (*current).second->lastrendertick > 10000)
			{
				_IMAGESET* image = (*current).second->ptr;
				while (image != 0)
				{
					_IMAGESET* old = image;
					image = image->next;
					delete old;
				}
				delete (*current).second;
				m_TextSet.erase(current);
				current = m_TextSet.begin();
				continue;
			}
			current++;
		}
		m_RecentCheckTime = now;
	}
}

CTextOut::_TEXTSET* CTextOut::GetTextSet(const char* text, int fontSize)
{
	list::multimap_str<_TEXTSET*>::iterator match;
	match = m_TextSet.find(text);
	if (match != m_TextSet.end())
	{
		list::multimap_str<_TEXTSET*>::iterator limit =
			m_TextSet.upper_bound(text);
		for (; match != limit; match++)
		{
			if ((*match).second->fntsize == fontSize)
				return (*match).second;
		}
	}

	unsigned long length = strlen(text);
	_TEXTSET* set = (_TEXTSET*)new char[sizeof(_TEXTSET) + length];
	set->valid = false;
	set->width = 0;
	set->fntsize = fontSize;
	set->lastrendertick = timeGetTime();
	set->ptr = 0;
	strcpy(set->text, text);
	UpdateText(set);
	m_TextSet.insert(std::make_pair(set->text, set));
	return set;
}

void CTextOut::UpdateText(_TEXTSET* textset)
{
	_IMAGESET* root = 0;
	_IMAGESET* cur = 0;
	int wlen;
	_IMAGESET set;
	set.idx = m_WorkTextureCnt;
	set.x = m_WorkX;
	set.h = 0;
	int w = 0;
	int h = 0;
	wlen = 0;
	set.y = m_WorkY[0];
	m_pFontSet->ChangFontSize(textset->fntsize);
	const char* text = textset->text;
	for (int i = 0; text[i] != '\0';)
	{
		signed char value = (signed char)text[i];
		if (value & 0x80)
		{
			unsigned short code =
				(unsigned char)value | ((unsigned char)text[i + 1] << 8);
			wchar_t Uni[16];
			MultiByteToWideChar(0, 0, (const char*)&code, 2, Uni, 1);
			code = Uni[0];
			m_pFontSet->Render(code, &w, &h);
			i += 2;
		}
		else
		{
			m_pFontSet->Render((unsigned short)(short)value, &w, &h);
			++i;
		}

		if (m_WorkX + w > 0x100)
		{
			set.w = m_WorkX - set.x;
			if (set.w != 0)
			{
				_IMAGESET* image = new _IMAGESET;
				wlen += set.w;
				*image = set;
				image->next = 0;
				if (cur == 0)
					root = cur = image;
				else
					cur = cur->next = image;
				m_bUpdateTexture = true;
			}
			m_WorkX = 0;
			set.x = 0;
			set.h = 0;
			m_WorkY[0] = m_WorkY[1];
			set.y = m_WorkY[0];
		}

		if (m_WorkY[0] + h > 0x100 || m_WorkY[0] + set.h > 0x100)
		{
			set.w = m_WorkX - set.x;
			if (set.w != 0)
			{
				_IMAGESET* image = new _IMAGESET;
				wlen += set.w;
				*image = set;
				image->next = 0;
				if (cur == 0)
					root = cur = image;
				else
					cur = cur->next = image;
				m_bUpdateTexture = true;
			}
			UpdateTexture();
			UseNextTexture();
			set.idx = m_WorkTextureCnt;
			set.h = 0;
			m_WorkX = 0;
			set.x = 0;
			m_WorkY[1] = 0;
			m_WorkY[0] = 0;
			set.y = 0;
		}

		int pitch = m_WorkImage.pitch;
		m_pFontSet->Write32a(
			m_WorkImage.vram + m_WorkY[0] * pitch + m_WorkX * 4, pitch);
		m_WorkX += w;
		m_bUpdateTexture = true;
		if (h > set.h)
		{
			set.h = h;
			if (h + m_WorkY[0] > m_WorkY[1])
				m_WorkY[1] = h + m_WorkY[0];
		}
	}

	set.w = m_WorkX - set.x;
	if (set.w != 0)
	{
		wlen += set.w;
		_IMAGESET* image = new _IMAGESET;
		*image = set;
		image->next = 0;
		if (cur == 0)
			root = image;
		else
			cur->next = image;
		cur = image;
		m_bUpdateTexture = true;
	}

	_IMAGESET* old = textset->ptr;
	while (old != 0)
	{
		_IMAGESET* image = old;
		old = old->next;
		delete image;
	}
	textset->ptr = root;
	textset->valid = true;
	textset->width = wlen;
	textset->lastrendertick = timeGetTime();
}

void CTextOut::Clear(void)
{
	Reset();
	for (list::multimap_str<_TEXTSET*>::iterator current = m_TextSet.begin();
		current != m_TextSet.end(); current++)
	{
		_TEXTSET* set = (*current).second;
		_IMAGESET* image = set->ptr;
		while (image != 0)
		{
			_IMAGESET* old = image;
			image = image->next;
			delete old;
		}
		delete (*current).second;
	}
	m_TextSet.clear();

	m_DisplayList.clear();

	for (unsigned int index = 0; index < m_TextList.size(); ++index)
		GetResrcManager()->Release(m_TextList[index]);
	m_TextList.clear();
}

void CTextOut::UpdateTexture(void)
{
	if (m_bUpdateTexture == true)
	{
		if (m_WorkTextureCnt >= (int)m_TextList.size())
		{
			int texture = GetResrcManager()->UploadTexture(0, &m_WorkImage,
				0x10000000, 0);
			m_TextList.push_back(texture);
		}
		GetResrcManager()->FixTexture(m_TextList[m_WorkTextureCnt],
			&m_WorkImage, 0, 0);
	}
	m_bUpdateTexture = false;
}

void CTextOut::UseNextTexture(void)
{
	m_WorkTextureCnt = (m_WorkTextureCnt + 1) % m_WorkTextureNum;
	memset(m_WorkImage.vram, 0xff, m_WorkImage.Size());

	for (int y = 0; y < 256; ++y)
		for (int x = 0; x < 256; ++x)
			m_WorkImage.vram[m_WorkImage.pitch * y + x * 4 + 3] = 0;

	list::multimap_str<_TEXTSET*>::iterator current = m_TextSet.begin();
	while (current != m_TextSet.end())
	{
		for (_IMAGESET* image = (*current).second->ptr; image != 0;
			image = image->next)
		{
			if (image->idx == m_WorkTextureCnt)
			{
				(*current).second->valid = false;
				break;
			}
		}
		current++;
	}
	if (m_WorkTextureCnt == m_BeginWorkTextureCnt)
		m_bResetTexture = true;
}

void CTextOut::Reset(void)
{
	list::multimap_str<_TEXTSET*>::iterator current = m_TextSet.begin();
	while (current != m_TextSet.end())
	{
		(*current).second->valid = false;
		current++;
	}
	m_WorkTextureCnt = 0;
	m_WorkX = 0;
	m_WorkY[0] = 0;
	m_WorkY[1] = 0;
	m_bUpdateTexture = false;
	m_BeginWorkTextureCnt = 0;
}
