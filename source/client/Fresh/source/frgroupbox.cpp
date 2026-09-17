#include "frgroupbox.h"

static __declspec(thread) int __rtti_obj;

struct __sFrGroupBox
{
	__sFrGroupBox();
};

IObject* FrGroupBoxMakeInstance()
{
	return new FrGroupBox;
}

__sFrGroupBox::__sFrGroupBox()
{
	ObjectFactory().AddObjectFunctor(FrGroupBoxMakeInstance, "FrGroupBox");
}

FrGroupBox::FrGroupBox()
{
}

const WRTTI* FrGroupBox::GetRTTI() const
{
	return &m_RTTI;
}

FrGroupBox::~FrGroupBox()
{
}

void FrGroupBox::Init(FrGuiItem& item, FrWndManager* manager, FrWnd* parent)
{
	manager->GetDocument();
	WRect rect((float)item.m_rect.left, (float)item.m_rect.top,
		(short)(item.m_rect.right - item.m_rect.left),
		(short)(item.m_rect.bottom - item.m_rect.top));

	Create("GroupBox", item.m_name.c_str(), manager, 0x841, rect, parent);

	const std::list<FrGuiItem*>* children = item.GetChildList();
	if (children)
	{
		std::list<FrGuiItem*>::const_iterator it = children->begin();
		std::list<FrGuiItem*>::const_iterator end = children->end();
		for (; it != end; ++it)
			manager->DoCreate(**it, manager, this, m_pOwner);
	}
}

const WRTTI FrGroupBox::m_RTTI("FrGroupBox", &FrWnd::m_RTTI);

static __sFrGroupBox __implFrGroupBox;
