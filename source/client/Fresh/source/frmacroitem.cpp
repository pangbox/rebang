#include "frmacroitem.h"
#include "frelement.h"
#include "frwndmanager.h"

// HACK: stands in for an unknown erased inline use of __rtti_obj
static __declspec(thread) int __rtti_tls_pad;

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
