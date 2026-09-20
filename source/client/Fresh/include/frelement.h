#pragma once
#include <string>
#include <map>
#include <list>

class FrElement;
class Bitmap;

struct resBitmap_t
{
	Bitmap* bitmap;
	int size;
	std::string name;
};

typedef std::map<std::string, resBitmap_t> FreshBitmapMap;

class FrElementDoc
{
public:
	FrElementDoc() { }
	virtual ~FrElementDoc();
	bool Load(const char* pszXmlFilename);
	const FreshBitmapMap& GetBitmapList() const { return m_mapBitmap; }
	const Bitmap* GetBitmap(const std::string& id);

protected:
	int LoadPic(const char* filename);

	std::list<FrElement*> m_elementList;
	FreshBitmapMap m_mapBitmap;
	std::string m_xmlFileName;
};
