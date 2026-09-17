#include <list>
#include <string>

static __declspec(thread) int __rtti_obj;

class FrCmdTarget;
class FrWnd;
class FrWndManager;

class FrGuiItem
{
public:
	char unknown_00[8];
	std::string m_resource;
};

class FrElementMacroItem
{
public:
	char unknown_00[0x24];
	std::list<FrGuiItem*> m_guiList;
};

class FrElementDoc
{
public:
	FrElementMacroItem* GetMacroItem(const std::string& name);
};

class FrWndManager
{
public:
	FrElementDoc* GetDocument() const;
	FrWnd* DoCreate(FrGuiItem& item, FrWndManager* manager, FrWnd* parent,
		FrCmdTarget* owner);
};

class FrMacroItem
{
public:
	FrMacroItem();
	void SetOwner(FrCmdTarget* owner);
	void Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent);

private:
	FrCmdTarget* m_pOwner;
};

FrMacroItem::FrMacroItem()
	: m_pOwner(0)
{
}

void FrMacroItem::SetOwner(FrCmdTarget* owner)
{
	m_pOwner = owner;
}

void FrMacroItem::Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent)
{
	FrElementDoc* document = manager->GetDocument();
	FrElementMacroItem* macro = document->GetMacroItem(item.m_resource);
	if (macro)
	{
		std::list<FrGuiItem*>::iterator it = macro->m_guiList.begin();
		std::list<FrGuiItem*>::iterator end = macro->m_guiList.end();
		for (; it != end; ++it)
		{
			manager->DoCreate(**it, manager, parent, m_pOwner);
		}
	}
}
