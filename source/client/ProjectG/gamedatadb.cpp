#include "gamedatadb.h"
#include "gdeventnotice.h"
#include "gdtextbox.h"
#include "gdbitmapbox.h"
#include "gdvoiceitem.h"

CGameDataDB::CGameDataDB()
{
	RegisterGameData();
	LoadGameData("gamedata.xml");
}

CGameDataDB::~CGameDataDB()
{
	Destory();
}

void CGameDataDB::RegisterGameDataToDB(eGameDataDB type, std::string& name,
	IGameData* pData)
{
	std::map<std::string, IGameData*>::iterator it =
		m_gameData[type].dataMap.find(name);
	if (it == m_gameData[type].dataMap.end())
		m_gameData[type].dataMap.insert(
			std::map<std::string, IGameData*>::value_type(name, pData));
}

void CGameDataDB::LoadGameData(const char* filename)
{
	TiXmlDocument doc;
	if (!doc.LoadFileEx(filename))
		return;
	ParseGameData(doc);
}

void CGameDataDB::RegisterCreateFunction(eGameDataDB type,
	IGameData*(__fastcall* pfnCreate)())
{
	m_gameData[type].pfnCreate = pfnCreate;
}

void CGameDataDB::RegisterDestoryFunction(eGameDataDB type,
	void(__fastcall* pfnDestory)(IGameData*))
{
	m_gameData[type].pfnDestory = pfnDestory;
}

void CGameDataDB::RegisterGameID(eGameDataDB type, const char* id)
{
	m_gameData[type].gameID = id;
}

void CGameDataDB::RegisterGameData()
{
	CGDTextBox::RegisterToGameDB();
	CGDBitmapBox::RegisterToGameDB();
	CGDEventNotice::RegisterToGameDB();
	CGDVoiceItem::RegisterToGameDB();
}

void CGameDataDB::ParseGameData(TiXmlDocument& doc)
{
	for (int i = 0; i < GAMEDATA_MAX; ++i)
	{
		TiXmlNode* pNode = doc.FirstChild(m_gameData[i].gameID.c_str());
		while (pNode)
		{
			IGameData* pData = m_gameData[i].pfnCreate();
			std::string name = pNode->ToElement()->Attribute("name");
			pData->Initialize(pNode);
			RegisterGameDataToDB((eGameDataDB)i, name, pData);
			pNode = pNode->NextSibling(m_gameData[i].gameID.c_str());
		}
	}
}

void CGameDataDB::Destory()
{
	for (int i = 0; i < GAMEDATA_MAX; ++i)
	{
		std::map<std::string, IGameData*>::iterator it =
			m_gameData[i].dataMap.begin();
		for (; it != m_gameData[i].dataMap.end(); ++it)
			m_gameData[i].pfnDestory((*it).second);
	}
}

IGameData* CGameDataDB::GetGameData(eGameDataDB type, const char* name)
{
	std::map<std::string, IGameData*>::iterator it =
		m_gameData[type].dataMap.find(name);
	if (it != m_gameData[type].dataMap.end())
		return (*it).second;
	return NULL;
}
