#include "wresrcmng.h"
#include "wvideo.h"
#include "wview.h"
#include "woverlay.h"
#include "wfont.h"
#include "textout.h"
#include "wpuppet.h"
#include "wxpuppet.h"
#include "w3dspr.h"
#include "w3danispr.h"
#include "bitmap.h"
#include "westpak.h"
#include "xunzip.h"
#include "ijl.h"
#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTANCE_LOCK(type) WInstanceLock __instanceLock##type(&m_lock[type])

png_const_charp msg;
static png_structp png_ptr = NULL;
static png_infop info_ptr = NULL;

WResourceManager::WResourceManager()
	: m_missingTextureList(8, 8), m_matchList(1024, 1024), texList(64, 64)
{
	video = 0;
	audio = 0;
	remove("files_of_the_same_name.txt");

	m_matchDirectory = false;
	m_blankTexture = 0;
	m_nByteUsedTexture = 0;

	for (int i = 0; i < 3; i++)
		m_savemem[i] = false;
}

WResourceManager::~WResourceManager()
{
	if (m_blankTexture)
		video->DestroyTexture(m_blankTexture);

	for (WFont* font = fontList.FirstResource(); font;
		font = fontList.NextResource())
	{
		if (fontList.DeleteResource(font) == true)
			delete font;
	}

	for (WPuppet* puppet = puppetList.FirstResource(); puppet;
		puppet = puppetList.NextResource())
	{
		if (puppetList.DeleteResource(puppet) == true)
			delete puppet;
	}

	for (WResource* resrc = originList.Start(); resrc;
		resrc = originList.Next())
		delete resrc;

	do
	{
		w_texlist* tex = texList.Start();
		if (!tex)
			break;

		Release(tex->texhandle);
	} while (1);

	ReleaseMatchList();

	for (char* text = m_missingTextureList.Start(); text;
		text = m_missingTextureList.Next())
	{
		if (text)
			delete[] text;
	}
	m_missingTextureList.Reset();
}

void WResourceManager::Lock(rmlType_t type)
{
	m_lock[type].Lock();
}

void WResourceManager::Unlock(rmlType_t type)
{
	m_lock[type].Unlock();
}

void WResourceManager::UnlockAllByThread(unsigned long threadID)
{
	for (int i = 0; i < 3; i++)
		m_lock[i].UnlockInThread(threadID);
}

void WResourceManager::SetUseLessVideoRam(int level)
{
	m_savemem[0] = false;
	m_savemem[1] = level > 0;
	m_savemem[2] = level > 1;
}

void WResourceManager::Release(WResource* resrc)
{
	delete resrc;
}

void WResourceManager::AddOriginList(WResource* resrc)
{
	originList += resrc;
}

bool WResourceManager::IsOrigin(WResource* resrc)
{
	for (WResource* origin = originList.Start(); origin;
		origin = originList.Next())
		if (origin == resrc)
			return true;
	return false;
}

template <class T>
static void ReleaseIdleOrigin(WResrcCache<T>* list,
	WList<WResource*>* originList, int cnt)
{
	char next[128];
	next[0] = 0;

	char* idle;
	while ((idle = list->FindIdleOrigin(next[0] ? next : 0, cnt)) != 0)
	{
		strcpy(next, idle);

		T resrc = list->FindOrigin(idle);
		list->ClearOrigin(idle);
		*originList -= resrc;
		delete resrc;
	}
}

void WResourceManager::ReleaseIdleResources(int cnt)
{
	ReleaseIdleOrigin(&puppetList, &originList, cnt);

	ReleaseIdleOrigin(&fontList, &originList, cnt);
}

WView* WResourceManager::GetView(const char* type)
{
	WView* view;

	if (type && strstr(type, "rth"))
		view = new WViewOrth;
	else
		view = new WView;

	view->SetResourceManager(this);
	view->SetViewport((float)video->GetWidth(), (float)video->GetHeight());

	return view;
}

static void ExtractFilename(char* out, const char* filename)
{
	strcpy(out, filename);
	char* p = strchr(out, '\\');
	while (p)
	{
		*p = '/';
		p = strchr(out, '\\');
	}
}

WPuppet* WResourceManager::GetPuppet(const char* f, bool alphatest,
	bool bNeedTnLBuf, bool bForceLegacy)
{
	char filename[260];
	char name[260];
	WPuppet* origin = 0;
	int type;

	INSTANCE_LOCK(RML_PUPPET_LOADING);

	ExtractFilename(filename, f);
	strcpy(name,
		strrchr(filename, '/') ? strrchr(filename, '/') + 1 : filename);

	origin = puppetList.FindOrigin(name);

	char* ext = strrchr(name, '.');
	if (!strcmpi(ext, ".pet"))
		type = 0xff;
	else if (!strcmpi(ext, ".mpet"))
		type = 0x11b;
	else if (!strcmpi(ext, ".apet"))
		type = 0x66;
	else if (!strcmpi(ext, ".bpet"))
		type = 0x82;
	else
		return 0;

	if (origin)
	{
		if (type == 0x66 || type == 0x11b)
		{
			puppetList.AddResource(name, origin);
			return origin;
		}

		WPuppet* puppet = origin->MakeClone(bNeedTnLBuf);
		puppet->SetResourceManager(this);
		puppet->xBuildTransfMatPtrList();
		puppet->SetAlpha(0xff);
		puppetList.AddResource(name, puppet);
		return puppet;
	}

	ulong dwCaps = 0;
	video->Command(WX_VDEV_GET_CAPS, 1, (int)&dwCaps);

	WPuppet* puppet;
	if (bForceLegacy || !dwCaps || type == 0x66 || type == 0x11b)
		puppet = new WPuppet;
	else
		puppet = new WxPuppet(bNeedTnLBuf);

	puppet->SetResourceManager(this);
	if (!puppet->LoadPET(filename, alphatest, type))
	{
		puppet->SetAlpha(0xff);
		puppetList.AddResource(name, puppet);
		AddOriginList(puppet);
		return puppet;
	}

	delete puppet;
	return 0;
}

void WResourceManager::Release(WPuppet* puppet, bool bForceDeleteOrigin)
{
	if (!puppet)
		return;

	if (puppetList.DeleteResource(puppet) == true)
	{
		delete puppet;
		return;
	}

	if (bForceDeleteOrigin == false)
		return;

	originList -= puppet;
	puppetList.ClearOrigin(puppet->GetPuppetName());

	delete puppet;
}

void WResourceManager::ReleaseAllPets()
{
	for (WPuppet* puppet = puppetList.FirstResource(); puppet;
		puppet = puppetList.NextResource())
	{
		if (puppetList.DeleteResource(puppet) == true)
			delete puppet;
	}

	for (WPuppet* puppet = puppetList.FirstOrigin(); puppet;
		puppet = puppetList.NextOrigin())
	{
		originList -= puppet;
		puppetList.ClearOrigin(puppet->GetPuppetName());
		delete puppet;
	}
}

WOverlay* WResourceManager::GetOverlay(const char* filename, unsigned long flag)
{
	WOverlay* overlay = new WOverlay;
	overlay->SetResourceManager(this);

	if (filename)
	{
		if (overlay->Load(filename, flag))
		{
			delete overlay;
			return 0;
		}
	}

	return overlay;
}

WOverlay* WResourceManager::GetOverlay(const char* name, Bitmap* bitmap,
	unsigned long flag)
{
	WOverlay* overlay = new WOverlay;
	overlay->SetResourceManager(this);

	if (name)
	{
		if (overlay->Load(name, bitmap, flag))
		{
			delete overlay;
			return 0;
		}
	}

	return overlay;
}

WTitleFont* WResourceManager::GetTitleFont()
{
	WTitleFont* font = new WTitleFont;
	font->SetResourceManager(this);
	return font;
}

void WResourceManager::Release(WTitleFont* font)
{
	delete font;
}

WFont* WResourceManager::GetTrueFont(const char* filename, int fontsize,
	int fontindex)
{
	WFont* font = GetFntClone(filename);
	if (font)
		return font;

	CTextOut* textout = new CTextOut(0);
	textout->SetResourceManager(this);
	if (textout->Load(filename, fontsize, fontindex, true) == true)
	{
		if (fontList.AddResource(filename, textout) == true)
			AddOriginList(textout);
		return textout;
	}

	return 0;
}

WFont* WResourceManager::GetFntClone(const char* filename)
{
	WFont* origin = fontList.FindOrigin(filename);
	if (origin)
	{
		WFont* font = origin->MakeClone();
		if (font)
		{
			font->SetResourceManager(this);
			fontList.AddResource(filename, font);
			return font;
		}
	}
	return 0;
}

WFont* WResourceManager::GetTexFont(const char* filename)
{
	WFont* font = GetFntClone(filename);
	if (!font)
	{
		WTexFont* texfont = new WTexFont(0);
		texfont->SetResourceManager(this);
		texfont->Load(filename, 0);
		if (fontList.AddResource(filename, texfont) == true)
			AddOriginList(texfont);
		font = texfont;
	}
	return font;
}

WFont* WResourceManager::GetFntFont(const char* filename)
{
	WFont* font = GetFntClone(filename);
	if (font)
		return font;

	WFntFont* fntfont = new WFntFont(0);
	fntfont->SetResourceManager(this);
	if (fntfont->Load(filename))
	{
		delete fntfont;
		return 0;
	}
	if (fontList.AddResource(filename, fntfont) == true)
		AddOriginList(fntfont);
	return fntfont;
}

WFont* WResourceManager::GetFixedFont(const char* filename)
{
	WFont* font = GetFntClone(filename);
	if (font)
		return font;

	WFixedFont* fixedfont = new WFixedFont(8);
	fixedfont->SetResourceManager(this);
	if (fixedfont->Load(filename))
	{
		delete fixedfont;
		return 0;
	}
	if (fontList.AddResource(filename, fixedfont) == true)
		AddOriginList(fixedfont);
	return fixedfont;
}

void WResourceManager::Release(WFont* font)
{
	if (fontList.DeleteResource(font) == true)
		delete font;
}

W3dSpr* WResourceManager::Get3DSpr(const char* filename, int type)
{
	W3dSpr* spr = new W3dSpr;
	spr->SetResourceManager(this);
	if (!spr->LoadSprite(filename, type))
		return spr;

	delete spr;
	return 0;
}

W3dAniSpr* WResourceManager::Get3DAniSpr(const char* filename, int type,
	float fSprSizeX, float fSprSizeY)
{
	W3dAniSpr* spr = new W3dAniSpr;
	spr->SetResourceManager(this);
	if (!spr->LoadSpritesInOneTexture(filename, type, fSprSizeX, fSprSizeY))
		return spr;

	delete spr;
	return 0;
}

w_texlist* WResourceManager::FindTexture(const char* texName)
{
	char name[64];
	const char* p = strchr(texName, '/') ? strrchr(texName, '/') + 1 : texName;
	StrcpyLower(name, p);
	return texList.Find(name);
}

w_texlist* WResourceManager::FindTexture(int texHandle)
{
	for (w_texlist* tex = texList.Start(); tex; tex = texList.Next())
		if (tex->texhandle == texHandle)
			return tex;
	return 0;
}

int WResourceManager::GetTextureWidth(int texHandle)
{
	for (w_texlist* tex = texList.Start(); tex; tex = texList.Next())
		if (tex->texhandle == texHandle)
			return tex->width;
	return 0;
}

int WResourceManager::GetTextureHeight(int texHandle)
{
	for (w_texlist* tex = texList.Start(); tex; tex = texList.Next())
		if (tex->texhandle == texHandle)
			return tex->height;
	return 0;
}

void WResourceManager::AddMissingTexture(const char* filename)
{
	if (!m_missingTextureList.Find(filename))
	{
		char* text = new char[strlen(filename) + 1];
		strcpy(text, filename);
		m_missingTextureList.AddItem(text, text, false);
	}
}

w_texlist* WResourceManager::AddTexture(const char* texturename, int texhandle,
	int width, int height)
{
	char name[64];
	const char* p =
		strchr(texturename, '/') ? strrchr(texturename, '/') + 1 : texturename;
	StrcpyLower(name, p);

	w_texlist* tex = (w_texlist*)new char[strlen(name) + sizeof(w_texlist)];
	tex->count = 1;
	tex->texhandle = texhandle;
	tex->width = width;
	tex->height = height;
	tex->size = 0;
	strcpy(tex->texname, name);
	texList.AddItem(tex, tex->texname, false);
	return tex;
}

int WResourceManager::UploadTexture(const char* texName, Bitmap* bitmap,
	unsigned long type, RECT* rect)
{
	if (texName)
	{
		w_texlist* tex = FindTexture(texName);
		if (tex)
		{
			tex->count++;
			return tex->texhandle;
		}
	}

	int texhandle;
	int width;
	int height;

	if (!rect)
	{
		width = bitmap->Width();
		height = bitmap->Height();
		texhandle = video->CreateTexture(bitmap->bi, type);
		if (!(type & 0x1001f000))
			video->UpdateTexture(texhandle, bitmap->bi, bitmap->vram, type);
	}
	else
	{
		Bitmap temp;
		temp.Create(rect->right - rect->left + 1, rect->bottom - rect->top + 1,
			bitmap->bi->bmiHeader.biBitCount);

		int bpp = (bitmap->BitsPerPixel() + 7) / 8;
		if (bpp <= 1)
		{
			int num = bitmap->bi->bmiHeader.biClrUsed
				? bitmap->bi->bmiHeader.biClrUsed
				: 256;
			for (int i = 0; i < num; i++)
			{
				temp.bi->bmiColors[i].rgbRed = bitmap->bi->bmiColors[i].rgbRed;
				temp.bi->bmiColors[i].rgbGreen =
					bitmap->bi->bmiColors[i].rgbGreen;
				temp.bi->bmiColors[i].rgbBlue =
					bitmap->bi->bmiColors[i].rgbBlue;
			}
		}

		int offset = 0;
		for (int y = rect->top; y <= rect->bottom; y++)
		{
			memcpy(temp.vram + offset,
				bitmap->vram + bitmap->pitch * y + rect->left * bpp,
				(rect->right - rect->left + 1) * bpp);
			offset += temp.pitch;
		}

		width = temp.bi->bmiHeader.biWidth;
		height = temp.bi->bmiHeader.biHeight;
		texhandle = video->CreateTexture(temp.bi, type);
		video->UpdateTexture(texhandle, temp.bi, temp.vram, type);
	}

	const char* name = texName;
	if (!texName || rect)
		name = "Unknown";

	w_texlist* tex = AddTexture(name, texhandle, width, height);
	tex->size = ((type >> 31) | 2) * height * width;
	if (type & 0x40000000)
		tex->size += tex->size / 2;
	m_nByteUsedTexture += tex->size;
	if (!tex->texhandle)
		AddMissingTexture(tex->texname);
	return tex->texhandle;
}

int WResourceManager::GetBlankTexture()
{
	if (!m_blankTexture)
	{
		Bitmap blank;
		blank.Create(32, 32, 24);
		memset(blank.vram, 0xff, blank.bi->bmiHeader.biHeight * blank.pitch);
		m_blankTexture = video->CreateTexture(blank.bi, 0);
		video->UpdateTexture(m_blankTexture, blank.bi, blank.vram, 0);
	}
	return m_blankTexture;
}

void WResourceManager::FixTexture(int texHandle, Bitmap* bitmap,
	unsigned long type, RECT* rect)
{
	if (!rect)
		video->UpdateTexture(texHandle, bitmap->bi, bitmap->vram, type);
	else
		video->FixTexturePart(texHandle, *rect, bitmap->bi, bitmap->vram, type);
}

void WResourceManager::Release(int texhandle)
{
	w_texlist* tex = FindTexture(texhandle);
	if (tex)
	{
		if (--tex->count <= 0)
		{
			m_nByteUsedTexture -= tex->size;
			video->DestroyTexture(texhandle);
			texList -= tex;
			delete[] (char*)tex;
		}
	}
}

int WResourceManager::LoadTexture(const char* filename, unsigned long type,
	int level, const char* texname)
{
	int texhandle = 0;

	INSTANCE_LOCK(RML_TEXTURE_LOADING);

	if (!texname || !stricmp(filename, texname))
	{
		w_texlist* tex = FindTexture(filename);
		if (tex)
		{
			tex->count++;
			return tex->texhandle;
		}
	}

	Bitmap* bitmap = LoadBitmap(filename, level, false);
	if (bitmap)
	{
		bitmap->Update();
		if (strchr(filename, '{'))
		{
			type &= ~0x40000000;
			type |= 0x800;
		}
		if (strchr(filename, '!'))
			type |= 0x20000000;
		CheckMaskTexture(&bitmap, filename);
		texhandle =
			UploadTexture(texname ? texname : filename, bitmap, type, 0);
		delete bitmap;
	}
	else
	{
		char afname[260];
		strcpy(afname, filename);
		strlwr(afname);
		if (strstr(afname, ".dds"))
		{
			cFile* fp = ::GetCFile(FindMatchFile(filename), 0xffff, 0);
			if (!fp)
			{
				AddMissingTexture(filename);
				return 0;
			}
			int len = fp->Length();
			unsigned char* buff = new unsigned char[len];
			fp->Read(buff, len);
			CloseCFile(fp);
			texhandle = video->UploadCompressedTexture(buff, len, type);
			AddTexture(texname ? texname : filename, texhandle,
				video->GetTextureWidth(texhandle),
				video->GetTextureHeight(texhandle));
			delete[] buff;
		}
	}

	return texhandle;
}

int WResourceManager::LoadTexture_DDS(unsigned char* dds, unsigned int size,
	const char* filename, int type, int level)
{
	INSTANCE_LOCK(RML_TEXTURE_LOADING);

	int texhandle = video->UploadCompressedTexture(dds, size, type);
	if (!texhandle)
		AddMissingTexture(filename);
	AddTexture(filename, texhandle, video->GetTextureWidth(texhandle),
		video->GetTextureHeight(texhandle));
	return texhandle;
}

int WResourceManager::LoadTexture(Bitmap* bitmap, const char* filename,
	int type, int level)
{
	INSTANCE_LOCK(RML_TEXTURE_LOADING);

	w_texlist* tex = FindTexture(filename);
	if (tex)
	{
		tex->count++;
		return tex->texhandle;
	}

	bitmap->Update();
	if (strchr(filename, '{'))
	{
		type &= ~0x40000000;
		type |= 0x800;
	}
	if (strchr(filename, '!'))
		type |= 0x20000000;
	CheckMaskTexture(&bitmap, filename);
	return UploadTexture(filename, bitmap, type, 0);
}

void WResourceManager::ReloadTextures()
{
	INSTANCE_LOCK(RML_TEXTURE_LOADING);

	for (w_texlist* tex = texList.Start(); tex; tex = texList.Next())
	{
		Bitmap* bitmap = LoadBitmap(tex->texname, 0, false);
		if (bitmap)
		{
			bitmap->Update();
			CheckMaskTexture(&bitmap, tex->texname);
			video->UpdateTexture(tex->texhandle, bitmap->bi, bitmap->vram, 0);
			delete bitmap;
		}
		else
		{
			char filename[260];
			strcpy(filename, tex->texname);
			strlwr(filename);
			if (strstr(filename, ".dds"))
			{
				unsigned long type = 0;
				if (strchr(tex->texname, '{'))
					type = 0x800;
				if (strchr(tex->texname, '!'))
					type |= 0x20000000;
				cFile* fp = ::GetCFile(FindMatchFile(tex->texname), 0xffff, 0);
				if (fp)
				{
					int len = fp->Length();
					unsigned char* buff = new unsigned char[len];
					fp->Read(buff, len);
					CloseCFile(fp);
					video->UpdateCompressedTexture(tex->texhandle, buff, len,
						type);
					delete[] buff;
				}
			}
		}
	}
}

bool WResourceManager::CheckMaskTexture(Bitmap** ppBitmap, const char* filename)
{
	char afname[260];

	if (!ppBitmap || !*ppBitmap || !filename || !*filename)
		return false;

	if (strchr(filename, '[') || strchr(filename, '+'))
	{
		strcpy(afname, filename);
		sprintf(strrchr(afname, '.'), "_mask%s", strrchr(filename, '.'));
		Bitmap* mask = LoadBitmap(afname, 0, false);
		if (mask)
		{
			if (mask->bi->bmiHeader.biBitCount > 8)
				MessageBox(0,
					"\xb8\xb6\xbd\xba\xc5\xa9 \xc5\xd8\xbd\xba\xc3\xe7\xb4\xc2 "
					"8\xba\xf1\xc6\xae \xb1\xd7\xb7\xb9\xc0\xcc\xb8\xa6 "
					"\xc0\xcc\xbf\xeb\xc7\xd8\xc1\xd6\xbc\xbc\xbf\xe4.\n0.0a",
					filename, MB_OK);
			if (*ppBitmap &&
					(*ppBitmap)->bi->bmiHeader.biWidth !=
						mask->bi->bmiHeader.biWidth ||
				(*ppBitmap)->bi->bmiHeader.biHeight !=
					mask->bi->bmiHeader.biHeight)
				return false;
			Bitmap* bitmap = Combine4Alpha(*ppBitmap, mask);
			delete *ppBitmap;
			delete mask;
			*ppBitmap = bitmap;
			return true;
		}
	}
	return false;
}

bool WResourceManager::ChangeTexturePart(char* dstName, char* srcName,
	Bitmap** ppBmp, RECT* pRc)
{
	w_texlist* tex = FindTexture(dstName);
	if (!tex)
		return false;
	return ChangeTexturePart(tex->texhandle, srcName, ppBmp, pRc);
}

void WResourceManager::ChangeTexturePart(char* dstName, Bitmap* pBmp,
	const RECT& rc)
{
	if (!dstName || !pBmp)
		return;

	w_texlist* tex = FindTexture(dstName);
	if (tex)
		ChangeTexturePart(tex->texhandle, pBmp, rc);
}

bool WResourceManager::ChangeTexturePart(int hDstTex, char* srcName,
	Bitmap** ppBmp, RECT* pRc)
{
	if (!hDstTex)
		return false;

	w_texlist* tex = FindTexture(hDstTex);
	if (!tex)
		return false;

	Bitmap* pSrc = LoadBitmapPart(tex->texname, srcName);
	if (pSrc && tex->width == pSrc->Width() && tex->height == pSrc->Height())
	{
		RECT rc;
		int nFlag = 0;
		if (GetRectInSourceTexture(pSrc->bi, pSrc->vram, rc))
		{
			if (pSrc->bi->bmiHeader.biBitCount == 32)
				nFlag = 0x40000;
			Bitmap* pDst = new Bitmap(rc.right - rc.left, rc.bottom - rc.top,
				pSrc->BitsPerPixel());
			unsigned char* pDstBit = pDst->vram;
			unsigned char* pSrcBit = pSrc->vram + pSrc->pitch * rc.top +
				pSrc->BitsPerPixel() * rc.left / 8;
			int iSize = (rc.right - rc.left) * pSrc->BitsPerPixel() / 8;
			for (int i = 0; i < rc.bottom - rc.top; i++)
			{
				memcpy(pDstBit, pSrcBit, iSize);
				pDstBit += pDst->pitch;
				pSrcBit += pSrc->pitch;
			}
			delete pSrc;
			pSrc = pDst;
			video->FixTexturePart(hDstTex, rc, pSrc->bi, pSrc->vram, nFlag);
		}
		if (ppBmp)
			*ppBmp = pSrc;
		else
			delete pSrc;
		if (pRc)
			*pRc = rc;
		return true;
	}
	return false;
}

void WResourceManager::ChangeTexturePart(int hDstTex, Bitmap* pBmp,
	const RECT& rc)
{
	if (!pBmp)
		return;

	w_texlist* tex = FindTexture(hDstTex);
	if (tex && tex->width >= rc.right && tex->height >= rc.bottom)
	{
		BITMAPINFO* bi = pBmp->bi;
		if ((unsigned)tex->width >= (unsigned)bi->bmiHeader.biWidth &&
			(unsigned)tex->height >= (unsigned)bi->bmiHeader.biHeight)
			video->FixTexturePart(hDstTex, rc, bi, pBmp->vram,
				bi->bmiHeader.biBitCount == 32 ? 0x40000 : 0);
	}
}

bool WResourceManager::GetRectInSourceTexture(const BITMAPINFO* src,
	const void* data, RECT& rc) const
{
	int y;
	int x;
	int cpp = src->bmiHeader.biBitCount / 8;
	int pitch = (src->bmiHeader.biWidth * cpp + 3) & ~3;
	int padbytes = pitch - src->bmiHeader.biWidth * cpp;
	const unsigned char* p = (const unsigned char*)data;

	if (src->bmiHeader.biBitCount == 8)
	{
		for (y = 0; y < src->bmiHeader.biHeight; y++, p += padbytes)
		{
			for (x = 0; x < src->bmiHeader.biWidth; x++, p++)
			{
				if (*p != *(const unsigned char*)data)
					goto GET_RECT_SIZE;
			}
		}
	}
	else if (src->bmiHeader.biBitCount == 24 || src->bmiHeader.biBitCount == 32)
	{
		for (y = 0; y < src->bmiHeader.biHeight; y++, p += padbytes)
		{
			for (x = 0; x < src->bmiHeader.biWidth; x++, p += cpp)
			{
				if ((*(const unsigned int*)p & 0xffffff) != 0xff00)
					goto GET_RECT_SIZE;
			}
		}
	}
	return false;

GET_RECT_SIZE:
	int left = x;
	int top = y;
	int right, bottom;
	if (src->bmiHeader.biBitCount == 8)
	{
		unsigned char head = *(const unsigned char*)data;
		right = x + 1;
		const unsigned char* q =
			(const unsigned char*)data + top * pitch + right;
		while (right < src->bmiHeader.biWidth && *q != head)
		{
			right++;
			q++;
		}
		bottom = top + 1;
		q = (const unsigned char*)data + bottom * pitch + left;
		while (bottom < src->bmiHeader.biHeight && *q != head)
		{
			q += pitch;
			bottom++;
		}
	}
	else
	{
		right = x + 1;
		const unsigned char* q =
			(const unsigned char*)data + top * pitch + right * cpp;
		while (right < src->bmiHeader.biWidth &&
			(*(const unsigned int*)q & 0xffffff) != 0xff00)
		{
			right++;
			q += cpp;
		}
		bottom = top + 1;
		q = (const unsigned char*)data + bottom * pitch + left * cpp;
		while (bottom < src->bmiHeader.biHeight &&
			(*(const unsigned int*)q & 0xffffff) != 0xff00)
		{
			q += pitch;
			bottom++;
		}
	}

	int width = right - left;
	int height = bottom - top;
	if (width < 1 || height < 1)
		return false;

	rc.left = left;
	rc.top = top;
	rc.right = left + width;
	rc.bottom = top + height;
	return true;
}

Bitmap* WResourceManager::LoadBitmap(const char* filename, int level, bool bNet)
{
	Bitmap* bitmap = 0;

	INSTANCE_LOCK(RML_BITMAP_LOADING);

	const char* extension = strrchr(filename, '.');
	const char* inname = strrchr(filename, '?');
	if (inname)
	{
		char temp[260] = "";
		memcpy(temp, filename, inname - filename);
		bitmap = LoadBRES(inname + 1, temp);
	}
	else if (extension)
	{
		if (!strcmpi(extension, ".bmp"))
			bitmap = LoadBMP(filename);
		else if (!strcmpi(extension, ".jpg"))
			bitmap = LoadJPG(filename);
		else if (!strcmpi(extension, ".png"))
			bitmap = LoadPNG(filename, false, bNet);
		else if (!strcmpi(extension, ".tga"))
			bitmap = LoadTGA(filename);

		if (!bitmap &&
			(!strcmpi(extension, ".jpg") || !strcmpi(extension, ".bmp")))
		{
			char temp[256];
			strcpy(temp, filename);
			strcpy(strrchr(temp, '.'), ".png");
			bitmap = LoadPNG(temp, false, false);
		}
	}

	if (bitmap)
	{
		if (bitmap->Width() > 8 && bitmap->Height() > 8 && m_savemem[level])
		{
			Bitmap* quarter = MakeQuarterBitmap(bitmap);
			delete bitmap;
			bitmap = quarter;
		}
	}

	if (level == 0 && bitmap)
		bitmap->Update();

	return bitmap;
}

Bitmap* WResourceManager::LoadBitmapPart(char* dstName, char* srcName)
{
	Bitmap* pSrc;

	if (*dstName == '[' || *dstName == '+')
	{
		pSrc = LoadBitmap(srcName, 0, false);
		if (pSrc == 0)
			return 0;
		pSrc->Update();
		if (!CheckMaskTexture(&pSrc, srcName) &&
			!CheckMaskTexture(&pSrc, dstName))
		{
			delete pSrc;
			return 0;
		}
	}
	else
	{
		pSrc = LoadBitmap(srcName, 0, false);
		if (pSrc == 0)
			return 0;
	}
	return pSrc;
}

Bitmap* WResourceManager::MakeQuarterBitmap(Bitmap* bitmap)
{
	bitmap->Update();

	Bitmap* quarter = new Bitmap(bitmap->Width() / 2, bitmap->Height() / 2,
		bitmap->BitsPerPixel());

	if (bitmap->BitsPerPixel() <= 8)
		memcpy(quarter->bi->bmiColors, bitmap->bi->bmiColors,
			256 * sizeof(RGBQUAD));

	for (unsigned y = 0; y < quarter->Height(); y++)
	{
		for (unsigned x = 0; x < quarter->Width(); x++)
		{
			unsigned u = bitmap->Width() * x / quarter->Width();
			unsigned v = bitmap->Height() * y / quarter->Height();
			if (bitmap->BitsPerPixel() <= 8)
				quarter->vram[quarter->pitch * y + x] =
					bitmap->vram[bitmap->pitch * v + u];
			else
				memcpy(quarter->vram + quarter->pitch * y + x * 3,
					bitmap->vram + bitmap->pitch * v + u * 3, 3);
		}
	}
	return quarter;
}

Bitmap* WResourceManager::Combine4Alpha(Bitmap* bitmap, Bitmap* alpha)
{
	bitmap->Update();
	alpha->Update();

	Bitmap* result = new Bitmap;
	result->Create(bitmap->Width(), bitmap->Height(), 32);

	for (unsigned y = 0; y < bitmap->Height(); y++)
	{
		for (unsigned x = 0; x < bitmap->Width(); x++)
		{
			unsigned char r, g, b, a;
			if (bitmap->BitsPerPixel() <= 8)
			{
				r = bitmap->bi->bmiColors[bitmap->vram[bitmap->pitch * y + x]]
						.rgbRed;
				g = bitmap->bi->bmiColors[bitmap->vram[bitmap->pitch * y + x]]
						.rgbGreen;
				b = bitmap->bi->bmiColors[bitmap->vram[bitmap->pitch * y + x]]
						.rgbBlue;
			}
			else
			{
				r = bitmap->vram[bitmap->pitch * y + x * 3 + 2];
				g = bitmap->vram[bitmap->pitch * y + x * 3 + 1];
				b = bitmap->vram[bitmap->pitch * y + x * 3];
			}
			if (alpha->BitsPerPixel() <= 8)
				a = alpha->bi->bmiColors[alpha->vram[alpha->pitch * y + x]]
						.rgbBlue;
			else
				a = alpha->vram[alpha->pitch * y +
					x * (alpha->BitsPerPixel() / 8)];

			result->vram[result->pitch * y + x * 4] = b;
			result->vram[result->pitch * y + x * 4 + 1] = g;
			result->vram[result->pitch * y + x * 4 + 2] = r;
			result->vram[result->pitch * y + x * 4 + 3] = a;
		}
	}
	return result;
}

Bitmap* WResourceManager::Combine(Bitmap* dest, Bitmap* src)
{
	Bitmap* result = new Bitmap;
	result->Create(dest->Width(), dest->Height(), 32);

	unsigned char SrcBpp = (unsigned char)(src->BitsPerPixel() / 8);
	unsigned char DestBpp = (unsigned char)(dest->BitsPerPixel() / 8);

	for (unsigned y = 0; y < dest->Height(); y++)
	{
		for (unsigned x = 0; x < dest->Width(); x++)
		{
			unsigned char alpha = src->vram[src->pitch * y + x * SrcBpp + 3];
			for (unsigned char i = 0; i < 3; i++)
			{
				result->vram[result->pitch * y + x * 4 + i] =
					(unsigned char)((dest->vram[dest->pitch * y + x * DestBpp +
										 i] *
											(255 - alpha) +
										src->vram[src->pitch * y + x * SrcBpp +
											i] *
											alpha) /
						255);
			}
			result->vram[result->pitch * y + x * 4 + 3] = 0xff;
		}
	}
	return result;
}

static void PNGAPI png_cexcept_error(png_structp png_ptr, png_const_charp msg)
{
	throw msg;
}

static void PNGAPI png_read_data(png_structp png_ptr, png_bytep data,
	png_size_t length)
{
	cFile* fp = (cFile*)png_ptr->io_ptr;
	if (fp->Read(data, length) != (int)length)
		png_error(png_ptr, "Read Error");
}

static void PNGAPI png_read_data2(png_structp png_ptr, png_bytep data,
	png_size_t length)
{
	png_bytep p = (png_bytep)png_ptr->io_ptr;
	memcpy(data, p, length);
	png_ptr->io_ptr = p + length;
}

Bitmap* WResourceManager::LoadPNG(const char* filename, bool bFromMem,
	bool bNet)
{
	static png_bytepp ppbRowPointers;
	png_byte pngSig[8];
	int iBitDepth;
	int iColorType;
	png_uint_32 iWidth;
	png_uint_32 iHeight;
	cFile* fp = 0;
	const unsigned char* data = 0;
	Bitmap* pBitmap;

	if (!bFromMem)
	{
		fp = ::GetCFile(FindMatchFile(filename), 0xffff, 0);
		if (!fp)
		{
			AddMissingTexture(filename);
			return 0;
		}
	}
	else
		data = (const unsigned char*)filename;

	try
	{
		if (bFromMem)
			memcpy(pngSig, data, 8);
		else
			fp->Read(pngSig, 8);

		if (!png_check_sig(pngSig, 8))
		{
			if (fp)
				CloseCFile(fp);
			return 0;
		}

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
			png_cexcept_error, NULL);
		if (!png_ptr)
		{
			if (fp)
				CloseCFile(fp);
			return 0;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			if (fp)
				CloseCFile(fp);
			return 0;
		}

		if (!bFromMem)
		{
			png_set_read_fn(png_ptr, fp, png_read_data);
			png_set_sig_bytes(png_ptr, 8);
		}
		else
		{
			data += 8;
			png_set_read_fn(png_ptr, (void*)data, png_read_data2);
			png_set_sig_bytes(png_ptr, 8);
		}
		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth,
			&iColorType, NULL, NULL, NULL);

		if (iBitDepth == 16)
			png_set_strip_16(png_ptr);
		if (bNet && info_ptr->pixel_depth == 24 && iBitDepth == 8)
			png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
		if (iColorType == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png_ptr);
		if (iBitDepth < 8)
			png_set_expand(png_ptr);
		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
			png_set_expand(png_ptr);
		if (iColorType == PNG_COLOR_TYPE_GRAY ||
			iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_gray_to_rgb(png_ptr);
		png_set_bgr(png_ptr);

		png_read_update_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth,
			&iColorType, NULL, NULL, NULL);

		png_uint_32 ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
		int iChannels = png_get_channels(png_ptr, info_ptr);

		pBitmap = new Bitmap;
		if (!pBitmap)
		{
			if (fp)
				CloseCFile(fp);
			return 0;
		}
		pBitmap->Create(iWidth, iHeight, iChannels * 8);

		ppbRowPointers =
			(png_bytepp)png_malloc(png_ptr, iHeight * sizeof(png_bytep));
		if (!ppbRowPointers)
			png_error(png_ptr, "Visual PNG: out of memory");

		for (unsigned i = 0; i < iHeight; i++)
			ppbRowPointers[i] = pBitmap->vram + i * ulRowBytes;

		png_read_image(png_ptr, ppbRowPointers);
		png_read_end(png_ptr, NULL);

		free(ppbRowPointers);
		ppbRowPointers = NULL;

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	}
	catch (png_const_charp)
	{
		if (ppbRowPointers)
			free(ppbRowPointers);
		ppbRowPointers = NULL;
		if (fp)
			CloseCFile(fp);
		png_ptr = NULL;
		info_ptr = NULL;
		return 0;
	}
	catch (...)
	{
		png_ptr = NULL;
		info_ptr = NULL;
		return 0;
	}

	if (fp)
		CloseCFile(fp);
	return pBitmap;
}

#pragma pack(push, 1)
struct sTgaHeader
{
	unsigned char id_size;
	unsigned char color_map_type;
	unsigned char iType;
	unsigned short color_map_origin;
	unsigned short color_map_length;
	unsigned char color_map_size;
	short origin_x;
	short origin_y;
	short width;
	short height;
	unsigned char bitcount;
	unsigned char descript;
};
#pragma pack(pop)

Bitmap* WResourceManager::LoadTGA(const char* filename)
{
	Bitmap* bitmap = 0;

	cFile* fp = ::GetCFile(FindMatchFile(filename), 0xffff, 0);
	if (fp)
	{
		sTgaHeader header;
		fp->Read(&header, sizeof(header));
		if (header.iType == 2)
		{
			fp->Seek(header.id_size, 1);
			bitmap = new Bitmap;
			bitmap->Create(header.width, header.height, header.bitcount);
			for (int y = 0; y < header.height; y++)
				fp->Read(bitmap->vram + (header.height - y - 1) * bitmap->pitch,
					header.width * header.bitcount / 8);
		}
		CloseCFile(fp);
	}
	else
		AddMissingTexture(filename);

	return bitmap;
}

Bitmap* WResourceManager::LoadBMP(const char* filename)
{
	cFile* fp = ::GetCFile(FindMatchFile(filename), 0xffff, 0);
	if (!fp)
	{
		AddMissingTexture(filename);
		return 0;
	}

	char header[14];
	fp->Read(header, 14);
	if (!memcmp("header", "bm", 2))
	{
		CloseCFile(fp);
		return 0;
	}

	int offset = *(unsigned int*)&header[10] - 14;
	int len = fp->Length() - fp->Tell();
	BITMAPINFO* bi = (BITMAPINFO*)new unsigned char[len];
	fp->Read(bi, len);

	Bitmap* bitmap = new Bitmap;
	unsigned char* vram = (unsigned char*)bi + offset;
	int bpl = (bi->bmiHeader.biWidth * (bi->bmiHeader.biBitCount / 8) + 3) & ~3;
	bitmap->Create(bi->bmiHeader.biWidth, bi->bmiHeader.biHeight,
		bi->bmiHeader.biBitCount);
	if (bi->bmiHeader.biBitCount <= 8)
	{
		int num = bi->bmiHeader.biClrUsed;
		if (!num)
			num = 1 << bi->bmiHeader.biBitCount;
		memcpy(bitmap->bi->bmiColors, bi->bmiColors, num * sizeof(RGBQUAD));
	}
	for (int y = 0; y < bi->bmiHeader.biHeight; y++)
		memcpy(bitmap->vram + y * bitmap->pitch,
			vram + (bi->bmiHeader.biHeight - y - 1) * bpl, bitmap->pitch);

	delete[] bi;
	CloseCFile(fp);
	return bitmap;
}

Bitmap* WResourceManager::LoadBRES(const char* zipname, const char* filename)
{
	ZIPENTRY ze;

	cFile* fp = ::GetCFile(FindMatchFile(zipname), -1, 0);
	if (!fp)
		return 0;

	int len = fp->Length();
	unsigned char* buff = new unsigned char[len];
	fp->Read(buff, len);
	CloseCFile(fp);

	HZIP hz = OpenZipU(buff, len, ZIP_MEMORY);
	if (!hz)
	{
		delete[] buff;
		return 0;
	}

	GetZipItemA(hz, -1, &ze);
	int numItems = ze.index;
	for (int i = 0; i < numItems; i++)
	{
		GetZipItemA(hz, i, &ze);
		if (!strcmpi(ze.name, filename))
		{
			unsigned char* item = new unsigned char[ze.unc_size];
			UnzipItem(hz, i, item, ze.unc_size, ZIP_MEMORY);

			char* ext = strrchr(ze.name, '.');
			Bitmap* pBitmap;
			unsigned char* vram;
			unsigned height;
			unsigned bpl;
			if (!stricmp(ext, ".bmp"))
			{
				vram = item + 0x36;
				pBitmap = new Bitmap(*(int*)(item + 0x12), *(int*)(item + 0x16),
					*(short*)(item + 0x1c));
				height = *(int*)(item + 0x16);
				bpl = pBitmap->pitch;
			}
			else if (!stricmp(ext, ".tga"))
			{
				vram = item + 0x12;
				pBitmap = new Bitmap(*(short*)(item + 0xc),
					*(short*)(item + 0xe), item[0x10]);
				height = *(short*)(item + 0xe);
				bpl = item[0x10] * *(short*)(item + 0xc) / 8;
			}
			else
			{
				delete[] item;
				break;
			}

			for (unsigned y = 0; y < height; y++)
				memcpy(pBitmap->vram + pBitmap->pitch * y,
					vram + (height - 1 - y) * bpl, bpl);

			delete[] item;
			CloseZipU(hz);
			delete[] buff;
			return pBitmap;
		}
	}

	CloseZipU(hz);
	delete[] buff;
	return 0;
}

class JpegBitmap : public Bitmap
{
public:
	JpegBitmap(char* data, int len)
		: m_data(data), m_size(len)
	{
	}

	virtual ~JpegBitmap()
	{
		if (m_data)
		{
			delete[] m_data;
			m_data = 0;
		}
	}

	virtual void Update()
	{
		if (m_data)
		{
			if (DecodeImage() == true)
			{
				if (m_data)
				{
					delete[] m_data;
					m_data = 0;
				}
			}
		}
	}

	bool DecodeHeader();

protected:
	bool DecodeImage();

private:
	char* m_data;
	int m_size;
};

bool JpegBitmap::DecodeHeader()
{
	JPEG_CORE_PROPERTIES image;

	memset(&image, 0, sizeof(JPEG_CORE_PROPERTIES));
	if (ijlInit(&image) != IJL_OK)
		return false;

	image.JPGBytes = (unsigned char*)m_data;
	image.JPGSizeBytes = m_size;
	if (ijlRead(&image, IJL_JBUFF_READPARAMS) == IJL_OK)
	{
		bi = (BITMAPINFO*)new unsigned char[sizeof(BITMAPINFOHEADER) +
			(image.JPGChannels == 1 ? 256 * sizeof(RGBQUAD) : 0)];
		bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi->bmiHeader.biPlanes = 1;
		bi->bmiHeader.biCompression = BI_RGB;
		bi->bmiHeader.biSizeImage = 0;
		bi->bmiHeader.biClrUsed = 0;
		bi->bmiHeader.biWidth = image.JPGWidth;
		bi->bmiHeader.biHeight = image.JPGHeight;
		bi->bmiHeader.biBitCount = image.JPGChannels == 1 ? 8 : 24;
		ijlFree(&image);
		return true;
	}

	ijlFree(&image);
	return false;
}

bool JpegBitmap::DecodeImage()
{
	JPEG_CORE_PROPERTIES image;

	memset(&image, 0, sizeof(JPEG_CORE_PROPERTIES));
	if (ijlInit(&image) != IJL_OK)
		return false;

	image.JPGBytes = (unsigned char*)m_data;
	image.JPGSizeBytes = m_size;
	if (ijlRead(&image, IJL_JBUFF_READPARAMS) == IJL_OK)
	{
		Create(image.JPGWidth, image.JPGHeight,
			image.JPGChannels == 1 ? 8 : 24);

		image.DIBBytes = vram;
		image.DIBColor = image.JPGChannels == 1 ? IJL_G : IJL_BGR;
		image.DIBHeight = image.JPGHeight;
		image.DIBWidth = image.JPGWidth;
		image.DIBChannels = image.JPGChannels == 1 ? 1 : 3;
		image.DIBPadBytes = pitch - image.DIBChannels * image.DIBWidth;

		switch (image.JPGChannels)
		{
		case 1:
			image.JPGColor = IJL_G;
			break;
		case 3:
			image.JPGColor = IJL_YCBCR;
			break;
		default:
			image.DIBColor = IJL_OTHER;
			image.JPGColor = IJL_OTHER;
			break;
		}

		if (ijlRead(&image, IJL_JBUFF_READWHOLEIMAGE) == IJL_OK)
		{
			if (image.JPGChannels == 1)
			{
				unsigned char pal[768];
				int i;
				for (i = 0; i < 8; i++)
				{
					pal[i * 3 + 2] = 0;
					pal[i * 3 + 1] = 0;
					pal[i * 3 + 0] = 0;
				}
				for (i = 8; i < 248; i++)
				{
					pal[i * 3 + 2] = (unsigned char)i;
					pal[i * 3 + 1] = (unsigned char)i;
					pal[i * 3 + 0] = (unsigned char)i;
				}
				for (i = 248; i < 256; i++)
				{
					pal[i * 3 + 2] = 0xff;
					pal[i * 3 + 1] = 0xff;
					pal[i * 3 + 0] = 0xff;
				}
				SetPalette(pal);
			}
			ijlFree(&image);
			return true;
		}
	}

	ijlFree(&image);
	return false;
}

Bitmap* WResourceManager::LoadJPG(const char* filename)
{
	cFile* fp = ::GetCFile(FindMatchFile(filename), 0xffff, 0);
	if (!fp)
	{
		AddMissingTexture(filename);
		return 0;
	}

	int len = fp->Length();
	char* data = new char[len];
	fp->Read(data, len);
	CloseCFile(fp);

	JpegBitmap* bitmap = new JpegBitmap(data, len);
	if (bitmap->DecodeHeader() == true)
		return bitmap;

	delete bitmap;
	return 0;
}

void WResourceManager::SetAutoMatchDirectory(const char* directory)
{
	m_matchDirectory = directory != 0;
	ReleaseMatchList();
	LoadMatchList(directory);
}

void WResourceManager::LoadMatchList(const char* directory)
{
	WList<char*> list(512, 512);

	if (directory)
	{
		char name[256];
		strcpy(name, directory);
		char* p = strchr(name, '\\');
		while (p)
		{
			*p = '/';
			p = strchr(name, '\\');
		}
		if (name[0] && name[strlen(name) - 1] != '/')
			strcat(name, "/");

		EnumFileList(&list);
		for (char* filename = list.Start(); filename; filename = list.Next())
		{
			AddMatchList(filename);
			delete[] filename;
		}
	}
}

void WResourceManager::ReleaseMatchList()
{
	for (w_match_filename* n = m_matchList.Start(); n; n = m_matchList.Next())
		g_mem.Free(n);
	m_matchList.Reset();
}

void WResourceManager::LoadMatchCacheList(const char* filename)
{
	char fname[256];

	cFile* fp = ::GetCFile(filename, 0x8000, 0);
	if (fp)
	{
		do
		{
			fp->Scan("%s", fname);
			AddMatchList(fname);
		} while (fp->Tell() < fp->Length());
		CloseCFile(fp);
	}
}

void WResourceManager::SaveMatchCacheList(const char* filename)
{
	FILE* fp = fopen(filename, "wb");
	for (w_match_filename* n = m_matchList.Start(); n; n = m_matchList.Next())
		fprintf(fp, "%s\n", n->fullname);
	fclose(fp);
}

void WResourceManager::UpdateMatchList(const char* directory)
{
	char name[256];
	char filter[256];
	WIN32_FIND_DATA fd;

	strcpy(filter, directory);
	strcat(filter, "*.*");
	HANDLE find = FindFirstFile(filter, &fd);
	if (find != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(fd.cFileName, ".") && strcmp(fd.cFileName, "..") &&
					strcmpi(fd.cFileName, "cvs"))
				{
					sprintf(name, "%s%s/", directory, fd.cFileName);
					UpdateMatchList(name);
				}
			}
			else
			{
				sprintf(name, "%s%s", directory, fd.cFileName);
				AddMatchList(name);
			}
		} while (FindNextFile(find, &fd));
	}
	FindClose(find);
}

const char* WResourceManager::FindMatchFile(const char* filename)
{
	char fname[64];
	fname[0] = 0;

	if (m_matchDirectory)
	{
		const char* p = strrchr(filename, '/');
		if (!p)
			p = strrchr(filename, '\\');
		StrcpyLower(fname, p == 0 ? filename : p + 1);
		w_match_filename* n = m_matchList.Find(fname);
		if (n)
			return n->fullname;
	}
	return filename;
}

cFile* WResourceManager::GetCFile(const char* filename, int len)
{
	return ::GetCFile(FindMatchFile(filename), len, 0);
}

bool WResourceManager::FindFileInZip(const char* zipname, const char* filename)
{
	ZIPENTRY ze;

	cFile* fp = ::GetCFile(FindMatchFile(zipname), -1, 0);
	if (!fp)
		return false;

	int len = fp->Length();
	unsigned char* buff = new unsigned char[len];
	fp->Read(buff, len);
	CloseCFile(fp);

	HZIP hz = OpenZipU(buff, len, ZIP_MEMORY);
	if (hz)
	{
		GetZipItemA(hz, -1, &ze);
		int numItems = ze.index;
		for (int i = 0; i < numItems; i++)
		{
			GetZipItemA(hz, i, &ze);
			if (!strcmpi(ze.name, filename))
			{
				CloseZipU(hz);
				delete[] buff;
				return true;
			}
		}
		CloseZipU(hz);
	}
	delete[] buff;
	return false;
}

void WResourceManager::StrcpyLower(char* out, const char* src)
{
	int i;
	for (i = 0; src[i]; i++)
		out[i] = src[i] + (src[i] >= 'A' && src[i] <= 'Z' ? 'a' - 'A' : 0);
	out[i] = 0;
}

void WResourceManager::AddMatchList(const char* filename)
{
	char fullname[256];
	StrcpyLower(fullname, filename);

	const char* p =
		strrchr(fullname, '/') ? strrchr(fullname, '/') + 1 : fullname;
	if (!m_matchList.Find(p))
	{
		w_match_filename* n =
			(w_match_filename*)g_mem.Alloc((int)strlen(fullname) + 1);
		StrcpyLower(n->fullname, fullname);
		m_matchList.AddItem(n, n->fullname + (p - fullname), false);
	}
}

WxStaticPuppetGrp* WResourceManager::xGetStaticPuppetGrp(int nSPets,
	const char* filename, WPuppet** ppaSPets)
{
	WxStaticPuppetGrp* grp = new WxStaticPuppetGrp;
	grp->SetResourceManager(this);
	if (!grp->xLoad(nSPets, filename, ppaSPets))
		return grp;

	delete grp;
	return 0;
}

__forceinline WView::~WView()
{
}

#include "wmemblock.inl"
