#pragma once
#include <string>
#include <map>
#include <list>
#include "../../../shared/coord.h"
#include "commonutil.h"

class Bitmap;
class TiXmlNode;

enum enumElement
{
	ED_NONE = 0x0,
	ED_FRAME = 0x1,
	ED_LAYOUT = 0x2,
	ED_FORM = 0x3,
	ED_BITMAP = 0x4,
	ED_ICON = 0x5,
	ED_INCLUDE = 0x6,
	ED_MACROITEM = 0x7,
	ED_LAST = 0x8,
};

enum enumGuiType
{
	GI_NONE = 0x0,
	GI_FORM = 0x1,
	GI_STATIC = 0x2,
	GI_TEXTBUTTON = 0x3,
	GI_EDIT = 0x4,
	GI_COMBOBOX = 0x5,
	GI_COMBOCTLEX = 0x6,
	GI_BUTTON = 0x7,
	GI_FRAME = 0x8,
	GI_RESOURCE = 0x9,
	GI_BITMAP = 0xA,
	GI_AREA = 0xB,
	GI_LISTBOX = 0xC,
	GI_GAUGEBAR = 0xD,
	GI_GAUGEBAREX = 0xE,
	GI_GAUGEBARIMAGE = 0xF,
	GI_VIEWER = 0x10,
	GI_CONTEXTMENU = 0x11,
	GI_TABBUTTON = 0x12,
	GI_GROUPBOX = 0x13,
	GI_MACROITEM = 0x14,
	GI_LAST = 0x15,
};

template <class T>
struct FrTypeID
{
	FrTypeID() { }
	~FrTypeID() { }

	std::string operator[](const T& type);
	T operator[](const std::string& typeId)
	{
		std::map<T, std::string>::iterator it;
		for (it = m_map.begin(); it != m_map.end(); ++it)
		{
			if (it->second == typeId)
				return it->first;
		}
		return (T)0;
	}

protected:
	std::map<T, std::string> m_map;
};

struct FrElementTID : public FrTypeID<enumElement>
{
	FrElementTID();
};

struct FrGuiTID : public FrTypeID<enumGuiType>
{
	FrGuiTID();
};

extern FrElementTID ElementType;
extern FrGuiTID GuiType;

class FrGuiItem
{
public:
	FrGuiItem()
		: m_type(GI_NONE), m_flag(0)
	{
	}
	virtual ~FrGuiItem() { }
	virtual void Init(const TiXmlNode* pSrc);
	virtual const std::list<FrGuiItem*>* GetChildList();

	enumGuiType m_type;
	std::string m_resource;
	std::string m_caption;
	std::string m_name;
	std::map<std::string, std::string> m_param;
	unsigned long m_flag;
	_RectangleSHORT m_rect;
};

class FrGuiItemNested : public FrGuiItem
{
public:
	virtual ~FrGuiItemNested();
	virtual void Init(const TiXmlNode* pSrc);
	virtual const std::list<FrGuiItem*>* GetChildList();

protected:
	std::list<FrGuiItem*> m_childList;
};

class __declspec(novtable) FrElement
{
public:
	FrElement();
	FrElement(enumElement type)
		: m_type(type)
	{
	}
	FrElement(enumElement type, const std::string& name);
	virtual ~FrElement() { }
	virtual void Init(const TiXmlNode* pSrc) = 0;

	std::string m_name;
	enumElement m_type;
};

class FrElementFrame : public FrElement
{
public:
	FrElementFrame()
		: FrElement(ED_FRAME)
	{
	}
	FrElementFrame(const std::string& name);
	virtual ~FrElementFrame() { }
	virtual void Init(const TiXmlNode* pSrc);

	unsigned long m_color;
	int m_layers;
	int m_aType[8];
	int m_aPos[8];
	int m_aHeight[8];
	int m_aInvisible[8];
	std::string m_bfrmName;
	std::string m_sfrmName;
	std::string m_cfrmName;
};

class FrElementLayout : public FrElement
{
public:
	FrElementLayout()
		: FrElement(ED_LAYOUT)
	{
	}
	FrElementLayout(const std::string& name);
	virtual ~FrElementLayout()
	{
		sequence_delete(m_guiList.begin(), m_guiList.end());
		m_guiList.clear();
	}
	virtual void Init(const TiXmlNode* pSrc);

	struct sSize
	{
		int cx;
		int cy;
	};

	sSize m_bgSize;
	unsigned long m_bgColor;
	std::string m_bgFilename;
	std::list<FrGuiItem*> m_guiList;
	FrGuiItem m_aniBg;
};

class FrElementBitmap : public FrElement
{
public:
	FrElementBitmap()
		: FrElement(ED_BITMAP)
	{
	}
	virtual ~FrElementBitmap() { }
	bool Create(const char* name, const char* filename);
	virtual void Init(const TiXmlNode* pSrc);

	std::string m_bitmapFilename;
};

class FrElementIcon : public FrElement
{
public:
	FrElementIcon()
		: FrElement(ED_ICON)
	{
	}
	virtual ~FrElementIcon() { }
	virtual void Init(const TiXmlNode* pSrc);
};

class FrElementForm : public FrElement
{
public:
	FrElementForm()
		: FrElement(ED_FORM)
	{
	}
	virtual ~FrElementForm()
	{
		sequence_delete(m_guiList.begin(), m_guiList.end());
		m_guiList.clear();
	}
	virtual void Init(const TiXmlNode* pSrc);

	std::string m_caption;
	std::string m_desc;

	struct sSize
	{
		int w;
		int h;
	};

	sSize m_bgSize;
	FrGuiItem m_bgFrame;
	std::list<FrGuiItem*> m_guiList;
};

class FrElementMacroItem : public FrElement
{
public:
	FrElementMacroItem()
		: FrElement(ED_MACROITEM)
	{
	}
	virtual ~FrElementMacroItem()
	{
		sequence_delete(m_guiList.begin(), m_guiList.end());
		m_guiList.clear();
	}
	virtual void Init(const TiXmlNode* pSrc);

	std::list<FrGuiItem*> m_guiList;
};

struct resBitmap_t
{
	Bitmap* bitmap;
	int size;
	std::string name;
};

typedef std::map<std::string, resBitmap_t> FreshBitmapMap;

template <class L, class T>
T* FindElement(L& eList, enumElement eType, const std::string& resID)
{
	L::iterator it;
	L::iterator itEnd = eList.end();
	it = std::find_if(eList.begin(), itEnd,
		sameElement<FrElement*>(eType, resID));
	if (it != itEnd)
		return static_cast<T*>(*it);
	return 0;
}

class FrElementDoc
{
public:
	FrElementDoc() { }
	virtual ~FrElementDoc();
	bool Load(const char* pszXmlFilename);
	FrElementLayout* GetLayout(const std::string& resID);
	const std::list<FrElement*>& GetList() const { return m_elementList; }
	const FreshBitmapMap& GetBitmapList() const { return m_mapBitmap; }
	FrElementFrame* GetFrame(const std::string& resID);
	FrElementForm* GetForm(const std::string& resID);
	const Bitmap* GetBitmap(const std::string& id);
	FrElementMacroItem* GetMacroItem(const std::string& resID);

protected:
	bool LoadXml(const char* pszXmlFilename, std::list<FrElement*>& tempList);
	int LoadPic(const char* filename);
	void ClearBitmaps();

	std::list<FrElement*> m_elementList;
	FreshBitmapMap m_mapBitmap;
	std::string m_xmlFileName;
};
