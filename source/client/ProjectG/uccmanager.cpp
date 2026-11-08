#include "stdafx.h"
#include "uccmanager.h"
#include "ucclibrary.h"
#include "netresourcemanager.h"

extern WResourceManager* g_resrcmng;

CUccManager* UccManager()
{
	static CUccManager manager;
	return &manager;
}

CUccManager::CUccManager()
{
	InitializeCriticalSection(&m_csRefresh);
}

CUccManager::~CUccManager()
{
	DeleteCriticalSection(&m_csRefresh);
	ClearClothes();
	ClearAztec();
	m_refreshItem.clear();
	ClearTempClothes();
}

void CUccManager::AddClothes(unsigned char slot, unsigned long id,
	unsigned long typeId, const char* uccIndex, bool bSend, bool bForce)
{
	sUccClothes clothes;
	clothes.id = id;
	clothes.typeId = typeId;
	strncpy(clothes.uccIndex, uccIndex, 9);
	AddClothes(slot, clothes, bSend, bForce);
}

void CUccManager::AddClothes(unsigned char slot, sUccClothes& clothes,
	bool bSend, bool bForce)
{
	sUccClothes* found = FindClothes(clothes.typeId, clothes.uccIndex);
	if (!found)
	{
		m_clothes.push_back(clothes);
		if (!clothes.downloadCount)
			RequestClothes(clothes.typeId, clothes.uccIndex, bForce);
	}
	else if (clothes.id != -1 && !FindClothes(clothes.id))
	{
		sUccClothes copy = *found;
		copy.typeId = clothes.typeId;
		copy.id = clothes.id;
		m_clothes.push_back(copy);
		AddRefreshItem(copy.typeId, copy.uccIndex);
	}
	if (bSend)
	{
		WSendPacket packet((enumClientPacket)177);
		packet.Encode1(1);
		packet.Encode4(clothes.id);
		packet.Encode1(slot);
		packet.Send((eSendTo)0);
	}
}

void CUccManager::AddAztec(sUccAztec& aztec, bool bForce)
{
	if (!FindAztec(aztec.typeId, aztec.uccIndex))
		m_aztec.push_back(aztec);
}

sUccClothes* CUccManager::FindClothes(unsigned long typeId,
	const char* uccIndex)
{
	std::list<sUccClothes>::iterator it;
	for (it = m_clothes.begin(); it != m_clothes.end(); it++)
	{
		sUccClothes& clothes = *it;
		if (clothes.typeId == typeId && !strcmp(clothes.uccIndex, uccIndex))
			return &clothes;
	}
	for (it = m_clothes.begin(); it != m_clothes.end(); it++)
	{
		sUccClothes& clothes = *it;
		if (!strcmp(clothes.uccIndex, uccIndex) &&
			IsSameClothes(typeId, clothes.typeId))
			return &clothes;
	}
	return NULL;
}

sUccAztec* CUccManager::FindAztec(unsigned long typeId, const char* uccIndex)
{
	std::list<sUccAztec>::iterator it;
	for (it = m_aztec.begin(); it != m_aztec.end(); it++)
	{
		sUccAztec& aztec = *it;
		if (aztec.typeId == typeId && !strcmp(aztec.uccIndex, uccIndex))
			return &aztec;
	}
	return NULL;
}

sUccClothes* CUccManager::FindClothes(unsigned long id)
{
	sItemInfo* item = GetMyItemInfo(id);
	if (item)
	{
		std::list<sItemInfo>::iterator it;
		for (it = CSharedDoc::Instance()->m_uccItemList.begin();
			it != CSharedDoc::Instance()->m_uccItemList.end(); it++)
		{
			sItemInfo& clothes = *it;
			if (clothes.tid == item->tid &&
				!strcmp(clothes.UccIndex, item->UccIndex))
			{
				sUccClothes* found = _GetClothes(clothes.guid);
				if (found)
					return found;
			}
		}
		for (it = CSharedDoc::Instance()->m_uccItemList.begin();
			it != CSharedDoc::Instance()->m_uccItemList.end(); it++)
		{
			sItemInfo& clothes = *it;
			if (!strcmp(clothes.UccIndex, item->UccIndex) &&
				IsSameClothes(item->tid, clothes.tid))
			{
				sUccClothes* found = _GetClothes(clothes.guid);
				if (found)
					return found;
			}
		}
	}
	else
		return _GetClothes(id);
	return NULL;
}

sUccClothes* CUccManager::_GetClothes(unsigned long id)
{
	std::list<sUccClothes>::iterator it;
	for (it = m_clothes.begin(); it != m_clothes.end(); it++)
	{
		sUccClothes& clothes = *it;
		if (clothes.id == id)
			return &clothes;
	}
	return NULL;
}

sUccAztec* CUccManager::FindAztec(unsigned long id)
{
	sItemInfo* item = GetMyItemInfo(id);
	if (!item)
		return _GetAztec(id);
	std::list<sItemInfo>::iterator it;
	for (it = CSharedDoc::Instance()->m_uccAztecList.begin();
		it != CSharedDoc::Instance()->m_uccAztecList.end(); it++)
	{
		sItemInfo& aztec = *it;
		if (aztec.tid == item->tid && !strcmp(aztec.UccIndex, item->UccIndex))
		{
			sUccAztec* found = _GetAztec(aztec.guid);
			if (found)
				return found;
		}
	}
	return NULL;
}

sUccAztec* CUccManager::_GetAztec(unsigned long id)
{
	std::list<sUccAztec>::iterator it;
	for (it = m_aztec.begin(); it != m_aztec.end(); it++)
	{
		sUccAztec& aztec = *it;
		if (aztec.id == id)
			return &aztec;
	}
	return NULL;
}

bool CUccManager::IsDownloadComplete(unsigned long typeId, const char* uccIndex)
{
	sUccClothes* clothes = FindClothes(typeId, uccIndex);
	sUccAztec* aztec = FindAztec(typeId, uccIndex);
	if (clothes)
		return IsDownloadComplete(clothes);
	if (aztec)
		return IsDownloadComplete(aztec);
	return false;
}

bool CUccManager::SetDownloadComplete(unsigned long typeId,
	const char* uccIndex)
{
	sUccClothes* clothes = FindClothes(typeId, uccIndex);
	sUccAztec* aztec = FindAztec(typeId, uccIndex);
	if (clothes)
		return SetDownloadComplete(clothes);
	if (aztec)
		return SetDownloadComplete(aztec);
	return false;
}

bool CUccManager::IsDownloadComplete(sUccClothes* clothes)
{
	if (clothes)
	{
		if (clothes->downloadCount == 1)
			return true;
	}
	return false;
}

bool CUccManager::IsDownloadComplete(sUccAztec* aztec)
{
	if (aztec)
	{
		if (aztec->downloadCount == 1)
			return true;
	}
	return false;
}

bool CUccManager::SetDownloadComplete(sUccClothes* clothes)
{
	if (!clothes)
		return false;
	++clothes->downloadCount;
	if (clothes->downloadCount == 1)
	{
		std::list<sUccClothes>::iterator it;
		for (it = m_clothes.begin(); it != m_clothes.end(); it++)
		{
			sUccClothes& other = *it;
			if (IsSameClothes(clothes->typeId, other.typeId) &&
				!strcmpi(clothes->uccIndex, other.uccIndex))
			{
				other.downloadCount = clothes->downloadCount;
				other.icon = clothes->icon;
				strncpy(other.uccName, clothes->uccName, 41);
			}
		}
		return true;
	}
	return false;
}

bool CUccManager::SetDownloadComplete(sUccAztec* aztec)
{
	if (aztec)
	{
		++aztec->downloadCount;
		if (aztec->downloadCount == 1)
			return true;
		else
			return false;
	}
	return false;
}

void CUccManager::RequestClothes(unsigned long typeId, const char* uccIndex,
	bool bForce)
{
	NetResManager()->DownloadUccClothes(typeId, uccIndex, bForce, false);
}

void CUccManager::RequestAztec(unsigned long typeId, const char* uccIndex,
	bool bForce)
{
}

void CUccManager::Clear()
{
	ClearTexture();
	ClearClothes();
	ClearAztec();
	ClearIcon();
	m_refreshItem.clear();
	m_displayClothes.clear();
	ClearTempClothes();
}

void CUccManager::ClearTexture()
{
	std::list<sUccClothes>::iterator it;
	for (it = m_clothes.begin(); it != m_clothes.end();)
	{
		for (int i = 0; i < 2; ++i)
		{
			std::string name;
			if (i == 0)
				name = GenerateTextureName_F((*it).typeId, (*it).uccIndex);
			else if (i == 1)
				name = GenerateTextureName_B((*it).typeId, (*it).uccIndex);
			w_texlist* texture = g_resrcmng->FindTexture(name.c_str());
			if (texture)
				g_resrcmng->Release(texture->texhandle);
		}
		it = m_clothes.erase(it);
	}
}

void CUccManager::ClearIcon()
{
	UCC_ICON_IT it;
	for (it = m_icon.begin(); it != m_icon.end(); it++)
	{
		Bitmap* bitmap = (*it).second;
		ClearTexCache(bitmap);
		delete bitmap;
	}
	m_icon.clear();
}

void CUccManager::ClearTempClothes()
{
	std::list<sUccTempClothes>::iterator it;
	for (it = m_tempClothes.begin(); it != m_tempClothes.end(); it++)
	{
		ClearTexCache((*it).front);
		delete (*it).front;
		(*it).front = NULL;
		ClearTexCache((*it).back);
		delete (*it).back;
		(*it).back = NULL;
	}
	m_tempClothes.clear();
}

void CUccManager::AddRefreshItem(unsigned long typeId, const char* uccIndex)
{
	sUccRefreshItem item;
	item.typeId = typeId;
	strncpy(item.uccIndex, uccIndex, 9);
	EnterCriticalSection(&m_csRefresh);
	m_refreshItem.push_back(item);
	LeaveCriticalSection(&m_csRefresh);
	IFF_STRUCT::sPart* part = ItemManager()->FindPart(typeId);
	for (PartIterator it = ItemManager()->m_PartMap.begin();
		it != ItemManager()->m_PartMap.end(); ++it)
	{
		IFF_STRUCT::sPart* other = &(*it).second;
		if (typeId != other->c.TypeId && IsSameClothes(part, other))
		{
			sUccRefreshItem item;
			item.typeId = other->c.TypeId;
			strncpy(item.uccIndex, uccIndex, 9);
			EnterCriticalSection(&m_csRefresh);
			m_refreshItem.push_back(item);
			LeaveCriticalSection(&m_csRefresh);
		}
	}
}

bool CUccManager::GetRefreshItem(sUccRefreshItem& item)
{
	EnterCriticalSection(&m_csRefresh);
	int count = m_refreshItem.size();
	if (count > 0)
	{
		item = m_refreshItem.front();
		m_refreshItem.pop_front();
	}
	LeaveCriticalSection(&m_csRefresh);
	return count > 0;
}

void CUccManager::SetUccTempClothes(unsigned long typeId, const char* uccIndex,
	Bitmap* front, Bitmap* back)
{
	std::list<sUccTempClothes>::iterator it;
	for (it = m_tempClothes.begin(); it != m_tempClothes.end(); it++)
	{
		if ((*it).typeId == typeId && !strcmpi((*it).uccIndex, uccIndex))
		{
			*(*it).front = *front;
			*(*it).back = *back;
			return;
		}
	}
	sUccTempClothes clothes;
	clothes.typeId = typeId;
	strncpy(clothes.uccIndex, uccIndex, 9);
	*clothes.front = *front;
	*clothes.back = *back;
	m_tempClothes.push_back(clothes);
}

bool CUccManager::GetUccTempClothes(unsigned long typeId, const char* uccIndex,
	Bitmap& front, Bitmap& back)
{
	std::list<sUccTempClothes>::iterator it;
	for (it = m_tempClothes.begin(); it != m_tempClothes.end(); it++)
	{
		if ((*it).typeId == typeId && !strcmpi((*it).uccIndex, uccIndex))
		{
			front = *(*it).front;
			back = *(*it).back;
			return true;
		}
	}
	return false;
}

void CUccManager::BuildDisplayClothesSet()
{
	if (m_displayClothes.size() == 0)
		return;
}

void CUccManager::AddDisplayClothes(unsigned long partTypeId,
	unsigned long itemId, std::string name, int rank)
{
	sDisplayUccClothes clothes;
	clothes.partTypeId = partTypeId;
	clothes.itemId = itemId;
	clothes.name = name;
	clothes.rank = rank;
	clothes.part.c.TypeId = partTypeId;
	m_displayClothes.push_back(clothes);
	WSendPacket packet((enumClientPacket)177);
	packet.Encode1(1);
	packet.Encode4(itemId);
	packet.Encode1(0);
	packet.Send((eSendTo)0);
}

bool CUccManager::IsDisplayUccClothes(const IFF_ITEM_COMMON* item)
{
	return strcmpi(item->Icon, "UCCDISPLAY") == 0;
}

unsigned long CUccManager::GetWinnerCharacterTypeId()
{
	unsigned long typeId = 0;
	int rank = 999999;
	std::list<sDisplayUccClothes>::iterator it;
	for (it = m_displayClothes.begin(); it != m_displayClothes.end(); it++)
	{
		if ((*it).rank < rank)
		{
			typeId = (*it).partTypeId;
			rank = (*it).rank;
		}
	}
	return ((typeId >> 18) & 0xff) | 0x4000000;
}
