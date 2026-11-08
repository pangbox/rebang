#include "gdeventnotice.h"

CGDEventNotice::CGDEventNotice()
{
}

CGDEventNotice::~CGDEventNotice()
{
}

void CGDEventNotice::Initialize(TiXmlNode* pNode)
{
	TiXmlNode* pChild = pNode->FirstChild("row");
	while (pChild)
	{
		std::string strKey = pChild->ToElement()->Attribute("key");
		int key = atoi(strKey.c_str());
		std::string text = pChild->ToElement()->Attribute("text");

		str_Replace(text, "\\n", "\n");
		str_Replace(text, "\\\\", "\\");

		text = text.c_str();

		std::map<int, std::string>::iterator it = m_textMap.find(key);
		if (it == m_textMap.end())
		{
			m_textMap.insert(std::map<int, std::string>::value_type(key, text));
		}
		pChild = pChild->NextSibling("row");
	}
}

const char* CGDEventNotice::GetText(int key)
{
	std::map<int, std::string>::iterator it = m_textMap.find(key);
	if (it != m_textMap.end())
	{
		return (*it).second.c_str();
	}

	return NULL;
}
