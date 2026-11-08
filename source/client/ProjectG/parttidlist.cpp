#include "parttidlist.h"
#include "../../shared/localize.h"

CItemManager* CPartTidList::m_pItemManager = NULL;

void CPartTidList::SetItemManager(CItemManager* pItemManager)

{
	m_pItemManager = pItemManager;
}

CPartTidList::CPartTidList()
	: m_charTid(0),
	  m_defPartNum(0),
	  m_partNum(0),
	  m_hairColor(0),
	  m_shirtsColor(0)

{
	memset(m_tid, 0, sizeof(m_tid));
	memset(m_uid, 0, sizeof(m_uid));
}

bool CPartTidList::SetTids(sCharacterInfo* pInfo, unsigned char level)

{
	if (!pInfo)
		return false;

	bool ret = SetTids(pInfo->tid, pInfo->hairClr, pInfo->shirtsClr,
		pInfo->tidParts, level, pInfo->ItemIdList);
	if (!ret)
		m_charTid = pInfo->tid;

	return ret;
}

bool CPartTidList::SetTids(unsigned long charTid, unsigned char hairColor,
	unsigned char shirtsColor, unsigned long* tids, unsigned char level,
	unsigned long* uids)

{
	IFF_STRUCT::sChar* pChar = m_pItemManager->FindChar(charTid);

	if (!pChar)
		return false;

	unsigned long charType = charTid & 0x3FFFFFF;
	if (charType == 4)
	{
		tids[18] = 0x8124400;
		tids[19] = 0x8126400;
	}
	else if (charType == 6)
	{
		tids[15] = 0x819e400;
		tids[19] = 0x81a6400;
		tids[20] = 0x81a8400;
	}
	else if (charType == 2)
	{
		tids[18] = 0x80a4400;
	}
	else if (charType == 0)
	{
		tids[18] = 0x8024400;
	}
	else if (charType == 1)
	{
		tids[19] = 0x8066400;
	}
	else if (charType == 7)
	{
		tids[22] = 0x81ec400;
	}
	else if (charType == 3)
	{
		tids[19] = 0x80e6400;
	}
	else if (charType == 5)
	{
		tids[9] = 0x8152400;
	}
	else if (charType == 8)
	{
		tids[19] = 0x8226400;
	}
	else if (charType == 9)
	{
		if (m_pItemManager->FindPart(0x826a400) &&
			m_pItemManager->FindPart(0x826c400))
		{
			tids[21] = 0x826a400;
			tids[22] = 0x826c400;
		}
	}

	if (m_partNum)
	{
		memset(m_tid, 0, sizeof(unsigned long) * m_partNum);
		memset(m_uid, 0, sizeof(m_uid));
	}

	unsigned long mask = 0;
	for (int i = 0; i < pChar->nParts + pChar->nAcsries; i++)
	{
		if (tids[i])
		{
			if (((tids[i] >> 18) & 0xFF) != (charTid & 0x3FFFFFF))
				return false;

			IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(tids[i]);

			if (!pPart)
				return false;

			m_tid[i] = tids[i];
			mask |= pPart->PosMask;

			if (IsLocalContent(S4_GUID_BASE))
			{
				m_uid[i] = uids[i];
			}
		}
	}

	for (int j = 0; j < pChar->nParts; j++)
	{
		if (!(mask & (1 << j)))
		{
			unsigned long tid;
			if (j == 0)
				tid = (charTid << 18) | hairColor;
			else
				tid = ((charTid << 5) | j) << 13;
			tid |= 0x8000600;

			IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(tid);
			if (pPart && pPart->c.Level <= level)
				m_tid[j] = tid;
			else
				m_tid[j] = 0;

			if (IsLocalContent(S4_GUID_BASE))
			{
				m_uid[j] = 0;
			}
		}
	}

	m_charTid = charTid;

	m_hairColor = hairColor;
	m_shirtsColor = shirtsColor;
	m_defPartNum = pChar->nParts;
	m_partNum = pChar->nParts + pChar->nAcsries;

	for (int k = 0; k < pChar->nParts + pChar->nAcsries; k++)
	{
		if (tids[k])
		{
			IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(tids[k]);

			if (!pPart->c.IsUnderLvl && pPart->c.Level > level)
			{
				if (!SetTid(pPart->c.TypeId, uids[k], 0, 0))
				{
					ItemManager()->GetDefCombo(m_charTid, m_tid);
				}

				memcpy(tids, m_tid, sizeof(unsigned long) * m_partNum);
			}
		}
	}

	if (IsLocalContent(S4_GUID_BASE))
	{
		memcpy(uids, m_uid, sizeof(m_uid));
	}

	return true;
}

bool CPartTidList::SetTid(unsigned long tid, unsigned long uid,
	const char* name, unsigned short flag)
{
	unsigned char slot = (tid >> 13) & 0x1F;
	bool bSkip = false;
	IFF_STRUCT::sPart* pPart;

	if (m_tid[slot] == tid && (!uid || m_uid[slot] == uid))
	{
		int i;
		unsigned long setTid;

		for (i = 0; i < m_partNum; i++)
		{
			if ((m_tid[i] & 0x600) == 0x200)
			{
				setTid = m_tid[i] & ~0x200;
				pPart = ItemManager()->FindPart(setTid);

				if (pPart)
				{
					if (pPart->PosMask & (1 << slot))
					{
						tid = setTid;
						break;
					}
				}
			}
		}

		if (slot < m_defPartNum && i == m_partNum)
			tid = 0x8000400 | (((m_charTid << 5) | slot) << 13);
		else
			bSkip = true;
	}

	pPart = ItemManager()->FindPart(tid);

	if (!pPart)
		return false;

	unsigned long remain = pPart->PosMask;

	for (int i = 0; i < m_partNum; i++)
	{
		if (m_tid[i])
		{
			IFF_STRUCT::sPart* pOther = ItemManager()->FindPart(m_tid[i]);
			unsigned long overlap = pPart->PosMask & pOther->PosMask;

			if (overlap)
			{
				remain &= ~overlap;

				if (overlap == pOther->PosMask)
				{
					EmptyList(overlap);
				}
				else
				{
					switch ((m_tid[i] >> 9) & 3)
					{
					case 0:
					case 2:
					{
						unsigned long setTid = m_tid[i] | 0x200;
						IFF_STRUCT::sPart* pSet =
							ItemManager()->FindPart(setTid);

						if (pSet)
						{
							EmptyList(pSet->PosMask);

							for (int j = 0; j < m_partNum; j++)
							{
								if (pSet->PosMask & (1 << j))
								{
									m_tid[j] = setTid;
									m_uid[j] = 0;

									break;
								}
							}
						}
						else
						{
							unsigned long rest = pOther->PosMask &
								(pOther->PosMask ^ pPart->PosMask);
							for (int j = 0; j < m_partNum; j++)
							{
								if (!(rest & (-1 << j)))
									break;

								if (rest & (1 << j))
								{
									unsigned long defTid = 0x8000400 |
										(((m_charTid << 5) | j) << 13);

									if (ItemManager()->FindPart(defTid))
									{
										m_tid[j] = defTid;
										m_uid[j] = 0;
									}
								}
							}
						}
					}
					break;
					}
					EmptyList(overlap);
				}
			}

			if (!remain)
				break;
		}
	}

	if (!bSkip)
	{
		m_tid[slot] = tid;

		if (IsLocalContent(S4_GUID_BASE) && uid)
		{
			m_uid[slot] = uid;
		}
	}

	unsigned long mask = 0;
	for (int i = 0; i < m_partNum; i++)
	{
		IFF_STRUCT::sPart* p = ItemManager()->FindPart(m_tid[i]);

		if (p)
		{
			mask |= p->PosMask;
		}
	}

	for (int i = 0; i < m_defPartNum; i++)
	{
		if (!(mask & (1 << i)))
		{
			unsigned long defTid = 0x8000400 | (((m_charTid << 5) | i) << 13);
			IFF_STRUCT::sPart* p = ItemManager()->FindPart(defTid);

			if (p && !(p->PosMask & mask))
			{
				m_tid[i] = defTid;
				m_uid[i] = 0;
			}
		}
	}

	return true;
}

void CPartTidList::EmptyList(unsigned long mask)
{
	for (int i = 0; i < m_partNum; i++)
	{
		if (!(mask & (-1 << i)))
			break;

		if (mask & (1 << i))
		{
			m_tid[i] = 0;

			if (IsLocalContent(S4_GUID_BASE))
			{
				m_uid[i] = 0;
			}
		}
	}
}

void CPartTidList::SetDefaultTids()
{
	memset(m_tid, 0, sizeof(m_tid));
	memset(m_uid, 0, sizeof(m_uid));

	m_pItemManager->GetDefCombo(m_charTid, m_tid);
}

bool CPartTidList::IsComboValid() const
{
	unsigned long mask = 0;

	for (int i = 0; i < m_partNum; i++)
	{
		if (m_tid[i])
		{
			if (((m_tid[i] >> 18) & 0xFF) != (m_charTid & 0x3FFFFFF))
				return false;

			IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(m_tid[i]);
			if (!pPart)
				return false;

			if (mask & pPart->PosMask)
				return false;

			mask |= pPart->PosMask;
		}
	}

	return (mask & ~(-1 << m_defPartNum)) == ~(-1 << m_defPartNum) ? true
																   : false;
}

bool CPartTidList::IsEquipped(unsigned long tid, unsigned long uid) const
{
	for (int i = 0; i < m_partNum; i++)
	{
		if (uid && m_uid[i])
		{
			if (m_tid[i] == tid && uid == m_uid[i])
				return true;
		}
		else
		{
			if (m_tid[i] == tid)
				return true;
		}
	}

	return false;
}

unsigned long CPartTidList::GetOverlapped(unsigned long tid) const
{
	if (((tid >> 18) & 0xFF) != (m_charTid & 0x3FFFFFF))
		return 0;

	IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(tid);
	unsigned long ret = 0;

	for (int i = 0; i < m_partNum; i++)
	{
		if (m_tid[i])
		{
			if (((m_tid[i] >> 18) & 0xFF) != (m_charTid & 0x3FFFFFF))
				return 0;

			IFF_STRUCT::sPart* pOther = m_pItemManager->FindPart(m_tid[i]);
			if (!pOther)
				return 0;

			if (pOther->PosMask & pPart->PosMask)
				ret |= 1 << i;
		}
	}

	return ret;
}

bool CPartTidList::CanSetPart(unsigned long tid) const
{
	if (((tid >> 18) & 0xFF) != (m_charTid & 0x3FFFFFF))
		return false;

	IFF_STRUCT::sPart* pPart = m_pItemManager->FindPart(tid);

	for (int i = 0; i < m_defPartNum; i++)
	{
		if (m_tid[i])
		{
			IFF_STRUCT::sPart* pOther = m_pItemManager->FindPart(m_tid[i]);
			if (!pOther)
				return false;

			if (pPart->PosMask & pOther->PosMask)
			{
				unsigned long rest =
					pOther->PosMask & (pPart->PosMask ^ pOther->PosMask);

				for (int j = 0; j < m_defPartNum; j++)
				{
					if (!(rest & (-1 << j)))
						break;

					if (rest & (1 << i))
					{
						unsigned long defTid =
							0x8000400 | (((m_charTid << 5) | i) << 13);
						IFF_STRUCT::sPart* pDef =
							ItemManager()->FindPart(defTid);

						if (pDef && (pPart->PosMask & pDef->PosMask))
							return false;
					}
				}
			}
		}
	}

	return true;
}
