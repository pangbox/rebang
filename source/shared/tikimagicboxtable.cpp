#include <stack>

#include "tikimagicboxtable.h"

void CTikiMagicBoxDoc::InsertOutput(const IFF_STRUCT::sTikiOutputTable& table)
{
	IFF_STRUCT::sTikiOutputTable* pRecord = new IFF_STRUCT::sTikiOutputTable;
	if (pRecord == NULL)
		return;
	*pRecord = table;

	std::map<std::string,
		std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>*>::iterator itr =
		m_OPTCategory.find(pRecord->strCategory);

	if (itr == m_OPTCategory.end())
	{
		std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>* pMap =
			new std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>;
		if (pMap)
		{
			m_OPTCategory.insert(std::make_pair(pRecord->strCategory, pMap));
		}

		itr = m_OPTCategory.find(pRecord->strCategory);
	}

	if (itr != m_OPTCategory.end())
	{
		std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>* pMap =
			(*itr).second;

		if (pMap)
		{
			pMap->insert(std::make_pair(pRecord->uiIndex, pRecord));
		}
	}
}

void CTikiMagicBoxDoc::InsertPointTable(
	const IFF_STRUCT::sTikiPointTable& table)
{
	IFF_STRUCT::sTikiPointTable* pRecord = new IFF_STRUCT::sTikiPointTable;
	if (pRecord == NULL)
		return;
	*pRecord = table;

	m_PointTable.insert(std::make_pair(pRecord->uiIndex, pRecord));
}

void CTikiMagicBoxDoc::InsertSpecialRecipe(
	const IFF_STRUCT::sTikiSpecialRecipe& table)
{
	IFF_STRUCT::sTikiSpecialRecipe* pRecord =
		new IFF_STRUCT::sTikiSpecialRecipe;
	if (pRecord == NULL)
		return;
	*pRecord = table;

	m_SpecialTable.insert(std::make_pair(pRecord->uiIndex, pRecord));
}

bool CTikiMagicBoxDoc::GetOutput(const int mtrCount,
	const sTikiMagicBoxMtr* const mtr, sTikiMagicBoxMtr& output)
{
	if (mtrCount == 0 || mtr == NULL)
		return false;

	char szCategory[32] = "\0";

	memset(&output, 0, sizeof(output));

	bool bFound = false;
	std::map<unsigned int, IFF_STRUCT::sTikiSpecialRecipe*>::iterator
		spBegin = m_SpecialTable.begin(),
		spEnd = m_SpecialTable.end();

	for (; spBegin != spEnd; ++spBegin)
	{
		IFF_STRUCT::sTikiSpecialRecipe* pRecord = (*spBegin).second;

		if (pRecord)
		{
			if (mtrCount == pRecord->uiElemCount)
			{
				unsigned long* data[2] = { NULL, NULL };

				for (int i = 0; i < 2; i++)
				{
					data[i] = new unsigned long[mtrCount];
				}

				for (int i = 0; i < mtrCount; i++)
				{
					data[0][i] = mtr[i].dwTypeID;
					data[1][i] = pRecord->uiElem[i];
				}

				for (int i = 0; i < mtrCount; i++)
				{
					for (int j = i; j < mtrCount; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							if (data[k][i] > data[k][j])
							{
								unsigned long temp = data[k][i];
								data[k][i] = data[k][j];
								data[k][j] = temp;
							}
						}
					}
				}

				std::stack<unsigned long> MaterialStack;
				std::stack<unsigned long> RecipeStack;

				for (int i = 0; i < mtrCount; i++)
				{
					MaterialStack.push(data[0][i]);
					RecipeStack.push(data[1][i]);
				}

				for (int i = 0; i < 2; i++)
				{
					delete[] data[i];
					data[i] = NULL;
				}

				for (int i = 0; i < mtrCount; i++)
				{
					if (MaterialStack.top() == RecipeStack.top())
					{
						MaterialStack.pop();
						RecipeStack.pop();
					}
				}

				if (MaterialStack.size() == 0 && RecipeStack.size() == 0)
				{
					strcpy(szCategory, pRecord->strCategory);
					bFound = true;
					break;
				}
			}
		}
	}

	if (bFound == false)
	{
		unsigned int uiPoint = 0;
		for (int i = 0; i < mtrCount; i++)
		{
			switch (mtr[i].dwTypeID >> 26)
			{
			case PIG_PART:
			{
				IFF_STRUCT::sPart* pPart =
					ItemManager()->FindPart(mtr[i].dwTypeID);
				if (pPart)
				{
					uiPoint += pPart->Point * mtr[i].dwCount;
				}
			}
			break;
			case PIG_ITEM:
			{
				IFF_STRUCT::sItem* pItem =
					ItemManager()->FindItem(mtr[i].dwTypeID);
				if (pItem)
				{
					uiPoint += pItem->Point * mtr[i].dwCount;
				}
			}
			break;
			}
		}

		std::map<unsigned int, IFF_STRUCT::sTikiPointTable*>::iterator
			ptBegin = m_PointTable.begin(),
			ptEnd = m_PointTable.end();
		for (; ptBegin != ptEnd; ++ptBegin)
		{
			IFF_STRUCT::sTikiPointTable* pPoint = (*ptBegin).second;
			if (pPoint)
			{
				if (pPoint->uiMin <= uiPoint && pPoint->uiMax >= uiPoint)
				{
					strcpy(szCategory, pPoint->strCategory);
					break;
				}
			}
		}
	}

	if (strlen(szCategory) != 0)
	{
		std::map<std::string,
			std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>*>::iterator
			itr = m_OPTCategory.find(szCategory);

		if (itr != m_OPTCategory.end())
		{
			std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>* pOutput =
				(*itr).second;

			if (pOutput)
			{
				unsigned int uiIndex = rand() % pOutput->size();
				IFF_STRUCT::sTikiOutputTable* pTable = (*pOutput)[uiIndex];

				if (pTable)
				{
					output.dwTypeID = pTable->uiTypeID;
					output.dwCount = pTable->uiCount;
					output.dwGuid = 0;
				}
			}
		}
	}

	return false;
}

void CTikiMagicBoxDoc::CleanUp()
{
	std::map<std::string,
		std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>*>::iterator
		opcBegin = m_OPTCategory.begin(),
		opcEnd = m_OPTCategory.end();

	for (; opcBegin != opcEnd; ++opcBegin)
	{
		std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>* pMap =
			(*opcBegin).second;
		if (pMap)
		{
			std::map<unsigned int, IFF_STRUCT::sTikiOutputTable*>::iterator
				opBegin = pMap->begin(),
				opEnd = pMap->end();

			for (; opBegin != opEnd; ++opBegin)
			{
				IFF_STRUCT::sTikiOutputTable* pTable = (*opBegin).second;
				if (pTable)
				{
					delete pTable;
					(*opBegin).second = NULL;
				}
			}
			pMap->clear();

			delete pMap;
			(*opcBegin).second = NULL;
		}
	}
	m_OPTCategory.clear();

	std::map<unsigned int, IFF_STRUCT::sTikiPointTable*>::iterator
		ptBegin = m_PointTable.begin(),
		ptEnd = m_PointTable.end();

	for (; ptBegin != ptEnd; ++ptBegin)
	{
		IFF_STRUCT::sTikiPointTable* pTable = (*ptBegin).second;
		if (pTable)
		{
			delete pTable;
			(*ptBegin).second = NULL;
		}
	}
	m_PointTable.clear();

	std::map<unsigned int, IFF_STRUCT::sTikiSpecialRecipe*>::iterator
		spBegin = m_SpecialTable.begin(),
		spEnd = m_SpecialTable.end();

	for (; spBegin != spEnd; ++spBegin)
	{
		IFF_STRUCT::sTikiSpecialRecipe* pRecord = (*spBegin).second;
		if (pRecord)
		{
			delete pRecord;
			(*spBegin).second = NULL;
		}
	}
	m_SpecialTable.clear();
}
