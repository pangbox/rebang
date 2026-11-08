#include "stdafx.h"
#include "partsinfo.h"
#include "parttidlist.h"
#include "wbone.h"
#include "ucclibrary.h"
#include "../../shared/exceptionreport.h"
#include "uccmanager.h"
#include "taskmanager.h"
#include "golfdoc.h"

int g_iIndexWigColor[10] = { 16, 16, 16, 16, 16, 20, 14, 16, 16, 16 };

CPartList::CPartList()
	: m_tidChar(0),
	  m_defPartNum(0),
	  m_partNum(0),
	  m_hairColor(0),
	  m_shirtsColor(0)
{
}

CPartList::~CPartList()
{
}

bool CPartList::Build(const CPartTidList* tids)
{
	if (!tids->IsComboValid())
		ItemManager()->GetDefCombo(tids->m_charTid,
			const_cast<unsigned long*>(tids->m_tid));

	memset(m_part, 0, sizeof(m_part));
	m_tidChar = tids->m_charTid;
	m_defPartNum = tids->m_defPartNum;
	m_partNum = tids->m_partNum;
	m_hairColor = tids->m_hairColor;
	m_shirtsColor = tids->m_shirtsColor;

	for (int i = 0; i < m_partNum; i++)
	{
		if (tids->m_tid[i])
		{
			IFF_STRUCT::sPart* part = ItemManager()->FindPart(tids->m_tid[i]);
			m_part[i].tid = tids->m_tid[i];
			m_part[i].pPart = part;
			if (IsLocalContent(S4_GUID_BASE))
				m_part[i].uid = tids->m_uid[i];
		}
	}

	return true;
}

unsigned long CPartList::SetPart(CPartTidList* tids, unsigned long tid,
	unsigned long uid)
{
	unsigned int character = (tid >> 18) & 255;
	if (character != (m_tidChar & 0x3ffffff) ||
		character != (tids->m_charTid & 0x3ffffff) || !IsIdentical(tids) ||
		!tids->IsComboValid() || !SetPart(tid, uid) || !CheckSubParts())
	{
		return 0;
	}

	unsigned long mask = 0;
	for (int i = 0; i < m_partNum; i++)
	{
		if (tids->m_tid[i] != m_part[i].tid ||
			(IsLocalContent(S4_UCC) && m_part[i].tid == tid &&
				IsUccClothes(m_part[i].tid)))
		{
			mask |= 1 << i;
			tids->m_tid[i] = m_part[i].tid;
			if (IsLocalContent(S4_GUID_BASE))
				tids->m_uid[i] = m_part[i].uid;
		}
	}

	return mask;
}

bool CPartList::SetPart(unsigned long tid, unsigned long uid)
{
	IFF_STRUCT::sPart* part;
	IFF_STRUCT::sPart* subPart;
	unsigned char slot = (tid >> 13) & 31;
	bool skip = false;

	if (m_part[slot].tid == tid &&
		(!uid || !m_part[slot].uid || m_part[slot].uid == uid))
	{
		if (slot < m_defPartNum)
		{
			tid = 0x8000400 | (((m_tidChar << 5) | slot) << 13);
			uid = 0;
		}
		else
			skip = true;
	}

	part = ItemManager()->FindPart(tid);
	if (!part)
		return false;

	unsigned long remain = part->PosMask;
	for (int i = 0; i < m_partNum; i++)
	{
		if (m_part[i].tid)
		{
			unsigned long overlap = part->PosMask & m_part[i].pPart->PosMask;
			if (overlap)
			{
				remain &= ~overlap;
				if (overlap == m_part[i].pPart->PosMask)
				{
					EmptyList(overlap);
				}
				else
				{
					switch ((m_part[i].tid >> 9) & 3)
					{
					case 0:
					case 2:
					{
						unsigned long setTid = m_part[i].tid | 0x200;
						subPart = ItemManager()->FindPart(setTid);
						if (subPart)
						{
							EmptyList(subPart->PosMask);
							for (int j = 0; j < m_partNum; j++)
								if (subPart->PosMask & (1 << j))
								{
									m_part[j].tid = setTid;
									m_part[j].pPart = subPart;
									if (IsLocalContent(S4_GUID_BASE))
										m_part[j].uid = 0;
									break;
								}
						}
						else
						{
							unsigned long rest = m_part[i].pPart->PosMask &
								(m_part[i].pPart->PosMask ^ part->PosMask);
							for (int j = 0; j < m_partNum; j++)
							{
								if (!(rest & (-1 << j)))
									break;
								if (rest & (1 << j))
								{
									unsigned long defTid = 0x8000400 |
										(((m_tidChar << 5) | j) << 13);
									subPart = ItemManager()->FindPart(defTid);
									if (subPart)
									{
										if (subPart->PosMask & part->PosMask)
											return false;
										EmptyList(subPart->PosMask);
										m_part[j].tid = defTid;
									}
									else
										m_part[j].tid = 0;
									m_part[j].pPart = subPart;
									if (IsLocalContent(S4_GUID_BASE))
										m_part[j].uid = 0;
								}
							}
						}
						break;
					}
					}
					EmptyList(overlap);
				}
			}
			if (!remain)
				break;
		}
	}

	if (!skip)
	{
		m_part[slot].tid = tid;
		m_part[slot].pPart = part;
		if (IsLocalContent(S4_GUID_BASE))
			m_part[slot].uid = uid;
	}

	unsigned long mask = 0;
	for (int i = 0; i < m_partNum; i++)
		if (m_part[i].pPart)
			mask |= m_part[i].pPart->PosMask;

	if (!skip)
	{
		unsigned long defTid = 0x8000600 | (((m_tidChar << 5) | slot) << 13);
		part = ItemManager()->FindPart(defTid);
		if (part)
		{
			unsigned long posMask = part->PosMask;
			if (!(posMask & mask))
			{
				int i;
				for (i = 0; i < m_defPartNum; i++)
					if (posMask & (1 << i))
						break;
				if (!m_part[i].tid)
				{
					m_part[i].tid = defTid;
					m_part[i].pPart = part;
					if (IsLocalContent(S4_GUID_BASE))
						m_part[i].uid = 0;
				}
			}
		}
	}

	for (int i = 0; i < m_defPartNum; i++)
	{
		if (!(mask & (1 << i)))
		{
			unsigned long defTid = 0x8000400 | (((m_tidChar << 5) | i) << 13);
			part = ItemManager()->FindPart(defTid);
			if (part && !(mask & part->PosMask))
			{
				m_part[i].tid = defTid;
				m_part[i].pPart = part;
				if (IsLocalContent(S4_GUID_BASE))
					m_part[i].uid = 0;
			}
		}
	}

	return true;
}

void CPartList::EmptyList(unsigned long mask)
{
	for (int i = 0; i < m_partNum; i++)
	{
		if (!(mask & (-1 << i)))
			break;

		if (mask & (1 << i))
		{
			m_part[i].tid = 0;
			m_part[i].pPart = NULL;
			if (IsLocalContent(S4_GUID_BASE))
				m_part[i].uid = 0;
		}
	}
}

bool CPartList::CheckSubParts()
{
	int i;
	for (i = 0; i < m_partNum; i++)
	{
		sPart* current = &m_part[i];
		unsigned long tid = current->tid;
		if (tid && (tid & 0x600) == 0x200)
		{
			int j;
			for (j = 0; j < m_partNum; j++)
				if (m_part[j].pPart && i != j)
				{
					int k;
					for (k = 0; k < 2; k++)
						if (m_part[j].pPart->SubPart[k] == tid)
							break;
					if (k < 2)
						break;
				}
			if (j >= m_partNum)
			{
				if (!SetPart(tid, current->uid))
					return false;
			}
			else if (i >= m_defPartNum)
			{
				if (current->pPart && !IsEmpty(current->pPart->PosMask, i))
				{
					m_part[i].tid = 0;
					m_part[i].pPart = NULL;
					if (IsLocalContent(S4_GUID_BASE))
						m_part[i].uid = 0;
				}
			}
		}
	}

	for (i = 0; i < m_partNum; i++)
	{
		if (m_part[i].pPart)
		{
			for (int j = 0; j < 2; j++)
			{
				unsigned long tid = m_part[i].pPart->SubPart[j];
				if (tid)
				{
					unsigned char slot = (tid >> 13) & 31;
					if (slot < m_defPartNum)
					{
						if ((m_part[slot].tid & 0x600) == 0x400 &&
							!SetPart(tid, m_part[i].uid))
							return false;
					}
					else
					{
						IFF_STRUCT::sPart* part = ItemManager()->FindPart(tid);
						if (part && IsEmpty(part->PosMask, 255) &&
							!SetPart(m_part[i].pPart->SubPart[j],
								m_part[i].uid))
							return false;
					}
				}
			}
		}
	}

	return true;
}

bool CPartList::IsEmpty(unsigned long mask, unsigned char except)
{
	for (int i = 0; i < m_partNum; i++)
		if (m_part[i].pPart && except != i && (mask & m_part[i].pPart->PosMask))
			return false;
	return true;
}

bool CPartList::SetHairClr(CPartTidList* tids, unsigned char color)
{
	if (m_tidChar != tids->m_charTid)
		return false;
	if (tids->m_hairColor == color)
		return false;
	tids->m_hairColor = color;
	m_hairColor = color;
	return true;
}

bool CPartList::SetShirtsClr(CPartTidList* tids, unsigned char color)
{
	if (m_tidChar != tids->m_charTid)
		return false;
	if (tids->m_shirtsColor == color)
		return false;
	tids->m_shirtsColor = color;
	m_shirtsColor = color;
	return true;
}

bool CPartList::IsIdentical(CPartTidList* tids)
{
	if (m_tidChar != tids->m_charTid || m_defPartNum != tids->m_defPartNum ||
		m_partNum != tids->m_partNum)
		return false;
	for (int i = 0; i < m_partNum; i++)
		if (m_part[i].tid != tids->m_tid[i])
			return false;
	if (IsLocalContent(S4_GUID_BASE))
		for (int i = 0; i < m_partNum; i++)
			if (m_part[i].uid && tids->m_uid[i] &&
				m_part[i].uid != tids->m_uid[i])
				return false;
	return true;
}

bool CPartList::FindPart(unsigned long tid) const
{
	for (unsigned char i = 0; i < GetNumTotalParts(); i++)
		if (GetPart(i)->tid == tid)
			return true;
	return false;
}

CPetFrame::CPetFrame()
	: m_tidChar(0),
	  m_pBonePet(NULL),
	  m_hairTex(0),
	  m_shirtsTex(0),
	  m_faceTex(0),
	  m_pCurAniPet(NULL),
	  m_defPartNum(0),
	  m_partNum(0),
	  m_makeupIndex(1),
	  m_bShirtsClr(true),
	  m_bMakeup(false)
{
	memset(m_pPartPet, 0, sizeof(m_pPartPet));
	memset(m_partTex, 0, sizeof(m_partTex));
	memset(m_auxPart, 0, sizeof(m_auxPart));
}

CPetFrame::~CPetFrame()
{
	ReleasePets(7);
}

void CPetFrame::ReleasePets(int mask)
{
	if ((mask & 1) && g_resrcmng && m_pBonePet)
	{
		g_resrcmng->Release(m_pBonePet, 0);
		m_pBonePet = NULL;
	}

	if (mask & 2)
	{
		for (std::vector<WPuppet*>::iterator it = m_aniPetList.begin();
			it != m_aniPetList.end(); ++it)
			if (*it)
				g_resrcmng->Release(*it, 0);
		m_aniPetList.clear();
	}

	if (mask & 4)
	{
		for (int i = 0; i < m_partNum; i++)
		{
			if (g_resrcmng && m_pPartPet[i])
			{
				g_resrcmng->Release(m_pPartPet[i], 0);
				m_pPartPet[i] = NULL;
			}
			for (int j = 0; j < 3; j++)
				if (g_resrcmng && m_partTex[i][j])
				{
					g_resrcmng->Release(m_partTex[i][j]);
					m_partTex[i][j] = 0;
				}
		}
		if (m_partNum)
		{
			memset(m_pPartPet, 0, sizeof(m_pPartPet));
			memset(m_partTex, 0, sizeof(m_partTex));
			m_partNum = 0;
			m_defPartNum = 0;
		}
		if (g_resrcmng && m_faceTex)
		{
			g_resrcmng->Release(m_faceTex);
			m_faceTex = 0;
		}
		if (g_resrcmng && m_hairTex)
		{
			g_resrcmng->Release(m_hairTex);
			m_hairTex = 0;
		}
		if (g_resrcmng && m_shirtsTex)
		{
			g_resrcmng->Release(m_shirtsTex);
			m_shirtsTex = 0;
		}
	}
}

bool CPetFrame::Build(const CPartTidList* tids, unsigned long* aux, bool color,
	bool aniPet, int face)
{
	if (!m_partList.Build(tids))
		return false;
	bool same = tids->m_charTid == m_tidChar;
	if (!same)
		ReleasePets(7);
	m_tidChar = tids->m_charTid;
	m_defPartNum = tids->m_defPartNum;
	m_partNum = tids->m_partNum;
	IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_tidChar);
	if (!character)
		return false;
	char boneName[128], aniName[128];
	sprintf(boneName, "%s.bpet", character->Data);

	if (!(same && m_pBonePet) &&
		!(m_pBonePet = g_resrcmng->GetPuppet(boneName, 0, 0, 0)))
	{
		g_exceptionReport.Crash();
		return false;
	}

	sprintf(aniName, "%s.apet", character->Data);
	AddAniPet(aniName);
	SetPartPets(-1);

	if (aux)
		memcpy(m_auxPart, aux, sizeof(m_auxPart));
	else
		memset(m_auxPart, 0, sizeof(m_auxPart));

	char texture[256];
	strcpy(texture, character->FaceTex);
	sprintf(strrchr(texture, '.'), "_%d%s", face,
		strrchr(character->FaceTex, '.'));
	m_faceTex = g_resrcmng->LoadTexture(character->FaceTex, 0, 0, texture);
	w_texlist* tex = g_resrcmng->FindTexture(character->FaceTex);
	if (tex)
		m_pBonePet->ChangeTextureByHandle(m_faceTex, tex->texhandle, 0);
	EndSetPart();

	if (aniPet)
	{
		m_pBonePet->xUpdateTnLBuffers(true);
	}

	if (!color)
	{
		ReleasePets(2);
	}
	else
	{
		SetHairClr(const_cast<CPartTidList*>(tids), tids->m_hairColor);
		SetShirtsClr(NULL, tids->m_shirtsColor);
		m_bShirtsClr = true;
	}

	return true;
}

void CPetFrame::SetPartPets(unsigned long mask)
{
	int shirtsTex = 0;
	m_bShirtsClr = false;
	if (mask == -1)
	{
		m_bMakeup = false;
		m_makeupIndex = 1;
	}

	IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_tidChar);
	if (character->ShirtsTex[0])
		shirtsTex = g_resrcmng->LoadTexture(character->ShirtsTex, 0, 0, 0);
	int textures[3], originals[3];
	for (int i = 0; i < m_partNum; i++)
	{
		if (!(mask & (-1 << i)))
			break;
		if (mask & (1 << i))
		{
			memset(textures, 0, sizeof(textures));
			memset(originals, 0, sizeof(originals));
			IFF_STRUCT::sPart* part = m_partList.GetPart(i)->pPart;
			if (part)
			{
				if ((part->c.TypeId & 0x1800) == 0x1000)
				{
					m_makeupIndex = i;
					m_bMakeup = true;
				}
				else if (part->PosMask & 2)
				{
					m_makeupIndex = 1;
					m_bMakeup = false;
				}
				char name[32];
				sprintf(name, "%s.mpet", part->Data);
				if (IsLocalContent(S4_UCC) && IsLocalContent(S4_GUID_BASE) &&
					IsUccClothes(part->c.TypeId))
				{
					unsigned char partIndex = i;
					char index[9];
					memset(index, 0, sizeof(index));
					sUccClothes* clothes = UccManager()->FindClothes(
						m_partList.GetPart(partIndex)->uid);
					if (clothes)
					{
						strncpy(index, clothes->uccIndex, 9);
						if (UccManager()->IsDownloadComplete(clothes))
						{
							textures[0] = g_resrcmng->LoadTexture(
								GenerateTextureName_F(part->c.TypeId, index)
									.c_str(),
								0, 0, 0);
							originals[0] =
								g_resrcmng->LoadTexture(part->Tex[0], 0, 0, 0);
							textures[1] = g_resrcmng->LoadTexture(
								GenerateTextureName_B(part->c.TypeId, index)
									.c_str(),
								0, 0, 0);
							originals[1] =
								g_resrcmng->LoadTexture(part->Tex[1], 0, 0, 0);
							if (!textures[0])
								textures[0] = g_resrcmng->LoadTexture(
									part->Tex[0], 0, 0, 0);
							if (!textures[1])
								textures[1] = g_resrcmng->LoadTexture(
									part->Tex[1], 0, 0, 0);
						}
						else
						{
							for (int j = 0; j < 3; j++)
							{
								if (part->Tex[j][0] && part->OrgTex[j][0])
								{
									textures[j] = g_resrcmng->LoadTexture(
										part->Tex[j], 0, 0, 0);
									originals[j] = g_resrcmng->LoadTexture(
										part->OrgTex[j], 0, 0, 0);
								}
							}
						}
					}
					else
					{
						sItemInfo* item =
							GetMyItemInfo(m_partList.GetPart(partIndex)->uid);
						if (item)
							strncpy(index, item->UccIndex, 9);
						bool found = false;
						ITEM_LIST_IT it, end = Doc()->m_uccItemList.end();
						for (it = Doc()->m_uccItemList.begin(); it != end; it++)
						{
							if (it->tid == part->c.TypeId && strlen(index) &&
								!strcmp(it->UccIndex, index) &&
								!(it->status & 1))
							{
								found = true;
								break;
							}
						}
						if (found)
						{
							textures[0] = g_resrcmng->LoadTexture(
								GenerateTextureName_F(part->c.TypeId, index)
									.c_str(),
								0, 0, 0);
							originals[0] =
								g_resrcmng->LoadTexture(part->Tex[0], 0, 0, 0);
							textures[1] = g_resrcmng->LoadTexture(
								GenerateTextureName_B(part->c.TypeId, index)
									.c_str(),
								0, 0, 0);
							originals[1] =
								g_resrcmng->LoadTexture(part->Tex[1], 0, 0, 0);
						}
						else
						{
							for (int j = 0; j < 3; j++)
							{
								if (part->Tex[j][0] && part->OrgTex[j][0])
								{
									textures[j] = g_resrcmng->LoadTexture(
										part->Tex[j], 0, 0, 0);
									originals[j] = g_resrcmng->LoadTexture(
										part->OrgTex[j], 0, 0, 0);
								}
							}
						}
					}
				}
				else
				{
					for (int j = 0; j < 3; j++)
					{
						if (part->Tex[j][0] && part->OrgTex[j][0])
						{
							textures[j] =
								g_resrcmng->LoadTexture(part->Tex[j], 0, 0, 0);
							originals[j] = g_resrcmng->LoadTexture(
								part->OrgTex[j], 0, 0, 0);
						}
					}
				}
				if (m_pPartPet[i] &&
					!stricmp(m_pPartPet[i]->GetPuppetName(), name))
				{
					for (int j = 0; j < 3; j++)
						m_pBonePet->ChangeTextureByHandle(textures[j],
							originals[j], m_pPartPet[i]);
				}
				else
				{
					WPuppet* pet = g_resrcmng->GetPuppet(name, 0, 1, 0);
					if (pet)
					{
						m_pBonePet->ReplaceMeshBones(m_pPartPet[i], pet,
							textures[0], originals[0]);
						for (int j = 1; j < 3; j++)
							if (textures[j] && originals[j])
								m_pBonePet->ChangeTextureByHandle(textures[j],
									originals[j], pet);
						if (m_pPartPet[i])
							g_resrcmng->Release(m_pPartPet[i], 0);
						m_pPartPet[i] = pet;
					}
					else
					{
						CTask* task = AfxGetTask();
						if (task)
						{
							CGolfDoc* doc = (CGolfDoc*)task->GetDocument();
							if (doc)
							{
								char* copy = new char[strlen(name) + 1];
								strcpy(copy, name);
								doc->m_modelNameList.AddItem(copy, copy, false);
							}
						}
					}
				}
				for (int j = 0; j < 3; j++)
				{
					if (originals[j] == shirtsTex &&
						originals[j] == textures[j])
						m_bShirtsClr = true;
					if (g_resrcmng && originals[j])
					{
						g_resrcmng->Release(originals[j]);
						originals[j] = 0;
					}
				}
			}
			else
			{
				m_pBonePet->ReplaceMeshBones(m_pPartPet[i], 0, 0, 0);
				if (g_resrcmng && m_pPartPet[i])
				{
					g_resrcmng->Release(m_pPartPet[i], 0);
					m_pPartPet[i] = 0;
				}
			}
			for (int j = 0; j < 3; j++)
			{
				if (g_resrcmng && m_partTex[i][j])
				{
					g_resrcmng->Release(m_partTex[i][j]);
					m_partTex[i][j] = 0;
				}
				m_partTex[i][j] = textures[j];
			}
		}
	}

	int faceIndex = -1, maskIndex = -1;
	switch (character->c.TypeId & 0x3ffffff)
	{
	case 2:
	case 7:
		maskIndex = 19;
		faceIndex = 1;
		break;
	case 4:
		maskIndex = 20;
		faceIndex = 1;
		break;
	}

	bool maskPresent = false, makeup = false;
	if (maskIndex >= 0 && m_partList.GetPart(maskIndex) &&
		m_partList.GetPart(maskIndex)->tid)
		maskPresent = true;

	const CPartList& parts = m_partList;
	if (faceIndex >= 0 && parts.GetPart(faceIndex) &&
		(m_partList.GetPart(faceIndex)->tid & 0x1800) == 0x1000)
		makeup = true;

	if (maskPresent)
	{
		if (faceIndex >= 0)
		{
			m_pBonePet->ReplaceMeshBones(m_pPartPet[faceIndex], 0, 0, 0);
			if (g_resrcmng && m_pPartPet[faceIndex])
			{
				g_resrcmng->Release(m_pPartPet[faceIndex], 0);
				m_pPartPet[faceIndex] = 0;
			}
		}
	}
	else if (faceIndex >= 0 && m_partList.GetPart(faceIndex))
	{
		IFF_STRUCT::sPart* part = m_partList.GetPart(faceIndex)->pPart;
		if (part)
		{
			char name[32] = { 0 };
			sprintf(name, "%s.mpet", part->Data);
			for (int j = 0; j < 3; j++)
			{
				textures[j] = 0;
				originals[j] = 0;
				if (part->Tex[j][0] && part->OrgTex[j][0])
				{
					textures[j] =
						g_resrcmng->LoadTexture(part->Tex[j], 0, 0, 0);
					originals[j] =
						g_resrcmng->LoadTexture(part->OrgTex[j], 0, 0, 0);
				}
			}
			WPuppet* pet = g_resrcmng->GetPuppet(name, 0, 1, 0);
			if (pet)
			{
				m_pBonePet->ReplaceMeshBones(m_pPartPet[faceIndex], pet,
					textures[0], originals[0]);
				for (int j = 1; j < 3; j++)
					if (textures[j] && originals[j])
						m_pBonePet->ChangeTextureByHandle(textures[j],
							originals[j], pet);
				if (m_pPartPet[faceIndex])
					g_resrcmng->Release(m_pPartPet[faceIndex], 0);
				m_pPartPet[faceIndex] = pet;
			}
		}
	}

	if (makeup)
	{
		m_makeupIndex = faceIndex;
		m_bMakeup = true;
		if (g_resrcmng && m_faceTex)
		{
			g_resrcmng->Release(m_faceTex);
			m_faceTex = 0;
		}
		char name[256];
		strcpy(name, character->FaceTex);
		sprintf(strrchr(name, '.'), "_%d%s", faceIndex,
			strrchr(character->FaceTex, '.'));
		m_faceTex = g_resrcmng->LoadTexture(character->FaceTex, 0, 0, name);
		w_texlist* tex = g_resrcmng->FindTexture(character->FaceTex);
		if (tex)
			m_pBonePet->ChangeTextureByHandle(m_faceTex, tex->texhandle, 0);
	}

	if (g_resrcmng && shirtsTex)
		g_resrcmng->Release(shirtsTex);
}

char* CPetFrame::GetMakeupTexture()
{
	return m_partList.GetPart((unsigned char)m_makeupIndex)->pPart->Tex[0];
}

WPuppet* CPetFrame::GetBonePet()
{
	return m_pBonePet;
}

bool CPetFrame::Refresh(CPartTidList* tids)
{
	if (!ResetFace(tids))
		return false;
	if (!SetMakeup())
		return false;
	return true;
}

bool CPetFrame::ResetFace(CPartTidList* tids)
{
	IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_tidChar);
	if (!character)
		return false;
	g_resrcmng->ChangeTexturePart(m_faceTex, character->FaceTex, NULL, NULL);
	SetHairClr(tids, tids->m_hairColor);
	return true;
}

void CPetFrame::AddAniPet(char* name)
{
	for (std::vector<WPuppet*>::iterator it = m_aniPetList.begin();
		it != m_aniPetList.end(); ++it)
		if (!stricmp((*it)->GetPuppetName(), name))
			return;
	WPuppet* pet = g_resrcmng->GetPuppet(name, 0, 1, 0);
	if (pet)
		m_aniPetList.push_back(pet);
}

std::vector<WPuppet*>& CPetFrame::GetAniPetList()
{
	return m_aniPetList;
}

w_motion_data* CPetFrame::FindMotionData(const char* name, bool select,
	WPuppet** out)
{
	for (std::vector<WPuppet*>::iterator it = m_aniPetList.begin();
		it != m_aniPetList.end(); ++it)
	{
		w_motion_data* motion = (*it)->FindMotionData(name);
		if (motion)
		{
			if (out)
				*out = *it;
			if (select)
				m_pCurAniPet = *it;
			return motion;
		}
	}

	return NULL;
}

int CPetFrame::GetMotionCountIncludeName(const char* name, const char* include)
{
	int result = 0;
	for (std::vector<WPuppet*>::iterator it = m_aniPetList.begin();
		it != m_aniPetList.end(); ++it)
	{
		result = (*it)->GetMotionCountIncludeName(name, include);
		if (result > 0)
			return result;
	}

	return result;
}

WPuppet* CPetFrame::GetCurAniPet()
{
	return m_pCurAniPet;
}

void CPetFrame::ApplyBones(WBoneSet* bones, const WMatrix& matrix)
{
	m_pBonePet->ApplyBones(bones, matrix);
}

void CPetFrame::Transform(WView* view, float delta, int flag)
{
	m_pBonePet->Transform(view, delta, flag);
}

void CPetFrame::UpdateBound(bool force)
{
	m_pBonePet->UpdateBound(force, false);
}

void CPetFrame::UpdateLightSource(LightSet* light, bool update, WScene* scene)
{
	m_pBonePet->UpdateLightSource(light, update, scene);
}

void CPetFrame::SetAlpha(unsigned char alpha)
{
	m_pBonePet->SetAlpha(alpha);
}

void CPetFrame::SetAlpha(char* name, unsigned char alpha)
{
	m_pBonePet->SetAlpha(name, alpha);
}

void CPetFrame::Render(WView* view, bool shadow, float alpha, bool b1, bool b2,
	bool b3)
{
	m_pBonePet->Render(view, shadow, alpha, b1, b2, b3);
}

bool CPetFrame::HasMPet(char* name)
{
	for (int i = 0; i < m_partNum; i++)
		if (m_pPartPet[i] && !stricmp(m_pPartPet[i]->GetPuppetName(), name))
			return true;
	return false;
}

void CPetFrame::SetInvisible(bool invisible)
{
	if (m_pBonePet)
	{
		m_pBonePet->GetRootBone()->SetInvisible(true, invisible);
		if (!invisible)
			ApplyHideMask(true);
	}
}

unsigned long CPetFrame::SetPart(CPartTidList* tids, unsigned long tid,
	unsigned long uid)
{
	ApplyHideMask(false);

	unsigned long mask = m_partList.SetPart(tids, tid, uid);
	if (mask)
	{
		SetPartPets(mask);
		SetHairClr(NULL, tids->m_hairColor);
		SetShirtsClr(NULL, tids->m_shirtsColor);
		m_bShirtsClr = true;
		EndSetPart();
		return mask;
	}

	return 0;
}

bool CPetFrame::SetAuxPart(unsigned char index, unsigned long tid)
{
	if (index >= 5)
		return false;
	m_auxPart[index] = tid;
	return true;
}

bool CPetFrame::FindAuxPart(unsigned long tid)
{
	for (int i = 0; i < 5; i++)
		if (m_auxPart[i] == tid)
			return true;
	return false;
}

FIXHAIR::sFixItem FIXHAIR::exTable[3] = {
	{ 0, 134242353, "m_hair_02-01.jpg"   },
    { 0, 134242354, "m_hair_02-01.jpg"   },
	{ 5, 135692297, "2#e_hair_07-07.jpg" }
};

int IsFixedHairStyle(unsigned long tid)
{
	for (int i = 0; i < 3; i++)
		if (FIXHAIR::exTable[i].tid == tid)
			return i;
	return -1;
}

bool CPetFrame::SetHairClr(CPartTidList* tids, unsigned char color)
{
	char texture[40] = { 0 };
	bool fixed = false;
	char fixedTexture[50] = { 0 };

	if (tids)
	{
		int i;
		for (i = 0; i < tids->m_partNum; i++)
		{
			int index = IsFixedHairStyle(tids->m_tid[i]);
			if (index != -1)
			{
				fixed = true;
				strcpy(fixedTexture, FIXHAIR::exTable[index].texture.c_str());
			}
			if (((tids->m_tid[i] >> 13) & 31) ==
				g_iIndexWigColor[tids->m_charTid & 0x3ffffff])
			{
				IFF_STRUCT::sPart* part =
					ItemManager()->FindPart(tids->m_tid[i]);
				if (part)
				{
					int oldTex =
						g_resrcmng->LoadTexture(part->OrgTex[0], 0, 0, 0);
					if (fixed && fixedTexture[0])
						strncpy(texture, fixedTexture, 39);
					else
						strcpy(texture, part->OrgTex[0]);
					char* ext = strrchr(texture, '.');
					if (!ext)
						return false;
					ext -= 2;
					int num = atoi(ext);
					if (fixed)
						sprintf(ext, "%02d.jpg", num);
					else
						sprintf(ext, "%02d.jpg", num + color);
					int newTex = g_resrcmng->LoadTexture(texture, 0, 0, 0);
					m_pBonePet->ChangeTextureByHandle(newTex, oldTex, 0);
				}
				break;
			}
		}
		if (m_tidChar != tids->m_charTid)
			return false;
	}

	IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_tidChar);
	int oldTex;
	if (fixed == true && fixedTexture[0])
		oldTex = g_resrcmng->LoadTexture(fixedTexture, 0, 0, 0);
	else
		oldTex = g_resrcmng->LoadTexture(character->HairTex, 0, 0, 0);

	strcpy(texture, character->HairTex);
	if (fixed == true && fixedTexture[0])
		strncpy(texture, fixedTexture, 39);

	char* ext = strrchr(texture, '.');
	if (!ext)
		return false;
	ext -= 2;

	int num = atoi(ext);
	if (fixed == true)
		sprintf(ext, "%02d.jpg", num);
	else
		sprintf(ext, "%02d.jpg", num + color);

	int newTex = g_resrcmng->LoadTexture(texture, 0, 0, 0);
	m_pBonePet->ChangeTextureByHandle(newTex, oldTex, 0);
	if (g_resrcmng && oldTex)
		g_resrcmng->Release(oldTex);
	if (m_hairTex)
		g_resrcmng->Release(m_hairTex);
	m_hairTex = newTex;

	if ((character->c.TypeId & 0x3ffffff) > 2)
	{
		char base[40], name[40];
		strcpy(base, character->FaceTex);
		*strrchr(base, '.') = 0;
		sprintf(name, "%s_hr%02d.tga", base, color + 1);
		Bitmap* face = g_resrcmng->LoadBitmapA(character->FaceTex, 0, 0);
		Bitmap* hair = g_resrcmng->LoadBitmapA(name, 0, 0);
		if (face)
		{
			if (hair)
			{
				Bitmap* combined = g_resrcmng->Combine(face, hair);
				RECT rect = { 0, 0, combined->Width(), combined->Height() };
				g_resrcmng->ChangeTexturePart(m_faceTex, combined, rect);
				delete face;
				delete hair;
				delete combined;
			}
			else
				delete face;
		}
		else if (hair)
			delete hair;
		w_texlist* tex = g_resrcmng->FindTexture(character->FaceTex);
		if (tex)
			m_pBonePet->ChangeTextureByHandle(m_faceTex, tex->texhandle, 0);
	}

	return true;
}

bool CPetFrame::SetShirtsClr(CPartTidList* tids, unsigned char color)
{
	if (!m_bShirtsClr)
		return true;

	if (tids)
	{
		if (m_tidChar != tids->m_charTid)
			return false;
		if (!m_partList.SetShirtsClr(tids, color))
			return false;
	}

	IFF_STRUCT::sChar* character = ItemManager()->FindChar(m_tidChar);
	if (!character->ShirtsTex[0])
		return false;
	char* name = character->ShirtsTex;
	int oldTex = g_resrcmng->LoadTexture(name, 0, 0, 0);
	char* ext = strrchr(name, '.');
	if (!ext)
		return false;
	ext--;
	*ext = color + '1';
	int newTex = g_resrcmng->LoadTexture(name, 0, 0, 0);
	m_pBonePet->ChangeTextureByHandle(newTex, oldTex, 0);
	*ext = '1';

	if (g_resrcmng && oldTex)
		g_resrcmng->Release(oldTex);
	if (m_shirtsTex)
		g_resrcmng->Release(m_shirtsTex);

	m_shirtsTex = newTex;
	return true;
}

bool CPetFrame::SetMakeup()
{
	if (m_bMakeup)
	{
		IFF_STRUCT::sPart* part =
			m_partList.GetPart((unsigned char)m_makeupIndex)->pPart;
		if (!m_pBonePet->ChangeTexturePart(m_faceTex, part->Tex[0], NULL, NULL))
			return false;
	}

	return true;
}

void CPetFrame::CalcBVolume()
{
	if (!m_pBonePet)
		return;

	std::vector<WPuppet*>::iterator it = m_aniPetList.begin();
	if (it == m_aniPetList.end())
		m_pBonePet->CalcBound(NULL, true);
	else
	{
		m_pBonePet->CalcBound(true, *it, true, NULL, NULL);
		for (++it; it != m_aniPetList.end(); ++it)
			m_pBonePet->CalcBound(false, *it, true, NULL, NULL);
	}
}

void CPetFrame::EndSetPart()
{
	m_pBonePet->EndMeshBoneWork();
	ApplyHideMask(true);
}

void CPetFrame::ApplyHideMask(bool apply)
{
	char boneName[256];
	for (int i = 0; i < m_partNum; i++)
	{
		if (m_partList.m_part[i].tid)
		{
			IFF_STRUCT::sPart* part = m_partList.m_part[i].pPart;
			if (part && part->HideMask)
			{
				for (int j = 0; j < m_partNum; j++)
				{
					if ((part->HideMask & (1 << j)) && i != j &&
						m_partList.m_part[j].tid &&
						m_partList.m_part[j].pPart && m_pPartPet[j])
					{
						for (WBone* mesh = m_pPartPet[j]->FindMeshBone(NULL);
							mesh; mesh = m_pPartPet[j]->FindMeshBone(mesh))
						{
							WBone* bone =
								m_pBonePet->FindBone(mesh->GetBoneName());
							if (bone)
								bone->SetInvisible(false, apply);
						}
					}
				}
			}
		}
	}
}

bool CPetFrame::ChangeTexturePart(char* anim, char* tex, Bitmap** bitmap,
	tagRECT* rect)
{
	w_face_animation* face = FindFaceAnim(anim);
	if (!face)
		return false;
	return g_resrcmng->ChangeTexturePart(face->filename, tex, bitmap, rect);
}

void CPetFrame::ChangeTexturePart(char* anim, Bitmap* bitmap,
	const tagRECT& rect)
{
	w_face_animation* face = FindFaceAnim(anim);
	if (face)
		g_resrcmng->ChangeTexturePart(face->filename, bitmap, rect);
}

void CPetFrame::ChangeTexture(char* from, char* to)
{
	int oldTex = g_resrcmng->LoadTexture(from, 0, 0, 0);
	int newTex = g_resrcmng->LoadTexture(to, 0, 0, 0);
	if (oldTex && newTex)
		m_pBonePet->ChangeTextureByHandle(newTex, oldTex, 0);
}

w_face_animation* CPetFrame::FindFaceAnim(char* name)
{
	w_face_animation* result = NULL;

	for (int i = 0; i < m_partNum; i++)
	{
		if (m_pPartPet[i])
		{
			result = m_pPartPet[i]->FindFaceAnim(name);
			if (result)
				break;
		}
	}

	return result;
}
