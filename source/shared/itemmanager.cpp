#include "minatl.h"
#include <atltime.h>
#include <sstream>

#include "itemmanager.h"
#include "tikimagicboxtable.h"
#include "localize.h"
#include "cardsystem.h"

#include "recycletable.h"

namespace S5
{
	bool ISCutinItem(unsigned long typeId);
}

struct IFF_FILE_HEADER
{
	unsigned short nRecords;
	unsigned int Version;
};

void DevFinalStockPrice(IFF_ITEM_COMMON& c)
{
	if (c.Final && c.Price >= 1000000)
	{
		if (c.InStock != 0 && c.InStock != 4)
		{
			char szMsg[128] = { 0 };
			sprintf(szMsg,
				"\xbb\xf3\xc1\xa1 \xc1\xf8\xbf\xad\xbb\xf3\xc7\xb0 "
				"\xb0\xa1\xb0\xdd\xc0\xcc\xbb\xf3(%s)",
				c.Name);
			MessageBox(NULL, szMsg, "PangYa", MB_OK);
			LogOut(0, szMsg);

			c.IsSalable = 0;
			c.InStock = 0;
		}
	}
}

void DevRentalPreventSale(IFF_STRUCT::sPart& part)
{
	if (part.c.Final && part.c.IsSalable != 0)
	{
		if (part.RentalPrice > 0)
		{
			char szMsg[128] = { 0 };
			sprintf(szMsg,
				"\xb0\xc5\xb7\xa1\xb0\xa1 \xb5\xc7\xb4\xc2 \xb7\xa3\xc5\xbb "
				"\xc6\xc4\xc3\xf7 \xb9\xdf\xb0\xdf(%s)",
				part.c.Name);
			MessageBox(NULL, szMsg, "PangYa", MB_OK);
			LogOut(0, szMsg);

			part.RentalPrice = 0;
		}
	}
}

CItemManager::CItemManager()
{
	m_IsReloading = false;
}

CItemManager::~CItemManager()
{
	Reset();
}

void CItemManager::Reload()
{
	_client::_private::CLock<_client::CCriticalSection> lock(m_cs);

	DWORD dwTick = GetTickCount();

	m_IsReloading = true;
	Reset();
	Load();
	m_IsReloading = false;
}

void CItemManager::Reset()
{
	m_CharMap.clear();
	m_PartMap.clear();
	m_ClubMap.clear();
	m_ClubSetMap.clear();
	m_BallMap.clear();
	m_ItemMap.clear();
	m_CaddieMap.clear();
	m_CadItemMap.clear();
	m_SetItemMap.clear();
	m_CourseMap.clear();
	m_MatchMap.clear();
	m_TitleMap.clear();
	m_EnchantMap.clear();
	m_SkinMap.clear();
	m_HairStyleMap.clear();
	m_ChildItemMap.clear();
	m_MascotMap.clear();
	m_DescMap.clear();
	m_AuxPartMap.clear();
	m_QuestDropMap.clear();
	m_FurnitureMap.clear();
	m_CardMap.clear();
	m_OfflineShopMap.clear();
	m_RandomRecycleMaps.clear();
	m_CadieMagicBox.clear();
	m_FurnitureAbility.clear();

	m_CutinInformationMap.clear();

	for (std::map<unsigned int, IFF_STRUCT::sQuest>::iterator itr =
			 m_QuestMap.begin();
		itr != m_QuestMap.end(); ++itr)
	{
		IFF_STRUCT::sQuest* pQuest = &(*itr).second;

		if (pQuest && pQuest->aTidItems)
		{
			delete[] pQuest->aTidItems;
			pQuest->aTidItems = NULL;
		}
	}

	m_QuestMap.clear();

	m_IsReloading = false;

	if (g_ArrayTidMixItem)
	{
		for (int i = 0; i < g_NumIndexRecycleItem; i++)
		{
			if (g_ArrayTidMixItem[i])
			{
				delete[] g_ArrayTidMixItem[i];
				g_ArrayTidMixItem[i] = NULL;
			}
		}

		delete[] g_ArrayTidMixItem;
		g_ArrayTidMixItem = NULL;
	}

	if (g_ArrayRecycleItem)
	{
		delete[] g_ArrayRecycleItem;
		g_ArrayRecycleItem = NULL;
	}
}

struct ChangeWordPair
{
	std::string from;
	std::string to;
};

ChangeWordPair ChangeWordList[] = {
	{ "'", "`" },
};

int ChangeWordListCount = sizeof(ChangeWordList) / sizeof(ChangeWordPair);

template <class T>
void CheckCommonItemName(T& data)
{
	std::string str(data.second.c.Name);

	for (int i = 0; i < ChangeWordListCount; i++)
	{
		if (str_Replace(str, ChangeWordList[i].from, ChangeWordList[i].to) ==
			true)
		{
			if (str.length() >= sizeof(data.second.c.Name) - 1)
			{
				strncpy(data.second.c.Name, str.c_str(),
					sizeof(data.second.c.Name) - 1);
				data.second.c.Name[sizeof(data.second.c.Name) - 1] = 0;
			}
			else
			{
				strcpy(data.second.c.Name, str.c_str());
			}
		}
	}
}

template <class T>
void CheckItemName(T& data)
{
	std::string str(data.second.Name);

	for (int i = 0; i < ChangeWordListCount; i++)
	{
		if (str_Replace(str, ChangeWordList[i].from, ChangeWordList[i].to) ==
			true)
		{
			if (str.length() >= sizeof(data.second.Name) - 1)
			{
				strncpy(data.second.Name, str.c_str(),
					sizeof(data.second.Name) - 1);
				data.second.Name[sizeof(data.second.Name) - 1] = 0;
			}
			else
			{
				strcpy(data.second.Name, str.c_str());
			}
		}
	}
}

void CItemManager::CheckInvalidItemName()
{
	std::for_each(m_CharMap.begin(), m_CharMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sChar> >);
	std::for_each(m_PartMap.begin(), m_PartMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sPart> >);
	std::for_each(m_ClubMap.begin(), m_ClubMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sClub> >);
	std::for_each(m_ClubSetMap.begin(), m_ClubSetMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sClubSet> >);
	std::for_each(m_BallMap.begin(), m_BallMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sBall> >);
	std::for_each(m_ItemMap.begin(), m_ItemMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sItem> >);
	std::for_each(m_CaddieMap.begin(), m_CaddieMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sCaddie> >);
	std::for_each(m_CadItemMap.begin(), m_CadItemMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sCadItem> >);
	std::for_each(m_SetItemMap.begin(), m_SetItemMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sSetItem> >);
	std::for_each(m_SkinMap.begin(), m_SkinMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sSkin> >);
	std::for_each(m_HairStyleMap.begin(), m_HairStyleMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sHairStyle> >);
	std::for_each(m_MascotMap.begin(), m_MascotMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sMascot> >);
	std::for_each(m_AuxPartMap.begin(), m_AuxPartMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sAuxPart> >);
	std::for_each(m_QuestDropMap.begin(), m_QuestDropMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sQuestDrop> >);
	std::for_each(m_FurnitureMap.begin(), m_FurnitureMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sFurniture> >);
	std::for_each(m_CardMap.begin(), m_CardMap.end(),
		CheckCommonItemName<std::pair<const unsigned int, IFF_STRUCT::sCard> >);
	std::for_each(m_OfflineShopMap.begin(), m_OfflineShopMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sOfflineShop> >);
	std::for_each(m_CourseMap.begin(), m_CourseMap.end(),
		CheckCommonItemName<
			std::pair<const unsigned int, IFF_STRUCT::sCourse> >);

	std::for_each(m_MatchMap.begin(), m_MatchMap.end(),
		CheckItemName<std::pair<const unsigned int, IFF_STRUCT::sMatch> >);
	std::for_each(m_TitleMap.begin(), m_TitleMap.end(),
		CheckItemName<std::pair<const unsigned int, IFF_STRUCT::sTitle> >);
	std::for_each(m_ChildItemMap.begin(), m_ChildItemMap.end(),
		CheckItemName<std::pair<const unsigned int, IFF_STRUCT::sChildItem> >);
}

bool CItemManager::Load()
{
	std::stringstream path;

	path << "PangYa";

	path << ".iff";

	WFile file(path.str().c_str());

	unsigned int len = file.Length();
	unsigned char* buff = new unsigned char[len];

	file.Read(buff, len);
	file.Close();

	HZIP hz = OpenZipU(buff, len, ZIP_MEMORY);
	if (!hz)
	{
		delete[] buff;
		return false;
	}

	Reset();

	ZIPENTRY ze;
	GetZipItemA(hz, -1, &ze);
	int numItems = ze.index;

	for (int i = 0; i < numItems; i++)
	{
		GetZipItemA(hz, i, &ze);
		char* pBuffer = new char[ze.unc_size];
		UnzipItem(hz, i, pBuffer, ze.unc_size, ZIP_MEMORY);

		if (!stricmp(ze.name, "Character.iff"))
		{
			MakeCharMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Part.iff"))
		{
			MakePartMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Club.iff"))
		{
			MakeClubMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "ClubSet.iff"))
		{
			MakeClubSetMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Ball.iff"))
		{
			MakeBallMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Item.iff"))
		{
			MakeItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Caddie.iff"))
		{
			MakeCaddieMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "CaddieItem.iff"))
		{
			MakeCadItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "SetItem.iff"))
		{
			MakeSetItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Course.iff"))
		{
			MakeCourseMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Match.iff"))
		{
			MakeMatchMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Title.iff"))
		{
			MakeTitleMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Enchant.iff"))
		{
			MakeEnchantMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Skin.iff"))
		{
			MakeSkinMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "HairStyle.iff"))
		{
			MakeHairStyleMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "ChildItem.iff"))
		{
			MakeChildItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Mascot.iff"))
		{
			MakeMascotMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "AuxPart.iff"))
		{
			MakeAuxPartMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "QuestDrop.iff"))
		{
			MakeQuestDropMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Quest.iff"))
		{
			MakeQuestMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Card.iff"))
		{
			if (IsLocalContent(S4_CARD_SYSTEM))
				MakeCardMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Furniture.iff"))
		{
			if (IsLocalContent(S4_REAL_MYROOM))
				MakeFurnitureMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "Desc.iff"))
		{
			MakeDescMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "OfflineShop.iff"))
		{
			if (IsLocalContent(S4_OFFLINE_SHOP))
				MakeOfflineMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "CadieMagicBox.iff"))
		{
			MakeCadieMagicBoxMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "CadieMagicBoxRandom.iff"))
		{
			MakeRandomRecycleMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "FurnitureAbility.iff"))
		{
			MakeFurnitureAbilityList(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "TikiRecipe.iff"))
		{
			MakeTikiRecipeMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "TikiPointTable.iff"))
		{
			MakeTikiPointTableMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "TikiSpecialTable.iff"))
		{
			MakeTikiSpecialTableMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "CutinInfomation.iff"))
		{
			MakeCutinInforMationTableMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "SpecialPrizeItem.iff"))
		{
			MakeSpecialPrizeItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "RandomBox.sff"))
		{
			MakeRandomBoxMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "NonVisibleItemTable.iff"))
		{
			MakeNonVisibleItemMap(pBuffer, ze.unc_size);
		}
		else if (!stricmp(ze.name, "SubscriptionItemTable.iff"))
		{
			MakeSubscriptionItemMap(pBuffer, ze.unc_size);
		}

		delete[] pBuffer;
	}

	CloseZipU(hz);
	delete[] buff;

	MakeItemBuffMap();

	CheckInvalidItemName();

	return true;
}

void CItemManager::MakeCharMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sChar sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sChar));
		p += sizeof(IFF_STRUCT::sChar);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_CharMap.insert(
				std::map<unsigned int, IFF_STRUCT::sChar>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakePartMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sPart sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sPart));
		p += sizeof(IFF_STRUCT::sPart);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			DevRentalPreventSale(sItem);
			m_PartMap.insert(
				std::map<unsigned int, IFF_STRUCT::sPart>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeClubMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sClub sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sClub));
		p += sizeof(IFF_STRUCT::sClub);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			m_ClubMap.insert(
				std::map<unsigned int, IFF_STRUCT::sClub>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeClubSetMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sClubSet sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sClubSet));
		p += sizeof(IFF_STRUCT::sClubSet);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_ClubSetMap.insert(
				std::map<unsigned int, IFF_STRUCT::sClubSet>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeBallMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sBall sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sBall));
		p += sizeof(IFF_STRUCT::sBall);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_BallMap.insert(
				std::map<unsigned int, IFF_STRUCT::sBall>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sItem sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sItem));
		p += sizeof(IFF_STRUCT::sItem);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			m_ItemMap.insert(
				std::map<unsigned int, IFF_STRUCT::sItem>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeCaddieMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sCaddie sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sCaddie));
		p += sizeof(IFF_STRUCT::sCaddie);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_CaddieMap.insert(
				std::map<unsigned int, IFF_STRUCT::sCaddie>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeCadItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sCadItem sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sCadItem));
		p += sizeof(IFF_STRUCT::sCadItem);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_CadItemMap.insert(
				std::map<unsigned int, IFF_STRUCT::sCadItem>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeSetItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sSetItem sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sSetItem));
		p += sizeof(IFF_STRUCT::sSetItem);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			m_SetItemMap.insert(
				std::map<unsigned int, IFF_STRUCT::sSetItem>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeCourseMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sCourse sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sCourse));
		p += sizeof(IFF_STRUCT::sCourse);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_CourseMap.insert(
				std::map<unsigned int, IFF_STRUCT::sCourse>::value_type(
					sItem.c.TypeId, sItem));
		}
	}

	memset(&sItem, 0, sizeof(IFF_STRUCT::sCourse));
	sItem.c.TypeId = 0x280000fd;

	sItem.Difficulty = 1;
	strcpy(sItem.c.Name, "TUTORIAL");
	strcpy(sItem.Data, "tuto");
	sItem.Slope = 1.1f;
	strcpy(sItem.TexProp, "tuto_property.xml");

	m_CourseMap.insert(std::map<unsigned int, IFF_STRUCT::sCourse>::value_type(
		sItem.c.TypeId, sItem));

	memset(&sItem, 0, sizeof(IFF_STRUCT::sCourse));
	sItem.c.TypeId = 0x280000fe;

	sItem.Difficulty = 1;
	strcpy(sItem.c.Name, "TEST");
	strcpy(sItem.Data, "test");
	sItem.Slope = 1.1f;
	strcpy(sItem.TexProp, "test_property.xml");

	m_CourseMap.insert(std::map<unsigned int, IFF_STRUCT::sCourse>::value_type(
		sItem.c.TypeId, sItem));

	memset(&sItem, 0, sizeof(IFF_STRUCT::sCourse));
	sItem.c.TypeId = 0x2800007f;
	sItem.c.Final = true;

	sItem.Difficulty = 1;
	strcpy(sItem.c.Name, "RANDOM");
	strcpy(sItem.Data, "random");
	sItem.Slope = 1.1f;
	strcpy(sItem.TexProp, "test_property.xml");
	strcpy(sItem.c.Icon, "map_random");

	m_CourseMap.insert(std::map<unsigned int, IFF_STRUCT::sCourse>::value_type(
		sItem.c.TypeId, sItem));
}

void CItemManager::MakeMatchMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sMatch sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sMatch));
		p += sizeof(IFF_STRUCT::sMatch);

		if (sItem.Final)
		{
			m_MatchMap.insert(
				std::map<unsigned int, IFF_STRUCT::sMatch>::value_type(
					sItem.TypeId, sItem));
		}
	}
}

void CItemManager::MakeTitleMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sTitle sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sTitle));
		p += sizeof(IFF_STRUCT::sTitle);

		if (sItem.Final)
		{
			m_TitleMap.insert(
				std::map<unsigned int, IFF_STRUCT::sTitle>::value_type(
					sItem.TypeId, sItem));
		}
	}
}

void CItemManager::MakeEnchantMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sEnchant sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sEnchant));
		p += sizeof(IFF_STRUCT::sEnchant);

		if (sItem.Final)
		{
			m_EnchantMap.insert(
				std::map<unsigned int, IFF_STRUCT::sEnchant>::value_type(
					sItem.TypeId, sItem));
		}
	}
}

void CItemManager::MakeSkinMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sSkin sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sSkin));
		p += sizeof(IFF_STRUCT::sSkin);

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_SkinMap.insert(
				std::map<unsigned int, IFF_STRUCT::sSkin>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeCardMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sCard sCard;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sCard, p, sizeof(IFF_STRUCT::sCard));
		p += sizeof(IFF_STRUCT::sCard);

		if (sCard.c.SalePrice == 0)
			sCard.c.SalePrice = sCard.c.Price;

		if (sCard.c.UsedPrice >= sCard.c.SalePrice)
			sCard.c.UsedPrice = 0;

		if (sCard.c.saleStart.wYear != 0 || sCard.c.saleEnd.wYear != 0)
		{
			sCard.c.New = 0;
			sCard.c.Hit = 0;
		}

		if (sCard.c.Final)
		{
			m_CardMap.insert(
				std::map<unsigned int, IFF_STRUCT::sCard>::value_type(
					sCard.c.TypeId, sCard));
		}
	}
}

void CItemManager::MakeFurnitureMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sFurniture sFurni;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sFurni, p, sizeof(IFF_STRUCT::sFurniture));
		p += sizeof(IFF_STRUCT::sFurniture);

		if (sFurni.c.SalePrice == 0)
			sFurni.c.SalePrice = sFurni.c.Price;

		if (sFurni.c.UsedPrice >= sFurni.c.SalePrice)
			sFurni.c.UsedPrice = 0;

		if (sFurni.c.saleStart.wYear != 0 || sFurni.c.saleEnd.wYear != 0)
		{
			sFurni.c.New = 0;
			sFurni.c.Hit = 0;
		}

		if (sFurni.c.Final)
		{
			DevFinalStockPrice(sFurni.c);
			m_FurnitureMap.insert(
				std::map<unsigned int, IFF_STRUCT::sFurniture>::value_type(
					sFurni.c.TypeId, sFurni));
		}
	}
}

void CItemManager::MakeOfflineMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sOfflineShop sOs;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sOs, p, sizeof(IFF_STRUCT::sOfflineShop));
		p += sizeof(IFF_STRUCT::sOfflineShop);

		if (sOs.c.SalePrice == 0)
			sOs.c.SalePrice = sOs.c.Price;

		if (sOs.c.UsedPrice >= sOs.c.SalePrice)
			sOs.c.UsedPrice = 0;

		if (sOs.c.saleStart.wYear != 0 || sOs.c.saleEnd.wYear != 0)
		{
			sOs.c.New = 0;
			sOs.c.Hit = 0;
		}

		if (sOs.c.Final)
		{
			m_OfflineShopMap.insert(
				std::map<unsigned int, IFF_STRUCT::sOfflineShop>::value_type(
					sOs.c.TypeId, sOs));
		}
	}
}

void CItemManager::MakeCadieMagicBoxMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sCadieMagicBox record;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	m_CadieMagicBox.clear();

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&record, p, sizeof(IFF_STRUCT::sCadieMagicBox));
		p += sizeof(IFF_STRUCT::sCadieMagicBox);

		m_CadieMagicBox.push_back(record);

		switch (record.uiCategory)
		{
		case 0:
			g_NumLowRecycleItem++;
			break;
		case 1:
			g_NumMidRecycleItem++;
			break;
		case 2:
			g_NumHighRecycleItem++;
			break;
		case 3:
			g_NumSpecialRecycleItem++;
			break;
		case 4:
			g_NumEventRecycleItem++;
			break;
		}

		g_NumIndexRecycleItem++;
	}

	if (g_ArrayTidMixItem)
	{
		for (int i = 0; i < 4; i++)
		{
			if (g_ArrayTidMixItem[i])
			{
				delete[] g_ArrayTidMixItem[i];
				g_ArrayTidMixItem[i] = NULL;
			}
		}

		delete[] g_ArrayTidMixItem;
		g_ArrayTidMixItem = NULL;
	}

	if (g_ArrayRecycleItem)
	{
		delete[] g_ArrayRecycleItem;
		g_ArrayRecycleItem = NULL;
	}

	g_ArrayTidMixItem = new sRecycleItem*[g_NumIndexRecycleItem];
	if (g_ArrayTidMixItem)
	{
		for (int i = 0; i < g_NumIndexRecycleItem; i++)
			g_ArrayTidMixItem[i] = new sRecycleItem[4];
	}

	g_ArrayRecycleItem = new sRecycleItem[g_NumIndexRecycleItem];

	std::list<IFF_STRUCT::sCadieMagicBox>::iterator it =
		m_CadieMagicBox.begin();
	std::list<IFF_STRUCT::sCadieMagicBox>::iterator itEnd =
		m_CadieMagicBox.end();

	for (; it != itEnd; ++it)
	{
		memset(&g_ArrayRecycleItem[(*it).uiNumber - 1], 0,
			sizeof(sRecycleItem));

		g_ArrayRecycleItem[(*it).uiNumber - 1].dwTid = (*it).uiOutput;
		g_ArrayRecycleItem[(*it).uiNumber - 1].iNum =
			(unsigned short)(*it).uiOutputCount;
		g_ArrayRecycleItem[(*it).uiNumber - 1].byLevel =
			(unsigned char)(*it).iLevel;
		g_ArrayRecycleItem[(*it).uiNumber - 1].iChar = (short)(*it).iCharacter;

		if ((*it).uiRandSeq > 0)
		{
			g_ArrayRecycleItem[(*it).uiNumber - 1].dwRandSeq = (*it).uiRandSeq;
			strncpy(g_ArrayRecycleItem[(*it).uiNumber - 1].szRandName,
				(*it).szRandName, 39);
			g_ArrayRecycleItem[(*it).uiNumber - 1].szRandName[39] = 0;
		}

		for (int j = 0; j < 4; j++)
		{
			memset(&g_ArrayTidMixItem[(*it).uiNumber - 1][j], 0,
				sizeof(sRecycleItem));
			g_ArrayTidMixItem[(*it).uiNumber - 1][j].dwTid = (*it).uiElem[j];
			g_ArrayTidMixItem[(*it).uiNumber - 1][j].iNum =
				(unsigned short)(*it).uiElemCount[j];
			g_ArrayTidMixItem[(*it).uiNumber - 1][j].byLevel =
				(unsigned char)(*it).iLevel;
		}
	}

	m_CadieMagicBox.clear();
}

void CItemManager::MakeRandomRecycleMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sRandomRecycle record;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	std::set<unsigned long> seqNumberSet;
	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&record, p, sizeof(IFF_STRUCT::sRandomRecycle));
		p += sizeof(IFF_STRUCT::sRandomRecycle);

		if (seqNumberSet.find(record.uiRandSeq) == seqNumberSet.end())
			seqNumberSet.insert(record.uiRandSeq);

		m_RandomRecycleMaps.insert(
			std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle>::value_type(
				record.uiRandSeq, record));
	}

	std::set<unsigned long>::const_iterator ci;
	for (ci = seqNumberSet.begin(); ci != seqNumberSet.end(); ++ci)
	{
		std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle>::const_iterator
			lower = m_RandomRecycleMaps.lower_bound(*ci);
		std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle>::const_iterator
			upper = m_RandomRecycleMaps.upper_bound(*ci);
		while (lower != upper)
		{
			++lower;
		}
	}
}

void CItemManager::MakeTikiRecipeMap(char* Buf, int len)
{
	IFF_STRUCT::sTikiOutputTable record;

	IFF_FILE_HEADER _hdr;
	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&record, p, sizeof(IFF_STRUCT::sTikiOutputTable));
		p += sizeof(IFF_STRUCT::sTikiOutputTable);

		if (CTikiMagicBoxDoc::Instance())
			CTikiMagicBoxDoc::Instance()->InsertOutput(record);
	}
}

void CItemManager::MakeTikiPointTableMap(char* Buf, int len)
{
	IFF_STRUCT::sTikiPointTable record;

	IFF_FILE_HEADER _hdr;
	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&record, p, sizeof(IFF_STRUCT::sTikiPointTable));
		p += sizeof(IFF_STRUCT::sTikiPointTable);

		if (CTikiMagicBoxDoc::Instance())
			CTikiMagicBoxDoc::Instance()->InsertPointTable(record);
	}
}

void CItemManager::MakeTikiSpecialTableMap(char* Buf, int len)
{
	IFF_STRUCT::sTikiSpecialRecipe record;

	IFF_FILE_HEADER _hdr;
	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&record, p, sizeof(IFF_STRUCT::sTikiSpecialRecipe));
		p += sizeof(IFF_STRUCT::sTikiSpecialRecipe);

		if (CTikiMagicBoxDoc::Instance())
			CTikiMagicBoxDoc::Instance()->InsertSpecialRecipe(record);
	}
}

struct stFindRandomBox
{
	stFindRandomBox(unsigned long typeId, unsigned long checkTypeId)
		: m_typeId(typeId), m_checkTypeId(checkTypeId)
	{
	}

	bool operator()(
		const std::pair<unsigned long, _private::sRandomBoxItem>& item) const
	{
		return item.second.box.typeId == m_typeId &&
			item.second.box.checkTypeId ==
			(m_checkTypeId ? m_checkTypeId : item.second.box.checkTypeId);
	}

	unsigned long m_typeId;
	unsigned long m_checkTypeId;
};

void CItemManager::MakeRandomBoxMap(char* Buf, int len)
{
	static unsigned long s_itemIndex;

	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sRandomBox box;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	m_RandomBoxMap.clear();

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&box, p, sizeof(IFF_STRUCT::sRandomBox));
		p += sizeof(IFF_STRUCT::sRandomBox);

		if (!box.active)
			continue;

		unsigned char kind = box.kind;
		if (!kind)
			continue;

		std::map<unsigned int, _private::sRandomBox>::iterator it =
			m_RandomBoxMap.find(box.boxTypeId);
		if (it == m_RandomBoxMap.end() && kind == 1)
		{
			std::pair<std::map<unsigned int, _private::sRandomBox>::iterator,
				bool>
				ret = m_RandomBoxMap.insert(
					std::map<unsigned int, _private::sRandomBox>::value_type(
						box.boxTypeId, _private::sRandomBox()));
			if (!ret.second)
				continue;

			it = ret.first;
		}

		if (it == m_RandomBoxMap.end())
			continue;

		_private::sRandomBox& randomBox = (*it).second;

		switch (kind)
		{
		case 1:
			memcpy(&randomBox.box, &box, sizeof(IFF_STRUCT::sRandomBox));
			break;
		case 2:
		{
			_private::sRandomBoxItem item;
			memcpy(&item.box, &box, sizeof(IFF_STRUCT::sRandomBox));
			std::pair<
				std::map<unsigned long, _private::sRandomBoxItem>::iterator,
				bool>
				result = randomBox.items.insert(std::map<unsigned long,
					_private::sRandomBoxItem>::value_type(++s_itemIndex, item));
			if (!result.second)
				break;
		}
		break;
		case 3:
		{
			std::map<unsigned long, _private::sRandomBoxItem>::iterator itItem =
				std::find_if(randomBox.items.begin(), randomBox.items.end(),
					stFindRandomBox(box.typeId, box.checkTypeId));
			if (itItem == randomBox.items.end())
				break;

			(*itItem).second.checkItems.push_back(box);
		}
		break;
		case 4:
		{
			_private::sRandomBoxStuff stuff;
			memcpy(&stuff.box, &box, sizeof(IFF_STRUCT::sRandomBox));
			randomBox.stuffs.push_back(stuff);
		}
		break;
		}
	}
}

void CItemManager::MakeNonVisibleItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sNonVisibleItem item;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	m_NonVisibleItemMap.clear();

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&item, p, sizeof(IFF_STRUCT::sNonVisibleItem));
		p += sizeof(IFF_STRUCT::sNonVisibleItem);

		if (item.active)
		{
			std::pair<std::map<unsigned int,
						  IFF_STRUCT::sNonVisibleItem>::const_iterator,
				bool>
				result = m_NonVisibleItemMap.insert(std::map<unsigned int,
					IFF_STRUCT::sNonVisibleItem>::value_type(item.typeId,
					item));
		}
	}
}

void CItemManager::MakeSubscriptionItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sSubscriptionItem item;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	m_SubscriptionItemMap.clear();

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&item, p, sizeof(IFF_STRUCT::sSubscriptionItem));
		p += sizeof(IFF_STRUCT::sSubscriptionItem);

		if (item.active)
		{
			std::pair<std::map<unsigned int,
						  IFF_STRUCT::sSubscriptionItem>::const_iterator,
				bool>
				result = m_SubscriptionItemMap.insert(std::map<unsigned int,
					IFF_STRUCT::sSubscriptionItem>::value_type(item.typeId,
					item));
		}
	}
}

void CItemManager::MakeHairStyleMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sHairStyle sHair;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sHair, p, sizeof(IFF_STRUCT::sHairStyle));
		p += sizeof(IFF_STRUCT::sHairStyle);

		if (sHair.c.SalePrice == 0)
			sHair.c.SalePrice = sHair.c.Price;

		if (sHair.c.UsedPrice >= sHair.c.SalePrice)
			sHair.c.UsedPrice = 0;

		if (sHair.c.saleStart.wYear != 0 || sHair.c.saleEnd.wYear != 0)
		{
			sHair.c.New = 0;
			sHair.c.Hit = 0;
		}

		if (sHair.c.Final)
		{
			m_HairStyleMap.insert(
				std::map<unsigned int, IFF_STRUCT::sHairStyle>::value_type(
					sHair.c.TypeId, sHair));
		}
	}
}

void CItemManager::MakeChildItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sChildItem sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sChildItem));
		p += sizeof(IFF_STRUCT::sChildItem);

		if (sItem.Final)
		{
			m_ChildItemMap.insert(
				std::map<unsigned int, IFF_STRUCT::sChildItem>::value_type(i,
					sItem));
		}
	}
}

void CItemManager::MakeAuxPartMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sAuxPart sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sAuxPart));
		p += sizeof(IFF_STRUCT::sAuxPart);

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			m_AuxPartMap.insert(
				std::map<unsigned int, IFF_STRUCT::sAuxPart>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeQuestDropMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sQuestDrop sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sQuestDrop));
		p += sizeof(IFF_STRUCT::sQuestDrop);

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			m_QuestDropMap.insert(
				std::map<unsigned int, IFF_STRUCT::sQuestDrop>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeQuestMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sQuest sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sQuest));
		p += sizeof(IFF_STRUCT::sQuest);

		sItem.ProbTotal = 0;
		sItem.CurItemIdx = 0;
		sItem.aTidItems = NULL;

		if (sItem.Final)
		{
			m_QuestMap.insert(
				std::map<unsigned int, IFF_STRUCT::sQuest>::value_type(
					sItem.TypeId, sItem));
		}
	}
}

void CItemManager::MakeMascotMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sMascot sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::sMascot));
		p += sizeof(IFF_STRUCT::sMascot);

		if (sItem.c.SalePrice == 0)
			sItem.c.SalePrice = sItem.c.Price;

		if (sItem.c.UsedPrice >= sItem.c.SalePrice)
			sItem.c.UsedPrice = 0;

		if (sItem.c.saleStart.wYear != 0 || sItem.c.saleEnd.wYear != 0)
		{
			sItem.c.New = 0;
			sItem.c.Hit = 0;
		}

		if (sItem.c.Final)
		{
			DevFinalStockPrice(sItem.c);
			m_MascotMap.insert(
				std::map<unsigned int, IFF_STRUCT::sMascot>::value_type(
					sItem.c.TypeId, sItem));
		}
	}
}

void CItemManager::MakeDescMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sDesc desc;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&desc, p, sizeof(IFF_STRUCT::sDesc));
		p += sizeof(IFF_STRUCT::sDesc);

		m_DescMap.insert(std::map<unsigned int, IFF_STRUCT::sDesc>::value_type(
			desc.TypeId, desc));
	}
}

void CItemManager::MakeFurnitureAbilityList(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sFurnitureAbility sOs;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sOs, p, sizeof(IFF_STRUCT::sFurnitureAbility));
		p += sizeof(IFF_STRUCT::sFurnitureAbility);

		if (sOs.bFinal)
			m_FurnitureAbility.push_back(sOs);
	}
}

IFF_STRUCT::sChar* CItemManager::FindChar(unsigned long tid)
{
	IFF_STRUCT::sChar* pChar = NULL;
	std::map<unsigned int, IFF_STRUCT::sChar>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CharMap.find(tid);

		if (it != m_CharMap.end())
			pChar = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CharMap.find(tid);

		if (it != m_CharMap.end())
			return &(*it).second;
	}

	return pChar;
}

IFF_STRUCT::sPart* CItemManager::FindPart(unsigned long tid)
{
	IFF_STRUCT::sPart* pPart = NULL;
	std::map<unsigned int, IFF_STRUCT::sPart>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_PartMap.find(tid);

		if (it != m_PartMap.end())
			pPart = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_PartMap.find(tid);

		if (it != m_PartMap.end())
			return &(*it).second;
	}

	return pPart;
}

IFF_STRUCT::sClub* CItemManager::FindClub(unsigned long tid)
{
	IFF_STRUCT::sClub* pClub = NULL;
	std::map<unsigned int, IFF_STRUCT::sClub>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_ClubMap.find(tid);

		if (it != m_ClubMap.end())
			pClub = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_ClubMap.find(tid);

		if (it != m_ClubMap.end())
			return &(*it).second;
	}

	return pClub;
}

IFF_STRUCT::sClubSet* CItemManager::FindClubSet(unsigned long tid)
{
	IFF_STRUCT::sClubSet* pClubSet = NULL;
	std::map<unsigned int, IFF_STRUCT::sClubSet>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_ClubSetMap.find(tid);

		if (it != m_ClubSetMap.end())
			pClubSet = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_ClubSetMap.find(tid);

		if (it != m_ClubSetMap.end())
			return &(*it).second;
	}

	return pClubSet;
}

IFF_STRUCT::sBall* CItemManager::FindBall(unsigned long tid)
{
	IFF_STRUCT::sBall* pBall = NULL;
	std::map<unsigned int, IFF_STRUCT::sBall>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_BallMap.find(tid);

		if (it != m_BallMap.end())
			pBall = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_BallMap.find(tid);

		if (it != m_BallMap.end())
			return &(*it).second;
	}

	return pBall;
}

IFF_STRUCT::sItem* CItemManager::FindItem(unsigned long tid)
{
	IFF_STRUCT::sItem* pItem = NULL;
	std::map<unsigned int, IFF_STRUCT::sItem>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_ItemMap.find(tid);

		if (it != m_ItemMap.end())
			pItem = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_ItemMap.find(tid);

		if (it != m_ItemMap.end())
			return &(*it).second;
	}

	return pItem;
}

IFF_STRUCT::sCaddie* CItemManager::FindCaddie(unsigned long tid)
{
	IFF_STRUCT::sCaddie* pCaddie = NULL;
	std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CaddieMap.find(tid);

		if (it != m_CaddieMap.end())
			pCaddie = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CaddieMap.find(tid);

		if (it != m_CaddieMap.end())
			return &(*it).second;
	}

	return pCaddie;
}

IFF_STRUCT::sCadItem* CItemManager::FindCadItem(unsigned long tid)
{
	IFF_STRUCT::sCadItem* pCadItem = NULL;
	std::map<unsigned int, IFF_STRUCT::sCadItem>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CadItemMap.find(tid);

		if (it != m_CadItemMap.end())
			pCadItem = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CadItemMap.find(tid);

		if (it != m_CadItemMap.end())
			return &(*it).second;
	}

	return pCadItem;
}

IFF_STRUCT::sSetItem* CItemManager::FindSetItem(unsigned long tid)
{
	IFF_STRUCT::sSetItem* pSetItem = NULL;
	std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_SetItemMap.find(tid);

		if (it != m_SetItemMap.end())
			pSetItem = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_SetItemMap.find(tid);

		if (it != m_SetItemMap.end())
			return &(*it).second;
	}

	return pSetItem;
}

IFF_STRUCT::sSetItem* CItemManager::FindSetItemAtConstruct(unsigned long tid)
{
	if (m_IsReloading)
	{
		m_cs.lock();

		for (std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it =
				 m_SetItemMap.begin();
			it != m_SetItemMap.end(); ++it)
		{
			IFF_STRUCT::sSetItem* pSetItem = &(*it).second;
			if (pSetItem)
			{
				for (int i = 0; i < pSetItem->nElems; i++)
				{
					if (pSetItem->ElemIds[i] == tid)
					{
						m_cs.unlock();
						return pSetItem;
					}
				}
			}
		}

		m_cs.unlock();
	}
	else
	{
		for (std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator it =
				 m_SetItemMap.begin();
			it != m_SetItemMap.end(); ++it)
		{
			IFF_STRUCT::sSetItem* pSetItem = &(*it).second;
			if (pSetItem)
			{
				for (int i = 0; i < pSetItem->nElems; i++)
				{
					if (pSetItem->ElemIds[i] == tid)
						return pSetItem;
				}
			}
		}
	}

	return NULL;
}

IFF_STRUCT::sCourse* CItemManager::FindCourse(unsigned long tid)
{
	IFF_STRUCT::sCourse* pCourse = NULL;
	std::map<unsigned int, IFF_STRUCT::sCourse>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CourseMap.find(tid);

		if (it != m_CourseMap.end())
			pCourse = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CourseMap.find(tid);

		if (it != m_CourseMap.end())
			return &(*it).second;
	}

	return pCourse;
}

IFF_STRUCT::sMatch* CItemManager::FindMatch(unsigned long tid)
{
	IFF_STRUCT::sMatch* pMatch = NULL;
	std::map<unsigned int, IFF_STRUCT::sMatch>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_MatchMap.find(tid);

		if (it != m_MatchMap.end())
			pMatch = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_MatchMap.find(tid);

		if (it != m_MatchMap.end())
			return &(*it).second;
	}

	return pMatch;
}

IFF_STRUCT::sTitle* CItemManager::FindTitle(unsigned long tid)
{
	IFF_STRUCT::sTitle* pTitle = NULL;
	std::map<unsigned int, IFF_STRUCT::sTitle>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_TitleMap.find(tid);

		if (it != m_TitleMap.end())
			pTitle = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_TitleMap.find(tid);

		if (it != m_TitleMap.end())
			return &(*it).second;
	}

	return pTitle;
}

IFF_STRUCT::sEnchant* CItemManager::FindEnchant(unsigned long tid)
{
	IFF_STRUCT::sEnchant* pEnchant = NULL;
	std::map<unsigned int, IFF_STRUCT::sEnchant>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_EnchantMap.find(tid);

		if (it != m_EnchantMap.end())
			pEnchant = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_EnchantMap.find(tid);

		if (it != m_EnchantMap.end())
			return &(*it).second;
	}

	return pEnchant;
}

IFF_STRUCT::sSkin* CItemManager::FindSkin(unsigned long tid)
{
	IFF_STRUCT::sSkin* pSkin = NULL;
	std::map<unsigned int, IFF_STRUCT::sSkin>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_SkinMap.find(tid);

		if (it != m_SkinMap.end())
			pSkin = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_SkinMap.find(tid);

		if (it != m_SkinMap.end())
			return &(*it).second;
	}

	return pSkin;
}

IFF_STRUCT::sRandomBox* CItemManager::FindRandomBox(unsigned long typeId)
{
	IFF_STRUCT::sRandomBox* pBox = NULL;

	_private::sRandomBox* pRandomBox = findRandomBox(typeId);
	if (pRandomBox)
	{
		pBox = &pRandomBox->box;
	}

	return pBox;
}

void CItemManager::MakeCutinInforMationTableMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::S5::sCutinInformation sItem;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&sItem, p, sizeof(IFF_STRUCT::S5::sCutinInformation));
		p += sizeof(IFF_STRUCT::S5::sCutinInformation);

		if (sItem.bFinal)
		{
			m_CutinInformationMap.insert(
				std::map<unsigned int, IFF_STRUCT::S5::sCutinInformation>::
					value_type(sItem.NormalTypeId, sItem));
		}
	}
}

IFF_STRUCT::S5::sCutinInformation* CItemManager::FindCutinInformation(
	unsigned long tid)
{
	IFF_STRUCT::S5::sCutinInformation* pCutin = NULL;
	std::map<unsigned int, IFF_STRUCT::S5::sCutinInformation>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CutinInformationMap.find(tid);

		if (it != m_CutinInformationMap.end())
			pCutin = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CutinInformationMap.find(tid);

		if (it != m_CutinInformationMap.end())
			return &(*it).second;
	}

	return pCutin;
}

IFF_STRUCT::sHairStyle* CItemManager::FindHairStyle(unsigned long tid)
{
	IFF_STRUCT::sHairStyle* pHairStyle = NULL;
	std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_HairStyleMap.find(tid);

		if (it != m_HairStyleMap.end())
			pHairStyle = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_HairStyleMap.find(tid);

		if (it != m_HairStyleMap.end())
			return &(*it).second;
	}

	return pHairStyle;
}

IFF_STRUCT::sHairStyle* CItemManager::FindHairStyle(unsigned char charID,
	unsigned char hairID)
{
	IFF_STRUCT::sHairStyle* pHairStyle = NULL;
	std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		for (it = m_HairStyleMap.begin(); it != m_HairStyleMap.end(); ++it)
		{
			if ((*it).second.cCharID == charID &&
				(*it).second.cHairID == hairID)
			{
				pHairStyle = &(*it).second;
				break;
			}
		}

		m_cs.unlock();
	}
	else
	{
		for (it = m_HairStyleMap.begin(); it != m_HairStyleMap.end(); ++it)
		{
			if ((*it).second.cCharID == charID &&
				(*it).second.cHairID == hairID)
			{
				pHairStyle = &(*it).second;
				break;
			}
		}
	}

	return pHairStyle;
}

IFF_STRUCT::sChildItem* CItemManager::FindChildItem(unsigned long tid,
	int iIndex)
{
	IFF_STRUCT::sChildItem* pChildItem = NULL;
	int iNum = 0;
	std::map<unsigned int, IFF_STRUCT::sChildItem>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		for (it = m_ChildItemMap.begin(); it != m_ChildItemMap.end(); ++it)
		{
			pChildItem = &(*it).second;

			if (pChildItem)
			{
				if (pChildItem->TypeId == tid)
				{
					if (iIndex == iNum)
						break;

					iNum++;
				}
			}
		}

		m_cs.unlock();
	}
	else
	{
		for (it = m_ChildItemMap.begin(); it != m_ChildItemMap.end(); ++it)
		{
			pChildItem = &(*it).second;

			if (pChildItem)
			{
				if (pChildItem->TypeId == tid)
				{
					if (iIndex == iNum)
						break;

					iNum++;
				}
			}
		}
	}

	return pChildItem;
}

int CItemManager::FindChildItemNumber(unsigned long tid)
{
	int count = 0;

	if (m_IsReloading)
	{
		m_cs.lock();

		for (std::map<unsigned int, IFF_STRUCT::sChildItem>::iterator it =
				 m_ChildItemMap.begin();
			it != m_ChildItemMap.end(); ++it)
		{
			IFF_STRUCT::sChildItem* pChildItem = &(*it).second;

			if (pChildItem)
			{
				if (pChildItem->TypeId == tid)
					count++;
			}
		}

		m_cs.unlock();
	}
	else
	{
		for (std::map<unsigned int, IFF_STRUCT::sChildItem>::iterator it =
				 m_ChildItemMap.begin();
			it != m_ChildItemMap.end(); ++it)
		{
			IFF_STRUCT::sChildItem* pChildItem = &(*it).second;

			if (pChildItem)
			{
				if (pChildItem->TypeId == tid)
					count++;
			}
		}
	}

	return count;
}

IFF_STRUCT::sAuxPart* CItemManager::FindAuxPart(unsigned long tid)
{
	IFF_STRUCT::sAuxPart* pAuxPart = NULL;
	std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_AuxPartMap.find(tid);

		if (it != m_AuxPartMap.end())
			pAuxPart = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_AuxPartMap.find(tid);

		if (it != m_AuxPartMap.end())
			return &(*it).second;
	}

	return pAuxPart;
}

IFF_STRUCT::sQuestDrop* CItemManager::FindQuestDrop(unsigned long tid)
{
	IFF_STRUCT::sQuestDrop* pQuestDrop = NULL;
	std::map<unsigned int, IFF_STRUCT::sQuestDrop>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_QuestDropMap.find(tid);

		if (it != m_QuestDropMap.end())
			pQuestDrop = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_QuestDropMap.find(tid);

		if (it != m_QuestDropMap.end())
			return &(*it).second;
	}

	return pQuestDrop;
}

IFF_STRUCT::sQuest* CItemManager::FindQuest(unsigned long tid)
{
	IFF_STRUCT::sQuest* pQuest = NULL;
	std::map<unsigned int, IFF_STRUCT::sQuest>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_QuestMap.find(tid);

		if (it != m_QuestMap.end())
			pQuest = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_QuestMap.find(tid);

		if (it != m_QuestMap.end())
			return &(*it).second;
	}

	return pQuest;
}

IFF_STRUCT::sMascot* CItemManager::FindMascot(unsigned long tid)
{
	std::map<unsigned int, IFF_STRUCT::sMascot>::iterator it;
	it = m_MascotMap.find(tid);

	if (it != m_MascotMap.end())
		return &(*it).second;

	return NULL;
}

IFF_STRUCT::sCard* CItemManager::FindCard(unsigned long tid)
{
	IFF_STRUCT::sCard* pCard = NULL;
	std::map<unsigned int, IFF_STRUCT::sCard>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_CardMap.find(tid);

		if (it != m_CardMap.end())
			pCard = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_CardMap.find(tid);

		if (it != m_CardMap.end())
			return &(*it).second;
	}

	return pCard;
}

IFF_STRUCT::sFurniture* CItemManager::FindFurniture(unsigned long tid)
{
	if (!IsLocalContent(S4_REAL_MYROOM))
		return NULL;

	IFF_STRUCT::sFurniture* pFurniture = NULL;
	std::map<unsigned int, IFF_STRUCT::sFurniture>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_FurnitureMap.find(tid);

		if (it != m_FurnitureMap.end())
			pFurniture = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_FurnitureMap.find(tid);

		if (it != m_FurnitureMap.end())
			pFurniture = &(*it).second;
	}

	return pFurniture;
}

IFF_STRUCT::sOfflineShop* CItemManager::FindOfflineShop(unsigned long tid)
{
	if (!IsLocalContent(S4_OFFLINE_SHOP))
		return NULL;

	IFF_STRUCT::sOfflineShop* pOfflineShop = NULL;
	std::map<unsigned int, IFF_STRUCT::sOfflineShop>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_OfflineShopMap.find(tid);

		if (it != m_OfflineShopMap.end())
			pOfflineShop = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_OfflineShopMap.find(tid);

		if (it != m_OfflineShopMap.end())
			pOfflineShop = &(*it).second;
	}

	return pOfflineShop;
}

IFF_STRUCT::sDesc* CItemManager::FindDesc(unsigned long typeId)
{
	std::map<unsigned int, IFF_STRUCT::sDesc>::iterator it;
	it = m_DescMap.find(typeId);

	if (it == m_DescMap.end())
		return NULL;

	return &(*it).second;
}

IFF_ITEM_COMMON* CItemManager::FindCommonItem(unsigned long tid)
{
	IFF_ITEM_COMMON* pCommon = NULL;

	switch (tid >> 26)
	{
	case 1:
	{
		IFF_STRUCT::sChar* pChar = FindChar(tid);
		if (pChar)
			return &pChar->c;
	}
	break;
	case 2:
	{
		IFF_STRUCT::sPart* pPart = FindPart(tid);
		if (pPart)
			return &pPart->c;
	}
	break;
	case 3:
	{
		IFF_STRUCT::sClub* pClub = FindClub(tid);
		if (pClub)
			return &pClub->c;
	}
	break;
	case 4:
	{
		IFF_STRUCT::sClubSet* pClubSet = FindClubSet(tid);
		if (pClubSet)
			return &pClubSet->c;
	}
	break;
	case 5:
	{
		IFF_STRUCT::sBall* pBall = FindBall(tid);
		if (pBall)
			return &pBall->c;
	}
	break;
	case 6:
	{
		IFF_STRUCT::sItem* pItem = FindItem(tid);
		if (pItem)
			return &pItem->c;
	}
	break;
	case 7:
	{
		IFF_STRUCT::sCaddie* pCaddie = FindCaddie(tid);
		if (pCaddie)
			return &pCaddie->c;
	}
	break;
	case 8:
	{
		IFF_STRUCT::sCadItem* pCadItem = FindCadItem(tid);
		if (pCadItem)
			return &pCadItem->c;
	}
	break;
	case 9:
	{
		IFF_STRUCT::sSetItem* pSetItem = FindSetItem(tid);
		if (pSetItem)
			return &pSetItem->c;
	}
	break;
	case 10:
	{
		IFF_STRUCT::sCourse* pCourse = FindCourse(tid);
		if (pCourse)
			return &pCourse->c;
	}
	break;
	case 14:
	{
		IFF_STRUCT::sSkin* pSkin = FindSkin(tid);
		if (pSkin)
			return &pSkin->c;
	}
	break;
	case 15:
	{
		IFF_STRUCT::sHairStyle* pHairStyle = FindHairStyle(tid);
		if (pHairStyle)
			return &pHairStyle->c;
	}
	break;
	case 16:
	{
		pCommon = (IFF_ITEM_COMMON*)FindMascot(tid);
	}
	break;
	case 17:
		return NULL;
	case 28:
	{
		IFF_STRUCT::sAuxPart* pAuxPart = FindAuxPart(tid);
		if (pAuxPart)
			return &pAuxPart->c;
	}
	break;
	case 29:
	{
		IFF_STRUCT::sQuestDrop* pQuestDrop = FindQuestDrop(tid);
		if (pQuestDrop)
			return &pQuestDrop->c;
	}
	break;
	case 31:
	{
		IFF_STRUCT::sCard* pCard = FindCard(tid);
		if (pCard)
			return &pCard->c;
	}
	break;
	case 18:
	{
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			IFF_STRUCT::sFurniture* pFurniture = FindFurniture(tid);
			if (pFurniture)
				return &pFurniture->c;
		}
	}
	break;
	case 19:
	{
		if (!IsLocalContent(S4_OFFLINE_SHOP))
			return NULL;

		pCommon = (IFF_ITEM_COMMON*)FindOfflineShop(tid);
	}
	break;
	}

	if (pCommon)
		return pCommon;

	return NULL;
}

bool CItemManager::GetSetItemElem(IFF_STRUCT::sSetItem* pSetItem, int index,
	unsigned long& typeId, unsigned long& count, unsigned long& type)
{
	if (pSetItem == NULL)
		return false;

	if (index >= pSetItem->nElems)
		return false;

	typeId = pSetItem->ElemIds[index];
	switch (typeId >> 26)
	{
	case 8:
	{
		IFF_STRUCT::sCadItem* pCadItem = ItemManager()->FindCadItem(typeId);
		if (pCadItem == NULL)
			return false;

		type = pCadItem->c.Time;
		count = 1;
	}
	break;
	case 16:
	{
		IFF_STRUCT::sMascot* pMascot = ItemManager()->FindMascot(typeId);
		if (pMascot == NULL)
			return false;

		type = pMascot->c.Time;
		count = 1;
	}
	break;
	case 14:
	{
		IFF_STRUCT::sSkin* pSkin = ItemManager()->FindSkin(typeId);
		if (pSkin == NULL)
			return false;

		type = pSkin->c.Time;
		count = 1;
	}
	break;
	case 6:
	{
		IFF_STRUCT::sItem* pItem = ItemManager()->FindItem(typeId);
		if (pItem == NULL)
			return false;

		type = pItem->c.Time;
		count = pItem->COM[0];
	}
	break;
	case 5:
	{
		IFF_STRUCT::sBall* pBall = ItemManager()->FindBall(typeId);
		if (pBall == NULL)
			return false;

		type = pBall->c.Time;
		count = pBall->COM[0];
	}
	break;
	case 4:
	{
		IFF_STRUCT::sClubSet* pClubSet = ItemManager()->FindClubSet(typeId);
		if (pClubSet == NULL)
			return false;

		type = pClubSet->c.Time;
		count = 1;
	}
	break;
	case 2:
	{
		IFF_STRUCT::sPart* pPart = ItemManager()->FindPart(typeId);
		if (pPart == NULL)
			return false;

		type = pPart->c.Time;
		count = 1;
	}
	break;
	case 1:
	{
		IFF_STRUCT::sChar* pChar = ItemManager()->FindChar(typeId);
		if (pChar == NULL)
			return false;

		type = 0;
		count = 1;
	}
	break;
	case 31:
	{
		IFF_STRUCT::sCard* pCard = ItemManager()->FindCard(typeId);
		if (pCard == NULL)
			return false;

		type = 0;
		count = pCard->COM[0];
	}
	break;
	case 9:
		return false;
	default:
		type = 0;
		count = 1;
		break;
	}

	return true;
}

bool CItemManager::GetDefCombo(unsigned long tidChar,
	unsigned long* atidDefCombo)
{
	if (atidDefCombo)
	{
		IFF_STRUCT::sChar* pChar = FindChar(tidChar);
		if (pChar)
		{
			for (int i = 0; i < 24; i++)
			{
				unsigned long partTypeId =
					0x8000400 | (((tidChar << 5) | i) << 13);
				atidDefCombo[i] = FindPart(partTypeId) ? partTypeId : 0;
			}

			return true;
		}
	}

	return false;
}

bool CItemManager::GetRecycleRandomMixOutItems(unsigned long randSeqNum,
	std::list<const IFF_STRUCT::sRandomRecycle*>& ltMixOutList)
{
	ltMixOutList.clear();

	std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle>::const_iterator
		lower = m_RandomRecycleMaps.lower_bound(randSeqNum);
	std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle>::const_iterator
		upper = m_RandomRecycleMaps.upper_bound(randSeqNum);
	for (; lower != upper; ++lower)
		ltMixOutList.push_back(&lower->second);

	return !ltMixOutList.empty();
}

unsigned int CItemManager::GetItemPrice(unsigned long tid)
{
	IFF_ITEM_COMMON* pItem = FindCommonItem(tid);
	if (pItem)
		return pItem->Price;

	return (unsigned int)-1;
}

int CItemManager::CheckStockType(unsigned long typeId, unsigned int stock)
{
	int result = 0;

	IFF_ITEM_COMMON* pItem = FindCommonItem(typeId);
	if (pItem)
	{
		if (pItem->InStock == stock)
			result = 1;
	}

	return result;
}

bool CItemManager::IsValidBuyItemCount(unsigned long in_dwItemType,
	int in_iItemCount)
{
	if ((in_dwItemType & 0xfc000000) == 0x18000000)
	{
		IFF_STRUCT::sItem* pItem = FindItem(in_dwItemType);
		if (pItem == NULL)
			return false;

		if (pItem->c.IsCash && pItem->COM[0] > 0 &&
			in_iItemCount != pItem->COM[0])
			return false;
	}

	return true;
}

unsigned int CItemManager::IsValidBuyItem(unsigned long in_dwItemType)
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	if (!IsInSalePeriod(in_dwItemType, sysTime))
	{
		return 11;
	}

	IFF_ITEM_COMMON* pItem = FindCommonItem(in_dwItemType);
	if (pItem)
	{
		return pItem->InStock ? 20 : 19;
	}

	return 19;
}

unsigned int CItemManager::GetItemSalePrice(unsigned long in_dwTid,
	int in_iCount, unsigned long in_dwDayCount)
{
	unsigned long dwSalesPrice = (unsigned long)-1;

	IFF_ITEM_COMMON* pItem = FindCommonItem(in_dwTid);
	if (pItem)
	{
		unsigned long itemType = in_dwTid >> 26;

		switch (in_dwTid >> 26)
		{
		case 6:
		case 8:
		case 14:
		case 16:
			break;

		case 18:
			if (IsLocalContent(S4_OFFLINE_SHOP))
			{
				IFF_STRUCT::sFurniture* pFurniture = FindFurniture(in_dwTid);
				if (pFurniture && pFurniture->IsFunction == 4)
					break;
			}
		default:
			if (pItem->SalePrice > 0 && pItem->SalePrice < pItem->Price)
				return pItem->SalePrice;

			return pItem->Price;
		}

		unsigned long sPrice[5] = { 0, 0, 0, 0, 0 };

		switch (itemType)
		{
		case 6:
		{
			unsigned int unitPrice;
			if (pItem->SalePrice > 0 && pItem->SalePrice < pItem->Price)
				unitPrice = pItem->SalePrice;
			else
				unitPrice = pItem->Price;

			IFF_STRUCT::sItem* pItem2 = FindItem(in_dwTid);

			int num;
			if (pItem2->COM[0] > 0)
				num = in_iCount / pItem2->COM[0];
			else
				num = in_iCount;

			dwSalesPrice = num * unitPrice;
		}
		break;
		case 14:
		{
			if (S5::ISCutinItem(in_dwTid))
			{
				if (pItem->SalePrice > 0 && pItem->SalePrice < pItem->Price)
					return pItem->SalePrice;

				return pItem->Price;
			}

			if ((in_dwTid & 0x3c00000) == 0x1800000)
			{
				if (pItem->SalePrice > 0 && pItem->SalePrice < pItem->Price)
					return pItem->SalePrice;

				return pItem->Price;
			}

			IFF_STRUCT::sSkin* pSkin = FindSkin(in_dwTid);
			if (pSkin == NULL)
				return (unsigned int)-1;

			for (int i = 0; i < 5; i++)
				sPrice[i] = pSkin->COM[i];
		}
		break;
		case 8:
		{
			IFF_STRUCT::sCadItem* pCadItem = FindCadItem(in_dwTid);
			if (pCadItem == NULL)
				return (unsigned int)-1;

			for (int i = 0; i < 5; i++)
				sPrice[i] = pCadItem->COM[i];

			sPrice[1] = -1;
		}
		break;
		case 16:
		{
			IFF_STRUCT::sMascot* pMascot = FindMascot(in_dwTid);
			if (pMascot == NULL)
				return (unsigned int)-1;

			for (int i = 0; i < 5; i++)
				sPrice[i] = pMascot->COM[i];

			sPrice[2] = -1;
		}
		break;
		case 18:
		{
			if (IsLocalContent(S4_OFFLINE_SHOP))
			{
				IFF_STRUCT::sFurniture* pFurniture = FindFurniture(in_dwTid);
				if (pFurniture == NULL)
					return (unsigned int)-1;

				for (int i = 0; i < 5; i++)
					sPrice[i] = pFurniture->COM[i];
			}
		}
		break;
		}

		switch (in_dwTid >> 26)
		{
		case 8:
		case 14:
		case 16:
		case 18:
			switch (in_dwDayCount)
			{
			case 1:
				dwSalesPrice = sPrice[0];
				break;
			case 7:
				dwSalesPrice = sPrice[1];
				break;
			case 15:
				dwSalesPrice = sPrice[2];
				break;
			case 30:
				dwSalesPrice = sPrice[3];
				break;
			case 365:
				dwSalesPrice = sPrice[4];
				break;
			default:
				dwSalesPrice = (unsigned int)-1;
				break;
			}
			break;
		}
	}

	return dwSalesPrice;
}

unsigned int CItemManager::GetChildItemPrice(unsigned long tid, int iIndex)
{
	IFF_STRUCT::sChildItem* pChildItem = FindChildItem(tid, iIndex);
	if (pChildItem)
	{
		if (pChildItem->NumberFlag)
			return pChildItem->NumberPrice;

		if (pChildItem->TimeFlag)
			return pChildItem->TimePrice;
	}

	return (unsigned int)-1;
}

bool CItemManager::IsCashItem(unsigned long tid)
{
	switch (tid >> 26)
	{
	case 1:
	{
		IFF_STRUCT::sChar* pChar = FindChar(tid);
		if (pChar)
			return pChar->c.IsCash;
	}
	break;
	case 2:
	{
		IFF_STRUCT::sPart* pPart = FindPart(tid);
		if (pPart)
			return pPart->c.IsCash;
	}
	break;
	case 3:
	{
		IFF_STRUCT::sClub* pClub = FindClub(tid);
		if (pClub)
			return pClub->c.IsCash;
	}
	break;
	case 4:
	{
		IFF_STRUCT::sClubSet* pClubSet = FindClubSet(tid);
		if (pClubSet)
			return pClubSet->c.IsCash;
	}
	break;
	case 5:
	{
		IFF_STRUCT::sBall* pBall = FindBall(tid);
		if (pBall)
			return pBall->c.IsCash;
	}
	break;
	case 6:
	{
		IFF_STRUCT::sItem* pItem = FindItem(tid);
		if (pItem)
			return pItem->c.IsCash;
	}
	break;
	case 7:
	{
		IFF_STRUCT::sCaddie* pCaddie = FindCaddie(tid);
		if (pCaddie)
			return pCaddie->c.IsCash;
	}
	break;
	case 8:
	{
		IFF_STRUCT::sCadItem* pCadItem = FindCadItem(tid);
		if (pCadItem)
			return pCadItem->c.IsCash;
	}
	break;
	case 9:
	{
		IFF_STRUCT::sSetItem* pSetItem = FindSetItem(tid);
		if (pSetItem)
			return pSetItem->c.IsCash;
	}
	break;
	case 14:
	{
		IFF_STRUCT::sSkin* pSkin = FindSkin(tid);
		if (pSkin)
			return pSkin->c.IsCash;
	}
	break;
	case 15:
	{
		IFF_STRUCT::sHairStyle* pHairStyle = FindHairStyle(tid);
		if (pHairStyle)
			return pHairStyle->c.IsCash;
	}
	break;
	case 16:
	{
		IFF_STRUCT::sMascot* pMascot = FindMascot(tid);
		if (pMascot)
			return pMascot->c.IsCash;
	}
	break;
	case 28:
	{
		IFF_STRUCT::sAuxPart* pAuxPart = FindAuxPart(tid);
		if (pAuxPart)
			return pAuxPart->c.IsCash;
	}
	break;
	case 29:
	{
		IFF_STRUCT::sQuestDrop* pQuestDrop = FindQuestDrop(tid);
		if (pQuestDrop)
			return pQuestDrop->c.IsCash;
	}
	break;
	case 31:
	{
		IFF_STRUCT::sCard* pCard = FindCard(tid);
		if (pCard)
			return pCard->c.IsCash;
	}
	break;
	case 18:
	{
		if (IsLocalContent(S4_REAL_MYROOM))
		{
			IFF_STRUCT::sFurniture* pFurniture = FindFurniture(tid);
			if (pFurniture)
				return pFurniture->c.IsCash;
		}
	}
	break;
	case 19:
	{
		if (IsLocalContent(S4_OFFLINE_SHOP))
		{
			IFF_STRUCT::sOfflineShop* pOfflineShop = FindOfflineShop(tid);
			if (pOfflineShop)
				return pOfflineShop->c.IsCash;
		}
	}
	break;
	}

	return true;
}

bool CItemManager::IsCashChildItem(unsigned long tid, int iIndex)
{
	IFF_STRUCT::sChildItem* pChildItem = FindChildItem(tid, iIndex);
	if (pChildItem)
		return pChildItem->IsCash;

	return true;
}

bool CItemManager::IsTimeLimit(unsigned long tid)
{
	unsigned long group = tid >> 26;

	switch (group)
	{
	case 6:
	{
		IFF_STRUCT::sItem* pItem = FindItem(tid);
		if (pItem == NULL)
			break;

		if (pItem->c.TimeFlag)
			return true;
	}
	break;
	case 14:
	{
		IFF_STRUCT::sSkin* pSkin = FindSkin(tid);
		if (pSkin == NULL)
			break;

		if (pSkin->c.TimeFlag == 1 || pSkin->c.TimeFlag == 2)
			return true;
	}
	break;
	}

	return false;
}

const char* CItemManager::GetItemName(unsigned long tid)
{
	if (tid != 0)
	{
		switch (tid >> 26)
		{
		case 1:
		{
			IFF_STRUCT::sChar* pChar = FindChar(tid);
			if (pChar)
				return pChar->c.Name;
		}
		break;
		case 2:
		{
			IFF_STRUCT::sPart* pPart = FindPart(tid);
			if (pPart)
				return pPart->c.Name;
		}
		break;
		case 3:
		{
			IFF_STRUCT::sClub* pClub = FindClub(tid);
			if (pClub)
				return pClub->c.Name;
		}
		break;
		case 4:
		{
			IFF_STRUCT::sClubSet* pClubSet = FindClubSet(tid);
			if (pClubSet)
				return pClubSet->c.Name;
		}
		break;
		case 5:
		{
			IFF_STRUCT::sBall* pBall = FindBall(tid);
			if (pBall)
				return pBall->c.Name;
		}
		break;
		case 6:
		{
			IFF_STRUCT::sItem* pItem = FindItem(tid);
			if (pItem)
				return pItem->c.Name;
		}
		break;
		case 7:
		{
			IFF_STRUCT::sCaddie* pCaddie = FindCaddie(tid);
			if (pCaddie)
				return pCaddie->c.Name;
		}
		break;
		case 8:
		{
			IFF_STRUCT::sCadItem* pCadItem = FindCadItem(tid);
			if (pCadItem)
				return pCadItem->c.Name;
		}
		break;
		case 9:
		{
			IFF_STRUCT::sSetItem* pSetItem = FindSetItem(tid);
			if (pSetItem)
				return pSetItem->c.Name;
		}
		break;
		case 10:
		{
			IFF_STRUCT::sCourse* pCourse = FindCourse(tid);
			if (pCourse)
				return pCourse->c.Name;
		}
		break;
		case 11:
		{
			IFF_STRUCT::sMatch* pMatch = FindMatch(tid);
			if (pMatch)
				return pMatch->Name;
		}
		break;
		case 12:
		{
			IFF_STRUCT::sTitle* pTitle = FindTitle(tid);
			if (pTitle)
				return pTitle->Name;
		}
		break;
		case 14:
		{
			IFF_STRUCT::sSkin* pSkin = FindSkin(tid);
			if (pSkin)
				return pSkin->c.Name;
		}
		break;
		case 15:
		{
			IFF_STRUCT::sHairStyle* pHairStyle = FindHairStyle(tid);
			if (pHairStyle)
				return pHairStyle->c.Name;
		}
		break;
		case 16:
		{
			IFF_STRUCT::sMascot* pMascot = FindMascot(tid);
			if (pMascot)
				return pMascot->c.Name;
		}
		break;
		case 28:
		{
			IFF_STRUCT::sAuxPart* pAuxPart = FindAuxPart(tid);
			if (pAuxPart)
				return pAuxPart->c.Name;
		}
		break;
		case 29:
		{
			IFF_STRUCT::sQuestDrop* pQuestDrop = FindQuestDrop(tid);
			if (pQuestDrop)
				return pQuestDrop->c.Name;
		}
		break;
		case 30:
		{
			IFF_STRUCT::sQuest* pQuest = FindQuest(tid);
			if (pQuest)
				return pQuest->Name;
		}
		break;
		case 31:
		{
			IFF_STRUCT::sCard* pCard = FindCard(tid);
			if (pCard)
				return pCard->c.Name;
		}
		break;
		case 18:
		{
			if (IsLocalContent(S4_REAL_MYROOM))
			{
				IFF_STRUCT::sFurniture* pFurniture = FindFurniture(tid);
				if (pFurniture)
					return pFurniture->c.Name;
			}
		}
		break;
		}
	}

	return "unknown";
}

bool CItemManager::HasSalePeriod(unsigned long tid, _SYSTEMTIME** pSaleS,
	_SYSTEMTIME** pSaleE)
{
	*pSaleS = NULL;
	*pSaleE = NULL;

	IFF_ITEM_COMMON* pItem = FindCommonItem(tid);
	if (pItem)
	{
		*pSaleS = &pItem->saleStart;
		*pSaleE = &pItem->saleEnd;
	}

	return *pSaleS != NULL ? true : false;
}

bool CItemManager::IsInSalePeriod(unsigned long tid, _SYSTEMTIME& sysTime)
{
	_SYSTEMTIME* pStart;
	_SYSTEMTIME* pEnd;

	if (!HasSalePeriod(tid, &pStart, &pEnd))
		return true;

	return IsBetween(pStart, pEnd, sysTime);
}

int CItemManager::CompareSystemTime(_SYSTEMTIME& tm0, _SYSTEMTIME& tm1)
{
	if (tm0.wYear < tm1.wYear)
		return -1;
	if (tm0.wYear > tm1.wYear)
		return 1;

	if (tm0.wMonth < tm1.wMonth)
		return -1;
	if (tm0.wMonth > tm1.wMonth)
		return 1;

	if (tm0.wDay < tm1.wDay)
		return -1;
	if (tm0.wDay > tm1.wDay)
		return 1;

	if (tm0.wHour < tm1.wHour)
		return -1;
	if (tm0.wHour > tm1.wHour)
		return 1;

	if (tm0.wMinute < tm1.wMinute)
		return -1;
	if (tm0.wMinute > tm1.wMinute)
		return 1;

	if (tm0.wSecond < tm1.wSecond)
		return -1;
	if (tm0.wSecond > tm1.wSecond)
		return 1;

	if (tm0.wMilliseconds < tm1.wMilliseconds)
		return -1;
	if (tm0.wMilliseconds > tm1.wMilliseconds)
		return 1;

	return 0;
}

int CItemManager::GetCouponKind(unsigned long dwTid)
{
	int kind = 0;

	if (dwTid & 0x2000000)
	{
		switch (dwTid & 0x1ffffff)
		{
		case 0x15:
		case 0x16:
		case 0x17:
		case 0x1d:
		case 0x1e:
		case 0x2b:
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x3a:
		case 0x3c:
		case 0x4e:
			kind = 1;
			break;
		case 0x3d:
		case 0x3f:
		case 0x53:
			kind = 5;
			break;
		case 0x28:
		case 0x29:
		case 0x2a:
			kind = 2;
			break;
		case 0x30:
		case 0x33:
		case 0x34:
		case 0xa3:
			kind = 3;
			break;
		case 0x36:
		case 0x63:
		case 0x64:
		case 0x65:
		case 0x66:
		case 0x67:
		case 0x68:
		case 0x69:
		case 0x6a:
		case 0x6b:
		case 0x6c:
		case 0x6d:
		case 0x6e:
		case 0x6f:
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0xd2:
			kind = 4;
			break;
		default:
			kind = 0;
			break;
		}
	}

	return kind;
}

bool CItemManager::IsDisableDuplicateItem(unsigned long dwTid)
{
	if (dwTid & 0x2000000)
		return (dwTid & 0x1ffffff) == 0xf ? true : false;

	return false;
}

bool CItemManager::IsDisplayItemNumber(unsigned long dwTid)
{
	bool bDisplay = false;

	switch (dwTid >> 26)
	{
	case 6:
		if ((dwTid & 0x2000000) && (dwTid & 0x1ffffff) == 6)
			break;

		bDisplay = true;
		break;
	case 5:
		if (IsUnlimitAztec(dwTid))
			break;

		bDisplay = true;
		break;
	case 28:
	{
		IFF_STRUCT::sAuxPart* pAuxPart = FindAuxPart(dwTid);
		if (pAuxPart == NULL)
			break;

		if (pAuxPart->COM[0] == 0)
			break;

		bDisplay = true;
	}
	break;
	case 31:
		bDisplay = true;
		break;
	}

	return bDisplay;
}

unsigned long CItemManager::GetIndexAbilityItem(unsigned long dwTid)
{
	unsigned long index = 0;

	switch (dwTid)
	{
	case 0x801a80a:
		index = 1;
		break;
	case 0x70000005:
		index = 8;
		break;
	case 0x821a801:
		index = 1;
		break;
	case 0x821a810:
	case 0x821a811:
		index = 1;
		break;
	case 0x8016800:
	case 0x8058800:
	case 0x8098800:
	case 0x80dc800:
	case 0x8118800:
	case 0x8160800:
	case 0x8190800:
	case 0x81e2800:
	case 0x8214800:
	case 0x8254800:
		index = 8;
		break;
	case 0x801a80e:
		index = 1;
		break;
	case 0x8050810:
		index = 1;
		break;
	case 0x8064800:
	case 0x8064801:
	case 0x8064802:
	case 0x8064803:
		index = 13;
		break;
	case 0x8112017:
		index = 1;
		break;
	case 0x81d8806:
	case 0x81d8807:
	case 0x81d8808:
	case 0x81d8809:
		index = 10;
		break;
	case 0x81d881b:
		index = 1;
		break;
	case 0x816e801:
	case 0x816e802:
	case 0x816e803:
		index = 13;
		break;
	case 0x81d881f:
	case 0x81d8820:
		index = 1;
		break;
	case 0x821a805:
	case 0x821a806:
		index = 10;
		break;
	case 0x8268800:
		index = 13;
		break;
	case 0x809080d:
	case 0x80d0839:
	case 0x80d083a:
	case 0x811a81e:
	case 0x811a81f:
	case 0x8180064:
	case 0x821a807:
	case 0x825200c:
	case 0x825200d:
	case 0x825a805:
	case 0x825a806:
	case 0x825a80c:
		index = 1;
		break;
	case 0x800082d:
	case 0x800082e:
		index = 1;
		break;
	case 0x801a817:
	case 0x801a818:
		index = 1;
		break;
	case 0x8026800:
	case 0x8026801:
	case 0x8026802:
		index = 13;
		break;
	case 0x8090810:
		index = 1;
		break;
	case 0x809a822:
	case 0x809a823:
		index = 1;
		break;
	case 0x80d0819:
	case 0x80d081a:
		index = 10;
		break;
	case 0x811200f:
		index = 1;
		break;
	case 0x8112012:
		index = 1;
		break;
	case 0x816280a:
		index = 1;
		break;
	case 0x818e04a:
	case 0x818e04b:
		index = 1;
		break;
	case 0x8196009:
		index = 10;
		break;
	case 0x81da807:
		index = 10;
		break;
	case 0x81da812:
	case 0x81da813:
		index = 10;
		break;
	case 0x821a802:
		index = 10;
		break;
	case 0x8228800:
	case 0x8228801:
	case 0x8228802:
	case 0x8228803:
		index = 13;
		break;
	case 0x825a80d:
		index = 10;
		break;
	case 0x8122800:
	case 0x8122801:
	case 0x8122802:
		index = 13;
		break;
	case 0x8000884:
		index = 1;
		break;
	case 0x80c0033:
		index = 1;
		break;
	case 0x80d480e:
		index = 1;
		break;
	case 0x80d4823:
	case 0x80d4824:
		index = 1;
		break;
	case 0x8162815:
	case 0x8162816:
		index = 1;
		break;
	case 0x8196010:
	case 0x8196011:
		index = 10;
		break;
	case 0x8196012:
		index = 1;
		break;
	case 0x81a4800:
	case 0x81a4801:
		index = 13;
		break;
	case 0x81da808:
		index = 1;
		break;
	case 0x81da80b:
		index = 1;
		break;
	case 0x81ea800:
		index = 13;
		break;
	case 0x8000828:
	case 0x8000829:
	case 0x800082a:
	case 0x800082b:
	case 0x801a806:
	case 0x801a80c:
	case 0x801a80d:
	case 0x8050809:
	case 0x805080e:
	case 0x805080f:
	case 0x805a809:
	case 0x805a80a:
	case 0x805a80b:
	case 0x805a80c:
	case 0x805a80f:
	case 0x805a810:
	case 0x809080c:
	case 0x8090815:
	case 0x8090816:
	case 0x809a80c:
	case 0x809a80d:
	case 0x809a80e:
	case 0x809a80f:
	case 0x80d0813:
	case 0x80d0814:
	case 0x80d0815:
	case 0x80d0816:
	case 0x80d480b:
	case 0x80d4817:
	case 0x80d4818:
	case 0x811200e:
	case 0x8112015:
	case 0x8112016:
	case 0x811a00c:
	case 0x811a00d:
	case 0x811a00e:
	case 0x811a00f:
	case 0x815a018:
	case 0x815a019:
	case 0x815a01a:
	case 0x815a01b:
	case 0x815a01f:
	case 0x815a020:
	case 0x8162807:
	case 0x816280c:
	case 0x816280d:
	case 0x818e01c:
	case 0x818e01d:
	case 0x818e01e:
	case 0x818e01f:
	case 0x818e026:
	case 0x818e027:
		index = 10;
		break;
	case 0x8000886:
	case 0x8000887:
		index = 1;
		break;
	case 0x80d0817:
	case 0x80d0818:
		index = 1;
		break;
	case 0x8140046:
		index = 1;
		break;
	case 0x8196008:
		index = 1;
		break;
	case 0x819600d:
		index = 1;
		break;
	case 0x81d880a:
	case 0x81d880b:
		index = 1;
		break;
	case 0x81da814:
		index = 1;
		break;
	case 0x801a807:
	case 0x8050808:
	case 0x805a80d:
	case 0x805a80e:
	case 0x805a823:
	case 0x805a824:
	case 0x8090817:
	case 0x8090824:
	case 0x8090825:
	case 0x809a810:
	case 0x809a811:
	case 0x809a81f:
	case 0x8100037:
	case 0x8112020:
	case 0x8112021:
	case 0x811a010:
	case 0x811a011:
	case 0x816280e:
	case 0x819601c:
	case 0x819601d:
	case 0x81da81c:
	case 0x81da81d:
	case 0x824001c:
		index = 1;
		break;
	case 0x804086e:
		index = 1;
		break;
	case 0x805080c:
		index = 1;
		break;
	case 0x8050816:
	case 0x8050817:
		index = 1;
		break;
	case 0x80a2800:
	case 0x80a2801:
	case 0x80a2802:
		index = 13;
		break;
	case 0x80d480a:
		index = 1;
		break;
	case 0x80d4819:
		index = 1;
		break;
	case 0x80e4800:
	case 0x80e4801:
	case 0x80e4802:
		index = 13;
		break;
	case 0x815a01d:
	case 0x815a01e:
		index = 1;
		break;
	case 0x815a84f:
	case 0x815a850:
		index = 1;
		break;
	case 0x8162806:
		index = 1;
		break;
	case 0x818e024:
	case 0x818e025:
		index = 1;
		break;
	case 0x821203f:
		index = 1;
		break;
	case 0x8212043:
	case 0x8212044:
		index = 1;
		break;
	}

	return index;
}

unsigned long CItemManager::GetMascotBonusPang(unsigned long dwTid,
	bool bBestShot)
{
	unsigned long bonus = 0;

	if ((dwTid & 0xfc000000) == 0x40000000)
	{
		switch (dwTid & 0x3ffffff)
		{
		case 0:
		case 1:
		case 2:
		case 3:
			bonus = bBestShot ? 10 : 0;
			break;
		case 6:
		case 0xe:
		case 0x12:
			bonus = bBestShot ? 10 : 5;
			break;
		case 4:
		case 0xd:
		case 0x10:
		case 0x13:
			bonus = bBestShot ? 15 : 5;
			break;
		case 9:
		case 0xa:
		case 0xb:
		case 0xc:
			bonus = bBestShot ? 15 : 0;
			break;
		}
	}

	return bonus;
}

bool CItemManager::IsBetween(_SYSTEMTIME* pStart, _SYSTEMTIME* pEnd,
	_SYSTEMTIME& sysTime)
{
	if (pStart != NULL && pEnd != NULL)
	{
		if (pStart->wYear > 0 && pEnd->wYear > 0)
		{
			if (CompareSystemTime(*pStart, sysTime) <= 0 &&
				CompareSystemTime(sysTime, *pEnd) <= 0)
				return true;
			else
				return false;
		}
		else if (pStart->wYear > 0)
		{
			return CompareSystemTime(*pStart, sysTime) <= 0 ? true : false;
		}
		else if (pEnd->wYear > 0)
		{
			return CompareSystemTime(sysTime, *pEnd) <= 0 ? true : false;
		}
	}
	else if (pStart != NULL)
	{
		return CompareSystemTime(*pStart, sysTime) <= 0 ? true : false;
	}
	else if (pEnd != NULL)
	{
		return CompareSystemTime(sysTime, *pEnd) <= 0 ? true : false;
	}

	return true;
}

int CItemManager::GetNumItems(PANGYA_ITEM_GROUP pig)
{
	int num = 0;

	if (m_IsReloading)
	{
		_client::_private::CLock<_client::CCriticalSection> lock(m_cs);

		switch (pig)
		{
		case PIG_CHAR:
			num = m_CharMap.size();
			break;
		case PIG_PART:
			num = m_PartMap.size();
			break;
		case PIG_CLUB:
			num = m_ClubMap.size();
			break;
		case PIG_CLUBSET:
			num = m_ClubSetMap.size();
			break;
		case PIG_BALL:
			num = m_BallMap.size();
			break;
		case PIG_ITEM:
			num = m_ItemMap.size();
			break;
		case PIG_CADDIE:
			num = m_CaddieMap.size();
			break;
		case PIG_CADITEM:
			num = m_CadItemMap.size();
			break;
		case PIG_SETITEM:
			num = m_SetItemMap.size();
			break;
		case PIG_COURSE:
			num = m_CourseMap.size();
			break;
		case PIG_MATCH:
			num = m_MatchMap.size();
			break;
		case PIG_TITLE:
			num = m_TitleMap.size();
			break;
		case PIG_SKIN:
			num = m_SkinMap.size();
			break;
		case PIG_ENCHANT:
			num = m_EnchantMap.size();
			break;
		case PIG_HAIR:
			num = m_HairStyleMap.size();
			break;
		case PIG_MASCOT:
			num = m_MascotMap.size();
			break;
		case PIG_CHILDITEM:
			num = m_ChildItemMap.size();
			break;
		case PIG_AUXPART:
			num = m_AuxPartMap.size();
			break;
		case PIG_QUESTDROP:
			num = m_QuestDropMap.size();
			break;
		case PIG_QUEST:
			num = m_QuestMap.size();
			break;
		case PIG_CARD:
			num = m_CardMap.size();
			break;
		case PIG_FURNITURE:
			if (IsLocalContent(S4_REAL_MYROOM))
				num = m_FurnitureMap.size();
			break;
		case PIG_OFFLINESHOP:
			if (IsLocalContent(S4_OFFLINE_SHOP))
				num = m_OfflineShopMap.size();
			break;
		}
	}
	else
	{
		switch (pig)
		{
		case PIG_CHAR:
			num = m_CharMap.size();
			break;
		case PIG_PART:
			num = m_PartMap.size();
			break;
		case PIG_CLUB:
			num = m_ClubMap.size();
			break;
		case PIG_CLUBSET:
			num = m_ClubSetMap.size();
			break;
		case PIG_BALL:
			num = m_BallMap.size();
			break;
		case PIG_ITEM:
			num = m_ItemMap.size();
			break;
		case PIG_CADDIE:
			num = m_CaddieMap.size();
			break;
		case PIG_CADITEM:
			num = m_CadItemMap.size();
			break;
		case PIG_SETITEM:
			num = m_SetItemMap.size();
			break;
		case PIG_COURSE:
			num = m_CourseMap.size();
			break;
		case PIG_MATCH:
			num = m_MatchMap.size();
			break;
		case PIG_TITLE:
			num = m_TitleMap.size();
			break;
		case PIG_SKIN:
			num = m_SkinMap.size();
			break;
		case PIG_ENCHANT:
			num = m_EnchantMap.size();
			break;
		case PIG_HAIR:
			num = m_HairStyleMap.size();
			break;
		case PIG_CHILDITEM:
			num = m_ChildItemMap.size();
			break;
		case PIG_AUXPART:
			num = m_AuxPartMap.size();
			break;
		case PIG_QUESTDROP:
			num = m_QuestDropMap.size();
			break;
		case PIG_QUEST:
			num = m_QuestMap.size();
			break;
		case PIG_CARD:
			num = m_CardMap.size();
			break;
		case PIG_FURNITURE:
			if (IsLocalContent(S4_REAL_MYROOM))
				num = m_FurnitureMap.size();
			break;
		case PIG_OFFLINESHOP:
			if (IsLocalContent(S4_OFFLINE_SHOP))
				num = m_OfflineShopMap.size();
			break;
		}
	}

	return num;
}

int CItemManager::GetCourseNum()
{
	int num;

	if (m_IsReloading)
	{
		_client::_private::CLock<_client::CCriticalSection> lock(m_cs);

		num = m_CourseMap.size() - 3;
	}
	else
	{
		num = m_CourseMap.size() - 3;
	}

	return num;
}

unsigned char CItemManager::GetItemLevel(eLvlType eType,
	const unsigned long* patidParts, const unsigned long* patidAuxParts)
{
	if (patidParts == NULL)
		return 0;

	unsigned char level = 0;

	IFF_STRUCT::sPart* apParts[24];
	for (int i = 0; i < 24; i++)
	{
		if (patidParts[i] == 0)
			apParts[i] = NULL;
		else
			apParts[i] = FindPart(patidParts[i]);

		if (apParts[i])
			level += (unsigned char)apParts[i]->Attr[eType];
	}

	if (patidAuxParts && eType < 5)
	{
		IFF_STRUCT::sAuxPart* apAuxParts[5];
		for (int i = 0; i < 5; i++)
		{
			if (patidAuxParts[i] == 0)
				apAuxParts[i] = NULL;
			else
				apAuxParts[i] = FindAuxPart(patidAuxParts[i]);

			if (apAuxParts[i] && apAuxParts[i]->Attr[eType] != 99)
				level += apAuxParts[i]->Attr[eType];
		}
	}

	return level;
}

unsigned char CItemManager::GetCharCapacity(eLvlType eType,
	unsigned long tidChar, const unsigned long* patidParts,
	const unsigned long* patidAuxParts, unsigned char level)
{
	unsigned char capacity;

	if (tidChar)
	{
		IFF_STRUCT::sChar* pChar = FindChar(tidChar);
		if (pChar)
			capacity = (unsigned char)pChar->Attr[eType];
		else
			capacity = 0;
	}
	else
	{
		capacity = 0;
	}

	if (eType == 0 && level >= 6)
		capacity += (level - 1) / 5;

	if (patidParts)
	{
		IFF_STRUCT::sPart* apParts[24];
		for (int i = 0; i < 24; i++)
		{
			if (patidParts[i] == 0)
				apParts[i] = NULL;
			else
				apParts[i] = FindPart(patidParts[i]);
			if (apParts[i])
				capacity += (unsigned char)apParts[i]->Slot[eType];
		}
	}

	if (patidAuxParts && eType < 5)
	{
		IFF_STRUCT::sAuxPart* apAuxParts[5];
		for (int i = 0; i < 5; i++)
		{
			if (patidAuxParts[i] == 0)
				apAuxParts[i] = NULL;
			else
				apAuxParts[i] = FindAuxPart(patidAuxParts[i]);
			if (apAuxParts[i] && apAuxParts[i]->Slot[eType] != 99)
				capacity += apAuxParts[i]->Slot[eType];
		}
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
		capacity += CCardManager::Instance()->GetCardStatusSlot(eType);

	return capacity;
}

unsigned char CItemManager::GetCharLevel(eLvlType eType, unsigned long tidChar,
	const char* pCharPCL)
{
	if (tidChar == 0)
		return 0;

	IFF_STRUCT::sChar* pChar = FindChar(tidChar);
	if (pChar == NULL)
		return 0;

	unsigned char level = pChar->PCL[eType];
	if (pCharPCL)
		level += pCharPCL[eType];

	return level;
}

unsigned char CItemManager::GetCapacity(eLvlType type, unsigned long charTypeId,
	const char* pStat, const unsigned long* pParts, unsigned long clubTypeId,
	const short* pClubStat, unsigned long caddieTypeId, unsigned char upgrade)
{
	IFF_STRUCT::sChar* pChar = FindChar(charTypeId);
	IFF_STRUCT::sClubSet* pClubSet = FindClubSet(clubTypeId);

	unsigned char capacity;
	if (pChar)
	{
		switch (type)
		{
		case 0:
			capacity = (unsigned char)pChar->Attr[0];
			if (upgrade >= 6)
				capacity += (upgrade - 1) / 5;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			capacity = (unsigned char)pChar->Attr[type];
			break;
		}
	}
	else
	{
		capacity = 0;
	}

	if (pParts)
	{
		IFF_STRUCT::sPart* parts[24];
		for (int i = 0; i < 24; i++)
		{
			if (pParts[i] == 0)
				parts[i] = NULL;
			else
				parts[i] = FindPart(pParts[i]);
			if (parts[i])
				capacity += (unsigned char)parts[i]->Slot[type] +
					(unsigned char)parts[i]->Attr[type];
		}
	}

	if (pClubSet)
		capacity += (unsigned char)pClubSet->Slot[type];

	IFF_STRUCT::sCaddie* pCaddie;
	if ((caddieTypeId & 0xfc000000) == 0x20000000)
		pCaddie = FindCaddie(((caddieTypeId >> 21) & 0x1f) | 0x1c000000);
	else
		pCaddie = FindCaddie(caddieTypeId);
	unsigned char caddie;
	if (pCaddie && pCaddie->c.Level <= upgrade)
		caddie = (unsigned char)pCaddie->Attr[type];
	else
		caddie = 0;

	capacity += caddie;

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		capacity += CCardManager::Instance()->GetCardPeriodStatus(type);
		capacity += CCardManager::Instance()->GetCardStatusSlot(type);
	}

	return capacity;
}

unsigned char CItemManager::GetLevel(eLvlType type, unsigned long charTypeId,
	const char* pStat, const unsigned long* pParts,
	const unsigned long* pAuxParts, unsigned long clubTypeId,
	const short* pClubStat, unsigned long caddieTypeId, unsigned char upgrade)
{
	IFF_STRUCT::sChar* pChar = FindChar(charTypeId);
	IFF_STRUCT::sClubSet* pClubSet = FindClubSet(clubTypeId);

	unsigned char level;
	unsigned char capacity;

	if (pChar)
	{
		level = pChar->PCL[type];

		switch (type)
		{
		case 0:
			capacity = (unsigned char)pChar->Attr[0];
			if (upgrade >= 6)
				capacity += (upgrade - 1) / 5;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			capacity = (unsigned char)pChar->Attr[type];
			break;
		}
	}
	else
	{
		level = 0;
		capacity = 0;
	}

	if (pParts)
	{
		IFF_STRUCT::sPart* parts[24];
		for (int i = 0; i < 24; i++)
		{
			if (pParts[i] == 0)
				parts[i] = NULL;
			else
				parts[i] = FindPart(pParts[i]);

			if (parts[i])
			{
				level += (unsigned char)parts[i]->Attr[type];
				capacity += (unsigned char)parts[i]->Slot[type] +
					(unsigned char)parts[i]->Attr[type];
			}
		}
	}

	if (pAuxParts && type < 5)
	{
		IFF_STRUCT::sAuxPart* auxParts[5];
		for (int i = 0; i < 5; i++)
		{
			if (pAuxParts[i] == 0)
				auxParts[i] = NULL;
			else
				auxParts[i] = FindAuxPart(pAuxParts[i]);

			if (auxParts[i])
			{
				if (auxParts[i]->Attr[type] != 99)
				{
					level += auxParts[i]->Attr[type];
					capacity += auxParts[i]->Attr[type];
				}

				if (auxParts[i]->Slot[type] != 99)
					capacity += auxParts[i]->Slot[type];
			}
		}
	}

	if (IsLocalContent(S4_CARD_SYSTEM))
	{
		capacity += CCardManager::Instance()->GetCardStatusSlot(type) +
			CCardManager::Instance()->GetCardPeriodStatus(type);
	}

	if (pStat)
	{
		level += pStat[type];
		if (level > capacity)
			level = capacity;
	}
	else
	{
		level = capacity;
	}

	unsigned char club = 0;
	if (pClubSet)
		club = (unsigned char)pClubSet->Attr[type];

	if (pClubStat)
		club += (unsigned char)pClubStat[type];

	IFF_STRUCT::sCaddie* pCaddie;
	if ((caddieTypeId & 0xfc000000) == 0x20000000)
		pCaddie = FindCaddie(((caddieTypeId >> 21) & 0x1f) | 0x1c000000);
	else
		pCaddie = FindCaddie(caddieTypeId);

	unsigned char caddie;
	if (pCaddie && pCaddie->c.Level <= upgrade)
		caddie = (unsigned char)pCaddie->Attr[type];
	else
		caddie = 0;

	if (IsLocalContent(S4_CARD_SYSTEM))
		return CCardManager::Instance()->GetCardPeriodStatus(type) + caddie +
			club + level;

	return caddie + club + level;
}

int CalcPowerPenalty(int upgrade)
{
	if (upgrade >= 66)
		return 13;
	if (upgrade >= 61)
		return 12;
	if (upgrade >= 56)
		return 11;
	if (upgrade >= 51)
		return 10;
	if (upgrade >= 46)
		return 9;
	if (upgrade >= 41)
		return 8;
	if (upgrade >= 36)
		return 7;
	if (upgrade >= 31)
		return 6;
	if (upgrade >= 26)
		return 5;
	if (upgrade >= 21)
		return 4;
	if (upgrade >= 16)
		return 3;
	if (upgrade >= 11)
		return 2;
	if (upgrade >= 6)
		return 1;

	return 0;
}

status_t CItemManager::GetLevelWithPenalty(eLvlType type,
	unsigned long charTypeId, const char* pStat, const unsigned long* pParts,
	const unsigned long* pAuxParts, unsigned long clubTypeId,
	const short* pClubStat, unsigned long caddieTypeId, unsigned char upgrade)
{
	unsigned char penalty = 0;
	unsigned char level = GetLevel(type, charTypeId, pStat, pParts, pAuxParts,
		clubTypeId, pClubStat, caddieTypeId, upgrade);

	if (type == 1 || type == 2)
	{
		unsigned char power = GetLevel((eLvlType)0, charTypeId, pStat, pParts,
			pAuxParts, clubTypeId, pClubStat, caddieTypeId, upgrade);

		penalty = Max(0, power - CalcPowerPenalty(upgrade) - 20);

		if (level <= penalty)
		{
			level = 0;
		}
		else
		{
			level -= penalty;
		}
	}

	status_t status;
	status.level = level;
	status.penalty = penalty;

	return status;
}

bool CItemManager::isTrade(unsigned long typeId)
{
	IFF_ITEM_COMMON* pItem = FindCommonItem(typeId);
	if (pItem == NULL)
		return false;

	return pItem->IsSalable == 1 ? true : false;
}

bool CItemManager::IsCanOverlapped(unsigned long dwTID,
	unsigned char btCheckType, unsigned char exceptGroup)
{
	bool bResult = false;

	IFF_ITEM_COMMON* pItem = FindCommonItem(dwTID);
	if (pItem == NULL)
		return false;

	switch (dwTID >> 26)
	{
	case 1:
		if (exceptGroup == 1)
			bResult = true;
		break;
	case 7:
		if (btCheckType == 1)
		{
			if (dwTID == 0x1c000001 || dwTID == 0x1c000002 ||
				dwTID == 0x1c000003 || dwTID == 0x1c000007)
				bResult = true;
		}
		break;
	case 2:
		if (pItem->IsSalable != 1 && pItem->IsSalable != 3)
			bResult = false;
		else
			bResult = true;
		break;
	case 4:
	case 8:
	case 14:
	case 15:
		bResult = false;
		break;
	case 9:
		if (((dwTID >> 21) & 0x1f) == 6 || ((dwTID >> 21) & 0x1f) == 5)
			bResult = true;
		else
			bResult = false;
		break;
	case 28:
		if ((dwTID & 0x1f0000) == 0)
			bResult = true;
		break;
	default:
		bResult = true;
		break;
	}

	return bResult;
}

bool CItemManager::IsPartAttachCard(unsigned long dwCardTid,
	unsigned long dwPartTid, int iSlotNum)
{
	unsigned long kind = (dwCardTid >> 22) & 0xf;
	bool bAttach = false;

	IFF_STRUCT::sPart* pPart = FindPart(dwPartTid);
	if (pPart)
	{
		if (kind == 0)
		{
			if (pPart->CharacterSlot >= iSlotNum && iSlotNum > 0 &&
				iSlotNum < 5)
				bAttach = true;
		}
		else if (kind == 1)
		{
			iSlotNum -= 4;
			if (pPart->CaddieSlot >= iSlotNum && iSlotNum > 0 && iSlotNum < 5)
				bAttach = true;
		}
	}

	return bAttach;
}

bool CItemManager::GetCardSlotNum(unsigned long in_TID,
	unsigned short& out_LimitCharacterSlot, unsigned short& out_LimitCaddieSlot)
{
	out_LimitCharacterSlot = 0;
	out_LimitCaddieSlot = 0;

	switch (in_TID >> 26)
	{
	case 2:
	{
		IFF_STRUCT::sPart* pPart = FindPart(in_TID);
		if (pPart)
		{
			out_LimitCharacterSlot = pPart->CharacterSlot;
			out_LimitCaddieSlot = pPart->CaddieSlot;
			return true;
		}
	}
	break;
	}

	return false;
}

bool CItemManager::IsEquipCard(unsigned long in_CardTID)
{
	if ((in_CardTID & 0xfc000000) != 0x7c000000)
		return false;

	return (in_CardTID & 0x3c00000) <= 0x400000 ? true : false;
}

COUNTING_TYPE CItemManager::GetItemCountingType(unsigned long in_Tid,
	unsigned char in_itemType)
{
	int type = 0;

	switch (in_Tid >> 26)
	{
	case 5:
	case 6:
	case 28:
		type = in_itemType ? 2 : 1;
		break;
	case 2:
	case 4:
		type = 3;
		break;
	case 7:
		type = 4;
		break;
	case 31:
		if (IsLocalContent(S4_CARD_SYSTEM))
			type = 5;
		break;
	}

	return (COUNTING_TYPE)type;
}

void CItemManager::MakeSpecialPrizeItemMap(char* Buf, int len)
{
	IFF_FILE_HEADER _hdr;
	IFF_STRUCT::sSpecialPrizeItem item;

	len = sizeof(IFF_FILE_HEADER);
	memcpy(&_hdr, Buf, len);
	char* p = Buf + len;

	for (int i = 0; i < _hdr.nRecords; i++)
	{
		memcpy(&item, p, sizeof(IFF_STRUCT::sSpecialPrizeItem));
		p += sizeof(IFF_STRUCT::sSpecialPrizeItem);

		m_SPItemMap[item.ability].insert(
			std::map<unsigned int, IFF_STRUCT::sSpecialPrizeItem>::value_type(
				item.typeId, item));
	}
}

IFF_STRUCT::sSpecialPrizeItem* CItemManager::FindSPItem(unsigned long typeId,
	eSPAVILITYTYPE type)
{
	IFF_STRUCT::sSpecialPrizeItem* pItem = NULL;
	std::map<unsigned int, IFF_STRUCT::sSpecialPrizeItem>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_SPItemMap[type].find(typeId);

		if (it != m_SPItemMap[type].end())
			pItem = &(*it).second;
		m_cs.unlock();
	}
	else
	{
		it = m_SPItemMap[type].find(typeId);

		if (it != m_SPItemMap[type].end())
			pItem = &(*it).second;
	}

	return pItem;
}

void CItemManager::MakeItemBuffMap()
{
	sItemBuffHelper helper;
	BuffMap_Itr it;

	m_ItemBuff.clear();

	m_ItemBuff.insert(std::make_pair(0x1a0000b3, helper));
	it = m_ItemBuff.find(0x1a0000b3);
	if (it != m_ItemBuff.end())
	{
		(*it).second.Accum = 1;
		(*it).second.BuffType = SPECIAL_EXP_REAE_UP_TIME;
		(*it).second.Duration = 120;
		(*it).second.Value = 5;
		std::set<unsigned long>& exclusiveSet = (*it).second.ExclusiveSet;
		exclusiveSet.insert(0x1a0000b4);
	}

	m_ItemBuff.insert(std::make_pair(0x1a0000b4, helper));
	it = m_ItemBuff.find(0x1a0000b4);
	if (it != m_ItemBuff.end())
	{
		(*it).second.Accum = 1;
		(*it).second.BuffType = SPECIAL_EXP_REAE_UP_TIME;
		(*it).second.Duration = 120;
		(*it).second.Value = 50;
		std::set<unsigned long>& exclusiveSet = (*it).second.ExclusiveSet;
		exclusiveSet.insert(0x1a0000b3);
	}
}

bool CItemManager::IsItemBuff(unsigned long dwTID)
{
	BuffMap_Itr it = m_ItemBuff.find(dwTID);
	if (it != m_ItemBuff.end())
		return true;
	return false;
}

bool CItemManager::IsCompatibleBuff(unsigned long dwBuffTID,
	unsigned long dwWithTID)
{
	BuffMap_Itr it = m_ItemBuff.find(dwBuffTID);
	if (it == m_ItemBuff.end())
	{
		return false;
	}

	const std::set<unsigned long>& exclusiveSet = (*it).second.ExclusiveSet;
	std::set<unsigned long>::const_iterator check =
		exclusiveSet.find(dwWithTID);
	if (check == exclusiveSet.end())
		return true;

	return false;
}

bool CItemManager::GetBuffProperty(unsigned long dwTID,
	specialcardavility& eType, int& Value, int& Duration, int& Accum)
{
	BuffMap_Itr it = m_ItemBuff.find(dwTID);
	if (it == m_ItemBuff.end())
		return false;

	eType = (*it).second.BuffType;
	Value = (*it).second.Value;
	Duration = (*it).second.Duration;
	Accum = (*it).second.Accum;
	return true;
}

namespace S5
{
	bool ISCutinItem(unsigned long _typeId)
	{
		if ((_typeId & 0xfc000000) == 0x38000000 &&
			(_typeId & 0x3c00000) == 0x1400000)
			return true;

		return false;
	}
}

const char* CItemManager::GetIconName(unsigned long typeId)
{
	if ((typeId & 0xfc000000) != 0x2c000000)
	{
		IFF_ITEM_COMMON* pItem = FindCommonItem(typeId);
		if (pItem)
			return pItem->Icon;
	}
	else
	{
		IFF_STRUCT::sMatch* pMatch = FindMatch(typeId);
		if (pMatch)
			return pMatch->Icon[0];
	}

	return "";
}

bool CItemManager::IsUnlimitAztec(unsigned long typeId)
{
	if ((typeId & 0xfc000000) == 0x14000000)
	{
		IFF_STRUCT::sBall* pBall = FindBall(typeId);
		if (pBall && pBall->COM[0] == 0)
			return true;
	}

	return false;
}

bool CItemManager::IsBasicAztecFamily(unsigned long tid)
{
	if ((tid & 0xfc000000) == 0x14000000)
	{
		if (tid == 0x14000000)
			return true;

		IFF_STRUCT::sBall* pBall = FindBall(tid);
		if (pBall && pBall->c.Price == 0 && pBall->COM[0] == 0)
			return true;
	}

	return false;
}

const char* CItemManager::ChangePartsPosmask(unsigned long dwTypeID,
	unsigned long dwPosMask)
{
	IFF_STRUCT::sPart* pPart = FindPart(dwTypeID);
	if (pPart)
	{
		m_cs.lock();
		pPart->PosMask = dwPosMask;
		m_cs.unlock();
		return "The posmask is changed";
	}

	return "No exist";
}

const char* CItemManager::ChangePartsPetName(unsigned long dwTypeID,
	const char* pPetName)
{
	IFF_STRUCT::sPart* pPart = FindPart(dwTypeID);
	if (pPart)
	{
		m_cs.lock();
		char OldPetName[256];
		strcpy(OldPetName, pPart->Data);
		strcpy(pPart->Data, pPetName);
		m_cs.unlock();
		return "The petname is changed";
	}

	return "No exist";
}

const char* CItemManager::ChangePartsTexName(unsigned long dwTypeID,
	unsigned long dwTexType, unsigned long dwTexNum, const char* pTexName)
{
	if (dwTexType != 0 && dwTexType != 1)
		return "Invalid texture type.";

	if (dwTexNum <= 2)
	{
		IFF_STRUCT::sPart* pPart = FindPart(dwTypeID);
		if (pPart)
		{
			m_cs.lock();
			if (dwTexType == 0)
				strcpy(pPart->Tex[dwTexNum], pTexName);
			else if (dwTexType == 1)
				strcpy(pPart->OrgTex[dwTexNum], pTexName);
			m_cs.unlock();

			return "The texname is changed";
		}

		return "No exist";
	}

	return "Invalid texture.";
}

_private::sRandomBox* CItemManager::findRandomBox(unsigned long boxTypeId)
{
	_private::sRandomBox* pBox = NULL;
	std::map<unsigned int, _private::sRandomBox>::iterator it;

	if (m_IsReloading)
	{
		m_cs.lock();

		it = m_RandomBoxMap.find(boxTypeId);

		if (it != m_RandomBoxMap.end())
			pBox = &it->second;
		m_cs.unlock();
	}
	else
	{
		it = m_RandomBoxMap.find(boxTypeId);

		if (it != m_RandomBoxMap.end())
			return &it->second;
	}

	return pBox;
}

IFF_STRUCT::sRandomBox* CItemManager::GetRandomBoxInfo(unsigned long boxTypeId,
	unsigned long typeId, unsigned long itemIndex)
{
	_private::sRandomBox* pBox = findRandomBox(boxTypeId);
	if (pBox == NULL)
		return NULL;

	std::map<unsigned long, _private::sRandomBoxItem>::iterator it =
		pBox->items.find(itemIndex);
	if (it == pBox->items.end())
		return NULL;

	return &it->second.box;
}

bool CItemManager::GetEnumRandomBoxStuff(unsigned long boxTypeId,
	std::vector<IFF_STRUCT::sRandomBox>& out)
{
	_private::sRandomBox* pBox = findRandomBox(boxTypeId);
	if (pBox == NULL)
		return false;

	std::vector<_private::sRandomBoxStuff>::iterator it = pBox->stuffs.begin();
	std::vector<_private::sRandomBoxStuff>::iterator itEnd = pBox->stuffs.end();

	for (; it != itEnd; ++it)
		out.push_back((*it).box);

	return true;
}

bool CItemManager::GetEnumRandomBoxItem(unsigned long boxTypeId,
	std::vector<std::pair<unsigned long, IFF_STRUCT::sRandomBox> >& out)
{
	_private::sRandomBox* pBox = findRandomBox(boxTypeId);
	if (pBox == NULL)
		return false;

	std::map<unsigned long, _private::sRandomBoxItem>::iterator it =
		pBox->items.begin();
	std::map<unsigned long, _private::sRandomBoxItem>::iterator itEnd =
		pBox->items.end();

	for (; it != itEnd; ++it)
		out.push_back(std::make_pair(it->first, it->second.box));

	return true;
}

bool CItemManager::GetEnumRandomBoxCheckItem(unsigned long boxTypeId,
	unsigned long typeId, unsigned long itemIndex,
	std::vector<IFF_STRUCT::sRandomBox>& out)
{
	_private::sRandomBox* pBox = findRandomBox(boxTypeId);
	if (pBox == NULL)
		return false;

	std::map<unsigned long, _private::sRandomBoxItem>::iterator it =
		pBox->items.find(itemIndex);
	if (it == pBox->items.end())
		return false;

	if (typeId != it->second.box.typeId)
		return false;

	std::vector<IFF_STRUCT::sRandomBox>::const_iterator itCheck =
		it->second.checkItems.begin();
	std::vector<IFF_STRUCT::sRandomBox>::const_iterator itCheckEnd =
		it->second.checkItems.end();

	for (; itCheck != itCheckEnd; ++itCheck)
		out.push_back(*itCheck);

	return true;
}

bool CItemManager::IsNonVisibleItem(unsigned long typeId)
{
	if (IsSubScriptionCoupon(typeId))
		return true;

	std::map<unsigned int, IFF_STRUCT::sNonVisibleItem>::const_iterator it =
		m_NonVisibleItemMap.find(typeId);
	if (it == m_NonVisibleItemMap.end())
		return false;

	if (it->second.type & 1)
	{
		return true;
	}

	return false;
}

bool CItemManager::IsSubScriptionCoupon(unsigned long typeId)
{
	std::map<unsigned int, IFF_STRUCT::sSubscriptionItem>::const_iterator it =
		m_SubscriptionItemMap.find(typeId);
	if (it == m_SubscriptionItemMap.end())
		return false;

	if (0 != (it->second.type & 1))
		return true;

	return false;
}
