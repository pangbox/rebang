#include "titles_client.h"

TitleManager::TitleManager()
{
}

TitleManager::~TitleManager()
{
	TITLEITR Itr = TitleMap.begin();
	for (; Itr != TitleMap.end(); ++Itr)
	{
		delete Itr->second;
	}
	TitleMap.clear();
}

void TitleManager::Register(eTitle TitleNum, Title* ClassPtr)
{
	TITLEITR Iter = TitleMap.find(TitleNum);
	if (Iter == TitleMap.end())
	{
		TitleMap.insert(TITLEMAP::value_type(TitleNum, ClassPtr));
	}
	else
		delete ClassPtr;
}

Title* TitleManager::GetTitle(eTitle TitleNum)
{
	TITLEITR Itr = TitleMap.find(TitleNum);
	if (Itr == TitleMap.end())
	{
		return NULL;
	}
	return Itr->second;
}
