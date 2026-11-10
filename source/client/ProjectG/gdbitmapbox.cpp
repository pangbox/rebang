#include "gdbitmapbox.h"

CGDBitmapBox::CGDBitmapBox()
{
}

CGDBitmapBox::~CGDBitmapBox()
{
}

void CGDBitmapBox::Initialize(TiXmlNode* pNode)
{
	TiXmlNode* pChild = pNode->FirstChild("row");
	while (pChild)
	{
		std::string strKey = pChild->ToElement()->Attribute("key");
		std::string fileName = pChild->ToElement()->Attribute("file");
		std::string rect = pChild->ToElement()->Attribute("rect");

		sGD_BITMAP bitmap;
		strcpy(bitmap.fileName, fileName.c_str());
		sscanf(rect.c_str(), "%f %f %f %f", &bitmap.rect.x, &bitmap.rect.y,
			&bitmap.rect.w, &bitmap.rect.h);

		std::map<std::string, sGD_BITMAP>::iterator it =
			m_bitmapMap.find(strKey);
		if (it == m_bitmapMap.end())
		{
			m_bitmapMap.insert(
				std::map<std::string, sGD_BITMAP>::value_type(strKey, bitmap));
		}
		pChild = pChild->NextSibling("row");
	}
}

const char* CGDBitmapBox::GetFileName(char* key)
{
	std::map<std::string, sGD_BITMAP>::iterator it = m_bitmapMap.find(key);
	if (it != m_bitmapMap.end())
	{
		return (*it).second.fileName;
	}
	return NULL;
}

void CGDBitmapBox::GetRect(const char* key, _WRECT& rect)
{
	std::map<std::string, sGD_BITMAP>::iterator it = m_bitmapMap.find(key);
	if (it != m_bitmapMap.end())
	{
		rect.x = (*it).second.rect.x;
		rect.y = (*it).second.rect.y;
		rect.w = (*it).second.rect.w;
		rect.h = (*it).second.rect.h;
	}
}
