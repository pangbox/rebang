#include <algorithm>
#include <locale.h>
#include "frelement.h"
#include "tinyxml.h"

FrElementTID ElementType;
FrGuiTID GuiType;

FrElementTID::FrElementTID()
{
	m_map[ED_NONE] = "NONE";
	m_map[ED_FRAME] = "FRAME";
	m_map[ED_LAYOUT] = "LAYOUT";
	m_map[ED_FORM] = "FORM";
	m_map[ED_BITMAP] = "BITMAP";
	m_map[ED_ICON] = "ICON";
	m_map[ED_INCLUDE] = "INCLUDE";
	m_map[ED_MACROITEM] = "MACROITEM";
}

FrGuiTID::FrGuiTID()
{
	m_map[GI_NONE] = "NONE";
	m_map[GI_FORM] = "FORM";
	m_map[GI_STATIC] = "STATIC";
	m_map[GI_TEXTBUTTON] = "TEXTBUTTON";
	m_map[GI_EDIT] = "EDIT";
	m_map[GI_COMBOBOX] = "COMBOBOX";
	m_map[GI_COMBOCTLEX] = "COMBOCTLEX";
	m_map[GI_BUTTON] = "BUTTON";
	m_map[GI_FRAME] = "FRAME";
	m_map[GI_RESOURCE] = "RESOURCE";
	m_map[GI_BITMAP] = "BITMAP";
	m_map[GI_AREA] = "AREA";
	m_map[GI_LISTBOX] = "LISTBOX";
	m_map[GI_GAUGEBAR] = "GAUGEBAR";
	m_map[GI_GAUGEBAREX] = "GAUGEBAREX";
	m_map[GI_GAUGEBARIMAGE] = "GAUGEBARIMAGE";
	m_map[GI_VIEWER] = "VIEWER";
	m_map[GI_CONTEXTMENU] = "CONTEXTMENU";
	m_map[GI_TABBUTTON] = "TABBUTTON";
	m_map[GI_GROUPBOX] = "GROUPBOX";
	m_map[GI_MACROITEM] = "MACROITEM";
}

static void CreateGuiItem(const TiXmlNode* pSrc, std::list<FrGuiItem*>& guiList)
{
	TiXmlNode* pNode = pSrc->FirstChild("item");
	while (pNode)
	{
		FrGuiItem* pItem;
		if (pNode->FirstChild("item"))
			pItem = new FrGuiItemNested;
		else
			pItem = new FrGuiItem;

		if (pItem)
		{
			pItem->Init(pNode);
			guiList.push_back(pItem);
		}
		pNode = pNode->NextSibling("item");
	}
}

void FrGuiItem::Init(const TiXmlNode* pSrc)
{
	TiXmlElement* pElement = pSrc->ToElement();

	std::string aType = pElement->Attribute("type");
	std::string aRect = pElement->Attribute("rect");
	std::string aPos = pElement->Attribute("pos");

	memset(&m_rect, 0, sizeof(m_rect));
	if (aPos.empty())
	{
		int v[4];
		sscanf(aRect.c_str(), "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);
		m_rect.tl.x = v[0];
		m_rect.tl.y = v[1];
		m_rect.br.x = v[2];
		m_rect.br.y = v[3];
	}
	else
	{
		int v[2];
		sscanf(aPos.c_str(), "%d %d", &v[0], &v[1]);
		m_rect.tl.x = v[0];
		m_rect.tl.y = v[1];
		m_rect.br.x = v[0] + 10;
		m_rect.br.y = v[1] + 10;
	}

	m_type = GuiType[aType];
	m_resource = pElement->Attribute("resource");
	m_name = pElement->Attribute("name");
	m_caption = pElement->Attribute("caption");
	pElement->Attribute("flag", (int*)&m_flag);

	TiXmlNode* pNode = pSrc->FirstChild("param");
	while (pNode)
	{
		TiXmlElement* pParam = pNode->ToElement();
		std::string aName = pParam->Attribute("name");
		std::string aVar = pParam->Attribute("var");
		if (!aName.empty() && !aVar.empty())
			m_param[aName] = aVar;
		pNode = pNode->NextSibling();
	}
}

const std::list<FrGuiItem*>* FrGuiItem::GetChildList()
{
	return 0;
}

FrGuiItemNested::~FrGuiItemNested()
{
	sequence_delete(m_childList.begin(), m_childList.end());
	m_childList.clear();
}

void FrGuiItemNested::Init(const TiXmlNode* pSrc)
{
	FrGuiItem::Init(pSrc);

	CreateGuiItem(pSrc, m_childList);
}

const std::list<FrGuiItem*>* FrGuiItemNested::GetChildList()
{
	return &m_childList;
}

void FrElementFrame::Init(const TiXmlNode* pSrc)
{
	std::string color = pSrc->ToElement()->Attribute("color");
	sscanf(color.c_str(), "%x", &m_color);

	TiXmlNode* pNode = pSrc->FirstChild("layer");
	for (m_layers = 0; pNode && m_layers < sizeof(m_aType) / sizeof(m_aType[0]);
		m_layers++)
	{
		m_aInvisible[m_layers] = 0;
		TiXmlElement* pLayer = pNode->ToElement();
		if (pLayer)
		{
			pLayer->Attribute("type", &m_aType[m_layers]);
			pLayer->Attribute("height", &m_aHeight[m_layers]);
			pLayer->Attribute("pos", &m_aPos[m_layers]);
			pLayer->Attribute("invisible", &m_aInvisible[m_layers]);
			int pos;
			pLayer->Attribute("bottompos", &pos);
			if (pos)
				m_aPos[m_layers] = -pos;
		}
		pNode = pNode->NextSibling("layer");
	}

	TiXmlElement* pFrm = pSrc->FirstChild("bfrm")->ToElement();
	if (pFrm)
		m_bfrmName = pFrm->Attribute("filename");
	pFrm = pSrc->FirstChild("sfrm")->ToElement();
	if (pFrm)
		m_sfrmName = pFrm->Attribute("filename");
	pFrm = pSrc->FirstChild("cfrm")->ToElement();
	if (pFrm)
		m_cfrmName = pFrm->Attribute("filename");
}

void FrElementLayout::Init(const TiXmlNode* pSrc)
{
	TiXmlNode* pNode = pSrc->FirstChild("base");
	if (pNode)
	{
		char buff[256];
		TiXmlElement* pElement = pNode->ToElement();
		pElement->Attribute("width", &m_bgSize.cx);
		pElement->Attribute("height", &m_bgSize.cy);
		sprintf(buff, "0x%s", pElement->Attribute("color").c_str());
		sscanf(buff, "%X", &m_bgColor);
		m_bgFilename = pElement->Attribute("background");

		CreateGuiItem(pSrc, m_guiList);

		pNode = pSrc->FirstChild("anibg");
		if (pNode)
		{
			int v[4];
			pElement = pNode->ToElement();
			sprintf(buff, "%s", pElement->Attribute("rect").c_str());
			sscanf(buff, "%d %d %d %d", &v[0], &v[1], &v[2], &v[3]);
			m_aniBg.m_rect.tl.x = v[0];
			m_aniBg.m_rect.tl.y = v[1];
			m_aniBg.m_rect.br.x = v[2];
			m_aniBg.m_rect.br.y = v[3];
			m_aniBg.m_param["vel"] = pElement->Attribute("vel");
			m_aniBg.m_param["img"] = pElement->Attribute("img");
		}
	}
}

void FrElementForm::Init(const TiXmlNode* pSrc)
{
	m_caption = pSrc->ToElement()->Attribute("caption");
	m_desc = pSrc->ToElement()->Attribute("description");

	std::string bgSize = pSrc->ToElement()->Attribute("size");
	sscanf(bgSize.c_str(), "%d %d", &m_bgSize.w, &m_bgSize.h);
	m_bgFrame.m_resource = pSrc->ToElement()->Attribute("resource");

	short w = m_bgSize.w;
	short h = m_bgSize.h;
	m_bgFrame.m_rect.left = 0;
	m_bgFrame.m_rect.top = 0;
	m_bgFrame.m_rect.right = w;
	m_bgFrame.m_rect.bottom = h;
	m_bgFrame.m_type = GI_FRAME;
	m_bgFrame.m_flag = 1;

	CreateGuiItem(pSrc, m_guiList);
}

void FrElementBitmap::Init(const TiXmlNode* pSrc)
{
	m_bitmapFilename = pSrc->ToElement()->Attribute("filename");
}

void FrElementIcon::Init(const TiXmlNode* pSrc)
{
}

void FrElementMacroItem::Init(const TiXmlNode* pSrc)
{
	CreateGuiItem(pSrc, m_guiList);
}

const Bitmap* FrElementDoc::GetBitmap(const std::string& id)
{
	if (id.empty())
		return 0;

	if (m_mapBitmap.find(id) == m_mapBitmap.end())
	{
		LoadPic(id.c_str());
		if (m_mapBitmap.find(id) == m_mapBitmap.end())
			return 0;
	}
	return m_mapBitmap[id].bitmap;
}

void FrElementDoc::ClearBitmaps()
{
	for (FreshBitmapMap::iterator it = m_mapBitmap.begin();
		it != m_mapBitmap.end(); ++it)
		delete (*it).second.bitmap;

	m_mapBitmap.clear();
}

int FrElementDoc::LoadPic(const char* filename)
{
	int type = 1;
	cFile* file = g_resrcmng->GetCFile(MakeStr("%s.tga", filename), 0xffff);
	if (!file)
	{
		file = g_resrcmng->GetCFile(MakeStr("%s.bmp", filename), 0xffff);
		if (!file)
			return 0;
		type = 0;
	}

	int filesize = file->Length();
	BYTE* buf = new BYTE[filesize + 1];
	file->Read(buf, filesize);
	buf[filesize] = 0;
	CloseCFile(file);

	Bitmap* pBitmap;
	unsigned height;
	unsigned bpl;
	BYTE* data;
	if (type == 0)
	{
		data = buf + 54;
		pBitmap = new Bitmap(*(int*)(buf + 18), *(int*)(buf + 22),
			*(short*)(buf + 28));
		height = *(int*)(buf + 22);
		bpl = pBitmap->pitch;
	}
	else if (type == 1)
	{
		data = buf + 18;
		pBitmap = new Bitmap(*(short*)(buf + 12), *(short*)(buf + 14), buf[16]);
		height = *(short*)(buf + 14);
		bpl = *(short*)(buf + 12) * buf[16] / 8;
	}
	else
	{
		delete[] buf;
		return 0;
	}

	for (unsigned y = 0; y < height; y++)
		memcpy(pBitmap->GetVram(y), data + (height - y - 1) * bpl, bpl);

	resBitmap_t bm;
	bm.bitmap = pBitmap;
	bm.name = filename;
	bm.size = filesize;
	m_mapBitmap[filename] = bm;

	delete[] buf;

	return 1;
}

template <class T>
struct sameElement
{
	sameElement(enumElement type, const std::string& resID)
		: m_resID(resID), m_type(type)
	{
	}
	bool operator()(T src)
	{
		return src->m_type == m_type && src->m_name == m_resID;
	}

	const std::string& m_resID;
	const enumElement m_type;
};

bool FrElementDoc::Load(const char* pszXmlFilename)
{
	std::list<FrElement*> tempList;

	ClearBitmaps();

	setlocale(LC_CTYPE, "Kor");

	m_xmlFileName = pszXmlFilename;
	LoadXml(pszXmlFilename, tempList);

	sequence_delete(m_elementList.begin(), m_elementList.end());
	m_elementList.clear();

	for (std::list<FrElement*>::iterator it = tempList.begin();
		it != tempList.end(); ++it)
		m_elementList.push_back(*it);

	return true;
}

bool FrElementDoc::LoadXml(const char* pszXmlFilename,
	std::list<FrElement*>& tempList)
{
	TiXmlDocument xml;
	if (!xml.LoadFileEx(pszXmlFilename))
		return false;

	TiXmlNode* pNode = xml.FirstChild("resource");
	if (!pNode)
		return false;

	int count;
	pNode->ToElement()->Attribute("count", &count);
	pNode = pNode->FirstChild("element");
	while (pNode)
	{
		TiXmlElement* pXmlElement = pNode->ToElement();
		if (!pXmlElement)
		{
			pNode = pNode->NextSibling();
			continue;
		}

		std::string name = pXmlElement->Attribute("name");
		std::string type = pXmlElement->Attribute("type");
		enumElement eType = ElementType[type];

		std::list<FrElement*>::iterator it = m_elementList.end();
		switch (eType)
		{
		case ED_INCLUDE:
			if (!LoadXml(name.c_str(), tempList))
				return false;
			break;
		case ED_FRAME:
		case ED_LAYOUT:
		case ED_FORM:
		case ED_MACROITEM:
			break;
		default:
			it = std::find_if(m_elementList.begin(), m_elementList.end(),
				sameElement<FrElement*>(eType, name));
			break;
		}

		if (it != m_elementList.end())
		{
			tempList.push_back(*it);
			m_elementList.erase(it);
		}
		else
		{
			FrElement* pElement = 0;
			switch (eType)
			{
			case ED_FRAME:
				pElement = new FrElementFrame;
				break;
			case ED_LAYOUT:
				pElement = new FrElementLayout;
				break;
			case ED_BITMAP:
				pElement = new FrElementBitmap;
				break;
			case ED_FORM:
				pElement = new FrElementForm;
				break;
			case ED_MACROITEM:
				pElement = new FrElementMacroItem;
				break;
			}
			if (pElement)
			{
				pElement->m_name = name;
				pElement->Init(pNode);
				tempList.push_back(pElement);
			}
		}
		pNode = pNode->NextSibling();
	}
	return true;
}

FrElementDoc::~FrElementDoc()
{
	sequence_delete(m_elementList.begin(), m_elementList.end());
	m_elementList.clear();

	ClearBitmaps();
}

FrElementLayout* FrElementDoc::GetLayout(const std::string& resID)
{
	std::list<FrElement*>::iterator it;
	it = std::find_if(m_elementList.begin(), m_elementList.end(),
		sameElement<FrElement*>(ED_LAYOUT, resID));
	if (it != m_elementList.end())
		return static_cast<FrElementLayout*>(*it);
	return 0;
}

FrElementFrame* FrElementDoc::GetFrame(const std::string& resID)
{
	return FindElement<std::list<FrElement*>, FrElementFrame>(m_elementList,
		ED_FRAME, resID);
}

FrElementForm* FrElementDoc::GetForm(const std::string& resID)
{
	return FindElement<std::list<FrElement*>, FrElementForm>(m_elementList,
		ED_FORM, resID);
}

FrElementMacroItem* FrElementDoc::GetMacroItem(const std::string& resID)
{
	return FindElement<std::list<FrElement*>, FrElementMacroItem>(m_elementList,
		ED_MACROITEM, resID);
}
