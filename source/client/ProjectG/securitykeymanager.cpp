#include "securitykeymanager.h"

CSecurityKeyManager* SecurityKeyManager()
{
	static CSecurityKeyManager manager;
	return &manager;
}

CSecurityKeyManager::CSecurityKeyManager()
{
}

CSecurityKeyManager::~CSecurityKeyManager()
{
}

void CSecurityKeyManager::SetKey(eSKContent content, unsigned long index,
	std::string key)
{
	std::list<sSKItem>::iterator it;
	for (it = m_items.begin(); it != m_items.end(); it++)
	{
		if ((*it).content == content && (*it).index == index)
		{
			(*it).key = key;
			return;
		}
	}

	sSKItem item;
	item.content = content;
	item.index = index;
	item.key = key;
	m_items.push_back(item);
}

std::string CSecurityKeyManager::GetKey(eSKContent content, unsigned long index)
{
	sSKItem* pItem = GetItem(content, index);
	if (pItem)
		return pItem->key;
	return "";
}

sSKItem* CSecurityKeyManager::GetItem(eSKContent content, unsigned long index)
{
	std::list<sSKItem>::iterator it;
	for (it = m_items.begin(); it != m_items.end(); it++)
	{
		sSKItem* pItem = &(*it);
		if (pItem->content == content && pItem->index == index)
			return pItem;
	}
	return NULL;
}

void CSecurityKeyManager::ReqCreateKey(eSKContent content, unsigned long index)
{
	WSendPacket send((enumClientPacket)0xc1);
	send.Encode1(0);
	send.Encode4(MyUID());
	send.Encode1(content);
	send.Encode4(index);
	send.Send(TO_GAME);
}

void CSecurityKeyManager::ResCreateKey(eSKContent content, unsigned long index,
	std::string key, unsigned char result)
{
	if (result == 1)
		SetKey(content, index, key);

	switch (content)
	{
	case SK_UCC:
		if (result == 1)
			AfxGetTask()->GetActor("RealMyRoom")
				<< MsgObject(NULL, 0x212, 0, 0, 0, 0, 0);
		break;
	}
}

void CSecurityKeyManager::ReqCheckKey(eSKContent content, unsigned long index,
	std::string key)
{
	WSendPacket send((enumClientPacket)0xc1);
	send.Encode1(1);
	send.Encode4(MyUID());
	send.Encode1(content);
	send.Encode4(index);
	send.EncodeStr(key);
	send.Send(TO_GAME);
}

void CSecurityKeyManager::ResCheckKey(eSKContent content, unsigned long index,
	std::string key, unsigned char result)
{
}
