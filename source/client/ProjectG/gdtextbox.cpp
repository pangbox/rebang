#include "gdtextbox.h"

CGDTextBox::CGDTextBox()
{
}

CGDTextBox::~CGDTextBox()
{
}

void CGDTextBox::Initialize(TiXmlNode* pNode)
{
	TiXmlNode* pChild = pNode->FirstChild("row");
	while (pChild)
	{
		std::string strKey = pChild->ToElement()->Attribute("key");
		std::string text = pChild->ToElement()->Attribute("text");

		str_Replace(text, "\\n", "\n");
		str_Replace(text, "\\\\", "\\");
		const char* value = text.c_str();
		text = value;

		std::map<std::string, std::string>::iterator it =
			m_textMap.find(strKey);
		if (it == m_textMap.end())
		{
			m_textMap.insert(
				std::map<std::string, std::string>::value_type(strKey, text));
		}
		pChild = pChild->NextSibling("row");
	}
}

const char* CGDTextBox::GetText(const char* key)
{
	std::map<std::string, std::string>::iterator it = m_textMap.find(key);
	if (it != m_textMap.end())
	{
		return (*it).second.c_str();
	}
	return NULL;
}

const char* CGDTextBox::GetText(std::string& key)
{
	std::map<std::string, std::string>::iterator it = m_textMap.find(key);
	if (it != m_textMap.end())
	{
		return (*it).second.c_str();
	}
	return NULL;
}
