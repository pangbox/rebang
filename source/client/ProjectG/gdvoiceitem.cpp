#include "gdvoiceitem.h"

CGDVoiceItem::CGDVoiceItem()

{
}

CGDVoiceItem::~CGDVoiceItem()
{
}

void CGDVoiceItem::Initialize(TiXmlNode* pNode)
{
	TiXmlNode* pChild = pNode->FirstChild("row");
	while (pChild)
	{
		sGD_VOICEITEM item;
		std::string typeID = pChild->ToElement()->Attribute("typeid");
		item.name = pChild->ToElement()->Attribute("name");
		std::string state = pChild->ToElement()->Attribute("state");
		std::string numOfSounds = pChild->ToElement()->Attribute("numofsounds");

		sscanf(typeID.c_str(), "%d", &item.typeID);
		sscanf(state.c_str(), "%d", &item.state);
		sscanf(numOfSounds.c_str(), "%d", &item.numOfSounds);

		m_voiceList.push_back(item);

		pChild = pChild->NextSibling("row");
	}
}
