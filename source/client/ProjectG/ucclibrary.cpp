#include "stdafx.h"
#include "ucclibrary.h"
#include "uccmanager.h"
#include "rankingdlg.h"
#include "netresourcemanager.h"
#include "xzip.h"
#include "xunzip.h"
#include <shlwapi.h>

extern Fresh* g_pFresh;

bool NoDrawArea(const Bitmap*, int, int, int, int);
sRGBA GetAverageColor(const Bitmap*, int, int, int, int, bool);

const char* GetUccTempFoler()
{
	return "capture";
}

bool IsUccClothes(unsigned long typeId)
{
	if (Doc() && ItemManager() && (typeId & 0xfc000000) == 0x08000000)
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
		if (part &&
			(part->Category == 7 || part->Category == 8 || part->Category == 9))
			return true;
	}
	return false;
}

bool IsSameClothes(IFF_STRUCT::sPart* first, IFF_STRUCT::sPart* second)
{
	if (!first || !second)
		return false;
	bool same = strcmpi(first->Data, second->Data) == 0;
	for (int i = 0; i < 3; ++i)
		same = same && !strcmpi(first->Tex[i], second->Tex[i]) &&
			!strcmpi(first->OrgTex[i], second->OrgTex[i]);
	return same;
}

bool IsSameClothes(unsigned long first, unsigned long second)
{
	if (first == second)
		return true;
	if (!Doc() || !ItemManager())
		return false;
	IFF_STRUCT::sPart* a = ItemManager()->FindPart(first);
	IFF_STRUCT::sPart* b = ItemManager()->FindPart(second);
	return IsSameClothes(a, b);
}

void CreateItemThumbnail(const Bitmap* source, const Bitmap* mask,
	Bitmap* thumbnail)
{
	if (!source || !thumbnail)
		return;
	Bitmap resized;
	float scale = 64.0f / Max(source->Width(), source->Height());
	WRect rect;
	rect.w = source->Width() * scale;
	rect.h = source->Height() * scale;
	rect.x = (64.0f - rect.w) / 2;
	rect.y = (84.0f - rect.h) / 2;
	GetResizedImage(source, mask, &resized, (int)rect.w, (int)rect.h);
	thumbnail->Create(64, 84, 32);
	for (int y = 0; y < 84; ++y)
		for (int x = 0; x < 64; ++x)
		{
			if (x < rect.x || x >= rect.x + rect.w || y < rect.y ||
				y >= rect.y + rect.h)
				thumbnail->SetPixel(x, y, 255, 255, 255, 0);
			else
			{
				unsigned char r, g, b, a;
				resized.GetPixel(x - (int)rect.x, y - (int)rect.y, r, g, b, a);
				thumbnail->SetPixel(x, y, r, g, b, a);
			}
		}
}

const Bitmap* GetUccItemIcon(sItemInfo* item)
{
	if (!item)
		return NULL;
	if (item->status & 1)
	{
		sUccClothes* clothes =
			UccManager()->FindClothes(item->tid, item->UccIndex);
		if (clothes && UccManager()->IsDownloadComplete(clothes))
			return clothes->icon;
	}
	if (Doc() && ItemManager())
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
			return g_resrcmng->LoadBitmap(part->c.Icon, 0, 0);
	}
	return NULL;
}

const char* GetUccItemName(sItemInfo* item)
{
	if (!Doc() || !ItemManager())
		return "";
	if (!(item->status & 1))
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
			return part->c.Name;
		return "";
	}
	sUccClothes* clothes = UccManager()->FindClothes(item->tid, item->UccIndex);
	if (!clothes)
		return "";
	if (UccManager()->IsDownloadComplete(clothes))
		return clothes->uccName;
	if (!NetResManager()->IsDownloadRequested(RESOURCE_UCC_CLOTHES, item->tid,
			item->UccIndex))
		return MakeStr("%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0)",
			clothes->uccName);
	return MakeStr("%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xc1\xdf)",
		clothes->uccName);
}

sItemInfo* GetMyItemInfo(unsigned long id)
{
	ITEM_LIST_IT it;
	for (it = Doc()->m_uccItemList.begin(); it != Doc()->m_uccItemList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_uccAztecList.begin(); it != Doc()->m_uccAztecList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_partsList.begin(); it != Doc()->m_partsList.end(); it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_myItemList.begin(); it != Doc()->m_myItemList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_ballList.begin(); it != Doc()->m_ballList.end(); it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_cadItemList.begin(); it != Doc()->m_cadItemList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_setItemList.begin(); it != Doc()->m_setItemList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_cutinList.begin(); it != Doc()->m_cutinList.end(); it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_auxPartList.begin(); it != Doc()->m_auxPartList.end();
		it++)
		if (it->guid == id)
			return &*it;
	for (it = Doc()->m_questDropList.begin();
		it != Doc()->m_questDropList.end(); it++)
		if (it->guid == id)
			return &*it;
	return NULL;
}

sItemInfo* GetMyItemInfo(unsigned long typeId, const char* index,
	unsigned short seq)
{
	ITEM_LIST_IT it;
	std::list<sItemInfo>* items;
	items = &Doc()->m_uccItemList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_uccAztecList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_partsList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_myItemList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_ballList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_cadItemList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_setItemList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_cutinList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_auxPartList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	items = &Doc()->m_questDropList;
	for (it = items->begin(); it != items->end(); it++)
		if (strlen(it->UccIndex) && it->Seq && it->tid == typeId &&
			!strcmp(it->UccIndex, index) && (!seq || it->Seq == seq))
			return &*it;
	return NULL;
}

const Bitmap* GetUccItemIcon(unsigned long id)
{
	if (id == -1)
		return NULL;
	sItemInfo* item = GetMyItemInfo(id);
	if (item && !(item->status & 1) && ItemManager())
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
			return g_pFresh->GetBitmap(part->c.Icon);
	}
	sUccClothes* clothes = UccManager()->FindClothes(id);
	if (clothes && UccManager()->IsDownloadComplete(clothes))
		return clothes->icon;
	return NULL;
}

const char* GetUccItemName(unsigned long id)
{
	if (id != -1)
	{
		sItemInfo* item = GetMyItemInfo(id);
		if (item && !(item->status & 1) && ItemManager())
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
			if (part)
				return part->c.Name;
		}
		sUccClothes* clothes = UccManager()->FindClothes(id);
		if (clothes)
		{
			if (UccManager()->IsDownloadComplete(clothes))
				return clothes->uccName;
			if (!NetResManager()->IsDownloadRequested(RESOURCE_UCC_CLOTHES,
					clothes->typeId, clothes->uccIndex))
				return MakeStr(
					"%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0)",
					clothes->uccName);
			return MakeStr("%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xc1\xdf)",
				clothes->uccName);
		}
	}
	return "";
}

const Bitmap* GetUccItemIcon(unsigned long typeId, const char* index)
{
	if (!typeId || !index || !strlen(index))
		return NULL;
	sItemInfo* item = GetMyItemInfo(typeId, index, 0);
	if (item && !(item->status & 1) && ItemManager())
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
		if (part)
			return g_pFresh->GetBitmap(part->c.Icon);
	}
	sUccClothes* clothes = UccManager()->FindClothes(typeId, index);
	if (clothes && UccManager()->IsDownloadComplete(clothes))
		return clothes->icon;
	if (ItemManager())
	{
		IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
		if (part)
			return g_pFresh->GetBitmap(part->c.Icon);
	}
	return NULL;
}

const char* GetUccItemName(unsigned long typeId, const char* index)
{
	if (typeId && index && strlen(index))
	{
		sItemInfo* item = GetMyItemInfo(typeId, index, 0);
		if (item && !(item->status & 1) && ItemManager())
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(item->tid);
			if (part)
				return part->c.Name;
		}
		sUccClothes* clothes = UccManager()->FindClothes(typeId, index);
		if (clothes)
		{
			if (UccManager()->IsDownloadComplete(clothes))
				return clothes->uccName;
			if (!NetResManager()->IsDownloadRequested(RESOURCE_UCC_CLOTHES,
					clothes->typeId, clothes->uccIndex))
				return MakeStr(
					"%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xbd\xc7\xc6\xd0)",
					clothes->uccName);
			return MakeStr("%s (\xb4\xd9\xbf\xee\xb7\xce\xb5\xe5 \xc1\xdf)",
				clothes->uccName);
		}
		if (ItemManager())
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
			if (part)
				return part->c.Name;
		}
	}
	return "";
}

bool ZipUccClothes(Bitmap* front, Bitmap* back, Bitmap* icon,
	const char* filename)
{
	unsigned int frontSize = front
		? (front->Width() * front->Height() * front->BitsPerPixel()) >> 3
		: 0;
	unsigned int backSize =
		back ? (back->Width() * back->Height() * back->BitsPerPixel()) >> 3 : 0;
	unsigned int iconSize =
		icon ? (icon->Width() * icon->Height() * icon->BitsPerPixel()) >> 3 : 0;
	bool result = true;
	HZIP zip = CreateZipZ((void*)filename, 0, ZIP_FILENAME, 2);
	if (front)
		result = ZipAdd(zip, "front", front->GetVram(0), frontSize,
					 ZIP_MEMORY) == ZR_OK;
	if (back)
		result = result &&
			ZipAdd(zip, "back", back->GetVram(0), backSize, ZIP_MEMORY) ==
				ZR_OK;
	if (icon)
		result = result &&
			ZipAdd(zip, "icon", icon->GetVram(0), iconSize, ZIP_MEMORY) ==
				ZR_OK;
	if (IsZipHandleU(zip))
		CloseZipU(zip);
	else
		CloseZipZ(zip);
	return result;
}

bool UnzipUccClothes(unsigned char* data, unsigned long size, Bitmap* front,
	Bitmap* back, Bitmap* icon, sUccHeader* header)
{
	HZIP zip = OpenZipU(data, size, ZIP_MEMORY);
	bool result = true;
	int index;
	ZIPENTRY entry;
	if (header && FindZipItemA(zip, "header", true, &index, &entry) == ZR_OK)
		UnzipItem(zip, index, header, entry.unc_size, ZIP_MEMORY);
	if (front)
	{
		ZRESULT status = FindZipItemA(zip, "front", true, &index, &entry);
		if (status == ZR_OK)
		{
			if ((front->Width() * front->Height() * front->BitsPerPixel()) >>
					3 !=
				entry.unc_size)
			{
				if (IsZipHandleU(zip))
					CloseZipU(zip);
				else
					CloseZipZ(zip);
				return false;
			}
			UnzipItem(zip, index, front->GetVram(0), entry.unc_size,
				ZIP_MEMORY);
			result = result && status == ZR_OK;
		}
		else
			result = false;
	}
	if (back)
	{
		ZRESULT status = FindZipItemA(zip, "back", true, &index, &entry);
		if (status == ZR_OK)
		{
			if ((back->Width() * back->Height() * back->BitsPerPixel()) >> 3 !=
				entry.unc_size)
			{
				if (IsZipHandleU(zip))
					CloseZipU(zip);
				else
					CloseZipZ(zip);
				return false;
			}
			UnzipItem(zip, index, back->GetVram(0), entry.unc_size, ZIP_MEMORY);
			result = result && status == ZR_OK;
		}
		else
			result = false;
	}
	if (icon)
	{
		ZRESULT status = FindZipItemA(zip, "icon", true, &index, &entry);
		if (status == ZR_OK)
		{
			if ((icon->Width() * icon->Height() * icon->BitsPerPixel()) >> 3 !=
				entry.unc_size)
				result = false;
			UnzipItem(zip, index, icon->GetVram(0), entry.unc_size, ZIP_MEMORY);
			result = result && status == ZR_OK;
		}
		else
			result = false;
	}
	if (IsZipHandleU(zip))
		CloseZipU(zip);
	else
		CloseZipZ(zip);
	return result;
}

bool LoadWarningClothes(unsigned int type, Bitmap* front, Bitmap* back,
	Bitmap* icon)
{
	if (!type || !front || !back || !icon)
		return false;
	char filename[512] = { 0 };
	sprintf(filename, "data/ucc/warning/%d.jpg", type);
	cFile* file = GetCFile(filename, -1, 0);
	if (!file)
		return false;
	unsigned int size = file->Length();
	unsigned char* data = new unsigned char[size];
	file->Read(data, size);
	if (!data)
	{
		delete[] data;
		CloseCFile(file);
		return false;
	}
	CloseCFile(file);
	if (!UnzipUccClothes(data, size, front, back, icon, NULL))
	{
		delete[] data;
		return false;
	}
	delete[] data;
	return true;
}

FrGraphicInterface* GetGDI()
{
	if (!g_pFresh)
		return NULL;
	FrWndManager* manager = g_pFresh->GetManager();
	if (!manager)
		return NULL;
	return manager->GetGDI();
}

void RefreshTexCache(Bitmap& bitmap)
{
	FrGraphicInterface* gdi = GetGDI();
	if (gdi)
		gdi->RefreshTexCache(bitmap);
}

void RefreshTexCache(Bitmap* bitmap)
{
	if (bitmap)
		RefreshTexCache(*bitmap);
}

void ClearTexCache(Bitmap* bitmap)
{
	FrGraphicInterface* gdi = GetGDI();
	if (gdi && bitmap)
		gdi->InvalidateCache(*bitmap);
}

float Power(float value, float exponent)
{
	return exp(log(value) * exponent);
}

std::string Lower(std::string text)
{
	std::transform(text.begin(), text.end(), text.begin(), tolower);
	return text;
}

std::string FilterFilename(std::string filename)
{
	std::string result;
	for (unsigned int i = 0; i < filename.size(); ++i)
	{
		char ch = filename[i];
		if (!strchr("[]{}`~!@#$%^&()-+=\\|<>/?", ch))
			result += ch;
	}
	return result;
}

std::string GetMaskFilename(const char* filename)
{
	std::string name(filename);
	const char* extension = strrchr(filename, '.');
	if (!extension)
		return filename;
	name = name.substr(0, extension - filename);
	name += "_mask";
	name += extension;
	return Lower(name);
}

std::string GenerateTextureFullPath(int side, unsigned long typeId,
	const char* index)
{
	if (side == 0)
		return MakeStr("%s\\%s", GetUccTempFoler(),
			GenerateTextureName_F(typeId, index).c_str());
	else
		return MakeStr("%s\\%s", GetUccTempFoler(),
			GenerateTextureName_B(typeId, index).c_str());
}

std::string GenerateIconFilename(unsigned long typeId, const char* index)
{
	if (!Doc() || !ItemManager())
		return "";
	std::string name;
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
	if (!part)
		return "";
	name = FilterFilename(part->Tex[0]);
	name = name.substr(0, name.rfind('.'));
	name += "_";
	name += index;
	name += "_i.jpg";
	return Lower(name);
}

std::string GenerateTextureName_F(unsigned long typeId, const char* index)
{
	if (!Doc() || !ItemManager())
		return "";
	std::string name;
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
	if (!part)
		return "";
	name = FilterFilename(part->Tex[0]);
	name = name.substr(0, name.rfind('.'));
	name += "_";
	name += index;
	name += ".jpg";
	return Lower(name);
}

std::string GenerateTextureName_B(unsigned long typeId, const char* index)
{
	if (!Doc() || !ItemManager())
		return "";
	std::string name;
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
	if (!part)
		return "";
	name = FilterFilename(part->Tex[1]);
	name = name.substr(0, name.rfind('.'));
	name += "_";
	name += index;
	name += ".jpg";
	return Lower(name);
}

void Convert24to32(Bitmap* source, Bitmap* dest, const Bitmap* mask)
{
	if (!source || !dest)
		return;
	unsigned char r, g, b;
	for (unsigned int y = 0; y < source->Height(); ++y)
		for (unsigned int x = 0; x < source->Width(); ++x)
		{
			dest->SetPixel(x, y, 255, 255, 255, 255);
			if (mask)
			{
				mask->GetPixel(x, y, r, g, b);
				if (r < 5 && g < 5 && b < 5)
					dest->SetPixel(x, y, 255, 255, 255, 0);
			}
		}
}

void Convert32to24(Bitmap* source, Bitmap* dest, const Bitmap* mask)
{
	if (!source || !dest || !mask)
		return;
	dest->Create(source->Width(), dest->Height(), 24);
	sRGBA color, brightness, result;
	for (unsigned int y = 0; y < source->Height(); ++y)
		for (unsigned int x = 0; x < source->Width(); ++x)
		{
			source->GetPixel(x, y, color.r, color.g, color.b);
			mask->GetPixel(x, y, brightness.r, brightness.g, brightness.b);
			SetBrightness(color, brightness, result);
			dest->SetPixel(x, y, result.r, result.g, result.b, 255);
		}
}

void MixColor(sRGBA& first, sRGBA& second, sRGBA& result)
{
	float a = first.a / 255.0f, b = second.a / 255.0f;
	result.r = (unsigned char)(first.r * (1.0f - b) * a + second.r * b);
	result.g = (unsigned char)(first.g * (1.0f - b) * a + second.g * b);
	result.b = (unsigned char)(first.b * (1.0f - b) * a + second.b * b);
	result.a = 255;
}

void SetBrightness(sRGBA& color, sRGBA& brightness, sRGBA& result)
{
	float average = (color.r + color.g + color.b) / 3.0f;
	float level = (brightness.r + brightness.g + brightness.b) / 3.0f;
	float product = level * average / 255.0f;
	float scale = product / average;
	result.r = Min((int)(color.r * scale), 255);
	result.g = Min((int)(color.g * scale), 255);
	result.b = Min((int)(color.b * scale), 255);
	result.a = color.a;
}

void GetResizedImage(const Bitmap* source, const Bitmap* mask, Bitmap* dest,
	int width, int height)
{
	if (!source || !dest)
		return;
	float scaleX = (float)source->Width() / width;
	float scaleY = (float)source->Height() / height;
	bool useAlpha = !mask && source->BitsPerPixel() == 32;
	dest->Create(width, height, 32);
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			int left = (int)floor(x * scaleX), top = (int)floor(y * scaleY);
			int right = (int)ceil((x + 1) * scaleX - 1),
				bottom = (int)ceil((y + 1) * scaleY - 1);
			if (mask && NoDrawArea(mask, left, top, right, bottom))
				dest->SetPixel(x, y, 0, 0, 0, 0);
			else
			{
				sRGBA color =
					GetAverageColor(source, left, top, right, bottom, useAlpha);
				dest->SetPixel(x, y, color.r, color.g, color.b, color.a);
			}
		}
}

sRGBA GetAverageColor(const Bitmap* bitmap, int left, int top, int right,
	int bottom, bool useAlpha)
{
	unsigned int red = 0, green = 0, blue = 0, alpha = 0;
	unsigned int weightSum = 0, alphaCount = 0;
	for (int y = top; y <= bottom; ++y)
	{
		if ((unsigned int)y >= bitmap->Height())
			break;
		for (int x = left; x <= right; ++x)
		{
			if ((unsigned int)x >= bitmap->Width())
				break;
			unsigned char r, g, b, a;
			if (bitmap->BitsPerPixel() == 32)
				bitmap->GetPixel(x, y, r, g, b, a);
			else
			{
				a = 255;
				bitmap->GetPixel(x, y, r, g, b);
			}
			sRGBA color(r, g, b, a);
			unsigned char value = useAlpha ? a : color.toHSV().v;
			unsigned char weight = (255 - value) / 20 + 1;
			red += r * weight;
			green += g * weight;
			blue += b * weight;
			alpha += a;
			weightSum += weight;
			if (a > 0)
				++alphaCount;
		}
	}
	if (weightSum)
	{
		red /= weightSum;
		green /= weightSum;
		blue /= weightSum;
	}
	if (alphaCount)
		alpha /= alphaCount;
	return sRGBA(red, green, blue, alpha);
}

bool NoDrawArea(const Bitmap* bitmap, int left, int top, int right, int bottom)
{
	unsigned char r, g, b;
	for (int y = top; y <= bottom; ++y)
		for (int x = left; x <= right; ++x)
		{
			bitmap->GetPixel(x, y, r, g, b);
			if (r <= 16 && g <= 16 && b <= 16)
				return true;
		}
	return false;
}

sRGBA GetSimilarColor(sRGBA reference, sRGBA first, sRGBA second)
{
	sRGBA a = reference, b = first, c = second;
	a.RemoveAlpha();
	b.RemoveAlpha();
	c.RemoveAlpha();
	sHSV ah, bh, ch;
	ah = a.toHSV();
	bh = b.toHSV();
	ch = c.toHSV();
	int d1 = Min(abs(ah.h - bh.h), abs(bh.h - ah.h + 360));
	int d2 = Min(abs(ah.h - ch.h), abs(ch.h - ah.h + 360));
	if (d1 < d2)
		return first;
	if (d1 > d2)
		return second;
	if (abs(ah.v - bh.v) < abs(ah.v - ch.v))
		return first;
	return second;
}

void RGBToHLS(unsigned char r, unsigned char g, unsigned char b,
	unsigned char& h, unsigned char& l, unsigned char& s)
{
	WORD hue, luminance, saturation;
	ColorRGBToHLS(RGB(r, g, b), &hue, &luminance, &saturation);
	h = hue;
	l = luminance;
	s = saturation;
}

void HLSToRGB(unsigned char h, unsigned char l, unsigned char s,
	unsigned char& r, unsigned char& g, unsigned char& b)
{
	COLORREF color = ColorHLSToRGB(h, l, s);
	r = GetRValue(color);
	g = GetGValue(color);
	b = GetBValue(color);
}

unsigned char HueToRGB(unsigned char first, unsigned char second,
	unsigned char hue)
{
	if (hue > 240)
		hue -= 240;
	if (hue < 40)
		return first + ((second - first) * hue + 20) / 40;
	if (hue < 120)
		return second;
	if (hue < 160)
		return first + ((second - first) * (160 - hue) + 20) / 40;
	return first;
}

void GetUccInfoFromPacket(WReceivedPacket* packet)
{
	if (!packet)
		return;
	unsigned char count = packet->Decode1();
	for (int i = 0; i < count; ++i)
	{
		unsigned long id = packet->Decode4();
		unsigned long typeId = packet->Decode4();
		char index[9];
		memset(index, 0, sizeof(index));
		strncpy(index, packet->DecodeStr().c_str(), 9);
		UccManager()->AddClothes(0, id, typeId, index, false, false);
	}
}

void RequestUccInfo(sUserInfoTime& user)
{
	RequestUccInfo(user.info.charInfo);
}

void RequestUccInfo(sUserInfo& user)
{
	RequestUccInfo(user.charInfo);
}

void RequestUccInfo(sRoomSlot& slot)
{
	RequestUccInfo(slot.charInfo);
}

void RequestUccInfo(ranktype::sRankUserInfo& user)
{
	RequestUccInfo(user.charInfo);
}

void RequestUccInfo(sCharacterInfo& character)
{
	for (int i = 0; i < 24; ++i)
	{
		unsigned long typeId = character.tidParts[i],
					  id = character.ItemIdList[i];
		if (typeId && IsUccClothes(typeId) && !UccManager()->FindClothes(id))
		{
			WSendPacket packet((enumClientPacket)177);
			packet.Encode1(1);
			packet.Encode4(id);
			packet.Encode1(0);
			packet.Send(TO_GAME);
		}
	}
}

void RequestUccInfo(sMailIncludeItem& item)
{
	if (IsUccClothes(item.dwTID))
		UccManager()->AddClothes(0, 0, item.dwTID, item.csUCCIndex, false,
			false);
}

sHSV sRGB::toHSV()
{
	sHSV hsv;
	double minimum = Min(r, g, b), maximum = Max(r, g, b);
	double delta = maximum - minimum;
	hsv.v = maximum;
	double hue;
	if (delta == 0.0)
	{
		hsv.h = hsv.s = 0;
	}
	else
	{
		hsv.s = delta / maximum * 255.0;
		if (r == (unsigned int)maximum)
			hue = (g - b) / delta;
		else if (g == (unsigned int)maximum)
			hue = (b - r) / delta + 2.0;
		else
			hue = (r - g) / delta + 4.0;
		hue *= 60.0;
		if (hue < 0)
			hue += 360;
		if (hue == 360)
			hue = 0;
		hsv.h = hue;
	}
	return hsv;
}

sRGB sHSV::toRGB()
{
	sRGB rgb;
	double value = v;
	double chroma = s * value / 255.0;
	double minimum = value - chroma;
	double hue = h;
	if (h > 300 || h <= 60)
	{
		rgb.r = value;
		if (h > 300)
		{
			rgb.g = minimum;
			double fraction = (hue - 360.0) / 60.0;
			rgb.b = (fraction * chroma - minimum) * -1.0;
		}
		else
		{
			rgb.b = minimum;
			double fraction = hue / 60.0;
			rgb.g = fraction * chroma + minimum;
		}
	}
	else if (h < 180)
	{
		rgb.g = value;
		if (h < 120)
		{
			rgb.b = minimum;
			rgb.r = minimum - (hue / 60.0 - 2) * chroma;
		}
		else
		{
			rgb.r = minimum;
			rgb.b = (hue / 60.0 - 2) * chroma + minimum;
		}
	}
	else
	{
		rgb.b = value;
		if (h < 240)
		{
			rgb.r = minimum;
			rgb.g = minimum - (hue / 60.0 - 4) * chroma;
		}
		else
		{
			rgb.g = minimum;
			rgb.r = (hue / 60.0 - 4) * chroma + minimum;
		}
	}
	return rgb;
}

void sRGBA::RemoveAlpha()
{
	if (a == 255)
		return;
	sHSV hsv = toHSV();
	hsv.s = hsv.s * (float)a / 255.0f;
	sRGB rgb = hsv.toRGB();
	r = rgb.r;
	g = rgb.g;
	b = rgb.b;
	a = 255;
}
