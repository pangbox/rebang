#include "ghostmanager.h"
#include "ghostdocument.h"
#include "ghosthandler.h"
#include "ghostloader.h"
#include "netresourcemanager.h"
#include "type_convert.h"
#include <stdio.h>

CGhostDocument* CreateDocument()
{
	return new CGhostDocument;
}

CGhostManager::CGhostManager()
	: m_pDocument(NULL), m_pLatestGameBrief(NULL)
{
}

CGhostManager::~CGhostManager()
{
	if (m_pDocument)
	{
		delete m_pDocument;
		m_pDocument = NULL;
	}
	if (m_pLatestGameBrief)
	{
		delete m_pLatestGameBrief;
		m_pLatestGameBrief = NULL;
	}
}

CGhostDocument* CGhostManager::GetDocument()
{
	if (!m_pDocument)
		m_pDocument = CreateDocument();
	return m_pDocument;
}

void CGhostManager::ReleaseDocument()
{
	if (!m_pDocument)
		return;
	delete m_pDocument;
	m_pDocument = NULL;
}

void CGhostManager::SaveDocument(const std::string& fileName)
{
	if (!m_pDocument)
		return;
	DWORD start = GetTickCount();
	m_pDocument->Export(fileName);
	ReleaseDocument();
	sResource resource;
	resource.filename = fileName;
	resource.type = RESOURCE_GHOST;
	sprintf(resource.arg, "%d", MyUID());
	NetResManager()->Upload(resource,
		"http://qa.contents.pangya.gametree.co.kr:50006/GHOST/upload.asp");
}

void CGhostManager::LoadDocument(const std::string& fileName)
{
	m_latestFileName = fileName;
	if (m_pLatestGameBrief)
	{
		delete m_pLatestGameBrief;
		m_pLatestGameBrief = NULL;
	}
	m_pLatestGameBrief = new GHOST_GAME_BRIEF;
	unsigned long owner =
		GetGhostGameBriefFromFileName(m_pLatestGameBrief, fileName.c_str());
	sResource resource;
	resource.filename = fileName;
	resource.type = RESOURCE_GHOST;
	sprintf(resource.arg, "%d", owner);
	NetResManager()->Download(resource,
		"http://qa.contents.pangya.gametree.co.kr:50006/GHOST/GHOST/");
}

void CGhostManager::LoadDocumentFromLocalFile(const std::string& fileName)
{
	m_latestFileName = fileName;
	if (m_pLatestGameBrief)
	{
		delete m_pLatestGameBrief;
		m_pLatestGameBrief = NULL;
	}
	m_pLatestGameBrief = new GHOST_GAME_BRIEF;
	GetGhostGameBriefFromFileName(m_pLatestGameBrief, fileName.c_str());
	CGhostHandler::Instance()->LoadingComplete(
		CGhostLoader().LoadFromFile(fileName));
}

void conv_ghostdata_to_userinfo(const GHOST_USER_DATA* from, sUserInfo* to)
{
	to->charInfo.tid = from->charTypeId;
	to->charInfo.hairClr = from->hairColor;
	memcpy(to->charInfo.tidParts, from->partTypeId, 96);
	memcpy(to->charInfo.tidAuxParts, from->auxPartTypeId, 20);
	memcpy(to->charInfo.PCL, from->charPcl, 5);
	to->clubInfo.tid = from->clubTypeId;
	memcpy(to->clubInfo.PCL, from->clubPcl, 10);
	to->userEquip.tidBall = from->ballTypeId;
	to->caddieInfo.tid = from->caddieTypeId;
	to->caddieInfo.tidPart = from->caddiePartsTypeId;
	to->caddieInfo.Level = from->caddieLevel;
	to->mascotInfo.tid = from->mascotTypeId;
	py_strcpy(to->mascotInfo.szMsg, from->mascotMsg);
	memcpy(to->userEquip.tidItemSlot, from->itemSlot, 40);
	memcpy(to->charInfo.UccIndexList, from->charReserved, 216);
	py_strcpy(to->info.sNick, from->nickname);
	py_strcpy(to->info.sID, from->id);
	py_strcpy(to->info.sGuild, from->guildName);
	py_strcpy(to->info.szEmblemName, from->guildMark);
	to->info.dwIdentity = from->capability;
	to->info.gender = from->female;
	to->info.devilWings = from->devilWing;
	to->stat.Level = from->level;
	to->info.angelicWings = from->angelWing;
	to->info.angelicWingsEffect = from->angelEffect;
	to->info.dwUID = from->uid;
}

void conv_userinfo_to_ghostdata(const sUserInfo* from, GHOST_USER_DATA* to)
{
	to->charTypeId = from->charInfo.tid;
	to->hairColor = from->charInfo.hairClr;
	memcpy(to->partTypeId, from->charInfo.tidParts, 96);
	memcpy(to->auxPartTypeId, from->charInfo.tidAuxParts, 20);
	memcpy(to->charPcl, from->charInfo.PCL, 5);
	to->clubTypeId = from->clubInfo.tid;
	memcpy(to->clubPcl, from->clubInfo.PCL, 10);
	to->ballTypeId = from->userEquip.tidBall;
	to->caddieTypeId = from->caddieInfo.tid;
	to->caddiePartsTypeId = from->caddieInfo.tidPart;
	to->caddieLevel = from->caddieInfo.Level;
	to->mascotTypeId = from->mascotInfo.tid;
	py_strcpy(to->mascotMsg, from->mascotInfo.szMsg);
	memcpy(to->itemSlot, from->userEquip.tidItemSlot, 40);
	memcpy(to->charReserved, from->charInfo.UccIndexList, 216);
	py_strcpy(to->nickname, from->info.sNick);
	py_strcpy(to->id, from->info.sID);
	py_strcpy(to->guildName, from->info.sGuild);
	py_strcpy(to->guildMark, from->info.szEmblemName);
	to->capability = from->info.dwIdentity;
	to->female = from->info.gender;
	to->angelWing = from->info.angelicWings;
	to->angelEffect = from->info.angelicWingsEffect;
	to->devilWing = from->info.devilWings;
	to->level = from->stat.Level;
	to->uid = from->info.dwUID;
}

void conv_ghostdata_to_slotinfo(const GHOST_USER_DATA* from, sSlotInfo* to)
{
	py_strcpy(to->sNick, from->nickname);
	py_strcpy(to->sGuild, from->guildName);
	py_strcpy(to->szEmblemName, from->guildMark);
	to->level = from->level;
	to->dwIdentity = from->capability;
	to->gender = from->female;
	to->angelicWings = from->angelWing;
	to->angelicWingsEffect = from->angelEffect;
	to->bReady = 1;
	to->connectionRank = 2;
	to->dwGuid = -2;
	to->tidChar = from->charTypeId;
	to->tidMascot = from->mascotTypeId;
}

void conv_legacycard_to_ghostcard(const sSCardAvilityPeriodInfo* from,
	GHOST_CARD_DATA* to)
{
	to->typeId = from->tid;
	to->itemId = from->uid;
	to->partsTypeId = from->partsTid;
	to->partsItemId = from->partsUid;
	to->ability = from->Avility;
	to->abilityValue = from->AvilityValue;
	to->slotNum = from->slotNum;
	to->cardType = from->cardType;
}

void conv_ghostcard_to_legacycard(const GHOST_CARD_DATA* from,
	sSCardAvilityPeriodInfo* to)
{
	to->Avility = from->ability;
	to->AvilityValue = from->abilityValue;
	to->cardType = from->cardType;
	to->partsTid = from->partsTypeId;
	to->partsUid = from->partsItemId;
	to->slotNum = from->slotNum;
	to->tid = from->typeId;
	to->uid = from->itemId;
	to->valid = 1;
}

void conv_ghostshot_to_shotprotocol(const sShotProtocol* from,
	GHOST_SHOT_PROTOCOL* to)
{
	to->maxClick = from->maxClick;
	to->minClick = from->minClick;
	to->curve = from->curve;
	to->spin = from->spin;
	to->accuracy = from->accuracy;
	to->special = from->speical;
	to->frame = from->frame;
	to->shotAngle = from->angle;
	to->shotTime = from->shotTime;
	to->center = from->center;
	to->clubIndex = from->clubIndex;
	to->randomPower = from->randomPower;
	to->randomAngle = from->randomAngle;
}

void conv_shotprotocol_to_ghostshot(const GHOST_SHOT_PROTOCOL* from,
	sShotProtocol* to)
{
	to->maxClick = from->maxClick;
	to->minClick = from->minClick;
	to->curve = from->curve;
	to->spin = from->spin;
	to->accuracy = from->accuracy;
	to->speical = from->special;
	to->frame = from->frame;
	to->angle = from->shotAngle;
	to->shotTime = from->shotTime;
	to->center = from->center;
	to->clubIndex = from->clubIndex;
	to->randomPower = from->randomPower;
	to->randomAngle = from->randomAngle;
}

static bool registered_conv_userinfo_to_ghostdata =
	type_convert::insert_converter<sUserInfo, GHOST_USER_DATA>(
		(converter_func)conv_userinfo_to_ghostdata);

static bool registered_conv_ghostdata_to_userinfo =
	type_convert::insert_converter<GHOST_USER_DATA, sUserInfo>(
		(converter_func)conv_ghostdata_to_userinfo);

static bool registered_conv_ghostdata_to_slotinfo =
	type_convert::insert_converter<GHOST_USER_DATA, sSlotInfo>(
		(converter_func)conv_ghostdata_to_slotinfo);

static bool registered_conv_ghostcard_to_legacycard =
	type_convert::insert_converter<GHOST_CARD_DATA, sSCardAvilityPeriodInfo>(
		(converter_func)conv_ghostcard_to_legacycard);

static bool registered_conv_legacycard_to_ghostcard =
	type_convert::insert_converter<sSCardAvilityPeriodInfo, GHOST_CARD_DATA>(
		(converter_func)conv_legacycard_to_ghostcard);

static bool registered_conv_ghostshot_to_shotprotocol =
	type_convert::insert_converter<sShotProtocol, GHOST_SHOT_PROTOCOL>(
		(converter_func)conv_ghostshot_to_shotprotocol);

static bool registered_conv_shotprotocol_to_ghostshot =
	type_convert::insert_converter<GHOST_SHOT_PROTOCOL, sShotProtocol>(
		(converter_func)conv_shotprotocol_to_ghostshot);
