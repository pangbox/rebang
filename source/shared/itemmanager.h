#pragma once

#include <string.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>

#include "classdefine.h"
#include "lock.hpp"

enum eLvlType
{
	LVL_POWER,
	LVL_CONTROL,
	LVL_ACCURACY,
	LVL_SPIN,
	LVL_CURVE,
	LVL_NUM_MAX
};

enum PANGYA_ITEM_GROUP
{
	PIG_CHAR = 1,
	PIG_PART,
	PIG_CLUB,
	PIG_CLUBSET,
	PIG_BALL,
	PIG_ITEM,
	PIG_CADDIE,
	PIG_CADITEM,
	PIG_SETITEM,
	PIG_COURSE,
	PIG_MATCH,
	PIG_TITLE,
	PIG_ENCHANT,
	PIG_SKIN,
	PIG_HAIR,
	PIG_MASCOT,
	PIG_CHILDITEM,
	PIG_FURNITURE,
	PIG_OFFLINESHOP,
	PIG_AUXPART = 28,
	PIG_QUESTDROP,
	PIG_QUEST,
	PIG_CARD,
	PIG_MAX
};

enum specialcardavility
{
	SPECIAL_EXP_ADD,
	SPECIAL_PANG_RATE_UP_TIME,
	SPECIAL_EXP_REAE_UP_TIME,
	SPECIAL_PANG_ADD,
	SPECIAL_POWER_UP_TIME,
	SPECIAL_CONTROL_UP_TIME,
	SPECIAL_ACCCURACY_UP_TIME,
	SPECIAL_SPIN_UP_TIME,
	SPECIAL_CURVE_UP_TIME,
	SPECIAL_START_COMBOGAUGE_UP_TIME,
	SPECIAL_SLOT_UP_TIME,
	SPECIAL_PANGYAZONE_UP,
	SPECIAL_CLEARBONUS_UP_SEPIAWIND,
	SPECIAL_CLEARBONUS_UP_WINDHILL,
	SPECIAL_CLEARBONUS_UP_PINKWIND,
	SPECIAL_CLEARBONUS_UP_BLUEMOON,
	SPECIAL_RANDOMPANG_ADD,
	SPECIAL_TREASUREPOINT_UP,
	SPECIAL_RAINFALL_PROBABILITY_UP,
	SPECIAL_AVILITY_MAX
};

enum COUNTING_TYPE;
enum eSPAVILITYTYPE;

struct status_t
{
	unsigned char level;
	unsigned char penalty;
};

typedef std::map<unsigned int, IFF_STRUCT::sChar>::iterator CharIterator;
typedef std::map<unsigned int, IFF_STRUCT::sPart>::iterator PartIterator;
typedef std::map<unsigned int, IFF_STRUCT::sClub>::iterator ClubIterator;
typedef std::map<unsigned int, IFF_STRUCT::sClubSet>::iterator ClubSetIterator;
typedef std::map<unsigned int, IFF_STRUCT::sBall>::iterator BallIterator;
typedef std::map<unsigned int, IFF_STRUCT::sItem>::iterator ItemIterator;
typedef std::map<unsigned int, IFF_STRUCT::sCaddie>::iterator CaddieIterator;
typedef std::map<unsigned int, IFF_STRUCT::sCadItem>::iterator CadItemIterator;
typedef std::map<unsigned int, IFF_STRUCT::sSetItem>::iterator SetItemIterator;
typedef std::map<unsigned int, IFF_STRUCT::sCourse>::iterator CourseIterator;
typedef std::map<unsigned int, IFF_STRUCT::sMatch>::iterator MatchIterator;
typedef std::map<unsigned int, IFF_STRUCT::sTitle>::iterator TitleIterator;
typedef std::map<unsigned int, IFF_STRUCT::sEnchant>::iterator EnchantIterator;
typedef std::map<unsigned int, IFF_STRUCT::sDesc>::iterator DescIterator;
typedef std::map<unsigned int, IFF_STRUCT::sSkin>::iterator SkinIterator;
typedef std::map<unsigned int, IFF_STRUCT::sHairStyle>::iterator
	HairStyleIterator;
typedef std::map<unsigned int, IFF_STRUCT::sChildItem>::iterator
	ChildItemIterator;
typedef std::map<unsigned int, IFF_STRUCT::sMascot>::iterator MascotIterator;
typedef std::map<unsigned int, IFF_STRUCT::sAuxPart>::iterator AuxPartIterator;
typedef std::map<unsigned int, IFF_STRUCT::sQuestDrop>::iterator
	QuestDropIterator;
typedef std::map<unsigned int, IFF_STRUCT::sQuest>::iterator QuestIterator;
typedef std::map<unsigned int, IFF_STRUCT::sFurniture>::iterator
	FurnitureIterator;
typedef std::map<unsigned int, IFF_STRUCT::sCard>::iterator CardIterator;
typedef std::map<unsigned int, IFF_STRUCT::sOfflineShop>::iterator
	OfflineShopIterator;
typedef std::list<IFF_STRUCT::sCadieMagicBox>::iterator CadieMagicBoxIterator;
typedef std::list<IFF_STRUCT::sTikiPointTable>::iterator TikiPointTableIterator;
typedef std::list<IFF_STRUCT::sTikiSpecialRecipe>::iterator
	TikiSpecialRecipeIterator;
typedef std::map<unsigned int, IFF_STRUCT::S5::sCutinInformation>::iterator
	CutinInformationIterator;
typedef std::map<unsigned int, IFF_STRUCT::sSpecialPrizeItem>::iterator
	SpecialPrizeItemIterator;

namespace _private
{
	struct sRandomBoxItem
	{
		IFF_STRUCT::sRandomBox box;
		std::vector<IFF_STRUCT::sRandomBox> checkItems;

		sRandomBoxItem()
		{
			memset(this, 0, sizeof(IFF_STRUCT::sRandomBox));
			checkItems.clear();
		}

		~sRandomBoxItem() { checkItems.clear(); }
	};

	struct sRandomBoxStuff
	{
		IFF_STRUCT::sRandomBox box;

		sRandomBoxStuff() { memset(this, 0, sizeof(sRandomBoxStuff)); }
	};

	struct sRandomBox
	{
		IFF_STRUCT::sRandomBox box;
		std::map<unsigned long, sRandomBoxItem> items;
		std::vector<sRandomBoxStuff> stuffs;

		sRandomBox()
		{
			memset(this, 0, sizeof(IFF_STRUCT::sRandomBox));
			items.clear();
			stuffs.clear();
		}

		~sRandomBox()
		{
			items.clear();
			stuffs.clear();
		}
	};
}

typedef std::map<unsigned int, _private::sRandomBox>::iterator
	RandomBoxIterator;
typedef std::map<unsigned int, IFF_STRUCT::sNonVisibleItem>::iterator
	NonVisibleItemIterator;
typedef std::map<unsigned int, IFF_STRUCT::sSubscriptionItem>::iterator
	SubscriptionItemIterator;

class CItemManager
{
public:
	CItemManager();
	virtual ~CItemManager();

	void Reset();
	bool Load();
	void Reload();

	_client::CCriticalSection m_cs;
	bool m_IsReloading;
	std::map<unsigned int, IFF_STRUCT::sChar> m_CharMap;
	std::map<unsigned int, IFF_STRUCT::sPart> m_PartMap;
	std::map<unsigned int, IFF_STRUCT::sClub> m_ClubMap;
	std::map<unsigned int, IFF_STRUCT::sClubSet> m_ClubSetMap;
	std::map<unsigned int, IFF_STRUCT::sBall> m_BallMap;
	std::map<unsigned int, IFF_STRUCT::sItem> m_ItemMap;
	std::map<unsigned int, IFF_STRUCT::sCaddie> m_CaddieMap;
	std::map<unsigned int, IFF_STRUCT::sCadItem> m_CadItemMap;
	std::map<unsigned int, IFF_STRUCT::sSetItem> m_SetItemMap;
	std::map<unsigned int, IFF_STRUCT::sCourse> m_CourseMap;
	std::map<unsigned int, IFF_STRUCT::sMatch> m_MatchMap;
	std::map<unsigned int, IFF_STRUCT::sTitle> m_TitleMap;
	std::map<unsigned int, IFF_STRUCT::sEnchant> m_EnchantMap;
	std::map<unsigned int, IFF_STRUCT::sDesc> m_DescMap;
	std::map<unsigned int, IFF_STRUCT::sSkin> m_SkinMap;
	std::map<unsigned int, IFF_STRUCT::sHairStyle> m_HairStyleMap;
	std::map<unsigned int, IFF_STRUCT::sChildItem> m_ChildItemMap;
	std::map<unsigned int, IFF_STRUCT::sMascot> m_MascotMap;
	std::map<unsigned int, IFF_STRUCT::sAuxPart> m_AuxPartMap;
	std::map<unsigned int, IFF_STRUCT::sQuestDrop> m_QuestDropMap;
	std::map<unsigned int, IFF_STRUCT::sQuest> m_QuestMap;
	std::map<unsigned int, IFF_STRUCT::sFurniture> m_FurnitureMap;
	std::map<unsigned int, IFF_STRUCT::sCard> m_CardMap;
	std::map<unsigned int, IFF_STRUCT::sOfflineShop> m_OfflineShopMap;
	std::multimap<unsigned int, IFF_STRUCT::sRandomRecycle> m_RandomRecycleMaps;
	std::list<IFF_STRUCT::sCadieMagicBox> m_CadieMagicBox;
	std::list<IFF_STRUCT::sFurnitureAbility> m_FurnitureAbility;
	std::list<IFF_STRUCT::sTikiPointTable> m_TikiPointTable;
	std::list<IFF_STRUCT::sTikiSpecialRecipe> m_TikiSpecialRecipe;
	std::map<unsigned int, _private::sRandomBox> m_RandomBoxMap;
	std::map<unsigned int, IFF_STRUCT::sNonVisibleItem> m_NonVisibleItemMap;
	std::map<unsigned int, IFF_STRUCT::sSubscriptionItem> m_SubscriptionItemMap;
	std::map<unsigned int, IFF_STRUCT::S5::sCutinInformation>
		m_CutinInformationMap;
	std::map<unsigned int, IFF_STRUCT::sSpecialPrizeItem> m_SPItemMap[5];

private:
	void MakeItemBuffMap();

	struct sItemBuffHelper
	{
		specialcardavility BuffType;
		int Value;
		int Duration;
		int Accum;
		std::set<unsigned long> ExclusiveSet;
	};
	typedef std::map<unsigned long, sItemBuffHelper> BuffMap;
	typedef BuffMap::iterator BuffMap_Itr;

	BuffMap m_ItemBuff;

	void MakeCharMap(char* Buf, int len);
	void MakePartMap(char* Buf, int len);
	void MakeClubMap(char* Buf, int len);
	void MakeClubSetMap(char* Buf, int len);
	void MakeBallMap(char* Buf, int len);
	void MakeItemMap(char* Buf, int len);
	void MakeFurnitureMap(char* Buf, int len);
	void MakeOfflineMap(char* Buf, int len);
	void MakeCaddieMap(char* Buf, int len);
	void MakeCadItemMap(char* Buf, int len);
	void MakeSetItemMap(char* Buf, int len);
	void MakeCourseMap(char* Buf, int len);
	void MakeMatchMap(char* Buf, int len);
	void MakeTitleMap(char* Buf, int len);
	void MakeEnchantMap(char* Buf, int len);
	void MakeSkinMap(char* Buf, int len);
	void MakeHairStyleMap(char* Buf, int len);
	void MakeChildItemMap(char* Buf, int len);
	void MakeMascotMap(char* Buf, int len);
	void MakeDescMap(char* Buf, int len);
	void MakeAuxPartMap(char* Buf, int len);
	void MakeQuestDropMap(char* Buf, int len);
	void MakeQuestMap(char* Buf, int len);
	void MakeCardMap(char* Buf, int len);
	void MakeCadieMagicBoxMap(char* Buf, int len);
	void MakeRandomRecycleMap(char* Buf, int len);
	void MakeFurnitureAbilityList(char* Buf, int len);
	void MakeTikiRecipeMap(char* Buf, int len);
	void MakeTikiPointTableMap(char* Buf, int len);
	void MakeTikiSpecialTableMap(char* Buf, int len);
	void MakeRandomBoxMap(char* Buf, int len);
	void MakeNonVisibleItemMap(char* Buf, int len);
	void MakeSubscriptionItemMap(char* Buf, int len);
	void MakeCutinInforMationTableMap(char* Buf, int len);
	void MakeSpecialPrizeItemMap(char* Buf, int len);
	_private::sRandomBox* findRandomBox(unsigned long boxTypeId);
	void CheckInvalidItemName();

public:
	IFF_STRUCT::sChar* FindChar(unsigned long tid);
	IFF_STRUCT::sPart* FindPart(unsigned long tid);
	IFF_STRUCT::sClub* FindClub(unsigned long tid);
	IFF_STRUCT::sClubSet* FindClubSet(unsigned long tid);
	IFF_STRUCT::sBall* FindBall(unsigned long tid);
	IFF_STRUCT::sItem* FindItem(unsigned long tid);
	IFF_STRUCT::sCaddie* FindCaddie(unsigned long tid);
	IFF_STRUCT::sCadItem* FindCadItem(unsigned long tid);
	IFF_STRUCT::sSetItem* FindSetItem(unsigned long tid);
	IFF_STRUCT::sSetItem* FindSetItemAtConstruct(unsigned long tid);
	IFF_STRUCT::sCourse* FindCourse(unsigned long tid);
	IFF_STRUCT::sMatch* FindMatch(unsigned long tid);
	IFF_STRUCT::sTitle* FindTitle(unsigned long tid);
	IFF_STRUCT::sEnchant* FindEnchant(unsigned long tid);
	IFF_STRUCT::sSkin* FindSkin(unsigned long tid);
	IFF_STRUCT::sRandomBox* FindRandomBox(unsigned long tid);
	IFF_STRUCT::sHairStyle* FindHairStyle(unsigned long tid);
	IFF_STRUCT::sHairStyle* FindHairStyle(unsigned char charID,
		unsigned char hairID);
	IFF_STRUCT::sChildItem* FindChildItem(unsigned long tid, int iIndex);
	int FindChildItemNumber(unsigned long tid);
	IFF_STRUCT::sMascot* FindMascot(unsigned long tid);
	IFF_STRUCT::sAuxPart* FindAuxPart(unsigned long tid);
	IFF_STRUCT::sQuestDrop* FindQuestDrop(unsigned long tid);
	IFF_STRUCT::sQuest* FindQuest(unsigned long tid);
	IFF_ITEM_COMMON* FindCommonItem(unsigned long tid);
	IFF_STRUCT::sCard* FindCard(unsigned long tid);
	IFF_STRUCT::sFurniture* FindFurniture(unsigned long tid);
	IFF_STRUCT::sOfflineShop* FindOfflineShop(unsigned long tid);
	IFF_STRUCT::sDesc* FindDesc(unsigned long tid);
	IFF_STRUCT::S5::sCutinInformation* FindCutinInformation(unsigned long tid);
	bool GetSetItemElem(IFF_STRUCT::sSetItem* pSetItem, int index,
		unsigned long& typeId, unsigned long& count, unsigned long& type);

	bool GetRecycleRandomMixOutItems(unsigned long randSeqNum,
		std::list<const IFF_STRUCT::sRandomRecycle*>& ltMixOutList);
	unsigned int GetItemPrice(unsigned long tid);
	int CheckStockType(unsigned long typeId, unsigned int stock);
	unsigned int GetItemSalePrice(unsigned long in_dwTid, int in_iCount,
		unsigned long in_dwDayCount);
	unsigned int GetChildItemPrice(unsigned long tid, int iIndex);
	bool IsCashItem(unsigned long tid);
	bool IsCashChildItem(unsigned long tid, int iIndex);
	bool IsTimeLimit(unsigned long tid);
	const char* GetItemName(unsigned long tid);
	bool HasSalePeriod(unsigned long tid, _SYSTEMTIME** pSaleS,
		_SYSTEMTIME** pSaleE);
	bool IsInSalePeriod(unsigned long tid, _SYSTEMTIME& sysTime);
	bool IsBetween(_SYSTEMTIME* pStart, _SYSTEMTIME* pEnd,
		_SYSTEMTIME& sysTime);
	int CompareSystemTime(_SYSTEMTIME& tm0, _SYSTEMTIME& tm1);
	int GetCouponKind(unsigned long dwTid);
	bool IsDisableDuplicateItem(unsigned long dwTid);
	unsigned int IsValidBuyItem(unsigned long in_dwItemType);
	bool IsValidBuyItemCount(unsigned long in_dwItemType, int in_iItemCount);
	bool IsDisplayItemNumber(unsigned long dwTid);
	unsigned long GetIndexAbilityItem(unsigned long dwTid);
	unsigned long GetMascotBonusPang(unsigned long dwTid, bool bBestShot);
	bool GetDefCombo(unsigned long tidChar, unsigned long* atidDefCombo);
	int GetNumItems(PANGYA_ITEM_GROUP pig);
	int GetCourseNum();
	bool isTrade(unsigned long typeId);
	bool IsCanOverlapped(unsigned long dwTID, unsigned char btCheckType,
		unsigned char exceptGroup);
	bool IsPartAttachCard(unsigned long dwCardTid, unsigned long dwPartTid,
		int iSlotNum);
	COUNTING_TYPE GetItemCountingType(unsigned long in_Tid,
		unsigned char in_itemType);
	IFF_STRUCT::sSpecialPrizeItem* FindSPItem(unsigned long typeId,
		eSPAVILITYTYPE type);
	bool GetCardSlotNum(unsigned long in_TID,
		unsigned short& out_LimitCharacterSlot,
		unsigned short& out_LimitCaddieSlot);
	bool IsEquipCard(unsigned long in_CardTID);
	bool IsItemBuff(unsigned long dwTID);
	bool IsCompatibleBuff(unsigned long dwBuffTID, unsigned long dwWithTID);
	bool GetBuffProperty(unsigned long dwTID, specialcardavility& eType,
		int& Value, int& Duration, int& Accum);
	unsigned char GetItemLevel(eLvlType eType, const unsigned long* patidParts,
		const unsigned long* patidAuxParts);
	unsigned char GetCharCapacity(eLvlType eType, unsigned long tidChar,
		const unsigned long* patidParts, const unsigned long* patidAuxParts,
		unsigned char level);
	unsigned char GetCharLevel(eLvlType eType, unsigned long tidChar,
		const char* pCharPCL);
	unsigned char GetCapacity(eLvlType type, unsigned long charTypeId,
		const char* pStat, const unsigned long* pParts,
		unsigned long clubTypeId, const short* pClubStat,
		unsigned long caddieTypeId, unsigned char upgrade);
	unsigned char GetLevel(eLvlType type, unsigned long charTypeId,
		const char* pStat, const unsigned long* pParts,
		const unsigned long* pAuxParts, unsigned long clubTypeId,
		const short* pClubStat, unsigned long caddieTypeId,
		unsigned char upgrade);
	status_t GetLevelWithPenalty(eLvlType type, unsigned long charTypeId,
		const char* pStat, const unsigned long* pParts,
		const unsigned long* pAuxParts, unsigned long clubTypeId,
		const short* pClubStat, unsigned long caddieTypeId,
		unsigned char upgrade);
	const char* GetIconName(unsigned long typeId);
	bool IsUnlimitAztec(unsigned long typeId);
	bool IsBasicAztecFamily(unsigned long tid);
	const char* ChangePartsPosmask(unsigned long dwTypeID,
		unsigned long dwPosMask);
	const char* ChangePartsTexName(unsigned long dwTypeID,
		unsigned long dwTexType, unsigned long dwTexNum, const char* pTexName);
	const char* ChangePartsPetName(unsigned long dwTypeID,
		const char* pPetName);
	IFF_STRUCT::sRandomBox* GetRandomBoxInfo(unsigned long boxTypeId,
		unsigned long typeId, unsigned long itemIndex);
	bool GetEnumRandomBoxStuff(unsigned long boxTypeId,
		std::vector<IFF_STRUCT::sRandomBox>& out);
	bool GetEnumRandomBoxItem(unsigned long boxTypeId,
		std::vector<std::pair<unsigned long, IFF_STRUCT::sRandomBox> >& out);
	bool GetEnumRandomBoxCheckItem(unsigned long boxTypeId,
		unsigned long typeId, unsigned long itemIndex,
		std::vector<IFF_STRUCT::sRandomBox>& out);
	bool IsNonVisibleItem(unsigned long typeId);
	bool IsSubScriptionCoupon(unsigned long typeId);
};

int CalcPowerPenalty(int upgrade);
